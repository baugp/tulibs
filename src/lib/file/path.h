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

#ifndef FILE_PATH_H
#define FILE_PATH_H

#include <stdlib.h>

/** \file path.h
  * \brief Simple file path-related functions
  * \author Ralf Kaestner
  * POSIX-compliant file path-related functions.
  */

/** \brief Check if path exists
  * \note This function is ignorant about the type of file the path is
  *   pointing to.
  * \param[in] path The path to be checked for existence.
  * \return One if the path exists and zero otherwise.
  */
int file_path_exits(
  const char* path);

/** \brief Check if the path points to a regular file
  * \param[in] path The path to be checked.
  * \return One if the path points to a regular file and zero otherwise.
  */
int file_path_is_file(
  const char* path);

/** \brief Check if the path points to a directory
  * \param[in] path The path to be checked.
  * \return One if the path points to a directory and zero otherwise.
  */
int file_path_is_directory(
  const char* path);

#endif
