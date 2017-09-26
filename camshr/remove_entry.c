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
// remove_entry.c
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
#include "cts_p.h"

//-------------------------------------------------------------------------
// remove_entry()
//-------------------------------------------------------------------------
// Thu Jan 11 11:27:09 EST 2001
// Fri Feb 22 10:43:30 EST 2002 -- fixed end of new blank line problem
//-------------------------------------------------------------------------
// Removes an entry from a db file. Entry removed is referenced by
//      its positional index in file.  File locking (ie critial section) is 
//      provided by use of a semaphore. Upon removal, the db in memory is 
//      updated to the actual disk file.
//
// input:       db type,
//                      index of entry in db file, zero-based
// outout:      status
//-------------------------------------------------------------------------
int remove_entry(int dbType, int index)
{
  void *dbptr;			// re-usable, generic db pointer
  char fmt[10];
  int entrySize, i, numOfEntries;
  int status = SUCCESS;
  extern struct MODULE *CTSdb;	// pointer to in-memory copy of data file
  extern struct CRATE *CRATEdb;	// pointer to in-memory copy of data file

  if (MSGLVL(FUNCTION_NAME))
    printf("remove_entry(#:%d)\n", index + 1);

//----------------------------
//      removing an entry 
//----------------------------
//-- 'critical section' start
//----------------------------
  // 'lock' with semaphore
  if (lock_file() == ERROR) {
    if (MSGLVL(ALWAYS))
      fprintf(stderr, "error locking file\n");

    status = LOCK_ERROR;
    goto RemoveEntry_Exit;
  }
  // get number of current entries
  if ((numOfEntries = get_file_count(dbType)) == 0) {	// no entries in cts db file
    if (MSGLVL(IMPORTANT))
      fprintf(stderr, "db file empty, no entries to remove\n");

    status = DEASSIGN_ERROR;
    goto RemoveEntry_Exit;
  }
  // cull db specific info
  switch (dbType) {
  case CTS_DB:
    dbptr = (void *)CTSdb;
    entrySize = MODULE_ENTRY;
    break;

  case CRATE_DB:
    dbptr = (void *)CRATEdb;
    entrySize = CRATE_ENTRY;
    break;
  default:
    unlock_file();
    return 0;
  }

  // set up 'blank-entry' format string
  sprintf(fmt, "%%-%ds\n", entrySize - 1);

  // local block
  {
    char line[entrySize + 1];

    // remove entry -- move appropriate entries one earlier in list
    for (i = index; i < numOfEntries; ++i)
      memcpy((char *)dbptr + (i * entrySize), (char *)dbptr + ((i + 1) * entrySize), entrySize);

    // 'blank' out last, old entry
    sprintf(line, fmt, " ");
    memcpy((char *)dbptr + ((i + 1) * entrySize), line, entrySize);
  }

  // commit change to file
  if (commit_entry(dbType) != SUCCESS) {
    status = COMMIT_ERROR;
    goto RemoveEntry_Exit;
  }
  // release semaphore
  if (unlock_file() == ERROR) {
    if (MSGLVL(ALWAYS))
      fprintf(stderr, "error unlocking file\n");

    status = UNLOCK_ERROR;
    goto RemoveEntry_Exit;
  }
//----------------------------
//-- 'critical section' finish
//----------------------------
 RemoveEntry_Exit:
  if (MSGLVL(DETAILS)) {
    printf("remove_entry(): ");
    ShowStatus(status);
  }

  return status;
}
