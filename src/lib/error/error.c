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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <execinfo.h>

#include "error.h"

#include "string/string.h"

int error_debug = 0;

void error_set_where(error_t* error, size_t frame);
void error_print_trace(FILE* stream, const error_t* error);

void error_init(error_t* error, const char** descriptions) {
  error->code = 0;
  error->descriptions = descriptions;

  error->where = 0;
  error->what = 0;

  error->blame = 0;
}

void error_init_copy(error_t* error, const error_t* src_error) {
  error->code = src_error->code;
  error->descriptions = src_error->descriptions;
  
  string_init_copy(&error->where, src_error->where);
  string_init_copy(&error->what, src_error->what);
  
  if (src_error->blame) {
    error->blame = malloc(sizeof(error_t));
    error_init_copy(error->blame, src_error->blame);
  }
  else
    error->blame = 0;
}

void error_destroy(error_t* error) {
  error_clear(error);
}

void error_set(error_t* error, int code) {
  error_clear(error);
  
  error->code = code;
  error_set_where(error, 1);
}

void error_set_where(error_t* error, size_t frame) {
  void* frames[frame+2];
  size_t num_frames = backtrace(frames, frame+2);
  
  char** symbols = 0;
  if ((frame+1 < num_frames) &&
      (symbols = backtrace_symbols(&frames[frame+1], 1))) {
    string_copy(&error->where, symbols[0]);
    free(symbols);
  }
  else
    string_destroy(&error->where);
}

void error_blame(error_t* error, const error_t* blame, int code) {
  error_clear(error);

  error->code = code;
  error_set_where(error, 1);
  
  if (blame) {
    error->blame = malloc(sizeof(error_t));
    error_init_copy(error->blame, blame);
  }
}

void error_setf(error_t* error, int code, const char* format, ...) {
  error_clear(error);

  error->code = code;
  error_set_where(error, 1);
  
  va_list vargs;  
  va_start(vargs, format);
  string_vprintf(&error->what, format, vargs);
  va_end(vargs);
}

void error_blamef(error_t* error, const error_t* blame, int code, const
    char* format, ...) {
  error_clear(error);

  error->code = code;
  error_set_where(error, 1);
  
  va_list vargs;
  va_start(vargs, format);  
  string_vprintf(&error->what, format, vargs);
  va_end(vargs);
  
  if (blame) {
    error->blame = malloc(sizeof(error_t));
    error_init_copy(error->blame, blame);
  }
}

int error_get(const error_t* error) {
  return error->code;
}

const char* error_get_description(const error_t* error) {
  return error->descriptions[error->code];
}

void error_clear(error_t* error) {
  error->code = 0;
  
  string_destroy(&error->where);
  string_destroy(&error->what);
  
  if (error->blame) {
    error_destroy(error->blame);

    free(error->blame);
    error->blame = 0;
  }
}

void error_print(FILE* stream, const error_t* error) {
  fprintf(stream, "Error: %s%s%s", 
    error->descriptions[error->code],
    !string_empty(error->what) ? ": " : "",
    !string_empty(error->what) ? error->what : "");
  
  if (error_debug) {
    fprintf(stream, "\n");
    error_print_trace(stream, error);
  }
}

void error_print_trace(FILE* stream, const error_t* error) {
  fprintf(stream, "  from %s: %s%s%s", 
    !string_empty(error->where) ? error->where : "",
    error->descriptions[error->code],
    !string_empty(error->what) ? ": " : "",
    !string_empty(error->what) ? error->what : "");
  
  if (error->blame) {
    fprintf(stream, "\n");
    error_print_trace(stream, error->blame);
  }
}

void error_exit(const error_t* error) {
  if (error->code) {
    error_print(stderr, error);
    fprintf(stderr, "\n");
    
    exit(EXIT_FAILURE);
  }
}
