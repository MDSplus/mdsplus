#pragma once
#include "../mdsshr/mdsthreadstatic.h"
#include "treeshrp.h"

typedef struct
{
  int conid;
  int connections;
  char *unique;
} host_t;

typedef struct host_list
{
  struct host_list *next;
  host_t h;
} host_list_t;

void destroy_host_list_t(host_list_t *host);

#define TREETHREADSTATIC_VAR TreeThreadStatic_p
#define TREETHREADSTATIC_TYPE TreeThreadStatic_t
#define TREETHREADSTATIC_ARG TREETHREADSTATIC_TYPE *TREETHREADSTATIC_VAR
#define TREETHREADSTATIC(MTS) TREETHREADSTATIC_ARG = TreeGetThreadStatic(MTS)
#define TREETHREADSTATIC_INIT TREETHREADSTATIC(NULL)
typedef struct
{
  void *dbid;
  host_list_t *hostlist;
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
