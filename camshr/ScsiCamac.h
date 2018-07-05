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

#include "ESR.h"

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
#include "RSD.h"

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
#include "CDC.h"

#include "SD.h"

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
