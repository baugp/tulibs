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
#include <ctype.h>

#include "file.h"

const char* config_file_errors[] = {
  "Success",
  "Failed to read configuration file",
  "Failed to write configuration file",
  "Invalid configuration file format",
};

config_param_t config_file_default_params[] = {
  {CONFIG_FILE_PARAMETER_TITLE,
    config_param_type_string,
    "",
    "",
    "The leading title comment of the generated configuration file"},
  {CONFIG_FILE_PARAMETER_MAX_WIDTH,
    config_param_type_int,
    "80",
    "[80, 1000]",
    "The maximum character width of the generated configuration file, "
    "applies to comments only"},
  {CONFIG_FILE_PARAMETER_COMMENT_LEVEL,
    config_param_type_enum,
    "minimal",
    "minimal|sections|verbose",
    "The comment level of the generated configuration file, where "
    "'minimal' suppresses any section or variable comments"},
};

config_t config_file_default_options = {
  config_file_default_params,
  sizeof(config_file_default_params)/sizeof(config_param_t),
};

const char* config_file_description =
  "A configuration variable named VAR is expected to be defined as VAR=VALUE. "
  "For structuring purposes, configuration variables may be grouped into "
  "variable sections. A section named SECTION is started with the expression "
  "[SECTION]. Comment lines are indicated by a leading '"
  CONFIG_FILE_COMMENT_START "' and will be ignored during reading.";

void config_file_init(config_file_p file, const char* title, size_t
    max_width, config_file_comment_level_t comment_level) {
  if (title)
    strcpy(file->title, title);
  else
    file->title[0] = 0;
  
  file->sections = 0;
  file->num_sections = 0;
  
  file->max_width = max_width;
  file->comment_level = comment_level;
  
  file->error = CONFIG_FILE_ERROR_NONE;
  file->error_what[0] = 0;
}

void config_file_init_config(config_file_p file, config_p config) {
  config_file_init(file,
    config_get_string(config, CONFIG_FILE_PARAMETER_TITLE),
    config_get_int(config, CONFIG_FILE_PARAMETER_MAX_WIDTH),
    config_get_enum(config, CONFIG_FILE_PARAMETER_COMMENT_LEVEL));
}

void config_file_destroy(config_file_p file) {
  int i, j;
  
  for (i = 0; i < file->num_sections; ++i) {
    config_file_section_p section = &file->sections[i];
    
    for (j = 0; j < section->num_vars; ++j)
      if (section->vars[j].description)
        free(section->vars[j].description);
      
    if (section->num_vars)
      free(section->vars);
  }

  if (file->num_sections) {
    free(file->sections);
    
    file->sections = 0;
    file->num_sections = 0;
  }
}

config_file_section_p config_file_add_section(config_file_p file,
    const char* name, const char* title) {
  file->sections = realloc(file->sections, (file->num_sections+1)*
    sizeof(config_file_section_t));
  config_file_section_p section = &file->sections[file->num_sections];
  ++file->num_sections;
  
  if (name)
    strcpy(section->name, name);
  else
    section->name[0] = 0;
  if (title)
    strcpy(section->title, title);
  else
    section->title[0] = 0;
  
  section->vars = 0;
  section->num_vars = 0;
  
  return section;
}

config_file_section_p config_file_add_config(config_file_p file, const char*
    name, const char* title, config_p config) {
  config_file_section_p section = config_file_add_section(file, name, title);
  
  int i;
  for (i = 0; i < config->num_params; ++i)
    config_file_add_param(section, &config->params[i]);
  
  return section;
}

config_file_section_p config_file_get_section(config_file_p file,
    const char* name) {
  int i;
  
  for (i = 0; i < file->num_sections; ++i)
    if (!strcmp(file->sections[i].name, name))
      return &file->sections[i];
    
  return 0;
}

config_file_var_p config_file_add_var(config_file_section_p section,
    const char* name, const char* value, const char* description) {
  section->vars = realloc(section->vars, (section->num_vars+1)*
    sizeof(config_file_var_t));
  config_file_var_p var = &section->vars[section->num_vars];
  ++section->num_vars;
  
  strcpy(var->name, name);
  strcpy(var->value, value);
  if (description) {
    var->description = malloc(strlen(description+1));
    strcpy(var->description, description);
  }
  else
    var->description = 0;
  
  return var;
}

config_file_var_p config_file_add_param(config_file_section_p section,
    config_param_p param) {
  return config_file_add_var(section, param->key, param->value,
    param->description);
}

config_file_var_p config_file_get_var(config_file_section_p section,
    const char* name) {
  int i;
  
  for (i = 0; i < section->num_vars; ++i)
    if (!strcmp(section->vars[i].name, name))
      return &section->vars[i];
    
  return 0;
}

int config_file_read(const char* filename, config_file_p file) {
  file_t _file;

  file->error = CONFIG_FILE_ERROR_NONE;
  file->error_what[0] = 0;
  
  file_init_name(&_file, filename);
  if (strcmp(filename, "-"))
    file_open(&_file, file_mode_read);
  else
    file_open_stream(&_file, stdin, file_mode_read);

  if (!_file.handle) {
    file->error = CONFIG_FILE_ERROR_READ;
    strcpy(file->error_what, filename);
  }

  if (!file->error) {
    config_file_section_p section = 0;
    char* line = 0;
    int result;
    
    while ((result = config_file_read_line(&_file, &line)) > 0) {
      char section_name[128], var_name[128], var_value[128], junk;
      
      if (sscanf(line, " [ %[^]] ] %1s", section_name, &junk) == 1) {
        char* section_name_end = section_name+strlen(section_name)-1;
        while ((section_name_end > section_name) && isspace(*section_name_end))
          --section_name_end;
        section_name_end[1] = 0;
        
        section = config_file_add_section(file, section_name, 0);
      }
      else if (sscanf(line, " %[^= ] = %[^ ] %1s", var_name, var_value,
          &junk) == 2) {
        if (!section)
          section = config_file_add_section(file, 0, 0);
        config_file_add_var(section, var_name, var_value, 0);
      }
      else {
        file->error = CONFIG_FILE_ERROR_FORMAT;
        strncpy(file->error_what, line, sizeof(file->error_what)-1);
        file->error_what[sizeof(file->error_what)-1] = 0;
        
        break;
      }
    }
    if (line)
      free(line);
    
    if (result < 0) {
      file->error = result;
      strcpy(file->error_what, filename);
    }
    
    file_close(&_file);
  }
  
  return file->error;
}

int config_file_write(const char* filename, config_file_p file) {
  file_t _file;
  int i, result = CONFIG_FILE_ERROR_NONE;
  
  file_init_name(&_file, filename);
  if (strcmp(filename, "-"))
    file_open(&_file, file_mode_write);
  else
    file_open_stream(&_file, stdout, file_mode_write);

  if (!_file.handle)
    return CONFIG_FILE_ERROR_WRITE;
  
  if (!(result = config_file_write_header(&_file, file->title,
      file->max_width))) {
    for (i = 0; i < file->num_sections; ++i)
        if (file->sections[i].num_vars) {
      if (file_printf(&_file, "\n") != 1) {
        result = CONFIG_FILE_ERROR_WRITE;
        break;
      }
      
      if ((result = config_file_write_section(&_file, &file->sections[i],
          file->max_width, file->comment_level)))
        break;
    }
  }
      
  file_close(&_file);
  return result;
}

int config_file_write_header(file_p file, const char* title, size_t
    max_width) {
  int result;
  
  if (title[0]) {
    if ((result = config_file_write_comment(file, title, max_width)) ||
        (result = config_file_write_comment(file, " ", max_width)))
      return result;
  }
  
  if ((result = config_file_write_comment(file, config_file_description,
        max_width)))
    return result;
  
  return CONFIG_FILE_ERROR_NONE;
}

int config_file_write_section(file_p file, config_file_section_p section,
    size_t max_width, config_file_comment_level_t comment_level) {
  int i, result;
  
  if (section->title[0] &&
    (comment_level >= config_file_comment_level_sections) &&
    (result = config_file_write_comment(file, section->title, max_width)))
      return result;
  
  if (section->name[0] && (file_printf(file, "[%s]\n", section->name) < 0))
    return CONFIG_FILE_ERROR_WRITE;

  for (i = 0; i < section->num_vars; ++i) {
    if (i && file_printf(file, "\n") != 1) {
      result = CONFIG_FILE_ERROR_WRITE;
      break;
    }
    
    if ((result = config_file_write_var(file, &section->vars[i],
      max_width, comment_level)))
    return result;
  }

  return CONFIG_FILE_ERROR_NONE;
}

int config_file_write_var(file_p file, config_file_var_p var, size_t
    max_width, config_file_comment_level_t comment_level) {
  int result = CONFIG_FILE_ERROR_NONE;
  
  if (var->description && var->description[0] &&
    (comment_level >= config_file_comment_level_verbose) &&
    (result = config_file_write_comment(file, var->description,
      max_width)))
    return result;
  
  if (file_printf(file, "%s = %s\n", var->name, var->value) < 0)
    result = CONFIG_FILE_ERROR_WRITE;

  return result;
}

int config_file_read_line(file_p file, char** line) {
  size_t comment_start_length = strlen(CONFIG_FILE_COMMENT_START);
  int result = 0;
  
  while (!file_eof(file) &&
      ((result = file_read_line(file, line, 128)) >= 0)) {
    if ((result > 0) && strncmp(*line, CONFIG_FILE_COMMENT_START, 
        comment_start_length)) {
      char* line_end = *line+result-1;
      while ((line_end > *line) && isspace(*line_end))
        --line_end;
      
      if (line_end > *line) {
        line_end[1] = 0;
        return line_end-*line+1;
      }
    }
  }
  
  if (result >= 0)
    return 0;
  else
    return CONFIG_FILE_ERROR_READ;
}

int config_file_write_comment(file_p file, const char* comment, size_t
    max_width) {
  size_t comment_start_length = strlen(CONFIG_FILE_COMMENT_START);
  size_t line_length = 0;
  const char* line = comment;

  while (1) {
    size_t i = 0, word_length = 0;
    const char* word = &line[line_length];
    
    if (word[0]) {
      while (isspace(word[i])) {
        i++;
        word_length++;
      }
      while (word[i] && !isspace(word[i])) {
        i++;
        word_length++;
      }
      if (word[i]) {
        i++;
        word_length++;
      }
    }
    
    if (comment_start_length+1+line_length+word_length >= max_width) {
      if (line_length > 0) {
        if (file_printf(file, "%s %.*s\n", CONFIG_FILE_COMMENT_START,
            (int)line_length, line) < 0)
          return CONFIG_FILE_ERROR_WRITE;
        
        line += isspace(word[i]) ? line_length+1 : line_length;
        line_length = 0;
      }
      else {
        if (file_printf(file, "%s %.*s", CONFIG_FILE_COMMENT_START,
            (int)word_length, line) < 0)
          return CONFIG_FILE_ERROR_WRITE;
        
        line += word_length;
      }
    }
    else if (!word_length) {
      if (line_length) {
        if (file_printf(file, "%s %s\n", CONFIG_FILE_COMMENT_START, line) < 0)
          return CONFIG_FILE_ERROR_WRITE;
      }
      break;
    }
    else
      line_length += word_length;
  }
  
  return CONFIG_FILE_ERROR_NONE;
}
