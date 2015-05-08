// check_sema4.c
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
// check_sema4()
// Thu Jan 11 13:34:22 EST 2001
//-------------------------------------------------------------------------
// Check to see if we can get the semaphore
//
// input:       none
// output:      semaphore count
//      NB! count should be zero for success (ie binary semaphore)
//-------------------------------------------------------------------------
int check_sema4()
{
  int count;
  union semun u;

  extern int semid;

  if (MSGLVL(FUNCTION_NAME))
    printf("check_sema4()\n");

  // check to see if sema4 is in use
  count = semctl(semid, 0, GETVAL, u);

  return count;
}
