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
#include <ctype.h>

#include "help.h"

const char* config_help_errors[] = {
  "Success",
  "Maximum output width exceeded",
};

int config_help_print_arguments(FILE* stream, const char* section_title,
    config_p arguments, size_t max_width, size_t key_indent, size_t
    par_indent) {
  int result = CONFIG_HELP_ERROR_NONE;
  int i;
  
  if (arguments->num_params && section_title && section_title[0]) {
    char title[strlen(section_title)+2];
    sprintf(title, "%s:", section_title);
    result |= config_help_print(stream, title, max_width, 0, 0, 0);
    fprintf(stream, "\n");
  }
  
  for (i = 0; i < arguments->num_params; ++i) {
    result |= config_help_print_argument(stream, &arguments->params[i],
      max_width, key_indent, par_indent);
    fprintf(stream, "\n");
  }
  
  return result;
}

int config_help_print_argument(FILE* stream, config_param_p param,
    size_t max_width, size_t key_indent, size_t par_indent) {
  int result = CONFIG_HELP_ERROR_NONE;
  char argument[sizeof(param->key)+sizeof(param->range)+5];
  char value[sizeof(param->range)];
  char description[sizeof(param->description)+sizeof(param->value)+4];

  if ((param->type == config_param_type_enum) ||
      (param->type == config_param_type_bool))
    strcpy(value, param->range);
  else
    strcpy(value, config_param_types[param->type]);
  
  int argument_length = sprintf(argument, "%s: <%s>", param->key, value);
  if (argument_length+key_indent > max_width)
    result = CONFIG_HELP_ERROR_WIDTH;

  description[0] = 0;
  if (param->description[0])
    strcpy(description, param->description);
  if (param->value[0]) {
    char default_value[sizeof(param->value)+3];
    sprintf(default_value, " (%s)", param->value); 
    strcat(description, default_value);
  }

  config_help_print(stream, argument, max_width, key_indent+
    strlen(param->key)+3, key_indent, 0);
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
  
  return result;
}

int config_help_print_options(FILE* stream, const char* section_title,
    config_p options, const char* prefix, size_t max_width, size_t
    key_indent, size_t par_indent) {
  int result = CONFIG_HELP_ERROR_NONE;
  int i;
  
  if (options->num_params && section_title && section_title[0]) {
    char title[strlen(section_title)+2];
    sprintf(title, "%s:", section_title);
    result |= config_help_print(stream, title, max_width, 0, 0, 0);
    fprintf(stream, "\n");
  }
  
  for (i = 0; i < options->num_params; ++i) {
    result |= config_help_print_option(stream, &options->params[i],
      prefix, max_width, key_indent, par_indent);
    fprintf(stream, "\n");
  }
  
  return result;
}

int config_help_print_option(FILE* stream, config_param_p param,
    const char* prefix, size_t max_width, size_t key_indent, size_t
    par_indent) {
  int result = CONFIG_HELP_ERROR_NONE;
  char argument[sizeof(param->key)+sizeof(param->range)+5];
  char value[sizeof(param->range)];
  char description[sizeof(param->description)+sizeof(param->value)+4];

  value[0] = 0;
  if (param->type == config_param_type_enum)
    strcpy(value, param->range);
  else if (param->type != config_param_type_bool)
    strcpy(value, config_param_types[param->type]);

  int prefix_length = prefix ? strlen(prefix) : 0;
  int argument_length = sprintf(argument, "--%s%s%s%s%s",
    prefix ? prefix : "", param->key, value[0] ? "=<" : "",
    value[0] ? value : "", value[0] ? ">" : "");
  if (argument_length+key_indent > max_width)
    result = CONFIG_HELP_ERROR_WIDTH;

  description[0] = 0;
  if (param->description[0]) {
    if (param->type != config_param_type_bool)
      sprintf(description, "%s (%s)", param->description, param->value);
    else
      strcpy(description, param->description);
  }
  else if (param->type != config_param_type_bool)
    sprintf(description, "(%s)", param->value);
  
  config_help_print(stream, argument, max_width, key_indent+prefix_length+
    strlen(param->key)+4, key_indent, 0);
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

int config_help_justify(char* text, size_t max_width) {
  int result = CONFIG_HELP_ERROR_NONE;
  size_t line_length = 0;
  char* line = text;

  while (1) {
    size_t i = 0, word_length = 0;
    char* word = &line[line_length];
    
    if (word[0]) {
      while (isspace(word[i])) {
        if (word[i] == '\t')
          word[i] = ' ';
        
        i++;
        word_length++;
      }
      while (word[i] && !isspace(word[i++]))
        word_length++;
    }
    
    if (!word_length)
      break;
    
    if (line_length+word_length >= max_width) {
      if (line_length > 0) {
        line[line_length] = '\n';
        line += line_length+1;
        line_length = 0;
      }
      else {
        line[word_length] = '\n';
        line += word_length+1;
        
        result = CONFIG_HELP_ERROR_WIDTH;
      }
    }
    else
      line_length += word_length;
  }
  
  return result;
}
