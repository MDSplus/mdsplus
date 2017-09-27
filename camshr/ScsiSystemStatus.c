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
// ScsiSystemStatus.c
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
// Thu Sep  6 12:23:10 EDT 2001 -- initial release
// Tue Oct 23 11:43:03 EDT 2001 -- streamlined
//-------------------------------------------------------------------------

//-----------------------------------------------------------
// include files
//-----------------------------------------------------------
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "prototypes.h"

//-----------------------------------------------------------
// local static info
//-----------------------------------------------------------
static char *scsimsg = "Attached devices: none";

int ScsiSystemStatus(void)
{
  char line[80], *pline;
  int scsiSystemStatus = 0;	// assume the worst :(
  FILE *fp, *fopen();

  if (MSGLVL(FUNCTION_NAME))
    printf("ScsiSystemStatus()\n");

  if ((fp = fopen(PROC_FILE, "r")) == NULL) {
    fprintf(stderr, "can't open '%s' for read\n", PROC_FILE);
    scsiSystemStatus = 0;
    goto ScsiSystemStatus_Exit;
  }

  pline = &line[0];
  if ((pline = fgets(line, sizeof(line), fp)) != NULL) {
    if (strncmp(pline, scsimsg, strlen(scsimsg)))
      scsiSystemStatus = 1;	// something is attached
  }

 ScsiSystemStatus_Exit:
  if (fp)			// still open
    fclose(fp);

  return scsiSystemStatus;
}
