/*------------------------------------------------------------------------------

		Name:   SERVER$CREATE_PULSE   

		Type:   C function

     		Author:	TOM FREDIAN

		Date:   17-APR-1992

    		Purpose: Create a single tree pulse file 

------------------------------------------------------------------------------

	Call sequence: 

int SERVER$CREATE_PULSE(int efn, struct dsc$descriptor *server, struct dsc$descriptor *tree, int *shot,
                        void (*ast)(), int astprm, int *netid, void (*link_down)())

------------------------------------------------------------------------------
   Copyright (c) 1992
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

 	Description:


------------------------------------------------------------------------------*/

#include <mdsdescrip.h>
#include <mdsserver.h>
#include <servershr.h>
#include <strroutines.h>
#include <string.h>

#define MIN(a,b) (((a) < (b)) ? (a) : (b))

int ServerCreatePulse(int efn, char *server, char *tree, int shot,
                        void (*ast)(), void *astprm, int *retstatus, int *netid, void (*link_down_handler)(),void (*before_ast)())
{ 
  CreatePulseMsg msg;
  int i;
  strncpy(msg.treename,tree,MIN(sizeof(msg.treename),strlen(tree)));
  for (i=strlen(tree); i<sizeof(msg.treename); i++) msg.treename[i]=' ';
  msg.shot = shot;
  ServerSetLinkDownHandler(link_down_handler);
  return ServerSendMessage(0, server, create_pulse, sizeof(msg), (char *)&msg, retstatus, ast, astprm, before_ast, netid);
}
