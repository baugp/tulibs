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
#include <string.h>
#include <errno.h>

#include <libudev.h>
#include <libusb.h>

#include "usb.h"

#define usb_error(e) (e < 0 ? (e > -13 ? -e : 13) : e)

usb_context_t _usb_default_context = {
  0,
  usb_debug_level_minimal,
  0,
  0
};

usb_context_p usb_default_context = &_usb_default_context;

const char* usb_errors[] = {
  "success",
  "input/output error",
  "invalid parameter",
  "access denied",
  "no such device",
  "entity not found",
  "resource busy",
  "operation timed out",
  "overflow",
  "pipe error",
  "system call interrupted",
  "insufficient memory",
  "operation not supported"
  "other error"
};

int usb_context_init(usb_context_p context) {
  int error;

  context->libusb_context = 0;
  context->debug_level = usb_debug_level_minimal;
  context->num_devices = 0;
  context->devices = 0;
  
  if (context != usb_default_context) {
    error = libusb_init((libusb_context**)&context->libusb_context);
    if (error)
      context->libusb_context = 0;
  }
  else
    error = libusb_init(0);
  
  if (error)
    return usb_error(error);
  else
    return usb_context_refresh(context);
}

int usb_context_release(usb_context_p context) {
  if (context != usb_default_context) {
    if (context->libusb_context) {
      libusb_exit(context->libusb_context);
      
      context->libusb_context = 0;
      
      if (context->num_devices)
        free(context->devices);
      
      context->num_devices = 0;
      context->devices = 0;
    }
    else
      return USB_ERROR_INVALID_PARAMETER;
  }
  else
    libusb_exit(0);
      
  return USB_ERROR_NONE;
}

int usb_context_setup(usb_context_p context, usb_debug_level_t debug_level) {
  if (context->libusb_context || (context == usb_default_context)) {
    libusb_set_debug(context->libusb_context, debug_level);
    context->debug_level = debug_level;
  }
  else
    return USB_ERROR_INVALID_PARAMETER;
  
  return USB_ERROR_NONE;
}

int usb_context_refresh(usb_context_p context) {
  libusb_device** libusb_devices = 0;
  struct libusb_device_descriptor descriptor;
  int i;
    
  if (context->libusb_context || (context == usb_default_context)) {
    if (context->num_devices)
      free(context->devices);

    context->num_devices = 0;
    context->devices = 0;
    
    ssize_t n = libusb_get_device_list(context->libusb_context,
      &libusb_devices);
    if (n < 0)
      return usb_error(n);
    else
      context->num_devices = n;
    
    if (context->num_devices) {
      context->devices = malloc(context->num_devices*sizeof(usb_device_t));
      
      for (i = 0; i < context->num_devices; ++i) {
        context->devices[i].libusb_device = libusb_devices[i];
        context->devices[i].libusb_handle = 0;
       
        context->devices[i].bus = libusb_get_bus_number(libusb_devices[i]);
        context->devices[i].address = libusb_get_device_address(
          libusb_devices[i]);
        
        if (!libusb_get_device_descriptor(libusb_devices[i], &descriptor)) {
          context->devices[i].vendor_id = descriptor.idVendor;
          context->devices[i].product_id = descriptor.idProduct;
          context->devices[i].class = descriptor.bDeviceClass;
          
          context->devices[i].timeout = 1e-2;
        }
        else {
          context->devices[i].vendor_id = 0;
          context->devices[i].product_id = 0;
          context->devices[i].class = usb_class_unknown;
          
          context->devices[i].timeout = 0.0;
        }
        
        context->devices[i].num_read = 0;
        context->devices[i].num_written = 0;
      }
    }

    if (libusb_devices)
      libusb_free_device_list(libusb_devices, 1);
  }
  else
    return USB_ERROR_INVALID_PARAMETER;
  
  return USB_ERROR_NONE;
}

usb_device_p usb_match_name(usb_context_p context, const char* name) {
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

usb_device_p usb_match_vendor_product(usb_context_p context, int vendor_id,
    int product_id) {
  int i;
  
  for (i = 0; i < context->num_devices; ++i) {
    if ((context->devices[i].vendor_id == vendor_id) &&
        (context->devices[i].product_id == product_id))
      return &context->devices[i];
  }
  
  return 0;
}

int usb_open(usb_device_p dev) {
  int error;
  
  error = libusb_open(dev->libusb_device, (libusb_device_handle**)
    &dev->libusb_handle);

  if (!error) {
    if (libusb_kernel_driver_active(dev->libusb_handle, 0) == 1)
      libusb_detach_kernel_driver(dev->libusb_handle, 0);
  }
  else
    dev->libusb_handle = 0;
    
  return usb_error(error);
}

int usb_close(usb_device_p dev) {
  if (dev->libusb_handle) {
    libusb_close(dev->libusb_handle);
    dev->libusb_handle = 0;
  }
  
  return USB_ERROR_NONE;
}

int usb_setup(usb_device_p dev, double timeout) {
  if (timeout >= 0.0)
    dev->timeout = timeout;
  else
    return USB_ERROR_INVALID_PARAMETER;
  
  return USB_ERROR_NONE;
}

int usb_control_read(usb_device_p dev, usb_request_type_t request_type,
    usb_recipient_t recipient, unsigned char request, unsigned char value,
    unsigned char index, unsigned char* data, size_t num) {
  usb_control_transfer_t transfer;
  
  transfer.request_type = request_type;
  transfer.recipient = recipient;
  transfer.direction = usb_direction_in;
  
  transfer.request = request;
  transfer.value = value;
  transfer.index = index;
  
  transfer.num = num;
  transfer.data = data;
  
  return usb_control_transfer(dev, &transfer);
}

int usb_control_write(usb_device_p dev, usb_request_type_t request_type,
    usb_recipient_t recipient, unsigned char request, unsigned char value,
    unsigned char index, unsigned char* data, size_t num) {
  usb_control_transfer_t transfer;
  
  transfer.request_type = request_type;
  transfer.recipient = recipient;
  transfer.direction = usb_direction_out;
  
  transfer.request = request;
  transfer.value = value;
  transfer.index = index;
  
  transfer.num = num;
  transfer.data = data;
  
  return usb_control_transfer(dev, &transfer);
}

int usb_bulk_read(usb_device_p dev, unsigned char endpoint_number,
    unsigned char* data, size_t num) {
  usb_bulk_transfer_t transfer;

  transfer.endpoint_number = endpoint_number;
  transfer.direction = usb_direction_in;
  
  transfer.num = num;
  transfer.data = data;
  
  return usb_bulk_transfer(dev, &transfer);
}

int usb_bulk_write(usb_device_p dev, unsigned char endpoint_number,
    unsigned char* data, size_t num) {
  usb_bulk_transfer_t transfer;

  transfer.endpoint_number = endpoint_number;
  transfer.direction = usb_direction_out;
  
  transfer.num = num;
  transfer.data = data;
  
  return usb_bulk_transfer(dev, &transfer);
}

int usb_control_transfer(usb_device_p dev, usb_control_transfer_p transfer) {
  ssize_t result;
  unsigned char request_type = transfer->recipient |
    (transfer->request_type << 5) | (transfer->direction << 7);
  
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
    return -usb_error(result);
}

int usb_bulk_transfer(usb_device_p dev, usb_bulk_transfer_p transfer) {
  ssize_t result;
  int transferred = 0;
  unsigned char endpoint_address = transfer->endpoint_number |
    (transfer->direction << 7);
  
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
    return -usb_error(result);
}

void usb_print(FILE* stream, usb_device_p dev) {
  fprintf(stream, "Bus %03d Device %03d: ID %04x:%04x Class %03d\n",
    dev->bus, dev->address, dev->vendor_id, dev->product_id, dev->class);
}
