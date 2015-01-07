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
