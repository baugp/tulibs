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

#ifndef TIMER_H
#define TIMER_H

/** \defgroup timer Timer Module
  * \brief Library functions for working with timers
  * 
  * The timer module provides basic library functions for representing,
  * measuring, and delaying time.
  */

/** \file timer/timer.h
  * \ingroup timer
  * \brief Simple timer implementation
  * \author Ralf Kaestner
  * 
  * A timer is a useful tool in time-variant applications. This
  * implementation specifically targets periodic tasks which require
  * to measure and delay time in order to ensure relatively constant
  * frequencies.
  */

/** \name Error Codes
  * \brief Predefined timer error codes
  */
//@{
#define TIMER_ERROR_NONE              0
//!< Success
#define TIMER_ERROR_FAULT             1
//!< Timer fault
//@}

/** \brief Predefined timer error descriptions
  */
extern const char* timer_errors[];

/** \brief Start the timer
  * \param[out] timestamp The timestamp that will contain the start time.
  */
void timer_start(
  double* timestamp);

/** \brief Correct the start time of a timer by averaging
  * \param[in] timestamp The timestamp containing the timer's start time.
  */
void timer_correct(
  double* timestamp);

/** \brief Stop the timer and return the elapsed time
  * \param[in] timestamp The timestamp containing the timer's start time.
  * \return The ellapsed time in [s].
  */
double timer_stop(
  double timestamp);

/** \brief Get the timer frequency
  * \param[in] timestamp The timestamp containing the timer's start time.
  * \return The timer frequency in [Hz].
  */
double timer_get_frequency(
  double timestamp);

/** \brief Wait for the expiration of the timer
  * \param[in] timestamp The timestamp containing the timer's start time.
  * \param[in] frequency The frequency that corresponds to the timer's
  *   expiration period.
  * \return The resulting error code. If the given frequency cannot be
  *   reached by the timer, an TIMER_ERROR_FAULT will be returned.
  */
int timer_wait(
  double timestamp,
  double frequency);

/** \brief Sleep for a specified amount of time
  * \param[in] seconds The sleep duration in [s].
  * \return The resulting error code.
  */
int timer_sleep(
  double seconds);

#endif
