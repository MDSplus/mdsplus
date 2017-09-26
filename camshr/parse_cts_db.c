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
// parse_cts_db.c
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
#include "module.h"

//-------------------------------------------------------------------------
// parse_cts_db()
//-------------------------------------------------------------------------
// Fri Jan  5 12:00:48 EST 2001
// Tue Mar 20 15:50:36 EST 2001 -- conversion fixup
// Wed Jan 16 10:29:30 EST 2002 --     "        "
// Fri Jan 18 11:09:18 EST 2002 -- fixed comment field
// Fri Jan 25 12:52:12 EST 2002 -- 'really' fixed it this time!
// Thu Feb  7 13:29:05 EST 2002 -- fix from 'module.h', ie comment field
//-------------------------------------------------------------------------
// Parse a database entry from memory mapped file
//
// input:       pointer to cts.db data (in memory)
// output:      pointer to struct of db data
//-------------------------------------------------------------------------
void parse_cts_db(struct MODULE *in, struct Module_ *out)
{
  char adpt, fmt[6], line[MODULE_ENTRY + 1];
  char comm[41];
  int i;

  if (MSGLVL(FUNCTION_NAME))
    printf("parse_cts_db()\n");

  sprintf(fmt, "%%.%ds", (int)(MODULE_ENTRY - 1));	// create format string
  memset(line, ' ', MODULE_ENTRY + 1);	// 2002.02.06
  sprintf(line, fmt, (char *)in);	// extract first (single) line

  // parse ...
  sscanf(line, "%32s GK%c%1d%02d:N%2d %40c", out->name, &adpt,	// temporary
	 &out->id, &out->crate, &out->slot, comm	// temporary
      );
  out->adapter = adpt - 'A';	// adjustment ...

  // find end of comment
  for (i = sizeof(comm) - 1; i >= 0; --i) {
    if (comm[i] != ' ')
      break;
  }
  comm[i + 1] = '\0';		// 'early' termination
  strncpy(out->comment, comm, i);	// 'copy' it over       [2002.02.07]
}
