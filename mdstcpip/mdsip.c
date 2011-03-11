#include "mdsip_connections.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
  IoRoutines *io;
  ParseCommand(argc,argv);
  io=LoadIo(GetProtocol());
  if (io && io->listen)
    io->listen(argc,argv);
  else {
    fprintf(stderr,"Protocol %s does not support servers\n",GetProtocol());
    return 1;
  }
  return 0;
}
