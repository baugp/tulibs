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

#include "point.h"

void spline_point_init(spline_point_t* point, double x, double y) {
  point->x = x;
  point->y = y;
}

void spline_point_copy(spline_point_t* dst, const spline_point_t* src) {
  dst->x = src->x;
  dst->y = src->y;
}

void spline_point_print(FILE* stream, const spline_point_t* point) {
  fprintf(stream, "%10lg %10lg",
    point->x,
    point->y);
}
