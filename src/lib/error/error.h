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

#ifndef ERROR_H
#define ERROR_H

/** \defgroup error Error Handling Module
  * \brief Library functions for error handling support
  * 
  * The error module provides library functions for maintaining conclusive
  * error descriptions across multiple compilation or runtime units and for
  * handling them.
  */

/** \file error/error.h
  * \ingroup error
  * \brief Error maintenance interface
  * \author Ralf Kaestner
  * 
  * The error maintenance interface provides support for the construction
  * and derivation of error descriptions across multiple compilation or
  * runtime units.
  */

#include <stdio.h>

/** \brief Error structure
  */
typedef struct error_t {
  int code;                       //!< The error code.
  const char** descriptions;      //!< The error descriptions.
  
  char* where;                    //!< The location of the error.
  char* what;                     //!< The error explanation.
  
  struct error_t* blame;          //!< The error to blame.
} error_t;

/** \brief Error debugging flag
  * \note This flag influences the output of error_print().
  */
extern int error_debug;

/** \brief Initialize error
  * \param[in] error The error to be initialized.
  * \param[in] descriptions The local descriptions of the error to be
  *   initialized.
  */
void error_init(
  error_t* error,
  const char** descriptions);

/** \brief Initialize error by copying
  * \param[in] error The error to be initialized.
  * \param[in] src_error The source error used for initializing the error.
  */
void error_init_copy(
  error_t* error,
  const error_t* src_error);

/** \brief Destroy error
  * \param[in] error The error to be destroyed.
  */
void error_destroy(
  error_t* error);

/** \brief Copy error
  * \param[in] dst The destination error.
  * \param[in] src The source error.
  */
void error_copy(
  error_t* dst,
  const error_t* src);

/** \brief Set error
  * \param[in] error The error to be set.
  * \param[in] code The code to be set for the error.
  */
void error_set(
  error_t* error,
  int code);

/** \brief Blame another error
  * \param[in] error The error caused by the error to be blamed.
  * \param[in] blame The underlying error to be blamed as being the
  *   cause of this error.
  * \param[in] code The code to be set for the error.
  * 
  * The concept of blaming allows for giving the cause of an error in
  * the form of an underlying error.
  */
void error_blame(
  error_t* error,
  const error_t* blame,
  int code);

/** \brief Set formatted error explanation
  * \param[in] error The error to be set.
  * \param[in] code The code to be set for the error.
  * \param[in] format A string defining the expected format and conversion
  *   specififiers of the error's explanation. This string must be followed
  *   by a variadic list of arguments, where each argument is of appropriate
  *   type.
  */
void error_setf(
  error_t* error,
  int code,
  const char* format,
  ...);

/** \brief Blame another error with formatted explanation
  * \param[in] error The error caused by the error to be blamed.
  * \param[in] blame The underlying error to be blamed as being the
  *   cause of this error.
  * \param[in] code The code to be set for the error.
  * \param[in] format A string defining the expected format and conversion
  *   specififiers of the error's explanation. This string must be followed
  *   by a variadic list of arguments, where each argument is of appropriate
  *   type.
  * 
  * The concept of blaming allows for giving the cause of an error in
  * the form of an underlying error.
  */
void error_blamef(
  error_t* error,
  const error_t* blame,
  int code,
  const char* format,
  ...);

/** \brief Retrieve error
  * \param[in] error The error to be retrieved.
  * \return The error code.
  */
int error_get(
  const error_t* error);

/** \brief Retrieve error description
  * \param[in] error The error to retrieve the description for.
  * \return The error description.
  */
const char* error_get_description(
  const error_t* error);

/** \brief Clear error
  * \param[in] error The error to be cleared.
  * \return The zero error code.
  * 
  * Clearing an error simply is the process of setting a zero error code
  * and destroying its blame.
  */
void error_clear(
  error_t* error);

/** \brief Print error
  * \note The error locations are determined using the function backtrace().
  *   For compiler optimization levels other than zero, they may turn out
  *   to be wrong.
  * \param[in] stream The output stream that will be used for printing the
  *   error.
  * \param[in] error The error that will be printed.
  * 
  * Depending on the current value of error_debug, this function produces
  * different print results. If error_debug is zero, only a description
  * of the provided error will be printed. If error_debug is one, error
  * descriptions will include a location and be printed recursively, starting
  * with the provided error and then tracing through the blamed errors in
  * order.
  */
void error_print(
  FILE* stream,
  const error_t* error);

/** \brief Exit on error
  * \param[in] error The error to exit on.
  * 
  * In case of an error, this function first calls error_print() and then
  * terminates its calling process.
  */
void error_exit(
  const error_t* error);

/** \brief Return on error
  * \param[in] error The error to return on.
  * 
  * In case of an error, this pre-processor macro returns with the error code.
  */
#define error_return(error) if ((error)->code) return (error)->code;

#endif
