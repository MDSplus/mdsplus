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
/************************************************************
 * ScsiCamac.h                                              *
 * NB! this header file was copied from work by TWF and JAS *
 ************************************************************/
/*  CMS REPLACEMENT HISTORY, Element SCSI_CAMAC.H */
/*  *1    22-JUN-2000 15:10:00 SRS "structure definitions for SCSI CAMAC" */
/*  CMS REPLACEMENT HISTORY, Element SCSI_CAMAC.H */
#ifndef __SCSICAMAC_H
#define __SCSICAMAC_H

#include "OS.h"

#if OS == LINUX
// for Linux of x86
#include <stdint.h>
#define __u8 uint8_t
#define __u16 uint16_t
#define __u32 uint32_t
#endif

#define QSTOP_MODE        		0
#define QREP_MODE         		2
#define FQSTOP_MODE       		4
#define FSTOP_MODE        		5
#define STOP_MODE         		6
#define STOP_ON_WORD_MODE 		7	/* used for PIO */

#define NO_MODE          	   -1	/* unsupported I/0 mode */
#define SP                		1	/* serial(=>1)  parallel(=>0) flag */

//  sense codes
#define SENSE_NO_SENSE			0x0
#define SENSE_HARDWARE_ERROR	0x4
#define SENSE_ILLEGAL_REQUEST	0x5
#define SENSE_UNIT_ATTENTION	0x6
#define SENSE_SHORT_TRANSFER	0x9

//  additional sense codes
#define SENSE2_NO_SENSE			0x00
#define SENSE2_INVCMD  			0x20
#define SENSE2_INVCDB  			0x24
#define SENSE2_UNSUPLUN			0x25
#define SENSE2_RESET   			0x29
#define SENSE2_INVIDMSG			0x3d
#define SENSE2_NOX      		0x44
#define SENSE2_SCSIPARITY  		0x47
#define SENSE2_NOQ     			0x80

// transfer modes
#define QStop   				0
#define QIgnore 				1
#define QRep    				2
#define QScan   				3

#define OpCodeTestUnitReady			0
#define CDBTestUnitReady(name)      struct {char bytes[6];} name = {{OpCodeTestUnitReady,0,0,0,0,0}}

#define OpCodeRequestSense			3
#define CDBRequestSense(name)  		struct {char bytes[6];} name = {{OpCodeRequestSense,0,0,0,sizeof(RequestSenseData),0}}

#define OpCodeRegisterAccess		0x0D
#define CDBRegisterAccess(name,addr_l,addr_h)  struct {char bytes[6];} name = {{OpCodeRegisterAccess,0,addr_h,addr_l,1,0}}

#define OpCodeInquiryCommand		0x12
#define CDBInquiryCommand(name)  	struct {char bytes[6];} name = {{OpCodeInquiryCommand,0,0,0,sizeof(InquiryData),0}}

#define OpCodeSendDiagnostic		0x1D
#define CDBSendDiagnostic(x) 		SendDiagnostic x = {OpCodeSendDiagnostic, 0,0,1,0,0,0,0}

#define OpCodeSingleCAMAC			0x21

#define OpCodeBlockCAMAC			0xA2

typedef struct {
  char bytes[6];
} CamacNonData;
#define OpCodeCamacNonDataCommand	0xC1
#define CDBCamacNonDataCommand(name)  CamacNonData  name = {{OpCodeCamacNonDataCommand,0,0,0,0,0}}

#define OpCodeCamacDataCommand 		0xE1
#define CDBCamacDataCommand(name)  CamacDataCommand name = {OpCodeCamacDataCommand, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {0, 0, 0}, 0}

#if OS == VMS
// original version
typedef struct {
  unsigned noq:1;
  unsigned nox:1;
  unsigned ste:1;
  unsigned adnr:1;
  unsigned tpe:1;
  unsigned lpe:1;
  unsigned n_gt_23:1;
  unsigned err:1;
  unsigned no_sync:1;
  unsigned tmo:1;
  unsigned derr:1;
  unsigned should_be_zero:5;
  unsigned error_code:4;
  unsigned should_be_0:2;
  unsigned no_halt:1;
  unsigned read:1;
  unsigned ad:1;
  unsigned word_size:2;
  unsigned qmd:2;
  unsigned tm:2;
  unsigned cm:1;
} ErrorStatusRegister;

#elif OS == LINUX
// Linux version
#include "ESR.h"
#endif

typedef struct {
  unsigned short status;
  unsigned short bytcnt;
  unsigned fill:24;
  unsigned char scsi_status;
} Iosb;

typedef struct {
  unsigned short status;
  unsigned short bytcnt;
  unsigned x:1;
  unsigned q:1;
  unsigned err:1;
  unsigned lpe:1;
  unsigned tpe:1;
  unsigned no_sync:1;
  unsigned tmo:1;
  unsigned adnr:1;
  unsigned list:1;
  unsigned fill:7;
  unsigned short lbytcnt;
} TranslatedIosb;

typedef struct {
  char scsi_port;
  char scsi_address;
  char slot;
  char crate;
} CamKey;

//  Request Sense Command - page 26
#if OS == VMS
// original version
typedef struct {
  unsigned error_code:7;
  unsigned valid:1;
  unsigned segment_number:8;
  unsigned sense_key:4;
  unsigned rsvd:1;
  unsigned ili:1;
  unsigned eom:1;
  unsigned fm:1;
  unsigned int information_long;
  unsigned char sense_length;
  unsigned int command_specific_information;
  unsigned char sense_code;
  unsigned char sense_qual;
  unsigned reserved;
  unsigned int stat;

  union {
    unsigned short stacsr;
    struct {
      unsigned noq:1;
      unsigned nox:1;
      unsigned done:1;
      unsigned lam_pending:1;
      unsigned:1;
      unsigned qrpt_timeout:1;
      unsigned:1;
      unsigned abort:1;
      unsigned xmt_fifo_empty:1;
      unsigned xmt_fifo_full:1;
      unsigned rcv_fifo_empty:1;
      unsigned rcv_fifo_full:1;
      unsigned high_byte_first:1;
      unsigned scsi_id:3;
    } csr;
  } u1;

  union {
    unsigned int staesr;
    ErrorStatusRegister esr;
  } u2;

  unsigned int staccs;
  unsigned int stasum;
  unsigned int stacnt;
} RequestSenseData;
#elif OS == LINUX
// Linux version
#include "RSD.h"
#endif

//  Inquiry Command - page 37
typedef struct {
  unsigned device_type:5;
  unsigned device_qual:3;
  unsigned reserved_1:7;
  unsigned removable:1;
  unsigned ansi_version:3;
  unsigned ecma_version:3;
  unsigned iso_version:2;
  unsigned response_data_format:4;
  unsigned reserved_2:2;
  unsigned trmiop:1;
  unsigned aenc:1;
  unsigned char additional_length;
  unsigned char reserved_3;
  unsigned char reserved_4;
  unsigned srst:1;
  unsigned que:1;
  unsigned rsvd:1;
  unsigned link:1;
  unsigned sync:1;
  unsigned wbus16:1;
  unsigned wbus32:1;
  unsigned reladr:1;
  char identification[49];
} InquiryData;

// Send Diagnostic Command - page 41

// CAMAC NAF Specifications - Page 70
typedef struct {
  unsigned f:5;
  unsigned a:4;
  unsigned n:5;
  unsigned zero:2;
} NAF;

// Executing Single Transfer Operations -  Page 71
//   and
// Executing Block Transfer Operations  -  Page 74
typedef struct {
  unsigned ignore_x:1;
  unsigned bits_16:1;
  unsigned zero1:1;
  unsigned mode:2;
  unsigned conservative:1;
  unsigned enhanced:1;
  unsigned zero2:1;
} TransferOp;

// Send Diagnostic Command - page 41
typedef struct {
  unsigned char opcode;
  unsigned unitofl:1;
  unsigned devofl:1;
  unsigned slftest:1;
  unsigned rsvd:1;
  unsigned pf:1;
  unsigned lun:3;
  unsigned long reserved;
} SendDiagnostic;

// Single CAMAC Operation Command - page 42
typedef struct {
  unsigned char opcode;
  unsigned char zero1;
  unsigned char crate;
  TransferOp mode;
  char naf_h;
  char naf_l;
  unsigned char zero2[4];
} SingleCAMAC;

// Block CAMAC Operation Command - page 43
typedef struct {
  unsigned char opcode;
  unsigned char zero1;
  unsigned char crate;
  TransferOp mode;
  char naf_h;
  char naf_l;
  char bytcnt_hi;
  char bytcnt_mid;
  char bytcnt_lo;
  unsigned char zero2[3];
} BlockCAMAC;

typedef struct {
  unsigned int opcode;
  unsigned direction:1;
  unsigned disconnect:1;
  unsigned synchronous:1;
  unsigned obsolete:1;
  unsigned tagged_req:1;
  unsigned tag:3;
  unsigned autosense:1;
  unsigned zero1:23;
  unsigned char *command;
  unsigned int command_len;
  unsigned char *data;
  unsigned int data_len;
  unsigned int pad_len;
  unsigned int phase_change_timeout;
  unsigned int disconnect_timeout;
  char *sense;
  int sense_len;
  unsigned int zero2[4];
} SCSIdescr;

// Register Access Command - page 50

// CAMAC non data command  F8-15 and F24-F31
// CAMAC data commands
#if OS == VMS
typedef struct {
  unsigned char opcode;
  unsigned char zero1;
  unsigned f:5;
  unsigned bs:1;		/* 1->24 bit  0->16 bit */
  unsigned zero2:2;
  unsigned n:5;
  unsigned m:3;
  unsigned a:4;
  unsigned sncx:1;		/* skip non existant 0 */
  unsigned scs:1;		/* single crate scan 0 */
  unsigned dd:1;		/* disables SCSI detection and reconnection 0 */
  unsigned zero3:1;
  unsigned crate:7;
  unsigned sp:1;		/* serial or parallel */
  unsigned char transfer_len[3];	/* reverse order bytes !! */
  unsigned char zero4;
} CamacDataCommand;
#elif OS == LINUX
// Linux version
#include "CDC.h"
#endif

#if OS == VMS
// original version
typedef struct {
  unsigned code:7;
  unsigned valid:1;
  unsigned char segment_no;	/* 0 */
  unsigned sense_key:4;
  unsigned zero1:12;
  unsigned char word_count_deficit[3];	/* reverse order !!!! */
  unsigned char additional_sense_length;

  struct {
    unsigned bdmd:1;
    unsigned dsne:1;
    unsigned bdsq:1;
    unsigned snex:1;
    unsigned crto:1;
    unsigned to:1;
    unsigned no_x:1;
    unsigned no_q:1;
  } main_status_reg;

  struct {
    /* 'high' byte */
    unsigned cret:1;
    unsigned timos:1;
    unsigned rpe:1;
    unsigned hdrrec:1;
    unsigned cmdfor:1;
    unsigned rnre1:1;
    unsigned rnrg1:1;
    unsigned snex:1;
    /* 'low' byte */
    unsigned fill:2;
    unsigned hngd:1;
    unsigned spare:1;
    unsigned sync:1;
    unsigned losyn:1;
    unsigned rerr:1;
    unsigned derr:1;
  } serial_status_reg;		/* reversed bytes !!!! */

  unsigned char zero2;
  unsigned char additional_sense_code;
  unsigned char zero3;
  unsigned slot_high_bit:1;
  unsigned char crate:7;
  unsigned char address:4;
  unsigned char slot:4;
} SenseData;
#elif OS == LINUX
// Linux version
#include "SD.h"
#endif

typedef struct {
  Iosb actual_iosb;
  TranslatedIosb *iosb;
  int wait;
  void (*ast) ();
  int astprm;
  CamKey key;
  BlockCAMAC b_command;
  SingleCAMAC s_command;
  SCSIdescr scsi_descr;
  CamacNonData nondata_command;
  CamacDataCommand data_command;
  RequestSenseData sense;
  SenseData jorway_sense;
} UserParams;

#endif
