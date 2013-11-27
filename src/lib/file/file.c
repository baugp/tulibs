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

#include <stdio.h>
#include <stdarg.h>
#include <sys/stat.h>

#include <zlib.h>
#include <bzlib.h>

#include "file.h"
#include "path.h"

#include "string/string.h"

const char* file_errors[] = {
  "Success",
  "No such file",
  "Failed to attain file position",
  "Failed to open file",
  "Failed to read from file",
  "Failed to write to file",
  "Failed to flush file",
  "Illegal file operation",
};

const char* file_modes[] = {
  "r",
  "w",
  "a",
};

void file_init(file_t* file, const char* filename, file_compression_t
    compression) {
  string_init_copy(&file->name, filename);
  file->handle = 0;
  
  file->compression = compression;
  file->pos = -1;
  
  error_init(&file->error, file_errors);
}

void file_init_name(file_t* file, const char* filename) {
  file_init(file, filename, file_compression_none);
  
  if (string_ends_with(file->name, ".gz"))
    file->compression = file_compression_gzip;
  else if (string_ends_with(file->name, ".bz2"))
    file->compression = file_compression_bzip2;
}

void file_destroy(file_t* file) {
  if (file->handle)
    file_close(file);
  
  string_destroy(&file->name);
  error_destroy(&file->error);
}

int file_exists(const file_t* file) {
  return file_path_is_file(file->name);
}

const char* file_get_extension(const file_t* file) {
  const char* extension_start = string_rfind(file->name, ".");
  return extension_start ? extension_start+1 : 0;
}

ssize_t file_get_size(const file_t* file) {  
  void* handle;
  
  switch (file->compression) {
    case file_compression_gzip:
      if (!file_exists(file))
        return 0;
      
      handle = fopen(file->name, file_modes[file_mode_read]);
      if (handle) {
        unsigned int size;        
        
        if (fseek(handle, -sizeof(size), SEEK_END) < 0) {
          fclose(handle);
          return 0;
        }

        if (fread(&size, sizeof(size), 1, handle) == sizeof(size)) {
          fclose(handle);
          return size;
        }
        else {
          fclose(handle);
          return 0;
        }
      }
      else
        return 0;
    case file_compression_bzip2:
      if (!file_exists(file))
        return 0;

      handle = BZ2_bzopen(file->name, file_modes[file_mode_read]);
      if (handle) {
        char buffer[4096];
        size_t size = 0;
        ssize_t result;
        int error = BZ_OK;
        
        do {
          result = BZ2_bzRead(&error, handle, buffer, sizeof(buffer));
          if (result > 0)
            size += result;

          if ((result != sizeof(buffer)) && (error != BZ_STREAM_END)) {
            BZ2_bzclose(handle);
            return 0;
          }
        }
        while (error != BZ_STREAM_END);
        
        BZ2_bzclose(handle);

        return size;
      }
      else
        return 0;
    default:
      break;
  };
  
  return file_get_actual_size(file);
}

ssize_t file_get_actual_size(const file_t* file) {
  if (!file_exists(file))
    return 0;
  
  FILE* handle = fopen(file->name, file_modes[file_mode_read]);
  if (handle) {
    ssize_t result;
    
    if (fseek(handle, 0, SEEK_END)) {
      fclose(handle);
      return 0;
    }

    result = ftell(handle);
    fclose(handle);
    
    if (result >= 0)
      return result;
    else
      return 0;
  }
  else
    return 0;
}

int file_open(file_t* file, file_mode_t mode) {
  if (file->handle)
    file_close(file);

  error_clear(&file->error);
  
  switch (file->compression) {
    case file_compression_gzip:
      file->handle = gzopen(file->name, file_modes[mode]);
      break;
    case file_compression_bzip2:
      if ((mode == file_mode_read) || (mode == file_mode_write)) {
        file->handle = BZ2_bzopen(file->name, file_modes[mode]);
        if (file->handle)
          file->pos = 0;
      }
      break;
    default:
      file->handle = fopen(file->name, file_modes[mode]);
  }

  if (!file->handle)
    error_setf(&file->error, FILE_ERROR_OPEN, file->name);
  
  return error_get(&file->error);
}

int file_open_stream(file_t* file, FILE* stream, file_mode_t mode) {
  error_clear(&file->error);
  
  int fd = dup(fileno(stream));
  
  switch (file->compression) {
    case file_compression_gzip:
      file->handle = gzdopen(fd, file_modes[mode]);
      break;
    case file_compression_bzip2:
      if ((mode == file_mode_read) || (mode == file_mode_write)) {
        file->handle = BZ2_bzdopen(fd, file_modes[mode]);
        if (file->handle)
          file->pos = 0;
      }
      break;
    default:
      file->handle = fdopen(fd, file_modes[mode]);
  }

  if (!file->handle)
    error_setf(&file->error, FILE_ERROR_OPEN, file->name);

  return error_get(&file->error);  
}

void file_close(file_t* file) {
  if (!file->handle)
    return;

  switch (file->compression) {
    case file_compression_gzip:
      gzclose(file->handle);
      break;
    case file_compression_bzip2:
      BZ2_bzclose(file->handle);
      break;
    default:
      fclose(file->handle);
  }
  
  file->handle = 0;
  file->pos = -1;
}

int file_eof(const file_t* file) {
  if (file->handle) {
    int error;
    
    switch (file->compression) {
      case file_compression_gzip:
        return (gzeof(file->handle) == 1);
      case file_compression_bzip2:
        BZ2_bzerror(file->handle, &error);
        return (error == BZ_STREAM_END);
      default:
        return (feof(file->handle) != 0);
    }
  }
  
  return 0;
}

int file_error(const file_t* file) {
  if (file->handle) {
    int error;
    
    switch (file->compression) {
      case file_compression_gzip:
        gzerror(file->handle, &error);
        return (error != Z_OK);
      case file_compression_bzip2:
        BZ2_bzerror(file->handle, &error);
        return (error != BZ_OK);
      default:
        return (ferror(file->handle) != 0);
    }
  }
  
  return 0;
}

ssize_t file_seek(file_t* file, ssize_t offset, file_whence_t whence) {
  if (!file->handle) {
    error_set(&file->error, FILE_ERROR_OPERATION);
    return -error_get(&file->error);
  }

  error_clear(&file->error);
  
  int whence_int;
  switch (whence) {
    case file_whence_end:
      whence_int = SEEK_END;
      break;
    case file_whence_current:
      whence_int = SEEK_CUR;
      break;
    default:
      whence_int = SEEK_SET;
  };
  
  ssize_t pos, result;
  switch (file->compression) {
    case file_compression_gzip:
      if ((result = gzseek(file->handle, offset, whence_int)) < 0) {
        error_set(&file->error, FILE_ERROR_SEEK);
        return -error_get(&file->error);
      }
      break;
    case file_compression_bzip2:
      switch (whence) {
        case file_whence_end:
          pos = file_get_size(file)+offset;
          break;
        case file_whence_current:
          pos = file->pos+offset;
          break;
        default:
          pos = offset;
      };
      
      if (pos >= file->pos) {
        int error = BZ_OK;
        char buffer[4096];
        ssize_t num_read;

        while ((file->pos < pos) && (error == BZ_OK)) {
          num_read = BZ2_bzRead(&error, file->handle, buffer,
            sizeof(buffer) < pos-file->pos ? sizeof(buffer) : pos-file->pos);
          if (num_read > 0)
            file->pos += num_read;
        }
          
        if (error != BZ_OK) {
          error_setf(&file->error, FILE_ERROR_READ, file->name);
          return -error_get(&file->error);
        }
        else
          result = file->pos;
      }
      else {
        error_set(&file->error, FILE_ERROR_SEEK);
        return -error_get(&file->error);
      }
        
      break;
    default:
      if (!fseek(file->handle, offset, whence_int)) {
        if ((result = ftell(file->handle)) < 0) {
          error_set(&file->error, FILE_ERROR_SEEK);
          return -error_get(&file->error);
        }
      }
      else {
        error_set(&file->error, FILE_ERROR_SEEK);
        return -error_get(&file->error);
      }
  }

  return result;
}

ssize_t file_tell(const file_t* file) {
  if (file->handle) {
    ssize_t result;
    
    switch (file->compression) {
      case file_compression_gzip:
        if ((result = gztell(file->handle)) >= 0)
          return result;
        break;
      case file_compression_bzip2:
        return file->pos;
      default:
        if ((result = ftell(file->handle)) >= 0)
          return result;
    }
  }
  
  return -1;
}

ssize_t file_read(file_t* file, unsigned char* data, size_t size) {
  if (!file->handle) {
    error_set(&file->error, FILE_ERROR_OPERATION);
    return -error_get(&file->error);
  }

  error_clear(&file->error);
  
  ssize_t result;
  switch (file->compression) {
    case file_compression_gzip:
      if ((result = gzread(file->handle, data, size)) <= 0) {
        error_setf(&file->error, FILE_ERROR_READ, file->name);
        return -error_get(&file->error);
      }
      break;
    case file_compression_bzip2:
      if ((result = BZ2_bzread(file->handle, data, size)) <= 0) {
        error_setf(&file->error, FILE_ERROR_READ, file->name);
        return -error_get(&file->error);
      }
      else
        file->pos += result;
      break;
    default:
      if (!(result = fread(data, size, 1, file->handle)) &&
          ferror(file->handle)) {
        error_setf(&file->error, FILE_ERROR_READ, file->name);
        return -error_get(&file->error);
      }
  }
  
  return result;
}

ssize_t file_write(file_t* file, const unsigned char* data, size_t size) {
  if (!file->handle) {
    error_set(&file->error, FILE_ERROR_OPERATION);
    return -error_get(&file->error);
  }

  error_clear(&file->error);
  
  ssize_t result;
  switch (file->compression) {
    case file_compression_gzip:
      if (!(result = gzwrite(file->handle, data, size))) {
        error_setf(&file->error, FILE_ERROR_WRITE, file->name);
        return -error_get(&file->error);
      }
      break;
    case file_compression_bzip2:
      if (!(result = BZ2_bzwrite(file->handle, (unsigned char*)data, size))) {
        error_setf(&file->error, FILE_ERROR_WRITE, file->name);
        return -error_get(&file->error);
      }
      else
        file->pos += result;
      break;
    default:
      if (!(result = fwrite(data, size, 1, file->handle))) {
        error_setf(&file->error, FILE_ERROR_WRITE, file->name);
        return -error_get(&file->error);
      }
  }
  
  return result;
}

ssize_t file_read_line(file_t* file, char** line, size_t block_size) {
  ssize_t line_length = 0;
  unsigned char character;
  ssize_t result;

  while ((result = file_read(file, &character, 1) == 1)) {
    if (character == '\n')
      break;
    
    if (line_length % block_size == 0)
      *line = realloc(*line, line_length+block_size);
    (*line)[line_length] = character;
    ++line_length;
  }
  
  if (result >= 0) {
    if (line_length && (line_length % block_size == 0))
      *line = realloc(*line, line_length+block_size);
    (*line)[line_length] = 0;
      
    return line_length;
  }
  else
    return result;
}

ssize_t file_printf(file_t* file, const char* format, ...) {
  if (!file->handle) {
    error_set(&file->error, FILE_ERROR_OPERATION);
    return -error_get(&file->error);
  }

  error_clear(&file->error);
  
  va_list vargs;  
  ssize_t result;  
  if ((file->compression == file_compression_gzip) ||
      (file->compression == file_compression_bzip2)) {
    size_t size = 256;
    while (1) {
      char buffer[size];
      
      va_start(vargs, format);
      result = vsnprintf(buffer, size, format, vargs);
      va_end(vargs);
      
      if ((result > -1) && (result < size)) {
        result = file_write(file, (unsigned char*)buffer,
          string_length(buffer));
        break;
      }
      
      if (result > -1)
        size = result+1;
      else
        size *= 2;
    }  
  }
  else {
    va_start(vargs, format);
    if ((result = vfprintf(file->handle, format, vargs)) <= 0) {
      error_setf(&file->error, FILE_ERROR_WRITE, file->name);
      return -error_get(&file->error);
    }
    va_end(vargs);
  }
  
  return result;
}

int file_flush(file_t* file) {
 if (!file->handle) {
    error_set(&file->error, FILE_ERROR_OPERATION);
    return error_get(&file->error);
 }

  error_clear(&file->error);
  
  switch (file->compression) {
    case file_compression_gzip:
      if (gzflush(file->handle, Z_SYNC_FLUSH) != Z_OK)
        error_setf(&file->error, FILE_ERROR_FLUSH, file->name);
      break;
    case file_compression_bzip2:
      if (BZ2_bzflush(file->handle))
        error_setf(&file->error, FILE_ERROR_FLUSH, file->name);
      break;
    default:
      if (fflush(file->handle))
        error_setf(&file->error, FILE_ERROR_FLUSH, file->name);
  }
  
  return error_get(&file->error);
}
