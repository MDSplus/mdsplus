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
// get_file_count.c
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
// get_file_count()
// Thu Jan  4 12:06:05 EST 2001
// Wed Apr 18 11:22:47 EDT 2001 -- use macro support, ie 'Open()'
// Thu Sep  6 13:43:00 EDT 2001 -- fixed debug printout
// Wed Sep 19 12:19:00 EDT 2001 -- switched to memory mapped file use; faster
// Tue Jan  8 17:16:18 EST 2002 -- fixed end of dbfile problem
//-------------------------------------------------------------------------
// Get number of entries in database file. Unused records have a leading
//      space character. Uses memory mapped 'version' of db file.
//
// input:       db type
// output:      number of entries [0..n]; FILE_ERROR [-2 * 2] if error
//-------------------------------------------------------------------------
int get_file_count(int dbType)
{
  void *dbptr;			// generic pointer to struct's
  char dbFileName[16];
  int dbFileSize, entrySize, i, numOfEntries;
  int *FileIsMapped;
  extern struct MODULE *CTSdb;
  extern struct CRATE *CRATEdb;
  extern int CTSdbFileIsMapped;
  extern int CRATEdbFileIsMapped;

  switch (dbType) {
  case CTS_DB:
    dbptr = (void *)CTSdb;
    entrySize = MODULE_ENTRY;
    FileIsMapped = &CTSdbFileIsMapped;
    sprintf(dbFileName, "%s", CTS_DB_FILE);
    break;

  case CRATE_DB:
    dbptr = (void *)CRATEdb;
    entrySize = CRATE_ENTRY;
    FileIsMapped = &CRATEdbFileIsMapped;
    sprintf(dbFileName, "%s", CRATE_DB_FILE);
    break;

  default:
    return 0;
  }

  if (MSGLVL(FUNCTION_NAME))
    printf("get_file_count()\n");

  // check for memory mapped file
  if (*FileIsMapped == FALSE) {
    if (map_data_file(dbType) != SUCCESS) {
      numOfEntries = MAP_ERROR;
      goto GetFileCount_Exit;
    }
  }
  // get total db file size in bytes
  if ((dbFileSize = get_db_file_size(dbFileName)) < 0) {
    numOfEntries = FAILURE;
    goto GetFileCount_Exit;
  }
  // get the appropriate count
  numOfEntries = 0;
  for (i = 0;; i += entrySize) {
    if ((i + entrySize) > dbFileSize)	// make sure we don't fall off the end ...
      break;

    //              sprintf(&ch, "%.1s", (char *)(dbptr+i));

    if (*(char *)(dbptr + i) == ' ')	// we're done, so out'a here
      break;

    ++numOfEntries;
  }

 GetFileCount_Exit:
  if (MSGLVL(DETAILS))
    printf("get_file_count(%d):\n", numOfEntries);

  return numOfEntries;
}
