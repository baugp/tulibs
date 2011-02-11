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

#include <math.h>

#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>

#include "transform.h"

const char* transform_errors[] = {
  "success",
};

void transform_init_identity(transform_t transform) {
  int i, j;

  for (i = 0; i < 4; ++i) {
    for (j = 0; j < 4; ++j) {
      if (i == j)
        transform[i][j] = 1.0;
      else
        transform[i][j] = 0.0;
    }
  }
}

void transform_init_translation(transform_t transform, double t_x, double t_y,
    double t_z) {
  transform_init_identity(transform);

  transform[0][3] = t_x;
  transform[1][3] = t_y;
  transform[2][3] = t_z;
}

void transform_init_scaling(transform_t transform, double s_x, double s_y,
    double s_z) {
  transform_init_identity(transform);

  transform[0][0] = s_x;
  transform[1][1] = s_y;
  transform[2][2] = s_z;
}

void transform_init_rotation(transform_t transform, double yaw, double pitch,
    double roll) {
  transform_init_identity(transform);

  transform[0][0] = cos(yaw)*cos(pitch);
  transform[0][1] = cos(yaw)*sin(pitch)*sin(roll)-sin(yaw)*cos(roll);
  transform[0][2] = cos(yaw)*sin(pitch)*cos(roll)+sin(yaw)*sin(roll);

  transform[1][0] = sin(yaw)*cos(pitch);
  transform[1][1] = sin(yaw)*sin(pitch)*sin(roll)+cos(yaw)*cos(roll);
  transform[1][2] = sin(yaw)*sin(pitch)*cos(roll)-cos(yaw)*sin(roll);

  transform[2][0] = -sin(pitch);
  transform[2][1] = cos(pitch)*sin(roll);
  transform[2][2] = cos(pitch)*cos(roll);
}

void transform_copy(transform_t dst, transform_t src) {
  int i, j;

  for (i = 0; i < 4; ++i)
    for (j = 0; j < 4; ++j)
      dst[i][j] = src[i][j];
}

void transform_print(FILE* stream, transform_t transform) {
  int i;

  for (i = 0; i < 4; ++i) {
    fprintf(stream, "%10lg  %10lg  %10lg  %10lg\n",
      transform[i][0],
      transform[i][1],
      transform[i][2],
      transform[i][3]);
  }
}

void transform_multiply_left(transform_t right, transform_t left) {
  transform_t result;
  int i, j, k;

  for (i = 0; i < 4; ++i) {
    for (j = 0; j < 4; ++j) {
      result[i][j] = 0;
      
      for (k = 0; k < 4; ++k)
        result[i][j] += left[i][k]*right[k][j];
    }
  }

  transform_copy(right, result);
}

void transform_invert(transform_t transform) {
  gsl_matrix* A = gsl_matrix_calloc(4, 4);
  gsl_matrix* A_inv = gsl_matrix_calloc(4, 4);
  gsl_permutation* perm = gsl_permutation_alloc(4);
  int i, j, signum;

  for (i = 0; i < 4; ++i)
    for (j = 0; j < 4; ++j)
      gsl_matrix_set(A, i, j, transform[i][j]);
    
  gsl_linalg_LU_decomp(A, perm, &signum);
  gsl_linalg_LU_invert(A, perm, A_inv);
  
  for (i = 0; i < 4; ++i)
    for (j = 0; j < 4; ++j)
      transform[i][j] = gsl_matrix_get(A_inv, i, j);
    
  gsl_matrix_free(A);
  gsl_matrix_free(A_inv);
  gsl_permutation_free(perm);
}

void transform_translate(transform_t transform, double t_x, double t_y,
    double t_z) {
  transform_t translation;

  transform_init_translation(translation, t_x, t_y, t_z);
  transform_multiply_left(transform, translation);
}

void transform_scale(transform_t transform, double s_x, double s_y,
    double s_z) {
  transform_t scaling;

  transform_init_scaling(scaling, s_x, s_y, s_z);
  transform_multiply_left(transform, scaling);
}

void transform_rotate(transform_t transform, double yaw, double pitch,
    double roll) {
  transform_t rotation;

  transform_init_rotation(rotation, yaw, pitch, roll);
  transform_multiply_left(transform, rotation);
}

void transform_point(transform_t transform, transform_point_t point) {
  transform_point_t result;
  int i, j;

  for (i = 0; i < 3; ++i) {
    result[i] = 0.0;
    for (j = 0; j < 3; ++j)
      result[i] += transform[i][j]*point[j];
    result[i] += transform[i][3];
  }

  transform_point_copy(point, result);
}

void transform_points(transform_t transform, transform_point_t* points,
    size_t num_points) {
  int i;

  for (i = 0; i < num_points; +i)
    transform_point(transform, points[i]);
}
