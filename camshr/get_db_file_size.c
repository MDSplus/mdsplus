// get_db_file_size.c
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
// get_db_file_size()
// Thu Jan  4 12:00:36 EST 2001
// Wed Apr 18 11:21:57 EDT 2001 -- use macro support, ie 'Stat()'
//-------------------------------------------------------------------------
// Get the actual size of the db file -- includes records not yet in use
//
// input:       name of file
// output:      return the size of the db file, or error
//-------------------------------------------------------------------------
int get_db_file_size(char *FileName)
{
  int retval;
  struct stat sbuf;

  if (MSGLVL(FUNCTION_NAME))
    printf("get_db_file_size('%s')\n", FileName);

  if (Stat(FileName, &sbuf) == ERROR) {
    perror("stat()");
    retval = FAILURE;		// error code is negative and odd
  } else
    retval = sbuf.st_size;

  if (MSGLVL(DETAILS))
    printf("gdbfs('%s')  size %d\n", FileName, retval);

  return retval;
}
