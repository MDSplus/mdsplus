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
// KsTranslateIosb.c
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

#include "prototypes.h"

//-----------------------------------------------------------
int KsTranslateIosb(RequestSenseData * sense, int cam_status)
{
  int status;

  //  union {
  //    ErrorStatusRegister esr;
  //    int     l;
  //    BYTE    b[4];
  //  } u;

  if (Verbose) {
    printf("SCSI Sense data:  error code=%d,valid=%d,sense_key=%d\n\n", sense->error_code,
	   sense->valid, sense->sense_key);
    printf("     CSR status register:\n\n");
    printf
	("                  noq=%d,nox=%d,done=%d,lam_pending=%d,qrpt_timeout=%dabort=%d,xmt_fifo_empty=%d,xmt_fifo_full=%d,\n",
	 sense->u1.csr.noq, sense->u1.csr.nox, sense->u1.csr.done, sense->u1.csr.lam_pending,
	 sense->u1.csr.qrpt_timeout, sense->u1.csr.abort, sense->u1.csr.xmt_fifo_empty,
	 sense->u1.csr.xmt_fifo_full);
    printf("                  rcv_fifo_empty=%d,rcv_fifo_full=%d,high_byte_first=%d,scsi_id=%d\n\n",
	   sense->u1.csr.rcv_fifo_empty, sense->u1.csr.rcv_fifo_full, sense->u1.csr.high_byte_first,
	   sense->u1.csr.scsi_id);
    printf("     Error status register:\n\n");
    printf
	("                  noq=%d,nox=%d,ste=%d,adnr=%d,tpe=%d,lpe=%d,n_gt_23=%d,err=%d,no_sync=%d\n",
	 sense->u2.esr.noq, sense->u2.esr.nox, sense->u2.esr.ste, sense->u2.esr.adnr,
	 sense->u2.esr.tpe, sense->u2.esr.lpe, sense->u2.esr.n_gt_23, sense->u2.esr.err,
	 sense->u2.esr.no_sync);
    printf
	("                  tmo=%d,derr=%d,error_code=%d,no_halt=%d,read=%d,ad=%d,word_size=%d,qmd=%d,tm=%d,cm=%d\n\n",
	 sense->u2.esr.tmo, sense->u2.esr.derr, sense->u2.esr.error_code, sense->u2.esr.no_halt,
	 sense->u2.esr.read, sense->u2.esr.ad, sense->u2.esr.word_size, sense->u2.esr.qmd,
	 sense->u2.esr.tm, sense->u2.esr.cm);
    printf("     Additional registers:\n\n");
    printf("                  stat=%d,stacss=%d,stasum=%d,stacnt=%d\n\n",
	   sense->stat, sense->staccs, sense->stasum, sense->stacnt);
  }
  if (MSGLVL(FUNCTION_NAME))
    printf("%s()\n", KT_ROUTINE_NAME);

  status = CamSERTRAERR;
  LastIosb.status = Shorten(CamSERTRAERR);
  if (cam_status == 0 || cam_status == 1 || cam_status == 2) {
    switch (sense->u2.esr.error_code) {
    case 0:
      status = 1;
      LastIosb.status = Shorten(CamDONE_Q);
      if (MSGLVL(DETAILS))
	printf("CamDONE_Q\n");
      break;
    case 7:
      status = 1;
      LastIosb.status = Shorten(CamDONE_NOQ);
      if (MSGLVL(DETAILS))
	printf("CamDONE_NOQ\n");
      break;
    case 8:
      status = CamDONE_NOX;
      LastIosb.status = Shorten(CamDONE_NOX);
      if (MSGLVL(DETAILS))
	printf("CamDONE_NOX\n");
      break;
    case 12:
      status = CamSCCFAIL;
      LastIosb.status = Shorten(CamSCCFAIL);
      if (MSGLVL(DETAILS))
	printf("CamSCCFAIL\n");
      break;
    default:
      status = CamSERTRAERR;
      LastIosb.status = Shorten(CamSERTRAERR);
      if (MSGLVL(DETAILS))
	printf("CamSERTRAERR\n");
      break;
    }
    LastIosb.x = !sense->u2.esr.nox;
    LastIosb.q = !sense->u2.esr.noq;
    LastIosb.err = sense->u2.esr.err;
    LastIosb.lpe = sense->u2.esr.lpe;
    LastIosb.tpe = sense->u2.esr.tpe;
    LastIosb.no_sync = sense->u2.esr.no_sync;
    LastIosb.tmo = sense->u2.esr.tmo;
    LastIosb.adnr = sense->u2.esr.adnr;
    if (sense->u2.esr.adnr) {
      status = CamDONE_NOX;
      LastIosb.status = Shorten(CamDONE_NOX);
    }
    LastIosb.list = 0;		// list processing not supported
//      LastIosb.lbytcnt =  0;                  // list processing not supported
  }
  if (MSGLVL(DETAILS)) {
    printf("%s(): LastIosb.status = %d [0x%x]\n", KT_ROUTINE_NAME, LastIosb.status,
	   LastIosb.status);
    printf("%s(): LastIosb.x = %d\n", KT_ROUTINE_NAME, LastIosb.x);
    printf("%s(): LastIosb.q = %d\n", KT_ROUTINE_NAME, LastIosb.q);
  }

  return status;
}
