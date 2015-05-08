// debug routines
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
// Tue Mar 27 16:52:46 EST 2001
// Fri Oct 26 15:24:28 EDT 2001 -- added version display
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// include files
//-------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include "common.h"
#include "module.h"
#include "prototypes.h"

//*************************************************************************
//*************************************************************************
// debug functions
//*************************************************************************
//*************************************************************************
void dump_db(int count)
{
  int i;
  extern struct MODULE *CTSdb;

  printf("dump_db(%d)\n", count);
  for (i = 0; i < count; ++i)
    printf("%3d@: >%.84s<\n", i, (char *)CTSdb + (i * MODULE_ENTRY));
}

//-------------------------------------------------------------------------
// display error messages for diagnostics purposes
//-------------------------------------------------------------------------
void ShowStatus(int statcode)
{
#if 1
// various status conditions
  switch (statcode) {
    // good things :-)
  case SUCCESS:
    printf("SUCCESS !!!\n");
    break;			//      +1 an odd number

    // bad things  :(
  case FAILURE:
    printf("failure--failure--failure\n");
    break;			//       0 an even number
  case ERROR:
    printf("general status error\n");
    break;			//  -1 * 2
  case FILE_ERROR:
    printf("file doesn't exist\n");
    break;			//  -2 * 2
  case DUPLICATE:
    printf("duplicate module name\n");
    break;			//  -3 * 2
  case LOCK_ERROR:
    printf("unable to lock file\n");
    break;			//  -4 * 2
  case MAP_ERROR:
    printf("db file failed to map to memory\n");
    break;			//  -5 * 2
  case COMMIT_ERROR:
    printf("data did not get added to db\n");
    break;			//  -6 * 2
  case UNLOCK_ERROR:
    printf("unable to unlock file\n");
    break;			//  -7 * 2
  case EXPAND_ERROR:
    printf("failure to expand db file\n");
    break;			//  -8 * 2
  case COPY_ERROR:
    printf("file copy error\n");
    break;			//  -9 * 2
  case ASSIGN_ERROR:
    printf("assign failure\n");
    break;			// -10 * 2
  case DEASSIGN_ERROR:
    printf("deassign failure\n");
    break;			// -11 * 2
  case DELCRATE_ERROR:
    printf("delete crate failure\n");
    break;			// -12 * 2
  case NO_MEMORY:
    printf("malloc() failure\n");
    break;			// -13 * 2
  case NO_DEVICE:
    printf("no scsi device found\n");
    break;			// -14 * 2
  case QUERY_ERROR:
    printf("error determining highway type\n");
    break;			// -15 * 2
  case NO_PERMISSION:
    printf("need read/write permission\n");
    break;			// -16 * 2

    // what the ...?
  default:
    printf("Unknown ERROR code\n");
    break;
  }
#endif
  return;
}

//-------------------------------------------------------------------------
// runs once, on startup. MSGLVL = 0, displays library version
//-------------------------------------------------------------------------
void _init()
{
  char base[] = "libCamShr.so";
  char file[64];
  char *pDIR;
  int major = 0, minor = 0;
  int major_done = 0, minor_done = 0;
  struct stat sbuf;
  struct tm *pTM;
  extern int CTSdbFileIsMapped;
  extern int CRATEdbFileIsMapped;

  CTSdbFileIsMapped = FALSE;	// initial state [2002.01.03]
  CRATEdbFileIsMapped = FALSE;	// initial state [2002.01.03]

  if (MSGLVL(0)) {
    pDIR = getenv("MDSPLUS_DIR");

    while (!major_done) {
      sprintf(file, "%s/lib/%s.%d", pDIR, base, major);
      if (stat(file, &sbuf) == -1)
	++major;
      else {
	major_done = 1;

	while (!minor_done) {
	  sprintf(file, "%s/lib/%s.%d.%d", pDIR, base, major, minor);
	  if (stat(file, &sbuf) == -1)
	    ++minor;
	  else
	    minor_done = 1;
	}
      }
    }

    pTM = localtime(&sbuf.st_mtime);

    printf("library version: %s$MDSPLUS/lib/%s.%d.%d%s\nbuild date:      %s%04d-%02d-%02d%s\n",
	   YELLOW,
	   base, major, minor,
	   NORMAL, YELLOW, pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday, NORMAL);
  }
}
