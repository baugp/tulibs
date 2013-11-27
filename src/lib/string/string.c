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

#define _GNU_SOURCE

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "string.h"
#include "list.h"

void string_init(char** string, size_t length) {
  if (length > 0)
    *string = malloc(length);
  else
    *string = 0;
}

size_t string_init_copy(char** string, const char* src_string) {
  size_t src_length = string_length(src_string);
  
  if (src_length) {
    *string = malloc(src_length+1);
    strcpy(*string, src_string);    
  }
  else
    *string = 0;
  
  return src_length;
}

void string_destroy(char** string) {
  if (*string) {
    free(*string);
    *string = 0;
  }
}

size_t string_length(const char* string) {
  return string ? strlen(string) : 0;
}

int string_empty(const char* string) {
  return !string || !string[0];
}

int string_equal(const char* string, const char* other_string) {
  if (!string_empty(string) && !string_empty(other_string))
    return !strcmp(string, other_string);
  
  return 1;
}

int string_equal_ignore_case(const char* string, const char* other_string) {
  if (!string_empty(string) && !string_empty(other_string))
    return !strcasecmp(string, other_string);
  
  return 1;
}

int string_starts_with(const char* string, const char* prefix) {
  if (!string_empty(prefix)) {
    size_t length = string_length(string);
    size_t prefix_length = string_length(prefix);
    
    if (length < prefix_length)
      return 0;
    
    return !strncmp(string, prefix, prefix_length);
  }
  
  return 1;
}

int string_starts_with_ignore_case(const char* string, const char* prefix) {
  if (!string_empty(prefix)) {
    size_t length = string_length(string);
    size_t prefix_length = string_length(prefix);
    
    if (length < prefix_length)
      return 0;
    
    return !strncasecmp(string, prefix, prefix_length);
  }
  
  return 1;
}

int string_ends_with(const char* string, const char* suffix) {
  if (!string_empty(suffix)) {
    size_t length = string_length(string);
    size_t suffix_length = string_length(suffix);
    
    if (length < suffix_length)
      return 0;
    
    return !strcmp(string+length-suffix_length, suffix);
  }
  
  return 1;
}

int string_ends_with_ignore_case(const char* string, const char* suffix) {
  if (!string_empty(suffix)) {
    size_t length = string_length(string);
    size_t suffix_length = string_length(suffix);
    
    if (length < suffix_length)
      return 0;
    
    return !strcasecmp(string+length-suffix_length, suffix);
  }
  
  return 1;
}

const char* string_find(const char* string, const char* substring) {
  if (!string_empty(string) && !string_empty(substring)) {
    if (substring[1] != 0)
      return strstr(string, substring);
    else
      return strchr(string, substring[0]);
  }

  return 0;
}

const char* string_rfind(const char* string, const char* substring) {
  if (!string_empty(string) && !string_empty(substring)) {
    if (substring[1] != 0) {
      size_t length = string_length(string);
      size_t find_length = string_length(substring);
      
      if (length >= find_length) {
        ssize_t i = length-find_length;
        for ( ; i >= 0; --i)
          if (!strcmp(string+i, substring))
            return string+i;
      }
    }
    else
      return strrchr(string, substring[0]);
  }

  return 0;
}

size_t string_copy(char** dst, const char* src) {
  size_t src_length = string_length(src);
  
  if (src_length) {
    *dst = realloc(*dst, src_length+1);
    strcpy(*dst, src);
  }
  else
    string_destroy(dst);
  
  return src_length;
}

size_t string_upper(char* string) {
  if (string) {
    size_t i = 0;
    
    while(string[i]) {
      string[i] = toupper(string[i]);
      ++i;
    }
    
    return i;
  }
  else
    return 0;
}

size_t string_lower(char* string) {
  if (string) {
    size_t i = 0;
    
    while(string[i]) {
      string[i] = tolower(string[i]);
      ++i;
    }
    
    return i;
  }
  else
    return 0;
}

size_t string_reverse(char* string) {
  size_t length = string_length(string);
  
  if (length) {
    size_t i = 0, j = length-1;
    for ( ; i < j; ++i, --j) {
      char swap = string[i];
      string[i] = string[j];
      string[j] = swap;
    }
  }
  
  return length;
}

size_t string_cat(char** prefix, const char* suffix) {
  size_t prefix_length = string_length(*prefix);
  size_t suffix_length = string_length(suffix);

  if (suffix_length) {
    *prefix = realloc(*prefix, prefix_length+suffix_length+1);
    strcpy(*prefix+prefix_length, suffix);
  }
  
  return prefix_length+suffix_length;
}

size_t string_split(const char* joint, char*** strings, const char*
    delimiter) {
  size_t num_strings = 0;

  string_list_destroy(strings);
  if (!string_empty(joint)) {
    size_t delim_length = string_length(delimiter);
    num_strings = 1;
    
    if (!string_empty(delimiter)) {
      const char* start = joint;
      
      while ((start = string_find(start, delimiter))) {
        start += delim_length;
        ++num_strings;
      }
    }

    if (num_strings) {
      string_list_init(strings, num_strings);
      const char* start = joint;
      size_t i;
      
      for (i = 0; i < num_strings-1; ++i) {
        const char* end = string_find(start, delimiter);
        
        (*strings)[i] = malloc(end-start+1);
        strncpy((*strings)[i], start, end-start);
        (*strings)[i][end-start] = 0;
        
        start = end+delim_length;
      }
      
      (*strings)[num_strings-1] = malloc(string_length(start)+1);
      strcpy((*strings)[num_strings-1], start);
    }
  }
  
  return num_strings;
}

size_t string_scanf(const char* string, const char* format, ...) {
  va_list vargs;
  
  va_start(vargs, format);
  size_t num_args = string_vscanf(string, format, vargs);
  va_end(vargs);
  
  return num_args;
}

size_t string_vscanf(const char* string, const char* format, va_list vargs) {
  if (!string_empty(string)) {
    ssize_t result;
    
    if ((result = vsscanf(string, format, vargs)) < 0)
      result = 0;
  
    return result;
  }
  else
    return 0;
}

size_t string_printf(char** string, const char* format, ...) {
  va_list vargs;
  
  va_start(vargs, format);
  size_t length = string_vprintf(string, format, vargs);
  va_end(vargs);
  
  return length;
}

size_t string_vprintf(char** string, const char* format, va_list vargs) {
  ssize_t result;
  
  string_destroy(string);
  if ((result = vasprintf(string, format, vargs)) < 0) {
    string_destroy(string);
    result = 0;
  }
  
  return result;
}

size_t string_justify(char* string, size_t line_length) {
  size_t cur_line_length = 0;
  size_t max_line_length = 0;
  char* line = string;

  while (1) {
    size_t i = 0, word_length = 0;
    char* word = &line[cur_line_length];
    
    if (word[0]) {
      while (isspace(word[i])) {
        if (word[i] == '\t')
          word[i] = ' ';
        
        i++;
        word_length++;
      }
      while (word[i] && !isspace(word[i++]))
        word_length++;
    }
    
    if (!word_length)
      break;
    
    if (cur_line_length+word_length >= line_length) {
      if (cur_line_length > 0) {
        line[cur_line_length] = '\n';
        line += cur_line_length+1;
        cur_line_length = 0;
      }
      else {
        line[word_length] = '\n';
        line += word_length+1;
      }
    }
    else
      cur_line_length += word_length;

    if (cur_line_length > max_line_length)
      max_line_length = cur_line_length;    
  }
  
  return max_line_length;
}
