#include "mdsip_connections.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
  IoRoutines *io;
  int extra_argc;
  char **extra_argv;
  ParseStdArgs(argc,argv,&extra_argc,&extra_argv);
  io=LoadIo(GetProtocol());
  if (io && io->listen)
    io->listen(extra_argc,extra_argv);
  else {
    fprintf(stderr,"Protocol %s does not support servers\n",GetProtocol());
    return 1;
  }
  return 0;
}
