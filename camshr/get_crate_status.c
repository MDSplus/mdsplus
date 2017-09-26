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
// get_crate_status.c
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

//-----------------------------------------------------------
// include files
//-----------------------------------------------------------
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "crate.h"
#include "prototypes.h"

//-----------------------------------------------------------
// eg. *crate_name == "GKB509"
// Tue Apr  3 16:37:04 EDT 2001
// Fri Aug 24 15:59:54 EDT 2001 -- fixed powered off condition (???)
// Mon Aug 27 11:26:10 EDT 2001 -- re-fixed (?) powered off condition
//-----------------------------------------------------------
int get_crate_status(char *crate_name, int *ptr_crate_status)
{
  char controller[11];
  short SCCdata;
  int status = SUCCESS;		// optimistic ...
  TranslatedIosb iosb;

  if (MSGLVL(FUNCTION_NAME))
    printf("get_crate_status()\n");

  // create full crate controller designation
  // NB! all crate controllers reside in station(slot) 30
  sprintf(controller, "%.6s:N30", crate_name);

  // get crate status
  SCCdata = 0;

  status = CamPiow(controller,	// serial crate controller name
		   0,		// A    --\__ read status register
		   1,		// F    --/
		   &SCCdata,	// returned status
		   16,		// mem == 16-bit data
		   &iosb	// *iosb
      );

  *ptr_crate_status = (short)((status & 1) ? SCCdata : 0) & 0x0ffff;

  if (MSGLVL(DETAILS))
    printf("gcs(): %.6s  SCCdata = 0x%0x  CamPiow()status = 0x%0x  is %s-LINE\n", controller, SCCdata,	//*ptr_crate_status,
	   status, (*ptr_crate_status & 0x3c00 || *ptr_crate_status & 3) ? "off" : "ON");

  return SUCCESS;
}
