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

#include <file/file.h>

#include "config/config.h"

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
#define CONFIG_MAN_PARAMETER_PROJECT_AUTHORS        "project-authors"
#define CONFIG_MAN_PARAMETER_PROJECT_CONTACT        "project-contact"
#define CONFIG_MAN_PARAMETER_PROJECT_HOME           "project-home"
#define CONFIG_MAN_PARAMETER_PROJECT_LICENSE        "project-license"
//@}

/** \name Error Codes
  * \brief Predefined manual page error codes
  */
//@{
#define CONFIG_MAN_ERROR_NONE                       0
#define CONFIG_MAN_ERROR_WRITE                      1
//@}

/** \brief Predefined configuration help error descriptions
  */
extern const char* config_help_errors[];

/** \brief Predefined manual page default options
  */
extern config_t config_man_default_options;

/** \brief Write manual page header
  * \param[in] file The open file that will be used for writing the
  *   generated manual page header.
  * \param[in] page_name The name of the manual page to appear in the
  *   generated header.
  * \param[in] page_section The section number of the manual page to appear
  *   in the generated header.
  * \param[in] page_title The manual page title to appear in the generated
  *   header.
  * \param[in] command The command to generate the manual page header for.
  * \param[in] summary The command summary to appear in the manual page
  *   header.
  * \return The resulting error code.
  */
int config_man_write_header(
  file_p file,
  const char* page_name,
  size_t page_section,
  const char* page_title,
  const char* command,
  const char* summary);

/** \brief Write manual page synopsis
  * \param[in] file The open file that will be used for writing the
  *   generated manual page synopsis.
  * \param[in] usage The usage line to appear in the manual page synopsis.
  * \return The resulting error code.
  */
int config_man_write_synopsis(
  file_p file,
  const char* usage);

/** \brief Write manual page description
  * \param[in] file The open file that will be used for writing the
  *   generated manual page description.
  * \param[in] text The text to appear in the manual page description.
  * \return The resulting error code.
  */
int config_man_write_description(
  file_p file,
  const char* text);

/** \brief Write manual page section for a group of positional arguments
  * \param[in] file The open file that will be used for writing the
  *   generated manual page section.
  * \param[in] section_title The title of the manual page section.
  * \param[in] section_preface An optional preface of the manual page section.
  * \param[in] arguments The arguments for which to generate the manual
  *   page section.
  * \return The resulting error code.
  */
int config_man_write_arguments(
  file_p file,
  const char* section_title,
  const char* section_preface,
  config_p arguments);

/** \brief Write manual page paragraph for a positional argument parameter
  * \param[in] file The open file that will be used for writing the
  *   generated manual page paragraph.
  * \param[in] param The argument parameter for which to generate the manual
  *   page paragraph.
  * \return The resulting error code.
  */
int config_man_write_argument(
  file_p file,
  config_param_p param);

/** \brief Write manual page section for a group of non-positional options
  * \param[in] file The open file that will be used for writing the
  *   generated manual page section.
  * \param[in] section_title The title of the manual page section.
  * \param[in] section_preface An optional preface of the manual page section.
  * \param[in] options The options for which to generate the manual page
  *   section.
  * \param[in] prefix An optional argument prefix that will be appended to
  *   the parameters' keys.
  * \return The resulting error code.
  */
int config_man_write_options(
  file_p file,
  const char* section_title,
  const char* section_preface,
  config_p options,
  const char* prefix);

/** \brief Write manual page paragraph for a non-positional option parameter
  * \param[in] file The open file that will be used for writing the
  *   generated manual page paragraph.
  * \param[in] param The option parameter for which to generate the manual
  *   page paragraph.
  * \param[in] prefix An optional argument prefix that will be appended to
  *   the parameter's key.
  * \return The resulting error code.
  */
int config_man_write_option(
  file_p file,
  config_param_p param,
  const char* prefix);

/** \brief Write manual page section crediting the authors
  * \param[in] file The open file that will be used for writing the
  *   generated manual page section.
  * \param[in] project_authors The project authors to appear in the manual
  *   page section.
  * \return The resulting error code.
  */
int config_man_write_authors(
  file_p file,
  const char* project_authors);

/** \brief Write manual page section for bug reporting
  * \param[in] file The open file that will be used for writing the
  *   generated manual page section.
  * \param[in] project_contact The project contact of the maintainer
  *   responsible for taking bug reports.
  * \return The resulting error code.
  */
int config_man_write_bugs(
  file_p file,
  const char* project_contact);

/** \brief Write manual page copyright section
  * \param[in] file The open file that will be used for writing the
  *   generated manual page section.
  * \param[in] project_name The name of the project for which to generate
  *   the copyright section.
  * \param[in] project_license The license under which the project is
  *   published.
  * \return The resulting error code.
  */
int config_man_write_copyright(
  file_p file,
  const char* project_name,
  const char* project_license);

/** \brief Write manual page colophon
  * \param[in] file The open file that will be used for writing the
  *   generated manual page colophon.
  * \param[in] project_name The name of the project for which to generate
  *   the manual page colophon.
  * \param[in] project_version The optional version of the project for
  *   which to generate the manual page colophon.
  * \param[in] project_home The optional project homepage to appear in the
  *   manual page colophon.
  * \return The resulting error code.
  */
int config_man_write_colophon(
  file_p file,
  const char* project_name,
  const char* project_version,
  const char* project_home);

#endif
