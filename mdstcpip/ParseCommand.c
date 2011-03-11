#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void PrintHelp(char *option) {
  if (option)
    printf("Invalid options specified: %s\n\n",option);
  printf("mdsip - MDSplus data server\n\n"
	 "  Format:\n\n"
	 "    mdsip -P protocol -p port|service [-m|-s] [-i|-r] [-h hostfile] [-c|+c]\n\n"
	 "      -P protocol        Specifies server protocol, defaults to tcp\n"
	 "      -p port|service    Specifies port number or tcp service name\n"
	 "      -m                 Use multi mode (accepts multiple connections each with own context)\n"
	 "      -s                 Use server mode (accepts multiple connections with shared context)\n"
	 "      -I                 Use inetd mode (accepts multiple connections spawning a process to handle each connection)\n"
	 "      -i                 Install service (WINNT only)\n"
	 "      -r                 Remove service (WINNT only)\n"
	 "      -h hostfile        Specify alternate mdsip_connections.hosts file\n"
	 "      -c [level]         Specify maximum zlip compression level, 0=nocompression,1-9 fastest/least to slowest/most, 0-default\n"
	 "      +c                 Allow compression up to maximum level 9\n\n"
	 "    longform switches are also permitted:\n\n"
	 "      --protocol protocal-name\n"
	 "      --port port|service\n"
	 "      --protocol io-protocol-name\n"
	 "      --multi\n"
	 "      --server\n"
	 "      --install\n"
	 "      --remove\n"
	 "      --hostfile hostfile\n"
	 "      --cert certificatefile (globus only)\n"
	 "      --key  keyfile (globus only)\n"
	 "      --nocompression\n"
	 "      --compression [level,default=9]\n\n"
	 "  Deprecated Format:\n\n    mdsip port|service [multi|server|install|install_server|install_multi|remove] [hostfile]\n");
  exit(1);
} 

static unsigned char NewMode(unsigned char newmode, unsigned char oldmode) {
  int multiple = 0;
  unsigned char ans=0;
  switch (newmode)
  {
  case 'X': if (oldmode == 0)
     ans='X';
    else
      multiple = 1;
    break;
  case 'I': if (oldmode == 0)
              ans = 'I';
            else
              multiple = 1;
            break;
  case 'm':
    SetMulti(1);
    SetContextSwitching(1);
    if (oldmode == 0)
      ans = 'm';
    else if (oldmode == 'i')
      ans = 'M';
    else
      multiple = 1;
    break;
  case 's':
    SetMulti(1);
    SetContextSwitching(0);
    if (oldmode == 0)
      ans = 's';
    else if (oldmode == 'i')
      ans = 'S';
    else
      multiple = 1;
    break;
  case 'i': 
    if (oldmode == 0)
      ans = 'i';
    else if (oldmode == 's')
      ans = 'S';
    else if (oldmode == 'm')
      ans = 'M';
    else
      multiple = 1;
    break;
  case 'r':
    if ((oldmode == 0) || (oldmode  == 's') || (oldmode == 'm'))
      ans = 'r';
    else
      multiple = 1;
    break;
  }
  if (multiple)
  {
    printf("Multiple or incompatible modes selected\n\n");
    PrintHelp(0);
  }
  return ans;
}


#ifndef HAVE_VXWORKS_H
static char *ParseOption(char *option)
{
  static char *options[][2] = 
    {{"--port","-p"},
     {"--protocol","-P"},
     {"--multi","-m"},
     {"--server","-s"},
     {"--install","-i"},
     {"--remove","-r"},
     {"--hostfile","-h"},
     {"--nocompression","-0"},
     {"--compression","-c"},
     {0,0}};
  char **opt;
  int i;
  for (i=0,opt=options[i];opt[0];i++,opt=options[i]) {
    if (strcmp(option,opt[0])==0)
      return opt[1];
  }
  fprintf(stderr,"Invalid option specified: --%s\n\n",option);
  PrintHelp(0);
  return (char *)0;
}

#endif



#ifndef HAVE_VXWORKS_H

static char *OptionValue(int argc, char **argv, int i) {
  char *ans = 0;
  if (argc > (i+1))
    ans = strcpy((char *)malloc(strlen(argv[i+1])+1),argv[i+1]);
  else {
    fprintf(stderr,"%s requires value\n",argv[i]);
    PrintHelp(0);
  }
  return ans;
}

void ParseCommand(int argc, char **argv) {
  int i;
  unsigned char mode = 0;
  if (argc < 2)
	PrintHelp(0);
  for (i=1;i<argc;i++) {
    char *option = argv[i];
    switch (option[0]) {
    case '-':
      if (option[1] == '-')
	option = ParseOption(option);
      if (strlen(option) > 2) {
	fprintf(stderr,"Invalid option specified: %s\n\n",option);
	PrintHelp(0);
      }
      switch ( option[1] ) {
      case 'P': SetProtocol(OptionValue(argc, argv, i++)); break;
      case 'p': SetPortname(OptionValue(argc, argv, i++)); break;
      case 'I': mode=NewMode('I',mode); break;
      case 'm': mode=NewMode('m',mode); break;
      case 's': mode=NewMode('s',mode); break;
      case 'i': mode=NewMode('i',mode); break;
      case 'r': mode=NewMode('r',mode); break;
      case 'h': SetHostfile(OptionValue(argc, argv, i++)); break;
      case 'c': SetCompressionLevel(atoi(argv[++i])); break;
      case '0': SetCompressionLevel(0); break;
#ifdef HAVE_WINDOWS_H
      case 'F': SetFlags(atoi(argv[++i])); break;
      case 'W': i += 2; mode='w'; break;
#endif
      default: 
	fprintf(stderr,"Invalid option specified %s\n\n",option);
	PrintHelp(option); break;
      }
      break;
    case '+':
      if (strlen(option) != 2) {
	fprintf(stderr,"Invalid option specified: %s\n\n",option);
	PrintHelp(0);
      }
      switch ( option[1] ) {
      case 'c':
	SetCompressionLevel(9);
	break;
      default: 
	PrintHelp(option);
	break;
      }
      break;
    default:
      fprintf(stderr,"Invalid option specified: %s\n\n",option);
      break;
    }
    SetMode(mode);
  }
}
#endif
