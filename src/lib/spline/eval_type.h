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

#ifndef SPLINE_EVAL_TYPE_H
#define SPLINE_EVAL_TYPE_H

/** \file spline/eval_type.h
  * \ingroup spline
  * \brief Definition of the spline evaluation type
  * \author Ralf Kaestner
  * 
  * The spline evaluation type determines the evaluation of a spline
  * in terms of its base function, its first derivative, or its second
  * derivative.
  */

/** \brief Spline evaluation type
  */
typedef enum {
  spline_eval_type_base_function,       //!< Evaluate base function.
  spline_eval_type_first_derivative,    //!< Evaluate first derivative.
  spline_eval_type_second_derivative,   //!< Evaluate second derivative.
} spline_eval_type_t;

#endif
