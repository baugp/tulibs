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

#include "list.h"
#include "string.h"

void string_list_init(char*** list, size_t length) {
  if (length > 0) {
    *list = malloc((length+1)*sizeof(char*));
    
    size_t i;
    for (i = 0; i < length+1; ++i)
      (*list)[i] = 0;
  }
  else
    *list = 0;
}

size_t string_list_init_copy(char*** list, const char** src_list) {
  size_t src_length = string_list_length(src_list);
  
  if (src_length) {
    *list = malloc((src_length+1)*sizeof(char*));
    
    size_t i;
    for (i = 0; i < src_length; ++i)
      string_init_copy(list[i], src_list[i]);
    (*list)[src_length] = 0;
  }
  else
    *list = 0;
  
  return src_length;
}

void string_list_destroy(char*** list) {
  if (*list) {
    size_t i = 0;
    
    while ((*list)[i])
      string_destroy(&(*list)[i++]);
    
    free(*list);
    *list = 0;
  }
}

size_t string_list_length(const char** list) {
  if (list) {
    size_t i = 0;
    
    while (list[i])
      ++i;
    
    return i;
  }
  else
    return 0;
}

int string_list_empty(const char** list) {
  return !list || !list[0];
}

ssize_t string_list_find(const char** list, const char* string) {
  if (list) {
    ssize_t i = 0;
    
    while (list[i]) {
      if (string_equal(list[i], string))
        return i;
      else
        ++i;
    }
  }
  
  return -1;
}

ssize_t string_list_rfind(const char** list, const char* string) {
  if (list) {
    ssize_t i = string_list_length(list);
    
    while (i >= 0) {
      if (string_equal(list[i], string))
        return i;
      else
        --i;
    }
  }
  
  return -1;
}

size_t string_list_copy(char*** dst, const char** src) {
  size_t src_length = string_list_length(src);
  
  if (src_length) {
    size_t dst_length = string_list_length((const char**)*dst);

    size_t i;
    for (i = src_length; i < dst_length; ++i)
      string_destroy(dst[i]);
    *dst = realloc(*dst, (src_length+1)*sizeof(char*));
    for (i = dst_length; i < src_length+1; ++i)
      (*dst)[i] = 0;

    for (i = 0; i < src_length; ++i)
      string_copy(dst[i], src[i]);
  }
  else
    string_list_destroy(dst);
  
  return src_length;
}

size_t string_list_push(char*** list, const char* string) {
  size_t length = string_list_length((const char**)list);
  
  *list = realloc(*list, (length+2)*sizeof(char*));
  string_init_copy(list[length], string);
  (*list)[length+1] = 0;
  
  return length+1;
}

size_t string_list_pop(char*** list, char** string) {
  size_t length = string_list_length((const char**)list);

  if (length > 0) {
    *string = (*list)[length-1];
    (*list)[length-1] = 0;
    
    if (length > 1)
      *list = realloc(*list, (length)*sizeof(char*));
    else
      string_list_destroy(list);
  
    return length-1;
  }
  else {
    *string = 0;
    return 0;
  }
}

size_t string_list_reverse(char** list) {
  size_t length = string_list_length((const char**)list);
  
  if (length) {
    size_t i = 0, j = length-1;
    for ( ; i < j; ++i, --j) {
      char* swap = list[i];
      list[i] = list[j];
      list[j] = swap;
    }
  }
  
  return length;
}

size_t string_list_join(const char** list, char** joint, const char*
    delimiter) {
  size_t joint_length = 0;

  if (list) {
    size_t i, delim_length = string_length(delimiter);

    for (i = 0; list[i]; ++i) {
      if (i)
        joint_length += delim_length;
      joint_length += string_length(list[i]);
    }
    
    if (joint_length) {
      *joint = realloc(*joint, joint_length+1);
      char* start = *joint;
      
      for (i = 0; list[i]; ++i) {
        if (i)
          start += sprintf(start, "%s", delimiter);
        start += sprintf(start, "%s", list[i]);
      }
    }
    else
      string_destroy(joint);
  }

  return joint_length;
}

void string_list_print(FILE* stream, const char** list) {
  if (list) {
    size_t i = 0;
    
    while (list[i]) {
      fprintf(stream, i ? ", %s" : "%s", list[i]);
      ++i;
    }
  }
}
