// create_tmp_file.c
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
// create_tmp_file()
// Thu Jan 11 10:13:02 EST 2001
// Tue Apr 17 16:39:26 EDT 2001 -- changed file creation to 'my_creat()'
// Wed Apr 18 10:36:47 EDT 2001 -- changed 'my_creat()' to macro 'Creat()'
//-------------------------------------------------------------------------
// Create a temporary file for expanding db file
//
// input:       db type, number of place holders
// output:      status
//-------------------------------------------------------------------------
int create_tmp_file(int dbType, int count, char *filename)
{
  char fmt[10];
  int entrySize, fd, i, status = SUCCESS;

  if (MSGLVL(FUNCTION_NAME))
    printf("create_tmp_file()\n");

  if ((fd = mkstemp(filename)) == -1) {
    if (MSGLVL(ALWAYS)) {
      fprintf(stderr, "create_tmp_file(): file error %d\n", fd);
      perror("creat()");
    }

    status = FILE_ERROR;
    goto CreateTmpFile_Exit;
  }
  chmod(filename, 0666);
  // figure out size of each entry
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

  // make 'blank-entry' format specifier
  sprintf(fmt, "%%-%ds\n", entrySize - 1);

  // a local block
  {
    char line[entrySize];

    // 'blank' entry
    sprintf(line, fmt, " ");

    for (i = 0; i < count; ++i) {
      if (write(fd, line, entrySize) != entrySize) {
	if (MSGLVL(ALWAYS))
	  perror("write()");

	status = EXPAND_ERROR;
	goto CreateTmpFile_Exit;
      }
    }
  }

  close(fd);			// only get here if all is OK

 CreateTmpFile_Exit:
  if (MSGLVL(DETAILS)) {
    printf("create_tmp_file(): ");
    ShowStatus(status);
  }

  return status;
}
