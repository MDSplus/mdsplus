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

#include "camdef.h"
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
static BYTE JorwayModes[][4] = {
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
			};

#define	JORWAY_DISCONNECT			0
#define	JORWAYMODE(mode, enhanced)	JorwayModes[enhanced][mode]
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
void 		JorwayTranslateIosb( UserParams *user );
void 		KsTranslateIosb( UserParams *user );

//-----------------------------------------------------------
// local, global
//-----------------------------------------------------------
static TranslatedIosb	LastIosb;

//-----------------------------------------------------------
// helper routines
//-----------------------------------------------------------
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

	return iosb_use->bytcnt;
}

//-----------------------------------------------------------
int CamStatus( TranslatedIosb *iosb )		// CAM$STATUS_SCSI()
{
	TranslatedIosb	*iosb_use;
	iosb_use = (iosb) ? iosb : &LastIosb;

	return iosb_use->status;
}

//-----------------------------------------------------------
int CamGetStat( TranslatedIosb *iosb )		// CAM$GET_STAT_SCSI()
{
	*iosb = LastIosb;
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
CallSingleIo( PioQrep, QRep    )	//       : int = CamPioQrep( ... )

//CallMultiIo( FQstop, QStop,   1 )	//       : int = CamFQstop( ... )
//CallMultiIo( FStop,  QIgnore, 1 )	//       : int = CamFStop( ... )
CallMultiIo( FQstopw, QStop,   1 )	//       : int = CamFQstopw( ... )		[2001.09.13]
CallMultiIo( FStopw,  QIgnore, 1 )	//       : int = CamFStopw( ... )		[2001.09.13]

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

				mode = JORWAYMODE(dmode, (Enhanced && highwayType == JORWAY));
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
	UserParams  *user;
	SCSIdescr	scsi_descr;

	if( MSGLVL(FUNCTION_NAME) )
		printf( "%s()\n", J_ROUTINE_NAME );

	if( (user = (UserParams *)malloc(sizeof(UserParams))) == NULL) {
		if( MSGLVL(ALWAYS) )
			fprintf( stderr, "%s(): malloc() failed\n", J_ROUTINE_NAME );

		status = NO_MEMORY;
		goto JorwayDoIo_Exit;
	}

	sprintf(dev_name, "GK%c%d", Key.scsi_port, Key.scsi_address);
    if( (scsiDevice = get_scsi_device_number( dev_name )) < 0 ) {
		if( MSGLVL(IMPORTANT) )
			fprintf( stderr, "%s(): error -- no scsi device found for '%s'\n", J_ROUTINE_NAME, dev_name );

		status = NO_DEVICE;
		goto JorwayDoIo_Exit;
	}

	if( MSGLVL(DETAILS) )
		printf( "%s(): device '%s' = '/dev/sg%d'\n", J_ROUTINE_NAME, dev_name, scsiDevice );

	IsDataCommand = (F & 0x08) ? FALSE : TRUE;

	if( IsDataCommand ) {
		union {
			BYTE	     b[4];
			unsigned int l;
		} transfer_len;

		CDBCamacDataCommand( DATAcommand );
		DATAcommand.f     = F;
		DATAcommand.bs    = Mem == 24;
		DATAcommand.n     = Key.slot;
		DATAcommand.m     = JORWAYMODE(dmode, Enhanced);
		DATAcommand.a     = A;
		DATAcommand.sncx  = 0;
		DATAcommand.scs   = 0;

		transfer_len.l    = Count * ((Mem == 24) ? 4 : 2);
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

		user->data_command      = DATAcommand;
		scsi_descr.command      = (BYTE *)&user->data_command;
		scsi_descr.command_len  = sizeof(user->data_command);
		scsi_descr.direction    = (F < 16) ? FROM_CAMAC : TO_CAMAC;
		scsi_descr.data         = (BYTE *)Data;
		scsi_descr.data_len     = transfer_len.l;
	}
	else {
		CDBCamacNonDataCommand( NONDATAcommand );
		NONDATAcommand.bytes[1] = F;
		NONDATAcommand.bytes[2] = Key.slot;
		NONDATAcommand.bytes[3] = A;
		NONDATAcommand.bytes[4] = (HIGHWAY_SERIAL << 7) | Key.crate;

		user->nondata_command   = NONDATAcommand;
		scsi_descr.command      = (BYTE *)&user->nondata_command;
		scsi_descr.command_len  = COMMAND_SIZE(NONDATAcommand.bytes[0]);
		scsi_descr.direction    = (F < 16) ? FROM_CAMAC : TO_CAMAC;
		scsi_descr.data         = 0;
		scsi_descr.data_len     = 0;
	}

	if( MSGLVL(DETAILS) )
		printf( "%s(): F = %2d  A = %2d\n", J_ROUTINE_NAME, F, A );

	// common assignments
	scsi_descr.sense            = (BYTE *)&user->jorway_sense;
	user->iosb                  = iosb;
	user->key                   = Key;
	user->scsi_descr            = scsi_descr;

	if( MSGLVL(DETAILS) ) {
		int i;
		printf( "%s(): %s cmd\n", J_ROUTINE_NAME, IsDataCommand ? "data" : "non-data" );
		printf( "%s(): direction:   %s CAMAC\n", J_ROUTINE_NAME, (F<16) ? "from[read]" : "to[write]" );
		printf( "%s(): xfer length: %d\n", J_ROUTINE_NAME, scsi_descr.data_len );
		if( scsi_descr.data_len ) {
			printf( "%s():", J_ROUTINE_NAME );
			for( i = 0; i < scsi_descr.data_len; ++i ) 
				printf( "%s0x%02x ", (i%ItemsPerLine==0) ? "\n" : "", *(BYTE*)(Data+i) );
			printf( "\n" );
		}
	}

	memset((void *)(&user->jorway_sense), -1, sizeof(user->jorway_sense));

	if( MSGLVL(8) ) {
		int i;
		printf("##########################\n");
		printf("%s():\n", J_ROUTINE_NAME);
		printf("scsi command: ");
		for(i=0; i < user->scsi_descr.command_len; ++i)
			printf("0x%02x ", *(user->scsi_descr.command+i));
		printf("\n");
		printf("cmd len:     %d\n", user->scsi_descr.command_len);
		printf("direction:   %s_CAMAC\n", (user->scsi_descr.direction==TO_CAMAC) ? "TO" : "FROM");
		if( user->scsi_descr.data_len ) {
			printf("data:         ");
			for(i=0; i < user->scsi_descr.data_len; ++i)
				printf("%s0x%02x ", (i%16==0 && i)?"\n             ":"", *(user->scsi_descr.data+i));
			printf("\n");
		}
		printf("data len:    %d\n", user->scsi_descr.data_len);
		printf("##########################\n");
	}

	// talk to the physical device
	status = ScsiIo( scsiDevice, user, JorwayTranslateIosb );

	if( MSGLVL(8) ) {
		printf("##########################\n");
		printf("jorway sense data\n");
		printf("error code:       0x%02x\n", user->jorway_sense.code);
		printf("sense key:        0x%02x\n", user->jorway_sense.sense_key);
		printf("addnl sense code: 0x%02x\n", user->jorway_sense.additional_sense_code);
		printf("no_x:             0x%02x\n", user->jorway_sense.main_status_reg.no_x);
		printf("no_q:             0x%02x\n", user->jorway_sense.main_status_reg.no_q);
		printf("##########################\n");
	}
	
	iosb->status  = status;				// result of ScsiIo()  command
	iosb->bytcnt  = xfer_data_length;
	iosb->x       = !user->jorway_sense.main_status_reg.no_x;
	iosb->q       = !user->jorway_sense.main_status_reg.no_q;
	iosb->err     = 0;
	iosb->lpe     = 0;
	iosb->tpe     = 0;
	iosb->no_sync = 0;
	iosb->tmo     = 0;
	iosb->adnr    = 0;
	iosb->list    = 0;
	iosb->lbytcnt = 0;

JorwayDoIo_Exit:
	if( MSGLVL(DETAILS) ) {
		printf( "%s(): iosb->status [0x%x]\n", J_ROUTINE_NAME, iosb->status );
		printf( "%s(): iosb->x      [0x%x]\n", J_ROUTINE_NAME, iosb->x      );
		printf( "%s(): iosb->q      [0x%x]\n", J_ROUTINE_NAME, iosb->q      );
	}

	if( user )
		free(user);

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
			status = FAILURE;
			goto CamAssign_Exit;
		}
	}

	// final sanity check
	status = ( Key->scsi_port    >= 'A' && Key->scsi_port    <= ('A' + MAX_SCSI_BUSES - 1) &&
			   Key->scsi_address >= 0   && Key->scsi_address <= 7                          &&
			   Key->crate        >= 0   && Key->crate        <= 99                         &&
			   Key->slot         >= 0   && Key->slot         <= 30                            )
			   ? SUCCESS : FAILURE;

CamAssign_Exit:
	return status;
}

//-----------------------------------------------------------
// extract CAMAC status info for Jorway highways
//-----------------------------------------------------------
void JorwayTranslateIosb( UserParams *user )
{
	static const TranslatedIosb QIosb = { 0,	// status
										  0,	// bytcnt
										  1,	// x ------------------------+
										  1,	// q ------------------------|
										  0,	// err ----------------------|
										  0,	// lpe ----------------------|
										  0,	// tpe ----------------------|--- bit flags
										  0,	// no_sync ------------------|
										  0,	// tmo ----------------------|
										  0,	// adnr ---------------------+
										  0,	// list		NB! always 0
										  0,	// --filler--
										  0 	// lbytcnt	NB! always 0
										};
	LastIosb = QIosb;

	if( MSGLVL(FUNCTION_NAME) )
		printf( "%s()\n", JT_ROUTINE_NAME );

	if( MSGLVL(DETAILS) ) {
		printf( "%s(): user->actual_iosb.scsi_status 0x%x\n", JT_ROUTINE_NAME, user->actual_iosb.scsi_status );
		printf( "%s(): user->actual_iosb.bytcnt      %d\n",   JT_ROUTINE_NAME, user->actual_iosb.bytcnt      );
		printf( "%s(): user->actual_iosb.status      %d\n",   JT_ROUTINE_NAME, user->actual_iosb.status      );
		printf( "%s(): user->scsi_descr.data_len     %d\n",   JT_ROUTINE_NAME, user->scsi_descr.data_len     );
	}

	if( user->actual_iosb.status & 1 ) {
		// assign status information
		user->jorway_sense.code                    = *(user->scsi_descr.sense     ) & 0x7f;
		user->jorway_sense.sense_key               = *(user->scsi_descr.sense +  2) & 0x0f;
		user->jorway_sense.additional_sense_code   = *(user->scsi_descr.sense + 12);

		user->jorway_sense.main_status_reg.snex    = *(user->scsi_descr.sense +  8) & SNEX;
		user->jorway_sense.main_status_reg.to      = *(user->scsi_descr.sense +  8) & TO;
		user->jorway_sense.main_status_reg.no_x    = *(user->scsi_descr.sense +  8) & NO_X;
		user->jorway_sense.main_status_reg.no_q    = *(user->scsi_descr.sense +  8) & NO_Q;

		user->jorway_sense.serial_status_reg.sync  = *(user->scsi_descr.sense + 10) & SYNC;
		user->jorway_sense.serial_status_reg.losyn = *(user->scsi_descr.sense + 10) & LOSYN;
		user->jorway_sense.serial_status_reg.timos = *(user->scsi_descr.sense +  9) & TIMOS;
		user->jorway_sense.serial_status_reg.rpe   = *(user->scsi_descr.sense +  9) & RPE;

		// default status
		LastIosb.status = Shorten( CamSERTRAERR );

		switch( user->actual_iosb.scsi_status ) {
			case 0:		// GOOD
				LastIosb.x = 1;
				LastIosb.q = ((user->actual_iosb.bytcnt != user->scsi_descr.data_len) ) ? 0 : 1;
				LastIosb.status = 1;

				if( MSGLVL(DETAILS) )
					printf( "%s(): 'GOOD' LastIosb.q = %d\n", JT_ROUTINE_NAME, LastIosb.q );
				break;

			case 2:		// CHECK_CONDITION
				switch( user->jorway_sense.sense_key ) {
					case SENSE_HARDWARE_ERROR:
						if( user->jorway_sense.additional_sense_code == SENSE2_NOX ) {
							LastIosb.q = 0;
							LastIosb.x = 0;
							if( user->jorway_sense.main_status_reg.snex )
								LastIosb.status = Shorten( CamSCCFAIL );
							else
								LastIosb.status = Shorten( CamDONE_NOX );
						}
						else if( user->jorway_sense.additional_sense_code == SENSE2_NOQ ) {
							LastIosb.q = 0;
							LastIosb.status = Shorten( CamDONE_NOQ );
						}
						else {
							LastIosb.err     = 1;
							LastIosb.q       = !user->jorway_sense.main_status_reg.no_q;
							LastIosb.x       = !user->jorway_sense.main_status_reg.no_x;
							LastIosb.tmo     =  user->jorway_sense.main_status_reg.to | user->jorway_sense.serial_status_reg.timos;
							LastIosb.no_sync =  user->jorway_sense.serial_status_reg.losyn | user->jorway_sense.serial_status_reg.sync;
							LastIosb.lpe     =  LastIosb.tpe = user->jorway_sense.serial_status_reg.rpe;
						}
						break;

					case SENSE_SHORT_TRANSFER:
						LastIosb.q      = 0;
						LastIosb.status = Shorten( CamDONE_NOQ );
						break;
					
					case SENSE_UNIT_ATTENTION:
						LastIosb.q       = !user->jorway_sense.main_status_reg.no_q;
						LastIosb.x       = !user->jorway_sense.main_status_reg.no_x;
						LastIosb.tmo     =  user->jorway_sense.main_status_reg.to | user->jorway_sense.serial_status_reg.timos;
						LastIosb.no_sync =  user->jorway_sense.serial_status_reg.losyn | user->jorway_sense.serial_status_reg.sync;
						LastIosb.lpe     =  LastIosb.tpe = user->jorway_sense.serial_status_reg.rpe;
						LastIosb.err     =  LastIosb.lpe || LastIosb.no_sync || LastIosb.tmo;
						if( LastIosb.err )
							break;
						if( !LastIosb.x )
							LastIosb.status = Shorten( CamDONE_NOX );
						else if( !LastIosb.q )
							LastIosb.status = Shorten( CamDONE_NOQ );
						else
							LastIosb.status = Shorten( CamDONE_Q   );
						break;
				}
				break;

			case 4:		// CONDITION_MET
				LastIosb.q = LastIosb.x = 1;
				LastIosb.status = Shorten( CamDONE_Q );
				break;
		}
	}
	else
		LastIosb.status = user->actual_iosb.status;

	LastIosb.bytcnt = user->actual_iosb.bytcnt;		// [2001.10.03]

	if( MSGLVL(DETAILS) ) {
		printf( "%s(): LastIosb.status = %d\n", JT_ROUTINE_NAME, LastIosb.status );
		printf( "%s(): LastIosb.x = %d\n",      JT_ROUTINE_NAME, LastIosb.x      );
		printf( "%s(): LastIosb.q = %d\n",      JT_ROUTINE_NAME, LastIosb.q      );
	}

	if( MSGLVL(DETAILS) )
		printf( "%s(): user->iosb is", JT_ROUTINE_NAME );
	if( user->iosb ) {
		if( MSGLVL(DETAILS) )
			printf( " not" );
		*user->iosb = LastIosb;
	}
	if( MSGLVL(DETAILS) )
		printf( " NULL\n" );

	return;
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
