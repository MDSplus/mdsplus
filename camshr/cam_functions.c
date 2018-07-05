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
// CamFunctions.c ala Cam_...()
//-------------------------------------------------------------------------
//      Stuart Sherman
//      MIT / PSFC
//      Cambridge, MA 02139  USA
//
//      This is a port of the MDSplus system software from VMS to Linux, 
//      specifically:
//                      CAMAC subsystem, ie libCamShr.so and verbs.c for CTS.
//-------------------------------------------------------------------------
//      $Id$
//-------------------------------------------------------------------------
// Tue Aug  1 11:22:06 EDT 2000
// Tue Apr  3 09:57:52 EDT 2001
// Wed Apr  4 15:29:00 EDT 2001
// Mon Apr  9 14:10:02 EDT 2001
// Thu Apr 12 12:52:09 EDT 2001
// Tue Jun 19 10:10:55 EDT 2001 -- cleanup, prettify
// Thu Jul 12 10:36:57 EDT 2001 -- fixed 'status' return
// Thu Jul 26 16:21:13 EDT 2001 -- changed way ScsiIo() called
// Mon Aug 20 16:33:50 EDT 2001 -- fixed parameter passing to/from ScsiIo()
// Tue Sep  4 16:04:22 EDT 2001 -- dynamic debug printout
// Fri Dec 13 15:17:46 EST 2002 -- fixing iosb->bytcnt -- not working, yet
/********************************************************************
 * example use: CallSingleIo( Piow, QIgnore )                       *
 * creates:     CamPiow( char*, int, int, void*, int, Iosb* )       *
 *                                                                  *
 *              CallMultiIo( Fstop, QIgnore, 1 )                    *
 *              CamFstop( char*, int, int, int, void*, int, Iosb* ) *
 *******************************************************************/
#define	ItemsPerLine	16

#define J_ROUTINE_NAME	"\033[01;31mJorwayDoIo\033[0m"
#define JT_ROUTINE_NAME "\033[01;34mJorwayTranslateIosb\033[0m"
#define	KS_ROUTINE_NAME	"\033[01;36mKsSingle\033[0m"
#define	KM_ROUTINE_NAME	"\033[01;32mKsMulti\033[0m"
#define KT_ROUTINE_NAME "\033[01;33mKsTranslateIosb\033[0m"

//-----------------------------------------------------------
// roundup the regular suspects ...
//-----------------------------------------------------------
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <scsi/sg.h>
#include <dlfcn.h>

#include <camdef.h>
#include <mdsshr_messages.h>
#include "common.h"
#include "prototypes.h"

//-----------------------------------------------------------
// scsi command lengths, per SCSI-2 spec (ANSI X3T9.2/375R)
//              group 0:         6 bytes
//              group 1:        10 bytes
//              group 2:        10 bytes
//              group 3:        -- reserved
//              group 4:        -- reserved
//              group 5:        12 bytes
//              group 6:        vendor specified
//              group 7:        vendor specified
//
// linux 'sg()' driver, version 2.1.36
//              group 0:         6 bytes
//              group 1:        10 bytes
//              group 2:        10 bytes
//              group 3:        12 bytes        NB difference
//              group 4:        12 bytes        NB difference
//              group 5:        12 bytes
//              group 6:        10 bytes        NB difference
//              group 7:        10 bytes        NB difference
//-----------------------------------------------------------
//                       group: 0   1   2   3   4   5   6   7
static BYTE scsi_cmd_size[] = { 6, 10, 10, 12, 12, 12, 10, 10 };

#define COMMAND_SIZE(opcode) scsi_cmd_size[((opcode) >> 5) & 7]

//-----------------------------------------------------------
// transfer modes
//-----------------------------------------------------------
static BYTE JorwayModes[2][2][4] = {
  {
   {				// non-enhanced modes
    STOP_MODE,			// QStop                        (M = 0)
    STOP_MODE,			// QIgnore              (M = 6)
    QREP_MODE,			// QRep                         (M = 2)
    NO_MODE			// QScan        NB! tbi
    },
   {				// enhanced modes
    STOP_MODE,			//                                      (M = 4)
    STOP_MODE,			//                                      (M = 5)
    NO_MODE,
    NO_MODE}
   },
  {
   {				// non-enhanced modes
    QSTOP_MODE,			// QStop                        (M = 0)
    STOP_MODE,			// QIgnore              (M = 6)
    QREP_MODE,			// QRep                         (M = 2)
    NO_MODE			// QScan        NB! tbi
    },
   {				// enhanced modes
    FQSTOP_MODE,		//                                      (M = 4)
    FSTOP_MODE,			//                                      (M = 5)
    NO_MODE,
    NO_MODE}
   }
};

#define	JORWAY_DISCONNECT			0
#define	JORWAYMODE(mode, enhanced, multisample)	JorwayModes[multisample][enhanced][mode]
#define	KSMODE(mode)				mode

//-----------------------------------------------------------
// local function prototypes
//-----------------------------------------------------------
static int SingleIo(CamKey Key,
		    BYTE A, BYTE F, BYTE * Data, BYTE Mem, TranslatedIosb * iosb, int dmode);
static int MultiIo(CamKey Key,
		   BYTE A,
		   BYTE F,
		   int Count,
		   BYTE * Data, BYTE Mem, TranslatedIosb * iosb, int dmode, int Enhanced);
static int JorwayDoIo(CamKey Key,
		      BYTE A,
		      BYTE F,
		      int Count,
		      BYTE * Data, BYTE Mem, TranslatedIosb * iosb, int dmode, int Enhanced);
static int Jorway73ADoIo(CamKey Key,
			 BYTE A,
			 BYTE F,
			 int Count,
			 BYTE * Data, BYTE Mem, TranslatedIosb * iosb, int dmode, int Enhanced);
static int KsSingleIo(CamKey Key,
		      BYTE A, BYTE F, BYTE * Data, BYTE Mem, TranslatedIosb * iosb, int dmode);
static int KsMultiIo(CamKey Key,
		     BYTE A,
		     BYTE F,
		     int Count,
		     BYTE * Data, BYTE Mem, TranslatedIosb * iosb, int dmode, int Enhanced);

typedef struct {
  __u8 code:7;
  __u8 valid:1;

  __u8 segment_no;

  __u8 sense_key:4;
  __u8 zero1:4;

  __u8 fifo_status;

  __u8 DMA_byte_count[3];

  __u8 additional_sense_length;

  __u8 zero2[3];

  __u8 additional_sense_code;

  __u8 zero3[5];

} J73ASenseData;
static int CamAssign(char *Name, CamKey * Key);
static int NOT_SUPPORTED();
static void str2upcase(char *str);

//-----------------------------------------------------------
// function prototypes -- not necessarily local
//-----------------------------------------------------------
static int JorwayTranslateIosb(int reqbytcnt, SenseData * sense, char senseretlen,
			       unsigned int bytcnt, int scsi_status);
static int Jorway73ATranslateIosb(int datacmd, int reqbytcnt, J73ASenseData * sense,
				  int scsi_status);
static int KsTranslateIosb(RequestSenseData * sense, int scsi_status);

static int isRemote = -1;
static int checkRemote() {
  if (isRemote == -1) {
    isRemote = getenv("camac_server") ? 1 : 0;
  }
  return isRemote;
}
//-----------------------------------------------------------
// local, global
//-----------------------------------------------------------
static TranslatedIosb LastIosb;
static int Verbose = 0;
//-----------------------------------------------------------
// helper routines
//-----------------------------------------------------------
extern int RemCamVerbose();
extern int RemCamQ();
extern int RemCamX();
extern int RemCamXandQ();
extern int RemCamGetStat();
extern int RemCamError();
extern int RemCamStatus();
extern int RemCamBytcnt();
extern int RemCamPiow();
extern int RemCamQrepw();
extern int RemCamPioQrepw();
extern int RemCamFQstopw();
extern int RemCamFStopw();
extern int RemCamFQrepw();
extern int RemCamQrepw();
extern int RemCamQscanw();
extern int RemCamQstopw();
extern int RemCamStopw();
extern int RemCamSetMAXBUF();
extern int RemCamGetMAXBUF();


EXPORT int CamVerbose(int mode)
{
  if ((isRemote == 1 || (isRemote == -1 && checkRemote()))) {
	  return RemCamVerbose(mode);
  } else {
    Verbose = mode;
    return 1;
  }
}

EXPORT int CamQ(TranslatedIosb * iosb)	// CAM$Q_SCSI()
{
  if ((isRemote == 1 || (isRemote == -1 && checkRemote()))) {
	  return RemCamQ(iosb);
  }
  else {
    TranslatedIosb *iosb_use;
    iosb_use = (iosb) ? iosb : &LastIosb;

    if (MSGLVL(DETAILS))
      printf("CamQ(): using %s %p  CamQ()=%d\n", (iosb) ? "iosb" : "&LastIosb", iosb, iosb_use->q);
//printf("CamQ(iosb)    ::->> bytecount= %d\n", iosb->bytcnt);          // [2002.12.13]
//printf("CamQ(iosb_use)::->> bytecount= %d\n", iosb_use->bytcnt);      // [2002.12.13]
//printf("CamQ(LastIosb)::->> bytecount= %d\n", LastIosb.bytcnt);               // [2002.12.13]
    return iosb_use->q;
  }
}

//-----------------------------------------------------------
EXPORT int CamX(TranslatedIosb * iosb)	// CAM$X_SCSI()
{
  if ((isRemote == 1 || (isRemote == -1 && checkRemote()))) {
	  return RemCamX(iosb);
  } else {
    TranslatedIosb *iosb_use;
    iosb_use = (iosb) ? iosb : &LastIosb;

    if (MSGLVL(DETAILS))
      printf("CamX(): using %s %p  CamX()=%d\n", (iosb) ? "iosb" : "&LastIosb", iosb, iosb_use->x);

//printf("CamX(iosb)    ::->> bytecount= %d\n", iosb->bytcnt);          // [2002.12.13]
//printf("CamX(iosb_use)::->> bytecount= %d\n", iosb_use->bytcnt);      // [2002.12.11]
//printf("CamX(LastIosb)::->> bytecount= %d\n", LastIosb.bytcnt);               // [2002.12.13]
    return iosb_use->x;
  }
}

//-----------------------------------------------------------
EXPORT int CamXandQ(TranslatedIosb * iosb)	// CAM$XANDQ_SCSI()
{
  if ((isRemote == 1 || (isRemote == -1 && checkRemote()))) {
	  return RemCamXandQ(iosb);
  } else {
    TranslatedIosb *iosb_use;
    iosb_use = (iosb) ? iosb : &LastIosb;

    if (MSGLVL(DETAILS))
      printf("CamXandQ(): using %s %p  %s .x=%d .q=%d  CamXandQ()=%d\n",
	     (iosb) ? "iosb" : "&LastIosb",
	     iosb,
	     (iosb) ? "iosb" : "&LastIosb", iosb_use->x, iosb_use->q, iosb_use->x && iosb_use->q);
    
//printf("CamXnQ(iosb)    ::->> bytecount= %d\n", iosb->bytcnt);                // [2002.12.13]
//printf("CamXnQ(iosb_use)::->> bytecount= %d\n", iosb_use->bytcnt);    // [2002.12.13]
//printf("CamXnQ(LastIosb)::->> bytecount= %d\n", LastIosb.bytcnt);     // [2002.12.13]
    return iosb_use->x && iosb_use->q;
  }
}

//-----------------------------------------------------------
EXPORT int CamBytcnt(TranslatedIosb * iosb)	// CAM$BYTCNT_SCSI()
{
  if ((isRemote == 1 || (isRemote == -1 && checkRemote()))) {
	  return RemCamBytcnt(iosb);
  } else {
    TranslatedIosb *iosb_use;
    iosb_use = (iosb) ? iosb : &LastIosb;

    if (MSGLVL(DETAILS))
      printf("CamBytcnt(): using %s %p  CamBytcnt()=%d\n",
	   (iosb) ? "iosb" : "&LastIosb", iosb, iosb_use->bytcnt);

    //printf("CamBytcnt(iosb)    ::->> bytecount= %d\n", iosb->bytcnt);             // [2002.12.13]
    //printf("CamBytcnt(iosb_use)::->> bytecount= %d\n", iosb_use->bytcnt); // [2002.12.13]
    //printf("CamBytcnt(LastIosb)::->> bytecount= %d\n", LastIosb.bytcnt);  // [2002.12.13]
    return ((int)iosb_use->bytcnt) | (((int)iosb_use->lbytcnt) << 16);
  }
}

//-----------------------------------------------------------
EXPORT int CamStatus(TranslatedIosb * iosb)	// CAM$STATUS_SCSI()
{
  if ((isRemote == 1 || (isRemote == -1 && checkRemote()))) {
	  return RemCamStatus(iosb);
  } else {
    TranslatedIosb *iosb_use;
    iosb_use = (iosb) ? iosb : &LastIosb;

    //printf("CamStatus(iosb)    ::->> bytecount= %d\n", iosb->bytcnt);             // [2002.12.13]
    //printf("CamStatus(iosb_use)::->> bytecount= %d\n", iosb_use->bytcnt); // [2002.12.13]
    //printf("CamStatus(LastIosb)::->> bytecount= %d\n", LastIosb.bytcnt);  // [2002.12.13]
    return iosb_use->status;
  }
}

//-----------------------------------------------------------
EXPORT int CamGetStat(TranslatedIosb * iosb)	// CAM$GET_STAT_SCSI()
{
  if ((isRemote == 1 || (isRemote == -1 && checkRemote()))) {
    return RemCamGetStat(iosb);
  } else {
    *iosb = LastIosb;
    //printf("CamGetStatus(iosb)    ::->> bytecount= %d\n", iosb->bytcnt);  // [2002.12.13]
    //printf("CamGetStatus(LastIosb)::->> bytecount= %d\n", LastIosb.bytcnt);       // [2002.12.13]
    return 1;
  }
}

//-----------------------------------------------------------
EXPORT int CamError(int xexp, int qexp, TranslatedIosb * iosb)
{
  if ((isRemote == 1 || (isRemote == -1 && checkRemote()))) {
    return RemCamError(xexp, qexp, iosb);
  } else {
    int xexp_use;
    int qexp_use;
    TranslatedIosb *iosb_use;
    
    if (MSGLVL(DETAILS))
      printf("CamError(): xexp:%d qexp:%d\n", xexp, qexp);
    
    xexp_use = xexp ? xexp : 0;
    qexp_use = qexp ? qexp : 0;
    iosb_use = iosb ? iosb : &LastIosb;
    
    iosb_use->err = !iosb_use->x && !iosb_use->q;
    
    if (MSGLVL(DETAILS))
      printf("CamError(): x:%d q:%d iosb->err %d\n", xexp_use, qexp_use, iosb_use->err);
    return iosb_use->err;
  }
}

//-----------------------------------------------------------
// Pio modes
// NB! it has been found empirically that the length
//      of the message for a CAMAC non-data message must
//      be set to 10 bytes, not the expected 6. The reasoning
//      is left as an excercise for the reader ;-)
// NB! NEWS Flash -- SCSI-2 spec says that group 6 & 7
//      commands are 10 bytes long, and further, group 6 & 7
//      commands are vendor specific. But the scsi driver wants
//      10 bytes length, so that's what we give it.
//-----------------------------------------------------------
#define	CallSingleIo( pname, dmode )                \
EXPORT int Cam##pname(                                     \
				char			*Name,              \
				BYTE			A,                  \
				BYTE			F,                  \
				void			*Data,              \
				BYTE			Mem,                \
				TranslatedIosb	*iosb               \
				)                                   \
{                                                   \
	int 		status;              				\
	CamKey		Key;                                \
        static  int debug=-1; \
	\
	if ((isRemote == 1 || (isRemote == -1 && checkRemote()))) { \
	  return RemCam##pname(Name, A, F, Data, Mem, iosb); \
	}\
                                                    \
        if (debug==-1) { char *tmp=getenv("CAM_DEBUG"); debug=tmp?1:0;} \
        if (debug) printf("Cam"#pname": name=%s, A=%d, F=%d, data=%d, mem=%d\n",Name,A,F,Data ? *(int *)Data : -1,Mem); \
	status = CamAssign( Name, &Key );               \
	if( status == SUCCESS ) {                       \
		status = SingleIo(                          \
						Key,                        \
						A,                          \
						F,                          \
						Data,                       \
						Mem,                        \
						iosb,                       \
						dmode                       \
						);                          \
	}                                               \
                                                    \
	return status;                                  \
}

//-----------------------------------------------------------
// possibly enhanced modes
//-----------------------------------------------------------
#define	CallMultiIo( pname, dmode, enhanced )       \
EXPORT int Cam##pname(                                     \
				char			*Name,              \
				BYTE			A,                  \
				BYTE			F,                  \
				int				Count,              \
				void			*Data,              \
				BYTE			Mem,                \
				TranslatedIosb	*iosb               \
				)                                   \
{                                                   \
	int 		status;              				\
	CamKey		Key;                                \
                                                    \
        static  int debug=-1; \
                                                    \
	if ((isRemote == 1 || (isRemote == -1 && checkRemote()))) { \
	  return RemCam##pname(Name, A, F, Count, Data, Mem, iosb);	\
	}\
                                                    \
        if (debug==-1) { char *tmp=getenv("CAM_DEBUG"); debug=tmp?1:0;} \
        if (debug) printf("Cam"#pname": name=%s, A=%d, F=%d, Count=%d, data=%p, mem=%d\n",Name,A,F,Count,Data,Mem); \
	status = CamAssign( Name, &Key );               \
	if( status == SUCCESS ) {                       \
		status = MultiIo(                           \
						Key,                        \
						A,                          \
						F,                          \
						Count,                      \
						Data,                       \
						Mem,                        \
						iosb,                       \
						dmode,                      \
						enhanced                    \
						);                          \
	}                                               \
	                                                \
	return status;                                  \
}

//-----------------------------------------------------------
// macro expansion
//-----------------------------------------------------------
CallSingleIo(Piow, QIgnore)	// yields: int = CamPiow( ... )
    CallSingleIo(PioQrepw, QRep)//       : int = CamPioQrepw( ... )
    CallMultiIo(FQstopw, QStop, 1)
				//       : int = CamFQstopw( ... )              [2001.09.13]
    CallMultiIo(FStopw, QIgnore, 1)
				//       : int = CamFStopw( ... )               [2001.09.13]
    CallMultiIo(FQrepw, QRep, 1)//       : int = CamQrepw( ... )                [2001.04.18]
    CallMultiIo(Qrepw, QRep, 0)	//       : int = CamQrepw( ... )                [2001.04.18]
    CallMultiIo(Qscanw, QScan, 0)
				//       : int = CamQscanw( ... )               [2001.04.18]
    CallMultiIo(Qstopw, QStop, 0)
				//       : int = CamQstopw( ... )               [2001.08.30]
    CallMultiIo(Stopw, QIgnore, 0)
				//       : int = CamStopw( ... )                [2001.04.10]
//-----------------------------------------------------------
static int SingleIo(CamKey Key,
		    BYTE A, BYTE F, BYTE * Data, BYTE Mem, TranslatedIosb * iosb, int dmode)
{
  char tmp[9];
  int highwayType, status;

  if (MSGLVL(FUNCTION_NAME))
    printf("\033[01;31mSingleIo(F=%d)-->\033[0m", F);
  sprintf(tmp, "GK%c%d", Key.scsi_port, Key.scsi_address);
  if ((status = QueryHighwayType(tmp)) == SUCCESS) {
    highwayType = NUMERIC(tmp[5]);	// extract type

    switch (highwayType) {	// check highway type
    case JORWAY:
    case JORWAY_OLD:
      if (MSGLVL(DETAILS))
	printf("-->>JorwayDoIo()\n");

      status = JorwayDoIo(Key,	// module info
			  A,	// module sub address
			  F,	// module function
			  1,	// implied count of 1
			  Data,	// data
			  Mem,	// 16 or 24 bit data
			  iosb,	// status struct
			  dmode,	// mode
			  0	// non-enhanced
	  );
      break;

    case KINSYSCO:
      if (MSGLVL(DETAILS))
	printf("-->>KsSingleIo()\n");

      status = KsSingleIo(Key,	// module info
			  A,	// module sub-address
			  F,	// module function
			  Data,	// data
			  Mem,	// 16 or 24 bit data
			  iosb,	// status struct
			  KSMODE(dmode)	// mode
	  );
      break;

    case JORWAY_73A:
      if (MSGLVL(DETAILS))
	printf("-->>JorwayDoIo()\n");

      status = Jorway73ADoIo(Key,	// module info
			     A,	// module sub address
			     F,	// module function
			     1,	// implied count of 1
			     Data,	// data
			     Mem,	// 16 or 24 bit data
			     iosb,	// status struct
			     dmode,	// mode
			     0	// non-enhanced
	  );
      break;

    default:
      if (MSGLVL(IMPORTANT))
	fprintf(stderr, "highway type(%d) not supported\n", highwayType);

      status = FAILURE;
      break;
    }				// end of switch()
    if (MSGLVL(FUNCTION_NAME))	// show data, if there is some
      if (Data) {
	if (Mem == 16)
	  printf("\033[01;31mSingleIo(F=%d)-->>%d\033[0m\n", F, *(short *)Data);
	else
	  printf("\033[01;31mSingleIo(F=%d)-->>%d\033[0m\n", F, *(int *)Data);
      }

  }				// end of if()

//printf("SingleIo(iosb)::->> bytecount= %d\n", iosb->bytcnt);  // [2002.12.13]
  return status;
}

//-----------------------------------------------------------
static int MultiIo(CamKey Key,
		   BYTE A,
		   BYTE F,
		   int Count, BYTE * Data, BYTE Mem, TranslatedIosb * iosb, int dmode, int Enhanced)
{
  char tmp[10];
  int highwayType, mode, status;

  if (MSGLVL(FUNCTION_NAME))
    printf("\033[01;31mMultiIo(F=%d, count=%d)-->>\033[0m\n", F, Count);
  sprintf(tmp, "GK%c%d", Key.scsi_port, Key.scsi_address);

  if ((status = QueryHighwayType(tmp)) == SUCCESS) {
    highwayType = NUMERIC(tmp[5]);	// extract type

    switch (highwayType) {	// check highway type
    case JORWAY:
    case JORWAY_OLD:
      if (MSGLVL(DETAILS))
	printf("-->>JorwayDoIo()\n");

      mode = JORWAYMODE(dmode, (Enhanced && highwayType == JORWAY), Count > 1);
      if (mode != NO_MODE)
	status = JorwayDoIo(Key,	// module info
			    A,	// module sub address
			    F,	// module function
			    Count,	// data count int bytes
			    Data,	// data
			    Mem,	// 16 or 24 bit data
			    iosb,	// status struct
			    dmode,	// mode
			    Enhanced	// highway mode
	    );
      else
	status = NOT_SUPPORTED();

      break;

    case KINSYSCO:
      if (MSGLVL(DETAILS))
	printf("-->>KsMultiIo()\n");

      status = KsMultiIo(Key,	// module info
			 A,	// module sub-address
			 F,	// module function
			 Count,	// data count in bytes
			 Data,	// data
			 Mem,	// 16 or 24 bit data
			 iosb,	// status struct
			 KSMODE(dmode),	// mode
			 Enhanced	// enhanced
	  );
      break;

    case JORWAY_73A:

      mode = JORWAYMODE(dmode, (Enhanced && highwayType == JORWAY), Count > 1);
      if (mode != NO_MODE)
	status = Jorway73ADoIo(Key,	// module info
			       A,	// module sub address
			       F,	// module function
			       Count,	// data count int bytes
			       Data,	// data
			       Mem,	// 16 or 24 bit data
			       iosb,	// status struct
			       dmode,	// mode
			       Enhanced	// highway mode
	    );
      else
	status = NOT_SUPPORTED();

      break;

    default:
      if (MSGLVL(IMPORTANT))
	fprintf(stderr, "highway type(%d) not supported\n", highwayType);

      status = FAILURE;
      break;
    }				// end of switch()
  }				// end of if()

  if (MSGLVL(DETAILS)) {
    if (!iosb)
      printf("MultiIo null iosb ptr");
    else
      printf("MultiIo(iosb)::->> bytecount= %d\n", iosb->bytcnt);	// [2002.12.13]
  }
  return status;
}

//-----------------------------------------------------------
static int JorwayDoIo(CamKey Key,
		      BYTE A,
		      BYTE F,
		      int Count,
		      BYTE * Data, BYTE Mem, TranslatedIosb * iosb, int dmode, int Enhanced)
{
  char dev_name[12];
  int IsDataCommand, scsiDevice;
  int status;
  unsigned char *cmd = 0;
  unsigned char cmdlen;
  int direction;
  int bytcnt;
  int reqbytcnt = 0;
  SenseData sense;
  unsigned char sensretlen;
  int online;
  int enhanced;
  CDBCamacDataCommand(DATAcommand);
  CDBCamacNonDataCommand(NONDATAcommand);

  if (MSGLVL(FUNCTION_NAME))
    printf("%s()\n", J_ROUTINE_NAME);
//printf( "%s(iosb is %sNULL)\n", J_ROUTINE_NAME, (iosb)?"NOT ":"" );           // [2002.12.13]

  sprintf(dev_name, "GK%c%d%02d", Key.scsi_port, Key.scsi_address, Key.crate);
  if ((scsiDevice = get_scsi_device_number(dev_name, &enhanced, &online)) < 0) {
    if (MSGLVL(IMPORTANT))
      fprintf(stderr, "%s(): error -- no scsi device found for '%s'\n", J_ROUTINE_NAME, dev_name);

    status = NO_DEVICE;
    goto JorwayDoIo_Exit;
  }
  if (!online && Key.slot != 30)
    return CamOFFLINE;

  if (!Enhanced)
    enhanced = 0;
  if (MSGLVL(DETAILS))
    printf("%s(): device '%s' = '/dev/sg%d'\n", J_ROUTINE_NAME, dev_name, scsiDevice);

  IsDataCommand = (F & 0x08) ? FALSE : TRUE;

  if (IsDataCommand) {
    union {
      BYTE b[4];
      unsigned int l;
    } transfer_len;

    DATAcommand.f = F;
    DATAcommand.bs = Mem == 24;
    DATAcommand.n = Key.slot;
    DATAcommand.m = JORWAYMODE(dmode, enhanced, Count > 1);
    DATAcommand.a = A;
    DATAcommand.sncx = 0;
    DATAcommand.scs = 0;

    reqbytcnt = transfer_len.l = Count * ((Mem == 24) ? 4 : 2);
#	if 	JORWAY_DISCONNECT
#		ifdef SG_BIG_BUFF
    DATAcommand.dd = transfer_len.l > SG_BIG_BUFF;
#		else
    DATAcommand.dd = transfer_len.l > 4096;
#		endif
#	else
    DATAcommand.dd = 0;
#	endif
    DATAcommand.crate = Key.crate;
    DATAcommand.sp = HIGHWAY_SERIAL;

    DATAcommand.transfer_len[0] = transfer_len.b[2];	// NB! order reversal
    DATAcommand.transfer_len[1] = transfer_len.b[1];
    DATAcommand.transfer_len[2] = transfer_len.b[0];

    cmd = (unsigned char *)&DATAcommand;
    cmdlen = sizeof(DATAcommand);
    direction = (F < 8) ? 1 : 2;
  } else {
    NONDATAcommand.bytes[1] = F;
    NONDATAcommand.bytes[2] = Key.slot;
    NONDATAcommand.bytes[3] = A;
    NONDATAcommand.bytes[4] = (HIGHWAY_SERIAL << 7) | Key.crate;

    cmd = (unsigned char *)&NONDATAcommand;
    cmdlen = sizeof(NONDATAcommand);
    direction = 0;
  }
  memset(&sense, 0, sizeof(sense));
  sensretlen = 0;
  bytcnt = 0;
  scsi_lock(scsiDevice, 1);
  status = scsi_io(scsiDevice, direction, cmd, cmdlen, (char *)Data, reqbytcnt, (unsigned char *)&sense,
		   sizeof(sense), &sensretlen, &bytcnt);
  scsi_lock(scsiDevice, 0);
  status = JorwayTranslateIosb(reqbytcnt, &sense, sensretlen, bytcnt, status);
  if (iosb)
    *iosb = LastIosb;		// [2002.12.11]

 JorwayDoIo_Exit:
  if (MSGLVL(DETAILS) && (cmd != NULL)) {
    printf("%s(): iosb->status [0x%x]\n", J_ROUTINE_NAME, iosb->status);
    printf("%s(): iosb->x      [0x%x]\n", J_ROUTINE_NAME, iosb->x);
    printf("%s(): iosb->q      [0x%x]\n", J_ROUTINE_NAME, iosb->q);

//printf( "%s(): iosb->bytcnt [%d]\n", J_ROUTINE_NAME, iosb->bytcnt);   // [2002.12.11]
  }

//printf("JorwayDoIo(iosb)::->> bytecount= %d\n", iosb->bytcnt);        // [2002.12.13]
  return status;
}

//-----------------------------------------------------------
static int Jorway73ADoIo(CamKey Key,
			 BYTE A,
			 BYTE F,
			 int Count,
			 BYTE * Data, BYTE Mem, TranslatedIosb * iosb, int dmode, int Enhanced __attribute__ ((unused)))
{
  char dev_name[12];
  int IsDataCommand, scsiDevice;
  int status;
  unsigned char *cmd = 0;
  unsigned char cmdlen;
  int direction;
  int bytcnt;
  int reqbytcnt = 0;
  J73ASenseData sense;
  unsigned char sensretlen;
  int online;
  int enhanced;

  struct {
    __u8 opcode;

    __u8 f:5;
    __u8 lu:3;

    __u8 n:5;
    __u8 bs:1;
    __u8 m:2;

    __u8 a:4;
    __u8 zero1:4;

    __u8 transfer_len;
    __u8 zero2;
  } NONDATAcommand = {
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  struct {
    __u8 opcode;

    __u8 f:5;
    __u8 lu:3;

    __u8 n:5;
    __u8 bs:1;
    __u8 m:2;

    __u8 a:4;
    __u8 zero1:4;

    __u8 transfer_len;
    __u8 zero2;
  } ShortDATAcommand = {
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  struct {
    __u8 opcode;

    __u8 zero1:5;
    __u8 lu:3;

    __u8 f:5;
    __u8 zero2:3;

    __u8 n:5;
    __u8 bs:1;
    __u8 m:2;

    __u8 a:4;
    __u8 zero3:4;

    __u8 zero4;
    __u8 transfer_len[3];
    __u8 zero5;
  } LongDATAcommand = {
    0x21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {0}, 0};
  static char modes[4] = { 2, 2, 3, 1 };	/* QStop, QIgnore, QRep, QScan */
  static char singlemodes[4] = { 0, 2, 3, 1 };
  if (MSGLVL(FUNCTION_NAME))
    printf("%s()\n", J_ROUTINE_NAME);
//printf( "%s(iosb is %sNULL)\n", J_ROUTINE_NAME, (iosb)?"NOT ":"" );           // [2002.12.13]

  sprintf(dev_name, "GK%c%d%02d", Key.scsi_port, Key.scsi_address, Key.crate);
  if ((scsiDevice = get_scsi_device_number(dev_name, &enhanced, &online)) < 0) {
    if (MSGLVL(IMPORTANT))
      fprintf(stderr, "%s(): error -- no scsi device found for '%s'\n", J_ROUTINE_NAME, dev_name);

    status = NO_DEVICE;
    goto Jorway73ADoIo_Exit;
  }
  if (!online && Key.slot != 30)
    return CamOFFLINE;

  if (online && Key.slot == 30 && F == 1 && A == 0) {
    *(short *)Data = 0x30;
    return CamDONE_Q;
  }
  if (MSGLVL(DETAILS))
    printf("%s(): device '%s' = '/dev/sg%d'\n", J_ROUTINE_NAME, dev_name, scsiDevice);

  IsDataCommand = (F & 0x08) ? FALSE : TRUE;

  if (IsDataCommand) {
    union {
      BYTE b[4];
      unsigned int l;
    } transfer_len;
    reqbytcnt = transfer_len.l = Count * ((Mem == 24) ? 4 : 2);
    direction = (F < 8) ? 1 : 2;
    if (reqbytcnt < 256) {
      cmd = (unsigned char *)&ShortDATAcommand;
      cmdlen = sizeof(ShortDATAcommand);
      ShortDATAcommand.f = F;
      ShortDATAcommand.bs = Mem == 24;
      ShortDATAcommand.n = Key.slot;
      ShortDATAcommand.m = Count > 1 ? modes[dmode] : singlemodes[dmode];
      ShortDATAcommand.a = A;
      ShortDATAcommand.transfer_len = transfer_len.l;
    } else {
      cmd = (unsigned char *)&LongDATAcommand;
      cmdlen = sizeof(LongDATAcommand);
      LongDATAcommand.f = F;
      LongDATAcommand.bs = Mem == 24;
      LongDATAcommand.n = Key.slot;
      LongDATAcommand.m = modes[dmode];
      LongDATAcommand.a = A;
      LongDATAcommand.transfer_len[0] = transfer_len.b[2];	// NB! order reversal
      LongDATAcommand.transfer_len[1] = transfer_len.b[1];
      LongDATAcommand.transfer_len[2] = transfer_len.b[0];

    }
  } else {
    NONDATAcommand.f = F;
    NONDATAcommand.n = Key.slot;
    NONDATAcommand.a = A;
    cmd = (unsigned char *)&NONDATAcommand;
    cmdlen = sizeof(NONDATAcommand);
    direction = 0;
  }
  scsi_lock(scsiDevice, 1);
  status = scsi_io(scsiDevice, direction, cmd, cmdlen, (char *)Data, reqbytcnt, (unsigned char *)&sense,
		   sizeof(sense), &sensretlen, &bytcnt);
  scsi_lock(scsiDevice, 0);
  status = Jorway73ATranslateIosb(IsDataCommand, reqbytcnt, &sense, status);
  if (iosb)
    *iosb = LastIosb;		// [2002.12.11]

 Jorway73ADoIo_Exit:
  if (MSGLVL(FUNCTION_NAME + 1) && (cmd != NULL)) {
    // This is only rough - depends on the nature of the "overloaded" vars
    printf("scsi_mode opcode=%d, dmode=%d, modes[dmode]=%d, [1]=%d, [3]=%d, [5]=%d [7]=%d\n",
	   cmd[0], dmode, modes[dmode], cmd[1], cmd[3], cmd[5], cmd[7]);

    if (!iosb) {
      printf("Jorway73ADoIo_Exit: Null pointer to iosb\n");
    } else {
      printf("%s(): iosb->status [0x%x]\n", J_ROUTINE_NAME, iosb->status);
      printf("%s(): iosb->x      [0x%x]\n", J_ROUTINE_NAME, iosb->x);
      printf("%s(): iosb->q      [0x%x]\n", J_ROUTINE_NAME, iosb->q);

//printf( "%s(): iosb->bytcnt [%d]\n", J_ROUTINE_NAME, iosb->bytcnt);   // [2002.12.11]
    }
  }

//printf("Jorway73ADoIo(iosb)::->> bytecount= %d\n", iosb->bytcnt);     // [2002.12.13]
  return status;
}

//-----------------------------------------------------------
// static int KsSingleIo() { .. }
#include "KsS.c"

//-----------------------------------------------------------
// static int KsMultiIo() { .. }
#include "KsM.c"

//-----------------------------------------------------------
// derive module address parameters
//-----------------------------------------------------------
static int CamAssign(char *Name, CamKey * Key)
{
  static char ha;		// NB! these must be static, or else doesn't work
  static int id, crate, slot;	// NB! these must be static, or else doesn't work
  int status;

  if (strchr(Name, ':')) {	// physical names contain a ':'
    str2upcase(Name);
    sscanf(Name, "GK%1s%1d%2d:N%d", &ha, &id, &crate, &slot);
    Key->scsi_port = ha;
    Key->scsi_address = id;
    Key->crate = crate;
    Key->slot = slot;
  } else {			// ... logical module name does not
    if ((status = xlate_logicalname(Name, Key)) != SUCCESS) {
      status = LibNOTFOU;
      goto CamAssign_Exit;
    }
  }

  // final sanity check
  status = (Key->scsi_port >= 'A' && Key->scsi_port <= ('A' + MAX_SCSI_BUSES - 1) &&
	    Key->scsi_address >= 0 && Key->scsi_address <= 7 &&
	    Key->crate >= 0 && Key->crate <= 99 && Key->slot >= 0 && Key->slot <= 30)
      ? SUCCESS : LibNOTFOU;

 CamAssign_Exit:
  return status;
}

// extract CAMAC status info for Jorway highways
//-----------------------------------------------------------
static int JorwayTranslateIosb(int reqbytcnt, SenseData * sense, char sensretlen,
			       unsigned int ret_bytcnt, int scsi_status)
{
  int status;
  int bytcnt = reqbytcnt - ((int)sense->word_count_defect[2]) +
      (((int)sense->word_count_defect[1]) << 8) + (((int)sense->word_count_defect[0]) << 16);

  LastIosb.bytcnt = (unsigned short)(bytcnt & 0xffff);
  LastIosb.lbytcnt = (unsigned short)(bytcnt >> 16);
  LastIosb.x = 0;
  LastIosb.q = 0;
  LastIosb.err = 0;
  LastIosb.lpe = 0;
  LastIosb.tpe = 0;
  LastIosb.no_sync = 0;
  LastIosb.tmo = 0;
  LastIosb.adnr = 0;
  LastIosb.list = 0;

  if (MSGLVL(FUNCTION_NAME))
    printf("%s()\n", JT_ROUTINE_NAME);

  if (MSGLVL(DETAILS)) {
    printf("%s(): scsi status 0x%x\n", JT_ROUTINE_NAME, scsi_status);
  }

  LastIosb.q = !sense->main_status_reg.no_q;
  LastIosb.x = !sense->main_status_reg.no_x;
  status = CamSERTRAERR;
  switch (scsi_status) {
  case 0:
    status = 1;
    break;
  case 1:{
      switch (sense->sense_key) {
      case SENSE_HARDWARE_ERROR:
	if (sense->additional_sense_code == SENSE2_NOX) {
	  LastIosb.q = 0;
	  LastIosb.x = 0;
	  if (sense->main_status_reg.snex)
	    status = CamSCCFAIL;
	  else
	    status = CamDONE_NOX;
	} else if (sense->additional_sense_code == SENSE2_NOQ) {
	  LastIosb.q = 0;
	  status = CamDONE_NOQ;
	} else {
	  LastIosb.err = 1;
	  LastIosb.tmo = sense->main_status_reg.to | sense->serial_status_reg.timos;
	  LastIosb.no_sync = sense->serial_status_reg.losyn | sense->serial_status_reg.sync;
	  LastIosb.lpe = LastIosb.tpe = sense->serial_status_reg.rpe;
	}
	break;
      case SENSE_SHORT_TRANSFER:
	LastIosb.q = 0;
	status = CamDONE_NOQ;
	break;
      case SENSE_UNIT_ATTENTION:
	LastIosb.q = !sense->main_status_reg.no_q;
	LastIosb.x = !sense->main_status_reg.no_x;
	LastIosb.tmo = sense->main_status_reg.to | sense->serial_status_reg.timos;
	LastIosb.no_sync = sense->serial_status_reg.losyn | sense->serial_status_reg.sync;
	LastIosb.lpe = LastIosb.tpe = sense->serial_status_reg.rpe;
	LastIosb.err = LastIosb.lpe || LastIosb.no_sync || LastIosb.tmo;
	if (LastIosb.err)
	  break;
	if (!LastIosb.x)
	  status = CamDONE_NOX;
	else if (!LastIosb.q)
	  status = CamDONE_NOQ;
	else
	  status = CamDONE_Q;
	break;
      }
    }
    break;
  case 2:
    if (!LastIosb.x)
      status = CamDONE_NOX;
    else if (!LastIosb.q)
      status = CamDONE_NOQ;
    else
      status = CamDONE_Q;
    break;
  }
  LastIosb.status = (unsigned short)status & 0xffff;
  if (Verbose || status == CamSERTRAERR) {
    if (status == CamSERTRAERR) {
      printf("Serial Transmission Error detected, debug information follows \n\n"
	     "******************************************************************\n");
    }

    printf("Sense return length: %d, bytcnt: %d, scsi_status: %d, reqbytcnt: %d\n", sensretlen,
	   ret_bytcnt, scsi_status, reqbytcnt);
    printf("SCSI Sense data:  code=%d,valid=%d,sense_key=%d,word count deficit=%d\n\n", sense->code,
	   sense->valid, sense->sense_key,
	   ((int)sense->word_count_defect[2]) + (((int)sense->word_count_defect[1]) << 8) +
	   (((int)sense->word_count_defect[0]) << 16));
    printf("     Main status register:\n\n");
    printf("                  bdmd=%d,dsne=%d,bdsq=%d,snex=%d,crto=%d,to=%d,no_x=%d,no_q=%d\n\n",
	   sense->main_status_reg.bdmd, sense->main_status_reg.dsne, sense->main_status_reg.bdsq,
	   sense->main_status_reg.snex, sense->main_status_reg.crto, sense->main_status_reg.to,
	   sense->main_status_reg.no_x, sense->main_status_reg.no_q);
    printf("     Serial status register:\n\n");
    printf
	("                  cret=%d,timos=%d,rpe=%d,hdrrec=%d,cmdfor=%d,rnre1=%d,rnrg1=%d,snex=%d,hngd=%d\n",
	 sense->serial_status_reg.cret, sense->serial_status_reg.timos,
	 sense->serial_status_reg.rpe, sense->serial_status_reg.hdrrec,
	 sense->serial_status_reg.cmdfor, sense->serial_status_reg.rnre1,
	 sense->serial_status_reg.rnrg1, sense->serial_status_reg.snex,
	 sense->serial_status_reg.hngd);
    printf("                  sync=%d,losyn=%d,rerr=%d,derr=%d\n\n", sense->serial_status_reg.sync,
	   sense->serial_status_reg.losyn, sense->serial_status_reg.rerr,
	   sense->serial_status_reg.derr);
    printf("                  Additional Sense Code=%d,slot=%d,crate=%d\n\n",
	   sense->additional_sense_code, sense->slot_high_bit * 16 + sense->slot, sense->crate);
    if (status == CamSERTRAERR) {
      printf("******************************************************************\n\n");
    }
  }
  return status;
}

// extract CAMAC status info for Jorway highways
//-----------------------------------------------------------
static int Jorway73ATranslateIosb(int isdatacmd, int reqbytcnt, J73ASenseData * sense,
				  int scsi_status)
{
  int status;
  int bytcnt = reqbytcnt - (int)(((unsigned int)sense->DMA_byte_count[2]) +
				 (((unsigned int)sense->DMA_byte_count[1]) << 8) +
				 (((unsigned int)sense->DMA_byte_count[0]) << 16));

  if (Verbose) {
    printf
	("SCSI Sense data:  code=%d,valid=%d,sense_key=%d,DMA byte count=%d, additonal sense code=%d\n\n",
	 sense->code, sense->valid, sense->sense_key, bytcnt, sense->additional_sense_code);
    /*
       printf("     Main status register:\n\n");
       printf("                  bdmd=%d,dsne=%d,bdsq=%d,snex=%d,crto=%d,to=%d,no_x=%d,no_q=%d\n\n",
       sense->main_status_reg.bdmd,sense->main_status_reg.dsne,sense->main_status_reg.bdsq,
       sense->main_status_reg.snex,sense->main_status_reg.crto,sense->main_status_reg.to,
       sense->main_status_reg.no_x,sense->main_status_reg.no_q);
       printf("     Serial status register:\n\n");
       printf("                  cret=%d,timos=%d,rpe=%d,hdrrec=%d,cmdfor=%d,rnre1=%d,rnrg1=%d,snex=%d,hngd=%d\n",
       sense->serial_status_reg.cret,sense->serial_status_reg.timos,sense->serial_status_reg.rpe,
       sense->serial_status_reg.hdrrec,sense->serial_status_reg.cmdfor,
       sense->serial_status_reg.rnre1,sense->serial_status_reg.rnrg1,
       sense->serial_status_reg.snex,sense->serial_status_reg.hngd);
       printf("                  sync=%d,losyn=%d,rerr=%d,derr=%d\n\n",sense->serial_status_reg.sync,
       sense->serial_status_reg.losyn,sense->serial_status_reg.rerr,
       sense->serial_status_reg.derr);
       printf("                  Additional Sense Code=%d,slot=%d,crate=%d\n\n",sense->additional_sense_code,
       sense->slot_high_bit * 16 + sense->slot,sense->crate);
     */
  }
  LastIosb.x = 0;
  LastIosb.q = 0;
  LastIosb.err = 0;
  LastIosb.lpe = 0;
  LastIosb.tpe = 0;
  LastIosb.no_sync = 0;
  LastIosb.tmo = 0;
  LastIosb.adnr = 0;
  LastIosb.list = 0;
  LastIosb.bytcnt = (unsigned short)(bytcnt & 0xffff);
  LastIosb.lbytcnt = (unsigned short)(bytcnt >> 16);
  status = CamSCCFAIL;
  if (isdatacmd) {
    switch (scsi_status) {
    case 0:
      LastIosb.x = 1;
      LastIosb.q = 1;
      LastIosb.err = 0;
      LastIosb.lpe = 0;
      LastIosb.tpe = 0;
      LastIosb.no_sync = 0;
      LastIosb.tmo = 0;
      LastIosb.adnr = 0;
      LastIosb.list = 0;
      status = CamDONE_Q;
      break;
    case 1:
      LastIosb.x = sense->additional_sense_code != 0x44;
      if (sense->sense_key == 9 || (sense->sense_key == 4 && sense->additional_sense_code == 0)) {
	LastIosb.q = 0;
	status = CamDONE_NOQ;
      } else
	LastIosb.q = sense->additional_sense_code != 0x80;
      LastIosb.err = 0;
      LastIosb.lpe = sense->additional_sense_code == 0x47;
      LastIosb.tpe = sense->additional_sense_code == 0x47;
      LastIosb.no_sync = 0;
      LastIosb.tmo = 0;
      LastIosb.adnr = sense->additional_sense_code == 0x4;
      LastIosb.list = 0;
      switch (sense->additional_sense_code) {
      case 0x44:
	status = CamDONE_NOX;
	break;
      case 0x80:
	status = CamDONE_NOQ;
	break;
      }
      break;
    }
  } else {
    switch (scsi_status) {
    case 0:
      LastIosb.x = 1;
      LastIosb.q = 0;
      LastIosb.err = 0;
      LastIosb.lpe = 0;
      LastIosb.tpe = 0;
      LastIosb.no_sync = 0;
      LastIosb.tmo = 0;
      LastIosb.adnr = 0;
      LastIosb.list = 0;
      status = CamDONE_NOQ;
      break;
    case 1:
      LastIosb.x = 0;
      LastIosb.q = 0;
      LastIosb.err = 0;
      LastIosb.lpe = sense->additional_sense_code == 0x47;
      LastIosb.tpe = sense->additional_sense_code == 0x47;
      LastIosb.no_sync = 0;
      LastIosb.tmo = 0;
      LastIosb.adnr = sense->additional_sense_code == 0x4;
      LastIosb.list = 0;
      switch (sense->additional_sense_code) {
      case 0x44:
	status = CamDONE_NOX;
	break;
      case 0x80:
	status = CamDONE_NOQ;
	break;
      }
      break;
    case 2:
      LastIosb.x = 1;
      LastIosb.q = 1;
      LastIosb.err = 0;
      LastIosb.lpe = 0;
      LastIosb.tpe = 0;
      LastIosb.no_sync = 0;
      LastIosb.tmo = 0;
      LastIosb.adnr = 0;
      LastIosb.list = 0;
      status = CamDONE_Q;
      break;
    }
  }

  LastIosb.status = (unsigned short)status & 0xffff;
  return status;
}

//-----------------------------------------------------------
// extract CAMAC status info for KineticSystems highways
//-----------------------------------------------------------
// void KsTranslateIosb( UserParams *user ) { .. }
#include "KsT.c"

//-----------------------------------------------------------

//-----------------------------------------------------------
static void str2upcase(char *str)
{
  char *ptr;

  ptr = str;

  while (*ptr) {
    *ptr = toupper(*ptr);
    ptr++;
  }
}

//-----------------------------------------------------------
static int NOT_SUPPORTED()
{
  printf("reference to unsupported call made\n");
  return FAILURE;
}

EXPORT int CamSetMAXBUF(char *Name, int new)
{
  if ((isRemote == 1 || (isRemote == -1 && checkRemote()))) {
    return RemCamSetMAXBUF(Name, new);
  } else {
    int scsiDevice, enhanced, online;
    CamKey Key;
    int status = CamAssign(Name, &Key);
    if (status & 1) {
      char dev_name[12];
      sprintf(dev_name, "GK%c%d%02d", Key.scsi_port, Key.scsi_address, Key.crate);
      if ((scsiDevice = get_scsi_device_number(dev_name, &enhanced, &online)) < 0) {
	return -1;
      } else
	return SGSetMAXBUF(scsiDevice, new);
    } else {
      printf("Module: %s not defined\n", Name);
      return -1;
    }
  }
}

EXPORT int CamGetMAXBUF(char *Name)
{
  if ((isRemote == 1 || (isRemote == -1 && checkRemote()))) {
    return RemCamGetMAXBUF(Name);
  } else {
    int scsiDevice, enhanced, online;
    CamKey Key;
    int status = CamAssign(Name, &Key);
    if (status & 1) {
      char dev_name[12];
    sprintf(dev_name, "GK%c%d%02d", Key.scsi_port, Key.scsi_address, Key.crate);
    if ((scsiDevice = get_scsi_device_number(dev_name, &enhanced, &online)) < 0) {
      return -1;
    } else
      return SGGetMAXBUF(scsiDevice);
    } else {
      printf("Module: %s not defined\n", Name);
      return -1;
    }
  }
}
