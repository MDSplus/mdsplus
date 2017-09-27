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
// contract_db.c
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

//-------------------------------------------------------------------------
// contract_db()
// Wed Jan 30 10:44:01 EST 2002
//-------------------------------------------------------------------------
// Contract the CTS or CRATE db files
//
// input:       db type,
//                      current number of entries in CTS data file
// output:      status
//-------------------------------------------------------------------------
int contract_db(int dbType, int numOfEntries)
{
  char *FileName;
  int FileIncr, newCount;
  int status = SUCCESS;		// optimistic, aren't we ... :>
  char tmpfile[1024];
  strcpy(tmpfile, get_file_name("mdscts_temp_file_XXXXXX"));

  if (MSGLVL(FUNCTION_NAME))
    printf("contract_db()\n");

  // assimilate db specific information ...
  switch (dbType) {
  case CTS_DB:
    FileName = CTS_DB_FILE;
    FileIncr = CTS_DB_INCREMENT;
    break;

  case CRATE_DB:
    FileName = CRATE_DB_FILE;
    FileIncr = CRATE_DB_INCREMENT;
    break;
  }

  // make db file smaller ....  [2002.01.30]
  //
  //
  // calculate new db file size
  newCount = (((int)numOfEntries / FileIncr) + 1) * FileIncr;

  // create a TMP file
  if ((status = create_tmp_file(dbType, newCount, tmpfile)) != SUCCESS) {
    if (MSGLVL(ALWAYS))
      fprintf(stderr, "error creating TMP file\n");

    goto ContractDB_Exit;
  }
  // only need to copy old data if there is any
  if (numOfEntries) {		// copy current db file to TMP file
    if ((status = copy(dbType, FileName, tmpfile, numOfEntries)) != SUCCESS) {
      if (MSGLVL(ALWAYS))
	fprintf(stderr, "error copying db to TMP file\n");

      goto ContractDB_Exit;
    }
    // remove old db file
    if (Remove(FileName)) {	// non-zero is an error
      if (MSGLVL(ALWAYS)) {
	fprintf(stderr, "error removing old db file\n");
	perror("remove()");
      }

      status = CONTRACT_ERROR;
      goto ContractDB_Exit;
    }
  }

  if (rename(tmpfile, get_file_name(FileName))) {	// non-zero is an error
    if (MSGLVL(ALWAYS)) {
      fprintf(stderr, "error renaming temp db file\n");
      perror("rename()");
    }

    status = CONTRACT_ERROR;
    goto ContractDB_Exit;
  }
  chmod(get_file_name(FileName), O666);
  // re-map file
  if (map_data_file(dbType) != SUCCESS) {
    if (MSGLVL(ALWAYS))
      fprintf(stderr, "unable to map contracted file\n");

    status = MAP_ERROR;
    goto ContractDB_Exit;
  }

 ContractDB_Exit:
  if (MSGLVL(DETAILS)) {
    printf("contract_db('%s'): ", get_file_name(FileName));
    ShowStatus(status);
  }

  return status;
}
