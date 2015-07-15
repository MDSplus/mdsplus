

#include <stdlib.h>
#include <stdio.h>

#include "mdstcpip/mdsip_connections.h"

// NOTE:
// This function is a mere copy of the mdsip server main and it is compiled
// here to provide the main anchor to tests programs that want to launch a
// server instance. (obviously MdsIpShr.so must be linked too..)

int mdsip_main(int argc, char **argv)
{
  IoRoutines *io;
  int extra_argc;
  char **extra_argv;
  ParseStdArgs(argc, argv, &extra_argc, &extra_argv);
  io = LoadIo(GetProtocol());
  if (io && io->listen)
    io->listen(extra_argc, extra_argv);
  else {
    fprintf(stderr, "Protocol %s does not support servers\n", GetProtocol());
    return 1;
  }
  return 0;
}
