/**
 * This file is part of liberate.
 *
 * Author(s): Jens Finkhaeuser <jens@finkhaeuser.de>
 *
 * Copyright (c) 2021 Jens Finkhaeuser.
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
#ifndef LIBERATE_CONCURRENCY_LOCK_POLICY_H
#define LIBERATE_CONCURRENCY_LOCK_POLICY_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <liberate.h>

#include <memory>

namespace liberate::concurrency {

/**
 * Do you want to write code that may or may not require concurrent access
 * and need lock based synchronization? Lock policies are for you.
 *
 * Are you not sure if your code should own/hold a lock, or you need a shared
 * lock? The shared_lock_policy can be combined with other lock policies to
 * meet your needs.
 *
 * A lock policy is a simple struct that defines a mutex_type and a lock_type.
 * The only requirement on the mutex_type is that it defines a lock() and an
 * unlock() function. The only requirement on the lock_type is that it takes
 * a mutex_type reference in the constructor and calls its lock() function, and
 * in the destructor calls it's unlock() function.
 *
 * This follows the basic pattern of scoped locks as you might know them from
 * boost and modern C++. No other patterns are supported to keep things simple,
 * such as timed locks or temporarily unlocking, etc.
 *
 * - The null_lock_policy defines types that do nothing. Use this when you want
 *   to specialize your code to not perform synchronization.
 *
 * - The std_lock_policy is a template that can be parametrized with a mutex
 *   and a lock type. The types it exposes are thin wrappers around the template
 *   parameters that enforce conformity with the lock policy usage pattern. This
 *   is mostly to prevent you from writing code that is specialized to your
 *   choice of mutex and lock; if you *want* to specialize your code to that,
 *   there is no need for using lock policies.
 *
 * - The *_ext_lock_policy are templates that take another lock policy.
 *   Instead of exposing the wrapped policy's raw mutex type, they expose
 *   a mutex proxy object which in turn holds a reference to the raw mutex.
 *   This permits moving the mutex ownership external to your code base,
 *   e.g. for sharing it amongst different code pieces. The proxy has a
 *   set_reference() and a clear() method, and may also take the reference
 *   in its constructor.
 *
 *   There three specific types are:
 *   - raw_ext_lock_policy::mutex_type takes raw pointers to a mutex type.
 *   - shared_ext_lock_policy::mutex_type takes std::shared_ptr to a mutex type.
 *   - ref_ext_lock_policy::mutex_type takes references to a mutex type. Note
 *     that it is up to you to provide the means by which the mutex type can
 *     be initialized in the constructor.
 *
 *   The first two also allow for setting/clearing the held mutex via
 *   set_proxied()/clear_proxied(). These functions, however, only affect each
 *   proxy object. They do not let you modify all proxies to the same shared
 *   mutex at the same time. As such, they have limited use.
 *
 * The declaration of all these types follows the detail implementation.
 */
namespace detail {

/**
 * The null mutex type does nothing.
 */
struct null_mutex
{
  inline void lock() {}
  inline void unlock() {}
};


/**
 * The null lock type takes a mutex, but does nothing with it.
 */
template <typename mutexT>
struct null_lock
{
  inline null_lock(mutexT &)
  {
  }
};


/**
 * The std_mutex wraps a std::mutex-like type.
 */
template <typename mutexT>
struct std_mutex
{
  inline void lock()
  {
    mtx.lock();
  }


  inline void unlock()
  {
    mtx.unlock();
  }

  mutexT mtx;
};


/**
 * The default_lock does pretty much what's described above, and can be
 * applyed to std_mutex.
 */
template <typename mutexT>
struct default_lock
{
  inline default_lock(mutexT & mutex)
    : m_mutex{mutex}
  {
    m_mutex.lock();
  }

  inline ~default_lock()
  {
    m_mutex.unlock();
  }

  mutexT & m_mutex;
};


/**
 * The mutex_proxy class proxies a mutex. We specialize on pointer and
 * reference types.
 */
template <typename mutexT, typename refT, bool is_reference>
struct mutex_proxy
{
};


template <typename mutexT, typename refT>
struct mutex_proxy<mutexT, refT, false>
{
  using proxied_type = mutexT;

  inline mutex_proxy(refT proxied)
    : m_proxied{proxied}
  {
  }

  inline void set_proxied(refT proxied)
  {
    m_proxied = proxied;
  }

  inline void clear_proxied()
  {
    m_proxied = nullptr;
  }

  inline void lock()
  {
    if (m_proxied) {
      m_proxied->lock();
    }
  }

  inline void unlock()
  {
    if (m_proxied) {
      m_proxied->unlock();
    }
  }

  refT m_proxied;
};


template <typename mutexT, typename refT>
struct mutex_proxy<mutexT, refT, true>
{
  using proxied_type = mutexT;

  inline mutex_proxy(refT proxied)
    : m_proxied{proxied}
  {
  }

  inline void lock()
  {
    m_proxied.lock();
  }

  inline void unlock()
  {
    m_proxied.unlock();
  }

  refT m_proxied;
};


/**
 * Helper to select the appropriate proxy type for ext_lock_policy
 */
template <typename mutexT, typename refT>
struct mutex_proxy_selector
{
  using type = mutex_proxy<mutexT, refT, std::is_reference<refT>::value>;
};


} // namespace detail


/**
 * See above, the null_lock_policy does nothing.
 */
struct null_lock_policy
{
  using mutex_type = detail::null_mutex;
  using lock_type = detail::null_lock<mutex_type>;
};


/**
 * Specialize the std_lock_policy with e.g. std::mutex and std::lock_guard.
 */
template <
  typename mutexT,
  typename lockT
>
struct std_lock_policy
{
  using mutex_type = detail::std_mutex<mutexT>;
  using lock_type = detail::default_lock<mutex_type>;
};


/**
 * The *_ext_lock_policy templates take another policy type as their parameter.
 */
template <typename policyT>
struct raw_ext_lock_policy
{
  using mutex_type = typename detail::mutex_proxy_selector<
    typename policyT::mutex_type,
    typename policyT::mutex_type *
  >::type;
  using lock_type = detail::default_lock<mutex_type>;
};


template <typename policyT>
struct std_ext_lock_policy
{
  using mutex_type = typename detail::mutex_proxy_selector<
    typename policyT::mutex_type,
    std::shared_ptr<typename policyT::mutex_type>
  >::type;
  using lock_type = detail::default_lock<mutex_type>;
};


template <typename policyT>
struct ref_ext_lock_policy
{
  using mutex_type = typename detail::mutex_proxy_selector<
    typename policyT::mutex_type,
    typename policyT::mutex_type &
  >::type;
  using lock_type = detail::default_lock<mutex_type>;
};



} // namespace liberate::concurrency

#endif // guard
