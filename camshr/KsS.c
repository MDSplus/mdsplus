// KsSingleIo.c
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

//-----------------------------------------------------------
static int KsSingleIo(
					CamKey			Key,
					BYTE			A,
					BYTE			F,
					BYTE			*Data,
					BYTE			Mem,
					TranslatedIosb	*iosb,		// to be returned to caller
					int				dmode
					)
{
	char			dev_name[5];
	BYTE			Command[COMMAND_SIZE(OpCodeSingleCAMAC)];
	int				rc, scsiDevice;
	int				status = SUCCESS;			// optimistic -- function status, eg SUCCESS(=1) or FAILURE(=0)
	UserParams 		*user;
	SCSIdescr 		scsi_descr;

	if( MSGLVL(FUNCTION_NAME) )
		printf( "%s()\n", KS_ROUTINE_NAME );

	// allocate some memory
	if( (user = (UserParams *)malloc(sizeof(UserParams))) == NULL) {
		if( MSGLVL(ALWAYS) )
			fprintf( stderr, "%s(): malloc() failed\n", KS_ROUTINE_NAME );

		status = NO_MEMORY;
		goto KsSingleIo_Exit;
	}

	// find the scsi device number (ie '/dev/sg#')
	sprintf(dev_name, "GK%c%d", Key.scsi_port, Key.scsi_address);
    if( (scsiDevice = get_scsi_device_number( dev_name )) < 0 ) {
		if( MSGLVL(IMPORTANT) )
			fprintf( stderr, "%s(): error -- no scsi device found for '%s'\n", KS_ROUTINE_NAME, dev_name );

		status = NO_DEVICE;
		goto KsSingleIo_Exit;
	}
	if( MSGLVL(DETAILS) )
		printf( "%s(): device '%s' = '/dev/sg%d'\n", KS_ROUTINE_NAME, dev_name, scsiDevice );

	// prepare CAMAC command
	memset(Command, 0, COMMAND_SIZE(OpCodeSingleCAMAC));
	Command[0] = OpCodeSingleCAMAC;
	Command[2] = Key.crate;
	Command[3] = MODE(dmode, Mem);
	Command[4] = NAFhi(Key.slot, A, F);
	Command[5] = NAFlo(Key.slot, A, F);

	// load up structure members
	scsi_descr.command     = &Command[0];
	scsi_descr.command_len = COMMAND_SIZE(OpCodeSingleCAMAC);
	scsi_descr.direction   = (F < 16) ? FROM_CAMAC : TO_CAMAC;
	scsi_descr.data        = (F < 7 || F > 15) ? (BYTE*)Data : 0;		// [2001.10.03]
	scsi_descr.data_len    = scsi_descr.data ? ((Mem == 16) ? 2 : 4) : 0;
	scsi_descr.sense       = (BYTE *)&user->sense;
	scsi_descr.sense_len   = SG_MAX_SENSE;

	user->iosb             = iosb;			// 'translated' version
	user->scsi_descr       = scsi_descr;
	user->key              = Key;			// need to pass this to ScsiIo() to pass to KsTranslateIosb() [2001.09.07]
	
	if( MSGLVL(8) ) {
		int i;
		printf("##########################\n");
		printf("%s()\n", KS_ROUTINE_NAME);
		printf( "scsi command: " );
		for(i=0; i < user->scsi_descr.command_len; ++i)
			printf("0x%02x ", *(user->scsi_descr.command+i));
		printf("\n");
		printf("cmd len:      %d\n", user->scsi_descr.command_len);
		printf("direction:    %s_CAMAC\n", (user->scsi_descr.direction==TO_CAMAC) ? "TO" : "FROM");
		if( user->scsi_descr.data_len ) {
			printf("data:         ");
			for(i=0; i < user->scsi_descr.data_len; ++i)
				printf("%s0x%02x ", (i%16==0 && i)?"\n              ":"", *(user->scsi_descr.data+i));
			printf("\n");
		}
		printf("data len:     %d\n", user->scsi_descr.data_len);
		printf("##########################\n");
	}

	// talk to the physical device
	rc = ScsiIo( scsiDevice, user, KsTranslateIosb );

	if( MSGLVL(DETAILS) )
		printf( "%s(): ScsiIo() returned %d\n", KS_ROUTINE_NAME, rc );

	if( MSGLVL(8) ) {
		if( rc == SUCCESS ) {	// ScsiIo() returned OK
			printf("##########################\n");
			printf("kinetic sense data\n");
			printf("error code:       0x%02x\n", user->sense.error_code);
			printf("sense key:        0x%02x\n", user->sense.sense_key);
			printf("sense code:       0x%02x\n", user->sense.sense_code);
			printf("sense qual:       0x%02x\n", user->sense.sense_qual);
			printf("no_x:             0x%02x\n", user->sense.u2.esr.nox);
			printf("no_q:             0x%02x\n", user->sense.u2.esr.noq);
			printf("##########################\n");
		}
	}

	iosb->status  = status;			// CAMAC status
	iosb->bytcnt  = (Data) ? ((Mem == 16) ? 2 : 4) : 0;
	iosb->x       = !user->sense.u2.esr.nox;
	iosb->q       = !user->sense.u2.esr.noq;
	iosb->err     = 0;
	iosb->lpe     = 0;
	iosb->tpe     = 0;
	iosb->no_sync = 0;
	iosb->tmo     = 0;
	iosb->adnr    = 0;
	iosb->list    = 0;
	iosb->lbytcnt = 0;

KsSingleIo_Exit:
	if( MSGLVL(DETAILS) ) {
		printf( "%s(): iosb->status [0x%x]\n", KS_ROUTINE_NAME, iosb->status );
		printf( "%s(): iosb->bytcnt [%d]\n",   KS_ROUTINE_NAME, iosb->bytcnt );
		printf( "%s(): iosb->x      [0x%x]\n", KS_ROUTINE_NAME, iosb->x      );
		printf( "%s(): iosb->q      [0x%x]\n", KS_ROUTINE_NAME, iosb->q      );
	}

	if( user->scsi_descr.direction == FROM_CAMAC  && user->scsi_descr.data_len ) {
		memcpy((BYTE*)Data, user->scsi_descr.data, user->scsi_descr.data_len);
	}

	if( user )
		free(user);

	return status;
}
