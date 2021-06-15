#pragma once
#include "../mdsshr/mdsthreadstatic.h"
#include "../mdsip_connections.h"

#define MDSIPTHREADSTATIC_VAR MdsIpThreadStatic_p
#define MDSIPTHREADSTATIC_TYPE MdsIpThreadStatic_t
#define MDSIPTHREADSTATIC_ARG MDSIPTHREADSTATIC_TYPE *MDSIPTHREADSTATIC_VAR
#define MDSIPTHREADSTATIC(MTS) MDSIPTHREADSTATIC_ARG = MdsIpGetThreadStatic(MTS)
#define MDSIPTHREADSTATIC_INIT MDSIPTHREADSTATIC(NULL)
typedef struct
{
  Connection *connections;
  uint32_t clientaddr;
} MDSIPTHREADSTATIC_TYPE;
#define MDSIP_CLIENTADDR MDSIPTHREADSTATIC_VAR->clientaddr
#define MDSIP_CONNECTIONS MDSIPTHREADSTATIC_VAR->connections

extern DEFINE_GETTHREADSTATIC(MDSIPTHREADSTATIC_TYPE, MdsIpGetThreadStatic);
