// unlock_file.c
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
// unlock_file()
//-------------------------------------------------------------------------
// Thu Jan 11 13:52:17 EST 2001
//-------------------------------------------------------------------------
// Remove file lock provided by a sempahore.
//
// input:       none
// output:      status
//-------------------------------------------------------------------------
int unlock_file()
{
  int status = SUCCESS;		// assume the best case
  struct sembuf sb = { 0, P_SEMA4, 0 };
  extern int semid;

  if (MSGLVL(FUNCTION_NAME))
    printf("unlock_file()\n");

  sb.sem_op = V_SEMA4;		// prepare to 'vend' a semaphore
  if (semop(semid, &sb, 1) == ERROR) {
    if (MSGLVL(ALWAYS))
      perror("semop()");

    status = ERROR;
  }

  if (MSGLVL(DETAILS)) {
    printf("unlock_file(): ");
    ShowStatus(status);
  }

  return status;
}
