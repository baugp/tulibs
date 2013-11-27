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

#ifndef SPLINE_SEGMENT_H
#define SPLINE_SEGMENT_H

/** \file spline/knot.h
  * \ingroup spline
  * \brief Segment definition for the cubic spline
  * \author Ralf Kaestner
  * 
  * A spline segment describes a cubic spline between two spline knots and is
  * defined by the coefficients of a third-order polynomial.
  */

#include <stdlib.h>
#include <stdio.h>

#include "spline/eval_type.h"

/** \brief Structure defining a spline segment
  * 
  * A spline segment is defined by the coefficients a, b, c, and d of the
  * third-order polynomial f(x) = a*x^3+b*x^2+c*x+d and its location x_0.
  */
typedef struct spline_segment_t {
  double a;             //!< The spline segment's cubic coefficient.
  double b;             //!< The spline segment's quadratic coefficient.
  double c;             //!< The spline segment's linear coefficient.
  double d;             //!< The spline segment's constant offset.

  double x_0;           //!< The location of the spline segment.
} spline_segment_t;

/** \brief Initialize spline segment
  * \param[in] segment The spline segment to be initialized.
  * \param[in] a The cubic coefficient of the spline segment to be
  *   initialized.
  * \param[in] b The quadratic coefficient of the spline segment to be
  *   initialized.
  * \param[in] c The linear coefficient of the spline segment to be
  *   initialized.
  * \param[in] d The constant offset of the spline segment to be
  *   initialized.
  * \param[in] x_0 The location of the spline segment to be initialized.
  */
void spline_segment_init(
  spline_segment_t* segment,
  double a,
  double b, 
  double c,
  double d,
  double x_0);

/** \brief Initialize spline segment with zeros
  * \param[in] segment The spline segment to be initialized with zero
  *   coefficients and location.
  */
void spline_segment_init_zero(
  spline_segment_t* segment);

/** \brief Copy spline segment
  * \param[in] dst The destination spline segment to copy to.
  * \param[in] src The source spline segment to copy from.
  */
void spline_segment_copy(
  spline_segment_t* dst,
  const spline_segment_t* src);

/** \brief Print spline segment
  * \param[in] stream The output stream that will be used for printing the
  *   spline segment.
  * \param[in] segment The spline segment that will be printed.
  */
void spline_segment_print(
  FILE* stream,
  const spline_segment_t* segment);

/** \brief Evaluate spline segment at a given location
  * \param[in] segment The spline segment to be evaluated.
  * \param[in] eval_type The evaluation type to be used.
  * \param[in] x The location at which to evaluate the spline segment.
  * \return The value of the spline segment at the given location.
  */
double spline_segment_eval(
  const spline_segment_t* segment,
  spline_eval_type_t eval_type,
  double x);

#endif
