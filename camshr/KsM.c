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
// KsMultiIo.c
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
static int KsMultiIo(CamKey Key,	// module info
		     BYTE A,	// module sub-address
		     BYTE F,	// module function
		     int Count,	// data count in bytes
		     BYTE * Data,	// data
		     BYTE Mem,	// 16 or 24 bit data
		     TranslatedIosb * iosb,	// status struct
		     int dmode,	// mode
		     int Enhanced	// enhanced
    )
{
  char dev_name[12];
  BYTE Command[COMMAND_SIZE(OpCodeBlockCAMAC)];
  int scsiDevice, status;

  union {
    long l;
    BYTE b[4];
  } xfer_len;

  int direction;
  RequestSenseData sense;
  unsigned char sb_out_len;
  int transfer_len;
  int enhanced;
  int online;
  int dummy;
  int wc;
  if (MSGLVL(FUNCTION_NAME))
    printf("%s()\n", KM_ROUTINE_NAME);

  // sprintf(dev_name, "GK%c%d", Key.scsi_port, Key.scsi_address); 
  sprintf(dev_name, "GK%c%2d%2.2d", Key.scsi_port, Key.scsi_address, Key.crate);

  if ((scsiDevice = get_scsi_device_number(dev_name, &enhanced, &online)) < 0) {
    if (MSGLVL(IMPORTANT))
      fprintf(stderr, "%s(): error -- no scsi device found for '%s'\n", KM_ROUTINE_NAME, dev_name);

    status = NO_DEVICE;
    goto KsMultiIo_Exit;
  }

  if (!online && (Key.slot != 30))
    return CamOFFLINE;

  if (!Enhanced)
    enhanced = 0;
  if (MSGLVL(DETAILS))
    printf("%s(): device '%s' = '/dev/sg%d'\n", KM_ROUTINE_NAME, dev_name, scsiDevice);

  xfer_len.l = Count ? Count * ((Mem == 16) ? 2 : 4) : 0;
  memset(Command, 0, sizeof(Command));
  Command[0] = OpCodeBlockCAMAC;
  Command[1] = 0;
  Command[2] = Key.crate;
  Command[3] = (enhanced << 6) | (!enhanced << 5) | MODE(dmode, Mem);
  Command[4] = NAFhi(Key.slot, A, F);
  Command[5] = NAFlo(Key.slot, A, F);
  Command[6] = xfer_len.b[HI];
  Command[7] = xfer_len.b[MID];
  Command[8] = xfer_len.b[LO];

  direction = ((F & 0x08) == 0) ? ((F < 8) ? 1 : 2) : 0;

  // talk to the physical device
  scsi_lock(scsiDevice, 1);
  status = scsi_io(scsiDevice, direction, Command, sizeof(Command),
		   (char *)Data, xfer_len.l, (unsigned char *)&sense, sizeof(sense), &sb_out_len, &transfer_len);

#ifdef DEBUG
  if (Verbose) {
    struct {
      char c2;
      char c3;
      char nb;
      char *name;
    } reg[] = { {
    0x0, 0x00, 1, "Transfer Count Low"}, {
    0x0, 0x02, 1, "Transfer Count Middle"}, {
    0x0, 0x04, 1, "FIFO"}, {
    0x0, 0x06, 1, "Command"}, {
    0x0, 0x08, 1, "Status/Select"}, {
    0x0, 0x0A, 1, "Interrupt"}, {
    0x0, 0x0C, 1, "Sequence Step /Synchonous Transfer Period"}, {
    0x0, 0x0E, 1, "FIFO Flags /Synchronous Transfer Offset"}, {
    0x0, 0x10, 1, "Configuration #1"}, {
    0x0, 0x12, 1, "Clock Conversion"}, {
    0x0, 0x14, 1, "Test"}, {
    0x0, 0x16, 1, "Configuration #2"}, {
    0x0, 0x18, 1, "Configuration #3"}, {
    0x0, 0x1A, 1, "Transfer Count High"}, {
    0x0, 0x1C, 1, "FIFO Bottom"}, {
    0x0, 0x80, 2, "Timer Control/SCSI ID/Strap Selections"}, {
    0x0, 0x82, 2, "Control/Status"}, {
    0x0, 0x86, 4, "Buffer Interval Counter #1"}, {
    0x0, 0x8A, 2, "Buffer end Address/Counter #2"}, {
    0x1, 0x84, 4, "Command Memory Data"}, {
    0x1, 0x88, 4, "CAMAC Word Count"}, {
    0x2, 0x00, 2, "Command Memory Address"}, {
    0x2, 0x02, 2, "Demand Message"}};
    int i;
    for (i = 0; i < (sizeof(reg) / sizeof(reg[0])); i++) {
      unsigned int l;
      unsigned short s;
      unsigned char c;
      char *dptr;
      switch (reg[i].nb) {
      case 1:
	dptr = (char *)&c;
	break;
      case 2:
	dptr = (char *)&s;
	break;
      case 4:
	dptr = (char *)&l;
	break;
      }
      Command[0] = OpCodeRegisterAccess;
      Command[1] = 0;
      Command[2] = reg[i].c2;
      Command[3] = reg[i].c3;
      Command[4] = 1;
      Command[5] = 0;
      status = scsi_io(scsiDevice, 1, Command, 6, dptr, reg[i].nb, 0, 0, &sb_out_len, &dummy);
      switch (reg[i].nb) {
      case 1:
	printf("%s = %d,%x\n", reg[i].name, c, c);
	break;
      case 2:
	printf("%s = %d,%x\n", reg[i].name, s, s);
	break;
      case 4:
	printf("%s = %d,%x\n", reg[i].name, l, l);
	break;
      }
    }
  }
#endif
  Command[0] = OpCodeRegisterAccess;
  Command[1] = 0;
  Command[2] = 0x01;
  Command[3] = 0x80;
  Command[4] = 1;
  Command[5] = 0;
  status = scsi_io(scsiDevice, 1, Command, 6,
		   (char *)&sense.u2.esr, sizeof(sense.u2.esr), 0, 0, &sb_out_len, &dummy);
  Command[3] = 0x88;
  Command[4] = 1;
  Command[5] = 0;
  status = scsi_io(scsiDevice, 1, Command, 6, (char *)&wc, sizeof(wc), 0, 0, &sb_out_len, &dummy);
  scsi_lock(scsiDevice, 0);
  transfer_len = (wc < 0) ? xfer_len.l + (wc - ((Mem == 16) ? 1 : 2)) * 2 : xfer_len.l;
  LastIosb.bytcnt = (unsigned short)(transfer_len & 0xFFFF);
  LastIosb.lbytcnt = (unsigned short)(transfer_len >> 16);
  status = KsTranslateIosb(&sense, status);
  if (iosb)
    *iosb = LastIosb;

  if (MSGLVL(DETAILS))
    printf("%s(): ScsiIo() returned %d\n", KM_ROUTINE_NAME, status);

 KsMultiIo_Exit:
  if (MSGLVL(DETAILS)) {
    printf("%s(): iosb->status [0x%x]\n", KM_ROUTINE_NAME, iosb->status);
    printf("%s(): iosb->x      [0x%x]\n", KM_ROUTINE_NAME, iosb->x);
    printf("%s(): iosb->q      [0x%x]\n", KM_ROUTINE_NAME, iosb->q);
  }

  return status;
}
