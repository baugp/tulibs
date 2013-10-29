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
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "param.h"

const char* config_param_errors[] = {
  "success",
  "parameter value type mismatch"
  "parameter value out of range"
};

const char* config_param_types[] = {
  "string",
  "int",
  "float",
  "enum",
  "bool"
};

void config_param_init(config_param_p param, const char* key,
    config_param_type_t type, const char* description) {
  strcpy(param->key, key);
  param->type = type;
  param->value[0] = 0;
    
  param->range[0] = 0;
  if (description)
    strcpy(param->description, description);
  else
    param->description[0] = 0;
}

int config_param_init_value_range(config_param_p param, const char* key,
    config_param_type_t type, const char* value, const char* range,
    const char* description) {
  config_param_init(param, key, type, description);
  strcpy(param->range, range);
  
  return config_param_set_value(param, value);
}

int config_param_init_string(config_param_p param, const char* key,
    const char* value, const char* description) {
  config_param_init(param, key, config_param_type_string, description);
  return config_param_set_string(param, value);
}

int config_param_init_string_range(config_param_p param, const char* key,
    const char* value, const char* range, const char* description) {
  config_param_init(param, key, config_param_type_string, description);
  if (range)
    strcpy(param->range, range);
  
  return config_param_set_string(param, value);
}

void config_param_init_int(config_param_p param, const char* key, int value,
    const char* description) {
  config_param_init(param, key, config_param_type_int, description);
  config_param_set_int(param, value);
}

int config_param_init_int_range(config_param_p param, const char* key,
    int value, int min_value, int max_value, const char* description) {
  config_param_init(param, key, config_param_type_int, description);
  sprintf(param->range, "[%d, %d]", min_value, max_value);
  
  return config_param_set_int(param, value);
}

void config_param_init_float(config_param_p param, const char* key,
    double value, const char* description) {
  config_param_init(param, key, config_param_type_float, description);
  config_param_set_float(param, value);
}

int config_param_init_float_range(config_param_p param, const char* key,
    double value, double min_value, double max_value, const char*
    description) {
  config_param_init(param, key, config_param_type_float, description);
  sprintf(param->range, "[%lf, %lf]", min_value, max_value);
  
  return config_param_set_float(param, value);
}

int config_param_init_enum_range(config_param_p param, const char* key,
    int value, const char** values, size_t num_values, const char*
    description) {
  config_param_init(param, key, config_param_type_enum, description);
  
  strcpy(param->range, "{");
  int i;
  for (i = 0; i < num_values; i++) {
    if (i)
      strcat(param->range, ", ");
    strcat(param->range, values[i]);
  }
  strcat(param->range, "}");
  
  return config_param_set_enum(param, value);
}

void config_param_init_bool(config_param_p param, const char* key,
    config_param_bool_t value, const char* description) {
  config_param_init(param, key, config_param_type_bool, description);
  strcpy(param->range, "false|true");
  
  config_param_set_bool(param, value);
}

void config_param_init_bool_range(config_param_p param, const char* key,
    config_param_bool_t value, const char* false_value, const char*
    true_value, const char* description) {
  config_param_init(param, key, config_param_type_bool, description);
  sprintf(param->range, "%s|%s", false_value, true_value);
  
  config_param_set_bool(param, value);
}

void config_param_init_copy(config_param_p param, config_param_p src_param) {
  config_param_copy(param, src_param);
}

void config_param_copy(config_param_p dst_param, config_param_p src_param) {
  dst_param->type = src_param->type;
  strcpy(dst_param->key, src_param->key);
  strcpy(dst_param->value, src_param->value);

  strcpy(dst_param->range, src_param->range);
  strcpy(dst_param->description, src_param->description);
}

void config_param_print(FILE* stream, config_param_p param) {
  fprintf(stream, "%s = %s\n", param->key, param->value);
}

int config_param_set_value(config_param_p param, const char* value) {
  int int_value;
  double float_value;
  
  switch (param->type) {
    case config_param_type_string:
      return config_param_set_string(param, value);
    case config_param_type_int:
      if (sscanf(value, "%d", &int_value) == 1)
        return config_param_set_int(param, int_value);
      else
        break;
    case config_param_type_float:
      if (sscanf(value, "%lf", &float_value) == 1)
        return config_param_set_float(param, float_value);
      else
        break;
    case config_param_type_enum:
      if (param->range[0]) {
        const char* value_start = param->range;
        const char* value_end;
        int i = 0;
        
        while ((value_end = strchr(value_start, '|'))) {
          if (!strncmp(value, value_start, value_end-value_start))
            return config_param_set_enum(param, i);
          
          value_start = value_end+1;
          i++;
        }
        
        if (!strcmp(value, value_start))
          return config_param_set_enum(param, i);
      }
      break;
    case config_param_type_bool:
      if (param->range[0]) {
        const char* value_split = strchr(param->range, '|');
        if ((value_split > param->range) && value_split[1]) {
          if (!strncmp(value, param->range, value_split-param->range))
            return config_param_set_bool(param, config_param_false);
          else if (!strcmp(value, value_split+1))
            return config_param_set_bool(param, config_param_true);
        }
      }
      break;
  };
  
  return CONFIG_PARAM_ERROR_TYPE;
}

int config_param_set_string(config_param_p param, const char* value) {
  if (param->type != config_param_type_string)
    return CONFIG_PARAM_ERROR_TYPE;
  
  if (param->range[0]) {
    char format[strlen(param->range)+2];
    sprintf(format, "%%%s", param->range);
    if (sscanf(value, format, param->value) != 1)
      return CONFIG_PARAM_ERROR_RANGE;
  }
  else
    strcpy(param->value, value);
  
  return CONFIG_PARAM_ERROR_NONE;
}

const char* config_param_get_string(config_param_p param) {
  if (param->type != config_param_type_string)
    return 0;
  
  return param->value;
}

int config_param_set_int(config_param_p param, int value) {
  if (param->type != config_param_type_int)
    return CONFIG_PARAM_ERROR_TYPE;
    
  if (param->range[0]) {
    int min_value, max_value;
    unsigned char min_bound, max_bound;
    
    if ((sscanf(param->range, "%c%d,%d%c", &min_bound, &min_value,
        &max_value, &max_bound) != 4) ||
      ((min_bound == '[') && (value < min_value)) ||
        ((min_bound == '(') && (value <= min_value)) ||
      ((max_bound == ']') && (value > max_value)) ||
        ((min_bound == ')') && (value >= max_value)))
      return CONFIG_PARAM_ERROR_RANGE;    
  }
  sprintf(param->value, "%d", value);
  
  return CONFIG_PARAM_ERROR_NONE;  
}

int config_param_get_int(config_param_p param) {
  if (param->type != config_param_type_int)
    return 0;
  
  int value;
  if (sscanf(param->value, "%d", &value) != 1)
    return 0;

  return value;
}

int config_param_set_float(config_param_p param, double value) {
  if (param->type != config_param_type_float)
    return CONFIG_PARAM_ERROR_TYPE;
    
  if (param->range[0]) {
    double min_value, max_value;
    unsigned char min_bound, max_bound;
    
    if ((sscanf(param->range, "%c%lf,%lf%c", &min_bound, &min_value,
        &max_value, &max_bound) != 4) ||
      ((min_bound == '[') && (value < min_value)) ||
        ((min_bound == '(') && (value <= min_value)) ||
      ((max_bound == ']') && (value > max_value)) ||
        ((min_bound == ')') && (value >= max_value)))
      return CONFIG_PARAM_ERROR_RANGE;    
  }
  sprintf(param->value, "%lf", value);
  
  return CONFIG_PARAM_ERROR_NONE;  
}

double config_param_get_float(config_param_p param) {
  if (param->type != config_param_type_float)
    return NAN;

  double value;
  if (sscanf(param->value, "%lf", &value) != 1)
    return NAN;

  return value;
}

int config_param_set_enum(config_param_p param, int value) {
  if (param->type != config_param_type_enum)
    return CONFIG_PARAM_ERROR_TYPE;
    
  if (param->range[0]) {
    const char* value_start = param->range;
    const char* value_end;
    int i = 0;
    
    while ((value_end = strchr(value_start, '|'))) {
      if (i == value)
        break;
      
      value_start = value_end+1;
      i++;
    }

    if (i != value)
      return CONFIG_PARAM_ERROR_RANGE;

    if (value_end) {
      strncpy(param->value, value_start, value_end-value_start);
      param->value[value_end-value_start] = 0;
    }
    else
      strcpy(param->value, value_start);
  }
  else
    return CONFIG_PARAM_ERROR_RANGE;
  
  return CONFIG_PARAM_ERROR_NONE;
}

int config_param_get_enum(config_param_p param) {
  if (param->type != config_param_type_enum)
    return -1;
  
  if (param->range[0]) {
    const char* value_start = param->range;
    const char* value_end;
    int i = 0;
    
    while ((value_end = strchr(value_start, '|'))) {
      if (!strncmp(param->value, value_start, value_end-value_start))
        return i;
      
      value_start = value_end+1;
      i++;
    }

    if (!strcmp(param->value, value_start))
      return i;
  }

  return -1;
}

int config_param_set_bool(config_param_p param, config_param_bool_t
    value) {
  if (param->type != config_param_type_bool)
    return CONFIG_PARAM_ERROR_TYPE;

  if (param->range[0]) {
    const char* value_split = strchr(param->range, '|');
    
    if ((value_split > param->range) && value_split[1]) {
      if (value == config_param_false)
        strncpy(param->value, param->range, value_split-param->range);
      else if (value == config_param_true)
        strcpy(param->value, value_split+1);
      else
        return CONFIG_PARAM_ERROR_RANGE;
    }
    else
      return CONFIG_PARAM_ERROR_RANGE;
  }
  else
    return CONFIG_PARAM_ERROR_RANGE;
  
  return CONFIG_PARAM_ERROR_NONE;
}

config_param_bool_t config_param_get_bool(config_param_p param) {
  if (param->type != config_param_type_bool)
    return config_param_false;
  
  if (param->range[0]) {
    const char* value_split = strchr(param->range, '|');
    
    if ((value_split > param->range) && value_split[1]) {
      if (!strncmp(param->value, param->range, value_split-param->range))
        return config_param_false;
      else if (!strcmp(param->value, value_split+1))
        return config_param_true;
    }
  }
  
  return config_param_false;
}
