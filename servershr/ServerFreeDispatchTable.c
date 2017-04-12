/*------------------------------------------------------------------------------

		Name:   SERVER$FREE_DISPATCH_TABLE

		Type:   C function

     		Author:	TOM FREDIAN

		Date:    9-NOV-1993

    		Purpose: Free a dispatch table

------------------------------------------------------------------------------

	Call sequence:

int SERVER$FREE_DISPATCH_TABLE( void *vtable)

------------------------------------------------------------------------------
   Copyright (c) 1993
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

 	Description:

------------------------------------------------------------------------------*/

#include <mdsdescrip.h>
#include <mdsshr.h>
#include <stdlib.h>
#include <servershr.h>
#include "servershrp.h"
#include <treeshr.h>

EXPORT int ServerFreeDispatchTable(void *vtable){
  if (vtable) {
    DispatchTable *table = vtable;
    ActionInfo *actions = table->actions;
    int num_actions = table->num;
    int i;
    for (i = 0; i < num_actions; i++) {
      if (actions[i].referenced_by)
	free(actions[i].referenced_by);
      if (actions[i].path)
	TreeFree(actions[i].path);
      if (actions[i].event)
	free(actions[i].event);
      if (actions[i].condition) {
	MdsFree1Dx((struct descriptor_xd *)actions[i].condition, 0);
	free(actions[i].condition);
      }
    }
    free(vtable);
  }
  return MDSplusSUCCESS;
}
