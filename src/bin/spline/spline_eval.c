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
#include <math.h>

#include "config/parser.h"
#include "spline/spline.h"
#include "string/string.h"
#include "file/file.h"

#define SPLINE_EVAL_PARAMETER_FILE              "FILE"
#define SPLINE_EVAL_PARAMETER_STEP_SIZE         "STEP_SIZE"

#define SPLINE_EVAL_PARSER_OPTION_GROUP         "spline-eval"
#define SPLINE_EVAL_PARAMETER_TYPE              "type"
#define SPLINE_EVAL_PARAMETER_OUTPUT            "output"

config_param_t spline_eval_default_arguments_params[] = {
  {SPLINE_EVAL_PARAMETER_FILE,
    config_param_type_string,
    "",
    "",
    "Read spline from the specified input file or '-' for stdin"},
  {SPLINE_EVAL_PARAMETER_STEP_SIZE,
    config_param_type_float,
    "",
    "(0.0, inf)",
    "The step size used to generate equidistant locations of the "
    "spline function"},
};

const config_default_t spline_eval_default_arguments = {
  spline_eval_default_arguments_params,
  sizeof(spline_eval_default_arguments_params)/sizeof(config_param_t),
};

config_param_t spline_eval_default_options_params[] = {
  {SPLINE_EVAL_PARAMETER_TYPE,
    config_param_type_enum,
    "base",
    "base|first|second",
    "The type of spline evaluation requested, where 'base' refers to "
    "the base function, and 'first' or 'second' indicates the first or "
    "second derivative, respectively"},
  {SPLINE_EVAL_PARAMETER_OUTPUT,
    config_param_type_string,
    "-",
    "",
    "Write values to the specified output file or '-' for stdout"},
};

const config_default_t spline_eval_default_options = {
  spline_eval_default_options_params,
  sizeof(spline_eval_default_options_params)/sizeof(config_param_t),
};

int main(int argc, char **argv) {
  config_parser_t parser;
  spline_t spline;
  file_t output_file;

  config_parser_init_default(&parser, &spline_eval_default_arguments, 0,
    "Evaluate a cubic spline at equidistant locations",
    "The command evaluates a cubic input spline at equidistant "
    "locations and prints the corresponding function values to a file "
    "or stdout. Depending on the options provided, these values may be "
    "generated from the base function or its derivatives.");
  config_parser_add_option_group(&parser, SPLINE_EVAL_PARSER_OPTION_GROUP,
    &spline_eval_default_options, "Spline evaluation options",
    "These options control the spline evaluation performed by the command.");
  config_parser_parse(&parser, argc, argv, config_parser_exit_error);
  
  const char* file = config_get_string(&parser.arguments,
    SPLINE_EVAL_PARAMETER_FILE);
  double step_size = config_get_float(&parser.arguments,
    SPLINE_EVAL_PARAMETER_STEP_SIZE);
  
  config_parser_option_group_t* spline_eval_option_group =
    config_parser_get_option_group(&parser, SPLINE_EVAL_PARSER_OPTION_GROUP);
  spline_eval_type_t eval_type = config_get_enum(
    &spline_eval_option_group->options, SPLINE_EVAL_PARAMETER_TYPE);
  const char* output = config_get_string(
    &spline_eval_option_group->options, SPLINE_EVAL_PARAMETER_OUTPUT);

  spline_init(&spline);
  
  spline_read(file, &spline);
  error_exit(&spline.error);

  file_init_name(&output_file, output);
  if (string_equal(output, "-"))
    file_open_stream(&output_file, stdout, file_mode_write);
  else
    file_open(&output_file, file_mode_write);
  error_exit(&output_file.error);
  
  double x = spline.num_knots ? spline.knots[0].x : 0.0;
  double f_x;
  size_t i = 0, j = 0;
  
  while (!isnan(f_x = spline_eval_linear(&spline, eval_type, x, &i))) {
    file_printf(&output_file, "%10lg %10lg\n", x, f_x);
    error_exit(&output_file.error);
    
    ++j;
    x = spline.knots[0].x+step_size*j;
  }

  spline_destroy(&spline);
  file_destroy(&output_file);
  config_parser_destroy(&parser);
    
  return 0;
}
