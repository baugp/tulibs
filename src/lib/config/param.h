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

#ifndef CONFIG_PARAM_H
#define CONFIG_PARAM_H

/** \file config/param.h
  * \ingroup config
  * \brief Simple parameter implementation
  * \author Ralf Kaestner
  * 
  * A configuration parameter stores a key, a value type, the corresponding
  * value representation in the form of a string, a formal expression for
  * the permissible range of its values, and a description used by the
  * command line help and manual page generating functions. Due to the lack
  * of template meta-programming in C, type-compatibility is enforced at the
  * runtime level.
  */

#include <stdio.h>

/** \name Error Codes
  * \brief Predefined configuration parameter error codes
  */
//@{
#define CONFIG_PARAM_ERROR_NONE                0
//!< Success
#define CONFIG_PARAM_ERROR_TYPE                1
//!< Parameter value type mismatch
#define CONFIG_PARAM_ERROR_RANGE               2
//!< Parameter value out of range
//@}

/** \brief Predefined configuration parameter error descriptions
  */
extern const char* config_param_errors[];

/** \brief Configuration parameter type
  */
typedef enum {
  config_param_type_string,     //!< Parameter type is string.
  config_param_type_int,        //!< Parameter type is integer.
  config_param_type_float,      //!< Parameter type is floating point.
  config_param_type_enum,       //!< Parameter type is enumerable.
  config_param_type_bool        //!< Parameter type is boolean.
} config_param_type_t;

/** \brief Predefined configuration parameter type string representations
  */
extern const char* config_param_types[];

/** \brief Configuration parameter booleans
  */
typedef enum {
  config_param_false,           //!< Parameter value is false.
  config_param_true             //!< Parameter value is true.
} config_param_bool_t;

/** \brief Parameter structure
  */
typedef struct config_param_t {
  char* key;                    //!< The parameter's key.
  config_param_type_t type;     //!< The parameter's value type.
  char* value    ;              //!< The parameter's value.
  
  char* range;                  //!< The parameter's range.
  char* description;            //!< The parameter's description.
} config_param_t;

/** \brief Initialize a parameter without value
  * \param[in] param The parameter to be initialized.
  * \param[in] key The key of the parameter to be initialized.
  * \param[in] type The type of the parameter value to be initialized.
  * \param[in] description An optional description of the parameter.
  */
void config_param_init(
  config_param_t* param,
  const char* key,
  config_param_type_t type,
  const char* description);

/** \brief Initialize a parameter by value and range
  * \param[in] param The parameter to be initialized.
  * \param[in] key The key of the parameter to be initialized.
  * \param[in] type The type of the parameter value to be initialized.
  * \param[in] value The string representation of the parameter's value.
  * \param[in] range An optional formal string expression representing
  *   the permissible range of the parameter value. For enumerable parameters,
  *   the range may be specified as logic concatenation val1|val2|... of valid
  *   string representations. For boolean parameters, the range val1|val2
  *   similarly defines the string representations for false and true,
  *   respectively. For numerical parameters, the expression may take the
  *   form of an open interval [min_value, max_value], a closed interval
  *   (min_value, max_value), or combinations thereof.
  * \param[in] description An optional description of the parameter.
  * \return The resulting error code.
  * 
  * This function generically constructs a parameter from string
  * representations for its value and range.
  */
int config_param_init_value_range(
  config_param_t* param,
  const char* key,
  config_param_type_t type,
  const char* value,
  const char* range,
  const char* description);

/** \brief Initialize a string parameter by value
  * \param[in] param The parameter to be initialized.
  * \param[in] key The key of the parameter to be initialized.
  * \param[in] value The string value of the parameter to be initialized.
  * \param[in] description An optional description of the parameter.
  * \return The resulting error code.
  */
int config_param_init_string(
  config_param_t* param,
  const char* key,
  const char* value,
  const char* description);

/** \brief Initialize a string parameter by value and range
  * \param[in] param The parameter to be initialized.
  * \param[in] key The key of the parameter to be initialized.
  * \param[in] value The string value of the parameter to be initialized.
  * \param[in] range An optional formal string expression representing
  *   the permissible range of the parameter value. For enumerable parameters,
  *   the range may be specified as set {val1, val2, ...} of valid string
  *   representations. For numerical parameters, the expression may take the
  *   form of an open interval [min_value, max_value], a closed interval
  *   (min_value, max_value), or combinations thereof.
  * \param[in] description An optional description of the parameter.
  * \return The resulting error code.
  */
int config_param_init_string_range(
  config_param_t* param,
  const char* key,
  const char* value,
  const char* range,
  const char* description);

/** \brief Initialize an integer parameter by value
  * \param[in] param The parameter to be initialized.
  * \param[in] key The key of the parameter to be initialized.
  * \param[in] value The integer value of the parameter to be initialized.
  * \param[in] description An optional description of the parameter.
  */
void config_param_init_int(
  config_param_t* param,
  const char* key,
  int value,
  const char* description);

/** \brief Initialize an integer parameter by value and range
  * \param[in] param The parameter to be initialized.
  * \param[in] key The key of the parameter to be initialized.
  * \param[in] value The integer value of the parameter to be initialized.
  * \param[in] min_value The minimum permissible integer value of the
  *   parameter.
  * \param[in] max_value The maximum permissible integer value of the
  *   parameter.
  * \param[in] description An optional description of the parameter.
  * \return The resulting error code.
  */
int config_param_init_int_range(
  config_param_t* param,
  const char* key,
  int value,
  int min_value,
  int max_value,
  const char* description);

/** \brief Initialize a floating point parameter by value
  * \param[in] param The parameter to be initialized.
  * \param[in] key The key of the parameter to be initialized.
  * \param[in] value The floating point value of the parameter to be
  * \param[in] description An optional description of the parameter.
  */
void config_param_init_float(
  config_param_t* param,
  const char* key,
  double value,
  const char* description);

/** \brief Initialize a floating point parameter by value and range
  * \param[in] param The parameter to be initialized.
  * \param[in] key The key of the parameter to be initialized.
  * \param[in] value The floating point value of the parameter to be
  * \param[in] min_value The minimum permissible floating point value
  *   of the parameter.
  * \param[in] max_value The maximum permissible floating point value
  *   of the parameter.
  * \param[in] description An optional description of the parameter.
  * \return The resulting error code.
  */
int config_param_init_float_range(
  config_param_t* param,
  const char* key,
  double value,
  double min_value,
  double max_value,
  const char* description);

/** \brief Initialize an enumerable parameter by value and range
  * \param[in] param The parameter to be initialized.
  * \param[in] key The key of the parameter to be initialized.
  * \param[in] value The enumerable value of the parameter to be
  *   initialized.
  * \param[in] values A null pointer-terminated list of permissible string
  *   values for the enumerable parameter. Note that the enumerable value
  *   will be interpreted as an index into this list.
  * \param[in] description An optional description of the parameter.
  * \return The resulting error code.
  */
int config_param_init_enum_range(
  config_param_t* param,
  const char* key,
  int value,
  const char** values,
  const char* description);

/** \brief Initialize a boolean parameter by value
  * \param[in] param The parameter to be initialized.
  * \param[in] key The key of the parameter to be initialized.
  * \param[in] value The boolean value of the parameter to be initialized.
  * \param[in] description An optional description of the parameter.
  */
void config_param_init_bool(
  config_param_t* param,
  const char* key,
  config_param_bool_t value,
  const char* description);

/** \brief Initialize a boolean parameter by value and range
  * \param[in] param The parameter to be initialized.
  * \param[in] key The key of the parameter to be initialized.
  * \param[in] value The boolean value of the parameter to be initialized.
  * \param[in] false_value The string representation corresponding to a
  *   false value of the parameter.
  * \param[in] true_value The string representation corresponding to a
  *   true value of the parameter.
  * \param[in] description An optional description of the parameter.
  */
void config_param_init_bool_range(
  config_param_t* param,
  const char* key,
  config_param_bool_t value,
  const char* false_value,
  const char* true_value,
  const char* description);

/** \brief Initialize a configuration parameter by copying
  * \param[in] param The configuration parameter to be initialized.
  * \param[in] src_param The source configuration parameter used to
  *   initialize the configuration parameter.
  */
void config_param_init_copy(
  config_param_t* param,
  const config_param_t* src_param);

/** \brief Destroy a configuration parameter
  * \param[in] param The parameter to be destroyed.
  */
void config_param_destroy(
  config_param_t* param);

/** \brief Copy a parameter
  * \param[in] dst The destination parameter.
  * \param[in] src The source parameter.
  */
void config_param_copy(
  config_param_t* dst,
  const config_param_t* src);

/** \brief Print a parameter
  * \param[in] stream The output stream that will be used for printing the
  *   parameter.
  * \param[in] param The parameter that will be printed.
  */
void config_param_print(
  FILE* stream,
  const config_param_t* param);

/** \brief Set a parameter's value
  * \param[in] param The parameter to set the value for.
  * \param[in] value The string representation of the value to be set.
  * \return The resulting error code.
  * 
  * This function generically attempts to convert a string representation
  * of the parameter's value into an equivalent representation of the
  * parameter's predefined type and verifies its validity with respect to
  * the parameter's range.
  */
int config_param_set_value(
  config_param_t* param,
  const char* value);

/** \brief Get a parameter's value
  * \param[in] param The parameter to get the value for.
  * \return The string representation of the parameter's value.
  * 
  * This function generically returns a string representation of the
  * parameter's value.
  */
const char* config_param_get_value(
  const config_param_t* param);

/** \brief Set a parameter's string value
  * \param[in] param The parameter to set the string value for.
  * \param[in] value The string value to be set.
  * \return The resulting error code.
  */
int config_param_set_string(
  config_param_t* param,
  const char* value);

/** \brief Retrieve a parameter's string value
  * \param[in] param The parameter to retrieve the string value from.
  * \return The parameter's string value or null if the parameter
  *   type mismatches.
  */
const char* config_param_get_string(
  const config_param_t* param);

/** \brief Set a parameter's integer value
  * \param[in] param The parameter to set the integer value for.
  * \param[in] value The integer value to be set.
  * \return The resulting error code.
  */
int config_param_set_int(
  config_param_t* param,
  int value);

/** \brief Retrieve a parameter's integer value
  * \param[in] param The parameter to retrieve the integer value from.
  * \return The parameter's integer value or zero if the parameter type
  *   mismatches or if the parameter value does not represent a valid
  *   integer value.
  */
int config_param_get_int(
  const config_param_t* param);

/** \brief Set a parameter's floating point value
  * \param[in] param The parameter to set the floating point value for.
  * \param[in] value The floating point value to be set.
  * \return The resulting error code.
  */
int config_param_set_float(
  config_param_t* param,
  double value);

/** \brief Retrieve a parameter's floating point value
  * \param[in] param The parameter to retrieve the floating point value from.
  * \return The parameter's floating point value or NaN if the parameter type
  *   mismatches or if the parameter value does not represent a valid floating
  *   point value.
  */
double config_param_get_float(
  const config_param_t* param);

/** \brief Set a parameter's enumerable value
  * \param[in] param The parameter to set the enumerable value for.
  * \param[in] value The enumerable value to be set.
  * \return The resulting error code.
  */
int config_param_set_enum(
  config_param_t* param,
  int value);

/** \brief Retrieve a parameter's enumerable value
  * \param[in] param The parameter to retrieve the enumerable value from.
  * \return The parameter's enumerable value or -1 if the parameter type
  *   mismatches or if the parameter value does not represent a valid
  *   enumerable value.
  */
int config_param_get_enum(
  const config_param_t* param);

/** \brief Set a parameter's boolean value
  * \param[in] param The parameter to set the boolean value for.
  * \param[in] value The boolean value to be set.
  * \return The resulting error code.
  */
int config_param_set_bool(
  config_param_t* param,
  config_param_bool_t value);

/** \brief Retrieve a parameter's boolean value
  * \param[in] param The parameter to retrieve the boolean value from.
  * \return The parameter's boolean value or false if the parameter type
  *   mismatches or the parameter value does not represent a valid boolean
  *   value.
  */
config_param_bool_t config_param_get_bool(
  const config_param_t* param);

#endif
