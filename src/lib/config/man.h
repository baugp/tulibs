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
#include "config/project.h"

#include "error/error.h"

/** \file config/man.h
  * \ingroup config
  * \brief Configuration manual page output
  * \author Ralf Kaestner
  * 
  * The functions provided in this file facilitate automated generation
  * of Linux manual page content from configuration parameter properties.
  * In most applications, they should be used indirectly through the
  * configuration parser interface.
  */

/** \brief Predefined manual page parser option group
  */
#define CONFIG_MAN_PARSER_OPTION_GROUP              "man"

/** \name Parameters
  * \brief Predefined manual page parameters
  */
//@{
#define CONFIG_MAN_PARAMETER_SECTION                "section"
#define CONFIG_MAN_PARAMETER_TITLE                  "title"
//@}

/** \name Sections
  * \brief Predefined manual page sections
  */
//@{
#define CONFIG_MAN_SECTION_SUMMARY                  "NAME"
#define CONFIG_MAN_SECTION_SYNOPSIS                 "SYNOPSIS"
#define CONFIG_MAN_SECTION_DESCRIPTION              "DESCRIPTION"
#define CONFIG_MAN_SECTION_AUTHORS                  "AUTHORS"
#define CONFIG_MAN_SECTION_BUGS                     "REPORTING BUGS"
#define CONFIG_MAN_SECTION_COPYRIGHT                "COPYRIGHT"
#define CONFIG_MAN_SECTION_COLOPHON                 "COLOPHON"
//@}

/** \name Error Codes
  * \brief Predefined manual page error codes
  */
//@{
#define CONFIG_MAN_ERROR_NONE                       0
//!< Success
#define CONFIG_MAN_ERROR_WRITE                      1
//!< Failed to write manual page to file
//@}

/** \brief Predefined manual page error descriptions
  */
extern const char* config_man_errors[];

/** \brief Predefined manual page default options
  */
extern const config_t config_man_default_options;

/** \brief Manual page header structure
  */
typedef struct config_man_page_header_t {
  char *name;                //!< The name of the manual page.
  size_t section;            //!< The section of the manual page.
  
  char* title;               //!< The title of the manual page.
} config_man_page_header_t;

/** \brief Manual page section structure
  */
typedef struct config_man_page_section_t {
  char* title;               //!< The title of the manual page section.

  char** paragraphs;         //!< The section paragraphs.
  size_t num_paragraphs;     //!< The number of section paragraphs.
} config_man_page_section_t;

/** \brief Manual page structure
  */
typedef struct config_man_page_t {
  config_man_page_header_t
    header;                  //!< The manual page header. 
  
  config_man_page_section_t*
    sections;                //!< The manual page sections.
  size_t num_sections;       //!< The number of manual page sections.

  error_t error;             //!< The most recent manual page error.  
} config_man_page_t;

/** \brief Initialize manual page
  * \param[in] page The manual page to be initialized.
  * \param[in] name The name of the manual page to be initialized.
  * \param[in] section The section of the manual page to be initialized.
  * \param[in] title The title of the manual page to be initialized.
  */
void config_man_init(
  config_man_page_t* page,
  const char* name,
  size_t section,
  const char* title);

/** \brief Initialize manual page from configuration parameters
  * \param[in] page The manual page to be initialized.
  * \param[in] name The name of the manual page to be initialized.
  * \param[in] config The manual page configuration parameters.
  */
void config_man_init_config(
  config_man_page_t* page,
  const char* name,
  const config_t* config);

/** \brief Destroy manual page
  * \param[in] page The manual page to be destroyed.
  */
void config_man_destroy(
  config_man_page_t* page);

/** \brief Add manual page section
  * \note Calling this function may invalidate previously acquired section
  *   pointers.
  * \param[in] page The manual page to add the section to.
  * \param[in] title The title of the manual page section to be added.
  *   Following the standards for Linux manual pages, this title will
  *   be capitalized.
  * \return The added manual page section.
  * 
  * The added section will be appended to the manual page. The manual
  * page sections will be re-allocated to accommodate the added section.
  */
config_man_page_section_t* config_man_add_section(
  config_man_page_t* page,
  const char* title);

/** \brief Add manual page summary
  * \note Calling this function may invalidate previously acquired section
  *   pointers.
  * \param[in] page The manual page to add the summary to.
  * \param[in] summary The summary text to be added to the manual page.
  * \return The added manual page section containing the summary.
  * 
  * This is a convenience function which adds a dedicated section to the
  * manual page by calling config_man_add_section().
  */
config_man_page_section_t* config_man_add_summary(
  config_man_page_t* page,
  const char* summary);

/** \brief Add manual page command summary
  * \note Calling this function may invalidate previously acquired section
  *   pointers.
  * \param[in] page The manual page to add the command summary to.
  * \param[in] command The command to appear in the manual page summary.
  * \param[in] summary A short summary of the command to appear in the
  *   manual page summary.
  * \return The added manual page section containing the command summary.
  * 
  * This is a convenience function which adds a dedicated section to the
  * manual page by calling config_man_add_section().
  */
config_man_page_section_t* config_man_add_command_summary(
  config_man_page_t* page,
  const char* command,
  const char* summary);

/** \brief Add manual page synopsis
  * \note Calling this function may invalidate previously acquired section
  *   pointers.
  * \param[in] page The manual page to add the synopsis to.
  * \param[in] synopsis The synopsis text to be added to the manual page.
  * \return The added manual page section containing the synopsis.
  * 
  * This is a convenience function which adds a dedicated section to the
  * manual page by calling config_man_add_section().
  */
config_man_page_section_t* config_man_add_synopsis(
  config_man_page_t* page,
  const char* synopsis);

/** \brief Add manual page description
  * \note Calling this function may invalidate previously acquired section
  *   pointers.
  * \param[in] page The manual page to add the description to.
  * \param[in] description The description text to be added to the manual page.
  * \return The added manual page section containing the description.
  * 
  * This is a convenience function which adds a dedicated section to the
  * manual page by calling config_man_add_section().
  */
config_man_page_section_t* config_man_add_description(
  config_man_page_t* page,
  const char* description);

/** \brief Add manual page section describing configuration parameters
  * \note Calling this function may invalidate previously acquired section
  *   pointers.
  * \param[in] page The manual page to add the section describing
  *   configuration parameters to.
  * \param[in] title The title of the manual page section.
  * \param[in] preface An optional preface of the manual page section.
  * \param[in] config The configuration parameters described by the manual
  *   page section.
  * \param[in] format The format for printing each parameter's key and
  *   value. See config_man_add_param() for details.
  * \return The added manual page section describing the configuration
  *   parameters.
  * 
  * This is a convenience function which adds a dedicated section to the
  * manual page by calling config_man_add_section().
  */
config_man_page_section_t* config_man_add_config(
  config_man_page_t* page,
  const char* title,
  const char* preface,
  const config_t* config,
  const char* format);

/** \brief Add manual page paragraph describing a configuration parameter
  * \note Calling this function may invalidate previously acquired section
  *   pointers.
  * \param[in] section The manual page section to add the paragraph
  *   describing the configuration parameter to.
  * \param[in] param The configuration parameter described by the manual
  *   page paragraph.
  * \param[in] format The formatting of the parameter's key and value in
  *   the paragraph's heading. The format string should accommodate two
  *   string values, the first being the key and the second being the value.
  * \return The added manual page paragraph describing the configuration
  *   parameter.
  * 
  * This is a convenience function which adds a dedicated paragraph to the
  * manual page section by calling config_man_printf().
  */
const char* config_man_add_param(
  config_man_page_section_t* section,
  const config_param_t* param,
  const char* format);

/** \brief Add manual page section describing positional arguments
  * \note Calling this function may invalidate previously acquired section
  *   pointers.
  * \param[in] page The manual page to add the section describing positional
  *   arguments to.
  * \param[in] title The title of the manual page section.
  * \param[in] preface An optional preface of the manual page section.
  * \param[in] arguments The positional arguments described by the manual
  *   page section.
  * \return The added manual page section describing the positional
  *   arguments.
  * 
  * This is a convenience function which adds a dedicated section to the
  * manual page by calling config_man_add_config().
  */
config_man_page_section_t* config_man_add_arguments(
  config_man_page_t* page,
  const char* title,
  const char* preface,
  const config_t* arguments);

/** \brief Add manual page section describing non-positional arguments
  * \note Calling this function may invalidate previously acquired section
  *   pointers.
  * \param[in] page The manual page to add the section describing
  *   non-positional arguments to.
  * \param[in] title The title of the manual page section.
  * \param[in] preface An optional preface of the manual page section.
  * \param[in] options The non-positional arguments described by the manual
  *   page section.
  * \param[in] prefix An optional key prefix of the non-positional
  *   arguments.
  * \return The added manual page section describing the non-positional
  *   arguments.
  * 
  * This is a convenience function which adds a dedicated section to the
  * manual page by calling config_man_add_config().
  */
config_man_page_section_t* config_man_add_options(
  config_man_page_t* page,
  const char* title,
  const char* preface,
  const config_t* options,
  const char* prefix);

/** \brief Add manual page section crediting the authors
  * \note Calling this function may invalidate previously acquired section
  *   pointers.
  * \param[in] page The manual page to add the authors section to.
  * \param[in] authors The authors to appear in the manual page section.
  * \return The added manual page section crediting the authors.
  * 
  * This is a convenience function which adds a dedicated section to the
  * manual page by calling config_man_add_section().
  */
config_man_page_section_t* config_man_add_authors(
  config_man_page_t* page,
  const char* authors);

/** \brief Add manual page section about bug reporting
  * \note Calling this function may invalidate previously acquired section
  *   pointers.
  * \param[in] page The manual page to add the bug reporting section to.
  * \param[in] contact The contact of the maintainer responsible for
  *   taking bug reports.
  * \return The added manual page section about bug reporting.
  * 
  * This is a convenience function which adds a dedicated section to the
  * manual page by calling config_man_add_section().
  */
config_man_page_section_t* config_man_add_bugs(
  config_man_page_t* page,
  const char* contact);

/** \brief Add manual page section about copyright
  * \note Calling this function may invalidate previously acquired section
  *   pointers.
  * \param[in] page The manual page to add the copyright section to.
  * \param[in] project The name of the project to appear in the copyright
  *   section.
  * \param[in] license The license to appear in the copyright section.
  * \return The added manual page section about copyright.
  * 
  * This is a convenience function which adds a dedicated section to the
  * manual page by calling config_man_add_section().
  */
config_man_page_section_t* config_man_add_copyright(
  config_man_page_t* page,
  const char* project,
  const char* license);

/** \brief Add manual page colophon
  * \note Calling this function may invalidate previously acquired section
  *   pointers.
  * \param[in] page The manual page to add the colophon to.
  * \param[in] project The name of the project to appear in the manual
  *   page colophon.
  * \param[in] version The optional version to appear in the manual
  *   page colophon.
  * \param[in] home The optional homepage to appear in the manual
  *   page colophon.
  * \return The added manual page section about copyright.
  * 
  * This is a convenience function which adds a dedicated section to the
  * manual page by calling config_man_add_section().
  */
config_man_page_section_t* config_man_add_colophon(
  config_man_page_t* page,
  const char* project,
  const char* version,
  const char* home);

/** \brief Add manual page sections about the project
  * \note Calling this function may invalidate previously acquired section
  *   pointers.
  * \param[in] page The manual page to add the project sections to.
  * \param[in] project The project to appear in the manual page sections.
  * 
  * This is a convenience function which adds dedicated sections to the
  * manual page by calling config_man_add_authors(), config_man_add_bugs(),
  * config_man_add_copyright(), and config_man_add_colophon().
  */
void config_man_add_project_sections(
  config_man_page_t* page,
  const config_project_t* project);

/** \brief Print formatted text to manual page
  * \param[in] section The manual page section to add the paragraph to.
  * \param[in] format A string defining the expected format and conversion
  *   specififiers of the text to be written. This string must be followed by
  *   a variadic list of arguments, where each argument is of appropriate
  *   type.
  * \return The added manual page paragraph.
  * 
  * A paragraph will be appended to the specified manual page section.
  */
const char* config_man_printf(
  config_man_page_section_t* section,
  const char* format,
  ...);

/** \brief Write manual page to file
  * \param[in] filename The name of the file to write the manual page to.
  *   The special filename '-' indicates that the manual page content shall
  *   be written to stdout.
  * \param[in] page The manual page to be written.
  * \return The resulting error code.
  */
int config_man_write(
  const char* filename,
  config_man_page_t* page);

#endif
