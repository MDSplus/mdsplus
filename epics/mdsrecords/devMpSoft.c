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
#include "mdsputRecord.h"
#include "epicsExport.h"
#include "epicsString.h"

#include "callback.h"
#include "epicsThread.h"

/* Create the dset for devWfSoft */
static long init_record(mdsputRecord *prec);
static long read_mp(mdsputRecord *prec);
extern int openMds(char *expName, int shot, int isLocal, char *ipAddr, char *path, unsigned int *nodeId, char *errMsg);
extern int writeMds(int nodeId, double *vals, int dtype, int preTrigger, int nVals, int dim1, int dim2, int dataIdx, double period, double trigger, long  epicsTime, char *errMsg, int debug);
//extern int writeMds(int nodeId, int dtype, double *vals, int nVals, int dim1, int dim2, int dataIdx, int isEpicsTime, long *epicsTime, int preTrigger, double period, double trigger, char *errMsg);
struct {
    long      number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN read_mp;
} devMpSoft = {
    5,
    NULL,
    NULL,
    init_record,
    NULL,
    read_mp
};
epicsExportAddress(dset, devMpSoft);

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



static long init_record(mdsputRecord *prec)
{
     CALLBACK *pcallback;
    /* INP must be CONSTANT, PV_LINK, DB_LINK or CA_LINK*/
    switch (prec->inp.type) {
    case CONSTANT:
        prec->nord = 0;
        break;
    case PV_LINK:
    case DB_LINK:
    case CA_LINK:
          pcallback = (CALLBACK *)(calloc(1,sizeof(CALLBACK)));
          callbackSetCallback(callbackFunc,pcallback);
          callbackSetUser(prec,pcallback);
          prec->dpvt = (void *)pcallback;
        break;
    default:
        recGblRecordError(S_db_badField, (void *)prec,
            "devMpSoft (init_record) Illegal INP field");
        return(S_db_badField);
    }
    prec->errs = openMds(prec->exp, prec->shot, (prec->loc == mdsputLOCREM_Local), prec->dsip, 
		prec->path, &prec->nid, prec->errm);
    prec->didx = 0;
    return 0;
}

static long asynchRead(CALLBACK *pcallback)
{
     struct dbCommon  *precord;
     struct rset      *prset;
     mdsputRecord *prec;
     long nRequest;
     int nRows;
     int i;
     long currTime;

     callbackGetUser(precord,pcallback);
     prec = (mdsputRecord *)precord;
     nRequest = prec->nelm;

    dbGetLink(&prec->inp, prec->ftvl, prec->bptr, 0, &nRequest);
    if (nRequest > 0) {
        prec->nord = nRequest;
        if (prec->tsel.type == CONSTANT &&
            prec->tse == epicsTimeEventDeviceTime)
            dbGetTimeStamp(&prec->inp, &prec->time);

//extern int writeMds(int nodeId, double *vals, int nVals, int dim1, int dim2, int dataIdx, int isEpicsTime, long epicsTime, int preTrigger, double period, double trigger, char *errMsg);

        if(prec->dim1 > 0 && prec->dim2 > 0)
	    nRows = prec->nord/(prec->dim1*prec->dim2);
    	else if(prec->dim1 > 0)
	    nRows = prec->nord/prec->dim1;
    	else
	    nRows = prec->nord;

	currTime = (unsigned long)prec->time.nsec + ((unsigned long)prec->time.secPastEpoch * 1000000000L);

    	prec->errs = writeMds(prec->nid, prec->bptr, prec->ftvl, prec->pre, nRows, prec->dim1, prec->dim2, prec->didx, 
	    prec->prd, prec->trig, currTime, prec->errm, prec->dbg);
 
        prec->didx += nRows;
    }
    callbackRequest(pcallback);
    return 0;
}


static long read_mp(mdsputRecord *prec)
{
    long nRequest = prec->nelm;
     CALLBACK *pcallback = (CALLBACK *)prec->dpvt;
     if(prec->pact) {
          prec->udf = FALSE;
          return 0;
     }
     prec->pact=TRUE;


    dbGetLink(&prec->inp, prec->ftvl, prec->bptr, 0, &nRequest);



    epicsThreadCreate("MDSPUT_THREAD", epicsThreadPriorityHigh, epicsThreadGetStackSize(epicsThreadStackMedium), asynchRead, pcallback);
    return 0;
}



