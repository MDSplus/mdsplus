// parse_crate_db.c
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
#include "crate.h"

//-------------------------------------------------------------------------
// parse_crate_db()
//-------------------------------------------------------------------------
// Fri Jan 12 09:55:17 EST 2001
// Wed Mar  7 14:31:29 EST 2001 -- fixed conversion of DSFname to device
// Thu Mar 15 12:00:33 EST 2001 -- changed format of crate.db
// Tue Mar 20 12:26:15 EST 2001 -- return -1 for non assigned /dev/sg#
// Mon Apr 30 13:15:27 EDT 2001 -- added highway type member
//-------------------------------------------------------------------------
// Parse a crate database entry from memory mapped file
//
// input:       pointer to crate.db data (in memory), and
//                      pointer to struct of crate data
// output:      none
//-------------------------------------------------------------------------
void parse_crate_db(struct CRATE *in, struct Crate_ *out)
{
  if (MSGLVL(FUNCTION_NAME))
    printf("parse_crate_db()\n");

  // build full physical crate name, eg. 'GKB509'
  sprintf(out->name, "%.2s%c%c%.2s", in->Phys_Name.prefix,	// 'GK'
	  in->Phys_Name.Adapter,	// SCSI host adapter
	  in->Phys_Name.Id,	// SCSI id number
	  in->Phys_Name.Crate	// CAMAC crate number
      );

  out->device = (in->DSFname[0] != '.') ? atoi(in->DSFname)	// valid /dev/sg#
      : -1;			// in-valid

  out->type = in->HwyType;	// highway type
  out->enhanced = in->enhanced == '1';
  out->online = in->online == '1';
}
