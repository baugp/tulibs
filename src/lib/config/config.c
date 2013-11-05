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
#include <math.h>

#include "config.h"

const char* config_errors[] = {
  "Success",
  "Parameter key error",
  "Parameter value error"
};

void config_init(config_p config) {
  config->params = 0;
  config->num_params = 0;
}

void config_init_copy(config_p config, config_p src_config) {
  config_init(config);
  config_copy(config, src_config);
}

void config_destroy(config_p config) {
  if (config->params)
    free(config->params);

  config->params = 0;
  config->num_params = 0;
}

void config_copy(config_p dst_config, config_p src_config) {
  if (src_config->num_params) {
    dst_config->params = realloc(dst_config->params,
      src_config->num_params*sizeof(config_param_t));
    memcpy(dst_config->params, src_config->params,
      src_config->num_params*sizeof(config_param_t));
  }
  else if (dst_config->num_params)
    free(dst_config->params);
  
  dst_config->num_params = src_config->num_params;
}

void config_merge(config_p dst_config, config_p src_config) {
  int i;
  
  for (i = 0; i < src_config->num_params; ++i)
    config_set_param(dst_config, &src_config->params[i]);
}

void config_print(FILE* stream, config_p config) {
  int i;

  for (i = 0; i < config->num_params; ++i)
    config_param_print(stream, &config->params[i]);
}

int config_set(config_p dst_config, config_p src_config) {
  int i, result;

  for (i = 0; i < src_config->num_params; ++i) {
    config_param_p src_param = &src_config->params[i];
    if ((result = config_set_string(dst_config, src_param->key,
        src_param->value)))
      return result;
  }
  
  return CONFIG_ERROR_NONE;
}

config_param_p config_set_param(config_p config, config_param_p param) {
  config_param_p config_param = config_get_param(config, param->key);

  if (!config_param) {
    config->params = realloc(config->params, (config->num_params+1)*
      sizeof(config_param_t));
    
    config_param = &config->params[config->num_params];
    config_param_init_copy(config_param, param);
  
    ++config->num_params;
  }
  else
    config_param_copy(config_param, param);
  
  return config_param;
}

config_param_p config_get_param(config_p config, const char* key) {
  int i;

  for (i = 0; i < config->num_params; ++i)
    if (!strcmp(config->params[i].key, key))
      return &config->params[i];

  return 0;
}

int config_set_string(config_p config, const char* key, const char* value) {
  config_param_p param = config_get_param(config, key);

  if (param) {
    if (config_param_set_string(param, value))
      return CONFIG_ERROR_PARAM_VALUE;
  }
  else
    return CONFIG_ERROR_PARAM_KEY;
  
  return CONFIG_ERROR_NONE;
}

const char* config_get_string(config_p config, const char* key) {
  config_param_p param = config_get_param(config, key);

  if (param)
    return config_param_get_string(param);
  else
    return 0;
}

int config_set_int(config_p config, const char* key, int value) {
  config_param_p param = config_get_param(config, key);

  if (param) {
    if (config_param_set_int(param, value))
      return CONFIG_ERROR_PARAM_VALUE;
  }
  else
    return CONFIG_ERROR_PARAM_KEY;
  
  return CONFIG_ERROR_NONE;
}

int config_get_int(config_p config, const char* key) {
  config_param_p param = config_get_param(config, key);

  if (param)
    return config_param_get_int(param);
  else
    return 0;
}

int config_set_float(config_p config, const char* key, double value) {
  config_param_p param = config_get_param(config, key);

  if (param) {
    if (config_param_set_float(param, value))
      return CONFIG_ERROR_PARAM_VALUE;
  }
  else
    return CONFIG_ERROR_PARAM_KEY;
  
  return CONFIG_ERROR_NONE;
}

double config_get_float(config_p config, const char* key) {
  config_param_p param = config_get_param(config, key);

  if (param)
    return config_param_get_float(param);
  else
    return NAN;
}

int config_set_enum(config_p config, const char* key, int value) {
  config_param_p param = config_get_param(config, key);

  if (param) {
    if (config_param_set_enum(param, value))
      return CONFIG_ERROR_PARAM_VALUE;
  }
  else
    return CONFIG_ERROR_PARAM_KEY;
  
  return CONFIG_ERROR_NONE;
}

int config_get_enum(config_p config, const char* key) {
  config_param_p param = config_get_param(config, key);

  if (param)
    return config_param_get_enum(param);
  else
    return -1;
}

int config_set_bool(config_p config, const char* key, config_param_bool_t
    value) {
  config_param_p param = config_get_param(config, key);

  if (param) {
    if (config_param_set_bool(param, value))
      return CONFIG_ERROR_PARAM_VALUE;
  }
  else
    return CONFIG_ERROR_PARAM_KEY;
  
  return CONFIG_ERROR_NONE;
}

config_param_bool_t config_get_bool(config_p config, const char* key) {
  config_param_p param = config_get_param(config, key);

  if (param)
    return config_param_get_bool(param);
  else
    return config_param_false;
}
