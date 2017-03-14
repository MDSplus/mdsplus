#include "dcl_p.h"
#include <pthread_port.h>
typedef struct _ThreadStatic {
char *prompt;
char *def_file;
} ThreadStatic;

#define GET_THREADSTATIC_P ThreadStatic *ThreadStatic_p = GetThreadStatic()
#define PROMPT   (ThreadStatic_p->prompt)
#define DEF_FILE (ThreadStatic_p->def_file)


extern ThreadStatic *GetThreadStatic();
extern void mdsdclSetPrompt(const char *prompt);
extern char *mdsdclGetPrompt();
extern void mdsdclSetDefFile(const char *deffile);
extern void dclLock();
extern void dclUnlock();


