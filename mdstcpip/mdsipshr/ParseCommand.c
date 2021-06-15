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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../mdsip_connections.h"

static void print_help(char *option)
{
  if (option)
    printf("Invalid options specified: %s\n\n", option);
  printf(
      "mdsip - MDSplus data server\n\n"
      "  Format:\n\n"
      "    mdsip [-P protocol] [-m|-s] "
#ifdef _WIN32
      "[-i|-r] "
#endif
      "[-h hostfile] [-c[level]] [-p port] [protocol-specific-options]\n\n"
      "      -?, --help         This help message\n"
      "      -P protocol        Specifies server protocol, defaults to tcp\n"
      "          --protocol=protocal-name\n"
      "      -p port|service    Specifies port number or tcp service name\n"
      "          --port=port|service\n"
      "      -m, --multi        Use multi mode\n"
      "                         Accepts multiple connections each with own "
      "context.\n"
      "      -s, --server       Use server mode\n"
      "                         Accepts multiple connections with shared "
      "context.\n"
      "                         Used for action servers.\n"
#ifdef _WIN32
      "      -i, --install      Install service\n"
      "      -r, --remove       Remove service\n"
#endif
      "      -h <hostfile>      Specify alternate mdsip.hosts file.\n"
      "                         Defaults to "
#ifdef _WIN32
      "C:\\mdsip.hosts"
#else
      "/etc/mdsip.hosts"
#endif
      ".\n"
      "          --hostfile=hostfile\n"
      "      -c[<level>]        Specify maximum zlip compression level\n"
      "                         0: none or 1-9: fastest/least to slowest/most\n"
      "                         defaults to 9 if present and 0 if absent\n"
      "          --compression[=<level>]\n\n");
  exit(option ? 1 : 0);
}

///
/// Parse standard input command options.
///
void ParseCommand(int argc, char **argv, Options options[], int more,
                  int *rem_argc, char ***rem_argv)
{
  char **extra_argv = malloc(argc * sizeof(char *));
  int i;
  int extra_argc = 1;
  extra_argv[0] = argv[0];
  for (i = 1; i < argc; i++)
  {
    char *arg = argv[i];
    size_t arglen = strlen(arg);
    if (arg[0] == '-')
    {
      int option_found = FALSE;
      int opt, islong;
      arg++;
      arglen--;
      if ((islong = *arg == '-'))
      {
        arg++;
        arglen--;
      }
      if (strcmp("help", arg) == 0)
        print_help(0);
      for (opt = 0; options[opt].short_name || options[opt].long_name; opt++)
      {
        if (islong)
        {
          if (!options[opt].long_name)
            continue;
          char *long_name = options[opt].long_name;
          size_t optlen = strlen(long_name);
          if ((arglen == optlen || (arglen > optlen && arg[optlen] == '=')) &&
              strncmp(arg, long_name, optlen) == 0)
          {
            option_found = options[opt].present = TRUE;
            if (arglen > optlen)
              options[opt].value = &arg[optlen + 1];
            else if (options[opt].expects_value == 1)
            {
              if (++i < argc)
              {
                options[opt].value = argv[i];
              }
              else
              {
                fprintf(stderr,
                        "Invalid option: --%s expects value but non given.\n",
                        options[opt].long_name);
                exit(1); // ERROR_INVALID_PARAMETER
              }
            }
            else if (options[opt].expects_value == 2)
            {
              if (i + 1 < argc && argv[i + 1][0] != '-')
                options[opt].value = argv[++i];
            }
            break;
          }
        }
        else if (options[opt].short_name &&
                 arg[0] == options[opt].short_name[0])
        {
          option_found = options[opt].present = TRUE;
          if (arglen > 1)
          {
            options[opt].value = &arg[arg[1] == '=' ? 2 : 1];
          }
          else if (options[opt].expects_value == 1)
          {
            if (++i < argc)
              options[opt].value = argv[i];
            else
            {
              fprintf(stderr,
                      "Invalid option: -%s expects value but non given.\n",
                      options[opt].short_name);
              exit(1); // ERROR_INVALID_PARAMETER
            }
          }
          else if (options[opt].expects_value == 2)
          {
            if (i + 1 < argc && argv[i + 1][0] != '-')
              options[opt].value = argv[++i];
          }
          break;
        }
      }
      if (!option_found)
      {
        if (!more)
          print_help(argv[i]);
        else
        {
          extra_argv[extra_argc] = argv[i];
          extra_argc++;
        }
      }
    }
    else
    {
      extra_argv[extra_argc] = argv[i];
      extra_argc++;
    }
  }
  if (rem_argc)
  {
    *rem_argc = extra_argc;
    *rem_argv = extra_argv;
  }
  else
    free(extra_argv);
}

///
/// Parse standard input command options.
///
void ParseStdArgs(int argc, char **argv, int *extra_argc, char ***extra_argv)
{
  Options options[] = {{"P", "protocol", 1, 0, 0},
                       {"h", "hostfile", 1, 0, 0},
                       {"s", "server", 0, 0, 0},
                       {"m", "multi", 0, 0, 0},
                       {"c", "compression", 2, 0, 0},
                       {"?", 0, 0, 0, 0},
                       {0, 0, 0, 0, 0}};
  ParseCommand(argc, argv, options, 1, extra_argc, extra_argv);
  if (options[0].present && options[0].value)
    SetProtocol(options[0].value);
  if (options[1].present && options[1].value)
    SetHostfile(options[1].value);
  if (options[2].present)
  {
    if (options[3].present)
    {
      fprintf(
          stderr,
          "Invalid option: Cannot select both server mode and multi mode\n\n");

      print_help(0);
    }
    else
    {
      SetMulti(1);
      SetContextSwitching(0);
    }
  }
  if (options[3].present)
  {
    SetMulti(1);
    SetContextSwitching(1);
  }
  if (options[4].present)
  {
    int cmpr = 9;
    if (options[4].value)
    {
      char *invalid;
      cmpr = strtol(options[4].value, &invalid, 0);
      if (cmpr == 0 && (errno == EINVAL || options[4].value == invalid))
      {
        fprintf(stderr, "Invalid compression value: %s\n", options[4].value);
        exit(1);
      }
    }
    SetCompressionLevel(cmpr);
  }
  if (options[5].present)
    print_help(0);
}
