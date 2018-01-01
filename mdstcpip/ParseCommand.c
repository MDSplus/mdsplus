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
#include <mdsplus/mdsconfig.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mdsip_connections.h"

void PrintHelp(char *option)
{
  if (option)
    printf("Invalid options specified: %s\n\n", option);
  printf("mdsip - MDSplus data server\n\n"
	 "  Format:\n\n"
	 "    mdsip [-P protocol] [-m|-s] [-i|-r] [-h hostfile] [-c|+c] [-p port] [protocol-specific-options]\n\n"
	 "      -P protocol        Specifies server protocol, defaults to tcp\n"
	 "      -p port|service    Specifies port number or tcp service name\n"
	 "      -m                 Use multi mode (accepts multiple connections each with own context)\n"
	 "      -s                 Use server mode (accepts multiple connections with shared context. Used for action servers.)\n"
	 "      -i                 Install service (Windows only)\n"
	 "      -r                 Remove service (Windows only)\n"
	 "      -h hostfile        Specify alternate mdsip.hosts file. Defaults to /etc/mdsip.hosts (linux) or C:\\mdsip.hosts(windows).\n"
	 "      -c [level]         Specify maximum zlip compression level, 0=nocompression,1-9 fastest/least to slowest/most, default=9\n\n"
	 "    longform switches are also permitted:\n\n"
	 "      --help             This help message\n"
	 "      --protocol=protocal-name\n"
	 "      --port=port|service\n"
	 "      --protocol=io-protocol-name\n"
	 "      --multi\n"
	 "      --server\n"
	 "      --install\n"
	 "      --remove\n"
	 "      --hostfile=hostfile\n" "      --compression [level,default=9]\n\n");
  exit(1);
}

///
/// Parse standard input command options.
///
void ParseCommand(int argc, char **argv, Options options[], int more, int *rem_argc,
		  char ***rem_argv)
{
  int i;
  int extra_argc = 1;
  char **extra_argv = malloc(argc * sizeof(char *));
  extra_argv[0] = argv[0];
  for (i = 1; i < argc; i++) {
    char *arg = argv[i];
    size_t arglen = strlen(arg);
    if (arg[0] == '-') {
      int option_found = 0;
      int opt;
      arg++;
      arglen--;
      if (strcmp("help", arg) == 0)
	PrintHelp(0);
      for (opt = 0; options[opt].short_name || options[opt].long_name; opt++) {
	if (*arg == '-' && options[opt].long_name) {
	  char *lonarg = arg + 1;
	  size_t lonarglen = arglen - 1;
	  char *long_name = options[opt].long_name;
	  size_t optlen = strlen(long_name);
	  if (lonarglen >= optlen && strncmp(lonarg, long_name, optlen) == 0) {
	    option_found = options[opt].present = 1;
	    if (options[opt].expects_value) {
	      if (lonarglen > optlen && lonarg[optlen] == '=')
		options[opt].value = &lonarg[optlen + 1];
	      else {
		options[opt].value = argv[i + 1];
		i++;
	      }
	    }
	    break;
	  }
	} else if (options[opt].short_name && arglen > 0 && arg[0] == options[opt].short_name[0]) {
	  option_found = options[opt].present = 1;
	  if (options[opt].expects_value) {
	    if (arglen > 2 && arg[1] == '=')
	      options[opt].value = &arg[2];
	    else {
	      options[opt].value = argv[i + 1];
	      i++;
	    }
	  }
	  break;
	}
      }
      if (!option_found) {
	if (!more)
	  PrintHelp(argv[i]);
	else {
	  extra_argv[extra_argc] = argv[i];
	  extra_argc++;
	}
      }
    } else {
      extra_argv[extra_argc] = argv[i];
      extra_argc++;
    }
  }
  if (rem_argc) {
    *rem_argc = extra_argc;
    *rem_argv = extra_argv;
  } else
     free(extra_argv);
}

///
/// Parse standard input command options.
///
void ParseStdArgs(int argc, char **argv, int *extra_argc, char ***extra_argv)
{
  Options options[] = { 
      {"P", "protocol", 1, 0, 0},
      {"h", "hostfile", 1, 0, 0},
      {"s", "server", 0, 0, 0},
      {"m", "multi", 0, 0, 0},
      {"c", "compression", 1, 0, 0},
      {0, 0, 0, 0,0}
  };
  ParseCommand(argc, argv, options, 1, extra_argc, extra_argv);
  if (options[0].present && options[0].value)
    SetProtocol(options[0].value);
  if (options[1].present && options[1].value)
    SetHostfile(options[1].value);
  if (options[2].present) {
    if (options[3].present) {
      fprintf(stderr, "Cannot select both server mode and multi mode\n\n");
      PrintHelp(0);
    } else {
      SetMulti(1);
      SetContextSwitching(0);
    }
  }
  if (options[3].present) {
    SetMulti(1);
    SetContextSwitching(1);
  }
  if (options[4].present && options[4].value)
    SetCompressionLevel(atoi(options[4].value));
}
