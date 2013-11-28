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

#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

#include <stdlib.h>

#include "config/config.h"

#include "file/file.h"

#include "error/error.h"

/** \file config/file.h
  * \ingroup config
  * \brief Configuration file input/output functions
  * \author Ralf Kaestner
  * 
  * The functions provided in this file facilitate automated generation
  * of configuration files from configuration parameters and reading back
  * the values of configuration parameters from such configuration files.
  * In most applications, they should be used indirectly through the
  * configuration parser interface.
  */

/** \brief Predefined configuration file comment character
  */
#define CONFIG_FILE_COMMENT_START                    "#"

/** \brief Predefined configuration file parser option group
  */
#define CONFIG_FILE_PARSER_OPTION_GROUP              "config-file"

/** \name Parameters
  * \brief Predefined manual page parameters
  */
//@{
#define CONFIG_FILE_PARAMETER_TITLE                  "title"
#define CONFIG_FILE_PARAMETER_MAX_WIDTH              "max-width"
#define CONFIG_FILE_PARAMETER_COMMENT_LEVEL          "comment-level"
//@}

/** \name Error Codes
  * \brief Predefined configuration file error codes
  */
//@{
#define CONFIG_FILE_ERROR_NONE                       0
//!< Success
#define CONFIG_FILE_ERROR_READ                       1
//!< Failed to read configuration file
#define CONFIG_FILE_ERROR_WRITE                      2
//!< Failed to write configuration file
#define CONFIG_FILE_ERROR_FORMAT                     3
//!< Invalid configuration file format
//@}

/** \brief Predefined configuration file error descriptions
  */
extern const char* config_file_errors[];

/** \brief Predefined configuration file default options
  */
extern const config_default_t config_file_default_options;

/** \brief Predefined configuration file description
  */
extern const char* config_file_description;

/** \brief Configuration file comment levels
  */
typedef enum {
  config_file_comment_level_minimal,  //!< Minimal comments in file.
  config_file_comment_level_sections, //!< Comment configuration file sections.
  config_file_comment_level_verbose   //!< Verbose comments in file.
} config_file_comment_level_t;

/** \brief Configuration file variable structure
  */
typedef struct config_file_var_t {
  char *name;               //!< The name of the variable.
  char *value;              //!< The value of the variable.
  
  char* description;        //!< A description of the variable.
} config_file_var_t;

/** \brief Configuration file section structure
  */
typedef struct config_file_section_t {
  char* name;               //!< The name of the configuration file section.
  char* title;              //!< A title of the configuration file section.

  config_file_var_t* vars;  //!< The file section's variables.
  size_t num_vars;          //!< The file section's number of variables.
} config_file_section_t;

/** \brief Configuration file structure
  */
typedef struct config_file_t {
  char* title;              //!< The configuration file title.
  
  config_file_section_t*
    sections;               //!< The configuration file sections.
  size_t num_sections;      //!< The number of configuration file sections.
  
  size_t max_width;         //!< The maximum character width of the file.
  config_file_comment_level_t
    comment_level;          //!< The file comment level.

  error_t error;            //!< The most recent configuration file error.
} config_file_t;

/** \brief Initialize configuration file
  * \param[in] file The configuration file to be initialized.
  * \param[in] title An optional title of the configuration file to be
  *   initialized. This parameter only takes effect during generation of
  *   the configuration file.
  * \param[in] max_width The maximum character width of the configuration
  *   file. This parameter only takes effect on comments during generation
  *   of the configuration file.
  * \param[in] comment_level The comment level of the configuration file.
  *   This parameter only takes effect on comments during generation of
  *   the configuration file.
  */
void config_file_init(
  config_file_t* file,
  const char* title,
  size_t max_width,
  config_file_comment_level_t comment_level);

/** \brief Initialize configuration file from configuration parameters
  * \param[in] file The configuration file to be initialized.
  * \param[in] config The configuration file parameters. 
  */
void config_file_init_config(
  config_file_t* file,
  const config_t* config);

/** \brief Destroy configuration file
  * \param[in] file The configuration file to be destroyed.
  */
void config_file_destroy(
  config_file_t* file);

/** \brief Add configuration file section
  * \note Calling this function may invalidate previously acquired section
  *   pointers.
  * \param[in] file The configuration file to add the section to.
  * \param[in] name An optional name of the configuration file section to
  *   be added.
  * \param[in] title An optional title of the configuration file section.
  * \return The added configuration file section.
  * 
  * The added section will be appended to the file. The configuration file
  * sections will be re-allocated to accommodate the added section.
  */
config_file_section_t* config_file_add_section(
  config_file_t* file,
  const char* name,
  const char* title);

/** \brief Add configuration file section containing configuration parameters
  * \note Calling this function may invalidate previously acquired section
  *   pointers.
  * \param[in] file The configuration file to add the section containing
  *   the configuration parameters to.
  * \param[in] name An optional name of the configuration file section to
  *   be added.
  * \param[in] title An optional title of the configuration file section.
  * \param[in] config The configuration parameters to be contained by
  *   the section.
  * \return The added configuration file section containing the configuration
  *   parameters.
  * 
  * This is a convenience function which adds a dedicated section to the
  * configuration file by calling config_file_add_section().
  */
config_file_section_t* config_file_add_config(
  config_file_t* file,
  const char* name,
  const char* title,
  const config_t* config);

/** \brief Retrieve configuration file section
  * \param[in] file The configuration file to retrieve the section for.
  * \param[in] name The name of the configuration file section to
  *   be retrieved.
  * \return The configuration file section with the given name or null
  *   if no such section exists in the configuration file.
  */
config_file_section_t* config_file_get_section(
  const config_file_t* file,
  const char* name);

/** \brief Add configuration variable
  * \note Calling this function may invalidate previously acquired variable
  *   pointers.
  * \param[in] section The configuration file section to add the variable to.
  * \param[in] name The name of the configuration file variable to
  *   be added.
  * \param[in] value The value of the configuration file variable to
  *   be added.
  * \param[in] description An optional description of the configuration
  *   file variable.
  * \return The added configuration file variable.
  * 
  * The added variable will be appended to the section. The configuration
  * file section variables will be re-allocated to accommodate the added
  * variable.
  */
config_file_var_t* config_file_add_var(
  config_file_section_t* section,
  const char* name,
  const char* value,
  const char* description);

/** \brief Add configuration file variable for a configuration parameter
  * \note Calling this function may invalidate previously acquired section
  *   pointers.
  * \param[in] section The configuration file section to add the variable
  *   corresponding to the configuration parameter to.
  * \param[in] param The configuration parameter for which to add the
  *   configuration file variable.
  * \return The added configuration file variable for the configuration
  *   parameter.
  * 
  * This is a convenience function which adds a dedicated variable to the
  * configuration file section by calling config_file_add_var().
  */
config_file_var_t* config_file_add_param(
  config_file_section_t* section,
  const config_param_t* param);

/** \brief Retrieve configuration variable
  * \param[in] section The configuration file section to retrieve the
  *   variable from.
  * \param[in] name The name of the configuration file variable to
  *   be retrieved.
  * \return The configuration file variable with the given name or null
  *   if no such variable exists in the configuration file section.
  */
config_file_var_t* config_file_get_var(
  const config_file_section_t* section,
  const char* name);

/** \brief Read configuration file
  * \param[in] filename The name of the configuration file to read.
  *   The special filename '-' indicates that the configuration file
  *   content shall be read from stdin.
  * \param[in,out] file The initialized configuration file to be read.
  * \return The resulting error code.
  */
int config_file_read(
  const char* filename,
  config_file_t* file);

/** \brief Write configuration file
  * \param[in] filename The name of the configuration file to write.
  *   The special filename '-' indicates that the configuration file
  *   content shall be written to stdout.
  * \param[in] file The configuration file to be written.
  * \return The resulting error code.
  */
int config_file_write(
  const char* filename,
  config_file_t* file);

#endif
