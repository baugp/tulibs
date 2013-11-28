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

#include <ctype.h>

#include "file.h"

#include "string/string.h"

const char* config_file_errors[] = {
  "Success",
  "Failed to read configuration from file",
  "Failed to write configuration to file",
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

const config_default_t config_file_default_options = {
  config_file_default_params,
  sizeof(config_file_default_params)/sizeof(config_param_t),
};

const char* config_file_description =
  "A configuration variable named VAR is expected to be defined as VAR=VALUE. "
  "For structuring purposes, configuration variables may be grouped into "
  "variable sections. A section named SECTION is started with the expression "
  "[SECTION]. Comment lines are indicated by a leading '"
  CONFIG_FILE_COMMENT_START "' and will be ignored during reading.";

  
int config_file_read_line(file_t* file, char** line);
int config_file_write_header(file_t* file, const char* title,
  size_t max_width);
int config_file_write_comment(file_t* file, const char* comment,
  size_t max_width);
  
void config_file_section_init(config_file_section_t* section, const char*
  name, const char* title);
void config_file_section_destroy(config_file_section_t* section);
int config_file_section_write(file_t* file, const config_file_section_t*
  section, size_t max_width, config_file_comment_level_t comment_level);

void config_file_var_init(config_file_var_t* var, const char* name, const
  char* value, const char* description);
void config_file_var_destroy(config_file_var_t* var);
int config_file_var_write(file_t* file, const config_file_var_t* var,
  size_t max_width, config_file_comment_level_t comment_level);

void config_file_init(config_file_t* file, const char* title, size_t
    max_width, config_file_comment_level_t comment_level) {
  string_init_copy(&file->title, title);
  
  file->sections = 0;
  file->num_sections = 0;
  
  file->max_width = max_width;
  file->comment_level = comment_level;
  
  error_init(&file->error, config_file_errors);
}

void config_file_init_config(config_file_t* file, const config_t* config) {
  config_file_init(file,
    config_get_string(config, CONFIG_FILE_PARAMETER_TITLE),
    config_get_int(config, CONFIG_FILE_PARAMETER_MAX_WIDTH),
    config_get_enum(config, CONFIG_FILE_PARAMETER_COMMENT_LEVEL));
}

void config_file_destroy(config_file_t* file) {
  int i;
  
  string_destroy(&file->title);
  
  if (file->num_sections) {
    for (i = 0; i < file->num_sections; ++i)
      config_file_section_destroy(&file->sections[i]);

    free(file->sections);
    file->sections = 0;
    file->num_sections = 0;
  }

  error_destroy(&file->error);
}

config_file_section_t* config_file_add_section(config_file_t* file,
    const char* name, const char* title) {
  file->sections = realloc(file->sections, (file->num_sections+1)*
    sizeof(config_file_section_t));
  config_file_section_t* section = &file->sections[file->num_sections];
  ++file->num_sections;
  
  config_file_section_init(section, name, title);
  
  return section;
}

config_file_section_t* config_file_add_config(config_file_t* file, const char*
    name, const char* title, const config_t* config) {
  config_file_section_t* section = config_file_add_section(file, name, title);
  
  int i;
  for (i = 0; i < config->num_params; ++i)
    config_file_add_param(section, &config->params[i]);
  
  return section;
}

config_file_section_t* config_file_get_section(const config_file_t* file,
    const char* name) {
  int i;
  
  for (i = 0; i < file->num_sections; ++i)
    if (string_equal(file->sections[i].name, name))
      return &file->sections[i];
    
  return 0;
}

config_file_var_t* config_file_add_var(config_file_section_t* section,
    const char* name, const char* value, const char* description) {
  section->vars = realloc(section->vars, (section->num_vars+1)*
    sizeof(config_file_var_t));
  config_file_var_t* var = &section->vars[section->num_vars];
  ++section->num_vars;
  
  config_file_var_init(var, name, value, description);
  
  return var;
}

config_file_var_t* config_file_add_param(config_file_section_t* section,
    const config_param_t* param) {
  return config_file_add_var(section, param->key, param->value,
    param->description);
}

config_file_var_t* config_file_get_var(const config_file_section_t* section,
    const char* name) {
  int i;
  
  for (i = 0; i < section->num_vars; ++i)
    if (string_equal(section->vars[i].name, name))
      return &section->vars[i];
    
  return 0;
}

int config_file_read(const char* filename, config_file_t* file) {
  file_t _file;

  error_clear(&file->error);
  
  file_init_name(&_file, filename);
  if (string_equal(filename, "-"))
    file_open_stream(&_file, stdin, file_mode_read);
  else
    file_open(&_file, file_mode_read);

  if (!_file.handle) {
    error_blame(&file->error, &_file.error, CONFIG_FILE_ERROR_READ);
    file_destroy(&_file);
    
    return error_get(&file->error);
  }

  config_file_section_t* section = 0;
  char* line = 0;
  
  while (config_file_read_line(&_file, &line) > 0) {
    char* section_name = 0;
    char* var_name = 0;
    char *var_value = 0;
    char junk;
    
    if (string_scanf(line, " [ %a[^]] ] %1s", &section_name, &junk) == 1) {
      char* section_name_end = section_name+string_length(section_name)-1;
      while ((section_name_end > section_name) && isspace(*section_name_end))
        --section_name_end;
      section_name_end[1] = 0;
      
      section = config_file_add_section(file, section_name, 0);
      string_destroy(&section_name);
    }
    else if (string_scanf(line, " %a[^= ] = %a[^ ] %1s", &var_name, 
        &var_value, &junk) == 2) {
      if (!section)
        section = config_file_add_section(file, 0, 0);
      config_file_add_var(section, var_name, var_value, 0);
    
      string_destroy(&var_name);
      string_destroy(&var_value);
    }
    else {
      error_setf(&file->error, CONFIG_FILE_ERROR_FORMAT, line);
      break;
    }
  }
  string_destroy(&line);
  
  if (_file.error.code)
    error_blame(&file->error, &_file.error, CONFIG_FILE_ERROR_READ);
  
  file_destroy(&_file);
  
  return error_get(&file->error);
}

int config_file_write(const char* filename, config_file_t* file) {
  file_t _file;
  int i;
  
  error_clear(&file->error);
  
  file_init_name(&_file, filename);
  if (string_equal(filename, "-"))
    file_open_stream(&_file, stdout, file_mode_write);
  else
    file_open(&_file, file_mode_write);

  if (!_file.handle) {
    error_blame(&file->error, &_file.error, CONFIG_FILE_ERROR_READ);
    file_destroy(&_file);
    
    return error_get(&file->error);
  }
  
  if (!config_file_write_header(&_file, file->title, file->max_width)) {
    for (i = 0; i < file->num_sections; ++i)
        if (file->sections[i].num_vars) {
      if (file_printf(&_file, "\n") != 1) {
        error_blame(&file->error, &_file.error, CONFIG_FILE_ERROR_WRITE);
        break;
      }
      
      if (config_file_section_write(&_file, &file->sections[i],
          file->max_width, file->comment_level)) {
        error_blame(&file->error, &_file.error, CONFIG_FILE_ERROR_WRITE);
        break;
      }
    }
  }
  file_destroy(&_file);
  
  return error_get(&file->error);
}

int config_file_read_line(file_t* file, char** line) {
  int result = 0;
  
  while (!file_eof(file) &&
      ((result = file_read_line(file, line, 128)) >= 0)) {
    if ((result > 0) && string_starts_with(*line, CONFIG_FILE_COMMENT_START)) {
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

int config_file_write_header(file_t* file, const char* title, size_t
    max_width) {
  int result;
  
  if (!string_empty(title)) {
    if ((result = config_file_write_comment(file, title, max_width)) ||
        (result = config_file_write_comment(file, " ", max_width)))
      return result;
  }
  
  if ((result = config_file_write_comment(file, config_file_description,
        max_width)))
    return result;
  
  return CONFIG_FILE_ERROR_NONE;
}

int config_file_write_comment(file_t* file, const char* comment, size_t
    max_width) {
  size_t comment_start_length = string_length(CONFIG_FILE_COMMENT_START);
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

void config_file_section_init(config_file_section_t* section, const char*
    name, const char* title) {
  string_init_copy(&section->name, name);
  string_init_copy(&section->title, title);

  section->vars = 0;
  section->num_vars = 0;
}

void config_file_section_destroy(config_file_section_t* section) {
  int i;
  
  string_destroy(&section->name);
  string_destroy(&section->title);
  
  if (section->num_vars) {
    for (i = 0; i < section->num_vars; ++i)
      config_file_var_destroy(&section->vars[i]);
    free(section->vars);
  }
}

int config_file_section_write(file_t* file, const config_file_section_t*
    section, size_t max_width, config_file_comment_level_t comment_level) {
  int i, result;
  
  if (!string_empty(section->title) &&
    (comment_level >= config_file_comment_level_sections) &&
    (result = config_file_write_comment(file, section->title, max_width)))
      return result;
  
  if (!string_empty(section->name) &&
    (file_printf(file, "[%s]\n", section->name) < 0))
      return CONFIG_FILE_ERROR_WRITE;

  for (i = 0; i < section->num_vars; ++i) {
    if (i && file_printf(file, "\n") != 1) {
      result = CONFIG_FILE_ERROR_WRITE;
      break;
    }
    
    if ((result = config_file_var_write(file, &section->vars[i],
      max_width, comment_level)))
    return result;
  }

  return CONFIG_FILE_ERROR_NONE;
}

void config_file_var_init(config_file_var_t* var, const char* name, const
    char* value, const char* description) {
  string_init_copy(&var->name, name);
  string_init_copy(&var->value, value);
  
  string_init_copy(&var->description, description);
}

void config_file_var_destroy(config_file_var_t* var) {
  string_destroy(&var->name);
  string_destroy(&var->value);
  
  string_destroy(&var->description);
}

int config_file_var_write(file_t* file, const config_file_var_t* var, size_t
    max_width, config_file_comment_level_t comment_level) {
  int result = CONFIG_FILE_ERROR_NONE;
  
  if (!string_empty(var->description) &&
    (comment_level >= config_file_comment_level_verbose) &&
    (result = config_file_write_comment(file, var->description,
        max_width)))
      return result;
  
  if (file_printf(file, "%s = %s\n", var->name, var->value) < 0)
    result = CONFIG_FILE_ERROR_WRITE;

  return result;
}
