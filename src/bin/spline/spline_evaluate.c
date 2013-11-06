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

config_param_t file_param = {
  "FILE",
  config_param_type_string,
  "",
  "",
  "The name of the input file containing the spline",
};

config_param_t step_size_param = {
  "STEP_SIZE",
  config_param_type_float,
  "",
  "(0.0, inf)",
  "The step size used to generate equidistant arguments "
  "of the spline function",
};

config_param_t eval_type_param = {
  "eval-type",
  config_param_type_enum,
  "base",
  "base|first|second",
  "The type of spline evaluation requested, where 'base' refers to "
  "the base function, and 'first' or 'second' indicates the first or "
  "second derivative, respectively",
};

int main(int argc, char **argv) {
  config_parser_t parser;

  config_parser_init_default(&parser,
    "Evaluate a cubic spline for equidistant arguments",
    "The command evaluates a cubic input spline for equidistant "
    "arguments and prints the corresponding function values to stdout. "
    "Depending on the options provided, these values may be generated "
    "from the base function or its derivatives.");
  config_set_param(&parser.arguments, &file_param);
  config_set_param(&parser.arguments, &step_size_param);
  config_parser_option_group_p spline_option_group =
    config_parser_add_option_group(&parser, 0, "spline-", "Spline options",
    "These options control the spline operations performed by the command.");
  config_set_param(&spline_option_group->options, &eval_type_param);
  config_parser_parse(&parser, argc, argv, config_parser_exit_both);
  
  const char* file = config_param_get_string(&file_param);
  double step_size = config_param_get_float(&step_size_param);
  spline_eval_type_t eval_type = config_param_get_enum(&eval_type_param);
  
  spline_t spline;
  int result;
  
  if ((result = spline_read(file, &spline)) < 0)
    fprintf(stderr, "%s\n", spline_errors[-result]);
  double x = 0.0, f_x;
  
  int i = 0;
  while ((i = spline_evaluate_linear_search(
      &spline, eval_type, x, i, &f_x)) >= 0) {
    fprintf(stdout, "%lf %lf\n", x, f_x);
    x += step_size;
  }

  spline_destroy(&spline);
  return 0;
}
