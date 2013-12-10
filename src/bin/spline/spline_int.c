/***************************************************************************
 *   Copyright (C) 2004 by Ralf Kaestner                                   *
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

#include <stdio.h>

#include "config/parser.h"
#include "spline/spline.h"
#include "string/string.h"
#include "file/file.h"

#define SPLINE_INT_PARAMETER_FILE             "FILE"

#define SPLINE_INT_PARSER_OPTION_GROUP        "spline-int"
#define SPLINE_INT_PARAMETER_TYPE             "type"
#define SPLINE_INT_PARAMETER_OUTPUT           "output"
#define SPLINE_INT_PARAMETER_Y1_0             "y1_0"
#define SPLINE_INT_PARAMETER_Y1_N             "y1_n"
#define SPLINE_INT_PARAMETER_Y2_0             "y2_0"
#define SPLINE_INT_PARAMETER_Y2_N             "y2_n"
#define SPLINE_INT_PARAMETER_R_0              "r_0"
#define SPLINE_INT_PARAMETER_R_N              "r_n"

typedef enum {
  spline_type_y1,
  spline_type_y2,
  spline_type_y1_y2,
  spline_type_natural,
  spline_type_clamped,
  spline_type_periodic,
  spline_type_not_a_knot,
} spline_type_t;

config_param_t spline_int_default_arguments_params[] = {
  {SPLINE_INT_PARAMETER_FILE,
    config_param_type_string,
    "",
    "",
    "Read spline interpolation points from the specified input file or '-' "
    "for stdin"},
};

const config_default_t spline_int_default_arguments = {
  spline_int_default_arguments_params,
  sizeof(spline_int_default_arguments_params)/sizeof(config_param_t),
};

config_param_t spline_int_default_options_params[] = {
  {SPLINE_INT_PARAMETER_TYPE,
    config_param_type_enum,
    "natural",
    "y1|y2|y1-y2|natural|clamped|periodic|not-a-knot",
    "The type of boundary conditions for the interpolating spline, which may "
    "be 'y1' for known first derivatives, 'y2' for known second derivatives, "
    "'y1-y2' for known both first and second derivatives, 'clamped' for zero "
    "first derivatives, 'natural' for zero second derivatives, 'periodic' for "
    "equal first and second derivatives, or 'not-a-knot' for no additional "
    "boundary conditions"},
  {SPLINE_INT_PARAMETER_Y1_0,
    config_param_type_float,
    "0.0",
    "(-inf, inf)",
    "The first derivative at the first spline knot if the requested spline "
    "type is 'y1' or 'y1-y2'"},
  {SPLINE_INT_PARAMETER_Y1_N,
    config_param_type_float,
    "0.0",
    "(-inf, inf)",
    "The first derivative at the last spline knot if the requested spline "
    "type is 'y1' or 'y1-y2'"},
  {SPLINE_INT_PARAMETER_Y2_0,
    config_param_type_float,
    "0.0",
    "(-inf, inf)",
    "The second derivative at the first spline knot if the requested spline "
    "type is 'y2' or 'y1-y2'"},
  {SPLINE_INT_PARAMETER_Y2_N,
    config_param_type_float,
    "0.0",
    "(-inf, inf)",
    "The second derivative at the last spline knot if the requested spline "
    "type is 'y2' or 'y1-y2'"},
  {SPLINE_INT_PARAMETER_R_0,
    config_param_type_float,
    "0.5",
    "(0.0, 1.0)",
    "The ratio defining the relative location of the first intermediate knot "
    "in the original first spline segment with respect to the first knot if "
    "the requested spline type is 'y1-y2'"},
  {SPLINE_INT_PARAMETER_R_N,
    config_param_type_float,
    "0.5",
    "(0.0, 1.0)",
    "The ratio defining the relative location of the last intermediate knot "
    "in the original last spline segment with respect to the last knot if "
    "the requested spline type is 'y1-y2'"},
  {SPLINE_INT_PARAMETER_OUTPUT,
    config_param_type_string,
    "-",
    "",
    "Write interpolating spline to the specified output file or '-' for "
    "stdout"},
};

const config_default_t spline_int_default_options = {
  spline_int_default_options_params,
  sizeof(spline_int_default_options_params)/sizeof(config_param_t),
};

int main(int argc, char **argv) {
  config_parser_t parser;
  spline_t spline;
  file_t input_file;

  config_parser_init_default(&parser, &spline_int_default_arguments, 0,
    "Cubic spline interpolation from data points",
    "The command performs cubic spline interpolation for a sequence of data "
    "points with different boundary conditions and prints the resulting "
    "spline to a file or stdout.");
  config_parser_add_option_group(&parser, SPLINE_INT_PARSER_OPTION_GROUP,
    &spline_int_default_options, "Spline interpolation options",
    "These options control the spline interpolation performed by the "
    "command.");
  config_parser_parse(&parser, argc, argv, config_parser_exit_error);
  
  const char* file = config_get_string(&parser.arguments,
    SPLINE_INT_PARAMETER_FILE);
  
  config_parser_option_group_t* spline_int_option_group =
    config_parser_get_option_group(&parser, SPLINE_INT_PARSER_OPTION_GROUP);
  spline_type_t type = config_get_enum(&spline_int_option_group->options,
    SPLINE_INT_PARAMETER_TYPE);
  double y1_0 = config_get_float(&spline_int_option_group->options,
    SPLINE_INT_PARAMETER_Y1_0);
  double y1_n = config_get_float(&spline_int_option_group->options,
    SPLINE_INT_PARAMETER_Y1_N);
  double y2_0 = config_get_float(&spline_int_option_group->options,
    SPLINE_INT_PARAMETER_Y2_0);
  double y2_n = config_get_float(&spline_int_option_group->options,
    SPLINE_INT_PARAMETER_Y2_N);
  double r_0 = config_get_float(&spline_int_option_group->options,
    SPLINE_INT_PARAMETER_R_0);
  double r_n = config_get_float(&spline_int_option_group->options,
    SPLINE_INT_PARAMETER_R_N);
  const char* output = config_get_string(&spline_int_option_group->options,
    SPLINE_INT_PARAMETER_OUTPUT);
  
  file_init_name(&input_file, file);
  if (string_equal(file, "-"))
    file_open_stream(&input_file, stdin, file_mode_read);
  else
    file_open(&input_file, file_mode_read);
  error_exit(&input_file.error);

  char* line = 0;
  spline_point_t* points = 0;
  size_t num_points = 0;
  
  while (!file_eof(&input_file) &&
      (file_read_line(&input_file, &line, 128) >= 0)) {
    if (string_empty(line) || string_starts_with(line, "#"))
      continue;
    
    double x, y;
    if (string_scanf(line, "%lg %lg\n", &x, &y) == 2) {
      if (!(num_points % 64))
        points = realloc(points, (num_points+64)*sizeof(spline_point_t));
      spline_point_init(&points[num_points], x, y);
      
      ++num_points;
    }
  }
  string_destroy(&line);
  error_exit(&input_file.error);
  file_destroy(&input_file);
  
  spline_init(&spline);
  
  switch (type) {
    case spline_type_y1:
      spline_int_y1(&spline, points, num_points, y1_0, y1_n);
      break;
    case spline_type_y2:
      spline_int_y2(&spline, points, num_points, y2_0, y2_n);
      break;
    case spline_type_y1_y2:
      spline_int_y1_y2(&spline, points, num_points, y1_0, y1_n,
        y2_0, y2_n, r_0, r_n);
      break;
    case spline_type_natural:
      spline_int_natural(&spline, points, num_points);
      break;
    case spline_type_clamped:
      spline_int_clamped(&spline, points, num_points);
      break;
    case spline_type_periodic:
      spline_int_periodic(&spline, points, num_points);
      break;
    case spline_type_not_a_knot:
      spline_int_not_a_knot(&spline, points, num_points);
      break;
  }

  error_exit(&spline.error);
  if (points)
    free(points);
  
  spline_write(output, &spline);
  error_exit(&spline.error);
  
  spline_destroy(&spline);
  config_parser_destroy(&parser);
    
  return 0;
}
