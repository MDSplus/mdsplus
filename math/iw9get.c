#include <stdlib.h>
#include <string.h>
#include <mdstypes.h>
#include <config.h>

int MAIN__( )
{ return(0);
}


#if SIZEOF_INT_P == 8
typedef _int64 INTEGER;
#else
typedef int INTEGER;
#endif

void iw9get_(int *iwant, INTEGER *igot) {
  INTEGER *bytesAllocated = (INTEGER *)&igot[1];
  if (*bytesAllocated != *iwant) {
    if (*bytesAllocated > 0) {
      free(((char *)&igot[0]) + (igot[0]*sizeof(INTEGER)));
      *bytesAllocated=0;
    }
    if (*iwant > 0) {
      void *mem=memset(malloc(*iwant*sizeof(INTEGER)),0,*iwant*sizeof(INTEGER));
      INTEGER offset=((INTEGER)mem-(INTEGER)igot);
      offset=offset/(int)sizeof(INTEGER);
      igot[0]=offset;
      *bytesAllocated=*iwant;
    }
  }
}
