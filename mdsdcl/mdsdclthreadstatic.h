#pragma once
#include "dcl_p.h"
#include "../mdsshr/mdsthreadstatic.h"

#define DCLTHREADSTATIC_VAR  DclThreadStatic_p
#define DCLTHREADSTATIC_TYPE DclThreadStatic_t
#define DCLTHREADSTATIC_ARG  DCLTHREADSTATIC_TYPE *const DCLTHREADSTATIC_VAR
#define DCLTHREADSTATIC(MTS) DCLTHREADSTATIC_ARG = DclGetThreadStatic(MTS)
#define DCLTHREADSTATIC_INIT DCLTHREADSTATIC(NULL)
typedef struct {
  char *prompt;
  char *def_file;
  dclDocListPtr docs;
} DCLTHREADSTATIC_TYPE;
#define DCL_PROMPT	DCLTHREADSTATIC_VAR->prompt
#define DCL_DEFFILE	DCLTHREADSTATIC_VAR->def_file
#define DCL_DOCS	DCLTHREADSTATIC_VAR->docs

extern DEFINE_GETTHREADSTATIC(DCLTHREADSTATIC_TYPE, DclGetThreadStatic);
