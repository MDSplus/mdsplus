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
/* Copyright (C) 1997 Hewlett-Packatd Company */
/* VXIplug&play Instrument Driver fot the HPE1429 */

/*================================*/
/*           HPE1429              */
/*================================*/
/* Framework Revision: WIN95 4.0  */
/*                     WINNT 4.0  */
/* Driver Revision:    A.02.04    */
/* Firmware Revision:  A.01.00    */
/*================================*/

/*=============================*/
/* Source Files:               */
/*=============================*/
/*   hpe1429.c                 */
/*   hpe1429.h                 */
/*=============================*/

/*=============================*/
/* WIN95/WINNT Framework Files */
/*=============================*/
/*   hpe1429.def               */
/*   hpe1429.rc                */
/*   hpe1429.bas               */
/*=============================*/

#define MSGB_DRIVER

#include <stdlib.h>		/* prototype for malloc() */
#include <string.h>		/* prototype for strcpy() */
#include <stdio.h>		/* prototype for sprintf() */
#include <malloc.h>
#include <windows.h>
#include <math.h>
#include <sys\timeb.h>
#include <time.h>

#ifdef WIN32
#define _fmemcpy memcpy
#endif

#include "visa.h"
#undef _VI_FUNC
#define _VI_FUNC
#include "hpe1429.h"
#include "fast1429.h"

#define hpe1429_MODEL_CODE 448
#define hpe1429_MANF_ID     4095
#define hpe1429_IDN_STRING  "HEWLETT-PACKARD,E1429"

#define hpe1429_REV_CODE "A.02.03"  /* Driver Revision */	/* ??? you must change this */

#define hpe1429_ERR_MSG_LENGTH 256	/* size of error message buffer */

						    /*#define hpe1429_MAX_STAT_HAP 37*//* number of happenings ??? */
#define hpe1429_MAX_STAT_HAP 34
#define hpe1429_MAX_STAT_REG 4	/* number of IEEE 488.2 status registers */

/* this has to match the index of the ESR register in hpe1429_accessInfo[] */
#define hpe1429_ESR_REG_IDX 2

/* this needs to match the index location of hpe1429_USER_ERROR_HANLDER
 * in the hpe1429_statusHap[] array.  Normally, this is zero.
 */
#define hpe1429_USER_ERROR_HANDLER_IDX 0

/* you can remove the SWAP's below if you do not have block IO (aritrary block)
   if you do then you must also remove the routines that use them (_cmd_arr functions)
*/
#define SWAP_FLOAT64(dest) \
 { unsigned char    src[8];                                        \
      *((double *)src) = *((double *)dest);                        \
      ((unsigned char *)(dest))[0] =  ((unsigned char*)(src))[7];  \
      ((unsigned char *)(dest))[1] =  ((unsigned char*)(src))[6];  \
      ((unsigned char *)(dest))[2] =  ((unsigned char*)(src))[5];  \
      ((unsigned char *)(dest))[3] =  ((unsigned char*)(src))[4];  \
      ((unsigned char *)(dest))[4] =  ((unsigned char*)(src))[3];  \
      ((unsigned char *)(dest))[5] =  ((unsigned char*)(src))[2];  \
      ((unsigned char *)(dest))[6] =  ((unsigned char*)(src))[1];  \
      ((unsigned char *)(dest))[7] =  ((unsigned char*)(src))[0];  \
 }

#define SWAP_FLOAT32(dest) \
 { unsigned char    src[4];                                        \
      *((float *)src) = *((float *)dest);                        \
      ((unsigned char *)(dest))[0] =  ((unsigned char*)(src))[3];  \
      ((unsigned char *)(dest))[1] =  ((unsigned char*)(src))[2];  \
      ((unsigned char *)(dest))[2] =  ((unsigned char*)(src))[1];  \
      ((unsigned char *)(dest))[3] =  ((unsigned char*)(src))[0];  \
 }

#define SWAP_32(dest) \
 { unsigned char    src[4];                                        \
      *((long *)src) = *((long *)dest);                        \
      ((unsigned char *)(dest))[0] =  ((unsigned char*)(src))[3];  \
      ((unsigned char *)(dest))[1] =  ((unsigned char*)(src))[2];  \
      ((unsigned char *)(dest))[2] =  ((unsigned char*)(src))[1];  \
      ((unsigned char *)(dest))[3] =  ((unsigned char*)(src))[0];  \
 }

#define SWAP_16(dest) \
 { unsigned char    src[2];                                        \
      *((int *)src) = *((int *)dest);                        \
      ((unsigned char *)(dest))[0] =  ((unsigned char*)(src))[1];  \
      ((unsigned char *)(dest))[1] =  ((unsigned char*)(src))[0];  \
 }
/*=============================================================== 
 *
 *  All messages are stored in this area to aid in localization 
 *
 *=============================================================== 
 */

#define hpe1429_MSG_VI_OPEN_ERR 				\
	"vi was zero.  Was the hpe1429_init() successful?"

#define hpe1429_MSG_CONDITION					\
	"condition"
	/* hpe1429_statCond_Q() */

#define hpe1429_MSG_EVENT						\
	"event"
	/* hpe1429_statEvent_Q() */

#define hpe1429_MSG_EVENT_HDLR_INSTALLED				\
	"event handler is already installed for event happening"
	/* hpe1429_statEvent_Q() */

#define hpe1429_MSG_EVENT_HDLR_INST2				\
	"Only 1 handler can be installed at a time."
	/* hpe1429_statEvent_Q() */

#define hpe1429_MSG_INVALID_HAPPENING				\
	"is not a valid happening."
	/* hpe1429_statCond_Q() */
	/* hpe1429_statEven_Q() */
	/* hpe1429_statEvenHdlr() */
	/* hpe1429_statEvenHdlr_Q() */

#define hpe1429_MSG_NOT_QUERIABLE					\
	"is not queriable."
	/* hpe1429_statCond_Q() */
	/* hpe1429_statEven_Q() */

#define hpe1429_MSG_IN_FUNCTION					\
	"in function"
	/* hpe1429_error_message() */

#define hpe1429_MSG_INVALID_STATUS					\
  	"Parameter 2 is invalid"				\
	" in function hpe1429_error_message()."
	/* hpe1429_error_message() */

#define hpe1429_MSG_INVALID_STATUS_VALUE				\
	" is not a valid viStatus value."
	/* hpe1429_error_message() */

#define  hpe1429_MSG_INVALID_VI					\
  	"Parameter 1 is invalid"				\
	" in function hpe1429_error_message()"			\
	".  Using an inactive ViSession may cause this error."	\
	"  Was the instrument driver closed prematurely?"
	/* hpe1429_message_query() */

#define hpe1429_MSG_NO_ERRORS					\
	"No Errors."
	/* hpe1429_error_message() */

#define hpe1429_MSG_SELF_TEST_FAILED 				\
	"Self test failed."
	/* hpe1429_self_test() */

#define hpe1429_MSG_SELF_TEST_PASSED 				\
	"Self test passed."
	/* hpe1429_self_test() */

/* the following messages are used by the functions to check parameters */

#define hpe1429_MSG_BOOLEAN   "Expected 0 or 1; Got %hd."

#define hpe1429_MSG_REAL   "Expected %lg to %lg; Got %lg."

#define hpe1429_MSG_INT   "Expected %hd to %hd; Got %hd."

#define hpe1429_MSG_LONG   "Expected %ld to %ld; Got %ld."

#define hpe1429_MSG_LOOKUP "Error converting string response to integer."

#define hpe1429_MSG_NO_MATCH "Could not match string %s."

/* 
 * static error message 
 */

#define VI_ERROR_PARAMETER1_MSG				\
	"Parameter 1 is invalid."

#define VI_ERROR_PARAMETER2_MSG				\
	"Parameter 2 is invalid."

#define VI_ERROR_PARAMETER3_MSG				\
	"Parameter 3 is invalid."

#define VI_ERROR_PARAMETER4_MSG				\
	"Parameter 4 is invalid."

#define VI_ERROR_PARAMETER5_MSG				\
	"Parameter 5 is invalid."

#define VI_ERROR_PARAMETER6_MSG				\
	"Parameter 6 is invalid."

#define VI_ERROR_PARAMETER7_MSG				\
	"Parameter 7 is invalid."

#define VI_ERROR_PARAMETER8_MSG				\
	"Parameter 8 is invalid."

#define VI_ERROR_PARAMETER9_MSG				\
	"Parameter 9 is invalid."

#define VI_ERROR_PARAMETER10_MSG			\
	"Parameter 10 is invalid."

#define VI_ERROR_PARAMETER11_MSG			\
	"Parameter 11 is invalid."

#define VI_ERROR_PARAMETER12_MSG			\
	"Parameter 12 is invalid."

#define VI_ERROR_PARAMETER13_MSG			\
	"Parameter 13 is invalid."

#define VI_ERROR_PARAMETER14_MSG			\
	"Parameter 14 is invalid."

#define VI_ERROR_PARAMETER15_MSG			\
	"Parameter 15 is invalid."

#define VI_ERROR_PARAMETER16_MSG			\
	"Parameter 16 is invalid."

#define VI_ERROR_PARAMETER17_MSG			\
	"Parameter 17 is invalid."

#define VI_ERROR_PARAMETER18_MSG			\
	"Parameter 18 is invalid."

#define VI_ERROR_FAIL_ID_QUERY_MSG				\
	"Instrument IDN does not match."

#define INSTR_ERROR_INV_SESSION_MSG 				\
	"ViSession (parameter 1) was not created by this driver."

#define INSTR_ERROR_NULL_PTR_MSG				\
	"NULL pointer detected."

#define INSTR_ERROR_RESET_FAILED_MSG				\
	"Reset failed."

#define INSTR_ERROR_UNEXPECTED_MSG 				\
	"An unexpected error occurred."

#define INSTR_ERROR_DETECTED_MSG			\
	"Instrument Error Detected, call hpe1429_error_query()."

#define INSTR_ERROR_LOOKUP_MSG   				\
	"String not found in table."

/*================================================================*/

/* don't check the debug pointer all the time!*/
#ifdef DEBUG
#define hpe1429_DEBUG_CHK_THIS( vi, thisPtr) 			\
	/* check for NULL user data */				\
	if( 0 == thisPtr)					\
	{							\
 		hpe1429_LOG_STATUS(                             	\
		  vi, NULL, hpe1429_INSTR_ERROR_INV_SESSION );	\
	}							\
	{							\
		ViSession defRM;				\
								\
		/* This should never fail */			\
		errStatus = viGetAttribute( vi,                 \
			VI_ATTR_RM_SESSION, &defRM);		\
		if( VI_SUCCESS > errStatus )			\
		{						\
 			hpe1429_LOG_STATUS(				\
			  vi, NULL, hpe1429_INSTR_ERROR_UNEXPECTED );	\
		}						\
		if( defRM != thisPtr->defRMSession)		\
		{						\
 			hpe1429_LOG_STATUS(				\
			  vi, NULL, hpe1429_INSTR_ERROR_INV_SESSION );	\
		}						\
	}
#else
#define hpe1429_DEBUG_CHK_THIS( vi, thisPtr)
#endif

#ifdef WANT_CDE_INIT
#define hpe1429_CDE_INIT( funcname)  				\
	strcpy(thisPtr->errFuncName, funcname);			\
	thisPtr->errNumber = VI_SUCCESS;			\
	thisPtr->errMessage[0] = 0;

#define hpe1429_CDE_MESSAGE( message ) 	 			\
	strcpy(thisPtr->errMessage, message)
#else
#define hpe1429_CDE_INIT( funcname) 				\
	thisPtr->errNumber = VI_SUCCESS;			\

#define hpe1429_CDE_MESSAGE( message ) 	 			\
	strcpy(thisPtr->errMessage, message)
#endif

struct hpe1429_globals {
  ViSession defRMSession;

  ViStatus errNumber;
  char errFuncName[40];
  char errMessage[160];

  ViBoolean errQueryDetect;

  ViInt32 controler;
  /*    2 if embedded              */
  /*+4096 checked stuff already    */
  /*+8192 flag to not clear bits   */
  /* For Embedded:                 */
  /*   +1 if VXLINK embedded       */
  /*   +4 if fast init             */
  /*   +8 if never mapped before   */
  /*  +16 if embedded fast fetch   */
  /* >>5 & 7  == sampleTime div_by */
  /* >>8 & 15 == sampleTime power  */
  /*16384 if SCPI INIT done        */
  /*32768 if fast init disabled    */
  /*65536 if disable block xfer    */
  /* For E1406:                    */
  /*    1 if gpib to 1406          */
  /*  +16 if LBUS "off"  and       */
  /*      (precount==0 or          */
  /*       armCount==1)            */
  /* >>5 & 7 == VME Chan           */
  /*  256 VME:MODE                 */
  char cmdAddr[64];		/* e1406 resource         */
  long myDelay;
  long a24_addr;

  /* Setting this to VI_TRUE,
   * will prevent I/O from occuring in an SRQ
   * This is needed, because VTL 3.0 doesn't provide
   * an atomic write/read operations.  Don't want
   * to do I/O if the driver is in the middle of
   * a read from the instrument.
   */
  ViBoolean blockSrqIO;
  ViInt32 countSrqIO;
  int32 trigs_per_arm;
  int32 precount;

};

#ifdef WIN32

#include <windows.h>
#include <winbase.h>

long setDelay(double val)
{
//Delay should be absolute (e.g. 100 micro sec = 1e-4)

  double slice;
  _int64_t count;

  if (!QueryPerformanceFrequency((LARGE_INTEGER *) & count)) {
    //hdw doens't have high perfomance count so use getickcount
    slice = 1e-3;		//units for gettick count
  } else {
    slice = 1.0 / count;	//Seconds per tick
  }

  return (long)(val / slice) + 1;

}

void doDelay(long ticks)
{
  _int64_t startval, tmp;

  if (!QueryPerformanceCounter((LARGE_INTEGER *) & startval)) {
    DWORD sval;
    sval = GetTickCount();
    while (GetTickCount() - sval < (DWORD) ticks) ;
    return;
  }
  tmp = startval;
  while (tmp - startval < (DWORD) ticks) {
    QueryPerformanceCounter((LARGE_INTEGER *) & tmp);
  }

}

#else
long setDelay(double val)
{
  DWORD ts;
  DWORD te;
  ViInt32 count;
  ts = GetTickCount();
  count = 0;
  while (count < 100000)
    count++;
  te = GetTickCount();
  if ((te - ts) >= 100)		// looks like (te-ts)/100 uS per count
  {
    return (long)(val / ((te - ts) / 100000000.0)) + 1;
  }
  ts = GetTickCount();
  count = 0;
  while (count < 1000000)
    count++;
  te = GetTickCount();
  if ((te - ts) >= 100)		// looks like (te-ts) nS per count
  {
    return (long)(val / ((te - ts) / 1000000000.0)) + 1;
  }
  ts = GetTickCount();
  count = 0;
  while (count < 10000000)
    count++;
  te = GetTickCount();
  if ((te - ts) >= 100)		// looks like (te-ts)/10 nS per count
  {
    return (long)(val / ((te - ts) / 10000000000.0)) + 1;
  }
  ts = GetTickCount();
  count = 0;
  while (count < 100000000)
    count++;
  te = GetTickCount();
  if ((te - ts) >= 100)		// looks like (te-ts)/100 nS per count
  {
    return (long)(val / ((te - ts) / 100000000000.0)) + 1;
  }
  // just how fast is that clock?  Give up -- use GetTickCount
  return (long)(-((val / 1e-3) + 1));
}

void doDelay(long ticks)
{
  DWORD t = GetTickCount();
  long count;
  if (ticks < 0) {
    ticks = -ticks;
    while (GetTickCount() - t < (DWORD) ticks) ;
  } else {
    count = 0;
    while (count < ticks)
      count++;
  }
}
#endif

/* add the following to the globals data structure */

/* Note: for debugging, you can add __FILE__, __LINE__ as parameters
 * to hpe1429_LOG_STATUS, and ViString filename, and ViInt32 lineNumber to
 * hpe1429_statusUpdate() in order to determine exactly where an error
 * occured in a driver.
 */
#define hpe1429_LOG_STATUS( vi, thisPtr, status ) 	\
  return hpe1429_statusUpdate( vi, thisPtr, status)

/* declare this here since it is called by statusUpdate */
static void hpe1429_srqTraverse(ViSession vi, ViInt32 eventReg);

ViStatus hpe1429_statusUpdate(ViSession vi, struct hpe1429_globals *thisPtr, ViStatus s)
{
  ViUInt32 rc;
  ViStatus errStatus;
  char lc[20];
  ViInt16 eventQ;

  if (!thisPtr)
    return s;
  /*      if(thisPtr)doDelay(thisPtr->myDelay);  */
  if (thisPtr->controler & 8192)
    thisPtr->controler &= 0xFFFFDFFF;	/* clear the flag Bit 13 */
  else
    thisPtr->controler &= 0xFFFFE00B;	/* 0 some bits */

  if (thisPtr && thisPtr->errQueryDetect &&
      s != VI_ERROR_TMO /* don't access if already timed out! */ ) {
    errStatus = viWrite(vi, "*ESR?", 5, &rc);
    if (errStatus < VI_SUCCESS)
      return VI_ERROR_SYSTEM_ERROR;

    errStatus = viRead(vi, lc, 20, &rc);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS)
      return VI_ERROR_SYSTEM_ERROR;

    eventQ = atoi(lc);

    if ((0x04			/* Query Error */
	 | 0x08			/* Device Dependent Error */
	 | 0x10			/* Execution Error */
	 | 0x20			/* Command Error */
	) & eventQ)
      return hpe1429_INSTR_ERROR_DETECTED;
  }
  return s;
}

/* 
 * Error Message Structures
 */

struct instrErrStruct {
  ViStatus errStatus;
  ViString errMessage;
};

const static struct instrErrStruct instrErrMsgTable[] = {
  {VI_ERROR_PARAMETER1, VI_ERROR_PARAMETER1_MSG},
  {VI_ERROR_PARAMETER2, VI_ERROR_PARAMETER2_MSG},
  {VI_ERROR_PARAMETER3, VI_ERROR_PARAMETER3_MSG},
  {VI_ERROR_PARAMETER4, VI_ERROR_PARAMETER4_MSG},
  {VI_ERROR_PARAMETER5, VI_ERROR_PARAMETER5_MSG},
  {VI_ERROR_PARAMETER6, VI_ERROR_PARAMETER6_MSG},
  {VI_ERROR_PARAMETER7, VI_ERROR_PARAMETER7_MSG},
  {VI_ERROR_PARAMETER8, VI_ERROR_PARAMETER8_MSG},
  {hpe1429_INSTR_ERROR_PARAMETER9, VI_ERROR_PARAMETER9_MSG},
  {hpe1429_INSTR_ERROR_PARAMETER10, VI_ERROR_PARAMETER10_MSG},
  {hpe1429_INSTR_ERROR_PARAMETER11, VI_ERROR_PARAMETER11_MSG},
  {hpe1429_INSTR_ERROR_PARAMETER12, VI_ERROR_PARAMETER12_MSG},
  {hpe1429_INSTR_ERROR_PARAMETER13, VI_ERROR_PARAMETER13_MSG},
  {hpe1429_INSTR_ERROR_PARAMETER14, VI_ERROR_PARAMETER14_MSG},
  {hpe1429_INSTR_ERROR_PARAMETER15, VI_ERROR_PARAMETER15_MSG},
  {hpe1429_INSTR_ERROR_PARAMETER16, VI_ERROR_PARAMETER16_MSG},
  {hpe1429_INSTR_ERROR_PARAMETER17, VI_ERROR_PARAMETER17_MSG},
  {hpe1429_INSTR_ERROR_PARAMETER18, VI_ERROR_PARAMETER18_MSG},
  {VI_ERROR_FAIL_ID_QUERY, VI_ERROR_FAIL_ID_QUERY_MSG},

  {hpe1429_INSTR_ERROR_INV_SESSION, INSTR_ERROR_INV_SESSION_MSG},
  {hpe1429_INSTR_ERROR_NULL_PTR, INSTR_ERROR_NULL_PTR_MSG},
  {hpe1429_INSTR_ERROR_RESET_FAILED, INSTR_ERROR_RESET_FAILED_MSG},
  {hpe1429_INSTR_ERROR_UNEXPECTED, INSTR_ERROR_UNEXPECTED_MSG},
  {hpe1429_INSTR_ERROR_DETECTED, INSTR_ERROR_DETECTED_MSG},
  {hpe1429_INSTR_ERROR_LOOKUP, INSTR_ERROR_LOOKUP_MSG},
};

/* macros for testing parameters */
#define hpe1429_CHK_BOOLEAN( my_val, err ) if( hpe1429_chk_boolean( thisPtr, my_val) ) hpe1429_LOG_STATUS( vi, thisPtr, err);

static ViBoolean hpe1429_chk_boolean(struct hpe1429_globals *thisPtr, ViBoolean my_val)
{
  char message[hpe1429_ERR_MSG_LENGTH];
  if ((my_val != VI_TRUE) && (my_val != VI_FALSE)) {
    /* true = parameter is invalid */
    sprintf(message, hpe1429_MSG_BOOLEAN, my_val);
    hpe1429_CDE_MESSAGE(message);
    /* true = parameter is invalid */
    return VI_TRUE;
  }

  /* false = okay */
  return VI_FALSE;
}

#define hpe1429_CHK_REAL_RANGE( my_val, min, max, err ) if( hpe1429_chk_real_range( thisPtr, my_val, min, max) ) hpe1429_LOG_STATUS( vi, thisPtr, err);

static ViBoolean hpe1429_chk_real_range(struct hpe1429_globals *thisPtr,
					ViReal64 my_val, ViReal64 min, ViReal64 max)
{
  char message[hpe1429_ERR_MSG_LENGTH];

  if ((my_val < min) || (my_val > max)) {
    sprintf(message, hpe1429_MSG_REAL, min, max, my_val);
    hpe1429_CDE_MESSAGE(message);
    /* true = parameter is invalid */
    return VI_TRUE;
  }
  return VI_FALSE;
}

#define hpe1429_CHK_INT_RANGE( my_val, min, max, err ) if( hpe1429_chk_int_range( thisPtr, my_val, min, max) ) hpe1429_LOG_STATUS( vi, thisPtr, err);

static ViBoolean hpe1429_chk_int_range(struct hpe1429_globals *thisPtr,
				       ViInt16 my_val, ViInt16 min, ViInt16 max)
{
  char message[hpe1429_ERR_MSG_LENGTH];

  if ((my_val < min) || (my_val > max)) {
    sprintf(message, hpe1429_MSG_INT, min, max, my_val);
    hpe1429_CDE_MESSAGE(message);
    /* true = parameter is invalid */
    return VI_TRUE;
  }
  return VI_FALSE;
}

#define hpe1429_CHK_LONG_RANGE( my_val, min, max, err ) if( hpe1429_chk_long_range( thisPtr, my_val, min, max) ) hpe1429_LOG_STATUS( vi, thisPtr, err);

static ViBoolean hpe1429_chk_long_range(struct hpe1429_globals *thisPtr,
					ViInt32 my_val, ViInt32 min, ViInt32 max)
{
  char message[hpe1429_ERR_MSG_LENGTH];

  if ((my_val < min) || (my_val > max)) {
    sprintf(message, hpe1429_MSG_LONG, min, max, my_val);
    hpe1429_CDE_MESSAGE(message);
    /* true = parameter is invalid */
    return VI_TRUE;
  }
  return VI_FALSE;
}

#define hpe1429_CHK_ENUM( my_val, limit, err ) if( hpe1429_chk_enum( thisPtr, my_val, limit) ) hpe1429_LOG_STATUS( vi, thisPtr, err);

/* utility routine which searches for a string in an array of strings. */
/* This is used by the CHK_ENUM macro */
static ViBoolean hpe1429_chk_enum(struct hpe1429_globals *thisPtr, ViInt16 my_val, ViInt16 limit)
{
  char message[hpe1429_ERR_MSG_LENGTH];

  if ((my_val < 0) || (my_val > limit)) {
    sprintf(message, hpe1429_MSG_INT, 0, limit, my_val);
    hpe1429_CDE_MESSAGE(message);
    /* true = parameter is invalid */
    return VI_TRUE;
  }

  return VI_FALSE;
}

/*  ==========================================================================  
     This function searches an array of strings for a specific string and     
     returns its index.  If successful, a VI_SUCCESS is returned, 
     else hpe1429_INSTR_ERROR_LOOKUP is returned.
    ======================================================================== */
ViStatus hpe1429_findIndex(struct hpe1429_globals * thisPtr, const char *const array_of_strings[],
			   /*last entry in array must be 0 */
			   const char *string,	/* string read from instrument */
			   ViPInt16 index)
{				/* result index */
  ViInt16 i;
  ViInt16 my_len;
  char search_str[20];
  char message[80];

  strcpy(search_str, string);

  /* get rid of newline if present in string */
  /* needed because %s includes newline in some VTL's */
  my_len = strlen(search_str);
  if (search_str[my_len - 1] == '\n')
    search_str[my_len - 1] = '\0';

  for (i = 0; array_of_strings[i]; i++) {
    if (!strcmp(array_of_strings[i], search_str)) {
      *index = i;
      return VI_SUCCESS;
    }
  }

  /* if we got here, we did not find it */
  sprintf(message, hpe1429_MSG_NO_MATCH, search_str);
  hpe1429_CDE_MESSAGE(message);
  return hpe1429_INSTR_ERROR_LOOKUP;
}

/* returns the globals pointer */

#define GetGlobals(vi,thisPtr)\
{\
	errStatus = viGetAttribute( vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);\
	if( VI_SUCCESS > errStatus)\
	{\
	hpe1429_LOG_STATUS( vi, NULL, errStatus);\
	}\
}

/****************************************************************************
hpe1429_init
*****************************************************************************
    Parameter Name                                       Type    Direction
   ------------------------------------------------------------------------
  | InstrDesc                                         ViRsrc      IN
  |   ---------------------------------------------------------------------
  |  | The Instrument Description.
  |  |
  |  | Examples: VXI::5, GPIB-VXI::80::INSTR
   ------------------------------------------------------------------------
  | id_query                                          ViBoolean   IN
  |   ---------------------------------------------------------------------
  |  | if( VI_TRUE) Perform In-System Verification.
  |  | if(VI_FALSE) Do not perform In-System Verification
   ------------------------------------------------------------------------
  | do_reset                                          ViBoolean   IN
  |   ---------------------------------------------------------------------
  |  | IF( VI_TRUE) Perform Reset Operation.
  |  | if(VI_FALSE) Do not perform Reset operation
   ------------------------------------------------------------------------
  | vi                                                ViPSession  OUT
  |   ---------------------------------------------------------------------
  |  | Instrument Handle. This is VI_NULL if an error occurred
  |  | during the init.

*****************************************************************************/

ViStatus _VI_FUNC hpe1429_init(ViRsrc InstrDesc, ViBoolean id_query, ViBoolean do_reset,
			       ViPSession vi)
{
  struct hpe1429_globals *thisPtr;
  ViStatus errStatus;
  ViSession defRM;
  ViUInt16 intf;

  ViUInt16 manfId;
  ViUInt16 modelCode;
  long base_addr;

  *vi = VI_NULL;

  /* Find the Default Resource Manager */
  errStatus = viOpenDefaultRM(&defRM);
  if (VI_SUCCESS > errStatus) {
    /* Errors: VI_ERROR_SYSTEM_ERROR 
     *         VI_ERROR_ALLOC
     */
    hpe1429_LOG_STATUS(*vi, NULL, errStatus);
  }

  /* Open a session to the instrument */
  errStatus = viOpen(defRM, InstrDesc, VI_NULL, VI_NULL, vi);
  if (VI_SUCCESS > errStatus) {
    viClose(defRM);
    /* Errors: VI_ERROR_NSUP_OPER
     *         VI_ERROR_INV_RSRC_NAME
     *         VI_ERROR_INV_ACC_MODE
     *         VI_ERROR_RSRC_NFOUND
     *         VI_ERROR_ALLOC
     */
    *vi = VI_NULL;
    hpe1429_LOG_STATUS(*vi, NULL, errStatus);
  }

  /* get memory for instance specific globals */
  thisPtr = (struct hpe1429_globals *)malloc(sizeof(struct hpe1429_globals));
  if (0 == thisPtr) {
    viClose(defRM);		/* also closes vi session */
    *vi = VI_NULL;
    hpe1429_LOG_STATUS(*vi, NULL, VI_ERROR_ALLOC);
  }

  /* associate memory with session, should not fail because
   *   session is valid; and attribute is defined, supported,
   *   and writable.
   */
  errStatus = viSetAttribute(*vi, VI_ATTR_USER_DATA, (ViAttrState) thisPtr);
  if (VI_SUCCESS > errStatus) {
    viClose(*vi);
    viClose(defRM);		/* also closes vi session */
    *vi = VI_NULL;
    hpe1429_LOG_STATUS(*vi, NULL, errStatus);
  }

  /* initialize instance globals */
  thisPtr->defRMSession = defRM;
  thisPtr->errNumber = VI_SUCCESS;
  thisPtr->errFuncName[0] = 0;
  thisPtr->errMessage[0] = 0;
  thisPtr->errQueryDetect = VI_FALSE;
  thisPtr->blockSrqIO = VI_FALSE;
  thisPtr->countSrqIO = 0;
  thisPtr->myDelay = setDelay(100e-6);

  thisPtr->controler = 0;	/* Assume no commander */
  /* Check to see if this e1429 has an e1406 commander */
  if (!viGetAttribute(*vi, VI_ATTR_INTF_TYPE, &intf)) {	/* Find interface type */
    if ((intf == VI_INTF_GPIB_VXI) || (intf == VI_INTF_GPIB)) {	/* may have e1406 */
      ViUInt16 num, primary, secondary;
      ViSession vi1406;
      char idn_buf[256];

      /* Can't use VI_ATTR_INTF_NUM since
         /* HP VTL 3.0 has a defect which returns 7 instead of 0 */
      /* Read the number from the descriptor instead, assuming
         it's GPIB-VXIn.
         /* Set the GPIB board number is the same had the GPIB-VXI 
         board number */
      if (intf == VI_INTF_GPIB_VXI) {
	if (memcmp(InstrDesc, "GPIB-VXI", 8) || InstrDesc[8] < '0' || InstrDesc[8] > '9')
	  num = 0;		/* Problem with InstrDesc */
	else
	  num = (ViUInt16) atoi(InstrDesc + 8);
	sprintf(thisPtr->cmdAddr, "GPIB-VXI%hu::0", num);
	if (viGetAttribute(*vi, VI_ATTR_GPIB_SECONDARY_ADDR, &secondary) < VI_SUCCESS) {
	  viGetAttribute(*vi, VI_ATTR_VXI_LA, &secondary);
	  secondary = secondary / 8;
	}
      } else {
	if (memcmp(InstrDesc, "GPIB", 4) || InstrDesc[4] < '0' || InstrDesc[4] > '9')
	  num = 0;		/* Problem with InstrDesc */
	else
	  num = (ViUInt16) atoi(InstrDesc + 4);
	viGetAttribute(*vi, VI_ATTR_GPIB_PRIMARY_ADDR, &primary);
	viGetAttribute(*vi, VI_ATTR_GPIB_SECONDARY_ADDR, &secondary);
	sprintf(thisPtr->cmdAddr, "GPIB%hu::%hu::0", num, primary);
      }
      errStatus = viOpen(defRM, thisPtr->cmdAddr, VI_NULL, VI_NULL, &vi1406);	/* Open commander */
      if (errStatus >= VI_SUCCESS) {	/* opened commander */
	viSetAttribute(vi1406, VI_ATTR_TMO_VALUE, 1000);
	errStatus = viClear(vi1406);
	if (errStatus < VI_SUCCESS)
	  viClose(vi1406);
	else {
	  errStatus = viPrintf(vi1406, "*IDN?\n");
	  if (errStatus < VI_SUCCESS)
	    viClose(vi1406);
	  else {
	    errStatus = viScanf(vi1406, "%t", idn_buf);
	    if (thisPtr)
	      doDelay(thisPtr->myDelay);
	    if (errStatus < VI_SUCCESS || memcmp(idn_buf, "HEWLETT-PACKARD,E140", 20))	/* not an e140x */
	      viClose(vi1406);
	    else {
	      char rd_addr[80];
	      base_addr = (0x1FC000 + (secondary * 512)) + 6;
	      sprintf(rd_addr, "DIAG:PEEK? %ld, 16\n", base_addr);
	      errStatus = viPrintf(vi1406, rd_addr);
	      if (errStatus < VI_SUCCESS)
		viClose(vi1406);
	      else {
		ViChar length_str[32];
		ViUInt32 retbytes;
		errStatus = viRead(vi1406, length_str, 31, &retbytes);
		if (thisPtr)
		  doDelay(thisPtr->myDelay);
		viClose(vi1406);
		if (errStatus >= VI_SUCCESS) {
		  if (retbytes < 31) {
		    length_str[retbytes] = 0;
		    thisPtr->a24_addr = atol(length_str) * 256;
		    thisPtr->controler = 1;	/* passed all the tests */
		  }
		}
	      }
	    }
	  }
	}
      }
    } else {
      if (intf == VI_INTF_VXI)
	thisPtr->controler = 10;	/* a not as yet mapped embedded controler */
    }
  }

  if (VI_TRUE == do_reset) {
    /* dev clr andcall the reset function to reset the instrument */
    if (viClear(*vi) < VI_SUCCESS || hpe1429_reset(*vi) < VI_SUCCESS) {
      /* ignore any errors in PREFIX_close */
      hpe1429_close(*vi);
      *vi = VI_NULL;
      hpe1429_LOG_STATUS(*vi, NULL, hpe1429_INSTR_ERROR_RESET_FAILED);
    }
    Sleep(100);
  }

  if (VI_TRUE == id_query) {
    ViUInt16 intf;
    char idn_buf[256];

    /* find the interface type */
    if (errStatus = viGetAttribute(*vi, VI_ATTR_INTF_TYPE, &intf)) {
      /* Errors: VI_ERROR_NSUP_ATTR */

      /* ignore any errors in PREFIX_close */
      hpe1429_close(*vi);
      *vi = VI_NULL;

      hpe1429_LOG_STATUS(*vi, NULL, errStatus);
    }

    switch (intf) {
    case VI_INTF_GPIB:
      if (viClear(*vi) < VI_SUCCESS ||
	  viPrintf(*vi, "*IDN?\n") < VI_SUCCESS || viScanf(*vi, "%t", idn_buf) < VI_SUCCESS ||
	  /* check for a idn match */
	  strncmp(idn_buf, hpe1429_IDN_STRING, strlen(hpe1429_IDN_STRING))
	  ) {
	/* ignore any errors in PREFIX_close */
	hpe1429_close(*vi);
	*vi = VI_NULL;
	hpe1429_LOG_STATUS(*vi, NULL, VI_ERROR_FAIL_ID_QUERY);
      }
      if (thisPtr)
	doDelay(thisPtr->myDelay);

      break;

    case VI_INTF_VXI:
    case VI_INTF_GPIB_VXI:

      /* find the VXI manfacturer's ID */
      errStatus = viGetAttribute(*vi, VI_ATTR_MANF_ID, &manfId);
      if (VI_SUCCESS > errStatus) {
	/* Errors: VI_ERROR_NSUP_ATTR */

	/* ignore any errors in PREFIX_close */
	hpe1429_close(*vi);
	*vi = VI_NULL;

	hpe1429_LOG_STATUS(*vi, NULL, errStatus);
      }

      /* find the instrument's model code */
      errStatus = viGetAttribute(*vi, VI_ATTR_MODEL_CODE, (ViPAttrState) (&modelCode));
      if (VI_SUCCESS > errStatus) {
	/* Errors: VI_ERROR_NSUP_ATTR */
	/* Note: this should never happen 
	 *   with a VXI instrument
	 */

	/* ignore any errors in PREFIX_close */
	hpe1429_close(*vi);
	*vi = VI_NULL;
	hpe1429_LOG_STATUS(*vi, NULL, errStatus);
      }

      if ((manfId != hpe1429_MANF_ID) || (modelCode != hpe1429_MODEL_CODE)) {
	/* ignore any errors in PREFIX_close */
	hpe1429_close(*vi);
	*vi = VI_NULL;

	hpe1429_LOG_STATUS(*vi, NULL, VI_ERROR_FAIL_ID_QUERY);
      }
      break;

    default:
      /* ignore any errors in PREFIX_close */
      hpe1429_close(*vi);
      *vi = VI_NULL;
      hpe1429_LOG_STATUS(*vi, NULL, hpe1429_INSTR_ERROR_UNEXPECTED);

    }

  }
  /* if - id_query */
  hpe1429_LOG_STATUS(*vi, thisPtr, VI_SUCCESS);
}

/* _init*/

/****************************************************************************
hpe1429_close
*****************************************************************************
    Parameter Name                                       Type    Direction
   ------------------------------------------------------------------------
  | vi                                                ViSession   IN
  |   ---------------------------------------------------------------------
  |  | Instrument Handle returned from hpe1429_init()

*****************************************************************************/
ViStatus _VI_FUNC hpe1429_close(ViSession vi)
{
  struct hpe1429_globals *thisPtr;
  ViStatus errStatus;
  ViSession defRM;

  GetGlobals(vi, thisPtr);
  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);

  /* retrieve Resource Management session */
  defRM = thisPtr->defRMSession;

  /* free memory */
  if (thisPtr) {
    /* make sure there is something to free */
    free(thisPtr);
  }

  /* close the vi and RM sessions */
  return viClose(defRM);
}

/* _close */

/****************************************************************************
hpe1429_reset
*****************************************************************************
    Parameter Name                                       Type    Direction
   ------------------------------------------------------------------------
  | vi                                                ViSession   IN
  |   ---------------------------------------------------------------------
  |  | Instrument Handle returned from hpe1429_init()

*****************************************************************************/
ViStatus _VI_FUNC hpe1429_reset(ViSession vi)
{
  struct hpe1429_globals *thisPtr;
  ViStatus errStatus;

  GetGlobals(vi, thisPtr)
      hpe1429_DEBUG_CHK_THIS(vi, thisPtr);

  errStatus = viPrintf(vi, "*RST\n");
  if (VI_SUCCESS > errStatus) {

    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/* _reset */

/****************************************************************************
hpe1429_self_test
*****************************************************************************
    Parameter Name                                       Type    Direction
   ------------------------------------------------------------------------
  | vi                                                ViSession   IN
  |   ---------------------------------------------------------------------
  |  | Instrument Handle returned from hpe1429_init()
   ------------------------------------------------------------------------
  | test_result                                       ViPInt16    OUT
  |   ---------------------------------------------------------------------
  |  | Numeric result from self-test operation
  |  |
  |  | 0 = no error ( test passed)
  |  | 1 = failure
   ------------------------------------------------------------------------
  | test_message                                      ViChar _VI_FAR []OUT
  |   ---------------------------------------------------------------------
  |  | Self-test status message.  This is limited to 256 characters.

*****************************************************************************/

ViStatus _VI_FUNC hpe1429_self_test(ViSession vi, ViPInt16 test_result, ViPString test_message)
{
  struct hpe1429_globals *thisPtr;
  ViStatus errStatus;

  /* initialize output parameters */
  *test_result = -1;
  test_message[0] = 0;

  GetGlobals(vi, thisPtr)
      hpe1429_DEBUG_CHK_THIS(vi, thisPtr);

  thisPtr->blockSrqIO = VI_TRUE;

  errStatus = viPrintf(vi, "*TST?\n");
  if (VI_SUCCESS > errStatus) {

    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  errStatus = viScanf(vi, "%hd%*t", test_result);
  if (thisPtr)
    doDelay(thisPtr->myDelay);
  if (VI_SUCCESS > errStatus) {
    *test_result = -1;

    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  switch (*test_result) {
  case 0:
    sprintf(test_message, hpe1429_MSG_SELF_TEST_PASSED);
    break;

    /*??? add instrument specific cases here */

  default:
    sprintf(test_message, hpe1429_MSG_SELF_TEST_FAILED);
    break;
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/* _self_test */

/****************************************************************************
hpe1429_error_query
*****************************************************************************
    Parameter Name                                       Type    Direction
   ------------------------------------------------------------------------
  | vi                                                ViSession   IN
  |   ---------------------------------------------------------------------
  |  | Instrument Handle returned from hpe1429_init()
   ------------------------------------------------------------------------
  | error_number                                      ViPInt32    OUT
  |   ---------------------------------------------------------------------
  |  | Instruments error code
   ------------------------------------------------------------------------
  | error_message                                     ViChar _VI_FAR []OUT
  |   ---------------------------------------------------------------------
  |  | Instrument's error message.  This is limited to 256 characters.

*****************************************************************************/
ViStatus _VI_FUNC hpe1429_error_query(ViSession vi, ViPInt32 error_number, ViPString error_message)
{
  struct hpe1429_globals *thisPtr;
  ViStatus errStatus;

  /* initialize output parameters */
  *error_number = -1;
  error_message[0] = 0;

  GetGlobals(vi, thisPtr)
      hpe1429_DEBUG_CHK_THIS(vi, thisPtr);

  thisPtr->blockSrqIO = VI_TRUE;

  errStatus = viPrintf(vi, "SYST:ERR?\n");
  if (VI_SUCCESS > errStatus) {

    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  /* get the error number */
  errStatus = viScanf(vi, "%ld,%t", error_number, error_message);
  if (thisPtr)
    doDelay(thisPtr->myDelay);
  /* check for error during the scan */
  if (VI_SUCCESS > errStatus) {
    *error_number = -1;
    error_message[0] = 0;
    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  /* get rid of extra LF at the end of the error_message */
  error_message[strlen(error_message) - 1] = 0;

  thisPtr->controler |= 8192;	/* Set Flag to not clear bits */
  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/* _error_query */

/****************************************************************************
hpe1429_error_message
*****************************************************************************
    Parameter Name                                       Type    Direction
   ------------------------------------------------------------------------
  | vi                                                ViSession   IN
  |   ---------------------------------------------------------------------
  |  | Instrument Handle returned from hpe1429_init().
  |  | This may be VI_NULL.
   ------------------------------------------------------------------------
  | error_number                                      ViStatus    IN
  |   ---------------------------------------------------------------------
  |  | The error return value from an instrument driver function
   ------------------------------------------------------------------------
  | message                                           ViChar _VI_FAR []OUT
  |   ---------------------------------------------------------------------
  |  | Error message string.  This is limited to 256 characters.

*****************************************************************************/

ViStatus _VI_FUNC hpe1429_error_message(ViSession vi,
					ViStatus error_number, ViChar _VI_FAR message[])
/* works for either kind of driver */
{
  struct hpe1429_globals *thisPtr;
  ViStatus errStatus;
  ViInt32 idx;

  /* initialize output parameters */
  message[0] = 0;

  thisPtr = NULL;

  /* try to find a thisPtr */
  if (VI_NULL != vi) {
    GetGlobals(vi, thisPtr)
	hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  }

  if (VI_SUCCESS == error_number) {
    sprintf(message, hpe1429_MSG_NO_ERRORS);
    hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
  }

  /* return the static error message */
  for (idx = 0; idx < (sizeof instrErrMsgTable / sizeof(struct instrErrStruct)); idx++) {
    /* check for a matching error number */
    if (instrErrMsgTable[idx].errStatus == error_number) {
      if ((thisPtr) && (thisPtr->errNumber == error_number)) {
	/* context dependent error
	 * message is available.
	 */
	sprintf(message,
		"%s " hpe1429_MSG_IN_FUNCTION " %s() %s",
		instrErrMsgTable[idx].errMessage, thisPtr->errFuncName, thisPtr->errMessage);
      } else {
	/* No context dependent eror 
	 * message available so copy 
	 * the static error message
	 */
	strcpy(message, instrErrMsgTable[idx].errMessage);

      }

      if (thisPtr)
	thisPtr->controler |= 8192;	/* Flag to not clear bits */
      hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
    }
  }

  /* try VTL/VISA to see if it exists there 
   *
   * At this point vi is either VALID or VI_NULL
   */
  errStatus = viStatusDesc(vi, error_number, message);
  if (VI_SUCCESS == errStatus) {
    /* check for a context dependent error message */
    if ((thisPtr) && (thisPtr->errNumber == error_number)) {
      /* context dependent error
       * message is available.
       */
      strcat(message, " ");
      strcat(message, hpe1429_MSG_IN_FUNCTION);
      strcat(message, " ");
      strcat(message, thisPtr->errFuncName);
      strcat(message, "() ");
      strcat(message, thisPtr->errMessage);
    }

    /* VTL found an error message, so return success */
    if (thisPtr)
      thisPtr->controler |= 8192;	/* Flag to not clear bits */
    hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
  }

  /* if we have a VI_NULL, then we need to return a error message */
  if (VI_NULL == vi) {
    strcpy(message, hpe1429_MSG_VI_OPEN_ERR);
    hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
  }

  /* user passed in a invalid status */
  sprintf(message,
	  hpe1429_MSG_INVALID_STATUS "  %ld" hpe1429_MSG_INVALID_STATUS_VALUE, (long)error_number);

  hpe1429_LOG_STATUS(vi, thisPtr, VI_ERROR_PARAMETER2);
}

/****************************************************************************
hpe1429_revision_query
*****************************************************************************
    Parameter Name                                       Type    Direction
   ------------------------------------------------------------------------
  | vi                                                ViSession      IN
  |   ---------------------------------------------------------------------
  |  | Instrument Handle returned from hpe1429_init()
   ------------------------------------------------------------------------
  | driver_rev                                ViChar _VI_FAR []      OUT
  |   ---------------------------------------------------------------------
  |  | Instrument driver revision.  This is limited to 256 characters.
   ------------------------------------------------------------------------
  | instr_rev                                 ViChar _VI_FAR []      OUT
  |   ---------------------------------------------------------------------
  |  | Instrument firmware revision.  This is limited to 256 characters.

*****************************************************************************/

ViStatus _VI_FUNC hpe1429_revision_query(ViSession vi,
					 ViChar _VI_FAR driver_rev[], ViChar _VI_FAR instr_rev[])
{
  struct hpe1429_globals *thisPtr;
  ViStatus errStatus;
  char temp_str[256];		/* temp hold for instr rev string */
  char *last_comma;		/* last comma in *IDN string */

  /* initialize output parameters */
  driver_rev[0] = 0;
  instr_rev[0] = 0;

  GetGlobals(vi, thisPtr)
      hpe1429_DEBUG_CHK_THIS(vi, thisPtr);

  sprintf(driver_rev, "%s", hpe1429_REV_CODE);

  thisPtr->blockSrqIO = VI_TRUE;

  if (errStatus = viPrintf(vi, "*IDN?\n")) {
    driver_rev[0] = 0;

    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  if (errStatus = viScanf(vi, "%s%*t", temp_str)) {
    driver_rev[0] = 0;
    instr_rev[0] = 0;
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }
  if (thisPtr)
    doDelay(thisPtr->myDelay);

  last_comma = strrchr(temp_str, ',');
  /* error and exit if last comma not found */
  if (!last_comma) {
    instr_rev[0] = 0;
    hpe1429_CDE_MESSAGE("no last comma found in IDN string");
    hpe1429_LOG_STATUS(vi, thisPtr, hpe1429_INSTR_ERROR_UNEXPECTED);
  }

  strcpy(instr_rev, last_comma + 1);

  if (thisPtr)
    doDelay(thisPtr->myDelay);
  thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/* _rev_query */

/****************************************************************************
hpe1429_timeOut
*****************************************************************************
    Parameter Name                                       Type    Direction
   ------------------------------------------------------------------------
  | vi                                                ViSession   IN
  |   ---------------------------------------------------------------------
  |  | Instrument Handle returned from hpe1429_init()
   ------------------------------------------------------------------------
  | timeOut                                           ViInt32     IN
  |   ---------------------------------------------------------------------
  |  | This value sets the I/O timeout for all functions in
  |  | the driver. It is specified in milliseconds.

*****************************************************************************/
/* ----------------------------------------------------------------------- */
/* Purpose:  Changes the timeout value of the instrument.  Input is in     */
/*           milliseconds.                                                 */
/* ----------------------------------------------------------------------- */
ViStatus _VI_FUNC hpe1429_timeOut(ViSession vi, ViInt32 timeOut)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  GetGlobals(vi, thisPtr)
      hpe1429_DEBUG_CHK_THIS(vi, thisPtr);

  hpe1429_CHK_LONG_RANGE(timeOut, 1, 2147483647, VI_ERROR_PARAMETER2);

  errStatus = viSetAttribute(vi, VI_ATTR_TMO_VALUE, timeOut);
  if (VI_SUCCESS > errStatus) {
    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/****************************************************************************
hpe1429_timeOut_Q
*****************************************************************************
    Parameter Name                                       Type    Direction
   ------------------------------------------------------------------------
  | vi                                                ViSession   IN
  |   ---------------------------------------------------------------------
  |  | Instrument Handle returned from hpe1429_init()
   ------------------------------------------------------------------------
  | pTimeOut                                          ViPInt32    OUT
  |   ---------------------------------------------------------------------
  |  | This is the minimum timeout period that the driver
  |  | can be set to. It is specified in milliseconds.

*****************************************************************************/
/* ----------------------------------------------------------------------- */
/* Purpose:  Returns the current setting of the timeout value of the       */
/*           instrument in milliseconds.                                   */
/* ----------------------------------------------------------------------- */
ViStatus _VI_FUNC hpe1429_timeOut_Q(ViSession vi, ViPInt32 timeOut)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  GetGlobals(vi, thisPtr)
      hpe1429_DEBUG_CHK_THIS(vi, thisPtr);

  errStatus = viGetAttribute(vi, VI_ATTR_TMO_VALUE, timeOut);
  if (VI_SUCCESS > errStatus) {
    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/****************************************************************************
hpe1429_errorQueryDetect
*****************************************************************************
    Parameter Name                                       Type    Direction
   ------------------------------------------------------------------------
  | vi                                                ViSession   IN
  |   ---------------------------------------------------------------------
  |  | Instrument Handle returned from hpe1429_init()
   ------------------------------------------------------------------------
  | errorQueryDetect                                  ViBoolean   IN
  |   ---------------------------------------------------------------------
  |  | Boolean which enables (VI_TRUE) or disables (VI_FALSE)
  |  | automatic instrument error querying.

*****************************************************************************/
ViStatus _VI_FUNC hpe1429_errorQueryDetect(ViSession vi, ViBoolean errDetect)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  GetGlobals(vi, thisPtr)
      hpe1429_DEBUG_CHK_THIS(vi, thisPtr);

  hpe1429_CHK_BOOLEAN(errDetect, VI_ERROR_PARAMETER2);

  thisPtr->errQueryDetect = errDetect;

  thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/****************************************************************************
hpe1429_errorQueryDetect_Q
*****************************************************************************
    Parameter Name                                       Type    Direction
   ------------------------------------------------------------------------
  | vi                                                ViSession   IN
  |   ---------------------------------------------------------------------
  |  | Instrument Handle returned from hpe1429_init()
   ------------------------------------------------------------------------
  | pErrDetect                                        ViPBoolean  OUT
  |   ---------------------------------------------------------------------
  |  | Boolean indicating if automatic instrument error
  |  | querying is performed.

*****************************************************************************/
ViStatus _VI_FUNC hpe1429_errorQueryDetect_Q(ViSession vi, ViPBoolean pErrDetect)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  GetGlobals(vi, thisPtr)
      hpe1429_DEBUG_CHK_THIS(vi, thisPtr);

  *pErrDetect = thisPtr->errQueryDetect;

  thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/****************************************************************************
hpe1429_dcl
*****************************************************************************
    Parameter Name                                       Type    Direction
   ------------------------------------------------------------------------
  | vi                                                ViSession   IN
  |   ---------------------------------------------------------------------
  |  | Instrument Handle returned from hpe1429_init()

*****************************************************************************/
ViStatus _VI_FUNC hpe1429_dcl(ViSession vi)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  GetGlobals(vi, thisPtr)
      hpe1429_DEBUG_CHK_THIS(vi, thisPtr);

  errStatus = viClear(vi);
  if (VI_SUCCESS > errStatus) {
    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/* Begin status functions */
/****************************************************************************
hpe1429_readStatusByte_Q
*****************************************************************************
    Parameter Name                                       Type    Direction
   ------------------------------------------------------------------------
  | vi                                                ViSession   IN
  |   ---------------------------------------------------------------------
  |  | Instrument Handle returned from hpe1429_init()
   ------------------------------------------------------------------------
  | statusByte                                        ViPInt16    OUT
  |   ---------------------------------------------------------------------
  |  | returns the contents of the status byte

*****************************************************************************/
ViStatus _VI_FUNC hpe1429_readStatusByte_Q(ViSession vi, ViPInt16 statusByte)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;
  ViUInt16 stb;

  GetGlobals(vi, thisPtr);
  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);

  errStatus = viReadSTB(vi, &stb);
  if (thisPtr)
    doDelay(thisPtr->myDelay);
  if (VI_SUCCESS > errStatus) {
    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  *statusByte = (ViInt16) stb;

  thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/* hpe1429_operEvent_Q */
ViStatus _VI_FUNC hpe1429_operEvent_Q(ViSession vi, ViPInt32 val)
{
  ViStatus errStatus;
  struct hpe1429_globals *thisPtr;
  ViInt32 count;
  char buf[20];

  GetGlobals(vi, thisPtr) {
    errStatus = viWrite(vi, "STAT:OPER:EVEN?", 15, &count);
    if (errStatus < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viRead(vi, buf, 20, &count);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    buf[count] = '\0';
    *val = atoi(buf);
  }
  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/* hpe1429_operCond_Q */
ViStatus _VI_FUNC hpe1429_operCond_Q(ViSession vi, ViPInt32 val)
{
  ViStatus errStatus;
  struct hpe1429_globals *thisPtr;
  ViInt32 count;
  char buf[20];

  GetGlobals(vi, thisPtr) {
    errStatus = viWrite(vi, "STAT:OPER:COND?", 15, &count);
    if (errStatus < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viRead(vi, buf, 20, &count);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    buf[count] = '\0';
    *val = atoi(buf);
  }
  thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/* hpe1429_statusQuesEven_Q */
ViStatus _VI_FUNC hpe1429_quesEvent_Q(ViSession vi, ViPInt32 val)
{
  ViStatus errStatus;
  struct hpe1429_globals *thisPtr;
  ViInt32 count;
  char buf[20];

  GetGlobals(vi, thisPtr) {
    errStatus = viWrite(vi, "STAT:QUES:EVEN?", 15, &count);
    if (errStatus < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viRead(vi, buf, 20, &count);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    buf[count] = '\0';
    *val = atoi(buf);
  }
  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/* hpe1429_QuesCond_Q */
ViStatus _VI_FUNC hpe1429_quesCond_Q(ViSession vi, ViPInt32 val)
{
  ViStatus errStatus;
  struct hpe1429_globals *thisPtr;
  ViInt32 count;
  char buf[20];

  GetGlobals(vi, thisPtr) {
    errStatus = viWrite(vi, "STAT:QUES:COND?", 15, &count);
    if (errStatus < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viRead(vi, buf, 20, &count);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    buf[count] = '\0';
    *val = atoi(buf);
  }
  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/* The scpi escape funnctions */

/* 	hpe1429_cmdString_Q
	Scpi escape function that returns a string.
	
	input:
		vi			- session
		p1			- The scpi command to be exectuted
		p2			- The size of p3
	
	output:
		p3			- the string returned by the instrument
		
*/
ViStatus _VI_FUNC hpe1429_cmdString_Q(ViSession vi, ViString p1, ViInt32 p2, ViChar _VI_FAR p3[])
{
  ViStatus errStatus;
  ViInt32 mySize;
  struct hpe1429_globals *thisPtr;

/* hopefully this can be expanded on WIN32 */
  if (p2 < 2 || p2 > 32767)
    hpe1429_LOG_STATUS(vi, thisPtr, VI_ERROR_PARAMETER2);

  GetGlobals(vi, thisPtr);
  /* If (message-based I/O) then write to instrument. */
  {
    if ((errStatus = viPrintf(vi, "%s\n", p1)) < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);

    mySize = p2 - 1;

    {
      int sz;
      sz = (int)p2;
      errStatus = viScanf(vi, "%#t", &sz, p3);
      if (thisPtr)
	doDelay(thisPtr->myDelay);
      if (errStatus < VI_SUCCESS)
	hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
      if (sz >= (int)p2)
	sz = (int)p2 - 1;
      p3[sz] = '\0';
    }
  }

  hpe1429_LOG_STATUS(vi, thisPtr, errStatus);

}

/* 	hpe1429_cmdData_Q
	Scpi escape function that returns a string.
	
	input:
		vi			- session
		p1			- The scpi command to be exectuted
		p2			- length of p3 in bytes
	
	output:
		p3			- the string returned by the instrument
		
		
*/
ViStatus _VI_FUNC hpe1429_cmdData_Q(ViSession vi, ViString p1, ViInt32 p2, ViChar _VI_FAR p3[])
{
  ViStatus errStatus;
  ViInt32 mySize;
  struct hpe1429_globals *thisPtr;

/* hopefully on WIN32 this can be expanded */
  if (p2 < 2 || p2 > 32767)
    hpe1429_LOG_STATUS(vi, thisPtr, VI_ERROR_PARAMETER2);

  GetGlobals(vi, thisPtr);
  /* If (message-based I/O) then write to instrument. */
  {
    if ((errStatus = viPrintf(vi, "%s\n", p1)) < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);

    mySize = p2 - 1;

    {
      int sz;
      sz = (int)p2;
      errStatus = viScanf(vi, "%#t", &sz, p3);
      if (thisPtr)
	doDelay(thisPtr->myDelay);
      if (errStatus < VI_SUCCESS)
	hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }
  }

  hpe1429_LOG_STATUS(vi, thisPtr, errStatus);

}

/*	PREFXI_cmd:
	Send a scpi command, it does not look for a response
	
	input:
		vi			- session
		p1			- Scpi command string

	comment:
		assumes p1 is NULL terminated C string.
*/

ViStatus _VI_FUNC hpe1429_cmd(ViSession vi, ViString p1)
{
  ViInt32 bogus_size = 50;	/* Parameter required for cscpi_exe() */
  ViStatus errStatus;
  struct hpe1429_globals *thisPtr;

  GetGlobals(vi, thisPtr)
      /* If (message-based I/O) then write to instrument. */
  {
    errStatus = viPrintf(vi, "%s\n", p1);
  }

  hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
}

/*	hpe1429_cmdInt32_Q:
	Sends scpi command and waits for a response that must be representable as an int32
	
	input:
		vi			- session
		p1			- NULL terminated C string with scpi command.
	output:
		p2			- Result
		
	comment:
		a non number instrument response would yeild zero in p2.
*/
ViStatus _VI_FUNC hpe1429_cmdInt32_Q(ViSession vi, ViString p1, ViPInt32 p2)
{
  ViStatus errStatus;
  struct hpe1429_globals *thisPtr;

  GetGlobals(vi, thisPtr)
      /* If (message-based I/O) then write to instrument. */
  {
    if ((errStatus = viPrintf(vi, "%s\n", p1)) < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viScanf(vi, "%ld%*t", p2);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);

  }

  thisPtr->controler |= 8192;	/* Set Flag to not clear bits */
  hpe1429_LOG_STATUS(vi, thisPtr, errStatus);

}

/*	hpe1429_cmdReal32_Q
	sends scpi command that must elicit a response that can be represented as a real32
	
	input:
		vi			- session
		p1			- NULL terminated C string containing scpi command
	
	output:
		p2			- Response converted to real 32
		
	comment:
		non numeric response will yeild 0 in p2, in case of underflow underfined.
*/

ViStatus _VI_FUNC hpe1429_cmdReal32_Q(ViSession vi, ViString p1, ViPReal32 p2)
{
  ViStatus errStatus;
  struct hpe1429_globals *thisPtr;

  GetGlobals(vi, thisPtr)
      /* If (message-based I/O) then write to instrument. */
  {
    if ((errStatus = viPrintf(vi, "%s\n", p1)) < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viScanf(vi, "%f%*t", p2);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  hpe1429_LOG_STATUS(vi, thisPtr, errStatus);

}

/*	hpe1429_cmdReal64_Q
	sends scpi command that must elicit a response that can be represented as a real64 (double)
	
	input:
		vi			- session
		p1			- NULL terminated C string containing scpi command
	
	output:
		p2			- Response converted to real 64
		
	comment:
		non numeric response will yeild 0 in p2, in case of underflow underfined.
*/

ViStatus _VI_FUNC hpe1429_cmdReal64_Q(ViSession vi, ViString p1, ViPReal64 p2)
{
  ViStatus errStatus;
  struct hpe1429_globals *thisPtr;

  GetGlobals(vi, thisPtr)
      /* If (message-based I/O) then write to instrument. */
  {
    if ((errStatus = viPrintf(vi, "%s\n", p1)) < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viScanf(vi, "%lf%*t", p2);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  hpe1429_LOG_STATUS(vi, thisPtr, errStatus);

}

/*	hpe1429_cmdInt16_Q:
	Scpi command that must respond with a number that can be interpreted as an int16
	
	input:
		vi			- session
		p1			- NULL terminated C string with command
	
	output:
		p2			- response converted to int 16
		
	comment:
		will return 0 if response is not a valid number
*/

ViStatus _VI_FUNC hpe1429_cmdInt16_Q(ViSession vi, ViString p1, ViPInt16 p2)
{
  ViStatus errStatus;
  struct hpe1429_globals *thisPtr;

  GetGlobals(vi, thisPtr)
      /* If (message-based I/O) then write to instrument. */
  {
    if ((errStatus = viPrintf(vi, "%s\n", p1)) < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viScanf(vi, "%hd%*t", p2);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  hpe1429_LOG_STATUS(vi, thisPtr, errStatus);

}

/* 	hpe1429_cmdReal64Arr_Q
	Scpi command that returns a real64 definate arbitrary block
	
	input:
		vi			- session
		p1			- NULL terminated C string containing scpi command
		p2 			- size of p3
	
	output:
		p3			- reutrns array of real64s
		p4			- # of bytes in p3
	
*/

ViStatus _VI_FUNC hpe1429_cmdReal64Arr_Q(ViSession vi,
					 ViString p1, ViInt32 p2, ViReal64 _VI_FAR p3[],
					 ViPInt32 p4)
/*	This entry point assumes that the return result is a arbitrary block.  Do not use
	this entry point for ascii values.  For that just get the data from _cmd_string_Q
	routine.
*/
{
  ViStatus errStatus;
  ViUInt32 cnt;
  struct hpe1429_globals *thisPtr;
  char lc[100];
  ViInt32 i, ArrSize, nbytes;
  char _huge *Array;

  GetGlobals(vi, thisPtr)

      if (p2 < hpe1429_CMDREAL64ARR_Q_MIN || p2 > hpe1429_CMDREAL64ARR_Q_MAX)
    hpe1429_LOG_STATUS(vi, thisPtr, VI_ERROR_PARAMETER3);

  /* If (message-based I/O) then write to instrument. */
  {
    if ((errStatus = viWrite(vi, p1, strlen(p1), &cnt)) < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);

    if ((errStatus = viRead(vi, lc, 1, &cnt)) < VI_SUCCESS) {
      if (thisPtr)
	doDelay(thisPtr->myDelay);
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    if (lc[0] != '#') {
      if (thisPtr)
	doDelay(thisPtr->myDelay);
      hpe1429_LOG_STATUS(vi, thisPtr, VI_ERROR_INV_EXPR);
    }

    if ((errStatus = viRead(vi, lc, 1, &cnt)) < VI_SUCCESS) {
      if (thisPtr)
	doDelay(thisPtr->myDelay);
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    nbytes = lc[0] - '0';

    if ((errStatus = viRead(vi, lc, nbytes, &cnt)) < VI_SUCCESS) {
      if (thisPtr)
	doDelay(thisPtr->myDelay);
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }
    lc[cnt] = '\0';

    ArrSize = atol(lc);

#ifdef WIN32
    Array = (char *)malloc(ArrSize);
#else
    Array = (char _huge *)halloc(ArrSize, 1);
#endif
    if (!Array)
      hpe1429_LOG_STATUS(vi, thisPtr, VI_ERROR_ALLOC);

    errStatus = viRead(vi, Array, ArrSize, &cnt);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    if (errStatus == VI_SUCCESS_MAX_CNT)
      viClear(vi);
    ArrSize = cnt / sizeof(ViReal64);
    ArrSize = (ArrSize > p2) ? p2 : ArrSize;
  }

  _fmemcpy(p3, Array, (size_t) ArrSize * sizeof(ViReal64));	/*copy only as many as the user allows */

#ifdef WIN32
  free(Array);
#else
  _hfree(Array);
#endif

  *p4 = ArrSize;

  for (i = 0; i < *p4; i++)
    SWAP_FLOAT64(&p3[i]);

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/* 	hpe1429_cmdReal32Arr_Q
	Scpi command that returns a real32 definate arbitrary block
	
	input:
		vi			- session
		p1			- NULL terminated C string containing scpi command
		p2			- size of p3
	
	output:
		p3			- reutrns array of real32s
		p4			- # of items in p3 
	
*/
ViStatus _VI_FUNC hpe1429_cmdReal32Arr_Q(ViSession vi,
					 ViString p1, ViInt32 p2, ViReal32 _VI_FAR p3[],
					 ViPInt32 p4)
/*	This entry point assumes that the return result is a arbitrary block.  Do not use
	this entry point for ascii values.  For that just get the data from _cmd_string_Q
	routine.
*/
{
  ViStatus errStatus;
  ViUInt32 cnt;
  struct hpe1429_globals *thisPtr;
  char lc[100];
  ViInt32 i, ArrSize, nbytes;
  char _huge *Array;

  GetGlobals(vi, thisPtr)

      if (p2 < hpe1429_CMDREAL32ARR_Q_MIN || p2 > hpe1429_CMDREAL32ARR_Q_MAX)
    hpe1429_LOG_STATUS(vi, thisPtr, VI_ERROR_PARAMETER3);

  /* If (message-based I/O) then write to instrument. */

  {
    if ((errStatus = viWrite(vi, p1, strlen(p1), &cnt)) < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);

    if ((errStatus = viRead(vi, lc, 1, &cnt)) < VI_SUCCESS) {
      if (thisPtr)
	doDelay(thisPtr->myDelay);
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    if (lc[0] != '#') {
      if (thisPtr)
	doDelay(thisPtr->myDelay);
      hpe1429_LOG_STATUS(vi, thisPtr, VI_ERROR_INV_EXPR);
    }

    if ((errStatus = viRead(vi, lc, 1, &cnt)) < VI_SUCCESS) {
      if (thisPtr)
	doDelay(thisPtr->myDelay);
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    nbytes = lc[0] - '0';

    if ((errStatus = viRead(vi, lc, nbytes, &cnt)) < VI_SUCCESS) {
      if (thisPtr)
	doDelay(thisPtr->myDelay);
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    lc[cnt] = '\0';
    ArrSize = atol(lc);
#ifdef WIN32
    Array = (char *)malloc(ArrSize);
#else
    Array = (char _huge *)halloc(ArrSize, 1);
#endif
    if (!Array)
      hpe1429_LOG_STATUS(vi, thisPtr, VI_ERROR_ALLOC);

    errStatus = viRead(vi, Array, ArrSize, &cnt);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    if (errStatus == VI_SUCCESS_MAX_CNT)
      viClear(vi);
    ArrSize = cnt / sizeof(ViReal32);
    ArrSize = (ArrSize > p2) ? p2 : ArrSize;
  }

  _fmemcpy(p3, Array, (size_t) ArrSize * sizeof(ViReal32));	/*copy only as many as the user allows */

#ifdef WIN32
  free(Array);
#else
  _hfree(Array);
#endif

  *p4 = ArrSize;

  for (i = 0; i < *p4; i++)
    SWAP_FLOAT32(&p3[i]);

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/* 	hpe1429_cmdInt16Arr_Q
	Scpi command that returns a int16 definate arbitrary block
	
	input:
		vi			- session
		p1			- NULL terminated C string containing scpi command
		p2			- size of p3
	
	output:
		p3			- reutrns array of int16s
		p4			- # of items in p3
	
*/
ViStatus _VI_FUNC hpe1429_cmdInt16Arr_Q(ViSession vi,
					ViString p1, ViInt32 p2, ViInt16 _VI_FAR p3[], ViPInt32 p4)
/*	This entry point assumes that the return result is a arbitrary block.  Do not use
	this entry point for ascii values.  For that just get the data from _cmd_string_Q
	routine.
*/
{
  ViStatus errStatus;
  ViUInt32 cnt;
  struct hpe1429_globals *thisPtr;
  char lc[100];
  ViInt32 i, ArrSize, nbytes;
  char _huge *Array;

  GetGlobals(vi, thisPtr)

      if (p2 < hpe1429_CMDINT16ARR_Q_MIN || p2 > hpe1429_CMDINT16ARR_Q_MAX)
    hpe1429_LOG_STATUS(vi, thisPtr, VI_ERROR_PARAMETER3);

  /* If (message-based I/O) then write to instrument. */
  {
    if ((errStatus = viWrite(vi, p1, strlen(p1), &cnt)) < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);

    if ((errStatus = viRead(vi, lc, 1, &cnt)) < VI_SUCCESS) {
      if (thisPtr)
	doDelay(thisPtr->myDelay);
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    if (lc[0] != '#') {
      if (thisPtr)
	doDelay(thisPtr->myDelay);
      hpe1429_LOG_STATUS(vi, thisPtr, VI_ERROR_INV_EXPR);
    }

    if ((errStatus = viRead(vi, lc, 1, &cnt)) < VI_SUCCESS) {
      if (thisPtr)
	doDelay(thisPtr->myDelay);
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    nbytes = lc[0] - '0';

    if ((errStatus = viRead(vi, lc, nbytes, &cnt)) < VI_SUCCESS) {
      if (thisPtr)
	doDelay(thisPtr->myDelay);
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    lc[cnt] = '\0';
    ArrSize = atol(lc);

#ifdef WIN32
    Array = (char *)malloc(ArrSize);
#else
    Array = (char _huge *)halloc(ArrSize, 1);
#endif
    if (!Array)
      hpe1429_LOG_STATUS(vi, thisPtr, VI_ERROR_ALLOC);

    errStatus = viRead(vi, Array, ArrSize, &cnt);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);

    if (errStatus == VI_SUCCESS_MAX_CNT)
      viClear(vi);
    ArrSize = cnt / sizeof(ViInt16);
    ArrSize = (ArrSize > p2) ? p2 : ArrSize;
  }

  _fmemcpy(p3, Array, (size_t) ArrSize * sizeof(ViInt16));	/*copy only as many as the user allows */

#ifdef WIN32
  free(Array);
#else
  _hfree(Array);
#endif

  *p4 = ArrSize;

  for (i = 0; i < *p4; i++)
    SWAP_16(&p3[i]);

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/* 	hpe1429_cmdInt32Arr_Q
	Scpi command that returns a int16 definate arbitrary block
	
	input:
		vi			- session
		p1			- NULL terminated C string containing scpi command
		p2			- size of p3
	output:
		p3			- reutrns array of int16s
		p4			- # of items in p3
	
*/
ViStatus _VI_FUNC hpe1429_cmdInt32Arr_Q(ViSession vi,
					ViString p1, ViInt32 p2, ViInt32 _VI_FAR p3[], ViPInt32 p4)
/*	This entry point assumes that the return result is a arbitrary block.  Do not use
	this entry point for ascii values.  For that just get the data from _cmd_string_Q
	routine.
*/
{
  ViStatus errStatus;
  ViUInt32 cnt;
  struct hpe1429_globals *thisPtr;
  char lc[100];
  ViInt32 i, ArrSize, nbytes;
  char _huge *Array;

  GetGlobals(vi, thisPtr)

      if (p2 < hpe1429_CMDREAL32ARR_Q_MIN || p2 > hpe1429_CMDREAL32ARR_Q_MAX)
    hpe1429_LOG_STATUS(vi, thisPtr, VI_ERROR_PARAMETER3);

  /* If (message-based I/O) then write to instrument. */
  {
    if ((errStatus = viWrite(vi, p1, strlen(p1), &cnt)) < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);

    if ((errStatus = viRead(vi, lc, 1, &cnt)) < VI_SUCCESS) {
      if (thisPtr)
	doDelay(thisPtr->myDelay);
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    if (lc[0] != '#') {
      if (thisPtr)
	doDelay(thisPtr->myDelay);
      hpe1429_LOG_STATUS(vi, thisPtr, VI_ERROR_INV_EXPR);
    }

    if ((errStatus = viRead(vi, lc, 1, &cnt)) < VI_SUCCESS) {
      if (thisPtr)
	doDelay(thisPtr->myDelay);
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    nbytes = lc[0] - '0';

    if ((errStatus = viRead(vi, lc, nbytes, &cnt)) < VI_SUCCESS) {
      if (thisPtr)
	doDelay(thisPtr->myDelay);
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    lc[cnt] = '\0';
    ArrSize = atol(lc);

#ifdef WIN32
    Array = (char *)malloc(ArrSize);
#else
    Array = (char _huge *)halloc(ArrSize, 1);
#endif
    if (!Array)
      hpe1429_LOG_STATUS(vi, thisPtr, VI_ERROR_ALLOC);

    errStatus = viRead(vi, Array, ArrSize, &cnt);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);

    if (errStatus == VI_SUCCESS_MAX_CNT)
      viClear(vi);
    ArrSize = cnt / sizeof(ViInt32);
    ArrSize = (ArrSize > p2) ? p2 : ArrSize;
  }

  _fmemcpy(p3, Array, (size_t) ArrSize * sizeof(ViInt32));	/*copy only as many as the user allows */

#ifdef WIN32
  free(Array);
#else
  _hfree(Array);
#endif

  *p4 = ArrSize;

  for (i = 0; i < *p4; i++)
    SWAP_32(&p3[i]);

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/* 	hpe1429_cmdInt
	Scpi command that takes ONE int16 or int32 command
	
	input:
		vi			- session
		p1			- NULL terminated C string containing scpi command
		p2			- integer parameter
	
*/
ViStatus _VI_FUNC hpe1429_cmdInt(ViSession vi, ViString p1, ViInt32 p2)
{
  ViStatus errStatus;
  struct hpe1429_globals *thisPtr;

  GetGlobals(vi, thisPtr);
  /* If (message-based I/O) then write to instrument. */
  {
    if ((errStatus = viPrintf(vi, "%s %ld\n", p1, p2)) < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  hpe1429_LOG_STATUS(vi, thisPtr, errStatus);

}

/* 	hpe1429_cmdReal
	Scpi command that takes ONE Real32 or Real64 param
	
	input:
		vi			- session
		p1			- NULL terminated C string containing scpi command
		p2			- Real parameter
	
*/
ViStatus _VI_FUNC hpe1429_cmdReal(ViSession vi, ViString p1, ViReal64 p2)
{

  ViStatus errStatus;
  struct hpe1429_globals *thisPtr;

  GetGlobals(vi, thisPtr);
  {
    if ((errStatus = viPrintf(vi, "%s %g\n", p1, p2)) < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  hpe1429_LOG_STATUS(vi, thisPtr, errStatus);

}

/* 	hpe1429_opc_Q
	Returns VI_TRUE if operations still pending
	
	input:
		vi			- session
	output
		p1			- VI_TRUE if operations pending
	
*/
ViStatus _VI_FUNC hpe1429_opc_Q(ViSession vi, ViPBoolean p1)
{

  ViStatus errStatus;
  struct hpe1429_globals *thisPtr;

  GetGlobals(vi, thisPtr);
  {
    if ((errStatus = viPrintf(vi, "*OPC?\n")) < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    errStatus = viScanf(vi, "%hd%*t", p1);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
  hpe1429_LOG_STATUS(vi, thisPtr, errStatus);

}

ViStatus hpe1429_checkE1406(ViSession vi, struct hpe1429_globals *thisPtr)
{
  ViStatus errStatus;
  ViInt32 fetchCount;
  ViInt16 vmeMode, vmeSource;
  ViInt16 lbusMode, lbusSource;
  ViInt32 preCount, armCount;
  char idn_buf[256];

  if ((thisPtr->controler & 496) == 0) {	/* no lbus/arm or vme bits set yet */
    errStatus = viPrintf(vi, "SWE:OFFS:POIN?\n");
    if (errStatus < VI_SUCCESS)
      return errStatus;
    errStatus = viScanf(vi, "%ld%*t", &preCount);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS)
      return errStatus;

    errStatus = viPrintf(vi, "ARM:COUN?\n");
    if (errStatus < VI_SUCCESS)
      return errStatus;
    errStatus = viScanf(vi, "%ld%*t", &armCount);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS)
      return errStatus;

    if ((armCount == 1) || (preCount == 0)) {
      if ((errStatus = viPrintf(vi, "*IDN?\n")) < VI_SUCCESS)
	return errStatus;
      if ((errStatus = viScanf(vi, "%t", idn_buf)) < VI_SUCCESS)
	return errStatus;
      if (thisPtr)
	doDelay(thisPtr->myDelay);
      if (strncmp(idn_buf, "HEWLETT-PACKARD,E1429B", 22))
	lbusMode = hpe1429_LBUS_OFF;
      else {
	thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
	errStatus = hpe1429_confLocalBus_Q(vi, &lbusMode, &lbusSource);
	if (errStatus < VI_SUCCESS)
	  return errStatus;
      }

      if (lbusMode == hpe1429_LBUS_OFF) {
	thisPtr->controler |= 16;
	thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
	errStatus = hpe1429_confVME_Q(vi, &vmeMode, &vmeSource);
	if (errStatus < VI_SUCCESS)
	  return errStatus;
	thisPtr->controler |= (ViInt16) (vmeSource * 32);
	if (vmeMode == hpe1429_VME_GEN)
	  thisPtr->controler |= 256;
      }
    }
  }
  if (thisPtr->controler & 16) {
    thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
    errStatus = hpe1429_cmdInt32_Q(vi, "FETC:COUN?", &fetchCount);
    if (errStatus < VI_SUCCESS)
      return errStatus;
    thisPtr->trigs_per_arm = fetchCount;
  }
  thisPtr->controler |= 4096;	/* set the checked bit */
  return VI_SUCCESS;
}

ViStatus hpe1429_fetcE1406(ViSession vi, ViInt16 chan, ViInt32 arrayLength, ViUInt32 * nbytes,
			   struct hpe1429_globals * thisPtr,
#ifdef WIN32
			   ViPInt16 fptr
#else
			   ViInt16 _huge * fptr
#endif
    )
{
  ViStatus errStatus;
  ViStatus errStatus2;
  ViChar c[2];
  ViUInt32 digits;
  ViChar length_str[16];
  ViUInt32 retbytes;
  ViInt16 gotit;
  ViSession vi1406;
  ViInt32 fetchCount;
  ViInt16 vmeMode, vmeSource;
  ViInt32 timeOut;
  ViBoolean opc;

  struct _timeb start_time, now_time;

  gotit = 0;
  errStatus = viGetAttribute(vi, VI_ATTR_TMO_VALUE, &timeOut);
  if (errStatus < VI_SUCCESS)
    return errStatus;

  if (!(thisPtr->controler & 4096)) {
    errStatus = hpe1429_checkE1406(vi, thisPtr);
    if (errStatus < VI_SUCCESS)
      return errStatus;
  }

  if (!(thisPtr->controler & 16))
    return (VI_SUCCESS - 1);
  fetchCount = thisPtr->trigs_per_arm;
  if (fetchCount > arrayLength)
    return VI_ERROR_PARAMETER3;

  errStatus = viOpen(thisPtr->defRMSession, thisPtr->cmdAddr, VI_NULL, VI_NULL, &vi1406);
  if (errStatus < VI_SUCCESS)
    return errStatus;		/* didn't open commander */

  viSetAttribute(vi1406, VI_ATTR_TMO_VALUE, timeOut);
  errStatus = viClear(vi1406);
  if (errStatus < VI_SUCCESS) {
    viClose(vi1406);
    return errStatus;
  }

  if (arrayLength > timeOut * 10) {
    errStatus = viSetAttribute(vi1406, VI_ATTR_TMO_VALUE, arrayLength / 10);
    if (errStatus < VI_SUCCESS) {
      viClose(vi1406);
      return errStatus;
    }
  }

  if (thisPtr->controler & 256)
    vmeMode = hpe1429_VME_GEN;
  else
    vmeMode = hpe1429_VME_OFF;

  vmeSource = (thisPtr->controler & 224) / 32;

  switch (chan) {
  case hpe1429_CHAN1:
    if ((vmeMode == hpe1429_VME_OFF) || (vmeSource != hpe1429_VME_MEM_CHAN1)) {
      thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
      errStatus = hpe1429_confVME(vi, hpe1429_VME_GEN, hpe1429_VME_MEM_CHAN1);
      if (errStatus < VI_SUCCESS) {
	thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
	hpe1429_confVME(vi, vmeMode, vmeSource);
      } else
	gotit = 1;
    }
    break;
  case hpe1429_CHAN2:
    if ((vmeMode == hpe1429_VME_OFF) || (vmeSource != hpe1429_VME_MEM_CHAN2)) {
      thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
      errStatus = hpe1429_confVME(vi, hpe1429_VME_GEN, hpe1429_VME_MEM_CHAN2);
      if (errStatus < VI_SUCCESS) {
	thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
	hpe1429_confVME(vi, vmeMode, vmeSource);
      } else
	gotit = 1;
    }
    break;
  default:
    errStatus = VI_SUCCESS - 1;
    break;
  }
  if (errStatus < VI_SUCCESS) {
    viClose(vi1406);
    return errStatus;
  }

  if (gotit) {			/* had to change VME--need to re-init it */
    ViInt16 bigdelay;
    if (thisPtr && !(thisPtr->errQueryDetect)) {
      thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
      errStatus = hpe1429_opc_Q(vi, &opc);	/* let 1429 catch up */
      if (errStatus < VI_SUCCESS) {
	viClose(vi1406);
	return errStatus;
      }
    }

    errStatus = viPrintf(vi, "VINS:VME:MEM:INIT\n");

    if (thisPtr) {		/* Give the 1429 about a millisecond to init */
      for (bigdelay = 0; bigdelay < 10; bigdelay++)
	doDelay(thisPtr->myDelay);
    }

  }

  if (errStatus >= VI_SUCCESS)
    errStatus =
	viPrintf(vi1406, "DIAG:UPL:SADD? %ld, %ld\n", thisPtr->a24_addr + 0x0C, 2 * fetchCount);
  if (errStatus < VI_SUCCESS) {
    viClose(vi1406);
    return errStatus;
  }

  _ftime(&start_time);
  errStatus = viRead(vi1406, c, 2, &retbytes);	/* Read Header #[1-9] */
  if (errStatus < VI_SUCCESS) {
    viClose(vi1406);
    return errStatus;
  }

  if (retbytes != 2) {
    viScanf(vi1406, "%*t");	/* Clear input. */
    viClose(vi1406);
    return VI_ERROR_INV_RESPONSE;
  }

  digits = c[1] - '0';
  if ((digits < 1) || (9 < digits)) {
    viScanf(vi1406, "%*t");	/* Clear input. */
    viClose(vi1406);
    return VI_ERROR_INV_RESPONSE;
  }

  /* Scan DAB array count. */
  errStatus = viRead(vi1406, length_str, digits, &retbytes);
  if (errStatus < VI_SUCCESS) {
    viClose(vi1406);
    return errStatus;
  }
  if (retbytes != digits) {
    viScanf(vi1406, "%*t");	/* Clear input. */
    viClose(vi1406);
    return VI_ERROR_INV_RESPONSE;
  }

  length_str[digits] = '\0';	/* null terminate the string */
  *nbytes = atol(length_str);

  /* Verify that caller's array is big enough. */
  if (((ViUInt32) arrayLength * 2) < *nbytes) {
    viScanf(vi1406, "%*t");	/* Clear input. */
    viClose(vi1406);
    return VI_ERROR_PARAMETER3;	/* Caller's array too small. */
  }

  errStatus2 = viRead(vi1406, (unsigned char *)fptr, *nbytes, &retbytes);
  if (errStatus2 == VI_SUCCESS_MAX_CNT)
    errStatus2 = viScanf(vi1406, "%*t");	/* Clear input. */
  _ftime(&now_time);
  if (thisPtr)
    doDelay(thisPtr->myDelay);
  thisPtr->precount = 1000 * (now_time.time - start_time.time);
  thisPtr->precount += ((int32) now_time.millitm - (int32) start_time.millitm);
  gotit += 2;

  viClose(vi1406);

  errStatus = VI_SUCCESS;
  if (gotit & 1) {		/* VME has been changed--change it back */
    errStatus = viPrintf(vi, "ABOR\n");	/* Just in case transfer not complete */
    if (errStatus >= VI_SUCCESS) {
      thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
      errStatus = hpe1429_confVME(vi, vmeMode, vmeSource);
    }
  }
  if (errStatus2 < VI_SUCCESS)
    return errStatus2;
  if (errStatus < VI_SUCCESS)
    return errStatus;

  return VI_SUCCESS;
}

ViStatus hpe1429_fetcSCPI(ViSession vi, ViInt16 chan, ViInt32 arrayLength, ViUInt32 * nbytes,
			  struct hpe1429_globals * thisPtr,
#ifdef WIN32
			  ViPInt16 fptr
#else
			  ViInt16 _huge * fptr
#endif
    )
{
  ViStatus errStatus;
  ViChar c[2];
  ViUInt32 digits;
  ViChar length_str[16];
  ViUInt32 retbytes;

  errStatus = viPrintf(vi, "FORM PACK;:FETC%hd?\n", chan);
  if (errStatus < VI_SUCCESS)
    return errStatus;

  errStatus = viRead(vi, c, 2, &retbytes);	/* Read Header (should be #[1-9] */
  if (errStatus < VI_SUCCESS)
    return errStatus;
  if (retbytes != 2) {
    errStatus = viScanf(vi, "%*t");	/* Clear input. */
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    return VI_ERROR_INV_RESPONSE;
  }

  digits = c[1] - '0';
  if ((digits < 1) || (9 < digits)) {
    errStatus = viScanf(vi, "%*t");	/* Clear input. */
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    return VI_ERROR_INV_RESPONSE;
  }

  /* Scan DAB array count. */
  errStatus = viRead(vi, length_str, digits, &retbytes);
  if (errStatus < VI_SUCCESS)
    return errStatus;
  if (retbytes != digits) {
    errStatus = viScanf(vi, "%*t");	/* Clear input. */
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    return VI_ERROR_INV_RESPONSE;
  }

  length_str[digits] = '\0';	/* null terminate the string */
  *nbytes = atol(length_str);

  /* Verify that caller's array is big enough. */
  if (((ViUInt32) arrayLength * 2) < *nbytes) {
    errStatus = viScanf(vi, "%*t");	/* Clear input. */
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    return VI_ERROR_PARAMETER3;	/* Caller's array too small. */
  }

  errStatus = viRead(vi, (unsigned char *)fptr, *nbytes, &retbytes);
  if (errStatus == VI_SUCCESS_MAX_CNT)
    errStatus = viScanf(vi, "%*t");	/* Clear input. */

  if (thisPtr)
    doDelay(thisPtr->myDelay);

  if (errStatus < VI_SUCCESS)
    return errStatus;
  return VI_SUCCESS;
}

ViStatus hpe1429_fetcFast(ViSession vi, ViInt16 chan, ViInt32 arrayLength, ViUInt32 * nbytes,
			  struct hpe1429_globals * thisPtr,
#ifdef WIN32
			  ViPInt16 fptr)
#else
			  ViInt16 _huge * fptr)
#endif
{
  int32 i;
  ViStatus errStatus;
  int32 arm_count = 1;		/* number of arms MUST BE 1 for this routine */
  ViAddr baseAddr;
  char *base_addr;

  int32 trig_count = (int32) thisPtr->trigs_per_arm;

  *nbytes = 0;
  if (arrayLength < trig_count)
    return VI_ERROR_PARAMETER3;	/* Caller's array too small. */
  if (trig_count % 4)
    return VI_SUCCESS - 1;

  errStatus = viMapAddress(vi, VI_A24_SPACE, 0x00, 0xFF, VI_FALSE, VI_NULL, &baseAddr);
  if (errStatus < VI_SUCCESS)
    return errStatus;
  base_addr = (char *)baseAddr;
  thisPtr->a24_addr = (long)baseAddr;

  if (!(thisPtr->controler & 16384)) {	/* SCPI INIT doesn't need this */
/*
 *     This section sets up the traffic register and variables so that
 *     memory is configured to be read.
 *
 *     Note that with pretrigger readings, pre-arm data may have wrapped around
 *     the circular buffer and overwritten older pre-arm data.  The data that
 *     is supposed to be there after the reading completes is the post-arm
 *     data and the most recent pre-arm data.  The location of the most recent
 *     readings are derived from the final address pointer and the wrapped
 *     indicator.
 *
 *     NOTE:  THIS ROUTINE IS GOOD ONLY FOR A SINGLE SEGMENT OF PRE-POST
 *     DATA - i.e. the arm:coun MUST be 1.  */

    int32 start_addr;
    uint8 temp_reg, temp_base0, temp_base1;
    uint32 b_addr;
    uint8 junk, junk2, junk3;
    int32 last_addr;
    int32 seg_start_addr;
    uint16 upper_word, lower_word;
    int16 was_wrapped = 0;

    start_addr = seg_start_addr = 524288 - trig_count;
    last_addr = 0;
    upper_word = (uint16) peek1429b(base_addr + mem_addr0);
    was_wrapped = upper_word & 0x0080;
    upper_word = (upper_word & 0x0007) << 8;	/* pick off A18 thru A16 */
    upper_word += (uint16) (peek1429b(base_addr + mem_addr1));	/* get A15 to A08 */
    lower_word = (uint16) peek1429b(base_addr + mem_addr2);	/* get A07 to A00 */
    last_addr = (last_addr + upper_word) << 8;

    /* get lower byte and subtract 1 because counter points past last filled */
    last_addr += ((int32) lower_word - 1);
    if (last_addr < seg_start_addr)
      last_addr = 524287;

    /* check to see if we wrapped around, and if we did, get most recent   */
    /* readings.                                                           */
    if (was_wrapped > 0) {
      start_addr = last_addr - trig_count + 1;
      if (start_addr < seg_start_addr)	/* we are in middle of buffer */
	start_addr += trig_count;
    }

    start_addr = start_addr >> 2;	/* A1 and A0 not needed because modulo 4 */

/* Next we have to enter the address we want to start counting from -- this
   is the start_addr.  */
    temp_reg = peek1429b(base_addr + mem_control);
    temp_reg &= ~(ADDR_COUNTER | MEM_WRITE | MEM_READ);	/* clears these fields */
    poke1429b(base_addr + mem_control, temp_reg);

    temp_base0 = (uint8) ((start_addr & 0x0ff00) >> 8);	/* get BA17 to BA10 */
    poke1429b(base_addr + base_addr0, temp_base0);
    temp_base1 = (uint8) ((start_addr & 0x000ff));	/* get BA09 to BA02 */
    poke1429b(base_addr + base_addr1, temp_base1);

/*  set addr_counter high so can load in start addr, and set mode to READ   */
    temp_reg |= (ADDR_COUNTER | MEM_READ);
    poke1429b(base_addr + mem_control, temp_reg);

/* set up traffic register for clock source pulse reg and data src memory  */
/* set the channel source to be channel 1, this will be over written later */
/* when clock source changed again, but channel 1 is necessary if we are   */
/* alternating with DATA_BOTH, so send it now to save time later.          */

    poke1429b(base_addr + traffic, (DATA_SRC_MEM | CLK_SRC_PULSE | DATA_CHAN1));

/* give one clock to load starting address, then input the base address */
    junk2 = peek1429b(base_addr + pulse_reg);

/* now input the base address of the segment, this is what we wrap around
   back to when we go past 524287.
*/
    b_addr = (uint32) (524288 - trig_count);
    b_addr = b_addr >> 2;	/* throw away lower two bits */

    temp_base0 = (uint8) ((b_addr & 0x0ff00) >> 8);	/* get BA17 to BA10 */
    poke1429b(base_addr + base_addr0, temp_base0);
    temp_base1 = (uint8) ((b_addr & 0x000ff));	/* get BA09 to BA02 */
    poke1429b(base_addr + base_addr1, temp_base1);

/* generate 3 pulses to load the address pipeline              */

    junk = peek1429b(base_addr + pulse_reg);
    junk3 = peek1429b(base_addr + pulse_reg);
    junk2 = peek1429b(base_addr + pulse_reg);
  }

/* Change the clock source to be the data register, and set up the final data */
/* source.								      */
  switch (chan) {
  case hpe1429_CHAN1:
    poke1429b(base_addr + traffic, DATA_SRC_MEM | CLK_SRC_REG | DATA_CHAN1);
    break;
  case hpe1429_CHAN2:
    poke1429b(base_addr + traffic, DATA_SRC_MEM | CLK_SRC_REG | DATA_CHAN2);
    break;
  default:
    errStatus = VI_SUCCESS - 1;
    break;
  }

  /* Now read the data */
#ifdef WIN32
  if (thisPtr->controler & 65536) {	/* User has turned off block xfer */
#else
  /* Windows 3.1 does not always have block move -- use single xfers */
#endif
  for (i = 0; i < trig_count; i++)
    fptr[i] = peek1429w(base_addr + reading);
  viUnmapAddress(vi);
#ifdef WIN32
}

else
{
  /* WIN32 has block move */
  viUnmapAddress(vi);
  errStatus = viSetAttribute(vi, VI_ATTR_SRC_INCREMENT, 0);	/* Set fifo mode */
  if (errStatus < VI_SUCCESS)
    return errStatus;
  errStatus = viMoveIn16(vi, VI_A24_SPACE, (ViUInt32) (reading), trig_count, fptr);
  if (errStatus < VI_SUCCESS)
    return errStatus;
}
#endif

*nbytes = 2 * (ViUInt32) trig_count;
return VI_SUCCESS;
}

ViStatus hpe1429_fastInit(char *base_addr, struct hpe1429_globals * thisPtr)
{
  uint8 div_by = (uint8) ((thisPtr->controler >> 5) & 7);
  uint8 the_power = (uint8) ((thisPtr->controler >> 8) & 15);
  int32 trigCount = thisPtr->trigs_per_arm;

  uint8 temp_reg;
  int32 start_addr, segment_size, posprecount;
  ViUInt32 counter = 0;

  poke1429b(base_addr + arm_count_msb, 0);
  poke1429b(base_addr + arm_count_lsb, 1);	/* we only support 1 arm */

  if (trigCount % 4)
    segment_size = trigCount + (4 - (trigCount % 4));
  else
    segment_size = trigCount;

  start_addr = (524288 - segment_size) >> 2;

  temp_reg = peek1429b(base_addr + mem_control);
  temp_reg &= ~(ADDR_COUNTER | MEM_WRITE | MEM_READ);
  poke1429b(base_addr + mem_control, temp_reg);

/*  set addr_counter high so can load in start addr, and set mode to R or W */
  temp_reg |= (ADDR_COUNTER | MEM_WRITE);
  poke1429b(base_addr + mem_control, temp_reg);

/* set traffic register to allow A/D converter to pace things */
  poke1429b(base_addr + traffic, DATA_SRC_ADC | CLK_SRC_ADC | DATA_CHAN2);
  poke1429b(base_addr + traffic, DATA_SRC_ADC | CLK_SRC_ADC | DATA_BOTH);

  {				/* begin chip-init routine -- DON'T CHANGE THE SEQUENCE */
    int32 posttrigs;

    poke1429b(base_addr + tb_abort, 0);	/* stop triggering */
    poke1429b(base_addr + tb_reset, 1);	/* write forces chip init */
    poke1429b(base_addr + interp_ctrl, 1);
    poke1429b(base_addr + stop_data, 5);
    poke1429b(base_addr + interp_cal, 0);
    poke1429b(base_addr + chip_test, 0);
    poke1429b(base_addr + mem_burp_lsb, 0);	/* Mem burp init */
    poke1429b(base_addr + mem_burp_msb, 0);

    posprecount = thisPtr->precount;
    if (posprecount < 0)
      posprecount = -posprecount;
    if (posprecount > 0) {
      poke1429b(base_addr + trig_pre_lsb, (uint8) (posprecount - 2));
      poke1429b(base_addr + trig_pre_msb, (uint8) ((posprecount - 2) >> 8));
    } else {
      poke1429b(base_addr + trig_pre_lsb, 1);
      poke1429b(base_addr + trig_pre_msb, 0);
    }

    posttrigs = trigCount - posprecount;
    if (posprecount > 0)
      posttrigs -= 6;		/* load N-6 if pre-arm readings */
    else
      posttrigs -= 3;		/* load N-3 if all readings post arm */

    poke1429b(base_addr + trig_post_lsb, (uint8) posttrigs);
    poke1429b(base_addr + trig_post_mid, (uint8) (posttrigs >> 8));
    poke1429b(base_addr + trig_post_msb, (uint8) (posttrigs >> 16));
  }				/* end chip-init routine */

  {				/* begin set sample time */
    uint8 refbase, ref_power, temp_reg11;
    uint8 temp_trig_ctrl;

    temp_reg11 = peek1429b(base_addr + tbase_reg11);
    temp_trig_ctrl = (peek1429b(base_addr + trig_control) & (~TRIG_SRC_MASK));

    poke1429b(base_addr + refdiv_amount, 129);	/* turn clk+ output off */
    poke1429b(base_addr + refdiv_power, 255);

    if (the_power == 8) {
      ref_power = 0;
      refbase = 129;
    } else {
      refbase = 128;
      ref_power = (uint8) (1 << the_power);
    }

    refbase += (div_by * 2);	/* set div_by bits appropriately */

    if (the_power < 5)
      temp_reg11 &= ~(TB_RECLK10);	/* disable RECLOCK/10 */
    else
      temp_reg11 |= TB_RECLK10;	/* enable RECLOCK/10 */

    if (the_power > 0 || div_by > 1)
      temp_trig_ctrl |= T_REFDIV;
    else
      temp_trig_ctrl &= ~T_REFDIV;

    poke1429b(base_addr + trig_control, temp_trig_ctrl);
    poke1429b(base_addr + tbase_reg11, temp_reg11);
    poke1429b(base_addr + refdiv_power, ref_power);
    poke1429b(base_addr + refdiv_amount, refbase);

  }				/* end set sample time */

  poke1429b(base_addr + tb_load_acount, 0);
  poke1429b(base_addr + tb_load_acount, 0);
  poke1429b(base_addr + tb_load_acount, 0);

  poke1429b(base_addr + tb_init, 0);	/* send initiate pulse */
  do {
    temp_reg = peek1429b(base_addr + tb_stat);
    ++counter;			/* this keeps the compiler from being "too efficient" */
  } while (temp_reg & 0x2);	/* loop until bit 1-timebase status low */

  {
    uint8 temp_reg, junk;

    /* set up pulse register and flush last reading into mem */
    temp_reg = peek1429b(base_addr + traffic) & CLK_SRC_CLR;
    poke1429b(base_addr + traffic, temp_reg | CLK_SRC_PULSE);
    junk = peek1429b(base_addr + pulse_reg);
  }
  return VI_SUCCESS;
}

void hpe1429_checkEmbedded(ViSession vi, struct hpe1429_globals *thisPtr)
{
  /* Fast init/access will be enabled if and only if:
   * ARM:COUN  == 1
   ****  TRIG:SOUR == TIMER    -- Removed in Revision A.02.02 ***
   ****  ROSC:SOUR == INTERNAL -- Removed in Revision A.02.02 ***
   * SENS:SWE:POIN is a multiple of 4
   * LBUS:MODE == OFF
   * and a successful viMapAddress can be done         */
  /* In addition, fast init will not be done if the total sample time
     is greater than 1 second or there are pre-trigger readings 
     or arm source is not immediate
     or TRIG:SOUR != TIMER    -- Added in Revision A.02.02 ***
     or ROSC:SOUR != INTERNAL -- Added in Revision A.02.02 ***
   */
  ViStatus errStatus;
  ViInt32 armCount;
  ViInt16 trigSour;
  ViReal64 trigTimer1;
  ViInt16 lbusMode, lbusSource;
  char answer[32];
  char idn_buf[256];
  thisPtr->controler &= 0xFFFFFFEB;	/* disable fast init and access Bit 4, 2 */

  thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits Bit 13 */
  errStatus = hpe1429_cmdInt32_Q(vi, "ARM:COUN?", &armCount);
  if (errStatus >= VI_SUCCESS) {
    if (armCount != 1)
      errStatus = VI_SUCCESS - 1;	/* don't support non arm==1 */
  }

  if (errStatus >= VI_SUCCESS) {
    if ((errStatus = viPrintf(vi, "*IDN?\n")) >= VI_SUCCESS) {
      if ((errStatus = viScanf(vi, "%t", idn_buf)) >= VI_SUCCESS) {
	if (thisPtr)
	  doDelay(thisPtr->myDelay);
	if (strncmp(idn_buf, "HEWLETT-PACKARD,E1429B", 22))
	  lbusMode = hpe1429_LBUS_OFF;
	else {
	  if (thisPtr)
	    doDelay(thisPtr->myDelay);
	  thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
	  errStatus = hpe1429_confLocalBus_Q(vi, &lbusMode, &lbusSource);
	  if (errStatus >= VI_SUCCESS) {
	    if (lbusMode != hpe1429_LBUS_OFF)
	      errStatus = (VI_SUCCESS - 1);
	  }
	}
      }
    }
  }

  if (errStatus >= VI_SUCCESS) {
    ViInt32 trigs;
    thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
    errStatus = hpe1429_cmdInt32_Q(vi, "SENS:SWE:POIN?", &trigs);
    thisPtr->trigs_per_arm = trigs;
    if (trigs % 4)
      errStatus = VI_SUCCESS - 1;
  }

  if (errStatus >= VI_SUCCESS) {
    ViInt32 offset;
    ViInt16 goOn;

    goOn = 1;
    thisPtr->controler |= 16;	/* enable fast fetch */
    thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
    errStatus = hpe1429_cmdInt32_Q(vi, "SENS:SWE:OFFS:POIN?", &offset);
    if (errStatus < VI_SUCCESS)
      goOn = 0;
    thisPtr->precount = offset;

    /* The fast init routine can be up to 3 places off in the
       starting location if there are precounts.  0x8000 indicates
       that the user has specifically requested fast init not be
       done.  If trigs*timer is too big or Arm source is not Immediate
       or the trig source is not timer
       the init routine may have to wait too long--that is, SCPI INIT
       is an "immediate release" of the driver to move on (for abort
       or manual ARM), but a "fast init" must wait in the
       driver for measurement complete.  Let's not hang here. */

    thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
    errStatus = hpe1429_trigger_Q(vi, &trigSour, &trigTimer1);
    if (errStatus < VI_SUCCESS)
      goOn = 0;
    if (trigSour != hpe1429_TRIG_TIM)
      goOn = 0;
    if (offset != 0)
      goOn = 0;

    if (goOn) {
      thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
      errStatus = hpe1429_cmdString_Q(vi, "ROSC:SOUR?", 31, answer);
      if (errStatus >= VI_SUCCESS) {
	if ((answer[0] != 'I') && ((answer[0] != '"') || (answer[1] != 'I')))
	  goOn = 0;		/* don't support non INT rosc sour */
      } else
	goOn = 0;
    }

    if (goOn &&
	(!(thisPtr->controler & 0x00008000)) && ((thisPtr->trigs_per_arm * trigTimer1) < 1.0)) {
      thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
      errStatus = hpe1429_cmdString_Q(vi, "ARM:SOUR1?", 31, answer);
      if ((errStatus >= VI_SUCCESS) && (answer[2] == 'M')) {	/* IMM arm sour */
	ViInt32 ratio = (ViInt32) ((trigTimer1 / 0.00000005) + .2);
	ViInt16 the_power = 0;
	while (ratio > 9) {
	  ratio /= 10;
	  the_power++;
	}
	if (the_power > 8)
	  errStatus = VI_SUCCESS - 1;	/* something bad happened */
	else {
	  switch ((ViInt16) ratio) {
	  case 1:
	  case 2:
	  case 4:
	    thisPtr->controler &= 0xFFFFF01F;	/* clear sample bits */
	    thisPtr->controler |= (((ViInt16) ratio) << 5);
	    thisPtr->controler |= (the_power << 8);
	    break;
	  default:
	    errStatus = VI_SUCCESS - 1;	/* something bad happened */
	    break;
	  }
	}
	if (errStatus >= VI_SUCCESS)
	  thisPtr->controler |= 4;	/* enable fast init */
      }
    }
  }

  if (thisPtr->controler & 8)	// a never before mapped embedded controler
  {
    ViAddr base_addr;
    if (VI_SUCCESS <= viMapAddress(vi, VI_A24_SPACE, 0x00, 0xFF, VI_FALSE, VI_NULL, &base_addr)) {
      ViUInt16 kind;
      thisPtr->a24_addr = (long)base_addr;
      if (VI_SUCCESS <= viGetAttribute(vi, VI_ATTR_WIN_ACCESS, &kind)) {
	switch (kind) {
	case VI_USE_OPERS:
	  thisPtr->controler |= 1;
	  break;		/* VX_LINK */
	case VI_DEREF_ADDR:
	  break;		/* can use direct peek/poke */
	default:
	  thisPtr->controler = 0;
	  break;		/* access problem */
	}
	thisPtr->controler &= 0xFFFFFFF7;	/* 0 the not-mapped bit Bit 3 */
      } else
	thisPtr->controler = 0;	/* access problem */
      viUnmapAddress(vi);
    } else
      thisPtr->controler = 0;	/* access problem */
  }
  if (thisPtr->controler)
    thisPtr->controler |= 4096;	/* set "checked" bit */
}

ViStatus _VI_FUNC hpe1429_1406time(ViSession vi, ViPReal64 time)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;
  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }
  if ((thisPtr->controler & 4115) == 4113)
    *time = (thisPtr->precount) / 1000.0;
  else
    *time = -1.00;
  return VI_SUCCESS;
}

ViStatus hpe1429_fetcLINK(ViSession vi, ViInt16 chan, ViInt32 arrayLength, ViUInt32 * nbytes,
			  struct hpe1429_globals * thisPtr,
#ifdef WIN32
			  ViPInt16 fptr)
#else
			  ViInt16 _huge * fptr)
#endif
{
  ViInt16 data;
  int32 i;
  ViStatus errStatus;
  int32 arm_count = 1;		/* number of arms MUST BE 1 for this routine */
  ViAddr baseAddr;
  char *base_addr;

  int32 trig_count = (int32) thisPtr->trigs_per_arm;

  *nbytes = 0;
  if (arrayLength < trig_count)
    return VI_ERROR_PARAMETER3;	/* Caller's array too small. */
  if (trig_count % 4)
    return VI_SUCCESS - 1;

  errStatus = viMapAddress(vi, VI_A24_SPACE, 0x00, 0xFF, VI_FALSE, VI_NULL, &baseAddr);
  if (errStatus < VI_SUCCESS)
    return errStatus;
  base_addr = (char *)baseAddr;
  thisPtr->a24_addr = (long)baseAddr;

  if (!(thisPtr->controler & 16384)) {	/* SCPI INIT doesn't need this */
/*
 *     This section sets up the traffic register and variables so that
 *     memory is configured to be read.
 *
 *     Note that with pretrigger readings, pre-arm data may have wrapped around
 *     the circular buffer and overwritten older pre-arm data.  The data that
 *     is supposed to be there after the reading completes is the post-arm
 *     data and the most recent pre-arm data.  The location of the most recent
 *     readings are derived from the final address pointer and the wrapped
 *     indicator.
 *
 *     NOTE:  THIS ROUTINE IS GOOD ONLY FOR A SINGLE SEGMENT OF PRE-POST
 *     DATA - i.e. the arm:coun MUST be 1.  */

    int32 start_addr;
    uint8 temp_reg, temp_base0, temp_base1;
    uint32 b_addr;
    uint8 junk, junk2, junk3;
    int32 last_addr;
    int32 seg_start_addr;
    uint16 upper_word, lower_word;
    int16 was_wrapped = 0;
    ViUInt8 abyte;

    start_addr = seg_start_addr = 524288 - trig_count;
    last_addr = 0;
    viPeek8(vi, (ViAddr) (base_addr + mem_addr0), &abyte);
    upper_word = (uint16) abyte;
    was_wrapped = upper_word & 0x0080;
    upper_word = (upper_word & 0x0007) << 8;	/* pick off A18 thru A16 */
    viPeek8(vi, (ViAddr) (base_addr + mem_addr1), &abyte);
    upper_word += (uint16) abyte;	/* get A15 to A08 */
    viPeek8(vi, (ViAddr) (base_addr + mem_addr2), &abyte);
    lower_word = (uint16) abyte;	/* get A07 to A00 */
    last_addr = (last_addr + upper_word) << 8;

    /* get lower byte and subtract 1 because counter points past last filled */
    last_addr += ((int32) lower_word - 1);
    if (last_addr < seg_start_addr)
      last_addr = 524287;

    /* check to see if we wrapped around, and if we did, get most recent   */
    /* readings.                                                           */
    if (was_wrapped > 0) {
      start_addr = last_addr - trig_count + 1;
      if (start_addr < seg_start_addr)	/* we are in middle of buffer */
	start_addr += trig_count;
    }

    start_addr = start_addr >> 2;	/* A1 and A0 not needed because modulo 4 */

/* Next we have to enter the address we want to start counting from -- this
   is the start_addr.  */
    viPeek8(vi, (ViAddr) (base_addr + mem_control), &temp_reg);
    temp_reg &= ~(ADDR_COUNTER | MEM_WRITE | MEM_READ);	/* clears these fields */
    linkbpoke(base_addr + mem_control, temp_reg);

    temp_base0 = (uint8) ((start_addr & 0x0ff00) >> 8);	/* get BA17 to BA10 */
    linkbpoke(base_addr + base_addr0, temp_base0);
    temp_base1 = (uint8) ((start_addr & 0x000ff));	/* get BA09 to BA02 */
    linkbpoke(base_addr + base_addr1, temp_base1);

/*  set addr_counter high so can load in start addr, and set mode to READ   */
    temp_reg |= (ADDR_COUNTER | MEM_READ);
    linkbpoke(base_addr + mem_control, temp_reg);

/* set up traffic register for clock source pulse reg and data src memory  */
/* set the channel source to be channel 1, this will be over written later */
/* when clock source changed again, but channel 1 is necessary if we are   */
/* alternating with DATA_BOTH, so send it now to save time later.          */

    linkbpoke(base_addr + traffic, (DATA_SRC_MEM | CLK_SRC_PULSE | DATA_CHAN1));

/* give one clock to load starting address, then input the base address */
    viPeek8(vi, (ViAddr) (base_addr + pulse_reg), &abyte);

/* now input the base address of the segment, this is what we wrap around
   back to when we go past 524287.
*/
    b_addr = (uint32) (524288 - trig_count);
    b_addr = b_addr >> 2;	/* throw away lower two bits */

    temp_base0 = (uint8) ((b_addr & 0x0ff00) >> 8);	/* get BA17 to BA10 */
    linkbpoke(base_addr + base_addr0, temp_base0);
    temp_base1 = (uint8) ((b_addr & 0x000ff));	/* get BA09 to BA02 */
    linkbpoke(base_addr + base_addr1, temp_base1);

/* generate 3 pulses to load the address pipeline              */

    viPeek8(vi, (ViAddr) (base_addr + pulse_reg), &junk);
    viPeek8(vi, (ViAddr) (base_addr + pulse_reg), &junk2);
    viPeek8(vi, (ViAddr) (base_addr + pulse_reg), &junk3);
  }

/* Change the clock source to be the data register, and set up the final data */
/* source.								      */
  switch (chan) {
  case hpe1429_CHAN1:
    linkbpoke(base_addr + traffic, DATA_SRC_MEM | CLK_SRC_REG | DATA_CHAN1);
    break;
  case hpe1429_CHAN2:
    linkbpoke(base_addr + traffic, DATA_SRC_MEM | CLK_SRC_REG | DATA_CHAN2);
    break;
  default:
    return (VI_SUCCESS - 1);
    break;
  }

  /* Now read the data */
#ifdef WIN32
  if (thisPtr->controler & 65536) {	/* User has disabled block xfer */
#else
  /* Windows 3.1 does not always have block move -- use single xfers */
#endif
  for (i = 0; i < trig_count; i++) {
    viPeek16(vi, (ViAddr) (base_addr + reading), &data);
    fptr[i] = data;
  }
  viUnmapAddress(vi);
#ifdef WIN32
}

else
{
  /* WIN32 has block move */
  viUnmapAddress(vi);
  errStatus = viSetAttribute(vi, VI_ATTR_SRC_INCREMENT, 0);	/* Set fifo mode */
  if (errStatus < VI_SUCCESS)
    return errStatus;
  errStatus = viMoveIn16(vi, VI_A24_SPACE, (ViUInt32) (reading), trig_count, fptr);
  if (errStatus < VI_SUCCESS)
    return errStatus;
}
#endif

*nbytes = 2 * (ViUInt32) trig_count;
return VI_SUCCESS;
}

ViStatus hpe1429_LINKInit(ViSession vi, char *base_addr, struct hpe1429_globals * thisPtr)
{
  uint8 div_by = (uint8) ((thisPtr->controler >> 5) & 7);
  uint8 the_power = (uint8) ((thisPtr->controler >> 8) & 15);
  int32 trigCount = thisPtr->trigs_per_arm;

  uint8 temp_reg;
  int32 start_addr, segment_size, posprecount;
  ViUInt32 counter = 0;

  linkbpoke(base_addr + arm_count_msb, 0);
  linkbpoke(base_addr + arm_count_lsb, 1);	/* we only support 1 arm */

  if (trigCount % 4)
    segment_size = trigCount + (4 - (trigCount % 4));
  else
    segment_size = trigCount;

  start_addr = (524288 - segment_size) >> 2;

  viPeek8(vi, (ViAddr) (base_addr + mem_control), &temp_reg);
  temp_reg &= ~(ADDR_COUNTER | MEM_WRITE | MEM_READ);
  linkbpoke(base_addr + mem_control, temp_reg);

/*  set addr_counter high so can load in start addr, and set mode to R or W */
  temp_reg |= (ADDR_COUNTER | MEM_WRITE);
  linkbpoke(base_addr + mem_control, temp_reg);

/* set traffic register to allow A/D converter to pace things */
  linkbpoke(base_addr + traffic, DATA_SRC_ADC | CLK_SRC_ADC | DATA_CHAN2);
  linkbpoke(base_addr + traffic, DATA_SRC_ADC | CLK_SRC_ADC | DATA_BOTH);

  {				/* begin chip-init routine -- DON'T CHANGE THE SEQUENCE */
    int32 posttrigs;

    linkbpoke(base_addr + tb_abort, 0);	/* stop triggering */
    linkbpoke(base_addr + tb_reset, 1);	/* write forces chip init */
    linkbpoke(base_addr + interp_ctrl, 1);
    linkbpoke(base_addr + stop_data, 5);
    linkbpoke(base_addr + interp_cal, 0);
    linkbpoke(base_addr + chip_test, 0);
    linkbpoke(base_addr + mem_burp_lsb, 0);	/* Mem burp init */
    linkbpoke(base_addr + mem_burp_msb, 0);

    posprecount = thisPtr->precount;
    if (posprecount < 0)
      posprecount = -posprecount;
    if (posprecount > 0) {
      linkbpoke(base_addr + trig_pre_lsb, (uint8) (posprecount - 2));
      linkbpoke(base_addr + trig_pre_msb, (uint8) ((posprecount - 2) >> 8));
    } else {
      linkbpoke(base_addr + trig_pre_lsb, 1);
      linkbpoke(base_addr + trig_pre_msb, 0);
    }

    posttrigs = trigCount - posprecount;
    if (posprecount > 0)
      posttrigs -= 6;		/* load N-6 if pre-arm readings */
    else
      posttrigs -= 3;		/* load N-3 if all readings post arm */

    linkbpoke(base_addr + trig_post_lsb, (uint8) posttrigs);
    linkbpoke(base_addr + trig_post_mid, (uint8) (posttrigs >> 8));
    linkbpoke(base_addr + trig_post_msb, (uint8) (posttrigs >> 16));
  }				/* end chip-init routine */

  {				/* begin set sample time */
    uint8 refbase, ref_power, temp_reg11;
    uint8 temp_trig_ctrl;

    viPeek8(vi, (ViAddr) (base_addr + tbase_reg11), &temp_reg11);
    viPeek8(vi, (ViAddr) (base_addr + trig_control), &temp_trig_ctrl);
    temp_trig_ctrl &= (~TRIG_SRC_MASK);

    linkbpoke(base_addr + refdiv_amount, 129);	/* turn clk+ output off */
    linkbpoke(base_addr + refdiv_power, 255);

    if (the_power == 8) {
      ref_power = 0;
      refbase = 129;
    } else {
      refbase = 128;
      ref_power = (uint8) (1 << the_power);
    }

    refbase += (div_by * 2);	/* set div_by bits appropriately */

    if (the_power < 5)
      temp_reg11 &= ~(TB_RECLK10);	/* disable RECLOCK/10 */
    else
      temp_reg11 |= TB_RECLK10;	/* enable RECLOCK/10 */

    if (the_power > 0 || div_by > 1)
      temp_trig_ctrl |= T_REFDIV;
    else
      temp_trig_ctrl &= ~T_REFDIV;

    linkbpoke(base_addr + trig_control, temp_trig_ctrl);
    linkbpoke(base_addr + tbase_reg11, temp_reg11);
    linkbpoke(base_addr + refdiv_power, ref_power);
    linkbpoke(base_addr + refdiv_amount, refbase);

  }				/* end set sample time */

  linkbpoke(base_addr + tb_load_acount, 0);
  linkbpoke(base_addr + tb_load_acount, 0);
  linkbpoke(base_addr + tb_load_acount, 0);

  linkbpoke(base_addr + tb_init, 0);	/* send initiate pulse */
  do {
    viPeek8(vi, (ViAddr) (base_addr + tb_stat), &temp_reg);
    ++counter;			/* this keeps the compiler from being "too efficient" */
  } while (temp_reg & 0x2);	/* loop until bit 1-timebase status low */

  {
    uint8 temp_reg, junk;

    /* set up pulse register and flush last reading into mem */
    viPeek8(vi, (ViAddr) (base_addr + traffic), &temp_reg);
    temp_reg &= CLK_SRC_CLR;
    linkbpoke(base_addr + traffic, temp_reg | CLK_SRC_PULSE);
    viPeek8(vi, (ViAddr) (base_addr + pulse_reg), &junk);
  }
  return VI_SUCCESS;
}

ViStatus _VI_FUNC hpe1429_fetcCheck_Q(ViSession vi,
				      ViInt16 chan,
				      ViPInt32 checkTest,
				      ViPInt32 numHits,
				      ViInt32 arrayLength,
				      ViInt16 _VI_FAR data[], ViInt16 errData[])
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;
  *checkTest = -1;
  *numHits = 0;
  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_fetcCheck_Q");

  if (((chan & 3) < 1) || ((chan & 3) > 2)) {
    *checkTest = -2;
    hpe1429_LOG_STATUS(vi, 0, VI_ERROR_PARAMETER2);
  }
  *checkTest = -4;
  hpe1429_CHK_LONG_RANGE(arrayLength, hpe1429_FETC_SIZE_MIN, hpe1429_FETC_SIZE_MAX,
			 VI_ERROR_PARAMETER4);

  *checkTest = 0;
  if (!(chan & 4)) {
    if (arrayLength <= 500)
      *checkTest = -5;
    if ((thisPtr->controler & 3) == 0)
      *checkTest = -6;
  }
  {
    ViInt32 controlerWas;
    ViInt32 precountWas;
    ViInt16 j;
    ViInt32 i;
#ifdef WIN32
    ViInt16 *Wfy2;
#else
    ViInt16 __huge *Wfy2;
#endif
    ViInt16 MAXerrData = 16;
#ifdef WIN32
    ViPInt16 fptr = data;
#else
    ViInt16 _huge *fptr;
    fptr = (ViInt16 _huge *) data;
#endif

    if (*checkTest == 0) {
#ifdef WIN32
      Wfy2 = (ViInt16 *) malloc((long)arrayLength * sizeof(ViInt16));
#else
      Wfy2 = (ViInt16 __huge *) _halloc((long)arrayLength, sizeof(ViInt16));
#endif
      if (Wfy2 == NULL)
	*checkTest = -7;
      else {
	if (chan & 8) {
	  controlerWas = thisPtr->controler;
	  precountWas = thisPtr->precount;
	  thisPtr->controler = 0;
	}
	if (chan & 4) {		/* Send a test pattern into the e1429 */
	  char cmd[32];
	  sprintf(cmd, "DIAG:MEM%hd:FILL 1, %ld", chan & 3, arrayLength);
	  {
	    errStatus = viPrintf(vi, "%s\n", cmd);
	  }
	  thisPtr->controler &= 0xFFFFBFFF;	/* clear theSCPI init bit */
	}

	for (i = 0; i < arrayLength; i++)
	  Wfy2[i] = 32766;

	for (j = 0; j < MAXerrData; j++)
	  errData[(int)j] = -32767;

	errStatus = hpe1429_fetc_Q(vi, (ViInt16) (chan & 3), arrayLength, Wfy2);
	if (errStatus < VI_SUCCESS) {
#ifdef WIN32
	  free(Wfy2);
#else
	  _hfree(Wfy2);
#endif
	  *checkTest = -8;
	  hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
	}
      }
    }
    if (chan & 8)
      thisPtr->controler = controlerWas;
    else {
      controlerWas = thisPtr->controler;
      thisPtr->controler = 0;
      precountWas = thisPtr->precount;
    }
    for (i = 0; i < arrayLength; i++)
      fptr[i] = 32765;

    errStatus = hpe1429_fetc_Q(vi, (ViInt16) (chan & 3), arrayLength, fptr);
    thisPtr->controler = controlerWas;
    thisPtr->precount = precountWas;
    if (*checkTest == 0) {
      ViInt32 nextoff = 0;
      if (chan & 4) {
	ViInt16 shouldBe;
	for (i = 0; i < arrayLength; i++) {
	  shouldBe = ((ViInt16) ((i + 1) % 2000)) << 4;
	  if (fptr[i] != shouldBe)
	    *checkTest = i - 5999999;

	  if (Wfy2[i] != shouldBe)
	    *checkTest = i - 6999999;

	  if (*checkTest) {
	    if ((ViInt32) MAXerrData > (arrayLength - i))
	      MAXerrData = (ViInt16) (arrayLength - i);
	    for (j = 0; j < MAXerrData; j++)
	      errData[j] = Wfy2[i + (ViInt32) j];
	    i = arrayLength;
	  }
	}
      } else {
	for (i = 0; i < arrayLength; i++) {
	  if (fptr[i] != Wfy2[i]) {
	    (*numHits)++;
	    if (*checkTest) {
	      if (!nextoff)
		nextoff = i;
	    } else {
	      if ((ViInt32) MAXerrData > (arrayLength - i))
		MAXerrData = (ViInt16) (arrayLength - i);
	      for (j = 0; j < MAXerrData; j++)
		errData[j] = Wfy2[i + (ViInt32) j];

	      *checkTest = (i + 1);
	    }
	  }
	}
	if (*checkTest) {	/* something errored -- check more details */
	  if (nextoff == *checkTest) {	/* The next point is bad too *//* Try an off-by-one check   */
	    ViInt32 moreHits = 1;
	    nextoff = 999999;
	    for (i = *checkTest; i < arrayLength; i++) {
	      if (fptr[i - 1] != Wfy2[i]) {
		moreHits++;
		if (!nextoff)
		  nextoff = i;
	      }
	    }
	    if (moreHits < *numHits)
	      *numHits = moreHits;	/* better this way */
	    if ((nextoff - 10) <= *checkTest) {	/* Not off that way, try the other */
	      moreHits = 1;
	      nextoff = 999999;
	      for (i = *checkTest; i < arrayLength; i++) {
		if (fptr[i] != Wfy2[i - 1]) {
		  moreHits++;
		  if (!nextoff)
		    nextoff = i;
		}
	      }
	      if (moreHits < *numHits)
		*numHits = moreHits;
	      if ((nextoff - 10) > *checkTest) {	/* it was off by 1 */
		if (nextoff == 999999)	/*purely off by 1 */
		  *checkTest -= 2000000;
		else		/* some more points bad */
		  *checkTest -= 3000000;
	      }
	    } else {		/* it was off by 1 */

	      if (nextoff == 999999)	/*purely off by 1 */
		*checkTest -= 4000000;
	      else		/* some more points bad */
		*checkTest -= 5000000;
	    }
	  }
	}
      }
#ifdef WIN32
      free(Wfy2);
#else
      _hfree(Wfy2);
#endif
    }
  }

  thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_A24
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function returns the a24 address of the HP E1429.
 *            This may be used for debugging purposes.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViPInt32 a24_addr
 * OUT       
 *            Returns the A24 address of the HP E1429.
 * 
 * RETURN  :  VI_SUCCESS: No error.
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1429_A24(ViSession vi, ViPInt32 a24_addr)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_A24");

  {
    *a24_addr = thisPtr->a24_addr;
    thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_HighSpeedStatus
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function is used to check the current status of the
 *            high-speed data access.  The bits may be interpreted as:
 *             BIT#   (Value)           Condition
 *            ------ --------- ----------------------------------
 *               0   (    1 )  HP E1406 Controler or non-
 *                             dereferenced embedded controler
 *                             (such as VXLINK).
 *               1   (    2 )  Embedded controler.
 *               2   (    4 )  Using fast init for embedded.
 *               3   (    8 )  Embedded controler not mapped yet.
 *               4   (   16 )  Using fast fetch for embedded or
 *                             VINS:LBUS OFF and (ARM:COUN==1
 *                             or SWE:OFFS:POIN==0) for hpe1406.
 *               5-            Internally use.
 *              11
 *              12   ( 4096 )  Checked set-up.  (Sending commands
 *                             to the instrument through this
 *                             driver clears the bit.  Sending
 *                             hpe1429_initImm() sets the bit.)
 *              13-            Internal use.
 *              14
 *              15             Indicates high speed init has been
 *                             permanently disabled.
 *              16             Indicates that block transfer has
 *                             been disabled.
 *              17-31          Not used.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViPInt32 status
 * OUT       
 *            Returns the status of the high speed data access.
 * 
 * RETURN  :  VI_SUCCESS: No error.
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1429_HighSpeedStatus(ViSession vi, ViPInt32 status)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_HighSpeedStatus");

  {
    *status = thisPtr->controler;
    thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_abor
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  The hpe1429_abor() function removes the HP E1429 from the
 *           wait-for-trigger state and places it in the idle state. 
 *           Measurement is halted and can only be restarted by another
 *           hpe1429_init() function.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1429_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1429_abor(ViSession vi)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_abor");

  errStatus = viPrintf(vi, "ABOR\n");
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_armStarDel
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function specifies how long to delay entering the
 *           wait-for-trigger state after the arming pulse is received and
 *           processed.  Delays of greater than 0 can be specified only if no
 *           pre-arm readings are being taken, that is, preTriggers=0 in
 *           hpe1429_configure(). The delay can be:
 *                0 to 65534*T in steps of T
 *                65540*T to 655350*T in steps of 10*T
 *           
 *           where T is the current reference oscillator period.
 *            NOTE: Additional delays are present in the instrument.  Please
 *            refer to Appendix A in the manuals for additional information
 *            on the inherent delay.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViReal64 armStarDel
 * IN        
 *            Specifies how long to delay entering the wait-for-trigger state
 *            after the arming pulse is received and processed.
 * 
 *      MAX = hpe1429_ARM_STAR_DEL_MAX   65.535
 *      MIN = hpe1429_ARM_STAR_DEL_MIN   0.0
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1429_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1429_armStarDel(ViSession vi, ViPReal64 armStarDel)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_armStarDel");

  hpe1429_CHK_REAL_RANGE(*armStarDel, hpe1429_ARM_STAR_DEL_MIN, hpe1429_ARM_STAR_DEL_MAX,
			 VI_ERROR_PARAMETER2);

  errStatus = viPrintf(vi, "ARM:STAR:DEL %g\n", *armStarDel);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_armStarDel_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function returns the value of the delay that the
 *           instrument uses before entering the wait-for-trigger state after
 *           the arming pulse is received and processed.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViPReal64 armStarDel
 * OUT       
 *            Returns how long the instrument will delay entering the
 *            wait-for-trigger state after the arming pulse is received and
 *           processed.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1429_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1429_armStarDel_Q(ViSession vi, ViPReal64 armStarDel)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_armStarDel_Q");

  thisPtr->blockSrqIO = VI_TRUE;
  errStatus = viPrintf(vi, "ARM:STAR:DEL?\n");
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  errStatus = viScanf(vi, "%lg%*t", armStarDel);
  if (thisPtr)
    doDelay(thisPtr->myDelay);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_armStarEvent
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function combines two instrument SCPI commands:
 *              ARM[:STARt]:SOURce<n> source;
 *              ARM[:STARt]:SLOPe<n> edge;
 *            Together, these two commands allow you to configure the arm
 *           event subsystem to respond to a specified source and edge.
 *           
 *           The available sources are:
 *              Bus: The Group Execute Trigger (GET) HP-IB command.
 *           
 *              ECLTrg0 and ECLTrg1: The VXIbus ECL trigger lines.
 *           
 *              TTLTrg0 through TTLTrg7: The VXIbus TTL trigger lines.
 *           
 *              External 1: The HP E1429's front panel "Ext 1" BNC connector.
 *           
 *              Internal 1: The HP E1429's channel 1 signal.
 *           
 *              Internal 2: The HP E1429's channel 2 signal.
 *           
 *              Hold: Disable this arming event.  If both event are set
 *                    to Hold, the hpe1429_armStarImm() function must be
 *                    sent before measurements will proceed.
 *           
 *              Immediate: Arming will occur as soon as hpe1429_initImm()
 *                         is sent.  This choice is valid only for event 1,
 *                         and requires event 2 be set to HOLD.
 *           
 *           
 *            This slope selects which edge - Positive, Negative, or Either
 *           on the arming source  will cause the arm event to occur.  The
 *           slope is only active when the arming source is set to External
 *           1, Internal 1, or Internal 2.  The "Either" setting may be used
 *           only when the arming source is set to Internal 1 or Internal 2. 
 *           The "Either" setting causes the window specified
 *           hpe1429_armStarLevels() for the Internal channel to be in
 *           effect.  When the slope is "Either", having the positive value
 *           of hpe1429_armStarLevels() for the corresponding channel greater
 *           than the negative value will cause arming to occur when the
 *           signal exits the defined window.  If negative value is greater
 *           than the positive value, then an arm event will occur when the
 *           signal enters into the defined window.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 event
 * IN        
 *            Specifies the event number (1 or 2) being specified.  Note
 *            that the event number is NOT the same as the channel number.
 *            There just happens to be two of each.  The numbers associated
 *            with sources Internal 1 and Internal 2 are the channel numbers.
 * 
 *      MAX = hpe1429_ARM_EVENT_MAX   2
 *      MIN = hpe1429_ARM_EVENT_MIN   1
 * 
 * PARAM 3 : ViInt16 source
 * IN        
 *            This parameter configures the arm system to respond to the
 *            specified source.
 *           
 *            The types are defined as macros and are shown below:
 * 
 * PARAM 4 : ViInt16 slope
 * IN        
 *            This parameter selects which edge - Positive, Negative, or
 *           Either on the arming source  will cause the arm event to occur. 
 *           The slope is only active when the arming source is set to
 *           External 1, Internal 1, or Internal 2.  The "Either" setting may
 *           be used only when the arming source is set to Internal 1 or
 *           Internal 2.  The "Either" setting causes the window specified
 *           hpe1429_armStarLevels() for the Internal channel to be in
 *           effect.
 *            The types are defined as macros and are shown below:
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1429_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1429_armStarEvent_source_a[] = { "BUS", "HOLD",
  "ECLT0", "ECLT1", "TTLT0", "TTLT1", "TTLT2", "TTLT3", "TTLT4", "TTLT5",
  "TTLT6", "TTLT7", "EXT", "INT1", "INT2", "IMM", 0
};

static const char *const hpe1429_armStarEvent_slope_a[] = { "POS", "NEG",
  "EITH", 0
};

ViStatus _VI_FUNC hpe1429_armStarEvent(ViSession vi, ViInt16 event, ViInt16 source, ViInt16 slope)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_armStarEvent");

  hpe1429_CHK_INT_RANGE(event, hpe1429_ARM_EVENT_MIN, hpe1429_ARM_EVENT_MAX, VI_ERROR_PARAMETER2);

  hpe1429_CHK_ENUM(source, 15, VI_ERROR_PARAMETER3);
  hpe1429_CHK_ENUM(slope, 2, VI_ERROR_PARAMETER4);
  {
    errStatus = viPrintf(vi, "ARM:SOUR%hd %s\n", event, hpe1429_armStarEvent_source_a[source]);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = viPrintf(vi, "ARM:SLOP%hd %s\n", event, hpe1429_armStarEvent_slope_a[slope]);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
  }
  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_armStarEvent_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This routine queries the arm source and slope in a single call.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 event
 * IN        
 *            Specifies the event number (1 or 2) being specified.  Note
 *            that the event number is NOT the same as the channel number.
 *            There just happens to be two of each.  The numbers associated
 *            with sources Internal 1 and Internal 2 are the channel numbers.
 * 
 *      MAX = hpe1429_ARM_EVENT_MAX   2
 *      MIN = hpe1429_ARM_EVENT_MIN   1
 * 
 * PARAM 3 : ViPInt16 source
 * OUT       
 *            This returns an integer value indicating the source
 *            to which the arm system will respond.
 *           
 *            The types are defined as macros and are shown below:
 * 
 * PARAM 4 : ViPInt16 slope
 * OUT       
 *            This returns an integer value indicating the slope
 *            to which the arm system will respond.
 *           
 *            The types are defined as macros and are shown below:
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1429_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1429_armStarEvent_Q_source_a[] = { "BUS",
  "HOLD", "ECLT0", "ECLT1", "TTLT0", "TTLT1", "TTLT2", "TTLT3", "TTLT4",
  "TTLT5", "TTLT6", "TTLT7", "EXT", "INT1", "INT2", "IMM", 0
};

static const char *const hpe1429_armStarEvent_Q_slope_a[] = { "POS", "NEG",
  "EITH", 0
};

ViStatus _VI_FUNC hpe1429_armStarEvent_Q(ViSession vi,
					 ViInt16 event, ViPInt16 source, ViPInt16 slope)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;
  char source_str[32];
  char slope_str[32];

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_armStarEvent_Q");

  hpe1429_CHK_INT_RANGE(event, hpe1429_ARM_EVENT_MIN, hpe1429_ARM_EVENT_MAX, VI_ERROR_PARAMETER2);

  {
    thisPtr->blockSrqIO = VI_TRUE;
    errStatus = viPrintf(vi, "ARM:SOUR%hd?\n", event);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = viScanf(vi, "%s%*t", source_str);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = hpe1429_findIndex(thisPtr, hpe1429_armStarEvent_Q_source_a, source_str, source);
    if (errStatus < VI_SUCCESS) {	/* could be INT or INTERNAL2 */
      int quoted;
      quoted = 0;
      if (source_str[0] == '"')
	quoted = 1;
      if ((source_str[quoted] == 'I') && (source_str[quoted + 1] == 'N')) {	/*some INT */
	if (source_str[quoted + 3] == 'E')	/* INTERNAL2 */
	  *source = 14;
	else			/* INT */
	  *source = 13;
      } else {
	hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
      }
    }

    errStatus = viPrintf(vi, "ARM:SLOP%hd?\n", event);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = viScanf(vi, "%s%*t", slope_str);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = hpe1429_findIndex(thisPtr, hpe1429_armStarEvent_Q_slope_a, slope_str, slope);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_armStarImm
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function will cause the start trigger to be armed
 *           immediately, regardless of the selected ArmSource.  The
 *           instrument must be initiated and in the wait-for-arm state when
 *           this function is sent.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1429_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1429_armStarImm(ViSession vi)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_armStarImm");

  {
    errStatus = viPrintf(vi, "ARM\n");
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }
    thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_armStarLevels
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function sets the levels which will arm a measurement
 *           cycle when an hpe1429_armStarEvent() event source is set to
 *           Internal <chan>. The allowable levels depend on the measurement
 *           range as set by the "expected" parameter of hpe1429_configure().
 *            The levels must be separated by an amount defined by:
 *               (50 mV / 1.0235V) * measurement range
 *            This accounts for offset errors in the levels specified and
 *           enables the levels to be detected.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 chan
 * IN        
 *            Specifies the channel for which the levels are being specified.
 *           
 *            The channels are defined as macros and are shown below:
 * 
 * PARAM 3 : ViReal64 negative
 * IN        
 *            This selects the input voltage level which will arm a
 *           measurement cycle. This setting is used only when the
 *           hpe1429_armStarEvent() function sets an event source to Internal
 *           <chan> and sets slope to Negative or Either.
 * 
 *      MAX = hpe1429_ARM_STAR_LEV_MAX   102.2418
 *      MIN = hpe1429_ARM_STAR_LEV_MIN   -102.2418
 * 
 * PARAM 4 : ViReal64 positive
 * IN        
 *            This selects the input voltage level which will arm a
 *           measurement cycle. This setting is used only when the
 *           hpe1429_armStarEvent() function sets an event source to Internal
 *           <chan> and sets slope to Positive or Either.
 * 
 *      MAX = hpe1429_ARM_STAR_LEV_MAX   102.2418
 *      MIN = hpe1429_ARM_STAR_LEV_MIN   -102.2418
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1429_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1429_armStarLevels_chan_a[] = { "", "1", "2", 0 };

ViStatus _VI_FUNC hpe1429_armStarLevels(ViSession vi,
					ViInt16 chan, ViReal64 negative, ViReal64 positive)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_armStarLevels");

  hpe1429_CHK_ENUM(chan, 2, VI_ERROR_PARAMETER2);
  hpe1429_CHK_REAL_RANGE(negative, hpe1429_ARM_STAR_LEV_MIN, hpe1429_ARM_STAR_LEV_MAX,
			 VI_ERROR_PARAMETER3);

  hpe1429_CHK_REAL_RANGE(positive, hpe1429_ARM_STAR_LEV_MIN, hpe1429_ARM_STAR_LEV_MAX,
			 VI_ERROR_PARAMETER4);

  errStatus =
      viPrintf(vi, "ARM:STAR:LEV%s:NEG %g;POS %g\n", hpe1429_armStarLevels_chan_a[chan], negative,
	       positive);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_armStarLevels_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function gets the values of the arm levels for the
 *           specified channel.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 chan
 * IN        
 *            Specifies the channel for which the levels are being queried.
 *           
 *            The channels are defined as macros and are shown below:
 * 
 * PARAM 3 : ViPReal64 negative
 * OUT       
 *            Returns the negative going arm level.
 * 
 * PARAM 4 : ViPReal64 positive
 * OUT       
 *            Returns the positive going arm level.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1429_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1429_armStarLevels_Q_chan_a[] = { "", "1", "2", 0 };

ViStatus _VI_FUNC hpe1429_armStarLevels_Q(ViSession vi,
					  ViInt16 chan, ViPReal64 negative, ViPReal64 positive)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_armStarLevels_Q");

  hpe1429_CHK_ENUM(chan, 2, VI_ERROR_PARAMETER2);
  thisPtr->blockSrqIO = VI_TRUE;
  errStatus = viPrintf(vi, "ARM:LEV%s:NEG?\n", hpe1429_armStarLevels_Q_chan_a[chan]);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  errStatus = viScanf(vi, "%lg%*t", negative);
  if (thisPtr)
    doDelay(thisPtr->myDelay);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  thisPtr->blockSrqIO = VI_TRUE;
  errStatus = viPrintf(vi, "ARM:LEV%s:POS?\n", hpe1429_armStarLevels_Q_chan_a[chan]);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  errStatus = viScanf(vi, "%lg%*t", positive);
  if (thisPtr)
    doDelay(thisPtr->myDelay);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_calZero
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function performs a calibration of the zero offset of the
 *           specified channel using the specified number of readings and
 *           sample rate on the specified range(s).  When the command
 *           completes, the new calibration constants will be automatically
 *           stored to non-volatile calibration memory unless the
 *           CAL:STOR:AUTO command, which may be accessed through the
 *           Pass-through functions, is set to OFF.  The product of the
 *           number of readings times the period must be less than or equal to 10 seconds.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 chan
 * IN        
 *            Specifies which channel to calibrate.
 *           
 *            The channels are defined as macros and are shown below:
 * 
 * PARAM 3 : ViInt32 readings
 * IN        
 *            Specifies the number of readings to use during calibration.
 * 
 *      MAX = hpe1429_CAL_ZERO_READ_MAX   32767
 *      MIN = hpe1429_CAL_ZERO_READ_MIN   100
 * 
 * PARAM 4 : ViReal64 period
 * IN        
 *            Specifies the sample rate to use during calibration.
 * 
 *      MAX = hpe1429_CAL_ZERO_PER_MAX   0.1
 *      MIN = hpe1429_CAL_ZERO_PER_MIN   0.00000005
 * 
 * PARAM 5 : ViInt16 mode
 * IN        
 *            The mode specifies whether to calibrate just the present range
 *           and port or to calibrate all ranges on both ports of the
 *           specified channel. The types are defined as macros and are shown
 *           below:
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1429_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1429_calZero_chan_a[] = { "", "1", "2", 0 };
static const char *const hpe1429_calZero_mode_a[] = { "ALL", "ONE", 0 };

ViStatus _VI_FUNC hpe1429_calZero(ViSession vi,
				  ViInt16 chan, ViInt32 readings, ViReal64 period, ViInt16 mode)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_calZero");

  hpe1429_CHK_ENUM(chan, 2, VI_ERROR_PARAMETER2);
  hpe1429_CHK_LONG_RANGE(readings, hpe1429_CAL_ZERO_READ_MIN, hpe1429_CAL_ZERO_READ_MAX,
			 VI_ERROR_PARAMETER3);

  hpe1429_CHK_REAL_RANGE(period, hpe1429_CAL_ZERO_PER_MIN, hpe1429_CAL_ZERO_PER_MAX,
			 VI_ERROR_PARAMETER4);

  hpe1429_CHK_ENUM(mode, 1, VI_ERROR_PARAMETER5);
  errStatus =
      viPrintf(vi, "CAL%s:ZERO %ld, %g, %s\n", hpe1429_calZero_chan_a[chan], readings, period,
	       hpe1429_calZero_mode_a[mode]);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_confLocalBus
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function will configure the HP E1429B's Local Bus for data
 *           transfer. Sending this function to an HP E1429A will generate a
 *           "Missing Hardware" error.
 *           
 *            There are two ways to transfer data over the Local bus:
 *           
 *            Data can be transferred directly from the analog to digital
 *           converter(s) using the ARM and TRIGger subsystems in conjunction
 *           with the hpe1429_initImm() command. Everything proceeds exactly
 *           the same as if readings were going to HP E1429B internal memory,
 *           except in this case, readings are going to the Local bus as well
 *           as to internal memory.  If the consumer on the Local Bus is
 *           unable to maintain the data transfer (sampling) rate, then the
 *           data going out over the Local bus is lost and an error is
 *           indicated.
 *            The second method of transferring data over the Local bus is to
 *           empty the HP E1429B internal memory after the measurements have
 *           occurred. This transfer will automatically proceed after the
 *           measurements are completed if the user has previously set the
 *           local bus mode to Append, Generate, or Insert, and if the source
 *            is one of the Memory choices.  If the measurement is aborted
 *           with the hpe1429_abor() command, or if mode is Off or Pipline
 *           during the measurement, then no automatic transfer is attempted.
 *            Instead, the VINS:LBUS:MEM:INIT command (sent with a
 *           Pass-Through function) must be used to start the transfer after
 *           the mode and source have been set to the desired values.
 *           
 *            The mode selects the operating mode for the VXI Local bus.
 *           The available modes are:
 *                Append: Local bus data is received from the left, and
 *                     passed on to the right until an end of frame is
 *                     detected.  When end of frame is received from the left
 *                     side, all data from this module is appended, followed
 *                     by an end of block flag and a new end of frame flag.
 *                     After sending the end of frame flag, the module enters
 *                     the paused state.  This mode requires a module to the
 *                     left that is in Generate mode.  The mode is not active
 *                     until either an hpe1429_initImm() command or a
 *                     VINStrument:LBUS:MEMory:INITiate command is sent.
 *           
 *                Generate: Local bus data originates in this module and is
 *                     passed to the right, followed by an end of frame flag.
 *                     The mode is not active until either an hpe1429_initImm
 *                     command or a VINS:LBUS:MEM:INIT command is sent.
 *           
 *                Insert: Local bus data is inserted onto the bus from this
 *                     module.  The module will place its data out onto the
 *                     Local bus with an end of block flag at the end and no
 *                     end of frame flag.  The module will then pass through
 *                     (pipeline) any data it receives from the left, and
 *                     will enter the paused state when an end of frame flag
 *                     is received from the left.  This mode requires at
 *                     least one module to the left which is in GENerate
 *                     mode.  The mode is not active until either an
 *                     hpe1429_initImm command or a VINS:LBUS:MEM:INIT
 *                     command is sent.
 *           
 *                Off: The Local bus interface is disabled immediately upon
 *                     receipt of this function.  Local bus data is neither
 *                     used nor passed through.
 *           
 *                Pipeline: Local bus data is passed through and not altered.
 *                     This mode becomes effective immediately upon receipt
 *                     of this function.  Select this mode when data should
 *                     be transparently passed through the HP E1429B.  The
 *                     module will remain in the Pipeline mode even after an
 *                     end of frame flag is received; therefore, it is
 *                     necessary to change modes to take the module out of
 *                     Pipeline mode.
 *           
 *           
 *            The source parameter indicates the source of the data which
 *           will be output to the Local bus. The
 *            data source may be channel 1, channel 2, or both channels. The
 *           data may come from memory or directly from the A/D converter(s).
 *           Sources beginning with "Memory" are the post measurement modes,
 *           sources beginning with "Converter" are the real time modes. 
 *           When the source is one or both A/D converters, care must be
 *           taken that other active instruments in the pipeline can maintain
 *           the data generation rate. If data is available from the A/D
 *           converter but the Local bus is busy and can not accept it, the
 *           data is lost and error 1019; "Data loss detected during LBUS
 *           transfer" is reported.
 *           The possible sources are:
 *                Memory Channel 1: Channel 1 memory is the data source for
 *                      the Local bus. Two bytes per reading will be output
 *                      to the bus.
 *           
 *                Memory Channel 2: Channel 2 memory is the data source for
 *                      the Local bus. Two bytes per reading will be output
 *                      to the bus.
 *           
 *                Memory Both: Both channels of memory are the data
 *                      source for the Local bus.  The channel 2 reading is
 *                      output first, followed by the channel 1 reading.
 *                      Four bytes for each set of readings will be output
 *                      to the bus.
 *           
 *                Converter Channel 1: The channel 1 A/D converter is the
 *                      data source for the Local bus. Two bytes per reading
 *                      will be output to the bus.
 *           
 *                Converter Channel 2: The channel 2 A/D converter is the
 *                      data source for the Local bus. Two bytes per reading
 *                      will be output to the bus.
 *           
 *                Converter Both: Both A/D converters are the data source
 *                      for the Local bus.  The channel 2 reading is output
 *                      first, followed by the channel 1 reading. Four bytes
 *                      for each set of readings will be output to the bus.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 mode
 * IN        
 *            Specifies the operating mode for the VXI Local Bus.
 *           
 *            The types are defined as macros and are shown below:
 * 
 * PARAM 3 : ViInt16 source
 * IN        
 *            Specifies the source of the data which will be output to the
 *           Local Bus.
 *            The types are defined as macros and are shown below:
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1429_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1429_confLocalBus_mode_a[] = { "OFF", "PIP",
  "APP", "INS", "GEN", 0
};

static const char *const hpe1429_confLocalBus_source_a[] = {
  "\"MEM:CHAN1\"", "\"MEM:CHAN2\"", "\"MEM:BOTH\"", "\"CONV:CHAN1\"",
  "\"CONV:CHAN2\"", "\"CONV:BOTH\"", 0
};

ViStatus _VI_FUNC hpe1429_confLocalBus(ViSession vi, ViInt16 mode, ViInt16 source)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_confLocalBus");

  hpe1429_CHK_ENUM(mode, 4, VI_ERROR_PARAMETER2);
  hpe1429_CHK_ENUM(source, 5, VI_ERROR_PARAMETER3);
  errStatus =
      viPrintf(vi, "VINS:LBUS:MODE %s;FEED %s\n", hpe1429_confLocalBus_mode_a[mode],
	       hpe1429_confLocalBus_source_a[source]);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_confLocalBus_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function will query the HP E1429B's Local Bus
 *           configuration for its mode and source.  Sending this function to
 *           an HP E1429A will generate a "Missing Hardware" error.
 *           
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViPInt16 mode
 * OUT       
 *            Returns the operating mode of the VXI Local Bus.
 *           
 *            The types are defined as macros and are shown below:
 * 
 * PARAM 3 : ViPInt16 source
 * OUT       
 *            Returns the source of the data which will be output to the
 *           Local Bus.
 *            The types are defined as macros and are shown below:
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1429_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1429_confLocalBus_Q_mode_a[] = { "OFF", "PIP",
  "APP", "INS", "GEN", 0
};

static const char *const hpe1429_confLocalBus_Q_source_a[] = {
  "\"MEM:CHAN1\"", "\"MEM:CHAN2\"", "\"MEM:BOTH\"", "\"CONV:CHAN1\"",
  "\"CONV:CHAN2\"", "\"CONV:BOTH\"", 0
};

ViStatus _VI_FUNC hpe1429_confLocalBus_Q(ViSession vi, ViPInt16 mode, ViPInt16 source)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;
  char mode_str[32];
  char source_str[32];

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_confLocalBus_Q");

  {
    thisPtr->blockSrqIO = VI_TRUE;
    errStatus = viPrintf(vi, "VINS:LBUS?\n");
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = viScanf(vi, "%s%*t", mode_str);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = hpe1429_findIndex(thisPtr, hpe1429_confLocalBus_Q_mode_a, mode_str, mode);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = viPrintf(vi, "VINS:LBUS:FEED?\n");
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = viScanf(vi, "%s%*t", source_str);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = hpe1429_findIndex(thisPtr, hpe1429_confLocalBus_Q_source_a, source_str, source);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_confVME
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function will configure the HP E1429's VME Bus for data
 *           transfer.
 *            When data is transferred over the VME bus directly from the A/D
 *           converter, a read of VME A24 address space, offset 12 (0x0C),
 *           causes a measurement to be taken and transferred all in the same
 *           read cycle.  During the read cycle, the HP E1429 takes a reading
 *           and puts the data into the register before the read cycle
 *           completes. The ARM sources may be set to any legal source for
 *           this mode, but hpe1429_trigger() source must be set to VME. 
 *           Selections can be made using the hpe1429_confVME() source such
 *            that a single read produces data from only one channel (16
 *           bits), or both channels simultaneously (32 bits).
 *           
 *            When the data is transferred post measurement, completion of
 *           the hpe1429_initImm() command will automatically configure for a
 *           VME transfer from memory, based on the settings of the
 *           hpe1429_confVME() source parameter. When the measurement has
 *           completed and the VME transfer has been set up by the HP E1429,
 *           bit 1 (Memory Read Enable) of the A24 memory control register
 *           (base + 0x21) will go high (1). At this point, data transfer can
 *           be initiated by the receiver by reading the A24 data register
 *           (base + 0x0C).  Again, the hpe1429_confVME() source parameter is
 *           used to specify whether a single read will produce one channel
 *           (16 bits) of data, or two channels (32 bits) of data.  The
 *           VINStrument:VME:MEMory:INITiate command (sent by a Pass-Through
 *           function) will also configure for a post measurement VME data
 *           transfer, but it need not be sent unless it is desirable to read
 *           the same data multiple times.
 *            The mode selects the operating mode for the VME bus.
 *            The available modes are Generate and Off.  The Generate mode is
 *            used when it is desirable to transfer data over the
 *            VME bus by reading the data register at offset 12 (0x0C) in
 *            A24 address space.  The Generate mode is not allowed unless
 *            hpe1429_confLocalBus() mode is set to OFF or PIPeline.
 *           
 *            The source parameter selects which data source will feed the
 *           VME (VXI data transfer) bus. The bus is driven by reading the
 *           data register, offset 12 (0x0C) in A24 address space. Sources
 *           beginning with "Memory" are the post measurement modes, sources
 *           beginning with "Converter" are the real time modes.
 *           The possible sources are:
 *                "Memory Channel 1": Channel 1 memory is the data source for
 *                     the VME bus. One 16-bit reading is returned.
 *           
 *                "Memory Channel 2": Channel 2 memory is the data source for
 *                     the VME bus. One 16-bit reading is returned.
 *           
 *                "Memory Both": Both channels of memory are the data source
 *                     for the VME bus.  In this mode, channel 1 will be
 *                     output the first time the data register is accessed,
 *                     channel 2 is output the second time the data register
 *                     is accessed. One 16-bit reading is returned with each
 *                     access.
 *           
 *                "Memory Both 32": Both channels of memory are the data
 *                     source for the VME bus.  In this mode, accessing the
 *                     data register returns a 32-bit number where the high
 *                     order 16 bits are the channel 2 reading and the low
 *                     order 16 bits are the channel 1 reading.
 *           
 *                "Converter Channel 1": The channel 1 A/D converter is the
 *                     data source for the VME bus. One 16-bit reading is
 *                     returned.
 *           
 *                "Converter Channel 2": The channel 2 A/D converter is the
 *                     data source for the VME bus. One 16-bit reading is
 *                     returned.
 *           
 *                "Converter Both": Accessing the data register triggers both
 *                     A/D converters at the same time, and one 16-bit
 *                     reading (channel 1) is returned.  Accessing the data
 *                     register a second time returns the second 16-bit
 *                     reading (channel 2), but does not trigger the A/Ds.
 *           
 *                "Converter Both 32": Accessing the data register triggers
 *                     both A/D converters at the same time, and one 32-bit
 *                     number is returned.  The high order 16 bits are the
 *                     channel 2 reading, and the low order 16 bits are the
 *                     channel 1 reading.
 *           
 *            If the data in memory is in multiple segments, then
 *            there will be a small delay (630 microseconds) between segments
 *           while the CPU switches the memory address to point to the next
 *           segment. It is possible to determine when data is available
 *           again by monitoring bit 1 of the A24 arm status register (base +
 *           0x43).  This bit goes high when the data is again ready for
 *           transfer. See "VME Bus Data Transfers" in Chapter 3 of the
 *           instrument manual for additional information.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 mode
 * IN        
 *            Specifies the operating mode for the VME Bus.
 *           
 *            The types are defined as macros and are shown below:
 * 
 * PARAM 3 : ViInt16 source
 * IN        
 *            Specifies the source of the data which will feed the VME bus.
 *           
 *            The types are defined as macros and are shown below:
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1429_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1429_confVME_mode_a[] = { "OFF", "GEN", 0 };

static const char *const hpe1429_confVME_source_a[] = { "\"MEM:CHAN1\"",
  "\"MEM:CHAN2\"", "\"MEM:BOTH\"", "\"MEM:BOTH32\"", "\"CONV:CHAN1\"",
  "\"CONV:CHAN2\"", "\"CONV:BOTH\"", "\"CONV:BOTH32\"", 0
};

ViStatus _VI_FUNC hpe1429_confVME(ViSession vi, ViInt16 mode, ViInt16 source)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_confVME");

  hpe1429_CHK_ENUM(mode, 1, VI_ERROR_PARAMETER2);
  hpe1429_CHK_ENUM(source, 7, VI_ERROR_PARAMETER3);
  errStatus =
      viPrintf(vi, "VINS:VME:MODE %s;FEED %s\n", hpe1429_confVME_mode_a[mode],
	       hpe1429_confVME_source_a[source]);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_confVME_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function will query the HP E1429's VME Bus configuration
 *            for its mode and source.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViPInt16 mode
 * OUT       
 *            Returns the operating mode for the VME Bus.
 *           
 *            The types are defined as macros and are shown below:
 * 
 * PARAM 3 : ViPInt16 source
 * OUT       
 *            Returns the source of the data which will be output over the
 *           VME Bus.
 *            The types are defined as macros and are shown below:
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1429_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1429_confVME_Q_mode_a[] = { "OFF", "GEN", 0 };

static const char *const hpe1429_confVME_Q_source_a[] = { "\"MEM:CHAN1\"",
  "\"MEM:CHAN2\"", "\"MEM:BOTH\"", "\"MEM:BOTH32\"", "\"CONV:CHAN1\"",
  "\"CONV:CHAN2\"", "\"CONV:BOTH\"", "\"CONV:BOTH32\"", 0
};

ViStatus _VI_FUNC hpe1429_confVME_Q(ViSession vi, ViPInt16 mode, ViPInt16 source)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;
  char mode_str[32];
  char source_str[32];

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_confVME_Q");

  {
    thisPtr->blockSrqIO = VI_TRUE;
    errStatus = viPrintf(vi, "VINS:VME?\n");
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = viScanf(vi, "%s%*t", mode_str);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = hpe1429_findIndex(thisPtr, hpe1429_confVME_Q_mode_a, mode_str, mode);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = viPrintf(vi, "VINS:VME:FEED?\n");
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = viScanf(vi, "%s%*t", source_str);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = hpe1429_findIndex(thisPtr, hpe1429_confVME_Q_source_a, source_str, source);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_configure
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function will configure a channel (1 or 2) to take
 *           readings on the specified port (the single ended port or the
 *           differential port). The expected value parameter specified
 *           should be the maximum expected measurement value.  The voltage
 *           range is set according to the expected value supplied.  If the
 *           value is greater than 98% of the instrument's range, the next
 *           higher range is automatically chosen. The following table gives
 *           the crossover points for range changes.  Ranges greater than 1
 *           volt are allowed only on the differential port.
 *             Maximum Expected Value   Voltage Range   Resolution
 *                         0.1             0.10235        .00005
 *                         0.2             0.20470        .00010
 *                         0.5             0.51175        .00025
 *                         1.0             1.02350        .00050
 *                         2.0             2.04700        .00100
 *                         5.0             5.11750        .00250
 *                        10.0            10.23500        .00500
 *                        20.0            20.47000        .01000
 *                        50.0            51.17500        .02500
 *                       100.0           102.35000        .05000
 *           
 *            The numTriggers specifies how many readings will be taken
 *           during each numArms cycle of the trigger system.  The
 *           preTriggers specifies how many of those readings will be before
 *           the arming event occurs.  The total number of readings is
 *           limited to at most 524288, although further limits may occur due
 *           to memory partitionings.  The following table discribes the
 *           limits:
 *             if preTriggers == 0:
 *                   Maximum numTriggers = 524288 / numArms
 *           
 *             if preTriggers >  0:
 *                   numArms    (Segments)   Maximum numTriggers
 *                      1              1          524288
 *                      2              2          262144
 *                    3 - 4            4          131072
 *                    5 - 8            8           65536
 *                    9 - 16          16           32768
 *                   17 - 32          32           16384
 *                   33 - 64          64            8192
 *                   65 - 128        128            4096
 *           
 *            If the non-volatile mode of memory is enabled,
 *            hpe1429_memBattStat(vi,VI_TRUE), then all of the maximum
 *           numTriggers counts shown above decrease by four.  These four
 *           memory locations in each segment hold the data necessary to
 *           recover all readings after a power failure.
 *           
 *            The value of preTriggers can be 0 or range from 3 to 65535.  If
 *           preTriggers is not 0 then numTriggers must be greater than or
 *           equal to preTriggers+7. Also, if preTriggers is not 0 then
 *           numArms is limited to 128 rather than 65535.
 *           
 *            Also note that the numTriggers, preTriggers and numArms for
 *           channel 1 and channel 2 are the same.  Both channels will be set
 *           to whatever has been sent to the instrument last.
 *           
 *            If you desire to set numTriggers or numArms to INF, you may use
 *            a Pass-Through function to do it.  Please read the instrument
 *           manual for information concerning when and where this is legal
 *           to do.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 chan
 * IN        
 *            Specifies which channel to configure.
 *           
 *            The channels are defined as macros and are shown below:
 * 
 * PARAM 3 : ViInt16 port
 * IN        
 *            Specifies the desired port for the channel.
 *           
 *            The types are defined as macros and are shown below:
 * 
 * PARAM 4 : ViReal64 expected
 * IN        
 *            Specifies the maximum expected voltage for the specified port
 *           of the specified channel.
 * 
 *      MAX = hpe1429_VOLT_MAX   102.35
 *      MIN = hpe1429_VOLT_MIN   -102.30
 * 
 * PARAM 5 : ViInt32 numTriggers
 * IN        
 *            Specifies the total number of readings to take for each arm
 *           count.
 * 
 *      MAX = hpe1429_TRIG_POINTS_MAX   524288
 *      MIN = hpe1429_TRIG_POINTS_MIN   1
 * 
 * PARAM 6 : ViInt32 preTriggers
 * IN        
 *            Specifies how many pre-arm readings will be taken for each arm.
 * 
 *      MAX = hpe1429_TRIG_OFFSET_MAX   65535
 *      MIN = hpe1429_TRIG_OFFSET_MIN   0
 * 
 * PARAM 7 : ViInt32 numArms
 * IN        
 *            Specifies how many measurement cycles will occur after
 *           hpe1429_initImm() before the trigger system returns to the idle
 *           state.
 * 
 *      MAX = hpe1429_ARM_POINTS_MAX   65535
 *      MIN = hpe1429_ARM_POINTS_MIN   1
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1429_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1429_configure_chan_a[] = { "", "1", "2", 0 };
static const char *const hpe1429_configure_port_a[] = { "SINGLE", "DIFFER", 0 };

ViStatus _VI_FUNC hpe1429_configure(ViSession vi,
				    ViInt16 chan,
				    ViInt16 port,
				    ViPReal64 expected,
				    ViInt32 numTriggers, ViInt32 preTriggers, ViInt32 numArms)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_configure");

  hpe1429_CHK_ENUM(chan, 2, VI_ERROR_PARAMETER2);
  hpe1429_CHK_ENUM(port, 1, VI_ERROR_PARAMETER3);
  hpe1429_CHK_REAL_RANGE(*expected, hpe1429_VOLT_MIN, hpe1429_VOLT_MAX, VI_ERROR_PARAMETER4);

  hpe1429_CHK_LONG_RANGE(numTriggers, hpe1429_TRIG_POINTS_MIN, hpe1429_TRIG_POINTS_MAX,
			 VI_ERROR_PARAMETER5);

  hpe1429_CHK_LONG_RANGE(preTriggers, hpe1429_TRIG_OFFSET_MIN, hpe1429_TRIG_OFFSET_MAX,
			 VI_ERROR_PARAMETER6);

  hpe1429_CHK_LONG_RANGE(numArms, hpe1429_ARM_POINTS_MIN, hpe1429_ARM_POINTS_MAX,
			 VI_ERROR_PARAMETER7);

  {
    /* Single ended port for channel 1 is @1 */
    /* Single ended port for channel 2 is @2 */
    /* Differential port for channel 1 is @3 */
    /* Differential port for channel 2 is @4 */
    /* Use (chan+2*port) to translate user's port to instrument's port */
    errStatus = viPrintf(vi, "CONF%hd:ARR (%ld),%lg,(@%hd)\n",
			 chan, numTriggers, *expected, (chan + 2 * port));
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    /* The instrument uses negative numbers for offset because the   */
    /* SCPI definition of offset can be either way, but for this     */
    /* instrument there is only one way.  And giving the user a      */
    /* positive number is a better presentation--so convert it here. */
    errStatus = viPrintf(vi, "SWE:OFFS:POIN %ld\n", (-preTriggers));
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }
    errStatus = viPrintf(vi, "ARM:COUN %ld\n", numArms);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_configure_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function will query a channel (1 or 2) for its current
 *           settings, indicating which port (single ended or differential)
 *           will be used, the voltage range, the number of total triggers
 *           per arming cycle, the number of triggers which will occur before
 *           the arm is accepted, and the number of arms for each initiation
 *           of the instrument. If numArms or numTriggers has been set to
 *           INF, this function will return -1 for that parameter as 9.9E37
 *           cannot be represented in an ViInt32 variable.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 chan
 * IN        
 *            Specifies which channel to query.
 *           
 *            The channels are defined as macros and are shown below:
 * 
 * PARAM 3 : ViPInt16 port
 * OUT       
 *            Returns an integer indicating the port for the channel.
 *           
 *            The types are defined as macros and are shown below:
 * 
 * PARAM 4 : ViPReal64 range
 * OUT       
 *            Returns the voltage range of the specified channel.
 * 
 * PARAM 5 : ViPInt32 numTriggers
 * OUT       
 *            Returns the total number of readings that will be taken for
 *           each arm count.
 * 
 * PARAM 6 : ViPInt32 preTriggers
 * OUT       
 *            Returns how many pre-arm readings will be taken for each arm.
 * 
 * PARAM 7 : ViPInt32 numArms
 * OUT       
 *            Returns how many measurement cycles will occur after
 *           hpe1429_initImm() before the trigger system returns to the idle
 *           state.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1429_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1429_configure_Q_chan_a[] = { "", "1", "2", 0 };

static const char *const hpe1429_configure_Q_port_a[] = { "SINGLE",
  "DIFFER", 0
};

ViStatus _VI_FUNC hpe1429_configure_Q(ViSession vi,
				      ViInt16 chan,
				      ViPInt16 port,
				      ViPReal64 range,
				      ViPInt32 numTriggers, ViPInt32 preTriggers, ViPInt32 numArms)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;
  char port_str[32];

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_configure_Q");

  hpe1429_CHK_ENUM(chan, 2, VI_ERROR_PARAMETER2);
  {
    /* ViChar port_str[32]; -- don't need to declare this (autoscript does it) */
    ViReal64 bigTrig;
    ViReal64 bigArm;
    thisPtr->blockSrqIO = VI_TRUE;
    errStatus = viPrintf(vi, "SENS%hd:FUNC?\n", chan);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }
    errStatus = viScanf(vi, "%t", port_str);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = viPrintf(vi, "SENS%hd:VOLT:RANG?\n", chan);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }
    errStatus = viScanf(vi, "%lg%*t", range);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = viPrintf(vi, "SENS:SWE:POIN?\n");
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }
    errStatus = viScanf(vi, "%lg%*t", &bigTrig);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = viPrintf(vi, "SENS:SWE:OFFS:POIN?\n");
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }
    errStatus = viScanf(vi, "%ld%*t", preTriggers);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = viPrintf(vi, "ARM:COUN?\n");
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }
    errStatus = viScanf(vi, "%lg%*t", &bigArm);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    *port = 0;
    if (chan == 1) {		/* Find 1 or 3 port number & convert to Single / Diff enum */
      if (port_str[5] == '3')
	*port = 1;
    } else {			/* chan 2 - Find 2 or 4 port number & convert to Single / Diff enum */

      if (port_str[5] == '4')
	*port = 1;
    }

    if (bigTrig < 18000000)	/* The instrument could be set to 9.9E37 */
      *numTriggers = (ViInt32) (bigTrig + 0.1);	/* add .1 to solve CVI round off */
    else
      *numTriggers = -1;	/* This driver doesn't support INF triggers */

    if (bigArm < 18000000)	/* The instrument could be set to 9.9E37 */
      *numArms = (ViInt32) (bigArm + 0.1);	/* add .1 to solve CVI round off */
    else
      *numArms = -1;		/* This driver doesn't support INF arms */

    *preTriggers = -*preTriggers;	/* Instr uses negative numbers, we show + */

    if (errStatus < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_disableHighSpeed
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function is used to permanently disable high-speed VME
 *           access attempts by the hpe1429_fetc_Q() function for this
 *           session. A mask of 0 will disable all high speed access.  A mask
 *           value of 2 will disable block transfers in WIN32 embedded
 *           controlers. All other non-0 masks will disable the fast init
 *           routine of embedded controlers.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 mask
 * IN        
 *            Specifies what to disable.
 * 
 *      MAX = hpe1429_DISABLE_MAX   32767
 *      MIN = hpe1429_DISABLE_MIN   0
 * 
 * RETURN  :  VI_SUCCESS: No error.
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1429_disableHighSpeed(ViSession vi, ViInt16 mask)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_disableHighSpeed");

  hpe1429_CHK_INT_RANGE(mask, hpe1429_DISABLE_MIN, hpe1429_DISABLE_MAX, VI_ERROR_PARAMETER2);

  {
    switch (mask) {
    case 0:
      thisPtr->controler = 0;
      break;
    case 2:
      if (thisPtr->controler & 2) {
	thisPtr->controler |= 0x00010000;
      }
      break;
    default:
      if (thisPtr->controler & 2) {
	thisPtr->controler &= 0xFFFFFFFB;
	thisPtr->controler |= 0x0000A000;
      }
      break;
    }
    errStatus = VI_SUCCESS;
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_fetcScal_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function is used in conjunction with the hpe1429_initImm()
 *            command to obtain readings.  The readings obtained by this
 *           function are scaled according to the specified range command,
 *           but the data transfer is slower than that of the
 *           hpe1429_fetc_Q() function which gives the data as 16 bit
 *           integers. The data may be read any number of times, as long as
 *           no parameters have changed which could affect new data.
 *           
 *            The number of readings that this function will return
 *            for each channel can be determined by calculating the
 *            armCount times the trigCount.  Note that if an hpe1429_abor()
 *            is done, fewer readings may be valid.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 chan
 * IN        
 *            Specifies which channel for which to obtain data.
 *           
 *            The channels are defined as macros and are shown below:
 * 
 * PARAM 3 : ViInt32 arrayLength
 * IN        
 *            Specifies the length of the data array in number of readings.
 * 
 *      MAX = hpe1429_FETC_SIZE_MAX   524288
 *      MIN = hpe1429_FETC_SIZE_MIN   1
 * 
 * PARAM 4 : ViReal64 _VI_FAR data[]
 * OUT       
 *            Pointer to the array of doubles which will hold the A/D reading
 *           values. Make sure it is large enough to hold the data.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1429_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1429_fetcScal_Q_chan_a[] = { "", "1", "2", 0 };

ViStatus _VI_FUNC hpe1429_fetcScal_Q(ViSession vi,
				     ViInt16 chan, ViInt32 arrayLength, ViReal64 _VI_FAR data[])
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_fetcScal_Q");

  hpe1429_CHK_ENUM(chan, 2, VI_ERROR_PARAMETER2);
  hpe1429_CHK_LONG_RANGE(arrayLength, hpe1429_FETC_SIZE_MIN, hpe1429_FETC_SIZE_MAX,
			 VI_ERROR_PARAMETER3);

  {
    ViChar c[2];
    ViUInt32 digits;
    ViChar length_str[16];
    ViUInt32 nbytes;
    char fmtStr[40];
    unsigned char src[8];
    ViInt32 i;

    thisPtr->blockSrqIO = VI_TRUE;

    errStatus = viPrintf(vi, "FORM REAL;:FETC%hd?\n", chan);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = viScanf(vi, "%2c", c);	/* Read Header (should be #[1-9] */
    if (errStatus >= VI_SUCCESS) {
      digits = c[1] - '0';
      if ((digits < 1) || (9 < digits)) {
	errStatus = viScanf(vi, "%*t");	/* Clear input. */
	errStatus = VI_ERROR_INV_RESPONSE;
      }
    }

    if (errStatus >= VI_SUCCESS) {
      /* Scan DAB array count. */
      errStatus = viScanf(vi, "%#c", &digits, length_str);
      if (errStatus >= VI_SUCCESS) {
	length_str[digits] = '\0';	/* null terminate the string */
	nbytes = atol(length_str);

	/* Verify that caller's array is big enough. */
	if (((ViUInt32) arrayLength * 8) < nbytes) {
	  errStatus = viScanf(vi, "%*t");	/* Clear input. */
	  errStatus = VI_ERROR_PARAMETER3;	/* Caller's array too small. */
	}
      }
    }

    if (errStatus >= VI_SUCCESS) {
      sprintf(fmtStr, "%%%ldc%%*t", nbytes);
      errStatus = viScanf(vi, fmtStr, (unsigned char *)data);

#ifndef B_ENDIAN
      /* need byte swapping */
      if (errStatus >= VI_SUCCESS) {
	for (i = 0; i < (ViInt32) (nbytes / 8); i++) {
	  *((ViReal64 *) src) = *((ViReal64 *) (&data[i]));
	  ((unsigned char *)(&data[i]))[0] = ((unsigned char *)(src))[7];
	  ((unsigned char *)(&data[i]))[1] = ((unsigned char *)(src))[6];
	  ((unsigned char *)(&data[i]))[2] = ((unsigned char *)(src))[5];
	  ((unsigned char *)(&data[i]))[3] = ((unsigned char *)(src))[4];
	  ((unsigned char *)(&data[i]))[4] = ((unsigned char *)(src))[3];
	  ((unsigned char *)(&data[i]))[5] = ((unsigned char *)(src))[2];
	  ((unsigned char *)(&data[i]))[6] = ((unsigned char *)(src))[1];
	  ((unsigned char *)(&data[i]))[7] = ((unsigned char *)(src))[0];
	}
      }
#endif
    }
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }
    thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_fetc_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function is used in conjunction with the hpe1429_initImm()
 *            command to obtain readings.  The readings obtained by this
 *           function are not scaled according to the specified range
 *           command, but they are transferred faster than the scaled
 *           readings obtained by the hpe1429_fetcScal_Q() function.
 *            The data may be read any number of times, as long as no
 *           parameters have changed which could affect new data.
 *           
 *            The number of readings that this function will return
 *            for each channel can be determined by calculating the
 *            armCount times the trigCount.  Note that if an hpe1429_abor()
 *            is done, fewer readings may be valid.
 *           
 *            Caution: Normally this driver will attempt to do High Speed
 *            data access.  If you experience problems with High
 *            Speed access, you may use the function
 *           hpe1429_disableHighSpeed() to disable this feature.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 chan
 * IN        
 *            Specifies which channel for which to obtain data.
 *           
 *            The channels are defined as macros and are shown below:
 * 
 * PARAM 3 : ViInt32 arrayLength
 * IN        
 *            Specifies the length of the data array in number of readings.
 * 
 *      MAX = hpe1429_FETC_SIZE_MAX   524288
 *      MIN = hpe1429_FETC_SIZE_MIN   1
 * 
 * PARAM 4 : ViInt16 _VI_FAR data[]
 * OUT       
 *            Pointer to the array of 16 bit integers which will hold the A/D
 *           reading values. Make sure it is large enough to hold the data.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1429_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1429_fetc_Q_chan_a[] = { "", "1", "2", 0 };

ViStatus _VI_FUNC hpe1429_fetc_Q(ViSession vi,
				 ViInt16 chan, ViInt32 arrayLength, ViInt16 _VI_FAR data[])
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_fetc_Q");

  hpe1429_CHK_ENUM(chan, 2, VI_ERROR_PARAMETER2);
  hpe1429_CHK_LONG_RANGE(arrayLength, hpe1429_FETC_SIZE_MIN, hpe1429_FETC_SIZE_MAX,
			 VI_ERROR_PARAMETER3);

  {
    ViUInt32 nbytes;
    unsigned char src[2];
    ViInt32 i;
    ViInt16 gotit;

#ifdef WIN32
    ViPInt16 fptr = data;
#else
    ViInt16 _huge *fptr;
    fptr = (ViInt16 _huge *) data;
#endif

    thisPtr->blockSrqIO = VI_TRUE;

    gotit = 0;

    if (arrayLength > 500) {
      if (((thisPtr->controler & 26) == 18) || ((thisPtr->controler & 19) == 17)
	  || ((thisPtr->controler & 4099) == 1)
	  ) {			/* wait for done */
	/* This section makes sure the e1429 is idle before fetching data */
	ViInt32 operCond;
	ViBoolean opc;
	ViInt32 timeOut;
	ViUInt32 digits;
	ViUInt32 burnMore;
	ViInt32 burnTime = 10000;
	struct _timeb start_time, tic_time, now_time;

	errStatus = viGetAttribute(vi, VI_ATTR_TMO_VALUE, &timeOut);
	if (errStatus < VI_SUCCESS) {
	  hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
	}
	if (timeOut < 2000000.0)
	  burnMore = (ViUInt32) (timeOut * 500);
	else
	  burnMore = 1000000000;

	if (burnMore < 6000)
	  burnMore = 6000;

	thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
	errStatus = hpe1429_opc_Q(vi, &opc);
	if (errStatus < VI_SUCCESS) {
	  hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
	}
	digits = 0;
	_ftime(&start_time);
	while (digits < burnMore) {
	  thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
	  errStatus = hpe1429_operCond_Q(vi, &operCond);	/* check idle */
	  thisPtr->controler &= 0xFFFFDFFF;	/* Clear Flag if operCond doesn't */
	  if (errStatus < VI_SUCCESS) {
	    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
	  }
	  if (operCond) {	/* busy, burn time */
	    _ftime(&tic_time);
	    for (operCond = 0; operCond < burnTime; operCond++) {
	      _ftime(&now_time);
	      if (((now_time.time + 0.001 * now_time.millitm) -
		   (tic_time.time + 0.001 * tic_time.millitm)) > 0.001)
		operCond = burnTime + 10;	/* exit for loop early on slow processor */
	    }
	    operCond = 256;
	    if ((((now_time.time + 0.001 * now_time.millitm) -
		  (start_time.time + 0.001 * start_time.millitm)) > timeOut) & (timeOut > 1))
	      digits = burnMore;	/* exit while loop early */
	  } else
	    digits = burnMore;
	  digits++;
	}
	if (operCond) {		/* not idle yet */
	  hpe1429_LOG_STATUS(vi, thisPtr, VI_ERROR_TMO);
	}
      }
    }

    if (((thisPtr->controler & 26) == 18) && (arrayLength > 500)) {	/* mapped embedded */
      if (thisPtr->controler & 1)
	errStatus = hpe1429_fetcLINK(vi, chan, arrayLength, &nbytes, thisPtr, fptr);
      else
	errStatus = hpe1429_fetcFast(vi, chan, arrayLength, &nbytes, thisPtr, fptr);
      if (errStatus >= VI_SUCCESS)
	gotit = 6;
      else {
	if (errStatus == VI_ERROR_PARAMETER3) {
	  hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
	}
      }
    }

    if (((thisPtr->controler & 3) == 1) && (arrayLength > 500)) {	/* hpe1406 */
      if ((thisPtr->controler & 16) || (!(thisPtr->controler & 4096))) {
	errStatus = hpe1429_fetcE1406(vi, chan, arrayLength, &nbytes, thisPtr, fptr);
	if (errStatus >= VI_SUCCESS)
	  gotit = 2;
	else {
	  if ((errStatus == VI_ERROR_PARAMETER3) || (errStatus == VI_ERROR_TMO)) {
	    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
	  }
	}
      }
    }

    if (gotit < 2) {		/* Something above failed--try FETCH way */
      errStatus = hpe1429_fetcSCPI(vi, chan, arrayLength, &nbytes, thisPtr, fptr);
      if (errStatus < VI_SUCCESS) {
	hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
      }
    }

#ifndef B_ENDIAN
    /* need byte swapping */
    if ((errStatus >= VI_SUCCESS) && (gotit < 6)) {	/* don't swap embedded */
      for (i = 0; i < (ViInt32) (nbytes / 2); i++) {
	*((ViInt16 *) src) = *((ViInt16 *) (&fptr[i]));
	((unsigned char *)(&fptr[i]))[0] = ((unsigned char *)(src))[1];
	((unsigned char *)(&fptr[i]))[1] = ((unsigned char *)(src))[0];
      }
    }
#endif
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }
    thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_initImm
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  The hpe1429_initImm() command controls the initiation of the
 *            trigger subsystem and prepares the HP E1429 to take voltage
 *            measurements.  Once initiated, triggers are armed on both
 *           channels, and a trigger received from the programmed source will
 *           cause voltage measurements to begin on both channels.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1429_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1429_initImm(ViSession vi)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_initImm");

  {
    ViAddr base_addr;
    //At the begining, controler==&B1010 (Embedded controler not mapped yet, Embedded controler)
    //22==&B10110 (Using fast fetch, Using fast init, Embedded controler)
    if ((thisPtr->controler & 22) == 22) {	/* give fast init */
      //Reset Bit 14, 15
      thisPtr->controler &= 0xFFFF3FFF;	/* 0 the SCPI init bit */
      if (VI_SUCCESS <= viMapAddress(vi, VI_A24_SPACE, 0x00, 0xFF, VI_FALSE, VI_NULL, &base_addr)) {
	if (thisPtr->controler & 1)	/* VX_LINK */
	  errStatus = hpe1429_LINKInit(vi, (char *)base_addr, thisPtr);
	else
	  errStatus = hpe1429_fastInit((char *)base_addr, thisPtr);

	viUnmapAddress(vi);
	if (errStatus >= VI_SUCCESS) {
	  //Set Bit 13
	  thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
	  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
	}
      } else
	thisPtr->controler = 0;	/* Failing -- remove fast access */
    }
    /* if we get here, fast init must not have happened--do normal init */
    thisPtr->controler &= 0xFFFFFFFB;	/* 0 the fast init bit at Bit 2 */
    errStatus = viPrintf(vi, "INIT\n");
    if (thisPtr->controler & 2)
      thisPtr->controler |= 16384;	/* set the used SCPI INIT flag, 16384==&x4000, Set Bit 14 */
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    if ((thisPtr->controler & 4098) == 2)	/* embedded controler not checked yet, 4098==&x1002, Checked Setup, Embedded controler */
      hpe1429_checkEmbedded(vi, thisPtr);

    if ((thisPtr->controler & 3) == 1)	/* hpe1406 command module */
      thisPtr->controler &= 0xFFFFEFFF;	/* clear checked bit (fect:coun can change) */

    thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
  }
  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_maxMin
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function will find the maximum and minimum voltage
 *           readings on a channel over the first arm cycle and report those
 *           values and the time or position of their occurrence.  This
 *           function does not take new readings.  Measurements must have
 *           been completed before sending this function.
 *           
 *            Caution: Normally this driver will attempt to do High Speed
 *            data access.  If you experience problems with High
 *            Speed access, you may use the function
 *           hpe1429_disableHighSpeed() to disable this feature.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 chan
 * IN        
 *            Specifies which channel to configure and measure.
 *           
 *            The channels are defined as macros and are shown below:
 * 
 * PARAM 3 : ViPReal64 maxVolt
 * OUT       
 *            Returns the maximum measured voltage on the specified channel.
 *            If the numArms is more than 1, the search period will be
 *           limited to the first arm cycle.
 * 
 * PARAM 4 : ViPReal64 maxTime
 * OUT       
 *            Returns the first time that maximum measured voltage occurred
 *            on the specified channel.  If the numArms is more than 1,
 *            the search period will be limited to the first arm cycle.
 *            If the trigger source is Timer or Dual timer, the time given
 *            will be the time delay from the Arm Event to the first
 *           occurrence of the maximum voltage.  If the trigger source is not
 *           a timer, the value given will be in number of triggers.  The
 *           maxTime may be negative if preTriggers is not 0 and the maximum
 *            Voltage occurs before the Arm Event.
 * 
 * PARAM 5 : ViPReal64 minVolt
 * OUT       
 *            Returns the minimum measured voltage on the specified channel.
 *            If the numArms is more than 1, the search period will be
 *           limited to the first arm cycle.
 * 
 * PARAM 6 : ViPReal64 minTime
 * OUT       
 *            Returns the first time that minimum measured voltage occurred
 *            on the specified channel.  If the numArms is more than 1,
 *            the search period will be limited to the first arm cycle.
 *            If the trigger source is Timer or Dual timer, the time given
 *            will be the time delay from the Arm Event to the first
 *           occurrence of the minimum voltage.  If the trigger source is not
 *           a timer, the value given will be in number of triggers.  The
 *           minTime may be negative if preTriggers is not 0 and the minimum
 *            Voltage occurs before the Arm Event.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1429_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1429_maxMin_chan_a[] = { "", "1", "2", 0 };

ViStatus _VI_FUNC hpe1429_maxMin(ViSession vi,
				 ViInt16 chan,
				 ViPReal64 maxVolt,
				 ViPReal64 maxTime, ViPReal64 minVolt, ViPReal64 minTime)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_maxMin");

  hpe1429_CHK_ENUM(chan, 2, VI_ERROR_PARAMETER2);
  {
    ViInt16 port;
    ViInt16 waiting;
    ViInt16 busy;
    ViInt16 spin;
    ViInt16 spinner;
    ViInt32 totalCount;
    ViInt32 trigCount;
    ViInt32 preTrigs;
    ViInt32 numArms;
    ViReal64 range;
    ViReal64 trigTime;
    ViReal64 timer2;
#ifdef WIN32
    ViInt16 *data;		/* Use integer -- faster & doesn't need as much memory */
#else
    ViInt16 _huge *data;	/* Use integer -- faster & doesn't need as much memory */
#endif
    ViInt16 maxV;
    ViInt16 minV;

    ViInt32 iMin;
    ViInt32 iMax;
    ViInt32 i;
    ViInt16 trigSour;

    *maxVolt = -99999;
    *minVolt = 99999;
    *maxTime = -99999999;
    *minTime = 99999999;
    maxV = -32767;
    minV = 32767;
    iMax = -99999999;
    iMin = 99999999;
    timer2 = 0.0;

    errStatus = hpe1429_configure_Q(vi, chan, &port, &range, &trigCount, &preTrigs, &numArms);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    if (trigCount < 0)
      trigCount = 524288;
    if (numArms < 0)
      numArms = 524288;

    errStatus = hpe1429_trigger_Q(vi, &trigSour, &trigTime);
    waiting = 0;

    thisPtr->blockSrqIO = VI_TRUE;
    do {
      errStatus = viPrintf(vi, "STAT:OPER:COND? \n");
      if (errStatus < VI_SUCCESS) {
	hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
      }
      errStatus = viScanf(vi, "%hd%*t", &busy);
      if (thisPtr)
	doDelay(thisPtr->myDelay);
      if (errStatus < VI_SUCCESS) {
	hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
      }

      if (busy & 256) {
	spinner = 0;
	for (spin = 0; spin < 1000; spin++)	/* burn some time to minimize I-O */
	  spinner = spinner + 1;
      }

      waiting++;
    } while ((waiting < 1000) && (busy & 256));

    if (busy & 256) {		/* still busy--set for max readings and see if timeout occurs */
      if ((ViReal64) numArms * (ViReal64) trigCount < 524288)
	totalCount = numArms * trigCount;
      else
	totalCount = 524288;
    } else {
      errStatus = viPrintf(vi, "FETC:COUN? \n");
      if (errStatus < VI_SUCCESS) {
	hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
      }
      errStatus = viScanf(vi, "%ld%*t", &totalCount);
      if (thisPtr)
	doDelay(thisPtr->myDelay);
      if (errStatus < VI_SUCCESS) {
	hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
      }
    }

    if (totalCount < trigCount)
      trigCount = totalCount;

#ifdef WIN32
    data = (ViInt16 *) malloc((long)totalCount * sizeof(ViInt16));
#else
    data = (ViInt16 _huge *) _halloc((long)totalCount, sizeof(ViInt16));
#endif
    if (data == NULL) {
      hpe1429_LOG_STATUS(vi, thisPtr, VI_ERROR_ALLOC);
    }

    errStatus = hpe1429_fetc_Q(vi, chan, totalCount, data);

    if (errStatus < VI_SUCCESS) {
#ifdef WIN32
      free(data);
#else
      _hfree(data);
#endif
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    /* OK, got the data ... analize  */
    for (i = 0; i < trigCount; i++) {
      if (data[i] < minV) {
	minV = data[i];
	iMin = i;
      }
      if (data[i] > maxV) {
	maxV = data[i];
	iMax = i;
      }
    }
#ifdef WIN32
    free(data);			/* done with the data, so free it */
#else
    _hfree(data);		/* done with the data, so free it */
#endif

    *maxVolt = range * maxV / 32752.0;
    *minVolt = range * minV / 32752.0;

    /* Subtract the pre-triggers */
    if (preTrigs) {
      iMin -= preTrigs;
      iMax -= preTrigs;
    }

    switch (trigSour) {		/* See if trigger source is timer */
    case hpe1429_TRIG_TIM:
      *maxTime = iMax * trigTime;
      *minTime = iMin * trigTime;
      break;
    case hpe1429_TRIG_DTIM:
      if ((iMax > 0) || (iMin > 0)) {
	errStatus = viPrintf(vi, "TRIG:TIM2? \n");
	if (errStatus < VI_SUCCESS) {
	  hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
	}
	errStatus = viScanf(vi, "%lg%*t", &timer2);
	if (thisPtr)
	  doDelay(thisPtr->myDelay);
	if (errStatus < VI_SUCCESS) {
	  hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
	}
      }
      if (iMax <= 0)
	*maxTime = iMax * trigTime;
      else
	*maxTime = iMax * timer2;

      if (iMin <= 0)
	*minTime = iMin * trigTime;
      else
	*minTime = iMin * timer2;

      break;
    default:
      *maxTime = iMax;
      *minTime = iMin;
      break;
    }
    thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_measure
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function will configure a channel (1 or 2) to take
 *           readings on the specified port (the single ended port or the
 *           differential port), initialize the instrument, and then fetch
 *           the scaled results. This is equivalent to hpe1429_configure_Q(),
 *            hpe1429_initImm(), plus hpe1429_fetcScal_Q() all done in
 *           sequence. The expected value parameter specified should be the
 *           maximum expected measurement value.  The voltage range is set
 *           according to the expected value supplied.  If the value is
 *           greater than 98% of the instrument's range, the next higher
 *           range is automatically chosen. The following table gives the
 *           crossover points for range changes.  Ranges greater than 1 volt
 *           are allowed only on the differential port.
 *             Maximum Expected Value   Voltage Range   Resolution
 *                         0.1             0.10235        .00005
 *                         0.2             0.2047         .00010
 *                         0.5             0.51175        .00025
 *                         1.0             1.0235         .00050
 *                         2.0             2.047          .00100
 *                         5.0             5.1175         .00250
 *                        10.0            10.235          .00500
 *                        20.0            20.47           .01000
 *                        50.0            51.175          .02500
 *                       100.0           102.35           .05000
 *           
 *            The numTriggers specifies how many readings will be taken
 *           during each numArms cycle of the trigger system.  The
 *           preTriggers specifies how many of those readings will be before
 *           the arming event occurs.  The total number of readings is
 *           limited to at most 524288, although further limits may occur due
 *           to memory partitionings.  The following table discribes the
 *           limits:
 *             preTriggers equal to 0:
 *                   Maximum numTriggers = 524288 / numArms
 *           
 *             preTriggers greater than 0:
 *                   numArms    (Segments)   Maximum numTriggers
 *                      1              1          524288
 *                      2              2          262144
 *                    3 - 4            4          131072
 *                    5 - 8            8           65536
 *                    9 - 16          16           32768
 *                   17 - 32          32           16384
 *                   33 - 64          64            8192
 *                   65 - 128        128            4096
 *           
 *            If the non-volatile mode of memory is enabled,
 *            hpe1429_memBattStat(vi,VI_TRUE), then all of the maximum
 *           numTriggers counts shown above decrease by four.  These four
 *           memory locations in each segment hold the data necessary to
 *           recover all readings after a power failure.
 *           
 *            The value of preTriggers can be 0 or range from 3 to 65535.  If
 *           preTriggers is not 0 then numTriggers must be greater than or
 *           equal to preTriggers+7. Also, if preTriggers is not 0 then
 *           numArms is limited to 128 rather than 65535.
 *           
 *            Also note that the numTriggers, preTriggers and numArms for
 *           channel 1 and channel 2 are the same.  Both channels will be set
 *           to whatever has been sent to the instrument last.
 *           
 *            If you desire to set numTriggers or numArms to INF, you may use
 *            a Pass-Through function to do it.  Please read the instrument
 *           manual for information concerning when and where this is legal
 *           to do.
 *            Caution: Normally this driver will attempt to do High Speed
 *            data access. If you experience problems with High
 *            Speed access, you may use the function
 *           hpe1429_disableHighSpeed() to disable this feature.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 chan
 * IN        
 *            Specifies which channel to configure and measure.
 *           
 *            The channels are defined as macros and are shown below:
 * 
 * PARAM 3 : ViInt16 port
 * IN        
 *            Specifies the desired port for the channel.
 *           
 *            The types are defined as macros and are shown below:
 * 
 * PARAM 4 : ViReal64 expected
 * IN        
 *            Specifies the maximum expected voltage for the specified port
 *           of the specified channel.
 * 
 *      MAX = hpe1429_VOLT_MAX   102.35
 *      MIN = hpe1429_VOLT_MIN   -102.30
 * 
 * PARAM 5 : ViInt32 numTriggers
 * IN        
 *            Specifies the total number of readings to take for each arm
 *           count.
 * 
 *      MAX = hpe1429_TRIG_POINTS_MAX   524288
 *      MIN = hpe1429_TRIG_POINTS_MIN   1
 * 
 * PARAM 6 : ViInt32 preTriggers
 * IN        
 *            Specifies how many pre-arm readings will be taken for each arm.
 * 
 *      MAX = hpe1429_TRIG_OFFSET_MAX   65535
 *      MIN = hpe1429_TRIG_OFFSET_MIN   0
 * 
 * PARAM 7 : ViInt32 numArms
 * IN        
 *            Specifies how many measurement cycles will occur after
 *           hpe1429_initImm() before the trigger system returns to the idle
 *           state.
 * 
 *      MAX = hpe1429_ARM_POINTS_MAX   65535
 *      MIN = hpe1429_ARM_POINTS_MIN   1
 * 
 * PARAM 8 : ViInt32 arrayLength
 * IN        
 *            Specifies the length of the data array in number of readings.
 * 
 *      MAX = hpe1429_FETC_SIZE_MAX   524288
 *      MIN = hpe1429_FETC_SIZE_MIN   1
 * 
 * PARAM 9 : ViReal64 _VI_FAR data[]
 * OUT       
 *            Pointer to the array of doubles which will hold the A/D reading
 *           values. Make sure it is large enough to hold the data.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1429_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1429_measure_chan_a[] = { "", "1", "2", 0 };
static const char *const hpe1429_measure_port_a[] = { "SINGLE", "DIFFER", 0 };

ViStatus _VI_FUNC hpe1429_measure(ViSession vi,
				  ViInt16 chan,
				  ViInt16 port,
				  ViReal64 expected,
				  ViInt32 numTriggers,
				  ViInt32 preTriggers,
				  ViInt32 numArms, ViInt32 arrayLength, ViReal64 _VI_FAR data[])
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_measure");

  hpe1429_CHK_ENUM(chan, 2, VI_ERROR_PARAMETER2);
  hpe1429_CHK_ENUM(port, 1, VI_ERROR_PARAMETER3);
  hpe1429_CHK_REAL_RANGE(expected, hpe1429_VOLT_MIN, hpe1429_VOLT_MAX, VI_ERROR_PARAMETER4);

  hpe1429_CHK_LONG_RANGE(numTriggers, hpe1429_TRIG_POINTS_MIN, hpe1429_TRIG_POINTS_MAX,
			 VI_ERROR_PARAMETER5);

  hpe1429_CHK_LONG_RANGE(preTriggers, hpe1429_TRIG_OFFSET_MIN, hpe1429_TRIG_OFFSET_MAX,
			 VI_ERROR_PARAMETER6);

  hpe1429_CHK_LONG_RANGE(numArms, hpe1429_ARM_POINTS_MIN, hpe1429_ARM_POINTS_MAX,
			 VI_ERROR_PARAMETER7);

  hpe1429_CHK_LONG_RANGE(arrayLength, hpe1429_FETC_SIZE_MIN, hpe1429_FETC_SIZE_MAX,
			 VI_ERROR_PARAMETER8);

  {
    errStatus =
	viPrintf(vi, "CONF%hd:ARR (%ld),%lg,(@%hd)\n", chan, numTriggers, expected,
		 (chan + 2 * port));
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }
    errStatus = viPrintf(vi, "SWE:OFFS:POIN %ld\n", (-preTriggers));
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }
    errStatus = viPrintf(vi, "ARM:COUN %ld\n", numArms);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }
    errStatus = hpe1429_initImm(vi);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }
    errStatus = hpe1429_fetcScal_Q(vi, chan, arrayLength, data);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_memBattStat
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function enables or disables  non-volatile
 *            memory. If the state is ON (VI_TRUE), then measurement data
 *           will be preserved in the HP E1429A's internal memory when power
 *           fails or the instrument is turned off.
 *           
 *            When enabled, the maximum trigger count is reduced by 4.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViBoolean memBattStat
 * IN        
 *            A boolean value indicating whether or not to enable the
 *           non-volatile memory.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1429_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1429_memBattStat(ViSession vi, ViBoolean memBattStat)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_memBattStat");

  hpe1429_CHK_BOOLEAN(memBattStat, VI_ERROR_PARAMETER2);

  errStatus = viPrintf(vi, "MEM:BATT:STAT %u\n", memBattStat);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_memBattStat_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function determines if the non-volatile memory is enabled
 *           or disabled. If the state is enabled (VI_TRUE), then measurement
 *           data will be preserved in the HP E1429A's internal memory when
 *           power fails or the instrument is turned off.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViPBoolean memBattStat
 * OUT       
 *            A boolean value indicating whether or not the non-volatile
 *            memory is enabled.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1429_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1429_memBattStat_Q(ViSession vi, ViPBoolean memBattStat)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_memBattStat_Q");

  thisPtr->blockSrqIO = VI_TRUE;
  errStatus = viPrintf(vi, "MEM:BATT:STAT?\n");
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  errStatus = viScanf(vi, "%hd%*t", memBattStat);
  if (thisPtr)
    doDelay(thisPtr->myDelay);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_recover_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function is used to obtain readings when the normal fetch
 *            commands will not work.  The readings obtained by this function
 *            are not scaled according to the specified range command.
 *            The data may be read any number of times.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 chan
 * IN        
 *            Specifies which channel for which to obtain data.
 *           
 *            The channels are defined as macros and are shown below:
 * 
 * PARAM 3 : ViInt32 arrayLength
 * IN        
 *            The length of the data array (number of elements).
 * 
 *      MAX = hpe1429_FETC_SIZE_MAX   524288
 *      MIN = hpe1429_FETC_SIZE_MIN   1
 * 
 * PARAM 4 : ViInt16 _VI_FAR data[]
 * OUT       
 *            Pointer to the array of 16 bit integers which will hold the A/D
 *           reading values. Make sure it is large enough to hold the data.
 * 
 * PARAM 5 : ViPInt32 count
 * OUT       
 *            The actual number of readings that are recovered.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1429_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1429_recover_Q_chan_a[] = { "", "1", "2", 0 };

ViStatus _VI_FUNC hpe1429_recover_Q(ViSession vi,
				    ViInt16 chan,
				    ViInt32 arrayLength, ViInt16 _VI_FAR data[], ViPInt32 count)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_recover_Q");

  hpe1429_CHK_ENUM(chan, 2, VI_ERROR_PARAMETER2);
  hpe1429_CHK_LONG_RANGE(arrayLength, hpe1429_FETC_SIZE_MIN, hpe1429_FETC_SIZE_MAX,
			 VI_ERROR_PARAMETER3);

  {

    ViChar c[2];
    ViUInt32 digits;
    ViChar length_str[16];
    ViUInt32 nbytes;
    ViUInt32 retbytes;
    unsigned char src[2];
    ViInt32 i;

#ifdef WIN32
    ViPInt16 fptr = data;
#else
    ViInt16 _huge *fptr;
    fptr = (ViInt16 _huge *) data;
#endif

    thisPtr->blockSrqIO = VI_TRUE;

    errStatus = viPrintf(vi, "FORM PACK;:FETC%hd:REC?\n", chan);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    /*   errStatus = viScanf(vi, "%2c", c);  Read Header (should be #[1-9] */
    errStatus = viRead(vi, c, 2, &retbytes);	/* Read Header (should be #[1-9] */
    if (retbytes != 2) {
      errStatus = viScanf(vi, "%*t");	/* Clear input. */
      errStatus = VI_ERROR_INV_RESPONSE;
    }

    if (errStatus >= VI_SUCCESS) {
      digits = c[1] - '0';
      if ((digits < 1) || (9 < digits)) {
	errStatus = viScanf(vi, "%*t");	/* Clear input. */
	errStatus = VI_ERROR_INV_RESPONSE;
      }
    }

    if (errStatus >= VI_SUCCESS) {
      /* Scan DAB array count. */
      /*    errStatus = viScanf(vi, "%#c", &digits, length_str);  */
      errStatus = viRead(vi, length_str, digits, &retbytes);
      if (retbytes != digits) {
	errStatus = viScanf(vi, "%*t");	/* Clear input. */
	errStatus = VI_ERROR_INV_RESPONSE;
      }

      if (errStatus >= VI_SUCCESS) {
	length_str[digits] = '\0';	/* null terminate the string */
	nbytes = atol(length_str);

	/* Verify that caller's array is big enough. */
	if (((ViUInt32) arrayLength * 2) < nbytes) {
	  errStatus = viScanf(vi, "%*t");	/* Clear input. */
	  errStatus = VI_ERROR_PARAMETER3;	/* Caller's array too small. */
	}
	*count = (ViInt32) (nbytes / 2);	/* Tell caller actual size */
      }
    }

    if (errStatus >= VI_SUCCESS) {
      /*  sprintf(fmtStr,"%%%ldc%%*t", nbytes);
         errStatus = viScanf(vi, fmtStr, (unsigned char*)fptr); */
      errStatus = viRead(vi, (unsigned char *)fptr, nbytes, &retbytes);
      if (errStatus == VI_SUCCESS_MAX_CNT)
	errStatus = viScanf(vi, "%*t");	/* Clear input. */

#ifndef B_ENDIAN
      /* need byte swapping */
      if (errStatus >= VI_SUCCESS) {
	for (i = 0; i < (ViInt32) (nbytes / 2); i++) {
	  *((ViInt16 *) src) = *((ViInt16 *) (&fptr[i]));
	  ((unsigned char *)(&fptr[i]))[0] = ((unsigned char *)(src))[1];
	  ((unsigned char *)(&fptr[i]))[1] = ((unsigned char *)(src))[0];
	}
      }
#endif
    }
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_trigStarImm
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function will cause a reading to be taken
 *            immediately when the digitizer is in the wait-for-trigger state
 *           (ARM event has occurred), regardless of the selected trigger
 *           source. The selected trigger source remains unchanged.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1429_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1429_trigStarImm(ViSession vi)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_trigStarImm");

  {
    errStatus = viPrintf(vi, "TRIG\n");
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }
    thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_trigger
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function combines two instrument SCPI commands:
 *              :TRIGger[:STARt]:SOURce
 *              :TRIGger[:STARt]:TIMer[1]
 *            Together, these two commands allow you to configure the trigger
 *            subsystem to respond to a specified source, and also specify
 *           the time period for timer1 (even if trigSour is not a timer
 *           mode).
 *            The TRIGger command subsystem controls the fourth state in a
 *           four state measurement process.  The four states which occur
 *           during a successful reading are idle, initiated, wait-for-arm,
 *           and wait-for-trigger.  The last two states have event detection
 *           associated with them which control when they exit the current
 *           state.  For a more detailed explaination of these states, please
 *           refer to the instrument manuals.
 *            In the wait-for-trigger state, the instrument
 *            waits for the specified trigger event to occur, and when it
 *           occurs, a reading is taken.  After a reading is taken, the
 *           cumulative number of readings taken thus far is compared to the
 *           count specified by numTriggers in hpe1429_configure().  When the
 *           count is met, the state is exited, otherwise, the instrument
 *           waits for another trigger and takes another reading.  Upon exit
 *           from this state, the instrument goes to the initiated state and
 *           checks to see whether or not the number of arming cycles as
 *           specified by numArms in hpe1429_configure() is satisfied.
 *            The trigSour parameter configures the trigger
 *            system to respond to the specified source for taking readings.
 *           
 *           The available sources are:
 *              Bus: The Group Execute Trigger (GET) HP-IB command.
 *           
 *              ECLTrg0 and ECLTrg1: The VXIbus ECL trigger lines.
 *           
 *              Dual ECL Trig: Samples are taken at a dual rate, using the
 *                   VXIbus ECLTRG0 trigger line to pace pre-arm readings,
 *                   and the ECLTRG1 trigger line to pace the post-arm
 *                   readings.
 *           
 *              Dual External: Samples are taken at a dual rate, using the
 *                   signal on the Ext 1 input to pace pre-arm readings, and
 *                   the signal on the Ext 2 input to pace the post-arm
 *                   readings.
 *           
 *              Dual Timer: Samples are taken at a dual rate, using
 *                   trigTimer1 to pace pre-arm readings, and the
 *                   TRIGger:STARt:TIMer2 rate to pace the post-arm readings.
 *           
 *              TTLTrg0 through TTLTrg7: The VXIbus TTL trigger lines.
 *           
 *              External 1: The HP E1429's front panel "Ext 1" BNC connector.
 *           
 *              External 2: The HP E1429's front panel "Ext 2" BNC connector.
 *           
 *              Hold: Suspend triggering.  The hpe1429_trigStarImm() function
 *                    must be used to trigger a reading.
 *           
 *              Timer: Use the period specified by trigTimer1 as the sample
 *                     rate.
 *           
 *              VME: This source is used to either trigger readings from VME
 *                   A24 register accesses, or to read measurement data out
 *                   of the HP E1429 internal memory by reading a VME A24
 *                   address.  This allows for faster data transfer rates
 *                   over the VME bus than would be possible over HPIB.  For
 *                   more information on how to transfer data out over the
 *                   VME bus using this function, see the VINStrument:VME
 *                   commands.
 *           
 *            The trigTimer1 parameter specifies the time
 *            period between each sampling event.  The time period must be a
 *           multiple of the reference oscillator period, with allowable
 *           multiples being 1,2,4,10,20,40,........1E8, 2E8, 4E8.
 *           
 *            If trigSour is DTIM, then both trigTimer1 and
 *           TRIGger:STARt:TIMer2 are used, and there is a coupling between
 *           the two settings.  The relationship between the two settings is
 *           such that one of these two values must be exactly one reference
 *           oscillator period and the other must be a multiple (greater than
 *           1) of the reference oscillator period.  A record is kept of
 *           which setting was changed the most recently, and that setting is
 *           assumed to be the desired setting.  Since one of these timers
 *           must be exactly one reference oscillator period, only trigTimer1
 *           is provided by this driver to avoid confusion. Note that it is
 *           only necessary to set the longest sample rate. You may use the
 *           "pass-through" functions if you desire to set the second timer
 *           to something other than 1 or 2 reference oscillator periods.
 *            If dual rate sampling is enabled,
 *            then the sample period specified by trigTimer1 will be the
 *           sample rate for the pre-arm readings of the dual rate
 *           measurement, and TRIGger:STARt:TIMer2 will be the post-arm
 *           sample rate.
 *            If the E1429 can not sample within 1 percent of the
 *            period specified by trigTimer1, then the TIME bit (bit 2)
 *            in the QUEStionable Status register is set.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 trigSour
 * IN        
 *            This parameter configures the trigger system to respond to the
 *            specified source for taking readings.
 *           
 *            The types are defined as macros and are shown below:
 * 
 * PARAM 3 : ViReal64 trigTimer1
 * IN        
 *            This parameter specifies the time period between each sampling
 *           event. The time period must be a multiple of the reference
 *           oscillator period, with allowable multiples being
 *           1,2,4,10,20,40,........1E8, 2E8, 4E8.
 * 
 *      MAX = hpe1429_TRIG_TIM_MAX   40000
 *      MIN = hpe1429_TRIG_TIM_MIN   0.00000005
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1429_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1429_trigger_trigSour_a[] = { "BUS", "HOLD",
  "ECLT0", "ECLT1", "TTLT0", "TTLT1", "TTLT2", "TTLT3", "TTLT4", "TTLT5",
  "TTLT6", "TTLT7", "EXT1", "EXT2", "TIM", "DEXT", "DECL", "DTIM", "VME", 0
};

ViStatus _VI_FUNC hpe1429_trigger(ViSession vi, ViInt16 trigSour, ViPReal64 trigTimer1)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_trigger");

  hpe1429_CHK_ENUM(trigSour, 18, VI_ERROR_PARAMETER2);
  hpe1429_CHK_REAL_RANGE(*trigTimer1, hpe1429_TRIG_TIM_MIN, hpe1429_TRIG_TIM_MAX,
			 VI_ERROR_PARAMETER3);

  errStatus =
      viPrintf(vi, "TRIG:SOUR %s;TIM %g\n", hpe1429_trigger_trigSour_a[trigSour], *trigTimer1);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_trigger_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This routine queries the trigger source and timer1 in a single
 *           call.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViPInt16 trigSour
 * OUT       
 *            This returns an integer representing the current source that
 *            the trigger system will use for taking readings.
 *           
 *            The types are defined as macros and are shown below:
 * 
 * PARAM 3 : ViPReal64 trigTimer1
 * OUT       
 *            This returns a value indicating the time period between each
 *           sampling event when trigSour is hpe1429_TRIG_TIM.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1429_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1429_trigger_Q_trigSour_a[] = { "BUS", "HOLD",
  "ECLT0", "ECLT1", "TTLT0", "TTLT1", "TTLT2", "TTLT3", "TTLT4", "TTLT5",
  "TTLT6", "TTLT7", "EXT1", "EXT2", "TIM", "DEXT", "DECL", "DTIM", "VME", 0
};

ViStatus _VI_FUNC hpe1429_trigger_Q(ViSession vi, ViPInt16 trigSour, ViPReal64 trigTimer1)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;
  char trigSour_str[32];

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_trigger_Q");

  {
    /* ViChar trigSour_str[32]; Don't need to declare -- (Autoscript does it) */
    thisPtr->blockSrqIO = VI_TRUE;
    errStatus = viPrintf(vi, "TRIG:SOUR?\n");
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }
    errStatus = viScanf(vi, "%s%*t", trigSour_str);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = viPrintf(vi, "TRIG:TIM? \n");
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }
    errStatus = viScanf(vi, "%lg%*t", trigTimer1);
    if (thisPtr)
      doDelay(thisPtr->myDelay);
    if (errStatus < VI_SUCCESS) {
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = hpe1429_findIndex(thisPtr, hpe1429_trigger_Q_trigSour_a, trigSour_str, trigSour);
    if (errStatus < VI_SUCCESS)
      hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
    thisPtr->controler |= 8192;	/* Flag LOG_STATUS to not clear bits */
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1429_vinsConfLbusRes
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function will reset the digitizer's Local bus chip.
 *            This function should be used when it is necessary to put the
 *            Local bus chip into a known state without altering any other
 *            digitizer settings.  The HP E1429B Local bus chip must be reset
 *           after each data transfer. When resetting the Local bus chip, the
 *           Local bus chips on all devices to the right of the HP E1429B
 *           must also be reset in a left-to-right sequence. Refer to the
 *           product documentation for information on how a particular
 *           device's Local bus chip is reset.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1429_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1429_vinsConfLbusRes(ViSession vi)
{
  ViStatus errStatus = 0;
  struct hpe1429_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1429_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1429_CDE_INIT("hpe1429_vinsConfLbusRes");

  errStatus = viPrintf(vi, "VINS:LBUS:RES\n");
  if (errStatus < VI_SUCCESS) {
    hpe1429_LOG_STATUS(vi, thisPtr, errStatus);
  }

  hpe1429_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}
