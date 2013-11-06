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

/** \defgroup serial Serial Communication Module
  * \brief Library functions for serial device support
  * 
  * The serial module provides library functions for working with standard
  * serial communication devices.
  */

/** \file serial.h
  * \ingroup serial
  * \brief Serial device interface
  * \author Ralf Kaestner
  * 
  * The serial device interface provides high-level support for configuring
  * and operating all standard serial devices.
  */

#include <unistd.h>

/** \name Error Codes
  * \brief Predefined serial error codes
  */
//@{
#define SERIAL_ERROR_NONE                 0
#define SERIAL_ERROR_OPEN                 1
#define SERIAL_ERROR_CLOSE                2
#define SERIAL_ERROR_DRAIN                3
#define SERIAL_ERROR_FLUSH                4
#define SERIAL_ERROR_INVALID_BAUD_RATE    5
#define SERIAL_ERROR_INVALID_DATA_BITS    6
#define SERIAL_ERROR_INVALID_STOP_BITS    7
#define SERIAL_ERROR_INVALID_PARITY       8
#define SERIAL_ERROR_INVALID_FLOW_CTRL    9
#define SERIAL_ERROR_SETUP                10
#define SERIAL_ERROR_TIMEOUT              11
#define SERIAL_ERROR_READ                 12
#define SERIAL_ERROR_WRITE                13
//@}

/** \brief Predefined serial error descriptions
  */
extern const char* serial_errors[];

/** \brief Parity enumerable type
  */
typedef enum {
  serial_parity_none,             //!< No parity.
  serial_parity_odd,              //!< Odd parity.
  serial_parity_even              //!< Even parity.
} serial_parity_t;

/** \brief Flow control enumerable type
  */
typedef enum {
  serial_flow_ctrl_off,           //!< Disable flow control.
  serial_flow_ctrl_xon_xoff,      //!< XON/XOFF flow control.
  serial_flow_ctrl_rts_cts        //!< RTS/CTS (hardware) flow control.
} serial_flow_ctrl_t;

/** \brief Serial device structure
  */
typedef struct serial_device_t {
  int fd;                         //!< File descriptor.
  char name[256];                 //!< Device name.

  int baud_rate;                  //!< Device baud rate.
  int data_bits;                  //!< Number of data bits.
  int stop_bits;                  //!< Number of stop bits.
  serial_parity_t parity;         //!< Device parity.
  serial_flow_ctrl_t flow_ctrl;   //!< Device flow control.

  double timeout;                 //!< Device select timeout in [s].

  size_t num_read;                //!< Number of bytes read from device.
  size_t num_written;             //!< Number of bytes written to device.
} serial_device_t, *serial_device_p;

/** \brief Open the serial device with the specified name
  * \param[in] dev The serial device to be opened.
  * \param[in] name The name of the device to be opened.
  * \return The resulting error code.
  */
int serial_open(
  serial_device_p dev,
  const char* name);

/** \brief Close an open serial device
  * \param[in] dev The open serial device to be closed.
  * \return The resulting error code.
  */
int serial_close(
  serial_device_p dev);

/** \brief Setup an already opened serial device
  * \param[in] dev The open serial device to be set up.
  * \param[in] baud_rate The device baud rate to be set.
  * \param[in] data_bits The device's number of data bits to be set.
  * \param[in] stop_bits The device's number of stop bits to be set.
  * \param[in] parity The device parity to be set.
  * \param[in] flow_ctrl The device flow control to be set.
  * \param[in] timeout The device select timeout to be set in [s].
  * \return The resulting error code.
  */
int serial_setup(
  serial_device_p dev,
  int baud_rate,
  int data_bits,
  int stop_bits,
  serial_parity_t parity,
  serial_flow_ctrl_t flow_ctrl,
  double timeout);

/** \brief Read data from open serial device
  * \param[in] dev The open serial device to read data from.
  * \param[in,out] data An array containing the data read from the device.
  * \param[in] num The number of data bytes to be read.
  * \return The number of bytes read from the serial device or the
  *   negative error code.
  */
int serial_read(
  serial_device_p dev,
  unsigned char* data,
  size_t num);

/** \brief Write data to open serial device
  * \param[in] dev The open serial device to write data to.
  * \param[in] data An array containing the data to be written to the device.
  * \param[in] num The number of data bytes to be written.
  * \return The number of bytes written to the serial device or the
  *   negative error code.
  */
int serial_write(
  serial_device_p dev,
  unsigned char* data,
  size_t num);

#endif
