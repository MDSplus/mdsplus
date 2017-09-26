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

		Name:   SERVER$DISPATCH_CLOSE

		Type:   C function

     		Author:	TOM FREDIAN

		Date:   17-APR-1992

    		Purpose: Tell all servers which performed actions in dispatch
                         table to close their trees

------------------------------------------------------------------------------

	Call sequence:

int SERVER$DISPATCH_CLOSE(DispatchTable *table)

------------------------------------------------------------------------------
   Copyright (c) 1992
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

 	Description:

------------------------------------------------------------------------------*/

#include <servershr.h>
#include "servershrp.h"
static char *Server(char *out, char *srv){
  int i;
  for (i = 0; i < 32; i++)
    out[i] = srv[i] == ' ' ? 0 : srv[i];
  out[i] = 0;
  return out;
}

EXPORT int ServerDispatchClose(void *vtable){
  char server[33];
  DispatchTable *table = (DispatchTable *) vtable;
  ActionInfo *action = table->actions;
  int num_actions = table->num;
  int i, j;
  for (i = 0; i < num_actions; i++) {
    if (action[i].dispatched && !action[i].closed) {
      ServerCloseTrees(Server(server, action[i].server));
      for (j = i + 1; j < num_actions; j++) {
	if (action[i].netid == action[j].netid)
	  action[j].closed = 1;
      }
    }
  }
  return MDSplusSUCCESS;
}
