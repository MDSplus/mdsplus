// KsMultiIo.c
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
static int KsMultiIo( 
					CamKey			Key,		// module info
					BYTE			A,			// module sub-address
					BYTE			F,			// module function
					int				Count,		// data count in bytes
					BYTE			*Data,		// data
					BYTE			Mem,		// 16 or 24 bit data
					TranslatedIosb	*iosb,		// status struct
					int				dmode,		// mode
					int				Enhanced	// enhanced
					)
{
	char			dev_name[5];
	BYTE			Command[COMMAND_SIZE(OpCodeBlockCAMAC)];
	int				scsiDevice, status;
	UserParams		*user;
	SCSIdescr		scsi_descr;

	union {
		long	l;
		BYTE	b[4];
	} xfer_len;

	if( MSGLVL(FUNCTION_NAME) )
		printf( "%s()\n", KM_ROUTINE_NAME );

	if( (user = (UserParams *)malloc(sizeof(UserParams))) == NULL) {
		if( MSGLVL(ALWAYS) )
			fprintf( stderr, "%s(): malloc() failed\n", KM_ROUTINE_NAME );

		status = NO_MEMORY;
		goto KsMultiIo_Exit;
	}

	sprintf(dev_name, "GK%c%d", Key.scsi_port, Key.scsi_address);
    if( (scsiDevice = get_scsi_device_number( dev_name )) < 0 ) {
		if( MSGLVL(IMPORTANT) )
			fprintf( stderr, "%s(): error -- no scsi device found for '%s'\n", KM_ROUTINE_NAME, dev_name );

		status = NO_DEVICE;
		goto KsMultiIo_Exit;
	}

	if( MSGLVL(DETAILS) )
		printf( "%s(): device '%s' = '/dev/sg%d'\n", KM_ROUTINE_NAME, dev_name, scsiDevice );

	memset(Command, 0, COMMAND_SIZE(OpCodeBlockCAMAC));

	xfer_len.l = Count ? Count * ((Mem == 16) ? 2 : 4) : 0;
	Command[0] = OpCodeBlockCAMAC;
	Command[2] = Key.crate;
	Command[3] = (Enhanced << 6) | (!Enhanced << 5) | MODE(dmode, Mem);
	Command[4] = NAFhi(Key.slot, A, F);
	Command[5] = NAFlo(Key.slot, A, F);
	Command[6] = xfer_len.b[HI];
	Command[7] = xfer_len.b[MID];
	Command[8] = xfer_len.b[LO];

	scsi_descr.command     = &Command[0];
	scsi_descr.command_len = COMMAND_SIZE(OpCodeBlockCAMAC);
	scsi_descr.direction   = (F < 16) ? FROM_CAMAC : TO_CAMAC;
	scsi_descr.data        = (F < 7 || F > 15) ? Data : 0;
	scsi_descr.data_len    = scsi_descr.data ? xfer_len.l : 0;
	scsi_descr.sense       = (BYTE *)&user->sense;

	user->iosb             = iosb;
	user->scsi_descr       = scsi_descr;
	user->key              = Key;			// need to pass this to ScsiIo() to pass to KsTranslateIosb() [2001.09.07]
	
	// talk to the physical device
	status = ScsiIo( scsiDevice, user, KsTranslateIosb );

	if( MSGLVL(DETAILS) )
		printf( "%s(): ScsiIo() returned %d\n", KM_ROUTINE_NAME, status );

	iosb->status  = status;
	iosb->bytcnt  = (Data) ? xfer_len.l : 0;
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

KsMultiIo_Exit:
	if( MSGLVL(DETAILS) ) {
		printf( "%s(): iosb->status [0x%x]\n", KM_ROUTINE_NAME, iosb->status );
		printf( "%s(): iosb->x      [0x%x]\n", KM_ROUTINE_NAME, iosb->x      );
		printf( "%s(): iosb->q      [0x%x]\n", KM_ROUTINE_NAME, iosb->q      );
	}

	if( user )
		free(user);

	return status;
}
