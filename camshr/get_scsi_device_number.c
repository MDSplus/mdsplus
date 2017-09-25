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
// get_scsi_device_number.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common.h"
#include "prototypes.h"

//-------------------------------------------------------------------------
// Fri Mar  9 15:12:40 EST 2001
// Thu Mar 15 13:40:01 EST 2001
// Fri Mar 23 13:31:57 EST 2001
// Mon Apr 30 13:37:24 EDT 2001 -- massaged highway name
// Tue Sep 18 15:29:52 EDT 2001 -- removed file check; should be using memory
//                                                                      mapped file
//-------------------------------------------------------------------------
// lookup a CAMAC device (serial highway) in crate.db 
// and return O/S specific device number, eg '/dev/sg#'
// returns '/dev/sg#' value; if non-existant, parse_crate_db() returns -1
//-------------------------------------------------------------------------
int get_scsi_device_number(char *highway_name, int *enhanced, int *online)
{
  char highway[5];		// temp copy
  int device_num, i;
  struct Crate_ crate;
  extern int CRATEdbFileIsMapped;
  extern struct CRATE *CRATEdb;

  if (MSGLVL(FUNCTION_NAME))
    printf("get_scsi_device_number('%s')\n", highway_name);

  // check to see if db file is memory mapped
  if (CRATEdbFileIsMapped == FALSE) {	// not mapped so ...
    if (map_data_file(CRATE_DB) != SUCCESS) {	// ... try to map
      device_num = MAP_ERROR;	// bummer   :-(
      goto GetScsiDeviceNumber_Exit;
    }
  }
  // lookup name
  sprintf(highway, "%.4s", highway_name);	// trim to highway name, only

  if ((i = lookup_entry(CRATE_DB, highway_name)) < 0) {	// lookup actual device num
    if (MSGLVL(IMPORTANT))
      fprintf(stderr, "no such highway in 'crate.db'\n");

    device_num = NO_DEVICE;	// doesn't exist
    goto GetScsiDeviceNumber_Exit;
  }

  parse_crate_db(CRATEdb + i, &crate);	// get data from db
  device_num = crate.device;	// extract dev num from db
  *enhanced = crate.enhanced;
  *online = crate.online;

 GetScsiDeviceNumber_Exit:
  if (MSGLVL(DETAILS)) {
    if (device_num >= 0)
      printf("gsdn(): (lookup[%d]=%s) = devnum(%d)\n", i, highway, device_num);
  }

  return device_num;
}
