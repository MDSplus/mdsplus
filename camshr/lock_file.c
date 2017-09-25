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
// lock_file.c
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
// lock_file()
//-------------------------------------------------------------------------
// 'lock' with semaphore -- may take more than one try, hence 'do-while' loop
// NB! see note about the 'dreaded' GOTO statement
//
// input:       none
// output:      status
//-------------------------------------------------------------------------
int lock_file()
{
  int status = SUCCESS;		// assume the best case
  struct sembuf sb = { 0, P_SEMA4, 0 };
  extern int semid;		// see 'create_sema4()'
  extern int sema4Exists;	// see 'create_sema4()'

  if (MSGLVL(FUNCTION_NAME))
    printf("lock_file()\n");

  sb.sem_op = P_SEMA4;		// prepare to 'purchase' a semaphore

  // check for semaphore existance
  //      NB! failure to create a sema4 is a fatal error
  if (sema4Exists == FALSE)
    create_sema4();

  do {
    if (semop(semid, &sb, 1) == ERROR) {
      if (MSGLVL(ALWAYS))
	perror("semop()");

      status = ERROR;

      goto LockFile_Exit;	// 'early' exit due to error
      // NOTE: 'goto' is used in kernel code for appropriate things;
      // it is believed that this is an appropriate place to use one
      // for 'exception handling'
    }
  } while (check_sema4());

 LockFile_Exit:
  if (MSGLVL(DETAILS)) {
    printf("lock_file(): ");
    ShowStatus(status);
  }

  return status;
}
