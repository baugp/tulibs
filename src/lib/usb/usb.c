/***************************************************************************
 *   Copyright (C) 2013 by Ralf Kaestner                                   *
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

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <libudev.h>
#include <libusb.h>

#include "usb.h"

#include "string/string.h"

#define libusb_error(e) (e < 0 ? (e > -13 ? -e : 13) : e)

usb_context_t _usb_default_context = {
  0,
  usb_debug_level_minimal,
  0,
  0
};

usb_context_t* usb_default_context = &_usb_default_context;

const char* usb_errors[] = {
  "Success",
  "Input/output error",
  "Invalid parameter",
  "Access denied",
  "No such device",
  "Entity not found",
  "Resource busy",
  "Operation timed out",
  "Overflow",
  "Pipe error",
  "System call interrupted",
  "Insufficient memory",
  "Operation not supported",
  "Other error",
  "Invalid FTDI context",
};

const char* usb_classes[] = {
  "Per-interface",
  "Audio",
  "Communication",
  "HID",
  "Physical",
  "Image",
  "Printer",
  "Mass storage",
  "Hub",
  "Data",
  "Smart card",
  "Content security",
  "Video",
  "Healthcare",
  "Diagnostic",
  "Wireless",
  "Application",
  "Vendor-specific",
  "Unknown",
};

void usb_device_init(usb_device_t* dev, libusb_device* libus_device);
void usb_device_destroy(usb_device_t* dev);

int usb_context_init(usb_context_t* context) {
  int error;

  context->libusb_context = 0;
  context->debug_level = usb_debug_level_minimal;
  context->num_devices = 0;
  context->devices = 0;
  
  error_init(&context->error, usb_errors);
  
  if (context != usb_default_context) {
    error = libusb_init((libusb_context**)&context->libusb_context);
    if (error)
      context->libusb_context = 0;
  }
  else
    error = libusb_init(0);
  
  if (error)
    error_set(&context->error, libusb_error(error));
  else
    usb_context_refresh(context);

  return error_get(&context->error);  
}

int usb_context_release(usb_context_t* context) {
  int i;
  
  error_clear(&context->error);
  
  if (context != usb_default_context) {
    if (context->libusb_context) {
      libusb_exit(context->libusb_context);
      
      context->libusb_context = 0;
      
      if (context->num_devices) {
        for (i = 0; i < context->num_devices; ++i)
          usb_device_destroy(&context->devices[i]);
        free(context->devices);
      }
      
      context->num_devices = 0;
      context->devices = 0;
    }
    else
      error_set(&context->error, USB_ERROR_INVALID_CONTEXT);
  }
  else
    libusb_exit(0);
      
  return error_get(&context->error);
}

int usb_context_setup(usb_context_t* context, usb_debug_level_t debug_level) {
  error_clear(&context->error);
  
  if (context->libusb_context || (context == usb_default_context)) {
    libusb_set_debug(context->libusb_context, debug_level);
    context->debug_level = debug_level;
  }
  else
    error_set(&context->error, USB_ERROR_INVALID_CONTEXT);
  
  return error_get(&context->error);
}

int usb_context_refresh(usb_context_t* context) {
  libusb_device** libusb_devices = 0;
  int i;

  error_clear(&context->error);
  
  if (context->libusb_context || (context == usb_default_context)) {
    if (context->num_devices) {
      for (i = 0; i < context->num_devices; ++i)
        usb_device_destroy(&context->devices[i]);
      free(context->devices);
    }

    context->num_devices = 0;
    context->devices = 0;
    
    ssize_t n = libusb_get_device_list(context->libusb_context,
      &libusb_devices);
    if (n < 0)
      error_set(&context->error, libusb_error(n));
    else
      context->num_devices = n;
    
    if (context->num_devices) {
      context->devices = malloc(context->num_devices*sizeof(usb_device_t));
      for (i = 0; i < context->num_devices; ++i)
        usb_device_init(&context->devices[i], libusb_devices[i]);
    }

    if (libusb_devices)
      libusb_free_device_list(libusb_devices, 1);
  }
  else
    error_set(&context->error, USB_ERROR_INVALID_CONTEXT);
  
  return error_get(&context->error);
}

usb_device_t* usb_context_match_name(const usb_context_t* context,
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

usb_device_t* usb_context_match_vendor_product(const usb_context_t* context,
    int vendor_id, int product_id) {
  int i;
  
  for (i = 0; i < context->num_devices; ++i) {
    if ((context->devices[i].vendor_id == vendor_id) &&
        (context->devices[i].product_id == product_id))
      return &context->devices[i];
  }
  
  return 0;
}

void usb_device_init(usb_device_t* dev, libusb_device* libus_device) {
  struct libusb_device_descriptor descriptor;
  
  dev->libusb_device = libus_device;
  dev->libusb_handle = 0;

  dev->bus = libusb_get_bus_number(libus_device);
  dev->address = libusb_get_device_address(libus_device);

  if (!libusb_get_device_descriptor(libus_device, &descriptor)) {
    dev->vendor_id = descriptor.idVendor;
    dev->product_id = descriptor.idProduct;
    
    switch (descriptor.bDeviceClass) {
      case LIBUSB_CLASS_PER_INTERFACE:
        dev->class = usb_class_per_interface;
        break;
      case LIBUSB_CLASS_AUDIO:
        dev->class = usb_class_audio;
        break;
      case LIBUSB_CLASS_COMM:
        dev->class = usb_class_comm;
        break;
      case LIBUSB_CLASS_HID:
        dev->class = usb_class_hid;
        break;
      case 0x05:
        dev->class = usb_class_physical;
        break;
      case 0x06:
        dev->class = usb_class_image;
        break;
      case LIBUSB_CLASS_PRINTER:
        dev->class = usb_class_printer;
        break;
      case LIBUSB_CLASS_MASS_STORAGE:
        dev->class = usb_class_mass_storage;
        break;
      case LIBUSB_CLASS_HUB:
        dev->class = usb_class_hub;
        break;
      case LIBUSB_CLASS_DATA:
        dev->class = usb_class_data;
        break;
      case 0x0b:
        dev->class = usb_class_smart_card;
        break;
      case 0x0d:
        dev->class = usb_class_content_security;
        break;
      case 0x0e:
        dev->class = usb_class_video;
        break;
      case 0x0f:
        dev->class = usb_class_healthcare;
        break;
      case 0xdc:
        dev->class = usb_class_diagnostic;
        break;
      case 0xe0:
        dev->class = usb_class_wireless;
        break;
      case 0xfe:
        dev->class = usb_class_application;
        break;
      case LIBUSB_CLASS_VENDOR_SPEC:
        dev->class = usb_class_vendor;
        break;
      default:
        dev->class = usb_class_unknown;
    }
  }
  else {
    dev->vendor_id = 0;
    dev->product_id = 0;
    dev->class = usb_class_unknown;
  }
  
  dev->timeout = 0.0;

  dev->num_read = 0;
  dev->num_written = 0;
      
  error_init(&dev->error, usb_errors);
}

void usb_device_destroy(usb_device_t* dev) {
  error_destroy(&dev->error);
}

int usb_device_open(usb_device_t* dev) {
  error_clear(&dev->error);

  int error = libusb_open(dev->libusb_device, (libusb_device_handle**)
    &dev->libusb_handle);

  if (!error) {
    if (libusb_kernel_driver_active(dev->libusb_handle, 0) == 1)
      libusb_detach_kernel_driver(dev->libusb_handle, 0);
  }
  else {
    dev->libusb_handle = 0;
    error_setf(&dev->error, libusb_error(error), "%03d:%03d",
      dev->bus, dev->address);
  }
    
  return error_get(&dev->error);
}

int usb_device_close(usb_device_t* dev) {
  error_clear(&dev->error);
  
  if (dev->libusb_handle) {
    libusb_close(dev->libusb_handle);
    dev->libusb_handle = 0;
  }
  
  return error_get(&dev->error);
}

int usb_device_setup(usb_device_t* dev, double timeout) {
  error_clear(&dev->error);
  
  dev->timeout = timeout;
  
  return error_get(&dev->error);
}

int usb_device_control_read(usb_device_t* dev, usb_request_type_t
    request_type, usb_recipient_t recipient, unsigned char request,
    unsigned char value, unsigned char index, unsigned char* data,
    size_t num) {
  usb_control_transfer_t transfer;
  
  transfer.request_type = request_type;
  transfer.recipient = recipient;
  transfer.direction = usb_direction_in;
  
  transfer.request = request;
  transfer.value = value;
  transfer.index = index;
  
  transfer.num = num;
  transfer.data = data;
  
  return usb_device_control_transfer(dev, &transfer);
}

int usb_device_control_write(usb_device_t* dev, usb_request_type_t
    request_type, usb_recipient_t recipient, unsigned char request,
    unsigned char value, unsigned char index, unsigned char* data,
    size_t num) {
  usb_control_transfer_t transfer;
  
  transfer.request_type = request_type;
  transfer.recipient = recipient;
  transfer.direction = usb_direction_out;
  
  transfer.request = request;
  transfer.value = value;
  transfer.index = index;
  
  transfer.num = num;
  transfer.data = data;
  
  return usb_device_control_transfer(dev, &transfer);
}

int usb_device_bulk_read(usb_device_t* dev, unsigned char endpoint_number,
    unsigned char* data, size_t num) {
  usb_bulk_transfer_t transfer;

  transfer.endpoint_number = endpoint_number;
  transfer.direction = usb_direction_in;
  
  transfer.num = num;
  transfer.data = data;
  
  return usb_device_bulk_transfer(dev, &transfer);
}

int usb_device_bulk_write(usb_device_t* dev, unsigned char endpoint_number,
    unsigned char* data, size_t num) {
  usb_bulk_transfer_t transfer;

  transfer.endpoint_number = endpoint_number;
  transfer.direction = usb_direction_out;
  
  transfer.num = num;
  transfer.data = data;
  
  return usb_device_bulk_transfer(dev, &transfer);
}

int usb_device_control_transfer(usb_device_t* dev, usb_control_transfer_t*
    transfer) {
  ssize_t result;
  unsigned char request_type = transfer->recipient |
    (transfer->request_type << 5) | (transfer->direction << 7);
  
  error_clear(&dev->error);
    
  result = libusb_control_transfer(dev->libusb_handle, request_type,
    transfer->request, transfer->value, transfer->index, transfer->data,
    transfer->num, dev->timeout*1e3);
  
  if (result > 0) {
    if (transfer->direction == usb_direction_out)
      dev->num_written += result;
    else
      dev->num_read += result;
  
    return result;
  }
  else
    error_setf(&dev->error, libusb_error(result), "%03d:%03d",
      dev->bus, dev->address);
    
  return -error_get(&dev->error);
}

int usb_device_bulk_transfer(usb_device_t* dev, usb_bulk_transfer_t*
    transfer) {
  ssize_t result;
  int transferred = 0;
  unsigned char endpoint_address = transfer->endpoint_number |
    (transfer->direction << 7);

  error_clear(&dev->error);
    
  result = libusb_bulk_transfer(dev->libusb_handle, endpoint_address,
    transfer->data, transfer->num, &transferred, dev->timeout*1e3);
  
  if (!result) {
    if (transfer->direction == usb_direction_out)
      dev->num_written += transferred;
    else
      dev->num_read += transferred;
    
    return transferred;
  }
  else
    error_setf(&dev->error, libusb_error(result), "%03d:%03d",
      dev->bus, dev->address);

  return -error_get(&dev->error);      
}

void usb_device_print(FILE* stream, const usb_device_t* dev) {
  fprintf(stream, "Bus %03d Device %03d: ID %04x:%04x Class %s",
    dev->bus, dev->address, dev->vendor_id, dev->product_id,
    usb_classes[dev->class]);
}
