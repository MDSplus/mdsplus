#include "mdsip_connections.h"
#include <status.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <libroutines.h>


////////////////////////////////////////////////////////////////////////////////
//  LoadIo  ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

IoRoutines *LoadIo(char *protocol_in){
  IoRoutines *(*rtn) ();
  if (protocol_in == 0)
    protocol_in = "TCP";
  char *protocol = strcpy((char *)malloc(strlen(protocol_in) + 1), protocol_in);
  size_t i;
  for (i = 0; i < strlen(protocol) ; i++)
    protocol[i] = toupper(protocol[i]);
  struct descriptor image_dsc = { 0, DTYPE_T, CLASS_S, 0 };
  image_dsc.pointer = strcpy((char *)malloc(strlen(protocol) + 36), "MdsIp");
  strcat(image_dsc.pointer, protocol);
  image_dsc.length = (unsigned short)strlen(image_dsc.pointer);
  const DESCRIPTOR(symbol_dsc, "Io");
  int status = LibFindImageSymbol(&image_dsc, &symbol_dsc, &rtn);
  free(image_dsc.pointer);
  if (STATUS_OK && rtn) {
    free(protocol);
    return rtn();
  } else {
    fprintf(stderr, "Protocol %s is not supported\n", protocol);
    free(protocol);
    return NULL;
  }
}
