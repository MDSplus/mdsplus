#ifndef _P2WTHREAD_HPP
#define _P2WTHREAD_HPP

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#define __STDC_FORMAT_MACROS
#include <pthread.h>
#include <inttypes.h>
#ifndef _WIN32
#include <errno.h>
#endif

#include "P2WMutex.hpp"
#include "mdsmsg.h"

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#ifndef _WIN32
#include <errno.h>
inline bool p2w_try_sched(bool const a = true)
{
  static bool try_sched = true;
  if (unlikely(!a))
    try_sched = false;
  return try_sched;
}
#endif

template <typename T = void>
class P2WThread
{
#define P2WTHREAD_NAME_MAX 32
public:
  /** Thread manager
     * @param routine	<i> routine to run in thread
     * @param priority	<i> thread priority: highest 0 to 255 lowest or -1 to inherit (default = -1)
     * @param cpu		<i> preferred cpu, -1 unspecified (default = -1)
     */
  P2WThread(void *(*routine)(T *), int priority = -1, int cpu = -1)
      : m_routine(routine), m_priority(priority), m_cpu(cpu), m_name(""), active(false)
  {
    m_name[0] = 0;
  }
  /** Thread manager
     * @param routine	<i> routine to run in thread
     * @param name      <i> name of thread for debugging and logs (<char*> will call free on pointer, cast to <const char*> if not desired)
     * @param priority	<i> thread priority: highest 0 to 255 lowest or -1 to inherit (default = -1)
     * @param cpu		<i> preferred cpu, -1 unspecified (default = -1)
     */
  P2WThread(void *(*routine)(T *), char *name, int priority = -1, int cpu = -1)
      : m_routine(routine), m_priority(priority), m_cpu(cpu), active(false)
  {
    rename(name);
  };
  /** Thread manager
     * @param routine	<i> routine to run in thread
     * @param name      <i> name of thread for debugging and logs
     * @param priority	<i> thread priority: highest 0 to 255 lowest or -1 to inherit (default = -1)
     * @param cpu		<i> preferred cpu, -1 unspecified ( default = -1)
     */
  P2WThread(void *(*routine)(T *), const char *name, int priority = -1, int cpu = -1)
      : m_routine(routine), m_priority(priority), m_cpu(cpu), active(false)
  {
    rename(name);
  };
  ~P2WThread() { stop(); };

private:
#ifdef _P2W_GTHREADS // i.e. WIN threads
  HANDLE native = NULL;
#else
  pthread_t native = 0;
#define m_thread_id native
#endif
  void *(*const m_routine)(T *);
  const int m_priority;
  const int m_cpu;
  char m_name[P2WTHREAD_NAME_MAX + 1];
  P2WMutex mutex;
  std::atomic_bool active;
  constexpr bool hasname() { return m_name[0] != 0; };

public:
  /**
     * @brief Returns true if thread was started.
     */
  bool is_active() const
  {
    return active;
  }
  /**
     * @brief Returns true if thread is still alive.
     */
  bool is_alive()
  {
    P2W_UNIQUE_LOCK_ACQUIRE(lock, mutex);
    if (native != 0 && !tryjoin(NULL))
      return true;
    reset();
    P2W_UNIQUE_LOCK_RELEASE(lock, mutex);
    return false;
  };
  /**
     * @brief Rename thread; Thread must be restarted to change internal name.
     */
  void rename(char *name)
  {
    rename((const char *)name);
    free(name);
  };
  void rename(const char *name)
  {
    if (name == NULL)
      m_name[0] = 0;
    else
      strncpy(m_name, name, P2WTHREAD_NAME_MAX);
  };
  const char *name() const
  {
    return m_name;
  };
  /**
     * @brief Start thread using optional args pointer
     */
  bool start(T *arg = NULL)
  {
    P2W_UNIQUE_LOCK_ACQUIRE(lock, mutex);
    if (native != 0 && !tryjoin(NULL))
      return true;
    reset();
    if (create(arg))
    {
      active = true;
      return true;
    }
    P2W_UNIQUE_LOCK_RELEASE(lock, mutex);
    return false;
  };
  /**
     * @brief Stop running thread using cancel
     */
  bool stop(void **result = NULL)
  {
    P2W_UNIQUE_LOCK_ACQUIRE(lock, mutex);
    if (native != 0)
    {
      if (cancel(result) || tryjoin(result))
      {
        if (hasname())
          MDSDBG("P2WThread::stop(\"%s\") Stopped ID: %" PRIxPTR ".", m_name, (intptr_t)m_thread_id);
      }
      else if (hasname())
        MDSWRN("P2WThread::stop(\"%s\") Failed ID: %" PRIxPTR ".", m_name, (intptr_t)m_thread_id);
    }
    reset();
    P2W_UNIQUE_LOCK_RELEASE(lock, mutex);
    return true;
  };
  /**
     * @brief Wait for thread to exit gracefully (blocking)
     */
  bool wait(void **result = NULL)
  {
    P2W_UNIQUE_LOCK_ACQUIRE(lock, mutex);
    if (native != 0)
    {
      if (join(result))
      {
        if (hasname())
          MDSDBG("P2WThread::join(\"%s\") Joined ID: %" PRIxPTR ".", m_name, (intptr_t)m_thread_id);
      }
      else if (hasname())
        MDSWRN("P2WThread::join(\"%s\") Failed ID: %" PRIxPTR ".", m_name, (intptr_t)m_thread_id);
    }
    reset();
    P2W_UNIQUE_LOCK_RELEASE(lock, mutex);
    return true;
  };

private:
#ifdef _P2W_GTHREADS // i.e. WIN threads
  DWORD m_thread_id = 0;
  inline void reset()
  {
    m_thread_id = 0;
    native = NULL;
  };
  inline bool cancel(void **const result) const { return native == NULL || !!TerminateThread(native, 0); };
  // WaitForSingleObject returns WAIT_OBJECT_0 or WAIT_TIMEOUT or WAIT_FAILED
  inline bool tryjoin(void **const result) const { return native == NULL || WaitForSingleObject(native, 0) != WAIT_TIMEOUT; };
  inline bool join(void **const result) const { return native == NULL || WaitForSingleObject(native, -1) != WAIT_TIMEOUT; };
  inline bool create(void *const args)
  {
    native = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)m_routine, args, 0, &m_thread_id);
    if (native == NULL)
    {
      m_thread_id = 0;
      if (hasname())
        MDSERR("P2WThread::create(\"%s\") Failed %ld", m_name, GetLastError());
      return false;
    }
    else
    {
      if (hasname())
        MDSMSG("P2WThread::create(\"%s\") 0x%lx", m_name, m_thread_id);
      return true;
    }
  }
#else // PTHREAD
  /**
     * @brief reset fields; thread not active
     */
  inline void reset()
  {
    active = false;
    native = 0;
  };
#ifdef _WIN32
  inline bool cancel(void **const result) const
  {
    for (int retry = 10; retry-- > 0;)
    {
      if (tryjoin(result))
        return true;
      usleep(100000);
    }
    HANDLE handle = pthread_gethandle(native);
    MDSWRN("P2WThread::cancel(\"%s\") failed to gracefully end 0x%lx!", m_name, m_thread_id);
    return TerminateThread(handle, 0xDEADBEEF) && join(result);
  };
#else
  inline bool cancel(void **const result) const
  {
    return pthread_cancel(native) == 0 && join(result);
  };
#endif
  inline bool join(void **const result) const
  {
    errno = pthread_join(native, result);
    return (errno == 0 || errno == EINVAL);
  };
  inline bool tryjoin(void **const result) const
  {
    errno = pthread_tryjoin_np(native, result);
    return (errno == 0 || errno == EINVAL);
  };
  bool create(void *const args)
  {
    int policy = SCHED_OTHER;
    int priority = 20;
    struct sched_param param;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
#ifndef _WIN32
    if (p2w_try_sched())
    {
      if (m_cpu >= 0)
      {
        cpu_set_t cpuSet;
        CPU_ZERO(&cpuSet);
        CPU_SET(m_cpu, &cpuSet);
        if (unlikely(pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuSet) != 0))
        {
          if (hasname())
            MDSWRN("P2WThread::create(\"%s\") Setting affinity to cpu %d failed", m_name, m_cpu);
        }
      }
      if (unlikely((errno = pthread_attr_setschedpolicy(&attr, SCHED_FIFO)) != 0))
      {
        if (hasname())
          MDSWRN("P2WThread::create(\"%s\") Setting policy to SCHED_FIFO failed", m_name);
      }
      else
        policy = SCHED_FIFO;
    }
#endif
    if (m_priority < 0)
    {
      errno = pthread_getschedparam(pthread_self(), &policy, &param);
      if (unlikely(errno != 0))
      {
        if (hasname())
          MDSWRN("P2WThread::create(\"%s\") failed to get sched param.", m_name);
      }
      else
        priority = param.sched_priority;
    }
    else
    {
      const int max_priority = sched_get_priority_max(policy);
      const int min_priority = sched_get_priority_min(policy);
      if (unlikely(m_priority > 255))
        priority = min_priority;
      else // map priority range [0..255] to native range of current policy
        priority = min_priority - (((255 - m_priority) * (min_priority - max_priority)) / 255);
      param.sched_priority = priority;
      MDSDBG("priority %d -> %d, [%d .. %d]", m_priority, priority, min_priority, max_priority);
      if (unlikely((errno = pthread_attr_setschedparam(&attr, &param)) != 0))
      {
        if (hasname())
          MDSWRN("P2WThread::create(\"%s\") Setting priority to %d failed for policy %d", m_name, m_priority, policy);
      }
    }
    errno = pthread_create(&native, &attr, (void *(*)(void *))(void *)m_routine, args);
    if (unlikely(errno != 0))
    {
#ifndef _WIN32
      if (errno == EPERM)
      {
        MDSWRN("P2WThread::create(\"%s\") Failed. Disabling SCHEDuler options", m_name);
        p2w_try_sched(false);
        pthread_attr_destroy(&attr);
        return create(args);
      }
#endif
      if (hasname())
        MDSWRN("P2WThread::create(\"%s\") Failed", m_name);
      pthread_attr_destroy(&attr);
      reset();
      return false;
    }
    pthread_attr_destroy(&attr);
    if (hasname())
    {
      errno = pthread_getschedparam(native, &policy, &param);
      if (unlikely(errno != 0))
      {
        MDSWRN("P2WThread::create(\"%s\") 0x%" PRIxPTR ", SCHED unknown!",
               m_name, (intptr_t)native);
      }
#ifndef _WIN32
      else if (p2w_try_sched() && m_cpu >= 0)
      {
        MDSMSG("P2WThread::create(\"%s\") 0x%" PRIxPTR " on cpu %d with priority %d %s.",
               m_name, (intptr_t)native, m_cpu, param.sched_priority,
               policy == SCHED_FIFO ? "SCHED_FIFO OK" : "NOT SCHED_FIFO");
      }
#endif
      else
      {
        MDSMSG("P2WThread::create(\"%s\") 0x%" PRIxPTR " with priority %d.",
               m_name, (intptr_t)native, param.sched_priority);
      }
      pthread_setname_np(native, m_name);
    }
    return true;
  }
#endif // PTHREAD
};
#endif // ifndef _P2WTHREAD_HPP
