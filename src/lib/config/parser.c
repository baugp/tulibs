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

#include "parser.h"
#include "help.h"
#include "man.h"
#include "project.h"
#include "file.h"

#include "string/string.h"
#include "string/list.h"

const char* config_parser_errors[] = {
  "Success",
  "Failed to write manual page",
  "Failed to read configuration",
  "Failed to write configuration",
  "Invalid configuration file section",
  "Unexpected argument",
  "Missing argument",
  "Invalid argument format",
  "Invalid argument key",
  "Invalid argument value",
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
  {CONFIG_PARSER_PARAMETER_DEBUG,
    config_param_type_bool,
    "false",
    "false|true",
    "Enable debugging output for runtime errors"},
};

const config_t config_parser_default_options = {
  config_parser_default_params,
  sizeof(config_parser_default_params)/sizeof(config_param_t),
};

const char* config_parser_usage = "[OPT1 [OPT2 [...]]]";

const char* config_parser_description =
  "Positional arguments must be supplied on the command line in the "
  "expected order. Non-positional arguments OPTi should precede any "
  "positional arguments and are required to be of the format --KEYi[=VALUEi].";

void config_parser_option_group_init(config_parser_option_group_t*
  option_group, const char* name, const config_t* options, const char*
  summary, const char* description);
void config_parser_option_group_destroy(config_parser_option_group_t*
  option_group);
  
void config_parser_init(config_parser_t* parser, const config_t* arguments,
    const config_t* options, const char* summary, const char* description) {
  config_parser_init_default(parser, summary, description);
  
  if (arguments)
    config_init_copy(&parser->arguments, arguments);
  else
    config_init(&parser->arguments);

  if (options)
    config_merge(&parser->options, options);
}

void config_parser_init_default(config_parser_t* parser, const char* summary,
    const char* description) {
  config_init(&parser->arguments);
  config_init(&parser->options);
  
  parser->option_groups = 0;
  parser->num_option_groups = 0;
  
  string_init_copy(&parser->summary, summary);
  string_init_copy(&parser->description, description);
  
  parser->command = 0;
  parser->usage = 0;

  error_init(&parser->error, config_parser_errors);
}

void config_parser_destroy(config_parser_t* parser) {
  config_destroy(&parser->arguments);
  config_destroy(&parser->options);
  
  if (parser->num_option_groups) {
    int i;
  
    for (i = 0; i < parser->num_option_groups; ++i)
      config_parser_option_group_destroy(&parser->option_groups[i]);
    
    free(parser->option_groups);
    parser->option_groups = 0;
    parser->num_option_groups = 0;
  }
  
  string_destroy(&parser->summary);
  string_destroy(&parser->description);

  string_destroy(&parser->command);
  string_destroy(&parser->usage);
  
  error_destroy(&parser->error);
}

config_parser_option_group_t* config_parser_add_option_group(
    config_parser_t* parser, const char* name, const config_t* options,
    const char* summary, const char* description) {
  config_parser_option_group_t* option_group = 0;
  
  parser->option_groups = realloc(parser->option_groups,
    (parser->num_option_groups+1)*sizeof(config_parser_option_group_t));
  option_group = &parser->option_groups[parser->num_option_groups];
  ++parser->num_option_groups;

  config_parser_option_group_init(option_group, name, options, summary,
    description);
  
  return option_group;
}

config_parser_option_group_t* config_parser_get_option_group(
    const config_parser_t* parser, const char* name) {
  int i;
  
  for (i = 0; i < parser->num_option_groups; ++i)
    if (string_equal(parser->option_groups[i].name, name))
      return &parser->option_groups[i];
  
  return 0;
}

int config_parser_parse(config_parser_t* parser, int argc, char** argv,
    config_parser_exit_t parser_exit) {
  int i;
  
  string_copy(&parser->command, argv[0]);
  const char* command = string_rfind(parser->command, "/");
  command = command ? command+1 : parser->command;

  char** usage_argv;
  string_list_init(&usage_argv, 2+parser->arguments.num_params);
  string_copy(&usage_argv[0], command);
  string_copy(&usage_argv[1], config_parser_usage);
  for (i = 0; i < parser->arguments.num_params; ++i) {
    config_param_t* param = &parser->arguments.params[i];
    string_printf(&usage_argv[2+i], "%s%s%s",
      !string_empty(param->value) ? "[" : "",
      param->key,
      !string_empty(param->value) ? "]" : "");
  }
  string_list_join((const char**)usage_argv, &parser->usage, " ");
  string_list_destroy(&usage_argv);

  error_clear(&parser->error);
  
  config_t parser_options, file_options, man_options, project_options;
  config_init_copy(&parser_options, &config_parser_default_options);
  config_init_copy(&file_options, &config_file_default_options);
  config_init_copy(&man_options, &config_man_default_options);
  config_init_copy(&project_options, &config_project_default_options);
  
  config_param_t* options[argc-1];
  const char* option_argv[argc-1];
  char* key = 0;
  size_t num_options = 0;
  
  for (i = 1; i < argc; ++i) {
    if (string_starts_with(argv[i], "--")) {
      config_param_t* param = 0;

      string_copy(&key, &argv[i][2]);
      const char* value = string_find(key, "=");
      if (value) {
        key[value-key] = 0;
        ++value;
      }
      
      if (value && !value[0]) {
        error_setf(&parser->error, CONFIG_PARSER_ERROR_ARGUMENT_FORMAT,
          argv[i]);
        break;
      }
      
      if ((param = config_get_param(&parser_options, key))) {
        int result;
        if (value)
          result = config_param_set_value(param, value);
        else
          result = config_param_set_bool(param, config_param_true);
        
        if (result) {
          if (value)
            error_setf(&parser->error, CONFIG_PARSER_ERROR_ARGUMENT_VALUE,
              value);
          else
            error_setf(&parser->error, CONFIG_PARSER_ERROR_ARGUMENT_FORMAT,
              argv[i]);
          break;
        }
      }
      else {
        param = config_get_param(&parser->options, key);
        
        if (!param && string_starts_with_ignore_case(key,
            CONFIG_FILE_PARSER_OPTION_GROUP"-"))
          param = config_get_param(&file_options,
            &key[string_length(CONFIG_FILE_PARSER_OPTION_GROUP)+1]);
          
        if (!param && string_starts_with_ignore_case(key,
            CONFIG_MAN_PARSER_OPTION_GROUP"-"))
          param = config_get_param(&man_options,
            &key[string_length(CONFIG_MAN_PARSER_OPTION_GROUP)+1]);
          
        if (!param && string_starts_with_ignore_case(key,
            CONFIG_PROJECT_PARSER_OPTION_GROUP"-"))
          param = config_get_param(&project_options,
            &key[string_length(CONFIG_PROJECT_PARSER_OPTION_GROUP)+1]);
          
        if (param) {
          int result;
          if (value)
            result = config_param_set_value(param, value);
          else
            result = config_param_set_bool(param, config_param_true);
          
          if (result) {
            if (value)
              error_setf(&parser->error, CONFIG_PARSER_ERROR_ARGUMENT_VALUE,
                value);
            else
              error_setf(&parser->error, CONFIG_PARSER_ERROR_ARGUMENT_FORMAT,
                argv[i]);
            break;
          }
        }
        else {
          int j;
          for (j = 0; j < parser->num_option_groups; ++j) {
            config_parser_option_group_t* option_group =
              &parser->option_groups[j];
            size_t prefix_length = string_length(option_group->name);
            
            if (string_starts_with_ignore_case(key, option_group->name) &&
                key[prefix_length] == '-')
              param = config_get_param(&option_group->options,
                &key[prefix_length+1]);
            if (param)
              break;
          }

          if (param) {
            options[num_options] = param;
            option_argv[num_options] = argv[i];
            ++num_options;
          }          
        }
        
        if (!param) {
          error_setf(&parser->error, CONFIG_PARSER_ERROR_ARGUMENT_KEY, key);        
          break;
        }
      }
    }
    else
      break;
  }
  string_destroy(&key);
  
  if (!parser->error.code && (config_get_bool(&parser_options,
      CONFIG_PARSER_PARAMETER_HELP))) {
    config_parser_print_help(stdout, parser);    
    exit(EXIT_SUCCESS);
  }
  
  if (config_get_bool(&parser_options, CONFIG_PARSER_PARAMETER_DEBUG))
    error_debug = 1;

  const char* man_output = config_get_string(&parser_options,
    CONFIG_PARSER_PARAMETER_MAN_OUTPUT);
  if (!parser->error.code && man_output && !config_parser_write_man(
      man_output, parser, &man_options, &project_options))
    exit(EXIT_SUCCESS);
  
  const char* config_file = config_get_string(&parser_options,
    CONFIG_PARSER_PARAMETER_FILE);
  if (!parser->error.code && config_file)
    config_parser_read_file(config_file, parser, &file_options);
  
  if (!parser->error.code) {
    int j;
    for (j = 0 ; j < num_options; ++j) {
      const char* value = string_find(option_argv[j], "=");
      int result;
      
      if (!string_empty(value+1))
        result = config_param_set_value(options[j], value+1);
      else
        result = config_param_set_bool(options[j], config_param_true);
      
      if (result) {
        if (!string_empty(value))
          error_setf(&parser->error, CONFIG_PARSER_ERROR_ARGUMENT_VALUE,
            value+1);
        else
          error_setf(&parser->error, CONFIG_PARSER_ERROR_ARGUMENT_FORMAT,
            option_argv[j]);
        break;
      }
    }
  }  
    
  int j = 0;
  if (!parser->error.code) {
    for (j = 0 ; i < argc; ++i, ++j) {
      if (j < parser->arguments.num_params) {
        config_param_t* param = &parser->arguments.params[j];
        int result = config_param_set_value(param, argv[i]);
        
        if (result) {
          error_setf(&parser->error, CONFIG_PARSER_ERROR_ARGUMENT_VALUE,
            argv[i]);
          break;
        }
      }
      else {
        error_setf(&parser->error, CONFIG_PARSER_ERROR_ARGUMENT, argv[i]);
        break;
      }
    }
  }

  const char* file_output = config_get_string(&parser_options,
    CONFIG_PARSER_PARAMETER_FILE_OUTPUT);
  if (!parser->error.code && file_output && !config_parser_write_file(
      file_output, parser, &file_options))
    exit(EXIT_SUCCESS);
    
  if (!parser->error.code && (j < parser->arguments.num_params) && 
      !parser->arguments.params[j].value)
    error_setf(&parser->error, CONFIG_PARSER_ERROR_ARGUMENT_MISSING,
      parser->arguments.params[j].key);
    
  if (parser->error.code && (parser_exit & config_parser_exit_error)) {
    config_parser_print_usage(stderr, parser);
    exit(EXIT_FAILURE);
  }

  config_destroy(&parser_options);
  config_destroy(&file_options);
  config_destroy(&man_options);
  config_destroy(&project_options);
  
  return error_get(&parser->error);
}

int config_parser_read_file(const char* filename, config_parser_t* parser,
    const config_t* file_options) {
  config_file_t file;
  config_file_init_config(&file, file_options);

  error_clear(&parser->error);
  
  if (config_file_read(filename, &file)) {
    error_blame(&parser->error, &file.error, CONFIG_PARSER_ERROR_FILE_READ);
    return error_get(&parser->error);
  }

  int i, j;
  for (i = 0; i < file.num_sections; ++i) {
    config_file_section_t* section = &file.sections[i];
    config_t* options;
    
    if (section->name) {
      config_parser_option_group_t* option_group =
        config_parser_get_option_group(parser, section->name);
      options = option_group ? &option_group->options : 0;
    }
    else
      options = &parser->options;
  
    if (!options) {
      error_setf(&parser->error, CONFIG_PARSER_ERROR_FILE_SECTION,
        section->name);
      return error_get(&parser->error);
    }
    
    for (j = 0; j < section->num_vars; ++j) {
      config_file_var_t* var = &section->vars[j];
      config_param_t* param = config_get_param(options, var->name);
      
      if (!param) {
        error_setf(&parser->error, CONFIG_PARSER_ERROR_ARGUMENT_KEY,
          var->name);
        return error_get(&parser->error);
      }
      if (config_param_set_value(param, var->value)) {
        error_setf(&parser->error, CONFIG_PARSER_ERROR_ARGUMENT_VALUE,
          var->value);
        return error_get(&parser->error);
      }
    }
  }
  
  return error_get(&parser->error);
}

int config_parser_write_file(const char* filename, config_parser_t* parser,
    const config_t* file_options) {
  config_file_t file;
  config_file_init_config(&file, file_options);

  config_file_add_config(&file, 0, 0, &parser->options);
  
  error_clear(&parser->error);
  
  int i;
  for (i = 0; i < parser->num_option_groups; ++i) {
    config_parser_option_group_t* option_group = &parser->option_groups[i];
    config_file_add_config(&file, option_group->name, option_group->summary,
      &option_group->options);
  }
  
  if (config_file_write(filename, &file))
    error_blame(&parser->error, &file.error, CONFIG_PARSER_ERROR_FILE_WRITE);
  
  config_file_destroy(&file);
  
  return error_get(&parser->error);
}

void config_parser_print_usage(FILE* stream, const config_parser_t* parser) {
  if (parser->error.code) {
    error_print(stream, &parser->error);
    fprintf(stream, "\n");
  }
  
  char* usage = 0;
  string_printf(&usage, "Usage: %s\n", parser->usage);
  config_help_print(stream, usage, CONFIG_PARSER_HELP_WIDTH,
    CONFIG_PARSER_HELP_HANG_INDENT, 0, 0);
  string_destroy(&usage);
    
  if (!parser->error.code && parser->description) {
    config_help_print(stream, parser->description,
      CONFIG_PARSER_HELP_WIDTH, 0, 0, 0);
    fprintf(stream, "\n");
  }
    
  fprintf(stream, "\n");
  config_help_print(stream, config_parser_description,
    CONFIG_PARSER_HELP_WIDTH, 0, 0, 0);
  fprintf(stream, "\n");
  
  if (parser->error.code) {
    fprintf(stream, "\n");
    config_help_print(stream, 
      "To see a summary of supported options and mandatory arguments, "
      "simply type\n",
      CONFIG_PARSER_HELP_WIDTH, 0, 0, 0);
    fprintf(stream, "> %s --%s\n",
      parser->command, CONFIG_PARSER_PARAMETER_HELP);
    config_help_print(stream, 
      "into your terminal or check out the documentation.\n",
      CONFIG_PARSER_HELP_WIDTH, 0, 0, 0);
  }
}

void config_parser_print_help(FILE* stream, const config_parser_t* parser) {
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
    config_parser_option_group_t* option_group = &parser->option_groups[i];
    char* prefix = 0;
    
    fprintf(stream, "\n");
    string_printf(&prefix, "%s-", option_group->name);
    config_help_print_options(stream, option_group->summary,
      &option_group->options, prefix, CONFIG_PARSER_HELP_WIDTH,
      CONFIG_PARSER_HELP_KEY_INDENT, CONFIG_PARSER_HELP_PAR_INDENT);
    string_destroy(&prefix);
  }

  fprintf(stream, "\n");
  config_help_print_options(stream, "Parser options",
    &config_parser_default_options, 0, CONFIG_PARSER_HELP_WIDTH,
    CONFIG_PARSER_HELP_KEY_INDENT, CONFIG_PARSER_HELP_PAR_INDENT);
}

int config_parser_write_man(const char* filename, config_parser_t* parser,
    const config_t* man_options, const config_t* project_options) {  
  const char* command = string_rfind(parser->command, "/");
  command = command ? command+1 : parser->command;

  error_clear(&parser->error);
  
  config_man_page_t man_page;
  config_man_init_config(&man_page, command, man_options);
  
  config_man_add_command_summary(&man_page, command, parser->summary);
  config_man_add_synopsis(&man_page, parser->usage);
  config_man_page_section_t* description = config_man_add_description(
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
    config_parser_option_group_t* option_group = &parser->option_groups[i];
    char* prefix = 0;
    
    string_printf(&prefix, "%s-", option_group->name);    
    config_man_add_options(&man_page, option_group->summary,
      option_group->description, &option_group->options, prefix);
    string_destroy(&prefix);
  }

  config_man_add_options(&man_page, "Parser options", 0,
    &config_parser_default_options, 0);
  
  config_project_t project;
  config_project_init_config(&project, project_options);
  config_man_add_project_sections(&man_page, &project);
  
  if (config_man_write(filename, &man_page))
    error_blame(&parser->error, &man_page.error,
      CONFIG_PARSER_ERROR_MAN_WRITE);

  config_man_destroy(&man_page);
  
  return error_get(&parser->error);
}

void config_parser_option_group_init(config_parser_option_group_t*
    option_group, const char* name, const config_t* options, const char*
    summary, const char* description) {
  string_init_copy(&option_group->name, name);
  if (options)
    config_init_copy(&option_group->options, options);
  else
    config_init(&option_group->options);
  
  string_init_copy(&option_group->summary, summary);
  string_init_copy(&option_group->description, description);
}

void config_parser_option_group_destroy(config_parser_option_group_t*
    option_group) {
  string_destroy(&option_group->name);
  config_destroy(&option_group->options);
  
  string_destroy(&option_group->summary);
  string_destroy(&option_group->description);
}
