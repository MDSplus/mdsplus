// commit_entry.c
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
// commit_entry()
// Thu Jan 11 13:28:35 EST 2001
//-------------------------------------------------------------------------
// Update memory mapped file with new data
//
// input:       db file type
// output:      status
//-------------------------------------------------------------------------
int commit_entry(int dbType)
{
  void *dbptr;			// generic pointer to dbs
  int status = SUCCESS;		// assume the best
  extern struct MODULE *CTSdb;	// pointer to in-memory copy of data file
  extern struct CRATE *CRATEdb;	//   "     "     "       "   "   "    "

  if (MSGLVL(FUNCTION_NAME))
    printf("commit_entry()\n");

  switch (dbType) {
  case CTS_DB:
    dbptr = (void *)CTSdb;
    break;

  case CRATE_DB:
    dbptr = (void *)CRATEdb;
    break;
  default:
    return 0;
  }

  // commit changes to file
  //      NB! a len of 0 flushes all memory pages mapped
  if (msync(dbptr, 0, MS_SYNC | MS_INVALIDATE) == ERROR)
    status = COMMIT_ERROR;

  if (MSGLVL(DETAILS)) {
    printf("commit_entry(): ");
    ShowStatus(status);
  }

  return status;
}
