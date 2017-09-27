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

/* mdsputRecord.c - Reccord support for MDSplus data storage */


#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
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
#define GEN_SIZE_OFFSET
#include "mdsexprRecord.h"
#undef  GEN_SIZE_OFFSET
#include "epicsExport.h"
#include "epicsString.h"

/* Create RSET - Record Support Entry Table*/
#define report NULL
#define initialize NULL
static long init_record(mdsexprRecord *, int);
static long process(mdsexprRecord *);
#define special NULL
#define get_value NULL
static long cvt_dbaddr(DBADDR *);
static long get_array_info(DBADDR *, long *, long *);
static long put_array_info(DBADDR *, long);
static long get_units(DBADDR *, char *);
static long get_precision(DBADDR *, long *);
#define get_enum_str NULL
#define get_enum_strs NULL
#define put_enum_str NULL
static long get_graphic_double(DBADDR *, struct dbr_grDouble *);
static long get_control_double(DBADDR *, struct dbr_ctrlDouble *);
#define get_alarm_double NULL
rset mdsexprRSET={
        RSETNUMBER,
        report,
        initialize,
        init_record,
        process,
        special,
        get_value,
        cvt_dbaddr,
        get_array_info,
        put_array_info,
        get_units,
        get_precision,
        get_enum_str,
        get_enum_strs,
        put_enum_str,
        get_graphic_double,
        get_control_double,
        get_alarm_double
};
epicsExportAddress(rset,mdsexprRSET);
struct mpdset { /* mdsexpr dset */
        long            number;
        DEVSUPFUN       dev_report;
        DEVSUPFUN       init;
        DEVSUPFUN       init_record; /*returns: (-1,0)=>(failure,success)*/
        DEVSUPFUN       get_ioint_info;
        DEVSUPFUN       read_mp; /*returns: (-1,0)=>(failure,success)*/
};

static void monitor(mdsexprRecord *);
static long readValue(mdsexprRecord *);
static void checkAlarms(mdsexprRecord *prec);

static long init_record(mdsexprRecord *prec, int pass)
{
    struct mpdset *pdset;
    long status;
    if (pass==0){
        if (prec->nelm <= 0)
            prec->nelm = 1;
        if (prec->ftvl > DBF_ENUM)
            prec->ftvl = DBF_UCHAR;
        prec->bptr = callocMustSucceed(prec->nelm, dbValueSize(prec->ftvl),
            "mdsexpr calloc failed");
        if (prec->nelm == 1) {
            prec->nord = 1;
        } else {
            prec->nord = 0;
        }
        return 0;
    }

    /* wf.siml must be a CONSTANT or a PV_LINK or a DB_LINK */
    if (prec->siml.type == CONSTANT) {
        recGblInitConstantLink(&prec->siml,DBF_USHORT,&prec->simm);
    }

    /* must have dset defined */
    if (!(pdset = (struct mpdset *)(prec->dset))) {
        recGblRecordError(S_dev_noDSET,(void *)prec,"mp: init_record");
        return S_dev_noDSET;
    }
    /* must have read_wf function defined */
    if ((pdset->number < 5) || (pdset->read_mp == NULL)) {
        recGblRecordError(S_dev_missingSup,(void *)prec,"mp: init_record");
        return S_dev_missingSup;
    }
    if (! pdset->init_record) return 0;

    status =  (*pdset->init_record)(prec);
    checkAlarms(prec);
    return status;
}

static long process(mdsexprRecord *prec)
{
    struct mpdset *pdset = (struct mpdset *)(prec->dset);
    long           status;
    unsigned char  pact=prec->pact;

    if ((pdset==NULL) || (pdset->read_mp==NULL)) {
        prec->pact=TRUE;
        recGblRecordError(S_dev_missingSup, (void *)prec, "read_mp");
        return S_dev_missingSup;
    }

    if (pact && prec->busy) return 0;

    recGblGetTimeStamp(prec);
    status=readValue(prec); /* read the new value */
    if (!pact && prec->pact) return 0;

    prec->pact = TRUE;
    prec->udf = FALSE;
//   recGblGetTimeStamp(prec);

    checkAlarms(prec);
    monitor(prec);

    /* process the forward scan link record */
    recGblFwdLink(prec);

    prec->pact=FALSE;
    return 0;
}

static long cvt_dbaddr(DBADDR *paddr)
{
    mdsexprRecord *prec = (mdsexprRecord *) paddr->precord;

    paddr->pfield = prec->bptr;
    paddr->no_elements = prec->nelm;
    paddr->field_type = prec->ftvl;
    paddr->field_size = dbValueSize(prec->ftvl);
    paddr->dbr_field_type = prec->ftvl;

    return 0;
}

static long get_array_info(DBADDR *paddr, long *no_elements, long *offset)
{
    mdsexprRecord *prec = (mdsexprRecord *) paddr->precord;

    *no_elements =  prec->nord;
    *offset = 0;

    return 0;
}

static long put_array_info(DBADDR *paddr, long nNew)
{
    mdsexprRecord *prec = (mdsexprRecord *) paddr->precord;

    prec->nord = nNew;
    if (prec->nord > prec->nelm)
        prec->nord = prec->nelm;

    return 0;
}

static long get_units(DBADDR *paddr, char *units)
{
    mdsexprRecord *prec = (mdsexprRecord *) paddr->precord;

    strncpy(units,prec->egu,DB_UNITS_SIZE);

    return 0;
}

static long get_precision(DBADDR *paddr, long *precision)
{
    mdsexprRecord *prec = (mdsexprRecord *) paddr->precord;
    int fieldIndex = dbGetFieldIndex(paddr);

    *precision = prec->prec;

    if (fieldIndex != mdsexprRecordVAL)
        recGblGetPrec(paddr, precision);

    return 0;
}

static long get_graphic_double(DBADDR *paddr, struct dbr_grDouble *pgd)
{
    mdsexprRecord *prec = (mdsexprRecord *) paddr->precord;

    if (dbGetFieldIndex(paddr) == mdsexprRecordVAL) {
        pgd->upper_disp_limit = prec->hopr;
        pgd->lower_disp_limit = prec->lopr;
    } else
        recGblGetGraphicDouble(paddr, pgd);
    return 0;
}

static long get_control_double(DBADDR *paddr, struct dbr_ctrlDouble *pcd)
{
    mdsexprRecord *prec = (mdsexprRecord *) paddr->precord;

    if (dbGetFieldIndex(paddr) == mdsexprRecordVAL) {
        pcd->upper_ctrl_limit = prec->hopr;
        pcd->lower_ctrl_limit = prec->lopr;
    } else
        recGblGetControlDouble(paddr, pcd);
    return 0;
}

static void monitor(mdsexprRecord *prec)
{
    unsigned short monitor_mask = 0;
    unsigned int hash = 0;

    monitor_mask = recGblResetAlarms(prec);

    if (prec->mpst == mdsexprPOST_Always)
        monitor_mask |= DBE_VALUE;
    if (prec->apst == mdsexprPOST_Always)
        monitor_mask |= DBE_LOG;

    /* Calculate hash if we are interested in OnChange events. */
    if ((prec->mpst == mdsexprPOST_OnChange) ||
        (prec->apst == mdsexprPOST_OnChange)) {
        hash = epicsMemHash((char *)prec->bptr,
            prec->nord * dbValueSize(prec->ftvl), 0);

        /* Only post OnChange values if the hash is different. */
        if (hash != prec->hash) {
            if (prec->mpst == mdsexprPOST_OnChange)
                monitor_mask |= DBE_VALUE;
            if (prec->apst == mdsexprPOST_OnChange)
                monitor_mask |= DBE_LOG;

            /* Store hash for next process. */
            prec->hash = hash;
            /* Post HASH. */
            db_post_events(prec, &prec->hash, DBE_VALUE);
        }
    }

    if (monitor_mask) {
        db_post_events(prec, prec->bptr, monitor_mask);
    }
}

static long readValue(mdsexprRecord *prec)
{
    long          status;
    struct mpdset *pdset = (struct mpdset *) prec->dset;

    if (prec->pact == TRUE){
        return (*pdset->read_mp)(prec);
    }

    status = dbGetLink(&(prec->siml), DBR_ENUM, &(prec->simm),0,0);
    if (status)
        return status;

    if (prec->simm == menuYesNoNO){
        return (*pdset->read_mp)(prec);
    }

    if (prec->simm == menuYesNoYES){
        long nRequest = prec->nelm;
        status = dbGetLink(&(prec->siol), prec->ftvl, prec->bptr, 0, &nRequest);
        /* nord set only for db links: needed for old db_access */
        if (prec->siol.type != CONSTANT) {
            prec->nord = nRequest;
            if (status == 0)
                prec->udf=FALSE;
        }
    } else {
        recGblSetSevr(prec, SOFT_ALARM, INVALID_ALARM);
        return -1;
    }
    recGblSetSevr(prec, SIMM_ALARM, prec->sims);

    return status;
}
static void checkAlarms(mdsexprRecord *prec)
{
	int status = prec->errs;


        if(prec->udf == TRUE){
                recGblSetSevr(prec,UDF_ALARM,INVALID_ALARM);
                return;
        }

	if(!(status & 1))
            recGblSetSevr(prec,WRITE_ALARM,MAJOR_ALARM);
}

