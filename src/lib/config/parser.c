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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"

#include "help.h"
#include "man.h"
#include "project.h"
#include "file.h"

const char* config_parser_errors[] = {
  "Success",
  "Failed to write manual page",
  "Failed to read configuration file",
  "Failed to write configuration file",
  "Invalid configuration file format",
  "Invalid configuration file section",
  "Unexpected argument",
  "Missing argument",
  "Invalid argument format",
  "Invalid argument key",
  "Invalid argument value"
};

config_param_t config_parser_default_params[] = {
  {CONFIG_PARSER_PARAMETER_HELP,
    config_param_type_bool,
    "false",
    "false|true",
    "Display help and exit"},
  {CONFIG_PARSER_PARAMETER_FILE,
    config_param_type_string,
    "",
    "",
    "Load options from the specified configuration file or '-' for stdin, "
    "values may be overridden by options provided on the command line"},
  {CONFIG_PARSER_PARAMETER_FILE_OUTPUT,
    config_param_type_string,
    "",
    "",
    "Save options to the specified configuration file or '-' for stdout, "
    "and exit"},
  {CONFIG_PARSER_PARAMETER_MAN_OUTPUT,
    config_param_type_string,
    "",
    "",
    "Write Linux manual page to the specified output file or '-' for "
    "stdout, and exit"},
};

config_t config_parser_default_options = {
  config_parser_default_params,
  sizeof(config_parser_default_params)/sizeof(config_param_t),
};

const char* config_parser_description =
  "Positional arguments must be supplied on the command line in the "
  "expected order. Non-positional arguments OPTi should precede any "
  "positional arguments and are required to be of the format --KEYi[=VALUEi].";

void config_parser_init(config_parser_p parser, config_p arguments,
    config_p options, const char* summary, const char* description) {
  config_parser_init_default(parser, summary, description);
  
  if (arguments)
    config_init_copy(&parser->arguments, arguments);
  else
    config_init(&parser->arguments);

  if (options)
    config_merge(&parser->options, options);
}

void config_parser_init_default(config_parser_p parser, const char* summary,
    const char* description) {
  config_init(&parser->arguments);
  config_init(&parser->options);
  
  parser->option_groups = 0;
  parser->num_option_groups = 0;
  
  if (summary)
    strcpy(parser->summary, summary);
  else
    parser->summary[0] = 0;
  if (description)
    strcpy(parser->description, description);
  else
    parser->description[0] = 0;
  
  parser->command[0] = 0;
  parser->usage[0] = 0;
  
  parser->error = CONFIG_PARSER_ERROR_NONE;
  parser->error_what[0] = 0;
}

void config_parser_destroy(config_parser_p parser) {
  config_destroy(&parser->arguments);
  config_destroy(&parser->options);
  
  if (parser->num_option_groups) {
    int i;
  
    for (i = 0; i < parser->num_option_groups; ++i)
      config_destroy(&parser->option_groups[i].options);
    free(parser->option_groups);
    
    parser->option_groups = 0;
    parser->num_option_groups = 0;
  }
}

config_parser_option_group_p config_parser_add_option_group(
    config_parser_p parser, const char* name, config_p options,
    const char* summary, const char* description) {
  config_parser_option_group_p option_group = 0;
  
  parser->option_groups = realloc(parser->option_groups,
    (parser->num_option_groups+1)*sizeof(config_parser_option_group_t));
  option_group = &parser->option_groups[parser->num_option_groups];
  ++parser->num_option_groups;

  strcpy(option_group->name, name);
  if (options)
    config_init_copy(&option_group->options, options);
  else
    config_init(&option_group->options);
  
  if (summary)
    strcpy(option_group->summary, summary);
  else
    option_group->summary[0] = 0;
  if (description)
    strcpy(option_group->description, description);
  else
    option_group->description[0] = 0;
  
  return option_group;
}

config_parser_option_group_p config_parser_get_option_group(
    config_parser_p parser, const char* name) {
  int i;
  
  for (i = 0; i < parser->num_option_groups; ++i)
    if (!strcmp(parser->option_groups[i].name, name))
      return &parser->option_groups[i];
  
  return 0;
}

int config_parser_parse(config_parser_p parser, int argc, char** argv,
    config_parser_exit_t parser_exit) {
  strcpy(parser->command, argv[0]);
  const char* command = strrchr(argv[0], '/');
  command = command ? command+1 : argv[0];
  int i;
    
  sprintf(parser->usage, "%s [OPT1 [OPT2 [...]]]", command);
  for (i = 0; i < parser->arguments.num_params; ++i) {
    char argument[sizeof(((config_param_p)0)->key)+20];
    config_param_p param = &parser->arguments.params[i];
    sprintf(argument, " %s%s%s", param->value[0] ? "[" : "", param->key,
      param->value[0] ? "]" : "");
    strcat(parser->usage, argument);
  }
  
  parser->error = CONFIG_PARSER_ERROR_NONE;
  parser->error_what[0] = 0;

  config_t parser_options, file_options, man_options, project_options;
  config_init_copy(&parser_options, &config_parser_default_options);
  config_init_copy(&file_options, &config_file_default_options);
  config_init_copy(&man_options, &config_man_default_options);
  config_init_copy(&project_options, &config_project_default_options);
  
  config_param_p options[argc-1];
  const char* option_values[argc-1];
  size_t num_options = 0;
  
  for (i = 1; i < argc; ++i) {
    if ((argv[i][0] == '-') && (argv[i][1] == '-')) {
      config_param_p param = 0;

      char* key_start = &argv[i][2];
      size_t key_length = 0;
      char* value_start = strchr(key_start, '=');
      if (value_start)
        ++value_start;
      
      if ((value_start > key_start) && key_start[0] && value_start[0])
        key_length = value_start-key_start-1;
      else if (key_start[0] && !value_start)
        key_length = strlen(key_start);
      else {
        parser->error = CONFIG_PARSER_ERROR_ARGUMENT_FORMAT;
        strcpy(parser->error_what, argv[i]);
        break;
      }
      
      char key[key_length+1];
      strncpy(key, key_start, key_length);
      key[key_length] = 0;
      
      if ((param = config_get_param(&parser_options, key))) {
        int result;
        if (value_start)
          result = config_param_set_value(param, value_start);
        else
          result = config_param_set_bool(param, config_param_true);
        
        if (result) {
          parser->error = CONFIG_PARSER_ERROR_ARGUMENT_VALUE;
          strcpy(parser->error_what, value_start);
          break;
        }
      }
      else {
        param = config_get_param(&parser->options, key);
        
        if (!param && !strncasecmp(key, CONFIG_FILE_ARG_PREFIX"-",
            strlen(CONFIG_MAN_ARG_PREFIX)+1))
          param = config_get_param(&file_options,
            &key[strlen(CONFIG_FILE_ARG_PREFIX)+1]);
          
        if (!param && !strncasecmp(key, CONFIG_MAN_ARG_PREFIX"-",
            strlen(CONFIG_MAN_ARG_PREFIX)+1))
          param = config_get_param(&man_options,
            &key[strlen(CONFIG_MAN_ARG_PREFIX)+1]);
          
        if (!param && !strncasecmp(key, CONFIG_PROJECT_ARG_PREFIX"-",
            strlen(CONFIG_PROJECT_ARG_PREFIX)+1))
          param = config_get_param(&project_options,
            &key[strlen(CONFIG_PROJECT_ARG_PREFIX)+1]);
          
        if (param) {
          int result = config_param_set_value(param, value_start);
          
          if (result) {
            parser->error = CONFIG_PARSER_ERROR_ARGUMENT_VALUE;
            strcpy(parser->error_what, argv[i]);
            break;
          }
        }
        else {
          int j;
          for (j = 0; j < parser->num_option_groups; ++j) {
            config_parser_option_group_p option_group =
              &parser->option_groups[j];
            size_t prefix_length = strlen(option_group->name);
            
            if (!strncasecmp(key, option_group->name, prefix_length) &&
                key[prefix_length] == '-')
              param = config_get_param(&option_group->options,
                &key[prefix_length+1]);
            if (param)
              break;
          }

          if (param) {
            options[num_options] = param;
            option_values[num_options] = value_start;
            ++num_options;
          }          
        }
        
        if (!param) {
          parser->error = CONFIG_PARSER_ERROR_ARGUMENT_KEY;
          strcpy(parser->error_what, key);        
          break;
        }
      }
    }
    else
      break;
  }

  if (!parser->error && (config_get_bool(&parser_options,
      CONFIG_PARSER_PARAMETER_HELP))) {
    config_parser_print_help(stdout, parser);    
    exit(EXIT_SUCCESS);
  }

  const char* man_output = config_get_string(&parser_options,
    CONFIG_PARSER_PARAMETER_MAN_OUTPUT);
  if (!parser->error && man_output[0] && !config_parser_write_man(
      man_output, parser, &man_options, &project_options))
    exit(EXIT_SUCCESS);
  
  const char* config_file = config_get_string(&parser_options,
    CONFIG_PARSER_PARAMETER_FILE);
  if (!parser->error && config_file[0])
    config_parser_read_file(config_file, parser, &file_options);
  
  if (!parser->error) {
    int j;
    for (j = 0 ; j < num_options; ++j) {
      int result;
      if (option_values[j])
        result = config_param_set_value(options[j], option_values[j]);
      else
        result = config_param_set_bool(options[j], config_param_true);
      
      if (result) {
        parser->error = CONFIG_PARSER_ERROR_ARGUMENT_VALUE;
        strcpy(parser->error_what, option_values[j]);
        break;
      }
    }
  }  
    
  int j = 0;
  if (!parser->error) {
    for (j = 0 ; i < argc; ++i, ++j) {
      if (j < parser->arguments.num_params) {
        config_param_p param = &parser->arguments.params[j];
        int result = config_param_set_value(param, argv[i]);
        
        if (result) {
          parser->error = CONFIG_PARSER_ERROR_ARGUMENT_VALUE;
          strcpy(parser->error_what, argv[i]);
          break;
        }
      }
      else {
        parser->error = CONFIG_PARSER_ERROR_ARGUMENT;
        strcpy(parser->error_what, argv[i]);
        break;
      }
    }
  }

  const char* file_output = config_get_string(&parser_options,
    CONFIG_PARSER_PARAMETER_FILE_OUTPUT);
  if (!parser->error && file_output[0] && !config_parser_write_file(
      file_output, parser, &file_options))
    exit(EXIT_SUCCESS);
    
  if (!parser->error && (j < parser->arguments.num_params) && 
      !parser->arguments.params[j].value[0]) {
    parser->error = CONFIG_PARSER_ERROR_ARGUMENT_MISSING;
    strcpy(parser->error_what, parser->arguments.params[j].key);
  }
    
  if (parser->error && (parser_exit & config_parser_exit_error)) {
    config_parser_print_usage(stderr, parser);
    exit(EXIT_FAILURE);
  }

  config_destroy(&parser_options);
  config_destroy(&file_options);
  config_destroy(&man_options);
  config_destroy(&project_options);
  
  return parser->error;
}

int config_parser_read_file(const char* filename, config_parser_p parser,
    config_p file_options) {
  config_file_t file;
  config_file_init_config(&file, file_options);
  int result;

  result = config_file_read(filename, &file);
  if (result == CONFIG_FILE_ERROR_FORMAT) {
    parser->error = CONFIG_PARSER_ERROR_FILE_FORMAT;
    strcpy(parser->error_what, file.error_what);
  }
  else if (result) {
    parser->error = CONFIG_PARSER_ERROR_FILE_READ;
    strcpy(parser->error_what, filename);
  }
  else {
    int i, j;
    for (i = 0; i < file.num_sections; ++i) {
      config_file_section_p section = &file.sections[i];
      config_p options;
      
      if (section->name[0]) {
        config_parser_option_group_p option_group =
          config_parser_get_option_group(parser, section->name);
        options = option_group ? &option_group->options : 0;
      }
      else
        options = &parser->options;
    
      if (!options) {
        parser->error = CONFIG_PARSER_ERROR_FILE_SECTION;
        strcpy(parser->error_what, section->name);
        break;
      }
      
      for (j = 0; j < section->num_vars; ++j) {
        config_file_var_p var = &section->vars[j];
        config_param_p param = config_get_param(options, var->name);
        
        if (!param) {
          parser->error = CONFIG_PARSER_ERROR_ARGUMENT_KEY;
          strcpy(parser->error_what, var->name);
          break;
        }
        
        if (config_param_set_value(param, var->value)) {
          parser->error = CONFIG_PARSER_ERROR_ARGUMENT_VALUE;
          strcpy(parser->error_what, var->value);
          break;
        }
      }
    }
  }
  
  return parser->error;
}

int config_parser_write_file(const char* filename, config_parser_p parser,
    config_p file_options) {
  config_file_t file;
  config_file_init_config(&file, file_options);

  config_file_add_config(&file, 0, 0, &parser->options);
  
  int i;
  for (i = 0; i < parser->num_option_groups; ++i) {
    config_parser_option_group_p option_group = &parser->option_groups[i];
    config_file_add_config(&file, option_group->name, option_group->summary,
      &option_group->options);
  }
  
  int result = config_file_write(filename, &file);
  
  config_file_destroy(&file);
  
  if (result) {
    parser->error = CONFIG_PARSER_ERROR_FILE_WRITE;
    strcpy(parser->error_what, filename);
  }

  return parser->error;
}

void config_parser_print_usage(FILE* stream, config_parser_p parser) {
  char help_command[strlen(parser->command)+
    strlen(CONFIG_PARSER_PARAMETER_HELP)+4];
  sprintf(help_command, "%s --%s", parser->command,
    CONFIG_PARSER_PARAMETER_HELP);
  char usage[strlen(parser->usage)+64];
  int i;

  if (parser->error) {
    char error[strlen(config_parser_errors[parser->error])+
      strlen(parser->error_what)+32];
    i = sprintf(error, "Error: %s", config_parser_errors[parser->error]);
    if (parser->error_what[0])
      sprintf(&error[i], " \"%s\"", parser->error_what);
    strcat(error, "\n");
    config_help_print(stream, error, CONFIG_PARSER_HELP_WIDTH,
      CONFIG_PARSER_HELP_HANG_INDENT, 0, 0);
  }
  
  sprintf(usage, "Usage: %s\n", parser->usage);
  config_help_print(stream, usage, CONFIG_PARSER_HELP_WIDTH,
    CONFIG_PARSER_HELP_HANG_INDENT, 0, 0);

  if (!parser->error && (parser->description[0] != 0)) {
    config_help_print(stream, parser->description,
      CONFIG_PARSER_HELP_WIDTH, 0, 0, 0);
    fprintf(stream, "\n");
  }
    
  fprintf(stream, "\n");
  config_help_print(stream, config_parser_description,
    CONFIG_PARSER_HELP_WIDTH, 0, 0, 0);
  fprintf(stream, "\n");
  
  if (parser->error) {
    fprintf(stream, "\n");
    config_help_print(stream, 
      "To see a summary of supported options and mandatory arguments, "
      "simply type\n",
      CONFIG_PARSER_HELP_WIDTH, 0, 0, 0);
    fprintf(stream, "> %s\n", help_command);
    config_help_print(stream, 
      "into your terminal or check out the documentation.\n",
      CONFIG_PARSER_HELP_WIDTH, 0, 0, 0);
  }
}

void config_parser_print_help(FILE* stream, config_parser_p parser) {
  config_parser_print_usage(stream, parser);
  int i;

  if (parser->arguments.num_params) {
    fprintf(stream, "\n");
    config_help_print_arguments(stream, "Positional arguments",
      &parser->arguments, CONFIG_PARSER_HELP_WIDTH,
      CONFIG_PARSER_HELP_KEY_INDENT, CONFIG_PARSER_HELP_PAR_INDENT);
  }
  
  if (parser->options.num_params) {
    fprintf(stream, "\n");
    config_help_print_options(stream, "General options", &parser->options,
      0, CONFIG_PARSER_HELP_WIDTH, CONFIG_PARSER_HELP_KEY_INDENT,
      CONFIG_PARSER_HELP_PAR_INDENT);
  }
  
  for (i = 0; i < parser->num_option_groups; ++i) {
    config_parser_option_group_p option_group = &parser->option_groups[i];
    char prefix[strlen(option_group->name)+2];
    sprintf(prefix, "%s-", option_group->name);
    
    fprintf(stream, "\n");
    config_help_print_options(stream, option_group->summary,
      &option_group->options, prefix, CONFIG_PARSER_HELP_WIDTH,
      CONFIG_PARSER_HELP_KEY_INDENT, CONFIG_PARSER_HELP_PAR_INDENT);
  }

  fprintf(stream, "\n");
  config_help_print_options(stream, "Parser options",
    &config_parser_default_options, 0, CONFIG_PARSER_HELP_WIDTH,
    CONFIG_PARSER_HELP_KEY_INDENT, CONFIG_PARSER_HELP_PAR_INDENT);
}

int config_parser_write_man(const char* filename, config_parser_p parser,
    config_p man_options, config_p project_options) {  
  const char* command = strrchr(parser->command, '/');
  command = command ? command+1 : parser->command;
  
  config_man_page_t man_page;
  config_man_init_config(&man_page, command, man_options);
  
  config_man_add_command_summary(&man_page, command, parser->summary);
  config_man_add_synopsis(&man_page, parser->usage);
  config_man_page_section_p description = config_man_add_description(
    &man_page, parser->description);
  config_man_printf(description, config_parser_description);

  if (parser->arguments.num_params)
    config_man_add_arguments(&man_page, "Positional arguments", 0,
      &parser->arguments);
    
  if (parser->options.num_params)
    config_man_add_options(&man_page, "General options", 0,
      &parser->options, 0);
    
  int i;
  for (i = 0; i < parser->num_option_groups; ++i) {
    config_parser_option_group_p option_group = &parser->option_groups[i];
    char prefix[strlen(option_group->name)+2];
    sprintf(prefix, "%s-", option_group->name);    
    
    config_man_add_options(&man_page, option_group->summary,
      option_group->description, &option_group->options, prefix);
  }

  config_man_add_options(&man_page, "Parser options", 0,
    &config_parser_default_options, 0);
  
  config_project_t project;
  config_project_init_config(&project, project_options);
  config_man_add_project_sections(&man_page, &project);
  
  int result = config_man_write(filename, &man_page);
  
  config_man_destroy(&man_page);
  
  if (result) {
    parser->error = CONFIG_PARSER_ERROR_MAN_WRITE;
    strcpy(parser->error_what, filename);
  }

  return parser->error;
}
