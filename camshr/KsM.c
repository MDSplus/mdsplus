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
  
  union {
    long	l;
    BYTE	b[4];
  } xfer_len;

  int direction;
  RequestSenseData sense;
  unsigned char sb_out_len;
  unsigned int transfer_len;
  
  if( MSGLVL(FUNCTION_NAME) )
    printf( "%s()\n", KM_ROUTINE_NAME );
  
  sprintf(dev_name, "GK%c%d", Key.scsi_port, Key.scsi_address);
  if( (scsiDevice = get_scsi_device_number( dev_name )) < 0 ) {
    if( MSGLVL(IMPORTANT) )
      fprintf( stderr, "%s(): error -- no scsi device found for '%s'\n", KM_ROUTINE_NAME, dev_name );
    
    status = NO_DEVICE;
    goto KsMultiIo_Exit;
  }

  if( MSGLVL(DETAILS) )
    printf( "%s(): device '%s' = '/dev/sg%d'\n", KM_ROUTINE_NAME, dev_name, scsiDevice );
  
  xfer_len.l = Count ? Count * ((Mem == 16) ? 2 : 4) : 0;
  Command[0] = OpCodeBlockCAMAC;
  Command[1] = 0;
  Command[2] = Key.crate;
  Command[3] = (Enhanced << 6) | (!Enhanced << 5) | MODE(dmode, Mem);
  Command[4] = NAFhi(Key.slot, A, F);
  Command[5] = NAFlo(Key.slot, A, F);
  Command[6] = xfer_len.b[HI];
  Command[7] = xfer_len.b[MID];
  Command[8] = xfer_len.b[LO];
  
  direction = ((F & 0x08) == 0) ? 0 : ((F < 8) ? 1 : 2);
  
  // talk to the physical device
  status = scsi_io( scsiDevice, direction, Command, sizeof(Command),
                    Data, xfer_len.l, 
                    (char *)&sense, sizeof(sense), &sb_out_len,&transfer_len);
  LastIosb.bytcnt = (unsigned short)(transfer_len & 0xFFFF);
  status = KsTranslateIosb(&sense,status);
  if (iosb) *iosb=LastIosb;
  
  if( MSGLVL(DETAILS) )
    printf( "%s(): ScsiIo() returned %d\n", KM_ROUTINE_NAME, status );

KsMultiIo_Exit:
  if( MSGLVL(DETAILS) ) {
    printf( "%s(): iosb->status [0x%x]\n", KM_ROUTINE_NAME, iosb->status );
    printf( "%s(): iosb->x      [0x%x]\n", KM_ROUTINE_NAME, iosb->x      );
    printf( "%s(): iosb->q      [0x%x]\n", KM_ROUTINE_NAME, iosb->q      );
  }
  
  return status;
}
