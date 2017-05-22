/*------------------------------------------------------------------------------

		Name:   ACTLOG

		Type:   C main program

		Author:	Tom Fredian, Timo Schroeder

		Date:   21-APR-1992, 2017

		Purpose: Action Logger

------------------------------------------------------------------------------

	Call sequence:

$ ACTLOG -monitor monitor-name

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

 External functions or symbols referenced:                                    */

#include "actlogp.h"

int main(int argc, char **argv)
{
  char* monitor = "ACTION_MONITOR";
  int i;
  for (i=1 ; i<argc ; i++)
    if (strcmp(argv[i],"-monitor"))
      if (++i<argc)
        monitor=argv[i];
  CheckIn(monitor);
  DoTimer();
  return 0;
}


const struct timespec ms100 = {0,100000000}; // 100ms
static void DoTimer(){
  while(!nanosleep(&ms100,NULL)) // not interrupted e.g. by Ctrl+C
     _DoTimer();
}

inline static void EventUpdate(LinkedEvent * event){
  _EventUpdate(event);
}

static void PutLog(char *time, char *mode, char *status, char *server, char *path){
  fprintf(stdout, "%s %12d %-10.10s %-44.44s %-20.20s %s\n", time, current_shot, mode, status, server, path);
}

static void PutError(char *time, char* mode, char *status, char *server, char *path){
  fprintf(stderr, "%s %12d %-10.10s %-44.44s %-20.20s %s\n", time, current_shot, mode, status, server, path);
}

static void DoOpenTree(LinkedEvent * event){
  _DoOpenTree(event);
}

static void Doing(LinkedEvent * event){
  _Doing(event);
}

static void Done(LinkedEvent * event){
  _Done(event);
}
