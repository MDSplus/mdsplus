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
// map_data_file.c
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
// map_data_file()
// Thu Jan  4 16:56:59 EST 2001
// Tue Apr  3 16:43:39 EDT 2001
// Wed Apr 18 11:23:26 EDT 2001 -- use macro support, ie 'Open()'
// Tue Sep 18 15:29:10 EDT 2001 -- added some comments
//-------------------------------------------------------------------------
// Memory map database file. This allows very fast data access to the
//      data bases. It also allows for data sharing. This function is called
//      because a db file was *NOT* memory mapped.
//
// input:       db type
// output:      status
//-------------------------------------------------------------------------
int map_data_file(int dbType)
{
  char *FileName=0;
  int db_size, fd=ERROR, *FileIsMapped = FALSE;
  int status = SUCCESS;
  extern int CTSdbFileIsMapped;
  extern int CRATEdbFileIsMapped;
  extern struct MODULE *CTSdb;	// pointer to in-memory copy of data file
  extern struct CRATE *CRATEdb;

  // set db specific parameters
  switch (dbType) {
  case CTS_DB:
    FileName = CTS_DB_FILE;
    FileIsMapped = &CTSdbFileIsMapped;
    break;

  case CRATE_DB:
    FileName = CRATE_DB_FILE;
    FileIsMapped = &CRATEdbFileIsMapped;
    break;
  }

  if (MSGLVL(FUNCTION_NAME))
    printf("map_data_file('%s')\n", FileName);

  // check to see if db file exists
  if (check_for_file(FileName) != SUCCESS) {
    status = FILE_ERROR;
    goto MapData_Exit;		// no file, so we're out'a here
  }
  // get file size
  if ((db_size = get_db_file_size(FileName)) == ERROR) {
    status = ERROR;
    goto MapData_Exit;
  }
  // get a file descriptor -- NB! special version of 'Open()'
  if ((fd = Open(FileName, O_RDWR)) == ERROR) {
    perror("open()");
    status = FILE_ERROR;	// error flag
    goto MapData_Exit;
  }
  // now, memory map database file
  switch (dbType) {
  case CTS_DB:
    if ((CTSdb =
	 (struct MODULE *)mmap((caddr_t) 0, db_size, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED,
			       fd, 0)) == MAP_FAILED) {
      if (MSGLVL(ALWAYS))
	perror("mmap(CTS)");

      status = MAP_ERROR;	// error flag -- could not map file
      *FileIsMapped = FALSE;
      goto MapData_Exit;
    }

    break;

  case CRATE_DB:
    if ((CRATEdb =
	 (struct CRATE *)mmap((caddr_t) 0, db_size, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED,
			      fd, 0)) == MAP_FAILED) {
      if (MSGLVL(ALWAYS))
	perror("mmap(CRATE)");

      status = MAP_ERROR;	// error flag -- could not map file
      *FileIsMapped = FALSE;
      goto MapData_Exit;
    }
    break;

  default:
    return 0;

    break;
  }

  // if we get this far, all is OK
  status = SUCCESS;		// success !!!
  *FileIsMapped = TRUE;

 MapData_Exit:
  if (fd != ERROR)		// still open ...
    close(fd);			// ... finished with file descriptor

  if (FileName == NULL)
    return status;
  if (status != SUCCESS)
    printf("Error mapping %s\n", FileName);
  if (MSGLVL(DETAILS)) {
    printf("map_data_file('%s'): ", FileName);
    ShowStatus(status);
  }

  return status;		// return results
}
