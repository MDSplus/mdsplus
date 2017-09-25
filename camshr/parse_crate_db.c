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
// parse_crate_db.c
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

//-------------------------------------------------------------------------
// include files
//-------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <errno.h>

#include "common.h"
#include "prototypes.h"
#include "crate.h"

//-------------------------------------------------------------------------
// parse_crate_db()
//-------------------------------------------------------------------------
// Fri Jan 12 09:55:17 EST 2001
// Wed Mar  7 14:31:29 EST 2001 -- fixed conversion of DSFname to device
// Thu Mar 15 12:00:33 EST 2001 -- changed format of crate.db
// Tue Mar 20 12:26:15 EST 2001 -- return -1 for non assigned /dev/sg#
// Mon Apr 30 13:15:27 EDT 2001 -- added highway type member
//-------------------------------------------------------------------------
// Parse a crate database entry from memory mapped file
//
// input:       pointer to crate.db data (in memory), and
//                      pointer to struct of crate data
// output:      none
//-------------------------------------------------------------------------
void parse_crate_db(struct CRATE *in, struct Crate_ *out)
{
  if (MSGLVL(FUNCTION_NAME))
    printf("parse_crate_db()\n");

  // build full physical crate name, eg. 'GKB509'
  sprintf(out->name, "%.2s%c%c%.2s", in->Phys_Name.prefix,	// 'GK'
	  in->Phys_Name.Adapter,	// SCSI host adapter
	  in->Phys_Name.Id,	// SCSI id number
	  in->Phys_Name.Crate	// CAMAC crate number
      );

  out->device = (in->DSFname[0] != '.') ? atoi(in->DSFname)	// valid /dev/sg#
      : -1;			// in-valid

  out->type = in->HwyType;	// highway type
  out->enhanced = in->enhanced == '1';
  out->online = in->online == '1';
}
