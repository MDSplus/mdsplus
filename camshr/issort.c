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
// issort.c
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

//-------------------------------------------------------------------------
// issort()
// Thu Jan 11 13:49:03 EST 2001
//-------------------------------------------------------------------------
// insertion sort
// The entry to be inserted is in the first location, subsequent entries
// are sorted. Only 1 pass through the list is needed, so algorithm is
// fairly fast.
//-------------------------------------------------------------------------
// input:       see parameter list
// output:      always 0
//-------------------------------------------------------------------------
// NB! Copied from : "Mastering Algorithms with C", Kyle Loudon,
//                                              O'Reilly & Associates, (c) 1999
//                                              ISBN: 1-56592-453-3
//-------------------------------------------------------------------------
int issort(void *data, int size, int esize, int (*compare) (const void *key1, const void *key2)
    )
{
  void *key;
  char *a = data;
  int i, j;

  //  Allocate storage for the key element.
  if ((key = (char *)malloc(esize)) == NULL)
    return -1;

  //  Repeatedly insert a key element among the sorted elements.
  for (j = 1; j < size; ++j) {
    memcpy(key, &a[j * esize], esize);
    i = j - 1;

    //  Determine the position at which to insert the key element.
    while (i >= 0 && compare(&a[i * esize], key) > 0) {
      memcpy(&a[(i + 1) * esize], &a[i * esize], esize);
      i--;
    }

    memcpy(&a[(i + 1) * esize], key, esize);
  }

  //  Free the storage allocated for sorting.
  free(key);

  return 0;
}
