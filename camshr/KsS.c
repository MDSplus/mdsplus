// KsSingleIo.c
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
// Mon Oct 15 16:35:42 EDT 2001 -- seperated out
//-----------------------------------------------------------

//-----------------------------------------------------------
static int KsSingleIo(CamKey Key, BYTE A, BYTE F, BYTE * Data, BYTE Mem, TranslatedIosb * iosb,	// to be returned to caller
		      int dmode)
{
  char dev_name[5];
  BYTE Command[COMMAND_SIZE(OpCodeSingleCAMAC)];
  int scsiDevice;
  int status = SUCCESS;		// optimistic -- function status, eg SUCCESS(=1) or FAILURE(=0)
  int direction;
  RequestSenseData sense;
  unsigned char sb_out_len;
  int transfer_len;
  int enhanced;
  int online;
  int dummy;

  if (MSGLVL(FUNCTION_NAME))
    printf("%s()\n", KS_ROUTINE_NAME);

  // find the scsi device number (ie '/dev/sg#')
  // sprintf(dev_name, "GK%c%d", Key.scsi_port, Key.scsi_address); 
  sprintf(dev_name, "GK%c%d%.2d", Key.scsi_port, Key.scsi_address, Key.crate);

  if ((scsiDevice = get_scsi_device_number(dev_name, &enhanced, &online)) < 0) {
    if (MSGLVL(IMPORTANT))
      fprintf(stderr, "%s(): error -- no scsi device found for '%s'\n", KS_ROUTINE_NAME, dev_name);

    status = NO_DEVICE;
    goto KsSingleIo_Exit;
  }
  if (!online && (Key.slot != 30))
    return CamOFFLINE;
  if (MSGLVL(DETAILS))
    printf("%s(): device '%s' = '/dev/sg%d'\n", KS_ROUTINE_NAME, dev_name, scsiDevice);

  // prepare CAMAC command
  Command[0] = OpCodeSingleCAMAC;
  Command[1] = 0;
  Command[2] = Key.crate;
  Command[3] = MODE(dmode, Mem);
  Command[4] = NAFhi(Key.slot, A, F);
  Command[5] = NAFlo(Key.slot, A, F);
  Command[6] = 0;
  Command[7] = 0;
  Command[8] = 0;
  Command[9] = 0;

  direction = ((F & 0x08) == 0) ? ((F < 8) ? 1 : 2) : 0;

  // talk to the physical device
  scsi_lock(scsiDevice, 1);
  status = scsi_io(scsiDevice, direction, Command, sizeof(Command),
		   (char *)Data, direction ? ((Mem == 16) ? 2 : 4) : 0,
		   (unsigned char *)&sense, sizeof(sense), &sb_out_len, &transfer_len);
  Command[0] = OpCodeRegisterAccess;
  Command[1] = 0;
  Command[2] = 0x01;
  Command[3] = 0x80;
  Command[4] = 1;
  Command[5] = 0;
  status = scsi_io(scsiDevice, 1, Command, 6,
		   (char *)&sense.u2.esr, sizeof(sense.u2.esr), 0, 0, &sb_out_len, &dummy);
  scsi_lock(scsiDevice, 0);
  LastIosb.bytcnt = (unsigned short)(transfer_len & 0xFFFF);
  status = KsTranslateIosb(&sense, status);
  if (iosb)
    *iosb = LastIosb;

  if (MSGLVL(DETAILS))
    printf("%s(): ScsiIo() returned %d\n", KS_ROUTINE_NAME, status);

 KsSingleIo_Exit:
  if (MSGLVL(DETAILS)) {
    printf("%s(): iosb->status [0x%x]\n", KS_ROUTINE_NAME, iosb->status);
    printf("%s(): iosb->bytcnt [%d]\n", KS_ROUTINE_NAME, iosb->bytcnt);
    printf("%s(): iosb->x      [0x%x]\n", KS_ROUTINE_NAME, iosb->x);
    printf("%s(): iosb->q      [0x%x]\n", KS_ROUTINE_NAME, iosb->q);
  }
  return status;
}
