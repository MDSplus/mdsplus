// CamFunctions.c ala Cam_...()
//-------------------------------------------------------------------------
//	Stuart Sherman
//	MIT / PSFC
//	Cambridge, MA 02139  USA
//
//	This is a port of the MDSplus system software from VMS to Linux, 
//	specifically:
//			CAMAC subsystem, ie libCamShr.so and verbs.c for CTS.
//-------------------------------------------------------------------------
//	$Id$
//-------------------------------------------------------------------------
// Tue Aug  1 11:22:06 EDT 2000
// Tue Apr  3 09:57:52 EDT 2001
// Wed Apr  4 15:29:00 EDT 2001
// Mon Apr  9 14:10:02 EDT 2001
// Thu Apr 12 12:52:09 EDT 2001
// Tue Jun 19 10:10:55 EDT 2001 -- cleanup, prettify
// Thu Jul 12 10:36:57 EDT 2001	-- fixed 'status' return
// Thu Jul 26 16:21:13 EDT 2001 -- changed way ScsiIo() called
// Mon Aug 20 16:33:50 EDT 2001	-- fixed parameter passing to/from ScsiIo()
// Tue Sep  4 16:04:22 EDT 2001	-- dynamic debug printout
// Fri Dec 13 15:17:46 EST 2002	-- fixing iosb->bytcnt -- not working, yet
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

#include <camdef.h>
#include <librtl_messages.h>
#include "common.h"
#include "prototypes.h"

//-----------------------------------------------------------
// scsi command lengths, per SCSI-2 spec (ANSI X3T9.2/375R)
// 		group 0:	 6 bytes
// 		group 1:	10 bytes
// 		group 2:	10 bytes
// 		group 3:	-- reserved
// 		group 4:	-- reserved
// 		group 5:	12 bytes
// 		group 6:	vendor specified
// 		group 7:	vendor specified
//
// linux 'sg()' driver, version 2.1.36
// 		group 0:	 6 bytes
// 		group 1:	10 bytes
// 		group 2:	10 bytes
// 		group 3:	12 bytes	NB difference
// 		group 4:	12 bytes	NB difference
// 		group 5:	12 bytes
// 		group 6:	10 bytes	NB difference
// 		group 7:	10 bytes	NB difference
//-----------------------------------------------------------
//                       group: 0   1   2   3   4   5   6   7
static BYTE scsi_cmd_size[] = { 6, 10, 10, 12, 12, 12, 10, 10 };
#define COMMAND_SIZE(opcode) scsi_cmd_size[((opcode) >> 5) & 7]

//-----------------------------------------------------------
// transfer modes
//-----------------------------------------------------------
static BYTE JorwayModes[2][2][4] = {
  {
  {	// non-enhanced modes
    STOP_MODE, 		// QStop			(M = 0)
    STOP_MODE, 			// QIgnore         	(M = 6)
    QREP_MODE,			// QRep				(M = 2)
    NO_MODE				// QScan	NB! tbi
  },
  {	// enhanced modes
    STOP_MODE,		//					(M = 4)
    STOP_MODE,			//					(M = 5)
    NO_MODE,
    NO_MODE	
  }
  },
  {
  {	// non-enhanced modes
    QSTOP_MODE, 		// QStop			(M = 0)
    STOP_MODE, 			// QIgnore         	(M = 6)
    QREP_MODE,			// QRep				(M = 2)
    NO_MODE				// QScan	NB! tbi
  },
  {	// enhanced modes
    FQSTOP_MODE,		//					(M = 4)
    FSTOP_MODE,			//					(M = 5)
    NO_MODE,
    NO_MODE	
  }
  }
};

#define	JORWAY_DISCONNECT			0
#define	JORWAYMODE(multisample, mode, enhanced)	JorwayModes[enhanced][mode][multisample]
#define	KSMODE(mode)				mode

//-----------------------------------------------------------
// local function prototypes
//-----------------------------------------------------------
static int  SingleIo(   CamKey 			Key, 
						BYTE 			A, 
						BYTE 			F,            
						BYTE 			*Data, 
						BYTE 			Mem, 
						TranslatedIosb *iosb, 
						int 			dmode 
						);
static int  MultiIo(    CamKey 			Key, 
						BYTE 			A, 
						BYTE 			F, 
						int 			Count, 
						BYTE 			*Data, 
						BYTE 			Mem, 
						TranslatedIosb *iosb, 
						int 			dmode, 
						int 			Enhanced 
						);
static int  JorwayDoIo( CamKey 			Key, 
						BYTE 			A, 
						BYTE 			F, 
						int 			Count, 
						BYTE 			*Data, 
						BYTE 			Mem, 
						TranslatedIosb *iosb, 
						int 			dmode, 
						int 			Enhanced 
						);
static int  KsSingleIo( CamKey 			Key, 
						BYTE 			A, 
						BYTE 			F,            
						BYTE 			*Data, 
						BYTE 			Mem, 
						TranslatedIosb *iosb, 
						int 			dmode 
						);
static int  KsMultiIo(  CamKey 			Key, 
						BYTE 			A, 
						BYTE 			F, 
						int 			Count, 
						BYTE 			*Data, 
						BYTE 			Mem, 
						TranslatedIosb *iosb, 
						int 			dmode, 
						int 			Enhanced 
						);

static int	CamAssign( char *Name, CamKey *Key );
static int  NOT_SUPPORTED();
static void str2upcase( char *str );

//-----------------------------------------------------------
// function prototypes -- not necessarily local
//-----------------------------------------------------------
void 		Blank( UserParams *user );
int 		JorwayTranslateIosb( int reqbytcnt, SenseData *sense, int scsi_status );
int 		KsTranslateIosb( RequestSenseData *sense, int scsi_status );

//-----------------------------------------------------------
// local, global
//-----------------------------------------------------------
static TranslatedIosb	LastIosb;
static int Verbose = 0;
//-----------------------------------------------------------
// helper routines
//-----------------------------------------------------------
int CamVerbose(int mode)
{
  Verbose = mode;
  return 1;
}

int CamQ( TranslatedIosb *iosb )			// CAM$Q_SCSI()
{
	TranslatedIosb	*iosb_use;
	iosb_use = (iosb) ? iosb : &LastIosb;

	if( MSGLVL(DETAILS) )
		printf( "CamQ(): using %s %p  CamQ()=%d\n",
			(iosb) ? "iosb" : "&LastIosb", 
			iosb,
			iosb_use->q
			);
//printf("CamQ(iosb)    ::->> bytecount= %d\n", iosb->bytcnt);		// [2002.12.13]
//printf("CamQ(iosb_use)::->> bytecount= %d\n", iosb_use->bytcnt);	// [2002.12.13]
//printf("CamQ(LastIosb)::->> bytecount= %d\n", LastIosb.bytcnt);		// [2002.12.13]
	return iosb_use->q;
}

//-----------------------------------------------------------
int CamX( TranslatedIosb *iosb )			// CAM$X_SCSI()
{
	TranslatedIosb	*iosb_use;
	iosb_use = (iosb) ? iosb : &LastIosb;

	if( MSGLVL(DETAILS) )
		printf( "CamX(): using %s %p  CamX()=%d\n",
			(iosb) ? "iosb" : "&LastIosb",
			iosb,
			iosb_use->x
			);

//printf("CamX(iosb)    ::->> bytecount= %d\n", iosb->bytcnt);		// [2002.12.13]
//printf("CamX(iosb_use)::->> bytecount= %d\n", iosb_use->bytcnt);	// [2002.12.11]
//printf("CamX(LastIosb)::->> bytecount= %d\n", LastIosb.bytcnt);		// [2002.12.13]
	return iosb_use->x;
}

//-----------------------------------------------------------
int CamXandQ( TranslatedIosb *iosb )		// CAM$XANDQ_SCSI()
{
	TranslatedIosb	*iosb_use;
	iosb_use = (iosb) ? iosb : &LastIosb;

	if( MSGLVL(DETAILS) )
		printf( "CamXandQ(): using %s %p  %s .x=%d .q=%d  CamXandQ()=%d\n",
			(iosb) ? "iosb" : "&LastIosb",
			iosb,
			(iosb) ? "iosb" : "&LastIosb", 
			iosb_use->x,
			iosb_use->q,
			iosb_use->x && iosb_use->q
			);

//printf("CamXnQ(iosb)    ::->> bytecount= %d\n", iosb->bytcnt);		// [2002.12.13]
//printf("CamXnQ(iosb_use)::->> bytecount= %d\n", iosb_use->bytcnt);	// [2002.12.13]
//printf("CamXnQ(LastIosb)::->> bytecount= %d\n", LastIosb.bytcnt);	// [2002.12.13]
	return iosb_use->x && iosb_use->q;
}

//-----------------------------------------------------------
int CamBytcnt( TranslatedIosb *iosb )		// CAM$BYTCNT_SCSI()
{
	TranslatedIosb	*iosb_use;
	iosb_use = (iosb) ? iosb : &LastIosb;

	if( MSGLVL(DETAILS) )
		printf( "CamBytcnt(): using %s %p  CamBytcnt()=%d\n",
			(iosb) ? "iosb" : "&LastIosb",
			iosb,
			iosb_use->bytcnt
			);

//printf("CamBytcnt(iosb)    ::->> bytecount= %d\n", iosb->bytcnt);		// [2002.12.13]
//printf("CamBytcnt(iosb_use)::->> bytecount= %d\n", iosb_use->bytcnt);	// [2002.12.13]
//printf("CamBytcnt(LastIosb)::->> bytecount= %d\n", LastIosb.bytcnt);	// [2002.12.13]
	return ((int)iosb_use->bytcnt) | (((int)iosb_use->lbytcnt) << 16);
}

//-----------------------------------------------------------
int CamStatus( TranslatedIosb *iosb )		// CAM$STATUS_SCSI()
{
	TranslatedIosb	*iosb_use;
	iosb_use = (iosb) ? iosb : &LastIosb;

//printf("CamStatus(iosb)    ::->> bytecount= %d\n", iosb->bytcnt);		// [2002.12.13]
//printf("CamStatus(iosb_use)::->> bytecount= %d\n", iosb_use->bytcnt);	// [2002.12.13]
//printf("CamStatus(LastIosb)::->> bytecount= %d\n", LastIosb.bytcnt);	// [2002.12.13]
	return iosb_use->status;
}

//-----------------------------------------------------------
int CamGetStat( TranslatedIosb *iosb )		// CAM$GET_STAT_SCSI()
{
	*iosb = LastIosb;
//printf("CamGetStatus(iosb)    ::->> bytecount= %d\n", iosb->bytcnt);	// [2002.12.13]
//printf("CamGetStatus(LastIosb)::->> bytecount= %d\n", LastIosb.bytcnt);	// [2002.12.13]
	return 1;
}

//-----------------------------------------------------------
int CamError( int xexp, int qexp, TranslatedIosb *iosb )
{
	int				xexp_use;
	int				qexp_use;
	TranslatedIosb	*iosb_use;

	if( MSGLVL(DETAILS) )
		printf( "CamError(): xexp:%d qexp:%d\n", xexp, qexp );

	xexp_use = xexp ? xexp : 0;
	qexp_use = qexp ? qexp : 0;
	iosb_use = iosb ? iosb : &LastIosb;

	iosb_use->err = !iosb_use->x && !iosb_use->q;

	if( MSGLVL(DETAILS) )
		printf( "CamError(): x:%d q:%d iosb->err %d\n", xexp_use, qexp_use, iosb_use->err );

	return iosb_use->err;
}

//-----------------------------------------------------------
// Pio modes
// NB! it has been found empirically that the length
// 	of the message for a CAMAC non-data message must
// 	be set to 10 bytes, not the expected 6. The reasoning
// 	is left as an excercise for the reader ;-)
// NB! NEWS Flash -- SCSI-2 spec says that group 6 & 7
// 	commands are 10 bytes long, and further, group 6 & 7
// 	commands are vendor specific. But the scsi driver wants
// 	10 bytes length, so that's what we give it.
//-----------------------------------------------------------
#define	CallSingleIo( pname, dmode )                \
int Cam##pname(                                     \
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
                                                    \
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
int Cam##pname(                                     \
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
CallSingleIo( Piow,    QIgnore )	// yields: int = CamPiow( ... )
CallSingleIo( PioQrepw, QRep    )	//       : int = CamPioQrepw( ... )

CallMultiIo( FQstopw, QStop,   1 )	//       : int = CamFQstopw( ... )		[2001.09.13]
CallMultiIo( FStopw,  QIgnore, 1 )	//       : int = CamFStopw( ... )		[2001.09.13]
CallMultiIo( FQrepw,  QRep,    1 )	//       : int = CamQrepw( ... )		[2001.04.18]
CallMultiIo( Qrepw,   QRep,    0 )	//       : int = CamQrepw( ... )		[2001.04.18]
CallMultiIo( Qscanw,  QScan,   0 )	//       : int = CamQscanw( ... )		[2001.04.18]
CallMultiIo( Qstopw,  QStop,   0 )	//       : int = CamQstopw( ... )		[2001.08.30]
CallMultiIo( Stopw,   QIgnore, 0 )	//       : int = CamStopw( ... )		[2001.04.10]

//-----------------------------------------------------------
static int SingleIo(
					CamKey			Key,
					BYTE			A,
					BYTE			F,
					BYTE			*Data,
					BYTE			Mem,
					TranslatedIosb	*iosb,
					int				dmode
					)
{
	char	tmp[7];
	int		highwayType, status;

	if( MSGLVL(FUNCTION_NAME) )
		printf( "SingleIo()-->>\n" );

	sprintf(tmp, "GK%c%d", Key.scsi_port, Key.scsi_address);
	if( (status = QueryHighwayType( tmp )) == SUCCESS ) {
		highwayType = NUMERIC(tmp[5]);				// extract type

		switch( highwayType ) {						// check highway type
			case JORWAY:
			case JORWAY_OLD:
				if( MSGLVL(DETAILS) )
					printf( "-->>JorwayDoIo()\n" );

				status = JorwayDoIo(
									Key, 			// module info
									A, 				// module sub address
									F, 				// module function
									1, 				// implied count of 1
									Data, 			// data
									Mem, 			// 16 or 24 bit data
									iosb,			// status struct
									dmode,			// mode
									0				// non-enhanced
									);
				break;

			case KINSYSCO:
				if( MSGLVL(DETAILS) )
					printf( "-->>KsSingleIo()\n" );

				status = KsSingleIo(
									Key,			// module info
									A,				// module sub-address
									F,				// module function
									Data,			// data
									Mem,			// 16 or 24 bit data
									iosb,			// status struct
									KSMODE(dmode)	// mode
									);
				break;

			default:
				if( MSGLVL(IMPORTANT) )
					fprintf( stderr, "highway type(%d) not supported\n", highwayType );

				status = FAILURE;
				break;
		} // end of switch()
	} // end of if()

//printf("SingleIo(iosb)::->> bytecount= %d\n", iosb->bytcnt);	// [2002.12.13]
	return status;
}

//-----------------------------------------------------------
static int MultiIo(
					CamKey			Key,
					BYTE			A,
					BYTE			F,
					int				Count,
					BYTE			*Data,
					BYTE			Mem,
					TranslatedIosb	*iosb,
					int				dmode,
					int				Enhanced
					)
{
	char	tmp[7];
	int		highwayType, mode, status;

	if( MSGLVL(FUNCTION_NAME) )
		printf( "MultiIo()-->>\n" );

	sprintf(tmp, "GK%c%d", Key.scsi_port, Key.scsi_address);

	if( (status = QueryHighwayType( tmp )) == SUCCESS ) {
		highwayType = NUMERIC(tmp[5]);				// extract type

		switch( highwayType ) {						// check highway type
			case JORWAY:
			case JORWAY_OLD:
				if( MSGLVL(DETAILS) )
					printf( "-->>JorwayDoIo()\n" );

				mode = JORWAYMODE(dmode, (Enhanced && highwayType == JORWAY), Count > 1);
				if( mode != NO_MODE )
					status = JorwayDoIo(
									Key, 			// module info
									A, 				// module sub address
									F, 				// module function
									Count,			// data count int bytes
									Data, 			// data
									Mem, 			// 16 or 24 bit data
									iosb,			// status struct
									dmode,			// mode
									Enhanced		// highway mode
									);
				else
					status = NOT_SUPPORTED();

				break;

			case KINSYSCO:
				if( MSGLVL(DETAILS) )
					printf( "-->>KsMultiIo()\n" );

				status = KsMultiIo(
									Key,			// module info
									A,				// module sub-address
									F,				// module function
									Count,			// data count in bytes
									Data,			// data
									Mem,			// 16 or 24 bit data
									iosb,			// status struct
									KSMODE(dmode),	// mode
									Enhanced		// enhanced
									);
				break;

			default:
				if( MSGLVL(IMPORTANT) )
					fprintf( stderr, "highway type(%d) not supported\n", highwayType );

				status = FAILURE;
				break;
		} // end of switch()
	} // end of if()

//printf("MultiIo(iosb)::->> bytecount= %d\n", iosb->bytcnt);	// [2002.12.13]
	return status;
}

//-----------------------------------------------------------
static int JorwayDoIo(
					CamKey			Key,
					BYTE			A,
					BYTE			F,
					int				Count,
					BYTE			*Data,
					BYTE			Mem,
					TranslatedIosb	*iosb,
					int				dmode,
					int				Enhanced
					)
{
	char		dev_name[5];
	int			IsDataCommand, scsiDevice;
	int 		xfer_data_length;
	int 		status;
        unsigned char *cmd;
        unsigned char cmdlen;
        int direction;
        unsigned int bytcnt;
        int reqbytcnt = 0;
        SenseData sense;
        char sensretlen;
        int online;
        int enhanced;
	CDBCamacDataCommand( DATAcommand );
	CDBCamacNonDataCommand( NONDATAcommand );

	if( MSGLVL(FUNCTION_NAME) )
		printf( "%s()\n", J_ROUTINE_NAME );
//printf( "%s(iosb is %sNULL)\n", J_ROUTINE_NAME, (iosb)?"NOT ":"" );		// [2002.12.13]

	sprintf(dev_name, "GK%c%d%02d", Key.scsi_port, Key.scsi_address, Key.crate);
        if( (scsiDevice = get_scsi_device_number( dev_name, &enhanced, &online )) < 0 ) {
		if( MSGLVL(IMPORTANT) )
			fprintf( stderr, "%s(): error -- no scsi device found for '%s'\n", J_ROUTINE_NAME, dev_name );

		status = NO_DEVICE;
		goto JorwayDoIo_Exit;
	}
        if (!online && Key.slot != 30)
          return CamOFFLINE;

        if (!Enhanced)
          enhanced = 0;
	if( MSGLVL(DETAILS) )
		printf( "%s(): device '%s' = '/dev/sg%d'\n", J_ROUTINE_NAME, dev_name, scsiDevice );

	IsDataCommand = (F & 0x08) ? FALSE : TRUE;

	if( IsDataCommand ) {
		union {
			BYTE	     b[4];
			unsigned int l;
		} transfer_len;

		DATAcommand.f     = F;
		DATAcommand.bs    = Mem == 24;
		DATAcommand.n     = Key.slot;
		DATAcommand.m     = JORWAYMODE(dmode, enhanced, Count > 1);
		DATAcommand.a     = A;
		DATAcommand.sncx  = 0;
		DATAcommand.scs   = 0;

		reqbytcnt = transfer_len.l    = Count * ((Mem == 24) ? 4 : 2);
#	if 	JORWAY_DISCONNECT
#		ifdef SG_BIG_BUFF
			DATAcommand.dd = transfer_len.l > SG_BIG_BUFF;
#		else
			DATAcommand.dd = transfer_len.l > 4096;
#		endif
#	else
		DATAcommand.dd    = 0;
#	endif
		DATAcommand.crate = Key.crate;
		DATAcommand.sp    = HIGHWAY_SERIAL;

		DATAcommand.transfer_len[0] = transfer_len.b[2];	// NB! order reversal
		DATAcommand.transfer_len[1] = transfer_len.b[1];
		DATAcommand.transfer_len[2] = transfer_len.b[0];

                cmd = (unsigned char *)&DATAcommand;
                cmdlen = sizeof(DATAcommand);
                direction = (F < 8) ? 1 : 2;
	}
	else {
		NONDATAcommand.bytes[1] = F;
		NONDATAcommand.bytes[2] = Key.slot;
		NONDATAcommand.bytes[3] = A;
		NONDATAcommand.bytes[4] = (HIGHWAY_SERIAL << 7) | Key.crate;

		cmd = (unsigned char *)&NONDATAcommand;
		cmdlen = sizeof(NONDATAcommand);
                direction = 0;
	}
        scsi_lock(scsiDevice,1);
        status = scsi_io( scsiDevice, direction, cmd, cmdlen, Data, reqbytcnt, (unsigned char *)&sense,
			  sizeof(sense), &sensretlen, &bytcnt);
        scsi_lock(scsiDevice,0);
        status = JorwayTranslateIosb(reqbytcnt,&sense,status);
	if ( iosb ) *iosb = LastIosb;					// [2002.12.11]


JorwayDoIo_Exit:
	if( MSGLVL(DETAILS) ) {
		printf( "%s(): iosb->status [0x%x]\n", J_ROUTINE_NAME, iosb->status );
		printf( "%s(): iosb->x      [0x%x]\n", J_ROUTINE_NAME, iosb->x      );
		printf( "%s(): iosb->q      [0x%x]\n", J_ROUTINE_NAME, iosb->q      );

//printf( "%s(): iosb->bytcnt [%d]\n", J_ROUTINE_NAME, iosb->bytcnt);	// [2002.12.11]
	}


//printf("JorwayDoIo(iosb)::->> bytecount= %d\n", iosb->bytcnt);	// [2002.12.13]
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
static int CamAssign( char *Name, CamKey *Key )
{
	static char ha;					// NB! these must be static, or else doesn't work
	static int  id, crate, slot;	// NB! these must be static, or else doesn't work
	int			status;

	if( strchr(Name, ':') ) {		// physical names contain a ':'
		str2upcase( Name );
		sscanf(Name, "GK%1s%1d%2d:N%d", &ha, &id, &crate, &slot);
		Key->scsi_port    = ha;
		Key->scsi_address = id;
		Key->crate        = crate;
		Key->slot         = slot;
	}
	else {							// ... logical module name does not
		if( (status = xlate_logicalname( Name, Key )) != SUCCESS ) {
			status = LibNOTFOU;
			goto CamAssign_Exit;
		}
	}

	// final sanity check
	status = ( Key->scsi_port    >= 'A' && Key->scsi_port    <= ('A' + MAX_SCSI_BUSES - 1) &&
			   Key->scsi_address >= 0   && Key->scsi_address <= 7                          &&
			   Key->crate        >= 0   && Key->crate        <= 99                         &&
			   Key->slot         >= 0   && Key->slot         <= 30                            )
			   ? SUCCESS : LibNOTFOU;

CamAssign_Exit:
	return status;
}
// extract CAMAC status info for Jorway highways
//-----------------------------------------------------------
JorwayTranslateIosb( int reqbytcnt, SenseData *sense, int scsi_status )
{
  int status;
  int bytcnt = reqbytcnt - ((int)sense->word_count_defect[2])+
    (((int)sense->word_count_defect[1])<<8)+
    (((int)sense->word_count_defect[0])<<16);
 
  if (Verbose)
  {
    printf("SCSI Sense data:  code=%d,valid=%d,sense_key=%d,word count deficit=%d\n\n",sense->code,sense->valid,
	   sense->sense_key, ((int)sense->word_count_defect[2])+
	   (((int)sense->word_count_defect[1])<<8)+
	   (((int)sense->word_count_defect[0])<<16));
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
  }
	LastIosb.bytcnt = (unsigned short)(bytcnt & 0xffff);
        LastIosb.lbytcnt = (unsigned short)(bytcnt >> 16);
        LastIosb.x=0;
        LastIosb.q=0;
        LastIosb.err=0;
        LastIosb.lpe=0;
        LastIosb.tpe=0;
        LastIosb.no_sync=0;
        LastIosb.tmo=0;
        LastIosb.adnr=0;
        LastIosb.list=0;

	if( MSGLVL(FUNCTION_NAME) )
		printf( "%s()\n", JT_ROUTINE_NAME );

	if( MSGLVL(DETAILS) ) {
		printf( "%s(): scsi status 0x%x\n", JT_ROUTINE_NAME, scsi_status );
	}

        LastIosb.q = !sense->main_status_reg.no_q;
        LastIosb.x = !sense->main_status_reg.no_x;
	status = CamSERTRAERR;
        switch (scsi_status) {
        case 0:
          status = 1;
          break;
	case 1: {
          switch(sense->sense_key) {
            case SENSE_HARDWARE_ERROR:
              if (sense->additional_sense_code == SENSE2_NOX) {
                LastIosb.q = 0;
                LastIosb.x = 0;
                if (sense->main_status_reg.snex)
                  status = CamSCCFAIL;
                else
                  status = CamDONE_NOX;
              }
              else if (sense->additional_sense_code == SENSE2_NOQ) {
                LastIosb.q = 0;
                status = CamDONE_NOQ;
              }
              else {
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
              if (LastIosb.err) break;
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
        LastIosb.status = (unsigned short)status&0xffff;
	return status;
}

//-----------------------------------------------------------
// extract CAMAC status info for KineticSystems highways
//-----------------------------------------------------------
// void KsTranslateIosb( UserParams *user ) { .. }
#include "KsT.c"

//-----------------------------------------------------------
void Blank( UserParams *user )
{
	if( MSGLVL(FUNCTION_NAME) )
		printf( "Blank()\n" );
}

//-----------------------------------------------------------
static void str2upcase( char *str )
{
	char *ptr;

	ptr = str;

	while( *ptr ) {
		*ptr = toupper(*ptr);
		ptr++;
	}
}

//-----------------------------------------------------------
static int NOT_SUPPORTED() { printf("reference to unsupported call made\n"); return FAILURE; }

int CamSetMAXBUF(char *Name, int new)
{
  int scsiDevice,enhanced,online;
  CamKey Key;
  int status = CamAssign( Name, &Key );
  if (status & 1)
  {
    char dev_name[20];
    sprintf(dev_name, "GK%c%d%02d", Key.scsi_port, Key.scsi_address, Key.crate);
    if( (scsiDevice = get_scsi_device_number( dev_name, &enhanced, &online )) < 0 )
    {
      return -1;
    }
    else
      return SGSetMAXBUF(scsiDevice,new);
  }
  else
  {
    printf("Module: %s not defined\n",Name);
    return -1;
  }
}

int CamGetMAXBUF(char *Name)
{
  int scsiDevice,enhanced,online;
  CamKey Key;
  int status = CamAssign( Name, &Key );
  if (status & 1)
  {
    char dev_name[20];
    sprintf(dev_name, "GK%c%d%02d", Key.scsi_port, Key.scsi_address, Key.crate);
    if( (scsiDevice = get_scsi_device_number( dev_name, &enhanced, &online )) < 0 )
    {
      return -1;
    }
    else
      return SGGetMAXBUF(scsiDevice);
  }
  else
  {
      printf("Module: %s not defined\n",Name);
      return -1;
  }
}
