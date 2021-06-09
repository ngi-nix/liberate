/**
 * This file is part of liberate.
 *
 * Author(s): Jens Finkhaeuser <jens@finkhaeuser.de>
 *
 * Copyright (c) 2014 Unwesen Ltd.
 * Copyright (c) 2015-2021 Jens Finkhaeuser.
 *
 * This software is licensed under the terms of the GNU GPLv3 for personal,
 * educational and non-profit use. For all other uses, alternative license
 * options are available. Please contact the copyright holder for additional
 * information, stating your intended usage.
 *
 * You can find the full text of the GPLv3 in the COPYING file in this code
 * distribution.
 *
 * This software is distributed on an "AS IS" BASIS, WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.
 **/

#include <build-config.h>

#include <thread>

#include <liberate/concurrency/tasklet.h>

namespace liberate::concurrency {

namespace {

struct extended_context : public tasklet::context
{
  // These fields are set in the constructor and remain unchanged during
  // the lifetime of the tasklet - even if the the values they point to
  // may be mutable.
  tasklet *                                 the_tasklet = nullptr;
  std::shared_ptr<tasklet::task_function>   func = nullptr;
  bool                                      condition_owned = false;

  // The thread object may change. Use the condition's lock to serialize
  // access to it.
  std::thread                               the_thread = {};


  extended_context(
      tasklet * _the_tasklet,
      std::shared_ptr<tasklet::task_function> _func,
      tasklet::sleep_condition * _condition,
      bool _condition_owned)
    : tasklet::context{_condition}
    , the_tasklet(_the_tasklet)
    , func(_func)
    , condition_owned(_condition_owned)
  {
  }


  virtual ~extended_context()
  {
    if (condition_owned) {
      delete condition;
      condition = nullptr;
    }
  }

};



static void tasklet_wrapper(void * arg)
{
  extended_context * ctx = static_cast<extended_context *>(arg);
  (*(ctx->func))(*ctx);
}



inline void
send_notification(extended_context * ctx)
{
  if (ctx->condition_owned) {
    ctx->condition->condition.notify_one();
  }
  else {
    ctx->condition->condition.notify_all();
  }
}



inline void
try_joining(std::unique_ptr<tasklet::context> & tasklet_context, bool with_stop = false)
{
  auto ctx = static_cast<extended_context *>(tasklet_context.get());

  // In order to join, we need to make sure that the tasklet has clean state; it
  // has clean state if either the tasklet and thread operate on the same context
  // with the same assumptions, or if they're separate.
  //
  // First, we'll acquire a lock to figure out whether the tasklet is currently
  // running; if not, we can exit relatively fast.
  auto condition_lock = std::unique_lock(ctx->condition->mutex);

  if (!ctx->the_thread.joinable()) {
    return;
  }

  // We now know that the thread object itself is running, and we may or may
  // not need to signal it to stop. Let's do that now.
  if (with_stop) {
    ctx->running = false;
  }

  // At this point, the thread is still running - but if we were to query the
  // tasklet, it would think it's not. So we have to reset the tasklet state.
  std::unique_ptr<tasklet::context> current;
  current.reset(new extended_context{
      ctx->the_tasklet,
      ctx->func,
      ctx->condition,
      ctx->condition_owned
  });
  if (ctx->condition_owned) {
    // Actually if the context owns the condition, we want to give each context
    // its own instance.
    current->condition = new tasklet::sleep_condition{};
  }

  // Current is now a clean-state clone of the tasklet's state. Let's swap the
  // pointers; afterwards, current is the thread-to-be-joined, and tasklet's
  // state is clean.
  tasklet_context.swap(current);

  // In order to join the thread pointed to by ctx/current now, we have to
  // release the lock, or we'll block everything else.
  condition_lock.unlock();

  // In case the thread is currently sleeping, we notify its condition.
  send_notification(ctx);

  // Now we can join.
  ctx->the_thread.join();

  // Since ctx and current is the same, and current dies at the end of the
  // function, this should also clean up the context.
}


} // anonymous namespace




/*****************************************************************************
 * tasklet::context
 */


tasklet::context::context(tasklet::sleep_condition * _condition)
  : running(false)
  , condition(_condition)
{
}



tasklet::context::~context()
{
}



bool
tasklet::context::nanosleep(std::chrono::nanoseconds nsecs) const
{
  auto ctx = reinterpret_cast<extended_context *>(const_cast<tasklet::context *>(this));
  auto condition_lock = std::unique_lock(ctx->condition->mutex);

  if (!ctx->running) {
    return false;
  }

  if (nsecs < std::chrono::nanoseconds(0)) {
    ctx->condition->condition.wait(condition_lock);
  }
  else {
    ctx->condition->condition.wait_for(condition_lock, nsecs);
  }

  return ctx->running;
}




/*****************************************************************************
 * tasklet
 */


tasklet::tasklet(tasklet::task_function && func, bool start_now /* = false */)
  : m_context{new extended_context{
      this,
      std::make_shared<tasklet::task_function>(std::move(func)),
      new tasklet::sleep_condition{},
      true
    }}
{
  if (start_now) {
    start();
  }
}



tasklet::tasklet(tasklet::task_function && func,
    sleep_condition * condition,
    bool start_now /* = false */)
  : m_context{new extended_context{
      this,
      std::make_shared<tasklet::task_function>(std::move(func)),
      condition,
      false
    }}
{
  if (start_now) {
    start();
  }
}



tasklet::~tasklet()
{
  try_joining(m_context, true);
}



bool
tasklet::start()
{
  auto ctx = static_cast<extended_context *>(m_context.get());
  auto condition_lock = std::unique_lock(ctx->condition->mutex);

  if (ctx->the_thread.joinable()) {
    return false;
  }

  ctx->running = true;
  ctx->the_thread = std::thread{tasklet_wrapper, ctx};
  return true;
}



bool
tasklet::stop()
{
  auto ctx = static_cast<extended_context *>(m_context.get());

  // Lock to make changes to the running flag. This should avoid a race in
  // the nanosleep function, where sleeping is only initiated under the lock
  // and if the flag is unset.
  {
    auto condition_lock = std::unique_lock(ctx->condition->mutex);

    if (!ctx->the_thread.joinable()) {
      return false;
    }

    ctx->running = false;
  }

  // Release the lock to send the notification. The worst that could happen
  // is that after the lock is released, the flag is changed back and the
  // notification is spurious.
  send_notification(ctx);

  return true;
}



void
tasklet::wait()
{
  try_joining(m_context, false);
}




void
tasklet::wakeup()
{
  auto ctx = static_cast<extended_context *>(m_context.get());
  send_notification(ctx);
}



} // namespace liberate::concurrency
