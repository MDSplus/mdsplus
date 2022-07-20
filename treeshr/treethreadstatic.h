#ifndef _TREETHREADSTATIC_H
#define _TREETHREADSTATIC_H

#include "../mdsshr/mdsthreadstatic.h"
#include "treeshrp.h"

#define HOST_UNIQUE_SIZE 64
typedef struct host
{
  struct host *next;
  int conid;
  int links;
  char unique[HOST_UNIQUE_SIZE];
} Host;
#define HOST_PRI "Host(conid=%d, links=%d, unique='%s')"
#define HOST_VAR(h) (h)->conid, (h)->links, (h)->unique

void destroy_host(Host *host);

#define TREETHREADSTATIC_VAR TreeThreadStatic_p
#define TREETHREADSTATIC_TYPE TreeThreadStatic_t
#define TREETHREADSTATIC_ARG TREETHREADSTATIC_TYPE *TREETHREADSTATIC_VAR
#define TREETHREADSTATIC(MTS) TREETHREADSTATIC_ARG = TreeGetThreadStatic(MTS)
#define TREETHREADSTATIC_INIT TREETHREADSTATIC(NULL)
typedef struct
{
  void *dbid;
  Host *hostlist;
  int64_t view_date;
  NCI temp_nci;
  int private_ctx;
  int nid_ref;
  int path_ref;
} TREETHREADSTATIC_TYPE;
#define TREE_DBID TREETHREADSTATIC_VAR->dbid
#define TREE_HOSTLIST TREETHREADSTATIC_VAR->hostlist
#define TREE_PRIVATECTX TREETHREADSTATIC_VAR->private_ctx
#define TREE_VIEWDATE TREETHREADSTATIC_VAR->view_date
#define TREE_NIDREF TREETHREADSTATIC_VAR->nid_ref
#define TREE_PATHREF TREETHREADSTATIC_VAR->path_ref
#define TREE_TEMPNCI TREETHREADSTATIC_VAR->temp_nci

extern DEFINE_GETTHREADSTATIC(TREETHREADSTATIC_TYPE, TreeGetThreadStatic);

extern void **TreeCtx();
extern EXPORT int TreeUsePrivateCtx(int onoff);
#endif // ifndef _TREETHREADSTATIC_H
