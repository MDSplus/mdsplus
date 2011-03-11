#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *GetProtocolFromCommand(int argc, char **argv) {
  int i;
  static char *defaultProtocol="tcp";
  for (i=1;i<argc;i++)
  {
    if ((strlen(argv[i])==2 && strncmp(argv[i],"-P",2)==0) ||
	(strlen(argv[i])==11 && strncmp(argv[i],"--protocol",11)==0)) {
      if (argc>i+2) {
	if (argv[i+1][0] != '-')
	  return argv[i+1];
	else {
	  fprintf(stderr,"Invalid protocol value specified: %s\n",argv[i+1]);
	  return 0;
	}
      } else {
	fprintf(stderr,"No protocol specified\n");
	return 0;
      }
    }
  }
  return defaultProtocol;
}
