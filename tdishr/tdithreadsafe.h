#ifndef TDIREFZONE_H
#include "tdirefzone.h"
#else
#ifndef TDITHREADSAFE_H
#define TDITHREADSAFE_H
#include <mdsdescrip.h>
#include <pthread_port.h>

typedef struct _thread_static {
  int TdiGetData_recursion_count;
  int TdiIntrinsic_mess_stat;
  int TdiIntrinsic_recursion_count;
  struct descriptor_d TdiIntrinsic_message;
  struct {
    void *head;
    void *head_zone;
    void *data_zone;
    int public;
  } TdiVar_private;
  int TdiVar_new_narg;
  struct descriptor TdiVar_new_narg_d;
  int compiler_recursing;
  struct descriptor *TdiRANGE_PTRS[3];
  struct descriptor_xd *TdiSELF_PTR;
  struct TdiZoneStruct TdiRefZone;
  unsigned int TdiIndent;
  unsigned short TdiDecompile_max;
  int TdiOnError;
} ThreadStatic;

extern ThreadStatic *TdiGetThreadStatic();
extern void LockTdiMutex(pthread_mutex_t *, int *);
extern void UnlockTdiMutex(pthread_mutex_t *);

#define TdiON_ERROR TdiThreadStatic_p->TdiOnError
#define GET_TDITHREADSTATIC_P ThreadStatic *TdiThreadStatic_p = TdiGetThreadStatic()

#endif
#endif
