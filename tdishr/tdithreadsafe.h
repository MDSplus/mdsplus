#include <mdsdescrip.h>
#include <pthread.h>
#if (defined(_DECTHREADS_) && (_DECTHREADS_ != 1)) || !defined(_DECTHREADS_)
#define pthread_attr_default NULL
#define pthread_mutexattr_default NULL
#define pthread_condattr_default NULL
#endif

typedef struct _thread_static {
  int TdiGetData_recursion_count;
  int TdiIntrinsic_mess_stat;
  int TdiIntrinsic_recursion_count;
  struct descriptor_d TdiIntrinsic_message;
  struct { 
    void *head;
    void *head_zone;
    void *data_zone;
    int   public;
  } TdiVar_private;
  int TdiVar_new_narg;
  struct descriptor TdiVar_new_narg_d;
} ThreadStatic;

extern ThreadStatic *TdiThreadStatic();
extern void LockTdiMutex(pthread_mutex_t *, int *);
extern void UnlockTdiMutex(pthread_mutex_t *);
