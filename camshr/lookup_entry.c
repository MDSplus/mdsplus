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
// lookup_entry.c
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
#include <ctype.h>
#include <unistd.h>
#include <mdsdescrip.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <errno.h>
#include <strroutines.h>

#include "common.h"
#include "module.h"
#include "prototypes.h"
#include "cts_p.h"

//-------------------------------------------------------------------------
// lookup_entry()
// Wed Jan 10 16:10:31 EST 2001
// Tue Jan 30 14:37:30 EST 2001
// Tue Apr  3 11:34:06 EDT 2001
// Tue Apr  3 16:42:08 EDT 2001 -- changed file count test to '<='
//-------------------------------------------------------------------------
// Looks up a entry name in one of the data bases.
//              During lookup, a semaphore should be set to prevent the 
//              data file from being updated 'under our feet.' 
//              If the semaphore is set, we should wait until it is 
//              released. Lookup is via a binary search so is
//              relatively fast.
//
// USAGE:
// int = lookup_entry( dbtype, name )
//              If return value is -1, then lookup() failed,
//              otherwise, success
//
// input:       db type, 
//                      pointer to the name of a logical module name
// output:      -1 indicates not found; >= 0 is index in db file
//-------------------------------------------------------------------------
int lookup_entry(int dbType, char *entry_name)
{
  int numOfEntries, retval;

  if (MSGLVL(FUNCTION_NAME))
    printf("lookup_entry('%s')\n", entry_name);

  // get number of entries in db file
  if ((numOfEntries = get_file_count(dbType)) <= 0) {
    if (MSGLVL(DETAILS))
      fprintf(stderr, "no entries in db -- lookup() failed\n");

    retval = ERROR;
    goto Lookup_Exit;
  }
  // Look it up ...
  retval = bisearch(dbType, entry_name, numOfEntries, compare_str);

 Lookup_Exit:
  return retval;		// return results
}

extern struct CRATE *CRATEdb;

int find_crate(char *wild, char **crate, void **ctx)
{
  struct descriptor wild_d = { strlen(wild), DTYPE_T, CLASS_S, wild };
  struct context {
    int numEntries;
    int next;
  } *context;
  int status = 0;
  if (wild[wild_d.length-1]==':')
    wild_d.length--;
  if (*ctx == 0) {
    *ctx = malloc(sizeof(struct context));
    ((struct context *)*ctx)->next = 0;
    ((struct context *)*ctx)->numEntries = get_file_count(CRATE_DB);
  }
  context = (struct context *)*ctx;
  while (context->next < context->numEntries) {
    struct descriptor crate_d =
	{ sizeof(struct CRATE_NAME), DTYPE_T, CLASS_S, (char *)&CRATEdb[context->next++] };
    if (StrMatchWild(&crate_d, &wild_d) & 1) {
      *crate = memcpy(malloc(crate_d.length + 1), crate_d.pointer, sizeof(struct CRATE_NAME));
      (*crate)[crate_d.length] = 0;
      status = 1;
      break;
    }
  }
  return status;
}

void find_crate_end(void **ctx)
{
  free(*ctx);
  *ctx = 0;
}
