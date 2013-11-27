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

#ifndef SPLINE_H
#define SPLINE_H

/** \defgroup spline Spline Math Module
  * \brief Library functions for working with splines
  * 
  * The spline module provides library functions for representing and
  * evaluating cubic splines, i.e., smooth polynomical functions serving
  * as a piecewise description for interpolated data points.
  */

/** \file spline/spline.h
  * \ingroup spline
  * \brief Simple cubic spline implementation
  * \author Ralf Kaestner
  * 
  * A cubic spline is defined by a sequence of spline knots. The interface
  * facilitates construction, manipulation, and evaluation of cubic splines.
  * A particular feature of this interface is the linear search evaluation
  * method.
  */

#include <stdlib.h>
#include <stdio.h>

#include "spline/point.h"
#include "spline/knot.h"
#include "spline/segment.h"
#include "spline/eval_type.h"

#include "error/error.h"

/** \name Error Codes
  * \brief Predefined spline error codes
  */
//@{
#define SPLINE_ERROR_NONE                  0
//!< Success
#define SPLINE_ERROR_SEGMENT               1
//!< Invalid spline segment
#define SPLINE_ERROR_FILE_READ             2
//!< Error reading spline from file
#define SPLINE_ERROR_FILE_FORMAT           3
//!< Invalid spline file format
#define SPLINE_ERROR_FILE_WRITE            4
//!< Error writing spline to file
#define SPLINE_ERROR_UNDEFINED             5
//!< Spline undefined at value
#define SPLINE_ERROR_INTERPOLATION         6
//!< Spline interpolation failed
//@}

/** \brief Predefined spline error descriptions
  */
extern const char* spline_errors[];

struct spline_segment_t;

/** \brief Structure defining the spline
  * \note The first spline segment is assumed to start at zero.
  */
typedef struct spline_t {
  spline_knot_t* knots;       //!< The knots of the spline.
  size_t num_knots;           //!< The number of spline knots.

  error_t error;              //!< The most recent spline error.
} spline_t;

/** \brief Initialize an empty cubic spline
  * \param[in] spline The cubic spline to be initialized.
  */
void spline_init(
  spline_t* spline);

/** \brief Destroy a cubic spline
  * \param[in] spline The cubic spline to be destroyed.
  */
void spline_destroy(
  spline_t* spline);

/** \brief Clear a cubic spline
  * \param[in] spline The cubic spline to be cleared.
  */
void spline_clear(
  spline_t* spline);

/** \brief Retrieve the cubic spline's number of segments
  * \param[in] spline The cubic spline to retrieve the number of
  *   segments for.
  * \return The number of segments of the cubic spline.
  * 
  * For a cubic spline with N > 0 knots, the number of segments is N-1.
  * An empty spline with zero knots has zero segments.
  */
size_t spline_get_num_segments(
  const spline_t* spline);

/** \brief Retrieve segment of the cubic spline
  * \param[in] spline The cubic spline to retrieve the segment for.
  * \param[in] index The index of the cubic spline segment to be retrieved.
  *   For a cubic spline with N > 0 knots, this index should be in the
  *   range [0, N-1].
  * \param[in,out] segment The segment to receive the cubic spline
  *   segment with the given index. If no such segment exists, it will
  *   not be modified.
  * \return The resulting error code.
  */
int spline_get_segment(
  spline_t* spline,
  size_t index,
  struct spline_segment_t* segment);

/** \brief Find segment of the cubic spline at a given location
  * \param[in] spline The cubic spline to be searched for the segment.
  * \param[in] x The location to find the spline segment for.
  * \return The index of the cubic spline segment at the given location
  *   or the negative error code if no such segment exists.
  * 
  * This is a convenience function which searches the entire spline by
  * means of the function spline_find_segment_bisect().
  */
ssize_t spline_find_segment(
  spline_t* spline,
  double x);

/** \brief Find segment of the cubic spline at a given location using
 *    bisection
  * \param[in] spline The cubic spline to be searched for the segment.
  * \param[in] x The location to find the spline segment for.
  * \param[in] index_min The lower bound of the search interval defined
  *   over the sequence of segment indexes.
  * \param[in] index_max The upper bound of the search interval defined
  *   over the sequence of segment indexes.
  * \return The index of the cubic spline segment at the given location
  *   or the negative error code if no such segment exists within the
  *   search interval.
  * 
  * Bisection search on the spline is optimal if sequential calls to
  * this function involve random locations. The spline will be searched
  * in the interval [index_min, index_max] of segment indexes.
  */
ssize_t spline_find_segment_bisect(
  spline_t* spline,
  double x,
  size_t index_min,
  size_t index_max);

/** \brief Find segment of the cubic spline at a given location using
 *    linear search
  * \param[in] spline The cubic spline to be searched for the segment.
  * \param[in] x The location to find the spline segment for.
  * \param[in] index_start The segment index at which to start the linear
  *   search.
  * \return The index of the cubic spline segment at the given location
  *   or the negative error code if no such segment exists.
  * 
  * Linear search on the spline is optimal if sequential calls to this
  * function involve incremental or decremental locations. The spline will
  * be searched forward or backwards from index_start until a terminal
  * knot is reached.
  */
ssize_t spline_find_segment_linear(
  spline_t* spline,
  double x,
  size_t index_start);

/** \brief Print a cubic spline
  * \param[in] stream The output stream that will be used for printing the
  *   cubic spline.
  * \param[in] spline The cubic spline that will be printed.
  */
void spline_print(
  FILE* stream,
  const spline_t* spline);

/** \brief Read cubic spline from file
  * \note Calling this function may invalidate previously acquired knot
  *   pointers.
  * \param[in] filename The name of the file containing the cubic spline.
  *   The special filename '-' indicates that the cubic spline shall be
  *   read from  stdin.
  * \param[in,out] spline The read cubic spline.
  * \return The number of spline knots read from the file or the negative
  *   error code.
  * 
  * The spline knots will be re-allocated to accommodate the read file
  * content.
  */
int spline_read(
  const char* filename,
  spline_t* spline);

/** \brief Write cubic spline to file
  * \param[in] filename The name of the file the cubic spline will be 
  *   written to. The special filename '-' indicates that the cubic
  *   spline shall be written to stdout.
  * \param[in] spline The cubic spline to be written.
  * \return The number of spline knots written to the file or the negative
  *   error code.
  */
int spline_write(
  const char* filename,
  spline_t* spline);

/** \brief Add knot to the cubic spline
  * \note Calling this function may invalidate previously acquired knot
  *   pointers.
  * \param[in] spline The cubic spline the knot will be added to.
  * \param[in] knot The spline knot to be added to the cubic spline.
  * \return The number of knots in the resulting cubic spline.
  * 
  * The spline knots will be re-allocated to accommodate the added knot.
  * Since the spline is represented by a sequence of knots, sorted
  * increasingly by their location, the added knot may have to be inserted
  * into this sequence such as to obey the required ordering. If a knot
  * with the same location is found in the spline, the added knot will
  * replace this knot.
  */
size_t spline_add_knot(
  spline_t* spline,
  const spline_knot_t* knot);

/** \brief Cubic spline interpolation from data points
  * \param[in,out] spline The cubic spline to be generated from the data.
  * \param[in] points An array of spline data points which will define the
  *   interpolation points of the resulting cubic spline.
  * \param[in] num_points The number of spline data points.
  * \param[in] e_0 The first boundary element of the tridiagonal system's
  *   upper sub-diagonal.
  * \param[in] c_n The last boundary element of the tridiagonal system's
  *   lower sub-diagonal.
  * \param[in] b_0 The first boundary element of the right-hand side vector.
  * \param[in] b_n The last boundary element of the right-hand side vector.
  * \return The number of segments in the resulting cubic spline or the
  *   negative error code.
  * 
  * The spline knots will be re-allocated to accommodate the interpolation
  * points. For N data points, the resulting cubic spline will consist in N
  * knots (N-1 segments). The generalized spline interpolation problem is
  * represented as a tridiagonal system of equations A*x = b with boundary
  * conditions as expressed by the provided boundary elements for A and b.
  * 
  * The main diagonal d of the tridiagonal N x N matrix A is a vector
  * d = (2, ..., 2)^T of length N. The lower sub-diagonal
  * c = (c_1, ..., c_N)^T and the upper sub-diagonal e = (e_0, ..., e_N-1)^T
  * both have length N-1. To solve the tridiagonal system, A and the
  * right-hand side vector b = (b_0, ..., b_N)^T are initialized from the
  * data and the provided boundary elements. The solution vector
  * x = (y2_0, ..., y2_N-1)^T directly yields the second order derivatives
  * at the N spline knots in O(N) computational time.
  */
ssize_t spline_int(
  spline_t* spline,
  const spline_point_t* points,
  size_t num_points,
  double e_0,
  double c_n,
  double b_0,
  double b_n);

/** \brief Cubic spline interpolation from data points with known first
  *   derivatives at the outer knots
  * \param[in,out] spline The cubic spline to be generated from the data.
  * \param[in] points An array of spline data points which will define the
  *   interpolation points of the resulting cubic spline.
  * \param[in] num_points The number of spline data points.
  * \param[in] y1_0 The first derivative at the first knot of the resulting
  *   cubic spline.
  * \param[in] y1_n The first derivative at the last knot of the resulting
  *   cubic spline.
  * \return The number of segments in the resulting cubic spline or the
  *   negative error code.
  * 
  * This is a convenience function which calls spline_int() with the boundary
  * elements adapted such as to force the first derivatives at the outer spline
  * knots to the values provided.
  */
ssize_t spline_int_y1(
  spline_t* spline,
  const spline_point_t* points,
  size_t num_points,
  double y1_0,
  double y1_n);

/** \brief Cubic spline interpolation from data points with known second
  *   derivatives at the outer knots
  * \param[in,out] spline The cubic spline to be generated from the data.
  * \param[in] points An array of spline data points which will define the
  *   interpolation points of the resulting cubic spline.
  * \param[in] num_points The number of spline data points.
  * \param[in] y2_0 The second derivative at the first knot of the resulting
  *   cubic spline.
  * \param[in] y2_n The second derivative at the last knot of the resulting
  *   cubic spline.
  * \return The number of segments in the resulting cubic spline or the
  *   negative error code.
  * 
  * This is a convenience function which calls spline_int() with the boundary
  * elements adapted such as to force the second derivatives at the outer
  * spline knots to the values provided.
  */
ssize_t spline_int_y2(
  spline_t* spline,
  const spline_point_t* points,
  size_t num_points,
  double y2_0,
  double y2_n);

/** \brief Natural cubic spline interpolation from data points
  * \param[in,out] spline The cubic spline to be generated from the data.
  * \param[in] points An array of spline data points which will define the
  *   interpolation points of the resulting cubic spline.
  * \param[in] num_points The number of spline data points.
  * \return The number of segments in the resulting cubic spline or the
  *   negative error code.
  * 
  * This is a convenience function which calls spline_int_y2() with the
  * boundary conditions adapted to natural cubic spline interpolation.
  * In the natural case, the second derivatives at the outer spline knots
  * are forced to zero.
  */
ssize_t spline_int_natural(
  spline_t* spline,
  const spline_point_t* points,
  size_t num_points);

/** \brief Clamped cubic spline interpolation from data points
  * \param[in,out] spline The cubic spline to be generated from the data.
  * \param[in] points An array of spline data points which will define the
  *   interpolation points of the resulting cubic spline.
  * \param[in] num_points The number of spline data points.
  * \return The number of segments in the resulting cubic spline or the
  *   negative error code.
  * 
  * This is a convenience function which calls spline_int_y1() with the
  * boundary conditions adapted to clamped cubic spline interpolation. In the
  * clamped case, the first derivatives at the outer spline knots are forced
  * to zero.
  */
ssize_t spline_int_clamped(
  spline_t* spline,
  const spline_point_t* points,
  size_t num_points);

/** \brief Evaluate the spline at a given location
  * \param[in] spline The cubic spline to be evaluated.
  * \param[in] eval_type The evaluation type to be used.
  * \param[in] x The location at which to evaluate the cubic spline.
  * \return The function value of the cubic spline at the given location
  *   or NaN if the spline is undefined at that location.
  * 
  * This is a convenience function which evaluates the spline value by 
  * means of the function spline_eval_bisect(), allowing for the 
  * corresponding segment to be searched on the entire spline.
  */
double spline_eval(
  spline_t* spline,
  spline_eval_type_t eval_type,
  double x);

/** \brief Evaluate the spline at a given location using bisection
  * \param[in] spline The cubic spline to be evaluated.
  * \param[in] eval_type The evaluation type to be used.
  * \param[in] x The location at which to evaluate the cubic spline.
  * \param[in] index_min The lower bound of the search interval defined
  *   over the sequence of segment indexes.
  * \param[in] index_max The upper bound of the search interval defined
  *   over the sequence of segment indexes.
  * \return The function value of the cubic spline at the given location
  *   or NaN if the spline is undefined at that location.
  * 
  * This method calls the function spline_find_segment_bisect() in order
  * to identify the spline segment at the given location. This is optimal
  * if sequential calls to this function involve random locations.
  */
double spline_eval_bisect(
  spline_t* spline,
  spline_eval_type_t eval_type,
  double x,
  size_t index_min,
  size_t index_max);

/** \brief Evaluate the spline at a given location using linear search
  * \param[in] spline The cubic spline to be evaluated.
  * \param[in] eval_type The evaluation type to be used.
  * \param[in] x The location at which to evaluate the cubic spline.
  * \param[in,out] index The segment index at which to start with the linear
  *   search. On return, the index will be modified to indicate the spline
  *   segment at the given location. If no such segment exists, the returned
  *   index will be undefined.
  * \return The function value of the cubic spline at the given location
  *   or NaN if the spline is undefined at that location.
  * 
  * This method calls the function spline_find_segment_linear() in order
  * to identify the spline segment at the given location. This is optimal
  * if sequential calls to this function involve incremental or decremental
  * locations.
  */
double spline_eval_linear(
  spline_t* spline,
  spline_eval_type_t eval_type,
  double x,
  size_t* index);

#endif
