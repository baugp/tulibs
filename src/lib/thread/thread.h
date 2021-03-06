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

#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

#include "thread/mutex.h"
#include "thread/condition.h"

/** \defgroup thread Threading Module
  * \brief Library functions for managing threads, mutexes, and conditions
  * 
  * The thread module provides library functions for working with
  * light-weight processes (so-called threads). It implements the means
  * to safely accessing memory objects in multi-threaded applications as
  * well as the basic tools of thread concurrency.
  */

/** \file thread/thread.h
  * \ingroup thread
  * \brief POSIX-compliant thread implementation
  * \author Ralf Kaestner
  * 
  * This simple thread interface mainly aims to support the implementation
  * of periodic worker tasks, e.g., in sensor acquisition or control
  * applications.
  */

/** \name Error Codes
  * \brief Predefined thread handling error codes
  */
//@{
#define THREAD_ERROR_NONE              0
//!< Success
#define THREAD_ERROR_CREATE            1
//!< Error creating thread
#define THREAD_ERROR_WAIT_TIMEOUT      2
//!< Wait operation timed out
#define THREAD_ERROR_STATE             3
//!< State error
//@}

/** \brief Predefined thread handling error descriptions
  */
extern const char* thread_errors[];

/** \brief Thread state enumerable type
  */
typedef enum {
  thread_state_stopped,          //!< Thread is stopped.
  thread_state_running,          //!< Thread is running.
} thread_state_t;

/** \brief Structure defining the thread context
  */
typedef struct thread_t {
  pthread_t thread;               //!< The thread handle.
  void* (*routine)(void*);        //!< The thread routine.
  void (*cleanup)(void*);         //!< The thread cleanup handler.
  void* arg;                      //!< The thread routine argument.

  thread_condition_t condition;   //!< The thread condition and mutex.

  double frequency;               //!< The thread cycle frequency in [Hz].
  double start_time;              //!< The thread start timestamp.
  thread_state_t state;           //!< The state of the thread.

  int exit_request;               //!< Flag signaling a pending exit request.
} thread_t;

/** \brief Start a thread
  * \param[in] thread The thread to be started.
  * \param[in] thread_routine The thread routine that will be executed
  *   within the thread.
  * \param[in] thread_cleanup The optional thread cleanup handler that will 
  *   be executed upon thread termination.
  * \param[in] thread_arg The argument to be passed on to the thread
  *   routine. The memory should be allocated by the caller and will be
  *   freed after thread termination.
  * \param[in] frequency The thread cycle frequency in [Hz]. If the frequency 
  *   is 0, the thread routine will be executed once.
  * \return The resulting error code.
  */
int thread_start(
  thread_t* thread,
  void* (*thread_routine)(void*),
  void (*thread_cleanup)(void*),
  void* thread_arg,
  double frequency);

/** \brief Exit a thread
  * \param[in] thread The thread to be cancelled.
  * \param[in] wait If 0, return instantly, wait for thread termination
  *   otherwise.
  * \return The resulting error code.
  */
int thread_exit(
  thread_t* thread,
  int wait);

/** \brief Exit the calling thread
  */
void thread_self_exit();

/** \brief Run the thread
  * \note This function is run within the thread and should never be called
  * directly.
  * \param[in] arg The arguments passed to the thread.
  * \return The result of the thread.
  */
void* thread_run(void* arg);

/** \brief Test thread for a pending exit request
  * \param[in] thread The thread to be tested for a pending exit request.
  * \return 1 if an exit request is pending, 0 otherwise.
  */
int thread_test_exit(
  thread_t* thread);

/** \brief Test the calling thread for a pending cancellation request
  */
void thread_self_test_exit();

/** \brief Exit the thread and wait for its termination
  * \param[in] thread The running thread to exit and wait for.
  */
void thread_wait_exit(
  thread_t* thread);

/** \brief Wait for thread termination
  * \param[in] thread The running thread to wait for.
  * \param[in] timeout The timeout of the wait operation in [s].
  * \return The resulting error code.
  */
int thread_wait(
  thread_t* thread,
  double timeout);

#endif
