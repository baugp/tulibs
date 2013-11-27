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

#include <stdarg.h>
#include <sys/time.h>
#include <time.h>
#include <ctype.h>

#include "man.h"

#include "string/string.h"

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

const config_t config_man_default_options = {
  config_man_default_params,
  sizeof(config_man_default_params)/sizeof(config_param_t),
};

int config_man_page_write(file_t* file, config_man_page_t* page);

void config_man_page_header_init(config_man_page_header_t* header,
  const char* name, size_t section, const char* title);
void config_man_page_header_destroy(config_man_page_header_t* header);
int config_man_page_header_write(file_t* file, const
  config_man_page_header_t* header);

void config_man_page_section_init(config_man_page_section_t* section,
  const char* title);
void config_man_page_section_destroy(config_man_page_section_t* section);
int config_man_page_section_write(file_t* file, const
  config_man_page_section_t* section);

void config_man_init(config_man_page_t* page, const char* name, size_t
    section, const char* title) {
  config_man_page_header_init(&page->header, name, section, title);
    
  page->sections = 0;
  page->num_sections = 0;

  error_init(&page->error, config_man_errors);
}

void config_man_init_config(config_man_page_t* page, const char* name,
    const config_t* config) {
  config_man_init(page, name, 
    config_get_int(config, CONFIG_MAN_PARAMETER_SECTION),
    config_get_string(config, CONFIG_MAN_PARAMETER_TITLE));
}

void config_man_destroy(config_man_page_t* page) {
  int i;
  
  config_man_page_header_destroy(&page->header);
  
  if (page->num_sections) {
    for (i = 0; i < page->num_sections; ++i)
      config_man_page_section_destroy(&page->sections[i]);

    free(page->sections);
    page->sections = 0;
    page->num_sections = 0;
  }
  
  error_destroy(&page->error);
}

config_man_page_section_t* config_man_add_section(config_man_page_t* page,
    const char* title) {
  page->sections = realloc(page->sections, (page->num_sections+1)*
    sizeof(config_man_page_section_t));
  config_man_page_section_t* section = &page->sections[page->num_sections];
  ++page->num_sections;

  config_man_page_section_init(section, title);
  
  return section;
}

config_man_page_section_t* config_man_add_summary(config_man_page_t* page,
    const char* summary) {
  config_man_page_section_t* section = config_man_add_section(page,
    CONFIG_MAN_SECTION_SUMMARY);
  config_man_printf(section, "%s%s",
    summary,
    string_ends_with(summary, ".") ? "" : ".");
  
  return section;
}

config_man_page_section_t* config_man_add_command_summary(config_man_page_t*
    page, const char* command, const char* summary) {
  config_man_page_section_t* section = config_man_add_section(page,
    CONFIG_MAN_SECTION_SUMMARY);
  config_man_printf(section, "%s - %s%s",
    command,
    summary,
    string_ends_with(summary, ".") ? "" : ".");
  
  return section;
}

config_man_page_section_t* config_man_add_synopsis(config_man_page_t* page,
    const char* synopsis) {
  config_man_page_section_t* section = config_man_add_section(page,
    CONFIG_MAN_SECTION_SYNOPSIS);
  config_man_printf(section, synopsis);
  
  return section;
}

config_man_page_section_t* config_man_add_description(config_man_page_t* page,
    const char* description) {
  config_man_page_section_t* section = config_man_add_section(page,
    CONFIG_MAN_SECTION_DESCRIPTION);
  config_man_printf(section, "%s%s",
    description,
    string_ends_with(description, ".") ? "" : ".");
  
  return section;
}

config_man_page_section_t* config_man_add_config(config_man_page_t* page,
    const char* title, const char* preface, const config_t* config, const
    char* format) {
  config_man_page_section_t*  section = config_man_add_section(page, title);
  int i;

  if (preface)
    config_man_printf(section, preface);
  if (config) {
    for (i = 0; i < config->num_params; ++i)
      config_man_add_param(section, &config->params[i], format);
  }
  
  return section;
}

const char* config_man_add_param(config_man_page_section_t* section,
    const config_param_t* param, const char* format) {
  char* par_format = 0;
  string_printf(&par_format, "%s\n.RS\n%%s%%s\n.P\n%%s.RE", format);
  
  char* param_range = 0;
  if ((param->type != config_param_type_enum) &&
      (param->type != config_param_type_bool) &&
      !string_empty(param->range))
    string_printf(&param_range,
      "The permissible range of this argument is '%s'%s%s%s.\n",
      param->range,
      !string_empty(param->value) ? ", and its value defaults to '" : "",
      !string_empty(param->value) ? param->value : "",
      !string_empty(param->value) ? "'" : "");
  else if ((param->type != config_param_type_bool) &&
      !string_empty(param->value))
    string_printf(&param_range,
      "The default value of this argument is '%s'.\n",
      param->value);
  
  return config_man_printf(section, par_format,
    param->key,
    (param->type == config_param_type_enum) ||
      (param->type == config_param_type_bool) ? param->range :
      config_param_types[param->type],
    !string_empty(param->description) ? param->description :
      "This argument requires documentation.",
    string_ends_with(param->description, ".") ? "" : ".",
    param_range ? param_range : "");
  
  string_destroy(&par_format);
  string_destroy(&param_range);
}

config_man_page_section_t* config_man_add_arguments(config_man_page_t* page,
    const char* title, const char* preface, const config_t* arguments) {
  config_man_page_section_t* section = config_man_add_config(page, title,
    preface, 0, 0);
  
  int i;
  for (i = 0; i < arguments->num_params; ++i) {
    const char* format = 0;
    if ((arguments->params[i].type == config_param_type_enum) ||
        (arguments->params[i].type == config_param_type_bool))
      format = ".BI \"%s\"\\c\n: <%s>";
    else
      format = ".BI \"%s\"\\c\n.RI \": <\" %s \">\"";
      
    config_man_add_param(section, &arguments->params[i], format);
  }
  
  return section;
}

config_man_page_section_t* config_man_add_options(config_man_page_t* page,
    const char* title, const char* preface, const config_t* options, const
    char* prefix) {
  config_man_page_section_t* section = config_man_add_config(page, title,
    preface, 0, 0);
  
  int i;
  for (i = 0; i < options->num_params; ++i) {
    char* format = 0;
    if (options->params[i].type == config_param_type_enum)
      string_printf(&format, ".BI \"--%s%%s\"\\c\n=<%%s>",
        prefix ? prefix : "");
    else if (options->params[i].type == config_param_type_bool)
      string_printf(&format, ".BI \"--%s%%s\"%%.0s", prefix ? prefix : "");
    else
      string_printf(&format, ".BI \"--%s%%s\"\\c\n.RI \"=<\" %%s \">\"",
        prefix ? prefix : "");
    
    config_man_add_param(section, &options->params[i], format);
    string_destroy(&format);
  }
  
  return section;
}

config_man_page_section_t* config_man_add_authors(config_man_page_t* page,
    const char* authors) {
  config_man_page_section_t* section = config_man_add_section(page,
    CONFIG_MAN_SECTION_AUTHORS);
  config_man_printf(section, "Written by %s.", authors);
  
  return section;
}

config_man_page_section_t* config_man_add_bugs(config_man_page_t* page,
    const char* contact) {
  config_man_page_section_t* section = config_man_add_section(page,
    CONFIG_MAN_SECTION_BUGS);
  config_man_printf(section, "Report bugs to <%s>.", contact);
  
  return section;
}

config_man_page_section_t* config_man_add_copyright(config_man_page_t* page,
    const char* project, const char* license) {
  config_man_page_section_t* section = config_man_add_section(page,
    CONFIG_MAN_SECTION_COPYRIGHT);
  config_man_printf(section, "%s is published under the %s.", project,
    license);
  
  return section;
}

config_man_page_section_t* config_man_add_colophon(config_man_page_t* page,
    const char* project, const char* version, const char* home) {
  config_man_page_section_t* section = config_man_add_section(page,
    CONFIG_MAN_SECTION_COLOPHON);
  config_man_printf(section, "This page is part%s%s of the %s project.",
    version ? " of version " : "",
    version ? version : "", project);
  if (home)
    config_man_printf(section, "A description of the project, and information "
      "about reporting bugs, can be found at %s.", home);
  
  return section;
}

void config_man_add_project_sections(config_man_page_t* page, const
    config_project_t* project) {
  if (project->authors)
    config_man_add_authors(page, project->authors);
  if (project->contact)
    config_man_add_bugs(page, project->contact);
  if (project->name && project->license)
    config_man_add_copyright(page, project->name, project->license);
  if (project->name && project->version && project->home)
    config_man_add_colophon(page, project->name, project->version,
      project->home);
}

const char* config_man_printf(config_man_page_section_t* section,
    const char* format, ...) {
  char* paragraph = 0;
  
  va_list vargs;
  va_start(vargs, format);
  string_vprintf(&paragraph, format, vargs);
  va_end(vargs);
  
  if (paragraph) {
    section->paragraphs = realloc(section->paragraphs,
      (section->num_paragraphs+1)*sizeof(char*));
    section->paragraphs[section->num_paragraphs] = paragraph;
    ++section->num_paragraphs;
  }

  return paragraph;
}

int config_man_write(const char* filename, config_man_page_t* page) {
  file_t file;
  
  error_clear(&page->error);
  
  file_init_name(&file, filename);
  if (string_equal(filename, "-"))
    file_open_stream(&file, stdout, file_mode_write);
  else
    file_open(&file, file_mode_write);

  if (!file.handle)
    error_blame(&page->error, &file.error, CONFIG_MAN_ERROR_WRITE);
  else
    config_man_page_write(&file, page);
    
  file_destroy(&file);
  
  return error_get(&page->error);
}

int config_man_page_write(file_t* file, config_man_page_t* page) {
  int i;
  
  error_clear(&page->error);
  
  if (!config_man_page_header_write(file, &page->header)) {
    for (i = 0; i < page->num_sections; ++i)
        if (config_man_page_section_write(file, &page->sections[i])) {
      error_blame(&page->error, &file->error, CONFIG_MAN_ERROR_WRITE);
      break;
    }
  }
  else
    error_blame(&page->error, &file->error, CONFIG_MAN_ERROR_WRITE);

  return error_get(&page->error);
}

void config_man_page_header_init(config_man_page_header_t* header,
    const char* name, size_t section, const char* title) {
  string_init_copy(&header->name, name);
  header->section = section;
  
  string_init_copy(&header->title, title);
}

void config_man_page_header_destroy(config_man_page_header_t* header) {
  string_destroy(&header->name);
  string_destroy(&header->title);
}

int config_man_page_header_write(file_t* file, const config_man_page_header_t*
    header) {
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

void config_man_page_section_init(config_man_page_section_t* section,
    const char* title) {
  string_init_copy(&section->title, title);
  string_upper(section->title);
  
  section->paragraphs = 0;
  section->num_paragraphs = 0;
}

void config_man_page_section_destroy(config_man_page_section_t* section) {
  int i;
  
  string_destroy(&section->title);
  
  if (section->num_paragraphs) {
    for (i = 0; i < section->num_paragraphs; ++i)
      free(section->paragraphs[i]);
    free(section->paragraphs);
  }
}

int config_man_page_section_write(file_t* file, const
    config_man_page_section_t* section) {
  int i;
  
  if (file_printf(file, ".SH %s\n", section->title) < 0)
    return CONFIG_MAN_ERROR_WRITE;
  for (i = 0; i < section->num_paragraphs; ++i)
    if (file_printf(file, ".P\n%s\n", section->paragraphs[i]) < 0)
      return CONFIG_MAN_ERROR_WRITE;
  
  return CONFIG_MAN_ERROR_NONE;
}
