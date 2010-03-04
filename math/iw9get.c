#include <stdlib.h>
#include <string.h>

void iw9get_(int *iwant, void **igot) {
  int *bytesAllocated = (int *)&igot[1];
  if (*bytesAllocated != *iwant) {
    if (*bytesAllocated > 0) {
      free((char *)&igot[0] + *((int *)&igot[0])*sizeof(int));
      *bytesAllocated=0;
    }
    if (*iwant > 0) {
      igot[0]=(void *)(((char *)memset(malloc(*iwant*sizeof(int)),0,*iwant*sizeof(int))-(char *)&igot[0])/sizeof(int));
      *bytesAllocated=*iwant;
    }
  }
}
