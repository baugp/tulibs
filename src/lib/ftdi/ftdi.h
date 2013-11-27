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

#ifndef FTDI_H
#define FTDI_H

/** \defgroup ftdi FTDI Communication Module
  * \brief Library functions for FTDI device support
  * 
  * The FTDI module provides library functions for working with FTDI's USB
  * to serial UART communication devices.
  */

/** \file ftdi/ftdi.h
  * \ingroup ftdi
  * \brief FTDI device interface
  * \author Ralf Kaestner
  * 
  * The FTDI device interface provides high-level support for enumerating,
  * configuring, and operating FTDI's USB to serial UART devices. Major parts
  * of the implementation are based on the open libftdi API.
  */

#include <unistd.h>

#include "error/error.h"

/** \brief Predefined FTDI vendor identifier
  */
#define FTDI_VENDOR_ID                      0x0403

/** \name Error Codes
  * \brief Predefined FTDI error codes
  */
//@{
#define FTDI_ERROR_NONE                     0
//!< Success
#define FTDI_ERROR_CONTEXT_INIT             1
//!< Error initializing FTDI context
#define FTDI_ERROR_CONTEXT_RELEASE          2
//!< Error releasing FTDI context
#define FTDI_ERROR_INVALID_CONTEXT          3
//!< Invalid FTDI context
#define FTDI_ERROR_OPEN                     4
//!< Error opening FTDI device
#define FTDI_ERROR_CLOSE                    5
//!< Error closing FTDI device
#define FTDI_ERROR_PURGE                    6
//!< Error purging FTDI device
#define FTDI_ERROR_INVALID_INTERFACE        7
//!< Invalid interface
#define FTDI_ERROR_INVALID_BAUD_RATE        8
//!< Invalid baud rate
#define FTDI_ERROR_INVALID_DATA_BITS        9
//!< Invalid number of data bits
#define FTDI_ERROR_INVALID_STOP_BITS        10
//!< Invalid number of stop bits
#define FTDI_ERROR_INVALID_PARITY           11
//!< Invalid parity
#define FTDI_ERROR_INVALID_FLOW_CTRL        12
//!< Invalid flow control
#define FTDI_ERROR_INVALID_BREAK            13
//!< Invalid break
#define FTDI_ERROR_INVALID_LATENCY          14
//!< Invalid latency
#define FTDI_ERROR_SETUP                    15
//!< Error setting FTDI device parameters
#define FTDI_ERROR_TIMEOUT                  16
//!< FTDI device select timeout
#define FTDI_ERROR_READ                     17
//!< Error reading from FTDI device
#define FTDI_ERROR_WRITE                    18
//!< Error writing to FTDI device
//@}

/** \brief Predefined FTDI error descriptions
  */
extern const char* ftdi_errors[];

/** \brief Chip enumerable type
  */
typedef enum {
  ftdi_chip_am,                 //!< FTDI AM chip series.
  ftdi_chip_bm,                 //!< FTDI BM chip series.
  ftdi_chip_2232c,              //!< FTDI 2232C chip series.
  ftdi_chip_r,                  //!< FTDI R chip series.
  ftdi_chip_2232h,              //!< FTDI 2232H chip series.
  ftdi_chip_4232h,              //!< FTDI 4232H chip series.
  ftdi_chip_232h,               //!< FTDI 232H chip series.
  ftdi_chip_unkown              //!< Unkown chip series.
} ftdi_chip_t;

/** \brief Predefined FTDI chip descriptions
  */
extern const char* ftdi_chips[];

/** \brief Interface enumerable type
  */
typedef enum {
  ftdi_interface_any,           //!< Any interface.
  ftdi_interface_a,             //!< Interface A.
  ftdi_interface_b,             //!< Interface B.
  ftdi_interface_c,             //!< Interface C.
  ftdi_interface_d              //!< Interface D.
} ftdi_interface_t;

/** \brief Parity enumerable type
  */
typedef enum {
  ftdi_parity_none,             //!< No parity.
  ftdi_parity_odd,              //!< Odd parity.
  ftdi_parity_even,             //!< Even parity.
  ftdi_parity_mark,             //!< Mark parity.
  ftdi_parity_space             //!< Space parity.
} ftdi_parity_t;

/** \brief Flow control enumerable type
  */
typedef enum {
  ftdi_flow_ctrl_off,           //!< Disable flow control.
  ftdi_flow_ctrl_xon_xoff,      //!< XON/XOFF flow control.
  ftdi_flow_ctrl_rts_cts,       //!< RTS/CTS (hardware) flow control.
  ftdi_flow_ctrl_dtr_dsr        //!< DTR/DSR (hardware) flow control.
} ftdi_flow_ctrl_t;

/** \brief Break enumerable type
  */
typedef enum {
  ftdi_break_off,               //!< Break off.
  ftdi_break_on                 //!< Break on.
} ftdi_break_t;

/** \brief FTDI device structure
  * \note The life-cycle of an FTDI device is managed by its context.
  */
typedef struct ftdi_device_t {
  void* libftdi_context;          //!< The libftdi context.
  void* libusb_device;            //!< The libusb device.

  int bus;                        //!< Number of bus device is connected to.
  int address;                    //!< Address on bus device is connected to.

  int product_id;                 //!< Product ID of device.
  ftdi_chip_t chip;               //!< Device chip type.
  ftdi_interface_t interface;     //!< Device interface number.
    
  int baud_rate;                  //!< Device baud rate in [baud].
  int data_bits;                  //!< Number of data bits.
  int stop_bits;                  //!< Number of stop bits.
  ftdi_parity_t parity;           //!< Device parity.
  ftdi_flow_ctrl_t flow_ctrl;     //!< Device flow control.
  ftdi_break_t break_type;        //!< Device break enabled.

  double timeout;                 //!< Device timeout in [s].
  double latency;                 //!< Device latency in [s].

  size_t num_read;                //!< Number of bytes read from device.
  size_t num_written;             //!< Number of bytes written to device.
  
  error_t error;                  //!< The most recent device error.
} ftdi_device_t;

/** \brief FTDI context structure
  */
typedef struct ftdi_context_t {
  size_t num_devices;             //!< Number of devices in the context.
  ftdi_device_t* devices;         //!< List of devices in the context.
  
  size_t num_references;          //!< Number of references to the context.

  error_t error;                  //!< The most recent context error.  
} ftdi_context_t;

/** \brief FTDI default context
  * 
  * The FTDI default context is a special context which will be shared
  * amongst its users.
  */
extern ftdi_context_t* ftdi_default_context;

/** \brief Initialize an FTDI context
  * \param[in] context The FTDI context to be initialized.
  * \return The resulting error code.
  */
int ftdi_context_init(
  ftdi_context_t* context);

/** \brief Release an FTDI context
  * \param[in] context The initialized FTDI context to be released.
  * \return The resulting error code.
  */
int ftdi_context_release(
  ftdi_context_t* context);

/** \brief Refresh device list of the FTDI context
  * \param[in] context The initialized FTDI context to be refreshed.
  * \return The resulting error code.
  */
int ftdi_context_refresh(
  ftdi_context_t* context);

/** \brief Match devices in FTDI context by device name
  * \param[in] context The initialized FTDI context to be searched.
  * \param[in] name The device's udev name.
  * \return The matching device or null.
  */
ftdi_device_t* ftdi_context_match_name(
  const ftdi_context_t* context,
  const char* name);

/** \brief Match devices in FTDI context by product ID
  * \param[in] context The initialized FTDI context to be searched.
  * \param[in] product_id The device's product ID to be matched.
  * \return The first matching device or null.
  */
ftdi_device_t* ftdi_context_match_product(
  const ftdi_context_t* context,
  int product_id);

/** \brief Match devices in FTDI context by chip
  * \param[in] context The initialized FTDI context to be searched.
  * \param[in] chip The device's chip to be matched.
  * \return The first matching device or null.
  */
ftdi_device_t* ftdi_context_match_chip(
  const ftdi_context_t* context,
  ftdi_chip_t chip);

/** \brief Open the FTDI device with the specified name
  * \param[in] dev The FTDI device to be opened.
  * \param[in] interface The device interface to be opened.
  * \return The resulting error code.
  */
int ftdi_device_open(
  ftdi_device_t* dev,
  ftdi_interface_t interface);

/** \brief Close an open FTDI device
  * \param[in] dev The open FTDI device to be closed.
  * \return The resulting error code.
  */
int ftdi_device_close(
  ftdi_device_t* dev);

/** \brief Setup an already opened FTDI device
  * \param[in] dev The open FTDI device to be set up.
  * \param[in] baud_rate The device baud rate to be set in [baud].
  * \param[in] data_bits The device's number of data bits to be set.
  * \param[in] stop_bits The device's number of stop bits to be set.
  * \param[in] parity The device parity to be set.
  * \param[in] flow_ctrl The device flow control to be set.
  * \param[in] break_type The device break type to be set.
  * \param[in] timeout The device timeout to be set in [s].
  * \param[in] latency The device latency to be set in [s].
  * \return The resulting error code.
  */
int ftdi_device_setup(
  ftdi_device_t* dev,
  int baud_rate,
  int data_bits,
  int stop_bits,
  ftdi_parity_t parity,
  ftdi_flow_ctrl_t flow_ctrl,
  ftdi_break_t break_type,
  double timeout,
  double latency);

/** \brief Read data from open FTDI device
  * \param[in] dev The open FTDI device to read data from.
  * \param[in,out] data An array containing the data read from the device.
  * \param[in] num The number of data bytes to be read.
  * \return The number of bytes read from the FTDI device or the
  *   negative error code.
  */
int ftdi_device_read(
  ftdi_device_t* dev,
  unsigned char* data,
  size_t num);

/** \brief Write data to open FTDI device
  * \param[in] dev The open FTDI device to write data to.
  * \param[in] data An array containing the data to be written to the device.
  * \param[in] num The number of data bytes to be written.
  * \return The number of bytes written to the FTDI device or the
  *   negative error code.
  */
int ftdi_device_write(
  ftdi_device_t* dev,
  unsigned char* data,
  size_t num);

/** \brief Print FTDI device
  * \param[in] stream The output stream that will be used for printing the
  *   FTDI device.
  * \param[in] dev The FTDI device that will be printed.
  */
void ftdi_device_print(
  FILE* stream,
  const ftdi_device_t* dev);

#endif
