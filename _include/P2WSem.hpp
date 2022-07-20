#ifndef _P2WSEM_HPP
#define _P2WSEM_HPP

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#define __STDC_FORMAT_MACROS

#include <semaphore.h>
#include <string.h>

#include <atomic>
#include <chrono>
#include <stdexcept>

#ifdef _P2W_GTHREADS
#include <windows.h>
#include <synchapi.h>
#include <processthreadsapi.h>
#endif

class P2WSem
{
public:
  sem_t native;

public:
  P2WSem(int count = 0)
  {
    if (sem_init(&native, 0, count) != 0)
      throw std::runtime_error(strerror(errno));
  }
  ~P2WSem()
  {
    sem_destroy(&native);
  }
  void release(std::ptrdiff_t update = 1)
  {
    for (intptr_t i = update; i-- > 0;)
      sem_post(&native);
  }
  void acquire()
  {
    sem_wait(&native);
  }
  bool try_acquire()
  {
    return sem_trywait(&native) == 0;
  }
  template <class Rep, class Period>
  bool try_acquire_for(const std::chrono::duration<Rep, Period> &rel_time)
  {
    return try_acquire_until(std::chrono::system_clock::now() + rel_time);
  }
  template <class Duration> // Clock could be template but cannot cast to different clocks, so ..
  bool try_acquire_until(const std::chrono::time_point<std::chrono::system_clock, Duration> &abs_time)
  {
    struct timespec ts;
    p2w_time_point_to_timespec(abs_time, ts);
    return sem_timedwait(&native, &ts) == 0;
  }
  sem_t *native_handle() { return &native; }
  // extra
  bool try_acquire_until(const struct timespec *abs_timeout)
  {
    return sem_timedwait((sem_t *)&native, abs_timeout) == 0;
  }
  void acquire_rest()
  {
    while (try_acquire())
      ;
  }
};
static_assert(sizeof(sem_t) == sizeof(P2WSem), "Size is not correct");
#endif // ifndef _P2WSEM_HPP
