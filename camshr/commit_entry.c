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
// commit_entry.c
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
#include "module.h"
#include "crate.h"
#include "prototypes.h"

//-------------------------------------------------------------------------
// commit_entry()
// Thu Jan 11 13:28:35 EST 2001
//-------------------------------------------------------------------------
// Update memory mapped file with new data
//
// input:       db file type
// output:      status
//-------------------------------------------------------------------------
int commit_entry(int dbType)
{
  void *dbptr;			// generic pointer to dbs
  int status = SUCCESS;		// assume the best
  extern struct MODULE *CTSdb;	// pointer to in-memory copy of data file
  extern struct CRATE *CRATEdb;	//   "     "     "       "   "   "    "

  if (MSGLVL(FUNCTION_NAME))
    printf("commit_entry()\n");

  switch (dbType) {
  case CTS_DB:
    dbptr = (void *)CTSdb;
    break;

  case CRATE_DB:
    dbptr = (void *)CRATEdb;
    break;
  default:
    return 0;
  }

  // commit changes to file
  //      NB! a len of 0 flushes all memory pages mapped
  if (msync(dbptr, 0, MS_SYNC | MS_INVALIDATE) == ERROR)
    status = COMMIT_ERROR;

  if (MSGLVL(DETAILS)) {
    printf("commit_entry(): ");
    ShowStatus(status);
  }

  return status;
}
