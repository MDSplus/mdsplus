#include <mdsplus/mdsconfig.h>
#include <mdsdescrip.h>
#include "treeshrp.h"
#include <pthread_port.h>

typedef struct _thread_static {
  void *DBID;
  int privateCtx;
  SEARCH_CONTEXT search_ctx[MAX_SEARCH_LEVELS];
  int64_t ViewDate;
  int nid_reference;
  int path_reference;
  NCI TemplateNci;
  char TreeFindTagWild_answer[128];
  struct descriptor dsc_d;  // used for TreeFileName but possibly reusable for other funs too
} TreeThreadStatic;

#define TREEGETTHREADSTATIC_P TreeThreadStatic * TreeGetThreadStatic_p = TreeGetThreadStatic()

extern TreeThreadStatic *TreeGetThreadStatic();
extern void **TreeCtx();
extern EXPORT int TreeUsePrivateCtx(int onoff);
