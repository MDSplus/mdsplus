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
