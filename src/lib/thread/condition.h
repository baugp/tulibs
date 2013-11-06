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

#ifndef THREAD_CONDITION_H
#define THREAD_CONDITION_H

/** \file condition.h
  * \ingroup thread
  * \brief Wait condition implementation
  * \author Ralf Kaestner
  * 
  * Wait conditions are highly useful in cases where multiple threads must
  * be synchronized. They basically employ mutexes which may be signaled to
  * trigger a race of the involved threads for acquiring that mutex.
  */

#include <pthread.h>

#include "thread/mutex.h"

/** \name Constants
  * \brief Predefined wait condition constants
  */
//@{
#define THREAD_CONDITION_WAIT_FOREVER             -1.0
//@}

/** \name Error Codes
  * \brief Predefined condition handling error codes
  */
//@{
#define THREAD_CONDITION_ERROR_NONE               0
#define THREAD_CONDITION_ERROR_MUTEX              1
#define THREAD_CONDITION_ERROR_WAIT_TIMEOUT       2
//@}

/** \brief Predefined condition handling error descriptions
  */
extern const char* thread_condition_errors[];

/** \brief Structure defining the thread condition
  */
typedef struct thread_condition_t {
  pthread_cond_t handle;       //!< The condition handle.
  thread_mutex_t mutex;        //!< The condition mutex.
} thread_condition_t, *thread_condition_p;

/** \brief Initialize a condition
  * \param[in] condition The condition to be initialized.
  */
void thread_condition_init(
  thread_condition_p condition);

/** \brief Destroy a condition
  * \param[in] condition The initialized condition to be destroyed.
  */
void thread_condition_destroy(
  thread_condition_p condition);

/** \brief Signal a condition
  * \param[in] condition The initialized condition to be signaled.
  */
void thread_condition_signal(
  thread_condition_p condition);

/** \brief Lock a thread condition mutex
  * \param[in] condition The initialized thread condition to lock the
  *   mutex for.
  */
void thread_condition_lock(
  thread_condition_p condition);

/** \brief Unlock a thread condition mutex
  * \param[in] condition The initialized thread condition to unlock the
  *   mutex for.
  */
void thread_condition_unlock(
  thread_condition_p condition);

/** \brief Wait for a condition to be signaled
  * \param[in] condition The initialized condition to wait for.
  * \param[in] timeout The timeout of the wait operation in [s].
  * \return The resulting error code.
  */
int thread_condition_wait(
  thread_condition_p condition,
  double timeout);

#endif
