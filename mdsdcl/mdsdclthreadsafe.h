/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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
