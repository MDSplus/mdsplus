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
// create_sema4.c
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

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
// union semun is defined by including <sys/sem.h>
#else
// according to X/OPEN we have to define it ourselves
union semun {
  int val;			// value for SETVAL
  struct semid_ds *buf;		// buffer for IPC_STAT, IPC_SET
  unsigned short int *array;	// array for GET_ALL, SET_ALL
  struct seminfo *__buf;	// buffer for IPC_INFO
};
#endif

//-------------------------------------------------------------------------
// global stuff -- for all library routines to be able to access
//-------------------------------------------------------------------------
int semid;
int sema4Exists = FALSE;	// initially doesn't exist

//-------------------------------------------------------------------------
// create_sema4()
// Mon Apr  9 15:04:34 EDT 2001 -- changed file name for 'ftok()' call
//-------------------------------------------------------------------------
// Create a semaphore for use in file locking
//
// input:       none
// output:      SUCCESS, otherwise fatal error
//-------------------------------------------------------------------------
int create_sema4()
{
  key_t key;
  union semun arg;

  sema4Exists = FALSE;		//  at least at this point

  if (MSGLVL(FUNCTION_NAME))
    printf("create_sema4()\n");

  // obtain a key
  if ((key = ftok(SEMAPHORE_KEY_SEED, 'A')) == ERROR) {
    perror("ftok(create_sema4)");
    exit(-1);			// fatal error !!!
  }
  // get a semaphore id
  if ((semid = semget(key, 1, 0666 | IPC_CREAT)) == ERROR) {
    perror("semget(create_sema4)");
    exit(-2);			// fatal error !!!
  }
  // create semaphore and initialize it
  arg.val = 1;
  if (semctl(semid, 0, SETVAL, arg) == ERROR) {
    perror("semctl(create_asema4)");
    exit(-3);			// fatal error !!!
  }

  sema4Exists = TRUE;		// if we get here, all's OK

  return SUCCESS;
}
