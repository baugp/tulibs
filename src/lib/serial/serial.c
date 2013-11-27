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
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>

#include "serial.h"

#include "string/string.h"

const char* serial_errors[] = {
  "Success",
  "Error opening serial device",
  "Error closing serial device",
  "Error draining serial device",
  "Error flushing serial device",
  "Invalid baud rate",
  "Invalid number of data bits",
  "Invalid number of stop bits",
  "Invalid parity",
  "Invalid flow control",
  "Error setting serial device parameters",
  "Serial device select timeout",
  "Error reading from serial device",
  "Error writing to serial device",
};

void serial_device_init(serial_device_t* dev, const char* name) {
  dev->fd = 0;
  string_init_copy(&dev->name, name);

  dev->baud_rate = 0;
  dev->data_bits = 0;
  dev->stop_bits = 0;
  dev->parity = serial_parity_none;
  dev->flow_ctrl = serial_flow_ctrl_off;

  dev->timeout = 0.0;
  
  dev->num_read = 0;
  dev->num_written = 0;
  
  error_init(&dev->error, serial_errors);
}

void serial_device_destroy(serial_device_t* dev) {
  if (dev->fd)
    serial_device_close(dev);
  
  string_destroy(&dev->name);
  error_destroy(&dev->error);
}

int serial_device_open(serial_device_t* dev) {
  error_clear(&dev->error);
  
  dev->fd = open(dev->name, O_RDWR | O_NDELAY);

  if (dev->fd < 0) {
    error_setf(&dev->error, SERIAL_ERROR_OPEN, dev->name);
    dev->fd = 0;
  }

  return error_get(&dev->error);
}

int serial_device_close(serial_device_t* dev) {
  error_clear(&dev->error);
  
  if (dev->fd) {
    if (tcdrain(dev->fd) < 0)
      error_setf(&dev->error, SERIAL_ERROR_DRAIN, dev->name);
    else if (tcflush(dev->fd, TCIOFLUSH) < 0)
      error_setf(&dev->error, SERIAL_ERROR_FLUSH, dev->name);
    else if (!close(dev->fd)) {
      dev->name[0] = 0;
      dev->fd = 0;
    }
    else
      error_setf(&dev->error, SERIAL_ERROR_CLOSE, dev->name);
  }

  return error_get(&dev->error);
}

int serial_device_setup(serial_device_t* dev, int baud_rate, int data_bits,
    int stop_bits, serial_parity_t parity, serial_flow_ctrl_t flow_ctrl,
    double timeout) {
  struct termios tio;
  memset(&tio, 0, sizeof(struct termios));
  
  error_clear(&dev->error);
  
  switch (baud_rate) {
    case 50:
      tio.c_cflag |= B50;
      break;
    case 75:
      tio.c_cflag |= B75;
      break;
    case 110:
      tio.c_cflag |= B110;
      break;
    case 134:
      tio.c_cflag |= B134;
      break;
    case 150:
      tio.c_cflag |= B150;
      break;
    case 200:
      tio.c_cflag |= B200;
      break;
    case 300:
      tio.c_cflag |= B300;
      break;
    case 600:
      tio.c_cflag |= B600;
      break;
    case 1200:
      tio.c_cflag |= B1200;
      break;
    case 1800:
      tio.c_cflag |= B1800;
      break;
    case 2400:
      tio.c_cflag |= B2400;
      break;
    case 4800:
      tio.c_cflag |= B4800;
      break;
    case 9600:
      tio.c_cflag |= B9600;
      break;
    case 19200:
      tio.c_cflag |= B19200;
      break;
    case 38400:
      tio.c_cflag |= B38400;
      break;
    case 57600:
      tio.c_cflag |= B57600;
      break;
    case 115200:
      tio.c_cflag |= B115200;
      break;
    case 230400:
      tio.c_cflag |= B230400;
      break;
    default:
      error_setf(&dev->error, SERIAL_ERROR_INVALID_BAUD_RATE, "%d",
        baud_rate);
      return error_get(&dev->error);
  }
  dev->baud_rate = baud_rate;
  
  switch (data_bits) {
    case 5:
      tio.c_cflag |= CS5;
      break;
    case 6:
      tio.c_cflag |= CS6;
      break;
    case 7:
      tio.c_cflag |= CS7;
      break;
    case 8:
      tio.c_cflag |= CS8;
      break;
    default:
      error_setf(&dev->error, SERIAL_ERROR_INVALID_DATA_BITS, "%d",
        data_bits);
      return error_get(&dev->error);
  }
  dev->data_bits = data_bits;
  
  switch (stop_bits) {
    case 1:
      break;
    case 2:
      tio.c_cflag |= CSTOPB;
      break;
    default:
      error_setf(&dev->error, SERIAL_ERROR_INVALID_STOP_BITS, "%d",
        stop_bits);
      return error_get(&dev->error);
  }
  dev->stop_bits = stop_bits;
  
  switch (parity) {
    case serial_parity_none:
      break;
    case serial_parity_even:
      tio.c_cflag |= PARENB;
      break;
    case serial_parity_odd:
      tio.c_cflag |= PARENB | PARODD;
      break;
    default:
      error_set(&dev->error, SERIAL_ERROR_INVALID_PARITY);
      return error_get(&dev->error);
  }
  dev->parity = parity;
  
  switch (flow_ctrl) {
    case serial_flow_ctrl_off:
      break;
    case serial_flow_ctrl_xon_xoff:
      tio.c_cflag |= IXON | IXOFF;
      break;
    case serial_flow_ctrl_rts_cts:
      tio.c_cflag |= CRTSCTS;
      break;
    default:
      error_set(&dev->error, SERIAL_ERROR_INVALID_FLOW_CTRL);
      return error_get(&dev->error);
  }
  dev->flow_ctrl = flow_ctrl;
  
  dev->timeout = timeout;

  tio.c_cflag |= CLOCAL;
  tio.c_iflag = IGNPAR;
  
  if (tcflush(dev->fd, TCIOFLUSH) < 0)
    error_setf(&dev->error, SERIAL_ERROR_FLUSH, dev->name);
  else if (tcsetattr(dev->fd, TCSANOW, &tio) < 0)
    error_setf(&dev->error, SERIAL_ERROR_SETUP, dev->name);
  
  return error_get(&dev->error);
}

int serial_device_read(serial_device_t* dev, unsigned char* data,
    size_t num) {
  size_t num_read = 0;
  struct timeval time;
  fd_set set;
  int error;

  error_clear(&dev->error);
  
  while (num_read < num) {
    time.tv_sec = 0;
    time.tv_usec = dev->timeout*1e6;

    FD_ZERO(&set);
    FD_SET(dev->fd, &set);

    error = select(dev->fd+1, &set, NULL, NULL, &time);
    if (error == 0) {
      error_setf(&dev->error, SERIAL_ERROR_TIMEOUT, dev->name);
      return -error_get(&dev->error);
    }

    ssize_t n;
    n = read(dev->fd, &data[num_read], num-num_read);
    if ((n < 0) && (errno != EWOULDBLOCK)) {
      error_setf(&dev->error, SERIAL_ERROR_READ, dev->name);
      return -error_get(&dev->error);
    }
    if (n > 0) {
      num_read += n;
      dev->num_read += n;
    }
  }
  
  return num_read;
}

int serial_device_write(serial_device_t* dev, unsigned char* data,
    size_t num) {
  size_t num_written = 0;

  error_clear(&dev->error);
  
  while (num_written < num) {
    ssize_t n;
    while ((n = write(dev->fd, &data[num_written], num-num_written)) == 0);
    if ((n < 0) && (errno != EWOULDBLOCK)) {
      error_setf(&dev->error, SERIAL_ERROR_WRITE, dev->name);
      return -error_get(&dev->error);
    }
    if (n > 0) {
      num_written += n;
      dev->num_written += n;
    }
  }
  
  return num_written;
}

void serial_device_print(FILE* stream, const serial_device_t* dev) {
  fputs(dev->name, stream);
}
