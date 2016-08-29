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
#include "waveformRecord.h"
#include "epicsExport.h"
#include "epicsString.h"

#include "callback.h"
#include "epicsThread.h"

/* Create the dset for devWfSoft */
static long init_record(waveformRecord *prec);
static long read_me(waveformRecord *prec);
extern int registerMdsEvent(char *eventName);
extern int waitMdsEvent(char *eventName, char *buf, int maxLen, int *retLen);
struct {
    long      number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN read_me;
} devMeSoft = {
    5,
    NULL,
    NULL,
    init_record,
    NULL,
    read_me
};
epicsExportAddress(dset, devMeSoft);

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



static long init_record(waveformRecord *prec)
{
     CALLBACK *pcallback;
     pcallback = (CALLBACK *)(calloc(1,sizeof(CALLBACK)));
     callbackSetCallback(callbackFunc,pcallback);
     callbackSetUser(prec,pcallback);
     prec->dpvt = (void *)pcallback;
     registerMdsEvent(prec->inp.value.instio.string);
     return 0;
}

static long asynchRead(CALLBACK *pcallback)
{
     struct dbCommon  *precord;
     waveformRecord *prec;
     int retLen, maxLen, elSize = 1;

     callbackGetUser(precord,pcallback);
     prec = (waveformRecord *)precord;
     switch(prec->ftvl)  {
     	case DBF_UCHAR: 
     	case DBF_CHAR: 
     		elSize = 1;
     		break;
     	case DBF_USHORT: 
     	case DBF_SHORT: 
     		elSize = 2;
     		break;
      	case DBF_ULONG: 
      	case DBF_LONG: 
      	case DBF_FLOAT: 
     		elSize = 4;
     		break;
      	case DBF_DOUBLE: 
     		elSize = 8;
     		break;
    	default:	
        	printf("Unexpected type for Waveform MDSplus event record\n");
        	return 0;
     }	
     maxLen = prec->nelm*elSize;
     waitMdsEvent(prec->inp.value.instio.string, prec->bptr, maxLen, &retLen); 
     	prec->nord = retLen/elSize;

     callbackRequest(pcallback);
     return 0;
}


static long read_me(waveformRecord *prec)
{
     CALLBACK *pcallback = (CALLBACK *)prec->dpvt;
     if(prec->pact) {
          prec->udf = FALSE;
          return 0;
     }
     prec->pact=TRUE;

    epicsThreadCreate("MDSPUT_THREAD", epicsThreadPriorityHigh, epicsThreadGetStackSize(epicsThreadStackMedium), asynchRead, pcallback);
    return 0;
}



