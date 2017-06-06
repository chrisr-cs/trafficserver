/** @file

  A brief file description

  @section license License

  Licensed to the Apache Software Foundation (ASF) under one
  or more contributor license agreements.  See the NOTICE file
  distributed with this work for additional information
  regarding copyright ownership.  The ASF licenses this file
  to you under the Apache License, Version 2.0 (the
  "License"); you may not use this file except in compliance
  with the License.  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 */

/****************************************************************************

  Basic Threads



**************************************************************************/
#include "P_EventSystem.h"
#include "ts/ink_string.h"

///////////////////////////////////////////////
// Common Interface impl                     //
///////////////////////////////////////////////

static ink_thread_key init_thread_key();

ink_hrtime Thread::cur_time                       = ink_get_hrtime_internal();
inkcoreapi ink_thread_key Thread::thread_data_key = init_thread_key();

Thread::Thread()
{
  mutex = new_ProxyMutex();
  MUTEX_TAKE_LOCK(mutex, (EThread *)this);
  mutex->nthread_holding += THREAD_MUTEX_THREAD_HOLDING;
}

Thread::~Thread()
{
  ink_release_assert(mutex->thread_holding == (EThread *)this);
  mutex->nthread_holding -= THREAD_MUTEX_THREAD_HOLDING;
  MUTEX_UNTAKE_LOCK(mutex, (EThread *)this);
}

ink_thread_key
init_thread_key()
{
  ink_thread_key_create(&Thread::thread_data_key, nullptr);
  return Thread::thread_data_key;
}

///////////////////////////////////////////////
// Unix & non-NT Interface impl              //
///////////////////////////////////////////////

struct thread_data_internal {
  ThreadFunction f;                  ///< Function to excecute in the thread.
  Thread *me;                        ///< The class instance.
  char name[MAX_THREAD_NAME_LENGTH]; ///< Name for the thread.
};

static void *
spawn_thread_internal(void *a)
{
  auto *p = static_cast<thread_data_internal *>(a);

  p->me->set_specific();
  ink_set_thread_name(p->name);

  if (p->f) {
    p->f();
  } else {
    p->me->execute();
  }

  delete p;
  return nullptr;
}

ink_thread
Thread::start(const char *name, void *stack, size_t stacksize, ThreadFunction const &f)
{
  auto *p = new thread_data_internal{f, this, ""};

  ink_zero(p->name);
  ink_strlcpy(p->name, name, MAX_THREAD_NAME_LENGTH);
  if (stacksize == 0) {
    stacksize = DEFAULT_STACKSIZE;
  }
  tid = ink_thread_create(spawn_thread_internal, p, 0, stacksize, stack);

  return tid;
}
