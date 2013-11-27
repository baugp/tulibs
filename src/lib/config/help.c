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

#include "help.h"

#include "string/string.h"

const char* config_help_errors[] = {
  "Success",
  "Maximum output width exceeded",
};

int config_help_print_arguments(FILE* stream, const char* section_title,
    const config_t* arguments, size_t max_width, size_t key_indent, size_t
    par_indent) {
  int result = CONFIG_HELP_ERROR_NONE;
  int i;
  
  if (arguments->num_params && !string_empty(section_title)) {
    char* title = 0;
    string_printf(&title, "%s:", section_title);
    result |= config_help_print(stream, title, max_width, 0, 0, 0);
    fprintf(stream, "\n");
    string_destroy(&title);
  }
  
  for (i = 0; i < arguments->num_params; ++i) {
    result |= config_help_print_argument(stream, &arguments->params[i],
      max_width, key_indent, par_indent);
    fprintf(stream, "\n");
  }
  
  return result;
}

int config_help_print_argument(FILE* stream, const config_param_t* param,
    size_t max_width, size_t key_indent, size_t par_indent) {
  int result = CONFIG_HELP_ERROR_NONE;
  char* argument = 0;
  char* description = 0;
  
  size_t argument_length = string_printf(&argument, "%s: <%s>",
    param->key,
    (param->type == config_param_type_enum) ||
      (param->type == config_param_type_bool) ? param->range : 
      config_param_types[param->type]);
  result |= config_help_print(stream, argument, max_width,
    key_indent+string_length(param->key)+3, key_indent, 0);
  string_destroy(&argument);
  
  string_printf(&description, "%s%s%s%s%s",
    !string_empty(param->description) ? param->description : "",
    !string_empty(param->description) && param->value ? " " : "",
    !string_empty(param->value) ? "(" : "",
    !string_empty(param->value) ? param->value : "",
    !string_empty(param->value) ? ")" : "");
  if (argument_length+key_indent+2 <= par_indent) {
    int line_indent = (int)par_indent-(int)(argument_length+
      key_indent);
    result |= config_help_print(stream, description,
      max_width, par_indent, line_indent, argument_length+key_indent);
  }
  else {
    fprintf(stream, "\n");
    result |= config_help_print(stream, description,
      max_width, par_indent, par_indent, 0);
  }                
  string_destroy(&description);
  
  return result;
}

int config_help_print_options(FILE* stream, const char* section_title,
    const config_t* options, const char* prefix, size_t max_width, size_t
    key_indent, size_t par_indent) {
  int result = CONFIG_HELP_ERROR_NONE;
  int i;
  
  if (options->num_params && section_title && section_title[0]) {
    char* title = 0;
    string_printf(&title, "%s:", section_title);
    result |= config_help_print(stream, title, max_width, 0, 0, 0);
    fprintf(stream, "\n");
    string_destroy(&title);
  }
  
  for (i = 0; i < options->num_params; ++i) {
    result |= config_help_print_option(stream, &options->params[i],
      prefix, max_width, key_indent, par_indent);
    fprintf(stream, "\n");
  }
  
  return result;
}

int config_help_print_option(FILE* stream, const config_param_t* param,
    const char* prefix, size_t max_width, size_t key_indent, size_t
    par_indent) {
  int result = CONFIG_HELP_ERROR_NONE;
  char* argument = 0;
  char* description = 0;
  
  int prefix_length = string_length(prefix);
  int argument_length = string_printf(&argument, "--%s%s%s%s%s",
    !string_empty(prefix) ? prefix : "",
    param->key,
    (param->type != config_param_type_bool) ? "=<" : "",
    (param->type != config_param_type_bool) ? 
      (param->type == config_param_type_enum) ? param->range :
      config_param_types[param->type] : "",
    (param->type != config_param_type_bool) ? ">" : "");
  if (argument_length+key_indent > max_width)
    result = CONFIG_HELP_ERROR_WIDTH;
  result |= config_help_print(stream, argument, max_width,
    key_indent+prefix_length+string_length(param->key)+4, key_indent, 0);
  string_destroy(&argument);

  string_printf(&description, "%s%s%s%s%s",
    !string_empty(param->description) ? param->description : "",
    !string_empty(param->description) && 
      (param->type != config_param_type_bool) && 
      !string_empty(param->value) ? " " : "",
    (param->type != config_param_type_bool) && 
      !string_empty(param->value) ? "(" : "",
    (param->type != config_param_type_bool) &&
      !string_empty(param->value) ? param->value : "",
    (param->type != config_param_type_bool) &&
      !string_empty(param->value) ? ")" : "");
  if (argument_length+key_indent+2 <= par_indent) {
    int line_indent = (int)par_indent-(int)(argument_length+
      key_indent);
    result |= config_help_print(stream, description,
      max_width, par_indent, line_indent, argument_length+key_indent);
  }
  else {
    fprintf(stream, "\n");
    result |= config_help_print(stream, description,
      max_width, par_indent, par_indent, 0);
  }
  string_destroy(&description);
  
  return result;
}

int config_help_print(FILE* stream, const char* text, size_t max_width,
    size_t par_indent, size_t line_indent, size_t line_offset) {
  int result = CONFIG_HELP_ERROR_NONE;
  size_t indent = line_indent;
  size_t offset = line_offset;
  size_t line_length = 0;
  const char* line = text;

  while (1) {
    size_t i = 0, word_length = 0;
    const char* word = &line[line_length];
    
    if (word[0]) {
      while (isspace(word[i]) || (word[i] == '|')) {
        i++;
        word_length++;
      }
      while (word[i] && !isspace(word[i]) && (word[i] != '|')) {
        i++;
        word_length++;
      }
      if (word[i]) {
        i++;
        word_length++;
      }
    }
    
    if (offset+indent+line_length+word_length >= max_width) {
      if (line_length > 0) {
        fprintf(stream, "%*s%.*s\n", (int)indent, "",
          (int)line_length, line);
        line += isspace(word[i]) ? line_length+1 : line_length;
        line_length = 0;
      }
      else {
        fprintf(stream, "%*s%.*s", (int)indent, "",
          (int)word_length, line);
        line += word_length;
        
        result = CONFIG_HELP_ERROR_WIDTH;
      }
      
      indent = par_indent;
      offset = 0;
    }
    else if (!word_length) {
      if (line_length)
        fprintf(stream, "%*s%s", (int)indent, "", line);
      break;
    }
    else
      line_length += word_length;
  }
  
  return result;
}
