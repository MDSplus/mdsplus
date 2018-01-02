/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
// map_scsi_device.c
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

#include "common.h"
#include "prototypes.h"

//-------------------------------------------------------------------------
// local struct
//-------------------------------------------------------------------------
struct scsi_info {
  char adapter;
  int scsi_id;
};

//-------------------------------------------------------------------------
// Fri Mar  9 15:12:40 EST 2001
// Thu Mar 15 13:40:01 EST 2001
// Mon Mar 26 13:49:08 EST 2001
// Tue Apr  3 16:45:23 EDT 2001
// Mon Apr 30 16:37:43 EDT 2001 -- added support for highway type
// Fri Jul  6 11:37:19 EDT 2001 -- 'tighten-up' search loop
//-------------------------------------------------------------------------
// lookup a CAMAC device (either a serial highway or a module) and
// return o/s specific device number, eg '/dev/sg#'
// puts value into crate.db; leaves unchanged if not found
// NB! called by 'autoconfig()' in cts::verbs
//-------------------------------------------------------------------------
int map_scsi_device(char *highway_name)
{
  char line[80], *pline, tmp[7];
  char dsf[11], hwytype='.';
  int adapter, i, numOfEntries, scsi_id, sg_number;
  int status = SUCCESS;		// optimistic
  int found = FALSE;
  FILE *fp, *fopen();
  extern struct CRATE *CRATEdb;	// pointer to in-memory copy of data file

  if (MSGLVL(FUNCTION_NAME))
    printf("map_scsi_device('%s')\n", highway_name);

  // open '/proc' filesystem scsi info
  if ((fp = fopen(PROC_FILE, "r")) == NULL) {
    if (MSGLVL(ALWAYS))
      fprintf(stderr, "failure to open '%s'\n", PROC_FILE);

    status = FILE_ERROR;	// serious error !!! no scsi devices to check
    goto MapScsiDevice_Exit;
  }
  // get current db file count
  if ((numOfEntries = get_file_count(CRATE_DB)) <= 0) {
    status = FILE_ERROR;
    goto MapScsiDevice_Exit;	// we're done  :<
  }
  if (MSGLVL(DETAILS))
    printf("crate.db count= %d\n", numOfEntries);

  // lookup highway name
  if (MSGLVL(DETAILS))
    printf("msd() looking up '%s'\n", highway_name);
  if ((i = lookup_entry(CRATE_DB, highway_name)) < 0) {
    status = NO_DEVICE;		// no such device in db file
    goto MapScsiDevice_Exit;
  }
  if (MSGLVL(DETAILS))
    printf("msd(): lookup index [%d]:%s\n", i, highway_name);

  // point to actual memory
  pline = &line[0];

  // scan all scsi devices
  sg_number = 0;		// start at the beginning
  while (!found && (pline = fgets(line, sizeof(line), fp)) != NULL) {
    if (strncmp(pline, "Host:", 5) == EQUAL) {
      sscanf(line, "Host: scsi%d Channel: %*2c Id: %d %*s", &adapter, &scsi_id);

      sprintf(tmp, "GK%c%d", 'A' + adapter, scsi_id);
      if (strncmp(tmp, highway_name, 4) == EQUAL) {	// found it
	if (QueryHighwayType(tmp) == SUCCESS)	// determine highway type
	  hwytype = tmp[5];

	// we're done, so exit
	found = TRUE;
      } else
	sg_number++;
    }				// end of if() ....
  }				// end of while() ...

  // 'lock' file with semaphore
  if (lock_file() != SUCCESS) {
    status = FAILURE;		// LOCK_ERROR;          [2001.07.12]
    goto MapScsiDevice_Exit;
  }
  // update memory mapped version
  if (found) {
    sprintf(dsf, "%03d", sg_number);	// format conversion
    strncpy((CRATEdb + i)->DSFname, dsf, 3);	// real device number
    (CRATEdb + i)->HwyType = hwytype;	// highway type
  } else {
    strncpy((CRATEdb + i)->DSFname, "...", 3);	// place-holder device number
    (CRATEdb + i)->HwyType = '.';
  }

  // commit changes to file
  if (commit_entry(CRATE_DB) != SUCCESS) {
    status = FAILURE;		// COMMIT_ERROR;        [2001.07.12]
    goto MapScsiDevice_Exit;
  }
  if (MSGLVL(DETAILS))
    printf("'%.4s+%.3s+%c'\n", highway_name, (CRATEdb + i)->DSFname, (CRATEdb + i)->HwyType);

  // unlock file
  if (unlock_file() != SUCCESS) {
    status = FAILURE;		// UNLOCK_ERROR;        [2001.07.12]
    goto MapScsiDevice_Exit;
  }

 MapScsiDevice_Exit:
  // cleanup
  if (fp)
    fclose(fp);

  return status;
}
