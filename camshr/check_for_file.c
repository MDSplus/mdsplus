// check_for_file.c
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
// check_for_file()
// Fri Jan 12 10:47:22 EST 2001
// Wed Apr 18 11:19:49 EDT 2001 -- use macro support, ie 'Open()'
//-------------------------------------------------------------------------
// See if database file exists
//
// input:       file to check
// output:      status (ie SUCCESS or ERROR)
//-------------------------------------------------------------------------
int check_for_file(char *FileName)
{
  int fd, status;

  if (MSGLVL(FUNCTION_NAME))
    printf("check_for_file('%s')\n", FileName);

  if ((fd = Open(FileName, O_RDONLY)) < 0) {	// file does not exist, yet
    status = FILE_ERROR;	// :<
    goto Filecheck_Exit;
  }
  // things are good !
  close(fd);
  status = SUCCESS;		// :>

 Filecheck_Exit:
  if (MSGLVL(DETAILS)) {
    printf("check_for_file('%s'): ", FileName);
    ShowStatus(status);
  }

  return status;
}
