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

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/stat.h>

#include <zlib.h>
#include <bzlib.h>

#include "file.h"

#include "file/path.h"

const char* file_errors[] = {
  "Success",
  "No such file",
  "Failed to attain file position",
  "Failed to retrieve file position",
  "Failed to open file",
  "Failed to read from file",
  "Failed to write to file",
  "Illegal file operation",
};

const char* file_modes[] = {
  "r",
  "w+",
  "a",
};

void file_init(file_p file, const char* filename, file_compression_t
    compression) {
  strcpy(file->name, filename);
  file->handle = 0;
  
  file->compression = compression;
  file->pos = -1;
}

void file_init_name(file_p file, const char* filename) {
  file_init(file, filename, file_compression_none);
  
  const char* extension = file_get_extension(file);
  if (!strcmp(extension, "gz"))
    file->compression = file_compression_gzip;
  else if (!strcmp(extension, "bz2"))
    file->compression = file_compression_bzip2;
}

int file_exists(file_p file) {
  return file_path_is_file(file->name);
}

const char* file_get_extension(file_p file) {
  const char* extension_start = strrchr(file->name, '.');
  return extension_start ? extension_start+1 :
    &file->name[strlen(file->name)];
}

ssize_t file_get_size(file_p file) {  
  void* handle;
  
  switch (file->compression) {
    case file_compression_gzip:
      if (!file_exists(file))
        return -FILE_ERROR_NOT_FOUND;
      
      handle = fopen(file->name, file_modes[file_mode_read]);
      if (handle) {
        unsigned int size;        
        
        if (fseek(handle, -sizeof(size), SEEK_END) < 0) {
          fclose(handle);
          return -FILE_ERROR_SEEK;
        }

        if (fread(&size, sizeof(size), 1, handle) == sizeof(size)) {
          fclose(handle);
          return size;
        }
        else {
          fclose(handle);
          return -FILE_ERROR_READ;
        }
      }
      else
        return -FILE_ERROR_OPEN;
    case file_compression_bzip2:
      if (!file_exists(file))
        return -FILE_ERROR_NOT_FOUND;

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
            return -FILE_ERROR_READ;
          }
        }
        while (error != BZ_STREAM_END);
        
        BZ2_bzclose(handle);
        return size;
      }
      else
        return -FILE_ERROR_OPEN;
    default:
      break;
  };
  
  return file_get_actual_size(file);
}

ssize_t file_get_actual_size(file_p file) {
  if (!file_exists(file))
    return -FILE_ERROR_NOT_FOUND;
  
  FILE* handle = fopen(file->name, file_modes[file_mode_read]);
  if (handle) {
    ssize_t result;
    
    if (fseek(handle, 0, SEEK_END)) {
      fclose(handle);
      return -FILE_ERROR_SEEK;
    }

    result = ftell(handle);
    fclose(handle);
    
    if (result >= 0)
      return result;
    else
      return -FILE_ERROR_TELL;
  }
  else
    return -FILE_ERROR_OPEN;
}

int file_open(file_p file, file_mode_t mode) {
  if (file->handle)
    file_close(file);

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

  if (file->handle)
    return FILE_ERROR_NONE;
  else
    return FILE_ERROR_OPEN;
}

int file_open_stream(file_p file, FILE* stream, file_mode_t mode) {
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

  if (file->handle)
    return FILE_ERROR_NONE;
  else
    return FILE_ERROR_OPEN;
}

void file_close(file_p file) {
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

int file_eof(file_p file) {
  if (!file->handle)
    return -FILE_ERROR_OPERATION;

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

int file_error(file_p file) {
  if (!file->handle)
    return -FILE_ERROR_OPERATION;

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

ssize_t file_seek(file_p file, ssize_t offset, file_whence_t whence) {
  if (!file->handle)
    return -FILE_ERROR_OPERATION;

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
      if ((result = gzseek(file->handle, offset, whence_int)) < 0)
        result = -FILE_ERROR_SEEK;
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
        ssize_t result;

        while ((file->pos < pos) && (error == BZ_OK)) {
          result = BZ2_bzRead(&error, file->handle, buffer,
            sizeof(buffer) < pos-file->pos ? sizeof(buffer) : pos-file->pos);
          if (result > 0)
            file->pos += result;
        }
          
        if (error != BZ_OK)
          result = -FILE_ERROR_READ;
        else
          result = file->pos;
      }
      else
        result = -FILE_ERROR_SEEK;
        
      break;
    default:
      if (!fseek(file->handle, offset, whence_int)) {
        if ((result = ftell(file->handle)) < 0)
          result = -FILE_ERROR_TELL;
      }
      else
        result = -FILE_ERROR_SEEK;
  }
  
  return result;
}

ssize_t file_tell(file_p file) {
  if (!file->handle)
    return -FILE_ERROR_OPERATION;
  
  ssize_t result;
  switch (file->compression) {
    case file_compression_gzip:
      if ((result = gztell(file->handle)) < 0)
        result = -FILE_ERROR_SEEK;
      break;
    case file_compression_bzip2:
      result = file->pos;
      break;
    default:
      if ((result = ftell(file->handle)) < 0)
        result = -FILE_ERROR_TELL;
  }
  
  return result;
}

ssize_t file_read(file_p file, unsigned char* data, size_t size) {
  if (!file->handle)
    return -FILE_ERROR_OPERATION;

  ssize_t result;
  switch (file->compression) {
    case file_compression_gzip:
      if ((result = gzread(file->handle, data, size)) <= 0)
        result = -FILE_ERROR_READ;
      break;
    case file_compression_bzip2:
      if ((result = BZ2_bzread(file->handle, data, size)) <= 0)
        result = -FILE_ERROR_READ;
      else
        file->pos += result;
      break;
    default:
      if (!(result = fread(data, size, 1, file->handle)))
        result = -FILE_ERROR_READ;
  }
  
  return result;
}

ssize_t file_write(file_p file, const unsigned char* data, size_t size) {
  if (!file->handle)
    return -FILE_ERROR_OPERATION;

  ssize_t result;
  switch (file->compression) {
    case file_compression_gzip:
      if (!(result = gzwrite(file->handle, data, size)))
        result = -FILE_ERROR_WRITE;
      break;
    case file_compression_bzip2:
      if (!(result = BZ2_bzwrite(file->handle, (unsigned char*)data, size)))
        result = -FILE_ERROR_WRITE;
      else
        file->pos += result;
      break;
    default:
      if (!(result = fwrite(data, size, 1, file->handle)))
        result = -FILE_ERROR_WRITE;
  }
  
  return result;
}

ssize_t file_printf(file_p file, const char* format, ...) {
  if (!file->handle)
    return -FILE_ERROR_OPERATION;

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
        result = file_write(file, (unsigned char*)buffer, strlen(buffer));
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
    if ((result = vfprintf(file->handle, format, vargs)) <= 0)
      result = -FILE_ERROR_WRITE;
    va_end(vargs);
  }
  
  return result;
}

int file_flush(file_p file) {
 if (!file->handle)
    return -FILE_ERROR_OPERATION;

  switch (file->compression) {
    case file_compression_gzip:
      if (gzflush(file->handle, Z_SYNC_FLUSH) != Z_OK)
        return FILE_ERROR_WRITE;
    case file_compression_bzip2:
      if (BZ2_bzflush(file->handle))
        return FILE_ERROR_WRITE;
      break;
    default:
      if (fflush(file->handle))
        return FILE_ERROR_WRITE;
  }
  
  return FILE_ERROR_NONE;
}
