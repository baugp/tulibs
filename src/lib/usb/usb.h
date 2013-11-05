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

#ifndef USB_H
#define USB_H

/** \file usb.h
  * \brief USB communication library
  * \author Ralf Kaestner
  * A USB communication library providing very basic functionality.
  */

#include <unistd.h>

/** \name Error Codes
  * \brief Predefined USB error codes
  */
//@{
#define USB_ERROR_NONE                    0
#define USB_ERROR_IO                      1
#define USB_ERROR_INVALID_PARAMETER       2
#define USB_ERROR_ACCESS                  3
#define USB_ERROR_NO_DEVICE               4
#define USB_ERROR_NOT_FOUND               5
#define USB_ERROR_BUSY                    6
#define USB_ERROR_TIMEOUT                 7
#define USB_ERROR_OVERFLOW                8
#define USB_ERROR_PIPE                    9
#define USB_ERROR_INTERRUPTED             10
#define USB_ERROR_NO_MEMORY               11
#define USB_ERROR_NOT_SUPPORTED           12
#define USB_ERROR_OTHER                   13
//@}

/** \brief Predefined USB error descriptions
  */
extern const char* usb_errors[];

/** \brief USB debug level enumeratable type
  */
typedef enum {
  usb_debug_level_minimal,          //!< No debugging output.
  usb_debug_level_error,            //!< Error debugging output.
  usb_debug_level_warning,          //!< Warning debugging output.
  usb_debug_level_verbose           //!< Verbose debugging output.
} usb_debug_level_t;

/** \brief USB device class enumeratable type
  */
typedef enum {
  usb_class_per_interface,          //!< Class specified per interface.
  usb_class_audio,                  //!< Audio device class.
  usb_class_comm,                   //!< Communication device class.
  usb_class_hid,                    //!< Human interface device class.
  usb_class_physical,               //!< Physical device class.
  usb_class_image,                  //!< Image device class.
  usb_class_printer,                //!< Printer device class.
  usb_class_mass_storage,           //!< Mass storage device class.
  usb_class_hub,                    //!< Hub device class.
  usb_class_data,                   //!< Data device class.
  usb_class_smart_card,             //!< Smart card device class.
  usb_class_content_security,       //!< Content security device class.
  usb_class_video,                  //!< Video device class.
  usb_class_healthcare,             //!< Healthcare device class.
  usb_class_diagnostic,             //!< Diagnostic device class.
  usb_class_wireless,               //!< Wireless device class.
  usb_class_application,            //!< Application device class.
  usb_class_vendor,                 //!< Vendor-specific device class.
  usb_class_unknown                 //!< Unknown device class.
} usb_class_t;

/** \brief Predefined USB device class strings
  */
extern const char* usb_classes[];

/** \brief USB request type enumeratable type
  */
typedef enum {
  usb_request_type_standard,        //!< Standard request type.
  usb_request_type_class,           //!< Class request type.
  usb_request_type_vendor,          //!< Vendor-specific request type.
  usb_request_type_reserved         //!< Reserved.
} usb_request_type_t;

/** \brief USB recipient enumeratable type
  */
typedef enum {
  usb_recipient_device,             //!< Device recipient.
  usb_recipient_interface,          //!< Interface recipient.
  usb_recipient_endpoint,           //!< Endpoint recipient.
  usb_recipient_other               //!< Other recipient.
} usb_recipient_t;

/** \brief USB direction enumeratable type
  */
typedef enum {
  usb_direction_out,                //!< Host-to-device direction.
  usb_direction_in                  //!< Device-to-host direction.
} usb_direction_t;

/** \brief USB device structure
  */
typedef struct usb_device_t {
  void* libusb_device;            //!< The libusb device.
  void* libusb_handle;            //!< The libusb handle.

  int bus;                        //!< Number of bus device is connected to.
  int address;                    //!< Address on bus device is connected to.

  int vendor_id;                  //!< Vendor ID of device.
  int product_id;                 //!< Product ID of device.
  usb_class_t class;              //!< Class code of device.

  double timeout;                 //!< Device request timeout in [s].
  
  size_t num_read;                //!< Number of bytes read from device.
  size_t num_written;             //!< Number of bytes written to device.
} usb_device_t, *usb_device_p;

/** \brief USB context structure
  */
typedef struct usb_context_t {
  void* libusb_context;           //!< The libusb context.
  usb_debug_level_t debug_level;  //!< Context debug level.
  
  size_t num_devices;             //!< Number of devices in the context.
  usb_device_t* devices;          //!< List of devices in the context.
} usb_context_t, *usb_context_p;

/** \brief USB default context
  * \note The USB default context is a special context which will be
  *   shared amongst its users.
  */
extern usb_context_p usb_default_context;

/** \brief USB control transfer structure
  */
typedef struct usb_control_transfer_t {
  usb_request_type_t request_type;  //!< Control transfer request type.
  usb_recipient_t recipient;        //!< Control transfer recipient.
  usb_direction_t direction;        //!< Control transfer direction.
  
  unsigned char request;            //!< Control transfer request field.
  unsigned char value;              //!< Control transfer value field.
  unsigned char index;              //!< Control transfer index field.
  
  size_t num;                       //!< Number of control transfer data bytes. 
  unsigned char* data;              //!< Control transfer data field.
} usb_control_transfer_t, *usb_control_transfer_p;

/** \brief USB bulk transfer structure
  */
typedef struct usb_bulk_transfer_t {
  unsigned char endpoint_number;    //!< Bulk transfer endpoint number.
  usb_direction_t direction;        //!< Bulk transfer direction.
  
  size_t num;                       //!< Number of bulk transfer data bytes. 
  unsigned char* data;              //!< Bulk transfer data field.
} usb_bulk_transfer_t, *usb_bulk_transfer_p;

/** \brief Initialize a USB context
  * \param[in] context The USB context to be initialized.
  * \return The resulting error code.
  */
int usb_context_init(
  usb_context_p context);

/** \brief Release a USB context
  * \param[in] context The initialized USB context to be release.
  * \return The resulting error code.
  */
int usb_context_release(
  usb_context_p context);

/** \brief Setup an already initialized USB context
  * \param[in] context The initialized USB context to be set up.
  * \param[in] debug_level The context debug level to be set.
  * \return The resulting error code.
  */
int usb_context_setup(
  usb_context_p context,
  usb_debug_level_t debug_level);

/** \brief Refresh device list of the USB context
  * \param[in] context The initialized USB context to be refreshed.
  * \return The resulting error code.
  */
int usb_context_refresh(
  usb_context_p context);

/** \brief Match USB devices by device name
  * \param[in] context The initialized USB context to be searched.
  * \param[in] name The device's udev name.
  * \return The matching device or null.
  */
usb_device_p usb_match_name(
  usb_context_p context,
  const char* name);

/** \brief Match USB devices by vendor and product ID
  * \param[in] context The initialized USB context to be searched.
  * \param[in] vendor_id The device's vendor ID to be matched.
  * \param[in] product_id The device's product ID to be matched.
  * \return The first matching device or null.
  */
usb_device_p usb_match_vendor_product(
  usb_context_p context,
  int vendor_id,
  int product_id);

/** \brief Open the USB device with the specified name
  * \param[in] dev The USB device to be opened.
  * \return The resulting error code.
  */
int usb_open(
  usb_device_p dev);

/** \brief Close an open USB device
  * \param[in] dev The open USB device to be closed.
  * \return The resulting error code.
  */
int usb_close(
  usb_device_p dev);

/** \brief Setup a USB device
  * \param[in] dev The USB device to be set up.
  * \param[in] timeout The device request timeout to be set in [s].
  * \return The resulting error code.
  */
int usb_setup(
  usb_device_p dev,
  double timeout);

/** \brief Read control data from open USB device
  * \param[in] dev The open USB device to read control data from.
  * \param[in] request_type The request type of the control transfer.
  * \param[in] recipient The recipient of the control transfer.
  * \param[in] request The request field of the control transfer.
  * \param[in] value The value field of the control transfer.
  * \param[in] index The index field of the control transfer.
  * \param[in,out] data An array containing the control data read from the
  *   device.
  * \param[in] num The number of control data bytes to be read.
  * \return The number of control data bytes read from the USB device or the
  *   negative error code.
  */
int usb_control_read(
  usb_device_p dev,
  usb_request_type_t request_type,
  usb_recipient_t recipient,
  unsigned char request,
  unsigned char value,
  unsigned char index,
  unsigned char* data,
  size_t num);

/** \brief Write control data to open USB device
  * \param[in] dev The open USB device to write control data to.
  * \param[in] request_type The request type of the control transfer.
  * \param[in] recipient The recipient of the control transfer.
  * \param[in] request The request field of the control transfer.
  * \param[in] value The value field of the control transfer.
  * \param[in] index The index field of the control transfer.
  * \param[in] data An array containing the control data to be written to
  *   the device.
  * \param[in] num The number of control data bytes to be written.
  * \return The number of control data bytes written to the USB device or the
  *   negative error code.
  */
int usb_control_write(
  usb_device_p dev,
  usb_request_type_t request_type,
  usb_recipient_t recipient,
  unsigned char request,
  unsigned char value,
  unsigned char index,
  unsigned char* data,
  size_t num);

/** \brief Read bulk data from open USB device
  * \param[in] dev The open USB device to read bulk data from.
  * \param[in] endpoint_number The number of the endpoint to read bulk
  *   data from.
  * \param[in,out] data An array containing the bulk data read from the device.
  * \param[in] num The number of bulk data bytes to be read.
  * \return The number of bulk data bytes read from the USB device or the
  *   negative error code.
  */
int usb_bulk_read(
  usb_device_p dev,
  unsigned char endpoint_number,
  unsigned char* data,
  size_t num);

/** \brief Write bulk data to open USB device
  * \param[in] dev The open USB device to write bulk data to.
  * \param[in] endpoint_number The number of the endpoint to write bulk
  *   data to.
  * \param[in] data An array containing the bulk data to be written to
  *   the device.
  * \param[in] num The number of bulk data bytes to be written.
  * \return The number of bulk data bytes written to the USB device or the
  *   negative error code.
  */
int usb_bulk_write(
  usb_device_p dev,
  unsigned char endpoint_number,
  unsigned char* data,
  size_t num);

/** \brief Perform synchronous USB control transfer
  * \param[in] dev The open USB device to communicate with.
  * \param[in,out] transfer The control transfer to be performed.
  * \return The resulting error code.
  */
int usb_control_transfer(
  usb_device_p dev,
  usb_control_transfer_p transfer);

/** \brief Perform synchronous USB bulk transfer
  * \param[in] dev The open USB device to communicate with.
  * \param[in,out] transfer The bulk transfer to be performed.
  * \return The resulting error code.
  */
int usb_bulk_transfer(
  usb_device_p dev,
  usb_bulk_transfer_p transfer);

/** \brief Print a USB device
  * \param[in] stream The output stream that will be used for printing the
  *   USB device.
  * \param[in] dev The USB device that will be printed.
  */
void usb_print(
  FILE* stream,
  usb_device_p dev);

#endif
