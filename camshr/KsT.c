// KsTranslateIosb.c
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
// Mon Oct 15 16:35:42 EDT 2001	-- seperated out
//-----------------------------------------------------------

// local function prototypes
static char *ShowLeds( int data );

//-----------------------------------------------------------
void KsTranslateIosb( UserParams *user )
{
	char		dev_name[5];
	int			scsiDevice, status; 
	SCSIdescr	scsi_descr;
	UserParams	*localUser;

	union {
		ErrorStatusRegister esr;
		int     l;
		BYTE	b[4];
	} u;

	if( MSGLVL(FUNCTION_NAME) )
		printf( "%s()\n", KT_ROUTINE_NAME );

	if( MSGLVL(DETAILS) ) {
		printf( "%s(): user->actual_iosb.scsi_status 0x%x\n",          KT_ROUTINE_NAME, user->actual_iosb.scsi_status               );
		printf( "%s(): user->actual_iosb.bytcnt      %d\n",            KT_ROUTINE_NAME, user->actual_iosb.bytcnt                    );
		printf( "%s(): user->actual_iosb.status      %d\n",            KT_ROUTINE_NAME, user->actual_iosb.status                    );
		printf( "%s(): user->scsi_descr.command      0x%02x\n",        KT_ROUTINE_NAME, *user->scsi_descr.command                   );
		printf( "%s(): user->scsi_descr.data_len     %d\n",            KT_ROUTINE_NAME, user->scsi_descr.data_len                   );
		if( user->scsi_descr.data_len )
			printf( "%s(): user->scsi_descr.data         0x%02x 0x%02x\n", 
				KT_ROUTINE_NAME, 
				*(user->scsi_descr.data), 
				*(user->scsi_descr.data + 1)
				);
		printf( "%s(): user->key.scsi_port=%c  .scsi_address=%d\n",    KT_ROUTINE_NAME, user->key.scsi_port, user->key.scsi_address );
	}

	memset(&LastIosb, 0, sizeof(LastIosb));
	LastIosb.status = Shorten( CamSERTRAERR );

	if( user->actual_iosb.status & 1 ) {			// Ks*Io() command returned OK
		Iosb 				iosb;

		CDBRequestSense( Sense_Command );
		CDBRegisterAccess( Register_Command, 0x80, 0x01 );	// error-status register	<lo-hi>

		// local instance
		if( (localUser = (UserParams *)malloc(sizeof(UserParams))) == NULL ) {
			printf( "'localUser' -- malloc error\n" );
			status = NO_MEMORY;
			goto KsTranslateIosb_Exit;
		}
		if( MSGLVL(DETAILS) ) printf( "allocated 'localUser' OK\n" );

		// find scsi device number (ie '/dev/sg#')
		sprintf(dev_name, "GK%c%d", user->key.scsi_port, user->key.scsi_address);
    	if( (scsiDevice = get_scsi_device_number( dev_name )) < 0 ) {
			if( MSGLVL(IMPORTANT) )
				fprintf( stderr, "%s(): error -- no scsi device found for '%s'\n", KT_ROUTINE_NAME, dev_name );

			status = NO_DEVICE;
			goto KsTranslateIosb_Exit;
		}

		// request sense
		scsi_descr.command     = (BYTE*)&Sense_Command;
		scsi_descr.command_len = sizeof(Sense_Command);
		scsi_descr.direction   = FROM_CAMAC;
		scsi_descr.data        = 0;
		scsi_descr.data_len    = 0;
		scsi_descr.sense       = (BYTE*)&user->sense;
		scsi_descr.sense_len   = SG_MAX_SENSE;
		localUser->scsi_descr  = scsi_descr;
		localUser->actual_iosb = iosb;

        // talk to the physical device
		status = ScsiIo( scsiDevice, localUser, Blank );

		// read highway cpu register @ address 0x180 <<-- error/status register
		scsi_descr.command     = (BYTE *)&Register_Command;
		scsi_descr.command_len = sizeof(Register_Command);
		scsi_descr.direction   = FROM_CAMAC;
		scsi_descr.data        = (BYTE*)&u.l;
		scsi_descr.data_len    = sizeof(ErrorStatusRegister);
		scsi_descr.sense       = (BYTE*)&user->sense;
		scsi_descr.sense_len   = SG_MAX_SENSE;
		localUser->scsi_descr  = scsi_descr;
		localUser->actual_iosb = iosb;

        // talk to the physical device
		status = ScsiIo( scsiDevice, localUser, Blank );

		// extract some status codes
		user->sense.error_code = *(localUser->scsi_descr.sense     ) & 0x7f;
		user->sense.sense_key  = *(localUser->scsi_descr.sense +  2) & 0x0f;
		user->sense.sense_code = *(localUser->scsi_descr.sense + 12);
		user->sense.sense_qual = *(localUser->scsi_descr.sense + 13);

		// extract status bits
		user->sense.u2.esr = u.esr;

	if( MSGLVL(8) ) {
		printf("%s():\n", KT_ROUTINE_NAME);
		printf("error code: 0x%x\n", user->sense.error_code);
		printf("sense key:  0x%x\n", user->sense.sense_key);
		printf("sense code: 0x%x\n", user->sense.sense_code);
		printf("sense qual: 0x%x\n", user->sense.sense_qual);
		printf("esr: 0x%08x\n", u.l);
		printf("noq:	 %s%d%s\n", (user->sense.u2.esr.noq    ) ? CYAN : NORMAL, user->sense.u2.esr.noq,     NORMAL);
		printf("nox:	 %s%d%s\n", (user->sense.u2.esr.nox    ) ? CYAN : NORMAL, user->sense.u2.esr.nox,     NORMAL);
		printf("adnr:	 %s%d%s\n", (user->sense.u2.esr.adnr   ) ? CYAN : NORMAL, user->sense.u2.esr.adnr,    NORMAL);
		printf("tpe:	 %s%d%s\n", (user->sense.u2.esr.tpe    ) ? CYAN : NORMAL, user->sense.u2.esr.tpe,     NORMAL);
		printf("lpe:	 %s%d%s\n", (user->sense.u2.esr.lpe    ) ? CYAN : NORMAL, user->sense.u2.esr.lpe,     NORMAL);
		printf("err:	 %s%d%s\n", (user->sense.u2.esr.err    ) ? CYAN : NORMAL, user->sense.u2.esr.err,     NORMAL);
		printf("no_sync: %s%d%s\n", (user->sense.u2.esr.no_sync) ? CYAN : NORMAL, user->sense.u2.esr.no_sync, NORMAL);
		printf("tmo:	 %s%d%s\n", (user->sense.u2.esr.tmo    ) ? CYAN : NORMAL, user->sense.u2.esr.tmo,     NORMAL);
		printf("LEDS:    %s\n", ShowLeds((BYTE)(u.l >> 16) & 0xf));
		printf("back from reading cpu register\n");
	}

		status = (status & 1) ? localUser->actual_iosb.status : status;

		if( status & 1 ) {
			LastIosb.bytcnt = user->actual_iosb.bytcnt;

			switch( user->sense.u2.esr.error_code ) {
				case 0:
					LastIosb.status = Shorten( CamDONE_Q );
					if( MSGLVL(DETAILS) )
						printf("CamDONE_Q\n");
					break;
				case 7:
					LastIosb.status = Shorten( CamDONE_NOQ );
					if( MSGLVL(DETAILS) )
						printf("CamDONE_NOQ\n");
					break;
				case 8:
					LastIosb.status = Shorten( CamDONE_NOX );
					if( MSGLVL(DETAILS) )
						printf("CamDONE_NOX\n");
					break;
				case 12:
					LastIosb.status = Shorten( CamSCCFAIL );
					if( MSGLVL(DETAILS) )
						printf("CamSCCFAIL\n");
					break;
				default:
					LastIosb.status = Shorten( CamSERTRAERR );
					if( MSGLVL(DETAILS) )
						printf("CamSERTRAERR\n");
					break;
			}

			LastIosb.x       = !user->sense.u2.esr.nox;
			LastIosb.q       = !user->sense.u2.esr.noq;
			LastIosb.err     =  user->sense.u2.esr.err;
			LastIosb.lpe     =  user->sense.u2.esr.lpe;
			LastIosb.tpe     =  user->sense.u2.esr.tpe;
			LastIosb.no_sync =  user->sense.u2.esr.no_sync;
			LastIosb.tmo     =  user->sense.u2.esr.tmo;
			LastIosb.adnr    =  user->sense.u2.esr.adnr;
			LastIosb.list    =  0;			// list processing not supported
			LastIosb.lbytcnt =  0;			// list processing not supported
		}
		else {
			LastIosb.status = status;
			LastIosb.bytcnt = 0;
		}
	}
	else
		LastIosb.status = user->actual_iosb.status;

	if( MSGLVL(DETAILS) )
		printf( "%s(): user->iosb is", KT_ROUTINE_NAME );
	if( user->iosb ) {
		if( MSGLVL(DETAILS) )
			printf( " not" );
		*user->iosb = LastIosb;
	}
	if( MSGLVL(DETAILS) )
		printf( " NULL\n" );

KsTranslateIosb_Exit:
	if( MSGLVL(DETAILS) ) {
		printf( "%s(): LastIosb.status = %d [0x%x]\n", KT_ROUTINE_NAME, LastIosb.status, LastIosb.status );
		printf( "%s(): LastIosb.x = %d\n",             KT_ROUTINE_NAME, LastIosb.x                       );
		printf( "%s(): LastIosb.q = %d\n",             KT_ROUTINE_NAME, LastIosb.q                       );
	}

	if( localUser )
		free(localUser);

	return;
}


//-----------------------------------------------------------
#define	MAX_LEDS	4
static char *ShowLeds( int data )
{
	static char output[64], *p;
	char 		tmp[strlen(RED) + strlen(NORMAL) + 2];	// allow enough space
	int 		i, LedState;

	p = &output[0];		// point to beginning of string
	strcpy(p, "");		// initialize string

	for( i = MAX_LEDS; i; --i ) {
		LedState = data >> (i - 1) & 1;
		sprintf(tmp, "%s%d%s ", 
					(LedState) ? RED: WHITE, 
					LedState, 
					NORMAL
					);

		strcat(p, tmp);
	}

	return p;
}
