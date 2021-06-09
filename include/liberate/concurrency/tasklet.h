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
#ifndef LIBERATE_CONCURRENCY_TASKLET_H
#define LIBERATE_CONCURRENCY_TASKLET_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <liberate.h>

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace liberate::concurrency {

/**
 * Tasklet class
 *
 * The tasklet class aims to make thread management a little easier.
 *
 * Specifically, it decouples the task to be executed from the lifetime of the
 * thread, effectively enabling restartable threads. The task to be executed
 * is handed to the tasklet as a task_function. Tasklet then adds start(),
 * stop(), wakeup() and join() functions to it.
 *
 * As oftentimes threads spend much of their time waiting for some external
 * condition, tasklet also accepts a condition variable (and associated mutex).
 * With these, a task_function can sleep until the condition has been met. If
 * no external condition is provided, an internal one is used to facilitate
 * sleeping.
 *
 * stop() and wakeup() both signal the condition. If the condition is shared
 * (provided from external), notify_all() is sent, as it cannot be determined
 * which waiting thread should be woken.
 **/
class LIBERATE_API tasklet
{
public:
  /***************************************************************************
   * Types
   **/
  /**
   * Bundle condition variable and mutex into one. This way, they cannot be
   * provided separately.
   */
  struct LIBERATE_API sleep_condition
  {
    std::condition_variable condition;
    std::mutex              mutex;
  };

  /**
   * The thread context. This is passed to the task function (below).
   */
  struct LIBERATE_API context
  {
    // Gets set or unset by tasklet functions. Can also be used in the
    // task_function to determine whether to exit the thread loop.
    std::atomic<bool>   running;

    // Gets notified when the running flag changes. Wait on this condition
    // variable to sleep in the thread loop.
    sleep_condition *   condition;

    // Ctor/dtor
    context(sleep_condition * condition);
    virtual ~context();

    /**
     * Sleep function. Use with std::chrono's durations. Returns true if the
     * tasklet is still supposed to be running (i.e. sleep was interrupted by
     * wakeup() rather than stop()), so you can build your thread function like
     * this:
     *
     *   void func(tasklet & t, void * baton)
     *   {
     *     while (t.sleep()) {
     *       // Do something
     *     }
     *   }
     **/
    template <typename durationT>
    inline bool sleep(durationT const & duration) const
    {
      return context::nanosleep(std::chrono::round<std::chrono::nanoseconds>(duration));
    }

    inline bool sleep() const
    {
      return context::nanosleep(std::chrono::nanoseconds(-1));
    }

  private:
    /***************************************************************************
     * Implementation functions
     **/
    bool nanosleep(std::chrono::nanoseconds nsecs) const;
  };

  /**
   * The task function passed to tasklet; implements the inner thread loop.
   */
  using task_function = std::function<void (context &)>;


  /***************************************************************************
   * Constructor/destructor
   **/
  /**
   * Create a tasklet either with or without a sleep_condition. The tasklet
   * does *not* take onwership, and assumes the caller does.
   **/
  explicit tasklet(task_function && func, bool start_now = false);
  tasklet(task_function && func, sleep_condition * condition,
      bool start_now = false);

  ~tasklet();

  /***************************************************************************
   * Main interface
   **/
  /**
   * Returns true if the tasklet was in a startable state and is now started.
   **/
  bool start();

  /**
   * Returns true if the tasklet was in a stoppable state and is now stopped.
   **/
  bool stop();

  /**
   * Wait for the tasklet to terminate. This is synonymous to thread::join();
   * the call will block until tasklet is restartable.
   **/
  void wait();

  /**
   * Wakes the thread up from sleeping on the condition variable.
   **/
  void wakeup();

private:
  tasklet(tasklet const &) = delete;
  tasklet(tasklet &&) = delete;
  tasklet & operator=(tasklet const &) = delete;

  /***************************************************************************
   * Data
   **/
  mutable std::unique_ptr<context> m_context;
};

} // namespace liberate::concurrency


#endif // guard
