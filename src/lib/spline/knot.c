/***************************************************************************
 *   Copyright (C) 2013 by Ralf Kaestner                                   *
 *   stfritz@ethz.ch, ralf.kaestner@gmail.com                              *
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

#include "knot.h"

#define sqr(a) ((a)*(a))
#define cub(a) ((a)*(a)*(a))

void spline_knot_init(spline_knot_t* knot, double x, double y, double y2) {
  knot->x = x;
  knot->y = y;
  knot->y2 = y2;
}

void spline_knot_copy(spline_knot_t* dst, const spline_knot_t* src) {
  dst->x = src->x;
  dst->y = src->y;
  dst->y2 = src->y2;
}

void spline_knot_print(FILE* stream, const spline_knot_t* knot) {
  fprintf(stream, "%10lg %10lg %10lg",
    knot->x,
    knot->y,
    knot->y2);
}

double spline_knot_eval(const spline_knot_t* knot_min, const spline_knot_t*
    knot_max, spline_eval_type_t eval_type, double x) {
  double h_i = knot_max->x-knot_min->x;
  double a = (knot_max->x-x)/h_i;
  double b = (x-knot_min->x)/h_i;

  if (eval_type == spline_eval_type_first_derivative)
    return (knot_max->y-knot_min->y)/h_i-0.5*sqr(a)*h_i*knot_min->y2+
      0.5*sqr(b)*h_i*knot_max->y2-(knot_max->y2-knot_min->y2)*h_i/6.0;
  else if (eval_type == spline_eval_type_second_derivative)
    return a*knot_min->y2+b*knot_max->y2;
  else
    return a*knot_min->y+b*knot_max->y+((cub(a)-a)*knot_min->y2+
      (cub(b)-b)*knot_max->y2)*sqr(h_i)/6.0;
}
