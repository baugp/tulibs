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

#ifndef SERIAL_H
#define SERIAL_H

#include <unistd.h>

typedef enum {
  none,
  even,
  odd
} serial_parity_t;

typedef struct {
  int fd;
  char name[256];

  int baudrate;
  int databits;
  int stopbits;
  serial_parity_t parity;

  double timeout;

  ssize_t num_read;
  ssize_t num_written;
} serial_device_t, *serial_device_p;

int serial_open(serial_device_p dev, const char* name);
int serial_close(serial_device_p dev);

int serial_setup(serial_device_p dev, int baudrate, int databits, int stopbits,
  serial_parity_t parity, double timeout);

int serial_read(serial_device_p dev, unsigned char* buffer, ssize_t num);
int serial_write(serial_device_p dev, unsigned char* buffer, ssize_t num);

#endif
