/***************************************************************************
 *   Copyright (C) 2008 by Ralf Kaestner                                   *
 *   ralf.kaestner@gmail.com                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdio.h>

#include <ftdi.h>
#include <libudev.h>

#include "ftdi.h"

#include "string/string.h"

#include "timer/timer.h"

ftdi_context_t _ftdi_default_context = {
  0,
  0,
  0
};

ftdi_context_t* ftdi_default_context = &_ftdi_default_context;

const char* ftdi_errors[] = {
  "Success",
  "Error initializing FTDI context",
  "Error releasing FTDI context",
  "Invalid FTDI context",
  "Error opening FTDI device",
  "Error closing FTDI device",
  "Error purging FTDI device",
  "Invalid interface",
  "Invalid baud rate",
  "Invalid number of data bits",
  "Invalid number of stop bits",
  "Invalid parity",
  "Invalid flow control",
  "Invalid break",
  "Invalid latency",
  "Error setting FTDI device parameters",
  "FTDI device select timeout",
  "Error reading from FTDI device",
  "Error writing to FTDI device",
};

const char* ftdi_chips[] = {
  "FTDI AM-series chip",
  "FTDI BM-series chip",
  "FTDI 2232C-series chip",
  "FTDI R-series chip",
  "FTDI 2232H-series chip",
  "FTDI 4232H-series chip",
  "FTDI 232H-series chip",
  "Unknown FTDI chip",
};

void ftdi_device_init(ftdi_device_t* dev, struct ftdi_context* libftdi_context,
  struct usb_bus* libusb_bus, struct usb_device* libusb_device);
void ftdi_device_destroy(ftdi_device_t* dev);

int ftdi_context_init(ftdi_context_t* context) {
  if (context != ftdi_default_context) {
    context->num_devices = 0;
    context->devices = 0;
      
    context->num_references = 1;    
  }
  else
    context->num_references++;          
  
  error_init(&context->error, ftdi_errors);
  
  return ftdi_context_refresh(context);
}

int ftdi_context_release(ftdi_context_t* context) {
  int i;

  error_clear(&context->error);
  
  if (context == ftdi_default_context) {
    if (context->num_references)
      context->num_references--;
    else {
      error_set(&context->error, FTDI_ERROR_CONTEXT_RELEASE);
      return error_get(&context->error);
    }
  }
  else
    context->num_references = 0;

  if (!context->num_references && context->num_devices) {
    for (i = 0; i < context->num_devices; ++i)
      ftdi_device_destroy(&context->devices[i]);
    
    free(context->devices);
    context->devices = 0;
    context->num_devices = 0;
  }
  
  return error_get(&context->error);
}

int ftdi_context_refresh(ftdi_context_t* context) {
  struct usb_bus* libusb_bus;
  struct usb_device* libusb_device;
  struct ftdi_context* libftdi_context;
  int i = 0;

  error_clear(&context->error);
  
  if (context->num_references) {
    if (context->num_devices) {
      for (i = 0; i < context->num_devices; ++i)
        ftdi_device_destroy(&context->devices[i]);
      
      free(context->devices);
      context->devices = 0;
      context->num_devices = 0;
    }

    usb_init();
    if ((usb_find_busses() > 0) && (usb_find_devices() > 0)) {
      for (libusb_bus = usb_get_busses(); libusb_bus;
          libusb_bus = libusb_bus->next)
        for (libusb_device = libusb_bus->devices; libusb_device;
            libusb_device = libusb_device->next)
          if (libusb_device->descriptor.idVendor == FTDI_VENDOR_ID)
            context->num_devices++;
    }
    
    if (context->num_devices) {
      context->devices = malloc(context->num_devices*sizeof(ftdi_device_t));
      
      i = 0;
      for (libusb_bus = usb_get_busses(); libusb_bus;
          libusb_bus = libusb_bus->next)
        for (libusb_device = libusb_bus->devices; libusb_device;
            libusb_device = libusb_device->next)
          if (libusb_device->descriptor.idVendor == FTDI_VENDOR_ID) {
        libftdi_context = ftdi_new();
        ftdi_init(libftdi_context);
      
        ftdi_device_init(&context->devices[i], libftdi_context, libusb_bus,
          libusb_device);
        i++;
      }
    }
  }
  else
    error_set(&context->error, FTDI_ERROR_INVALID_CONTEXT);
  
  return error_get(&context->error);
}

ftdi_device_t* ftdi_context_match_name(const ftdi_context_t* context,
    const char* name) {
  struct stat stat_buffer;
  struct udev* udev = 0;
  struct udev_device* dev = 0;
  int bus = 0;
  int address = 0;
  int i;
  
  if (!stat(name, &stat_buffer) && S_ISCHR(stat_buffer.st_mode)) {
    udev = udev_new();
    dev = udev_device_new_from_devnum(udev, 'c', stat_buffer.st_rdev);

    if (dev) {
      string_scanf(udev_device_get_sysattr_value(dev, "busnum"),
        "%d", &bus);
      string_scanf(udev_device_get_sysattr_value(dev, "devnum"),
        "%d", &address);
    }

    udev_unref(udev);
  }

  for (i = 0; i < context->num_devices; ++i) {
    if ((context->devices[i].bus == bus) &&
        (context->devices[i].address == address))
      return &context->devices[i];
  }
  
  return 0;
}

ftdi_device_t* ftdi_context_match_product(const ftdi_context_t* context,
    int product_id) {
  int i;
  
  for (i = 0; i < context->num_devices; ++i) {
    if (context->devices[i].product_id == product_id)
      return &context->devices[i];
  }
  
  return 0;
}

ftdi_device_t* ftdi_context_match_chip(const ftdi_context_t* context,
    ftdi_chip_t chip) {
  int i;
  
  for (i = 0; i < context->num_devices; ++i) {
    if (context->devices[i].chip == chip)
      return &context->devices[i];
  }
  
  return 0;
}

void ftdi_device_init(ftdi_device_t* dev, struct ftdi_context* libftdi_context,
    struct usb_bus* libusb_bus, struct usb_device* libusb_device) {
  dev->libftdi_context = libftdi_context;
  dev->libusb_device = libusb_device;

  dev->bus = libusb_bus->location;
  string_scanf(libusb_device->filename, "%d", &dev->address);

  dev->product_id = libusb_device->descriptor.idProduct;
  
  switch (libusb_device->descriptor.bcdDevice) {
    case 0x0400:
      dev->chip = ftdi_chip_bm;
      break;
    case 0x0200:
      if (!libusb_device->descriptor.iSerialNumber)
        dev->chip = ftdi_chip_bm;
      else
        dev->chip = ftdi_chip_am;
      break;
    case 0x0500:
      dev->chip = ftdi_chip_2232c;
      break;
    case 0x0600:
      dev->chip = ftdi_chip_r;
      break;
    case 0x0700:
      dev->chip = ftdi_chip_2232h;
      break;
    case 0x0800:
      dev->chip = ftdi_chip_4232h;
      break;
    case 0x0900:
      dev->chip = ftdi_chip_232h;
      break;
    default:
      dev->chip = ftdi_chip_unkown;
  }
  dev->interface = ftdi_interface_any;
  
  dev->baud_rate = 0;
  dev->data_bits = 0;
  dev->stop_bits = 0;
  dev->parity= ftdi_parity_none;
  dev->flow_ctrl = ftdi_flow_ctrl_off;
  dev->break_type = ftdi_break_off;

  dev->timeout = 0.0;
  dev->latency = 0.0;
  
  dev->num_read = 0;
  dev->num_written = 0;
  
  error_init(&dev->error, ftdi_errors);
}

void ftdi_device_destroy(ftdi_device_t* dev) {
  ftdi_deinit(dev->libftdi_context);
  ftdi_free(dev->libftdi_context);
  
  error_destroy(&dev->error);
}

int ftdi_device_open(ftdi_device_t* dev, ftdi_interface_t interface) {
  error_clear(&dev->error);
  
  if (ftdi_set_interface(dev->libftdi_context, interface))
    error_set(&dev->error, FTDI_ERROR_INVALID_INTERFACE);
  else if (ftdi_usb_open_dev(dev->libftdi_context, dev->libusb_device))
    error_setf(&dev->error, FTDI_ERROR_OPEN, "Bus %03d Device %03d", dev->bus,
      dev->address);
    
  return error_get(&dev->error);
}

int ftdi_device_close(ftdi_device_t* dev) {
  error_clear(&dev->error);
  
  if (ftdi_usb_purge_buffers(dev->libftdi_context))
    error_setf(&dev->error, FTDI_ERROR_PURGE, "Bus %03d Device %03d", dev->bus,
      dev->address);
  else if (ftdi_usb_close(dev->libftdi_context))
    error_setf(&dev->error, FTDI_ERROR_CLOSE, "Bus %03d Device %03d", dev->bus,
      dev->address);
  
  return error_get(&dev->error);
}

int ftdi_device_setup(ftdi_device_t* dev, int baud_rate, int data_bits, int
    stop_bits, ftdi_parity_t parity, ftdi_flow_ctrl_t flow_ctrl, ftdi_break_t
    break_type, double timeout, double latency) {
  struct ftdi_context* libftdi_context = dev->libftdi_context;
  enum ftdi_bits_type libftdi_data_bits;
  enum ftdi_stopbits_type libftdi_stop_bits;
  enum ftdi_parity_type libftdi_parity;
  int libftdi_flow_ctrl;
  enum ftdi_break_type libftdi_break;
  int error;
  
  error_clear(&dev->error);
  
  switch (data_bits) {
    case 7:
      libftdi_data_bits = BITS_7;
      break;
    case 8:
      libftdi_data_bits = BITS_8;
      break;
    default:
      error_setf(&dev->error, FTDI_ERROR_INVALID_DATA_BITS, "%d", data_bits);
      return error_get(&dev->error);
  }
  dev->data_bits = data_bits;

  switch (stop_bits) {
    case 1:
      libftdi_stop_bits = STOP_BIT_1;
      break;
    case 2 :
      libftdi_stop_bits = STOP_BIT_2;
      break;
    case 15:
      libftdi_stop_bits = STOP_BIT_15;
      break;
    default:
      error_setf(&dev->error, FTDI_ERROR_INVALID_STOP_BITS, "%d", stop_bits);
      return error_get(&dev->error);
  }
  dev->stop_bits = stop_bits;

  switch (parity) {
    case ftdi_parity_none:
      libftdi_parity = NONE;
      break;
    case ftdi_parity_odd:
      libftdi_parity = ODD;
      break;
    case ftdi_parity_even:
      libftdi_parity = EVEN;
      break;
    case ftdi_parity_mark:
      libftdi_parity = MARK;
      break;
    case ftdi_parity_space:
      libftdi_parity = SPACE;
      break;
    default:
      error_set(&dev->error, FTDI_ERROR_INVALID_PARITY);
      return error_get(&dev->error);
  }
  dev->parity = parity;

  switch (flow_ctrl) {
    case ftdi_flow_ctrl_off:
      libftdi_flow_ctrl = SIO_DISABLE_FLOW_CTRL;
      break;
    case ftdi_flow_ctrl_xon_xoff:
      libftdi_flow_ctrl = SIO_XON_XOFF_HS;
      break;
    case ftdi_flow_ctrl_rts_cts:
      libftdi_flow_ctrl = SIO_RTS_CTS_HS;
      break;
    case ftdi_flow_ctrl_dtr_dsr:
      libftdi_flow_ctrl = SIO_DTR_DSR_HS;
      break;
    default:
      error_set(&dev->error, FTDI_ERROR_INVALID_FLOW_CTRL);
      return error_get(&dev->error);
  }
  dev->flow_ctrl = flow_ctrl;
  
  switch (break_type) {
    case ftdi_break_off:
      libftdi_break = BREAK_OFF;
      break;
    case ftdi_break_on:
      libftdi_break = BREAK_ON;
      break;
    default:
      error_set(&dev->error, FTDI_ERROR_INVALID_BREAK);
      return error_get(&dev->error);
  }
  dev->break_type = break_type;

  if (ftdi_set_line_property2(libftdi_context, libftdi_data_bits,
      libftdi_stop_bits, libftdi_parity, libftdi_break)) {
    error_setf(&dev->error, FTDI_ERROR_SETUP, "Bus %03d Device %03d", dev->bus,
      dev->address);
    return error_get(&dev->error);
  }
  
  if (ftdi_setflowctrl(libftdi_context, libftdi_flow_ctrl)) {
    error_setf(&dev->error, FTDI_ERROR_SETUP, "Bus %03d Device %03d", dev->bus,
      dev->address);
    return error_get(&dev->error);
  }
  
  error = ftdi_set_baudrate(libftdi_context, baud_rate);
  if (error == -1) {
    error_setf(&dev->error, FTDI_ERROR_INVALID_BAUD_RATE, "%d", baud_rate);
    return error_get(&dev->error);
  }
  dev->baud_rate = baud_rate;
  if (error) {
    error_setf(&dev->error, FTDI_ERROR_SETUP, "Bus %03d Device %03d", dev->bus,
      dev->address);
    return error_get(&dev->error);
  }

  libftdi_context->usb_read_timeout = timeout*1e6;
  libftdi_context->usb_write_timeout = timeout*1e6;
  dev->timeout = timeout;
  
  error = ftdi_set_latency_timer(libftdi_context, latency*1e3);
  if (error == -1) {
    error_setf(&dev->error, FTDI_ERROR_INVALID_LATENCY, "%f", latency);
    return error_get(&dev->error);
  }
  dev->latency = latency;
  if (error) {
    error_setf(&dev->error, FTDI_ERROR_SETUP, "Bus %03d Device %03d", dev->bus,
      dev->address);
    return error_get(&dev->error);
  }
  
  return error_get(&dev->error);
}

int ftdi_device_read(ftdi_device_t* dev, unsigned char* data, size_t num) {
  ssize_t result = 0, num_read = 0;
  double time, period = 0.0;

  error_clear(&dev->error);
  
  timer_start(&time);
  while ((num_read < num) &&
    ((period = timer_stop(time)) <= dev->timeout) &&
    ((result = ftdi_read_data(dev->libftdi_context, &data[num_read],
      num-num_read)) >= 0)) {
    if (result)
      timer_start(&time);
    num_read += result;
  }
  dev->num_read += num_read;
    
  if (result < 0) {
    error_setf(&dev->error, FTDI_ERROR_READ, "Bus %03d Device %03d", dev->bus,
      dev->address);
    return -error_get(&dev->error);
  }
  else if (!num_read && (period > dev->timeout)) {
    error_setf(&dev->error, FTDI_ERROR_TIMEOUT, "Bus %03d Device %03d", dev->bus,
      dev->address);
    return -error_get(&dev->error);
  }

  return num_read;
}

int ftdi_device_write(ftdi_device_t* dev, unsigned char* data, size_t num) {
  ssize_t result;
  
  error_clear(&dev->error);
  
  result = ftdi_write_data(dev->libftdi_context, data, num);
  if (result >= 0)
    dev->num_written += result;
  else {
    error_setf(&dev->error, FTDI_ERROR_WRITE, "Bus %03d Device %03d", dev->bus,
      dev->address);
    return -error_get(&dev->error);
  }
  
  return result;
}

void ftdi_device_print(FILE* stream, const ftdi_device_t* dev) {
  fprintf(stream, "Bus %03d Device %03d: ID %04x:%04x %s",
    dev->bus, dev->address, FTDI_VENDOR_ID, dev->product_id,
    ftdi_chips[dev->chip]);
}
