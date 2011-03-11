#include "mdsip_connections.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

IoRoutines *LoadIo(char *protocol_in) {
  DESCRIPTOR(symbol_dsc,"Io");
  struct descriptor image_dsc={0,DTYPE_T,CLASS_S,0};
  IoRoutines *(*rtn)();
  char *protocol;
  int status;
  int i;
  if (protocol_in == 0)
    protocol_in="TCP";
  protocol=strcpy((char *)malloc(strlen(protocol_in)+1),protocol_in);
  for (i=0;i<strlen(protocol);i++)
    protocol[i]=toupper(protocol[i]);
  image_dsc.pointer=strcpy((char *)malloc(strlen(protocol)+36),"MdsIp");
  strcat(image_dsc.pointer,protocol);
  image_dsc.length=strlen(image_dsc.pointer);
  status=LibFindImageSymbol(&image_dsc,&symbol_dsc,&rtn);
  free(image_dsc.pointer);
  if (status & 1 && rtn != 0) {
    free(protocol);
    return rtn();
  } else {
    fprintf(stderr,"Protocol %s is not supported\n",protocol);
    free(protocol);
    return 0;
  }
}
