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

#include "pose.h"

void transform_pose_init(transform_pose_p pose, double x, double y,
    double z, double yaw, double pitch, double roll) {
  pose->x = x;
  pose->y = y;
  pose->z = z;

  pose->yaw = yaw;
  pose->pitch = pitch;
  pose->roll = roll;
}

void transform_pose_copy(transform_pose_p dst, transform_pose_p src) {
  dst->x = src->x;
  dst->y = src->y;
  dst->z = src->z;

  dst->yaw = src->yaw;
  dst->pitch = src->pitch;
  dst->roll = src->roll;
}

void transform_pose_print(FILE* stream, transform_pose_p pose) {
  fprintf(stream, "%10lg  %10lg  %10lg %10lg  %10lg  %10lg\n",
    pose->x,
    pose->y,
    pose->z,
    pose->yaw,
    pose->pitch,
    pose->roll);
}
