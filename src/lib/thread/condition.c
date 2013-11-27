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

#include <math.h>
#include <errno.h>

#include <time.h>
#include <sys/time.h>

#include "condition.h"

const char* thread_condition_errors[] = {
  "Success",
  "Mutex operation error",
  "Wait operation timed out",
};

void thread_condition_init(thread_condition_t* condition) {
  pthread_cond_init(&condition->handle, 0);
  thread_mutex_init(&condition->mutex);
}

void thread_condition_destroy(thread_condition_t* condition) {
  pthread_cond_destroy(&condition->handle);
  thread_mutex_destroy(&condition->mutex);
}

void thread_condition_signal(thread_condition_t* condition) {
  pthread_cond_signal(&condition->handle);
}

void thread_condition_lock(thread_condition_t* condition) {
  thread_mutex_lock(&condition->mutex);
}

void thread_condition_unlock(thread_condition_t* condition) {
  thread_mutex_unlock(&condition->mutex);
}

int thread_condition_wait(thread_condition_t* condition, double timeout) {
  int result = THREAD_CONDITION_ERROR_NONE;

  if (timeout < 0.0) {
    if (pthread_cond_wait(&condition->handle, &condition->mutex.handle))
      result = THREAD_CONDITION_ERROR_MUTEX;
  }
  else {
    struct timeval now;
    gettimeofday(&now, 0);

    double seconds = now.tv_sec+now.tv_usec/1e6+timeout;

    struct timespec time;
    time.tv_sec = (time_t)seconds;
    time.tv_nsec = (seconds-(time_t)seconds)*1e9;
    
    int error = pthread_cond_timedwait(&condition->handle,
      &condition->mutex.handle, &time);
    if (error == ETIMEDOUT)
      result = THREAD_CONDITION_ERROR_WAIT_TIMEOUT;
    else if (error)
      result = THREAD_CONDITION_ERROR_MUTEX;
  }

  return result;
}
