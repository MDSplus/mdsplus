#include <mdsdescrip.h>
#include <pthread.h>
#if (defined(_DECTHREADS_) && (_DECTHREADS_ != 1)) || !defined(_DECTHREADS_)
#define pthread_attr_default NULL
#define pthread_mutexattr_default NULL
#define pthread_condattr_default NULL
#endif

typedef struct _thread_static {
  char MdsGetMsg_text[1024];
  char MdsMsg_text[1024];
  struct descriptor MdsGetMsgDsc_tmp;
} MdsShrThreadStatic;

extern MdsShrThreadStatic *MdsShrGetThreadStatic();
extern void LockMdsShrMutex(pthread_mutex_t *, int *);
extern void UnlockMdsShrMutex(pthread_mutex_t *);
