#include <stdio.h>
#include <string.h>

/*
Program to wait for MDSPlus event from the command line.
*/

static int printdata = 0;

void EventOccurred(void *astprm, int len, char *data)
{
  if (printdata)
  {
    char *s = strncpy((char *)malloc(len+1),data,len);
    s[len] = 0;
    printf("%s\n",s);
  }
  exit(0);
}

main(int argc, char **argv)
{
    float forever = 1E6;
    int eventid;
    if(argc < 2){
        fprintf(stderr,"Usage: %s <event name> [-d]\n",argv[0]);
        exit(1);
    }
    printdata = (argc == 3) && (strcmp(argv[2],"-d") == 0); 
    MDSEventAst(argv[1], EventOccurred, 0, &eventid);
    LibWait(&forever);
}

