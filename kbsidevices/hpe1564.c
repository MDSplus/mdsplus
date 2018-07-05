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
/* ONLY one of these can be defined */

#define REGB_DRIVER

#if defined(REGB_DRIVER) && defined(MSGB_DRIVER)
#error Must define only ONE of REGB_DRIVER or MSGB_DRIVER
#endif

#if !defined(REGB_DRIVER) && !defined(MSGB_DRIVER)
#error MUST have either REGB_DRIVER or MSGB_DRIVER defined!
#endif

/* _HPUX_SOURCE and _XOPEN_SOURCE need to be defined before any standard
   include if they are to work correctly
   They provide the path through the #ifdefs in standard include files
   that extends the ANSI C ifdefs as needed for driver writing.
*/
#ifdef __hpux
#define _HPUX_SOURCE
#define _XOPEN_SOURCE
#endif

#include <stdlib.h>		/* prototype for malloc() */
#include <string.h>		/* prototype for strcpy() */
#include <stdio.h>		/* prototype for sprintf() */
#include <stdarg.h>		/* variable arg list for tracing */
#include <sys/timeb.h>		/* prototype for ftime() */
#include "visa.h"

#if defined _WIN32 || defined __WIN32__
#include <windows.h>
#define hpe1564_DECLARE_TIME_LOCALS        SYSTEMTIME st;
#define hpe1564_GET_TIME           GetLocalTime(&st);
#define hpe1564_TIME_FIELDS        st.wMonth, st.wDay, st.wHour, \
				st.wMinute, st.wSecond, st.wMilliseconds
#else				/* not win32 */
#include <time.h>		/* standard time functions */
#ifdef __hpux
#define DWORD int
#include <sys/time.h>
#define hpe1564_DECLARE_TIME_LOCALS        struct timeval tv; \
				struct timezone tz; \
				struct tm *tmp;
#define hpe1564_GET_TIME           gettimeofday(&tv, &tz); \
				tmp = localtime((time_t*)&tv.tv_sec);
#define hpe1564_TIME_FIELDS        tmp->tm_mon+1, tmp->tm_mday, tmp->tm_hour, \
				tmp->tm_min, tmp->tm_sec, tv.tv_usec/1000

#else				/* not unix, use ANSI time function */

#define hpe1564_DECLARE_TIME_LOCALS        struct tm *tmp; time_t seconds;
#define hpe1564_GET_TIME           time(&seconds); \
				tmp = localtime(&seconds);
#define hpe1564_TIME_FIELDS        tmp->tm_mon+1, tmp->tm_mday, tmp->tm_hour, \
				tmp->tm_min, tmp->tm_sec, 0
#endif				/* ifdef __hpux  */

#endif				/* ifdef _WIN32 */

#define INSTR_CALLBACKS		/* needed for handler prototypes in hpe1564.h */

#undef _VI_FUNC
#define _VI_FUNC
#include "hpe1564.h"

#include "core.h"

#define hpe1564_MODEL2_CODE 614	/* 2 channel DAC */
#define hpe1564_MODEL4_CODE 615	/* 4 channel DAC */

#define hpe1564_MANF_ID     4095
#define hpe1564_IDN_STRING4  "HEWLETT-PACKARD,E1564A"
#define hpe1564_IDN_STRING2  "HEWLETT-PACKARD,E1563A"

#define hpe1564_REV_CODE "A.02.02"	/* Driver Revision */

#define hpe1564_ERR_MSG_LENGTH 256	/* size of error message buffer */

#define hpe1564_MAX_STAT_REG 4	/* number of IEEE 488.2 status registers */

/* this has to match the index of the ESR register in hpe1564_accessInfo[] */
#define hpe1564_ESR_REG_IDX 2

/******************************************************
 * Callback events
 ******************************************************
 */

ViInt32 hpe1564_statusHap[hpe1564_MAX_STAT_HAP] = {
  hpe1564_USER_ERROR_HANDLER,
  hpe1564_INSTR_ERROR_HANDLER,

  hpe1564_VOLT_OVLD,
  hpe1564_LIM1_FAIL,
  hpe1564_LIM2_FAIL,
  hpe1564_LIM3_FAIL,
  hpe1564_LIM4_FAIL,

  hpe1564_GOT_TRIG,
  hpe1564_PRETRIG_DONE,
  hpe1564_MEAS_DONE
};

/* Assumes we have driver copies of the event register.  This is needed
 * because in IEEE 488.2, the event register are cleared after they are 
 * read.  Since the event register contains several events, we need to
 * keep this information around to pass back to the user.
 */
struct hpe1564_statusAccess {
  ViInt32 registerIdx;
  ViString condQry;
  ViString eventQry;
  ViString enableCmd;
};

const struct hpe1564_statusAccess hpe1564_accessInfo[hpe1564_MAX_STAT_REG] = {
  {0, "", "*STB?", "*SRE"},
  {400, "STAT:QUES:COND?", "STAT:QUES:EVEN?", "STAT:QUES:ENAB"},
  {600, "", "*ESR?", "*ESE"},
  {800, "STAT:OPER:COND?", "STAT:OPER:EVEN?", "STAT:OPER:ENAB"}
};

/* this will return the index associated with the happening */
ViBoolean hpe1564_findHappeningIdx(ViInt32 happening, ViPInt32 pIdx)
{
  /* Note: this is a linear search, for faster access this
   * could be done as a binary search since the data is arrange
   * in order numerically.
   */
  for (*pIdx = 0; *pIdx < hpe1564_MAX_STAT_HAP; *pIdx = *pIdx + 1) {
    if (hpe1564_statusHap[*pIdx] == happening) {
      return VI_TRUE;
    }
  }

  return VI_FALSE;
}

/* this will return the index that corresponds with regNum */
static ViBoolean hpe1564_findAccessIdx(ViInt32 regNum, ViPInt32 pIdx)
{
  for (*pIdx = 0; *pIdx < hpe1564_MAX_STAT_REG; *pIdx = *pIdx + 1) {
    if (regNum == hpe1564_accessInfo[*pIdx].registerIdx) {
      return VI_TRUE;
    }
  }
  return VI_FALSE;
}

/* you can remove the SWAP's below if you do not have block IO (arbitrary block)
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
      *((ViInt16 *)src) = *((ViInt16 *)dest);                        \
      ((unsigned char *)(dest))[0] =  ((unsigned char*)(src))[1];  \
      ((unsigned char *)(dest))[1] =  ((unsigned char*)(src))[0];  \
 }
/*=============================================================== 
 *
 *  All messages are stored in this area to aid in localization 
 *
 *=============================================================== 
 */

#define hpe1564_MSG_VI_OPEN_ERR 				\
	"vi was zero.  Was the hpe1564_init() successful?"

#define hpe1564_MSG_CONDITION					\
	"condition"
	/* hpe1564_statCond_Q() */

#define hpe1564_MSG_EVENT						\
	"event"
	/* hpe1564_statEvent_Q() */

#define hpe1564_MSG_EVENT_HDLR_INSTALLED				\
	"event handler is already installed for event happening"
	/* hpe1564_statEvent_Q() */

#define hpe1564_MSG_EVENT_HDLR_INST2				\
	"Only 1 handler can be installed at a time."
	/* hpe1564_statEvent_Q() */

#define hpe1564_MSG_INVALID_HAPPENING				\
	"is not a valid happening."
	/* hpe1564_statCond_Q() */
	/* hpe1564_statEven_Q() */
	/* hpe1564_statEvenHdlr() */
	/* hpe1564_statEvenHdlr_Q() */

#define hpe1564_MSG_NOT_QUERIABLE					\
	"is not queriable."
	/* hpe1564_statCond_Q() */
	/* hpe1564_statEven_Q() */

#define hpe1564_MSG_IN_FUNCTION					\
	"in function"
	/* hpe1564_error_message() */

#define hpe1564_MSG_INVALID_STATUS					\
  	"Parameter 2 is invalid"				\
	"in function hpe1564_error_message()."
	/* hpe1564_error_message() */

#define hpe1564_MSG_INVALID_STATUS_VALUE				\
	"is not a valid viStatus value."
	/* hpe1564_error_message() */

#define  hpe1564_MSG_INVALID_VI					\
  	"Parameter 1 is invalid"				\
	" in function hpe1564_error_message()"			\
	".  Using an inactive ViSession may cause this error."	\
	"  Was the instrument driver closed prematurely?"
	/* hpe1564_message_query() */

#define hpe1564_MSG_NO_ERRORS					\
	"No Errors"
	/* hpe1564_error_message() */

#define hpe1564_MSG_SELF_TEST_FAILED 				\
	"Self test failed."
	/* hpe1564_self_test() */

#define hpe1564_MSG_SELF_TEST_PASSED 				\
	"Self test passed."
	/* hpe1564_self_test() */

#define hpe1564_MSG_WARN_SELF_TEST 				\
	"Power-on self test failed, run full self test."
	/* hpe1564_self_test() */

/* the following messages are used by the functions to check parameters */

#define hpe1564_MSG_BOOLEAN   "Expected 0 or 1; Got %d"

#define hpe1564_MSG_REAL   "Expected %lg to %lg; Got %lg"

#define hpe1564_MSG_INT   "Expected %hd to %hd; Got %hd"

#define hpe1564_MSG_LONG   "Expected %ld to %ld; Got %ld"

#define hpe1564_MSG_LOOKUP "Error converting string response to integer"

#define hpe1564_MSG_NO_MATCH "Could not match string %s"

/* 
 * static error message 
 */

#define VI_ERROR_PARAMETER1_MSG				\
	"Parameter 1 is invalid"

#define VI_ERROR_PARAMETER2_MSG				\
	"Parameter 2 is invalid"

#define VI_ERROR_PARAMETER3_MSG				\
	"Parameter 3 is invalid"

#define VI_ERROR_PARAMETER4_MSG				\
	"Parameter 4 is invalid"

#define VI_ERROR_PARAMETER5_MSG				\
	"Parameter 5 is invalid"

#define VI_ERROR_PARAMETER6_MSG				\
	"Parameter 6 is invalid"

#define VI_ERROR_PARAMETER7_MSG				\
	"Parameter 7 is invalid"

#define VI_ERROR_PARAMETER8_MSG				\
	"Parameter 8 is invalid"

#define VI_ERROR_PARAMETER9_MSG				\
	"Parameter 9 is invalid"

#define VI_ERROR_PARAMETER10_MSG			\
	"Parameter 10 is invalid"

#define VI_ERROR_PARAMETER11_MSG			\
	"Parameter 11 is invalid"

#define VI_ERROR_PARAMETER12_MSG			\
	"Parameter 12 is invalid"

#define VI_ERROR_PARAMETER13_MSG			\
	"Parameter 13 is invalid"

#define VI_ERROR_PARAMETER14_MSG			\
	"Parameter 14 is invalid"

#define VI_ERROR_PARAMETER15_MSG			\
	"Parameter 15 is invalid"

#define VI_ERROR_PARAMETER16_MSG			\
	"Parameter 16 is invalid"

#define VI_ERROR_PARAMETER17_MSG			\
	"Parameter 17 is invalid"

#define VI_ERROR_PARAMETER18_MSG			\
	"Parameter 18 is invalid"

#define VI_ERROR_FAIL_ID_QUERY_MSG				\
	"Instrument IDN does not match."

#define INSTR_ERROR_INV_SESSION_MSG 				\
	"ViSession (parmeter 1) was not created by this driver"

#define INSTR_ERROR_NULL_PTR_MSG				\
	"NULL pointer detected"

#define INSTR_ERROR_RESET_FAILED_MSG				\
	"reset failed"

#define INSTR_ERROR_UNEXPECTED_MSG 				\
	"An unexpected error occurred"

#define INSTR_ERROR_DETECTED_MSG			\
	"Instrument Error Detected, call hpe1564_error_query()."

#define INSTR_ERROR_NOTFOUND_MSG			\
	"Instrument was not found."

#define INSTR_ERROR_MULTIPLE_MSG			\
	"Multiple Instruments were found, none were opened"

#define INSTR_ERROR_A16MAP_FAILED_MSG				\
	"reset failed"

#define INSTR_ERROR_LOOKUP_MSG   				\
	"String not found in table"

#define INSTR_ERROR_SAMPTTL_MSG   				\
	"output of SAMP disabled, sample source now using TTL trig line "

#define INSTR_ERROR_MASTER_TRIGTTL_MSG   				\
	"Trig source 2 conflicts with MASTER|SLAVE mode; trig source 2 set to HOLD"

#define INSTR_ERROR_MASTER_SAMPTTL_MSG   				\
	"Samp src TTLT<n> conflicts with trig mode MASTER<n>; setting samp src TIMer"

#define INSTR_ERROR_SLAVE_SAMPLE_MSG   				\
	"Sample source changes not allowed while trig mode is SLAVE<n>"

#define INSTR_ERROR_MASTER_OUTP_MSG   				\
	"Output state ON not allowed while trig mode is MASTER<n>|SLAVE<n>"

#define INSTR_ERROR_TRIGTTL_MSG   				\
	"output of TRIG disabled, trig source now using TTL trig line "

#define INSTR_ERROR_LEVMIN_MSG   				\
	"Trig or Limit level < MIN, setting to MIN."

#define INSTR_ERROR_LEVMAX_MSG   				\
	"Trig or Limit level > MAX, setting to MAX."

#define INSTR_ERROR_TRIGLIM_MSG   				\
	"Trig level/Limit check conflict, limit check disabled"

#define INSTR_ERROR_LIMTRIG_MSG   				\
	"Trig level/Limit check conflict, trig source set to IMMediate"

#define INSTR_ERROR_PRETRIG_MSG   				\
	"Pretrigger count >= total count; setting pretrigger to total - 1"

#define INSTR_ERROR_TRIG_IGN_MSG   				\
	"Trigger ignored."

#define INSTR_ERROR_SAMP_IGN_MSG   				\
	"Sample trigger ignored."

#define INSTR_ERROR_INIT_IGN_MSG   				\
	"Illegal while initiated"

#define INSTR_ERROR_CAL_OFF_MSG   				\
	"Calibration not enabled, see hpe1564_calState"

#define INSTR_ERROR_BAD_CAL_VAL_MSG			\
	"Calibration value not between 85% and 98% of full scale"

#define INSTR_ERROR_FLASH_PROG_MSG			\
	"Flash programming error; switches set incorrectly?"

#define INSTR_ERROR_FLASH_VPP_MSG			\
	"Flash VPP low error; switches set incorrectly?"

#define INSTR_ERROR_FLASH_ERASE_MSG			\
	"Error erasing flash; store aborted"

#define INSTR_ERROR_CAL_STALE_MSG			\
	"Calibration constants not modified; store aborted"

#define INSTR_ERROR_CAL_STORE_MSG			\
	"Calibration store unsuccessful; stored value incorrect"

#define INSTR_ERROR_CAL_NOT_STORED_MSG      \
	"Must STORE data before exiting Calibration state"

#define INSTR_ERROR_CAL_NO_CONVERGE_MSG      \
	"Calibration not converging; exiting"

#define INSTR_ERROR_BIG_VOLT_MSG      \
        "Overloads occurring, reduce signal level."

#define INSTR_ERROR_TRIG_DEAD_MSG 				\
	"Trigger source deadlock detected, data fetch aborted"

#define INSTR_ERROR_SAMP_DEAD_MSG 				\
	"Sample source deadlock detected, data fetch aborted"

#define INSTR_ERROR_CAL_GAIN_AUTO_MSG	\
	"Error generating gain constant"

#define INSTR_ERROR_CAL_OFFS_AUTO_MSG	\
	"Error generating filter offset"

#define INSTR_ERROR_OVLD_MSG	\
	"Overload detected; Attempting re-connect of input relays"

#define INSTR_ERROR_OVLD_DATA_MSG	\
	"Overload detected; data is questionable"

/*================================================================*/

/* don't check the debug pointer all the time!*/
#ifdef DEBUG
#define hpe1564_DEBUG_CHK_THIS( vi, thisPtr) 			\
	/* check for NULL user data */				\
	if( 0 == thisPtr)					\
	{							\
 		hpe1564_LOG_STATUS(                             	\
		  vi, NULL, hpe1564_INSTR_ERROR_INV_SESSION );	\
	}							\
	{							\
		ViSession defRM;				\
								\
		/* This should never fail */			\
		errStatus = viGetAttribute( vi,                 \
			VI_ATTR_RM_SESSION, &defRM);		\
		if( VI_SUCCESS > errStatus )			\
		{						\
 			hpe1564_LOG_STATUS(				\
			  vi, NULL, hpe1564_INSTR_ERROR_UNEXPECTED );	\
		}						\
		if( defRM != thisPtr->defRMSession)		\
		{						\
 			hpe1564_LOG_STATUS(				\
			  vi, NULL, hpe1564_INSTR_ERROR_INV_SESSION );	\
		}						\
	}
#else
#define hpe1564_DEBUG_CHK_THIS( vi, thisPtr)
#endif

/* add the following to the globals data structure */

/* Note: for debugging, you can add __FILE__, __LINE__ as parameters
 * to hpe1564_LOG_STATUS, and ViString filename, and ViInt32 lineNumber to
 * hpe1564_statusUpdate() in order to determine exactly where an error
 * occured in a driver.
 */
#define hpe1564_LOG_STATUS( vi, thisPtr, status ) 	\
  return hpe1564_statusUpdate( vi, thisPtr, status)

/* declare this here since it is called by statusUpdate */
static void hpe1564_srqTraverse(ViSession vi, ViInt32 eventReg);

ViStatus hpe1564_statusUpdate(ViSession vi, hpe1564_globals * thisPtr, ViStatus s)
{
  ViInt32 eventQ;
  ViUInt32 rc;
  char lc[20];
  ViStatus errStatus;

  if (!thisPtr)
    return s;

  /* This is only done, if the vi is valid and 
   * no other errors have occured.
   */
  if (s == VI_ERROR_TMO) {
    thisPtr->errNumber = s;
    return s;
  }

  /* Check if user wants to query the instrument state. */
  if (thisPtr && thisPtr->errQueryDetect) {
    /* assume IEEE 488.2 Instrument and query standard
     * status event register for a parser error
     */
    if (thisPtr->e1406) {
      errStatus = viWrite(vi, (ViBuf) "*ESR?", 5, &rc);
      if (errStatus < VI_SUCCESS)
	return VI_ERROR_SYSTEM_ERROR;

      errStatus = viRead(vi, (ViPBuf) lc, 20, &rc);
      if (errStatus < VI_SUCCESS)
	return VI_ERROR_SYSTEM_ERROR;

      eventQ = atoi(lc);

      if ((0x04			/* Query Error */
	   | 0x08		/* Device Dependent Error */
	   | 0x10		/* Execution Error */
	   | 0x20		/* Command Error */
	  ) & eventQ) {
	/*  See if the user has an instr error handler enabled */
	if (thisPtr->eventHandlerArray[hpe1564_INSTR_ERROR_HANDLER_IDX].eventHandler) {
	  /* call the users handler */
	  thisPtr->eventHandlerArray[hpe1564_INSTR_ERROR_HANDLER_IDX].eventHandler(vi, (ViInt32) s,
										   thisPtr->
										   eventHandlerArray
										   [hpe1564_INSTR_ERROR_HANDLER_IDX].
										   userData);
	}

	return hpe1564_INSTR_ERROR_DETECTED;
      }
    }
  }

  /* if a plug and play error occurs, see if the user has a handler enabled */
  if (s != hpe1564_INSTR_ERROR_DETECTED &&
      VI_SUCCESS > s && thisPtr->eventHandlerArray[hpe1564_USER_ERROR_HANDLER_IDX].eventHandler) {
    /* call the users handler */
    thisPtr->eventHandlerArray[hpe1564_USER_ERROR_HANDLER_IDX].eventHandler(vi, (ViInt32) s,
									    thisPtr->
									    eventHandlerArray
									    [hpe1564_USER_ERROR_HANDLER_IDX].
									    userData);
  }

  thisPtr->errNumber = s;

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
  {hpe1564_INSTR_ERROR_PARAMETER9, VI_ERROR_PARAMETER9_MSG},
  {hpe1564_INSTR_ERROR_PARAMETER10, VI_ERROR_PARAMETER10_MSG},
  {hpe1564_INSTR_ERROR_PARAMETER11, VI_ERROR_PARAMETER11_MSG},
  {hpe1564_INSTR_ERROR_PARAMETER12, VI_ERROR_PARAMETER12_MSG},
  {hpe1564_INSTR_ERROR_PARAMETER13, VI_ERROR_PARAMETER13_MSG},
  {hpe1564_INSTR_ERROR_PARAMETER14, VI_ERROR_PARAMETER14_MSG},
  {hpe1564_INSTR_ERROR_PARAMETER15, VI_ERROR_PARAMETER15_MSG},
  {hpe1564_INSTR_ERROR_PARAMETER16, VI_ERROR_PARAMETER16_MSG},
  {hpe1564_INSTR_ERROR_PARAMETER17, VI_ERROR_PARAMETER17_MSG},
  {hpe1564_INSTR_ERROR_PARAMETER18, VI_ERROR_PARAMETER18_MSG},
  {VI_ERROR_FAIL_ID_QUERY, VI_ERROR_FAIL_ID_QUERY_MSG},

  {hpe1564_INSTR_ERROR_INV_SESSION, INSTR_ERROR_INV_SESSION_MSG},
  {hpe1564_INSTR_ERROR_NULL_PTR, INSTR_ERROR_NULL_PTR_MSG},
  {hpe1564_INSTR_ERROR_RESET_FAILED, INSTR_ERROR_RESET_FAILED_MSG},
  {hpe1564_INSTR_ERROR_UNEXPECTED, INSTR_ERROR_UNEXPECTED_MSG},
  {hpe1564_INSTR_ERROR_DETECTED, INSTR_ERROR_DETECTED_MSG},
  {hpe1564_ERROR_NOTFOUND, INSTR_ERROR_NOTFOUND_MSG},
  {hpe1564_ERROR_MULTIPLE, INSTR_ERROR_MULTIPLE_MSG},
  {hpe1564_INSTR_ERROR_A16MAP_FAILED, INSTR_ERROR_A16MAP_FAILED_MSG},
  {hpe1564_INSTR_ERROR_LOOKUP, INSTR_ERROR_LOOKUP_MSG},
  {hpe1564_ERROR_SAMPTTL, INSTR_ERROR_SAMPTTL_MSG},
  {hpe1564_ERROR_TRIGTTL, INSTR_ERROR_TRIGTTL_MSG},
  {hpe1564_ERROR_LEVMIN, INSTR_ERROR_LEVMIN_MSG},
  {hpe1564_ERROR_LEVMAX, INSTR_ERROR_LEVMAX_MSG},
  {hpe1564_ERROR_TRIGLIM, INSTR_ERROR_TRIGLIM_MSG},
  {hpe1564_ERROR_PRETRIG, INSTR_ERROR_PRETRIG_MSG},
  {hpe1564_ERROR_TRIG_IGN, INSTR_ERROR_TRIG_IGN_MSG},
  {hpe1564_ERROR_SAMP_IGN, INSTR_ERROR_SAMP_IGN_MSG},
  {hpe1564_ERROR_INIT_IGN, INSTR_ERROR_INIT_IGN_MSG},
  {hpe1564_ERROR_CAL_OFF, INSTR_ERROR_CAL_OFF_MSG},
  {hpe1564_ERROR_BAD_CAL_VAL, INSTR_ERROR_BAD_CAL_VAL_MSG},
  {hpe1564_ERROR_FLASH_PROG, INSTR_ERROR_FLASH_PROG_MSG},
  {hpe1564_ERROR_FLASH_VPP, INSTR_ERROR_FLASH_VPP_MSG},
  {hpe1564_ERROR_FLASH_ERASE, INSTR_ERROR_FLASH_ERASE_MSG},
  {hpe1564_ERROR_CAL_STALE, INSTR_ERROR_CAL_STALE_MSG},
  {hpe1564_ERROR_CAL_STORE, INSTR_ERROR_CAL_STORE_MSG},
  {hpe1564_ERROR_CAL_NOT_STORED, INSTR_ERROR_CAL_NOT_STORED_MSG},
  {hpe1564_ERROR_CAL_NO_CONVERGE, INSTR_ERROR_CAL_NO_CONVERGE_MSG},
  {hpe1564_ERROR_BIG_VOLT, INSTR_ERROR_BIG_VOLT_MSG},
  {hpe1564_ERROR_LIMTRIG, INSTR_ERROR_LIMTRIG_MSG},
  {hpe1564_ERROR_TRIG_DEAD, INSTR_ERROR_TRIG_DEAD_MSG},
  {hpe1564_ERROR_SAMP_DEAD, INSTR_ERROR_SAMP_DEAD_MSG},
  {hpe1564_ERROR_SELF_TEST, hpe1564_MSG_SELF_TEST_FAILED},
  {hpe1564_ERROR_CAL_GAIN_AUTO, INSTR_ERROR_CAL_GAIN_AUTO_MSG},
  {hpe1564_ERROR_CAL_OFFS_AUTO, INSTR_ERROR_CAL_OFFS_AUTO_MSG},
  {hpe1564_ERROR_OVLD, INSTR_ERROR_OVLD_MSG},
  {hpe1564_ERROR_OVLD_DATA, INSTR_ERROR_OVLD_DATA_MSG},
  {hpe1564_ERROR_MASTER_TRIGTTL, INSTR_ERROR_MASTER_TRIGTTL_MSG},
  {hpe1564_ERROR_MASTER_SAMPTTL, INSTR_ERROR_MASTER_SAMPTTL_MSG},
  {hpe1564_ERROR_SLAVE_SAMPLE, INSTR_ERROR_SLAVE_SAMPLE_MSG},
  {hpe1564_ERROR_MASTER_OUTP, INSTR_ERROR_MASTER_OUTP_MSG},
  {hpe1564_WARN_SELF_TEST, hpe1564_MSG_WARN_SELF_TEST},
};

/* macros for testing parameters */
#define hpe1564_CHK_BOOLEAN( my_val, err ) if( hpe1564_chk_boolean( thisPtr, my_val) ) hpe1564_LOG_STATUS( vi, thisPtr, err);

static ViBoolean hpe1564_chk_boolean(hpe1564_globals * thisPtr, ViBoolean my_val)
{
  char message[hpe1564_ERR_MSG_LENGTH];
  if ((my_val != VI_TRUE) && (my_val != VI_FALSE)) {
    /* true = parameter is invalid */
    sprintf(message, hpe1564_MSG_BOOLEAN, my_val);
    hpe1564_CDE_MESSAGE(message);
    /* true = parameter is invalid */
    return VI_TRUE;
  }

  /* false = okay */
  return VI_FALSE;
}

#define hpe1564_CHK_REAL_RANGE( my_val, min, max, err ) if( hpe1564_chk_real_range( thisPtr, my_val, min, max) ) hpe1564_LOG_STATUS( vi, thisPtr, err);

static ViBoolean hpe1564_chk_real_range(hpe1564_globals * thisPtr,
					ViReal64 my_val, ViReal64 min, ViReal64 max)
{
  char message[hpe1564_ERR_MSG_LENGTH];

  if ((my_val < min) || (my_val > max)) {
    sprintf(message, hpe1564_MSG_REAL, min, max, my_val);
    hpe1564_CDE_MESSAGE(message);
    /* true = parameter is invalid */
    return VI_TRUE;
  }
  return VI_FALSE;
}

#define hpe1564_CHK_INT_RANGE( my_val, min, max, err ) if( hpe1564_chk_int_range( thisPtr, my_val, min, max) ) hpe1564_LOG_STATUS( vi, thisPtr, err);

static ViBoolean hpe1564_chk_int_range(hpe1564_globals * thisPtr,
				       ViInt16 my_val, ViInt16 min, ViInt16 max)
{
  char message[hpe1564_ERR_MSG_LENGTH];

  if ((my_val < min) || (my_val > max)) {
    sprintf(message, hpe1564_MSG_INT, min, max, my_val);
    hpe1564_CDE_MESSAGE(message);
    /* true = parameter is invalid */
    return VI_TRUE;
  }
  return VI_FALSE;
}

#define hpe1564_CHK_LONG_RANGE( my_val, min, max, err ) if( hpe1564_chk_long_range( thisPtr, my_val, min, max) ) hpe1564_LOG_STATUS( vi, thisPtr, err);

static ViBoolean hpe1564_chk_long_range(hpe1564_globals * thisPtr,
					ViInt32 my_val, ViInt32 min, ViInt32 max)
{
  char message[hpe1564_ERR_MSG_LENGTH];

  if ((my_val < min) || (my_val > max)) {
    sprintf(message, hpe1564_MSG_LONG, min, max, my_val);
    hpe1564_CDE_MESSAGE(message);
    /* true = parameter is invalid */
    return VI_TRUE;
  }
  return VI_FALSE;
}

#define hpe1564_CHK_ENUM( my_val, limit, err ) if( hpe1564_chk_enum( thisPtr, my_val, limit) ) hpe1564_LOG_STATUS( vi, thisPtr, err);

/* utility routine which searches for a string in an array of strings. */
/* This is used by the CHK_ENUM macro */
static ViBoolean hpe1564_chk_enum(hpe1564_globals * thisPtr, ViInt16 my_val, ViInt16 limit)
{
  char message[hpe1564_ERR_MSG_LENGTH];

  if ((my_val < 0) || (my_val > limit)) {
    sprintf(message, hpe1564_MSG_INT, 0, limit, my_val);
    hpe1564_CDE_MESSAGE(message);
    /* true = parameter is invalid */
    return VI_TRUE;
  }

  return VI_FALSE;
}

/*  ==========================================================================  
     This function searches an array of strings for a specific string and     
     returns its index.  If successful, a VI_SUCCESS is returned, 
     else hpe1564_INSTR_ERROR_LOOKUP is returned.
    ======================================================================== */
ViStatus hpe1564_findIndex(hpe1564_globals * thisPtr, const char *const array_of_strings[],
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
  sprintf(message, hpe1564_MSG_NO_MATCH, search_str);
  hpe1564_CDE_MESSAGE(message);
  return hpe1564_INSTR_ERROR_LOOKUP;
}

/* returns the globals pointer */

#define GetGlobals(vi,thisPtr)\
{\
    errStatus = viGetAttribute( vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);\
    if( VI_SUCCESS > errStatus)\
        hpe1564_LOG_STATUS( vi, NULL, errStatus);\
}

void hpe1564_delay(long delayTime)
{
  struct timeb timebuffer;
  long t0;
  long t1;

  ftime(&timebuffer);
  t0 = timebuffer.time;
  t1 = (long)timebuffer.millitm;
  ftime(&timebuffer);
  while (((timebuffer.time - t0) * 1000 + ((long)timebuffer.millitm - t1)) < delayTime) {
    ftime(&timebuffer);
  }
}

#ifndef __hpux

long setDelay(double val)
{
  double slice;
  _int64_t count;

  if (!QueryPerformanceFrequency((LARGE_INTEGER *) & count)) {
    //hdw doesn't have high perfomance count so use getickcount
    slice = 1e-3;		//units for gettick count
  } else {
    slice = 1.0 / count;	//Seconds per tick
  }

  return (long)(val / slice) + 1;

}

void hpe1564_doDelay(long ticks)
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
  return val * 1E6;
}

void doDelay(long ticks)
{
  struct timeval t0, t1;
  gettimeofday(&t0, NULL);
  t0.tv_usec += ticks;
  t0.tv_sec += t0.tv_usec / 1000000;
  t0.tv_usec = t0.tv_usec % 1000000;
  gettimeofday(&t1, NULL);
  while (t1.tv_sec < t0.tv_sec)
    gettimeofday(&t1, NULL);
  if (t1.tv_sec >= t0.tv_sec)
    return;			/* get out quick if past delay time */
  while (t1.tv_usec < t0.tv_usec)
    gettimeofday(&t1, NULL);
  return;
}

#endif

/****************************************************************************
hpe1564_init
*****************************************************************************
    Parameter Name                                       Type    Direction
   ------------------------------------------------------------------------
  | InstrDesc                                         ViRsrc      IN
  |   ---------------------------------------------------------------------
  |  | The Instrument Description.
  |  |
  |  | Examples: VXI::5, GPIB-VXI::128::INSTR
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

ViStatus _VI_FUNC hpe1564_init(ViRsrc InstrDesc, ViBoolean id_query, ViBoolean do_reset,
			       ViPSession vi)
{
  hpe1564_globals *thisPtr = NULL;
  ViSession defRM = 0;
  ViStatus errStatus;
  ViUInt16 intf, num, primary, secondary;

  char gpibdesc[64];
  ViSession vi1406;
  char idn_buf[256];

  ViUInt16 manfId;
  ViUInt16 modelCode;

  ViAddr addr;			/* A16 base address */
  ViInt16 result;		/* power on self test result */
  ViUInt32 tempBlock[4];	/* used to check for D32 capability */

  *vi = VI_NULL;

  /* Find the Default Resource Manager */
  errStatus = viOpenDefaultRM(&defRM);
  if (VI_SUCCESS > errStatus) {
    /* Errors: VI_ERROR_SYSTEM_ERROR 
     *         VI_ERROR_ALLOC
     */
    hpe1564_LOG_STATUS(*vi, NULL, errStatus);
  }

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
    hpe1564_LOG_STATUS(*vi, NULL, errStatus);
  }

  /* get memory for instance specific globals */
  thisPtr = (hpe1564_globals *) malloc(sizeof(hpe1564_globals));
  if (0 == thisPtr) {
    viClose(defRM);		/* also closes vi session */
    *vi = VI_NULL;
    hpe1564_LOG_STATUS(*vi, NULL, VI_ERROR_ALLOC);
  }

  /* initialize memory area to 0 */
  {
    int i;
    char *p;

    p = (char *)thisPtr;
    for (i = 0; i < sizeof(hpe1564_globals); i++)
      p[i] = 0;
  }

  if (errStatus = viGetAttribute(*vi, VI_ATTR_INTF_TYPE, &intf)) {
    viClose(*vi);
    viClose(defRM);
    *vi = VI_NULL;
    hpe1564_LOG_STATUS(*vi, NULL, errStatus);
  }

  /* check for 1406 internal driver by reading *IDN? of 0 and
   *IDN? of secondary for this address*/
  if (intf == VI_INTF_GPIB_VXI) {
    viGetAttribute(*vi, VI_ATTR_INTF_NUM, &num);
    viGetAttribute(*vi, VI_ATTR_GPIB_PRIMARY_ADDR, &primary);
    sprintf(gpibdesc, "GPIB%hd::%hd::0", num, primary);
    errStatus = viOpen(defRM, gpibdesc, VI_NULL, VI_NULL, &vi1406);
    if (errStatus < VI_SUCCESS)
      goto gpib_check_done;

    /* try sending an IDN? to 1406 */
    viSetAttribute(vi1406, VI_ATTR_TMO_VALUE, 1000);
    errStatus = viClear(vi1406);
    if (errStatus < VI_SUCCESS) {
      viClose(vi1406);
      goto gpib_check_done;
    }

    errStatus = viPrintf(vi1406, "*IDN?\n");
    if (errStatus < VI_SUCCESS) {
      viClose(vi1406);
      goto gpib_check_done;
    }

    /* it worked, try getting response and check it */
    errStatus = viScanf(vi1406, "%t", idn_buf);
    hpe1564_delay(10L);		/* delay 10 ms */

    viClose(vi1406);
    if (errStatus < VI_SUCCESS || memcmp(idn_buf, "HEWLETT-PACKARD,E140", 20))
      goto gpib_check_done;

    /* Since we got here, we ARE an E140x */
    /* try sending an IDN? to the card drivers */

    /* these next two calls fail for GPIB-VXI mode. They were
       in the original template, but I am commenting them out
       here for now.

       viSetAttribute(*vi, VI_ATTR_TMO_VALUE, 1000); 
       viClear(*vi);
     */

    errStatus = viPrintf(*vi, "*IDN?\n");
    if (errStatus < VI_SUCCESS)
      goto gpib_check_done;

    /* it worked, try getting response */
    errStatus = viScanf(*vi, "%t", idn_buf);
    hpe1564_delay(10L);		/* delay 10 ms */

    if (errStatus < VI_SUCCESS)
      goto gpib_check_done;

    /* got an answer so switch vi to gpib mode */
    viGetAttribute(*vi, VI_ATTR_GPIB_SECONDARY_ADDR, &secondary);
    sprintf(gpibdesc, "GPIB%hd::%hd::%hd", num, primary, secondary);
    viClose(*vi);
    errStatus = viOpen(defRM, gpibdesc, VI_NULL, VI_NULL, vi);
    if (errStatus < VI_SUCCESS) {	/* this should never happen */
      viClose(defRM);
      *vi = VI_NULL;
      hpe1564_LOG_STATUS(*vi, NULL, errStatus);
    }
    intf = VI_INTF_GPIB;
  }

 gpib_check_done:

  /* associate memory with session, should not fail because
   *   session is valid; and attribute is defined, supported,
   *   and writable.
   */
  errStatus = viSetAttribute(*vi, VI_ATTR_USER_DATA, (ViAttrState) thisPtr);
  if (VI_SUCCESS > errStatus) {
    viClose(*vi);
    viClose(defRM);		/* also closes vi session */
    *vi = VI_NULL;
    hpe1564_LOG_STATUS(*vi, NULL, errStatus);
  }

  /* initialize instance globals */
  viGetAttribute(*vi, VI_ATTR_RM_SESSION, &thisPtr->defRMSession);
  thisPtr->errNumber = VI_SUCCESS;
  thisPtr->errFuncName[0] = 0;
  thisPtr->errMessage[0] = 0;
  thisPtr->errQueryDetect = VI_FALSE;
  thisPtr->a16Addr = 0;
  thisPtr->digState.dataFormat = hpe1564_FORM_ASCII;

  /* initialize the flags we will use later */
  thisPtr->e1406 = 0;
  thisPtr->gpibVxi = 0;
  thisPtr->D16_only = 0;

/* mumble -- when split driver, only need one of these 
	thisPtr->E1563 = 1;
	thisPtr->E1563 = 0;
*/

  /* this gets clock ticks per 100 usec, and store into globals */
  thisPtr->delay_100_usec = setDelay(100.0e-6);

  /* next line is true if we switched to GPIB above */
  if (intf == VI_INTF_GPIB) {
    thisPtr->e1406 = 1;
/* mumble -- add back in when separate drivers
		if( VI_TRUE == id_query ) 
    	{
*/
    if (viClear(*vi) < VI_SUCCESS ||
	viPrintf(*vi, "*IDN?\n") < VI_SUCCESS || viScanf(*vi, "%t", idn_buf) < VI_SUCCESS) {
      /* ignore any errors in hpe1564_close */
      hpe1564_close(*vi);
      *vi = VI_NULL;
      hpe1564_LOG_STATUS(*vi, NULL, VI_ERROR_FAIL_ID_QUERY);
    } else {			/* we got *IDN successfully */

      /* mumble -- alter for each driver? */
      /* check for a idn match */
      if (!strncmp(idn_buf, hpe1564_IDN_STRING2, strlen(hpe1564_IDN_STRING2))) {
	thisPtr->E1563 = 1;
	thisPtr->numChans = 2;
      } else if (!strncmp(idn_buf, hpe1564_IDN_STRING4, strlen(hpe1564_IDN_STRING4))) {
	thisPtr->E1563 = 0;
	thisPtr->numChans = 4;
      } else {
	/* ignore any errors in hpe1564_close */
	hpe1564_close(*vi);
	*vi = VI_NULL;
	hpe1564_LOG_STATUS(*vi, NULL, VI_ERROR_FAIL_ID_QUERY);
      }
    }
/* mumble -- add back in when split into two drivers 
    	}
*/

    if (VI_TRUE == do_reset) {
      /* call the reset function to reset the instrument */
      /* dcl did not work if VXI or GPIB-VXI */
      if (hpe1564_dcl(*vi) < VI_SUCCESS || hpe1564_reset(*vi) < VI_SUCCESS) {
	/* ignore any errors in hpe1564_close */
	hpe1564_close(*vi);
	*vi = VI_NULL;
	hpe1564_LOG_STATUS(*vi, NULL, hpe1564_INSTR_ERROR_RESET_FAILED);
      }
    }

    /* set data format to packed */
    errStatus = viPrintf(*vi, "FORM:DATA PACK\n");
    if (errStatus < VI_SUCCESS) {
      /* ignore any errors in hpe1564_close */
      hpe1564_close(*vi);
      *vi = VI_NULL;
      hpe1564_LOG_STATUS(*vi, NULL, hpe1564_INSTR_ERROR_RESET_FAILED);
    }
    thisPtr->digState.dataFormat = hpe1564_FORM_PACK;

    /* specify motorola format */
    thisPtr->bigEndian = 1;
  } else {			/* either VXI or GPIB-VXI with no driver in command module */

#ifdef __hpux
    thisPtr->bigEndian = 1;
#else
    thisPtr->bigEndian = 0;
#endif

    if (intf == VI_INTF_GPIB_VXI)
      thisPtr->gpibVxi = 1;

    /* map A16 base */
    errStatus = viMapAddress(*vi, VI_A16_SPACE, 0x00, 0x40, VI_FALSE, VI_NULL, &addr);
    if (errStatus < VI_SUCCESS) {
      /* ignore any errors in hpe1564_close */
      hpe1564_close(*vi);
      *vi = VI_NULL;
      hpe1564_LOG_STATUS(*vi, NULL, hpe1564_INSTR_ERROR_A16MAP_FAILED);
    }

    thisPtr->a16Addr = addr;	/* put a16 addr into globals */

    /* if we are not e1406, then we need to know what model code we
       are dealing with.  e1406 path figures this out during its
       startup routine.  We need to do it here for the gpib-vxi and vxi
       interface types.
     */
    if ((intf == VI_INTF_VXI) || (intf == VI_INTF_GPIB_VXI)) {

      /* find the VXI manufacturer's ID */
      errStatus = viGetAttribute(*vi, VI_ATTR_MANF_ID, &manfId);
      if (VI_SUCCESS > errStatus) {
	/* Errors: VI_ERROR_NSUP_ATTR */

	/* ignore any errors in hpe1564_close */
	hpe1564_close(*vi);
	*vi = VI_NULL;

	hpe1564_LOG_STATUS(*vi, NULL, errStatus);
      }

      /* find the instrument's model code */
      errStatus = viGetAttribute(*vi, VI_ATTR_MODEL_CODE, (ViPAttrState) (&modelCode));
      if (VI_SUCCESS > errStatus) {
	/* Errors: VI_ERROR_NSUP_ATTR */
	/* Note: this should never happen 
	 *   with a VXI instrument
	 */

	/* ignore any errors in hpe1564_close */
	hpe1564_close(*vi);
	*vi = VI_NULL;
	hpe1564_LOG_STATUS(*vi, NULL, errStatus);
      }

      if (manfId != hpe1564_MANF_ID) {
	/* ignore any errors in hpe1564_close */
	hpe1564_close(*vi);
	*vi = VI_NULL;

	hpe1564_LOG_STATUS(*vi, NULL, VI_ERROR_FAIL_ID_QUERY);
      }

      if (modelCode == hpe1564_MODEL4_CODE) {
	thisPtr->E1563 = 0;
	thisPtr->numChans = 4;
      } else if (modelCode == hpe1564_MODEL2_CODE) {
	thisPtr->E1563 = 1;
	thisPtr->numChans = 2;
      } else {
	/* ignore any errors in hpe1564_close */
	hpe1564_close(*vi);
	*vi = VI_NULL;

	hpe1564_LOG_STATUS(*vi, NULL, VI_ERROR_FAIL_ID_QUERY);
      }
    }

    /* see if D32 supported on this platform; We will test this
       by writing a sample count out as two 16 bit writes, and
       then trying to read back in a 32 bit read.  The 32 bit
       read may report an error or just bring back incorrect
       data -- we check for either case.
     */
    hpe1564_POKE16(*vi, (ViUInt16 *) thisPtr->a16Addr + POSTSAMP_HIGH_REG, 0x0003);
    hpe1564_POKE16(*vi, (ViUInt16 *) thisPtr->a16Addr + POSTSAMP_LOW_REG, 0xFFCC);
    errStatus = viMoveIn32(*vi, VI_A16_SPACE, (ViBusAddress) 52, (ViBusSize) 2, tempBlock);
    if (errStatus < VI_SUCCESS)
      thisPtr->D16_only = 1;
    else if (tempBlock[1] != 0x0003FFCC)
      thisPtr->D16_only = 1;

    /* if not D-SCPI, we need to initialize the other globals.  This 
       also resets the card.  This is only way to get to a known
       state at card open time.
     */

    /* mumble -- could look at reset flag here and query state to
       get what we are set to.  Trigger source may be off -- don't
       know which is trig1 and which is trig2.
     */
    hpe1564_regInitGlobals(*vi, thisPtr);

    errStatus = hpe1564_sanityCheck(*vi, thisPtr, &result);
    if (result)
      errStatus = hpe1564_WARN_SELF_TEST;

  }				/* end else VXI or GPIB-VXI */

  hpe1564_LOG_STATUS(*vi, thisPtr, errStatus);
}

/****************************************************************************
hpe1564_close
*****************************************************************************
    Parameter Name                                       Type    Direction
   ------------------------------------------------------------------------
  | vi                                                ViSession   IN
  |   ---------------------------------------------------------------------
  |  | Instrument Handle returned from hpe1564_init()

*****************************************************************************/
ViStatus _VI_FUNC hpe1564_close(ViSession vi)
{
  hpe1564_globals *thisPtr;
  ViStatus errStatus;
  ViSession defRM;

  GetGlobals(vi, thisPtr);
  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);

  /* retrieve Resource Management session */
  defRM = thisPtr->defRMSession;

  /* if we have a handler, get rid of it */
  if (thisPtr->interrupts) {
    viUninstallHandler(vi, VI_EVENT_VXI_SIGP, VI_ANY_HNDLR, (ViAddr) thisPtr);
  }

  /* free memory */
  if (thisPtr) {
    /* make sure there is something to free */
    free(thisPtr);
  }

  /* close the vi and RM sessions */
  return viClose(defRM);
}

/****************************************************************************
hpe1564_reset
*****************************************************************************
    Parameter Name                                       Type    Direction
   ------------------------------------------------------------------------
  | vi                                                ViSession   IN
  |   ---------------------------------------------------------------------
  |  | Instrument Handle returned from hpe1564_init()

*****************************************************************************/
ViStatus _VI_FUNC hpe1564_reset(ViSession vi)
{
  hpe1564_globals *thisPtr;
  ViStatus errStatus;

  GetGlobals(vi, thisPtr)
      hpe1564_DEBUG_CHK_THIS(vi, thisPtr);

  if (thisPtr->e1406) {
    /* have to let ourselves know format has changed */
    thisPtr->digState.dataFormat = hpe1564_FORM_ASCII;
    errStatus = viPrintf(vi, "*RST\n");
  } else
    errStatus = hpe1564_regReset(vi, thisPtr);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/****************************************************************************
hpe1564_self_test
*****************************************************************************
    Parameter Name                                       Type    Direction
   ------------------------------------------------------------------------
  | vi                                                ViSession   IN
  |   ---------------------------------------------------------------------
  |  | Instrument Handle returned from hpe1564_init()
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

ViStatus _VI_FUNC hpe1564_self_test(ViSession vi, ViPInt16 test_result, ViPString test_message)
{
  hpe1564_globals *thisPtr;
  ViStatus errStatus;
  ViInt16 result, diagCode;
  char *errMssg;
  ViUInt32 timeOutVal;

  /* initialize output parameters */
  *test_result = -1;
  test_message[0] = 0;

  GetGlobals(vi, thisPtr)
      hpe1564_DEBUG_CHK_THIS(vi, thisPtr);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406) {
    char *theMssg[80];

    /* save the user's timeout setting */
    viGetAttribute(vi, VI_ATTR_TMO_VALUE, &timeOutVal);

    if (timeOutVal < 180000) {
      /* this command takes a long time, set for 180 seconds */
      viSetAttribute(vi, VI_ATTR_TMO_VALUE, 180000);
    }

    errStatus = viQueryf(vi, "*TST?\n", "%hd%*t", &result);

    if (timeOutVal < 180000) {
      /* re-store user's timeout value */
      viSetAttribute(vi, VI_ATTR_TMO_VALUE, timeOutVal);
    }

    if (VI_SUCCESS > errStatus) {
      *test_result = -1;
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    }

    /* if device clear, then blow outta here */
    if (thisPtr->device_clear) {
      thisPtr->device_clear = 0;
      return VI_SUCCESS;
    }

    if (result > 0) {
      errStatus = viQueryf(vi, "TEST:ERR? %hd\n", "%hd,%s%*t", result, &diagCode, theMssg);
      if (VI_SUCCESS > errStatus) {
	*test_result = -1;
	sprintf(test_message, "%s", "Visa error");
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
      }

      /* report failure */
      sprintf(test_message, "code: %d, %s", diagCode, theMssg);
    } else if (result == 0)
      sprintf(test_message, "%s", "Self test Passed");
    else
      /* got result < 0 */
      sprintf(test_message, "%s", "Self test Failed");
  } else {
    /* allocate some memory for use by self test */
    thisPtr->memPtr = malloc(400 * sizeof(ViInt16));
    errStatus = hpe1564_regSelfTest_Q(vi, thisPtr, &result);
    if (VI_SUCCESS > errStatus) {
      *test_result = -1;
      sprintf(test_message, "%s", "Visa error in test.");

      /* free the memory we were using */
      free(thisPtr->memPtr);
      thisPtr->memPtr = NULL;
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    }

    /* if we had a device_clear, handle it here */
    if (thisPtr->device_clear) {
      hpe1564_regAbort(vi, thisPtr);
      thisPtr->device_clear = 0;

      /* free the memory we were using */
      free(thisPtr->memPtr);
      thisPtr->memPtr = NULL;
      return VI_SUCCESS;
    }

    if (result > 0) {
      hpe1564_regTestErr_Q(vi, thisPtr, result, &diagCode, &errMssg);

      /* report failure */
      sprintf(test_message, "code: %d, %s", diagCode, errMssg);
    } else if (result == 0)
      sprintf(test_message, "%s", "Self test Passed");
    else
      /* got result < 0 */
      sprintf(test_message, "%s", "Self test Failed");
  }

  /* return the result, could be 0 for passed */
  *test_result = result;

  /* if not D-SCPI, then free the malloced memory */
  if (!thisPtr->e1406) {
    /* free the memory we were using */
    free(thisPtr->memPtr);
    thisPtr->memPtr = NULL;
  }

  hpe1564_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/****************************************************************************
hpe1564_error_query
*****************************************************************************
    Parameter Name                                       Type    Direction
   ------------------------------------------------------------------------
  | vi                                                ViSession   IN
  |   ---------------------------------------------------------------------
  |  | Instrument Handle returned from hpe1564_init()
   ------------------------------------------------------------------------
  | error_number                                      ViPInt32    OUT
  |   ---------------------------------------------------------------------
  |  | Instruments error code
   ------------------------------------------------------------------------
  | error_message                                     ViChar _VI_FAR []OUT
  |   ---------------------------------------------------------------------
  |  | Instrument's error message.  This is limited to 256 characters.

*****************************************************************************/
ViStatus _VI_FUNC hpe1564_error_query(ViSession vi, ViPInt32 error_number, ViPString error_message)
{
  hpe1564_globals *thisPtr;
  ViStatus errStatus;

  /* initialize output parameters */
  *error_number = 0;
  error_message[0] = 0;

  GetGlobals(vi, thisPtr)
      hpe1564_DEBUG_CHK_THIS(vi, thisPtr);

  /* get the error number */
  if (thisPtr->e1406) {
    errStatus = viQueryf(vi, "SYST:ERR?\n", "%ld,%t", error_number, error_message);
    if (VI_SUCCESS > errStatus) {

      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    }

    /* get rid of extra LF at the end of the error_message */
    error_message[strlen(error_message) - 1] = 0;
  }

  hpe1564_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/****************************************************************************
hpe1564_error_message
*****************************************************************************
    Parameter Name                                       Type    Direction
   ------------------------------------------------------------------------
  | vi                                                ViSession   IN
  |   ---------------------------------------------------------------------
  |  | Instrument Handle returned from hpe1564_init().
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

ViStatus _VI_FUNC hpe1564_error_message(ViSession vi,
					ViStatus error_number, ViChar _VI_FAR message[])
/* works for either kind of driver */
{
  hpe1564_globals *thisPtr;
  ViStatus errStatus;
  ViInt32 idx;

  /* initialize output parameters */
  message[0] = 0;

  thisPtr = NULL;

  /* try to find a thisPtr */
  if (VI_NULL != vi) {
    GetGlobals(vi, thisPtr)
	hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  }

  if (VI_SUCCESS == error_number) {
    sprintf(message, hpe1564_MSG_NO_ERRORS);
    hpe1564_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
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
		"%s " hpe1564_MSG_IN_FUNCTION " %s() %s",
		instrErrMsgTable[idx].errMessage, thisPtr->errFuncName, thisPtr->errMessage);
      } else {
	/* No context dependent error 
	 * message available so copy 
	 * the static error message
	 */
	strcpy(message, instrErrMsgTable[idx].errMessage);

      }

      hpe1564_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
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
      strcat(message, hpe1564_MSG_IN_FUNCTION);
      strcat(message, " ");
      strcat(message, thisPtr->errFuncName);
      strcat(message, "() ");
      strcat(message, thisPtr->errMessage);
    }

    /* VTL found an error message, so return success */
    hpe1564_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
  }

  /* if we have a VI_NULL, then we need to return a error message */
  if (VI_NULL == vi) {
    strcpy(message, hpe1564_MSG_VI_OPEN_ERR);
    hpe1564_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
  }

  /* user passed in a invalid status */
  sprintf(message,
	  hpe1564_MSG_INVALID_STATUS "  %ld" hpe1564_MSG_INVALID_STATUS_VALUE, (long)error_number);

  hpe1564_LOG_STATUS(vi, thisPtr, VI_ERROR_PARAMETER2);
}

/****************************************************************************
hpe1564_revision_query
*****************************************************************************
    Parameter Name                                       Type    Direction
   ------------------------------------------------------------------------
  | vi                                                ViSession      IN
  |   ---------------------------------------------------------------------
  |  | Instrument Handle returned from hpe1564_init()
   ------------------------------------------------------------------------
  | driver_rev                                ViChar _VI_FAR []      OUT
  |   ---------------------------------------------------------------------
  |  | Instrument driver revision.  This is limited to 256 characters.
   ------------------------------------------------------------------------
  | instr_rev                                 ViChar _VI_FAR []      OUT
  |   ---------------------------------------------------------------------
  |  | Instrument firmware revision.  This is limited to 256 characters.

*****************************************************************************/

ViStatus _VI_FUNC hpe1564_revision_query(ViSession vi,
					 ViChar _VI_FAR driver_rev[], ViChar _VI_FAR instr_rev[])
{
  hpe1564_globals *thisPtr;
  ViStatus errStatus;
  char temp_str[256];		/* temp hold for instr rev string */
  char *last_comma;		/* last comma in *IDN string */

  /* initialize output parameters */
  driver_rev[0] = 0;
  instr_rev[0] = 0;

  GetGlobals(vi, thisPtr)
      hpe1564_DEBUG_CHK_THIS(vi, thisPtr);

  sprintf(driver_rev, "%s", hpe1564_REV_CODE);

  if (thisPtr->e1406) {
    errStatus = viQueryf(vi, "*IDN?\n", "%s%*t", temp_str);
    if (errStatus < VI_SUCCESS) {
      driver_rev[0] = 0;
      instr_rev[0] = 0;

      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    }

    last_comma = strrchr(temp_str, ',');
    /* error and exit if last comma not found */
    if (!last_comma) {
      instr_rev[0] = 0;
      hpe1564_CDE_MESSAGE("no last comma found in IDN string");
      hpe1564_LOG_STATUS(vi, thisPtr, hpe1564_INSTR_ERROR_UNEXPECTED);
    }

    strcpy(instr_rev, last_comma + 1);

  } else {
    /* driver rev and instr rev are same here */
    sprintf(instr_rev, "%s", hpe1564_REV_CODE);
  }

  hpe1564_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/****************************************************************************
hpe1564_timeOut
*****************************************************************************
    Parameter Name                                       Type    Direction
   ------------------------------------------------------------------------
  | vi                                                ViSession   IN
  |   ---------------------------------------------------------------------
  |  | Instrument Handle returned from hpe1564_init()
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
ViStatus _VI_FUNC hpe1564_timeOut(ViSession vi, ViInt32 timeOut)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  GetGlobals(vi, thisPtr)
      hpe1564_DEBUG_CHK_THIS(vi, thisPtr);

  hpe1564_CHK_LONG_RANGE(timeOut, 1, 2147483647, VI_ERROR_PARAMETER2);

  errStatus = viSetAttribute(vi, VI_ATTR_TMO_VALUE, timeOut);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/****************************************************************************
hpe1564_timeOut_Q
*****************************************************************************
    Parameter Name                                       Type    Direction
   ------------------------------------------------------------------------
  | vi                                                ViSession   IN
  |   ---------------------------------------------------------------------
  |  | Instrument Handle returned from hpe1564_init()
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
ViStatus _VI_FUNC hpe1564_timeOut_Q(ViSession vi, ViPInt32 timeOut)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  GetGlobals(vi, thisPtr)
      hpe1564_DEBUG_CHK_THIS(vi, thisPtr);

  errStatus = viGetAttribute(vi, VI_ATTR_TMO_VALUE, timeOut);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/****************************************************************************
hpe1564_errorQueryDetect
*****************************************************************************
    Parameter Name                                       Type    Direction
   ------------------------------------------------------------------------
  | vi                                                ViSession   IN
  |   ---------------------------------------------------------------------
  |  | Instrument Handle returned from hpe1564_init()
   ------------------------------------------------------------------------
  | errorQueryDetect                                  ViBoolean   IN
  |   ---------------------------------------------------------------------
  |  | Boolean which enables (VI_TRUE) or disables (VI_FALSE)
  |  | automatic instrument error querying.

*****************************************************************************/
ViStatus _VI_FUNC hpe1564_errorQueryDetect(ViSession vi, ViBoolean errDetect)
/*same for both driver types */
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  GetGlobals(vi, thisPtr)
      hpe1564_DEBUG_CHK_THIS(vi, thisPtr);

  hpe1564_CHK_BOOLEAN(errDetect, VI_ERROR_PARAMETER2);

  thisPtr->errQueryDetect = errDetect;

  hpe1564_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/****************************************************************************
hpe1564_errorQueryDetect_Q
*****************************************************************************
    Parameter Name                                       Type    Direction
   ------------------------------------------------------------------------
  | vi                                                ViSession   IN
  |   ---------------------------------------------------------------------
  |  | Instrument Handle returned from hpe1564_init()
   ------------------------------------------------------------------------
  | pErrDetect                                        ViPBoolean  OUT
  |   ---------------------------------------------------------------------
  |  | Boolean indicating if automatic instrument error
  |  | querying is performed.

*****************************************************************************/
ViStatus _VI_FUNC hpe1564_errorQueryDetect_Q(ViSession vi, ViPBoolean pErrDetect)
/* same for both types of driver */
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  GetGlobals(vi, thisPtr)
      hpe1564_DEBUG_CHK_THIS(vi, thisPtr);

  *pErrDetect = thisPtr->errQueryDetect;

  hpe1564_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/****************************************************************************
hpe1564_dcl
*****************************************************************************
    Parameter Name                                       Type    Direction
   ------------------------------------------------------------------------
  | vi                                                ViSession   IN
  |   ---------------------------------------------------------------------
  |  | Instrument Handle returned from hpe1564_init()

*****************************************************************************/
ViStatus _VI_FUNC hpe1564_dcl(ViSession vi)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  GetGlobals(vi, thisPtr)
      hpe1564_DEBUG_CHK_THIS(vi, thisPtr);

  if (thisPtr->e1406)
    errStatus = viClear(vi);
  else
    thisPtr->device_clear = 1;

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/* Begin status functions */

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_cls
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function clears the status system on the instrument. 
 *           Additionally, any error messages in the queue are deleted, and
 *           the error light is turned off.
 *
 *	     This function is usually not called unless an HP Command
 *           module is used in the system.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from hpe1564_init().
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_cls(ViSession vi)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (VI_SUCCESS > errStatus) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_cls");

  if (thisPtr->e1406)
    errStatus = viPrintf(vi, "*CLS\n");
  else
    thisPtr->status = 0;

  /* Perform Error Checking on Each Parameter */
  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/****************************************************************************
hpe1564_readStatusByte_Q
*****************************************************************************
    Parameter Name                                       Type    Direction
   ------------------------------------------------------------------------
  | vi                                                ViSession   IN
  |   ---------------------------------------------------------------------
  |  | Instrument Handle returned from hpe1564_init()
   ------------------------------------------------------------------------
  | statusByte                                        ViPInt16    OUT
  |   ---------------------------------------------------------------------
  |  | returns the contents of the status byte

*****************************************************************************/
ViStatus _VI_FUNC hpe1564_readStatusByte_Q(ViSession vi, ViPInt16 statusByte)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;
  ViUInt16 stb;

  GetGlobals(vi, thisPtr);
  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);

  /* this function does nothing if not command module path */
  if (thisPtr->e1406) {
    errStatus = viReadSTB(vi, &stb);
    if (VI_SUCCESS > errStatus)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    *statusByte = (ViInt16) stb;
  }

  hpe1564_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/* 	hpe1564_opc_Q
	Returns VI_TRUE if operations still pending
	
	input:
		vi			- session
	output
		p1			- VI_TRUE if operations pending
	
*/
ViStatus _VI_FUNC hpe1564_opc_Q(ViSession vi, ViPBoolean p1)
{

  ViStatus errStatus;
  hpe1564_globals *thisPtr;

  GetGlobals(vi, thisPtr);
  /* If (message-based I/O) then write to instrument. */
  if (thisPtr->e1406) {
    errStatus = viQueryf(vi, "*OPC?\n", "%hd%*t", p1);
    hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
  } else {			/* Must be register-based I/O */

    /* if we get here, oper was complete */
    *p1 = 1;
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_abort
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function aborts a measurement in progress. The command is
 *           ignored without error if a measurement is not in progress.  If
 *           there is a measurement in progress and the measurement is
 *           aborted, there is an indeterminate amount of data in memory. 
 *           Attempts to read this data will result in the full count of data
 *           returned in the array, but some, all, or none of it may be from
 *           the aborted measurement (depending upon when the measurement
 *           process was aborted).  The data returned may therefore contain
 *           data from a previous measurement.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_abort(ViSession vi)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_abort");

  if (thisPtr->e1406)
    errStatus = viPrintf(vi, "ABOR\n");
  else
    errStatus = hpe1564_regAbort(vi, thisPtr);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/************
 * FUNCTION:   hpe1564_scan16()
 * PURPOSE:    Reads a block from input, placing block of bytes into caller's
 *             array. If caller requests it,  swaps all 2byte 16bit words
 *             appropriately from Motorola to Intel format.
 * ALGORITHM:  Read length.
 *             Read Buffer.
 *             if (swapping requested)
 *                Byte Swap 2 byte words.
 * PARAMS:     vi          - VTL session
 *             max_length  - number of int16 values that can be stored in b
 *             b           - array to store the data into.
 *             rtn_count   - count of number of 16bit words scanned.
 *             swap        - if == VI_TRUE, then does byte swapping.
 ************/
ViStatus hpe1564_scan16(ViSession vi,
			ViInt32 max_length, ViPChar b16, ViPInt32 rtn_count, ViBoolean swap)
{
  ViChar c[2];
  ViChar length_str[16];
  ViInt16 digits;
  ViInt32 n_bytes;
  ViInt32 i;
  char fmtStr[40];
  ViStatus rtn_code;

  /* Read Header (should be #[1-9], if #0 then Indefinite Block. */
  rtn_code = viScanf(vi, "%2c", c);
  if (rtn_code < VI_SUCCESS)
    return (rtn_code);
  digits = c[1] - '0';		/* c[0] is the # */

  /* If digits was not [0-9] then syntax problem, blow away input and return */
  if ((digits < 0) || (9 < digits)) {
    rtn_code = viScanf(vi, "%*t");	/* Clear input. */
    return (VI_ERROR_INV_RESPONSE);
  }

  /* If (Definite Arbitrary Block) */
  if (digits != 0) {
    /* Scan DAB array count. */
    sprintf(fmtStr, "%%%ldc", digits);
    rtn_code = viScanf(vi, fmtStr, length_str);
    if (rtn_code < VI_SUCCESS)
      return (rtn_code);

    length_str[digits] = '\0';	/* null terminate the string */

    n_bytes = atol(length_str);

    /* Verify that caller's array is big enough. */
    if (n_bytes > (max_length * 2))
      return (VI_ERROR_PARAMETER2);	/* Caller's length too small. */
  } else {			/* Indefinite Arbitrary Block */

    /* Read data up to max_length, else end of input.  */
    /* %t format reads to next END, else until n_bytes. */
    n_bytes = max_length * 2;
  }

  sprintf(fmtStr, "%%%ldc%%*t", n_bytes);

  /* if we got to here, all is well, do the read */
  rtn_code = viScanf(vi, fmtStr, b16);
  if (rtn_code < VI_SUCCESS)
    return (rtn_code);

  *rtn_count = n_bytes / 2;

  if (swap == VI_TRUE) {
    /* Byte reverse. */
    for (i = 0; i < n_bytes / 2; i++) {
      ViChar c;

      c = b16[1];
      b16[1] = b16[0];
      b16[0] = c;
      b16 += 2;			/* b16 is pointer to data bytes (char), increment it */
    }

  }
  /* If(swapping) */
  return (rtn_code);

}				/* hpe1564_scan16() */

/************
 * FUNCTION:   hpe1564_getHeader()
 * PURPOSE:    Reads the binary block header from a definite length block.
 *             Returns the number of bytes that need to be read.
 * 
 * PARAMS:     vi          - VTL session
 *             num_bytes   - number of bytes coming back.
 *  
 ************/
ViStatus hpe1564_readHeader(ViSession vi, ViPInt32 num_bytes)
{
  ViChar c[2];
  ViChar length_str[16];
  ViInt16 digits;
  ViInt32 n_bytes;
  char fmtStr[40];
  ViStatus rtn_code;

  /* Read Header (should be #[1-9], if #0 then Indefinite Block. */
  rtn_code = viScanf(vi, "%2c", c);
  if (rtn_code < VI_SUCCESS)
    return (rtn_code);
  digits = c[1] - '0';		/* c[0] is the # */

  /* If digits was not [0-9] then syntax problem, blow away input and return */
  if ((digits < 0) || (9 < digits)) {
    rtn_code = viScanf(vi, "%*t");	/* Clear input. */
    return (VI_ERROR_INV_RESPONSE);
  }

  /* If (Definite Arbitrary Block) */
  if (digits != 0) {
    /* Scan DAB array count. */
    sprintf(fmtStr, "%%%ldc", digits);
    rtn_code = viScanf(vi, fmtStr, length_str);
    if (rtn_code < VI_SUCCESS)
      return (rtn_code);

    length_str[digits] = '\0';	/* null terminate the string */

    n_bytes = atol(length_str);

  } else {			/* Indefinite Arbitrary Block, we don't handle, so error */

    rtn_code = viScanf(vi, "%*t");	/* Clear input. */
    return (VI_ERROR_INV_RESPONSE);
  }

  *num_bytes = n_bytes;

  return VI_SUCCESS;
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_calDacVolt
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function programs the internal DAC to output the specified
 *            voltage.  This command is only active if the calibration source
 *            (hpe1564_calSource) is set to INTernal.
 *           
 *            The maximum output levels are limited to the levels shown in
 *           the table below. Note that the 64 volt range and 256 volt range
 *           are not used.  See the hpe1564_calGain function for more
 *           information regarding calibrating those ranges.
 *           
 *            An error will occur if the specified voltage value is greater
 *           than that allowed for the present range setting.  Be sure that
 *           the desired range has been set before sending this command.
 *           
 *            The recommended DAC voltages for calibrating each range are
 *           given below.  These values are approximately 98 percent of full
 *           scale (except on the 64 and 256 volt ranges).
 *           
 *           Voltage Range   Max. DC Voltage (absolute value)
 *             0.0625        0.061256409
 *             0.2500        0.245025635
 *             1.0000        0.980102539
 *             4.0000        3.920410156
 *            16.0000        15.00
 *            64.0000        not used
 *           256.0000        not used
 *           
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViReal64 voltage
 * IN        
 *            This is the voltage to be output by the DAC.
 * 
 *      MAX = hpe1564_CAL_DAC_MAX   15.00
 *      MIN = hpe1564_CAL_DAC_MIN   -15.00
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_calDacVolt(ViSession vi, ViReal64 voltage)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_calDacVolt");

  hpe1564_CHK_REAL_RANGE(voltage, hpe1564_CAL_DAC_MIN, hpe1564_CAL_DAC_MAX, VI_ERROR_PARAMETER2);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406) {
    errStatus = viPrintf(vi, "CAL:DAC:VOLT %lg\n", voltage);
  } else
    errStatus = hpe1564_regDacSour(vi, thisPtr, voltage);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_calDacVolt_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function queries the present setting of the internal DAC
 *           voltage.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViPReal64 voltage
 * OUT       
 *            The present setting of the calibration DAC.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_calDacVolt_Q(ViSession vi, ViPReal64 voltage)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_calDacVolt_Q");

  if (thisPtr->e1406) {
    errStatus = viQueryf(vi, "CAL:DAC:VOLT?\n", "%lg%*t", voltage);
  } else
    errStatus = hpe1564_regDacSour_Q(vi, thisPtr, voltage);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_calData_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function returns the present calibration constants.  The
 *           constants are in an array, and the constants from all channels
 *           are returned in the array.  The following equations are used to
 *           access specific constants for a given channel and range:
 *           
 *            offsetIndex = 64 * ( channel - 1) + 32 * 1 + 8 * 0 + rangeIndex
 *            gainIndex   = 64 * ( channel - 1) + 32 * 0 + 8 * 0 + rangeIndex
 *           
 *           And rangeIndex is an integer 0 through 6 where:
 *           
 *               0 = 0.0625 volt range
 *               1 = 0.250 volt range
 *               2 = 1.0 volt range
 *               3 = 4.0 volt range
 *               4 = 16.0 volt range
 *               5 = 64.0 volt range
 *               6 = 256 volt range
 *           
 *           So if, for example, you wished to see the gain constant for the
 *           64 volt range on channel 2, the array index would be:
 *           
 *                offsetIndex = 64 * 1 + 32 + 0 + 5 = 101
 *                gainIndex   = 64 * 1 + 0 + 0 + 5 = 69
 *           
 *           so the gain constant would be found at calData[69], and the
 *           offset constant at calData[101].
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 arrayLength
 * IN        
 *            Specifies the length of the data array in total number of
 *           readings. An error will be reported if arrayLength is less than
 *           256.  If arrayLength is > 256, no error will occur, but only 256
 *           elements will be filled.
 * 
 *      MAX = hpe1564_CAL_ARRAY_MAX   16000000
 *      MIN = hpe1564_CAL_ARRAY_MIN   256
 * 
 * PARAM 3 : ViInt16 _VI_FAR calData[]
 * OUT       
 *            The array of calibration constants is returned in this
 *           parameter.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_calData_Q(ViSession vi, ViInt16 arrayLength, ViInt16 _VI_FAR calData[])
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_calData_Q");

  hpe1564_CHK_LONG_RANGE(arrayLength, hpe1564_CAL_ARRAY_MIN, hpe1564_CAL_ARRAY_MAX,
			 VI_ERROR_PARAMETER2);

  if (thisPtr->e1406) {
    ViInt32 howMany;

    errStatus = viPrintf(vi, "CAL:DATA?\n");

    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* last parameter indicates to byte swap the readings */
#ifdef BIG_ENDIAN
    errStatus = hpe1564_scan16(vi, arrayLength, (char *)calData, &howMany, 0);
#else
    errStatus = hpe1564_scan16(vi, arrayLength, (char *)calData, &howMany, 1);
#endif
  } else
    errStatus = hpe1564_regCalData_Q(vi, thisPtr, arrayLength, (ViInt16 *) calData);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_calGain
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function does a gain calibration on the specified channel.
 *            The number of samples and the sample rate would normally be
 *           defaulted (-1 and -1.0 respectively) to provide averaging over
 *           an integral number of either 50 Hertz or 60 Hertz power line
 *           cycles.  This allows the calibration to cancel out any noise
 *           that is periodic with the power supply. Specifying a value other
 *           than -1 for numSamples and period will result in those values
 *           being used for the gain calibration.
 *            Calibration of the 64 volt and 256 volt ranges are a special
 *           case.  These ranges are normally gain calibrated during the 16
 *           volt range gain calibration.  This is done by calculating the
 *           effects of the attenuators and amplifiers from the gain
 *           calibrations on the lower ranges and extrapolating a gain value
 *           for those two ranges.  NOTE: For this to work properly, it is
 *           important that all ranges below 16 volts are gain calibrated
 *           BEFORE calibrating the 16 volt range.  The autoCal parameter is
 *           used to disable this automatic gain calculation.  The autoCal
 *           value is only checked or used during the calibration of the 16
 *           volt range. Turning autoCal off (0) allows calibration of the 16
 *           volt range explicitly without affecting the gain on 64 volt and 256 volt ranges.
 *            Before this command is executed, the following steps must have
 *           been already completed:
 *               1. The instrument must be set to the desired range and
 *                  filtering on the channel of interest.
 *           
 *               2. Calibration must be enabled with
 *                  hpe1564_calState(vi, hpe1564_ON).
 *           
 *               3. A calibration source must be specified with
 *                  hpe1564_calSource.
 *           
 *               4. A calibration value must have been supplied with the
 *                  hpe1564_calValue command.  This value must be a least 85
 *                  percent (ideally 98 percent) of either a positive full
 *                  scale reading or a negative full scale reading.
 *           
 *               5. The DC level specified by hpe1564_calValue must be
 *                  present on either the input connector (calSource
 *                  EXTernal), or on the calibration bus
 *                  (calSource INTernal).
 *           
 *            Failure to do the above steps will result in errors during
 *           calibration and/or inaccurate calibration.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 channel
 * IN        
 *            Specifies which channel will be calibrated.
 *           
 *            The channels are defined as macros and are shown below:
 * 
 *      Macro Name         Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_CHAN1          1  1
 *      hpe1564_CHAN2          2  2
 *      hpe1564_CHAN3          3  3
 *      hpe1564_CHAN4          4  4
 * 
 * PARAM 3 : ViInt32 numSamples
 * IN        
 *            Specifies the total number of samples to be taken and averaged
 *           for gain calculations.  If the value specified is -1, then the
 *           default count of 100 samples will be used.
 * 
 *      MAX = hpe1564_CAL_COUNT_MAX   1000
 *      MIN = hpe1564_CAL_COUNT_MIN   25
 * 
 * PARAM 4 : ViReal64 period
 * IN        
 *            This is the sample period that will be used for each reading
 *            during the calibration.  If the value -1.0 is specified, the
 *            default sample period of 0.001 seconds will be used.
 * 
 *      MAX = hpe1564_SAMP_TIM_MAX   0.83886070
 *      MIN = hpe1564_SAMP_TIM_MIN   0.00000130
 * 
 * PARAM 5 : ViBoolean autoCal
 * IN        
 *            This parameter enables or disables the automatic gain
 *           calibration of the 64 volt and 256 volt ranges.  It is only
 *           checked during the gain calibration of the 16 volt range.  At
 *           that time, if enabled (hpe1564_ON), the gain constants for the
 *           64 volt and 256 volt ranges will be calculated from the
 *           attenuator and amplifier behaviors of the lower ranges.  If
 *           disabled (hpe1564_OFF), the gain values on the 64 volt and 256
 *           volt ranges will NOT be modified.  This allows someone to gain
 *           calibrate the 16 volt range without side affects.
 * 
 *      Macro Name       Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_OFF          0  Disable calibration
 *      hpe1564_ON           1  Enable calibration
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_calGain(ViSession vi,
				  ViInt16 channel,
				  ViInt32 numSamples, ViReal64 period, ViBoolean autoCal)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;
  ViPInt16 rdgs;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_calGain");

  hpe1564_CHK_INT_RANGE(channel, 1, 4, VI_ERROR_PARAMETER2);

  /* fix up any defaulted choices */
  if (numSamples < 0)
    numSamples = DEF_CAL_SAMP_COUNT;
  else
    hpe1564_CHK_LONG_RANGE(numSamples, hpe1564_CAL_COUNT_MIN, hpe1564_CAL_COUNT_MAX,
			   VI_ERROR_PARAMETER3);

  if (period < 0)
    period = DEF_CAL_PERIOD;
  else
    hpe1564_CHK_REAL_RANGE(period, hpe1564_SAMP_TIM_MIN, hpe1564_SAMP_TIM_MAX, VI_ERROR_PARAMETER4);

  hpe1564_CHK_BOOLEAN(autoCal, VI_ERROR_PARAMETER5);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406) {
    errStatus = viPrintf(vi, "CAL:GAIN%hd %ld, %lg, %hd\n", channel, numSamples, period, autoCal);
  } else {
    rdgs = (ViPInt16) malloc(numSamples * sizeof(ViInt16));
    if (rdgs == NULL)
      hpe1564_LOG_STATUS(vi, thisPtr, VI_ERROR_ALLOC);

    errStatus = hpe1564_regCalGain(vi, thisPtr, channel, numSamples, period, autoCal, rdgs);

    free(rdgs);
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_calOffset
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function does an offset calibration on the specified
 *           channel.  The number of samples and the sample rate would
 *           normally be defaulted (-1 and -1.0 respectively) to provide
 *           averaging over an integral number of either 50 Hertz or 60 Hertz
 *           power line cycles.  This allows the calibration to cancel out
 *           any noise that is periodic with the power supply. Specifying a
 *           value other than -1 for numSamples and/or period will result in
 *           those values being used for the gain calibration.
 *            Before this command is executed, the following steps must have
 *           been already completed:
 *           
 *               1. The instrument must be set to the desired range
 *                  on the channel of interest.
 *           
 *               2. Calibration must be enabled with
 *                  hpe1564_calState(vi, hpe1564_ON).
 *           
 *            Failure to do the above steps will result in errors.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 channel
 * IN        
 *            Specifies which channel will be calibrated.
 *           
 *            The channels are defined as macros and are shown below:
 * 
 *      Macro Name         Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_CHAN1          1  1
 *      hpe1564_CHAN2          2  2
 *      hpe1564_CHAN3          3  3
 *      hpe1564_CHAN4          4  4
 * 
 * PARAM 3 : ViInt32 numSamples
 * IN        
 *            Specifies the total number of samples to be taken and averaged
 *           for offset calculations.  If the value specified is -1, then the
 *           default count of 100 samples will be used.
 * 
 *      MAX = hpe1564_CAL_COUNT_MAX   1000
 *      MIN = hpe1564_CAL_COUNT_MIN   25
 * 
 * PARAM 4 : ViReal64 period
 * IN        
 *            This is the sample period that will be used for each reading
 *            during the calibration.  If the value -1.0 is specified, the
 *            default sample period of 0.001 seconds will be used.
 * 
 *      MAX = hpe1564_SAMP_TIM_MAX   0.83886070
 *      MIN = hpe1564_SAMP_TIM_MIN   0.00000130
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_calOffset(ViSession vi,
				    ViInt16 channel, ViInt32 numSamples, ViReal64 period)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;
  ViInt16 *rdgs;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_calOffset");

  hpe1564_CHK_INT_RANGE(channel, 1, 4, VI_ERROR_PARAMETER2);

  /* fix up any defaulted choices */
  if (numSamples < 0)
    numSamples = DEF_CAL_SAMP_COUNT;
  else
    hpe1564_CHK_LONG_RANGE(numSamples, hpe1564_CAL_COUNT_MIN, hpe1564_CAL_COUNT_MAX,
			   VI_ERROR_PARAMETER3);

  if (period < 0)
    period = DEF_CAL_PERIOD;
  else
    hpe1564_CHK_REAL_RANGE(period, hpe1564_SAMP_TIM_MIN, hpe1564_SAMP_TIM_MAX, VI_ERROR_PARAMETER4);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406) {
    errStatus = viPrintf(vi, "CAL:ZERO%hd %ld, %lg\n", channel, numSamples, period);

    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
  } else {
    rdgs = (ViPInt16) malloc(numSamples * sizeof(ViInt16));
    if (rdgs == NULL)
      hpe1564_LOG_STATUS(vi, thisPtr, VI_ERROR_ALLOC);

    errStatus = hpe1564_regCalOffset(vi, thisPtr, channel, numSamples, period, rdgs);

    free(rdgs);
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_calOffsAll_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function does an offset calibration of all ranges on the
 *           specified channel.  The number of samples and the sample rate
 *           would normally be defaulted (-1 and -1.0 respectively) to
 *           provide averaging over an integral number of either 50 Hertz or
 *           60 Hertz power line cycles.  This allows the calibration to
 *           cancel out any noise that is periodic with the power supply.
 *           Specifying a value other than -1 for numSamples and/or period
 *           will result in those values being used for the offset calibration.
 *            If an error occurs on any range, calibration proceeds on to the
 *           next range, and the bad range is noted as a high bit in the
 *           errCode parameter, where bit 0 represents the lowest range
 *           (0.0625 Volt) and bit 6 represents the highest range (256 Volt).
 *           
 *            Before this command is executed, the following steps must have
 *           been already completed:
 *           
 *               1. Calibration must be enabled with
 *                  hpe1564_calState(vi, hpe1564_ON).
 *           
 *            Failure to do the above steps will result in errors.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 channel
 * IN        
 *            Specifies which channel will be calibrated.
 *           
 *            The channels are defined as macros and are shown below:
 * 
 *      Macro Name         Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_CHAN1          1  1
 *      hpe1564_CHAN2          2  2
 *      hpe1564_CHAN3          3  3
 *      hpe1564_CHAN4          4  4
 * 
 * PARAM 3 : ViInt32 numSamples
 * IN        
 *            Specifies the total number of samples to be taken and averaged
 *           for offset calculations.  If the value specified is -1, then the
 *           default count of 100 samples will be used.
 * 
 *      MAX = hpe1564_CAL_COUNT_MAX   1000
 *      MIN = hpe1564_CAL_COUNT_MIN   25
 * 
 * PARAM 4 : ViReal64 period
 * IN        
 *            This is the sample period that will be used for each reading
 *            during the calibration.  If the value -1.0 is specified, the
 *            default sample period of 0.001 seconds will be used.
 * 
 *      MAX = hpe1564_SAMP_TIM_MAX   0.83886070
 *      MIN = hpe1564_SAMP_TIM_MIN   0.00000130
 * 
 * PARAM 5 : ViPInt16 errCode
 * OUT       
 *            An error code is returned in this parameter.  The lower byte
 *           indicates which range had an error (0 thru 6).  The bit position
 *           for the range (0 thru 6) is set high if an error occurred on
 *           that range. For example, if ranges 0 (0.0625 V) and 5 (64.0 V)
 *           failed, the return code would be 0x0021.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_calOffsAll_Q(ViSession vi,
				       ViInt16 channel,
				       ViInt32 numSamples, ViReal64 period, ViPInt16 errCode)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;
  ViUInt32 timeOutVal;
  ViPInt16 rdgs;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_calOffset");

  hpe1564_CHK_INT_RANGE(channel, 1, 4, VI_ERROR_PARAMETER2);

  /* fix up any defaulted choices */
  if (numSamples < 0)
    numSamples = DEF_CAL_SAMP_COUNT;
  else
    hpe1564_CHK_LONG_RANGE(numSamples, hpe1564_CAL_COUNT_MIN, hpe1564_CAL_COUNT_MAX,
			   VI_ERROR_PARAMETER3);

  if (period < 0)
    period = DEF_CAL_PERIOD;
  else
    hpe1564_CHK_REAL_RANGE(period, hpe1564_SAMP_TIM_MIN, hpe1564_SAMP_TIM_MAX, VI_ERROR_PARAMETER4);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406) {
    /* save the user's timeout setting */
    viGetAttribute(vi, VI_ATTR_TMO_VALUE, &timeOutVal);

    /* change user's timeout value if too small */
    if (timeOutVal < 180000) {
      /* this command takes a long time, set for 180 seconds */
      viSetAttribute(vi, VI_ATTR_TMO_VALUE, 180000);
    }

    errStatus = viPrintf(vi, "CAL:ZERO%hd:ALL? %ld, %lg\n", channel, numSamples, period);
    if (errStatus < VI_SUCCESS) {
      /* if error, re-store user's timeout value */
      viSetAttribute(vi, VI_ATTR_TMO_VALUE, timeOutVal);
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = viScanf(vi, "%hd%*t", errCode);

    if (timeOutVal < 180000) {
      /* re-store user's timeout value */
      viSetAttribute(vi, VI_ATTR_TMO_VALUE, timeOutVal);
    }

  } else {
    rdgs = (ViPInt16) malloc(numSamples * sizeof(ViInt16));
    if (rdgs == NULL)
      hpe1564_LOG_STATUS(vi, thisPtr, VI_ERROR_ALLOC);

    errStatus = hpe1564_regCalOffsAll(vi, thisPtr, channel, numSamples, period, errCode, rdgs);

    free(rdgs);
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_calSource
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function specifies the calibration source to be used for
 *           any subsequent gain or zero (offset) calibrations.  If
 *           "EXTernal" is specified, a voltage must be provided from an
 *           external source.  If "INTernal" is specified, the voltage
 *           specified by the hpe1564_calDacVolt command will be output onto
 *           the calibration bus.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 source
 * IN        
 *            This parameter specifies which calibration source to use for
 *            gain and offset calibrations.  These values are represented by
 *            the macros below.
 * 
 *      Macro Name                Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_CAL_SOUR_INT          0  Internal
 *      hpe1564_CAL_SOUR_EXT          1  External
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1564_calSource_source_a[] = { "INT", "EXT", 0 };

ViStatus _VI_FUNC hpe1564_calSource(ViSession vi, ViInt16 source)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_calSource");

  hpe1564_CHK_ENUM(source, 1, VI_ERROR_PARAMETER2);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406) {
    errStatus = viPrintf(vi, "CAL:SOUR %s\n", hpe1564_calSource_source_a[source]);
  } else
    errStatus = hpe1564_regCalSource(vi, thisPtr, source);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_calSource_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function queries the present setting of the calibration
 *           source. This setting is shared by all of the channels.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViPInt16 source
 * OUT       
 *            The present setting (EXTernal or INTernal) is returned as an
 *            integer in this parameter, where 0=INTernal and 1=EXTernal.
 *            These values are represented by the macros below.
 * 
 *      Macro Name                Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_CAL_SOUR_INT          0  Internal
 *      hpe1564_CAL_SOUR_EXT          1  External
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_calSource_Q(ViSession vi, ViPInt16 source)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;
  char source_str[32];

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_calSource_Q");

  if (thisPtr->e1406) {
    errStatus = viQueryf(vi, "CAL:SOUR?\n", "%s%*t", source_str);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = hpe1564_findIndex(thisPtr, hpe1564_calSource_source_a, source_str, source);
  } else
    errStatus = hpe1564_regCalSource_Q(vi, thisPtr, source);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_calState
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function enables the calibration of the instrument.  While
 *           enabled, many other instrument operations are not allowed, and
 *           will result in an error "Illegal while calibrating".  When
 *           finished with calibration, set calibration state to off.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViBoolean state
 * IN        
 *            This parameter enables or disables calibration of the
 *            instrument. The allowable settings are shown in the following
 *           macros.
 * 
 *      Macro Name       Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_OFF          0  Disable calibration
 *      hpe1564_ON           1  Enable calibration
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_calState(ViSession vi, ViBoolean state)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_calState");

  hpe1564_CHK_BOOLEAN(state, VI_ERROR_PARAMETER2);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406) {
    errStatus = viPrintf(vi, "CAL:STAT %hd\n", state);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
  } else
    errStatus = hpe1564_regCalState(vi, thisPtr, state);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_calState_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function queries the present calibration state of the
 *           instrument.  A return value of 1 indicates the instrument is in
 *           the calibration process. A return value of 0 indicates
 *           calibration is not enabled, and the calibration commands (like
 *           hpe1564_calGain) will return an error "Calibration not enabled".
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViPBoolean state
 * OUT       
 *            The calibration state (ON=1, 0=OFF) is returned in this
 *           parameter. The allowable settings are shown in the following
 *           macros.
 * 
 *      Macro Name       Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_OFF          0  Disable calibration
 *      hpe1564_ON           1  Enable calibration
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_calState_Q(ViSession vi, ViPBoolean state)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_calState_Q");

  if (thisPtr->e1406) {
    errStatus = viQueryf(vi, "CAL:STAT?\n", "%hd%*t", state);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
  } else
    errStatus = hpe1564_regCalState_Q(vi, thisPtr, state);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_calStore
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function stores the calibration constants to non-volatile
 *           RAM after calibration has been completed.  It may also be used
 *           to store constants into non-volatile RAM after downloading
 *           constants via the hpe1564_calData command.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_calStore(ViSession vi)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_calStore");

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406) {
    errStatus = viPrintf(vi, "CAL:STOR\n");
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
  } else
    errStatus = hpe1564_regCalStore(vi, thisPtr);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_calValue
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function specifies the voltage value applied to the
 *           channel being calibrated.  This value informs the digitizer what
 *           voltage is either being placed on the input (calSource EXTernal)
 *           or is being generated by the internal DAC (calSource INTernal)
 *           and output onto the calibration bus.
 *           
 *            When calSource INTernal is specified, the internal DAC outputs
 *           the voltage specified by the calDacVolt command; this voltage
 *           must be measured with a voltmeter by the person doing the
 *           calibration, and that measured voltage is what is input to this
 *           routine. The actual voltage value output from the internal DAC
 *           can be measured across pins 5 (high) and 9 (low) of the
 *           D-subminiature calibration bus connector.
 *            All ranges except for the 64 volt and 256 volt ranges, are
 *           limited to a maximum voltage around 98% of full scale.  This DC
 *           voltage may be either positive or negative in value.  The 64 and
 *           256 volt range gain values are calculated automatically when the
 *           16 volt range is gain calibrated, by using the values from the
 *           other ranges and adjusting for the amplifier and attenuator
 *           offsets.  It is therefore not necessary to calibrate gain on the
 *           64 volt and 256 volt ranges.
 *            Absolute maximum voltages for each range are shown below.
 *           
 *           Voltage Range   Max. DC Voltage (absolute value)
 *             0.0625        0.061256409
 *             0.2500        0.245025635
 *             1.0000        0.980102539
 *             4.0000        3.920410156
 *            16.0000        15.68164062 (15.00 if INTernal source)
 *            64.0000        extrapolated
 *           256.0000        extrapolated
 *           
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViReal64 voltage
 * IN        
 *            This is the the value entered by the user to provide the actual
 *            voltage being used for calibration.  This voltage is being
 *           output either by an external device (when calSource is EXTernal)
 *           or by the instrument's internal DAC (when calSource is
 *           INTernal).
 * 
 *      MAX = hpe1564_CAL_VALUE_MAX   251.00
 *      MIN = hpe1564_CAL_VALUE_MIN   -251.00
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_calValue(ViSession vi, ViReal64 voltage)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_calValue");

  hpe1564_CHK_REAL_RANGE(voltage, hpe1564_CAL_VALUE_MIN, hpe1564_CAL_VALUE_MAX,
			 VI_ERROR_PARAMETER2);

  if (thisPtr->e1406) {
    errStatus = viPrintf(vi, "CAL:VAL %lg\n", voltage);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
  } else
    errStatus = hpe1564_regCalValue(vi, thisPtr, voltage);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_calValue_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function queries the present setting of the calibration
 *           voltage on the specified channel.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViPReal64 voltage
 * OUT       
 *            The present setting of the calibration voltage value is
 *           returned in this parameter.  This is the voltage that the
 *           instrument attempts to calibrate to.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_calValue_Q(ViSession vi, ViPReal64 voltage)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_calValue_Q");

  if (thisPtr->e1406) {
    errStatus = viQueryf(vi, "CAL:VAL?\n", "%lg%*t", voltage);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
  } else
    errStatus = hpe1564_regCalValue_Q(vi, thisPtr, voltage);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_configure
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function will configure a channel (1 through 4 ) to take
 *           readings. The expectMax value parameter specified should be the
 *           maximum expected measurement value.  The voltage range is set
 *           according to the expected value supplied.  If the value is
 *           greater than 98% of the instrument's range, the next higher
 *           range is automatically chosen. The following table gives the
 *           crossover points for range changes.  Note that a full scale
 *           reading on a given range may actually be an overload condition.
 *             Maximum Expected Value   Voltage Range    Resolution
 *                     0.06125             0.0625        .000007629
 *                     0.24500             0.2500        .000030518
 *                     0.98000             1.0000        .000122070
 *                     3.92000             4.0000        .000488281
 *                    15.68000            16.0000        .001953125
 *                    62.72000            64.0000        .007812500
 *                   250.88000           256.0000        .031250000
 *           
 *            The numSamples parameter specifies how many total readings will
 *           be taken (the actual maximum value allowed will depend on which
 *           memory option is present on the module - 4, 8, 16, or 32
 *           Megabytes).  The numPreTriggers parameter specifies how many of
 *           those readings will take place before the trigger event occurs. 
 *           The total number of readings is limited to a maximum of 4194304
 *           with the 32 Megabyte memory option. The following table
 *           describes the maximum reading limits for the different memory options:
 *           
 *           Memory Size       Maximum numSamples
 *            4 MBytes           524288
 *            8 MBytes          1048576
 *           16 MBytes          2097152
 *           32 MBytes          4194304
 *           
 *            The value of numPreTriggers can range from 0 to (numSamples -
 *           1).
 *            Note that the numSamples and numPreTriggers settings are common
 *            to all channels.  All channels will be set to whatever
 *            values were most recently sent to the instrument.
 *           
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 channel
 * IN        
 *            Specifies which channel to configure.
 *            The channels are defined as macros and are shown below:
 * 
 *      Macro Name         Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_CHAN1          1  1
 *      hpe1564_CHAN2          2  2
 *      hpe1564_CHAN3          3  3
 *      hpe1564_CHAN4          4  4
 * 
 * PARAM 3 : ViReal64 expectMax
 * IN        
 *            Specifies the absolute value of the largest magnitude voltage
 *            expected for the specified channel.
 * 
 *      MAX = hpe1564_VOLT_MAX   256.0001
 *      MIN = hpe1564_VOLT_MIN   -256.0001
 * 
 * PARAM 4 : ViInt32 numSamples
 * IN        
 *            Specifies the total number of readings to take.
 * 
 *      MAX = hpe1564_SAMP_COUNT_MAX   4000000
 *      MIN = hpe1564_SAMP_COUNT_MIN   1
 * 
 * PARAM 5 : ViInt32 numPreTriggers
 * IN        
 *            Specifies how many of the total readings will be taken before
 *           (pre) the trigger event occurs.
 * 
 *      MAX = hpe1564_PRETRIG_MAX   3999999
 *      MIN = hpe1564_PRETRIG_MIN   0
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_configure(ViSession vi,
				    ViInt16 channel,
				    ViReal64 expectMax, ViInt32 numSamples, ViInt32 numPreTriggers)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_configure");

  hpe1564_CHK_INT_RANGE(channel, 1, 4, VI_ERROR_PARAMETER2);
  hpe1564_CHK_REAL_RANGE(expectMax, hpe1564_VOLT_MIN, hpe1564_VOLT_MAX, VI_ERROR_PARAMETER3);

  hpe1564_CHK_LONG_RANGE(numSamples, hpe1564_SAMP_COUNT_MIN, hpe1564_SAMP_COUNT_MAX,
			 VI_ERROR_PARAMETER4);

  hpe1564_CHK_LONG_RANGE(numPreTriggers, hpe1564_PRETRIG_MIN, hpe1564_PRETRIG_MAX,
			 VI_ERROR_PARAMETER5);

  /* make sure numSamps > numPreTriggers */
  hpe1564_CHK_LONG_RANGE(numPreTriggers, 0, (numSamples - 1), VI_ERROR_PARAMETER3);

  /* round expectMax to proper range */
  if (expectMax < 0.0)
    expectMax *= -1.0;		/* make it positive */

  if (expectMax <= 0.061250)
    expectMax = 0.062;
  else if (expectMax <= 0.2450)
    expectMax = 0.245;
  else if (expectMax <= 0.980)
    expectMax = 0.980;
  else if (expectMax <= 3.920)
    expectMax = 3.920;
  else if (expectMax <= 15.680)
    expectMax = 15.680;
  else if (expectMax <= 62.720)
    expectMax = 62.720;
  else
    expectMax = 250.9;

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406) {
    errStatus = viPrintf(vi, ":SWE:OFFS:POIN %ld;:SWE:POIN %ld\n", -numPreTriggers, numSamples);

    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* Range, Filter, and Input state relay are all in the same
       hardware register, and a settling delay of 15 milliseconds
       occurs when writing this register, so make sure it only gets
       updated once here, instead of 3 times.  This is done by sending
       one long string in the downloaded case.
     */
    /* send together so pmt causes only 1 settling delay */
    errStatus = viPrintf(vi, "VOLT%hd:RANG %lf;:INP%hd:FILT:STAT OFF;:INP%hd ON\n",
			 channel, expectMax, channel, channel);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viPrintf(vi, "TRIG:MODE NORM;SOUR1 IMM;SOUR2 HOLD\n");
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viPrintf(vi, "SAMP:SOUR TIM;TIM 1.3e-6\n");
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* We don't have the copy of the E1406 state here, so we don't
       know if any of the 7 TTL trig lines are enabled for output.
       Rather than burn the I/O time querying 7 lines for state,
       just force one ON for BOTH sample and trigger pulses (this turns
       any others off).  Then, if we now turn this one OFF, outputs are
       disabled on all lines. 
     */
    errStatus = viPrintf(vi, "OUTP:TTLT0:SOUR BOTH;STAT ON\n");
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viPrintf(vi, "OUTP:TTLT0:STAT OFF\n");
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

  } else {
    /* don't bother checking return status here, only setting
       conflicts will happen and they should resolve correctly
     */
    (void)hpe1564_regSampCount(vi, thisPtr, numSamples);
    (void)hpe1564_regPreSampCount(vi, thisPtr, numPreTriggers);
    (void)hpe1564_regSampCountPmt(vi, thisPtr);

    /* Range, Filter, and Input state relay are all in the same
       hardware register, and a settling delay of 20 milliseconds
       occurs when writing this register, so make sure it only gets
       updated once here, instead of 3 times.  This is done by sending
       one long string in the e1406 branch above.
     */
    /* send together so pmt causes only 1 settling delay */
    (void)hpe1564_regInpRange(vi, thisPtr, channel, expectMax);

    /* if input state is off, then connect the input */
    if (thisPtr->digState.inpStat[channel - 1] == 0)
      (void)hpe1564_regInpStat(vi, thisPtr, channel, hpe1564_ON);

    /* if filter is ON, then turn it off */
    if (thisPtr->digState.inpFiltStat[channel - 1])
      (void)hpe1564_regInpFiltStat(vi, thisPtr, channel, hpe1564_OFF);

    (void)hpe1564_regInputPmt(vi, thisPtr);

    /* set sample and trigger sources */
    (void)hpe1564_regSampSour(vi, thisPtr, hpe1564_SAMP_TIM);
    (void)hpe1564_regSampTim(vi, thisPtr, 1.3e-6);

    (void)hpe1564_regTrigMode(vi, thisPtr, hpe1564_TRIG_MODE_NORMAL);
    (void)hpe1564_regTrigSour(vi, thisPtr, 1, hpe1564_TRIG_IMM);
    (void)hpe1564_regTrigSour(vi, thisPtr, 2, hpe1564_TRIG_HOLD);

    /* disable any outputs */
    if (thisPtr->digState.outpTtltTrig != TTL_STATE_OFF)
      thisPtr->digState.outpTtltTrig = TTL_STATE_OFF;

    if (thisPtr->digState.outpTtltSamp != TTL_STATE_OFF)
      thisPtr->digState.outpTtltSamp = TTL_STATE_OFF;

    /* resolve the trigger, sample, output couplings */
    (void)hpe1564_regTrigPmt(vi, thisPtr);

  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_configure_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function will query any of the 4 channels for its current
 *            settings, indicating the voltage range, the total number of
 *           readings to take (number of samples), and the number of readings
 *           (of the total number) which will be taken before the trigger
 *           event.  If this query is sent during a continuous measurement,
 *           the numSamples parameter will be set to -1.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 channel
 * IN        
 *            Specifies which channel to query.
 *           
 *            The channels are defined as macros and are shown below:
 * 
 *      Macro Name         Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_CHAN1          1  1
 *      hpe1564_CHAN2          2  2
 *      hpe1564_CHAN3          3  3
 *      hpe1564_CHAN4          4  4
 * 
 * PARAM 3 : ViPReal64 range
 * OUT       
 *            Returns the voltage range of the specified channel.
 * 
 * PARAM 4 : ViPInt32 numSamples
 * OUT       
 *            Returns the total number of readings that will be taken.
 * 
 * PARAM 5 : ViPInt32 numPreTriggers
 * OUT       
 *            Returns how many readings will be taken before the trigger
 *           event.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_configure_Q(ViSession vi,
				      ViInt16 channel,
				      ViPReal64 range, ViPInt32 numSamples, ViPInt32 numPreTriggers)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_configure_Q");

  hpe1564_CHK_INT_RANGE(channel, 1, 4, VI_ERROR_PARAMETER2);
  if (thisPtr->e1406) {
    ViReal64 bigTrig;

    errStatus = viPrintf(vi, "VOLT%hd:RANG?\n", channel);
    if (errStatus < VI_SUCCESS) {
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    }
    errStatus = viScanf(vi, "%lg%*t", range);
    if (errStatus < VI_SUCCESS) {
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = viPrintf(vi, "SAMP:COUN?\n");
    if (errStatus < VI_SUCCESS) {
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    }
    errStatus = viScanf(vi, "%lg%*t", &bigTrig);
    if (errStatus < VI_SUCCESS) {
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = viPrintf(vi, "SAMP:PRET:COUN?\n");
    if (errStatus < VI_SUCCESS) {
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    }
    errStatus = viScanf(vi, "%ld%*t", numPreTriggers);
    if (errStatus < VI_SUCCESS) {
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    }

    if (bigTrig < 180000000)	/* The instrument could be set to 9.9E37 */
      *numSamples = (ViInt32) (bigTrig + 0.1);	/* needed for CVI round off */
    else
      *numSamples = -1;		/* Driver doesn't support INF samples */

  } else {

    errStatus = hpe1564_regInpRange_Q(vi, thisPtr, channel, range);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = hpe1564_regSampCount_Q(vi, thisPtr, numSamples);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = hpe1564_regPreSampCount_Q(vi, thisPtr, numPreTriggers);
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_diagDacGain
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function writes the specified data to the calibration gain
 *           DAC. NOTE: There must be a signal on the input for this to work
 *           properly.
 *            A positive full scale input combined with a DAC value of 255
 *           will result in +2.5 volts output from the DAC.
 *           
 *            A negative full scale input combined with a DAC value of 255
 *           will result in -2.5 volts output from the DAC.
 *           
 *            A DAC value of 0 combined with a 0 voltage input will result in
 *           0 Volts output.
 *           
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 channel
 * IN        
 *            Specifies which channel to configure.
 *            The channels are defined as macros and are shown below:
 * 
 *      Macro Name         Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_CHAN1          1  1
 *      hpe1564_CHAN2          2  2
 *      hpe1564_CHAN3          3  3
 *      hpe1564_CHAN4          4  4
 * 
 * PARAM 3 : ViInt16 data
 * IN        
 *            This is the data to send to the specified register.
 * 
 *      MAX = hpe1564_DAC_GAIN_MAX   255
 *      MIN = hpe1564_DAC_GAIN_MIN   0
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_diagDacGain(ViSession vi, ViInt16 chan, ViInt16 data)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_diagDacGain");

  hpe1564_CHK_INT_RANGE(chan, hpe1564_CHAN1, hpe1564_CHAN4, VI_ERROR_PARAMETER2);

  hpe1564_CHK_INT_RANGE(data, hpe1564_DAC_GAIN_MIN, hpe1564_DAC_GAIN_MAX, VI_ERROR_PARAMETER3);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406)
    errStatus = viPrintf(vi, "DIAG:DAC%hd:GAIN %hd\n", chan, data);
  else
    errStatus = hpe1564_regDacGain(vi, thisPtr, chan, data);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_diagDacOffs
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function writes the specified data to the calibration
 *           offset DAC. NOTE:  This routine does not short the input.  Use
 *           the hpe1564_diagShort command to short the input before calling
 *           this routine.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 channel
 * IN        
 *            Specifies which channel to configure.
 *            The channels are defined as macros and are shown below:
 * 
 *      Macro Name         Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_CHAN1          1  1
 *      hpe1564_CHAN2          2  2
 *      hpe1564_CHAN3          3  3
 *      hpe1564_CHAN4          4  4
 * 
 * PARAM 3 : ViInt16 data
 * IN        
 *            This is the data to send to the specified register.
 * 
 *      MAX = hpe1564_DAC_OFFS_MAX   255
 *      MIN = hpe1564_DAC_OFFS_MIN   0
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_diagDacOffs(ViSession vi, ViInt16 chan, ViInt16 data)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_diagDacOffs");

  hpe1564_CHK_INT_RANGE(chan, hpe1564_CHAN1, hpe1564_CHAN4, VI_ERROR_PARAMETER2);

  hpe1564_CHK_INT_RANGE(data, hpe1564_DAC_OFFS_MIN, hpe1564_DAC_OFFS_MAX, VI_ERROR_PARAMETER3);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406)
    errStatus = viPrintf(vi, "DIAG:DAC%hd:OFFS %hd", chan, data);
  else
    errStatus = hpe1564_regDacOffset(vi, thisPtr, chan, data);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_diagDacOffsRamp
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function outputs a ramp of DAC values from 0 to 255, with
 *           the DAC code changing about every 100 microseconds.  The "count"
 *           variable is how many ramps to output.  The timing is such that
 *           about 37.35 full ramps are output each second.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 channel
 * IN        
 *            Specifies which channel to configure.
 *            The channels are defined as macros and are shown below:
 * 
 *      Macro Name         Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_CHAN1          1  1
 *      hpe1564_CHAN2          2  2
 *      hpe1564_CHAN3          3  3
 *      hpe1564_CHAN4          4  4
 * 
 * PARAM 3 : ViInt16 count
 * IN        
 *            This is the number of ramps to put out.  A count of 2240 will
 *            output ramps for approximately 60 seconds.
 * 
 *      MAX = hpe1564_RAMP_COUNT_MAX   32767
 *      MIN = hpe1564_RAMP_COUNT_MIN   1
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_diagDacOffsRamp(ViSession vi, ViInt16 chan, ViInt16 count)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_diagDacOffsRamp");

  hpe1564_CHK_INT_RANGE(chan, hpe1564_CHAN1, hpe1564_CHAN4, VI_ERROR_PARAMETER2);

  hpe1564_CHK_INT_RANGE(count, hpe1564_RAMP_COUNT_MIN, hpe1564_RAMP_COUNT_MAX, VI_ERROR_PARAMETER3);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406)
    errStatus = viPrintf(vi, "DIAG:DAC%hd:OFFS:RAMP %hd", chan, count);
  else
    errStatus = hpe1564_regDacOffsRamp(vi, thisPtr, chan, count);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_diagDacSour
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function outputs the specified voltage from the internal
 *           calibration source DAC onto the calibration bus.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * 
 * PARAM 2 : ViReal64 voltage
 * IN        
 *            This is the voltage to output from the internal DAC.  The
 *            DAC can output voltages from -15.0 to +15.0 volts.
 * 
 *      MAX = hpe1564_DAC_SOUR_MAX   15.0
 *      MIN = hpe1564_DAC_SOUR_MIN   -15.0
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_diagDacSour(ViSession vi, ViReal64 voltage)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_diagDacSour");

  hpe1564_CHK_REAL_RANGE(voltage, hpe1564_DAC_SOUR_MIN, hpe1564_DAC_SOUR_MAX, VI_ERROR_PARAMETER2);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406)
    errStatus = viPrintf(vi, "DIAG:DAC:SOUR %hd", voltage);
  else
    errStatus = hpe1564_regDacSour(vi, thisPtr, voltage);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_diagDacSourRamp
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function outputs a ramp of DAC values from 0 to 4095, with
 *           the DAC code changing about every 100 microseconds.  The signal
 *           will be connected to the input relay of the specified channel
 *           until the count is completed, at which time the relay will be
 *           returned to its previous setting (which may be the same
 *           setting).  The "count" variable specifies how many ramps to
 *           output.  The timing is such that about 2.3257 full ramps are
 *           output each second.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 channel
 * IN        
 *            Specifies which channel will be calibrated.
 *           
 *            The channels are defined as macros and are shown below:
 * 
 *      Macro Name         Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_CHAN1          1  1
 *      hpe1564_CHAN2          2  2
 *      hpe1564_CHAN3          3  3
 *      hpe1564_CHAN4          4  4
 * 
 * PARAM 3 : ViInt16 count
 * IN        
 *            This is the number of ramps to put out.  A count of 139 will
 *            output ramps for just under 60 seconds.
 * 
 *      MAX = hpe1564_RAMP_COUNT_MAX   32767
 *      MIN = hpe1564_RAMP_COUNT_MIN   1
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_diagDacSourRamp(ViSession vi, ViInt16 count)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_diagDacSourRamp");

  hpe1564_CHK_INT_RANGE(count, hpe1564_RAMP_COUNT_MIN, hpe1564_RAMP_COUNT_MAX, VI_ERROR_PARAMETER2);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406)
    errStatus = viPrintf(vi, "DIAG:DAC:SOUR:RAMP %hd", count);
  else
    errStatus = hpe1564_regDacSourRamp(vi, thisPtr, count);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_diagFlashRead_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function returns the calibration constants, a ten digit
 *           serial number, and a three digit date code.  Each digit of the
 *           serial number occupies one location in flash, as does each digit
 *           of the date code.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt32 arrayLength
 * IN        
 *            Specifies the length of the flashData array in total number of
 *           readings. An error will be reported if arrayLength is less than
 *           269.  If arrayLength is > 269, no error will occur, but only 269
 *           elements will be filled.
 * 
 *      MAX = hpe1564_FLASH_ARRAY_MAX   32767
 *      MIN = hpe1564_FLASH_ARRAY_MIN   269
 * 
 * PARAM 3 : ViInt16 _VI_FAR flashData[]
 * OUT       
 *            269 integers are returned in this array.  The first 256 are the
 *           calibration constants, the last 13 are a ten digit serial number
 *           and a three digit date code respectively.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_diagFlashRead_Q(ViSession vi,
					  ViInt16 arrayLength, ViInt16 _VI_FAR flashData[])
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;
  ViInt16 ii;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (VI_SUCCESS > errStatus) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_diagFlashRead_Q");

  /* Perform Error Checking on Each Parameter */
  hpe1564_CHK_LONG_RANGE(arrayLength, hpe1564_FLASH_ARRAY_MIN, hpe1564_FLASH_ARRAY_MAX,
			 VI_ERROR_PARAMETER2);

  if (thisPtr->e1406) {
    ViInt32 howMany;

    errStatus = viPrintf(vi, "DIAG:FLAS:DATA?\n");

    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* last parameter indicates to byte swap the readings */
#ifdef BIG_ENDIAN
    errStatus = hpe1564_scan16(vi, arrayLength, (char *)flashData, &howMany, 0);
#else
    errStatus = hpe1564_scan16(vi, arrayLength, (char *)flashData, &howMany, 1);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
#endif
  } else {
    /* places the data into the calArray */
    errStatus = hpe1564_regFlashRead_Q(vi, thisPtr);

    /* now copy the data out of the calArray */
    for (ii = 0; ii < FLASH_DATA_SIZE; ii++)
      flashData[ii] = thisPtr->digState.calArray[ii];
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_diagFlashWrite
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function writes data into the flash rom.  The data array
 *           must contain 256 calibration constants, followed by 13 integers
 *           which are assumed to be a ten digit serial number and 3 digit
 *           date code.  Only the lower 8 bits are used in each integer.  If
 *           the doCheckSum flag is 1 (VI_TRUE) then a new checksum is
 *           calculated for the first 256 elements and saved into the flash
 *           rom.
 *            NOTE: If the calibration constants are changed, you MUST
 *           calculate a new checksum.  If you do not, a self test error will
 *           occur because the data in flash rom will no longer agree with
 *           the old checksum that self test compares against.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViBoolean doCheckSum
 * IN        
 *            Specifies whether or not to calculate and store a new checksum
 *           with the data.  Only the calibration constants (elements 1
 *           through 256 of the array) are used to compute the checksum.
 *           
 *            NOTE: If any of the calibration constants were modified, a new
 *           checksum MUST be calculated or a self test error will result
 *           when self test is run.
 * 
 *      MAX = hpe1564_ON   1
 *      MIN = hpe1564_OFF   0
 * 
 * PARAM 3 : ViInt16 _VI_FAR flashData[]
 * IN        
 *            The 269 integers which will be written to the flash rom.  Only
 *           the lower 8 bits of each word will be written.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_diagFlashWrite(ViSession vi,
					 ViBoolean doCheckSum, ViInt16 _VI_FAR flashData[])
{
  ViStatus errStatus = 0;
  ViInt16 ii;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (VI_SUCCESS > errStatus) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_diagFlashWrite");

  /* Perform Error Checking on Each Parameter */
  hpe1564_CHK_BOOLEAN(doCheckSum, VI_ERROR_PARAMETER2);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406) {
    errStatus = viPrintf(vi, "DIAG:FLAS:DATA %hd, %269hb\n", doCheckSum, flashData);

    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

  } else {
    /* copy data into calArray */
    for (ii = 0; ii < FLASH_DATA_SIZE; ii++)
      thisPtr->digState.calArray[ii] = flashData[ii];

    /* now write it to flash */
    errStatus = hpe1564_regFlashWrite(vi, thisPtr, doCheckSum);
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_diagIntLine
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  CAUTION:  This command is seldom needed, and care should be
 *           taken when using it.  Specifying an improper interrupt line will
 *           cause the CLOSe, OPEN, and SCAN commands to "hang" or time out
 *           because the controller will not receive the expected interrupt
 *           on the expected line.
 *            hpe1564_diagIntLine selects which interrupt line (none if 0)
 *           the card will interrupt upon.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 intLine
 * IN        
 *            Numeric value of the interrupt line to be enabled.  Note that
 *           enabling one line disables the previous setting.  Also, a value
 *           of 0 is equivalent to NO INTERRUPT or OFF.
 * 
 *      MAX = hpe1564_INTR_MAX   7
 *      MIN = hpe1564_INTR_MIN   0
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_diagIntLine(ViSession vi, ViInt16 intLine)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_diagIntLine");

  hpe1564_CHK_INT_RANGE(intLine, hpe1564_INTR_MIN, hpe1564_INTR_MAX, VI_ERROR_PARAMETER2);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406) {
    errStatus = viPrintf(vi, "DIAG:INT:LINE %hd\n", intLine);
  } else {
    errStatus = hpe1564_regDiagInt(vi, thisPtr, intLine);
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_diagIntLine_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  hpe1564_diagIntLine_Q returns the currently active interrupt
 *           line.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViPInt16 intLine
 * OUT       
 *            Numeric value returned here is the active interrupt line.  A
 *            return value of 0 indicates interrupts are off on all lines.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_diagIntLine_Q(ViSession vi, ViPInt16 intLine)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_diagIntLine_Q");

  if (thisPtr->e1406) {
    errStatus = viQueryf(vi, "DIAG:INT:LINE?\n", "%hd", intLine);
  } else {
    errStatus = hpe1564_regDiagInt_Q(vi, thisPtr, intLine);
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_diagMemSize
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This command is used to specify the memory size when a
 *           different memory SIMM has been added to the card.  The switche
 *           settings for FLASH and CALIBRATION CONSTANTS must be set to the
 *           Write Enable positions for this routine to work properly.
 *           
 *            The new setting will take effect until the instrument is reset
 *           with the hpe1564_reset command or power is cycled on the
 *           instrument.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt32 memSize
 * IN        
 *            Specifies the amount of memory in bytes.  This can either be
 *           the approximate size (i.e. 16.0E6) or the actual size (i.e.
 *           16777216); either number will be accepted as 16 MegaBytes of
 *           memory.
 * 
 *      MAX = hpe1564_MEM_SIZE_MAX   134217728
 *      MIN = hpe1564_MEM_SIZE_MIN   3800000
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_diagMemSize(ViSession vi, ViInt32 memSize)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_diagMemSize");

  hpe1564_CHK_LONG_RANGE(memSize, hpe1564_MEM_SIZE_MIN, hpe1564_MEM_SIZE_MAX, VI_ERROR_PARAMETER2);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406) {
    errStatus = viPrintf(vi, "DIAG:MEM:SIZE %ld\n", memSize);
  } else {
    errStatus = hpe1564_regDiagMem(vi, thisPtr, memSize);
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_diagMemSize_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function returns the size of memory in bytes.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViPInt32 memSize
 * OUT       
 *            Numeric value returned here is the memory size in bytes.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_diagMemSize_Q(ViSession vi, ViPInt32 memSize)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_diagMemSize_Q");

  if (thisPtr->e1406) {
    errStatus = viQueryf(vi, "DIAG:MEM:SIZE?\n", "%ld", memSize);
  } else {
    errStatus = hpe1564_regDiagMem_Q(vi, thisPtr, memSize);
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_diagPeek_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function queries the specified register and returns its
 *           contents.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 regNum
 * IN        
 *            This is the register number.  The register number is the
 *            offset, in words, into A16 space.  For example, the Status
 *            register is number 2, and the Fifo high word register is
 *            number 6.
 * 
 *      MAX = hpe1564_REG_ADDR_MAX   31
 *      MIN = hpe1564_REG_ADDR_MIN   0
 * 
 * PARAM 3 : ViPInt16 data
 * OUT       
 *            The contents of the register are returned in this parameter.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_diagPeek_Q(ViSession vi, ViInt16 regNum, ViPInt16 data)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_diagPeek_Q");

  hpe1564_CHK_INT_RANGE(regNum, hpe1564_REG_ADDR_MIN, hpe1564_REG_ADDR_MAX, VI_ERROR_PARAMETER2);

  if (thisPtr->e1406)
    errStatus = viQueryf(vi, "DIAG:PEEK? %hd\n", "%hd%*t", regNum, data);
  else
    errStatus = hpe1564_regPeek_Q(vi, thisPtr, regNum, data);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_diagPoke
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function writes the specified data to the specified
 *           register. NOTE: Be sure you know what you are doing if you use
 *           this command.  The driver keeps state information, and you are
 *           changing state behind its back here, so the driver can, and
 *           probably will, get out of sync. with the hardware if you use
 *           this command.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 regNum
 * IN        
 *            This is the register number.  The register number is the
 *            offset, in words, into A24 space.  For example, the Status
 *            register is number 2, and the Fifo high word register is
 *            number 6.
 * 
 *      MAX = hpe1564_REG_ADDR_MAX   31
 *      MIN = hpe1564_REG_ADDR_MIN   0
 * 
 * PARAM 3 : ViInt16 data
 * IN        
 *            This is the data to send to the specified register.
 * 
 *      MAX = hpe1564_REG_DATA_MAX   32767
 *      MIN = hpe1564_REG_DATA_MIN   -32768
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_diagPoke(ViSession vi, ViInt16 regNum, ViInt16 data)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_diagPoke");

  hpe1564_CHK_INT_RANGE(regNum, hpe1564_REG_ADDR_MIN, hpe1564_REG_ADDR_MAX, VI_ERROR_PARAMETER2);

  if (thisPtr->e1406)
    errStatus = viPrintf(vi, "DIAG:POKE %hd, %hd\n", regNum, data);
  else
    errStatus = hpe1564_regPoke(vi, thisPtr, regNum, data);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_diagShort
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function controls whether the input on the channel is
 *           shorted or not.  The short remains in effect until a reset or
 *           until it is disabled with another hpe1564_diagShort command.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 channel
 * IN        
 *            Specifies which channel will be affected.
 *           
 *            The channels are defined as macros and are shown below:
 * 
 *      Macro Name         Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_CHAN1          1  1
 *      hpe1564_CHAN2          2  2
 *      hpe1564_CHAN3          3  3
 *      hpe1564_CHAN4          4  4
 * 
 * PARAM 3 : ViBoolean state
 * IN        
 *            This is the desired state: 0 = OFF; 1 = ON.  Macros for these
 *            settings are shown below.
 * 
 *      MAX = hpe1564_ON   1
 *      MIN = hpe1564_OFF   0
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_diagShort(ViSession vi, ViInt16 channel, ViBoolean state)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_diagShort");

  hpe1564_CHK_INT_RANGE(channel, 1, 4, VI_ERROR_PARAMETER2);
  hpe1564_CHK_BOOLEAN(state, VI_ERROR_PARAMETER3);

  if (thisPtr->e1406)
    errStatus = viPrintf(vi, "diag:short%hd %hd\n", channel, state);
  else
    errStatus = hpe1564_regDiagShort(vi, thisPtr, channel, state);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_diagTest_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function returns the results of a self test in a test
 *            by test manner.  Each element in the array contains the
 *            result of that test number --- mumble mumble.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt32 arrayLength
 * IN        
 *            Specifies the length of the data array.  The array must be at
 *           least mumble elements in size.
 * 
 *      MAX = hpe1564_TEST_ARRAY_MAX    16000000 
 *      MIN = hpe1564_TEST_ARRAY_MIN    100 
 * 
 * PARAM 3 : ViReal64 _VI_FAR results[]
 * OUT       
 *            The array of test results is returned in this parameter.
 *            See "Description" for more details on the format of the
 *            results.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_diagTest_Q(ViSession vi, ViInt32 arrayLength, ViInt16 _VI_FAR results[])
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_diagTest_Q");

  hpe1564_CHK_LONG_RANGE(arrayLength, hpe1564_TEST_ARRAY_MIN, hpe1564_TEST_ARRAY_MAX,
			 VI_ERROR_PARAMETER2);

  if (thisPtr->e1406) {
    ViInt32 howMany;

    errStatus = viPrintf(vi, "TEST:TST?\n");

    /* last parameter indicates to byte swap the readings */
#ifdef BIG_ENDIAN
    errStatus = hpe1564_scan16(vi, arrayLength, (char *)results, &howMany, 0);
#else
    errStatus = hpe1564_scan16(vi, arrayLength, (char *)results, &howMany, 1);
#endif
  } else
    errStatus = hpe1564_regTestTst_Q(vi, thisPtr, results);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_diagTestNum_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function runs the specified test once, or multiple times,
 *           depending upon the value of the "iterations" parameter.  An
 *           integer is returned which contains the number of times the test
 *           failed (0 is returned if it passed all iterations).
 *           
 *            If the test failed (returned non-zero in result parameter),
 *           more information about the most recent failure can be obtained
 *           using the hpe1564_diagTestErr_Q function.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 testNum
 * IN        
 *            Specifies the test number to obtain detailed information about.
 *           mumble final number here in MAX.
 * 
 *      MAX = hpe1564_TEST_NUM_MAX   100
 *      MIN = hpe1564_TEST_NUM_MIN   1
 * 
 * PARAM 3 : ViInt16 iterations
 * IN        
 *            Specifies the number of times to run the test before bringing
 *           back the results.
 * 
 *      MAX = hpe1564_TEST_RUNS_MAX   32767
 *      MIN = hpe1564_TEST_RUNS_MIN   1
 * 
 * PARAM 4 : ViPInt16 failures
 * OUT       
 *            The integer returned here contains the number of times the test
 *           failed.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_diagTestNum_Q(ViSession vi,
					ViInt16 testNum, ViInt16 iterations, ViPInt16 failures)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_diagTestNum_Q");

  hpe1564_CHK_INT_RANGE(testNum, 1, hpe1564_TESTS_IMPLEMENTED, VI_ERROR_PARAMETER2);
  if (iterations == 0)
    hpe1564_LOG_STATUS(vi, thisPtr, VI_ERROR_PARAMETER3);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406) {
    errStatus = viQueryf(vi, "TEST:NUMB? %hd, %hd\n", "%hd%*t", testNum, iterations, failures);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, 0, errStatus);

  } else {
    /* allocate some memory for use by self test */
    thisPtr->memPtr = malloc(400 * sizeof(ViInt16));
    errStatus = hpe1564_regTestNum_Q(vi, thisPtr, testNum, iterations, failures);

    /* free the memory we were using */
    free(thisPtr->memPtr);
    thisPtr->memPtr = NULL;
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_diagTestErr_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function obtains more information about a particular test
 *           of the self test suite.  An error code and information string
 *           are returned.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 testNum
 * IN        
 *            Specifies the test number to obtain detailed information about.
 *           mumble final number here in MAX.
 * 
 *      MAX = hpe1564_TEST_NUM_MAX   100
 *      MIN = hpe1564_TEST_NUM_MIN   1
 * 
 * PARAM 3 : ViPInt16 diagCode
 * OUT       
 *           A diagnostic code giving more information about the type of
 *           failure is returned in this parameter.  See the service manual for
 *           information about the meaning of each diagnostic code.
 * 
 * PARAM 4 : ViChar _VI_FAR test_message[]
 * OUT       
 *            A message giving more detailed information about the failure is
 *           returned in this string.  This string must be at least 80
 *           characters in length.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_diagTestErr_Q(ViSession vi,
					ViInt16 testNum,
					ViPInt16 diagCode, ViChar _VI_FAR test_message[])
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;
  char *mssgPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_diagTestErr_Q");

  hpe1564_CHK_INT_RANGE(testNum, 1, hpe1564_TESTS_IMPLEMENTED, VI_ERROR_PARAMETER2);

  if (thisPtr->e1406) {
    errStatus = viQueryf(vi, "TEST:ERR? %hd\n", "%hd,%s%*t", testNum, diagCode, test_message);

  } else {
    errStatus = hpe1564_regTestErr_Q(vi, thisPtr, testNum, diagCode, &mssgPtr);

    sprintf(test_message, "%s", mssgPtr);
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_fetchAll_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function is the fastest way to get data out of the
 *           digitizer. Raw data from all four channels is returned in an
 *            interleaved fashion -- that is, the array contains the first
 *            reading from each channel, followed by the second reading from
 *           each channel, and so on.  Each reading is brought back as a 16
 *           bit integer that represents a voltage.  To convert the raw
 *           readings to voltages, use either of the two methods shown below:
 *           
 *            voltage = reading * range/32768
 *           
 *            voltage = reading * resolution
 *           
 *            Where resolution may be obtained from the
 *            hpe1564_resolution_Q() command, and range from the
 *            hpe1564_input_Q() or hpe1564_rang_Q() commands.
 *           
 *            NOTE: Because the instrument supports continuous measurements,
 *           the memory is a fifo (first in first out), and the read is a
 *           destructive read.  Therefore, it is not possible to read the
 *           same data multiple times even for a non-continuous measurement.
 *           
 *            See the help file for more information.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt32 arrayLength
 * IN        
 *            Specifies the total amount of data to bring back.  Normally
 *           this would be equal to four times the sample count (since all 4
 *           channels are brought back interleaved).  If the instrument has
 *           more than "arrayLength" data points in memory, only
 *           "arrayLength" points will be brought back.
 * 
 *      MAX = hpe1564_FETCH_ALL_MAX   16000000
 *      MIN = hpe1564_FETCH_ALL_MIN   4
 * 
 * PARAM 3 : ViInt16 _VI_FAR data[]
 * OUT       
 *            Pointer to the array of 16 bit integers which will hold the A/D
 *           reading values. The data points returned are interleaved -- that
 *           is, the array contains the first reading from each channel,
 *           followed by the second reading from each channel, etc.
 * 
 * PARAM 4 : ViPInt32 howMany
 * OUT       
 *            Specifies how many readings were actually placed into the data
 *            array.  This is useful when using one large array to cover
 *           several different measurements that vary in sample count, or
 *           when fetching out data in "blocks", where the last fetch of data
 *           may contain only a remainder of data and not a full block.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_fetchAll_Q(ViSession vi,
				     ViInt32 arrayLength, ViInt32 _VI_FAR data[], ViPInt32 howMany)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_fetchAll_Q");

  hpe1564_CHK_LONG_RANGE(arrayLength, hpe1564_FETCH_ALL_MIN, hpe1564_FETCH_ALL_MAX,
			 VI_ERROR_PARAMETER2);

  /* Format is set to PACKed when session is opened via hpe1564_init */
  if (thisPtr->e1406) {
    ViBoolean need_comma = 0;
    ViInt32 readingsPerChan;

    readingsPerChan = (arrayLength * 2) / thisPtr->numChans;

    if (thisPtr->digState.dataFormat != hpe1564_FORM_PACK) {
      errStatus = viPrintf(vi, "FORM:DATA PACK\n");
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
      thisPtr->digState.dataFormat = hpe1564_FORM_PACK;
    }

    errStatus = viPrintf(vi, "DATA:ALL? %ld\n", readingsPerChan);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    *howMany = 0;

    /* if device clear, then blow outta here */
    if (thisPtr->device_clear) {
      thisPtr->device_clear = 0;
      return VI_SUCCESS;
    }

    /* last parameter indicates to byte swap the readings */
#ifdef BIG_ENDIAN
    errStatus = hpe1564_scan16(vi, arrayLength * 2, (char *)data, howMany, 0);
#else
    errStatus = hpe1564_scan16(vi, arrayLength * 2, (char *)data, howMany, 1);
#endif
    /* if scan16 timed out, we are hung, so do device clear */
    if (errStatus == VI_ERROR_TMO) {
      viClear(vi);
      *howMany = 0;
    } else
      *howMany = *howMany / 2;
  } else {
    errStatus = hpe1564_regDataAll_Q(vi, thisPtr, arrayLength, data, howMany);
    /* if we had a device_clear, handle it here */
    if (thisPtr->device_clear) {
      hpe1564_regAbort(vi, thisPtr);
      thisPtr->device_clear = 0;
      return VI_SUCCESS;
    }
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_fetch_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function brings back raw data from the specified
 *            channels.  In addition, this routine sorts the interleaved
 *           instrument data into individual arrays for the specified
 *           channels.  Each reading is brought back as a 16 bit integer that
 *           represents a voltage.  To convert the raw readings to voltages,
 *           use either of the two methods shown below:
 *           
 *            voltage = reading * range/32768
 *           
 *            voltage = reading * resolution
 *           
 *            Where resolution may be obtained from the
 *            hpe1564_resolution_Q() command, and range from the
 *            hpe1564_input_Q() or hpe1564_rang_Q() commands.
 *           
 *            Because it separates the interleaved raw data, this function is
 *            slower than the hpe1564_fetchAll function.  Channel arrays that
 *           are specified as null (0L), will have no data returned.
 *           
 *            NOTE: Measurement data on channels which have an associated
 *           null array pointer (0L), is thrown away by this command and is
 *           not recoverable. Similarly, this command can read the data from
 *           a measurement only once, and it is a destructive read.
 *           
 *            This function will attempt to bring back "arrayLength" readings
 *           for each channel with a non-null array pointer, but will not
 *           error if fewer readings are available.  The number of readings
 *           actually read into the data array is returned in the "howMany"
 *           parameter.
 *            See the help file for more information about this command.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt32 arrayLength
 * IN        
 *            Specifies the number of readings to bring back in each data
 *           array.  The data arrays must be at least this large, or they
 *           must be NULL (0L).
 * 
 *      MAX = hpe1564_FETCH_MAX   4000000
 *      MIN = hpe1564_FETCH_MIN   1
 * 
 * PARAM 3 : ViInt16 _VI_FAR data1[]
 * OUT       
 *            Pointer to the array of 16 bit integers which will hold the A/D
 *           reading values from channel 1.  If a null pointer is given (0L),
 *           no data will be returned for this channel.
 * 
 * PARAM 4 : ViInt16 _VI_FAR data2[]
 * OUT       
 *            Pointer to the array of 16 bit integers which will hold the A/D
 *           reading values from channel 2.  If a null pointer is given (0L),
 *           no data will be returned for this channel.
 * 
 * PARAM 5 : ViInt16 _VI_FAR data3[]
 * OUT       
 *            Pointer to the array of 16 bit integers which will hold the A/D
 *           reading values from channel 3.  If a null pointer is given (0L),
 *           no data will be returned for this channel.
 * 
 * PARAM 6 : ViInt16 _VI_FAR data4[]
 * OUT       
 *            Pointer to the array of 16 bit integers which will hold the A/D
 *           reading values from channel 4.  If a null pointer is given (0L),
 *           no data will be returned for this channel.
 * 
 * PARAM 7 : ViPInt32 howMany
 * OUT       
 *            Specifies how many readings were actually placed into each
 *           non-null data array.  This is useful when using one large array
 *           to cover several different measurements that vary in sample
 *           count, or when fetching out data in "blocks" where the last
 *           fetch of data may contain only a remainder of data and not a
 *           full block.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */

 /* data block size for reads below */
#define BLOCK_SIZE	4000

ViStatus _VI_FUNC hpe1564_fetch_Q(ViSession vi,
				  ViInt32 arrayLength,
				  ViInt16 _VI_FAR data1[],
				  ViInt16 _VI_FAR data2[],
				  ViInt16 _VI_FAR data3[],
				  ViInt16 _VI_FAR data4[], ViPInt32 howMany)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  char chan_list[10];
  ViBoolean need_comma = 0;
  ViChar fmtStr[30];
  ViInt16 numRdgs;
  ViInt16 ii;
  ViInt16 dataBlock[BLOCK_SIZE];
  ViInt32 num_bytes;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_fetch_Q");

  hpe1564_CHK_LONG_RANGE(arrayLength, hpe1564_FETCH_MIN, hpe1564_FETCH_MAX, VI_ERROR_PARAMETER2);

  /* Format is set to PACKed when session is opened via hpe1564_init */
  if (thisPtr->e1406) {

    /* initialize chan_list to empty */
    chan_list[0] = '\0';

    if (thisPtr->digState.dataFormat != hpe1564_FORM_PACK) {
      errStatus = viPrintf(vi, "FORM:DATA PACK\n");
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
      thisPtr->digState.dataFormat = hpe1564_FORM_PACK;
    }

    if (data1 != NULL) {
      strcat(chan_list, "1");
      need_comma = 1;
    }

    if (data2 != NULL) {
      if (need_comma)
	strcat(chan_list, ",2");
      else {
	need_comma = 1;
	strcat(chan_list, "2");
      }
    }

    if (data3 != NULL) {
      if (need_comma)
	strcat(chan_list, ",3");
      else {
	need_comma = 1;
	strcat(chan_list, "3");
      }
    }

    if (data4 != NULL) {
      if (need_comma)
	strcat(chan_list, ",4");
      else {
	need_comma = 1;
	strcat(chan_list, "4");
      }
    }

    strcat(chan_list, ")");

    errStatus = viPrintf(vi, "DATA? %ld,(@%s\n", arrayLength, chan_list);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* initialize reading counter */
    *howMany = 0;

    /* if device clear, then blow outta here */
    if (thisPtr->device_clear) {
      thisPtr->device_clear = 0;
      return VI_SUCCESS;
    }

    /* Now we have to bring back the data in "chunks" of dataBlock
       so that we don't double allocate huge arrays when asking for
       lots of data.
     */
    errStatus = hpe1564_readHeader(vi, &num_bytes);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* set fmtStr to large block, don't use %*t */
    sprintf(fmtStr, "%%%ldc", BLOCK_SIZE * 2);

    while (num_bytes > 0) {
      /* if num_bytes > 8000 (dataBlock array), get a chunk */
      if (num_bytes >= BLOCK_SIZE * 2) {
	errStatus = viScanf(vi, fmtStr, (char *)dataBlock);
	if (errStatus < VI_SUCCESS)
	  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

	num_bytes -= BLOCK_SIZE * 2;

	/* update numRdgs for loop below */
	numRdgs = BLOCK_SIZE;
      } else {
	/* note %*t in format string to finish the read */
	sprintf(fmtStr, "%%%ldc%%*t", num_bytes);
	errStatus = viScanf(vi, fmtStr, (char *)dataBlock);
	if (errStatus < VI_SUCCESS)
	  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

	/* update numRdgs for loop below */
	numRdgs = num_bytes / 2;

	/* we've read it all, so set num_bytes to 0 */
	num_bytes = 0;
      }

      /* sort the data into the proper arrays */
      ii = 0;
      while (ii < numRdgs) {
	if (data1 != NULL) {
#ifndef BIG_ENDIAN
	  /* byte swap if not Motorola platform */
	  SWAP_16(&dataBlock[ii]);
#endif
	  data1[*howMany] = dataBlock[ii];
	  ii += 1;
	}

	if (data2 != NULL) {
#ifndef BIG_ENDIAN
	  /* byte swap if not Motorola platform */
	  SWAP_16(&dataBlock[ii]);
#endif
	  data2[*howMany] = dataBlock[ii];
	  ii += 1;
	}

	if (data3 != NULL) {
#ifndef BIG_ENDIAN
	  /* byte swap if not Motorola platform */
	  SWAP_16(&dataBlock[ii]);
#endif
	  data3[*howMany] = dataBlock[ii];
	  ii += 1;
	}

	if (data4 != NULL) {
#ifndef BIG_ENDIAN
	  /* byte swap if not Motorola platform */
	  SWAP_16(&dataBlock[ii]);
#endif
	  data4[*howMany] = dataBlock[ii];
	  ii += 1;
	}

	/* increment how many per channel have been read */
	*howMany += 1;
      }

    }				/* end while (num_bytes > 0) */

  } else {
    /* Cast data pointers to void and specify packed format (parm4 = 0 ) */
    errStatus = hpe1564_regData_Q(vi, thisPtr, arrayLength, 0,
				  (void *)data1, (void *)data2, (void *)data3, (void *)data4,
				  howMany);

    /* if we had a device_clear, handle it here */
    if (thisPtr->device_clear) {
      hpe1564_regAbort(vi, thisPtr);
      thisPtr->device_clear = 0;
      return VI_SUCCESS;
    }
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_fetchCvt_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function brings back the most recently taken reading on
 *           each channel.  The data is returned as 16 bit integers.  To
 *           convert the raw readings to voltages, use either of the two
 *           methods shown below: BEGIN VERBATIM
 *           
 *            voltage = reading * range/32768
 *           
 *            voltage = reading * resolution
 *           
 *            Where resolution may be obtained from the
 *            hpe1564_resolution_Q() command, and range from the
 *            hpe1564_input_Q() or hpe1564_rang_Q() commands.
 *            END VERBATIM
 *           
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 _VI_FAR data[]
 * OUT       
 *            Pointer to the array which will hold the Current Value Table
 *           readings from all channels.  The array must be at least four
 *           elements in size to hold the current reading from each of four
 *           channels.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_fetchCvt_Q(ViSession vi, ViInt16 _VI_FAR data[])
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_fetchCvt_Q");

  if (thisPtr->e1406) {
    ViInt32 howMany;

    if (thisPtr->digState.dataFormat != hpe1564_FORM_PACK) {
      errStatus = viPrintf(vi, "FORM:DATA PACK\n");
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
      thisPtr->digState.dataFormat = hpe1564_FORM_PACK;
    }

    errStatus = viPrintf(vi, "DATA:CVT? (@1:4)\n");
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* last parameter indicates to byte swap the readings */
#ifdef BIG_ENDIAN
    errStatus = hpe1564_scan16(vi, 4, (char *)data, &howMany, 0);
#else
    errStatus = hpe1564_scan16(vi, 4, (char *)data, &howMany, 1);
#endif
  } else
    errStatus = hpe1564_regDataCvt_Q(vi, thisPtr, data);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/************
 * FUNCTION:   hpe1564_scan64()
 * PURPOSE:    Reads an (Indef/Def) Arbitrary Block from input, placing the 
 *             byte array into the callers array. If the caller requests byte
 *             swapping, then swaps all 8byte 64bit words appropriately from
 *             Motorola to Intel format.
 * ALGORITHM:  Read length.
 *             Read Buffer.
 *             if (swapping needed)
 *                 Byte swap 8 byte words.
 * RETURNS: 
 *             VI_ERROR_INV_RESPONSE  - invalid response from instrument.
 *             VI_ERROR_PARAMETER2    - array length too short.
 *             VI_SUCCESS             - no errors.
 * PARAMS:     vi          - VTL session
 *             max_length  - # of float64 vals that can be stored in f64_array
 *             f64_array   - array to store float64 values.
 *             rtn_count  - pointer to count of float64 readings made.
 *             swap        - if == VI_TRUE, then swap bytes to Little Endian.
 ************/
ViStatus hpe1564_scan64(ViSession vi,
			ViInt32 max_length, ViPReal64 f64_array, ViPInt32 rtn_count, ViBoolean swap)
{
  ViStatus rtn_code;
  ViChar c[2];
  ViChar length_str[16];
  ViInt32 nbytes;
  ViUInt32 digits;
  ViInt32 i;
  char fmtStr[40];

  /* Initially, no readings made. */
  *rtn_count = 0;

  /* Read Header (should be #[1-9], if #0 then Indefinite Block. */
  rtn_code = viScanf(vi, "%2c", c);
  if (rtn_code < VI_SUCCESS)
    return (rtn_code);

  /* Convert 1st digit to integer. */
  digits = c[1] - '0';		/* c[0] is the # */

  /* If digits was not [0-9] then syntax problem, blow away input and return */
  if ((digits < 0) || (9 < digits)) {
    rtn_code = viScanf(vi, "%*t");	/* Clear input. */
    return (VI_ERROR_INV_RESPONSE);
  }

  if (digits == 0) {
    /* Is (Indefinite Block) */
    /* Read data up to max_length, else end of input.  */
    /* %nnnc format reads to next END, else until nnn bytes. */
    nbytes = max_length * 8;
  } else {			/* Must be DAB. */

    /* Scan DAB array count. */
    sprintf(fmtStr, "%%%ldc", digits);
    rtn_code = viScanf(vi, fmtStr, length_str);
    if (rtn_code < VI_SUCCESS)
      return (rtn_code);

    length_str[digits] = '\0';	/* null terminate the string */
    nbytes = atol(length_str);

    /* Verify that caller's array is big enough. */
    if ((max_length * 8) < nbytes)
      return (VI_ERROR_PARAMETER2);	/* Caller's array too small. */
  }

  sprintf(fmtStr, "%%%ldc%%*t", nbytes);

  /* if we got to here, all is well, do the read */
  rtn_code = viScanf(vi, fmtStr, (unsigned char *)f64_array);

  if (rtn_code < VI_SUCCESS)
    return (rtn_code);

  *rtn_count = nbytes / 8;

  /* If (caller requests byte swapping) */
  if (swap == VI_TRUE) {
    unsigned char src[8];

    /* Byte Swap (bytes [0->7] into bytes [7->0]). */
    for (i = 0; i < (*rtn_count); i++) {
      /* Place the 8 byte double into src[8]. */
      *((double *)src) = *((double *)(&f64_array[i]));

      /* Now swap the byte order. */
      ((unsigned char *)(&f64_array[i]))[0] = ((unsigned char *)(src))[7];
      ((unsigned char *)(&f64_array[i]))[1] = ((unsigned char *)(src))[6];
      ((unsigned char *)(&f64_array[i]))[2] = ((unsigned char *)(src))[5];
      ((unsigned char *)(&f64_array[i]))[3] = ((unsigned char *)(src))[4];
      ((unsigned char *)(&f64_array[i]))[4] = ((unsigned char *)(src))[3];
      ((unsigned char *)(&f64_array[i]))[5] = ((unsigned char *)(src))[2];
      ((unsigned char *)(&f64_array[i]))[6] = ((unsigned char *)(src))[1];
      ((unsigned char *)(&f64_array[i]))[7] = ((unsigned char *)(src))[0];
    }				/* For-Loop */

  }
  /* If (swapping needed) */
  return (rtn_code);

}				/* hpe1564_scan64() */

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_fetchScalCvt_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function returns the last voltage readings acquired on
 *           each channel.  The readings are in channel number order,
 *           beginning with channel 1.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViReal64 _VI_FAR data[]
 * OUT       
 *            Pointer to the array of doubles which will hold the A/D reading
 *           values. One data point from each channel is returned in the
 *           array.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_fetchScalCvt_Q(ViSession vi, ViReal64 _VI_FAR data[])
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_fetchScalCvt_Q");

  if (thisPtr->e1406) {
    ViInt32 howMany;

    if (thisPtr->digState.dataFormat != hpe1564_FORM_REAL64) {
      thisPtr->digState.dataFormat = hpe1564_FORM_REAL64;
      errStatus = viPrintf(vi, "FORM:DATA REAL,64\n");
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = viPrintf(vi, "DATA:CVT? (@1:4)\n");
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* last parameter indicates to byte swap the readings */
#ifdef BIG_ENDIAN
    errStatus = hpe1564_scan64(vi, 4, data, &howMany, 0);
#else
    errStatus = hpe1564_scan64(vi, 4, data, &howMany, 1);
#endif
  } else
    errStatus = hpe1564_regScalDataCvt_Q(vi, thisPtr, data);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_fetchScal_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function brings back voltage formatted data from the
 *           specified channels.  Because it separates the interleaved raw
 *           data into the distinct channel arrays provided, and converts the
 *           data to voltages, this function is slower than the
 *           hpe1564_fetchAll function.  Channel arrays that are specified as
 *           null (0L), will have no data returned in them.
 *            NOTE: Measurement data on channels which have an associated
 *           null array pointer (0L), is thrown away by this command and is
 *           not recoverable. Similarly, this command can read the data from
 *           a measurement only once, and it is a destructive read.
 *           
 *            This function will attempt to bring back "arrayLength" readings
 *           for each channel with a non-null array pointer, but will not
 *           error if fewer readings are available.  The number of readings
 *           actually read into the data array is returned in the "howMany"
 *           parameter.
 *            See the help file for more information about this command.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt32 arrayLength
 * IN        
 *            Specifies the number of readings to bring back in each data
 *           array.  The data arrays must be at least this large, or they
 *           must be NULL (0L).
 * 
 *      MAX = hpe1564_FETCH_MAX   4000000
 *      MIN = hpe1564_FETCH_MIN   1
 * 
 * PARAM 3 : ViReal64 _VI_FAR data1[]
 * OUT       
 *            Pointer to the array of floating point values which will hold
 *           the A/D reading values from channel 1.  If a null pointer is
 *           given (0L), no data will be returned for this channel.
 * 
 * PARAM 4 : ViReal64 _VI_FAR data2[]
 * OUT       
 *            Pointer to the array of floating point values which will hold
 *           the A/D reading values from channel 2.  If a null pointer is
 *           given (0L), no data will be returned for this channel.
 * 
 * PARAM 5 : ViReal64 _VI_FAR data3[]
 * OUT       
 *            Pointer to the array of floating point values which will hold
 *           the A/D reading values from channel 3.  If a null pointer is
 *           given (0L), no data will be returned for this channel.
 * 
 * PARAM 6 : ViReal64 _VI_FAR data4[]
 * OUT       
 *            Pointer to the array of floating point values which will hold
 *           the A/D reading values from channel 4.  If a null pointer is
 *           given (0L), no data will be returned for this channel.
 * 
 * PARAM 7 : ViPInt32 howMany
 * OUT       
 *            Specifies how many readings were actually placed into each
 *           non-null data array.  This is useful when using one large array
 *           to cover several different measurements that vary in sample
 *           count, or when fetching out data in "blocks" where the last
 *           fetch of data may contain only a remainder of data and not a
 *           full block.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_fetchScal_Q(ViSession vi,
				      ViInt32 arrayLength, ViReal64 _VI_FAR data1[],
				      ViReal64 _VI_FAR data2[], ViReal64 _VI_FAR data3[],
				      ViReal64 _VI_FAR data4[], ViPInt32 howMany)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_fetchScal_Q");

  hpe1564_CHK_LONG_RANGE(arrayLength, hpe1564_FETCH_MIN, hpe1564_FETCH_MAX, VI_ERROR_PARAMETER2);

  /* Format is set to PACKed when session is opened via hpe1564_init */
  if (thisPtr->e1406) {
    char chan_list[10];
    ViBoolean need_comma = 0;
    ViChar fmtStr[30];
    ViInt16 numRdgs;
    ViInt16 ii;
    ViReal64 dataBlock[BLOCK_SIZE];
    ViInt32 num_bytes;

    /* initialize list to empty */
    chan_list[0] = '\0';
    if (thisPtr->digState.dataFormat != hpe1564_FORM_REAL64) {
      thisPtr->digState.dataFormat = hpe1564_FORM_REAL64;
      errStatus = viPrintf(vi, "FORM:DATA REAL,64\n");
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    }

    if (data1 != NULL) {
      strcat(chan_list, "1");
      need_comma = 1;
    }

    if (data2 != NULL) {
      if (need_comma)
	strcat(chan_list, ",2");
      else {
	need_comma = 1;
	strcat(chan_list, "2");
      }
    }

/*   had this in here to check pointers from Visual Basic -- they were bad
	sprintf(mssg, "chan3 ptr: %lX, chan4 ptr: %lX", data3, data4);
      MessageBox(NULL, mssg, "ERROR in IPRINTF", MB_OK | MB_TASKMODAL);
*/

    if (data3 != NULL) {
      if (need_comma)
	strcat(chan_list, ",3");
      else {
	need_comma = 1;
	strcat(chan_list, "3");
      }
    }

    if (data4 != NULL) {
      if (need_comma)
	strcat(chan_list, ",4");
      else {
	need_comma = 1;
	strcat(chan_list, "4");
      }
    }

    strcat(chan_list, ")");

    errStatus = viPrintf(vi, "DATA? %ld,(@%s\n", arrayLength, chan_list);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* initialize reading counter */
    *howMany = 0;

    /* if device clear, then blow outta here */
    if (thisPtr->device_clear) {
      thisPtr->device_clear = 0;
      return VI_SUCCESS;
    }

    /* Now we have to bring back the data in "chunks" of dataBlock
       so that we don't double allocate huge arrays when asking for
       lots of data.
     */
    errStatus = hpe1564_readHeader(vi, &num_bytes);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* set fmtStr to large block, don't use %*t */
    sprintf(fmtStr, "%%%ldc", BLOCK_SIZE * 8);

    while (num_bytes > 0) {
      /* if num_bytes > 32000 (dataBlock array), get a chunk */
      if (num_bytes >= BLOCK_SIZE * 8) {
	errStatus = viScanf(vi, fmtStr, (char *)dataBlock);
	if (errStatus < VI_SUCCESS)
	  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

	num_bytes -= BLOCK_SIZE * 8;

	/* update numRdgs for loop below */
	numRdgs = BLOCK_SIZE;
      } else {
	/* note %*t in format string to finish the read */
	sprintf(fmtStr, "%%%ldc%%*t", num_bytes);
	errStatus = viScanf(vi, fmtStr, (char *)dataBlock);
	if (errStatus < VI_SUCCESS)
	  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

	/* update numRdgs for loop below */
	numRdgs = num_bytes / 8;

	/* we've read it all, so set num_bytes to 0 */
	num_bytes = 0;
      }

      /* sort the data into the proper arrays */
      ii = 0;
      while (ii < numRdgs) {
	if (data1 != NULL) {
#ifndef BIG_ENDIAN
	  /* byte swap if not Motorola platform */
	  SWAP_FLOAT64(&dataBlock[ii]);
#endif
	  data1[*howMany] = dataBlock[ii];
	  ii += 1;
	}

	if (data2 != NULL) {
#ifndef BIG_ENDIAN
	  /* byte swap if not Motorola platform */
	  SWAP_FLOAT64(&dataBlock[ii]);
#endif
	  data2[*howMany] = dataBlock[ii];
	  ii += 1;
	}

	if (data3 != NULL) {
#ifndef BIG_ENDIAN
	  /* byte swap if not Motorola platform */
	  SWAP_FLOAT64(&dataBlock[ii]);
#endif
	  data3[*howMany] = dataBlock[ii];
	  ii += 1;
	}

	if (data4 != NULL) {
#ifndef BIG_ENDIAN
	  /* byte swap if not Motorola platform */
	  SWAP_FLOAT64(&dataBlock[ii]);
#endif
	  data4[*howMany] = dataBlock[ii];
	  ii += 1;
	}

	/* increment how many per channel have been read */
	*howMany += 1;
      }

    }				/* end while (num_bytes > 0) */

  } else {
    /* Cast data pointers to void and specify real format (parm4 = 1 ) */
    errStatus = hpe1564_regData_Q(vi, thisPtr, arrayLength, 1,
				  (void *)data1, (void *)data2, (void *)data3, (void *)data4,
				  howMany);

    /* if we had a device_clear, handle it here */
    if (thisPtr->device_clear) {
      hpe1564_regAbort(vi, thisPtr);
      thisPtr->device_clear = 0;
      return VI_SUCCESS;
    }
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_initCont
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function is used to either begin or end a continuous
 *           measurement. The ON (1) setting will begin a measurement with
 *           infinite sample count.  Once the trigger condition is met, the
 *           instrument will sample until one of the following three things
 *           occurs: BEGIN VERBATIM
 *            1. The measurement is halted with the hpe1564_abort() function.
 *           
 *            2. The measurement is stopped by setting continuous mode to OFF
 *            by calling hpe1564_initCont(vi, hpe1564_OFF)
 *           
 *            3. The instrument fifo memory is filled.  This can be prevented
 *            from happening by fetching the data out in blocks faster than
 *            the sample rate can fill memory.
 *           
 *            Once initiated, the instrument enters the wait-for-trigger
 *            state to await a trigger before proceeding to the
 *           wait-for-sample state which is where readings are actually
 *           taken.  The instrument stays in the wait-for-sample state until
 *           one of the three events mentioned above occurs.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViBoolean state
 * IN        
 *            This parameter specifies whether continuous mode is ON (1) or
 *           OFF (0). See the function description for more information about
 *           ON/OFF.
 * 
 *      Macro Name       Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_OFF          0  Disabled
 *      hpe1564_ON           1  Enabled
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_initCont(ViSession vi, ViBoolean state)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_initCont");

  hpe1564_CHK_BOOLEAN(state, VI_ERROR_PARAMETER2);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr)) {
    /* we are already initiated, error if we are trying to turn ON */
    if (state)
      return hpe1564_ERROR_INIT_IGN;
  }

  if (thisPtr->e1406)
    errStatus = viPrintf(vi, "INIT:CONT %hd\n", state);
  else {
    if (state) {
      errStatus = hpe1564_regInitContOn(vi, thisPtr);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    } else
      errStatus = hpe1564_regInitContOff(vi, thisPtr);
  }
  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_initCont_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function is used to determine if the instrument is in the
 *            continuous measurement mode.  A return value of 1 indicates
 *           continous measurement mode is set, a return value of 0 indicates
 *           non-continous measurement mode.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViPBoolean state
 * OUT       
 *            This parameter specifies whether continuous mode is ON (1) or
 *           OFF (0).
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_initCont_Q(ViSession vi, ViPBoolean state)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_initCont_Q");

  if (thisPtr->e1406)
    errStatus = viQueryf(vi, "INIT:CONT?\n", "%hd%*t", state);
  else
    *state = (thisPtr->initiated == CONTINUOUS_INIT) ? 1 : 0;

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_initImm
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  The hpe1564_initImm() command controls the initiation of the
 *            trigger subsystem and prepares the HP E1564 to take voltage
 *            measurements.  Once initiated, the instrument enters the
 *           wait-for-trigger state to await a trigger before proceeding to
 *           the wait-for-sample state which is where readings are actually
 *           taken.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_initImm(ViSession vi)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_initImm");

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406)
    errStatus = viPrintf(vi, "INIT\n");
  else
    errStatus = hpe1564_regInitImm(vi, thisPtr);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_inpFilt
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function changes the filter frequency and enables or
 *            disables the filter.  The hardware will be set to the nearest
 *            value that can achieve the specified filter setting.  Use the
 *            hpe1564_inpFilt_Q function to see the final setting used in
 *            the hardware.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 channel
 * IN        
 *            Specifies the channel to adjust the filter on.
 *           
 *            The channels are defined as macros and are shown below:
 * 
 *      Macro Name         Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_CHAN1          1  1
 *      hpe1564_CHAN2          2  2
 *      hpe1564_CHAN3          3  3
 *      hpe1564_CHAN4          4  4
 * 
 * PARAM 3 : ViReal64 filter
 * IN        
 *            Specifies the filter setting desired.  The filters
 *            are 2 pole Bessel filters.
 *           
 *            Values of filter less than 750.0 will result in the
 *            OFF (0.0) setting.  Other values will be rounded
 *            to the nearest setting the hardware can achieve.
 *            The actual discrete hardware settings are shown below:
 * 
 *      Macro Name             Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_FILT_OFF         0.0  0.0
 *      hpe1564_FILT_1500     1500.0  1500.0
 *      hpe1564_FILT_6K       6000.0  6000.0
 *      hpe1564_FILT_25K     25000.0  25000.0
 *      hpe1564_FILT_100K   100000.0  100000.0
 * 
 *      MAX = hpe1564_FILT_MAX   100000.0001
 *      MIN = hpe1564_FILT_MIN   0.0
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_inpFilt(ViSession vi, ViInt16 channel, ViReal64 filter)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_inpFilt");

  hpe1564_CHK_INT_RANGE(channel, 1, 4, VI_ERROR_PARAMETER2);
  hpe1564_CHK_REAL_RANGE(filter, hpe1564_FILT_MIN, hpe1564_FILT_MAX, VI_ERROR_PARAMETER3);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406) {
    if (filter < 750.0)
      errStatus = viPrintf(vi, "INP%hd:FILT OFF\n", channel);
    else
      errStatus = viPrintf(vi, "INP%hd:FILT ON;FILT:FREQ %lg\n", channel, filter);
  } else {
    if (filter < 750.0)
      errStatus = hpe1564_regInpFiltStat(vi, thisPtr, channel, hpe1564_OFF);
    else {
      errStatus = hpe1564_regInpFiltFreq(vi, thisPtr, channel, filter);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = hpe1564_regInpFiltStat(vi, thisPtr, channel, hpe1564_ON);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = hpe1564_regInputPmt(vi, thisPtr);
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_inpFilt_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function queries the present setting of the filter on
 *            the specified channel.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 channel
 * IN        
 *            Specifies the channel to query the filter setting on.
 *           
 *            The channels are defined as macros and are shown below:
 * 
 *      Macro Name         Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_CHAN1          1  1
 *      hpe1564_CHAN2          2  2
 *      hpe1564_CHAN3          3  3
 *      hpe1564_CHAN4          4  4
 * 
 * PARAM 3 : ViPReal64 filter
 * OUT       
 *            The filter setting of the specfied channel is
 *            returned in this parameter.  0.0 indicates the
 *            filter is OFF.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_inpFilt_Q(ViSession vi, ViInt16 channel, ViPReal64 filter)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;
  ViBoolean state;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_inpFilt_Q");

  hpe1564_CHK_INT_RANGE(channel, 1, 4, VI_ERROR_PARAMETER2);

  if (thisPtr->e1406) {
    errStatus = viPrintf(vi, "INP%hd:FILT?\n", channel);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viScanf(vi, "%hd%*t", &state);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    if (state) {
      errStatus = viPrintf(vi, "INP%hd:FILT:FREQ?\n", channel);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = viScanf(vi, "%lg%*t", filter);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    } else
      *filter = 0.0;

  } else {
    errStatus = hpe1564_regInpFiltStat_Q(vi, thisPtr, channel, &state);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    if (state)
      errStatus = hpe1564_regInpFiltFreq_Q(vi, thisPtr, channel, filter);
    else
      *filter = 0.0;
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_inpState
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function connects or disconnects the input signal to
 *            the instrument's measurement circuitry.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 channel
 * IN        
 *            Specifies the channel to change the input relay state on.
 *           
 *            The channels are defined as macros and are shown below:
 * 
 *      Macro Name         Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_CHAN1          1  1
 *      hpe1564_CHAN2          2  2
 *      hpe1564_CHAN3          3  3
 *      hpe1564_CHAN4          4  4
 * 
 * PARAM 3 : ViBoolean state
 * IN        
 *            Boolean which connects (1) or disconnects (0) the signal
 *            to the measurement circuitry on the instrument.  The settings
 *           are shown in the macros below.
 * 
 *      Macro Name       Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_OFF          0  Disconnected
 *      hpe1564_ON           1  Connected
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_inpState(ViSession vi, ViInt16 channel, ViBoolean state)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_inpState");

  hpe1564_CHK_INT_RANGE(channel, 1, 4, VI_ERROR_PARAMETER2);
  hpe1564_CHK_BOOLEAN(state, VI_ERROR_PARAMETER3);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406)
    errStatus = viPrintf(vi, "INP%hd %hd\n", channel, state);
  else {
    errStatus = hpe1564_regInpStat(vi, thisPtr, channel, state);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = hpe1564_regInputPmt(vi, thisPtr);
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_inpState_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function queries the input relay on the specified channel
 *            to see if it is in the connected (1) or disconnected (0)
 *            state.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 channel
 * IN        
 *            Specifies the channel to query the input relay state on.
 *           
 *            The channels are defined as macros and are shown below:
 * 
 *      Macro Name         Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_CHAN1          1  1
 *      hpe1564_CHAN2          2  2
 *      hpe1564_CHAN3          3  3
 *      hpe1564_CHAN4          4  4
 * 
 * PARAM 3 : ViPBoolean state
 * OUT       
 *            The state of the input connection (1 = connected),
 *            (0 = disconnected) to the measurement circuitry
 *            is returned in this parameter.  These states are also
 *            available as macros.
 * 
 *      Macro Name       Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_OFF          0  Disconnected
 *      hpe1564_ON           1  Connected
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_inpState_Q(ViSession vi, ViInt16 channel, ViPBoolean state)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_inpState_Q");

  hpe1564_CHK_INT_RANGE(channel, 1, 4, VI_ERROR_PARAMETER2);
  if (thisPtr->e1406) {
    errStatus = viPrintf(vi, "INP%hd?\n", channel);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viScanf(vi, "%hd%*t", state);
  } else
    errStatus = hpe1564_regInpStat_Q(vi, thisPtr, channel, state);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_input
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function allows the user to configure 2 channels at once. 
 *           Since the hardware puts two channels into the same register,
 *           this command is the most time efficient way to change range
 *           and/or filter settings on a pair of channels, especially since
 *           there is a relay settling time of around 25 milliseconds
 *           whenever this register is written.
 *            It is important to note that this function will automatically
 *           close the input relay on each channel (if it was open), thereby
 *           connecting the input signal to the measurement path.
 *           
 *            The filter will be set to the nearest value that can be
 *           achieved;  a filter value less than 750.0 will result in the
 *           filter being turned off.
 *            There are several functions that allow single channel access to
 *           each of these same parameters.  See the "Low Level Configure"
 *           functions that begin with "hpe1564_inp", for the one(s) you
 *           need.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 chanPair
 * IN        
 *            Specifies which pair of channels will be altered.
 *            The allowable channel pairs are defined as macros and shown
 *           below:
 * 
 *      Macro Name            Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_CHANS_12          1  1
 *      hpe1564_CHANS_34          2  2
 * 
 *      MAX = hpe1564_CHANS_34   2
 *      MIN = hpe1564_CHANS_12   1
 * 
 * PARAM 3 : ViReal64 oddChanRange
 * IN        
 *            Specifies the voltage range to use on the lower numbered (or
 *           odd) channel of the pair.  The allowable settings are shown in
 *            the macros below.
 * 
 *      Macro Name               Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_RANGE_63MV      0.0625  0.0625
 *      hpe1564_RANGE_250MV      0.250  0.250
 *      hpe1564_RANGE_1            1.0  1.0
 *      hpe1564_RANGE_4            4.0  4.0
 *      hpe1564_RANGE_16          16.0  16.0
 *      hpe1564_RANGE_64          64.0  64.0
 *      hpe1564_RANGE_256        256.0  256.0
 * 
 *      MAX = hpe1564_VOLT_MAX   256.0001
 *      MIN = hpe1564_VOLT_MIN   -256.0001
 * 
 * PARAM 4 : ViReal64 oddChanFilter
 * IN        
 *            Specifies the filter setting desired on the lower numbered (or
 *           odd) channel in the channel pair. The filters are 2 pole Bessel
 *           filters.
 *            Values of filter less than 750.0 will result in the
 *            OFF (0.0) setting.  Other values will be rounded
 *            to the nearest setting the hardware can achieve.
 *            The actual discrete hardware settings are shown below:
 * 
 *      Macro Name             Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_FILT_OFF         0.0  0.0
 *      hpe1564_FILT_1500     1500.0  1500.0
 *      hpe1564_FILT_6K       6000.0  6000.0
 *      hpe1564_FILT_25K     25000.0  25000.0
 *      hpe1564_FILT_100K   100000.0  100000.0
 * 
 *      MAX = hpe1564_FILT_MAX   100000.0001
 *      MIN = hpe1564_FILT_MIN   0.0
 * 
 * PARAM 5 : ViReal64 evenChanRange
 * IN        
 *            Specifies the voltage range to use on the higher numbered (or
 *           even) channel of the pair.  The allowable settings are shown in
 *            the macros below.
 * 
 *      Macro Name               Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_RANGE_63MV      0.0625  0.0625
 *      hpe1564_RANGE_250MV      0.250  0.250
 *      hpe1564_RANGE_1            1.0  1.0
 *      hpe1564_RANGE_4            4.0  4.0
 *      hpe1564_RANGE_16          16.0  16.0
 *      hpe1564_RANGE_64          64.0  64.0
 *      hpe1564_RANGE_256        256.0  256.0
 * 
 *      MAX = hpe1564_VOLT_MAX   256.0001
 *      MIN = hpe1564_VOLT_MIN   -256.0001
 * 
 * PARAM 6 : ViReal64 evenChanFilter
 * IN        
 *            Specifies the filter setting desired on the higher numbered (or
 *           even) channel in the channel pair. The filters are 2 pole Bessel
 *           filters.
 *            Values of filter less than 750.0 will result in the
 *            OFF (0.0) setting.  Other values will be rounded
 *            to the nearest setting the hardware can achieve.
 *            The actual discrete hardware settings are shown below:
 * 
 *      Macro Name             Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_FILT_OFF         0.0  0.0
 *      hpe1564_FILT_1500     1500.0  1500.0
 *      hpe1564_FILT_6K       6000.0  6000.0
 *      hpe1564_FILT_25K     25000.0  25000.0
 *      hpe1564_FILT_100K   100000.0  100000.0
 * 
 *      MAX = hpe1564_FILT_MAX   100000.0001
 *      MIN = hpe1564_FILT_MIN   0.0
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_input(ViSession vi,
				ViPReal64 range1,
				ViPReal64 filt1,
				ViPReal64 range2,
				ViPReal64 filt2,
				ViPReal64 range3,
				ViPReal64 filt3, ViPReal64 range4, ViPReal64 filt4)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_input");

  /* Perform Error Checking on Each Parameter */
  hpe1564_CHK_REAL_RANGE(*range1, hpe1564_VOLT_MIN, hpe1564_VOLT_MAX, VI_ERROR_PARAMETER2);

  hpe1564_CHK_REAL_RANGE(*filt1, hpe1564_FILT_MIN, hpe1564_FILT_MAX, VI_ERROR_PARAMETER3);

  hpe1564_CHK_REAL_RANGE(*range2, hpe1564_VOLT_MIN, hpe1564_VOLT_MAX, VI_ERROR_PARAMETER4);

  hpe1564_CHK_REAL_RANGE(*filt2, hpe1564_FILT_MIN, hpe1564_FILT_MAX, VI_ERROR_PARAMETER5);

  hpe1564_CHK_REAL_RANGE(*range3, hpe1564_VOLT_MIN, hpe1564_VOLT_MAX, VI_ERROR_PARAMETER6);

  hpe1564_CHK_REAL_RANGE(*filt3, hpe1564_FILT_MIN, hpe1564_FILT_MAX, VI_ERROR_PARAMETER7);

  hpe1564_CHK_REAL_RANGE(*range4, hpe1564_VOLT_MIN, hpe1564_VOLT_MAX, VI_ERROR_PARAMETER8);

  hpe1564_CHK_REAL_RANGE(*filt4, hpe1564_FILT_MIN, hpe1564_FILT_MAX,
			 hpe1564_INSTR_ERROR_PARAMETER9);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406) {

    /* let's do 2 channels at a time -- that is, send the linefeed with
       the second channel.
     */
    if (*filt1 < 750.0)
      errStatus = viPrintf(vi, "INP1:STAT ON;FILT OFF;:VOLT1:RANG %lg\n", *range1);
    else
      errStatus = viPrintf(vi, "INP1:STAT ON;FILT ON;FILT:FREQ %lg;:VOLT1:RANG %lg\n",
			   *filt1, *range1);

    /* now do even channel, NOTE we send linefeed now. */
    if (*filt2 < 750.0)
      errStatus = viPrintf(vi, "INP2:STAT ON;FILT OFF;:VOLT2:RANG %lg\n", *range2);
    else
      errStatus = viPrintf(vi, "INP2:STAT ON;FILT ON;FILT:FREQ %lg;:VOLT2:RANG %lg\n",
			   *filt2, *range2);

    /* Now lets do the other two channels as a pair */
    if (*filt3 < 750.0)
      errStatus = viPrintf(vi, "INP3:STAT ON;FILT OFF;:VOLT3:RANG %lg\n", *range3);
    else
      errStatus = viPrintf(vi, "INP3:STAT ON;FILT ON;FILT:FREQ %lg;:VOLT3:RANG %lg\n",
			   *filt3, *range3);

    /* now do last channel, NOTE we send linefeed now. */
    if (*filt4 < 750.0)
      errStatus = viPrintf(vi, "INP4:STAT ON;FILT OFF;:VOLT4:RANG %lg\n", *range4);
    else
      errStatus = viPrintf(vi, "INP4:STAT ON;FILT ON;FILT:FREQ %lg;:VOLT4:RANG %lg\n",
			   *filt4, *range4);
  } else {
    /* do channel 1 */
    errStatus = hpe1564_regInpRange(vi, thisPtr, 1, *range1);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* We are setting the filter:freq and the filter:state with
       one call here.  If freq < 750, state is off; else state is
       on, and frequency is used.
     */
    if (*filt1 < 750.0)
      errStatus = hpe1564_regInpFiltStat(vi, thisPtr, 1, hpe1564_OFF);
    else {
      errStatus = hpe1564_regInpFiltStat(vi, thisPtr, 1, hpe1564_ON);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = hpe1564_regInpFiltFreq(vi, thisPtr, 1, *filt1);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = hpe1564_regInpStat(vi, thisPtr, 1, hpe1564_ON);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* do channel 2 */
    errStatus = hpe1564_regInpRange(vi, thisPtr, 2, *range2);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* We are setting the filter:freq and the filter:state with
       one call here.  If freq < 750, state is off; else state is
       on, and frequency is used.
     */
    if (*filt2 < 750.0)
      errStatus = hpe1564_regInpFiltStat(vi, thisPtr, 2, hpe1564_OFF);
    else {
      errStatus = hpe1564_regInpFiltStat(vi, thisPtr, 2, hpe1564_ON);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = hpe1564_regInpFiltFreq(vi, thisPtr, 2, *filt2);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = hpe1564_regInpStat(vi, thisPtr, 2, hpe1564_ON);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* do channel 3 */
    errStatus = hpe1564_regInpRange(vi, thisPtr, 3, *range3);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* We are setting the filter:freq and the filter:state with
       one call here.  If freq < 750, state is off; else state is
       on, and frequency is used.
     */
    if (*filt3 < 750.0)
      errStatus = hpe1564_regInpFiltStat(vi, thisPtr, 3, hpe1564_OFF);
    else {
      errStatus = hpe1564_regInpFiltStat(vi, thisPtr, 3, hpe1564_ON);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = hpe1564_regInpFiltFreq(vi, thisPtr, 3, *filt3);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = hpe1564_regInpStat(vi, thisPtr, 3, hpe1564_ON);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* do channel 4 */
    errStatus = hpe1564_regInpRange(vi, thisPtr, 4, *range4);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* We are setting the filter:freq and the filter:state with
       one call here.  If freq < 750, state is off; else state is
       on, and frequency is used.
     */
    if (*filt4 < 750.0)
      errStatus = hpe1564_regInpFiltStat(vi, thisPtr, 4, hpe1564_OFF);
    else {
      errStatus = hpe1564_regInpFiltStat(vi, thisPtr, 4, hpe1564_ON);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = hpe1564_regInpFiltFreq(vi, thisPtr, 4, *filt4);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = hpe1564_regInpStat(vi, thisPtr, 4, hpe1564_ON);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* this forces hardware to update */
    errStatus = hpe1564_regInputPmt(vi, thisPtr);

  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_input_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function queries the input range and filter settings on
 *           all channels.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViPReal64 range1
 * OUT       
 *            The present setting of the range on channel 1 is returned in
 *           this parameter.
 * 
 * PARAM 3 : ViPReal64 filter1
 * OUT       
 *            The present filter setting on channel 1 is returned in this
 *           parameter.
 * 
 * PARAM 4 : ViPReal64 range2
 * OUT       
 *            The present setting of the range on channel 2 is returned in
 *           this parameter.
 * 
 * PARAM 5 : ViPReal64 filter2
 * OUT       
 *            The present filter setting on channel 2 is returned in this
 *           parameter.
 * 
 * PARAM 6 : ViPReal64 range3
 * OUT       
 *            The present setting of the range on channel 3 is returned in
 *           this parameter.
 * 
 * PARAM 7 : ViPReal64 filter3
 * OUT       
 *            The present filter setting on channel 3 is returned in this
 *           parameter.
 * 
 * PARAM 8 : ViPReal64 range4
 * OUT       
 *            The present setting of the range on channel 4 is returned in
 *           this parameter.
 * 
 * PARAM 9 : ViPReal64 filter4
 * OUT       
 *            The present filter setting on channel 4 is returned in this
 *           parameter.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_input_Q(ViSession vi,
				  ViPReal64 range1,
				  ViPReal64 filt1,
				  ViPReal64 range2,
				  ViPReal64 filt2,
				  ViPReal64 range3,
				  ViPReal64 filt3, ViPReal64 range4, ViPReal64 filt4)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;
  ViBoolean state;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_input_Q");

  if (thisPtr->e1406) {
    /* get the data for chan 1 */
    errStatus = viPrintf(vi, "INP1:FILT?\n");
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viScanf(vi, "%hd%*t", &state);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    if (state) {
      errStatus = viPrintf(vi, "INP1:FILT:FREQ?\n");
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = viScanf(vi, "%lg%*t", filt1);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    } else
      *filt1 = 0.0;

    errStatus = viPrintf(vi, "VOLT1:RANG?\n", range1);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viScanf(vi, "%lg%*t", range1);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* get the data for chan 2 */
    errStatus = viPrintf(vi, "INP2:FILT?\n");
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viScanf(vi, "%hd%*t", &state);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    if (state) {
      errStatus = viPrintf(vi, "INP2:FILT:FREQ?\n");
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = viScanf(vi, "%lg%*t", filt2);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    } else
      *filt2 = 0.0;

    errStatus = viPrintf(vi, "VOLT2:RANG?\n", range2);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viScanf(vi, "%lg%*t", range2);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* get the data for chan 3 */
    errStatus = viPrintf(vi, "INP3:FILT?\n");
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viScanf(vi, "%hd%*t", &state);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    if (state) {
      errStatus = viPrintf(vi, "INP3:FILT:FREQ?\n");
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = viScanf(vi, "%lg%*t", filt3);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    } else
      *filt3 = 0.0;

    errStatus = viPrintf(vi, "VOLT3:RANG?\n", range3);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viScanf(vi, "%lg%*t", range3);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* get the data for chan 4 */
    errStatus = viPrintf(vi, "INP4:FILT?\n");
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viScanf(vi, "%hd%*t", &state);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    if (state) {
      errStatus = viPrintf(vi, "INP4:FILT:FREQ?\n");
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = viScanf(vi, "%lg%*t", filt4);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    } else
      *filt4 = 0.0;

    errStatus = viPrintf(vi, "VOLT4:RANG?\n", range4);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viScanf(vi, "%lg%*t", range4);

  } else {
    /* query channel 1 */
    errStatus = hpe1564_regInpRange_Q(vi, thisPtr, 1, range1);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = hpe1564_regInpFiltStat_Q(vi, thisPtr, 1, &state);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    if (state)
      errStatus = hpe1564_regInpFiltFreq_Q(vi, thisPtr, 1, filt1);
    else
      *filt1 = 0.0;

    /* query channel 2 */
    errStatus = hpe1564_regInpRange_Q(vi, thisPtr, 2, range2);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = hpe1564_regInpFiltStat_Q(vi, thisPtr, 2, &state);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    if (state)
      errStatus = hpe1564_regInpFiltFreq_Q(vi, thisPtr, 2, filt2);
    else
      *filt2 = 0.0;

    /* query channel 3 */
    errStatus = hpe1564_regInpRange_Q(vi, thisPtr, 3, range3);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = hpe1564_regInpFiltStat_Q(vi, thisPtr, 3, &state);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    if (state)
      errStatus = hpe1564_regInpFiltFreq_Q(vi, thisPtr, 3, filt3);
    else
      *filt3 = 0.0;

    /* query channel 4 */
    errStatus = hpe1564_regInpRange_Q(vi, thisPtr, 4, range4);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = hpe1564_regInpFiltStat_Q(vi, thisPtr, 4, &state);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    if (state)
      errStatus = hpe1564_regInpFiltFreq_Q(vi, thisPtr, 4, filt4);
    else
      *filt4 = 0.0;

  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_limitFail_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function queries the present status of the limit
 *            checking on the specified channel.  The returned value
 *            is a boolean, where 0 indicates the limit was not
 *            exceeded (passed), and 1 indicates the limit was
 *            exceeded (failed).
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 channel
 * IN        
 *            Specifies the channel to query the limit status on.
 *           
 *            The channels are defined as macros and are shown below:
 * 
 *      Macro Name         Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_CHAN1          1  1
 *      hpe1564_CHAN2          2  2
 *      hpe1564_CHAN3          3  3
 *      hpe1564_CHAN4          4  4
 * 
 * PARAM 3 : ViPBoolean status
 * OUT       
 *            The state of the limit checking on the most
 *            recent measurement is returned here.  A return
 *            value of 1 indicates the specified limit was exceeded (failed);
 *            a return value of 0 indicates that the measurement
 *            did not exceed the specified limit (passed).
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_limitFail_Q(ViSession vi, ViInt16 channel, ViPBoolean status)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_limitFail_Q");

  hpe1564_CHK_INT_RANGE(channel, 1, 4, VI_ERROR_PARAMETER2);
  if (thisPtr->e1406) {
    errStatus = viPrintf(vi, "CALC%hd:LIM:FAIL?\n", channel);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viScanf(vi, "%hd%*t", status);
  } else
    errStatus = hpe1564_regCalcLimFail_Q(vi, thisPtr, channel, status);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_limitSet
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function sets up a channel to detect when a limit has
 *            been exceeded in the specified manner (i.e. signal either rose
 *            above or fell below the specified level).  To determine if the
 *            level was exceeded, call the hpe1564_limitFail_Q function.  The
 *            status system can also be mumble enabled to do something when
 *            if the limit is exceeded.
 *           
 *            The limit detection is reset with each new measurement, so
 *            it is not a cumulative record.
 *           
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 channel
 * IN        
 *            Specifies the channel to set the limit on.
 *           
 *            The channels are defined as macros and are shown below:
 * 
 *      Macro Name         Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_CHAN1          1  1
 *      hpe1564_CHAN2          2  2
 *      hpe1564_CHAN3          3  3
 *      hpe1564_CHAN4          4  4
 * 
 * PARAM 3 : ViReal64 level
 * IN        
 *            Specifies the level that will be the limit.  The value
 *            allowed here will depend upon the present range setting.
 *            The resolution varies by range and is shown for each
 *            range in the COMMENTS section below.  An error will occur
 *            if a level is chosen outside the present range setting.
 *            The overall minimum and maximum are shown as macros below.
 * 
 *      MAX = hpe1564_LIMIT_MAX   254.00
 *      MIN = hpe1564_LIMIT_MIN   -252.00
 * 
 * PARAM 4 : ViInt16 mode
 * IN        
 *            Specifies the mode or direction of the limit detection
 *            desired.  The three choices are shown as macros below.
 * 
 *      Macro Name               Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_LIMIT_OFF            0  No limit checking
 *      hpe1564_LIMIT_BELOW          1  Falling below level
 *      hpe1564_LIMIT_ABOVE          2  Rising above level
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_limitSet(ViSession vi, ViInt16 channel, ViReal64 level, ViInt16 mode)
{

  ViStatus errStatus = 0;
  ViStatus tempErrStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_limitSet");

  hpe1564_CHK_INT_RANGE(channel, 1, 4, VI_ERROR_PARAMETER2);
  hpe1564_CHK_REAL_RANGE(level, hpe1564_LIMIT_MIN, hpe1564_LIMIT_MAX, VI_ERROR_PARAMETER3);

  hpe1564_CHK_ENUM(mode, 2, VI_ERROR_PARAMETER4);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406) {
    if (mode == hpe1564_LIMIT_OFF) {
      errStatus = viPrintf(vi, "CALC%hd:LIM:UPP OFF\n", channel);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = viPrintf(vi, "CALC%hd:LIM:LOW OFF\n", channel);
    } else if (mode == hpe1564_LIMIT_BELOW) {
      errStatus = viPrintf(vi, "CALC%hd:LIM:LOW:DATA %lg\n", channel, level);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = viPrintf(vi, "CALC%hd:LIM:UPP OFF;LOW ON\n", channel);
    } else {			/* wants to enable upper limit check */

      errStatus = viPrintf(vi, "CALC%hd:LIM:UPP:DATA %lg\n", channel, level);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = viPrintf(vi, "CALC%hd:LIM:LOW OFF;UPP ON\n", channel);
    }

  } else {
    if (mode == hpe1564_LIMIT_BELOW) {
      errStatus = hpe1564_regCalcLimLowData(vi, thisPtr, channel, level);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    } else if (mode == hpe1564_LIMIT_ABOVE) {
      errStatus = hpe1564_regCalcLimUppData(vi, thisPtr, channel, level);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    }

    /* if we had an error above, it was a settings conflict and not
       fatal, so carry on.
     */
    if (errStatus != VI_SUCCESS)
      tempErrStatus = errStatus;

    errStatus = hpe1564_regCalcLimStat(vi, thisPtr, channel, mode);

    /* return if we just had an error, if not, pass along the
       previous error (if any).
     */
    if (errStatus)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    else
      errStatus = tempErrStatus;	/* report earlier coupling error */
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_limitSet_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function queries the present settings for limit
 *            checking on the specified channel.  The returned values
 *            are the voltage level, and the detection mode -- above
 *            the level (2), below the level (1), and no limit
 *            checking (0).
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 channel
 * IN        
 *            Specifies the channel to query the limit setting on.
 *           
 *            The channels are defined as macros and are shown below:
 * 
 *      Macro Name         Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_CHAN1          1  1
 *      hpe1564_CHAN2          2  2
 *      hpe1564_CHAN3          3  3
 *      hpe1564_CHAN4          4  4
 * 
 * PARAM 3 : ViPReal64 level
 * OUT       
 *            The voltage level used for the limit checking
 *            is returned in this parameter.
 * 
 * PARAM 4 : ViPInt16 mode
 * OUT       
 *            The mode of the limit checking is returned in
 *            this parameter. A value of 0 indicates that limit
 *            checking is disabled, a value of 1 indicates
 *            detection of signals BELOW the specified limit,
 *            a value of 2 indicates detection of signals ABOVE
 *            the specified limit.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_limitSet_Q(ViSession vi, ViInt16 chan, ViPReal64 level, ViPInt16 mode)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;
  ViBoolean state;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_limitSet_Q");

  hpe1564_CHK_INT_RANGE(chan, 1, 4, VI_ERROR_PARAMETER2);

  if (thisPtr->e1406) {
    errStatus = viPrintf(vi, "CALC%hd:LIM:LOW?\n", chan);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viScanf(vi, "%hd%*t", &state);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    if (state) {
      *mode = hpe1564_LIMIT_BELOW;
      errStatus = viPrintf(vi, "CALC%hd:LIM:LOW:DATA?\n", chan);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = viScanf(vi, "%lg%*t", level);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    } else {			/* check for upper limit enabled */

      errStatus = viPrintf(vi, "CALC%hd:LIM:UPP?\n", chan);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = viScanf(vi, "%hd%*t", &state);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      if (state) {
	*mode = hpe1564_LIMIT_ABOVE;
	errStatus = viPrintf(vi, "CALC%hd:LIM:UPP:DATA?\n", chan);
	if (errStatus < VI_SUCCESS)
	  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

	errStatus = viScanf(vi, "%lg%*t", level);
	if (errStatus < VI_SUCCESS)
	  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
      } else {			/* limit checking not enabled */

	*level = 0.0;
	*mode = hpe1564_LIMIT_OFF;
      }
    }
  } else {			/* not E1406 */

    errStatus = hpe1564_regCalcLimStat_Q(vi, thisPtr, chan, mode);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    if (*mode == hpe1564_LIMIT_BELOW) {	/* lower limit check enabled */
      errStatus = hpe1564_regCalcLimLowData_Q(vi, thisPtr, chan, level);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    } else if (*mode == hpe1564_LIMIT_ABOVE) {	/* see if upper limit */
      errStatus = hpe1564_regCalcLimUppData_Q(vi, thisPtr, chan, level);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    } else {			/* limit checking not enabled */

      *level = 0.0;
    }
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_maxMin
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function will find the maximum and minimum voltage
 *           readings on a channel and report those values and the time or
 *           position of their occurrence.  This function does not take new
 *           readings.  The measurement must either have been started or have
 *           completed before calling this function.
 *           
 *            NOTE: We limit the search to 524288 readings in size.  To look
 *           through larger data sets, call this routine multiple times,
 *           saving the previous results to obtain the absolute min and max.
 *           
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 channel
 * IN        
 *            Specifies which channel to configure and measure.
 *           
 *            The channels are defined as macros and are shown below:
 * 
 *      Macro Name         Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_CHAN1          1  1
 *      hpe1564_CHAN2          2  2
 *      hpe1564_CHAN3          3  3
 *      hpe1564_CHAN4          4  4
 * 
 * PARAM 3 : ViPReal64 maxVolt
 * OUT       
 *            Returns the maximum measured voltage on the specified channel.
 * 
 * PARAM 4 : ViPReal64 maxTime
 * OUT       
 *            Returns the first time the maximum measured voltage occurred
 *            on the specified channel.
 *            If the sample source is Timer, the time given will be the time
 *            delay from the trigger event to the first occurrence of the
 *           maximum voltage.  If the sample source is not Timer, the value
 *           given will be in number of samples.  The maxTime may be negative
 *           if numPreTriggers is not 0 and the maximum Voltage occurs before
 *           the trigger event.
 * 
 * PARAM 5 : ViPReal64 minVolt
 * OUT       
 *            Returns the minimum measured voltage on the specified channel.
 * 
 * PARAM 6 : ViPReal64 minTime
 * OUT       
 *            Returns the first time that the minimum measured voltage
 *           occurred on the specified channel. If the sample source is
 *           Timer, the time given will be the time delay from the trigger
 *           event to the first occurrence of the minimum voltage.  If the
 *           sample source is not Timer, the value given will be in number of
 *            samples.  The minTime may be negative if numPreTriggers is
 *            not 0 and the minimum Voltage occurs before the trigger event.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_maxMin(ViSession vi,
				 ViInt16 chan,
				 ViPReal64 maxVolt,
				 ViPReal64 maxTime, ViPReal64 minVolt, ViPReal64 minTime)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (VI_SUCCESS > errStatus) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_maxMin");

  /* Perform Error Checking on Each Parameter */
  hpe1564_CHK_ENUM(chan, 4, VI_ERROR_PARAMETER2);

  {
    ViInt32 totalCount;
    ViInt32 sampCount;
    ViInt32 preTrigs;
    ViReal64 range;
    ViReal64 sampTime;
#if defined(WIN32) || defined(__hpux)
    ViInt16 *data;		/* Use integer -- faster & doesn't need as much memory */
#else
    ViInt16 _huge *data;	/* Use integer -- faster & doesn't need as much memory */
#endif
    ViInt16 maxV;
    ViInt16 minV;

    ViInt32 iMin;
    ViInt32 iMax;
    ViInt32 i;
    ViInt16 sampSour;
    ViInt16 blockCount;
    ViInt32 howMany;

    *maxVolt = -99999;
    *minVolt = 99999;
    *maxTime = -99999999;
    *minTime = 99999999;
    maxV = -32767;
    minV = 32767;
    iMax = -99999999;
    iMin = 99999999;

    /* make sure data format is packed */
    if (thisPtr->e1406) {
      errStatus = viPrintf(vi, "FORM:DATA PACK\n");
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
      thisPtr->digState.dataFormat = hpe1564_FORM_PACK;
    }

    errStatus = hpe1564_range_Q(vi, chan, &range);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = hpe1564_sample_Q(vi, &sampCount, &preTrigs, &sampSour, &sampTime);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* If infinite measurement, then just get 524288 rdgs */
    if (sampCount < 0)
      sampCount = 524288;

    /* To save memory, we will get data in "blocks" of readings.  Set
       the block size to be 10000 readings.  We will malloc an array
       now for sampCount readings or 10000 readings, whichever is
       smaller.
     */
    if (sampCount < 10000)
      blockCount = (ViInt16) sampCount;
    else
      blockCount = 10000;

#if defined(WIN32) || defined (__hpux)
    data = (ViInt16 *) malloc((long)blockCount * sizeof(ViInt16));
#else
    data = (ViInt16 _huge *) _halloc((long)blockCount, sizeof(ViInt16));
#endif

    if (data == NULL)
      hpe1564_LOG_STATUS(vi, thisPtr, VI_ERROR_ALLOC);

    totalCount = 0;		/* total number we have read so far */
    while (sampCount > totalCount) {

      if (chan == 1)
	errStatus = hpe1564_fetch_Q(vi, blockCount, data, NULL, NULL, NULL, &howMany);
      else if (chan == 2)
	errStatus = hpe1564_fetch_Q(vi, blockCount, NULL, data, NULL, NULL, &howMany);
      else if (chan == 3)
	errStatus = hpe1564_fetch_Q(vi, blockCount, NULL, NULL, data, NULL, &howMany);
      else if (chan == 4)
	errStatus = hpe1564_fetch_Q(vi, blockCount, NULL, NULL, NULL, data, &howMany);

      if (errStatus < VI_SUCCESS || howMany == 0)
	goto quit;

      /* OK, got the data ... analyze  */
      for (i = 0; i < howMany; i++) {
	if (data[i] < minV) {
	  minV = data[i];
	  iMin = i + totalCount;
	}
	if (data[i] > maxV) {
	  maxV = data[i];
	  iMax = i + totalCount;
	}
      }

      totalCount += howMany;
    }

    *maxVolt = range * maxV / 32768.0;
    *minVolt = range * minV / 32768.0;

    /* Subtract the pre-triggers */
    if (preTrigs) {
      iMin -= preTrigs;
      iMax -= preTrigs;
    }

    if (sampSour == hpe1564_SAMP_TIM) {	/* See if sample source is timer */
      *maxTime = iMax * sampTime;
      *minTime = iMin * sampTime;
    } else {
      *maxTime = iMax;
      *minTime = iMin;
    }

 quit:
#ifdef WIN32
    free(data);
#else
    _hfree(data);
#endif

  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_measure_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function will configure all channels to take readings on
 *           the range indicated by the "expected" parameter, will initiate
 *           the measurement, and bring back the scaled data for all
 *           channels. The trigger source is set to immediate and the sample
 *           source is set to timer with the fastest sampling period.
 *           
 *            This is equivalent to doing hpe1564_configure() on all
 *           channels, followed by hpe1564_initImm() and
 *           hpe1564_fetchScal_Q() all done in sequence.  The expected value
 *           parameter specified should be the maximum expected measurement
 *           amplitude (absolute value) for the worst case channel.  The
 *           voltage range for all channels is set according to the expected
 *           value supplied.  If the value is greater than 98% of the
 *           instrument's range, the next higher range is automatically
 *           chosen.  The following table gives the crossover points for
 *           range changes; note that negative expected values are converted to positive values when deciding range.
 *           
 *             Maximum Expected Value   Voltage Range    Resolution
 *                     0.06125             0.0625        .000007629
 *                     0.24500             0.2500        .000030518
 *                     0.98000             1.0000        .000122070
 *                     3.92000             4.0000        .000488281
 *                    15.68000            16.0000        .001953125
 *                    62.72000            64.0000        .007812500
 *                   250.88000           256.0000        .031250000
 *           
 *            The numSamples parameter specifies how many readings will be
 *           taken per channel. The total number of readings allowed is a
 *           function of optional memory, but at most will be 128 Meg
 *           (16,777,216 readings per channel).
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViReal64 expected
 * IN        
 *            Specifies the maximum expected voltage for the specified
 *           channel.
 * 
 *      MAX = hpe1564_VOLT_MAX   256.0001
 *      MIN = hpe1564_VOLT_MIN   -256.0001
 * 
 * PARAM 3 : ViInt32 numSamples
 * IN        
 *            Specifies the total number of readings to take on each channel.
 * 
 *      MAX = hpe1564_SAMP_COUNT_MAX   16777216
 *      MIN = hpe1564_SAMP_COUNT_MIN   1
 * 
 * PARAM 4 : ViInt32 arrayLength
 * IN        
 *            Specifies the number of readings to bring back in each data
 *           array.  The data arrays must be at least this large, or they
 *           must be NULL (0L).
 * 
 *      MAX = hpe1564_FETCH_MAX   2147483646
 *      MIN = hpe1564_FETCH_MIN   1
 * 
 * PARAM 5 : ViInt16 _VI_FAR data1[]
 * OUT       
 *            Pointer to the array of 16 bit integers which will hold the A/D
 *           reading values from channel 1.  If a null pointer is given (0L),
 *           no data will be returned for this channel.
 *           
 *            NOTE:  In HP VEE, NULL pointers are not allowed, so you must
 *           give a valid array pointer for each channel when using HP VEE.
 * 
 * PARAM 6 : ViInt16 _VI_FAR data2[]
 * OUT       
 *            Pointer to the array of 16 bit integers which will hold the A/D
 *           reading values from channel 2.  If a null pointer is given (0L),
 *           no data will be returned for this channel.
 *           
 *            NOTE:  In HP VEE, NULL pointers are not allowed, so you must
 *           give a valid array pointer for each channel when using HP VEE.
 * 
 * PARAM 7 : ViInt16 _VI_FAR data3[]
 * OUT       
 *            Pointer to the array of 16 bit integers which will hold the A/D
 *           reading values from channel 3.  If a null pointer is given (0L),
 *           no data will be returned for this channel.
 *           
 *            NOTE:  In HP VEE, NULL pointers are not allowed, so you must
 *           give a valid array pointer for each channel when using HP VEE.
 * 
 * PARAM 8 : ViInt16 _VI_FAR data4[]
 * OUT       
 *            Pointer to the array of 16 bit integers which will hold the A/D
 *           reading values from channel 4.  If a null pointer is given (0L),
 *           no data will be returned for this channel.
 *           
 *            NOTE:  In HP VEE, NULL pointers are not allowed, so you must
 *           give a valid array pointer for each channel when using HP VEE.
 * 
 * PARAM 9 : ViPInt32 howMany
 * OUT       
 *            The number of data points actually read into each data array is
 *           returned in this parameter.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_measure_Q(ViSession vi,
				    ViReal64 expected,
				    ViInt32 numSamples,
				    ViInt32 arrayLength,
				    ViReal64 _VI_FAR data1[],
				    ViReal64 _VI_FAR data2[],
				    ViReal64 _VI_FAR data3[],
				    ViReal64 _VI_FAR data4[], ViPInt32 howMany)
{

  ViStatus errStatus = 0;
  ViReal64 dTemp = 1.3e-6, dLevel = 0.0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_measure_Q");

  hpe1564_CHK_REAL_RANGE(expected, hpe1564_VOLT_MIN, hpe1564_VOLT_MAX, VI_ERROR_PARAMETER2);

  hpe1564_CHK_LONG_RANGE(numSamples, hpe1564_SAMP_COUNT_MIN, hpe1564_SAMP_COUNT_MAX,
			 VI_ERROR_PARAMETER3);

  hpe1564_CHK_LONG_RANGE(arrayLength, hpe1564_FETCH_ALL_MIN, hpe1564_FETCH_ALL_MAX,
			 VI_ERROR_PARAMETER4);

  hpe1564_CHK_LONG_RANGE(arrayLength, 1, hpe1564_FETCH_ALL_MAX, VI_ERROR_PARAMETER4);

  /* round expected to proper range */
  if (expected < 0.0)
    expected *= -1.0;		/* make it positive */

  if (expected <= 0.061250)
    expected = 0.062;
  else if (expected <= 0.2450)
    expected = 0.245;
  else if (expected <= 0.980)
    expected = 0.980;
  else if (expected <= 3.920)
    expected = 3.920;
  else if (expected <= 15.680)
    expected = 15.680;
  else if (expected <= 62.720)
    expected = 62.720;
  else
    expected = 250.9;

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  errStatus = hpe1564_trigMode(vi, hpe1564_TRIG_MODE_NORMAL);
  if (errStatus < VI_SUCCESS)
    hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

  errStatus = hpe1564_trigEvent(vi, 1, hpe1564_TRIG_IMM, &dLevel);
  if (errStatus < VI_SUCCESS)
    hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

  errStatus = hpe1564_trigEvent(vi, 2, hpe1564_TRIG_HOLD, &dLevel);
  if (errStatus < VI_SUCCESS)
    hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

  dTemp = hpe1564_OFF;
  errStatus = hpe1564_input(vi, &expected, &dTemp, &expected, &dTemp,
			    &expected, &dTemp, &expected, &dTemp);
  if (errStatus < VI_SUCCESS)
    hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

  errStatus = hpe1564_sample(vi, numSamples, 0, hpe1564_SAMP_TIM, &dTemp);
  if (errStatus < VI_SUCCESS)
    hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

  errStatus = hpe1564_initImm(vi);
  if (errStatus < VI_SUCCESS)
    hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

  errStatus = hpe1564_fetchScal_Q(vi, arrayLength, data1, data2, data3, data4, howMany);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_outpTtltSour
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function sets the source of output pulses for the
 *           specified TTL trigger line.  Note that only one line may be
 *           enabled to output a particular source.  For example, the trigger
 *           event can be output onto TTL line 0 and the sample event can be
 *            output onto TTL line 1 simultaneously, but the trigger event
 *           can not be output onto both TTL line 0 and 1 at the same time;
 *           and likewise, the sample event can not be output onto two lines
 *            simultaneously.  Additionally, BOTH sources may be enabled to
 *            output onto a single line, but no other lines may be enabled
 *            when this condition exists.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 line
 * IN        
 *            Specifies which of the TTL trigger lines will have the
 *            source changed.
 *           
 *            The trigger lines are defined as macros and are shown below:
 * 
 *      Macro Name         Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_TTLT0          0  TTL Trigger line 0
 *      hpe1564_TTLT1          1  TTL Trigger line 1
 *      hpe1564_TTLT2          2  TTL Trigger line 2
 *      hpe1564_TTLT3          3  TTL Trigger line 3
 *      hpe1564_TTLT4          4  TTL Trigger line 4
 *      hpe1564_TTLT5          5  TTL Trigger line 5
 *      hpe1564_TTLT6          6  TTL Trigger line 6
 *      hpe1564_TTLT7          7  TTL Trigger line 7
 * 
 * PARAM 3 : ViInt16 source
 * IN        
 *            The desired source to cause pulses on the specified TTL trigger
 *            line.  The allowable choices are shown in the following macros.
 * 
 *      Macro Name             Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_OUTP_TRIG          0  Trigger event
 *      hpe1564_OUTP_SAMP          1  Sample event(s)
 *      hpe1564_OUTP_BOTH          2  Both events
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1564_outpTtltSour_source_a[] = { "TRIG",
  "SAMP", "BOTH", 0
};

ViStatus _VI_FUNC hpe1564_outpTtltSour(ViSession vi, ViInt16 line, ViInt16 source)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_outpTtltSour");

  hpe1564_CHK_ENUM(line, 7, VI_ERROR_PARAMETER2);
  hpe1564_CHK_ENUM(source, 2, VI_ERROR_PARAMETER3);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406) {
    errStatus = viPrintf(vi, "OUTP:TTLT%hd:SOUR %s\n", line, hpe1564_outpTtltSour_source_a[source]);
  } else {
    errStatus = hpe1564_regOutpTtltSour(vi, thisPtr, line, source);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* force PMT call to handle couplings */
    errStatus = hpe1564_regTrigPmt(vi, thisPtr);
  }
  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_outpTtltSour_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function queries the present output pulse source
 *            for the specified TTL trigger line.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 line
 * IN        
 *            Specifies which TTL trigger line to query the source on.
 *           
 *            The possible TTL trigger lines are defined as macros and are
 *           shown below:
 * 
 *      Macro Name         Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_TTLT0          0  TTL trigger line 0
 *      hpe1564_TTLT1          1  TTL trigger line 1
 *      hpe1564_TTLT2          2  TTL trigger line 2
 *      hpe1564_TTLT3          3  TTL trigger line 3
 *      hpe1564_TTLT4          4  TTL trigger line 4
 *      hpe1564_TTLT5          5  TTL trigger line 5
 *      hpe1564_TTLT6          6  TTL trigger line 6
 *      hpe1564_TTLT7          7  TTL trigger line 7
 * 
 * PARAM 3 : ViPBoolean source
 * OUT       
 *            The source of the pulse, (0 = trigger event), (1 = sample
 *           event), and (2 = Both sample and trigger events).  These values
 *           are shown in the macros below.
 * 
 *      Macro Name             Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_OUTP_TRIG          0  Trigger event
 *      hpe1564_OUTP_SAMP          1  Sample event(s)
 *      hpe1564_OUTP_BOTH          2  Both events
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1564_outpTtltSour_Q_source_a[] = { "TRIG",
  "SAMP", "BOTH", 0
};

ViStatus _VI_FUNC hpe1564_outpTtltSour_Q(ViSession vi, ViInt16 line, ViPInt16 source)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;
  char source_str[10];

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_outpTtltSour_Q");

  hpe1564_CHK_ENUM(line, 7, VI_ERROR_PARAMETER2);
  if (thisPtr->e1406) {
    errStatus = viPrintf(vi, "OUTP:TTLT%hd:SOUR?\n", line);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viScanf(vi, "%s%*t", source_str);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = hpe1564_findIndex(thisPtr, hpe1564_outpTtltSour_Q_source_a, source_str, source);
  } else
    errStatus = hpe1564_regOutpTtltSour_Q(vi, thisPtr, line, source);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_outpTtltStat
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function enables the specified TTL line to provide
 *            output pulses when either the trigger event, sample event,
 *            or both occur.
 *           
 *            Because the TTL trigger lines can be used as either an input
 *            or an output, some resource conflicts can occur with the
 *            TTL trigger lines.  See the "COMMENTS" section of the help file
 *            for the specifics on the conflicts and their resolutions.
 *           
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 line
 * IN        
 *            Specifies which of the TTL trigger lines will have the
 *            source changed.
 *           
 *            The trigger lines are defined as macros and are shown below:
 * 
 *      Macro Name         Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_TTLT0          0  TTL Trigger line 0
 *      hpe1564_TTLT1          1  TTL Trigger line 1
 *      hpe1564_TTLT2          2  TTL Trigger line 2
 *      hpe1564_TTLT3          3  TTL Trigger line 3
 *      hpe1564_TTLT4          4  TTL Trigger line 4
 *      hpe1564_TTLT5          5  TTL Trigger line 5
 *      hpe1564_TTLT6          6  TTL Trigger line 6
 *      hpe1564_TTLT7          7  TTL Trigger line 7
 * 
 * PARAM 3 : ViBoolean state
 * IN        
 *            The desired state -- ON (1) or OFF (0) of the specified
 *            TTL trigger line.  When ON, pulses will be output onto the
 *            line when the TTL output source (hpe1564_outpTtltSour) event
 *            occurs.
 *            The allowable settings are shown in the following macros.
 * 
 *      Macro Name       Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_OFF          0  Disable outputting
 *      hpe1564_ON           1  Enable outputting
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_outpTtltStat(ViSession vi, ViInt16 line, ViBoolean state)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_outpTtltStat");

  hpe1564_CHK_ENUM(line, 7, VI_ERROR_PARAMETER2);
  hpe1564_CHK_BOOLEAN(state, VI_ERROR_PARAMETER3);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406)
    errStatus = viPrintf(vi, "OUTP:TTLT%hd %hd\n", line, state);
  else {
    errStatus = hpe1564_regOutpTtltStat(vi, thisPtr, line, state);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* force PMT call to handle couplings */
    errStatus = hpe1564_regTrigPmt(vi, thisPtr);
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_outpTtltStat_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function queries the present state of outputting
 *            for the TTL trigger line specified.  A return value for
 *            state of 0 indicates that outputting is disabled, while
 *            a 1 indicates outputting is enabled.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 line
 * IN        
 *            Specifies which TTL trigger line to query the state on.
 *           
 *            The possible TTL trigger lines are defined as macros and are
 *           shown below:
 * 
 *      Macro Name         Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_TTLT0          0  TTL trigger line 0
 *      hpe1564_TTLT1          1  TTL trigger line 1
 *      hpe1564_TTLT2          2  TTL trigger line 2
 *      hpe1564_TTLT3          3  TTL trigger line 3
 *      hpe1564_TTLT4          4  TTL trigger line 4
 *      hpe1564_TTLT5          5  TTL trigger line 5
 *      hpe1564_TTLT6          6  TTL trigger line 6
 *      hpe1564_TTLT7          7  TTL trigger line 7
 * 
 * PARAM 3 : ViPBoolean state
 * OUT       
 *            The state of the TTL trigger line for outputting pulses -- 0 =
 *           OFF, 1 = ON.
 * 
 *      Macro Name       Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_OFF          0  Disable output
 *      hpe1564_ON           1  Enable output
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_outpTtltStat_Q(ViSession vi, ViInt16 line, ViPBoolean state)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_outpTtltStat_Q");

  hpe1564_CHK_ENUM(line, 7, VI_ERROR_PARAMETER2);
  if (thisPtr->e1406) {
    errStatus = viPrintf(vi, "OUTP:TTLT%hd?\n", line);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viScanf(vi, "%hd%*t", state);
  } else
    errStatus = hpe1564_regOutpTtltStat_Q(vi, thisPtr, line, state);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_range
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function will change the range on the specified channel to
 *            at least the level specified.  If the range value specified
 *            falls between two of the instrument's ranges, the higher range
 *            is used (not the nearest value).
 *            The following table gives the crossover points for range
 *           changes. Note that a full scale reading on a given range may
 *           actually be an overload condition.
 *           
 *           Voltage Range    Resolution
 *             0.0625        .000007629
 *             0.2500        .000030518
 *             1.0000        .000122070
 *             4.0000        .000488281
 *            16.0000        .001953125
 *            64.0000        .007812500
 *           256.0000        .031250000
 *           
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 channel
 * IN        
 *            Specifies which channel to change range on.
 *           
 *            The channels are defined as macros and are shown below:
 * 
 *      Macro Name         Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_CHAN1          1  1
 *      hpe1564_CHAN2          2  2
 *      hpe1564_CHAN3          3  3
 *      hpe1564_CHAN4          4  4
 * 
 * PARAM 3 : ViReal64 range
 * IN        
 *            Specifies the voltage range to use for the measurement.
 *            There are 7 different ranges, the ranges are as shown in
 *            the macros below.
 * 
 *      Macro Name               Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_RANGE_63MV      0.0625  0.0625
 *      hpe1564_RANGE_250MV      0.250  0.250
 *      hpe1564_RANGE_1            1.0  1.0
 *      hpe1564_RANGE_4            4.0  4.0
 *      hpe1564_RANGE_16          16.0  16.0
 *      hpe1564_RANGE_64          64.0  64.0
 *      hpe1564_RANGE_256        256.0  256.0
 * 
 *      MAX = hpe1564_VOLT_MAX   256.0001
 *      MIN = hpe1564_VOLT_MIN   -256.0001
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_range(ViSession vi, ViInt16 chan, ViReal64 range)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_range");

  hpe1564_CHK_INT_RANGE(chan, 1, 4, VI_ERROR_PARAMETER2);
  hpe1564_CHK_REAL_RANGE(range, hpe1564_VOLT_MIN, hpe1564_VOLT_MAX, VI_ERROR_PARAMETER3);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406)
    errStatus = viPrintf(vi, "VOLT%hd:RANG %lg\n", chan, range);
  else {
    errStatus = hpe1564_regInpRange(vi, thisPtr, chan, range);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = hpe1564_regInputPmt(vi, thisPtr);
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_range_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function will query the range setting for the specified
 *            channel.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 channel
 * IN        
 *            Specifies the channel to query the range on.
 *           
 *            The channels are defined as macros and are shown below:
 * 
 *      Macro Name         Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_CHAN1          1  1
 *      hpe1564_CHAN2          2  2
 *      hpe1564_CHAN3          3  3
 *      hpe1564_CHAN4          4  4
 * 
 * PARAM 3 : ViPReal64 range
 * OUT       
 *            The present range setting on the specified channel is
 *            returned in this parameter.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_range_Q(ViSession vi, ViInt16 chan, ViPReal64 range)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_range_Q");

  hpe1564_CHK_INT_RANGE(chan, 1, 4, VI_ERROR_PARAMETER2);
  if (thisPtr->e1406) {
    errStatus = viPrintf(vi, "VOLT%hd:RANG?\n", chan);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viScanf(vi, "%lg%*t", range);
  } else
    errStatus = hpe1564_regInpRange_Q(vi, thisPtr, chan, range);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_sampCount
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function selects the total number of readings the
 *           instrument will take, and the number of readings that are taken
 *           before the trigger event is accepted (if numPreTriggers is
 *           non-zero).
 *            If numPreTriggers is non-zero, the instrument will begin taking
 *           readings immediately after the hpe1564_initImm is received, and
 *           will ignore all trigger events until numPreTriggers readings
 *           have been taken.  Once numPreTrigger readings have been taken,
 *           the instrument will continue taking pretrigger readings until
 *           the trigger occurs, keeping only the the most recent
 *           numPreTriggers readings in memory.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt32 numSamples
 * IN        
 *            Specifies the total number of readings to take.
 * 
 *      MAX = hpe1564_SAMP_COUNT_MAX   4000000
 *      MIN = hpe1564_SAMP_COUNT_MIN   1
 * 
 * PARAM 3 : ViInt32 numPreTriggers
 * IN        
 *            Specifies how many of the total readings will be taken before
 *           (pre) the trigger event occurs.
 * 
 *      MAX = hpe1564_PRETRIG_MAX   3999999
 *      MIN = hpe1564_PRETRIG_MIN   0
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_sampCount(ViSession vi, ViInt32 numSamples, ViInt32 numPreTriggers)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_sampCount");

  hpe1564_CHK_LONG_RANGE(numSamples, hpe1564_SAMP_COUNT_MIN, hpe1564_SAMP_COUNT_MAX,
			 VI_ERROR_PARAMETER2);

  hpe1564_CHK_LONG_RANGE(numPreTriggers, hpe1564_PRETRIG_MIN, hpe1564_PRETRIG_MAX,
			 VI_ERROR_PARAMETER3);

  /* make sure numSamps > numPreTriggers */
  hpe1564_CHK_LONG_RANGE(numPreTriggers, 0, (numSamples - 1), VI_ERROR_PARAMETER3);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406)
    errStatus = viPrintf(vi, ":SAMP:COUN %ld;PRET:COUN %ld\n", numSamples, numPreTriggers);
  else {
    char message[40];

    /* the following checks can only be done on this path, because the
       D-SCPI thisPtr is not the one we have here.
     */

    /* check sample count against specific memory option */
    if (thisPtr->maxSampCount < numSamples) {
      sprintf(message, "Max. count is %ld", thisPtr->maxSampCount);
      hpe1564_CDE_MESSAGE(message);
      hpe1564_LOG_STATUS(vi, thisPtr, VI_ERROR_PARAMETER2);
    }

    /* check pretrigger count against specific memory option */
    if (thisPtr->maxSampCount - 1 < numPreTriggers) {
      sprintf(message, "Max. pretrigger is %ld", thisPtr->maxSampCount - 1);
      hpe1564_CDE_MESSAGE(message);
      hpe1564_LOG_STATUS(vi, thisPtr, VI_ERROR_PARAMETER3);
    }

    errStatus = hpe1564_regSampCount(vi, thisPtr, numSamples);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = hpe1564_regPreSampCount(vi, thisPtr, numPreTriggers);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = hpe1564_regSampCountPmt(vi, thisPtr);
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_sampCount_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function queries the present setting of the sample count.
 *           
 *           The available counts are:
 *           
 *             Timer: The internal timebase controls when data is taken.
 *                    The rate is controlled by calling the hpe1564_sampTim
 *                    or hpe1564_sample functions.
 *           
 *             External 1: The "Ext 1" pin of the HP E1564's 9 pin
 *                         D-subminiature connector.
 *           
 *             External 2: The "Ext 2" pin of the HP E1564's 9 pin
 *                         D-subminiature connector.
 *           
 *             TTLTrg0 through TTLTrg7: The VXIbus TTL trigger lines.
 *           
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViPInt32 numSamples
 * OUT       
 *            The total number of readings the instrument will take during
 *            a measurement is returned here.  A value of -1 is returned here
 *           if continuous measurement is enabled (see hpe1564_initCont);
 *           this indicates an infinite total reading count.
 * 
 * PARAM 3 : ViPInt32 numPreTriggers
 * OUT       
 *            Returns how many readings will be taken before the trigger
 *           event may be accepted.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_sampCount_Q(ViSession vi, ViPInt32 numSamples, ViPInt32 numPreTriggers)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_sampCount_Q");

  if (thisPtr->e1406) {
    ViReal64 bigSamp;

    errStatus = viQueryf(vi, "SAMP:COUN?\n", "%lg%*t", &bigSamp);
    if (errStatus < VI_SUCCESS) {
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = viQueryf(vi, "SAMP:PRET:COUN?\n", "%ld%*t", numPreTriggers);
    if (errStatus < VI_SUCCESS) {
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    }

    /* If CONTinuous init, D-SCPI will report back 9.9E37 for count,
       which is infinite.  We will report back -1 here to caller.
     */
    if (bigSamp < 2.4E9) {	/* The instrument could be set to 9.9E37 */
      bigSamp = bigSamp + 0.1;	/* needed for CVI round off */
      *numSamples = (ViInt32) (bigSamp);
    } else
      *numSamples = -1;		/* Indicate INF triggers */
  } else {
    errStatus = hpe1564_regSampCount_Q(vi, thisPtr, numSamples);
    if (errStatus < VI_SUCCESS) {
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = hpe1564_regPreSampCount_Q(vi, thisPtr, numPreTriggers);
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_sampImm
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function is really a diagnostic type of function.  It is
 *            only useful when the sample source can be suspended (i.e. the
 *            sample source is HOLD, EXTernal, or TTLT<n>).
 *           
 *            This function, when called, will force a single sample event.
 *            The instrument must be initiated and in the
 *            wait-for-sample state when this function is called; otherwise,
 *           a "Sample ignored" error will result.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_sampImm(ViSession vi)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (VI_SUCCESS > errStatus) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_sampImm");

  /* Perform Error Checking on Each Parameter */
  if (thisPtr->e1406)
    errStatus = viPrintf(vi, "SAMP\n");
  else
    errStatus = hpe1564_regSampImm(vi, thisPtr);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_sampSlop
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function sets the sample slope.  The slope is
 *            set, but ignored, if the sample source is not one of
 *            External 1 or External 2.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViBoolean slope
 * IN        
 *            This parameter specifies the active edge of the sample source. 
 *           VI_TRUE (1) specifies a POSitive slope (rising edge), and
 *           VI_FALSE (0) specifies a NEGative slope (falling edge).
 * 
 *      Macro Name            Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_SLOP_NEG          0  Falling Edge
 *      hpe1564_SLOP_POS          1  Rising Edge
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_sampSlop(ViSession vi, ViBoolean slope)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_sampSlop");

  hpe1564_CHK_BOOLEAN(slope, VI_ERROR_PARAMETER2);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406) {
    if (slope == hpe1564_SLOP_NEG)
      errStatus = viPrintf(vi, ":SAMP:SLOP NEG\n");
    else
      errStatus = viPrintf(vi, ":SAMP:SLOP POS\n");
  } else {
    errStatus = hpe1564_regSampSlop(vi, thisPtr, slope);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    errStatus = hpe1564_regTrigPmt(vi, thisPtr);
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_sampSlop_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function queries the present setting of the sample
 *            slope.  This slope is only active if the sample source
 *            is one of External 1 or External 2.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViPBoolean slope
 * OUT       
 *            The active edge of the sample source is returned in this
 *           parameter. VI_TRUE (1) specifies a POSitive slope (rising edge),
 *           and VI_FALSE (0) specifies a NEGative slope (falling edge). 
 *           These settings are represented by the macros below.
 * 
 *      Macro Name            Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_SLOP_NEG          0  Falling Edge
 *      hpe1564_SLOP_POS          1  Rising Edge
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1564_sampSlop_Q_slope_a[] = { "NEG", "POS", 0 };

ViStatus _VI_FUNC hpe1564_sampSlop_Q(ViSession vi, ViPBoolean slope)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;
  char slope_str[10];
  ViInt16 tslope;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_sampSlop_Q");

  if (thisPtr->e1406) {
    errStatus = viPrintf(vi, ":SAMP:SLOP?\n");
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viScanf(vi, "%s%*t", slope_str);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = hpe1564_findIndex(thisPtr, hpe1564_sampSlop_Q_slope_a, slope_str, &tslope);
    *slope = (0 == tslope) ? VI_FALSE : VI_TRUE;
  } else
    errStatus = hpe1564_regSampSlop_Q(vi, thisPtr, slope);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_sampSour
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function combines two instrument SCPI commands:
 *              :SAMPle[:STARt]:SLOPe
 *              :SAMPle[:STARt]:SOURce
 *            Together, these commands allow you to configure the sample
 *            subsystem to respond to a specific source and a specific edge
 *            on a pulse (if source is External).
 *           
 *            The SAMPle command subsystem controls the fourth state in a
 *           four state measurement process.  The four states which occur
 *           during a successful reading are idle, initiated,
 *           wait-for-trigger, and wait-for-sample.  The last two states have
 *           event detection associated with them which control when they
 *           exit the current state.  For a more detailed explanation of
 *           these states, please refer to the instrument manuals.
 *            In the wait-for-sample state, the instrument waits for the
 *           specified sample event to occur, and when it occurs, a reading
 *           is taken.  After a reading is taken, the cumulative number of
 *           readings taken thus far is compared to the count specified by
 *           numSamples in hpe1564_configure(). When the count is met, the
 *           state is exited, otherwise, the instrument waits for another
 *           trigger and takes another reading.  Upon exit from this state,
 *           the instrument goes to the idle state.
 *            The source parameter configures the sample system to respond to
 *           the specified source for taking readings.
 *           
 *           The available sources are:
 *             External, Rising: The rising edge of a pulse on the
 *                "Sample" pin of the HP E1564's 9 pin D-subminiature
 *                connector.  The pulse width must be at least 30e-9
 *                seconds.
 *           
 *             External, Falling: The falling edge of a pulse on the
 *                "Sample" pin of the HP E1564's 9 pin D-subminiature
 *                connector.  The pulse width must be at least 30e-9
 *                seconds.
 *           
 *             TTLTrg0 through TTLTrg7: The VXIbus TTL sample lines.
 *                The pulse width must be at least 30e-9 seconds.
 *           
 *             Timer: Uses the specified period as the sample rate.
 *           
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 source
 * IN        
 *            This parameter configures the trigger system to respond to the
 *            specified source for taking readings.
 *           
 *            The types are defined as macros and are shown below:
 * 
 *      Macro Name                 Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_SAMP_HOLD              0  Wait for Software sample
 *      hpe1564_SAMP_TIM               1  Timer
 *      hpe1564_SAMP_TTLT0             2  TTLTrg0
 *      hpe1564_SAMP_TTLT1             3  TTLTrg1
 *      hpe1564_SAMP_TTLT2             4  TTLTrg2
 *      hpe1564_SAMP_TTLT3             5  TTLTrg3
 *      hpe1564_SAMP_TTLT4             6  TTLTrg4
 *      hpe1564_SAMP_TTLT5             7  TTLTrg5
 *      hpe1564_SAMP_TTLT6             8  TTLTrg6
 *      hpe1564_SAMP_TTLT7             9  TTLTrg7
 *      hpe1564_SAMP_EXT_RISE         10  External Rising Edge
 *      hpe1564_SAMP_EXT_FALL         11  External Falling Edge
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1564_sampSour_source_a[] = { "HOLD", "TIM",
  "TTLT0", "TTLT1", "TTLT2", "TTLT3", "TTLT4", "TTLT5", "TTLT6", "TTLT7",
  "EXT", "EXT", 0
};

ViStatus _VI_FUNC hpe1564_sampSour(ViSession vi, ViInt16 source)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;
  char source_str[10];

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_sampSour");

  hpe1564_CHK_ENUM(source, 11, VI_ERROR_PARAMETER2);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406) {
    switch (source) {
    case hpe1564_SAMP_HOLD:
      errStatus = viPrintf(vi, ":SAMP:SOUR HOLD\n");
      break;

    case hpe1564_SAMP_TIM:
      errStatus = viPrintf(vi, ":SAMP:SOUR TIM\n");
      break;

    case hpe1564_SAMP_EXT_RISE:
      errStatus = viPrintf(vi, ":SAMP:SOUR EXT;SLOP POS\n");
      break;

    case hpe1564_SAMP_EXT_FALL:
      errStatus = viPrintf(vi, ":SAMP:SOUR EXT;SLOP NEG\n");
      break;

    default:
      errStatus = viPrintf(vi, ":SAMP:SOUR %s\n", hpe1564_sampSour_source_a[source]);
    }

    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
  } else {
    errStatus = hpe1564_regSampSour(vi, thisPtr, source);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    if (source == hpe1564_SAMP_EXT_RISE) {
      errStatus = hpe1564_regSampSlop(vi, thisPtr, hpe1564_SLOP_POS);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    }

    if (source == hpe1564_SAMP_EXT_FALL) {
      errStatus = hpe1564_regSampSlop(vi, thisPtr, hpe1564_SLOP_NEG);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = hpe1564_regTrigPmt(vi, thisPtr);
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_sampSour_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function queries the present setting of the sample source.
 *           
 *           The available sources are:
 *           
 *             Hold: Sampling only occurs when sample immediate command sent.
 *           
 *             Timer: The internal timebase controls when data is taken.
 *                    The rate is controlled by calling the hpe1564_sampTim
 *                    or hpe1564_sample functions.
 *           
 *             External 1: The "Ext 1" pin of the HP E1564's 9 pin
 *                         D-subminiature connector.
 *           
 *             TTLTrg0 through TTLTrg7: The VXIbus TTL trigger lines.
 *           
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViPInt16 source
 * OUT       
 *            The present sample source setting is returned in this
 *           parameter. The integer returned represents one of the following
 *           macro settings.
 * 
 *      Macro Name              Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_SAMP_HOLD           0  Hold
 *      hpe1564_SAMP_TIM            1  Timer
 *      hpe1564_SAMP_TTLT0          2  TTLTrg0
 *      hpe1564_SAMP_TTLT1          3  TTLTrg1
 *      hpe1564_SAMP_TTLT2          4  TTLTrg2
 *      hpe1564_SAMP_TTLT3          5  TTLTrg3
 *      hpe1564_SAMP_TTLT4          6  TTLTrg4
 *      hpe1564_SAMP_TTLT5          7  TTLTrg5
 *      hpe1564_SAMP_TTLT6          8  TTLTrg6
 *      hpe1564_SAMP_TTLT7          9  TTLTrg7
 *      hpe1564_SAMP_EXT          10 External 1
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */

static const char *const hpe1564_sampSour_Q_source_a[] = { "HOLD", "TIM", "TTLT0",
  "TTLT1", "TTLT2", "TTLT3", "TTLT4", "TTLT5", "TTLT6", "TTLT7", "EXT", 0
};

ViStatus _VI_FUNC hpe1564_sampSour_Q(ViSession vi, ViPInt16 source)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;
  char source_str[10];
  char slope_str[10];
  ViInt16 slope;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_sampSour_Q");

  if (thisPtr->e1406) {
    errStatus = viPrintf(vi, ":SAMP:SOUR?\n");
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viScanf(vi, "%s%*t", source_str);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = hpe1564_findIndex(thisPtr, hpe1564_sampSour_Q_source_a, source_str, source);

    /* if source was EXT, we need to know slope */
    if (*source > hpe1564_SAMP_TTLT7) {
      errStatus = viPrintf(vi, "SAMP:SLOP?\n");
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = viScanf(vi, "%s%*t", slope_str);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = hpe1564_findIndex(thisPtr, hpe1564_sampSlop_Q_slope_a, slope_str, &slope);

      /* If we have negative slope, we need to change the
         source to EXT_FALL.  This is
         done by adding 1 to the current source value.
       */
      if (slope == hpe1564_SLOP_NEG)
	*source = *source + 1;
    }

  } else
    errStatus = hpe1564_regSampSour_Q(vi, thisPtr, source);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_sampTim
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function sets the sample period.  This is the
 *            rate at which each reading is taken during a measurement.
 *            This period is only active when the sample source is
 *            TIMer.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViReal64 period
 * IN        
 *            This parameter specifies the time period between each sample
 *           event when the sample source is TIMer. The sample period must be
 *           a multiple of 1.0E-7 seconds, up to a maximum of 0.8 seconds. 
 *           The specified value will be rounded to the nearest value that
 *           can be attained with the internal clock period.  Use the query
 *           form of this function to obtain the exact period used.
 * 
 *      MAX = hpe1564_SAMP_TIM_MAX   0.80
 *      MIN = hpe1564_SAMP_TIM_MIN   0.0000013
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_sampTim(ViSession vi, ViReal64 period)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_sampTim");

  hpe1564_CHK_REAL_RANGE(period, hpe1564_SAMP_TIM_MIN, hpe1564_SAMP_TIM_MAX, VI_ERROR_PARAMETER2);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406) {
    /* for longer periods, send 8 decimal places of info. */
    if (period > 0.065)
      errStatus = viPrintf(vi, ":SAMP:TIM %0.8lg\n", period);
    else
      errStatus = viPrintf(vi, ":SAMP:TIM %lg\n", period);
  } else
    errStatus = hpe1564_regSampTim(vi, thisPtr, period);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_sampTim_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function returns the present setting of the sample period.
 *            This is the rate at which each reading is taken during a
 *           measurement. This period is only active when the sample source
 *           is TIMer.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViPReal64 period
 * OUT       
 *            The time period between each reading taken is returned in this
 *            parameter.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_sampTim_Q(ViSession vi, ViPReal64 period)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_sampTim_Q");

  if (thisPtr->e1406) {
    errStatus = viPrintf(vi, ":SAMP:TIM?\n");
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viScanf(vi, "%lg%*t", period);
  } else
    errStatus = hpe1564_regSampTim_Q(vi, thisPtr, period);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_sample
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function combines five instrument SCPI commands:
 *              :SAMPle[:STARt]:SLOPe
 *              :SAMPle[:STARt]:SOURce
 *              :SAMPle[:STARt]:TIMer
 *              :SAMPle[:STARt]:COUNt
 *              :SAMPle[:STARt]:PRETrigger:COUNt
 *            Together, these commands allow you to configure the sample
 *            subsystem to take pre and post trigger readings, respond to a
 *           specific source, a specific edge on a pulse (if source is
 *           External), and also to specify the time period between each
 *           reading (if source is TIMer).  The time period is checked for
 *           valid range, but is not sent to the instrument unless the sample
 *           source is TIMer.
 *           
 *            The SAMPle command subsystem controls the fourth state in a
 *           four state measurement process.  The four states which occur
 *           during a successful reading are idle, initiated,
 *           wait-for-trigger, and wait-for-sample.  The last two states have
 *           event detection associated with them which control when they
 *           exit the current state.  For a more detailed explanation of
 *           these states, please refer to the instrument manuals.
 *            In the wait-for-sample state, the instrument waits for the
 *           specified sample event to occur, and when it occurs, a reading
 *           is taken.  After a reading is taken, the cumulative number of
 *           readings taken thus far is compared to the count specified by
 *           numSamples in hpe1564_configure(). When the count is met, the
 *           state is exited, otherwise, the instrument waits for another
 *           trigger and takes another reading.  Upon exit from this state,
 *           the instrument goes to the idle state.
 *            The source parameter configures the sample system to respond to
 *           the specified source for taking readings.
 *           
 *           The available sources are:
 *             External, Rising: The rising edge of a pulse on the
 *                "Sample" pin of the HP E1564's 9 pin D-subminiature
 *                connector.  The pulse width must be at least 30e-9
 *                seconds.
 *           
 *             External, Falling: The falling edge of a pulse on the
 *                "Sample" pin of the HP E1564's 9 pin D-subminiature
 *                connector.  The pulse width must be at least 30e-9
 *                seconds.
 *           
 *             TTLTrg0 through TTLTrg7: The VXIbus TTL sample lines.
 *                The pulse width must be at least 30e-9 seconds.
 *           
 *             Timer: Uses the specified period as the sample rate.
 *           
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt32 numSamples
 * IN        
 *            Specifies the total number of readings to take.
 * 
 *      MAX = hpe1564_SAMP_COUNT_MAX   4000000
 *      MIN = hpe1564_SAMP_COUNT_MIN   1
 * 
 * PARAM 3 : ViInt32 numPreTriggers
 * IN        
 *            Specifies how many of the total readings will be taken before
 *           (pre) the trigger event occurs.
 * 
 *      MAX = hpe1564_PRETRIG_MAX   3999999
 *      MIN = hpe1564_PRETRIG_MIN   0
 * 
 * PARAM 4 : ViInt16 source
 * IN        
 *            This parameter configures the trigger system to respond to the
 *            specified source for taking readings.
 *           
 *            The types are defined as macros and are shown below:
 * 
 *      Macro Name                 Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_SAMP_HOLD              0  Wait for Software sample
 *      hpe1564_SAMP_TIM               1  Timer
 *      hpe1564_SAMP_TTLT0             2  TTLTrg0
 *      hpe1564_SAMP_TTLT1             3  TTLTrg1
 *      hpe1564_SAMP_TTLT2             4  TTLTrg2
 *      hpe1564_SAMP_TTLT3             5  TTLTrg3
 *      hpe1564_SAMP_TTLT4             6  TTLTrg4
 *      hpe1564_SAMP_TTLT5             7  TTLTrg5
 *      hpe1564_SAMP_TTLT6             8  TTLTrg6
 *      hpe1564_SAMP_TTLT7             9  TTLTrg7
 *      hpe1564_SAMP_EXT_RISE         10  External Rising Edge
 *      hpe1564_SAMP_EXT_FALL         11  External Falling Edge
 * 
 * PARAM 5 : ViReal64 period
 * IN        
 *            This parameter specifies the time period between each sample
 *           event when the sample source is TIMer. The sample period must be
 *           a multiple of the timebase period (for INTernal timebase, this
 *           is 1.0E-7 seconds), up to a maximum of 0.8 seconds.  The
 *           specified value will be rounded to the nearest value that can be
 *           attained with the timebase reference period.  Use the query form
 *           of this function to obtain the exact period used.
 * 
 *      MAX = hpe1564_SAMP_TIM_MAX   0.83886070
 *      MIN = hpe1564_SAMP_TIM_MIN   0.00000124999
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1564_sample_source_a[] = { "HOLD", "TIM", "TTLT0",
  "TTLT1", "TTLT2", "TTLT3", "TTLT4", "TTLT5", "TTLT6", "TTLT7", "EXT", 0
};

ViStatus _VI_FUNC hpe1564_sample(ViSession vi,
				 ViInt32 numSamples,
				 ViInt32 numPreTriggers, ViInt16 source, ViPReal64 period)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_sample");

  hpe1564_CHK_LONG_RANGE(numSamples, hpe1564_SAMP_COUNT_MIN, hpe1564_SAMP_COUNT_MAX,
			 VI_ERROR_PARAMETER2);

  hpe1564_CHK_LONG_RANGE(numPreTriggers, hpe1564_PRETRIG_MIN, hpe1564_PRETRIG_MAX,
			 VI_ERROR_PARAMETER3);

  hpe1564_CHK_ENUM(source, 11, VI_ERROR_PARAMETER4);

  /* if source is timer, check parameter 5 */
  if (source == hpe1564_SAMP_TIM)
    hpe1564_CHK_REAL_RANGE(*period, hpe1564_SAMP_TIM_MIN, hpe1564_SAMP_TIM_MAX,
			   VI_ERROR_PARAMETER5);

  /* make sure numSamps > numPreTriggers */
  hpe1564_CHK_LONG_RANGE(numPreTriggers, 0, (numSamples - 1), VI_ERROR_PARAMETER3);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406) {
    switch (source) {
    case hpe1564_SAMP_HOLD:
      errStatus = viPrintf(vi, ":SAMP:SOUR HOLD\n");
      break;

    case hpe1564_SAMP_TIM:
      /* added 8 decimal places for large timer periods */
      if (*period > 0.065)
	errStatus = viPrintf(vi, ":SAMP:SOUR TIM;TIM %0.8lg\n", *period);
      else
	errStatus = viPrintf(vi, ":SAMP:SOUR TIM;TIM %lg\n", *period);
      break;

    case hpe1564_SAMP_EXT_RISE:
      errStatus = viPrintf(vi, ":SAMP:SOUR EXT;SLOP POS\n");
      break;

    case hpe1564_SAMP_EXT_FALL:
      errStatus = viPrintf(vi, ":SAMP:SOUR EXT;SLOP NEG\n");
      break;

    default:
      errStatus = viPrintf(vi, ":SAMP:SOUR %s\n", hpe1564_sample_source_a[source]);
    }

    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viPrintf(vi, ":SAMP:COUN %ld;PRET:COUN %ld\n", numSamples, numPreTriggers);
  } else {
    char message[40];

    /* the following checks can only be done on this path, because the
       D-SCPI thisPtr is not the one we have here.
     */

    /* check sample count against specific memory option */
    if (thisPtr->maxSampCount < numSamples) {
      sprintf(message, "Max. count is %ld", thisPtr->maxSampCount);
      hpe1564_CDE_MESSAGE(message);
      hpe1564_LOG_STATUS(vi, thisPtr, VI_ERROR_PARAMETER2);
    }

    /* check pretrigger count against specific memory option */
    if (thisPtr->maxSampCount - 1 < numPreTriggers) {
      sprintf(message, "Max. pretrigger is %ld", thisPtr->maxSampCount - 1);
      hpe1564_CDE_MESSAGE(message);
      hpe1564_LOG_STATUS(vi, thisPtr, VI_ERROR_PARAMETER3);
    }

    errStatus = hpe1564_regSampCount(vi, thisPtr, numSamples);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = hpe1564_regPreSampCount(vi, thisPtr, numPreTriggers);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = hpe1564_regSampCountPmt(vi, thisPtr);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = hpe1564_regSampSour(vi, thisPtr, source);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    switch (source) {
    case hpe1564_SAMP_TIM:
      errStatus = hpe1564_regSampTim(vi, thisPtr, *period);
      break;

    case hpe1564_SAMP_EXT_RISE:
      errStatus = hpe1564_regSampSlop(vi, thisPtr, hpe1564_SLOP_POS);
      break;

    case hpe1564_SAMP_EXT_FALL:
      errStatus = hpe1564_regSampSlop(vi, thisPtr, hpe1564_SLOP_NEG);
      break;

    }
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = hpe1564_regTrigPmt(vi, thisPtr);
  }				/* end if */

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_sample_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This routine queries the sample source, timer period, sample
 *           count, and pretrigger count all in a single call.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViPInt32 numSamples
 * OUT       
 *            The total number of readings the instrument will take during
 *            a measurement is returned here.  A value of -1 is returned here
 *           if continuous measurement is enabled (see hpe1564_initCont);
 *           this indicates an infinite total reading count.
 * 
 * PARAM 3 : ViPInt32 numPreTriggers
 * OUT       
 *            Returns how many readings will be taken before the trigger
 *           event may be accepted.
 * 
 * PARAM 4 : ViPInt16 source
 * OUT       
 *            This returns an integer representing the present source that
 *            the sample system will use to cause a reading to be taken.
 *           
 *            The types are defined as macros and are shown below:
 * 
 *      Macro Name                 Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_SAMP_HOLD              0  Wait for Software sample
 *      hpe1564_SAMP_TIM               1  Timer
 *      hpe1564_SAMP_TTLT0             2  TTLTrg0
 *      hpe1564_SAMP_TTLT1             3  TTLTrg1
 *      hpe1564_SAMP_TTLT2             4  TTLTrg2
 *      hpe1564_SAMP_TTLT3             5  TTLTrg3
 *      hpe1564_SAMP_TTLT4             6  TTLTrg4
 *      hpe1564_SAMP_TTLT5             7  TTLTrg5
 *      hpe1564_SAMP_TTLT6             8  TTLTrg6
 *      hpe1564_SAMP_TTLT7             9  TTLTrg7
 *      hpe1564_SAMP_EXT_RISE         10  External Rising Edge
 *      hpe1564_SAMP_EXT_FALL         11  External Falling Edge
 * 
 * PARAM 5 : ViPReal64 period
 * OUT       
 *            This returns a value indicating the time period between each
 *           sampling event when source is hpe1564_SAMP_TIM.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1564_sample_Q_source_a[] = { "HOLD", "TIM",
  "TTLT0", "TTLT1", "TTLT2", "TTLT3", "TTLT4", "TTLT5", "TTLT6", "TTLT7",
  "EXT", "EXT", 0
};
static const char *const hpe1564_sample_Q_slope_a[] = { "NEG", "POS", 0 };

ViStatus _VI_FUNC hpe1564_sample_Q(ViSession vi,
				   ViPInt32 numSamples,
				   ViPInt32 numPreTriggers, ViPInt16 source, ViPReal64 period)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;
  char source_str[10];
  char slope_str[10];
  ViBoolean slope;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_sample_Q");

  if (thisPtr->e1406) {

    ViReal64 bigSamp;

    errStatus = viPrintf(vi, "SAMP:SOUR?\n");
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viScanf(vi, "%s%*t", source_str);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viPrintf(vi, "SAMP:TIM? \n");
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viScanf(vi, "%lg%*t", period);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = hpe1564_findIndex(thisPtr, hpe1564_sample_Q_source_a, source_str, source);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* if source was EXT, we need to know slope */
    if (*source > hpe1564_SAMP_TTLT7) {
      errStatus = viPrintf(vi, "SAMP:SLOP?\n");
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = viScanf(vi, "%s%*t", slope_str);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = hpe1564_findIndex(thisPtr, hpe1564_sample_Q_slope_a, slope_str, &slope);

      /* If we have negative slope, we need to change the
         source to EXT_FALL.  This is
         done by adding 1 to the current source value.
       */
      if (slope == hpe1564_SLOP_NEG)
	*source = *source + 1;
    }

    errStatus = viQueryf(vi, "SAMP:COUN?\n", "%lg%*t", &bigSamp);
    if (errStatus < VI_SUCCESS) {
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    }

    errStatus = viQueryf(vi, "SAMP:PRET:COUN?\n", "%ld%*t", numPreTriggers);
    if (errStatus < VI_SUCCESS) {
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
    }

    /* If CONTinuous init, D-SCPI will report back 9.9E37 for count,
       which is infinite.  We will report back -1 here to caller.
     */
    if (bigSamp < 2.4E9) {	/* The instrument could be set to 9.9E37 */
      bigSamp = bigSamp;
      *numSamples = (ViInt32) (bigSamp + 0.1);
    } else
      *numSamples = -1;		/* Indicate INF triggers */
  } else {
    errStatus = hpe1564_regSampSour_Q(vi, thisPtr, source);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = hpe1564_regSampTim_Q(vi, thisPtr, period);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* now get the counts */
    errStatus = hpe1564_regSampCount_Q(vi, thisPtr, numSamples);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = hpe1564_regPreSampCount_Q(vi, thisPtr, numPreTriggers);
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_status_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function returns the contents of the instrument's status
 *            register.  A high value in a bit location indicates a
 *           particular event has occurred.  The bit positions and their
 *           meanings are as follows:
 *           
 *              Bit Position   Event Represented When Bit is High
 *           
 *                  0           Channel 1 limit was exceeded or
 *                              channel 1 trigger level was exceeded.
 *           
 *                  1           Channel 2 limit was exceeded or
 *                              channel 2 trigger level was exceeded.
 *           
 *                  2           Channel 3 limit was exceeded or
 *                              channel 3 trigger level was exceeded.
 *           
 *                  3           Channel 4 limit was exceeded or
 *                              channel 4 trigger level was exceeded.
 *           
 *                  4           The digitizer has a taken at least one
 *                              reading and has data available.
 *           
 *                  5           A block of data is available from the
 *                              instrument.  This is 256 bytes of data,
 *                              which is 32 readings per channel.
 *           
 *                  6           The measurement has completed normally,
 *                              or available memory has been filled and
 *                              the measurement was halted.
 *           
 *                  7           A valid trigger event was received after
 *                              the pretrigger acquisition (if any) was
 *                              completed.
 *           
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViPInt16 status
 * OUT       
 *            Returns a bit pattern representing the status of
 *            several events on the instrument.  See the description
 *            section below for the meaning of each status bit.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_status_Q(ViSession vi, ViPInt16 status)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_status_Q");

  if (thisPtr->e1406) {
    errStatus = viPrintf(vi, "DIAG:STAT?\n");
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viScanf(vi, "%hd%*t", status);
  } else
    errStatus = hpe1564_regDiagStat_Q(vi, thisPtr, status);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_timeBase
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function is used to specify the timebase.  The default
 *           timebase is the INTernal timebase which uses the VXI CLK10, 10
 *           MHz reference. The "source" parameter specifies either INTernal
 *           (10 MHz) or EXTernal timebase source.  The "freq" parameter
 *           specifies the frequency of the timebase.  The timebase reference
 *           is used when the sample source is TIMer.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 source
 * IN        
 *            This parameter specifies which timebase will be used as the
 *           reference for the sample period.  There are only two choices,
 *           INTernal and EXTernal.
 * 
 *      Macro Name                Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_TIMEBASE_INT          0  Internal
 *      hpe1564_TIMEBASE_EXT          1  External
 * 
 * PARAM 3 : ViReal64 freq
 * IN        
 *            This parameter specifies the frequency of the timebase.  If the
 *           source is EXTernal, this parameter must specify the frequency of
 *           that external timebase.  This value is used to calculate sample
 *           periods when the sample source is set to TIMer.  If the source
 *           is INTernal, the frequency is already known, and the value
 *           passed in here is checked for allowable range, but is not used.
 * 
 *      MAX = hpe1564_FREQ_MAX   30.001e6
 *      MIN = hpe1564_FREQ_MIN   9900
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_timeBase(ViSession vi, ViInt16 source, ViReal64 freq)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_timeBase");

  hpe1564_CHK_ENUM(source, 1, VI_ERROR_PARAMETER2);
  hpe1564_CHK_REAL_RANGE(freq, hpe1564_FREQ_MIN, hpe1564_FREQ_MAX, VI_ERROR_PARAMETER3);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406) {
    if (source == hpe1564_TIMEBASE_EXT)
      errStatus = viPrintf(vi, ":ROSC:SOUR EXT;EXT:FREQ %lg\n", freq);
    else
      errStatus = viPrintf(vi, ":ROSC:SOUR INT;EXT:FREQ 10.0e6\n");
  } else {
    /* no errors are returned from the next two calls */
    errStatus = hpe1564_regRoscFreq(vi, thisPtr, freq);
    errStatus = hpe1564_regRoscSour(vi, thisPtr, source);
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_timeBase_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function returns the timebase source and frequency.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViPReal64 source
 * OUT       
 *            The timebase source is returned in this parameter.  This will
 *           be either 0 (hpe1564_TIMEBASE_INT) or 1 (hpe1564_TIMEBASE_EXT).
 * 
 * PARAM 3 : ViPReal64 freq
 * OUT       
 *            The frequency of the timebase is returned in this parameter.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1564_timeBase_source_a[] = { "INT", "EXT", 0 };

ViStatus _VI_FUNC hpe1564_timeBase_Q(ViSession vi, ViPInt16 source, ViPReal64 freq)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;
  char source_str[10];

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_timeBase_Q");

  if (thisPtr->e1406) {
    errStatus = viQueryf(vi, ":ROSC:SOUR? %s%*t\n", source_str);
    if (VI_SUCCESS > errStatus)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = hpe1564_findIndex(thisPtr, hpe1564_timeBase_source_a, source_str, source);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viQueryf(vi, ":ROSC:EXT:FREQ?\n", "%lg%*t", freq);
  } else {
    /* no errors are returned from the next two calls */
    errStatus = hpe1564_regRoscFreq_Q(vi, thisPtr, freq);
    errStatus = hpe1564_regRoscSour_Q(vi, thisPtr, source);
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_trigEvent
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function combines two instrument SCPI commands:
 *              TRIGger[:STARt]:SOURce<n> source;
 *              TRIGger[:STARt]:SLOPe<n> edge;
 *            Together, these two commands allow you to configure the trigger
 *            subsystem to respond to a specific source and edge.
 *           
 *           The available sources are:
 *              Bus: The Group Execute Trigger (GET) HP-IB command.
 *           
 *              TTLTrg0 through TTLTrg7: The VXIbus TTL trigger lines.  The
 *                   trigger occurs when the specified line goes low.  The
 *                   line must be held low a minimum of 30e-9 seconds.
 *           
 *              External 1: The "Ext 1" pin of the HP E1564's 9 pin
 *                   D-subminiature connector.  The rising or falling
 *                   edge can be specified as the trigger event.
 *                   The pulse width must be at least 30e-9 seconds.
 *           
 *              Level 1: The HP E1564's channel 1 signal.  Rising or
 *                   falling through the level can be specified as the
 *                   trigger event.
 *           
 *              Level 2: The HP E1564's channel 2 signal.  Rising or
 *                   falling through the level can be specified as the
 *                   trigger event.
 *           
 *              Level 3: The HP E1564's channel 3 signal.  Rising or
 *                   falling through the level can be specified as the
 *                   trigger event.
 *           
 *              Level 4: The HP E1564's channel 4 signal.  Rising or
 *                   falling through the level can be specified as the
 *                   trigger event.
 *           
 *              Hold: Disable this trigger event.  If both events are set
 *                    to Hold, the hpe1564_trigImm() function must be
 *                    sent before measurements will proceed.
 *           
 *              Immediate: A trigger will occur as soon as hpe1564_initImm()
 *                         is sent.  This choice is valid only for event 1,
 *                         and requires that event 2 be set to HOLD.  This is
 *                         also the reset state.
 *           
 *           
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 event
 * IN        
 *            Specifies the event number (1 or 2) to modify.  Note
 *            that the event number is NOT the same as the channel number.
 *            The numbers associated with sources Internal 1 through Internal
 *           4 are the channel numbers.
 * 
 *      MAX = hpe1564_TRIG_EVENT_MAX   2
 *      MIN = hpe1564_TRIG_EVENT_MIN   1
 * 
 * PARAM 3 : ViInt16 source
 * IN        
 *            This parameter configures the trigger system to respond to the
 *            specified source.  Note that some sources have a rising or
 *           falling edge component to them.
 *           
 *            The types are defined as macros and are shown below:
 * 
 *      Macro Name                    Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_TRIG_BUS                  0  Bus
 *      hpe1564_TRIG_EXT_FALL            1  External 1 Falling Edge
 *      hpe1564_TRIG_EXT_RISE            2  External 1 Rising Edge
 *      hpe1564_TRIG_HOLD                 3  Hold
 *      hpe1564_TRIG_IMM                  4  Immediate
 *      hpe1564_TRIG_LEVEL1_FALL          5  Falling below chan 1 level
 *      hpe1564_TRIG_LEVEL1_RISE          6  Rising above chan 1 level
 *      hpe1564_TRIG_LEVEL2_FALL          7  Falling below chan 2 level
 *      hpe1564_TRIG_LEVEL2_RISE          8  Rising above chan 2 level
 *      hpe1564_TRIG_LEVEL3_FALL          9  Falling below chan 3 level
 *      hpe1564_TRIG_LEVEL3_RISE         10  Rising above chan 3 level
 *      hpe1564_TRIG_LEVEL4_FALL         11  Falling below chan 4 level
 *      hpe1564_TRIG_LEVEL4_RISE         12  Rising above chan 4 level
 *      hpe1564_TRIG_TTLT0               13  TTLTrg0
 *      hpe1564_TRIG_TTLT1               14  TTLTrg1
 *      hpe1564_TRIG_TTLT2               15  TTLTrg2
 *      hpe1564_TRIG_TTLT3               16  TTLTrg3
 *      hpe1564_TRIG_TTLT4               17  TTLTrg4
 *      hpe1564_TRIG_TTLT5               18  TTLTrg5
 *      hpe1564_TRIG_TTLT6               19  TTLTrg6
 *      hpe1564_TRIG_TTLT7               20  TTLTrg7
 * 
 *  PARAM 4: ViReal64 level 
 *  OUT
 *	Specifies the signal level which will cause a trigger to occur if the
 *	trigger source is the level on a channel.  If the trigger source is 
 *      not level, then this parameter is ignored.
 *
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1564_trigEvent_source_a[] = { "BUS", "EXT",
  "EXT", "HOLD", "IMM", "INT1", "INT1", "INT2", "INT2",
  "INT3", "INT3", "INT4", "INT4", "TTLT0", "TTLT1", "TTLT2", "TTLT3",
  "TTLT4", "TTLT5", "TTLT6", "TTLT7", 0
};

ViStatus _VI_FUNC hpe1564_trigEvent(ViSession vi, ViInt16 event, ViInt16 source, ViPReal64 level)
{

  ViStatus errStatus = 0;
  ViStatus ignore = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_trigEvent");

  hpe1564_CHK_INT_RANGE(event, hpe1564_TRIG_EVENT_MIN, hpe1564_TRIG_EVENT_MAX, VI_ERROR_PARAMETER2);

  hpe1564_CHK_ENUM(source, 20, VI_ERROR_PARAMETER3);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406) {
    switch (source) {
    case hpe1564_TRIG_LEVEL1_FALL:
    case hpe1564_TRIG_LEVEL2_FALL:
    case hpe1564_TRIG_LEVEL3_FALL:
    case hpe1564_TRIG_LEVEL4_FALL:
      /* param 4 is now active, so check it */
      hpe1564_CHK_REAL_RANGE(*level, hpe1564_LEVEL_MIN, hpe1564_LEVEL_MAX, VI_ERROR_PARAMETER4);

      /* NOTE: We are not sending linefeed yet. */
      errStatus = viPrintf(vi, "TRIG:SLOP%hd NEG;SOUR%hd %s;",
			   event, event, hpe1564_trigEvent_source_a[source]);

      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      /* we send the final line feed with the level setting */
      if (source == hpe1564_TRIG_LEVEL1_FALL)
	errStatus = viPrintf(vi, "LEV1 %lg\n", *level);
      else if (source == hpe1564_TRIG_LEVEL2_FALL)
	errStatus = viPrintf(vi, "LEV2 %lg\n", *level);
      else if (source == hpe1564_TRIG_LEVEL3_FALL)
	errStatus = viPrintf(vi, "LEV3 %lg\n", *level);
      else
	errStatus = viPrintf(vi, "LEV4 %lg\n", *level);

      break;

    case hpe1564_TRIG_EXT_FALL:
      errStatus = viPrintf(vi, "TRIG:SLOP%hd NEG;SOUR%hd %s\n",
			   event, event, hpe1564_trigEvent_source_a[source]);
      break;

    case hpe1564_TRIG_LEVEL1_RISE:
    case hpe1564_TRIG_LEVEL2_RISE:
    case hpe1564_TRIG_LEVEL3_RISE:
    case hpe1564_TRIG_LEVEL4_RISE:
      /* param 4 is now active, so check it */
      hpe1564_CHK_REAL_RANGE(*level, hpe1564_LEVEL_MIN, hpe1564_LEVEL_MAX, VI_ERROR_PARAMETER4);

      /* NOTE: We are not sending linefeed yet. */
      errStatus = viPrintf(vi, "TRIG:SLOP%hd POS;SOUR%hd %s;",
			   event, event, hpe1564_trigEvent_source_a[source]);

      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      /* we send the final line feed with the level setting */
      if (source == hpe1564_TRIG_LEVEL1_RISE)
	errStatus = viPrintf(vi, "LEV1 %lg\n", *level);
      else if (source == hpe1564_TRIG_LEVEL2_RISE)
	errStatus = viPrintf(vi, "LEV2 %lg\n", *level);
      else if (source == hpe1564_TRIG_LEVEL3_RISE)
	errStatus = viPrintf(vi, "LEV3 %lg\n", *level);
      else
	errStatus = viPrintf(vi, "LEV4 %lg\n", *level);

      break;

    case hpe1564_TRIG_EXT_RISE:
      errStatus = viPrintf(vi, "TRIG:SLOP%hd POS;SOUR%hd %s\n",
			   event, event, hpe1564_trigEvent_source_a[source]);
      break;

      /* BUS, HOLD, IMM, or TTLTn */
    default:
      errStatus = viPrintf(vi, "TRIG:SOUR%hd %s\n", event, hpe1564_trigEvent_source_a[source]);
    }				/* end switch */
  } else
    /* make sure download enum numbers match source int here */
  {
    switch (source) {
    case hpe1564_TRIG_LEVEL1_FALL:
    case hpe1564_TRIG_LEVEL2_FALL:
    case hpe1564_TRIG_LEVEL3_FALL:
    case hpe1564_TRIG_LEVEL4_FALL:
      /* param 4 is now active, so check it */
      hpe1564_CHK_REAL_RANGE(*level, hpe1564_LEVEL_MIN, hpe1564_LEVEL_MAX, VI_ERROR_PARAMETER4);

      errStatus = hpe1564_regTrigSlop(vi, thisPtr, event, hpe1564_SLOP_NEG);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      /* if we get error here, we want to keep going and report later */
      ignore = hpe1564_regTrigSour(vi, thisPtr, event, source);

      /* set the level */
      if (source == hpe1564_TRIG_LEVEL1_FALL)
	errStatus = hpe1564_regTrigLev(vi, thisPtr, 1, *level);
      else if (source == hpe1564_TRIG_LEVEL2_FALL)
	errStatus = hpe1564_regTrigLev(vi, thisPtr, 2, *level);
      else if (source == hpe1564_TRIG_LEVEL3_FALL)
	errStatus = hpe1564_regTrigLev(vi, thisPtr, 3, *level);
      else
	errStatus = hpe1564_regTrigLev(vi, thisPtr, 4, *level);

      break;

    case hpe1564_TRIG_EXT_FALL:
      errStatus = hpe1564_regTrigSlop(vi, thisPtr, event, hpe1564_SLOP_NEG);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = hpe1564_regTrigSour(vi, thisPtr, event, source);
      break;

    case hpe1564_TRIG_LEVEL1_RISE:
    case hpe1564_TRIG_LEVEL2_RISE:
    case hpe1564_TRIG_LEVEL3_RISE:
    case hpe1564_TRIG_LEVEL4_RISE:
      /* param 4 is now active, so check it */
      hpe1564_CHK_REAL_RANGE(*level, hpe1564_LEVEL_MIN, hpe1564_LEVEL_MAX, VI_ERROR_PARAMETER4);

      errStatus = hpe1564_regTrigSlop(vi, thisPtr, event, hpe1564_SLOP_POS);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      /* if we get error here, we want to keep going and report later */
      ignore = hpe1564_regTrigSour(vi, thisPtr, event, source);

      /* set the level */
      if (source == hpe1564_TRIG_LEVEL1_RISE)
	errStatus = hpe1564_regTrigLev(vi, thisPtr, 1, *level);
      else if (source == hpe1564_TRIG_LEVEL2_RISE)
	errStatus = hpe1564_regTrigLev(vi, thisPtr, 2, *level);
      else if (source == hpe1564_TRIG_LEVEL3_RISE)
	errStatus = hpe1564_regTrigLev(vi, thisPtr, 3, *level);
      else
	errStatus = hpe1564_regTrigLev(vi, thisPtr, 4, *level);

      break;

    case hpe1564_TRIG_EXT_RISE:
      errStatus = hpe1564_regTrigSlop(vi, thisPtr, event, hpe1564_SLOP_POS);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = hpe1564_regTrigSour(vi, thisPtr, event, source);
      break;

      /* BUS, HOLD, IMM, or TTLTn require no slope setting */
    default:
      errStatus = hpe1564_regTrigSour(vi, thisPtr, event, source);

    }				/* end switch */

    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* call pmt routine to resolve couplings */
    errStatus = hpe1564_regTrigPmt(vi, thisPtr);

    /* if we got an error setting trigger source and no errors after that,
       then report the settings conflict we got from the trigger source.
     */
    if (ignore < VI_SUCCESS && errStatus >= VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_trigEvent_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This routine queries the source and slope for the specified
 *            trigger event in a single call.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 event
 * IN        
 *            Specifies the event number (1 or 2) being specified.  Note
 *            that the event number is NOT the same as the channel number.
 *            The numbers associated with sources Level 1 through Level 4 are
 *            the channel numbers.
 * 
 *      MAX = hpe1564_TRIG_EVENT_MAX   2
 *      MIN = hpe1564_TRIG_EVENT_MIN   1
 * 
 * PARAM 3 : ViPInt16 source
 * OUT       
 *            This returns an integer value indicating the source
 *            to which the trigger will respond.
 *           
 *            The types are defined as macros and are shown below:
 * 
 *      Macro Name                    Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_TRIG_BUS                  0  Bus
 *      hpe1564_TRIG_EXT_FALL            1  External 1 Falling Edge
 *      hpe1564_TRIG_EXT_RISE            2  External 1 Rising Edge
 *      hpe1564_TRIG_HOLD                 3  Hold
 *      hpe1564_TRIG_IMM                  4  Immediate
 *      hpe1564_TRIG_LEVEL1_FALL          5  Falling below chan 1 level
 *      hpe1564_TRIG_LEVEL1_RISE          6  Rising above chan 1 level
 *      hpe1564_TRIG_LEVEL2_FALL          7  Falling below chan 2 level
 *      hpe1564_TRIG_LEVEL2_RISE          8  Rising above chan 2 level
 *      hpe1564_TRIG_LEVEL3_FALL          9  Falling below chan 3 level
 *      hpe1564_TRIG_LEVEL3_RISE         10  Rising above chan 3 level
 *      hpe1564_TRIG_LEVEL4_FALL         11  Falling below chan 4 level
 *      hpe1564_TRIG_LEVEL4_RISE         12  Rising above chan 4 level
 *      hpe1564_TRIG_TTLT0               13  TTLTrg0
 *      hpe1564_TRIG_TTLT1               14  TTLTrg1
 *      hpe1564_TRIG_TTLT2               15  TTLTrg2
 *      hpe1564_TRIG_TTLT3               16  TTLTrg3
 *      hpe1564_TRIG_TTLT4               17  TTLTrg4
 *      hpe1564_TRIG_TTLT5               18  TTLTrg5
 *      hpe1564_TRIG_TTLT6               19  TTLTrg6
 *      hpe1564_TRIG_TTLT7               20  TTLTrg7
 * 
 *  PARAM 4: ViPReal64 level 
 *  OUT
 *	Contains the present level setting on the channel being monitored if
 *	the trigger source is one of the LEVEL<n> choices; otherwise, this
 *      parameter is set to 0.0 if level triggering not enabled.
 *
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */

/* note that EXT may be returned instead of EXT1 */
static const char *const hpe1564_trigEvent_Q_source_a[] = { "BUS", "EXT",
  "EXT", "HOLD", "IMM", "INT", "INT1", "INT2", "INT2",
  "INT3", "INT3", "INT4", "INT4", "TTLT0", "TTLT1", "TTLT2", "TTLT3",
  "TTLT4", "TTLT5", "TTLT6", "TTLT7", 0
};
static const char *const hpe1564_trigEvent_Q_slope_a[] = { "NEG", "POS", 0 };

ViStatus _VI_FUNC hpe1564_trigEvent_Q(ViSession vi, ViInt16 event, ViPInt16 source, ViPReal64 level)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;
  char source_str[10];
  char slope_str[10];
  ViInt16 slope;
  ViBoolean tslope;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_trigEvent_Q");

  hpe1564_CHK_INT_RANGE(event, hpe1564_TRIG_EVENT_MIN, hpe1564_TRIG_EVENT_MAX, VI_ERROR_PARAMETER2);

  if (thisPtr->e1406) {
    errStatus = viPrintf(vi, "TRIG:SOUR%hd?\n", event);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viScanf(vi, "%s%*t", source_str);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = hpe1564_findIndex(thisPtr, hpe1564_trigEvent_Q_source_a, source_str, source);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* return 0.0 for level if not active source */
    *level = 0.0;

    switch (*source) {
    case hpe1564_TRIG_LEVEL1_FALL:
    case hpe1564_TRIG_LEVEL2_FALL:
    case hpe1564_TRIG_LEVEL3_FALL:
    case hpe1564_TRIG_LEVEL4_FALL:
      /* query the slope setting */
      errStatus = viPrintf(vi, "TRIG:SLOP%hd?\n", event);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = viScanf(vi, "%s%*t", slope_str);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = hpe1564_findIndex(thisPtr, hpe1564_trigEvent_Q_slope_a, slope_str, &slope);

      /* query the level setting */
      if (*source == hpe1564_TRIG_LEVEL1_FALL)
	errStatus = viPrintf(vi, "TRIG:LEV1?\n");
      else if (*source == hpe1564_TRIG_LEVEL2_FALL)
	errStatus = viPrintf(vi, "TRIG:LEV2?\n");
      else if (*source == hpe1564_TRIG_LEVEL3_FALL)
	errStatus = viPrintf(vi, "TRIG:LEV3?\n");
      else if (*source == hpe1564_TRIG_LEVEL4_FALL)
	errStatus = viPrintf(vi, "TRIG:LEV4?\n");

      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = viScanf(vi, "%lg%*t", level);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      /* If slope was positive, we were really _RISE and not _FALL,
         so indicate that in the *source param.
       */
      if (slope == hpe1564_SLOP_POS)
	*source = *source + 1;

      break;

    case hpe1564_TRIG_EXT_FALL:
      /* query the slope setting */
      errStatus = viPrintf(vi, "TRIG:SLOP%hd?\n", event);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = viScanf(vi, "%s%*t", slope_str);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = hpe1564_findIndex(thisPtr, hpe1564_trigEvent_Q_slope_a, slope_str, &slope);

      if (slope == hpe1564_SLOP_POS)
	*source = *source + 1;
      break;

      /* we should never take this next case, but if we do, handle it */
    case hpe1564_TRIG_LEVEL1_RISE:
    case hpe1564_TRIG_LEVEL2_RISE:
    case hpe1564_TRIG_LEVEL3_RISE:
    case hpe1564_TRIG_LEVEL4_RISE:
      /* query the slope setting */
      errStatus = viPrintf(vi, "TRIG:SLOP%hd?\n", event);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = viScanf(vi, "%s%*t", slope_str);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = hpe1564_findIndex(thisPtr, hpe1564_trigEvent_Q_slope_a, slope_str, &slope);

      /* query the level setting */
      if (*source == hpe1564_TRIG_LEVEL1_RISE)
	errStatus = viPrintf(vi, "TRIG:LEV1?\n");
      else if (*source == hpe1564_TRIG_LEVEL2_RISE)
	errStatus = viPrintf(vi, "TRIG:LEV2?\n");
      else if (*source == hpe1564_TRIG_LEVEL3_RISE)
	errStatus = viPrintf(vi, "TRIG:LEV3?\n");
      else if (*source == hpe1564_TRIG_LEVEL4_RISE)
	errStatus = viPrintf(vi, "TRIG:LEV4?\n");

      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = viScanf(vi, "%lg%*t", level);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      /* If slope was negative, we were really _FALL and not _RISE,
         so indicate that in the *source param.
       */
      if (slope == hpe1564_SLOP_NEG)
	*source = *source - 1;

      break;

    case hpe1564_TRIG_EXT_RISE:
      /* query the slope setting */
      errStatus = viPrintf(vi, "TRIG:SLOP%hd?\n", event);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = viScanf(vi, "%s%*t", slope_str);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      errStatus = hpe1564_findIndex(thisPtr, hpe1564_trigEvent_Q_slope_a, slope_str, &slope);

      if (slope == hpe1564_SLOP_NEG)
	*source = *source - 1;
      break;

    }

  } else {
    errStatus = hpe1564_regTrigSour_Q(vi, thisPtr, event, source);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    switch (*source) {
    case hpe1564_TRIG_LEVEL1_FALL:
    case hpe1564_TRIG_LEVEL2_FALL:
    case hpe1564_TRIG_LEVEL3_FALL:
    case hpe1564_TRIG_LEVEL4_FALL:
    case hpe1564_TRIG_EXT_FALL:
      errStatus = hpe1564_regTrigSlop_Q(vi, thisPtr, event, &tslope);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      slope = tslope ? 1 : 0;

      /* query the level */
      if (*source == hpe1564_TRIG_LEVEL1_FALL)
	errStatus = hpe1564_regTrigLev_Q(vi, thisPtr, 1, level);
      else if (*source == hpe1564_TRIG_LEVEL2_FALL)
	errStatus = hpe1564_regTrigLev_Q(vi, thisPtr, 2, level);
      else if (*source == hpe1564_TRIG_LEVEL3_FALL)
	errStatus = hpe1564_regTrigLev_Q(vi, thisPtr, 3, level);
      else if (*source == hpe1564_TRIG_LEVEL4_FALL)
	errStatus = hpe1564_regTrigLev_Q(vi, thisPtr, 4, level);

      if (slope == hpe1564_SLOP_POS)
	*source = *source + 1;

      break;

    case hpe1564_TRIG_LEVEL1_RISE:
    case hpe1564_TRIG_LEVEL2_RISE:
    case hpe1564_TRIG_LEVEL3_RISE:
    case hpe1564_TRIG_LEVEL4_RISE:
    case hpe1564_TRIG_EXT_RISE:
      errStatus = hpe1564_regTrigSlop_Q(vi, thisPtr, event, &tslope);
      if (errStatus < VI_SUCCESS)
	hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

      slope = tslope ? 1 : 0;

      /* query the level */
      if (*source == hpe1564_TRIG_LEVEL1_RISE)
	errStatus = hpe1564_regTrigLev_Q(vi, thisPtr, 1, level);
      else if (*source == hpe1564_TRIG_LEVEL2_RISE)
	errStatus = hpe1564_regTrigLev_Q(vi, thisPtr, 2, level);
      else if (*source == hpe1564_TRIG_LEVEL3_RISE)
	errStatus = hpe1564_regTrigLev_Q(vi, thisPtr, 3, level);
      else if (*source == hpe1564_TRIG_LEVEL4_RISE)
	errStatus = hpe1564_regTrigLev_Q(vi, thisPtr, 4, level);

      if (slope == hpe1564_SLOP_NEG)
	*source = *source - 1;
      break;

    }

  }				/* end else */

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_trigImm
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function will cause the instrument to transition to the
 *            wait-for-sample state immediately, regardless of the selected
 *            trigger sources.  The instrument must be initiated and in the
 *            wait-for-trigger state when this function is called; otherwise,
 *           a "Trigger ignored" error will result.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_trigImm(ViSession vi)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_trigImm");

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr) == 0)
    return hpe1564_ERROR_TRIG_IGN;

  if (thisPtr->e1406)
    errStatus = viPrintf(vi, "TRIG\n");
  else
    errStatus = hpe1564_regTrigImm(vi, thisPtr);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_trigLevel
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function sets the level on a channel that may be used for
 *            triggering the instrument.  For the level to be used, one of
 *           the trigger events must be set to
 *           "hpe1564_TRIG_LEVEL<chan>_RISE" or
 *           "hpe1564_TRIG_LEVEL<chan>_FALL".
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 channel
 * IN        
 *            Specifies the channel to set the level on.
 *           
 *            The channels are defined as macros and are shown below:
 * 
 *      Macro Name         Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_CHAN1          1  1
 *      hpe1564_CHAN2          2  2
 *      hpe1564_CHAN3          3  3
 *      hpe1564_CHAN4          4  4
 * 
 * PARAM 3 : ViReal64 level
 * IN        
 *            The desired level for the specified channel.  Note that the
 *            present range setting will determine the maximum and minimum
 *            values that may be entered without error.
 * 
 *      MAX = hpe1564_LEVEL_MAX   255.984375
 *      MIN = hpe1564_LEVEL_MIN   -255.9765625
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_trigLevel(ViSession vi, ViInt16 chan, ViReal64 level)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_trigLevel");

  hpe1564_CHK_INT_RANGE(chan, 1, 4, VI_ERROR_PARAMETER2);
  hpe1564_CHK_REAL_RANGE(level, hpe1564_LEVEL_MIN, hpe1564_LEVEL_MAX, VI_ERROR_PARAMETER3);

  /* error if initiated and not e1406 path */
  if (thisPtr->e1406 == 0 && hpe1564_regIsInitiated(vi, thisPtr))
    return hpe1564_ERROR_INIT_IGN;

  if (thisPtr->e1406)
    errStatus = viPrintf(vi, "TRIG:LEV%hd %lg\n", chan, level);
  else
    errStatus = hpe1564_regTrigLev(vi, thisPtr, chan, level);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_trigLevel_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function returns the value of the trigger level for the
 *           specified channel.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 channel
 * IN        
 *            Specifies the channel for which the level is being queried.
 *           
 *            The channels are defined as macros and are shown below:
 * 
 *      Macro Name         Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_CHAN1          1  1
 *      hpe1564_CHAN2          2  2
 *      hpe1564_CHAN3          3  3
 *      hpe1564_CHAN4          4  4
 * 
 * PARAM 3 : ViPReal64 level
 * OUT       
 *            Returns the present level setting.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC hpe1564_trigLevel_Q(ViSession vi, ViInt16 chan, ViPReal64 level)
{

  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (errStatus < VI_SUCCESS) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_trigLevel_Q");

  hpe1564_CHK_INT_RANGE(chan, 1, 4, VI_ERROR_PARAMETER2);
  if (thisPtr->e1406) {
    errStatus = viPrintf(vi, "TRIG:LEV%hd?\n", chan);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = viScanf(vi, "%lg%*t", level);
  } else
    errStatus = hpe1564_regTrigLev_Q(vi, thisPtr, chan, level);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_trigMode
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function is ONLY used when it is desired to synchronize
 *           multiple digitizers to sample on the same event.  A pair of TTL
 *           trigger lines is used to accomplish this; the <n> in the
 *           MASTER<n> or SLAVE<n> designation specifies that lines <n> and
 *           <n+1> will be the pair used for the measurement.
 *            The MASTER<n> setting specifies the digitizer that will control
 *           the sampling; there must be only 1 master.  The MASTER<n>
 *           digitizer, when triggered, will echo its trigger pulse out onto
 *           TTL trigger line <n+1>, and with each sample event, will pulse
 *           TTL trigger line <n>, which becomes the sample event for all of
 *           the slave digitizers.
 *            The MASTER and SLAVE modes have a few couplings (side effects)
 *           to be aware of:
 *              Trigger Source:  Trigger source 1 is set to the appropriate
 *                               TTL trigger line.  Trigger source 2 may be
 *                               set to anything.
 *           
 *              Outputs:  Outputs to the TTL trigger lines (see the
 *                        hpe1564_outpTtltStat function) are disabled.  The
 *                        only outputs that occur are the ones dictated by
 *                        the Master/Slave relationship.
 *           
 *              Sample Source:  The Master unit can have any sample source,
 *                              EXCEPT the two TTL trigger lines used in the
 *                              Master/Slave relationship.  The Slave's
 *                              sample source is the TTL trigger line
 *                              specified by the Master/Slave relationship.
 *                              Attempting to change the sample source on a
 *                              Slave unit will result in an error.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViInt16 mode
 * IN        
 *            Specifies the trigger mode.  See the "Description" section for
 *            explanation of each mode.  The modes are defined as macros and
 *            are shown below:
 *           
 * 
 *      Macro Name                     Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_TRIG_MODE_NORMAL           0  Normal triggering
 *      hpe1564_TRIG_MODE_MASTER0          1  Master, TTLT0 and TTLT1
 *      hpe1564_TRIG_MODE_MASTER2          2  Master, TTLT2 and TTLT3
 *      hpe1564_TRIG_MODE_MASTER4          3  Master, TTLT4 and TTLT5
 *      hpe1564_TRIG_MODE_MASTER6          4  Master, TTLT6 and TTLT7
 *      hpe1564_TRIG_MODE_SLAVE0           5  Master, TTLT0 and TTLT1
 *      hpe1564_TRIG_MODE_SLAVE2           6  Master, TTLT2 and TTLT3
 *      hpe1564_TRIG_MODE_SLAVE4           7  Master, TTLT4 and TTLT5
 *      hpe1564_TRIG_MODE_SLAVE6           8  Master, TTLT6 and TTLT7
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1564_trigMode_mode_a[] = { "NORMAL", "MASTER0",
  "MASTER2", "MASTER4", "MASTER6", "SLAVE0", "SLAVE2", "SLAVE4",
  "SLAVE6", 0
};

ViStatus _VI_FUNC hpe1564_trigMode(ViSession vi, ViInt16 mode)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (VI_SUCCESS > errStatus) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);

  hpe1564_CDE_INIT("hpe1564_trigMode");

  /* Perform Error Checking on Each Parameter */
  hpe1564_CHK_ENUM(mode, 8, VI_ERROR_PARAMETER2);
  if (thisPtr->e1406)
    errStatus = viPrintf(vi, "TRIG:MODE %s\n", hpe1564_trigMode_mode_a[mode]);
  else {
    errStatus = hpe1564_regTrigMode(vi, thisPtr, mode);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    /* call pmt routine to update hardware and resolve couplings */
    errStatus = hpe1564_regTrigPmt(vi, thisPtr);
  }

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC hpe1564_trigMode_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function returns the present setting of the trigger mode.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            The handle to the instrument.
 * 
 * PARAM 2 : ViPInt16 mode
 * OUT       
 *            The integer representing the present trigger mode setting is
 *            returned in this parameter.  The mode settings are shown in the
 *            macros below:
 *           
 * 
 *      Macro Name                     Value  Description
 *      -----------------------------------------------------------
 *      hpe1564_TRIG_MODE_NORMAL           0  Normal triggering
 *      hpe1564_TRIG_MODE_MASTER0          1  Master, TTLT0 and TTLT1
 *      hpe1564_TRIG_MODE_MASTER2          2  Master, TTLT2 and TTLT3
 *      hpe1564_TRIG_MODE_MASTER4          3  Master, TTLT4 and TTLT5
 *      hpe1564_TRIG_MODE_MASTER6          4  Master, TTLT6 and TTLT7
 *      hpe1564_TRIG_MODE_SLAVE0           5  Master, TTLT0 and TTLT1
 *      hpe1564_TRIG_MODE_SLAVE2           6  Master, TTLT2 and TTLT3
 *      hpe1564_TRIG_MODE_SLAVE4           7  Master, TTLT4 and TTLT5
 *      hpe1564_TRIG_MODE_SLAVE6           8  Master, TTLT6 and TTLT7
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "hpe1564_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
static const char *const hpe1564_trigMode_Q_mode_a[] = { "NORMAL",
  "MASTER0", "MASTER2", "MASTER4", "MASTER6", "SLAVE0", "SLAVE2",
  "SLAVE4", "SLAVE6", 0
};

ViStatus _VI_FUNC hpe1564_trigMode_Q(ViSession vi, ViPInt16 mode)
{
  ViStatus errStatus = 0;
  hpe1564_globals *thisPtr;
  char mode_str[32];

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (VI_SUCCESS > errStatus) {
    hpe1564_LOG_STATUS(vi, 0, errStatus);
  }

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);

  hpe1564_CDE_INIT("hpe1564_trigMode_Q");

  /* Perform Error Checking on Each Parameter */
  if (thisPtr->e1406) {
    errStatus = viQueryf(vi, ":TRIG:MODE?\n", "%s%*t", mode_str);
    if (errStatus < VI_SUCCESS)
      hpe1564_LOG_STATUS(vi, thisPtr, errStatus);

    errStatus = hpe1564_findIndex(thisPtr, hpe1564_trigMode_Q_mode_a, mode_str, mode);

  } else
    errStatus = hpe1564_regTrigMode_Q(vi, thisPtr, mode);

  hpe1564_LOG_STATUS(vi, thisPtr, errStatus);
}

/****************************************************************************
hpe1564_statEvenHdlr
*****************************************************************************
    Parameter Name                                       Type    Direction
   ------------------------------------------------------------------------
  | vi                                                ViSession   IN
  |   ---------------------------------------------------------------------
  |  | Instrument Handle returned from hpe1564_init()
   ------------------------------------------------------------------------
  | happening                                         ViInt32     IN
  |   ---------------------------------------------------------------------
  |  | Happenings refer to something that happens. These can refer to
  |  | conditions or events. Happenings are enumerated as ViInt32
  |  | numbers.  Only events can have happenings associated with it.
  |  |
   ------------------------------------------------------------------------
  | eventHandler                           hpe1564_InstrEventHandler  IN
  |   ---------------------------------------------------------------------
  |  | This is either NULL or a pointer to the user specified
  |  | event handler.  A NULL disables the event handler.
  |  |
  |  | An event handler has the following prototype:
  |  |
  |  | typedef void (_VI_FUNCH _VI_PTR  hpe1564_InstrEventHandler)(
  |  | ViSession vi,
  |  | ViInt32 happening,
  |  | ViAddr userData
  |  | );
   ------------------------------------------------------------------------
  | userData                                          ViAddr      IN
  |   ---------------------------------------------------------------------
  |  | This is a pointer that is passed to the handler when the
  |  | specified event occurs.  This can be used by the programmer
  |  | to pass additional information to the handler.

*****************************************************************************/
ViStatus _VI_FUNC hpe1564_statEvenHdlr(ViSession vi,
				       ViInt32 happening,
				       hpe1564_InstrEventHandler eventHandler, ViAddr userData)
{
  ViUInt16 intf;
  hpe1564_globals *thisPtr;
  ViStatus errStatus;
  ViInt32 hapIdx;
  char errMsg[80];

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);

  viGetAttribute(vi, VI_ATTR_INTF_TYPE, &intf);
  if (VI_SUCCESS > errStatus)
    hpe1564_LOG_STATUS(vi, NULL, errStatus);

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_statEvenHdlr");

  /* if we are not VI_INTF_VXI, then only one happening allowed */
  if (intf != VI_INTF_VXI) {
    /* return param 2 error if happening not USER_ERROR or INSTR_ERROR */
    if ((hpe1564_USER_ERROR_HANDLER != happening) && (hpe1564_INSTR_ERROR_HANDLER != happening)) {
      sprintf(errMsg, "%hd " hpe1564_MSG_INVALID_HAPPENING, happening);
      hpe1564_CDE_MESSAGE(errMsg);
      hpe1564_LOG_STATUS(vi, thisPtr, VI_ERROR_PARAMETER2);
    }
  }

  if (hpe1564_USER_ERROR_HANDLER == happening) {
    /* store handler and user data */
    thisPtr->eventHandlerArray[hpe1564_USER_ERROR_HANDLER_IDX].eventHandler = eventHandler;
    thisPtr->eventHandlerArray[hpe1564_USER_ERROR_HANDLER_IDX].userData = userData;
    hpe1564_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
  }

  if (hpe1564_INSTR_ERROR_HANDLER == happening) {
    /* store handler and user data */
    thisPtr->eventHandlerArray[hpe1564_INSTR_ERROR_HANDLER_IDX].eventHandler = eventHandler;
    thisPtr->eventHandlerArray[hpe1564_INSTR_ERROR_HANDLER_IDX].userData = userData;
    hpe1564_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
  }

  /* check validity of happening */
  if (VI_FALSE == hpe1564_findHappeningIdx(happening, &hapIdx)) {

    sprintf(errMsg, "%ld " hpe1564_MSG_INVALID_HAPPENING, (long)happening);
    hpe1564_CDE_MESSAGE(errMsg);

    hpe1564_LOG_STATUS(vi, thisPtr, VI_ERROR_PARAMETER2);
  }

  /* see if we want to enable or disable the handler */
  if (NULL != eventHandler) {
    /* see if event handler already exists */
    if (NULL == thisPtr->eventHandlerArray[hapIdx].eventHandler) {
      /* okay - no event handler */
      thisPtr->numEventHandlers = thisPtr->numEventHandlers + 1;
    } else {
      /* error - event handler already exists */
      sprintf(errMsg,
	      hpe1564_MSG_EVENT_HDLR_INSTALLED
	      " %ld.  " hpe1564_MSG_EVENT_HDLR_INST2, (long)happening);

      hpe1564_LOG_STATUS(vi, thisPtr, VI_ERROR_PARAMETER2);

    }

    /* store handler and user data */
    thisPtr->eventHandlerArray[hapIdx].eventHandler = eventHandler;
    thisPtr->eventHandlerArray[hapIdx].userData = userData;

  } /* if - install event handler */
  else {
    /* see if event handler already exists */
    if (NULL != thisPtr->eventHandlerArray[hapIdx].eventHandler) {
      /* We do have a handler we will be un-installing */
      thisPtr->numEventHandlers = thisPtr->numEventHandlers - 1;
    }

    /* clear handler and user data */
    thisPtr->eventHandlerArray[hapIdx].eventHandler = NULL;
    thisPtr->eventHandlerArray[hapIdx].userData = NULL;

  }				/* else - remove event handler */

  hpe1564_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}

/****************************************************************************
hpe1564_statEvenHdlr_Q
*****************************************************************************
    Parameter Name                                       Type    Direction
   ------------------------------------------------------------------------
  | vi                                                ViSession   IN
  |   ---------------------------------------------------------------------
  |  | Instrument Handle returned from hpe1564_init()
   ------------------------------------------------------------------------
  | happening                                         ViInt32 happeningIN
  |   ---------------------------------------------------------------------
  |  | Happenings refer to something that happens.  These can refer to
  |  | conditions or events.  Happenings are enumerated as ViInt32
  |  | numbers.
  |  |
   ------------------------------------------------------------------------
  | pEventHandler                     hpe1564_InstrPEventHandler      OUT
  |   ---------------------------------------------------------------------
  |  | This is the definition of a hpe1564_InstrPEventHandler:
  |  |
  |  | typedef void (_VI_PTR _VI_PTR hpe1564_InstrPEventHandler)(
  |  | ViSession vi,
  |  | ViInt32 happening,
  |  | ViAddr userData
  |  | );
   ------------------------------------------------------------------------
  | pUserData                                         ViPAddr     OUT
  |   ---------------------------------------------------------------------
  |  | This is a pointer to the userData that was registered
  |  | with the handler.

*****************************************************************************/
ViStatus _VI_FUNC hpe1564_statEvenHdlr_Q(ViSession vi,
					 ViInt32 happening,
					 hpe1564_InstrPEventHandler pEventHandler,
					 ViPAddr pUserData)
{
  ViUInt16 intf;
  hpe1564_globals *thisPtr;
  ViStatus errStatus;
  ViInt32 hapIdx;
  char errMsg[80];

  errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) & thisPtr);
  if (VI_SUCCESS > errStatus)
    hpe1564_LOG_STATUS(vi, NULL, errStatus);

  hpe1564_DEBUG_CHK_THIS(vi, thisPtr);
  hpe1564_CDE_INIT("hpe1564_statEvenHdlr_Q");

  viGetAttribute(vi, VI_ATTR_INTF_TYPE, &intf);

  /* only 2 happenings allowed if we are not VXI intf */
  if (intf != VI_INTF_VXI) {
    /* return param 2 error if happening not USER_ERROR or INSTR_ERROR */
    if (hpe1564_USER_ERROR_HANDLER != happening && hpe1564_INSTR_ERROR_HANDLER != happening) {
      sprintf(errMsg, "%hd " hpe1564_MSG_INVALID_HAPPENING, happening);
      hpe1564_CDE_MESSAGE(errMsg);
      hpe1564_LOG_STATUS(vi, thisPtr, VI_ERROR_PARAMETER2);
    }
  }

  /* check validity of happening */
  if (hpe1564_USER_ERROR_HANDLER == happening) {
    hapIdx = hpe1564_USER_ERROR_HANDLER_IDX;
  } else if (hpe1564_INSTR_ERROR_HANDLER == happening) {
    hapIdx = hpe1564_INSTR_ERROR_HANDLER_IDX;
  } else if (VI_FALSE == hpe1564_findHappeningIdx(happening, &hapIdx)) {
    sprintf(errMsg, "%hd " hpe1564_MSG_INVALID_HAPPENING, happening);
    hpe1564_CDE_MESSAGE(errMsg);
    hpe1564_LOG_STATUS(vi, thisPtr, VI_ERROR_PARAMETER2);
  }

  *((void **)pEventHandler) = (void *)thisPtr->eventHandlerArray[hapIdx].eventHandler;
  *pUserData = thisPtr->eventHandlerArray[hapIdx].userData;

  hpe1564_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
}
