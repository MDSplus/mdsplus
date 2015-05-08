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
