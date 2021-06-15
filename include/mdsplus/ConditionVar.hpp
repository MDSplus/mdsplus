/*
 * ConditionVar.hpp
 *
 *  Created on: May 4, 2014
 *      Author: kerickso
 */

#ifndef MDS_CONDITIONVAR_HPP_
#define MDS_CONDITIONVAR_HPP_

#include <mdsplus/mdsplus.h>

#include <cstddef>

#if defined(MDS_WINDOWS)
#include <windows.h>
#elif defined(MDS_PTHREAD)
#include <pthread.h>
#endif

#ifdef MDS_MACxxx
#include <mach/clock.h>
#include <mach/mach.h>
#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME 0
#endif
static void clock_gettime(int dummyClock __attribute__((unused)), timespec *ts)
{
  clock_serv_t cclock;
  mach_timespec_t mts;
  host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);

  ts->tv_sec = mts.tv_sec;
  ts->tv_nsec = mts.tv_nsec;
}
#endif

namespace MDSplus
{

  class ConditionVar
  {
  public:
    ConditionVar()
    {
      _create();
    }

    ~ConditionVar()
    {
      _destroy();
    }

    void wait()
    {
      _wait();
    }

    bool waitTimeout(std::size_t msec)
    {
      return _waitTimeout(msec);
    }

    void notify()
    {
      _notify();
    }

  private:
#if defined(MDS_PTHREAD)
    pthread_mutex_t mutex;
    pthread_cond_t condition;

    void _create()
    {
      //FIXME: Use PTHREAD_MUTEX_INITIALIZER when FC15 goes away
      pthread_mutex_init(&mutex, NULL);
      pthread_cond_init(&condition, NULL);
    }

    void _wait()
    {
      pthread_mutex_lock(&mutex);
      pthread_cond_wait(&condition, &mutex);
      pthread_mutex_unlock(&mutex);
    }

    bool _waitTimeout(std::size_t msec)
    {
      timespec t;
      clock_gettime(CLOCK_REALTIME, &t);
      t.tv_sec += msec / 1000L;
      t.tv_nsec += msec % 1000L * 1000000L;
      pthread_mutex_lock(&mutex);
      bool res = pthread_cond_timedwait(&condition, &mutex, &t) == 0 ? true : false;
      pthread_mutex_unlock(&mutex);
      return res;
    }

    void _notify()
    {
      pthread_mutex_lock(&mutex);
      pthread_cond_broadcast(&condition);
      pthread_mutex_unlock(&mutex);
    }

    void _destroy()
    {
      pthread_cond_destroy(&condition);
    }
#elif defined(MDS_WINDOWS)
    HANDLE condition;

    void _create()
    {
      condition = CreateEvent(NULL, FALSE, FALSE, NULL);
    }

    void _wait()
    {
      WaitForSingleObject(condition, INFINITE);
    }

    bool _waitTimeout(std::size_t msec)
    {
      return WaitForSingleObject(condition, msec) == WAIT_OBJECT_0 ? true : false;
    }

    void _notify()
    {
      SetEvent(condition);
    }

    void _destroy()
    {
      CloseHandle(condition);
    }
#endif
  };
}

#endif /* MDS_CONDITIONVAR_HPP_ */
