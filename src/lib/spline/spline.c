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

#include <string.h>
#include <math.h>

#include <gsl/gsl_linalg.h>

#include "spline.h"

#include "spline/segment.h"

#include "string/string.h"

#include "file/file.h"

#define sqr(a) ((a)*(a))
#define cub(a) ((a)*(a)*(a))

const char* spline_errors[] = {
  "Success",
  "Invalid spline segment",
  "Failed to read spline from file",
  "Invalid spline file format",
  "Failed to write spline to file",
  "Spline undefined at value",
  "Spline interpolation failed",
};

void spline_init(spline_t* spline) {
  spline->knots = 0;
  spline->num_knots = 0;
  
  error_init(&spline->error, spline_errors);
}

void spline_destroy(spline_t* spline) {
  spline_clear(spline);
  
  error_destroy(&spline->error);
}

void spline_clear(spline_t* spline) {
  if (spline->num_knots) {
    free(spline->knots);

    spline->knots = 0;
    spline->num_knots = 0;
  }
  
  error_clear(&spline->error);
}

size_t spline_get_num_segments(const spline_t* spline) {
  return spline->num_knots ? spline->num_knots-1 : 0;
}

int spline_get_segment(spline_t* spline, size_t index, spline_segment_t*
    segment) {
  error_clear(&spline->error);
  
  if ((index >= 0) && (index+1 < spline->num_knots)) {
    double x_1 = spline->knots[index+1].x-spline->knots[index].x;
      
    segment->a = (spline->knots[index+1].y2-spline->knots[index].y2)/(6.0*x_1);
    segment->b = 0.5*spline->knots[index].y2;
    segment->c = (spline->knots[index+1].y-segment->a*cub(x_1)-
      segment->b*sqr(x_1)-spline->knots[index].y)/x_1;
    segment->d = spline->knots[index].y;
  }
  else
    error_setf(&spline->error, SPLINE_ERROR_SEGMENT, "%d", (int)index);
  
  return spline->error.code;
}

ssize_t spline_find_segment(spline_t* spline, double x) {
  return spline_find_segment_bisect(spline, x, 0,
    spline->num_knots > 1 ? spline->num_knots-1 : 0);
}

ssize_t spline_find_segment_bisect(spline_t* spline, double x, size_t
    index_min, size_t index_max) {
  error_clear(&spline->error);

  if (spline->num_knots > 1) {
    size_t i = (index_min < spline->num_knots-1) ? index_min : 
      spline->num_knots-2;
    size_t j = (index_max < spline->num_knots) ? index_max : 
      spline->num_knots-1;
      
    if ((j > i) && (x >= spline->knots[i].x) && (x <= spline->knots[j].x)) {    
      while (j-i > 1) {
        size_t k = (i+j) >> 1;
        if (spline->knots[k].x > x)
          j = k;
        else
          i = k;
      }
      
      return i;
    }
  }

  error_setf(&spline->error, SPLINE_ERROR_UNDEFINED, "%lg", x);
  return -spline->error.code;
}

ssize_t spline_find_segment_linear(spline_t* spline, double x, size_t
    index_start) {
  error_clear(&spline->error);
  
  if ((spline->num_knots > 1) && (x >= spline->knots[0].x) &&
      (x <= spline->knots[spline->num_knots-1].x)) {
    size_t i = (index_start < spline->num_knots) ? index_start : 
      spline->num_knots-1;
      
    while (1) {
      if (x >= spline->knots[i].x) {
        if (x <= spline->knots[i+1].x)
          return i;
        else
          ++i;
      }
      else
        --i;
    }
  }
  
  error_setf(&spline->error, SPLINE_ERROR_UNDEFINED, "%lg", x);  
  return -spline->error.code;
}

void spline_print(FILE* stream, const spline_t* spline) {
  size_t i;
  
  for (i = 0; i < spline->num_knots; i++) {
    if (i)
      fprintf(stream, "\n");
    spline_knot_print(stream, &spline->knots[i]);
  }
}

int spline_read(const char* filename, spline_t* spline) {
  spline_knot_t knot;
  file_t file;

  spline_clear(spline);
  
  file_init_name(&file, filename);
  if (string_equal(filename, "-"))
    file_open_stream(&file, stdin, file_mode_read);
  else
    file_open(&file, file_mode_read);

  if (!file.handle) {
    error_blame(&spline->error, &file.error, SPLINE_ERROR_FILE_READ);
    file_destroy(&file);
    
    return -error_get(&spline->error);
  }
  
  char* line = 0;
  while (!file_eof(&file) && (file_read_line(&file, &line, 128) >= 0)) {
    if (string_empty(line) || string_starts_with(line, "#"))
      continue;
      
    if (string_scanf(line, "%lg %lg %lg", &knot.x, &knot.y, &knot.y2) != 3) {
      error_setf(&spline->error, SPLINE_ERROR_FILE_FORMAT, line);
      break;
    }

    spline_add_knot(spline, &knot);
  }
  string_destroy(&line);
  
  if (file.error.code)
    error_blame(&spline->error, &file.error, SPLINE_ERROR_FILE_READ);
  file_destroy(&file);

  return spline->error.code ? -spline->error.code : spline->num_knots;
}

int spline_write(const char* filename, spline_t* spline) {
  file_t file;

  error_clear(&spline->error);
  
  file_init_name(&file, filename);
  if (string_equal(filename, "-"))
    file_open_stream(&file, stdout, file_mode_write);
  else
    file_open(&file, file_mode_write);

  size_t i;
  for (i = 0; i < spline->num_knots; ++i)
    if (file_printf(&file, "%10lg %10lg %10lg\n",
      spline->knots[i].x,
      spline->knots[i].y,
      spline->knots[i].y2) < 0)
    break;

  if (file.error.code)
    error_blame(&spline->error, &file.error, SPLINE_ERROR_FILE_WRITE);
  file_destroy(&file);

  return spline->error.code ? -spline->error.code : spline->num_knots;
}
 
size_t spline_add_knot(spline_t* spline, const spline_knot_t* knot) {
  if (spline->num_knots &&
      (spline->knots[spline->num_knots-1].x >= knot->x)) {
    ssize_t i = spline_find_segment(spline, knot->x);
  
    if (i < 0) {
      error_clear(&spline->error);
      i = 0;
    }
    else
      ++i;

    if (i && (spline->knots[i].x == knot->x))
      spline_knot_copy(&spline->knots[i], knot);
    else if (spline->knots[i+1].x == knot->x)
      spline_knot_copy(&spline->knots[i+1], knot);
    else {
      spline->knots = realloc(spline->knots,
        (spline->num_knots+1)*sizeof(spline_knot_t));
      memmove(&spline->knots[i+1], &spline->knots[i],
        (spline->num_knots-i)*sizeof(spline_knot_t));
        
      spline_knot_copy(&spline->knots[i], knot);
      ++spline->num_knots;
    }
  }
  else {
    spline->knots = realloc(spline->knots,
      (spline->num_knots+1)*sizeof(spline_knot_t));
    
    spline_knot_copy(&spline->knots[spline->num_knots], knot);
    ++spline->num_knots;
  }
  
  return spline->num_knots;
}

ssize_t spline_int_y1(spline_t* spline, const spline_point_t* points,
    size_t num_points, double y1_0, double y1_n) {
  error_clear(&spline->error);
  
  if (num_points > 2) {
    double h_1 = points[1].x-points[0].x;
    double h_n = points[num_points-1].x-points[num_points-2].x;
    
    double b_1 = 6.0/h_1*((points[1].y-points[0].y)/h_1-y1_0);
    double b_n = 6.0/h_n*(y1_n-(points[num_points-1].y-
      points[num_points-2].y)/h_n);
    
    double* y2 = 0;
    ssize_t result;
    
    if ((result = spline_int_solve_tridiag_y2(points, num_points, 2.0, 2.0,
        1.0, 1.0, b_1, b_n, &y2)) > 0) {    
      spline->knots = realloc(spline->knots, result*sizeof(spline_knot_t));
      spline->num_knots = result;
    
      size_t i;
      for (i = 0; i < spline->num_knots; ++i) {
        spline_knot_t* knot = &spline->knots[i];

        knot->x = points[i].x;
        knot->y = points[i].y;
        knot->y2 = y2[i];
      }
      free(y2);
    }
    else
      error_set(&spline->error, -result);
  }
  else
    error_set(&spline->error, SPLINE_ERROR_INTERPOLATION);
  
  return spline->error.code ? -spline->error.code : spline->num_knots;  
}

ssize_t spline_int_y2(spline_t* spline, const spline_point_t* points,
    size_t num_points, double y2_0, double y2_n) {
  error_clear(&spline->error);
  
  if (num_points > 2) {
    double* y2 = 0;
    ssize_t result;
    
    if ((result = spline_int_solve_tridiag_y2(points, num_points, 1.0, 1.0,
        0.0, 0.0, y2_0, y2_n, &y2)) > 0) {    
      spline->knots = realloc(spline->knots, result*sizeof(spline_knot_t));
      spline->num_knots = result;
    
      size_t i;
      for (i = 0; i < spline->num_knots; ++i) {
        spline_knot_t* knot = &spline->knots[i];

        knot->x = points[i].x;
        knot->y = points[i].y;
        knot->y2 = y2[i];
      }
      free(y2);
    }
    else
      error_set(&spline->error, -result);
  }
  else
    error_set(&spline->error, SPLINE_ERROR_INTERPOLATION);
  
  return spline->error.code ? -spline->error.code : spline->num_knots;        
}

ssize_t spline_int_y1_y2(spline_t* spline, const spline_point_t* points,
    size_t num_points, double y1_0, double y1_n, double y2_0, double y2_n,
    double r_0, double r_n) {
  error_clear(&spline->error);
  
  if (num_points > 4) {
    double h_1 = r_0*(points[1].x-points[0].x);
    double h_2 = (1.0-r_0)*(points[1].x-points[0].x);
    double h_3 = points[2].x-points[1].x;
    double h_l = points[num_points-2].x-points[num_points-3].x;
    double h_m = (1.0-r_n)*(points[num_points-1].x-points[num_points-2].x);
    double h_n = r_n*(points[num_points-1].x-points[num_points-2].x);

    double d_1 = 2.0*h_2+h_1*(3.0+h_1/h_2);
    double d_2 = 2.0*(h_2+h_3);
    double d_m = 2.0*(h_l+h_m);
    double d_n = 2.0*h_m+h_n*(3.0+h_n/h_m);
    double e_1 = h_2;
    double e_2 = h_3;
    double e_m = h_m-sqr(h_n)/h_m;
    double c_1 = h_2-sqr(h_1)/h_2;
    double c_l = h_l;
    double c_m = h_m;
    double b_1 = 6.0*((points[1].y-points[0].y)/h_2-y1_0*(1.0+h_1/h_2)-
      y2_0*(0.5+h_1/(3.0*h_2))*h_1);
    double b_2 = 6.0*((points[2].y-points[1].y)/h_3-
      (points[1].y-points[0].y)/h_2+y1_0*h_1/h_2+y2_0*sqr(h_1)/(3.0*h_2));
    double b_m = 6.0*((points[num_points-1].y-points[num_points-2].y)/h_m-
      (points[num_points-2].y-points[num_points-3].y)/h_l-
      y1_n*h_n/h_m+y2_n*sqr(h_n)/(3.0*h_m));
    double b_n = 6.0*((points[num_points-2].y-points[num_points-1].y)/h_m+
      y1_n*(1.0+h_n/h_m)-y2_n*(0.5+h_n/(3.0*h_m))*h_n);
    
    gsl_vector* c = gsl_vector_alloc(num_points-1);
    gsl_vector* d = gsl_vector_alloc(num_points);
    gsl_vector* e = gsl_vector_alloc(num_points-1);
    gsl_vector* x = gsl_vector_alloc(num_points);
    gsl_vector* b = gsl_vector_alloc(num_points);
    
    gsl_vector_set(d, 0, d_1);
    gsl_vector_set(e, 0, e_1);
    gsl_vector_set(b, 0, b_1);
    gsl_vector_set(c, 0, c_1);
    gsl_vector_set(d, 1, d_2);
    gsl_vector_set(e, 1, e_2);
    gsl_vector_set(b, 1, b_2);
    
    size_t i;
    double h_i, h_j = 0.0;
    for (i = 2; i < num_points-2; ++i) {
      h_i = (i > 2) ? h_j : points[i].x-points[i-1].x;
      h_j = points[i+1].x-points[i].x;
      
      double d_i = 2.0*(h_i+h_j);
      double c_i = h_i;
      double e_i = h_j;
      double b_i = 6.0*((points[i+1].y-points[i].y)/h_j-
        (points[i].y-points[i-1].y)/h_i);
      
      gsl_vector_set(c, i-1, c_i);
      gsl_vector_set(d, i, d_i);
      gsl_vector_set(e, i, e_i);
      gsl_vector_set(b, i, b_i);
    }
    
    gsl_vector_set(c, num_points-3, c_l);
    gsl_vector_set(d, num_points-2, d_m);
    gsl_vector_set(e, num_points-2, e_m);
    gsl_vector_set(b, num_points-2, b_m);
    gsl_vector_set(c, num_points-2, c_m);
    gsl_vector_set(d, num_points-1, d_n);
    gsl_vector_set(b, num_points-1, b_n);

    if (!gsl_linalg_solve_tridiag(d, e, c, b, x)) {
      spline->knots = realloc(spline->knots, (num_points+2)*
        sizeof(spline_knot_t));
      spline->num_knots = num_points+2;

      spline->knots[0].x = points[0].x;
      spline->knots[0].y = points[0].y;
      spline->knots[0].y2 = y2_0;
      spline->knots[1].x = points[0].x+h_1;
      spline->knots[1].y2 = gsl_vector_get(x, 0);
      spline->knots[1].y = (y2_0/3.0*h_1+spline->knots[1].y2/6.0*h_1+y1_0)*
        h_1+points[0].y;
      
      size_t i;
      for (i = 1; i < spline->num_knots-2; ++i) {
        spline_knot_t* knot = &spline->knots[i+1];

        knot->x = points[i].x;
        knot->y = points[i].y;
        knot->y2 = gsl_vector_get(x, i);
      }

      spline->knots[spline->num_knots-2].x = points[num_points-1].x-h_n;
      spline->knots[spline->num_knots-2].y2 = gsl_vector_get(x, num_points-1);
      spline->knots[spline->num_knots-2].y = (y2_n/3.0*h_n+
        spline->knots[spline->num_knots-2].y2/6.0*h_n-y1_n)*h_n+
        points[num_points-1].y;
      spline->knots[spline->num_knots-1].x = points[num_points-1].x;
      spline->knots[spline->num_knots-1].y = points[num_points-1].y;
      spline->knots[spline->num_knots-1].y2 = y2_n;
    }
    else
      error_set(&spline->error, SPLINE_ERROR_INTERPOLATION);
    
    gsl_vector_free(c);
    gsl_vector_free(d);
    gsl_vector_free(e);
    gsl_vector_free(x);
    gsl_vector_free(b);
  }
  else
    error_set(&spline->error, SPLINE_ERROR_INTERPOLATION);
  
  return spline->error.code ? -spline->error.code : spline->num_knots;
}

ssize_t spline_int_natural(spline_t* spline, const spline_point_t*  points,
    size_t num_points) {
  return spline_int_y2(spline, points, num_points, 0.0, 0.0);
}

ssize_t spline_int_clamped(spline_t* spline, const spline_point_t* points,
    size_t num_points) {
  return spline_int_y1(spline, points, num_points, 0.0, 0.0);
}

ssize_t spline_int_periodic(spline_t* spline, const spline_point_t* points,
    size_t num_points) {
  error_clear(&spline->error);
  
  if (num_points > 2) {
    double h_1 = points[1].x-points[0].x;
    double h_m = points[num_points-1].x-points[num_points-2].x;
    
    double d_1 = 2.0*(h_1+h_m);
    double e_m = h_m;
    double b_1 = 6.0*((points[1].y-points[0].y)/h_1-
      (points[num_points-1].y-points[num_points-2].y)/h_m);
    
    double* y2 = 0;
    ssize_t result;
    
    if ((result = spline_int_solve_symm_cyc_tridiag_y2(points, num_points,
        d_1, e_m, b_1, &y2)) > 0) {
      spline->knots = realloc(spline->knots, (result+1)*
        sizeof(spline_knot_t));
      spline->num_knots = result+1;
      
      size_t i;
      for (i = 0; i < spline->num_knots; ++i) {
        spline_knot_t* knot = &spline->knots[i];

        knot->x = points[i].x;
        knot->y = points[i].y;
        knot->y2 = y2[(i+1 < spline->num_knots) ? i : 0];
      }
      free(y2);
    }
    else
      error_set(&spline->error, -result);
  }
  else
    error_set(&spline->error, SPLINE_ERROR_INTERPOLATION);
  
  return spline->error.code ? -spline->error.code : spline->num_knots;
}

ssize_t spline_int_not_a_knot(spline_t* spline, const spline_point_t* points,
    size_t num_points) {
  error_clear(&spline->error);
  
  if (num_points > 4) {
    double h_1 = points[1].x-points[0].x;
    double h_2 = points[2].x-points[1].x;
    double h_m = points[num_points-2].x-points[num_points-3].x;
    double h_n = points[num_points-1].x-points[num_points-2].x;

    double d_1 = 3.0*h_1+2.0*h_2+sqr(h_1)/h_2;
    double d_n = 3.0*h_n+2.0*h_m+sqr(h_n)/h_m;
    double e_1 = h_2-sqr(h_1)/h_2;
    double c_m = h_m-sqr(h_n)/h_m;
    double b_1 = 6.0*((points[2].y-points[1].y)/h_2-
      (points[1].y-points[0].y)/h_1);
    double b_n = 6.0*((points[num_points-1].y-points[num_points-2].y)/h_n-
      (points[num_points-2].y-points[num_points-3].y)/h_m);
        
    double* y2 = 0;
    ssize_t result;
    
    if ((result = spline_int_solve_tridiag_y2(&points[1], num_points-2,
        d_1, d_n, e_1, c_m, b_1, b_n, &y2)) > 0) {    
      spline->knots = realloc(spline->knots, result*sizeof(spline_knot_t));
      spline->num_knots = result;
    
      size_t i;
      for (i = 0; i < spline->num_knots; ++i) {
        spline_knot_t* knot = &spline->knots[i];

        knot->x = points[i+1].x;
        knot->y = points[i+1].y;
        knot->y2 = y2[i];
      }
      free(y2);
      
      spline_knot_t* knot_1 = &spline->knots[0];
      knot_1->y2 = spline_knot_eval(&spline->knots[0],
        &spline->knots[1], spline_eval_type_second_derivative, points[0].x);
      knot_1->x = points[0].x;
      knot_1->y = points[0].y;
      
      spline_knot_t* knot_n = &spline->knots[spline->num_knots-1];
      knot_n->y2 = spline_knot_eval(&spline->knots[spline->num_knots-2],
        &spline->knots[spline->num_knots-1],
        spline_eval_type_second_derivative, points[num_points-1].x);
      knot_n->x = points[num_points-1].x;
      knot_n->y = points[num_points-1].y;
    }
    else
      error_set(&spline->error, -result);
  }
  else
    error_set(&spline->error, SPLINE_ERROR_INTERPOLATION);
  
  return spline->error.code ? -spline->error.code : spline->num_knots;
}

ssize_t spline_int_solve_tridiag_y1(const spline_point_t* points, size_t
    num_points, double d_1, double d_n, double e_1, double c_m, double b_1,
    double b_n, double** y1) {
  if (num_points > 2) {
    *y1 = realloc(*y1, num_points*sizeof(double));
    
    gsl_vector* c = gsl_vector_alloc(num_points-1);
    gsl_vector* d = gsl_vector_alloc(num_points);
    gsl_vector* e = gsl_vector_alloc(num_points-1);
    gsl_vector_view x = gsl_vector_view_array(*y1, num_points);
    gsl_vector* b = gsl_vector_alloc(num_points);
    
    gsl_vector_set(d, 0, d_1);
    gsl_vector_set(e, 0, e_1);
    gsl_vector_set(b, 0, b_1);
    
    size_t i;
    double h_i, h_j = 0.0;
    for (i = 1; i < num_points-1; ++i) {
      h_i = (i > 1) ? h_j : points[i].x-points[i-1].x;
      h_j = points[i+1].x-points[i].x;
      
      double d_i = 2.0*(h_i+h_j);
      double c_i = h_i;
      double e_i = h_j;
      double b_i = 3.0*(h_i*(points[i+1].y-points[i].y)/h_j+
        h_j*(points[i].y-points[i-1].y)/h_i);
      
      gsl_vector_set(c, i-1, c_i);
      gsl_vector_set(d, i, d_i);
      gsl_vector_set(e, i, e_i);
      gsl_vector_set(b, i, b_i);
    }
    
    gsl_vector_set(c, num_points-2, c_m);
    gsl_vector_set(d, num_points-1, d_n);
    gsl_vector_set(b, num_points-1, b_n);

    int result = gsl_linalg_solve_tridiag(d, e, c, b, &x.vector);
    
    gsl_vector_free(c);
    gsl_vector_free(d);
    gsl_vector_free(e);
    gsl_vector_free(b);

    if (result) {
      free(*y1);
      *y1 = 0;
    }
    else
      return num_points;
  }
  
  return -SPLINE_ERROR_INTERPOLATION;
}

ssize_t spline_int_solve_tridiag_y2(const spline_point_t* points, size_t
    num_points, double d_1, double d_n, double e_1, double c_m, double b_1,
    double b_n, double** y2) {
  if (num_points > 2) {
    *y2 = realloc(*y2, num_points*sizeof(double));
    
    gsl_vector* c = gsl_vector_alloc(num_points-1);
    gsl_vector* d = gsl_vector_alloc(num_points);
    gsl_vector* e = gsl_vector_alloc(num_points-1);
    gsl_vector_view x = gsl_vector_view_array(*y2, num_points);
    gsl_vector* b = gsl_vector_alloc(num_points);
    
    gsl_vector_set(d, 0, d_1);
    gsl_vector_set(e, 0, e_1);
    gsl_vector_set(b, 0, b_1);
    
    size_t i;
    double h_i, h_j = 0.0;
    for (i = 1; i < num_points-1; ++i) {
      h_i = (i > 1) ? h_j : points[i].x-points[i-1].x;
      h_j = points[i+1].x-points[i].x;
      
      double d_i = 2.0*(h_i+h_j);
      double c_i = h_i;
      double e_i = h_j;
      double b_i = 6.0*((points[i+1].y-points[i].y)/h_j-
        (points[i].y-points[i-1].y)/h_i);
      
      gsl_vector_set(c, i-1, c_i);
      gsl_vector_set(d, i, d_i);
      gsl_vector_set(e, i, e_i);
      gsl_vector_set(b, i, b_i);
    }
    
    gsl_vector_set(c, num_points-2, c_m);
    gsl_vector_set(d, num_points-1, d_n);
    gsl_vector_set(b, num_points-1, b_n);

    int result = gsl_linalg_solve_tridiag(d, e, c, b, &x.vector);
    
    gsl_vector_free(c);
    gsl_vector_free(d);
    gsl_vector_free(e);
    gsl_vector_free(b);

    if (result) {
      free(*y2);
      *y2 = 0;
    }
    else
      return num_points;
  }
  
  return -SPLINE_ERROR_INTERPOLATION;
}

ssize_t spline_int_solve_symm_cyc_tridiag_y2(const spline_point_t* points,
    size_t num_points, double d_1, double e_m, double b_1, double** y2) {
  if (num_points > 2) {
    *y2 = realloc(*y2, (num_points-1)*sizeof(double));
    
    gsl_vector* d = gsl_vector_alloc(num_points-1);
    gsl_vector* e = gsl_vector_alloc(num_points-1);
    gsl_vector_view x = gsl_vector_view_array(*y2, num_points-1);
    gsl_vector* b = gsl_vector_alloc(num_points-1);
    
    gsl_vector_set(d, 0, d_1);
    gsl_vector_set(b, 0, b_1);
    
    size_t i;
    double h_i, h_j = 0.0;
    for (i = 1; i < num_points-1; ++i) {
      h_i = (i > 1) ? h_j : points[i].x-points[i-1].x;
      h_j = points[i+1].x-points[i].x;
      
      double d_i = 2.0*(h_i+h_j);
      double e_i = h_i;
      double b_i = 6.0*((points[i+1].y-points[i].y)/h_j-
        (points[i].y-points[i-1].y)/h_i);
      
      gsl_vector_set(d, i, d_i);
      gsl_vector_set(e, i-1, e_i);
      gsl_vector_set(b, i, b_i);
    }
    
    gsl_vector_set(e, num_points-2, e_m);

    int result = gsl_linalg_solve_symm_cyc_tridiag(d, e, b, &x.vector);
    
    gsl_vector_free(d);
    gsl_vector_free(e);
    gsl_vector_free(b);
    
    if (result) {
      free(*y2);
      *y2 = 0;
    }
    else
      return num_points-1;
    
  }

  return -SPLINE_ERROR_INTERPOLATION;
}

double spline_eval(spline_t* spline, spline_eval_type_t eval_type, double x) {
  return spline_eval_bisect(spline, eval_type, x, 0,
    spline->num_knots > 1 ? spline->num_knots-1 : 0);
}

double spline_eval_bisect(spline_t* spline, spline_eval_type_t eval_type,
    double x, size_t index_min, size_t index_max) {
  ssize_t i;
  
  if ((i = spline_find_segment_bisect(spline, x, index_min, index_max)) >= 0)
    return spline_knot_eval(&spline->knots[i], &spline->knots[i+1],
      eval_type, x);
  else
    return NAN;
}

double spline_eval_linear(spline_t* spline, spline_eval_type_t eval_type,
    double x, size_t* index) {
  ssize_t i;
  
  if ((i = spline_find_segment_linear(spline, x, *index)) >= 0) {
    *index = i;
    return spline_knot_eval(&spline->knots[i], &spline->knots[i+1],
      eval_type, x);
  }
  else
    return NAN;
}
