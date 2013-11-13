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
#include <string.h>

#include <ftdi.h>
#include <libudev.h>

#include "ftdi.h"

#include "timer/timer.h"

ftdi_context_t _ftdi_default_context = {
  0,
  0,
  0
};

ftdi_context_p ftdi_default_context = &_ftdi_default_context;

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

int ftdi_context_init(ftdi_context_p context) {
  if (context != ftdi_default_context) {
    context->num_devices = 0;
    context->devices = 0;
      
    context->num_references = 1;    
  }
  else
    context->num_references++;          
  
  return ftdi_context_refresh(context);
}

int ftdi_context_release(ftdi_context_p context) {
  int i;
  
  if (context == ftdi_default_context) {
    if (context->num_references)
      context->num_references--;
    else
      return FTDI_ERROR_CONTEXT_RELEASE;
  }
  else
    context->num_references = 0;

  if (!context->num_references && context->num_devices) {
    for (i = 0; i < context->num_devices; ++i) {
      ftdi_deinit(context->devices[i].libftdi_context);
      ftdi_free(context->devices[i].libftdi_context);
    }
    
    free(context->devices);
    context->devices = 0;
    context->num_devices = 0;
  }
  
  return FTDI_ERROR_NONE;
}

int ftdi_context_refresh(ftdi_context_p context) {
  struct usb_bus* libusb_bus;
  struct usb_device* libusb_device;
  struct ftdi_context* libftdi_context;
  int i = 0;
    
  if (context->num_references) {
    if (context->num_devices) {
      for (i = 0; i < context->num_devices; ++i) {
        ftdi_deinit(context->devices[i].libftdi_context);
        ftdi_free(context->devices[i].libftdi_context);
      }
      free(context->devices);
    }

    context->num_devices = 0;
    context->devices = 0;
    
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
      
        context->devices[i].libftdi_context = libftdi_context;
        context->devices[i].libusb_device = libusb_device;

        context->devices[i].bus = libusb_bus->location;
        sscanf(libusb_device->filename, "%d", &context->devices[i].address);

        context->devices[i].product_id = libusb_device->descriptor.idProduct;
        
        switch (libusb_device->descriptor.bcdDevice) {
          case 0x0400: context->devices[i].chip = ftdi_chip_bm;
                       break;
          case 0x0200: if (!libusb_device->descriptor.iSerialNumber)
                         context->devices[i].chip = ftdi_chip_bm;
                       else
                         context->devices[i].chip = ftdi_chip_am;
                       break;
          case 0x0500: context->devices[i].chip = ftdi_chip_2232c;
                       break;
          case 0x0600: context->devices[i].chip = ftdi_chip_r;
                       break;
          case 0x0700: context->devices[i].chip = ftdi_chip_2232h;
                       break;
          case 0x0800: context->devices[i].chip = ftdi_chip_4232h;
                       break;
          case 0x0900: context->devices[i].chip = ftdi_chip_232h;
                       break;
          default    : context->devices[i].chip = ftdi_chip_unkown;
        }
        context->devices[i].num_read = 0;
        context->devices[i].num_written = 0;
      
        i++;
      }
    }
  }
  else
    return FTDI_ERROR_INVALID_CONTEXT;
  
  return FTDI_ERROR_NONE;
}

ftdi_device_p ftdi_match_name(ftdi_context_p context, const char* name) {
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
      sscanf(udev_device_get_sysattr_value(dev, "busnum"), "%d", &bus);
      sscanf(udev_device_get_sysattr_value(dev, "devnum"), "%d", &address);
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

ftdi_device_p ftdi_match_product(ftdi_context_p context, int product_id) {
  int i;
  
  for (i = 0; i < context->num_devices; ++i) {
    if (context->devices[i].product_id == product_id)
      return &context->devices[i];
  }
  
  return 0;
}

ftdi_device_p ftdi_match_chip(ftdi_context_p context, ftdi_chip_t chip) {
  int i;
  
  for (i = 0; i < context->num_devices; ++i) {
    if (context->devices[i].chip == chip)
      return &context->devices[i];
  }
  
  return 0;
}

int ftdi_open(ftdi_device_p dev, const char* name, ftdi_interface_t
    interface) {
  if (ftdi_set_interface(dev->libftdi_context, interface))
    return FTDI_ERROR_INVALID_INTERFACE;
  
  if (ftdi_usb_open_dev(dev->libftdi_context, dev->libusb_device))
    return FTDI_ERROR_OPEN;
    
  return FTDI_ERROR_NONE;
}

int ftdi_close(ftdi_device_p dev) {
  if (ftdi_usb_purge_buffers(dev->libftdi_context))
    return FTDI_ERROR_PURGE;
  
  if (ftdi_usb_close(dev->libftdi_context))
    return FTDI_ERROR_CLOSE;
  
  return FTDI_ERROR_NONE;
}

int ftdi_setup(ftdi_device_p dev, int baud_rate, int data_bits, int stop_bits,
    ftdi_parity_t parity, ftdi_flow_ctrl_t flow_ctrl, ftdi_break_t break_type,
    double timeout, double latency) {
  struct ftdi_context* libftdi_context = dev->libftdi_context;
  enum ftdi_bits_type libftdi_data_bits;
  enum ftdi_stopbits_type libftdi_stop_bits;
  enum ftdi_parity_type libftdi_parity;
  int libftdi_flow_ctrl;
  enum ftdi_break_type libftdi_break;
  int error;
  
  switch (data_bits) {
    case 7 : libftdi_data_bits = BITS_7;
             break;
    case 8 : libftdi_data_bits = BITS_8;
             break;
    default: return FTDI_ERROR_INVALID_DATA_BITS;
  }
  dev->data_bits = data_bits;

  switch (stop_bits) {
    case 1 : libftdi_stop_bits = STOP_BIT_1;
             break;
    case 2 : libftdi_stop_bits = STOP_BIT_2;
             break;
    case 15: libftdi_stop_bits = STOP_BIT_15;
             break;
    default: return FTDI_ERROR_INVALID_STOP_BITS;
  }
  dev->stop_bits = stop_bits;

  switch (parity) {
    case ftdi_parity_none : libftdi_parity = NONE;
                            break;
    case ftdi_parity_odd  : libftdi_parity = ODD;
                            break;
    case ftdi_parity_even : libftdi_parity = EVEN;
                            break;
    case ftdi_parity_mark : libftdi_parity = MARK;
                            break;
    case ftdi_parity_space: libftdi_parity = SPACE;
                            break;
    default               : return FTDI_ERROR_INVALID_PARITY;
  }
  dev->parity = parity;

  switch (flow_ctrl) {
    case ftdi_flow_ctrl_off     : libftdi_flow_ctrl = SIO_DISABLE_FLOW_CTRL;
                                  break;
    case ftdi_flow_ctrl_xon_xoff: libftdi_flow_ctrl = SIO_XON_XOFF_HS;
                                  break;
    case ftdi_flow_ctrl_rts_cts : libftdi_flow_ctrl = SIO_RTS_CTS_HS;
                                  break;
    case ftdi_flow_ctrl_dtr_dsr : libftdi_flow_ctrl = SIO_DTR_DSR_HS;
                                  break;
    default                     : return FTDI_ERROR_INVALID_FLOW_CTRL;
  }
  dev->flow_ctrl = flow_ctrl;
  
  switch (break_type) {
    case ftdi_break_off: libftdi_break = BREAK_OFF;
                         break;
    case ftdi_break_on : libftdi_break = BREAK_ON;
                         break;
    default            : return FTDI_ERROR_INVALID_BREAK;
  }
  dev->break_type = break_type;

  if (ftdi_set_line_property2(libftdi_context, libftdi_data_bits,
      libftdi_stop_bits, libftdi_parity, libftdi_break))
    return FTDI_ERROR_SETUP;
  if (ftdi_setflowctrl(libftdi_context, libftdi_flow_ctrl))
    return FTDI_ERROR_SETUP;
  
  error = ftdi_set_baudrate(libftdi_context, baud_rate);
  if (error == -1)
    return FTDI_ERROR_INVALID_BAUD_RATE;
  dev->baud_rate = baud_rate;
  if (error)
    return FTDI_ERROR_SETUP;

  libftdi_context->usb_read_timeout = timeout*1e6;
  libftdi_context->usb_write_timeout = timeout*1e6;
  dev->timeout = timeout;
  
  error = ftdi_set_latency_timer(libftdi_context, latency*1e3);
  if (error == -1)
    return FTDI_ERROR_INVALID_LATENCY;
  dev->latency = latency;
  if (error)
    return FTDI_ERROR_SETUP;
  
  return FTDI_ERROR_NONE;
}

int ftdi_read(ftdi_device_p dev, unsigned char* data, size_t num) {
  ssize_t result = 0, num_read = 0;
  double time, period = 0.0;
  
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
    
  if (result < 0)
    return -FTDI_ERROR_READ;
  else if (!num_read && (period > dev->timeout))
    return -FTDI_ERROR_TIMEOUT;
  else
    return num_read;
}

int ftdi_write(ftdi_device_p dev, unsigned char* data, size_t num) {
  ssize_t result;
  
  result = ftdi_write_data(dev->libftdi_context, data, num);
  if (result >= 0)
    dev->num_written += result;
  else
    return -FTDI_ERROR_WRITE;
  
  return result;
}

void ftdi_print(FILE* stream, ftdi_device_p dev) {
  fprintf(stream, "Bus %03d Device %03d: ID %04x:%04x %s\n",
    dev->bus, dev->address, FTDI_VENDOR_ID, dev->product_id,
    ftdi_chips[dev->chip]);
}
