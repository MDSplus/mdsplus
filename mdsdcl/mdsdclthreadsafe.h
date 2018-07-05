#include "dcl_p.h"
#include <pthread_port.h>
typedef struct _ThreadStatic {
char *prompt;
char *def_file;
dclDocListPtr dclDocs;
} ThreadStatic;

#define GET_THREADSTATIC_P ThreadStatic *ThreadStatic_p = mdsdclGetThreadStatic()
#define PROMPT   (ThreadStatic_p->prompt)
#define DEF_FILE (ThreadStatic_p->def_file)
#define DCLDOCS  (ThreadStatic_p->dclDocs)


extern ThreadStatic *mdsdclGetThreadStatic();
extern void mdsdclSetPrompt(const char *prompt);
extern char *mdsdclGetPrompt();
extern void mdsdclSetDefFile(const char *deffile);
extern void mdsdclAllocDocDef(dclDocListPtr doc_l);
extern void dclLock();
extern void dclUnlock();
