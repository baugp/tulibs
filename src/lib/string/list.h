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

#ifndef STRING_LIST_H
#define STRING_LIST_H

#include <unistd.h>

/** \file string/list.h
  * \ingroup string
  * \brief Simple string list interface
  * \author Ralf Kaestner
  * 
  * The string list interface offers basic support for managing the memory
  * of string arrays. In this implementation, the string array is terminated
  * by a null pointer.
  */

/** \brief Initialize a list of empty strings
  * \param[in,out] list The string list to be initialized. On return, the
  *   initialized string list will have the specified length or be null if
  *   the length is zero.
  * \param[in] length The initial length of the string list.
  * 
  * This initializers allocates length+1 string pointers of memory and
  * initializes the list with null pointers.
  */
void string_list_init(
  char*** list,
  size_t length);

/** \brief Initialize string list by copying
  * \param[in,out] list The string list to be initialized. On return, the
  *   initialized string list will contain a copy of the source string list
  *   or be null if the source string list is empty.
  * \param[in] src_list The source string list used to initialize the string
  *   list.
  * \return The length of the initialized string list.
  */
size_t string_list_init_copy(
  char*** list,
  const char** src_list);

/** \brief Destroy string list
  * \param[in,out] list The string list to be destroyed. If the passed string
  *   list is null, this function will do nothing. On return, the string list
  *   will be null.
  * 
  * The string list is destroyed by destroying the strings and freeing
  * its memory.
  */
void string_list_destroy(
  char*** list);

/** \brief Retrieve the length of a string list
  * \param[in] list The string list to retrieve the length for. 
  * \return The length of the string list. For null string lists, a length of
  *   zero will be returned.
  */
size_t string_list_length(
  const char** list);

/** \brief Check if string list is empty
  * \param[in] list The string list to be checked for emptiness.
  * \return One if the string list is empty and zero otherwise.
  * 
  * An empty string list may either be null or have length zero.
  */
int string_list_empty(
  const char** list);

/** \brief Search a string list
  * \param[in] list The string list to be searched.
  * \param[in] string The string to be found in the list.
  * \return The index of the string found in the list or -1 if the string
  *   could not be found.
  * 
  * The string list will be searched from the beginning to the end.
  */
ssize_t string_list_find(
  const char** list,
  const char* string);

/** \brief Reversely search a string list
  * \param[in] list The string list to be searched.
  * \param[in] string The string to be found in the list.
  * \return The index of the string found in the list or -1 if the string
  *   could not be found.
  * 
  * The string list will be searched from the end to the beginning.
  */
ssize_t string_list_rfind(
  const char** list,
  const char* string);

/** \brief Copy string list
  * \param[in,out] dst The destination string list.
  * \param[in] src The source string list.
  * \return The new length of the destination string list.
  * 
  * The destination string list memory will be re-allocated to have
  * sufficient size for accommodating a copy of the source string list.
  */
size_t string_list_copy(
  char*** dst,
  const char** src);

/** \brief Append string to string list
  * \param[in,out] list The string list to which to append the string.
  * \param[in] string The string to be appended to the string list.
  * \return The new length of the string list.
  * 
  * The string list memory will be re-allocated to have sufficient size
  * for accommodating a copy of the appended string.
  */
size_t string_list_push(
  char*** list,
  const char* string);

/** \brief Remove string from end of string list
  * \param[in,out] list The string list from which to remove the last
  *   string.
  * \param[in,out] string The string removed from the end of the string
  *   list or null if the string list is empty.
  * \return The new length of the string list.
  * 
  * The destination string list memory will be re-allocated to release
  * the storage previously used by the removed string.
  */
size_t string_list_pop(
  char*** list,
  char** string);

/** \brief Reverse string list
  * \param[in,out] list The string list to be reversed.
  * \return The old and new length of the reversed string list.
  */
size_t string_list_reverse(
  char** list);

/** \brief Join string list
  * \param[in] list The string list to be joined.
  * \param[in,out] joint The joint string. On return, the joint string
  *   will be the string resulting from the concatenation of all strings in
  *   the list, separated by the given delimiter.
  * \param[in] delimiter The optional delimiter string which will separate
  *   the string list elements in the joint string.
  * \return The length of the joint string.
  * 
  * The string memory will be re-allocated to have sufficient size for
  * accommodating the joint string.
  */
size_t string_list_join(
  const char** list,
  char** joint,
  const char* delimiter);

/** \brief Print string list
  * \param[in] stream The output stream that will be used for printing the
  *   string list.
  * \param[in] list The string list that will be printed.
  */
void string_list_print(
  FILE* stream,
  const char** list);

#endif
