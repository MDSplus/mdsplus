#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Program to wait for MDSPlus event from the command line.
*/

main(int argc, char **argv)
{
    int len;
    char data[256];
    if(argc < 2){
        fprintf(stderr,"Usage: %s <event name> [-d]\n",argv[0]);
        exit(1);
    }
    MDSWfevent(argv[1], 255, data, &len);
    if ((argc == 3) && (strcmp(argv[2],"-d") == 0) && len)
    {
      if (len < 256)
        data[len] = 0;
      else
        data[255] = 0;
      printf("Event %s occurred with data = \\%s\\\n",argv[1],data);
    }
    return(1);
}

