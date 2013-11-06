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

const char* config_parser_errors[] = {
  "Success",
  "Failed to write manual page",
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
};

config_t config_parser_default_options = {
  config_parser_default_params,
  sizeof(config_parser_default_params)/sizeof(config_param_t),
};

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
  config_init_copy(&parser->options, &config_parser_default_options);
  
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
  
  config_parser_add_option_group(parser, &config_man_default_options,
    CONFIG_MAN_ARG_PREFIX, "Manual page generating options",
    "These options control the generation of manual pages for parser "
    "configurations. They are usually hidden from the textual help and "
    "manual page content of these parser.");
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
    config_parser_p parser, config_p options, const char* prefix,
    const char* summary, const char* description) {
  config_parser_option_group_p option_group = 0;
  
  parser->option_groups = realloc(parser->option_groups,
    (parser->num_option_groups+1)*sizeof(config_parser_option_group_t));
  option_group = &parser->option_groups[parser->num_option_groups];
  ++parser->num_option_groups;
  
  if (options)
    config_init_copy(&option_group->options, options);
  else
    config_init(&option_group->options);
  
  if (prefix)
    strcpy(option_group->prefix, prefix);
  else
    option_group->prefix[0] = 0;
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

  for (i = 1; i < argc; ++i) {
    if ((argv[i][0] == '-') && (argv[i][1] == '-')) {
      char key[sizeof(((config_param_p)0)->key)+
        sizeof(((config_parser_option_group_p)0)->prefix)];
      char value[sizeof(((config_param_p)0)->value)];
      config_param_p param = 0;

      char* key_start = &argv[i][2];
      char* value_start = strchr(key_start, '=');
      if (value_start)
        ++value_start;
      
      if ((value_start > key_start) && key_start[0] && value_start[0]) {
        strncpy(key, key_start, value_start-key_start-1);
        key[value_start-key_start-1] = 0;
        strcpy(value, value_start);
      }
      else if (key_start[0] && !value_start) {
        strcpy(key, key_start);
        value[0] = 0;
      }
      else {
        parser->error = CONFIG_PARSER_ERROR_ARGUMENT_FORMAT;
        strcpy(parser->error_what, argv[i]);
        break;
      }

      param = config_get_param(&parser->options, key);      
      if (!param) {
        int j;
        for (j = 0; j < parser->num_option_groups; ++j) {
          config_parser_option_group_p option_group =
            &parser->option_groups[j];
          size_t prefix_length = strlen(option_group->prefix);
          
          if (!strncmp(key, option_group->prefix, prefix_length))
            param = config_get_param(&option_group->options,
              &key[prefix_length]);
          if (param)
            break;
        }
      }
      
      if (param) {
        int result;
        if (value[0])
          result = config_param_set_value(param, value);
        else
          result = config_param_set_bool(param, config_param_true);
        
        if (result) {
          parser->error = CONFIG_PARSER_ERROR_ARGUMENT_VALUE;
          strcpy(parser->error_what, value);
          break;
        }
      }
      else {
        parser->error = CONFIG_PARSER_ERROR_ARGUMENT_KEY;
        strcpy(parser->error_what, key);        
        break;
      }
    }
    else
      break;
  }
  
  const char* man_output = config_get_string(
    &parser->option_groups[0].options, CONFIG_MAN_PARAMETER_OUTPUT);
  if (config_get_bool(&parser->options, CONFIG_PARSER_PARAMETER_HELP)) {
    if (parser_exit & config_parser_exit_help) {
      config_parser_print_help(stdout, parser);    
      exit(EXIT_SUCCESS);
    }
  }
  else if (man_output[0]) {
    if (parser_exit & config_parser_exit_help) {
      if (config_parser_write_man(man_output, parser)) {
        parser->error = CONFIG_PARSER_ERROR_WRITE_MAN;
        strcpy(parser->error_what, man_output);
      }
      else
        exit(EXIT_SUCCESS);
    }
  }
  else if (!parser->error) {
    int j;
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

    if (!parser->error && (j < parser->arguments.num_params) && 
        !parser->arguments.params[j].value[0]) {
      parser->error = CONFIG_PARSER_ERROR_ARGUMENT_MISSING;
      strcpy(parser->error_what, parser->arguments.params[j].key);
    }
  }

  if (parser->error && (parser_exit & config_parser_exit_error)) {
    config_parser_print_usage(stderr, parser);
    exit(EXIT_FAILURE);
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
  config_help_print(stream, 
    "Positional arguments must be supplied on the command line in the "
    "expected order. Non-positional arguments OPTi should precede any "
    "positional arguments and are required to be of the format "
    "--KEYi[=VALUEi].\n",
    CONFIG_PARSER_HELP_WIDTH, 0, 0, 0);
  
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
  
  for (i = 1; i < parser->num_option_groups; ++i) {
    config_parser_option_group_p option_group = &parser->option_groups[i];
    fprintf(stream, "\n");
    config_help_print_options(stream, option_group->summary,
      &option_group->options, option_group->prefix, CONFIG_PARSER_HELP_WIDTH,
      CONFIG_PARSER_HELP_KEY_INDENT, CONFIG_PARSER_HELP_PAR_INDENT);
  }
}

int config_parser_write_man(const char* filename, config_parser_p parser) {
  file_t file;
  
  file_init_name(&file, filename);
  if (strcmp(filename, "-"))
    file_open(&file, file_mode_write);
  else
    file_open_stream(&file, stdout, file_mode_write);

  if (!file.handle)
    return CONFIG_PARSER_ERROR_WRITE_MAN;
  
  config_parser_option_group_p man_option_group = &parser->option_groups[0];
  const char* command = strrchr(parser->command, '/');
  command = command ? command+1 : parser->command;
  int i;
  
  config_man_write_header(&file, command,
    config_get_int(&man_option_group->options,
      CONFIG_MAN_PARAMETER_SECTION),
    config_get_string(&man_option_group->options,
      CONFIG_MAN_PARAMETER_TITLE),
    command, parser->summary);
  
  config_man_write_synopsis(&file, parser->usage);
  
  char description[strlen(parser->description)+512];
  strcpy(description, parser->description);
  strcat(description, "\n\n"
    "Positional arguments must be supplied on the command line in the "
    "expected order. Non-positional arguments OPTi should precede any "
    "positional arguments and are required to be of the format "
    "--KEYi[=VALUEi].");
  config_man_write_description(&file, description);

  if (parser->arguments.num_params)
    config_man_write_arguments(&file, "Positional arguments", 0,
      &parser->arguments);

  if (parser->options.num_params)
    config_man_write_options(&file, "General options", 0, &parser->options, 0);

  for (i = 1; i < parser->num_option_groups; ++i) {
    config_parser_option_group_p option_group = &parser->option_groups[i];
    config_man_write_options(&file, option_group->summary,
      option_group->description, &option_group->options, option_group->prefix);
  }

  const char* project_name = config_get_string(&man_option_group->options,
    CONFIG_MAN_PARAMETER_PROJECT_NAME);
  const char* project_version = config_get_string(&man_option_group->options,
    CONFIG_MAN_PARAMETER_PROJECT_VERSION);
  const char* project_authors = config_get_string(&man_option_group->options,
    CONFIG_MAN_PARAMETER_PROJECT_AUTHORS);
  const char* project_contact = config_get_string(&man_option_group->options,
    CONFIG_MAN_PARAMETER_PROJECT_CONTACT);
  const char* project_home = config_get_string(&man_option_group->options,
    CONFIG_MAN_PARAMETER_PROJECT_HOME);
  const char* project_license = config_get_string(&man_option_group->options,
    CONFIG_MAN_PARAMETER_PROJECT_LICENSE);
  
  if (project_authors[0])
    config_man_write_authors(&file, project_authors);
  if (project_contact[0])
    config_man_write_bugs(&file, project_contact);
  if (project_name[0] && project_license[0])
    config_man_write_copyright(&file, project_name, project_license);
  if (project_name[0])
    config_man_write_colophon(&file, project_name, project_version,
      project_home);

  int error = file_error(&file);
  file_close(&file);
  
  if (error)
    return CONFIG_PARSER_ERROR_WRITE_MAN;
  else
    return CONFIG_PARSER_ERROR_NONE;
}
