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

#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <stdlib.h>

#include "config/config.h"

#include "error/error.h"

/** \file config/parser.h
  * \ingroup config
  * \brief Simple configuration parser
  * \author Ralf Kaestner
  * 
  * The parser interface allows for parsing parameter values for entire
  * configurations from command line arguments. It implements the concepts
  * of positional and non-positional arguments (called options), and
  * provides the possibility to group non-positional arguments for
  * re-usability in library projects.
  */

/** \name Parameters
  * \brief Predefined configuration parser parameters
  */
//@{
#define CONFIG_PARSER_PARAMETER_HELP              "help"
#define CONFIG_PARSER_PARAMETER_FILE              "file"
#define CONFIG_PARSER_PARAMETER_FILE_OUTPUT       "file-output"
#define CONFIG_PARSER_PARAMETER_MAN_OUTPUT        "man-output"
#define CONFIG_PARSER_PARAMETER_DEBUG             "debug"
//@}

/** \name Constants
  * \brief Predefined configuration parser constants
  */
//@{
#define CONFIG_PARSER_HELP_WIDTH                  80
#define CONFIG_PARSER_HELP_HANG_INDENT            2
#define CONFIG_PARSER_HELP_KEY_INDENT             2
#define CONFIG_PARSER_HELP_PAR_INDENT             30
//@}

/** \name Error Codes
  * \brief Predefined configuration error codes
  */
//@{
#define CONFIG_PARSER_ERROR_NONE                  0
//!< Success
#define CONFIG_PARSER_ERROR_MAN_WRITE             1
//!< Failed to write manual page
#define CONFIG_PARSER_ERROR_FILE_READ             2
//!< Failed to read configuration file
#define CONFIG_PARSER_ERROR_FILE_WRITE            3
//!< Invalid configuration file format
#define CONFIG_PARSER_ERROR_FILE_SECTION          4
//!< Invalid configuration file section
#define CONFIG_PARSER_ERROR_ARG_UNEXPECTED        5
//!< Unexpected argument
#define CONFIG_PARSER_ERROR_ARG_MISSING           6
//!< Missing argument
#define CONFIG_PARSER_ERROR_ARG_FORMAT            7
//!< Invalid argument format
#define CONFIG_PARSER_ERROR_PARAM_KEY             8
//!< Invalid parameter key
#define CONFIG_PARSER_ERROR_PARAM_VALUE_TYPE      9
//!< Parameter value type mismatch
#define CONFIG_PARSER_ERROR_PARAM_VALUE_RANGE     10
//!< Parameter value out of range
//@}

/** \brief Predefined configuration parser error descriptions
  */
extern const char* config_parser_errors[];

/** \brief Predefined configuration parser default options
  */
extern const config_default_t config_parser_default_options;

/** \brief Predefined configuration parser description
  */
extern const char* config_parser_description;

/** \brief Configuration parser exit strategies
  */
typedef enum {
  config_parser_exit_request,   //!< Parser only terminates on request.
  config_parser_exit_error      //!< Parser terminates in case of error.
} config_parser_exit_t;

/** \brief Configuration parser option group structure
  */
typedef struct config_parser_option_group_t {
  char* name;                   //!< The parser option group name.  
  config_t options;             //!< The parser option group's options.
  
  char* summary;                //!< The parser option group summary.
  char* description;            //!< The parser option group description.
} config_parser_option_group_t;

/** \brief Configuration parser structure
  */
typedef struct config_parser_t {
  config_t arguments;           //!< The positional parser arguments.
  config_t options;             //!< The non-positional parser options.
  
  config_parser_option_group_t*
    option_groups;              //!< The parser option groups.
  size_t num_option_groups;     //!< The number of parser option groups.
  
  char* summary;                //!< The configuration parser summary.
  char* description;            //!< The configuration parser description.
  
  char* command;                //!< The command running the parser.
  char* usage;                  //!< The usage line generated by the parser.
  
  error_t error;                //!< The most recent parser error.
} config_parser_t;

/** \brief Initialize configuration parser
  * \param[in] parser The configuration parser to be initialized.
  * \param[in] summary An optional, short summary describing the command
  *   which is running the parser.
  * \param[in] description An optional, long description of the command
  *   which is running the parser.
  */
void config_parser_init(
  config_parser_t* parser,
  const char* summary,
  const char* description);

/** \brief Initialize configuration parser from defaults
  * \param[in] parser The configuration parser to be initialized.
  * \param[in] default_arguments An optional default configuration used to
  *   initialize the positional parser arguments.
  * \param[in] default_options An optional default configuration used to
  *   initialize the non-positional parser options.
  * \param[in] summary An optional, short summary describing the command
  *   which is running the parser.
  * \param[in] description An optional, long description of the command
  *   which is running the parser.
  */
void config_parser_init_default(
  config_parser_t* parser,
  const config_default_t* default_arguments,
  const config_default_t* default_options,
  const char* summary,
  const char* description);

/** \brief Destroy configuration parser
  * \param[in] parser The configuration parser to be destroyed.
  */
void config_parser_destroy(
  config_parser_t* parser);

/** \brief Add option group to the configuration parser
  * \note Calling this function may invalidate previously acquired option
  *   group pointers.
  * \param[in] parser The configuration parser to which the option group
  *   will be added.
  * \param[in] name The unique name of the parser option group. The name
  *   will for instance be used to generate a prefix for the command line
  *   options associated with the option group.
  * \param[in] default_options The default options to initialize the option
  *   group from, may be null in which case the added option group will
  *   be empty.
  * \param[in] summary An optional, short summary of the parser option group.
  * \param[in] description An optional, long description of the parser option
  *   group.
  * \return The option group added to the configuration parser.
  * 
  * The added option group will be appended to the parser's option groups.
  * The parser's option groups are re-allocated to accommodate the added
  * option group.
  */
config_parser_option_group_t* config_parser_add_option_group(
  config_parser_t* parser,
  const char* name,
  const config_default_t* default_options,
  const char* summary,
  const char* description);

/** \brief Retrieve option group of the configuration parser
  * \param[in] parser The configuration parser for which the option group
  *   shall be retrieved.
  * \param[in] name The name of the parser option group.
  * \return The option group of the configuration parser with the given
  *   name or null if no such option group exists.
  */
config_parser_option_group_t* config_parser_get_option_group(
  const config_parser_t* parser,
  const char* name);

/** \brief Parse command line arguments
  * \param[in,out] parser The configuration parser used to parse the
  *   command line arguments. Its configuration will be updated from the
  *   values supplied by positional and non-positional arguments.
  * \param[in] argc The number of supplied command line arguments.
  * \param[in] argv The list of supplied command line arguments.
  * \param[in] exit The exit policy of the parser in case of an error
  *   or help request. If an error occurred and the parser should exit
  *   on errors, usage information will be printed to stderr before
  *   terminating the calling process with a non-zero value. If help is
  *   requested and the parser is asked to exit on help requests, the help
  *   text will be printed to stdout and the calling process will terminate
  *   with a zero result.
  * \return The resulting error code.
  */
int config_parser_parse(
  config_parser_t* parser,
  int argc,
  char** argv,
  config_parser_exit_t exit);

/** \brief Read parser configuration from file
  * \param[in] filename The name of the configuration file to read.
  *   The special filename '-' indicates that the configuration file
  *   content shall be read from stdin.
  * \param[in,out] parser The configuration parser whose non-positional
  *   arguments will be read from the configuration file.
  * \param[in] file_options The configuration file interpreting options.
  * \return The resulting error code.
  * 
  * This is a convenience function which may be used to read a  parser's
  * non-positional arguments from a configuration file.
  */
int config_parser_read_file(
  const char* filename,
  config_parser_t* parser,
  const config_t* file_options);

/** \brief Write parser configuration to file
  * \param[in] filename The name of the configuration file to write.
  *   The special filename '-' indicates that the configuration file
  *   content shall be written to stdout.
  * \param[in] parser The configuration parser whose non-positional
  *   arguments will be written to the configuration file.
  * \param[in] file_options The configuration file generating options.
  * \return The resulting error code.
  * 
  * This is a convenience function which may be used to write a parser's
  * non-positional arguments to a configuration file.
  */
int config_parser_write_file(
  const char* filename,
  config_parser_t* parser,
  const config_t* file_options);

/** \brief Print usage information for a configuration parser
  * \param[in] stream The output stream that will be used for printing the
  *   usage information.
  * \param[in] parser The configuration parser for which the usage
  *   information will be printed.
  * 
  * This is a convenience function which may be used to generate textual
  * usage information according to a parser's configuration. It is commonly
  * called after a failure to parse the command line.
  */
void config_parser_print_usage(
  FILE* stream,
  const config_parser_t* parser);

/** \brief Print help text for a configuration parser
  * \param[in] stream The output stream that will be used for printing the
  *   help text.
  * \param[in] parser The configuration parser for which the help text
  *   will be printed.
  * 
  * This is a convenience function which may be used to generate textual
  * help according to a parser's configuration.
  */
void config_parser_print_help(
  FILE* stream,
  const config_parser_t* parser);

/** \brief Write manual page for a configuration parser
  * \param[in] filename The name of the manual page file to be written.
  *   The special filename '-' indicates that the manual page content
  *   shall be written to stdout.
  * \param[in] parser The configuration parser for which the manpage
  *   will be written.
  * \param[in] man_options The manual page generating options.
  * \param[in] project_options The project options.
  * \return The resulting error code.
  * 
  * This is a convenience function which may be used to generate Linux
  * manual pages according to a parser's configuration and a project's
  * configuration.
  */
int config_parser_write_man(
  const char* filename,
  config_parser_t* parser,
  const config_t* man_options,
  const config_t* project_options);

#endif
