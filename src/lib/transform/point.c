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

#include "point.h"

void transform_point_init(transform_point_t* point, double x, double y,
    double z) {
  point->x = x;
  point->y = y;
  point->z = z;
}

void transform_point_copy(transform_point_t* dst, const transform_point_t*
    src) {
  dst->x = src->x;
  dst->y = src->y;
  dst->z = src->z;
}

void transform_point_print(FILE* stream, const transform_point_t* point) {
  fprintf(stream, "%10lg %10lg %10lg",
    point->x,
    point->y,
    point->z);
}
