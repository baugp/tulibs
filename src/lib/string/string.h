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

#ifndef STRING_H
#define STRING_H

#include <unistd.h>
#include <stdarg.h>

/** \defgroup string String Operations Module
  * \brief Library functions for string operations
  * 
  * The string operations module provides library functions for managing
  * string memory and supports the most common string manipulations.
  */

/** \file string/string.h
  * \ingroup string
  * \brief Simple string handling interface
  * \author Ralf Kaestner
  * 
  * In addition to standard string operations, this implementation provides
  * safe manipulations of dynamically-sized strings through a simple, unified
  * programming interface.
  */

/** \brief Initialize an empty string
  * \param[in,out] string The string to be initialized. On return, the
  *   initialized string will have the specified length or be null if
  *   the length is zero.
  * \param[in] length The initial length of the string.
  * 
  * This initializers allocates length+1 bytes of memory and sets the
  * terminating null character.
  */
void string_init(
  char** string,
  size_t length);

/** \brief Initialize string by copying
  * \param[in,out] string The string to be initialized. On return, the
  *   initialized string will contain a copy of the source string or be
  *   null if the source string is empty.
  * \param[in] src_string The source string used to initialize the string.
  * \return The length of the initialized string.
  */
size_t string_init_copy(
  char** string,
  const char* src_string);

/** \brief Destroy string
  * \param[in,out] string The string to be destroyed. If the passed string
  *   is null, this function will do nothing. On return, the string will
  *   be null.
  * 
  * The string is destroyed by freeing its memory.
  */
void string_destroy(
  char** string);

/** \brief Retrieve the length of a string
  * \param[in] string The string to retrieve the length for. 
  * \return The length of the string. For null strings, a length of zero
  *   will be returned.
  */
size_t string_length(
  const char* string);

/** \brief Check if string is empty
  * \param[in] string The string to be checked for emptiness.
  * \return One if the string is empty and zero otherwise.
  * 
  * An empty string may either be null or have length zero.
  */
int string_empty(
  const char* string);

/** \brief Compare strings
  * \note Two empty strings will compare to equal.
  * \param[in] string The string to be compared against another string.
  * \param[in] other_string Another string to be compared against the string.
  * \return One if the strings are equal and zero otherwise.
  */
int string_equal(
  const char* string,
  const char* other_string);

/** \brief Compare strings ignoring character case
  * \note Two empty strings will compare to equal.
  * \param[in] string The string to be compared against another string.
  * \param[in] other_string Another string to be compared against the string.
  * \return One if the strings are equal, ignoring the case of their
  *   characters, and zero otherwise.
  */
int string_equal_ignore_case(
  const char* string,
  const char* other_string);

/** \brief Compare first characters of string
  * \note An empty prefix will compare to equal.
  * \param[in] string The string whose first characters will be compared
  *   against the prefix.
  * \param[in] prefix Another string to be compared against the first
  *   characters of the string.
  * \return One if the string starts with the given prefix and zero otherwise.
  */
int string_starts_with(
  const char* string,
  const char* prefix);

/** \brief Compare first characters of string ingoring their case
  * \note An empty prefix will compare to equal.
  * \param[in] string The string whose first characters will be compared
  *   against the prefix.
  * \param[in] prefix Another string to be compared against the first
  *   characters of the string.
  * \return One if the string starts with the given prefix, ignoring the
  *   case of its characters, and zero otherwise.
  */
int string_starts_with_ignore_case(
  const char* string,
  const char* prefix);

/** \brief Compare last characters of string
  * \note An empty suffix will compare to equal.
  * \param[in] string The string whose last characters will be compared
  *   against the suffix.
  * \param[in] suffix Another string to be compared against the last
  *   characters of the string.
  * \return One if the string ends with the given suffix and zero otherwise.
  */
int string_ends_with(
  const char* string,
  const char* suffix);

/** \brief Compare last characters of string ignoring their case
  * \note An empty suffix will compare to equal.
  * \param[in] string The string whose last characters will be compared
  *   against the suffix.
  * \param[in] suffix Another string to be compared against the last
  *   characters of the string.
  * \return One if the string ends with the given suffix, ignoring the
  *   case of its characters, and zero otherwise.
  */
int string_ends_with_ignore_case(
  const char* string,
  const char* suffix);

/** \brief Search a string
  * \param[in] string The string to be searched.
  * \param[in] substring The substring to be found.
  * \return The search result or null if the substring could not be found.
  * 
  * The string will be searched from the beginning to the end.
  */
const char* string_find(
  const char* string,
  const char* substring);

/** \brief Reversely search a string
  * \param[in] string The string to be searched.
  * \param[in] substring The substring to be found.
  * \return The search result or null if the substring could not be found.
  * 
  * The string will be searched from the end to the beginning.
  */
const char* string_rfind(
  const char* string,
  const char* substring);

/** \brief Copy string
  * \param[in,out] dst The destination string.
  * \param[in] src The source string.
  * \return The new length of the destination string.
  * 
  * The destination string memory will be re-allocated to have sufficient
  * size for accommodating a copy of the source string.
  */
size_t string_copy(
  char** dst,
  const char* src);

/** \brief Convert string to upper casing
  * \param[in,out] string The string to be converted to upper casing.
  * \return The old and new length of the converted string.
  */
size_t string_upper(
  char* string);

/** \brief Convert string to lower casing
  * \param[in,out] string The string to be converted to lower casing.
  * \return The old and new length of the converted string.
  */
size_t string_lower(
  char* string);

/** \brief Reverse string
  * \param[in,out] string The string to be reversed.
  * \return The old and new length of the reversed string.
  */
size_t string_reverse(
  char* string);

/** \brief Concatenate strings
  * \param[in,out] prefix The prefix string. On return, the prefix string
  *   will be the string resulting from the concatenation of prefix and
  *   suffix.
  * \param[in] suffix The suffix string.
  * \return The new length of the prefix string.
  * 
  * The prefix string memory will be re-allocated to have sufficient
  * size for accommodating the concatenation of prefix and suffix.
  */
size_t string_cat(
  char** prefix,
  const char* suffix);

/** \brief Split string into list of strings
  * \param[in] string The joint string to be split.
  * \param[in,out] list The list of split strings. On return, the list
  *   will contain the strings resulting from splitting the joint string
  *   at the delimiters.
  * \param[in] delimiter The delimiter string which separates the split
  *   strings in the joint string.
  * \return The number of elements in the list of split strings.
  * 
  * The string list memory will be re-allocated to have sufficient size for
  * accommodating the split strings.
  */
size_t string_split(
  const char* string,
  char*** list,
  const char* delimiter);

/** \brief Read formatted input from string
  * \param[in] string The string to read the formatted input from.
  * \param[in] format A string defining the expected format and conversion
  *   specifiers of the input. This string must be followed by a variadic
  *   list of pointer arguments, where each pointer refers to an argument of
  *   appropriate type.
  * \return The number of arguments read from the string.
  */
size_t string_scanf(
  const char* string,
  const char* format,
  ...);

/** \brief Read formatted input from string
  * \param[in] string The string to read the formatted input from.
  * \param[in] format A string defining the expected format and conversion
  *   specififiers of the input.
  * \param[in,out] vargs  A list of variadic pointer arguments, where each
  *   pointer refers to an argument of appropriate type with respect to the
  *   requested input format.
  * \return The number of arguments read from the string.
  * 
  * This function is equivalent to string_scanf(), except that it expects
  * a va_list instead of a variadic list of arguments.
  */
size_t string_vscanf(
  const char* string,
  const char* format,
  va_list vargs);

/** \brief Print formatted output to string
  * \param[in,out] string The string to receive the formatted output.
  * \param[in] format A string defining the expected format and conversion
  *   specififiers of the output. This string must be followed by a variadic
  *   list of arguments, where each argument is of appropriate type.
  * \return The new length of the formatted string.
  * 
  * The string memory will be re-allocated to have sufficient size for
  * accommodating the formatted output.
  */
size_t string_printf(
  char** string,
  const char* format,
  ...);

/** \brief Print formatted output to string
  * \param[in,out] string The string to receive the formatted output.
  * \param[in] format A string defining the expected format and conversion
  *   specifiers of the output.
  * \param[in] vargs  A list of variadic arguments, where each arguments is
  *   of appropriate type with respect to the requested output format.
  * \return The new length of the formatted string.
  * 
  * This function is equivalent to string_printf(), except that it expects
  * a va_list instead of a variadic list of arguments.
  */
size_t string_vprintf(
  char** string,
  const char* format,
  va_list vargs);

/** \brief Justify string to not exceed a specified output width
  * \param[in,out] string The string that will be justified in place by
  *   substituting whitespace characters.
  * \param[in] line_length The maximum line length of the justified string.
  * \return The actual maximum line length of the justified string.
  * 
  * This is a string formatting helper function to generate multi-line
  * strings suitable for limited character width stream output.
  */
size_t string_justify(
  char* string,
  size_t line_length);

#endif
