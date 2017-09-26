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
// QueryHighwayType.c
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
/******************************************************************
* Inquire as to CAMAC highway type. Currently supported types are *
* Jorway 411 (both old and new revisions) and Kinetic Systems     *
******************************************************************/

//-------------------------------------------------------------------------
// include files
//-------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <scsi/scsi.h>
#include <scsi/sg.h>

#include "common.h"
#include "prototypes.h"

//-------------------------------------------------------------------------
// local, private functions
//-------------------------------------------------------------------------
static char *ShowType(int type);

//-------------------------------------------------------------------------
// Thu Feb 15 12:33:49 EST 2001
// Fri Mar  9 16:21:03 EST 2001
// Wed May 23 11:07:27 EDT 2001 -- now returns highway type, 
//                                                                              and command status
// Thu May 31 16:48:32 EDT 2001 -- only have to check once, now
// Fri Jun  1 10:27:13 EDT 2001 -- 'cleaned-up' type logic
// Thu Jul 12 10:42:01 EDT 2001 -- fix 'status' return from ScsiIo()
// Thu Jul 26 16:59:13 EDT 2001
// Wed Sep  5 12:39:14 EDT 2001 -- added MSGLVL() calls
// Fri Sep  7 16:52:31 EDT 2001 -- MAJOR CHANGE :: using /proc filesystem
//                                                                              for info; if scsi system not running,
//                                                                              don't bother reading highways
//-------------------------------------------------------------------------
// NB! 'serial_hwy_driver' must point to an array at least 7 characters long,
//              and be of the form 'GKB5'..., ie physical name.
//-------------------------------------------------------------------------
int QueryHighwayType(char *serial_hwy_driver)
{
  char line[80];
  char tmpVendor[9], tmpModel[17], tmpRev[5];
  int foundHost, foundVendor, highwayType = TYPE_UNKNOWN, host_adapter, scsi_id, tmpHost, tmpId;
  int status = SUCCESS;		// optimistic
  static int channels[MAX_SCSI_BUSES][MAX_SCSI_IDS];	// persistant, highway types
  FILE *fp, *fopen();

  if (MSGLVL(FUNCTION_NAME))
    printf("QHT('%s')\n", serial_hwy_driver);

  host_adapter = toupper(*(serial_hwy_driver + 2)) - 'A';
  scsi_id = NUMERIC(*(serial_hwy_driver + 3));

  if ((fp = fopen(PROC_FILE, "r")) == NULL) {
    if (MSGLVL(ALWAYS))
      fprintf(stderr, "could *NOT* open '%s' for reading\n", PROC_FILE);

    status = FAILURE;
    goto QueryHighwayType_Exit;
  }

  if (channels[host_adapter][scsi_id] == UNDEFINED) {	// a new one ...
    if (MSGLVL(DETAILS))
      printf(" new, unknown highway type\n");

    foundHost = FALSE;
    while (!foundHost && (fgets(line, sizeof(line), fp)) != NULL) {
      if (strstr(line, "Host:")) {
	sscanf(line, "Host: scsi%1d Channel: %*2d Id: %2d", &tmpHost, &tmpId);

	if (tmpHost == host_adapter && tmpId == scsi_id) {
	  foundHost = TRUE;
	  if (MSGLVL(DETAILS))
	    printf("%s", line);
	  foundVendor = FALSE;
	  while (!foundVendor && fgets(line, sizeof(line), fp) != NULL) {
	    if (strstr(line, "Vendor:")) {
	      sscanf(line, "  Vendor: %8s Model: %16s Rev: %5s", tmpVendor, tmpModel, tmpRev);

	      foundVendor = TRUE;
	      if (MSGLVL(DETAILS))
		printf("%s", line);
	    }
	  }
	}
      }				// end of if( "Host: ..." ) test
    }

    highwayType = TYPE_UNKNOWN;	// pesimistic here :<

    if (foundHost && foundVendor) {
      if (strstr(tmpVendor, "JORWAY")) {
	if (strstr(tmpModel, "73A"))
	  highwayType = JORWAY_73A;
	else {
	  if (atoi(tmpRev) >= 12)
	    highwayType = JORWAY;
	  else
	    highwayType = JORWAY_OLD;
	}
      } else {
	if (strstr(tmpVendor, "KINSYSCO"))
	  if (strncmp(tmpModel, "2145", 4) == EQUAL)
	    highwayType = KINSYSCO;
      }
    }

    channels[host_adapter][scsi_id] = highwayType;	// save result
  } else {			// ... take a short cut, we already know the type
    highwayType = channels[host_adapter][scsi_id];

    if (MSGLVL(DETAILS))
      printf("type found before '%c'\n", ASCII(highwayType));
  }

  // prepare to return highway type
  *(BYTE *) (serial_hwy_driver + 4) = ':';	// field seperator
  *(BYTE *) (serial_hwy_driver + 5) = ASCII(highwayType);
  *(BYTE *) (serial_hwy_driver + 6) = 0;	// terminate string

 QueryHighwayType_Exit:
  if (fp)			// ... still open
    fclose(fp);

  if (MSGLVL(DETAILS)) {
    printf("\033[01;33m%s\033[0m", ShowType(highwayType));	// '33m' == yellow
    printf(" status %d\n", status);
  }

  return status;
}

//-------------------------------------------------------------------------
static char *ShowType(int type)
{
  switch (type) {
  case KINSYSCO:
    return ("kinsysco");
  case JORWAY:
    return ("jorway");
  case JORWAY_OLD:
    return ("jorway-old");
  default:
    break;
  }
  return ("unknown");
}
