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

#define _GNU_SOURCE

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <time.h>
#include <ctype.h>

#include "man.h"

const char* config_man_errors[] = {
  "Success",
  "Failed to write manual page to file",
};

config_param_t config_man_default_params[] = {
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
};

config_t config_man_default_options = {
  config_man_default_params,
  sizeof(config_man_default_params)/sizeof(config_param_t),
};

void config_man_init(config_man_page_p page, const char* name, size_t section,
    const char* title) {
  strcpy(page->header.name, name);
  page->header.section = section;
  strcpy(page->header.title, title);
  
  page->sections = 0;
  page->num_sections = 0;
}

void config_man_init_config(config_man_page_p page, const char* name,
    config_p config) {
  config_man_init(page, name, 
    config_get_int(config, CONFIG_MAN_PARAMETER_SECTION),
    config_get_string(config, CONFIG_MAN_PARAMETER_TITLE));
}

void config_man_destroy(config_man_page_p page) {
  int i, j;
  
  for (i = 0; i < page->num_sections; ++i) {
    config_man_page_section_p section = &page->sections[i];
    for (j = 0; j < section->num_paragraphs; ++j)
      free(section->paragraphs[j]);
    free(section->paragraphs);
  }
  
  if (page->num_sections)
    free(page->sections);
  page->sections = 0;
  page->num_sections = 0;
}

config_man_page_section_p config_man_add_section(config_man_page_p page,
    const char* title) {
  page->sections = realloc(page->sections, (page->num_sections+1)*
    sizeof(config_man_page_section_t));
  config_man_page_section_p section = &page->sections[page->num_sections];
  ++page->num_sections;
  
  size_t title_length = strlen(title);
  int i;
  for (i = 0; i < title_length; ++i)
    section->title[i] = toupper(title[i]);
  section->title[title_length] = 0;
  
  section->paragraphs = 0;
  section->num_paragraphs = 0;
  
  return section;
}

config_man_page_section_p config_man_add_summary(config_man_page_p page,
    const char* summary) {
  config_man_page_section_p section = config_man_add_section(page,
    CONFIG_MAN_SECTION_SUMMARY);
  config_man_printf(section, summary);
  
  return section;
}

config_man_page_section_p config_man_add_command_summary(config_man_page_p
    page, const char* command, const char* summary) {
  config_man_page_section_p section = config_man_add_section(page,
    CONFIG_MAN_SECTION_SUMMARY);
  config_man_printf(section, "%s - %s", command, summary);
  
  return section;
}

config_man_page_section_p config_man_add_synopsis(config_man_page_p page,
    const char* synopsis) {
  config_man_page_section_p section = config_man_add_section(page,
    CONFIG_MAN_SECTION_SYNOPSIS);
  config_man_printf(section, synopsis);
  
  return section;
}

config_man_page_section_p config_man_add_description(config_man_page_p page,
    const char* description) {
  config_man_page_section_p section = config_man_add_section(page,
    CONFIG_MAN_SECTION_DESCRIPTION);
  config_man_printf(section, description);
  
  return section;
}

config_man_page_section_p config_man_add_config(config_man_page_p page,
    const char* title, const char* preface, config_p config, const char*
    format) {
  config_man_page_section_p  section = config_man_add_section(page, title);
  int i;

  if (preface && preface[0])
    config_man_printf(section, preface);
  if (config) {
    for (i = 0; i < config->num_params; ++i)
      config_man_add_param(section, &config->params[i], format);
  }
  
  return section;
}

const char* config_man_add_param(config_man_page_section_p section,
    config_param_p param, const char* format) {
  char par_format[strlen(format)+64];
  sprintf(par_format, "%s\n.RS\n%%s\n.P\n%%s.RE", format);
  
  char param_range[strlen(param->range)+strlen(param->value)+128];
  if ((param->type != config_param_type_enum) &&
      (param->type != config_param_type_bool) &&
      param->range[0])
    sprintf(param_range,
      "The permissible range of this argument is '%s'%s%s%s.\n",
      param->range,
      param->value[0] ? ", and its value defaults to '" : "",
      param->value[0] ? param->value : "",
      param->value[0] ? "'" : "");
  else if ((param->type != config_param_type_bool) &&
      param->value[0])
    sprintf(param_range, "The default value of this argument is '%s'.\n",
      param->value);
  else
    param_range[0] = 0;
  
  return config_man_printf(section, par_format, param->key,
    (param->type == config_param_type_enum) ||
      (param->type == config_param_type_bool) ? param->range :
      config_param_types[param->type],
    param->description[0] ? param->description :
      "This argument requires documentation.", param_range);
}

config_man_page_section_p config_man_add_arguments(config_man_page_p page,
    const char* title, const char* preface, config_p arguments) {
  config_man_page_section_p section = config_man_add_config(page, title,
    preface, 0, 0);
  
  char format[64];
  int i;

  for (i = 0; i < arguments->num_params; ++i) {
    if ((arguments->params[i].type == config_param_type_enum) ||
        (arguments->params[i].type == config_param_type_bool))
      strcpy(format, ".BI \"%s\"\\c\n: <%s>");
    else
      strcpy(format, ".BI \"%s\"\\c\n.RI \": <\" %s \">\"");
      
    config_man_add_param(section, &arguments->params[i], format);
  }
  
  return section;
}

config_man_page_section_p config_man_add_options(config_man_page_p page,
    const char* title, const char* preface, config_p options, const char*
    prefix) {
  config_man_page_section_p section = config_man_add_config(page, title,
    preface, 0, 0);
  
  size_t prefix_length = prefix ? strlen(prefix) : 0;
  char format[prefix_length+32];
  int i;

  for (i = 0; i < options->num_params; ++i) {
    if (options->params[i].type == config_param_type_enum)
      sprintf(format, ".BI \"--%s%%s\"\\c\n=<%%s>", prefix ? prefix : "");
    else if (options->params[i].type == config_param_type_bool)
      sprintf(format, ".BI \"--%s%%s\"%%.0s", prefix ? prefix : "");
    else
      sprintf(format, ".BI \"--%s%%s\"\\c\n.RI \"=<\" %%s \">\"",
        prefix ? prefix : "");
    
    config_man_add_param(section, &options->params[i], format);
  }
  
  return section;
}

config_man_page_section_p config_man_add_authors(config_man_page_p page,
    const char* authors) {
  config_man_page_section_p section = config_man_add_section(page,
    CONFIG_MAN_SECTION_AUTHORS);
  config_man_printf(section, "Written by %s.", authors);
  
  return section;
}

config_man_page_section_p config_man_add_bugs(config_man_page_p page,
    const char* contact) {
  config_man_page_section_p section = config_man_add_section(page,
    CONFIG_MAN_SECTION_BUGS);
  config_man_printf(section, "Report bugs to <%s>.", contact);
  
  return section;
}

config_man_page_section_p config_man_add_copyright(config_man_page_p page,
    const char* project, const char* license) {
  config_man_page_section_p section = config_man_add_section(page,
    CONFIG_MAN_SECTION_COPYRIGHT);
  config_man_printf(section, "%s is published under the %s.", project,
    license);
  
  return section;
}

config_man_page_section_p config_man_add_colophon(config_man_page_p page,
    const char* project, const char* version, const char* home) {
  config_man_page_section_p section = config_man_add_section(page,
    CONFIG_MAN_SECTION_COLOPHON);
  config_man_printf(section, "This page is part%s%s of the %s project.",
    (version && version[0]) ? " of version " : "",
    (version && version[0]) ? version : "", project);
  if (home && home[0])
    config_man_printf(section, "A description of the project, and information "
      "about reporting bugs, can be found at %s.", home);
  
  return section;
}

void config_man_add_project_sections(config_man_page_p page, config_project_p
    project) {
  if (project->authors[0])
    config_man_add_authors(page, project->authors);
  if (project->contact[0])
    config_man_add_bugs(page, project->contact);
  if (project->name[0] && project->license[0])
    config_man_add_copyright(page, project->name, project->license);
  if (project->name[0])
    config_man_add_colophon(page, project->name, project->version,
      project->home);
}

const char* config_man_printf(config_man_page_section_p section, const char*
    format, ...) {
  char* paragraph = 0;
  
  va_list vargs;
  va_start(vargs, format);
  if (vasprintf(&paragraph, format, vargs) < 0)
    paragraph = 0;
  va_end(vargs);
  
  if (paragraph) {
    section->paragraphs = realloc(section->paragraphs,
      (section->num_paragraphs+1)*sizeof(char*));
    section->paragraphs[section->num_paragraphs] = paragraph;
    ++section->num_paragraphs;
    
    return section->paragraphs[section->num_paragraphs-1];
  }
  else
    return 0;
}

int config_man_write(const char* filename, config_man_page_p page) {
  file_t file;
  
  file_init_name(&file, filename);
  if (strcmp(filename, "-"))
    file_open(&file, file_mode_write);
  else
    file_open_stream(&file, stdout, file_mode_write);

  if (!file.handle)
    return CONFIG_MAN_ERROR_WRITE;
  
  int result = config_man_write_page(&file, page);
  
  file_close(&file);
  return result;
}

int config_man_write_page(file_p file, config_man_page_p page) {
  int result, i;
  
  if ((result = config_man_write_header(file, &page->header)))
    return result;
  for (i = 0; i < page->num_sections; ++i)
    if ((result = config_man_write_section(file, &page->sections[i])))
      return result;

  return CONFIG_MAN_ERROR_NONE;
}

int config_man_write_header(file_p file, config_man_page_header_p header) {
  struct timeval time;
  struct tm local_time;
  
  gettimeofday(&time, 0);
  localtime_r(&time.tv_sec, &local_time);
  
  if (file_printf(file,
      ".TH \"%s\" %d \"%d-%02d-%02d\" Linux \"%s\"\n", header->name,
      (int)header->section, 1900+local_time.tm_year, local_time.tm_mon,
      local_time.tm_mday, header->title) < 0)
    return CONFIG_MAN_ERROR_WRITE;
  
  return CONFIG_MAN_ERROR_NONE;
}

int config_man_write_section(file_p file, config_man_page_section_p section) {
  int i;
  
  if (file_printf(file, ".SH %s\n", section->title) < 0)
    return CONFIG_MAN_ERROR_WRITE;
  for (i = 0; i < section->num_paragraphs; ++i)
    if (file_printf(file, ".P\n%s\n", section->paragraphs[i]) < 0)
      return CONFIG_MAN_ERROR_WRITE;
  
  return CONFIG_MAN_ERROR_NONE;
}
