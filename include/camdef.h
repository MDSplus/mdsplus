#ifndef __CAMDEF
#define __CAMDEF

#define CamDONE_Q    0x8018009
#define CamDONE_NOQ  0x8018011
#define CamDONE_NOX  0x8019000
#define CamSERTRAERR 0x8019802
#define CamSCCFAIL   0x8019842
#define CamOFFLINE   0x801986a
#define CamXE        0
#define CamQE        0
#define CamQDC       1
#define CamQNE       2
#define CamXI        4

#ifdef CREATE_STS_TEXT
#include        "facility_list.h"

static struct stsText  camshr_stsText[] = {
    STS_TEXT(CamDONE_Q,"I/O completed with X=1, Q=1")
   ,STS_TEXT(CamDONE_NOQ,"I/O completed with X=1, Q=0")
   ,STS_TEXT(CamDONE_NOX,"I/O completed with X=0 - probable failure")
   ,STS_TEXT(CamSERTRAERR,"serial transmission error on highway")
   ,STS_TEXT(CamSCCFAIL,"serial crate controller failure")
   ,STS_TEXT(CamOFFLINE,"crate is offline")
};
#endif

#endif
