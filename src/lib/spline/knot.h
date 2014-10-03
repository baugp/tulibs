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

#ifndef SPLINE_KNOT_H
#define SPLINE_KNOT_H

/** \file spline/knot.h
  * \ingroup spline
  * \brief Knot definition for the cubic spline
  * \author Ralf Kaestner
  * 
  * A spline knot defines the end-point of a cubic spline segment and
  * consists in three components describing location and curvature.
  */

#include <stdlib.h>
#include <stdio.h>

#include "spline/eval_type.h"

/** \brief Structure defining a spline knot
  * 
  * A spline knot is defined by an x and y-component, and its curvature.
  */
typedef struct spline_knot_t {
  double x;                    //!< The x-component of the spline knot.
  double y;                    //!< The y-component of the spline knot.
  double y2;                   //!< The curvature of the spline knot.
} spline_knot_t;

/** \brief Initialize spline knot
  * \param[in] knot The spline knot to be initialized.
  * \param[in] x The initial x-component of the spline knot.
  * \param[in] y The initial y-component of the spline knot.
  * \param[in] y2 The initial curvature of the spline knot.
  */
void spline_knot_init(
  spline_knot_t* knot,
  double x,
  double y,
  double y2);

/** \brief Copy spline knot
  * \param[in] dst The destination spline knot to copy to.
  * \param[in] src The source spline knot to copy from.
  */
void spline_knot_copy(
  spline_knot_t* dst,
  const spline_knot_t* src);

/** \brief Print spline knot
  * \param[in] stream The output stream that will be used for printing the
  *   spline knot.
  * \param[in] knot The spline knot that will be printed.
  */
void spline_knot_print(
  FILE* stream,
  const spline_knot_t* knot);

/** \brief Evaluate the third-order polynomial defined by two cubic spline
 *    knots at a given location
  * \param[in] knot_min The spline knot whose location defines the lower
  *   bound of the spline interval defined by both knots. This bound will
  *   not be checked nor enforced by the function.
  * \param[in] knot_max The spline knot whose location defines the upper
  *   bound of the spline interval defined by both knots. This bound will
  *   not be checked nor enforced by the function.
  * \param[in] eval_type The evaluation type to be used.
  * \param[in] x The location at which to evaluate the third-order
  *   polynomial defined by the knots.
  * \return The value of the third-order polynomial at the given location.
  */
double spline_knot_eval(
  const spline_knot_t* knot_min,
  const spline_knot_t* knot_max,
  spline_eval_type_t eval_type,
  double x);

#endif
