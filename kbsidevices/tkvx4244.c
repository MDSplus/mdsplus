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
/***************************************************************************
 *This file created using Driver Wizard version 1.8 and instrtmp.c 
 */

#include <string.h>		/* ANSI C string support */
#include <stdlib.h>		/* ANSI C support */
#include <stdio.h>		/* ANSI C support */
#include <visa.h>		/* VISA support */
#include <time.h>		/* used to support delay */
#include "tkvx4244.h"		/* Prototypes for this Driver */

/***************************************************************************
 * Tektronix VX4244 16-Channel Digitizer Module
 * Copyright Tektronix Inc. 1996, 1998
 * Originally written in C
 * Driver version 2.1.0
 * VXIPlug&Play WIN/WIN95/WINNT Frameworks Revision  4.0 
 * Compatible with Instrument Firmware Version 1.4 or later
 *
 */
 /***************************************************************************
  * Revision History: Original release 2.0.1
  * Revision 2.1.0:Nv 1998.
  * 1. Wrote a new function named tkvx4244_getBinaryData to retrieve binary data. 
  * (solves the  LabView transfer problem). Maintained the old function
  * for backward compatibility. Old function is not in the function panel file.
  * 2. Wrote a new function named tkvx4244_getFDCData to retrieve FDC data. 
  * (solves the  LabView transfer problem). Maintained the old function
  * for backward compatibility. Old function is not in the function panel file.
  * 3. Eliminated all references to VTL or vtl in comments.
  * 4. Eliminated all references to VXIplug&play specifications.
  * 5. Exposed the tkvx4244_write and tkvx4244_read functions in the function panel.
  * 6. Corrected all signed/unsigned mismatch warnings.
  * 7. Removed all unnecessary line termination escape characters '\'.
  * 8. Corrected the error_query function by having it return the appropriate
  *    value.
  */
#define tkvx4244_FIRMWARE		1.4
#define tkvx4244_DRVREV      	2.1
#define tkvx4244_DRVREV_MINOR   0
#define	tkvx4244_CMD_STRLEN		VI_FIND_BUFLEN
#define	tkvx4244_RCV_STRLEN		VI_FIND_BUFLEN
#define	tkvx4244_MAX_NUM_OF_LA	256
#define	tkvx4244_MANF_ID		0xffd
#define	tkvx4244_MAX_INSTR		12

/***************************************************************************
 * The following array is used to support multiple model codes with a
 * single driver.  
 */

ViUInt16 tkvx4244_modelCodes[] = { 0x70b };

#define tkvx4244_NUM_MODEL_CODES (sizeof(tkvx4244_modelCodes)/sizeof(ViUInt16))

/***************************************************************************
 * Instrument name which can be inserted in strings
 */

#define	tkvx4244_NAME "tkvx4244"

/***************************************************************************
 * The default VXIbus timeout to use
 */

#define	tkvx4244_DEFAULT_TIMEOUT	10000

/***************************************************************************
 * Encapsulates what search strings looks like
 */

#define	tkvx4244_VXI_SEARCH		"VXI?*INSTR"
#define	tkvx4244_GPIB_VXI_SEARCH	"GPIB-VXI?*INSTR"
#define	tkvx4244_GLOBAL_SEARCH	"?*INSTR"

/***************************************************************************
 TDI Function "_VI_FUNC" is unknown, so Trick part
 ***************************************************************************/
#undef _VI_FUNC
#define _VI_FUNC
/***************************************************************************
 * Driver Structure, encapsulates driver specific information on
 * a per instrument basis
 */

typedef struct {
  ViString instrDesc;		/* ASCII instrument descriptor string */
  ViReal64 tkvx4244_version;	/* Instrument firmware version */
  ViInt16 tkvx4244_chOneActive;
  ViInt16 tkvx4244_chTwoActive;
  ViInt16 tkvx4244_chThreeActive;
  ViInt16 tkvx4244_chFourActive;
  ViInt16 tkvx4244_chFiveActive;
  ViInt16 tkvx4244_chSixActive;
  ViInt16 tkvx4244_chSevenActive;
  ViInt16 tkvx4244_chEightActive;
  ViInt16 tkvx4244_chNineActive;
  ViInt16 tkvx4244_chTenActive;
  ViInt16 tkvx4244_chElevenActive;
  ViInt16 tkvx4244_chTwelveActive;
  ViInt16 tkvx4244_chThirteenActive;
  ViInt16 tkvx4244_chFourteenActive;
  ViInt16 tkvx4244_chFifteenActive;
  ViInt16 tkvx4244_chSixteenActive;

} tkvx4244_drvrStruct;

typedef tkvx4244_drvrStruct tkvx4244_VI_PTR tkvx4244_PdrvrStruct;

/***************************************************************************
 * Error Macros (private to driver)
 */

#define tkvx4244_GENERIC_ERROR  (_VI_ERROR+0x3FFC0800L)
#define tkvx4244_ERROR_VERSION_INVALID 	(1+tkvx4244_GENERIC_ERROR)
#define tkvx4244_NO_CHAN_SELECTED_ERROR	(2+tkvx4244_GENERIC_ERROR)
#define tkvx4244_NO_GRPS_SELECTED_ERROR	(3+tkvx4244_GENERIC_ERROR)
#define tkvx4244_THREE_CHAN_IN_GP_ERROR	(4+tkvx4244_GENERIC_ERROR)
#define tkvx4244_NUM_SAMPS_RANGE_ERROR	(5+tkvx4244_GENERIC_ERROR)
#define tkvx4244_FREQ_RANGE_ERROR	(6+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_CHAN_ONE (7+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_CHAN_TWO (8+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_CHAN_THREE (9+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_CHAN_FOUR (10+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_CHAN_FIVE (11+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_CHAN_SIX (12+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_CHAN_SEVEN (13+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_CHAN_EIGHT (14+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_CHAN_NINE (15+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_CHAN_TEN (16+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_CHAN_ELEVEN (17+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_CHAN_TWELVE (18+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_CHAN_THIRTEEN (19+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_CHAN_FOURTEEN (20+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_CHAN_FIFTEEN (21+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_CHAN_SIXTEEN (22+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_ALL_CHAN (23+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_OPEN_CLOSE (24+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_GP_ONE (25+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_GP_TWO (26+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_GP_THREE (27+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_GP_FOUR (28+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_ALL_GROUPS (29+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_INIT_TYPE (30+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_INIT_DELAY (31+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_EXT_ARM_CMD (32+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_TRIG_OUT_QUERY (33+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_MASTER_CLK_SRC (34+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_TRIG_SRC_QUERY (35+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_TRIG_ACTIVE_EDGE (36+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_EXT_TRIG (37+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_IMM_TRIG (38+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_THRE_TRIG (39+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_VXI_TRIG (40+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_TTL0_TRIG (41+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_TTL1_TRIG (42+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_TTL2_TRIG (43+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_TTL3_TRIG (44+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_TTL4_TRIG (45+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_TTL5_TRIG (46+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_TTL6_TRIG (47+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_TTL7_TRIG (48+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_TRIG_LOGIC (49+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_TRIG_SRC_CHOICE (50+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_SYNC_CLK_SRC (51+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_SYNC_CLK_DEF (52+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_SAMP_CNT (53+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_FETCH_CHL (54+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_BEGIN_MEM (55+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_GROUP_RANGE (56+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_SYNC_CLK_QUERY (57+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_ACT_CHNL_QUERY (58+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_ARM_SRC_QUERY (59+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_VOLT_RANGE_QUERY (60+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_CLOCK_SLOPE_QUERY (61+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_CLOCK_TTAG_QUERY (62+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_EVENT_REC_ENAB (63+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_4882_QUERY (64+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_DATA_BEG_MEM (65+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_DATA_SAMPS (66+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_DATA_TYPE (67+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_DATA_CHAN (68+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_OUT_TRG_TYPE (69+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_OUT_TRG_LOGIC (70+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_OUT_TRG_SLOPE (71+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_THRESH_LEVEL (72+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_THRESH_TYPE (73+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_EXT_ARM_SLP (74+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_EXT_ARM_ZERO (75+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_GROUP (76+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_INPUT_VOLT (77+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_FREQ (78+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_EXT_FREQ (79+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_FREQ_TYPE (80+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_FDC_MODE (81+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_FDC_CHAN (82+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_FDC_QUERY (83+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_MEM_ADDR (84+tkvx4244_GENERIC_ERROR)
#define tkvx4244_ERROR_FDC_BUF_SIZE (85+tkvx4244_GENERIC_ERROR)
/* Setup error messages, must be in same order as error codes above */

ViString tkvx4244_errorStrings[] = {
  "Generic error",
  "Firmware version is not compatible with this software driver.",
  "No channels selected",
  "No groups selected",
  "An attempt was made to enable three channels in a group (only 1,2 or 4 may be enabled).",
  "Parameter 2 (sample count) is out of range.",
  "Parameter 2 (freq) is out of range.",
  "Channel One Parameter is out of range.",
  "Channel Two Parameter is out of range.",
  "Channel Three Parameter is out of range.",
  "Channel Four Parameter is out of range.",
  "Channel Five Parameter is out of range.",
  "Channel Six Parameter is out of range.",
  "Channel Seven Parameter is out of range.",
  "Channel Eight Parameter is out of range.",
  "Channel Nine Parameter is out of range.",
  "Channel Ten Parameter is out of range.",
  "Channel Eleven Parameter is out of range.",
  "Channel Twelve Parameter is out of range.",
  "Channel Thirteen Parameter is out of range.",
  "Channel Fourteen Parameter is out of range.",
  "Channel Fifteen Parameter is out of range.",
  "Channel Sixteen Parameter is out of range.",
  "All Channel Parameter is out of range.",
  "Open Close Parameter is out of range.",
  "Group One Parameter is out of range.",
  "Group Two Parameter is out of range.",
  "Group Three Parameter is out of range.",
  "Group Four Parameter is out of range.",
  "All Groups Parameter is out of range.",
  "Type Init Parameter is out of range.",
  "Init Delay Parameter is out of range.",
  "External Trigger Slope Parameter is out of range.",
  "Output Trigger Query Parameter is out of range.",
  "Master Clock Source Parameter is out of range.",
  "Trigger source type query Parameter is out of range.",
  "Active edge Parameter is out of range.",
  "External trigger Parameter is out of range.",
  "Immediate trigger Parameter is out of range.",
  "Threshold trigger Parameter is out of range.",
  "VXI trigger Parameter is out of range.",
  "TTLTRG0 trigger Parameter is out of range.",
  "TTLTRG1 trigger Parameter is out of range.",
  "TTLTRG2 trigger Parameter is out of range.",
  "TTLTRG3 trigger Parameter is out of range.",
  "TTLTRG4 trigger Parameter is out of range.",
  "TTLTRG5 trigger Parameter is out of range.",
  "TTLTRG6 trigger Parameter is out of range.",
  "TTLTRG7 trigger Parameter is out of range.",
  "Trigger logic Parameter is out of range.",
  "No trigger source was chosen.",
  "Synch clock source parameter is out of range.",
  "Synch clock definition parameter is out of range.",
  "Number of samples parameter out of range.",
  "Channel number parameter out of range.",
  "Beginning memory location parameter out of range.",
  "Group selection parameter out of range.",
  "Synch clock query type parameter out of range.",
  "Active channel query type parameter out of range.",
  "Arm source query type parameter out of range.",
  "Volt range query type parameter out of range.",
  "Clock slope query type parameter out of range.",
  "Clock time tag query type parameter out of range.",
  "Event request enable parameter out of range.",
  "488.2 query parameter out of range.",
  "Fetch measurement begining memory location parameter out of range.",
  "Fetch measurement number of samples parameter out of range.",
  "Fetch measurement type parameter out of range.",
  "Fetch measurement channel parameter out of range.",
  "Output trigger type parameter out of range.",
  "Output trigger logic parameter out of range.",
  "Output trigger slope parameter out of range.",
  "Trigger threshold level parameter out of range.",
  "Trigger threshold type parameter out of range.",
  "External arm slope parameter out of range.",
  "External arm zero parameter out of range.",
  "Group number parameter out of range.",
  "Input voltage range parameter out of range.",
  "Frequency parameter out of range.",
  "External frequency parameter out of range.",
  "Frequency type parameter out of range.",
  "FDC mode parameter out of range.",
  "FDC Channel parameter (fdcGroup) out of range.",
  "FDC Query parameter out of range.",
  "FDC starting memory address out of range.",
  "Invalid FDC buffer size."
};

#define tkvx4244_NUMBER_OF_ERRORS (sizeof(tkvx4244_errorStrings)/sizeof(ViString))
/***************************************************************************
 * INSTRUMENT-DEPENDENT COMMAND ARRAYS
 */

	/*synch clock query */
ViString synch_clk[] = { "TTLT?", "LEVE?" };

	/*output trigger query */
ViString out_trig_qu[] = { "TRIG:SLOP?", "TRIG:TTLT?", "TRIG:LOGI?", "TRIG:MASK?" };

	/*trigger source and logic */
ViString trig_sorce[] = { "LOGI", "MASK", "SOUR" };

	/*Trigger arm slope */
ViString active_edge[] = { "POS", "NEG" };

	/*arm source query */
ViString arm_src_type[] = { "SOUR?", "ZERO?" };

	/*sample rate query */
ViString samp_rate_defs[] = { "TINT", "RANG", "SOUR" };

	/*volt range query */
ViString vlt_type[] = { "UPP", "LOW" };

	/*channel query */
ViString ch_cond[] = { "OPEN? (ALL)", "CLOS? (ALL)", "STAT?" };

    /* input range commands */
ViReal64 range[] = { 0.2, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0 };

    /*event setup */
ViString event_req[] = { "*SRE", "*ESE" };

    /*read status */
ViString read_status[] = { "*IDN?", "*STB?", "*SRE?", "*ESR?", "*ESE?" };

    /*External Arm */
ViString ext[] = { "EXT", "IMM" };

	/*Trig thresh channels */
ViString gp1[] = { "1,", "2,", "3,", "4,", "" };
ViString gp2[] = { "5,", "6,", "7,", "8,", "" };
ViString gp3[] = { "9,", "10,", "11,", "12,", "" };
ViString gp4[] = { "13,", "14,", "15,", "16,", "" };

	/*master trigger out */
ViString out_select[] = { "TTLT 0", "TTLT 1", "TTLT 2", "TTLT 3", "TTLT 4",
  "TTLT 5", "TTLT 6", "TTLT 7", "TTLT 8"
};

    /* sample clock */
ViString smpclk[] = { "EXT", "INT" };
ViString freqinval[] = { "TINT", "RANG" };

    /* arm commands */
ViString arm_slopZ[] = { "SLOP POS", "SLOP NEG", "ZERO 1", "ZERO 0" };

    /* trig source and logic commands */
ViString trgsrcL[] = { "LOGI AND", "LOGI OR", "SOUR EXT", "SOUR IMM",
  "SOUR TTL 0", "SOUR TTL 1", "SOUR TTL 2", "SOUR TTL 3", "SOUR TTL 4",
  "SOUR TTL 5", "SOUR TTL 6", "SOUR TTL 7", "SOUR THRE", "SOUR VXICMD"
};

    /* threshold trigger descriptor commands */
ViString thrshCmd[] = { "PSL", "NSL", "GTL", "LTL" };

    /* clock Synch Commands */
ViString clksynch[] = { "LEVEL POS", "LEVEL NEG", "LEVEL NEG", "IMM",
  "TTL 8", "TTL 1", "TTL 2", "TTL 3", "TTL 4", "TTL 5", "TTL 6", "TTL 7"
};

    /* postprocess commands */
ViString process[] = { "MIN", "MAX", "AVE", "PTR", "NTR", "TRMS" };

    /* master clock commands */
ViString mstr[] = { "CLK10", "INT" };

    /* external sample clock slope and time tag source commands */
ViString slopett[] = { "SLOP POS", "SLOP NEG", "TIME CLK10", "TIME ROSC" };
ViString fdc_mode[] = { "imm", "auto", "off" };
ViString fdcQuery[] = { "MODE", "TRANS", "QUE" };

/***************************************************************************
 * The following variables are used to keep track of available instruments.
 * By querying VISA once, and storing the results, autoconnects speeds
 * are sped up.  tkvx4244_gNumberFound is the number of instruments stored in
 * tkvx4244_instrAttr.  These variables are initialized in setupArrays().
 *
 */

ViInt16 tkvx4244_gNumberFound = 0;

struct {
  ViInt16 logAdr;		/* Instrument logical address */
  ViInt16 slot;			/* Instrument slot # */
  ViChar instrDesc[tkvx4244_CMD_STRLEN];	/* ASCII instrument descriptor string */
} tkvx4244_instrAttr[tkvx4244_MAX_INSTR];

/***************************************************************************
 * UTILITY ROUTINE PROTOTYPES (private to driver)
 */

ViStatus tkvx4244_invalidLongRange(ViInt32 val, ViInt32 min, ViInt32 max, ViStatus retError);
ViStatus tkvx4244_invalidRealRange(ViReal64 val, ViReal64 min, ViReal64 max, ViStatus retError);
ViStatus tkvx4244_invalidShortRange(ViInt16 val, ViInt16 min, ViInt16 max, ViStatus retError);
ViStatus tkvx4244_setupArrays(void);
ViStatus tkvx4244_findInstruments(ViSession resMgr, ViString searchString);

ViStatus tkvx4244_tryAutoConnect(ViInt16 slot, ViInt16 logAdr, ViPSession instrumentHandle);
ViStatus tkvx4244_verifyID(ViSession instrumentHandle);
ViStatus tkvx4244_returnDescriptor(ViInt16 theSlot, ViInt16 theLogAdr, ViString * descriptor);
ViStatus tkvx4244_variableInitialize(ViSession instrumentHandle);
ViStatus tkvx4244_chlst_parse(ViInt16 c1, ViInt16 c2, ViInt16 c3, ViInt16 c4,
			      ViInt16 c5, ViInt16 c6, ViInt16 c7, ViInt16 c8, ViInt16 c9,
			      ViInt16 c10, ViInt16 c11, ViInt16 c12, ViInt16 c13, ViInt16 c14,
			      ViInt16 c15, ViInt16 c16, ViInt16 ca, ViChar buf[]);
ViStatus tkvx4244_gplist_parse(ViInt16 g1, ViInt16 g2, ViInt16 g3, ViInt16 g4, ViInt16 ga,
			       ViChar buf[]);
/***************************************************************************
 *	User accessible routines follow. These routines provide the interface
 *	to the driver for user programs.
 */

/*-----------CHANGE: INSERT INSTRUMENT-DEPENDENT USER ROUTINES HERE -------*/

/***************************************************************************
 * Function: tkvx4244_autoConnectToAll
 *
 * Purpose:
 *	Connect to all tkvx4244 instruments found.
 *
 * Parameters:
 *	ViSession instrumentHndlArray[]: Array of VISA instrument handles used to access
 *		instrument specific data. One handle initialized by this routine
 *		for each instrument found.
 *	ViInt16 arrayLength: Number of entries which can be put in instrArray
 *	ViPInt16 numberConnected: Number of valid entries in instrArray upon return
 *
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 */

ViStatus _VI_FUNC tkvx4244_autoConnectToAll(ViSession instrumentHndlArray[],
					    ViInt16 arrayLength, ViPInt16 numberConnected)
{
  ViStatus retError;		/* Returned error status */
  ViInt16 numFound;		/* # instrs found */
  ViInt16 theLogAdr[tkvx4244_MAX_NUM_OF_LA];	/* temporary storage for LAs */
  ViInt16 ii;			/* loop count */

  *numberConnected = 0;

  retError = tkvx4244_getSlotandLAList(VI_NULL, theLogAdr, tkvx4244_RCV_STRLEN, &numFound);

  if (retError < VI_SUCCESS) {
    return retError;
  } else if (numFound == 0) {
    return VI_ERROR_RSRC_NFOUND;
  } else if (numFound > arrayLength) {
    numFound = arrayLength;
  }

  for (ii = 0; ii < numFound; ii++) {
    retError = tkvx4244_tryAutoConnect(VI_NULL, theLogAdr[ii], &instrumentHndlArray[ii]);

    if (retError < VI_SUCCESS) {
      return retError;
    }

    (*numberConnected)++;
  }

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_autoConnectToFirst
 *
 * Purpose:
 *		Connect to first tkvx4244 instrument found
 *
 * Parameters:
 *	ViPSession instrumentHandle: Address of VISA instrument handle used to
 *						access instrument specific data. Initialized by
 *						this routine.
 *
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 */

ViStatus _VI_FUNC tkvx4244_autoConnectToFirst(ViPSession instrumentHandle)
{
  ViStatus retError;		/* Returned error status */
  ViInt16 numFound;		/* # instrs found */
  ViInt16 theLogAdr;		/* logical address */

  retError = tkvx4244_getSlotandLAList(VI_NULL, &theLogAdr, 1, &numFound);
  if (retError < VI_SUCCESS) {
    return retError;
  } else if (numFound == 0) {
    return VI_ERROR_RSRC_NFOUND;
  }

  retError = tkvx4244_tryAutoConnect(VI_NULL, theLogAdr, instrumentHandle);
  if (retError < VI_SUCCESS)
    return retError;
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_autoConnectToLA
 *
 * Purpose:
 *	Connect to instrument in selected slot.
 *
 * Parameters:
 *	ViPSession instrumentHandle: Address of VISA instrument handle used to
 *						access instrument specific data. Initialized by
 *						this routine.
 *	ViInt16 logicalAddress: Logical address to connect to
 *
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 */

ViStatus _VI_FUNC tkvx4244_autoConnectToLA(ViPSession instrumentHandle, ViInt16 logicalAddress)
{
  ViStatus retError;		/* Returned error status */

  retError = tkvx4244_tryAutoConnect(VI_NULL, logicalAddress, instrumentHandle);
  if (retError < VI_SUCCESS)
    return retError;
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_autoConnectToSlot
 *
 * Purpose:
 *	Connect to instrument in selected slot.
 *
 * Parameters:
 *	ViSession instrumentArray[]: 	Array of VISA instrument handles used
 *		to access instrument specific data. One handle initialized
 *		by this routine for each instrument found.
 *	ViInt16 arrayLength: 	Number of entries which can be put in
 *      instrumentArray
 *	ViPInt16 numberConnected: Number of valid entries in instrumentArray
 *		upon return
 *	ViInt16 slot: 		Slot to connect to
 *
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 */

ViStatus _VI_FUNC tkvx4244_autoConnectToSlot(ViSession instrumentArray[],
					     ViInt16 arrayLength, ViPInt16 numberConnected,
					     ViInt16 slot)
{
  ViStatus retError;		/* Returned error status */
  ViInt16 numFound;		/* Temporary storage for # instrs found */
  ViInt16 theSlotPtr[tkvx4244_RCV_STRLEN];
  ViInt16 theLogAdr[tkvx4244_RCV_STRLEN];
  ViInt16 ii;			/* loop count */

  *numberConnected = 0;

  retError = tkvx4244_getSlotandLAList(theSlotPtr, theLogAdr, tkvx4244_RCV_STRLEN, &numFound);
  if (retError < VI_SUCCESS) {
    return retError;
  } else if (numFound == 0) {
    return VI_ERROR_RSRC_NFOUND;
  }

  for (ii = 0; ii < numFound; ii++) {
    if (theSlotPtr[ii] == slot) {
      retError = tkvx4244_tryAutoConnect(VI_NULL, theLogAdr[ii],
					 &instrumentArray[*numberConnected]);
      if (retError < VI_SUCCESS)
	return retError;

      (*numberConnected)++;
    }

    if (*numberConnected == arrayLength)
      break;
  }

  if (*numberConnected == 0) {
    return VI_ERROR_RSRC_NFOUND;
  } else {
    return VI_SUCCESS;
  }
}

/***************************************************************************
 * Function: tkvx4244_close
 *
 * Purpose:
 *	This function closes the instrument and returns memory allocated for
 *	instrument specific data storage.
 *
 * CHANGE: add deletion mechanisms here for any dynamically allocated
 *	data structures added to the driver structure.
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *						specific data.
 *
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 *
 * 	NOTE!!!! instrumentHandle is no longer valid, but cannot be changed by
 *	this routine. It is important that users do not use this handle
 *	after calling this routine.
 */

ViStatus _VI_FUNC tkvx4244_close(ViSession instrumentHandle)
{
  ViStatus retError;		/* Returned error status */
  ViAddr userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  if (instrumentHandle != VI_NULL) {
    retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
    if (retError < VI_SUCCESS)
      return retError;

    drvrHdl = (tkvx4244_PdrvrStruct) userData;

    if (drvrHdl != NULL) {
      /* Free any mallocs in the tkvx4244_instr_array struct */
      free(drvrHdl->instrDesc);

      free(drvrHdl);
    }

    retError = viClose(instrumentHandle);
    return retError;
  } else {
    return VI_SUCCESS;
  }
}

/***************************************************************************
 * Function: tkvx4244_init
 *
 * Purpose:
 *	This function opens the instrument, and depending upon flags passed
 *	in, queries for ID and initializes the instrument to a known state.
 *
 * Parameters:
 *	ViRsrc resourceName: Instrument search expression, 
 *	ViBoolean IDQuery: Boolean flag, VI_TRUE -> verify correct instrument
 *	ViBoolean resetDevice: Boolean flag, VI_TRUE -> reset instrument
 *	ViPSession instrumentHandle: VISA instrument handle used to access instrument
 *						 specific data. The handle is initialized by
 *						 this routine.
 *
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 *	If no error, instrumentHandle is initialized for use in rest of driver
 */

ViStatus _VI_FUNC tkvx4244_init(ViRsrc resourceName, ViBoolean IDQuery,
				ViBoolean resetDevice, ViPSession instrumentHandle)
{
  ViSession resMgr;		/* Resource Manager Session */
  ViStatus retError;		/* Returned error status */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  /* Initialize Instrument-Dependent Command arrays. */

  tkvx4244_setupArrays();

  /* Open Instrument */

  retError = viGetDefaultRM(&resMgr);
  if (retError < VI_SUCCESS) {
    *instrumentHandle = VI_NULL;
    return retError;
  }

  retError = viOpen(resMgr, resourceName, VI_NULL, VI_NULL, instrumentHandle);
  if (retError < VI_SUCCESS) {
    *instrumentHandle = VI_NULL;
    return retError;
  }

  /* No user data yet, so null it out */

  retError = viSetAttribute(*instrumentHandle, VI_ATTR_USER_DATA, (ViAttr) VI_NULL);
  if (retError < VI_SUCCESS) {
    tkvx4244_close(*instrumentHandle);
    *instrumentHandle = VI_NULL;
    return retError;
  }

  /* Perform ID Query */

  if (IDQuery) {
    retError = tkvx4244_verifyID(*instrumentHandle);
    if (retError < VI_SUCCESS) {
      tkvx4244_close(*instrumentHandle);
      *instrumentHandle = VI_NULL;
      return retError;
    }
  }

  /* Initialize the instrument to a known state. */
  if (resetDevice) {
    retError = tkvx4244_reset(*instrumentHandle);
    if (retError < VI_SUCCESS) {
      tkvx4244_close(*instrumentHandle);
      *instrumentHandle = VI_NULL;
      return retError;
    }
  }

  /* Set default timeout */

  retError = viSetAttribute(*instrumentHandle, VI_ATTR_TMO_VALUE, tkvx4244_DEFAULT_TIMEOUT);
  if (retError < VI_SUCCESS) {
    tkvx4244_close(*instrumentHandle);
    *instrumentHandle = VI_NULL;
    return retError;
  }

  /* Initialize interface for instrument. */

  drvrHdl = (tkvx4244_PdrvrStruct) malloc(sizeof(tkvx4244_drvrStruct));
  if (drvrHdl == NULL) {
    tkvx4244_close(*instrumentHandle);
    *instrumentHandle = VI_NULL;
    return VI_ERROR_ALLOC;
  }

  retError = viSetAttribute(*instrumentHandle, VI_ATTR_USER_DATA, (ViAttr) drvrHdl);
  if (retError < VI_SUCCESS) {
    tkvx4244_close(*instrumentHandle);
    *instrumentHandle = VI_NULL;
    return retError;
  }

  drvrHdl->instrDesc = (ViString) malloc(strlen(resourceName) + 1);
  if (drvrHdl->instrDesc == NULL) {
    tkvx4244_close(*instrumentHandle);
    *instrumentHandle = VI_NULL;
    return VI_ERROR_ALLOC;
  }

  strcpy(drvrHdl->instrDesc, resourceName);

  /* CHANGE:  Add instrument specific initialization here */
  retError = tkvx4244_variableInitialize(*instrumentHandle);
  /*if(retError) */
  return VI_SUCCESS;		/* return */
}

/*********************************************************************************
 * Function: tkvx4244_variableInitialize
 *
 * Purpose: Initialize instrument specific structure elements to power
 *         up condition.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4240_init()
 *		prior to use.
 * Returns:
 *	VI_VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus tkvx4244_variableInitialize(ViSession instrumentHandle)
{

  ViStatus retError;		/* Returned error status */
  ViAddr userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  drvrHdl->tkvx4244_chOneActive = tkvx4244_CH_ONE_ENABLE;
  drvrHdl->tkvx4244_chTwoActive = tkvx4244_CH_TWO_DISABLE;
  drvrHdl->tkvx4244_chThreeActive = tkvx4244_CH_THREE_DISABLE;
  drvrHdl->tkvx4244_chFourActive = tkvx4244_CH_FOUR_DISABLE;
  drvrHdl->tkvx4244_chFiveActive = tkvx4244_CH_FIVE_DISABLE;
  drvrHdl->tkvx4244_chSixActive = tkvx4244_CH_SIX_DISABLE;
  drvrHdl->tkvx4244_chSevenActive = tkvx4244_CH_SEVEN_DISABLE;
  drvrHdl->tkvx4244_chEightActive = tkvx4244_CH_EIGHT_DISABLE;
  drvrHdl->tkvx4244_chNineActive = tkvx4244_CH_NINE_DISABLE;
  drvrHdl->tkvx4244_chTenActive = tkvx4244_CH_TEN_DISABLE;
  drvrHdl->tkvx4244_chElevenActive = tkvx4244_CH_ELEVEN_DISABLE;
  drvrHdl->tkvx4244_chTwelveActive = tkvx4244_CH_TWELVE_DISABLE;
  drvrHdl->tkvx4244_chThirteenActive = tkvx4244_CH_THIRTEEN_DISABLE;
  drvrHdl->tkvx4244_chFourteenActive = tkvx4244_CH_FOURTEEN_DISABLE;
  drvrHdl->tkvx4244_chFifteenActive = tkvx4244_CH_FIFTEEN_DISABLE;
  drvrHdl->tkvx4244_chSixteenActive = tkvx4244_CH_SIXTEEN_DISABLE;

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_error_message
 *
 * Purpose:
 *	This function returns a text message for a corresponding instrument
 *	driver error code.
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data.
 *	ViStatus errorCode: error code to check
 *	ViChar errorMessage[]: Returned error string
 *
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 */

ViStatus _VI_FUNC tkvx4244_error_message(ViSession instrumentHandle, ViStatus errorCode,
					 ViChar errorMessage[])
{
  ViStatus retError;		/* Returned error status */

  retError = VI_SUCCESS;

  if ((errorCode >= tkvx4244_GENERIC_ERROR) && (errorCode <= 0xBFFC0FFFL)
      && (errorCode < VI_SUCCESS)) {
    errorCode -= tkvx4244_GENERIC_ERROR;

    if (errorCode < tkvx4244_NUMBER_OF_ERRORS) {
      sprintf(errorMessage, "%s Error: %s", tkvx4244_NAME, tkvx4244_errorStrings[errorCode]);
    } else {
      sprintf(errorMessage, "%s Error: Unrecognized error %ld",
	      tkvx4244_NAME, errorCode + tkvx4244_GENERIC_ERROR);
    }
  } else {
    switch (errorCode) {
    case VI_WARN_NSUP_ID_QUERY:
      {
	strcpy(errorMessage, "Warning: ID query is not supported.");
	break;
      }
    case VI_WARN_NSUP_RESET:
      {
	strcpy(errorMessage, "Warning: reset is not supported.");
	break;
      }
    case VI_WARN_NSUP_SELF_TEST:
      {
	strcpy(errorMessage, "Warning: self-test is not supported.");
	break;
      }
    case VI_WARN_NSUP_ERROR_QUERY:
      {
	strcpy(errorMessage, "Warning: error query is not supported.");
	break;
      }
    case VI_WARN_NSUP_REV_QUERY:
      {
	strcpy(errorMessage, "Warning: revision query is not supported.");
	break;
      }
    default:
      {
	retError = viStatusDesc(instrumentHandle, errorCode, errorMessage);
      }
    }
  }

  return retError;
}

/***************************************************************************
 * Function: tkvx4244_error_query
 *
 * Purpose:
 *	This function queries the instrument for errors.
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *			specific data.
 *	ViPStatus errorCode: Instrument error code returned
 *	ViChar errorMessage[]: Textual version of returned error code
 *
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 */

ViStatus _VI_FUNC tkvx4244_error_query(ViSession instrumentHandle, ViPInt32 errorCode,
				       ViChar errorMessage[])
{
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* Instrument command string */
  ViChar rcv[tkvx4244_RCV_STRLEN];	/* Instrument receive string */
  ViInt32 retCnt;		/* VISA return count */
  ViStatus retError;		/* Returned error status */

  strcpy(cmd, "SYSTEM:ERROR?");
  cmd[strlen(cmd)] = '\0';
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  retError = viRead(instrumentHandle, (ViPBuf) rcv, tkvx4244_RCV_STRLEN, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;

  rcv[retCnt] = '\0';

  *errorCode = (ViInt32) strtod(&rcv[0], (char **)NULL);
  strcpy(errorMessage, rcv);

  return VI_SUCCESS;;
}

/***************************************************************************
 * Function: tkvx4244_getInstrDesc
 *
 * Purpose:
 *	Return instrument descriptor string of instrument
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data.
 *	ViChar instrumentDescriptor[]: Storage for returned instrument descriptor
 *
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 */

ViStatus _VI_FUNC tkvx4244_getInstrDesc(ViSession instrumentHandle, ViChar instrumentDescriptor[])
{
  ViStatus retError;		/* Returned error status */
  ViAddr userData;		/* User data attribute value */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);

  if (retError < VI_SUCCESS) {
    return retError;
  }

  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /*
   * get the descriptor
   */

  strcpy(instrumentDescriptor, drvrHdl->instrDesc);

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_getLogicalAddress
 *
 * Purpose:
 *	Return logical address of instrument
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data.
 *	ViPInt16 logicalAddress: Storage for returned logical address #
 *
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 */

ViStatus _VI_FUNC tkvx4244_getLogicalAddress(ViSession instrumentHandle, ViPInt16 logicalAddress)
{
  ViStatus retError;		/* Returned error status */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_VXI_LA, logicalAddress);

  return retError;
}

/***************************************************************************
 * Function: tkvx4244_getManufacturerID
 *
 * Purpose:
 *	Returns manufacturer ID of instrument
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data.
 *	ViPInt16 manufacturerIdentification: pointer to storage for ID
 *
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 */

ViStatus _VI_FUNC tkvx4244_getManufacturerID(ViSession instrumentHandle,
					     ViPInt16 manufacturerIdentification)
{
  ViStatus retError;		/* Returned error status */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_MANF_ID, manufacturerIdentification);

  *manufacturerIdentification &= 0xfff;

  return retError;
}

/***************************************************************************
 * Function: tkvx4244_getInterfaceType
 *
 * Purpose:
 *	Returns interface type
 *
 * VI_INTF_GPIB (1)
 * VI_INTF_VXI (2)
 * VI_INTF_GPIB_VXI (3)
 * VI_INTF_ASRL (4)
 * VI_INTF_PXI (5)
 * VI_INTF_TCPIP (6)
 * VI_INTF_USB (7)
 */
ViStatus _VI_FUNC tkvx4244_getInterfaceType(ViSession instrumentHandle, ViPUInt16 interface)
{
  ViStatus retError;		/* Returned error status */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_INTF_TYPE, interface);

  return retError;
}

/***************************************************************************
 * Function: tkvx4244_getModelCode
 *
 * Purpose:
 *	Returns model code of instrument
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data.
 *	ViPInt16 modelCode: pointer to storage for code
 *
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 */

ViStatus _VI_FUNC tkvx4244_getModelCode(ViSession instrumentHandle, ViPInt16 modelCode)
{
  ViStatus retError;		/* Returned error status */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_MODEL_CODE, modelCode);

  return retError;
}

/***************************************************************************
 * Function: tkvx4244_getSlotandLAList
 *
 * Purpose:
 *	Return lists of slot numbers and logical address for all tkvx4244's.
 *
 * Parameters:
 *	ViInt16 slotArray[]: array of slot #s
 *	ViInt16 logicalAddressArray[]: array of logical addresses
 *	ViInt16  arrayLength: length of these arrays
 *	ViPInt16 numberFound: Return value for number of matching instruments found
 *
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 */

ViStatus _VI_FUNC tkvx4244_getSlotandLAList(ViInt16 slotArray[],
					    ViInt16 logicalAddressArray[], ViInt16 arrayLength,
					    ViPInt16 numberFound)
{
  ViStatus retError;		/* Returned error status */
  ViInt16 ii;			/* loop count */

  /* Initialize Instrument-Dependent Command arrays. */

  retError = tkvx4244_setupArrays();

  if (retError < VI_SUCCESS)
    return retError;

  if (tkvx4244_gNumberFound == 0) {
    return VI_ERROR_RSRC_NFOUND;
  } else if (tkvx4244_gNumberFound > arrayLength) {
    *numberFound = arrayLength;
  } else {
    *numberFound = tkvx4244_gNumberFound;
  }

  if ((slotArray == NULL) && (logicalAddressArray == NULL)) {
    *numberFound = 0;
  }

  for (ii = 0; ii < *numberFound; ii++) {
    /*
     * Add instrument slot number and logical address to arrays
     */

    if (slotArray != NULL) {
      slotArray[ii] = tkvx4244_instrAttr[ii].slot;
    }

    if (logicalAddressArray != NULL) {
      logicalAddressArray[ii] = tkvx4244_instrAttr[ii].logAdr;
    }
  }

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_getSlotNumber
 *
 * Purpose:
 *	Return slot number of instrument
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *	ViPInt16 slot: Storage for returned slot #
 *
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 */

ViStatus _VI_FUNC tkvx4244_getSlotNumber(ViSession instrumentHandle, ViPInt16 slot)
{
  ViStatus retError;		/* Returned error status */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_SLOT, slot);

  return retError;
}

/***************************************************************************
 * Function: tkvx4244_getSlotList
 *
 * Purpose:
 *	Return lists of slot numbers for all tkvx4244's.
 *
 * Parameters:
 *	ViInt16 slotArray[]: array of slot #s
 *	ViInt16  slotArrayLength: length of these arrays
 *	ViPInt16 numberFound: Return value for number of matching instruments found
 *
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 */

ViStatus _VI_FUNC tkvx4244_getSlotList(ViInt16 slotArray[], ViInt16 slotArrayLength,
				       ViPInt16 numberFound)
{
  ViStatus retError;		/* Returned error status */

  retError = tkvx4244_getSlotandLAList(slotArray, VI_NULL, slotArrayLength, numberFound);

  return retError;
}

/***************************************************************************
 * Function: tkvx4244_getVisaRev
 *
 * Purpose:
 *	Return visa revison as an ASCII string
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *	ViChar revision[]: Storage for returned instrument descriptor
 *
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 */

ViStatus _VI_FUNC tkvx4244_getVisaRev(ViSession instrumentHandle, ViChar revision[])
{
  ViStatus retError;		/* Returned error status */
  ViVersion tmpRev;		/* VISA revision attribute */
  ViInt16 tmpMajor;		/* VISA major revison digits */
  ViInt16 tmpMinor;		/* VISA minor revison digits */
  ViInt16 tmpSubMinor;		/* VISA subMinor revison digits */
  ViChar vtl[10];
  /*
   * get the descriptor
   */
  retError = viGetAttribute(instrumentHandle, VI_ATTR_RSRC_SPEC_VERSION, &tmpRev);

  if (retError < VI_SUCCESS) {
    return retError;
  }

  tmpMajor = (ViInt16) (tmpRev >> 16);
  tmpMinor = (ViInt16) (tmpRev & 0xff00);
  tmpMinor = (ViInt16) (tmpMinor >> 8);
  tmpSubMinor = (ViInt16) (tmpRev & 0x00ff);

  if (tmpMajor < 4)
    strcpy(vtl, "VTL ");
  else {
    strcpy(vtl, "");
    tmpMajor = (ViInt16) VI_VERSION_MAJOR(tmpRev);
    tmpMinor = (ViInt16) VI_VERSION_MINOR(tmpRev);
    tmpSubMinor = (ViInt16) VI_VERSION_SUBMINOR(tmpRev);
  }

  if (tmpSubMinor == 0) {
    sprintf(revision, "%s%hd.%hd", vtl, tmpMajor, tmpMinor);
  } else {
    sprintf(revision, "%s%hd.%hd.%hd", vtl, tmpMajor, tmpMinor, tmpSubMinor);
  }

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_reset
 *
 * Purpose:
 *	This function resets the instrument.
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data.
 *
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 */

ViStatus _VI_FUNC tkvx4244_reset(ViSession instrumentHandle)
{
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* Instrument command string */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* VISA return count */

  sprintf(cmd, "*RST");
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);

  /*  CHANGE: If reset is not supported return the following

     return VI_WARN_NSUP_RESET;  */

  return retError;
}

/***************************************************************************
 * Function: tkvx4244_revision_query
 *
 * Purpose:
 *	This function returns the driver and instrument revisions.
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data.
 *	ViChar driverRevision[]: Revision string for driver
 *	ViChar instrumentRevision[]: Revison string for instrument firmware
 *
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 */

ViStatus _VI_FUNC tkvx4244_revision_query(ViSession instrumentHandle, ViChar driverRevision[],
					  ViChar instrumentRevision[])
{
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* Instrument command string */
  ViChar rcv[tkvx4244_RCV_STRLEN];	/* Instrument receive string */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* VISA return count */
  ViAddr userData;		/* User data attribute value */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  sprintf(driverRevision, "%2.1f%hd", tkvx4244_DRVREV, tkvx4244_DRVREV_MINOR);

  strcpy(cmd, "*IDN?\n");
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  retError = viRead(instrumentHandle, (ViPBuf) rcv, tkvx4244_RCV_STRLEN, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  rcv[retCnt] = '\0';

  drvrHdl->tkvx4244_version = strtod(&rcv[40], (char **)NULL);
  sprintf(instrumentRevision, "%3.2f", drvrHdl->tkvx4244_version);
  if (drvrHdl->tkvx4244_version < tkvx4244_FIRMWARE)
    return tkvx4244_ERROR_VERSION_INVALID;

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_self_test
 *
 * Purpose:
 *	This function executes the instrument self-test and returns the result.
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data.
 *	ViPInt16 selfTestResult: Boolean flag specifying self-test status
 *	ViChar selfTestMessage[]: Textual description of self-test status
 *
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 */

ViStatus _VI_FUNC tkvx4244_self_test(ViSession instrumentHandle, ViPInt16 selfTestResult,
				     ViChar selfTestMessage[])
{
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* Instrument command string */
  ViChar rcv[tkvx4244_RCV_STRLEN];	/* Instrument receive string */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* VISA return count */

  strcpy(cmd, "*TST\n");
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  tkvx4244_sleep(30);
  strcpy(cmd, "*TST?");
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  retError = viRead(instrumentHandle, (ViPBuf) rcv, tkvx4244_RCV_STRLEN, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;

  if (rcv[0] == '0') {
    *selfTestResult = VI_FALSE;
    sprintf(selfTestMessage, "Self Test Passed");
  } else {
    *selfTestResult = VI_TRUE;
    sprintf(selfTestMessage, "Self Test Failed\n\rUse Error query to check failures");
  }

  return VI_SUCCESS;
}

/*************************************************************
**************************************************************
***This second self_test function is for compatibility only.**
***Keep it around to ensure old code works********************
*************************************************************/
/***************************************************************************
 * Function: tkvx4244_perform_self_test
 *
 * Purpose:
 *	This function executes the instrument self-test and returns the result.
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data.
 *	ViPStatus selfTestResult: Boolean flag specifying self-test status
 *	ViChar selfTestMessage[]: Textual description of self-test status
 *
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 */

ViStatus _VI_FUNC tkvx4244_perform_self_test(ViSession instrumentHandle, ViPStatus selfTestResult,
					     ViChar selfTestMessage[])
{
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* Instrument command string */
  ViChar rcv[tkvx4244_RCV_STRLEN];	/* Instrument receive string */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* VISA return count */

  strcpy(cmd, "*TST\n");
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  tkvx4244_sleep(30);
  strcpy(cmd, "*TST?");
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  retError = viRead(instrumentHandle, (ViPBuf) rcv, tkvx4244_RCV_STRLEN, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;

  if (rcv[0] == '0') {
    *selfTestResult = VI_FALSE;
    sprintf(selfTestMessage, "Self Test Passed");
  } else {
    *selfTestResult = VI_TRUE;
    sprintf(selfTestMessage, "Self Test Failed\n\rUse Error query to check failures");
  }

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_sleep
 *
 * Purpose:
 *	Stop processing for specified # of seconds.
 *
 * Parameters:
 *	ViInt32 secondsToDelay: # of seconds to delay for
 *
 * Returns:
 *	ViStatus VI_SUCCESS
 */

ViStatus _VI_FUNC tkvx4244_sleep(ViInt32 secondsToDelay)
{
  time_t start;			/* time of entry into this routine */
  time_t current;		/* Updated time as routine progresses */

  time(&start);			/* initialize start time */
  time(&current);		/* initialize update time */

  while ((current - start) < (time_t) secondsToDelay) {	/* has delay completed? */
    time(&current);		/* no, update */
  }
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_write
 *
 * Purpose:
 *	This function writes to an instrument.
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data.
 *	ViString command:	String that is passed to an instrument.
 *
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 */

ViStatus _VI_FUNC tkvx4244_write(ViSession instrumentHandle, ViString command)
{
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* Instrument command string */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* VISA return count */

  sprintf(cmd, "%s", command);
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_read
 *
 * Purpose:
 *	This function executes a read.
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data.
 *	ViChar message[]:	Returns the read string.
 *
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 */

ViStatus _VI_FUNC tkvx4244_read(ViSession instrumentHandle, ViChar message[])
{
  ViChar rcv[tkvx4244_RCV_STRLEN];	/* Instrument receive string */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* VISA return count */

  retError = viRead(instrumentHandle, (ViPBuf) rcv, tkvx4244_RCV_STRLEN, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  rcv[retCnt] = '\0';
  strcpy(message, rcv);

  return VI_SUCCESS;
}

/***************************************************************************
 *	Utility routines follow. These routines are local to the driver and are
 *	not accessible from user applications.
 */

/*----------- INSERT INSTRUMENT-DEPENDENT UTILITY ROUTINES HERE -----------*/
/***************************************************************************
 * Function: tkvx4244_confGpOneTrigSrcLogic
 *
 * Purpose:
 *  This function captures data on a selected channel and returns 
 *  the data in a character array.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to 
 *  access instrument specific data. Must be initialized by 
 *  tkvx4244_init() prior to use.
 *  ViInt16 channel:
 *	ViInt16 voltageRange:
 *  ViInt16 intervalFrequency:
 *  ViReal64 samplingRate:
 *  ViInt32 startingAddress:
 *	ViInt16 numberOfSamples:
 *	ViChar dataArray[]:
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_measCaptureReturnData(ViSession instrumentHandle, ViInt16 channel,
			       ViInt16 voltageRange, ViInt16 intervalFrequency,
			       ViReal64 * samplingRate, ViInt32 startingAddress,
			       ViInt16 numberOfSamples, ViChar dataArray[])
{
  ViInt16 gpch1, gpch2, gpch3, gpch4;
  ViInt16 gone, gtwo, gthree, gfour;
  ViStatus retError;		/* Returned error status */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  retError = tkvx4244_invalidShortRange(channel, tkvx4244_ANALYZE_CH1,
					tkvx4244_ANALYZE_CH16, tkvx4244_ERROR_FETCH_CHL);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(voltageRange, tkvx4244_VOLT_RANGE_200MV,
					tkvx4244_VOLT_RANGE_20VOLT, tkvx4244_ERROR_INPUT_VOLT);
  if (retError)
    return retError;

  gpch1 = gpch2 = gpch3 = gpch4 = VI_FALSE;
  gone = gtwo = gthree = gfour = VI_FALSE;
  /*Reset the module */
  retError = tkvx4244_reset(instrumentHandle);
  if (retError)
    return retError;
  /*open all channels in Group 1(ch 1) */
  retError = tkvx4244_confGroupOneChannels(instrumentHandle,
					   tkvx4244_CH_ONE_ENABLE, tkvx4244_CH_TWO_ENABLE,
					   tkvx4244_CH_THREE_ENABLE, tkvx4244_CH_FOUR_ENABLE,
					   tkvx4244_SELECT_DISABLE);
  if (retError)
    return retError;
  /*Define and enable the selected channel. Set up selected parameters */
  if (channel < tkvx4244_ANALYZE_CH5) {
    gone = VI_TRUE;
    if (channel == tkvx4244_ANALYZE_CH1)
      gpch1 = 1;
    if (channel == tkvx4244_ANALYZE_CH2)
      gpch2 = 1;
    if (channel == tkvx4244_ANALYZE_CH3)
      gpch3 = 1;
    if (channel == tkvx4244_ANALYZE_CH4)
      gpch4 = 1;
    retError = tkvx4244_confGroupOneChannels(instrumentHandle, gpch1,
					     gpch2, gpch3, gpch4, tkvx4244_SELECT_ENABLE);
    if (retError)
      return retError;
    retError = tkvx4244_confGpOneVoltageRange(instrumentHandle, gpch1,
					      gpch2, gpch3, gpch4, voltageRange);
    if (retError)
      return retError;
    retError = tkvx4244_confSamplingInterval(instrumentHandle,
					     tkvx4244_CLOCK_INTERNAL, intervalFrequency,
					     samplingRate, gone, gtwo, gthree, gfour,
					     tkvx4244_ALL_GROUPS_DISABLE);
    if (retError)
      return retError;
    retError = tkvx4244_confDataCollectCount(instrumentHandle, 131072, gone, gtwo, gthree, gfour,
					     tkvx4244_ALL_GROUPS_DISABLE);
    if (retError)
      return retError;
    retError = tkvx4244_initMeasurementCycle(instrumentHandle, VI_FALSE, gone, gtwo, gthree, gfour,
					     tkvx4244_ALL_GROUPS_DISABLE, tkvx4244_INIT_IMM);
    if (retError)
      return retError;

  }

  if ((channel > tkvx4244_ANALYZE_CH4) && (channel < tkvx4244_ANALYZE_CH9)) {
    gtwo = VI_TRUE;
    if (channel == tkvx4244_ANALYZE_CH5)
      gpch1 = 1;
    if (channel == tkvx4244_ANALYZE_CH6)
      gpch2 = 1;
    if (channel == tkvx4244_ANALYZE_CH7)
      gpch3 = 1;
    if (channel == tkvx4244_ANALYZE_CH8)
      gpch4 = 1;
    retError = tkvx4244_confGroupTwoChannels(instrumentHandle, gpch1,
					     gpch2, gpch3, gpch4, tkvx4244_SELECT_ENABLE);
    if (retError)
      return retError;
    retError = tkvx4244_confGpTwoVoltageRange(instrumentHandle, gpch1,
					      gpch2, gpch3, gpch4, voltageRange);
    if (retError)
      return retError;
    retError = tkvx4244_confSamplingInterval(instrumentHandle,
					     tkvx4244_CLOCK_INTERNAL, intervalFrequency,
					     samplingRate, gone, gtwo, gthree, gfour,
					     tkvx4244_ALL_GROUPS_DISABLE);
    if (retError)
      return retError;
    retError = tkvx4244_confDataCollectCount(instrumentHandle, 131072, gone, gtwo, gthree, gfour,
					     tkvx4244_ALL_GROUPS_DISABLE);
    if (retError)
      return retError;
    retError = tkvx4244_initMeasurementCycle(instrumentHandle, VI_FALSE, gone, gtwo, gthree, gfour,
					     tkvx4244_ALL_GROUPS_DISABLE, tkvx4244_INIT_IMM);
    if (retError)
      return retError;
  }
  if ((channel > tkvx4244_ANALYZE_CH8) && (channel < tkvx4244_ANALYZE_CH13)) {
    gthree = VI_TRUE;
    if (channel == tkvx4244_ANALYZE_CH9)
      gpch1 = 1;
    if (channel == tkvx4244_ANALYZE_CH10)
      gpch2 = 1;
    if (channel == tkvx4244_ANALYZE_CH11)
      gpch3 = 1;
    if (channel == tkvx4244_ANALYZE_CH12)
      gpch4 = 1;
    retError = tkvx4244_confGroupThreeChannels(instrumentHandle, gpch1,
					       gpch2, gpch3, gpch4, tkvx4244_SELECT_ENABLE);
    if (retError)
      return retError;
    retError = tkvx4244_confGpThreeVoltageRange(instrumentHandle, gpch1,
						gpch2, gpch3, gpch4, voltageRange);
    if (retError)
      return retError;
    retError = tkvx4244_confSamplingInterval(instrumentHandle,
					     tkvx4244_CLOCK_INTERNAL, intervalFrequency,
					     samplingRate, gone, gtwo, gthree, gfour,
					     tkvx4244_ALL_GROUPS_DISABLE);
    if (retError)
      return retError;
    retError = tkvx4244_confDataCollectCount(instrumentHandle, 131072, gone, gtwo, gthree, gfour,
					     tkvx4244_ALL_GROUPS_DISABLE);
    if (retError)
      return retError;
    retError = tkvx4244_initMeasurementCycle(instrumentHandle, VI_FALSE, gone, gtwo, gthree, gfour,
					     tkvx4244_ALL_GROUPS_DISABLE, tkvx4244_INIT_IMM);
    if (retError)
      return retError;
  }
  if (channel > tkvx4244_ANALYZE_CH12) {
    gfour = VI_TRUE;
    if (channel == tkvx4244_ANALYZE_CH13)
      gpch1 = 1;
    if (channel == tkvx4244_ANALYZE_CH14)
      gpch2 = 1;
    if (channel == tkvx4244_ANALYZE_CH15)
      gpch3 = 1;
    if (channel == tkvx4244_ANALYZE_CH16)
      gpch4 = 1;
    retError = tkvx4244_confGroupFourChannels(instrumentHandle, gpch1,
					      gpch2, gpch3, gpch4, tkvx4244_SELECT_ENABLE);
    if (retError)
      return retError;
    retError = tkvx4244_confGpFourVoltageRange(instrumentHandle, gpch1,
					       gpch2, gpch3, gpch4, voltageRange);
    if (retError)
      return retError;
    retError = tkvx4244_confSamplingInterval(instrumentHandle,
					     tkvx4244_CLOCK_INTERNAL, intervalFrequency,
					     samplingRate, gone, gtwo, gthree, gfour,
					     tkvx4244_ALL_GROUPS_DISABLE);
    if (retError)
      return retError;
    retError = tkvx4244_confDataCollectCount(instrumentHandle, 131072, gone, gtwo, gthree, gfour,
					     tkvx4244_ALL_GROUPS_DISABLE);
    if (retError)
      return retError;
    retError = tkvx4244_initMeasurementCycle(instrumentHandle, VI_FALSE, gone, gtwo, gthree, gfour,
					     tkvx4244_ALL_GROUPS_DISABLE, tkvx4244_INIT_IMM);
    if (retError)
      return retError;
  }
  retError = tkvx4244_readASCIIData(instrumentHandle, numberOfSamples,
				    startingAddress, channel, dataArray);
  if (retError)
    return retError;

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_confTriggerSrcAndLogic
 *
 * Purpose:
 *NOTE: This Function is an "Exception Function" and is not
 *compliant with VXIplu&play standards because of the number of
 *parameters in the function. This function is being maintained in
 *this version of the VXIplug&play driver to provide compatibility
 *with existing applications. New functions have been written to
 *replace this function.  There will not be a function panel for
 *this function in WIN Framework 4.0 compliant drivers.
 *  This function sets various trigger parameters for logic and source.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 ext,imm,thre,vxi-- ttl0,ttl1,ttl2,ttl3,ttl4,ttl5,ttl6,ttl7
 *  ViInt16 gp1,gp2,gp3,gp4,gpa
 *  ViInt16 logic
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC tkvx4244_confTriggerSrcAndLogic(ViSession instrumentHandle, ViInt16 logic,
						  ViInt16 ext, ViInt16 imm, ViInt16 thre,
						  ViInt16 vxi, ViInt16 ttl0, ViInt16 ttl1,
						  ViInt16 ttl2, ViInt16 ttl3, ViInt16 ttl4,
						  ViInt16 ttl5, ViInt16 ttl6, ViInt16 ttl7,
						  ViInt16 gp1, ViInt16 gp2, ViInt16 gp3,
						  ViInt16 gp4, ViInt16 gpa)
{
  ViChar gplst[10];
  ViChar mask[10];
  int temp;
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for VISA calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;
  /*Validate the Command */
  if ((ext + imm + thre + vxi + ttl0 + ttl1 + ttl2 + ttl3 + ttl4 + ttl5 + ttl6 + ttl7) == 0)
    return tkvx4244_ERROR_TRIG_SRC_CHOICE;
  if (gpa + gp1 + gp2 + gp3 + gp4 == VI_FALSE)
    return tkvx4244_NO_GRPS_SELECTED_ERROR;
  retError = tkvx4244_invalidShortRange(logic, 0, 1, tkvx4244_ERROR_TRIG_LOGIC);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp1, 0, 1, tkvx4244_ERROR_GP_ONE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp2, 0, 1, tkvx4244_ERROR_GP_TWO);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp3, 0, 1, tkvx4244_ERROR_GP_THREE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp4, 0, 1, tkvx4244_ERROR_GP_FOUR);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gpa, 0, 1, tkvx4244_ERROR_ALL_GROUPS);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ext, 0, 1, tkvx4244_ERROR_EXT_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(imm, 0, 1, tkvx4244_ERROR_IMM_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(thre, 0, 1, tkvx4244_ERROR_THRE_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(vxi, 0, 1, tkvx4244_ERROR_VXI_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl0, 0, 1, tkvx4244_ERROR_TTL0_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl1, 0, 1, tkvx4244_ERROR_TTL1_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl2, 0, 1, tkvx4244_ERROR_TTL2_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl3, 0, 1, tkvx4244_ERROR_TTL3_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl4, 0, 1, tkvx4244_ERROR_TTL4_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl5, 0, 1, tkvx4244_ERROR_TTL5_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl6, 0, 1, tkvx4244_ERROR_TTL6_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl7, 0, 1, tkvx4244_ERROR_TTL7_TRIG);
  if (retError)
    return retError;

  mask[0] = 48;
  if (imm == 1)
    imm = 2;
  if (thre == 1)
    thre = 4;
  if (vxi == 1)
    vxi = 8;
  temp = ext + imm + thre + vxi;
  if (!temp)
    mask[1] = 48;
  if (temp == 1)
    mask[1] = 49;
  if (temp == 2)
    mask[1] = 50;
  if (temp == 3)
    mask[1] = 51;
  if (temp == 4)
    mask[1] = 52;
  if (temp == 5)
    mask[1] = 53;
  if (temp == 6)
    mask[1] = 54;
  if (temp == 7)
    mask[1] = 55;
  if (temp == 8)
    mask[1] = 56;
  if (temp == 9)
    mask[1] = 57;
  if (temp == 10)
    mask[1] = 65;
  if (temp == 11)
    mask[1] = 66;
  if (temp == 12)
    mask[1] = 67;
  if (temp == 13)
    mask[1] = 68;
  if (temp == 14)
    mask[1] = 69;
  if (temp == 15)
    mask[1] = 70;
  if (ttl5 == 1)
    ttl5 = 2;
  if (ttl6 == 1)
    ttl6 = 4;
  if (ttl7 == 1)
    ttl7 = 8;
  temp = ttl4 + ttl5 + ttl6 + ttl7;
  if (!temp)
    mask[2] = 48;
  if (temp == 1)
    mask[2] = 49;
  if (temp == 2)
    mask[2] = 50;
  if (temp == 3)
    mask[2] = 51;
  if (temp == 4)
    mask[2] = 52;
  if (temp == 5)
    mask[2] = 53;
  if (temp == 6)
    mask[2] = 54;
  if (temp == 7)
    mask[2] = 55;
  if (temp == 8)
    mask[2] = 56;
  if (temp == 9)
    mask[2] = 57;
  if (temp == 10)
    mask[2] = 65;
  if (temp == 11)
    mask[2] = 66;
  if (temp == 12)
    mask[2] = 67;
  if (temp == 13)
    mask[2] = 68;
  if (temp == 14)
    mask[2] = 69;
  if (temp == 15)
    mask[2] = 70;
  if (ttl1 == 1)
    ttl1 = 2;
  if (ttl2 == 1)
    ttl2 = 4;
  if (ttl3 == 1)
    ttl3 = 8;
  temp = ttl0 + ttl1 + ttl2 + ttl3;
  if (!temp)
    mask[3] = 48;
  if (temp == 1)
    mask[3] = 49;
  if (temp == 2)
    mask[3] = 50;
  if (temp == 3)
    mask[3] = 51;
  if (temp == 4)
    mask[3] = 52;
  if (temp == 5)
    mask[3] = 53;
  if (temp == 6)
    mask[3] = 54;
  if (temp == 7)
    mask[3] = 55;
  if (temp == 8)
    mask[3] = 56;
  if (temp == 9)
    mask[3] = 57;
  if (temp == 10)
    mask[3] = 65;
  if (temp == 11)
    mask[3] = 66;
  if (temp == 12)
    mask[3] = 67;
  if (temp == 13)
    mask[3] = 68;
  if (temp == 14)
    mask[3] = 69;
  if (temp == 15)
    mask[3] = 70;
  mask[4] = '\0';
  /* Parse the gp list */
  tkvx4244_gplist_parse(gp1, gp2, gp3, gp4, gpa, gplst);
  /* Build the command string */
  sprintf(cmd, "TRIG:%s (%s)\n", trgsrcL[logic], gplst);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Build the command string */
  sprintf(cmd, "TRIG:MASK %s (%s)\n", mask, gplst);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_confGpOneTrigSrcLogic
 *
 * Purpose:
 *  This function sets various trigger parameters for logic 
 *  and source for Group 1.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to 
 *  access instrument specific data. Must be initialized by 
 *  tkvx4244_init() prior to use.
 *  ViInt16 external,immediate,threshold,
 *  vxicmd-- ttl0,ttl1,ttl2,ttl3,ttl4,ttl5,ttl6,ttl7:
 *  ViInt16 logic:
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_confGpOneTrigSrcLogic(ViSession instrumentHandle,
			       ViInt16 logic, ViInt16 external, ViInt16 immediate,
			       ViInt16 threshold, ViInt16 vxicmd, ViInt16 ttl0, ViInt16 ttl1,
			       ViInt16 ttl2, ViInt16 ttl3, ViInt16 ttl4, ViInt16 ttl5, ViInt16 ttl6,
			       ViInt16 ttl7)
{
  ViChar mask[10];
  int temp;
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for VISA calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;
  /*Validate the Command */
  if ((external + immediate + threshold + vxicmd + ttl0 + ttl1 + ttl2 + ttl3 + ttl4 + ttl5 + ttl6 +
       ttl7) == 0)
    return tkvx4244_ERROR_TRIG_SRC_CHOICE;
  retError = tkvx4244_invalidShortRange(logic, tkvx4244_TRIGGER_LOGIC_AND,
					tkvx4244_TRIGGER_LOGIC_OR, tkvx4244_ERROR_TRIG_LOGIC);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(external, tkvx4244_TRIGGER_EXT_DISABLE,
					tkvx4244_TRIGGER_EXT_ENABLE, tkvx4244_ERROR_EXT_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(immediate, tkvx4244_TRIGGER_IMM_DISABLE,
					tkvx4244_TRIGGER_IMM_ENABLE, tkvx4244_ERROR_IMM_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(threshold, tkvx4244_TRIGGER_THRESH_DISABLE,
					tkvx4244_TRIGGER_THRESH_ENABLE, tkvx4244_ERROR_THRE_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(vxicmd, tkvx4244_TRIGGER_VXI_DISABLE,
					tkvx4244_TRIGGER_VXI_ENABLE, tkvx4244_ERROR_VXI_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl0, tkvx4244_TRIGGER_TTL0_DISABLE,
					tkvx4244_TRIGGER_TTL0_ENABLE, tkvx4244_ERROR_TTL0_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl1, tkvx4244_TRIGGER_TTL1_DISABLE,
					tkvx4244_TRIGGER_TTL1_ENABLE, tkvx4244_ERROR_TTL1_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl2, tkvx4244_TRIGGER_TTL2_DISABLE,
					tkvx4244_TRIGGER_TTL2_ENABLE, tkvx4244_ERROR_TTL2_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl3, tkvx4244_TRIGGER_TTL3_DISABLE,
					tkvx4244_TRIGGER_TTL3_ENABLE, tkvx4244_ERROR_TTL3_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl4, tkvx4244_TRIGGER_TTL4_DISABLE,
					tkvx4244_TRIGGER_TTL4_ENABLE, tkvx4244_ERROR_TTL4_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl5, tkvx4244_TRIGGER_TTL5_DISABLE,
					tkvx4244_TRIGGER_TTL5_ENABLE, tkvx4244_ERROR_TTL5_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl6, tkvx4244_TRIGGER_TTL6_DISABLE,
					tkvx4244_TRIGGER_TTL6_ENABLE, tkvx4244_ERROR_TTL6_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl7, tkvx4244_TRIGGER_TTL7_DISABLE,
					tkvx4244_TRIGGER_TTL7_ENABLE, tkvx4244_ERROR_TTL7_TRIG);
  if (retError)
    return retError;

  mask[0] = 48;
  if (immediate == 1)
    immediate = 2;
  if (threshold == 1)
    threshold = 4;
  if (vxicmd == 1)
    vxicmd = 8;
  temp = external + immediate + threshold + vxicmd;
  if (!temp)
    mask[1] = 48;
  if (temp == 1)
    mask[1] = 49;
  if (temp == 2)
    mask[1] = 50;
  if (temp == 3)
    mask[1] = 51;
  if (temp == 4)
    mask[1] = 52;
  if (temp == 5)
    mask[1] = 53;
  if (temp == 6)
    mask[1] = 54;
  if (temp == 7)
    mask[1] = 55;
  if (temp == 8)
    mask[1] = 56;
  if (temp == 9)
    mask[1] = 57;
  if (temp == 10)
    mask[1] = 65;
  if (temp == 11)
    mask[1] = 66;
  if (temp == 12)
    mask[1] = 67;
  if (temp == 13)
    mask[1] = 68;
  if (temp == 14)
    mask[1] = 69;
  if (temp == 15)
    mask[1] = 70;
  if (ttl5 == 1)
    ttl5 = 2;
  if (ttl6 == 1)
    ttl6 = 4;
  if (ttl7 == 1)
    ttl7 = 8;
  temp = ttl4 + ttl5 + ttl6 + ttl7;
  if (!temp)
    mask[2] = 48;
  if (temp == 1)
    mask[2] = 49;
  if (temp == 2)
    mask[2] = 50;
  if (temp == 3)
    mask[2] = 51;
  if (temp == 4)
    mask[2] = 52;
  if (temp == 5)
    mask[2] = 53;
  if (temp == 6)
    mask[2] = 54;
  if (temp == 7)
    mask[2] = 55;
  if (temp == 8)
    mask[2] = 56;
  if (temp == 9)
    mask[2] = 57;
  if (temp == 10)
    mask[2] = 65;
  if (temp == 11)
    mask[2] = 66;
  if (temp == 12)
    mask[2] = 67;
  if (temp == 13)
    mask[2] = 68;
  if (temp == 14)
    mask[2] = 69;
  if (temp == 15)
    mask[2] = 70;
  if (ttl1 == 1)
    ttl1 = 2;
  if (ttl2 == 1)
    ttl2 = 4;
  if (ttl3 == 1)
    ttl3 = 8;
  temp = ttl0 + ttl1 + ttl2 + ttl3;
  if (!temp)
    mask[3] = 48;
  if (temp == 1)
    mask[3] = 49;
  if (temp == 2)
    mask[3] = 50;
  if (temp == 3)
    mask[3] = 51;
  if (temp == 4)
    mask[3] = 52;
  if (temp == 5)
    mask[3] = 53;
  if (temp == 6)
    mask[3] = 54;
  if (temp == 7)
    mask[3] = 55;
  if (temp == 8)
    mask[3] = 56;
  if (temp == 9)
    mask[3] = 57;
  if (temp == 10)
    mask[3] = 65;
  if (temp == 11)
    mask[3] = 66;
  if (temp == 12)
    mask[3] = 67;
  if (temp == 13)
    mask[3] = 68;
  if (temp == 14)
    mask[3] = 69;
  if (temp == 15)
    mask[3] = 70;
  mask[4] = '\0';
  /* Build the command string */
  sprintf(cmd, "TRIG:%s (@1)\n", trgsrcL[logic]);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Build the command string */
  sprintf(cmd, "TRIG:MASK %s (@1)\n", mask);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_confGpTwoTrigSrcLogic
 *
 * Purpose:
 *  This function sets various trigger parameters for logic 
 *  and source for Group 2.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 external,immediate,threshold,vxicmd-- ttl0,ttl1,ttl2,ttl3,ttl4,ttl5,ttl6,ttl7
 *  ViInt16 logic
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_confGpTwoTrigSrcLogic(ViSession instrumentHandle,
			       ViInt16 logic, ViInt16 external, ViInt16 immediate,
			       ViInt16 threshold, ViInt16 vxicmd, ViInt16 ttl0, ViInt16 ttl1,
			       ViInt16 ttl2, ViInt16 ttl3, ViInt16 ttl4, ViInt16 ttl5, ViInt16 ttl6,
			       ViInt16 ttl7)
{
  ViChar mask[10];
  int temp;
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for VISA calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;
  /*Validate the Command */
  if ((external + immediate + threshold + vxicmd + ttl0 + ttl1 + ttl2 + ttl3 + ttl4 + ttl5 + ttl6 +
       ttl7) == 0)
    return tkvx4244_ERROR_TRIG_SRC_CHOICE;
  retError = tkvx4244_invalidShortRange(logic, tkvx4244_TRIGGER_LOGIC_AND,
					tkvx4244_TRIGGER_LOGIC_OR, tkvx4244_ERROR_TRIG_LOGIC);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(external, tkvx4244_TRIGGER_EXT_DISABLE,
					tkvx4244_TRIGGER_EXT_ENABLE, tkvx4244_ERROR_EXT_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(immediate, tkvx4244_TRIGGER_IMM_DISABLE,
					tkvx4244_TRIGGER_IMM_ENABLE, tkvx4244_ERROR_IMM_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(threshold, tkvx4244_TRIGGER_THRESH_DISABLE,
					tkvx4244_TRIGGER_THRESH_ENABLE, tkvx4244_ERROR_THRE_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(vxicmd, tkvx4244_TRIGGER_VXI_DISABLE,
					tkvx4244_TRIGGER_VXI_ENABLE, tkvx4244_ERROR_VXI_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl0, tkvx4244_TRIGGER_TTL0_DISABLE,
					tkvx4244_TRIGGER_TTL0_ENABLE, tkvx4244_ERROR_TTL0_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl1, tkvx4244_TRIGGER_TTL1_DISABLE,
					tkvx4244_TRIGGER_TTL1_ENABLE, tkvx4244_ERROR_TTL1_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl2, tkvx4244_TRIGGER_TTL2_DISABLE,
					tkvx4244_TRIGGER_TTL2_ENABLE, tkvx4244_ERROR_TTL2_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl3, tkvx4244_TRIGGER_TTL3_DISABLE,
					tkvx4244_TRIGGER_TTL3_ENABLE, tkvx4244_ERROR_TTL3_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl4, tkvx4244_TRIGGER_TTL4_DISABLE,
					tkvx4244_TRIGGER_TTL4_ENABLE, tkvx4244_ERROR_TTL4_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl5, tkvx4244_TRIGGER_TTL5_DISABLE,
					tkvx4244_TRIGGER_TTL5_ENABLE, tkvx4244_ERROR_TTL5_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl6, tkvx4244_TRIGGER_TTL6_DISABLE,
					tkvx4244_TRIGGER_TTL6_ENABLE, tkvx4244_ERROR_TTL6_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl7, tkvx4244_TRIGGER_TTL7_DISABLE,
					tkvx4244_TRIGGER_TTL7_ENABLE, tkvx4244_ERROR_TTL7_TRIG);
  if (retError)
    return retError;

  mask[0] = 48;
  if (immediate == 1)
    immediate = 2;
  if (threshold == 1)
    threshold = 4;
  if (vxicmd == 1)
    vxicmd = 8;
  temp = external + immediate + threshold + vxicmd;
  if (!temp)
    mask[1] = 48;
  if (temp == 1)
    mask[1] = 49;
  if (temp == 2)
    mask[1] = 50;
  if (temp == 3)
    mask[1] = 51;
  if (temp == 4)
    mask[1] = 52;
  if (temp == 5)
    mask[1] = 53;
  if (temp == 6)
    mask[1] = 54;
  if (temp == 7)
    mask[1] = 55;
  if (temp == 8)
    mask[1] = 56;
  if (temp == 9)
    mask[1] = 57;
  if (temp == 10)
    mask[1] = 65;
  if (temp == 11)
    mask[1] = 66;
  if (temp == 12)
    mask[1] = 67;
  if (temp == 13)
    mask[1] = 68;
  if (temp == 14)
    mask[1] = 69;
  if (temp == 15)
    mask[1] = 70;
  if (ttl5 == 1)
    ttl5 = 2;
  if (ttl6 == 1)
    ttl6 = 4;
  if (ttl7 == 1)
    ttl7 = 8;
  temp = ttl4 + ttl5 + ttl6 + ttl7;
  if (!temp)
    mask[2] = 48;
  if (temp == 1)
    mask[2] = 49;
  if (temp == 2)
    mask[2] = 50;
  if (temp == 3)
    mask[2] = 51;
  if (temp == 4)
    mask[2] = 52;
  if (temp == 5)
    mask[2] = 53;
  if (temp == 6)
    mask[2] = 54;
  if (temp == 7)
    mask[2] = 55;
  if (temp == 8)
    mask[2] = 56;
  if (temp == 9)
    mask[2] = 57;
  if (temp == 10)
    mask[2] = 65;
  if (temp == 11)
    mask[2] = 66;
  if (temp == 12)
    mask[2] = 67;
  if (temp == 13)
    mask[2] = 68;
  if (temp == 14)
    mask[2] = 69;
  if (temp == 15)
    mask[2] = 70;
  if (ttl1 == 1)
    ttl1 = 2;
  if (ttl2 == 1)
    ttl2 = 4;
  if (ttl3 == 1)
    ttl3 = 8;
  temp = ttl0 + ttl1 + ttl2 + ttl3;
  if (!temp)
    mask[3] = 48;
  if (temp == 1)
    mask[3] = 49;
  if (temp == 2)
    mask[3] = 50;
  if (temp == 3)
    mask[3] = 51;
  if (temp == 4)
    mask[3] = 52;
  if (temp == 5)
    mask[3] = 53;
  if (temp == 6)
    mask[3] = 54;
  if (temp == 7)
    mask[3] = 55;
  if (temp == 8)
    mask[3] = 56;
  if (temp == 9)
    mask[3] = 57;
  if (temp == 10)
    mask[3] = 65;
  if (temp == 11)
    mask[3] = 66;
  if (temp == 12)
    mask[3] = 67;
  if (temp == 13)
    mask[3] = 68;
  if (temp == 14)
    mask[3] = 69;
  if (temp == 15)
    mask[3] = 70;
  mask[4] = '\0';
  /* Build the command string */
  sprintf(cmd, "TRIG:%s (@2)\n", trgsrcL[logic]);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Build the command string */
  sprintf(cmd, "TRIG:MASK %s (@2)\n", mask);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_confGpThreeTrigSrcLogic
 *
 * Purpose:
 *  This function sets various trigger parameters for logic 
 *  and source for Group 3.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 external,immediate,threshold,vxicmd-- ttl0,ttl1,ttl2,ttl3,ttl4,ttl5,ttl6,ttl7
 *  ViInt16 logic
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_confGpThreeTrigSrcLogic(ViSession instrumentHandle,
				 ViInt16 logic, ViInt16 external, ViInt16 immediate,
				 ViInt16 threshold, ViInt16 vxicmd, ViInt16 ttl0, ViInt16 ttl1,
				 ViInt16 ttl2, ViInt16 ttl3, ViInt16 ttl4, ViInt16 ttl5,
				 ViInt16 ttl6, ViInt16 ttl7)
{
  ViChar mask[10];
  int temp;
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;
  /*Validate the Command */
  if ((external + immediate + threshold + vxicmd + ttl0 + ttl1 + ttl2 + ttl3 + ttl4 + ttl5 + ttl6 +
       ttl7) == 0)
    return tkvx4244_ERROR_TRIG_SRC_CHOICE;
  retError = tkvx4244_invalidShortRange(logic, tkvx4244_TRIGGER_LOGIC_AND,
					tkvx4244_TRIGGER_LOGIC_OR, tkvx4244_ERROR_TRIG_LOGIC);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(external, tkvx4244_TRIGGER_EXT_DISABLE,
					tkvx4244_TRIGGER_EXT_ENABLE, tkvx4244_ERROR_EXT_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(immediate, tkvx4244_TRIGGER_IMM_DISABLE,
					tkvx4244_TRIGGER_IMM_ENABLE, tkvx4244_ERROR_IMM_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(threshold, tkvx4244_TRIGGER_THRESH_DISABLE,
					tkvx4244_TRIGGER_THRESH_ENABLE, tkvx4244_ERROR_THRE_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(vxicmd, tkvx4244_TRIGGER_VXI_DISABLE,
					tkvx4244_TRIGGER_VXI_ENABLE, tkvx4244_ERROR_VXI_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl0, tkvx4244_TRIGGER_TTL0_DISABLE,
					tkvx4244_TRIGGER_TTL0_ENABLE, tkvx4244_ERROR_TTL0_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl1, tkvx4244_TRIGGER_TTL1_DISABLE,
					tkvx4244_TRIGGER_TTL1_ENABLE, tkvx4244_ERROR_TTL1_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl2, tkvx4244_TRIGGER_TTL2_DISABLE,
					tkvx4244_TRIGGER_TTL2_ENABLE, tkvx4244_ERROR_TTL2_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl3, tkvx4244_TRIGGER_TTL3_DISABLE,
					tkvx4244_TRIGGER_TTL3_ENABLE, tkvx4244_ERROR_TTL3_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl4, tkvx4244_TRIGGER_TTL4_DISABLE,
					tkvx4244_TRIGGER_TTL4_ENABLE, tkvx4244_ERROR_TTL4_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl5, tkvx4244_TRIGGER_TTL5_DISABLE,
					tkvx4244_TRIGGER_TTL5_ENABLE, tkvx4244_ERROR_TTL5_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl6, tkvx4244_TRIGGER_TTL6_DISABLE,
					tkvx4244_TRIGGER_TTL6_ENABLE, tkvx4244_ERROR_TTL6_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl7, tkvx4244_TRIGGER_TTL7_DISABLE,
					tkvx4244_TRIGGER_TTL7_ENABLE, tkvx4244_ERROR_TTL7_TRIG);
  if (retError)
    return retError;

  mask[0] = 48;
  if (immediate == 1)
    immediate = 2;
  if (threshold == 1)
    threshold = 4;
  if (vxicmd == 1)
    vxicmd = 8;
  temp = external + immediate + threshold + vxicmd;
  if (!temp)
    mask[1] = 48;
  if (temp == 1)
    mask[1] = 49;
  if (temp == 2)
    mask[1] = 50;
  if (temp == 3)
    mask[1] = 51;
  if (temp == 4)
    mask[1] = 52;
  if (temp == 5)
    mask[1] = 53;
  if (temp == 6)
    mask[1] = 54;
  if (temp == 7)
    mask[1] = 55;
  if (temp == 8)
    mask[1] = 56;
  if (temp == 9)
    mask[1] = 57;
  if (temp == 10)
    mask[1] = 65;
  if (temp == 11)
    mask[1] = 66;
  if (temp == 12)
    mask[1] = 67;
  if (temp == 13)
    mask[1] = 68;
  if (temp == 14)
    mask[1] = 69;
  if (temp == 15)
    mask[1] = 70;
  if (ttl5 == 1)
    ttl5 = 2;
  if (ttl6 == 1)
    ttl6 = 4;
  if (ttl7 == 1)
    ttl7 = 8;
  temp = ttl4 + ttl5 + ttl6 + ttl7;
  if (!temp)
    mask[2] = 48;
  if (temp == 1)
    mask[2] = 49;
  if (temp == 2)
    mask[2] = 50;
  if (temp == 3)
    mask[2] = 51;
  if (temp == 4)
    mask[2] = 52;
  if (temp == 5)
    mask[2] = 53;
  if (temp == 6)
    mask[2] = 54;
  if (temp == 7)
    mask[2] = 55;
  if (temp == 8)
    mask[2] = 56;
  if (temp == 9)
    mask[2] = 57;
  if (temp == 10)
    mask[2] = 65;
  if (temp == 11)
    mask[2] = 66;
  if (temp == 12)
    mask[2] = 67;
  if (temp == 13)
    mask[2] = 68;
  if (temp == 14)
    mask[2] = 69;
  if (temp == 15)
    mask[2] = 70;
  if (ttl1 == 1)
    ttl1 = 2;
  if (ttl2 == 1)
    ttl2 = 4;
  if (ttl3 == 1)
    ttl3 = 8;
  temp = ttl0 + ttl1 + ttl2 + ttl3;
  if (!temp)
    mask[3] = 48;
  if (temp == 1)
    mask[3] = 49;
  if (temp == 2)
    mask[3] = 50;
  if (temp == 3)
    mask[3] = 51;
  if (temp == 4)
    mask[3] = 52;
  if (temp == 5)
    mask[3] = 53;
  if (temp == 6)
    mask[3] = 54;
  if (temp == 7)
    mask[3] = 55;
  if (temp == 8)
    mask[3] = 56;
  if (temp == 9)
    mask[3] = 57;
  if (temp == 10)
    mask[3] = 65;
  if (temp == 11)
    mask[3] = 66;
  if (temp == 12)
    mask[3] = 67;
  if (temp == 13)
    mask[3] = 68;
  if (temp == 14)
    mask[3] = 69;
  if (temp == 15)
    mask[3] = 70;
  mask[4] = '\0';
  /* Build the command string */
  sprintf(cmd, "TRIG:%s (@3)\n", trgsrcL[logic]);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Build the command string */
  sprintf(cmd, "TRIG:MASK %s (@3)\n", mask);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_confGpFourTrigSrcLogic
 *
 * Purpose:
 *  This function sets various trigger parameters for logic 
 *  and source for Group 4.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 external,immediate,threshold,vxicmd-- ttl0,ttl1,ttl2,ttl3,ttl4,ttl5,ttl6,ttl7
 *  ViInt16 logic
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_confGpFourTrigSrcLogic(ViSession instrumentHandle,
				ViInt16 logic, ViInt16 external, ViInt16 immediate,
				ViInt16 threshold, ViInt16 vxicmd, ViInt16 ttl0, ViInt16 ttl1,
				ViInt16 ttl2, ViInt16 ttl3, ViInt16 ttl4, ViInt16 ttl5,
				ViInt16 ttl6, ViInt16 ttl7)
{
  ViChar mask[10];
  int temp;
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;
  /*Validate the Command */
  if ((external + immediate + threshold + vxicmd + ttl0 + ttl1 + ttl2 + ttl3 + ttl4 + ttl5 + ttl6 +
       ttl7) == 0)
    return tkvx4244_ERROR_TRIG_SRC_CHOICE;
  retError = tkvx4244_invalidShortRange(logic, tkvx4244_TRIGGER_LOGIC_AND,
					tkvx4244_TRIGGER_LOGIC_OR, tkvx4244_ERROR_TRIG_LOGIC);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(external, tkvx4244_TRIGGER_EXT_DISABLE,
					tkvx4244_TRIGGER_EXT_ENABLE, tkvx4244_ERROR_EXT_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(immediate, tkvx4244_TRIGGER_IMM_DISABLE,
					tkvx4244_TRIGGER_IMM_ENABLE, tkvx4244_ERROR_IMM_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(threshold, tkvx4244_TRIGGER_THRESH_DISABLE,
					tkvx4244_TRIGGER_THRESH_ENABLE, tkvx4244_ERROR_THRE_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(vxicmd, tkvx4244_TRIGGER_VXI_DISABLE,
					tkvx4244_TRIGGER_VXI_ENABLE, tkvx4244_ERROR_VXI_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl0, tkvx4244_TRIGGER_TTL0_DISABLE,
					tkvx4244_TRIGGER_TTL0_ENABLE, tkvx4244_ERROR_TTL0_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl1, tkvx4244_TRIGGER_TTL1_DISABLE,
					tkvx4244_TRIGGER_TTL1_ENABLE, tkvx4244_ERROR_TTL1_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl2, tkvx4244_TRIGGER_TTL2_DISABLE,
					tkvx4244_TRIGGER_TTL2_ENABLE, tkvx4244_ERROR_TTL2_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl3, tkvx4244_TRIGGER_TTL3_DISABLE,
					tkvx4244_TRIGGER_TTL3_ENABLE, tkvx4244_ERROR_TTL3_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl4, tkvx4244_TRIGGER_TTL4_DISABLE,
					tkvx4244_TRIGGER_TTL4_ENABLE, tkvx4244_ERROR_TTL4_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl5, tkvx4244_TRIGGER_TTL5_DISABLE,
					tkvx4244_TRIGGER_TTL5_ENABLE, tkvx4244_ERROR_TTL5_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl6, tkvx4244_TRIGGER_TTL6_DISABLE,
					tkvx4244_TRIGGER_TTL6_ENABLE, tkvx4244_ERROR_TTL6_TRIG);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttl7, tkvx4244_TRIGGER_TTL7_DISABLE,
					tkvx4244_TRIGGER_TTL7_ENABLE, tkvx4244_ERROR_TTL7_TRIG);
  if (retError)
    return retError;

  mask[0] = 48;
  if (immediate == 1)
    immediate = 2;
  if (threshold == 1)
    threshold = 4;
  if (vxicmd == 1)
    vxicmd = 8;
  temp = external + immediate + threshold + vxicmd;
  if (!temp)
    mask[1] = 48;
  if (temp == 1)
    mask[1] = 49;
  if (temp == 2)
    mask[1] = 50;
  if (temp == 3)
    mask[1] = 51;
  if (temp == 4)
    mask[1] = 52;
  if (temp == 5)
    mask[1] = 53;
  if (temp == 6)
    mask[1] = 54;
  if (temp == 7)
    mask[1] = 55;
  if (temp == 8)
    mask[1] = 56;
  if (temp == 9)
    mask[1] = 57;
  if (temp == 10)
    mask[1] = 65;
  if (temp == 11)
    mask[1] = 66;
  if (temp == 12)
    mask[1] = 67;
  if (temp == 13)
    mask[1] = 68;
  if (temp == 14)
    mask[1] = 69;
  if (temp == 15)
    mask[1] = 70;
  if (ttl5 == 1)
    ttl5 = 2;
  if (ttl6 == 1)
    ttl6 = 4;
  if (ttl7 == 1)
    ttl7 = 8;
  temp = ttl4 + ttl5 + ttl6 + ttl7;
  if (!temp)
    mask[2] = 48;
  if (temp == 1)
    mask[2] = 49;
  if (temp == 2)
    mask[2] = 50;
  if (temp == 3)
    mask[2] = 51;
  if (temp == 4)
    mask[2] = 52;
  if (temp == 5)
    mask[2] = 53;
  if (temp == 6)
    mask[2] = 54;
  if (temp == 7)
    mask[2] = 55;
  if (temp == 8)
    mask[2] = 56;
  if (temp == 9)
    mask[2] = 57;
  if (temp == 10)
    mask[2] = 65;
  if (temp == 11)
    mask[2] = 66;
  if (temp == 12)
    mask[2] = 67;
  if (temp == 13)
    mask[2] = 68;
  if (temp == 14)
    mask[2] = 69;
  if (temp == 15)
    mask[2] = 70;
  if (ttl1 == 1)
    ttl1 = 2;
  if (ttl2 == 1)
    ttl2 = 4;
  if (ttl3 == 1)
    ttl3 = 8;
  temp = ttl0 + ttl1 + ttl2 + ttl3;
  if (!temp)
    mask[3] = 48;
  if (temp == 1)
    mask[3] = 49;
  if (temp == 2)
    mask[3] = 50;
  if (temp == 3)
    mask[3] = 51;
  if (temp == 4)
    mask[3] = 52;
  if (temp == 5)
    mask[3] = 53;
  if (temp == 6)
    mask[3] = 54;
  if (temp == 7)
    mask[3] = 55;
  if (temp == 8)
    mask[3] = 56;
  if (temp == 9)
    mask[3] = 57;
  if (temp == 10)
    mask[3] = 65;
  if (temp == 11)
    mask[3] = 66;
  if (temp == 12)
    mask[3] = 67;
  if (temp == 13)
    mask[3] = 68;
  if (temp == 14)
    mask[3] = 69;
  if (temp == 15)
    mask[3] = 70;
  mask[4] = '\0';
  /* Build the command string */
  sprintf(cmd, "TRIG:%s (@4)\n", trgsrcL[logic]);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Build the command string */
  sprintf(cmd, "TRIG:MASK %s (@4)\n", mask);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_confSynchronousClocks
 *
 * Purpose:
 *  This function synchronizes the clocks for the channel groups.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 source, ttlSource
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC tkvx4244_confSynchronousClocks(ViSession instrumentHandle,
						 ViInt16 source, ViInt16 ttlSource)
{
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;
  /*Validate the Command */
  retError = tkvx4244_invalidShortRange(source, tkvx4244_CLK_SYNC_POS,
					tkvx4244_CLK_SYNC_IMM, tkvx4244_ERROR_SYNC_CLK_SRC);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ttlSource, tkvx4244_CLK_SYNC_TTLOFF,
					tkvx4244_CLK_SYNC_TTL7, tkvx4244_ERROR_SYNC_CLK_DEF);
  if (retError)
    return retError;
  /* Build the command string */
  sprintf(cmd, "FREQ:SYNC:%s\n", clksynch[source]);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Build the command string */
  sprintf(cmd, "FREQ:SYNC:%s\n", clksynch[ttlSource]);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_readASCIIData
 *
 * Purpose:
 *  This function allows retrieval of raw data in ASCII.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt32 startingAddress
 *  ViInt16 numberOfSamples, channel
 *  ViChar dataArray
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC tkvx4244_readASCIIData(ViSession instrumentHandle,
					 ViInt16 numberOfSamples, ViInt32 startingAddress,
					 ViInt16 channel, ViChar dataArray[])
{
  static char rdd[12288];
  static int dtt[1000];
  time_t delaytime;
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate Command */
  retError = tkvx4244_invalidShortRange(numberOfSamples, VI_TRUE, 1000, tkvx4244_ERROR_SAMP_CNT);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(channel, tkvx4244_ANALYZE_CH1,
					tkvx4244_ANALYZE_CH16, tkvx4244_ERROR_FETCH_CHL);
  if (retError)
    return retError;
  retError = tkvx4244_invalidLongRange(startingAddress, -262044L,
				       262144L, tkvx4244_ERROR_BEGIN_MEM);
  if (retError)
    return retError;
  if (numberOfSamples < 200)
    delaytime = 1;
  if ((numberOfSamples >= 200) && (numberOfSamples < 300))
    delaytime = 3;
  if ((numberOfSamples >= 300) && (numberOfSamples < 400))
    delaytime = 4;
  if ((numberOfSamples >= 400) && (numberOfSamples < 500))
    delaytime = 6;

  if ((numberOfSamples >= 500) && (numberOfSamples < 700))
    delaytime = 9;
  if (numberOfSamples > 700)
    delaytime = 12;
  /* Build the command string */
  sprintf(cmd, "FETCH:DAT?%hd,%ld(@%hd)\n", numberOfSamples, startingAddress, channel);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  tkvx4244_sleep(delaytime);
  /* Get the response from the instrument */
  retError = viRead(instrumentHandle, (ViPBuf) rdd, 12288, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  strcpy(dataArray, rdd);

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_getBinaryData
 *
 * Purpose:
 *  This function allows retrieval of raw data in Binary.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 numberOfSamples:
 *  ViInt32 startingAddress:
 *  ViInt16 channel:
 *  ViInt16 response[]:
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC tkvx4244_getBinaryData(ViSession instrumentHandle,
					 ViInt16 numberOfSamples, ViInt32 startingAddress,
					 ViInt16 channel, ViInt16 response[])
{
  ViInt16 btcnt;
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  ViPChar pData;
  ViInt32 i, j;
  ViChar redata;

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate Command */
  retError = tkvx4244_invalidShortRange(numberOfSamples, 1, 5000, tkvx4244_ERROR_SAMP_CNT);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(channel, tkvx4244_ANALYZE_CH1,
					tkvx4244_ANALYZE_CH16, tkvx4244_ERROR_FETCH_CHL);
  if (retError)
    return retError;
  retError = tkvx4244_invalidLongRange(startingAddress, -262044L,
				       262144L, tkvx4244_ERROR_BEGIN_MEM);
  if (retError)
    return retError;
  /* Build the command string */
  sprintf(cmd, "FETCH:BIN?%hd,%ld (@%hd)\n", numberOfSamples, startingAddress, channel);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;

  /* Get byte count and malloc buffer to fill binary data with */
  btcnt = 2 * numberOfSamples;

  /* Get the response from the instrument */
  viSetAttribute(instrumentHandle, VI_ATTR_TERMCHAR_EN, VI_FALSE);
  retError = viRead(instrumentHandle, (ViPBuf) response, btcnt, (ViPUInt32) & retCnt);

  retCnt /= 2;
  pData = (char *)response;	/* short(2Byte) array data -> char(1Byte) array */
  for (i = 0; i < retCnt; i++) {
    j = 2 * i;
    redata = pData[j];
    pData[j] = pData[j + 1];
    pData[j + 1] = redata;
  }
  if (retError < VI_SUCCESS)
    return retError;

  return VI_SUCCESS;

}

/***************************************************************************
 * Function: tkvx4244_getBinaryDataSFP
 *
 * Purpose:
 *  This function allows retrieval of raw data in Binary using ViInt16 array.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 numberOfSamples:
 *  ViInt32 startingAddress:
 *  ViInt16 channel:
 *  ViInt16 response[]:
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC tkvx4244_getBinaryDataSFP(ViSession instrumentHandle,
					    ViInt16 numberOfSamples, ViInt32 startingAddress,
					    ViInt16 channel, ViInt16 response[])
{
  ViInt16 btcnt;
  ViInt32 ii;
  static ViChar *rdd;
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate Command */
  retError = tkvx4244_invalidShortRange(numberOfSamples, 1, 5000, tkvx4244_ERROR_SAMP_CNT);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(channel, tkvx4244_ANALYZE_CH1,
					tkvx4244_ANALYZE_CH16, tkvx4244_ERROR_FETCH_CHL);
  if (retError)
    return retError;
  retError = tkvx4244_invalidLongRange(startingAddress, -262044L,
				       262144L, tkvx4244_ERROR_BEGIN_MEM);
  if (retError)
    return retError;
  /* Build the command string */
  sprintf(cmd, "FETCH:BIN?%hd,%ld (@%hd)\n", numberOfSamples, startingAddress, channel);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;

  /* Get byte count and malloc buffer to fill binary data with */
  btcnt = 2 * numberOfSamples;

  rdd = (ViChar *) malloc(btcnt + 10);

  /* Get the response from the instrument */
  viSetAttribute(instrumentHandle, VI_ATTR_TERMCHAR_EN, VI_FALSE);
  retError = viRead(instrumentHandle, (ViPBuf) rdd, btcnt, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS) {
    free(rdd);
    return retError;
  }

  for (ii = 0; ii < retCnt / 2; ii++)
    response[ii] = ((rdd[2 * ii]) << 8) + rdd[(2 * ii) + 1];
  /* Free rdd */
  free(rdd);

  return VI_SUCCESS;

}

/***************************************************************************
 * Function: tkvx4244_readBinaryData
 * NOTE:This function has been replaced by tkvx4244_getBinaryData. The use 
 * of a ViChar array to transfer data causes major problems when using
 * LabView. Therefore, this function is being kept for backward compatibility
 * only.
 * Purpose:
 *  This function allows retrieval of raw data in Binary.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 numberOfSamples:
 *  ViInt32 startingAddress:
 *  ViInt16 channel:
 *  ViChar response[]:
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_readBinaryData(ViSession instrumentHandle, ViInt16 numberOfSamples,
			ViInt32 startingAddress, ViInt16 channel, ViChar response[])
{
  ViInt16 btcnt;
  static ViChar rdd[10000];
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  btcnt = 2 * numberOfSamples;
  /* Validate Command */
  retError = tkvx4244_invalidShortRange(numberOfSamples, 1, 5000, tkvx4244_ERROR_SAMP_CNT);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(channel, tkvx4244_ANALYZE_CH1,
					tkvx4244_ANALYZE_CH16, tkvx4244_ERROR_FETCH_CHL);
  if (retError)
    return retError;
  retError = tkvx4244_invalidLongRange(startingAddress, -262044L,
				       262144L, tkvx4244_ERROR_BEGIN_MEM);
  if (retError)
    return retError;
  /* Build the command string */
  sprintf(cmd, "FETCH:BIN?%hd,%ld (@%hd)\n", numberOfSamples, startingAddress, channel);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /*tkvx4244_sleep(VI_TRUE); */
  /* Get the response from the instrument */
  viSetAttribute(instrumentHandle, VI_ATTR_TERMCHAR_EN, VI_FALSE);
  retError = viRead(instrumentHandle, (ViBuf) rdd, btcnt, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;

  memcpy(response, rdd, btcnt);

  return VI_SUCCESS;

}

/***************************************************************************
 * Function: tkvx4244_readTimetag
 *
 * Purpose:
 *  This function reads the time tag values of the specified group(s).
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 gp1-gp4,all
 *  ViChar timeTag[]
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_readTimetag(ViSession instrumentHandle, ViInt16 gp1, ViInt16 gp2,
		     ViInt16 gp3, ViInt16 gp4, ViInt16 all, ViChar timeTag[])
{
  ViChar gplst[10];
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViChar rcv[tkvx4244_RCV_STRLEN];	/* storage for instrument receive strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate the command */
  retError = tkvx4244_invalidShortRange(gp1, tkvx4244_GROUP_ONE_DISABLE,
					tkvx4244_GROUP_ONE_ENABLE, tkvx4244_ERROR_GP_ONE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp2, tkvx4244_GROUP_TWO_DISABLE,
					tkvx4244_GROUP_TWO_ENABLE, tkvx4244_ERROR_GP_TWO);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp3, tkvx4244_GROUP_THREE_DISABLE,
					tkvx4244_GROUP_THREE_ENABLE, tkvx4244_ERROR_GP_THREE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp4, tkvx4244_GROUP_FOUR_DISABLE,
					tkvx4244_GROUP_FOUR_ENABLE, tkvx4244_ERROR_GP_FOUR);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(all, tkvx4244_ALL_GROUPS_DISABLE,
					tkvx4244_ALL_GROUPS_ENABLE, tkvx4244_ERROR_ALL_GROUPS);
  if (retError)
    return retError;
  if (all + gp1 + gp2 + gp3 + gp4 == VI_FALSE)
    return tkvx4244_NO_GRPS_SELECTED_ERROR;
  /* Parse the gp list */
  tkvx4244_gplist_parse(gp1, gp2, gp3, gp4, all, gplst);
  /* Build the command string */
  sprintf(cmd, "FETCH:TIME?(%s)\n", gplst);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Get the response from the instrument */
  retError = viRead(instrumentHandle, (ViPBuf) rcv, tkvx4244_RCV_STRLEN, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  strcpy(timeTag, rcv);

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_confFrontPanelOutTrig
 *
 * Purpose:
 *  This function controls which group output is routed to the trig and
 *  clock outputs.
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 groupNumber
 *  ViChar groupOutput[]
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC tkvx4244_confFrontPanelOutTrig(ViSession instrumentHandle, ViInt16 groupNumber,
						 ViChar groupOutput[])
{
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViChar rcv[tkvx4244_RCV_STRLEN];	/* storage for instrument receive strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate command */
  retError = tkvx4244_invalidShortRange(groupNumber, tkvx4244_GROUP_ONE,
					tkvx4244_GROUP_FOUR, tkvx4244_ERROR_GROUP_RANGE);
  if (retError)
    return retError;
  /* Build the command string */
  sprintf(cmd, "OUTP:GROU %hd\n", groupNumber);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Build the command string */
  sprintf(cmd, "OUTPUT:GROUP?\n");
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Get the response from the instrument */
  retError = viRead(instrumentHandle, (ViPBuf) rcv, tkvx4244_RCV_STRLEN, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  rcv[retCnt] = '\0';
  strcpy(groupOutput, rcv);

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_readSyncClkSource
 *
 * Purpose:
 * This function queries the instrument for synch clock setups.
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 synchronize
 *  ViChar activeLevel[]
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_readSyncClkSource(ViSession instrumentHandle, ViInt16 synchronize, ViChar activeLevel[])
{
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViChar rcv[tkvx4244_RCV_STRLEN];	/* storage for instrument receive strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;
  /* Validate command */
  retError = tkvx4244_invalidShortRange(synchronize, tkvx4244_SYNC_QUERY_TTL,
					tkvx4244_SYNC_QUERY_LEVEL, tkvx4244_ERROR_SYNC_CLK_QUERY);
  if (retError)
    return retError;
  /* Build the command string */
  sprintf(cmd, "FREQ:SYNC:%s\n", synch_clk[synchronize]);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Get the response from the instrument */
  retError = viRead(instrumentHandle, (ViPBuf) rcv, tkvx4244_RCV_STRLEN, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  strcpy(activeLevel, rcv);
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_readActiveChannels
 *
 * Purpose:
 * This function queries the instrument for the active channels.
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 queryCondition
 *  ViChar list[]
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_readActiveChannels(ViSession instrumentHandle, ViInt16 queryCondition, ViChar list[])
{
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViChar rcv[tkvx4244_RCV_STRLEN];	/* storage for instrument receive strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;
  /* Validate command */
  retError =
      tkvx4244_invalidShortRange(queryCondition, tkvx4244_CHAN_QUERY_OPEN,
				 tkvx4244_CHAN_QUERY_STATE, tkvx4244_ERROR_ACT_CHNL_QUERY);
  if (retError)
    return retError;
  /* Build the command string */
  sprintf(cmd, "ROUT:%s\n", ch_cond[queryCondition]);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Get the response from the instrument */
  retError = viRead(instrumentHandle, (ViPBuf) rcv, tkvx4244_RCV_STRLEN, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  rcv[retCnt] = '\0';
  strcpy(list, rcv);
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_readSampleRates
 *
 * Purpose:
 * This function reads the sample rate setup of the specified group(s).
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to
 *  access instrument specific data. Must be initialized by
 *  tkvx4244_init() prior to use.
 *  ViInt16 definitions: Type of query.
 *  ViChar response[]: buffer for response.
 *  ViInt16 gp1-gp4,all: Select group(s) for query.
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_readSampleRates(ViSession instrumentHandle,
			 ViInt16 definitions, ViInt16 gp1, ViInt16 gp2, ViInt16 gp3, ViInt16 gp4,
			 ViInt16 all, ViChar response[])
{
  ViChar gplst[10];
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViChar rcv[tkvx4244_RCV_STRLEN];	/* storage for instrument receive strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate command */
  retError = tkvx4244_invalidShortRange(definitions, tkvx4244_RATE_QUERY_INTERVAL,
					tkvx4244_RATE_QUERY_SRC, tkvx4244_ERROR_ACT_CHNL_QUERY);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp1, tkvx4244_GROUP_ONE_DISABLE,
					tkvx4244_GROUP_ONE_ENABLE, tkvx4244_ERROR_GP_ONE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp2, tkvx4244_GROUP_TWO_DISABLE,
					tkvx4244_GROUP_TWO_ENABLE, tkvx4244_ERROR_GP_TWO);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp3, tkvx4244_GROUP_THREE_DISABLE,
					tkvx4244_GROUP_THREE_ENABLE, tkvx4244_ERROR_GP_THREE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp4, tkvx4244_GROUP_FOUR_DISABLE,
					tkvx4244_GROUP_FOUR_ENABLE, tkvx4244_ERROR_GP_FOUR);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(all, tkvx4244_ALL_GROUPS_DISABLE,
					tkvx4244_ALL_GROUPS_ENABLE, tkvx4244_ERROR_ALL_GROUPS);
  if (retError)
    return retError;
  if (all + gp1 + gp2 + gp3 + gp4 == VI_FALSE)
    return tkvx4244_NO_GRPS_SELECTED_ERROR;
  /* Parse the gp list */
  tkvx4244_gplist_parse(gp1, gp2, gp3, gp4, all, gplst);
  /* Build the command string */
  sprintf(cmd, "FREQ:%s?(%s)\n", samp_rate_defs[definitions], gplst);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Get the response from the instrument */
  retError = viRead(instrumentHandle, (ViPBuf) rcv, tkvx4244_RCV_STRLEN, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  rcv[retCnt] = '\0';
  strcpy(response, rcv);
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_readCollectionCount
 *
 * Purpose:
 * This function reads the collection setup of the specified group(s).
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViChar count[]
 *  ViInt16 gp1-gp4,all
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_readCollectionCount(ViSession instrumentHandle,
			     ViInt16 gp1, ViInt16 gp2, ViInt16 gp3, ViInt16 gp4, ViInt16 all,
			     ViChar count[])
{
  ViChar gplst[10];
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViChar rcv[tkvx4244_RCV_STRLEN];	/* storage for instrument receive strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate the command */
  retError = tkvx4244_invalidShortRange(gp1, tkvx4244_GROUP_ONE_DISABLE,
					tkvx4244_GROUP_ONE_ENABLE, tkvx4244_ERROR_GP_ONE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp2, tkvx4244_GROUP_TWO_DISABLE,
					tkvx4244_GROUP_TWO_ENABLE, tkvx4244_ERROR_GP_TWO);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp3, tkvx4244_GROUP_THREE_DISABLE,
					tkvx4244_GROUP_THREE_ENABLE, tkvx4244_ERROR_GP_THREE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp4, tkvx4244_GROUP_FOUR_DISABLE,
					tkvx4244_GROUP_FOUR_ENABLE, tkvx4244_ERROR_GP_FOUR);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(all, tkvx4244_ALL_GROUPS_DISABLE,
					tkvx4244_ALL_GROUPS_ENABLE, tkvx4244_ERROR_ALL_GROUPS);
  if (retError)
    return retError;
  if (all + gp1 + gp2 + gp3 + gp4 == VI_FALSE)
    return tkvx4244_NO_GRPS_SELECTED_ERROR;
  /* Parse the gp list */
  tkvx4244_gplist_parse(gp1, gp2, gp3, gp4, all, gplst);
  /* Build the command string */
  sprintf(cmd, "SWEEP:POINTS?(%s)\n", gplst);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Get the response from the instrument */
  retError = viRead(instrumentHandle, (ViPBuf) rcv, tkvx4244_RCV_STRLEN, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  rcv[retCnt] = '\0';
  strcpy(count, rcv);
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_readArmSrcZero
 *
 * Purpose:
 *  This function queries setup of arm source and memory zero for the
 *  specified group(s).
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 armSourceOrMemoryZero
 *  ViChar armOrMemoryZeroResponse[]
 *  ViInt16 gp1-gp4,all
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_readArmSrcZero(ViSession instrumentHandle,
			ViInt16 armSourceOrMemoryZero, ViInt16 gp1, ViInt16 gp2, ViInt16 gp3,
			ViInt16 gp4, ViInt16 all, ViChar armOrMemoryZeroResponse[])
{
  ViChar gplst[10];
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViChar rcv[tkvx4244_RCV_STRLEN];	/* storage for instrument receive strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate command */
  retError = tkvx4244_invalidShortRange(armSourceOrMemoryZero, 0, 1, tkvx4244_ERROR_ARM_SRC_QUERY);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp1, tkvx4244_GROUP_ONE_DISABLE,
					tkvx4244_GROUP_ONE_ENABLE, tkvx4244_ERROR_GP_ONE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp2, tkvx4244_GROUP_TWO_DISABLE,
					tkvx4244_GROUP_TWO_ENABLE, tkvx4244_ERROR_GP_TWO);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp3, tkvx4244_GROUP_THREE_DISABLE,
					tkvx4244_GROUP_THREE_ENABLE, tkvx4244_ERROR_GP_THREE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp4, tkvx4244_GROUP_FOUR_DISABLE,
					tkvx4244_GROUP_FOUR_ENABLE, tkvx4244_ERROR_GP_FOUR);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(all, tkvx4244_ALL_GROUPS_DISABLE,
					tkvx4244_ALL_GROUPS_ENABLE, tkvx4244_ERROR_ALL_GROUPS);
  if (retError)
    return retError;
  if (all + gp1 + gp2 + gp3 + gp4 == VI_FALSE)
    return tkvx4244_NO_GRPS_SELECTED_ERROR;
  /* Parse the gp list */
  tkvx4244_gplist_parse(gp1, gp2, gp3, gp4, all, gplst);
  /* Build the command string */
  sprintf(cmd, "ARM:%s (%s)\n", arm_src_type[armSourceOrMemoryZero], gplst);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Get the response from the instrument */
  retError = viRead(instrumentHandle, (ViPBuf) rcv, tkvx4244_RCV_STRLEN, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  rcv[retCnt] = '\0';
  strcpy(armOrMemoryZeroResponse, rcv);
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_readInputVoltageSettings
 *
 * Purpose:
 *  This function queries the input voltage ranges of the channels.
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 rangeQuery
 *  ViChar inputVoltage[]
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_readInputVoltageSettings(ViSession instrumentHandle,
				  ViInt16 rangeQuery, ViChar inputVoltage[])
{
  ViChar chan[40];
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViChar rcv[tkvx4244_RCV_STRLEN];	/* storage for instrument receive strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate the command */
  retError = tkvx4244_invalidShortRange(rangeQuery, tkvx4244_VOLT_RANGE_UPPER,
					tkvx4244_VOLT_RANGE_LOWER, tkvx4244_ERROR_VOLT_RANGE_QUERY);
  if (retError)
    return retError;

  /* Parse the ch list */
  tkvx4244_chlst_parse(VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE,
		       VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE,
		       VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE, VI_TRUE, chan);
  /* Build the command string */
  sprintf(cmd, "VOLT:RANG:%s? (%s)\n", vlt_type[rangeQuery], chan);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Get the response from the instrument */
  retError = viRead(instrumentHandle, (ViPBuf) rcv, tkvx4244_RCV_STRLEN, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  rcv[retCnt] = '\0';
  strcpy(inputVoltage, rcv);
  inputVoltage[retCnt] = '\0';

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_volt_rang_query
 *
 * Purpose:
 *	 NOTE: This Function is an "Exception Function" and is not
 *compliant with VXIplu&play standards because of the number of
 *parameters in the function. This function is being maintained in
 *this version of the VXIplug&play driver to provide compatibility
 *with existing applications. New functions have been written to
 *replace this function.  There will not be a function panel for
 *this function in WIN Framework 4.0 compliant drivers.
 *  This function queries the input voltage ranges of the channels.
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 rang
 *  ViChar answer[]
 *  ViInt16 c1-c16,ca
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC tkvx4244_volt_rang_query(ViSession instrumentHandle, ViInt16 rang, ViInt16 c1,
					   ViInt16 c2, ViInt16 c3, ViInt16 c4, ViInt16 c5,
					   ViInt16 c6, ViInt16 c7, ViInt16 c8, ViInt16 c9,
					   ViInt16 c10, ViInt16 c11, ViInt16 c12, ViInt16 c13,
					   ViInt16 c14, ViInt16 c15, ViInt16 c16, ViInt16 ca,
					   ViChar answer[])
{
  ViChar chan[40];
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViChar rcv[tkvx4244_RCV_STRLEN];	/* storage for instrument receive strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate the command */
  retError = tkvx4244_invalidShortRange(rang, 0, 1, tkvx4244_ERROR_VOLT_RANGE_QUERY);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c1, 0, 1, tkvx4244_ERROR_CHAN_ONE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c2, 0, 1, tkvx4244_ERROR_CHAN_TWO);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c3, 0, 1, tkvx4244_ERROR_CHAN_THREE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c4, 0, 1, tkvx4244_ERROR_CHAN_FOUR);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c5, 0, 1, tkvx4244_ERROR_CHAN_FIVE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c6, 0, 1, tkvx4244_ERROR_CHAN_SIX);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c7, 0, 1, tkvx4244_ERROR_CHAN_SEVEN);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c8, 0, 1, tkvx4244_ERROR_CHAN_EIGHT);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c9, 0, 1, tkvx4244_ERROR_CHAN_NINE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c10, 0, 1, tkvx4244_ERROR_CHAN_TEN);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c11, 0, 1, tkvx4244_ERROR_CHAN_ELEVEN);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c12, 0, 1, tkvx4244_ERROR_CHAN_TWELVE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c13, 0, 1, tkvx4244_ERROR_CHAN_THIRTEEN);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c14, 0, 1, tkvx4244_ERROR_CHAN_FOURTEEN);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c15, 0, 1, tkvx4244_ERROR_CHAN_FIFTEEN);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c16, 0, 1, tkvx4244_ERROR_CHAN_SIXTEEN);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ca, 0, 1, tkvx4244_ERROR_ALL_CHAN);
  if (retError)
    return retError;
  if (ca + c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8 + c9 + c10 + c11 + c12 + c13 + c14 + c15 + c16 ==
      VI_FALSE)
    return tkvx4244_NO_CHAN_SELECTED_ERROR;
  /* Parse the ch list */
  tkvx4244_chlst_parse(c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15, c16, ca,
		       chan);
  /* Build the command string */
  sprintf(cmd, "VOLT:RANG:%s? (%s)\n", vlt_type[rang], chan);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Get the response from the instrument */
  retError = viRead(instrumentHandle, (ViPBuf) rcv, tkvx4244_RCV_STRLEN, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  rcv[retCnt] = '\0';
  strcpy(answer, rcv);
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_confExtClkSlopAndTtag
 *
 * Purpose:
 * This function sets the active edge of the external clock to either
 * pos true or neg true.  It also sets the source of the Timetag clk.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 slope
 *  ViInt16 timeTag
 *  ViChar clockSlope[],
 *  ViChar timeTagSource[]
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_confExtClkSlopAndTtag(ViSession instrumentHandle,
			       ViInt16 slope, ViInt16 timeTag, ViChar clockSlope[],
			       ViChar timeTagSource[])
{

  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViChar rcv[tkvx4244_RCV_STRLEN];	/* storage for instrument receive strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /*Validate command */
  retError = tkvx4244_invalidShortRange(slope, tkvx4244_SLOPE_POS,
					tkvx4244_SLOPE_NEG, tkvx4244_ERROR_CLOCK_SLOPE_QUERY);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(timeTag, tkvx4244_TT_CLK10,
					tkvx4244_TT_ROSC, tkvx4244_ERROR_CLOCK_TTAG_QUERY);
  if (retError)
    return retError;

  /* Build the command string */
  sprintf(cmd, "FREQ:%s\n", slopett[slope]);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Build the command string */
  sprintf(cmd, "FREQ:%s\n", slopett[timeTag]);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Build the command string */
  sprintf(cmd, "FREQ:SLOP? \n");
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Get the response from the instrument */
  retError = viRead(instrumentHandle, (ViPBuf) rcv, tkvx4244_RCV_STRLEN, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  rcv[retCnt] = '\0';
  strcpy(clockSlope, rcv);
  /* Build the command string */
  sprintf(cmd, "FREQ:TIME? \n");
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Get the response from the instrument */
  retError = viRead(instrumentHandle, (ViPBuf) rcv, tkvx4244_RCV_STRLEN, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  rcv[retCnt] = '\0';
  strcpy(timeTagSource, rcv);
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_eventReqEnable
 *
 * Purpose:
 *  This function defines the mask for event status reporting and for
 *  generating VXI Request True interrupts.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 requestEventEnab
 *  ViInt16 maskDefinition
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_eventReqEnable(ViSession instrumentHandle,
			ViInt16 requestEventEnab, ViInt16 maskDefinition)
{
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;
  /* Validate command */
  retError = tkvx4244_invalidShortRange(requestEventEnab, tkvx4244_ENABLE_SRE,
					tkvx4244_ENABLE_ESE, tkvx4244_ERROR_EVENT_REC_ENAB);
  if (retError)
    return retError;
  /* Build the command string */
  sprintf(cmd, "%s %hd\n", event_req[requestEventEnab], maskDefinition);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_readStatus
 *
 * Purpose:
 *	This function returns the results of some standard 488.2 queries.
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 select4882Command
 *  ViChar get4882Status[]
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_readStatus(ViSession instrumentHandle, ViInt16 select4882Command, ViChar get4882Status[])
{
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViChar rcv[tkvx4244_RCV_STRLEN];	/* storage for instrument receive strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;
  /* Validate command */
  retError = tkvx4244_invalidShortRange(select4882Command, tkvx4244_QUERY_IDN,
					tkvx4244_QUERY_ESE, tkvx4244_ERROR_4882_QUERY);
  if (retError)
    return retError;
  /* Build the command string */
  sprintf(cmd, "%s\n", read_status[select4882Command]);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Get the response from the instrument */
  retError = viRead(instrumentHandle, (ViPBuf) rcv, tkvx4244_RCV_STRLEN, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  rcv[retCnt] = '\0';
  strcpy(get4882Status, rcv);
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_readPostprocessCalc
 *
 * Purpose:
 *  This function allows retrieval of some basic signal parameters.  The
 *  parameters are maximum, minimum, average, positive transition and
 *  negative transition.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 measurement:
 *  ViInt32 numberOfSamples:
 *  ViInt32 startingAddress:
 *  ViInt16 channel:
 *  ViChar response[]:
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_readPostprocessCalc(ViSession instrumentHandle,
			     ViInt16 measurement, ViInt32 numberOfSamples, ViInt32 startingAddress,
			     ViInt16 channel, ViChar response[])
{
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViChar rcv[tkvx4244_RCV_STRLEN];	/* storage for instrument receive strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */
  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate command */
  retError = tkvx4244_invalidLongRange(numberOfSamples, 1L, 262144L, tkvx4244_ERROR_DATA_BEG_MEM);
  if (retError)
    return retError;
  retError = tkvx4244_invalidLongRange(startingAddress, -262000L,
				       262144L, tkvx4244_ERROR_DATA_SAMPS);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(measurement, tkvx4244_ANALYZE_MIN,
					tkvx4244_ANALYZE_TRMS, tkvx4244_ERROR_DATA_TYPE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(channel, tkvx4244_ANALYZE_CH1,
					tkvx4244_ANALYZE_CH16, tkvx4244_ERROR_DATA_CHAN);
  if (retError)
    return retError;

  /* Build the command string */
  sprintf(cmd, "FETCH:%s? %ld,%ld(@%hd)\n", process[measurement],
	  numberOfSamples, startingAddress, channel);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  tkvx4244_sleep(1);
  /* Get the response from the instrument */
  retError = viRead(instrumentHandle, (ViPBuf) rcv, tkvx4244_RCV_STRLEN, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  rcv[retCnt] = '\0';
  strcpy(response, rcv);
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_initStopMeasurementCycle
 *
 * Purpose:
 *  This function aborts the collection process on the designated groups.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 gp1-gp4,all
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_initStopMeasurementCycle(ViSession instrumentHandle,
				  ViInt16 gp1, ViInt16 gp2, ViInt16 gp3, ViInt16 gp4, ViInt16 all)
{
  ViChar gplst[10];
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate the command */
  retError = tkvx4244_invalidShortRange(gp1, tkvx4244_GROUP_ONE_DISABLE,
					tkvx4244_GROUP_ONE_ENABLE, tkvx4244_ERROR_GP_ONE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp2, tkvx4244_GROUP_TWO_DISABLE,
					tkvx4244_GROUP_TWO_ENABLE, tkvx4244_ERROR_GP_TWO);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp3, tkvx4244_GROUP_THREE_DISABLE,
					tkvx4244_GROUP_THREE_ENABLE, tkvx4244_ERROR_GP_THREE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp4, tkvx4244_GROUP_FOUR_DISABLE,
					tkvx4244_GROUP_FOUR_ENABLE, tkvx4244_ERROR_GP_FOUR);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(all, tkvx4244_ALL_GROUPS_DISABLE,
					tkvx4244_ALL_GROUPS_ENABLE, tkvx4244_ERROR_ALL_GROUPS);
  if (retError)
    return retError;
  if (all + gp1 + gp2 + gp3 + gp4 == VI_FALSE)
    return tkvx4244_NO_GRPS_SELECTED_ERROR;
  /* Parse the gp list */
  tkvx4244_gplist_parse(gp1, gp2, gp3, gp4, all, gplst);
  /* Build the command string */
  sprintf(cmd, "ABORT (%s)\n", gplst);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_confMastertrigOut
 *
 * Purpose:
 *  This function controls the master trig output of the module.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to
 *  access instrument specific data. Must be initialized by 
 *  tkvx4244_init()	prior to use.
 *  ViInt16 outputSelect:Which TTLtrg line to use.
 *  ViInt16 logic: Logic AND or OR.
 *  ViInt16 slope: Pos or Neg.
 *  ViInt16 gp1,gp2,gp3,gp4: Select group(s).
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_confMastertrigOut(ViSession instrumentHandle,
			   ViInt16 outputSelect, ViInt16 logic, ViInt16 slope, ViInt16 gp1,
			   ViInt16 gp2, ViInt16 gp3, ViInt16 gp4)
{
  ViChar slpp[5];
  ViChar mask[10];
  ViInt16 temp;
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;
  /*Validate command */
  retError = tkvx4244_invalidShortRange(outputSelect, tkvx4244_TTLTRIG0,
					tkvx4244_TTL_TRIGGER_OFF, tkvx4244_ERROR_OUT_TRG_TYPE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(logic, tkvx4244_TRIGGER_LOGIC_AND,
					tkvx4244_TRIGGER_LOGIC_OR, tkvx4244_ERROR_OUT_TRG_LOGIC);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(slope, tkvx4244_SLOPE_POS,
					tkvx4244_SLOPE_NEG, tkvx4244_ERROR_OUT_TRG_SLOPE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp1, tkvx4244_GROUP_ONE_DISABLE,
					tkvx4244_GROUP_ONE_ENABLE, tkvx4244_ERROR_GP_ONE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp2, tkvx4244_GROUP_TWO_DISABLE,
					tkvx4244_GROUP_TWO_ENABLE, tkvx4244_ERROR_GP_TWO);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp3, tkvx4244_GROUP_THREE_DISABLE,
					tkvx4244_GROUP_THREE_ENABLE, tkvx4244_ERROR_GP_THREE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp4, tkvx4244_GROUP_FOUR_DISABLE,
					tkvx4244_GROUP_FOUR_ENABLE, tkvx4244_ERROR_GP_FOUR);
  if (retError)
    return retError;
  if (gp1 + gp2 + gp3 + gp4 == VI_FALSE)
    return tkvx4244_NO_GRPS_SELECTED_ERROR;

  mask[0] = 48;
  mask[1] = 48;
  mask[2] = 48;
  if (gp2 == 1)
    gp2 = 2;
  if (gp3 == 1)
    gp3 = 4;
  if (gp4 == 1)
    gp4 = 8;
  temp = gp1 + gp2 + gp3 + gp4;
  if (!temp)
    mask[3] = 48;
  if (temp == 1)
    mask[3] = 49;
  if (temp == 2)
    mask[3] = 50;
  if (temp == 3)
    mask[3] = 51;
  if (temp == 4)
    mask[3] = 52;
  if (temp == 5)
    mask[3] = 53;
  if (temp == 6)
    mask[3] = 54;
  if (temp == 7)
    mask[3] = 55;
  if (temp == 8)
    mask[3] = 56;
  if (temp == 9)
    mask[3] = 57;
  if (temp == 10)
    mask[3] = 65;
  if (temp == 11)
    mask[3] = 66;
  if (temp == 12)
    mask[3] = 67;
  if (temp == 13)
    mask[3] = 68;
  if (temp == 14)
    mask[3] = 69;
  if (temp == 15)
    mask[3] = 70;
  mask[4] = '\0';
  if (!slope)
    sprintf(slpp, "POS");
  else
    sprintf(slpp, "NEG");
  /* Build the command string */
  sprintf(cmd, "OUTP:TRIG:%s\n", trgsrcL[logic]);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Build the command string */
  sprintf(cmd, "OUTP:TRIG:MASK %s\n", mask);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Build the command string */
  sprintf(cmd, "OUTP:TRIG:SLOP %s\n", slpp);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Build the command string */
  sprintf(cmd, "OUTP:TRIG:%s\n", out_select[outputSelect]);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Build the command string */
  sprintf(cmd, "OUTP:TRIG:LOAD\n");
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  return VI_SUCCESS;

}

/***************************************************************************
 * Function: tkvx4244_confTriggerThreshold
 *
 * Purpose:
 *  This function sets trigger threshold settings.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to
 *  access instrument specific data. Must be initialized by 
 *  tkvx4244_init()	prior to use.
 *  ViInt16 thresholdDescriptor
 *  ViReal64 voltageLevel
 *  ViInt16 group1
 *  ViInt16 group2
 *  ViInt16 group3
 *  ViInt16 group4
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_confTriggerThreshold(ViSession instrumentHandle, ViInt16 thresholdDescriptor,
			      ViReal64 voltageLevel, ViInt16 group1, ViInt16 group2, ViInt16 group3,
			      ViInt16 group4)
{
  ViChar loc[20];
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate the command */
  retError = tkvx4244_invalidShortRange(group1, tkvx4244_GROUP_CHAN_ONE,
					tkvx4244_GROUP_CHAN_OFF, tkvx4244_ERROR_GP_ONE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(group2, tkvx4244_GROUP_CHAN_ONE,
					tkvx4244_GROUP_CHAN_OFF, tkvx4244_ERROR_GP_TWO);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(group3, tkvx4244_GROUP_CHAN_ONE,
					tkvx4244_GROUP_CHAN_OFF, tkvx4244_ERROR_GP_THREE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(group4, tkvx4244_GROUP_CHAN_ONE,
					tkvx4244_GROUP_CHAN_OFF, tkvx4244_ERROR_GP_FOUR);
  if (retError)
    return retError;
  if (group1 + group2 + group3 + group4 == 16)
    return tkvx4244_NO_GRPS_SELECTED_ERROR;
  retError = tkvx4244_invalidShortRange(thresholdDescriptor,
					tkvx4244_THRESH_POS_SLOPE, tkvx4244_THRESH_LESS_THAN,
					tkvx4244_ERROR_THRESH_TYPE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidRealRange(voltageLevel, -20.0, 20.0, tkvx4244_ERROR_THRESH_LEVEL);
  if (retError)
    return retError;

  sprintf(loc, "%s%s%s%s", gp1[group1], gp2[group2], gp3[group3], gp4[group4]);
  if (strlen(loc) < 2)
    return -3;
  /* Build the command string */
  sprintf(cmd, "TRIG:THRE:%s %f (@%s)\n", thrshCmd[thresholdDescriptor], voltageLevel, loc);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  return VI_SUCCESS;

}

/***************************************************************************
 * Function: tkvx4244_confArmSrcMemZero
 *
 * Purpose:
 *  This function defines various conditions for arming the group triggers .
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 source
 *  ViInt16 memoryZero
 *  ViInt16 gp1-gp4,allGroups
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_confArmSrcMemZero(ViSession instrumentHandle, ViInt16 source,
			   ViInt16 gp1, ViInt16 gp2, ViInt16 gp3, ViInt16 gp4, ViInt16 allGroups,
			   ViInt16 memoryZero)
{
  ViChar gplst[10];
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate the command */
  retError =
      tkvx4244_invalidShortRange(source, tkvx4244_EXTERNAL_ARM, tkvx4244_IMMEDIATE_ARM,
				 tkvx4244_ERROR_EXT_ARM_SLP);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(memoryZero, tkvx4244_MEMORY_ZERO_ON, tkvx4244_MEMORY_ZERO_OFF,
				 tkvx4244_ERROR_EXT_ARM_ZERO);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp1, tkvx4244_GROUP_ONE_DISABLE,
					tkvx4244_GROUP_ONE_ENABLE, tkvx4244_ERROR_GP_ONE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp2, tkvx4244_GROUP_TWO_DISABLE,
					tkvx4244_GROUP_TWO_ENABLE, tkvx4244_ERROR_GP_TWO);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp3, tkvx4244_GROUP_THREE_DISABLE,
					tkvx4244_GROUP_THREE_ENABLE, tkvx4244_ERROR_GP_THREE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp4, tkvx4244_GROUP_FOUR_DISABLE,
					tkvx4244_GROUP_FOUR_ENABLE, tkvx4244_ERROR_GP_FOUR);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(allGroups, tkvx4244_ALL_GROUPS_DISABLE,
					tkvx4244_ALL_GROUPS_ENABLE, tkvx4244_ERROR_ALL_GROUPS);
  if (retError)
    return retError;
  if (allGroups + gp1 + gp2 + gp3 + gp4 == VI_FALSE)
    return tkvx4244_NO_GRPS_SELECTED_ERROR;
  /* Parse the gp list */
  tkvx4244_gplist_parse(gp1, gp2, gp3, gp4, allGroups, gplst);
  /* Build the command string */
  sprintf(cmd, "ARM:%s (%s)\n", arm_slopZ[memoryZero], gplst);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Build the command string */
  sprintf(cmd, "ARM:SOUR %s (%s)\n", ext[source], gplst);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_confFrontPanelDisplay
 *
 * Purpose:
 *  This function controls the LED display on the face plate of the card.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViChar groupDisplayed[]
 *  ViInt16 groupNumber
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_confFrontPanelDisplay(ViSession instrumentHandle,
			       ViInt16 groupNumber, ViChar groupDisplayed[])
{
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViChar rcv[tkvx4244_RCV_STRLEN];	/* storage for instrument receive strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate command */
  retError = tkvx4244_invalidShortRange(groupNumber, tkvx4244_GROUP_ONE,
					tkvx4244_GROUP_FOUR, tkvx4244_ERROR_GROUP);
  if (retError)
    return retError;
  /* Build the command string */
  sprintf(cmd, "DISP:GROUP%hd\n", groupNumber);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Build the command string */
  sprintf(cmd, "DISP:GROUP?\n");
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Get the response from the instrument */
  retError = viRead(instrumentHandle, (ViPBuf) rcv, tkvx4244_RCV_STRLEN, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  rcv[retCnt] = '\0';
  strcpy(groupDisplayed, rcv);
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_confGpOneVoltageRange
 *
 * Purpose:
 *  This function sets the input voltage ranges of the channels
 *  in Group 1.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 voltageRange,ch1-ch4
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_confGpOneVoltageRange(ViSession instrumentHandle,
			       ViInt16 ch1, ViInt16 ch2, ViInt16 ch3, ViInt16 ch4,
			       ViInt16 voltageRange)
{
  ViChar chan[40];
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate the command */
  retError = tkvx4244_invalidShortRange(voltageRange, tkvx4244_VOLT_RANGE_200MV,
					tkvx4244_VOLT_RANGE_20VOLT, tkvx4244_ERROR_INPUT_VOLT);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch1, tkvx4244_CH_ONE_DISABLE, tkvx4244_CH_ONE_ENABLE,
				 tkvx4244_ERROR_CHAN_ONE);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch2, tkvx4244_CH_TWO_DISABLE, tkvx4244_CH_TWO_ENABLE,
				 tkvx4244_ERROR_CHAN_TWO);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch3, tkvx4244_CH_THREE_DISABLE, tkvx4244_CH_THREE_ENABLE,
				 tkvx4244_ERROR_CHAN_THREE);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch4, tkvx4244_CH_FOUR_DISABLE, tkvx4244_CH_FOUR_ENABLE,
				 tkvx4244_ERROR_CHAN_FOUR);
  if (retError)
    return retError;
  if ((ch1 + ch2 + ch3 + ch4) == VI_FALSE)
    return tkvx4244_NO_CHAN_SELECTED_ERROR;
/* Parse the ch list */
  tkvx4244_chlst_parse(ch1, ch2, ch3, ch4, VI_FALSE, VI_FALSE,
		       VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE,
		       VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE, chan);

  sprintf(cmd, "VOLT:RANG %3.1f (%s)\n", range[voltageRange], chan);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_confGpTwoVoltageRange
 *
 * Purpose:
 *  This function sets the input voltage ranges of the channels
 *  in Group 2.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 voltageRange,ch5-ch8
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_confGpTwoVoltageRange(ViSession instrumentHandle,
			       ViInt16 ch5, ViInt16 ch6, ViInt16 ch7, ViInt16 ch8,
			       ViInt16 voltageRange)
{
  ViChar chan[40];
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate the command */
  retError =
      tkvx4244_invalidShortRange(voltageRange, tkvx4244_VOLT_RANGE_200MV,
				 tkvx4244_VOLT_RANGE_20VOLT, tkvx4244_ERROR_INPUT_VOLT);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch5, tkvx4244_CH_FIVE_DISABLE, tkvx4244_CH_FIVE_ENABLE,
				 tkvx4244_ERROR_CHAN_FIVE);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch6, tkvx4244_CH_SIX_DISABLE, tkvx4244_CH_SIX_ENABLE,
				 tkvx4244_ERROR_CHAN_SIX);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch7, tkvx4244_CH_SEVEN_DISABLE, tkvx4244_CH_SEVEN_ENABLE,
				 tkvx4244_ERROR_CHAN_SEVEN);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch8, tkvx4244_CH_EIGHT_DISABLE, tkvx4244_CH_EIGHT_ENABLE,
				 tkvx4244_ERROR_CHAN_EIGHT);
  if (retError)
    return retError;
  if ((ch5 + ch6 + ch7 + ch8) == VI_FALSE)
    return tkvx4244_NO_CHAN_SELECTED_ERROR;
  /* Parse the ch list */
  tkvx4244_chlst_parse(VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE,
		       ch5, ch6, ch7, ch8, VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE,
		       VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE, chan);

  sprintf(cmd, "VOLT:RANG %f (%s)\n", range[voltageRange], chan);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_confGpThreeVoltageRange
 *
 * Purpose:
 *  This function sets the input voltage ranges of the channels
 *  in Group 3.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 voltageRange,ch9-ch12
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_confGpThreeVoltageRange(ViSession instrumentHandle,
				 ViInt16 ch9, ViInt16 ch10, ViInt16 ch11, ViInt16 ch12,
				 ViInt16 voltageRange)
{
  ViChar chan[40];
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate the command */
  retError =
      tkvx4244_invalidShortRange(voltageRange, tkvx4244_VOLT_RANGE_200MV,
				 tkvx4244_VOLT_RANGE_20VOLT, tkvx4244_ERROR_INPUT_VOLT);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch9, tkvx4244_CH_NINE_DISABLE, tkvx4244_CH_NINE_ENABLE,
				 tkvx4244_ERROR_CHAN_NINE);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch10, tkvx4244_CH_TEN_DISABLE, tkvx4244_CH_TEN_ENABLE,
				 tkvx4244_ERROR_CHAN_TEN);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch11, tkvx4244_CH_ELEVEN_DISABLE, tkvx4244_CH_ELEVEN_ENABLE,
				 tkvx4244_ERROR_CHAN_ELEVEN);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch12, tkvx4244_CH_TWELVE_DISABLE, tkvx4244_CH_TWELVE_ENABLE,
				 tkvx4244_ERROR_CHAN_TWELVE);
  if (retError)
    return retError;
  if ((ch9 + ch10 + ch11 + ch12) == VI_FALSE)
    return tkvx4244_NO_CHAN_SELECTED_ERROR;
  /* Parse the ch list */
  tkvx4244_chlst_parse(VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE,
		       VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE, ch9, ch10, ch11, ch12,
		       VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE, chan);

  sprintf(cmd, "VOLT:RANG %f (%s)\n", range[voltageRange], chan);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_confGpFourVoltageRange
 *
 * Purpose:
 *  This function sets the input voltage ranges of the channels
 *  in Group 4.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 voltageRange,ch13-ch16
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_confGpFourVoltageRange(ViSession instrumentHandle,
				ViInt16 ch13, ViInt16 ch14, ViInt16 ch15, ViInt16 ch16,
				ViInt16 voltageRange)
{
  ViChar chan[40];
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate the command */
  retError =
      tkvx4244_invalidShortRange(voltageRange, tkvx4244_VOLT_RANGE_200MV,
				 tkvx4244_VOLT_RANGE_20VOLT, tkvx4244_ERROR_INPUT_VOLT);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch13, tkvx4244_CH_THIRTEEN_DISABLE, tkvx4244_CH_THIRTEEN_ENABLE,
				 tkvx4244_ERROR_CHAN_THIRTEEN);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch14, tkvx4244_CH_FOURTEEN_DISABLE, tkvx4244_CH_FOURTEEN_ENABLE,
				 tkvx4244_ERROR_CHAN_FOURTEEN);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch15, tkvx4244_CH_FIFTEEN_DISABLE, tkvx4244_CH_FIFTEEN_ENABLE,
				 tkvx4244_ERROR_CHAN_FIFTEEN);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch16, tkvx4244_CH_SIXTEEN_DISABLE, tkvx4244_CH_SIXTEEN_ENABLE,
				 tkvx4244_ERROR_CHAN_SIXTEEN);
  if (retError)
    return retError;
  if ((ch13 + ch14 + ch15 + ch16) == VI_FALSE)
    return tkvx4244_NO_CHAN_SELECTED_ERROR;
  /* Parse the ch list */
  tkvx4244_chlst_parse(VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE,
		       VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE,
		       VI_FALSE, VI_FALSE, ch13, ch14, ch15, ch16, VI_FALSE, chan);

  sprintf(cmd, "VOLT:RANG %f (%s)\n", range[voltageRange], chan);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_volt_rang
 *
 * Purpose:
 *	 NOTE: This Function is an "Exception Function" and is not
 *compliant with VXIplu&play standards because of the number of
 *parameters in the function. This function is being maintained in
 *this version of the VXIplug&play driver to provide compatibility
 *with existing applications. New functions have been written to
 *replace this function.  There will not be a function panel for
 *this function in WIN Framework 4.0 compliant drivers.
 *
 *  This function sets the input voltage ranges of the channels.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 vlts,c1-c16,ca
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC tkvx4244_volt_rang(ViSession instrumentHandle, ViInt16 vlts, ViInt16 c1,
				     ViInt16 c2, ViInt16 c3, ViInt16 c4, ViInt16 c5, ViInt16 c6,
				     ViInt16 c7, ViInt16 c8, ViInt16 c9, ViInt16 c10, ViInt16 c11,
				     ViInt16 c12, ViInt16 c13, ViInt16 c14, ViInt16 c15,
				     ViInt16 c16, ViInt16 ca)
{
  ViChar chan[40];
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate the command */
  retError = tkvx4244_invalidShortRange(vlts, 0, 6, tkvx4244_ERROR_INPUT_VOLT);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c1, 0, 1, tkvx4244_ERROR_CHAN_ONE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c2, 0, 1, tkvx4244_ERROR_CHAN_TWO);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c3, 0, 1, tkvx4244_ERROR_CHAN_THREE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c4, 0, 1, tkvx4244_ERROR_CHAN_FOUR);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c5, 0, 1, tkvx4244_ERROR_CHAN_FIVE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c6, 0, 1, tkvx4244_ERROR_CHAN_SIX);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c7, 0, 1, tkvx4244_ERROR_CHAN_SEVEN);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c8, 0, 1, tkvx4244_ERROR_CHAN_EIGHT);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c9, 0, 1, tkvx4244_ERROR_CHAN_NINE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c10, 0, 1, tkvx4244_ERROR_CHAN_TEN);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c11, 0, 1, tkvx4244_ERROR_CHAN_ELEVEN);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c12, 0, 1, tkvx4244_ERROR_CHAN_TWELVE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c13, 0, 1, tkvx4244_ERROR_CHAN_THIRTEEN);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c14, 0, 1, tkvx4244_ERROR_CHAN_FOURTEEN);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c15, 0, 1, tkvx4244_ERROR_CHAN_FIFTEEN);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(c16, 0, 1, tkvx4244_ERROR_CHAN_SIXTEEN);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(ca, 0, 1, tkvx4244_ERROR_ALL_CHAN);
  if (retError)
    return retError;
  if (ca + c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8 + c9 + c10 + c11 + c12 + c13 + c14 + c15 + c16 ==
      VI_FALSE)
    return tkvx4244_NO_CHAN_SELECTED_ERROR;
  /* Parse the ch list */
  tkvx4244_chlst_parse(c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15, c16, ca,
		       chan);
  /* Build the command string */
  sprintf(cmd, "VOLT:RANG %f (%s)\n", range[vlts], chan);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  return VI_SUCCESS;

}

/***************************************************************************
 * Function: tkvx4244_confSamplingInterval
 *
 * Purpose:
 *  This function sets the Interval or frequency for the selected groups.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 source:
 *  ViInt16 intervalFrequency:
 *  ViReal64 samplingRate:
 *  ViInt16 gp1,gp2, gp3, gp4,allGroups
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_confSamplingInterval(ViSession instrumentHandle,
			      ViInt16 source, ViInt16 intervalFrequency, ViReal64 * samplingRate,
			      ViInt16 gp1, ViInt16 gp2, ViInt16 gp3, ViInt16 gp4, ViInt16 allGroups)
{
  ViChar gplst[10];
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate the command */
  retError = tkvx4244_invalidShortRange(source, 0, 1, tkvx4244_ERROR_EXT_FREQ);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(intervalFrequency, 0, 1, tkvx4244_ERROR_FREQ_TYPE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp1, tkvx4244_GROUP_ONE_DISABLE,
					tkvx4244_GROUP_ONE_ENABLE, tkvx4244_ERROR_GP_ONE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp2, tkvx4244_GROUP_TWO_DISABLE,
					tkvx4244_GROUP_TWO_ENABLE, tkvx4244_ERROR_GP_TWO);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp3, tkvx4244_GROUP_THREE_DISABLE,
					tkvx4244_GROUP_THREE_ENABLE, tkvx4244_ERROR_GP_THREE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp4, tkvx4244_GROUP_FOUR_DISABLE,
					tkvx4244_GROUP_FOUR_ENABLE, tkvx4244_ERROR_GP_FOUR);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(allGroups, tkvx4244_ALL_GROUPS_DISABLE,
					tkvx4244_ALL_GROUPS_ENABLE, tkvx4244_ERROR_ALL_GROUPS);
  if (retError)
    return retError;
  if (allGroups + gp1 + gp2 + gp3 + gp4 == VI_FALSE)
    return tkvx4244_NO_GRPS_SELECTED_ERROR;
  if (*samplingRate > 1.0) {
    retError = tkvx4244_invalidRealRange(*samplingRate, 30.6372, 200000.0, tkvx4244_ERROR_FREQ);
    if (retError)
      return retError;
  } else {
    retError = tkvx4244_invalidRealRange(*samplingRate, 0.000005, 0.03264, tkvx4244_ERROR_FREQ);
    if (retError)
      return retError;
  }
  if ((!intervalFrequency) && (*samplingRate > 1.0))
    *samplingRate = 1.0 / *samplingRate;
  if ((intervalFrequency == 1) && (*samplingRate < 1.0))
    *samplingRate = 1.0 / *samplingRate;
  /* Parse the gp list */
  tkvx4244_gplist_parse(gp1, gp2, gp3, gp4, allGroups, gplst);

  if (source) {
    /* Build the command string */
    sprintf(cmd, "FREQ:SOUR %s (%s)\n", smpclk[source], gplst);
    /* Send the command string */
    retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
    if (retError)
      return retError;
    /* Build the command string */
    sprintf(cmd, "FREQ:%s%f (%s)\n", freqinval[intervalFrequency], *samplingRate, gplst);
    /* Send the command string */
    retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
    if (retError)
      return retError;
  } else {
    /* Build the command string */
    sprintf(cmd, "FREQ:SOUR %s (%s)\n", smpclk[source], gplst);
    /* Send the command string */
    retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
    if (retError)
      return retError;
  }
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_confFDCTransfer
 *
 * Purpose:
 *    This function sets up the digitizer for FDC protocol.
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *		ViInt16 FDCmode
 *		ViInt16 gp1,ViInt16 gp2,ViInt16 gp3,ViInt16 gp4,ViInt16 allGroups
 *		ViInt32 fdcBufferSize
 *      ViInt32 startingAddress
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC tkvx4244_confFDCTransfer(ViSession instrumentHandle,
					   ViInt16 fdcMode, ViInt16 gp1, ViInt16 gp2, ViInt16 gp3,
					   ViInt16 gp4, ViInt16 allGroups, ViInt32 fdcBufferSize,
					   ViInt32 startingAddress)
{

  ViChar gplst[10];
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  if (retError < VI_SUCCESS)
    return retError;
  drvrHdl = (tkvx4244_PdrvrStruct) userData;
  retError = tkvx4244_invalidShortRange(fdcMode, tkvx4244_FDC_IMM,
					tkvx4244_FDC_OFF, tkvx4244_ERROR_FDC_MODE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp1, tkvx4244_GROUP_ONE_DISABLE,
					tkvx4244_GROUP_ONE_ENABLE, tkvx4244_ERROR_GP_ONE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp2, tkvx4244_GROUP_TWO_DISABLE,
					tkvx4244_GROUP_TWO_ENABLE, tkvx4244_ERROR_GP_TWO);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp3, tkvx4244_GROUP_THREE_DISABLE,
					tkvx4244_GROUP_THREE_ENABLE, tkvx4244_ERROR_GP_THREE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp4, tkvx4244_GROUP_FOUR_DISABLE,
					tkvx4244_GROUP_FOUR_ENABLE, tkvx4244_ERROR_GP_FOUR);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(allGroups, tkvx4244_ALL_GROUPS_DISABLE,
					tkvx4244_ALL_GROUPS_ENABLE, tkvx4244_ERROR_ALL_GROUPS);
  if (retError)
    return retError;
  retError = tkvx4244_invalidLongRange(fdcBufferSize, tkvx4244_FDC_BUF_2K,
				       tkvx4244_FDC_BUF_256K, tkvx4244_ERROR_FDC_BUF_SIZE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidLongRange(startingAddress, -262143, 262145, tkvx4244_ERROR_MEM_ADDR);
  if (retError)
    return retError;

  if (allGroups + gp1 + gp2 + gp3 + gp4 == VI_FALSE)
    return tkvx4244_NO_GRPS_SELECTED_ERROR;

  tkvx4244_gplist_parse(gp1, gp2, gp3, gp4, allGroups, gplst);

  if (fdcMode <= 1)
    sprintf(cmd, "vxi:fdc:mode %s %ld,%ld (%s)\n", fdc_mode[fdcMode],
	    fdcBufferSize, startingAddress, gplst);
  else
    sprintf(cmd, "vxi:fdc:mode %s (%s)\n", fdc_mode[fdcMode], gplst);

  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;

  return VI_SUCCESS;
}

ViStatus _VI_FUNC tkvx4244_getFDCAttribute(ViSession instrumentHandle, ViUInt16 attr[])
{
  ViStatus retError;

  /* 0-7 */
  retError = viGetAttribute(instrumentHandle, VI_ATTR_FDC_CHNL, attr);
  if (retError < VI_SUCCESS)
    return retError;
  /* 1=NORMAL, 2=STREAM */
  retError = viGetAttribute(instrumentHandle, VI_ATTR_FDC_MODE, attr + 1);
  if (retError < VI_SUCCESS)
    return retError;
  /* 0=FALSE, 1=TRUE */
  retError = viGetAttribute(instrumentHandle, VI_ATTR_FDC_USE_PAIR, attr + 2);
  if (retError < VI_SUCCESS)
    return retError;
  /* 0=MEMORY, 1=EXTENDED, 2=MESSAGE, 3=REGISTER, 4=OTHER */
  retError = viGetAttribute(instrumentHandle, VI_ATTR_VXI_DEV_CLASS, attr + 3);
  if (retError < VI_SUCCESS)
    return retError;
  /* 1=A16, 2=A24, 3=A32 */
  retError = viGetAttribute(instrumentHandle, VI_ATTR_MEM_SPACE, attr + 4);
  if (retError < VI_SUCCESS)
    return retError;
  /* 0=FALSE, 1=TRUE */
  retError = viGetAttribute(instrumentHandle, VI_ATTR_DMA_ALLOW_EN, attr + 5);
  if (retError < VI_SUCCESS)
    return retError;

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_getFDCData
 *
 * Purpose:
 *    This function reads FDC data into a short integer array.
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 fdcGroup:
 *  ViInt32 numberOfSamples:
 *  ViInt16 dataArray[]:
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_getFDCData(ViSession instrumentHandle, ViInt16 fdcGroup,
		    ViInt32 numberOfSamples, ViInt16 dataArray[])
{

  ViInt16 intface;
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  if (retError < VI_SUCCESS)
    return retError;
  drvrHdl = (tkvx4244_PdrvrStruct) userData;
  retError = viGetAttribute(instrumentHandle, VI_ATTR_INTF_TYPE, &intface);
  if (retError < VI_SUCCESS)
    return retError;
  if (intface != VI_INTF_VXI)
    return VI_ERROR_INV_ACC_MODE;
  retError = tkvx4244_invalidLongRange(numberOfSamples, 1L, 262144, tkvx4244_NUM_SAMPS_RANGE_ERROR);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(fdcGroup, tkvx4244_GROUP_CHAN_ONE,
					tkvx4244_GROUP_CHAN_FOUR, tkvx4244_ERROR_FDC_CHAN);
  if (retError)
    return retError;
/*	retError = viSetAttribute (instrumentHandle, VI_ATTR_FDC_MODE, VI_FDC_STREAM);
	   if (retError<VI_SUCCESS) return retError;
*/
  retError = viSetAttribute(instrumentHandle, VI_ATTR_FDC_CHNL, fdcGroup);
  if (retError < VI_SUCCESS)
    return retError;
  retError = viSetAttribute(instrumentHandle, VI_ATTR_IO_PROT, VI_FDC);
  if (retError < VI_SUCCESS)
    return retError;

  retError =
      viRead(instrumentHandle, (ViPBuf) dataArray, (2 * numberOfSamples), (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS) {
    viSetAttribute(instrumentHandle, VI_ATTR_IO_PROT, VI_NORMAL);
    return retError;
  }
  retError = viSetAttribute(instrumentHandle, VI_ATTR_IO_PROT, VI_NORMAL);
  if (retError < VI_SUCCESS)
    return retError;
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_readFDCData
 *
 * Purpose:
 *    This function reads FDC data.
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 fdcGroup:
 *  ViInt32 numberOfSamples:
 *  ViChar dataArray[]:
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_readFDCData(ViSession instrumentHandle, ViInt16 fdcGroup,
		     ViInt32 numberOfSamples, ViChar dataArray[])
{

  ViInt16 intface;
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  if (retError < VI_SUCCESS)
    return retError;
  drvrHdl = (tkvx4244_PdrvrStruct) userData;
  retError = viGetAttribute(instrumentHandle, VI_ATTR_INTF_TYPE, &intface);
  if (retError < VI_SUCCESS)
    return retError;
  if (intface != VI_INTF_VXI)
    return VI_ERROR_INV_ACC_MODE;
  retError = tkvx4244_invalidLongRange(numberOfSamples, 1L, 262144, tkvx4244_NUM_SAMPS_RANGE_ERROR);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(fdcGroup, tkvx4244_GROUP_CHAN_ONE,
					tkvx4244_GROUP_CHAN_FOUR, tkvx4244_ERROR_FDC_CHAN);
  if (retError)
    return retError;
  retError = viSetAttribute(instrumentHandle, VI_ATTR_FDC_CHNL, fdcGroup);
  if (retError < VI_SUCCESS)
    return retError;
  retError = viSetAttribute(instrumentHandle, VI_ATTR_IO_PROT, VI_FDC);
  if (retError < VI_SUCCESS)
    return retError;

  retError =
      viRead(instrumentHandle, (ViPBuf) dataArray, (2 * numberOfSamples), (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS) {
    viSetAttribute(instrumentHandle, VI_ATTR_IO_PROT, VI_NORMAL);
    return retError;
  }

  retError = viSetAttribute(instrumentHandle, VI_ATTR_IO_PROT, VI_NORMAL);
  if (retError < VI_SUCCESS)
    return retError;
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_readFDCState
 *
 * Purpose:
 *    This function reads FDC state information.
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 stateQuery, gp1,gp2,gp3,gp4,all:
 *  ViChar fdcQueryResponse[]:
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_readFDCState(ViSession instrumentHandle, ViInt16 stateQuery, ViInt16 gp1,
		      ViInt16 gp2, ViInt16 gp3, ViInt16 gp4, ViInt16 all, ViChar fdcQueryResponse[])
{
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViChar rcv[tkvx4244_RCV_STRLEN];
  ViChar gplst[10];
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  if (retError < VI_SUCCESS)
    return retError;
  drvrHdl = (tkvx4244_PdrvrStruct) userData;
  retError = tkvx4244_invalidShortRange(stateQuery, tkvx4244_FDC_QUERY_MODE,
					tkvx4244_FDC_QUERY_QUE, tkvx4244_ERROR_FDC_QUERY);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp1, tkvx4244_GROUP_ONE_DISABLE,
					tkvx4244_GROUP_ONE_ENABLE, tkvx4244_ERROR_GP_ONE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp2, tkvx4244_GROUP_TWO_DISABLE,
					tkvx4244_GROUP_TWO_ENABLE, tkvx4244_ERROR_GP_TWO);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp3, tkvx4244_GROUP_THREE_DISABLE,
					tkvx4244_GROUP_THREE_ENABLE, tkvx4244_ERROR_GP_THREE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp4, tkvx4244_GROUP_FOUR_DISABLE,
					tkvx4244_GROUP_FOUR_ENABLE, tkvx4244_ERROR_GP_FOUR);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(all, tkvx4244_ALL_GROUPS_DISABLE,
					tkvx4244_ALL_GROUPS_ENABLE, tkvx4244_ERROR_ALL_GROUPS);
  if (retError)
    return retError;
  if (all + gp1 + gp2 + gp3 + gp4 == VI_FALSE)
    return tkvx4244_NO_GRPS_SELECTED_ERROR;
  tkvx4244_gplist_parse(gp1, gp2, gp3, gp4, all, gplst);

  sprintf(cmd, "VXI:FDC:%s? (%s)", fdcQuery[stateQuery], gplst);
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;

  retError = viRead(instrumentHandle, (ViPBuf) rcv, tkvx4244_RCV_STRLEN, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  rcv[retCnt] = '\0';
  strcpy(fdcQueryResponse, rcv);

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_confGroupOneChannels
 *
 * Purpose:
 *    This function enables or disables selected channels
 *    in group 1.
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *		ViInt16 select, ch1-ch4
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_confGroupOneChannels(ViSession instrumentHandle,
			      ViInt16 ch1, ViInt16 ch2, ViInt16 ch3, ViInt16 ch4, ViInt16 select)
{
  ViChar cond[6];
  ViChar chan[40];
  ViInt16 chan1, chan2, chan3, chan4;
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAddr userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate the command */
  retError =
      tkvx4244_invalidShortRange(select, tkvx4244_SELECT_DISABLE, tkvx4244_SELECT_ENABLE,
				 tkvx4244_ERROR_OPEN_CLOSE);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch1, tkvx4244_CH_ONE_DISABLE, tkvx4244_CH_ONE_ENABLE,
				 tkvx4244_ERROR_CHAN_ONE);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch2, tkvx4244_CH_TWO_DISABLE, tkvx4244_CH_TWO_ENABLE,
				 tkvx4244_ERROR_CHAN_TWO);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch3, tkvx4244_CH_THREE_DISABLE, tkvx4244_CH_THREE_ENABLE,
				 tkvx4244_ERROR_CHAN_THREE);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch4, tkvx4244_CH_FOUR_DISABLE, tkvx4244_CH_FOUR_ENABLE,
				 tkvx4244_ERROR_CHAN_FOUR);
  if (retError)
    return retError;
/* no channels chosen */
  if ((ch1 + ch2 + ch3 + ch4) == VI_FALSE)
    return tkvx4244_NO_CHAN_SELECTED_ERROR;
  chan1 = drvrHdl->tkvx4244_chOneActive;
  chan2 = drvrHdl->tkvx4244_chTwoActive;
  chan3 = drvrHdl->tkvx4244_chThreeActive;
  chan4 = drvrHdl->tkvx4244_chFourActive;

  if ((ch1 == tkvx4244_CH_ONE_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    chan1 = tkvx4244_CH_ONE_ENABLE;
  if (ch1 == tkvx4244_CH_ONE_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    chan1 = tkvx4244_CH_ONE_DISABLE;

  if ((ch2 == tkvx4244_CH_TWO_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    chan2 = tkvx4244_CH_TWO_ENABLE;
  if (ch2 == tkvx4244_CH_TWO_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    chan2 = tkvx4244_CH_TWO_DISABLE;

  if ((ch3 == tkvx4244_CH_THREE_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    chan3 = tkvx4244_CH_THREE_ENABLE;
  if (ch3 == tkvx4244_CH_THREE_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    chan3 = tkvx4244_CH_THREE_DISABLE;

  if ((ch4 == tkvx4244_CH_FOUR_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    chan4 = tkvx4244_CH_FOUR_ENABLE;
  if (ch4 == tkvx4244_CH_FOUR_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    chan4 = tkvx4244_CH_FOUR_DISABLE;
  if ((chan1 + chan2 + chan3 + chan4) == 3)
    return tkvx4244_THREE_CHAN_IN_GP_ERROR;
/* Parse the ch list */
  tkvx4244_chlst_parse(ch1, ch2, ch3, ch4, VI_FALSE, VI_FALSE,
		       VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE,
		       VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE, chan);
  /* Build the command string */
  if (select == tkvx4244_SELECT_ENABLE)
    sprintf(cond, "CLOSE");
  else
    sprintf(cond, "OPEN");
  sprintf(cmd, "ROUTE:%s (%s)\n", cond, chan);
  cmd[strlen(cmd)] = '\0';
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;

  if ((ch1 == tkvx4244_CH_ONE_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    drvrHdl->tkvx4244_chOneActive = tkvx4244_CH_ONE_ENABLE;
  if (ch1 == tkvx4244_CH_ONE_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    drvrHdl->tkvx4244_chOneActive = tkvx4244_CH_ONE_DISABLE;

  if ((ch2 == tkvx4244_CH_TWO_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    drvrHdl->tkvx4244_chTwoActive = tkvx4244_CH_TWO_ENABLE;
  if (ch2 == tkvx4244_CH_TWO_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    drvrHdl->tkvx4244_chTwoActive = tkvx4244_CH_TWO_DISABLE;

  if ((ch3 == tkvx4244_CH_THREE_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    drvrHdl->tkvx4244_chThreeActive = tkvx4244_CH_THREE_ENABLE;
  if (ch3 == tkvx4244_CH_THREE_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    drvrHdl->tkvx4244_chThreeActive = tkvx4244_CH_THREE_DISABLE;

  if ((ch4 == tkvx4244_CH_FOUR_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    drvrHdl->tkvx4244_chFourActive = tkvx4244_CH_FOUR_ENABLE;
  if (ch4 == tkvx4244_CH_FOUR_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    drvrHdl->tkvx4244_chFourActive = tkvx4244_CH_FOUR_DISABLE;

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_confGroupTwoChannels
 *
 * Purpose:
 *    This function enables or disables selected channels
 *    in group 2.
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *		ViInt16 select, ch5-ch8
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_confGroupTwoChannels(ViSession instrumentHandle,
			      ViInt16 ch5, ViInt16 ch6, ViInt16 ch7, ViInt16 ch8, ViInt16 select)
{
  ViChar cond[6];
  ViChar chan[40];
  ViInt16 chan5, chan6, chan7, chan8;
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAddr userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate the command */
  retError =
      tkvx4244_invalidShortRange(select, tkvx4244_SELECT_DISABLE, tkvx4244_SELECT_ENABLE,
				 tkvx4244_ERROR_OPEN_CLOSE);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch5, tkvx4244_CH_FIVE_DISABLE, tkvx4244_CH_FIVE_ENABLE,
				 tkvx4244_ERROR_CHAN_FIVE);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch6, tkvx4244_CH_SIX_DISABLE, tkvx4244_CH_SIX_ENABLE,
				 tkvx4244_ERROR_CHAN_SIX);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch7, tkvx4244_CH_SEVEN_DISABLE, tkvx4244_CH_SEVEN_ENABLE,
				 tkvx4244_ERROR_CHAN_SEVEN);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch8, tkvx4244_CH_EIGHT_DISABLE, tkvx4244_CH_EIGHT_ENABLE,
				 tkvx4244_ERROR_CHAN_EIGHT);
  if (retError)
    return retError;
/* no channels chosen */
  if ((ch5 + ch6 + ch7 + ch8) == VI_FALSE)
    return tkvx4244_NO_CHAN_SELECTED_ERROR;
  chan5 = drvrHdl->tkvx4244_chFiveActive;
  chan6 = drvrHdl->tkvx4244_chSixActive;
  chan7 = drvrHdl->tkvx4244_chSevenActive;
  chan8 = drvrHdl->tkvx4244_chEightActive;

  if ((ch5 == tkvx4244_CH_FIVE_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    chan5 = tkvx4244_CH_FIVE_ENABLE;
  if (ch5 == tkvx4244_CH_FIVE_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    chan5 = tkvx4244_CH_FIVE_DISABLE;

  if ((ch6 == tkvx4244_CH_SIX_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    chan6 = tkvx4244_CH_SIX_ENABLE;
  if (ch6 == tkvx4244_CH_SIX_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    chan6 = tkvx4244_CH_SIX_DISABLE;

  if ((ch7 == tkvx4244_CH_SEVEN_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    chan7 = tkvx4244_CH_SEVEN_ENABLE;
  if (ch7 == tkvx4244_CH_SEVEN_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    chan7 = tkvx4244_CH_SEVEN_DISABLE;

  if ((ch8 == tkvx4244_CH_EIGHT_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    chan8 = tkvx4244_CH_EIGHT_ENABLE;
  if (ch8 == tkvx4244_CH_EIGHT_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    chan8 = tkvx4244_CH_EIGHT_DISABLE;

  if ((chan5 + chan6 + chan7 + chan8) == 3)
    return tkvx4244_THREE_CHAN_IN_GP_ERROR;

  /* Parse the ch list */
  tkvx4244_chlst_parse(VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE,
		       ch5, ch6, ch7, ch8, VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE,
		       VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE, chan);
  /* Build the command string */
  if (select == 1)
    sprintf(cond, "CLOSE");
  else
    sprintf(cond, "OPEN");
  sprintf(cmd, "ROUTE:%s (%s)\n", cond, chan);
  cmd[strlen(cmd)] = '\0';
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;

  if ((ch5 == tkvx4244_CH_FIVE_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    drvrHdl->tkvx4244_chFiveActive = tkvx4244_CH_FIVE_ENABLE;
  if (ch5 == tkvx4244_CH_FIVE_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    drvrHdl->tkvx4244_chFiveActive = tkvx4244_CH_FIVE_DISABLE;

  if ((ch6 == tkvx4244_CH_SIX_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    drvrHdl->tkvx4244_chSixActive = tkvx4244_CH_SIX_ENABLE;
  if (ch6 == tkvx4244_CH_SIX_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    drvrHdl->tkvx4244_chSixActive = tkvx4244_CH_SIX_DISABLE;

  if ((ch7 == tkvx4244_CH_SEVEN_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    drvrHdl->tkvx4244_chSevenActive = tkvx4244_CH_SEVEN_ENABLE;
  if (ch7 == tkvx4244_CH_SEVEN_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    drvrHdl->tkvx4244_chSevenActive = tkvx4244_CH_SEVEN_DISABLE;

  if ((ch8 == tkvx4244_CH_EIGHT_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    drvrHdl->tkvx4244_chEightActive = tkvx4244_CH_EIGHT_ENABLE;
  if (ch8 == tkvx4244_CH_EIGHT_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    drvrHdl->tkvx4244_chEightActive = tkvx4244_CH_EIGHT_DISABLE;
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_confGroupThreeChannels
 *
 * Purpose:
 *    This function enables or disables selected channels
 *    in group 3.
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *		ViInt16 select, ch9-ch12
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_confGroupThreeChannels(ViSession instrumentHandle,
				ViInt16 ch9, ViInt16 ch10, ViInt16 ch11, ViInt16 ch12,
				ViInt16 select)
{
  ViChar cond[6];
  ViChar chan[40];
  ViInt16 chan9, chan10, chan11, chan12;
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAddr userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate the command */
  retError =
      tkvx4244_invalidShortRange(select, tkvx4244_SELECT_DISABLE, tkvx4244_SELECT_ENABLE,
				 tkvx4244_ERROR_OPEN_CLOSE);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch9, tkvx4244_CH_NINE_DISABLE, tkvx4244_CH_NINE_ENABLE,
				 tkvx4244_ERROR_CHAN_NINE);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch10, tkvx4244_CH_TEN_DISABLE, tkvx4244_CH_TEN_ENABLE,
				 tkvx4244_ERROR_CHAN_TEN);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch11, tkvx4244_CH_ELEVEN_DISABLE, tkvx4244_CH_ELEVEN_ENABLE,
				 tkvx4244_ERROR_CHAN_ELEVEN);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch12, tkvx4244_CH_TWELVE_DISABLE, tkvx4244_CH_TWELVE_ENABLE,
				 tkvx4244_ERROR_CHAN_TWELVE);
  if (retError)
    return retError;
/* no channels chosen */
  if ((ch9 + ch10 + ch11 + ch12) == VI_FALSE)
    return tkvx4244_NO_CHAN_SELECTED_ERROR;

  chan9 = drvrHdl->tkvx4244_chNineActive;
  chan10 = drvrHdl->tkvx4244_chTenActive;
  chan11 = drvrHdl->tkvx4244_chElevenActive;
  chan12 = drvrHdl->tkvx4244_chTwelveActive;

  if ((ch9 == tkvx4244_CH_NINE_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    chan9 = tkvx4244_CH_NINE_ENABLE;
  if (ch9 == tkvx4244_CH_NINE_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    chan9 = tkvx4244_CH_NINE_DISABLE;

  if ((ch10 == tkvx4244_CH_TEN_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    chan10 = tkvx4244_CH_TEN_ENABLE;
  if (ch10 == tkvx4244_CH_TEN_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    chan10 = tkvx4244_CH_TEN_DISABLE;

  if ((ch11 == tkvx4244_CH_ELEVEN_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    chan11 = tkvx4244_CH_ELEVEN_ENABLE;
  if (ch11 == tkvx4244_CH_ELEVEN_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    chan11 = tkvx4244_CH_ELEVEN_DISABLE;

  if ((ch12 == tkvx4244_CH_TWELVE_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    chan12 = tkvx4244_CH_TWELVE_ENABLE;
  if (ch12 == tkvx4244_CH_TWELVE_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    chan12 = tkvx4244_CH_TWELVE_DISABLE;

  if ((chan9 + chan10 + chan11 + chan12) == 3)
    return tkvx4244_THREE_CHAN_IN_GP_ERROR;

  /* Parse the ch list */
  tkvx4244_chlst_parse(VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE,
		       VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE, ch9, ch10, ch11, ch12,
		       VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE, chan);
  /* Build the command string */
  if (select == 1)
    sprintf(cond, "CLOSE");
  else
    sprintf(cond, "OPEN");
  sprintf(cmd, "ROUTE:%s (%s)\n", cond, chan);
  cmd[strlen(cmd)] = '\0';
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;

  if ((ch9 == tkvx4244_CH_NINE_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    drvrHdl->tkvx4244_chNineActive = tkvx4244_CH_NINE_ENABLE;
  if (ch9 == tkvx4244_CH_NINE_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    drvrHdl->tkvx4244_chNineActive = tkvx4244_CH_NINE_DISABLE;

  if ((ch10 == tkvx4244_CH_TEN_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    drvrHdl->tkvx4244_chTenActive = tkvx4244_CH_TEN_ENABLE;
  if (ch10 == tkvx4244_CH_TEN_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    drvrHdl->tkvx4244_chTenActive = tkvx4244_CH_TEN_DISABLE;

  if ((ch11 == tkvx4244_CH_ELEVEN_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    drvrHdl->tkvx4244_chElevenActive = tkvx4244_CH_ELEVEN_ENABLE;
  if (ch11 == tkvx4244_CH_ELEVEN_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    drvrHdl->tkvx4244_chElevenActive = tkvx4244_CH_ELEVEN_DISABLE;

  if ((ch12 == tkvx4244_CH_TWELVE_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    drvrHdl->tkvx4244_chTwelveActive = tkvx4244_CH_TWELVE_ENABLE;
  if (ch12 == tkvx4244_CH_TWELVE_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    drvrHdl->tkvx4244_chTwelveActive = tkvx4244_CH_TWELVE_DISABLE;

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_confGroupFourChannels
 *
 * Purpose:
 *    This function enables or disables selected channels
 *    in group 4.
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *		ViInt16 select, ch13-ch16
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_confGroupFourChannels(ViSession instrumentHandle,
			       ViInt16 ch13, ViInt16 ch14, ViInt16 ch15, ViInt16 ch16,
			       ViInt16 select)
{
  ViChar cond[6];
  ViChar chan[40];
  ViInt16 chan13, chan14, chan15, chan16;
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAddr userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate the command */
  retError =
      tkvx4244_invalidShortRange(select, tkvx4244_SELECT_DISABLE, tkvx4244_SELECT_ENABLE,
				 tkvx4244_ERROR_OPEN_CLOSE);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch13, tkvx4244_CH_THIRTEEN_DISABLE, tkvx4244_CH_THIRTEEN_ENABLE,
				 tkvx4244_ERROR_CHAN_THIRTEEN);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch14, tkvx4244_CH_FOURTEEN_DISABLE, tkvx4244_CH_FOURTEEN_ENABLE,
				 tkvx4244_ERROR_CHAN_FOURTEEN);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch15, tkvx4244_CH_FIFTEEN_DISABLE, tkvx4244_CH_FIFTEEN_ENABLE,
				 tkvx4244_ERROR_CHAN_FIFTEEN);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch16, tkvx4244_CH_SIXTEEN_DISABLE, tkvx4244_CH_SIXTEEN_ENABLE,
				 tkvx4244_ERROR_CHAN_SIXTEEN);
  if (retError)
    return retError;
/* no channels chosen */
  if ((ch13 + ch14 + ch15 + ch16) == VI_FALSE)
    return tkvx4244_NO_CHAN_SELECTED_ERROR;

  chan13 = drvrHdl->tkvx4244_chThirteenActive;
  chan14 = drvrHdl->tkvx4244_chFourteenActive;
  chan15 = drvrHdl->tkvx4244_chFifteenActive;
  chan16 = drvrHdl->tkvx4244_chSixteenActive;

  if ((ch13 == tkvx4244_CH_NINE_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    chan13 = tkvx4244_CH_NINE_ENABLE;
  if (ch13 == tkvx4244_CH_NINE_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    chan13 = tkvx4244_CH_NINE_DISABLE;

  if ((ch14 == tkvx4244_CH_FOURTEEN_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    chan14 = tkvx4244_CH_FOURTEEN_ENABLE;
  if (ch14 == tkvx4244_CH_FOURTEEN_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    chan14 = tkvx4244_CH_FOURTEEN_DISABLE;

  if ((ch15 == tkvx4244_CH_FIFTEEN_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    chan15 = tkvx4244_CH_FIFTEEN_ENABLE;
  if (ch15 == tkvx4244_CH_FIFTEEN_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    chan15 = tkvx4244_CH_FIFTEEN_DISABLE;

  if ((ch16 == tkvx4244_CH_SIXTEEN_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    chan16 = tkvx4244_CH_SIXTEEN_ENABLE;
  if (ch16 == tkvx4244_CH_SIXTEEN_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    chan16 = tkvx4244_CH_SIXTEEN_DISABLE;

  if ((chan13 + chan14 + chan15 + chan16) == 3)
    return tkvx4244_THREE_CHAN_IN_GP_ERROR;

  /* Parse the ch list */
  tkvx4244_chlst_parse(VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE,
		       VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE, VI_FALSE,
		       VI_FALSE, VI_FALSE, ch13, ch14, ch15, ch16, VI_FALSE, chan);
  /* Build the command string */
  if (select == 1)
    sprintf(cond, "CLOSE");
  else
    sprintf(cond, "OPEN");
  sprintf(cmd, "ROUTE:%s (%s)\n", cond, chan);
  cmd[strlen(cmd)] = '\0';
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;

  if ((ch13 == tkvx4244_CH_THIRTEEN_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    drvrHdl->tkvx4244_chThirteenActive = tkvx4244_CH_THIRTEEN_ENABLE;
  if (ch13 == tkvx4244_CH_THIRTEEN_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    drvrHdl->tkvx4244_chThirteenActive = tkvx4244_CH_THIRTEEN_DISABLE;

  if ((ch14 == tkvx4244_CH_FOURTEEN_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    drvrHdl->tkvx4244_chFourteenActive = tkvx4244_CH_FOURTEEN_ENABLE;
  if (ch14 == tkvx4244_CH_FOURTEEN_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    drvrHdl->tkvx4244_chFourteenActive = tkvx4244_CH_FOURTEEN_DISABLE;

  if ((ch15 == tkvx4244_CH_FIFTEEN_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    drvrHdl->tkvx4244_chFifteenActive = tkvx4244_CH_FIFTEEN_ENABLE;
  if (ch15 == tkvx4244_CH_FIFTEEN_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    drvrHdl->tkvx4244_chFifteenActive = tkvx4244_CH_FIFTEEN_DISABLE;

  if ((ch16 == tkvx4244_CH_SIXTEEN_ENABLE) && (select == tkvx4244_SELECT_ENABLE))
    drvrHdl->tkvx4244_chSixteenActive = tkvx4244_CH_SIXTEEN_ENABLE;
  if (ch16 == tkvx4244_CH_SIXTEEN_ENABLE && (select == tkvx4244_SELECT_DISABLE))
    drvrHdl->tkvx4244_chSixteenActive = tkvx4244_CH_SIXTEEN_DISABLE;
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_chanlist
 *
 * Purpose:
 *	 NOTE: This Function is an "Exception Function" and is not
 *compliant with VXIplu&play standards because of the number of
 *parameters in the function. This function is being maintained in
 *this version of the VXIplug&play driver to provide compatibility
 *with existing applications. New functions have been written to
 *replace this function.  There will not be a function panel for
 *this function in WIN Framework 4.0 compliant drivers.
 *
 *    This function enables or disables selected channels.
 *    This function is an 'Exception Function'.
 *    Maintain for compatibility with existing applications.
 *    include in dll but not in function panel file.
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *		ViInt16 opn_cls, c1-c16, ca
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC tkvx4244_chanlist(ViSession instrumentHandle, ViInt16 select, ViInt16 ch1,
				    ViInt16 ch2, ViInt16 ch3, ViInt16 ch4, ViInt16 ch5, ViInt16 ch6,
				    ViInt16 ch7, ViInt16 ch8, ViInt16 ch9, ViInt16 ch10,
				    ViInt16 ch11, ViInt16 ch12, ViInt16 ch13, ViInt16 ch14,
				    ViInt16 ch15, ViInt16 ch16, ViInt16 all)
{
  ViChar cond[6];
  ViChar chan[40];
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAddr userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate the command */
  retError =
      tkvx4244_invalidShortRange(select, tkvx4244_SELECT_DISABLE, tkvx4244_SELECT_ENABLE,
				 tkvx4244_ERROR_OPEN_CLOSE);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch1, tkvx4244_CH_ONE_DISABLE, tkvx4244_CH_ONE_ENABLE,
				 tkvx4244_ERROR_CHAN_ONE);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch2, tkvx4244_CH_TWO_DISABLE, tkvx4244_CH_TWO_ENABLE,
				 tkvx4244_ERROR_CHAN_TWO);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch3, tkvx4244_CH_THREE_DISABLE, tkvx4244_CH_THREE_ENABLE,
				 tkvx4244_ERROR_CHAN_THREE);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch4, tkvx4244_CH_FOUR_DISABLE, tkvx4244_CH_FOUR_ENABLE,
				 tkvx4244_ERROR_CHAN_FOUR);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch5, tkvx4244_CH_FIVE_DISABLE, tkvx4244_CH_FIVE_ENABLE,
				 tkvx4244_ERROR_CHAN_FIVE);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch6, tkvx4244_CH_SIX_DISABLE, tkvx4244_CH_SIX_ENABLE,
				 tkvx4244_ERROR_CHAN_SIX);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch7, tkvx4244_CH_SEVEN_DISABLE, tkvx4244_CH_SEVEN_ENABLE,
				 tkvx4244_ERROR_CHAN_SEVEN);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch8, tkvx4244_CH_EIGHT_DISABLE, tkvx4244_CH_EIGHT_ENABLE,
				 tkvx4244_ERROR_CHAN_EIGHT);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch9, tkvx4244_CH_NINE_DISABLE, tkvx4244_CH_NINE_ENABLE,
				 tkvx4244_ERROR_CHAN_NINE);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch10, tkvx4244_CH_TEN_DISABLE, tkvx4244_CH_TEN_ENABLE,
				 tkvx4244_ERROR_CHAN_TEN);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch11, tkvx4244_CH_ELEVEN_DISABLE, tkvx4244_CH_ELEVEN_ENABLE,
				 tkvx4244_ERROR_CHAN_ELEVEN);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch12, tkvx4244_CH_TWELVE_DISABLE, tkvx4244_CH_TWELVE_ENABLE,
				 tkvx4244_ERROR_CHAN_TWELVE);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch13, tkvx4244_CH_THIRTEEN_DISABLE, tkvx4244_CH_THIRTEEN_ENABLE,
				 tkvx4244_ERROR_CHAN_THIRTEEN);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch14, tkvx4244_CH_FOURTEEN_DISABLE, tkvx4244_CH_FOURTEEN_ENABLE,
				 tkvx4244_ERROR_CHAN_FOURTEEN);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch15, tkvx4244_CH_FIFTEEN_DISABLE, tkvx4244_CH_FIFTEEN_ENABLE,
				 tkvx4244_ERROR_CHAN_FIFTEEN);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(ch16, tkvx4244_CH_SIXTEEN_DISABLE, tkvx4244_CH_SIXTEEN_ENABLE,
				 tkvx4244_ERROR_CHAN_SIXTEEN);
  if (retError)
    return retError;
  retError =
      tkvx4244_invalidShortRange(all, tkvx4244_DONT_SELECT_ALL_CHANNELS,
				 tkvx4244_SELECT_ALL_CHANNELS, tkvx4244_ERROR_ALL_CHAN);
  if (retError)
    return retError;

  /* no channels chosen */
  if ((all + ch1 + ch2 + ch3 + ch4 + ch5 + ch6 + ch7 + ch8 + ch9 + ch10 + ch11 + ch12 + ch13 +
       ch14 + ch15 + ch16) == VI_FALSE)
    return tkvx4244_NO_CHAN_SELECTED_ERROR;
  /* attempt to enable 3 channels in a group */
  if (((ch1 + ch2 + ch3 + ch4) == 3) || ((ch5 + ch6 + ch7 + ch8) == 3)
      || ((ch9 + ch10 + ch11 + ch12) == 3) || ((ch13 + ch14 + ch15 + ch16) == 3))
    return tkvx4244_THREE_CHAN_IN_GP_ERROR;
  /* Parse the ch list */
  tkvx4244_chlst_parse(ch1, ch2, ch3, ch4, ch5, ch6, ch7, ch8, ch9, ch10, ch11, ch12, ch13, ch14,
		       ch15, ch16, all, chan);
  /* Build the command string */
  if (select == 1)
    sprintf(cond, "CLOSE");
  else
    sprintf(cond, "OPEN");
  sprintf(cmd, "ROUTE:%s (%s)\n", cond, chan);
  cmd[strlen(cmd)] = '\0';
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;

  return VI_SUCCESS;
}

ViStatus _VI_FUNC tkvx4244_arm(ViSession instrumentHandle, ViUInt16 Protocol)
{
  ViStatus retError;
  ViInt16 trig_id;
  ViChar buf[256];
//      retError = viSetAttribute (handle, VI_ATTR_TRIG_ID, VI_TRIG_SW);
  retError = viGetAttribute(instrumentHandle, VI_ATTR_TRIG_ID, &trig_id);
  printf("After get Attribute %d\n", retError);
  printf("TRIG ID: %d\n", trig_id);
  if (retError) {
    viStatusDesc(instrumentHandle, retError, buf);
    printf("%s\n", buf);
    return retError;
  }
  retError = viAssertTrigger(instrumentHandle, Protocol);
  return retError;
}

/***************************************************************************
 * Function: tkvx4244_initMeasurementCycle
 *
 * Purpose:
 *  This function Begins the collection process on the designated
 *  groups.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *      ViReal64 delay - sets a delay before digitizing if desired.
 *		ViInt16 gp1-gp4,all,initiate
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_initMeasurementCycle(ViSession instrumentHandle,
			      ViReal64 * delay, ViInt16 gp1, ViInt16 gp2, ViInt16 gp3, ViInt16 gp4,
			      ViInt16 all, ViInt16 initiate)
{
  ViChar gplst[10];
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate the command */
  retError = tkvx4244_invalidRealRange(*delay, 0L, 1.0E9, tkvx4244_ERROR_INIT_DELAY);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(initiate, tkvx4244_INIT_IMM,
					tkvx4244_INIT_CONT, tkvx4244_ERROR_INIT_TYPE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp1, tkvx4244_GROUP_ONE_DISABLE,
					tkvx4244_GROUP_ONE_ENABLE, tkvx4244_ERROR_GP_ONE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp2, tkvx4244_GROUP_TWO_DISABLE,
					tkvx4244_GROUP_TWO_ENABLE, tkvx4244_ERROR_GP_TWO);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp3, tkvx4244_GROUP_THREE_DISABLE,
					tkvx4244_GROUP_THREE_ENABLE, tkvx4244_ERROR_GP_THREE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp4, tkvx4244_GROUP_FOUR_DISABLE,
					tkvx4244_GROUP_FOUR_ENABLE, tkvx4244_ERROR_GP_FOUR);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(all, tkvx4244_ALL_GROUPS_DISABLE,
					tkvx4244_ALL_GROUPS_ENABLE, tkvx4244_ERROR_ALL_GROUPS);
  if (retError)
    return retError;
  if (all + gp1 + gp2 + gp3 + gp4 == VI_FALSE)
    return tkvx4244_NO_GRPS_SELECTED_ERROR;
  /* Parse the gp list */
  tkvx4244_gplist_parse(gp1, gp2, gp3, gp4, all, gplst);
  /* Build the command string */
  switch (initiate) {
  case 0:
    sprintf(cmd, "INIT:IMM (%s)\n", gplst);
    break;
  case 1:
    sprintf(cmd, "INIT:DELAY%f(%s)\n", *delay, gplst);
    break;
  case 2:
    sprintf(cmd, "INIT:CONT(%s)\n", gplst);
    break;
  }
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_confDataCollectCount
 *
 * Purpose:
 *  This function sets up various parameters relating to the collection of
 *  data. The number of samples to collect and the group this number applies
 *  to is defined. Long count is the number of samples and char gplst
 *  the group or groups.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt32 numberOfSamples-number of samples.
 *  ViInt16 gp1-gp4,allGroups
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_confDataCollectCount(ViSession instrumentHandle,
			      ViInt32 numberOfSamples, ViInt16 gp1, ViInt16 gp2, ViInt16 gp3,
			      ViInt16 gp4, ViInt16 allGroups)
{
  ViChar gplst[10];
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate the command */
  retError = tkvx4244_invalidLongRange(numberOfSamples, 1L, 262144, tkvx4244_NUM_SAMPS_RANGE_ERROR);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp1, tkvx4244_GROUP_ONE_DISABLE,
					tkvx4244_GROUP_ONE_ENABLE, tkvx4244_ERROR_GP_ONE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp2, tkvx4244_GROUP_TWO_DISABLE,
					tkvx4244_GROUP_TWO_ENABLE, tkvx4244_ERROR_GP_TWO);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp3, tkvx4244_GROUP_THREE_DISABLE,
					tkvx4244_GROUP_THREE_ENABLE, tkvx4244_ERROR_GP_THREE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp4, tkvx4244_GROUP_FOUR_DISABLE,
					tkvx4244_GROUP_FOUR_ENABLE, tkvx4244_ERROR_GP_FOUR);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(allGroups, tkvx4244_ALL_GROUPS_DISABLE,
					tkvx4244_ALL_GROUPS_ENABLE, tkvx4244_ERROR_ALL_GROUPS);
  if (retError)
    return retError;

  if (allGroups + gp1 + gp2 + gp3 + gp4 == VI_FALSE)
    return tkvx4244_NO_GRPS_SELECTED_ERROR;
  /* Parse the gp list */
  tkvx4244_gplist_parse(gp1, gp2, gp3, gp4, allGroups, gplst);
  /* Build the command string */
  sprintf(cmd, "SWEEP:POINTS %ld(%s)\n", numberOfSamples, gplst);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;

  return VI_SUCCESS;

}

/***************************************************************************
 * Function: tkvx4244_confExternalArmSlope
 *
 * Purpose:
 *  This function programs the active edge of the external arm input.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 slope
 *  ViChar slopeVerification[]
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_confExternalArmSlope(ViSession instrumentHandle, ViInt16 slope, ViChar slopeVerification[])
{
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViChar rcv[tkvx4244_RCV_STRLEN];	/* storage for instrument receive strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;
  /*validate command */
  retError = tkvx4244_invalidShortRange(slope, tkvx4244_SLOPE_POS,
					tkvx4244_SLOPE_NEG, tkvx4244_ERROR_EXT_ARM_CMD);
  if (retError)
    return retError;
  /* Build the command string */
  sprintf(cmd, "ARM:%s\n", arm_slopZ[slope]);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Build the command string */
  sprintf(cmd, "ARM:SLOPE?\n");
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Get the response from the instrument */
  retError = viRead(instrumentHandle, (ViPBuf) rcv, tkvx4244_RCV_STRLEN, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  rcv[retCnt] = '\0';
  strcpy(slopeVerification, rcv);

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_readMasterOutTrigger
 *
 * Purpose:
 *  This function queries the instrument for output trig setups.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 outputTrigger
 *  ViChar masterOutputtrigger[]
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_readMasterOutTrigger(ViSession instrumentHandle,
			      ViInt16 outputTrigger, ViChar masterOutputtrigger[])
{
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViChar rcv[tkvx4244_RCV_STRLEN];	/* storage for instrument receive strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;
  /*Validate the command */
  retError = tkvx4244_invalidShortRange(outputTrigger, tkvx4244_OUT_QUERY_SLOPE,
					tkvx4244_OUT_QUERY_MASK, tkvx4244_ERROR_TRIG_OUT_QUERY);
  if (retError)
    return retError;

  /* Build the command string */
  sprintf(cmd, "OUTP:%s\n", out_trig_qu[outputTrigger]);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Get the response from the instrument */
  retError = viRead(instrumentHandle, (ViPBuf) rcv, tkvx4244_RCV_STRLEN, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  rcv[retCnt] = '\0';
  strcpy(masterOutputtrigger, rcv);

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_confMasterClock
 *
 * Purpose:
 *  This function defines the master sample rate clock
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViReal64 frequency
 *  ViInt16 source
 *  ViChar masterClockSource[]
 *  ViChar masterClockFrequency[]
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_confMasterClock(ViSession instrumentHandle,
			 ViReal64 frequency, ViInt16 source, ViChar masterClockSource[],
			 ViChar masterClockFrequency[])
{
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViChar rcv[tkvx4244_RCV_STRLEN];	/* storage for instrument receive strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  /*Validate the Command */
  retError = tkvx4244_invalidShortRange(source, tkvx4244_CLOCK_EXTERNAL,
					tkvx4244_CLOCK_INTERNAL, tkvx4244_ERROR_MASTER_CLK_SRC);
  if (retError)
    return retError;

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  if (!source) {
    if (tkvx4244_invalidRealRange(frequency, 30.6372, 2000000.0, tkvx4244_FREQ_RANGE_ERROR) != 0)
      return retError;
  } else
      if (tkvx4244_invalidRealRange
	  (frequency, 76.59310000000001, 5000000.0, tkvx4244_FREQ_RANGE_ERROR) != 0)
    return retError;

  /* Build the command string */
  sprintf(cmd, "ROSC:SOUR %s\n", mstr[source]);

  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;

  /* Build the command string */
  sprintf(cmd, "ROSC:FREQ %f\n", frequency);

  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;

  /* Build the command string */
  sprintf(cmd, "ROSC:SOUR?\n");

  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;

  /* Get the response from the instrument */
  retError = viRead(instrumentHandle, (ViPBuf) rcv, tkvx4244_RCV_STRLEN, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  rcv[retCnt] = '\0';
  strcpy(masterClockSource, rcv);

  /* Build the command string */
  sprintf(cmd, "ROSC:FREQ?");

  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;

  /* Get the response from the instrument */
  retError = viRead(instrumentHandle, (ViPBuf) rcv, tkvx4244_RCV_STRLEN, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  rcv[retCnt] = '\0';
  strcpy(masterClockFrequency, rcv);

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_readTrigThreshold
 *
 * Purpose:
 *  This function queries trigger threshold settings.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *
 *  ViChar threshold[]
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC tkvx4244_readTrigThreshold(ViSession instrumentHandle, ViChar threshold[])
{
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViChar rcv[tkvx4244_RCV_STRLEN];	/* storage for instrument receive strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;
  /* Build the command string */
  sprintf(cmd, "TRIG:THRE?;");
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;

  retError = viRead(instrumentHandle, (ViPBuf) rcv, tkvx4244_RCV_STRLEN, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  rcv[retCnt] = '\0';
  strcpy(threshold, rcv);

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_readTrigSrcAndLogic
 *
 * Purpose:
 *  This function queries various trigger parameters for logic and source.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 sourceAndLogic
 *  ViInt16 gp1-gp4,all
 *  ViChar triggerSettings[]
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_readTrigSrcAndLogic(ViSession instrumentHandle,
			     ViInt16 sourceAndLogic, ViInt16 gp1, ViInt16 gp2, ViInt16 gp3,
			     ViInt16 gp4, ViInt16 all, ViChar triggerSettings[])
{
  ViChar gplst[10];
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViChar rcv[tkvx4244_RCV_STRLEN];	/* storage for instrument receive strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;

  /* Validate the command */
  retError = tkvx4244_invalidShortRange(sourceAndLogic, tkvx4244_TRIG_QUERY_LOGIC,
					tkvx4244_TRIG_QUERY_SOURCE, tkvx4244_ERROR_TRIG_SRC_QUERY);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp1, tkvx4244_GROUP_ONE_DISABLE,
					tkvx4244_GROUP_ONE_ENABLE, tkvx4244_ERROR_GP_ONE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp2, tkvx4244_GROUP_TWO_DISABLE,
					tkvx4244_GROUP_TWO_ENABLE, tkvx4244_ERROR_GP_TWO);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp3, tkvx4244_GROUP_THREE_DISABLE,
					tkvx4244_GROUP_THREE_ENABLE, tkvx4244_ERROR_GP_THREE);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(gp4, tkvx4244_GROUP_FOUR_DISABLE,
					tkvx4244_GROUP_FOUR_ENABLE, tkvx4244_ERROR_GP_FOUR);
  if (retError)
    return retError;
  retError = tkvx4244_invalidShortRange(all, tkvx4244_ALL_GROUPS_DISABLE,
					tkvx4244_ALL_GROUPS_ENABLE, tkvx4244_ERROR_ALL_GROUPS);
  if (retError)
    return retError;
  if (all + gp1 + gp2 + gp3 + gp4 == VI_FALSE)
    return tkvx4244_NO_GRPS_SELECTED_ERROR;
  /* Parse the gp list */
  tkvx4244_gplist_parse(gp1, gp2, gp3, gp4, all, gplst);
  gplst[strlen(gplst)] = '\0';
  /* Build the command string */
  sprintf(cmd, "TRIG:%s? (%s)\n", trig_sorce[sourceAndLogic], gplst);
  cmd[strlen(cmd)] = '\0';
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;

  retError = viRead(instrumentHandle, (ViPBuf) rcv, tkvx4244_RCV_STRLEN, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  rcv[retCnt] = '\0';
  strcpy(triggerSettings, rcv);
  triggerSettings[retCnt] = '\0';
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_confTriggerSlope
 *
 * Purpose:
 *  This function queries the instrument for the trigger slope settings.
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data. Must be initialized by tkvx4244_init()
 *		prior to use.
 *  ViInt16 acttiveEdge
 *  ViChar slope[]
 *  Returns:
 *		VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus _VI_FUNC
tkvx4244_confTriggerSlope(ViSession instrumentHandle, ViInt16 activeEdge, ViChar slope[])
{
  ViChar cmd[tkvx4244_CMD_STRLEN];	/* storage for instrument command strings */
  ViChar rcv[tkvx4244_RCV_STRLEN];	/* storage for instrument receive strings */
  ViStatus retError;		/* Returned error status */
  ViInt32 retCnt;		/* required parameter for visa calls */
  ViAttrState userData;		/* User data stored in VI_ATTR_USER_DATA */
  tkvx4244_PdrvrStruct drvrHdl;	/* pointer to instrument specific data */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_USER_DATA, &userData);
  drvrHdl = (tkvx4244_PdrvrStruct) userData;
  /*Validate the Command */
  retError = tkvx4244_invalidShortRange(activeEdge, tkvx4244_SLOPE_POS,
					tkvx4244_SLOPE_NEG, tkvx4244_ERROR_TRIG_ACTIVE_EDGE);
  if (retError)
    return retError;

  /* Build the command string */
  sprintf(cmd, "TRIG:SLOP %s\n", active_edge[activeEdge]);
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;
  /* Build the command string */
  sprintf(cmd, "TRIG:SLOP?\n");
  /* Send the command string */
  retError = viWrite(instrumentHandle, (ViBuf) cmd, strlen(cmd), (ViPUInt32) & retCnt);
  if (retError)
    return retError;

  retError = viRead(instrumentHandle, (ViPBuf) rcv, tkvx4244_RCV_STRLEN, (ViPUInt32) & retCnt);
  if (retError < VI_SUCCESS)
    return retError;
  rcv[retCnt] = '\0';
  strcpy(slope, rcv);

  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_gplist_parse
 *
 * Purpose:
 *  This private function parses the integers representing groups
 *  to usable strings.
 *
 * Parameters:
 *	  ViUint16 gp1-gp4 and gpa for groups
 *    ViChar buf[] for the string.
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus
tkvx4244_gplist_parse(ViInt16 gp1, ViInt16 gp2, ViInt16 gp3, ViInt16 gp4, ViInt16 gpa, ViChar buf[])
{
  ViChar gplst[10];

  strcpy(gplst, "");

  if (gp1 == 1)
    strcat(gplst, "1,");
  if (gp2 == 1)
    strcat(gplst, "2,");
  if (gp3 == 1)
    strcat(gplst, "3,");
  if (gp4 == 1)
    strcat(gplst, "4,");
  if (!gpa && !gplst[0]) {
    return tkvx4244_NO_GRPS_SELECTED_ERROR;
  }
  if (gpa == 1)
    sprintf(buf, "all");
  else {
    gplst[strlen(gplst) - 1] = 0;
    sprintf(buf, "@");
    strcat(buf, gplst);
  }
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_chlst_parse
 *
 * Purpose:
 *  This private function parses the integers representing channels
 *  to usable strings.
 *
 * Parameters:
 *	  ViInt16 c1-c16 and ca for channels
 *     ViChar buf[] for the command string.
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 */
ViStatus
tkvx4244_chlst_parse(ViInt16 c1, ViInt16 c2, ViInt16 c3,
		     ViInt16 c4, ViInt16 c5, ViInt16 c6, ViInt16 c7, ViInt16 c8, ViInt16 c9,
		     ViInt16 c10, ViInt16 c11, ViInt16 c12, ViInt16 c13, ViInt16 c14,
		     ViInt16 c15, ViInt16 c16, ViInt16 ca, ViChar buf[])
{
  ViChar chan[40];

  strcpy(chan, "");

  if (c1 == 1)
    strcat(chan, "1,");
  if (c2 == 1)
    strcat(chan, "2,");
  if (c3 == 1)
    strcat(chan, "3,");
  if (c4 == 1)
    strcat(chan, "4,");
  if (c5 == 1)
    strcat(chan, "5,");
  if (c6 == 1)
    strcat(chan, "6,");
  if (c7 == 1)
    strcat(chan, "7,");
  if (c8 == 1)
    strcat(chan, "8,");
  if (c9 == 1)
    strcat(chan, "9,");
  if (c10 == 1)
    strcat(chan, "10,");
  if (c11 == 1)
    strcat(chan, "11,");
  if (c12 == 1)
    strcat(chan, "12,");
  if (c13 == 1)
    strcat(chan, "13,");
  if (c14 == 1)
    strcat(chan, "14,");
  if (c15 == 1)
    strcat(chan, "15,");
  if (c16 == 1)
    strcat(chan, "16,");
  if (!ca && !chan[0])
    return tkvx4244_NO_CHAN_SELECTED_ERROR;
  if (ca == 1)
    sprintf(buf, "all");
  else {
    chan[(strlen(chan)) - 1] = 0;
    sprintf(buf, "@%s", chan);
  }
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_invalidLongRange
 *
 * Purpose:
 *	This function checks an integer to see if it lies between a minimum and
 *	maximum value. If the value is out of range, return error.
 *
 * Parameters:
 *	ViInt32 val: value to check range of
 *	ViInt32 min: minimum acceptible value
 *	ViInt32 max: maximum acceptible value
 *	ViStatus retError: error code to return if out of range
 *
 * Returns:
 *	User defined error code if value out of range, else VI_SUCCESS
 */

ViStatus tkvx4244_invalidLongRange(ViInt32 val, ViInt32 min, ViInt32 max, ViStatus retError)
{
  if (val < min || val > max)
    return retError;
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_invalidRealRange
 *
 * Purpose:
 *	This function checks an integer to see if it lies between a minimum and
 *	maximum value. If the value is out of range, return error.
 *
 * Parameters:
 *	ViReal32 val: value to check range of
 *	ViReal32 min: minimum acceptible value
 *	ViReal32 max: maximum acceptible value
 *	ViStatus retError: error code to return if out of range
 *
 * Returns:
 *	User defined error code if value out of range, else VI_SUCCESS
 */

ViStatus tkvx4244_invalidRealRange(ViReal64 val, ViReal64 min, ViReal64 max, ViStatus retError)
{
  if ((val < min) || (val > max))
    return retError;
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_invalidShortRange
 *
 * Purpose:
 *	This function checks an integer to see if it lies between a minimum and
 *	maximum value. If the value is out of range, return error.
 *
 * Parameters:
 *	ViInt16 val: value to check range of
 *	ViInt16 min: minimum acceptible value
 *	ViInt16 max: maximum acceptible value
 *	ViStatus retError: error code to return if out of range
 *
 * Returns:
 *	User defined error code if value out of range, else VI_SUCCESS
 */

ViStatus tkvx4244_invalidShortRange(ViInt16 val, ViInt16 min, ViInt16 max, ViStatus retError)
{
  if (val < min || val > max)
    return retError;
  return VI_SUCCESS;
}

/***************************************************************************
 * Function: tkvx4244_setupArrays
 *
 * Purpose:
 *	Initialize driver instrAttr structure.
 *
 * Parameters:
 *	none
 *
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 */

ViStatus tkvx4244_setupArrays(void)
{
  ViSession resMgr;		/* Resource Manager Session */
  ViStatus retError;		/* Returned error status */

  if (tkvx4244_gNumberFound == 0) {	/* note that tkvx4244_gNumberFound is a global */
		/*************************************************************************
		 * Find all available tkvx4244s and store an instrument descriptor string
		 * for each.  This technique allows for faster autoconnects.
		 *
		 * Note: Both VXI and GPIB interfaces are searched for instruments.
		 */

    retError = viGetDefaultRM(&resMgr);
    if (retError >= VI_SUCCESS) {
      retError = tkvx4244_findInstruments(resMgr, tkvx4244_VXI_SEARCH);
      if ((retError < VI_SUCCESS) && (retError != VI_ERROR_RSRC_NFOUND) &&
	  (retError != VI_ERROR_SYSTEM_ERROR)) {
	viClose(resMgr);
	return retError;
      }

      retError = tkvx4244_findInstruments(resMgr, tkvx4244_GPIB_VXI_SEARCH);
      if ((retError < VI_SUCCESS) && (retError != VI_ERROR_RSRC_NFOUND)) {
	viClose(resMgr);
	return retError;
      }
    }

    viClose(resMgr);

    if (tkvx4244_gNumberFound > 0) {
      return VI_SUCCESS;
    } else {
      return VI_ERROR_RSRC_NFOUND;
    }
  } else {
    return VI_SUCCESS;
  }
}

/***************************************************************************
 * Function: tkvx4244_findInstruments
 *
 * Purpose:
 *	This private function opens each instrument on the interface specified
 * by searchString and determines if it is a tkvx4244.  If so, the instrument
 * descriptor string, logical address, and slot # are stored in a global
 * array. The number of instruments stored in the array is updated.
 *
 * After this routine has been called, instruments can be connected to
 * quickly by searching this array and using the instrument descriptor
 * string in a viOpen() call.
 *
 * Parameters:
 *	ViSession resMgr: Default resource manager to communicate with.
 * ViChar searchString[]: Search string which specifies which interfaces
 *								to search for instruments
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 */

ViStatus tkvx4244_findInstruments(ViSession resMgr, ViChar searchString[])
{
  ViStatus retError;		/* Returned error status */
  ViSession session;		/* VISA session */
  ViSession findSession;	/* VISA find session */
  ViInt32 retCnt;		/* Number of matches made by viFindRsrc */
  ViChar descriptor[tkvx4244_RCV_STRLEN];	/* instrument descriptor */
  ViInt16 logAdr;		/* logical address */
  ViInt16 slot;			/* slot # */

  findSession = VI_NULL;

  retError = viFindRsrc(resMgr, searchString, &findSession, (ViPUInt32) & retCnt, descriptor);
  if (retError < VI_SUCCESS) {
    viClose(findSession);
    return retError;
  }

  /*
   * Open each instrument looking for tkvx4244. Each
   * time a tkvx4244 is located, add it to a list.
   */

  do {
    /*
     * get the slot number, manufacture id and model code
     * from the instrument.
     */

    retError = viOpen(resMgr, descriptor, VI_NULL, VI_NULL, &session);
    if (retError < VI_SUCCESS) {
      viClose(findSession);
      return retError;
    }

    retError = tkvx4244_verifyID(session);

    if (retError >= VI_SUCCESS) {
      retError = viGetAttribute(session, VI_ATTR_SLOT, &slot);
      if (retError < VI_SUCCESS) {
	viClose(session);
	viClose(findSession);
	return retError;
      }

      retError = viGetAttribute(session, VI_ATTR_VXI_LA, &logAdr);
      if (retError < VI_SUCCESS) {
	viClose(session);
	viClose(findSession);
	return retError;
      }

      /*
       * Add instrument slot number,logical address and descriptor to
       * an instr Array
       */

      tkvx4244_instrAttr[tkvx4244_gNumberFound].logAdr = logAdr;
      tkvx4244_instrAttr[tkvx4244_gNumberFound].slot = slot;
      strcpy(tkvx4244_instrAttr[tkvx4244_gNumberFound].instrDesc, descriptor);
      (tkvx4244_gNumberFound)++;

      viClose(session);
    } else if (retError != VI_ERROR_RSRC_NFOUND) {
      viClose(session);
      viClose(findSession);
      return retError;
    } else {
      viClose(session);
    }

    retError = viFindNext(findSession, descriptor);

  } while (retError >= VI_SUCCESS);

  viClose(findSession);

  if (tkvx4244_gNumberFound > 0) {
    return VI_SUCCESS;
  } else {
    return VI_ERROR_RSRC_NFOUND;
  }
}

/***************************************************************************
 * Function: tkvx4244_tryAutoConnect
 *
 * Purpose:
 *	Attempt to connect to instrument in specified slot or logical address
 *
 * Parameters:
 *	ViInt16 slot: slot # to connect to (VI_NULL if not used)
 *	ViInt16 logAdr: logical address to connect to (VI_NULL if not used)
 *	ViPSession instrumentHandle: Pointer to VISA instrument handle used to access
 *		instrument specific data. This handle is initialized by this
 *		routine.  Note that if a slot is specified without a logical
 *		address and multiple instruments reside in the slot, this
 *		routine will connect to the first instrument found in the slot.
 *
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 */

ViStatus tkvx4244_tryAutoConnect(ViInt16 theSlot, ViInt16 theLogAdr, ViPSession instrumentHandle)
{
  ViStatus retError;		/* Returned error status */
  ViString descriptor;		/* instrument descriptor */

  /* Initialize Instrument-Dependent Command arrays. */
  tkvx4244_setupArrays();

  /*
   * Get the descriptor for the first VXI instrument it
   * finds.
   */

  retError = tkvx4244_returnDescriptor(theSlot, theLogAdr, &descriptor);

  if (retError < VI_SUCCESS) {
    /*
     * return error because instrument is not in theSlot or wrong LA
     */

    *instrumentHandle = VI_NULL;
  } else {
    retError = tkvx4244_init(descriptor, VI_FALSE, VI_TRUE, instrumentHandle);

    if (retError < VI_SUCCESS) {
      *instrumentHandle = VI_NULL;
    }
  }

  return retError;
}

/***************************************************************************
 * Function: tkvx4244_returnDescriptor
 *
 * Purpose:
 *	Returns instrument descriptor for instrument in specified slot/LA
 *
 * Parameters:
 *	ViInt16 theSlot: Instrument slot #, <= 0, don't test slot #
 *	ViInt16 theLogAdr: Instrument LA, <= 0, don't test LA
 *	ViString *descriptor: Instrument descriptor string
 *
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 */

ViStatus tkvx4244_returnDescriptor(ViInt16 theSlot, ViInt16 theLogAdr, ViString * descriptor)
{
  ViInt16 ii;			/* loop count */

  if ((theSlot > 0) && (theLogAdr > 0)) {
    for (ii = 0; ii < tkvx4244_gNumberFound; ii++) {
      if ((tkvx4244_instrAttr[ii].slot == theSlot) && (tkvx4244_instrAttr[ii].logAdr == theLogAdr)) {
	*descriptor = tkvx4244_instrAttr[ii].instrDesc;
	return VI_SUCCESS;
      }
    }
  } else if ((theSlot > 0) && (theLogAdr <= 0)) {
    for (ii = 0; ii < tkvx4244_gNumberFound; ii++) {
      if (tkvx4244_instrAttr[ii].slot == theSlot) {
	*descriptor = tkvx4244_instrAttr[ii].instrDesc;
	return VI_SUCCESS;
      }
    }
  } else if ((theSlot <= 0) && (theLogAdr > 0)) {
    for (ii = 0; ii < tkvx4244_gNumberFound; ii++) {
      if (tkvx4244_instrAttr[ii].logAdr == theLogAdr) {
	*descriptor = tkvx4244_instrAttr[ii].instrDesc;
	return VI_SUCCESS;
      }
    }
  } else {
    return VI_ERROR_RSRC_NFOUND;
  }
  return VI_ERROR_RSRC_NFOUND;
}

/***************************************************************************
 * Function: tkvx4244_verifyID
 *
 * Purpose:
 *	This private function verifies that the manufacturer ID and model code
 *	of the opened instrument are the same as those that this driver
 *	services.
 *
 * Parameters:
 *	ViSession instrumentHandle: VISA instrument handle used to access instrument
 *		specific data.
 *
 * Returns:
 *	VI_SUCCESS if no errors occurred, error code otherwise
 */

ViStatus tkvx4244_verifyID(ViSession instrumentHandle)
{
  ViUInt16 manfID;		/* instrument manufacturer ID code */
  ViUInt16 modelCode;		/* instrument model code */
  ViStatus retError;		/* Returned error status */
  ViInt16 ii;			/* loop counter */

  /* Read the manufacturer ID */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_MANF_ID, &manfID);
  if (retError < VI_SUCCESS)
    return retError;

  /* Read the model code */

  retError = viGetAttribute(instrumentHandle, VI_ATTR_MODEL_CODE, &modelCode);
  if (retError < VI_SUCCESS)
    return retError;

  manfID &= 0xfff;

  if (tkvx4244_modelCodes[0] < 0x1000)
    modelCode &= 0x0fff;

  if (manfID == tkvx4244_MANF_ID) {
    for (ii = 0; ii < tkvx4244_NUM_MODEL_CODES; ii++) {
      if (tkvx4244_modelCodes[ii] == modelCode) {
	return VI_SUCCESS;
      }
    }
  }

  return VI_ERROR_RSRC_NFOUND;
}

/*========END==============================================================*/

#ifdef tkvx4244_DLL
#include <windows.h>
int _VI_FUNC LibMain( /*Handle, WORD, WORD, LPSTR */ )
{
  /*
   * Put any initialization code here required
   * by your instrument Driver
   */
  return 1;			/* signifies success */
}

int _VI_FUNC WEP(ViInt32 arg)
{
  /*
   * Put any clean up code required by your
   * driver here.
   */
  return 1;			/* signifies success */
}
#endif
