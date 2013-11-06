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

#ifndef TRANSFORM_POSE_H
#define TRANSFORM_POSE_H

#include <stdlib.h>
#include <stdio.h>

/** \file pose.h
  * \ingroup transform
  * \brief Pose definition for the linear transformation module
  * \author Ralf Kaestner
  * 
  * A pose in 3-dimensional space consists in three components describing
  * location and three additional components describing orientation.
  */

/** \brief Structure defining a pose
  * \note A pose is defined by its x, y, and z-component and the yaw,
  *   pitch, and roll angles.
  */
typedef struct transform_pose_t {
  double x;                    //!< The x-component of the pose.
  double y;                    //!< The y-component of the pose.
  double z;                    //!< The z-component of the pose.

  double yaw;                  //!< The yaw angle of the pose.
  double pitch;                //!< The pitch angle of the pose.
  double roll;                 //!< The roll angle of the pose.
} transform_pose_t, *transform_pose_p;

/** \brief Initialize a pose
  * \param[in] pose The pose to be initialized.
  * \param[in] x The initial x-component of the pose.
  * \param[in] y The initial y-component of the pose.
  * \param[in] z The initial z-component of the pose.
  * \param[in] yaw The initial yaw angle of the pose.
  * \param[in] pitch The initial pitch angle of the pose.
  * \param[in] roll The initial roll angle of the pose.
  */
void transform_pose_init(
  transform_pose_p pose,
  double x,
  double y,
  double z,
  double yaw,
  double pitch,
  double roll);

/** \brief Copy a pose
  * \param[in] dst The destination pose to copy to.
  * \param[in] src The source pose to copy from.
  */
void transform_pose_copy(
  transform_pose_p dst,
  transform_pose_p src);

/** \brief Print a pose
  * \param[in] stream The output stream that will be used for printing the
  *   pose.
  * \param[in] pose The pose that will be printed.
  */
void transform_pose_print(
  FILE* stream,
  transform_pose_p pose);

#endif
