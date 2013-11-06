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

#ifndef TRANSFORM_POINT_H
#define TRANSFORM_POINT_H

#include <stdlib.h>
#include <stdio.h>

/** \file point.h
  * \ingroup transform
  * \brief Point definition for the linear transformation module
  * \author Ralf Kaestner
  * 
  * A point in 3-dimensional space consists in three components
  * describing location.
  */

/** \brief Structure defining a point
  * \note A point is defined by an x, y, and z-component.
  */
typedef struct transform_point_t {
  double x;                    //!< The x-component of the point.
  double y;                    //!< The y-component of the point.
  double z;                    //!< The z-component of the point.
} transform_point_t, *transform_point_p;

/** \brief Initialize a point
  * \param[in] point The point to be initialized.
  * \param[in] x The initial x-component of the point.
  * \param[in] y The initial y-component of the point.
  * \param[in] z The initial z-component of the point.
  */
void transform_point_init(
  transform_point_p point,
  double x,
  double y,
  double z);

/** \brief Copy a point
  * \param[in] dst The destination point to copy to.
  * \param[in] src The source point to copy from.
  */
void transform_point_copy(
  transform_point_p dst,
  transform_point_p src);

/** \brief Print a point
  * \param[in] stream The output stream that will be used for printing the
  *   point.
  * \param[in] point The point that will be printed.
  */
void transform_point_print(
  FILE* stream,
  transform_point_p point);

#endif
