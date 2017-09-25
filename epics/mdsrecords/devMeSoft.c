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



