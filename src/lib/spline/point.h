/***************************************************************************
 *   Copyright (C) 2013 by Ralf Kaestner                                   *
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

#ifndef SPLINE_POINT_H
#define SPLINE_POINT_H

/** \file spline/point.h
  * \ingroup spline
  * \brief Spline point definition
  * \author Ralf Kaestner
  * 
  * A spline point represents a point on a spline and consists in two
  * components describing its location.
  */

#include <stdlib.h>
#include <stdio.h>

/** \brief Structure defining a spline point
  * 
  * A spline point is defined by an x- and a y-component.
  */
typedef struct spline_point_t {
  double x;                    //!< The x-component of the spline point.
  double y;                    //!< The y-component of the spline point.
} spline_point_t;

/** \brief Initialize spline point
  * \param[in] point The spline point to be initialized.
  * \param[in] x The initial x-component of the spline point.
  * \param[in] y The initial y-component of the spline point.
  */
void spline_point_init(
  spline_point_t* point,
  double x,
  double y);

/** \brief Copy spline point
  * \param[in] dst The destination spline point to copy to.
  * \param[in] src The source spline point to copy from.
  */
void spline_point_copy(
  spline_point_t* dst,
  const spline_point_t* src);

/** \brief Print spline point
  * \param[in] stream The output stream that will be used for printing the
  *   spline point.
  * \param[in] point The spline point that will be printed.
  */
void spline_point_print(
  FILE* stream,
  const spline_point_t* point);

#endif
