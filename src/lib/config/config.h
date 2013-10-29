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

#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>

#include "param.h"

/** \file config.h
  * \brief Simple configuration implementation
  * \author Ralf Kaestner
  * A POSIX-compliant configuration implementation.
  */

/** \name Arguments
  * \brief Predefined configuration arguments
  */
//@{
#define CONFIG_ARG_HELP                         "--help"
//@}

/** \name Error Codes
  * \brief Predefined configuration error codes
  */
//@{
#define CONFIG_ERROR_NONE                       0
#define CONFIG_ERROR_PARAM_KEY                  1
#define CONFIG_ERROR_PARAM_VALUE                2
//@}

/** \brief Predefined configuration error descriptions
  */
extern const char* config_errors[];

/** \brief Configuration structure
  */
typedef struct config_t {
  config_param_p params;    //!< The configuration parameters.
  size_t num_params;        //!< The number of configuration parameters.
} config_t, *config_p;

/** \brief Initialize an empty configuration
  * \param[in] config The configuration to be initialized.
  */
void config_init(
  config_p config);

/** \brief Initialize a configuration by copying
  * \param[in] config The configuration to be initialized.
  * \param[in] src_config The source configuration used to initialize the
  *   configuration.
  */
void config_init_copy(
  config_p config,
  config_p src_config);

/** \brief Destroy a configuration
  * \param[in] config The configuration to be destroyed.
  */
void config_destroy(
  config_p config);

/** \brief Copy a configuration
  * \param[in] dst_config The destination configuration.
  * \param[in] src_config The source configuration.
  */
void config_copy(
  config_p dst_config,
  config_p src_config);

/** \brief Print a configuration
  * \param[in] stream The output stream that will be used for printing the
  *   configuration.
  * \param[in] config The configuration that will be printed.
  */
void config_print(
  FILE* stream,
  config_p config);

/** \brief Set configuration parameter values from a source configuration
  * \param[in] dst_config The configuration to set the parameter values for.
  * \param[in] src_config The configuration containing the source parameter
  *    values to be set.
  * \return The resulting error code.
  */
int config_set(
  config_p dst_config,
  config_p src_config);

/** \brief Set a configuration parameter
  * \note If a parameter with the same key already exists in the configuration,
  *   it will be replaced. Otherwise, the parameter will be inserted into the
  *   configuration.
  * \param[in] config The configuration to set the parameter for.
  * \param[in] param The parameter to be set.
  * \return The modified or inserted configuration parameter.
  */
config_param_p config_set_param(
  config_p config,
  config_param_p param);

/** \brief Retrieve a configuration parameter
  * \param[in] config The configuration to retrieve the parameter from.
  * \param[in] key The key of the parameter to be retrieved.
  * \return The configuration parameter with the specified key or null
  *   if no such parameter exists in the configuration.
  */
config_param_p config_get_param(
  config_p config,
  const char* key);

/** \brief Set a configuration parameter's string value
  * \param[in] config The configuration to set the string value for.
  * \param[in] key The key of the string value to be set.
  * \param[in] value The string value to be set.
  * \return The resulting error code.
  */
int config_set_string(
  config_p config,
  const char* key,
  const char* value);

/** \brief Retrieve a configuration parameter's string value
  * \param[in] config The configuration to retrieve the string value from.
  * \param[in] key The key of the string value to be retrieved.
  * \return The parameter's string value or null if no such parameter
  *   exists in the configuration.
  */
const char* config_get_string(
  config_p config,
  const char* key);

/** \brief Set a configuration parameter's integer value
  * \param[in] config The configuration to set the integer value for.
  * \param[in] key The key of the integer value to be set.
  * \param[in] value The integer value to be set.
  * \return The resulting error code.
  */
int config_set_int(
  config_p config,
  const char* key,
  int value);

/** \brief Retrieve a configuration parameter's integer value
  * \param[in] config The configuration to retrieve the integer value from.
  * \param[in] key The key of the integer value to be retrieved.
  * \return The parameter's integer value or zero if no such parameter
  *   exists in the configuration.
  */
int config_get_int(
  config_p config,
  const char* key);

/** \brief Set a configuration parameter's floating point value
  * \param[in] config The configuration to set the floating point value for.
  * \param[in] key The key of the floating point value to be set.
  * \param[in] value The floating point value to be set.
  * \return The resulting error code.
  */
int config_set_float(
  config_p config,
  const char* key,
  double value);

/** \brief Retrieve a configuration parameter's floating point value
  * \param[in] config The configuration to retrieve the floating point
  *   value from.
  * \param[in] key The key of the floating point value to be retrieved.
  * \return The parameter's floating point value or NaN if no such parameter
  *   exists in the configuration.
  */
double config_get_float(
  config_p config,
  const char* key);

/** \brief Set a configuration parameter's enumerable value
  * \param[in] config The configuration to set the enumerable value for.
  * \param[in] key The key of the enumerable value to be set.
  * \param[in] value The enumerable value to be set.
  * \return The resulting error code.
  */
int config_set_enum(
  config_p config,
  const char* key,
  int value);

/** \brief Retrieve a configuration parameter's enumerable value
  * \param[in] config The configuration to retrieve the enumerable
  *   value from.
  * \param[in] key The key of the enumerable value to be retrieved.
  * \return The parameter's enumerable value or -1 if no such parameter
  *   exists in the configuration.
  */
int config_get_enum(
  config_p config,
  const char* key);

/** \brief Set a configuration parameter's boolean value
  * \param[in] config The configuration to set the boolean value for.
  * \param[in] key The key of the boolean value to be set.
  * \param[in] value The boolean value to be set.
  * \return The resulting error code.
  */
int config_set_bool(
  config_p config,
  const char* key,
  config_param_bool_t value);

/** \brief Retrieve a configuration parameter's boolean value
  * \param[in] config The configuration to retrieve the boolean
  *   value from.
  * \param[in] key The key of the boolean value to be retrieved.
  * \return The parameter's boolean value or false if no such parameter
  *   exists in the configuration.
  */
config_param_bool_t config_get_bool(
  config_p config,
  const char* key);

#endif
