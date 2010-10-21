/*************************************************************************\
* Copyright (c) 2010 - RFX.
* EPICS BASE is distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution. 
\*************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "dbDefs.h"
#include "epicsPrint.h"
#include "alarm.h"
#include "dbAccess.h"
#include "dbEvent.h"
#include "dbFldTypes.h"
#include "dbScan.h"
#include "devSup.h"
#include "errMdef.h"
#include "recSup.h"
#include "recGbl.h"
#include "cantProceed.h"
#include "menuYesNo.h"
#include "mdsactionRecord.h"
#include "epicsExport.h"
#include "epicsString.h"

#include "callback.h"
#include "epicsThread.h"

/* Create the dset for devWfSoft */
static long init_record(mdsactionRecord *prec);
static long read_ma(mdsactionRecord *prec);
extern int openMds(char *expName, int shot, int isLocal, char *ipAddr, char *path, unsigned int *nodeId, char *errMsg);
extern int doMdsAction(char *path, int nodeId, char *errMsg);
struct {
    long      number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN read_ma;
} devMaSoft = {
    5,
    NULL,
    NULL,
    init_record,
    NULL,
    read_ma
};
epicsExportAddress(dset, devMaSoft);

static void callbackFunc(CALLBACK *pcallback)
{
     struct dbCommon  *precord;
     rset      *prset;
     
     callbackGetUser(precord,pcallback);
     prset = (rset *)(precord->rset);
     dbScanLock(precord);
     (*prset->process)(precord);
     dbScanUnlock(precord);
}



static long init_record(mdsactionRecord *prec)
{
     CALLBACK *pcallback;
     pcallback = (CALLBACK *)(calloc(1,sizeof(CALLBACK)));
     callbackSetCallback(callbackFunc,pcallback);
     callbackSetUser(prec,pcallback);
     prec->dpvt = (void *)pcallback;
 
     prec->errs = openMds(prec->exp, prec->shot, (prec->loc == mdsactionLOCREM_Local), prec->dsip, 
		prec->path, &prec->nid, prec->errm);
    return 0;
}

static long asynchRead(CALLBACK *pcallback)
{
     struct dbCommon  *precord;
     struct rset      *prset;
     mdsactionRecord *prec;
     long nRequest;

     callbackGetUser(precord,pcallback);
     prec = (mdsactionRecord *)precord;
     prec->errs = doMdsAction(prec->path, prec->nid, prec->errm);
     callbackRequest(pcallback);
     return 0;
}


static long read_ma(mdsactionRecord *prec)
{
     CALLBACK *pcallback = (CALLBACK *)prec->dpvt;
     if(prec->pact) {
          prec->udf = FALSE;
          return 0;
     }
     prec->pact=TRUE;

    epicsThreadCreate("mdsaction_THREAD", epicsThreadPriorityHigh, epicsThreadGetStackSize(epicsThreadStackMedium), asynchRead, pcallback);
    return 0;
}



