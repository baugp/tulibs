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

#include "config.h"

const char* config_errors[] = {
  "success",
  "argument help requested",
  "argument format error",
  "missing argument",
};

void config_init(config_p config) {
  config->params = 0;
  config->num_params = 0;
}

void config_init_default(config_p config, config_p default_config) {
  if (default_config->num_params) {
    config->params = malloc(default_config->num_params*sizeof(param_t));
    memcpy(config->params, default_config->params,
      default_config->num_params*sizeof(param_t));
  }
  else
    config->params = 0;

  config->num_params = default_config->num_params;
}

int config_init_arg(config_p config, int argc, char** argv, const char*
    prefix, const char* args) {
  int result = CONFIG_ERROR_NONE;
  size_t num_args = 0;
  int i;
  char prefix_arg[prefix ? strlen(prefix)+4 : 1];

  if (prefix)
    sprintf(prefix_arg, "--%s-", prefix);
  else
    prefix_arg[0] = 0;

  size_t expected_args = 0;
  const char* arg_space = args;
  while (arg_space) {
    arg_space = strchr(args, ' ');
    if (arg_space != args) {
      args = arg_space+1;
      ++expected_args;
    }
  }
  
  config_init(config);
  for (i = 1; i < argc; ++i) {
    if (!strncmp(argv[i], prefix_arg, strlen(prefix_arg))) {
      char* key_pos = &argv[i][strlen(prefix_arg)];
      char* value_pos = strchr(key_pos, '=');

      if (value_pos) {
        char key[PARAM_KEY_LENGTH];
        char value[PARAM_VALUE_LENGTH];
        strncpy(key, key_pos, value_pos-key_pos);
        key[value_pos-key_pos] = 0;
        strcpy(value, &value_pos[1]);

        param_t param;
        param_init_string(&param, key, value);
        config_set_param(config, &param);
      }
      else
        result = CONFIG_ERROR_ARGUMENT_FORMAT;
    }
    else if (strncmp(argv[i], "--", 2)) {
      ++num_args;

      char* swap_arg = argv[i];
      argv[i] = argv[num_args];
      argv[num_args] = swap_arg;
    }
    else if (!strncmp(argv[i], CONFIG_ARG_HELP, strlen(CONFIG_ARG_HELP)))
      result = CONFIG_ERROR_ARGUMENT_HELP;
  }

  if (!result && (num_args < expected_args))
    return CONFIG_ERROR_ARGUMENT_MISSING;
  else
    return result;
}

void config_destroy(config_p config) {
  if (config->params)
    free(config->params);

  config->params = 0;
  config->num_params = 0;
}

void config_print(FILE* stream, config_p config) {
  int i;

  for (i = 0; i < config->num_params; ++i)
    param_print(stream, &config->params[i]);
}

void config_print_usage(FILE* stream, const char* argv_0, const char* args,
    int error) {
  const char* arg_name = strrchr(argv_0, '/');
  if (arg_name)
    arg_name += 1;
  else
    arg_name = argv_0;

  if (error && (error != CONFIG_ERROR_ARGUMENT_HELP))
    fprintf(stream, "error: %s\n", config_errors[error]);

  fprintf(stream, "usage: %s [OPT1 [OPT2 [...]]]", arg_name);
  if (args)
    fprintf(stream, " %s\n", args);
  else
    fprintf(stream, "\n");
  fprintf(stream, "OPTi is expected to be of format KEYi=VALUEi\n");
}

void config_print_help(FILE* stream, config_p config, const char* prefix) {
  int i;

  for (i = 0; i < config->num_params; ++i)
    param_print_help(stream, &config->params[i], prefix);
}

void config_set(config_p dst_config, config_p src_config) {
  int i;

  for (i = 0; i < src_config->num_params; ++i)
    config_set_param(dst_config, &src_config->params[i]);
}

param_p config_set_param(config_p config, param_p param) {
  param_p config_param = config_get_param(config, param->key);
  
  if (!config_param) {
    config->params = realloc(config->params, (config->num_params+1)*
      sizeof(param_t));
    param_init_string(&config->params[config->num_params], param->key,
      param->value);
    
    ++config->num_params;
    return &config->params[config->num_params-1];
  }
  else {
    param_set_string_value(config_param, param->value);
    return config_param;
  }
}

param_p config_set_param_value(config_p config, const char* key,
    const char* value) {
  param_t config_param;
  
  param_init_string(&config_param, key, value);
  return config_set_param(config, &config_param);
}

param_p config_get_param(config_p config, const char* key) {
  int i;

  for (i = 0; i < config->num_params; ++i)
    if (!strcmp(config->params[i].key, key))
    return &config->params[i];

  return 0;
}

void config_set_string(config_p config, const char* key, const char* value) {
  param_t param;

  param_init_string(&param, key, value);
  config_set_param(config, &param);
}

const char* config_get_string(config_p config, const char* key) {
  const char* value = 0;
  param_p param = config_get_param(config, key);

  if (param)
    value = param_get_string_value(param);

  return value;
}

void config_set_int(config_p config, const char* key, int value) {
  param_t param;

  param_init_int(&param, key, value);
  config_set_param(config, &param);
}

int config_get_int(config_p config, const char* key) {
  int value = 0;
  param_p param = config_get_param(config, key);

  if (param)
    value = param_get_int_value(param);

  return value;
}

void config_set_float(config_p config, const char* key, double value) {
  param_t param;

  param_init_float(&param, key, value);
  config_set_param(config, &param);
}

double config_get_float(config_p config, const char* key) {
  double value = 0.0;
  param_p param = config_get_param(config, key);

  if (param)
    value = param_get_float_value(param);

  return value;
}
