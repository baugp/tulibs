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

#ifndef TRANSFORM_H
#define TRANSFORM_H

/** \defgroup transform Geometric Transformation Module
  * \brief Library functions for linear transformation math
  * 
  * The transformation module implements the algebraic formalisms of
  * linear transformations in 3-dimensional spaces.
  */

/** \file transform/transform.h
  * \ingroup transform
  * \brief Linear transformation interface
  * \author Ralf Kaestner
  * 
  * The linear transformation interface facilitates the construction and
  * chaining of matrix representations for translational, rotational, and
  * scaling transforms as well as their application to points in 3-dimensional
  * space.
  */

#include <stdlib.h>
#include <stdio.h>

#include "transform/point.h"
#include "transform/pose.h"

/** \brief Structure defining a transformation
  * 
  * A linear transformation is defined as a 4x4 transformation matrix.
  */
typedef double transform_t[4][4];

/** \brief Initialize identity transform
  * \param[in] transform The transform to be initialized to identity.
  */
void transform_init_identity(
  transform_t transform);

/** \brief Initialize translation transform
  * \param[in] transform The transform to be initialized with a translation.
  * \param[in] t_x The initial translation along the x-axis.
  * \param[in] t_y The initial translation along the y-axis.
  * \param[in] t_z The initial translation along the z-axis.
  */
void transform_init_translation(
  transform_t transform,
  double t_x,
  double t_y,
  double t_z);

/** \brief Initialize scaling transform
  * \param[in] transform The transform to be initialized with a scaling.
  * \param[in] s_x The initial scale along the x-axis.
  * \param[in] s_y The initial scale along the y-axis.
  * \param[in] s_z The initial scale along the z-axis.
  */
void transform_init_scaling(
  transform_t transform,
  double s_x,
  double s_y,
  double s_z);

/** \brief Initialize rotation transform
  * \param[in] transform The transform to be initialized with a rotation.
  * \param[in] yaw The initial rotation about the z-axis in [rad].
  * \param[in] pitch The initial rotation about the y-axis [rad].
  * \param[in] roll The initial rotation about the x-axis [rad].
  */
void transform_init_rotation(
  transform_t transform,
  double yaw,
  double pitch,
  double roll);

/** \brief Initialize pose transform
  * \param[in] transform The transform to be initialized from a pose.
  * \param[in] pose The pose to initialize the transform from.
  */
void transform_init_pose(
  transform_t transform,
  const transform_pose_t* pose);

/** \brief Copy transform
  * \param[in] dst The destination transform to copy to.
  * \param[in] src The source transform to copy from.
  */
void transform_copy(
  transform_t dst,
  transform_t src);

/** \brief Print transform
  * \param[in] stream The output stream that will be used for printing the
  *   transform.
  * \param[in] transform The transform that will be printed.
  */
void transform_print(
  FILE* stream,
  transform_t transform);

/** \brief Left-multiply transform with another transform
  * \param[in,out] right The transform that will be the right-hand factor
  *   of the multiplication and hold the result.
  * \param[in] left The transform that will be the left-hand factor of the
  *   multiplication.
  */
void transform_multiply_left(
  transform_t right,
  transform_t left);

/** \brief Invert transform
  * \param[in,out] transform The transform that will be inverted.
  */
void transform_invert(
  transform_t transform);

/** \brief Apply translation
  * \param[in,out] transform The transform to apply the translation to.
  * \param[in] t_x The translation along the x-axis.
  * \param[in] t_y The translation along the y-axis.
  * \param[in] t_z The translation along the z-axis.
  */
void transform_translate(
  transform_t transform,
  double t_x,
  double t_y,
  double t_z);

/** \brief Apply scaling
  * \param[in,out] transform The transform to apply the scaling to.
  * \param[in] s_x The scale along the x-axis.
  * \param[in] s_y The scale along the y-axis.
  * \param[in] s_z The scale along the z-axis.
  */
void transform_scale(
  transform_t transform,
  double s_x,
  double s_y,
  double s_z);

/** \brief Apply rotation
  * \param[in,out] transform The transform to apply the rotation to.
  * \param[in] yaw The rotation about the z-axis in [rad].
  * \param[in] pitch The rotation about the y-axis [rad].
  * \param[in] roll The rotation about the x-axis [rad].
  */
void transform_rotate(
  transform_t transform,
  double yaw,
  double pitch,
  double roll);

/** \brief Transform point
  * \param[in] transform The transform to apply to the point.
  * \param[in,out] point The point to be transformed.
  */
void transform_point(
  transform_t transform,
  transform_point_t* point);

/** \brief Transform array of points
  * \param[in] transform The transform to apply to the point.
  * \param[in,out] points The array of points to be transformed.
  * \param[in] num_points The number of points in the array.
  */
void transform_points(
  transform_t transform,
  transform_point_t* points,
  size_t num_points);

#endif
