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
