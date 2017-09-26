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
