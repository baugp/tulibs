/***************************************************************************
 *   Copyright (C) 2008 by Fritz Stoeckli, Ralf Kaestner                   *
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

#include "segment.h"

#include "spline/spline.h"

#define sqr(a) ((a)*(a))
#define cub(a) ((a)*(a)*(a))

void spline_segment_init(spline_segment_t* segment, double a, double b,
    double c, double d, double x_0) {
  segment->a = a;
  segment->b = b;
  segment->c = c;
  segment->d = d;

  segment->x_0 = x_0;
}

void spline_segment_init_zero(spline_segment_t* segment) {
  segment->a = 0.0;
  segment->b = 0.0;
  segment->c = 0.0;
  segment->d = 0.0;

  segment->x_0 = 0.0;
}

void spline_segment_copy(spline_segment_t* dst, const spline_segment_t* src) {
  dst->a = src->a;
  dst->b = src->b;
  dst->c = src->c;
  dst->d = src->d;

  dst->x_0 = src->x_0;
}

void spline_segment_print(FILE* stream, const spline_segment_t* segment) {
  fprintf(stream, "%10lg %10lg %10lg %10lg %10lg",
    segment->a,
    segment->b,
    segment->c,
    segment->d,
    segment->x_0);
}

double spline_segment_eval(const spline_segment_t* segment, spline_eval_type_t
    eval_type, double x) {
  x -= segment->x_0;

  if (eval_type == spline_eval_type_first_derivative)
    return 3.0*segment->a*sqr(x)+2.0*segment->b*x+segment->c;
  else if (eval_type == spline_eval_type_second_derivative)
    return 6.0*segment->a*x+2.0*segment->b;
  else
    return segment->a*cub(x)+segment->b*sqr(x)+segment->c*x+segment->d;
}
