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

#ifndef CONFIG_MAN_H
#define CONFIG_MAN_H

#include <stdlib.h>

#include "config.h"

/** \file man.h
  * \brief Configuration manual page output
  * \author Ralf Kaestner
  * POSIX-compliant manual page generator functions.
  */

/** \brief Predefined manual page argument prefix
  */
#define CONFIG_MAN_ARG_PREFIX                       "man-"

/** \name Parameters
  * \brief Predefined manual page parameters
  */
//@{
#define CONFIG_MAN_PARAMETER_OUTPUT                 "output"
#define CONFIG_MAN_PARAMETER_SECTION                "section"
#define CONFIG_MAN_PARAMETER_TITLE                  "title"
#define CONFIG_MAN_PARAMETER_PROJECT_NAME           "project-name"
#define CONFIG_MAN_PARAMETER_PROJECT_VERSION        "project-version"
#define CONFIG_MAN_PARAMETER_PROJECT_AUTHOR         "project-author"
#define CONFIG_MAN_PARAMETER_PROJECT_CONTACT        "project-contact"
#define CONFIG_MAN_PARAMETER_PROJECT_HOME           "project-home"
#define CONFIG_MAN_PARAMETER_PROJECT_LICENSE        "project-license"
//@}

/** \brief Predefined manual page default options
  */
extern config_t config_man_default_options;

/** \brief Print manual page header
  * \param[in] stream The output stream that will be used for printing the
  *   generated manual page header.
  * \param[in] page_name The name of the manual page to appear in the
  *   generated header.
  * \param[in] page_section The section number of the manual page to appear
  *   in the generated header.
  * \param[in] page_title The manual page title to appear in the generated
  *   header.
  * \param[in] command The command to print the manual page header for.
  * \param[in] summary The command summary to appear in the manual page
  *   header.
  */
void config_man_print_header(
  FILE* stream,
  const char* page_name,
  size_t page_section,
  const char* page_title,
  const char* command,
  const char* summary);

/** \brief Print manual page synopsis
  * \param[in] stream The output stream that will be used for printing the
  *   generated manual page synopsis.
  * \param[in] usage The usage line to appear in the manual page synopsis.
  */
void config_man_print_synopsis(
  FILE* stream,
  const char* usage);

/** \brief Print manual page description
  * \param[in] stream The output stream that will be used for printing the
  *   generated manual page description.
  * \param[in] text The text to appear in the manual page description.
  */
void config_man_print_description(
  FILE* stream,
  const char* text);

/** \brief Print manual page section for a group of positional arguments
  * \param[in] stream The output stream that will be used for printing the
  *   generated manual page section.
  * \param[in] section_title The title of the manual page section.
  * \param[in] arguments The arguments for which to generate the manual
  *   page section.
  */
void config_man_print_arguments(
  FILE* stream,
  const char* section_title,
  config_p arguments);

/** \brief Print manual page paragraph for a positional argument parameter
  * \param[in] stream The output stream that will be used for printing the
  *   generated manual page paragraph.
  * \param[in] param The argument parameter for which to generate the manual
  *   page paragraph.
  */
void config_man_print_argument(
  FILE* stream,
  config_param_p param);

/** \brief Print manual page section for a group of non-positional options
  * \param[in] stream The output stream that will be used for printing the
  *   generated manual page section.
  * \param[in] section_title The title of the manual page section.
  * \param[in] options The options for which to generate the manual page
  *   section.
  * \param[in] prefix An optional argument prefix that will be appended to
  *   the parameters' keys.
  */
void config_man_print_options(
  FILE* stream,
  const char* section_title,
  config_p options,
  const char* prefix);

/** \brief Print manual page paragraph for a non-positional option parameter
  * \param[in] stream The output stream that will be used for printing the
  *   generated manual page paragraph.
  * \param[in] param The option parameter for which to generate the manual
  *   page paragraph.
  * \param[in] prefix An optional argument prefix that will be appended to
  *   the parameter's key.
  */
void config_man_print_option(
  FILE* stream,
  config_param_p param,
  const char* prefix);

/** \brief Print manual page section crediting the author
  * \param[in] stream The output stream that will be used for printing the
  *   generated manual page section.
  * \param[in] author The author to appear in the manual page section.
  */
void config_man_print_author(
  FILE* stream,
  const char* author);

/** \brief Print manual page section for bug reporting
  * \param[in] stream The output stream that will be used for printing the
  *   generated manual page section.
  * \param[in] contact The contact of the maintainer responsible for
  *   taking bug reports.
  */
void config_man_print_bugs(
  FILE* stream,
  const char* contact);

/** \brief Print manual page copyright section
  * \param[in] stream The output stream that will be used for printing the
  *   generated manual page section.
  * \param[in] project_name The name of the project for which to generate
  *   the copyright section.
  * \param[in] license The license under which the project is published.
  */
void config_man_print_copyright(
  FILE* stream,
  const char* project_name,
  const char* license);

/** \brief Print manual page colophon
  * \param[in] stream The output stream that will be used for printing the
  *   generated manual page colophon.
  * \param[in] project_name The name of the project for which to generate
  *   the manual page colophon.
  * \param[in] project_version The optional version of the project for
  *   which to generate the manual page colophon.
  * \param[in] project_home The optional project homepage to appear in the
  *   manual page colophon.
  */
void config_man_print_colophon(
  FILE* stream,
  const char* project_name,
  const char* project_version,
  const char* project_home);

#endif
