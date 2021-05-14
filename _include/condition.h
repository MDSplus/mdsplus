#ifndef CONDITION_H
#define CONDITION_H
#include <pthread_port.h>

typedef struct _Condition
{
  pthread_cond_t cond;
  pthread_mutex_t mutex;
  int value;
} Condition;

typedef struct _Condition_p
{
  pthread_cond_t cond;
  pthread_mutex_t mutex;
  void *value;
} Condition_p;

#define CONDITION_INITIALIZER                                    \
  {                                                              \
    PTHREAD_COND_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, B_FALSE \
  }

#define CONDITION_INIT(input)                                       \
  do                                                                \
  {                                                                 \
    (input)->value = 0;                                             \
    pthread_cond_init(&(input)->cond, pthread_condattr_default);    \
    pthread_mutex_init(&(input)->mutex, pthread_mutexattr_default); \
  } while (0)
#define _CONDITION_LOCK(input) pthread_mutex_lock(&(input)->mutex)
#define _CONDITION_UNLOCK(input) pthread_mutex_unlock(&(input)->mutex)
#define _CONDITION_SIGNAL(input) pthread_cond_signal(&(input)->cond)
#define _CONDITION_WAIT(input) \
  pthread_cond_wait(&(input)->cond, &(input)->mutex)
#define _CONDITION_WAIT_SET(input) \
  while (!(input)->value)          \
  _CONDITION_WAIT(input)
#define _CONDITION_WAIT_RESET(input) \
  while ((input)->value)             \
  _CONDITION_WAIT(input)
#define _CONDITION_WAIT_1SEC(input)                                   \
  (                                                                   \
      {                                                               \
        struct timespec tp;                                           \
        clock_gettime(CLOCK_REALTIME, &tp);                           \
        tp.tv_sec++;                                                  \
        pthread_cond_timedwait(&(input)->cond, &(input)->mutex, &tp); \
      })
#define CONDITION_SET_TO(input, value_in) \
  do                                      \
  {                                       \
    _CONDITION_LOCK(input);               \
    (input)->value = value_in;            \
    _CONDITION_SIGNAL(input);             \
    _CONDITION_UNLOCK(input);             \
  } while (0)
#define CONDITION_SET(input) CONDITION_SET_TO(input, B_TRUE)
#define CONDITION_RESET(input) CONDITION_SET_TO(input, 0)
#define CONDITION_WAIT_SET(input) \
  do                              \
  {                               \
    _CONDITION_LOCK(input);       \
    _CONDITION_WAIT_SET(input);   \
    _CONDITION_UNLOCK(input);     \
  } while (0)
#define CONDITION_WAIT_1SEC(input) \
  do                               \
  {                                \
    _CONDITION_LOCK(input);        \
    _CONDITION_WAIT_1SEC(input);   \
    _CONDITION_UNLOCK(input);      \
  } while (0)
#define CONDITION_DESTROY(input, destroy_lock) \
  do                                           \
  {                                            \
    pthread_mutex_lock(destroy_lock);          \
    pthread_cond_destroy(&(input)->cond);      \
    pthread_mutex_destroy(&(input)->mutex);    \
    pthread_mutex_unlock(destroy_lock);        \
  } while (0)
#define CONDITION_DESTROY_PTR(input, destroy_lock) \
  do                                               \
  {                                                \
    pthread_mutex_lock(destroy_lock);              \
    if (input)                                     \
    {                                              \
      pthread_cond_destroy(&(input)->cond);        \
      pthread_mutex_destroy(&(input)->mutex);      \
      free(input);                                 \
      (input) = NULL;                              \
    }                                              \
    pthread_mutex_unlock(destroy_lock);            \
  } while (0)
#define CREATE_THREAD(thread, stacksize, target, args)                         \
  pthread_attr_t attr;                                                         \
  pthread_attr_init(&attr);                                                    \
  pthread_attr_setstacksize(&attr, DEFAULT_STACKSIZE stacksize);               \
  int c_status = pthread_create(&thread, &attr, (void *)target, (void *)args); \
  pthread_attr_destroy(&attr)
#define CREATE_DETACHED_THREAD(thread, stacksize, target, args) \
  CREATE_THREAD(thread, stacksize, target, args);               \
  if (!c_status)                                                \
    pthread_detach(thread);

#define CONDITION_START_THREAD(input, thread, stacksize, target, args) \
  do                                                                   \
  {                                                                    \
    _CONDITION_LOCK(input);                                            \
    if (!(input)->value)                                               \
    {                                                                  \
      CREATE_DETACHED_THREAD(thread, stacksize, target, args);         \
      if (c_status)                                                    \
      {                                                                \
        perror("Error creating pthread");                              \
        status = MDSplusERROR;                                         \
      }                                                                \
      else                                                             \
      {                                                                \
        _CONDITION_WAIT_SET(input);                                    \
        status = MDSplusSUCCESS;                                       \
      }                                                                \
    }                                                                  \
    _CONDITION_UNLOCK(input);                                          \
  } while (0) //"

#endif // CONDITION_H
