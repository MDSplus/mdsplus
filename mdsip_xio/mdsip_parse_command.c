#include "mdsip.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static char mdsip_server_type(void);

static int help(char *command)
{
  fprintf(stdout,"\n\n%s -p port -H hostfile -s security-level -shared -rcvbuf bufsize -sndbuf bufsize -d\n\n"
	  "  -p       specifies the port this server should listen on. Can be either service name or port number.\n"
          "           DO NOT SPECIFY a port if the daemon is being run from inetd\n"
          "  -h       display this help information\n"
          "  -H       specifies the hostfile which maps remote users to local accounts\n"
          "           Default is /etc/mdsip.hosts\n"
          "  -d       enable certificate delegation (not compatible with gt2 based clients)\n"
          "  -s       specifies the security level as follows:\n\n"
          "      0 use username@hostname authentication (default)\n"
          "      1 use certificates for authentication, data sent in clear\n"
          "      2 use certificates for authentication, data sent signed\n"
          "      3 use certificates for authentication, data sent encrypted\n\n"
          "  -shared  Use shared contexts for TDI and TREES. Shared context servers are\n"
          "           typically used for action servers\n"
          "  -rcvbuf  Specify size of receive buffers (default 32768)\n"
          "  -sndbuf  Specify size of send buffers (default 32768)\n\n"
          "Examples: \n\n"
          "  inetd/xinetd service with certificate authentication, signed data exchanges:\n\n"
          "    mdsip_server -H /etc/mdsip.hosts -s 2\n\n"
          "  action server with username@hostname authentication:\n\n"
          "    mdsip_server -p 8500 -H /etc/mdsip.hosts -s 0\n\n",command);
  return 0;
}

int mdsip_parse_command(int argc, char **argv, mdsip_options_t *options)
{
  int ctr;
  static const char default_hostfile[]="/etc/mdsip.hosts";
  options->port_name=NULL;
  options->hostfile=(char *)default_hostfile;
  options->security_level=0;
  options->shared_ctx=0;
  options->rcvbuf=32768;
  options->sndbuf=32768;
  options->server_type = mdsip_server_type();
  options->delegation=0;
  if (argc < 2)
  {
    return help(argv[0]);
  }

  for (ctr = 1; ctr < argc; ctr++)
  {
    if (strcmp(argv[ctr], "-h") == 0)
    {
      return help(argv[0]);
    }
    else if (strcmp(argv[ctr], "-p") == 0)
    {
      ctr++;
      if (ctr < argc)
      {
        options->port_name = argv[ctr];
        if ((options->port = atoi(options->port_name)) == 0)
        {
          struct servent *sp = getservbyname(options->port_name,"tcp");
          if (sp != NULL)
            options->port = sp->s_port;
        }
        if (options->port == 0)
	{
          fprintf(stderr,"\n\nInvalid port number or service name provided, %s",options->port_name);
          return help(argv[0]);
        }
      }
      else
      {
        fprintf(stderr,"\n\n-p switch requires service name or port number");
        return help(argv[0]);
      }
    }
    else if (strcmp(argv[ctr], "-H") == 0)
    {
      ctr++;
      if (ctr < argc)
      {
        options->hostfile = argv[ctr];
      }
      else
      {
        fprintf(stderr,"\n\n-H switch requires hostfile specification");
        return help(argv[0]);
      }
    }
    else if (strcmp(argv[ctr], "-s") == 0)
    {
      ctr++;
      if (ctr < argc)
      {
        options->security_level = atoi(argv[ctr]);
        if (options->security_level < 0 || options->security_level > 3)
	{
          fprintf(stderr,"\n\n-s switch requires a security level from 0 to 3");
          return help(argv[0]);
        }
      }
      else
      {
	fprintf(stderr,"\n\n-s switch requires a security level from 0 to 3");
	return help(argv[0]);
      }
    }
    else if (strcmp(argv[ctr], "-shared") == 0)
    {
      options->shared_ctx = 1;
    }
    else if (strcmp(argv[ctr], "-d") == 0)
    {
      options->delegation = 1;
    }
    else if (strcmp(argv[ctr], "-rcvbuf") == 0)
    {
      ctr++;
      if (ctr < argc)
      {
        options->rcvbuf = atoi(argv[ctr]);
      }
      else
      {
	fprintf(stderr,"\n\n-rcvbuf switch requires a buffser size argument");
	return help(argv[0]);
      }
    }
    else if (strcmp(argv[ctr], "-sndbuf") == 0)
    {
      ctr++;
      if (ctr < argc)
      {
        options->sndbuf = atoi(argv[ctr]);
      }
      else
      {
	fprintf(stderr,"\n\n-sndbuf switch requires a buffser size argument");
	return help(argv[0]);
      }
    }
    else
    {
      fprintf(stderr,"\n\nInvalid switch, %s",argv[ctr]);
      return help(argv[0]);
    }
  }
  return 1;
}

static char mdsip_server_type(void)
{
  static char ctype = 0;
  if (!ctype)
  {
    union { int i bits32;
            char  c[sizeof(double)];
            float x;
            double d;
          } client_test;
    client_test.x = 1.;
    if (client_test.i == 0x4080) {
      client_test.d=12345678;
      if(client_test.c[5])
       ctype = VMSG_CLIENT;
      else
      ctype = VMS_CLIENT;
    }
    else if (client_test.i == 0x3F800000)
    {
      if (sizeof(int) == 8)
        ctype = CRAY_IEEE_CLIENT;
      else
        ctype = IEEE_CLIENT;
    }
    else
      ctype = CRAY_CLIENT;
    client_test.i = 1;
    if (!client_test.c[0]) ctype |= BigEndian;
  }
  return ctype;
}
