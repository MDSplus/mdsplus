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
