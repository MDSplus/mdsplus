#ifndef _MDSTHREADSTATIC_H
#define _MDSTHREADSTATIC_H
#include <mdsdescrip.h>
#include <mdstypes.h>
#include <pthread_port.h>

#define THREADSTATIC_MDSSHR 0
#define THREADSTATIC_TDISHR 1
#define THREADSTATIC_TREESHR 2
#define THREADSTATIC_DCLSHR 3
#define THREADSTATIC_MDSIP 4
#define THREADSTATIC_SIZE 5

typedef struct
{
  void *buffer;
  void (*free)();
} buffer_t;

typedef struct
{
  buffer_t **buffers;
  int is_owned;
} MDSplusThreadStatic_t;

#define DEFINE_GETTHREADSTATIC(type, name) \
  type *name(MDSplusThreadStatic_t *mts)
#define IMPLEMENT_GETTHREADSTATIC(type, name, bufidx, buffer_allow, \
                                  buffer_free)                      \
  DEFINE_GETTHREADSTATIC(type, name)                                \
  {                                                                 \
    if (!mts)                                                       \
      mts = MDSplusThreadStatic(NULL);                              \
    if (!mts->buffers[bufidx])                                      \
    {                                                               \
      mts->buffers[bufidx] = (buffer_t *)malloc(sizeof(buffer_t));  \
      mts->buffers[bufidx]->buffer = (void *)buffer_alloc();        \
      mts->buffers[bufidx]->free = (void *)buffer_free;             \
    }                                                               \
    return (type *)mts->buffers[bufidx]->buffer;                    \
  }

extern DEFINE_GETTHREADSTATIC(MDSplusThreadStatic_t, MDSplusThreadStatic);

#define MDSTHREADSTATIC_VAR MdsThreadStatic_p
#define MDSTHREADSTATIC_TYPE MdsThreadStatic
#define MDSTHREADSTATIC_ARG MDSTHREADSTATIC_TYPE *const MDSTHREADSTATIC_VAR
#define MDSTHREADSTATIC(MTS) MDSTHREADSTATIC_ARG = MdsGetThreadStatic(MTS)
#define MDSTHREADSTATIC_INIT MDSTHREADSTATIC(NULL)
typedef struct
{
  char MdsMsg_cstr[1024];
  char MdsGetMsg_cstr[1024];
  mdsdsc_t MdsGetMsg_desc;
  char *librtl_fis_error;
} MDSTHREADSTATIC_TYPE;
#define MDS_MDSMSG_CSTR MDSTHREADSTATIC_VAR->MdsMsg_cstr
#define MDS_MDSGETMSG_CSTR MDSTHREADSTATIC_VAR->MdsGetMsg_cstr
#define MDS_MDSGETMSG_DESC MDSTHREADSTATIC_VAR->MdsGetMsg_desc
#define MDS_FIS_ERROR MDSTHREADSTATIC_VAR->librtl_fis_error

extern DEFINE_GETTHREADSTATIC(MDSTHREADSTATIC_TYPE, MdsGetThreadStatic);
#endif // ifndef _MDSTHREADSTATIC_H
