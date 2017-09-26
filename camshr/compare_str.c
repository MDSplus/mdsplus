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
// compare_str.c
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
// Fri Sep 14 11:17:47 EDT 2001 -- fixed 'subset' problem, now check string
//                                                                      lengths
// Mon Apr 29 14:48:04 EDT 2002 -- refixed length problem
// Tue May 14 16:36:00 EDT 2002 -- once again :(
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// include files
//-------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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

#define CLR		33

// function prototype (local)
static int strncmp_nocase(const char *s, const char *t, int n);

//-------------------------------------------------------------------------
// compare_str()
//-------------------------------------------------------------------------
// user supplied compare function for bisearch()
//
// input:       pointer to items to be compared
// output:      -1 for first less the second,
//                       0 for equal,
//                      +1 for first greater than second
//-------------------------------------------------------------------------
int compare_str(const void *key1, const void *key2)
{
  int length, retval;

  if (MSGLVL(11))		//              v                    v                   v                    v
    printf
	("comparing \033[01;%dm[\033[0m%s\033[01;%dm]\033[0m \033[01;%dm[\033[0m%s\033[01;%dm]\033[0m\n",
	 CLR, (char *)key1, CLR, CLR, (char *)key2, CLR);

  length = (strlen(key1) >= strlen(key2)) ? strlen(key1) : strlen(key2);
  if ((retval = strncmp_nocase((const char *)key1, (const char *)key2, length)) > 0) {
    if (MSGLVL(8))
      printf("comp(): key1 > key2\n");

    return 1;
  } else if (retval < 0) {
    if (MSGLVL(8))
      printf("comp(): key1 < key2\n");

    return -1;
  } else {
    if (MSGLVL(8))
      printf("comp(): key1 == key2\n");

    return 0;
  }
}

//-------------------------------------------------------------------------
// same as 'strncmp()' but ignores case 
//              (shamelessly copied from other mdsplus code)
//-------------------------------------------------------------------------
static int strncmp_nocase(const char *s, const char *t, int n)
{
  int i=0;

  if (MSGLVL(11))
    printf("string sizes: *s(%d)  *t(%d)  n(%d)\n", (int)strlen(s),(int)strlen(t), n);

  for (; n; n--, s++, t++) {
    if (MSGLVL(11))
      printf("*s='%c' *t='%c' n=%d\n", *s, *t, n);

    if ((i = toupper(*s) - toupper(*t)))
      break;
  }

  if (MSGLVL(11))
    printf("result of compare: %d\n", i);

  return i;
}
