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

#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <ctype.h>

#include "man.h"

config_param_t config_man_default_params[] = {
  {CONFIG_MAN_PARAMETER_OUTPUT,
    config_param_type_string,
    "",
    "",
    "The name of the manual page output file or '-' for stdout"},
  {CONFIG_MAN_PARAMETER_SECTION,
    config_param_type_int,
    "1",
    "[1, 9]",
    "The section number of the manual page"},
  {CONFIG_MAN_PARAMETER_TITLE,
    config_param_type_string,
    "",
    "",
    "The title of the manual page"},
  {CONFIG_MAN_PARAMETER_PROJECT_NAME,
    config_param_type_string,
    "",
    "",
    "The project name to appear in the manual page"},
  {CONFIG_MAN_PARAMETER_PROJECT_VERSION,
    config_param_type_string,
    "",
    "",
    "The project version to appear in the manual page"},
  {CONFIG_MAN_PARAMETER_PROJECT_AUTHORS,
    config_param_type_string,
    "",
    "",
    "The project authors to appear in the manual page"},
  {CONFIG_MAN_PARAMETER_PROJECT_CONTACT,
    config_param_type_string,
    "",
    "",
    "The project contact to appear in the manual page"},
  {CONFIG_MAN_PARAMETER_PROJECT_HOME,
    config_param_type_string,
    "",
    "",
    "The project homepage to appear in the manual page"},
  {CONFIG_MAN_PARAMETER_PROJECT_LICENSE,
    config_param_type_string,
    "",
    "",
    "The project licensing information to appear in the manual page"},
};

config_t config_man_default_options = {
  config_man_default_params,
  sizeof(config_man_default_params)/sizeof(config_param_t),
};

int config_man_write_header(file_p file, const char* page_name, size_t
    page_section, const char* page_title, const char* command, const char*
    summary) {
  struct timeval time;
  struct tm local_time;
  
  gettimeofday(&time, 0);
  localtime_r(&time.tv_sec, &local_time);
  
  file_printf(file,
    ".TH \"%s\" %d \"%d-%02d-%02d\" Linux \"%s\"\n", page_name,
    (int)page_section, 1900+local_time.tm_year, local_time.tm_mon,
    local_time.tm_mday, page_title);
  file_printf(file,
    ".SH NAME\n"
    "%s - %s\n",command, summary);
  
  return file_error(file) ? CONFIG_MAN_ERROR_WRITE : CONFIG_MAN_ERROR_NONE;
}

int config_man_write_synopsis(file_p file, const char* usage) {
  file_printf(file,
    ".SH SYNOPSIS\n"
    "%s\n", usage);

  return file_error(file) ? CONFIG_MAN_ERROR_WRITE : CONFIG_MAN_ERROR_NONE;
}

int config_man_write_description(file_p file, const char* text) {
  file_printf(file,
    ".SH DESCRIPTION\n"
    "%s\n", text);
  
  return file_error(file) ? CONFIG_MAN_ERROR_WRITE : CONFIG_MAN_ERROR_NONE;
}

int config_man_write_arguments(file_p file, const char* section_title,
    const char* section_preface, config_p arguments) {
  int i;    
   
  if (arguments->num_params) {
    char title[strlen(section_title)+1];
    for (i = 0; i < sizeof(title); ++i)
      title[i] = toupper(section_title[i]);
    file_printf(file, ".SH %s\n", title);
    
    if (section_preface && section_preface[0])
      file_printf(file, ".PP\n%s\n", section_preface);
  }
  
  for (i = 0; i < arguments->num_params; ++i)
    config_man_write_argument(file, &arguments->params[i]);

  return file_error(file) ? CONFIG_MAN_ERROR_WRITE : CONFIG_MAN_ERROR_NONE;
}

int config_man_write_argument(file_p file, config_param_p param) {
  file_printf(file,
    ".PP\n"
    ".BI \"%s\"\\c\n", param->key);
      
  if ((param->type == config_param_type_enum) ||
      (param->type == config_param_type_bool))
    file_printf(file, ": <%s>\n", param->range);
  else
    file_printf(file, ".RI \": <\" %s \">\"\n",
      config_param_types[param->type]);

  file_printf(file,
    ".RS\n"
    "%s\n", param->description[0] ? param->description :
      "This argument requires documentation.");
  if ((param->type != config_param_type_enum) &&
      (param->type != config_param_type_bool) &&
      param->range[0])
    file_printf(file,
      ".PP\n"
      "The permissible range of this argument is '%s'%s%s%s.\n",
      param->range,
      param->value[0] ? ", and its value defaults to '" : "",
      param->value[0] ? param->value : "",
      param->value[0] ? "'" : "");
  else if (param->value[0])
    file_printf(file,
      ".PP\n"
      "The default value of this argument is '%s'.\n", param->value);
  file_printf(file, ".RE\n");

  return file_error(file) ? CONFIG_MAN_ERROR_WRITE : CONFIG_MAN_ERROR_NONE;
}

int config_man_write_options(file_p file, const char* section_title,
    const char* section_preface, config_p options, const char* prefix) {
  int i;    
   
  if (options->num_params) {
    char title[strlen(section_title)+1];
    for (i = 0; i < sizeof(title); ++i)
      title[i] = toupper(section_title[i]);
    file_printf(file, ".SH %s\n", title);

    if (section_preface && section_preface[0])
      file_printf(file, ".PP\n%s\n", section_preface);    
  }
  
  for (i = 0; i < options->num_params; ++i)
    config_man_write_option(file, &options->params[i], prefix);

  return file_error(file) ? CONFIG_MAN_ERROR_WRITE : CONFIG_MAN_ERROR_NONE;  
}

int config_man_write_option(file_p file, config_param_p param,
    const char* prefix) {
  file_printf(file,
    ".PP\n"
    ".BI \"--%s%s\"\\c\n", prefix ? prefix : "", param->key);
      
  if (param->type == config_param_type_enum)
    file_printf(file, "=<%s>\n", param->range);
  else if (param->type != config_param_type_bool)
    file_printf(file, ".RI \"=<\" %s \">\"\n",
      config_param_types[param->type]);

  file_printf(file,
    ".RS\n"
    "%s\n", param->description[0] ? param->description :
      "This option requires documentation.");
  if ((param->type != config_param_type_enum) &&
      (param->type != config_param_type_bool) &&
      param->range[0])
    file_printf(file,
      ".PP\n"
      "The permissible range of this argument is '%s'%s%s%s.\n",
      param->range,
      param->value[0] ? ", and its value defaults to '" : "",
      param->value[0] ? param->value : "",
      param->value[0] ? "'" : "");
  else if (param->value[0] && (param->type != config_param_type_bool))
    file_printf(file,
      ".PP\n"
      "The default value of this argument is '%s'.\n", param->value);
  file_printf(file, ".RE\n");

  return file_error(file) ? CONFIG_MAN_ERROR_WRITE : CONFIG_MAN_ERROR_NONE;
}

int config_man_write_authors(file_p file, const char* project_authors) {
  file_printf(file,
    ".SH AUTHORS\n"
    "Written by %s.\n", project_authors);

  return file_error(file) ? CONFIG_MAN_ERROR_WRITE : CONFIG_MAN_ERROR_NONE;
}

int config_man_write_bugs(file_p file, const char* project_contact) {
  file_printf(file,
    ".SH REPORTING BUGS\n"
    "Report bugs to <%s>.\n", project_contact);

  return file_error(file) ? CONFIG_MAN_ERROR_WRITE : CONFIG_MAN_ERROR_NONE;
}

int config_man_write_copyright(file_p file, const char* project_name,
    const char* project_license) {
  file_printf(file,
    ".SH COPYRIGHT\n"
    "%s is published under the %s.\n", project_name, project_license);

  return file_error(file) ? CONFIG_MAN_ERROR_WRITE : CONFIG_MAN_ERROR_NONE;
}

int config_man_write_colophon(file_p file, const char* project_name,
    const char* project_version, const char* project_home) {
  file_printf(file,
    ".SH COLOPHON\n"
    "This page is part");
  if (project_version && project_version[0])
    file_printf(file, " of version %s", project_version);
  file_printf(file, " of the %s project.\n", project_name);
  
  if (project_home && project_home[0])
    file_printf(file,
      ".PP\n"
      "A description of the project, and information about reporting bugs, "
      "can be found at %s.", project_home);

  return file_error(file) ? CONFIG_MAN_ERROR_WRITE : CONFIG_MAN_ERROR_NONE;
}
