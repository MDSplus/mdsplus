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
// copy.c
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
// copy()
// Thu Jan 11 13:38:53 EST 2001
// Wed Mar  7 16:25:22 EST 2001
// Wed Apr 18 11:20:48 EDT 2001 -- use macro support, ie 'Open()'
//-------------------------------------------------------------------------
// Copy in_file's contents to out_file
//
// input:       db type, 
//                      pointers to inFile, outFile, 
//                      number of items to copy
// output:      status
//-------------------------------------------------------------------------
int copy(int dbType, char *inFile, char *outFile, int count)
{
  int Read_fd, Write_fd;
  int entrySize, i;
  int status = SUCCESS;		// optimistic

  if (MSGLVL(FUNCTION_NAME))
    printf("copy()\n");

  // get a read file descriptor
  if ((Read_fd = Open(inFile, O_RDONLY)) == -1) {
    if (MSGLVL(ALWAYS))
      perror("open(rd)");

    status = FILE_ERROR;
    goto Copy_Exit_1;
  }
  // get a write file descriptor
  if ((Write_fd = open(outFile, O_WRONLY)) == -1) {
    printf("Error opening %s\n", outFile);
    perror("open(wr)");

    status = FILE_ERROR;
    goto Copy_Exit_2;
  }
  // figure out size
  switch (dbType) {
  case CTS_DB:
    entrySize = MODULE_ENTRY;
    break;

  case CRATE_DB:
    entrySize = CRATE_ENTRY;
    break;
  default:
    return 0;
  }

  // local block
  {
    char line[entrySize];

    // read 'n write ...
    for (i = 0; i < count; ++i) {
      // read ...
      if (read(Read_fd, line, entrySize) != entrySize) {
	if (MSGLVL(ALWAYS))
	  perror("read()");

	status = COPY_ERROR;
	goto Copy_Exit_3;
      }
      // ... then write
      if (write(Write_fd, line, entrySize) != entrySize) {
	if (MSGLVL(ALWAYS))
	  perror("write()");

	status = COPY_ERROR;
	goto Copy_Exit_3;
      }
    }
  }

 Copy_Exit_3:
  if (Write_fd != -1)
    close(Write_fd);

 Copy_Exit_2:
  if (Read_fd != -1)
    close(Read_fd);

 Copy_Exit_1:
  if (MSGLVL(DETAILS)) {
    printf("copy(): ");
    ShowStatus(status);
  }

  return status;
}
