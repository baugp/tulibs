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

/** \file config.h
  * \brief Simple configuration implementation
  * \author Ralf Kaestner
  * A POSIX-compliant configuration implementation.
  */

#include <stdlib.h>

#include "param.h"

/** \brief Configuration structure
  */
typedef struct config_t {
  param_p params;         //!< The configuration parameters.
  ssize_t num_params;     //!< The number of configuration parameters.
} config_t, *config_p;

/** \brief Initialize an empty configuration
  * \param[in] config The configuration to be initialized.
  */
void config_init(
  config_p config);

/** \brief Initialize a configuration from default parameters
  * \param[in] config The configuration to be initialized.
  * \param[in] params The default configuration parameters used to initialize
  *   the configuration.
  * \param[in] num_params The number of default configuration parameters.
  */
void config_init_default(
  config_p config,
  param_p params,
  ssize_t num_params);

/** \brief Destroy a configuration
  * \param[in] config The configuration to be destroyed.
  */
void config_destroy(
  config_p config);

/** \brief Print a configuration
  * \param[in] stream The output stream that will be used for printing the
  *   configuration.
  * \param[in] config The configuration that will be printed.
  */
void config_print(
  FILE* stream,
  config_p config);

/** \brief Set a configuration parameter
  * \note If a parameter with the same key already exists in the configuration,
  *   its value will be modified accordingly. Otherwise, a new parameter
  *   will be appended to the configuration.
  * \param[in] config The configuration to set the parameter for.
  * \param[in] param The parameter to be set.
  */
void config_set_param(
  config_p config,
  param_p param);

/** \brief Retrieve a configuration parameter
  * \param[in] config The configuration to retrieve the parameter from.
  * \param[in] key The key of the parameter to be retrieved.
  * \return The configuration parameter with the specified key or null,
  *   if no such parameter exists.
  */
param_p config_get_param(
  config_p config,
  const char* key);

/** \brief Set configuration parameters from command line arguments
  * \param[in] config The configuration to set the parameters for.
  * \param[in] argc The number of supplied command line arguments.
  * \param[in] argv The list of supplied command line arguments.
  * \param[in] key_prefix An optional key prefix that will be stripped
  *    from the parameter keys.
  * \return The number of matched parameters.
  */
ssize_t config_set_arg_params(
  config_p config,
  int argc,
  char **argv,
  const char* key_prefix);

#endif
