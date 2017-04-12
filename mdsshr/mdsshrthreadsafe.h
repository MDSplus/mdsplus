#include <mdstypes.h>
#include <mdsdescrip.h>
#include <pthread_port.h>

typedef struct _thread_static {
  char MdsGetMsg_text[1024];
  char MdsMsg_text[1024];
  struct descriptor MdsGetMsgDsc_tmp;
} MdsShrThreadStatic;

#define TREEGETTHREADSTATIC_P TreeThreadStatic * TreeGetThreadStatic_p = TreeGetThreadStatic()

extern MdsShrThreadStatic *MdsShrGetThreadStatic();
EXPORT void LockMdsShrMutex(pthread_mutex_t *, int *);
EXPORT void UnlockMdsShrMutex(pthread_mutex_t *);
