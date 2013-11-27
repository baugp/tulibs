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

#include "thread.h"

#include "timer/timer.h"

const char* thread_errors[] = {
  "Success",
  "Error creating thread",
  "Wait operation timed out",
  "State error",
};

int thread_start(thread_t* thread, void* (*thread_routine)(void*),
  void (*thread_cleanup)(void*), void* thread_arg, double frequency) {
  int result = THREAD_ERROR_NONE;
  
  thread->routine = thread_routine;
  thread->cleanup = thread_cleanup;
  thread->arg = thread_arg;

  thread_condition_init(&thread->condition);

  thread->frequency = frequency;
  thread->start_time = 0.0;
  thread->state = thread_state_stopped;

  thread->exit_request = 0;

  thread_condition_lock(&thread->condition);
  if (thread->state == thread_state_stopped) {
    if (!pthread_create(&thread->thread, NULL, thread_run, thread))
      thread_condition_wait(&thread->condition, THREAD_CONDITION_WAIT_FOREVER);
    else
      result = THREAD_ERROR_CREATE;
  }
  else
    result = THREAD_ERROR_STATE;
  thread_condition_unlock(&thread->condition);
  
  return result;
}

int thread_exit(thread_t* thread, int wait) {
  int result = THREAD_ERROR_NONE;
  
  thread_condition_lock(&thread->condition);
  if (thread->state == thread_state_running)
    thread->exit_request = 1;
  else
    result = THREAD_ERROR_STATE;
  thread_condition_unlock(&thread->condition);

#ifdef HAVE_LIBGCC_S
  pthread_cancel(thread->thread);
#endif

  if (wait)
    thread_wait_exit(thread);

  return result;
}

void thread_self_exit() {
  pthread_exit(0);
}

void thread_cleanup(void* arg) {
  thread_t* thread = arg;

  if (thread->cleanup)
    thread->cleanup(thread->arg);

  thread_condition_lock(&thread->condition);
  thread->state = thread_state_stopped;
  thread_condition_signal(&thread->condition);
  thread_condition_unlock(&thread->condition);

  thread_condition_destroy(&thread->condition);
}

void* thread_run(void* arg) {
  thread_t* thread = arg;
  void* result = 0;

  pthread_cleanup_push(thread_cleanup, thread);

#ifdef HAVE_LIBGCC_S
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, 0);
#else
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0);
#endif 

  thread_condition_lock(&thread->condition);
  thread->state = thread_state_running;
  thread_condition_signal(&thread->condition);
  thread_condition_unlock(&thread->condition);
  
  timer_start(&thread->start_time);

  if (thread->frequency > 0.0) {
    while (!thread_test_exit(thread)) {
      double timestamp;
      timer_start(&timestamp);

      result = thread->routine(thread->arg);

      timer_wait(timestamp, thread->frequency);
    }
  }
  else
    result = thread->routine(thread->arg);

  pthread_cleanup_pop(1);

  return result;
}

int thread_test_exit(thread_t* thread) {
  int result;

  thread_condition_lock(&thread->condition);
  result = thread->exit_request;
  thread_condition_unlock(&thread->condition);

  return result;
}

void thread_self_test_exit() {
  pthread_testcancel();
}

void thread_wait_exit(thread_t* thread) {
  pthread_join(thread->thread, NULL);
}

int thread_wait(thread_t* thread, double timeout) {
  int result = THREAD_ERROR_NONE;
  
  thread_condition_lock(&thread->condition);
  if (thread->state == thread_state_running) {
    if (thread_condition_wait(&thread->condition, timeout))
      result = THREAD_ERROR_WAIT_TIMEOUT;
  }
  else
    result = THREAD_ERROR_STATE;
  thread_condition_unlock(&thread->condition);

  return result;
}
