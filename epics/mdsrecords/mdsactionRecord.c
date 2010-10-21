/*************************************************************************\
* Copyright (c) 2010 - RFX.
* EPICS BASE is distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution. 
\*************************************************************************/

/* mdsactionRecord.c - Reccord support for MDSplus local/remote action */


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
#include "mdsactionRecord.h"
#undef  GEN_SIZE_OFFSET
#include "epicsExport.h"
#include "epicsString.h"

/* Create RSET - Record Support Entry Table*/
#define report NULL
#define initialize NULL
static long init_record(mdsactionRecord *, int);
static long process(mdsactionRecord *);
#define special NULL
#define get_value NULL
#define cvt_dbaddr NULL
#define get_array_info NULL
#define put_array_info NULL
static long get_units(DBADDR *, char *);
static long get_precision(DBADDR *, long *);
#define get_enum_str NULL
#define get_enum_strs NULL
#define put_enum_str NULL
static long get_graphic_double(DBADDR *, struct dbr_grDouble *);
static long get_control_double(DBADDR *, struct dbr_ctrlDouble *);
#define get_alarm_double NULL
rset mdsactionRSET={
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
epicsExportAddress(rset,mdsactionRSET);
struct madset { /* mdsaction dset */
        long            number;
        DEVSUPFUN       dev_report;
        DEVSUPFUN       init;
        DEVSUPFUN       init_record; /*returns: (-1,0)=>(failure,success)*/
        DEVSUPFUN       get_ioint_info;
        DEVSUPFUN       read_ma; /*returns: (-1,0)=>(failure,success)*/
};

static void monitor(mdsactionRecord *);
static long doAction(mdsactionRecord *);
static void checkAlarms(mdsactionRecord *prec);

static long init_record(mdsactionRecord *prec, int pass)
{
    struct madset *pdset;
    long status;
    if (pass==0){
        return 0;
    }

    /* wf.siml must be a CONSTANT or a PV_LINK or a DB_LINK */
    if (prec->siml.type == CONSTANT) {
        recGblInitConstantLink(&prec->siml,DBF_USHORT,&prec->simm);
    }

    /* must have dset defined */
    if (!(pdset = (struct madset *)(prec->dset))) {
        recGblRecordError(S_dev_noDSET,(void *)prec,"ma: init_record");
        return S_dev_noDSET;
    }
    /* must have read_wf function defined */
    if ((pdset->number < 5) || (pdset->read_ma == NULL)) {
        recGblRecordError(S_dev_missingSup,(void *)prec,"ma: init_record");
        return S_dev_missingSup;
    }
    if (! pdset->init_record) return 0;

    status =  (*pdset->init_record)(prec);
    checkAlarms(prec);
    return status;
}

static long process(mdsactionRecord *prec)
{
    struct madset *pdset = (struct madset *)(prec->dset);
    long           status;
    unsigned char  pact=prec->pact;

    if ((pdset==NULL) || (pdset->read_ma==NULL)) {
        prec->pact=TRUE;
        recGblRecordError(S_dev_missingSup, (void *)prec, "read_ma");
        return S_dev_missingSup;
    }

    status=doAction(prec); /* read the new value */
    if (!pact && prec->pact) return 0;

    prec->pact = TRUE;
    prec->udf = FALSE;
    recGblGetTimeStamp(prec);

    checkAlarms(prec);
    monitor(prec);

    /* process the forward scan link record */
    recGblFwdLink(prec);

    prec->pact=FALSE;
    return 0;
}


static long get_units(DBADDR *paddr, char *units)
{
    mdsactionRecord *prec = (mdsactionRecord *) paddr->precord;

    strncpy(units,prec->egu,DB_UNITS_SIZE);

    return 0;
}

static long get_precision(DBADDR *paddr, long *precision)
{
    mdsactionRecord *prec = (mdsactionRecord *) paddr->precord;
    int fieldIndex = dbGetFieldIndex(paddr);

    *precision = prec->prec;

    if (fieldIndex != mdsactionRecordVAL)
        recGblGetPrec(paddr, precision);

    return 0;
}

static long get_graphic_double(DBADDR *paddr, struct dbr_grDouble *pgd)
{
    mdsactionRecord *prec = (mdsactionRecord *) paddr->precord;

    if (dbGetFieldIndex(paddr) == mdsactionRecordVAL) {
        pgd->upper_disp_limit = prec->hopr;
        pgd->lower_disp_limit = prec->lopr;
    } else
        recGblGetGraphicDouble(paddr, pgd);
    return 0;
}

static long get_control_double(DBADDR *paddr, struct dbr_ctrlDouble *pcd)
{
    mdsactionRecord *prec = (mdsactionRecord *) paddr->precord;

    if (dbGetFieldIndex(paddr) == mdsactionRecordVAL) {
        pcd->upper_ctrl_limit = prec->hopr;
        pcd->lower_ctrl_limit = prec->lopr;
    } else
        recGblGetControlDouble(paddr, pcd);
    return 0;
}

static void monitor(mdsactionRecord *prec)
{
    unsigned short monitor_mask = 0;
    unsigned int hash = 0;

    monitor_mask = recGblResetAlarms(prec);

    if (prec->mpst == mdsactionPOST_Always)
        monitor_mask |= DBE_VALUE;
    if (prec->apst == mdsactionPOST_Always)
        monitor_mask |= DBE_LOG;

    /* Calculate hash if we are interested in OnChange events. */
    if ((prec->mpst == mdsactionPOST_OnChange) ||
        (prec->apst == mdsactionPOST_OnChange)) {
	hash = prec->val;
        /* Only post OnChange values if the hash is different. */
        if (hash != prec->hash) {
            if (prec->mpst == mdsactionPOST_OnChange)
                monitor_mask |= DBE_VALUE;
            if (prec->apst == mdsactionPOST_OnChange)
                monitor_mask |= DBE_LOG;

            /* Store hash for next process. */
            prec->hash = hash;
            /* Post HASH. */
            db_post_events(prec, &prec->hash, DBE_VALUE);
        }
    }

    if (monitor_mask) {
        db_post_events(prec, &prec->val, monitor_mask);
    }
}

static long doAction(mdsactionRecord *prec)
{
    long          status;
    struct madset *pdset = (struct madset *) prec->dset;

    if (prec->pact == TRUE){
        return (*pdset->read_ma)(prec);
    }

    status = dbGetLink(&(prec->siml), DBR_ENUM, &(prec->simm),0,0);
    if (status)
        return status;

    if (prec->simm == menuYesNoNO){
        return (*pdset->read_ma)(prec);
    }

    recGblSetSevr(prec, SIMM_ALARM, prec->sims);

    return status;
}
static void checkAlarms(mdsactionRecord *prec)
{
	int status = prec->errs;


        if(prec->udf == TRUE){
                recGblSetSevr(prec,UDF_ALARM,INVALID_ALARM);
                return;
        }

	if(!(status & 1))
            recGblSetSevr(prec,WRITE_ALARM,MAJOR_ALARM);
}

