#include <stdio.h>

/*
Program to wait for MDSPlus event from the command line.
*/

void EventOccurred()
{
  exit(0);
}

main(int argc, char **argv)
{
    float forever = 1E6;
    int eventid;
    if(argc < 2){
        fprintf(stderr,"Usage: %s <event name>\n",argv[0]);
        exit(1);
    }

    MDSEventAst(argv[1], EventOccurred, 0, &eventid);
    LibWait(&forever);
}

