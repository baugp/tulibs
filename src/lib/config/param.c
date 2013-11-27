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

#include <math.h>

#include "param.h"

#include "string/string.h"
#include "string/list.h"

const char* config_param_errors[] = {
  "Success",
  "Parameter value type mismatch",
  "Parameter value out of range",
};

const char* config_param_types[] = {
  "string",
  "int",
  "float",
  "enum",
  "bool",
};

void config_param_init(config_param_t* param, const char* key,
    config_param_type_t type, const char* description) {
  string_init_copy(&param->key, key);
  param->type = type;
  param->value = 0;
    
  param->range = 0;
  string_init_copy(&param->description, description);
}

int config_param_init_value_range(config_param_t* param, const char* key,
    config_param_type_t type, const char* value, const char* range,
    const char* description) {
  config_param_init(param, key, type, description);
  string_init_copy(&param->range, range);
  
  return config_param_set_value(param, value);
}

int config_param_init_string(config_param_t* param, const char* key,
    const char* value, const char* description) {
  config_param_init(param, key, config_param_type_string, description);
  return config_param_set_string(param, value);
}

int config_param_init_string_range(config_param_t* param, const char* key,
    const char* value, const char* range, const char* description) {
  config_param_init(param, key, config_param_type_string, description);
  string_init_copy(&param->range, range);
  
  return config_param_set_string(param, value);
}

void config_param_init_int(config_param_t* param, const char* key, int value,
    const char* description) {
  config_param_init(param, key, config_param_type_int, description);
  config_param_set_int(param, value);
}

int config_param_init_int_range(config_param_t* param, const char* key,
    int value, int min_value, int max_value, const char* description) {
  config_param_init(param, key, config_param_type_int, description);
  string_printf(&param->range, "[%d, %d]", min_value, max_value);
  
  return config_param_set_int(param, value);
}

void config_param_init_float(config_param_t* param, const char* key,
    double value, const char* description) {
  config_param_init(param, key, config_param_type_float, description);
  config_param_set_float(param, value);
}

int config_param_init_float_range(config_param_t* param, const char* key,
    double value, double min_value, double max_value, const char*
    description) {
  config_param_init(param, key, config_param_type_float, description);
  string_printf(&param->range, "[%lf, %lf]", min_value, max_value);
  
  return config_param_set_float(param, value);
}

int config_param_init_enum_range(config_param_t* param, const char* key,
    int value, const char** values, const char* description) {
  config_param_init(param, key, config_param_type_enum, description);
  string_list_join(values, &param->range, "|");

  return config_param_set_enum(param, value);
}

void config_param_init_bool(config_param_t* param, const char* key,
    config_param_bool_t value, const char* description) {
  config_param_init(param, key, config_param_type_bool, description);
  string_init_copy(&param->range, "false|true");
  
  config_param_set_bool(param, value);
}

void config_param_init_bool_range(config_param_t* param, const char* key,
    config_param_bool_t value, const char* false_value, const char*
    true_value, const char* description) {
  config_param_init(param, key, config_param_type_bool, description);
  string_printf(&param->range, "%s|%s", false_value, true_value);
  
  config_param_set_bool(param, value);
}

void config_param_init_copy(config_param_t* param, const config_param_t*
    src_param) {
  string_init_copy(&param->key, src_param->key);
  param->type = src_param->type;
  string_init_copy(&param->value, src_param->value);

  string_init_copy(&param->range, src_param->range);
  string_init_copy(&param->description, src_param->description);
}

void config_param_destroy(config_param_t* param) {
  string_destroy(&param->key);
  string_destroy(&param->value);

  string_destroy(&param->range);
  string_destroy(&param->description);  
}

void config_param_copy(config_param_t* dst, const config_param_t* src) {
  string_copy(&dst->key, src->key);
  dst->type = src->type;
  string_copy(&dst->value, src->value);

  string_copy(&dst->range, src->range);
  string_copy(&dst->description, src->description);
}

void config_param_print(FILE* stream, const config_param_t* param) {
  fprintf(stream, "%s = %s\n",
    param->key,
    param->value ? param->value : "<undefined>");
}

int config_param_set_value(config_param_t* param, const char* value) {
  int int_value;
  double float_value;
  
  switch (param->type) {
    case config_param_type_string:
      return config_param_set_string(param, value);
    case config_param_type_int:
      if (string_scanf(value, "%d", &int_value) == 1)
        return config_param_set_int(param, int_value);
      else
        break;
    case config_param_type_float:
      if (string_scanf(value, "%lf", &float_value) == 1)
        return config_param_set_float(param, float_value);
      else
        break;
    default:
      if (!string_empty(param->range)) {
        char** values = 0;
        
        string_split(param->range, &values, "|");
        int i = string_list_find((const char**)values, value);
        string_list_destroy(&values);
        
        if (param->type == config_param_type_enum)
          return config_param_set_enum(param, i);
        else if (param->type == config_param_type_bool)
          return config_param_set_bool(param, i);
      }
  };
  
  return CONFIG_PARAM_ERROR_TYPE;
}

int config_param_set_string(config_param_t* param, const char* value) {
  if (param->type != config_param_type_string)
    return CONFIG_PARAM_ERROR_TYPE;
  
  if (!string_empty(param->range)) {
    char* format = 0;
    char* legal_value = 0;
    
    string_printf(&format, "%%%a", param->range);
    size_t result = string_scanf(value, format, &legal_value);
    string_destroy(&format);
    
    if (result == 1) {
      string_destroy(&param->value);
      param->value = legal_value;
    }
    else
      return CONFIG_PARAM_ERROR_RANGE;
  }
  else
    string_copy(&param->value, value);
    
  return CONFIG_PARAM_ERROR_NONE;
}

const char* config_param_get_string(const config_param_t* param) {
  if (param->type != config_param_type_string)
    return 0;
  
  return param->value;
}

int config_param_set_int(config_param_t* param, int value) {
  if (param->type != config_param_type_int)
    return CONFIG_PARAM_ERROR_TYPE;
    
  if (!string_empty(param->range)) {
    int min_value, max_value;
    unsigned char min_bound, max_bound;
    
    if ((string_scanf(param->range, "%c%d,%d%c", &min_bound,
        &min_value, &max_value, &max_bound) != 4) ||
      ((min_bound == '[') && (value < min_value)) ||
        ((min_bound == '(') && (value <= min_value)) ||
      ((max_bound == ']') && (value > max_value)) ||
        ((min_bound == ')') && (value >= max_value)))
      return CONFIG_PARAM_ERROR_RANGE;    
  }
  string_printf(&param->value, "%d", value);
  
  return CONFIG_PARAM_ERROR_NONE;  
}

int config_param_get_int(const config_param_t* param) {
  if (param->type != config_param_type_int)
    return 0;
  
  int value;
  if (string_scanf(param->value, "%d", &value) != 1)
    return 0;

  return value;
}

int config_param_set_float(config_param_t* param, double value) {
  if (param->type != config_param_type_float)
    return CONFIG_PARAM_ERROR_TYPE;
    
  if (!string_empty(param->range)) {
    double min_value, max_value;
    unsigned char min_bound, max_bound;
    
    if ((string_scanf(param->range, "%c%lf,%lf%c", &min_bound,
        &min_value, &max_value, &max_bound) != 4) ||
      ((min_bound == '[') && (value < min_value)) ||
        ((min_bound == '(') && (value <= min_value)) ||
      ((max_bound == ']') && (value > max_value)) ||
        ((min_bound == ')') && (value >= max_value)))
      return CONFIG_PARAM_ERROR_RANGE;    
  }
  string_printf(&param->value, "%lf", value);
  
  return CONFIG_PARAM_ERROR_NONE;  
}

double config_param_get_float(const config_param_t* param) {
  if (param->type != config_param_type_float)
    return NAN;

  double value;
  if (string_scanf(param->value, "%lf", &value) != 1)
    return NAN;

  return value;
}

int config_param_set_enum(config_param_t* param, int value) {
  if (param->type != config_param_type_enum)
    return CONFIG_PARAM_ERROR_TYPE;
    
  if ((value >= 0) && !string_empty(param->range)) {
    char** values = 0;
    
    size_t num_values = string_split(param->range, &values, "|");
    if (value < num_values)
      string_copy(&param->value, values[value]);
    string_list_destroy(&values);
    
    if (value >= num_values)
      return CONFIG_PARAM_ERROR_RANGE;
  }
  else
    return CONFIG_PARAM_ERROR_RANGE;
  
  return CONFIG_PARAM_ERROR_NONE;
}

int config_param_get_enum(const config_param_t* param) {
  if (param->type != config_param_type_enum)
    return -1;
  
  if (!string_empty(param->range)) {
    char** values = 0;
    
    string_split(param->range, &values, "|");
    ssize_t value = string_list_find((const char**)values, param->value);
    string_list_destroy(&values);

    return value;
  }

  return -1;
}

int config_param_set_bool(config_param_t* param, config_param_bool_t
    value) {
  if (param->type != config_param_type_bool)
    return CONFIG_PARAM_ERROR_TYPE;

  if ((value >= 0) && !string_empty(param->range)) {
    char** values = 0;
    
    size_t num_values = string_split(param->range, &values, "|");
    if (value < num_values)
      string_copy(&param->value, values[value]);
    string_list_destroy(&values);
    
    if (value >= num_values)
      return CONFIG_PARAM_ERROR_RANGE;
  }
  else
    return CONFIG_PARAM_ERROR_RANGE;
  
  return CONFIG_PARAM_ERROR_NONE;
}

config_param_bool_t config_param_get_bool(const config_param_t* param) {
  if (param->type != config_param_type_bool)
    return config_param_false;
  
  if (!string_empty(param->range)) {
    char** values = 0;
    
    string_split(param->range, &values, "|");
    ssize_t value = string_list_find((const char**)values, param->value);
    string_list_destroy(&values);

    if (value == config_param_true)
      return value;
  }
  
  return config_param_false;
}
