#ifndef _P2WMUTEX_HPP
#define _P2WMUTEX_HPP

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#define __STDC_FORMAT_MACROS

#include <string.h>

#include <atomic>
#include <chrono>
#include <stdexcept>
#include <mutex>
#include <condition_variable>

#ifdef _P2W_GTHREADS
#include <windows.h>
#include <synchapi.h>
#include <processthreadsapi.h>
#endif

#include "mdsmsg.h"

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#undef P2W_PTHREAD_CLEANUP
#ifdef P2W_PTHREAD_CLEANUP
template <class _Lock>
static void p2w_cleanup_lock(void *arg) { ((_Lock *)arg)->unlock(); }
#define P2W_LOCK_CLEANUP_PUSH(lock) pthread_cleanup_push(p2w_cleanup_lock<decltype(lock)>, (void *)&lock)
#define P2W_LOCK_CLEANUP_POP(lock) pthread_cleanup_pop(0)
#else
#define P2W_LOCK_CLEANUP_PUSH(lock)
#define P2W_LOCK_CLEANUP_POP(lock)
#endif
#define P2W_UNIQUE_LOCK_ACQUIRE(lock, mutex)             \
  {                                                      \
    std::unique_lock<const decltype(mutex)> lock(mutex); \
    P2W_LOCK_CLEANUP_PUSH(lock)
#define P2W_UNIQUE_LOCK_ADOPT(lock, mutex)                                \
  {                                                                       \
    std::unique_lock<const decltype(mutex)> lock(mutex, std::adopt_lock); \
    P2W_LOCK_CLEANUP_PUSH(lock)
#define P2W_UNIQUE_LOCK_RELEASE(lock, mutex) \
  P2W_LOCK_CLEANUP_POP(lock);                \
  }
#define P2W_SHARED_LOCK_ACQUIRE(lock, mutex)                              \
  {                                                                       \
    (mutex).lock_shared();                                                \
    std::unique_lock<const decltype(mutex)> lock(mutex, std::adopt_lock); \
    P2W_LOCK_CLEANUP_PUSH(lock)
#define P2W_SHARED_LOCK_ADOPT(lock, mutex)                                \
  {                                                                       \
    std::unique_lock<const decltype(mutex)> lock(mutex, std::adopt_lock); \
    P2W_LOCK_CLEANUP_PUSH(lock)
#define P2W_SHARED_LOCK_RELEASE(lock, mutex) \
  P2W_LOCK_CLEANUP_POP(lock);                \
  }

template <class Duration>
static inline void p2w_time_point_to_timespec(const std::chrono::time_point<std::chrono::system_clock, Duration> &time_point, struct timespec &ts)
{
  auto nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(time_point.time_since_epoch());
  auto sec = std::chrono::duration_cast<std::chrono::seconds>(nsec);
  nsec -= sec;
  ts.tv_sec = sec.count();
  ts.tv_nsec = nsec.count();
}

#define _P2W_PTHREAD // define st

#ifndef _P2W_PTHREAD
#define P2WMutex std::mutex
#else
#include <exception>

class P2WCond;
template <typename T, int (*TRYLOCK)(T *), int (*LOCK)(T *), int (*UNLOCK)(T *)>
class _P2WMutex
{
  friend P2WCond;

protected:
  T native;

public:
  bool try_lock() const
  {
    bool const locked = TRYLOCK((T *)&native) == 0;
    if (locked)
      MDSDBG("MUTEX: 0x%" PRIxPTR " locked 0x%" PRIxPTR, (uintptr_t)pthread_self(), (uintptr_t)this);
    return locked;
  }
  void lock() const
  {
#ifdef DEBUG
    if (try_lock())
      return;
#endif
    MDSDBG("MUTEX: 0x%" PRIxPTR " wait for 0x%" PRIxPTR, (uintptr_t)pthread_self(), (uintptr_t)this);
    int const err = LOCK((T *)&native);
    if (err != 0)
      throw std::runtime_error(strerror(err));
    MDSDBG("MUTEX: 0x%" PRIxPTR " locked 0x%" PRIxPTR, (uintptr_t)pthread_self(), (uintptr_t)this);
  }
  void unlock() const
  {
    MDSDBG("MUTEX: 0x%" PRIxPTR " unlocking 0x%" PRIxPTR, (uintptr_t)pthread_self(), (uintptr_t)this);
    int const err = UNLOCK((T *)&native);
    if (err != 0)
      throw std::runtime_error(strerror(err));
  }
  T *native_handle() { return &native; };
};

class P2WMutex : public _P2WMutex<pthread_mutex_t, pthread_mutex_trylock, pthread_mutex_lock, pthread_mutex_unlock>
{
public:
  P2WMutex(const pthread_mutexattr_t *attr = NULL)
  {
    int const err = pthread_mutex_init(&native, attr);
    if (unlikely(err != 0))
      throw std::runtime_error(strerror(err));
  };
  ~P2WMutex()
  {
    pthread_mutex_destroy(&native);
  }
  P2WMutex(int const type)
  {
    pthread_mutexattr_t attr;
    int err = pthread_mutexattr_init(&attr);
    if (unlikely(err != 0))
      throw std::runtime_error(strerror(err));
    err = pthread_mutexattr_settype(&attr, type);
    if (unlikely(err != 0))
      throw std::runtime_error(strerror(err));
    err = pthread_mutex_init(&native, &attr);
    if (unlikely(err != 0))
      throw std::runtime_error(strerror(err));
    pthread_mutexattr_destroy(&attr);
  }
};

class P2WSharedMutex : public _P2WMutex<pthread_rwlock_t, pthread_rwlock_trywrlock, pthread_rwlock_wrlock, pthread_rwlock_unlock>
{
public:
  P2WSharedMutex(const pthread_rwlockattr_t *attr = NULL)
  {
    int const err = pthread_rwlock_init(&native, attr);
    if (unlikely(err != 0))
      throw std::runtime_error(strerror(err));
  };
  ~P2WSharedMutex()
  {
    pthread_rwlock_destroy(&native);
  }
  bool try_lock_shared() const
  {
    bool const locked = pthread_rwlock_tryrdlock((pthread_rwlock_t *)&native) == 0;
    if (locked)
      MDSDBG("MUTEX: 0x%" PRIxPTR " rdlocked 0x%" PRIxPTR, (uintptr_t)pthread_self(), (uintptr_t)this);
    return locked;
  }
  void lock_shared() const
  {
#ifdef DEBUG
    if (try_lock_shared())
      return;
#endif
    MDSDBG("MUTEX: 0x%" PRIxPTR " rdwait for 0x%" PRIxPTR, (uintptr_t)pthread_self(), (uintptr_t)this);
    int const err = pthread_rwlock_rdlock((pthread_rwlock_t *)&native);
    if (err != 0)
      throw std::runtime_error(strerror(err));
    MDSDBG("MUTEX: 0x%" PRIxPTR " rdlocked 0x%" PRIxPTR, (uintptr_t)pthread_self(), (uintptr_t)this);
  }
  void unlock_shared() const { unlock(); }
};

static_assert(sizeof(pthread_mutex_t) == sizeof(P2WMutex), "Size is not correct");
#endif

#ifndef _P2W_PTHREAD
class P2WCond : public std::condition_variable
{
public:
  void wait(std::unique_lock<const P2WMutex> &lock)
  {
    while (std::cv_status::timeout == wait_until(
                                          lock, std::chrono::time_point<std::chrono::system_clock>::max()))
      ;
  }
  template <class Predicate>
  void wait(std::unique_lock<const P2WMutex> &lock, Predicate pred)
  {
    while (!wait_until(
        lock, std::chrono::time_point<std::chrono::system_clock>::max(), pred))
      ;
  }
};
#else
class P2WCond
{
public:
  pthread_cond_t native;
  P2WCond()
  {
    if (pthread_cond_init(&native, NULL) != 0)
      throw std::runtime_error(strerror(errno));
  }
  ~P2WCond()
  {
    pthread_cond_destroy(&native);
  }
  P2WCond(const P2WCond &) = delete;
  P2WCond &operator=(const P2WCond &) = delete;
  void notify_one() noexcept
  {
    pthread_cond_signal(&native);
  }
  void notify_all() noexcept
  {
    pthread_cond_broadcast(&native);
  }
  void wait(std::unique_lock<const P2WMutex> &lock)
  {
    const int err = pthread_cond_wait(&native, (pthread_mutex_t *)&lock.mutex()->native);
    if (unlikely(err != 0))
      throw std::runtime_error(strerror(err));
  }
  template <class Predicate>
  void wait(std::unique_lock<const P2WMutex> &lock, Predicate pred)
  {
    while (!pred())
    {
      const int err = pthread_cond_wait(&native, (pthread_mutex_t *)&lock.mutex()->native);
      if (unlikely(err != 0))
        throw std::runtime_error(strerror(err));
    }
  }
  template <class Duration>
  std::cv_status wait_until(std::unique_lock<const P2WMutex> &lock, const std::chrono::time_point<std::chrono::system_clock, Duration> &abs_time)
  {
    struct timespec ts;
    p2w_time_point_to_timespec(abs_time, ts);
    const int err = pthread_cond_timedwait(&native, &lock.mutex()->native, &ts);
    if (likely(err == 0))
      return std::cv_status::no_timeout;
    if (likely(err == ETIMEDOUT))
      return std::cv_status::timeout;
    throw std::runtime_error(strerror(err));
  }
  template <class Duration, class Predicate>
  bool wait_until(std::unique_lock<const P2WMutex> &lock, const std::chrono::time_point<std::chrono::system_clock, Duration> &abs_time, Predicate pred)
  {
    struct timespec ts;
    p2w_time_point_to_timespec(abs_time, ts);
    while (!pred())
    {
      const int err = pthread_cond_timedwait(&native, (pthread_mutex_t *)&lock.mutex()->native, &ts);
      if (likely(err == 0))
        continue;
      if (likely(err == ETIMEDOUT))
        return false;
      throw std::runtime_error(strerror(err));
    }
    return true;
  }
  template <class Rep, class Period>
  std::cv_status wait_for(std::unique_lock<const P2WMutex> &lock, const std::chrono::duration<Rep, Period> &rel_time)
  {
    return wait_until(lock, std::chrono::system_clock::now() + rel_time);
  }
  template <class Rep, class Period, class Predicate>
  bool wait_for(std::unique_lock<const P2WMutex> &lock, const std::chrono::duration<Rep, Period> &rel_time, Predicate pred)
  {
    return wait_until(lock, std::chrono::system_clock::now() + rel_time, pred);
  }
  pthread_cond_t *native_handle() { return &native; }
};
static_assert(sizeof(pthread_cond_t) == sizeof(P2WCond), "Size is not correct");
#endif
#endif // ifndef _P2WMUTEX_HPP
