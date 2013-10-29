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
  {CONFIG_MAN_PARAMETER_PROJECT_AUTHOR,
    config_param_type_string,
    "",
    "",
    "The project author to appear in the manual page"},
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

void config_man_print_header(FILE* stream, const char* page_name, size_t
    page_section, const char* page_title, const char* command, const char*
    summary) {
  struct timeval time;
  struct tm local_time;
  
  gettimeofday(&time, 0);
  localtime_r(&time.tv_sec, &local_time);
  
  fprintf(stream,
    ".TH \"%s\" %d \"%d-%02d-%02d\" Linux \"%s\"\n", page_name,
    (int)page_section, 1900+local_time.tm_year, local_time.tm_mon,
    local_time.tm_mday, page_title);
  fprintf(stream,
    ".SH NAME\n"
    "%s - %s\n",command, summary);
}

void config_man_print_synopsis(FILE* stream, const char* usage) {
  fprintf(stream,
    ".SH SYNOPSIS\n"
    "%s\n", usage);
}

void config_man_print_description(FILE* stream, const char* text) {
  fprintf(stream,
    ".SH DESCRIPTION\n"
    "%s\n", text);
}

void config_man_print_arguments(FILE* stream, const char* section_title,
    config_p arguments) {
  int i;    
   
  if (arguments->num_params) {
    char title[strlen(section_title)+1];
    for (i = 0; i < sizeof(title); ++i)
      title[i] = toupper(section_title[i]);
    fprintf(stream, ".SH %s\n", title);
  }
  
  for (i = 0; i < arguments->num_params; ++i)
    config_man_print_argument(stream, &arguments->params[i]);
}

void config_man_print_argument(FILE* stream, config_param_p param) {
  fprintf(stream,
    ".PP\n"
    ".BI \"%s\"\\c\n", param->key);
      
  if ((param->type == config_param_type_enum) ||
      (param->type == config_param_type_bool))
    fprintf(stream, ": <%s>\n", param->range);
  else
    fprintf(stream, ".RI \": <\" %s \">\"\n",
      config_param_types[param->type]);

  fprintf(stream,
    ".RS\n"
    "%s\n", param->description[0] ? param->description :
      "This argument requires documentation.");
  if ((param->type != config_param_type_enum) &&
      (param->type != config_param_type_bool) &&
      param->range[0])
    fprintf(stream,
      ".PP\n"
      "The permissible range of this argument is \"%s\"%s%s%s.\n",
      param->range,
      param->value[0] ? ", and its value defaults to \"" : "",
      param->value[0] ? param->value : "",
      param->value[0] ? "\"" : "");
  else if (param->value[0])
    fprintf(stream,
      ".PP\n"
      "The default value of this argument is \"%s\".\n", param->value);
  fprintf(stream, ".RE\n");
}

void config_man_print_options(FILE* stream, const char* section_title,
    config_p options, const char* prefix) {
  int i;    
   
  if (options->num_params) {
    char title[strlen(section_title)+1];
    for (i = 0; i < sizeof(title); ++i)
      title[i] = toupper(section_title[i]);
    fprintf(stream, ".SH %s\n", title);
  }
  
  for (i = 0; i < options->num_params; ++i)
    config_man_print_option(stream, &options->params[i], prefix);
}

void config_man_print_option(FILE* stream, config_param_p param,
    const char* prefix) {
  fprintf(stream,
    ".PP\n"
    ".BI \"--%s%s\"\\c\n", prefix ? prefix : "", param->key);
      
  if (param->type == config_param_type_enum)
    fprintf(stream, "=<%s>\n", param->range);
  else if (param->type != config_param_type_bool)
    fprintf(stream, ".RI \"=<\" %s \">\"\n",
      config_param_types[param->type]);

  fprintf(stream,
    ".RS\n"
    "%s\n", param->description[0] ? param->description :
      "This option requires documentation.");
  if ((param->type != config_param_type_enum) &&
      (param->type != config_param_type_bool) &&
      param->range[0])
    fprintf(stream,
      ".PP\n"
      "The permissible range of this argument is \"%s\"%s%s%s.\n",
      param->range,
      param->value[0] ? ", and its value defaults to \"" : "",
      param->value[0] ? param->value : "",
      param->value[0] ? "\"" : "");
  else if (param->value[0] && (param->type != config_param_type_bool))
    fprintf(stream,
      ".PP\n"
      "The default value of this argument is \"%s\".\n", param->value);
  fprintf(stream, ".RE\n");
}

void config_man_print_author(FILE* stream, const char* author) {
  fprintf(stream,
    ".SH AUTHOR\n"
    "Written by %s.\n", author);
}

void config_man_print_bugs(FILE* stream, const char* contact) {
  fprintf(stream,
    ".SH REPORTING BUGS\n"
    "Report bugs to <%s>.\n", contact);
}

void config_man_print_copyright(FILE* stream, const char* project_name,
    const char* license) {
  fprintf(stream,
    ".SH COPYRIGHT\n"
    "%s is published under the %s.\n", project_name, license);
}

void config_man_print_colophon(FILE* stream, const char* project_name,
    const char* project_version, const char* project_home) {
  fprintf(stream,
    ".SH COLOPHON\n"
    "This page is part");
  if (project_version && project_version[0])
    fprintf(stream, " of version %s", project_version);
  fprintf(stream, " of the %s project.", project_name);
  
  if (project_home && project_home[0])
    fprintf(stream,
      ".PP\n"
      "A description of the project, and information about reporting bugs, "
      "can be found at %s.", project_home);
}
