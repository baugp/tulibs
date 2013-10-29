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

#ifndef CONFIG_HELP_H
#define CONFIG_HELP_H

#include <stdlib.h>

#include "config.h"

/** \file help.h
  * \brief Configuration help output
  * \author Ralf Kaestner
  * POSIX-compliant help generator functions.
  */

/** \name Error Codes
  * \brief Predefined configuration help error codes
  */
//@{
#define CONFIG_HELP_ERROR_NONE                0
#define CONFIG_HELP_ERROR_WIDTH               1
//@}

/** \brief Predefined configuration help error descriptions
  */
extern const char* config_help_errors[];

/** \brief Print help section for a group of positional arguments
  * \param[in] stream The output stream that will be used for printing the
  *   generated help section.
  * \param[in] section_title An optional title of the section.
  * \param[in] arguments The arguments for which to generate the help section.
  * \param[in] max_width The absolute, maximum character width of the
  *   help section, including indentation.
  * \param[in] key_indent The indentation width of the parameters' keys.
  * \param[in] par_indent The indentation width of the paragraphs containing
  *   the parameter descriptions.
  * \return The resulting error code.
  */
int config_help_print_arguments(
  FILE* stream,
  const char* section_title,
  config_p arguments,
  size_t max_width,
  size_t key_indent,
  size_t par_indent);

/** \brief Print help paragraph for a positional argument parameter
  * \param[in] stream The output stream that will be used for printing the
  *   generated help paragraph.
  * \param[in] param The argument parameter for which to generate the help
  *   paragraph.
  * \param[in] max_width The absolute, maximum character width of the
  *   help paragraph, including indentation.
  * \param[in] key_indent The indentation width of the parameter's key.
  * \param[in] par_indent The indentation width of the paragraph containing
  *   the parameter's description.
  * \return The resulting error code.
  */
int config_help_print_argument(
  FILE* stream,
  config_param_p param,
  size_t max_width,
  size_t key_indent,
  size_t par_indent);

/** \brief Print help section for a group of non-positional options
  * \param[in] stream The output stream that will be used for printing the
  *   generated help section.
  * \param[in] section_title An optional title of the section.
  * \param[in] options The options for which to generate the help section.
  * \param[in] prefix An optional argument prefix that will be appended to
  *   the parameters' keys.
  * \param[in] max_width The absolute, maximum character width of the
  *   help section, including indentation.
  * \param[in] key_indent The indentation width of the parameters' keys.
  * \param[in] par_indent The indentation width of the paragraphs containing
  *   the parameter descriptions.
  * \return The resulting error code.
  */
int config_help_print_options(
  FILE* stream,
  const char* section_title,
  config_p options,
  const char* prefix,
  size_t max_width,
  size_t key_indent,
  size_t par_indent);

/** \brief Print help paragraph for a non-positional option parameter
  * \param[in] stream The output stream that will be used for printing the
  *   generated help paragraph.
  * \param[in] param The option parameter for which to generate the help
  *   paragraph.
  * \param[in] prefix An optional argument prefix that will be appended to
  *   the parameter's key.
  * \param[in] max_width The absolute, maximum character width of the
  *   help paragraph, including indentation.
  * \param[in] key_indent The indentation width of the parameter's key.
  * \param[in] par_indent The indentation width of the paragraph containing
  *   the parameter's description.
  * \return The resulting error code.
  */
int config_help_print_option(
  FILE* stream,
  config_param_p param,
  const char* prefix,
  size_t max_width,
  size_t key_indent,
  size_t par_indent);

/** \brief Print formatted help text
  * \param[in] stream The output stream that will be used for printing the
  *   help text.
  * \param[in] text The help text to be printed.
  * \param[in] max_width The absolute, maximum character width of the
  *   formatted help output, including indentation.
  * \param[in] par_indent The paragraph indentation width of the help text.
  * \param[in] line_indent The indentation width of the paragraph's first
  *   line.
  * \param[in] line_offset The indentation offset of the paragraph's first
  *   line.
  * \return The resulting error code.
  */
int config_help_print(
  FILE* stream,
  const char* text,
  size_t max_width,
  size_t par_indent,
  size_t line_indent,
  size_t line_offset);

/** \brief Justify help text to not exceed a specified character width
  * \note This is a string formatting helper function to generate multi-line
  *   help text suitable for limited character width stream output.
  * \param[in,out] text The help text that will be justified in place by
  *   substituting whitespace characters.
  * \param[in] max_width The maximum character width of the justified
  *   help text.
  * \return The resulting error code.
  */
int config_help_justify(
  char* text,
  size_t max_width);

#endif