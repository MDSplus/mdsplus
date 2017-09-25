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
#include "mdsexprRecord.h"
#include "epicsExport.h"
#include "epicsString.h"

#include "callback.h"
#include "epicsThread.h"

/* Create the dset for devWfSoft */
static long init_record(mdsexprRecord *prec);
static long read_mp(mdsexprRecord *prec);
extern int openMds(char *expName, int shot, int isLocal, char *ipAddr, char *path, unsigned int *nodeId, char *errMsg);
extern int evaluateExpr(char *expr, int treeIdx, int nBuffers, void **buffers, int *bufDims, int *bufTypes, int maxRetElements, int retType, void *retBuf, int *retElements, char *errMsg, int isLocal);

struct {
    long      number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN read_mp;
} devExprSoft = {
    5,
    NULL,
    NULL,
    init_record,
    NULL,
    read_mp
};
epicsExportAddress(dset, devExprSoft);

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



static long init_record(mdsexprRecord *prec)
{
    CALLBACK *pcallback;
    pcallback = (CALLBACK *)(calloc(1,sizeof(CALLBACK)));
    callbackSetCallback(callbackFunc,pcallback);
    callbackSetUser(prec,pcallback);
    prec->dpvt = (void *)pcallback;
    /* INP must be CONSTANT, PV_LINK, DB_LINK or CA_LINK*/
/*    switch (prec->inp.type) {
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
*/
    if(strlen(prec->exp) > 0 || (prec->loc == mdsexprLOCREM_Remote)) //If an experiment name is defined, or it is remote open it
    	prec->errs = openMds(prec->exp, prec->shot, (prec->loc == mdsexprLOCREM_Local), prec->dsip, 
		0, &prec->tidx, prec->errm);
    else
	prec->tidx = -1;
    return 0;
}

static long asynchRead(CALLBACK *pcallback)
{
    struct dbCommon  *precord;
    struct rset      *prset;
    mdsexprRecord *prec;
    long nRequest;
    int nRows;
    int i;
    long currTime;
    void *buffers[8];
    int bufDims[8];
    int bufTypes[8];
    int nBuffers;

    callbackGetUser(precord,pcallback);
    prec = (mdsexprRecord *)precord;
    nRequest = prec->nelm;

    for(i = 0; i < 8; i++)
    {
	buffers[i] = malloc(8*prec->nelm); //Allocate maximum memory
	bufDims[i] = prec->nelm;
    }
    dbGetLink(&prec->inp1, prec->fti1, buffers[0], 0, (void *)&bufDims[0]);
    dbGetLink(&prec->inp2, prec->fti2, buffers[1], 0, (void *)&bufDims[1]);
    dbGetLink(&prec->inp3, prec->fti3, buffers[2], 0, (void *)&bufDims[2]);
    dbGetLink(&prec->inp4, prec->fti4, buffers[3], 0, (void *)&bufDims[3]);
    dbGetLink(&prec->inp5, prec->fti5, buffers[4], 0, (void *)&bufDims[4]);
    dbGetLink(&prec->inp6, prec->fti6, buffers[5], 0, (void *)&bufDims[5]);
    dbGetLink(&prec->inp7, prec->fti7, buffers[6], 0, (void *)&bufDims[6]);
    dbGetLink(&prec->inp8, prec->fti8, buffers[7], 0, (void *)&bufDims[7]);

    bufTypes[0] = prec->fti1;
    bufTypes[1] = prec->fti2;
    bufTypes[2] = prec->fti3;
    bufTypes[3] = prec->fti4;
    bufTypes[4] = prec->fti5;
    bufTypes[5] = prec->fti6;
    bufTypes[6] = prec->fti7;
    bufTypes[7] = prec->fti8;

    for(nBuffers = 0; nBuffers < 8; nBuffers++)
	if(bufDims[nBuffers] == 0)
	    break;

    prec->errs = evaluateExpr(prec->expr, prec->tidx, nBuffers, buffers, bufDims, bufTypes, prec->nelm, prec->ftvl, prec->bptr, &prec->nord, prec->errm, (prec->loc == mdsexprLOCREM_Local));


//Deallocate memory
    for(i = 0; i < 8; i++)
	free(buffers[i]);

//    dbGetLink(&prec->inp, prec->ftvl, prec->bptr, 0, &nRequest);
/*    if (nRequest > 0) {
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
*/
    callbackRequest(pcallback);
    return 0;
}


static long read_mp(mdsexprRecord *prec)
{
    long nRequest = prec->nelm;
     CALLBACK *pcallback = (CALLBACK *)prec->dpvt;
     if(prec->pact) {
          prec->udf = FALSE;
          return 0;
     }
     prec->pact=TRUE;

/*************************Set Intermediate result to 0 UNTIL SET BY asyncRead*********************************/
     printf("Scrivo zero\n");
     switch(prec->ftvl) {
	    	case DBF_CHAR: 
	    	case DBF_UCHAR: 
		    *((char *)prec->bptr) = 0;
		    break;
	    	case DBF_SHORT: 
	    	case DBF_USHORT: 
		    *((short *)prec->bptr) = 0;
		    break;
	    	case DBF_LONG: 
	    	case DBF_ULONG: 
		    *((int *)prec->bptr) = 0;
		    break;
	    	case DBF_FLOAT: 
		    *((float *)prec->bptr) = 0;
		    break;
	    	case DBF_DOUBLE: 
		    *((double *)prec->bptr) = 0;
		    break;
	}

/**************************************************************************************************************/

    epicsThreadCreate("mdsexpr_THREAD", epicsThreadPriorityHigh, epicsThreadGetStackSize(epicsThreadStackMedium), asynchRead, pcallback);
//prec->pact = FALSE; //DA CAVARE
    return 0;
}



