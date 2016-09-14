// expand_db.c
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
// expand_db()
// Thu Jan  4 14:36:02 EST 2001
// Wed Apr 18 11:21:18 EDT 2001 -- use macro support, ie 'Remove()'
//-------------------------------------------------------------------------
// Expand the CTS or CRATE db files
//
// input:       db type,
//                      current number of entries in CTS data file
// output:      status
//-------------------------------------------------------------------------
int expand_db(int dbType, int numOfEntries)
{
  char *FileName;
  int FileIncr, newCount;
  int status = SUCCESS;		// optimistic, aren't we ... :>
  char tmpfile[1024];
  strcpy(tmpfile, get_file_name("mdscts_temp_file_XXXXXX"));
  if (MSGLVL(FUNCTION_NAME))
    printf("expand_db()\n");

  // assimilate db specific information ...
  switch (dbType) {
  case CTS_DB:
    FileName = CTS_DB_FILE;
    FileIncr = CTS_DB_INCREMENT;
    break;

  case CRATE_DB:
    FileName = CRATE_DB_FILE;
    FileIncr = CRATE_DB_INCREMENT;
    break;

  default:
    return 0;
  }

  // calculate new db file size
  newCount = (((int)numOfEntries / FileIncr) + 1) * FileIncr;

  // create a TMP file
  if ((status = create_tmp_file(dbType, newCount, tmpfile)) != SUCCESS) {
    if (MSGLVL(ALWAYS))
      fprintf(stderr, "error creating TMP file\n");

    goto ExpandDB_Exit;
  }
  // only need to copy old data if there is any
  if (numOfEntries) {		// copy current db file to TMP file
    if ((status = copy(dbType, FileName, tmpfile, numOfEntries)) != SUCCESS) {
      if (MSGLVL(ALWAYS))
	fprintf(stderr, "error copying db to TMP file\n");

      goto ExpandDB_Exit;
    }
    // remove old db file
    if (Remove(FileName)) {	// non-zero is an error
      if (MSGLVL(ALWAYS)) {
	fprintf(stderr, "error removing old db file\n");
	perror("remove()");
      }

      status = EXPAND_ERROR;
      goto ExpandDB_Exit;
    }
  }

  if (rename(tmpfile, get_file_name(FileName))) {	// non-zero is an error
    if (MSGLVL(ALWAYS)) {
      fprintf(stderr, "error renaming temp db file\n");
      perror("rename()");
    }

    status = EXPAND_ERROR;
    goto ExpandDB_Exit;
  }
  chmod(get_file_name(FileName), 0666);
  // re-map file
  if (map_data_file(dbType) != SUCCESS) {
    if (MSGLVL(ALWAYS))
      fprintf(stderr, "unable to map expanded file\n");

    status = MAP_ERROR;
    goto ExpandDB_Exit;
  }

 ExpandDB_Exit:
  if (MSGLVL(DETAILS)) {
    printf("expand_db('%s'): ", get_file_name(FileName));
    ShowStatus(status);
  }

  return status;
}
