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

#ifndef FILE_H
#define FILE_H

#include <stdlib.h>
#include <stdio.h>

/** \defgroup file File Input/Output Module
  * \brief Library functions for managing file input/output
  * 
  * The file module provides library functions for managing file input
  * and output operations, and for working with filesystem paths.
  */

/** \file file/file.h
  * \ingroup file
  * \brief Simple file input/output implementation
  * \author Ralf Kaestner
  * 
  * In addition to standard file input/ouput operations, this implementation
  * opaquely manages gzip-compressed and bzip2-compressed files through the
  * same interface.
  */

/** \name Error Codes
  * \brief Predefined file error codes
  */
//@{
#define FILE_ERROR_NONE                         0
#define FILE_ERROR_NOT_FOUND                    1
#define FILE_ERROR_SEEK                         2
#define FILE_ERROR_TELL                         3
#define FILE_ERROR_OPEN                         4
#define FILE_ERROR_READ                         5
#define FILE_ERROR_WRITE                        6
#define FILE_ERROR_OPERATION                    7
//@}

/** \brief Predefined file error descriptions
  */
extern const char* file_errors[];

/** \brief File compression type
  */
typedef enum {
  file_compression_none,        //!< File is not compressed.
  file_compression_gzip,        //!< File is gzip-compressed.
  file_compression_bzip2        //!< File is bzip2-compressed.
} file_compression_t;

/** \brief File modes
  */
typedef enum {
  file_mode_read,               //!< File is opened for reading.
  file_mode_write,              //!< File is opened for reading and writing.
  file_mode_append              //!< File is opened for appending.
} file_mode_t;

/** \brief Predefined file mode strings
  */
extern const char* file_modes[];

/** \brief File whence indicators
  */
typedef enum {
  file_whence_start,            //!< Indicator is relative to file start.
  file_whence_end,              //!< Indicator is relative to file end.
  file_whence_current           //!< Indicator is relative to current position.
} file_whence_t;

/** \brief File structure
  */
typedef struct file_t {
  char name[512];                   //!< The name of the file.
  void* handle;                     //!< The opaque handle of the file.

  file_compression_t compression;   //!< The compression of the file.

  ssize_t pos;                      //!< The bzip2-file position indicator.
} file_t, *file_p;

/** \brief Initialize file
  * \param[in] file The file to be initialized.
  * \param[in] filename The name of the file to be initialized.
  * \param[in] compression The compression type of the file.
  */
void file_init(
  file_p file,
  const char* filename,
  file_compression_t compression);

/** \brief Initialize file using its name only
  * \note This initializer will infer the file's compression type from
  *   the presented filename.
  * \param[in] file The file to be initialized.
  * \param[in] filename The name of the file to be initialized.
  */
void file_init_name(
  file_p file,
  const char* filename);

/** \brief Check if file exists
  * \param[in] file The initialized file to be checked for existence.
  * \return One if the file exists and zero otherwise.
  */
int file_exists(
  file_p file);

/** \brief Retrieve the file extension
  * \param[in] file The initialized file to retrieve the extension for.
  * \return The extension of the file.
  */
const char* file_get_extension(
  file_p file);

/** \brief Retrieve the file size
  * \note If the file is compressed, this function returns the size
  *   of the uncompressed data stream. Depending on the type of compression,
  *   it may therefore be necessary to uncompress the entire file.
  * \param[in] file The initialized file to retrieve the size for.
  * \return The file size or the negative error code.
  */
ssize_t file_get_size(
  file_p file);

/** \brief Retrieve the actual file size
  * \note If the file is compressed, this function returns the size
  *   of the actual file as reported by the file system.
  * \param[in] file The initialized file to retrieve the actual size for.
  * \return The actual file size or the negative error code.
  */
ssize_t file_get_actual_size(
  file_p file);

/** \brief Open file
  * \note If the file is already open, it will be closed and re-opened.
  *   Further, a compressed file may not support the requested mode in
  *   which case the function will return with an error.
  * \param[in] file The initialized file to be opened.
  * \param[in] mode The mode for opening the file.
  * \return The resulting error code.
  */
int file_open(
  file_p file,
  file_mode_t mode);

/** \brief Open a file by duplication of an open stream
  * \note This function opens the file after duplicating the stream's
  *   associated file descriptor.
  * \param[in] file The initialized file to be opened.
  * \param[in] stream The open stream to be duplicated.
  * \param[in] mode The mode for opening the file. Note that the mode
  *   must generally be compatible with the mode of the open stream.
  * \return The resulting error code.
  * 
  */
int file_open_stream(
  file_p file,
  FILE* stream,
  file_mode_t mode);

/** \brief Close file
  * \note If the file is already closed, this function does nothing.
  * \param[in] file The initialized file to be closed.
  */
void file_close(
  file_p file);

/** \brief Retrieve the end-of-file indicator
  * \param[in] file The open file to retrieve the end-of-file indicator for.
  * \return One if the end-of-file indicator is set and the negative error
  *   code otherwise.
  */
int file_eof(
  file_p file);

/** \brief Retrieve the file error indicator
  * \param[in] file The open file to retrieve the error indicator for.
  * \return One if the file error indicator is set and the negative error
  *   code otherwise.
  */
int file_error(
  file_p file);

/** \brief Set file position indicator
  * \note Depending on the file compression and the relative requested
  *   file position, the function may uncompress all data up to this
  *   position. Seeking reversely from the current file position is
  *   further unsupported for bzip2-compressed files.
  * \param[in] file The open file to set the file position indicator for.
  * \param[in] offset The offset of the file position pointer in bytes.
  * \param[in] whence The whence indicator of the seek operation.
  * \return The current offset of the file position indicator relative to the
  *   file start or the negative error code.
  */
ssize_t file_seek(
  file_p file,
  ssize_t offset,
  file_whence_t whence);

/** \brief Retrieve file position indicator
  * \param[in] file The open file to retrieve the file position indicator for.
  * \return The current offset of the file position indicator relative to the
  *   file start or the negative error code.
  */
ssize_t file_tell(
  file_p file);

/** \brief Read binary data from file
  * \param[in] file The open file to read the binary data from.
  * \param[in,out] data An array of sufficient size to hold the read data.
  * \param[in] size The requested number of bytes to read from the file.
  * \return The number of bytes actually read from the file or the negative
  *   error code.
  */
ssize_t file_read(
  file_p file,
  unsigned char* data,
  size_t size);

/** \brief Write binary data to file
  * \param[in] file The open file to write the binary data to.
  * \param[in] data An array holding the data to be written.
  * \param[in] size The requested number of bytes to write to the file.
  * \return The number of bytes actually written to the file or the negative
  *   error code.
  */
ssize_t file_write(
  file_p file,
  const unsigned char* data,
  size_t size);

/** \brief Read line from file
  * \param[in] file The open file to read the line from.
  * \param[in,out] line The pointer to a string whose length will dynamically
  *   be increased in order to hold all characters in the read line except for
  *   the trailing new-line character. If the line does not contain any such
  *   characters, the string will not be modified. Otherwise, it remains the
  *   caller's duty to free the allocated string.
  * \param[in] block_size The size of the string blocks to be allocated
  *   dynamically. Any time the line length exceeds the length of the
  *   allocated string buffer, the size of that buffer will be increased by
  *   the given block size.
  * \return The number of line characters read or the negative error code.
  */
ssize_t file_read_line(
  file_p file,
  char** line,
  size_t block_size);

/** \brief Write formatted data to file
  * \param[in] file The open file to write the formatted data to.
  * \param[in] format A string defining the expected format and conversion
  *   specififiers of the data to be written. This string must be followed by
  *   a variadic list of variables, where each variable is of appropriate
  *   type.
  * \return The number of characters written to the file or the negative
  *   error code.
  */
ssize_t file_printf(
  file_p file,
  const char* format,
  ...);

/** \brief Write buffered data to file
  * \param[in] file The open file to flush.
  * \return The resulting error code.
  */
int file_flush(
  file_p file);

#endif
