/*------------------------------------------------------------------------------

		Name:   SERVER$DISPATCH_COMMAND   

		Type:   C function

     		Author:	TOM FREDIAN

		Date:   17-APR-1992

    		Purpose: Dispatch a mdsdcl command to a server

------------------------------------------------------------------------------

	Call sequence: 

int SERVER$DISPATCH_COMMAND(int efn, struct dsc$descriptor *server, struct dsc$descriptor *cli, struct dsc$descriptor *command,
                        void (*ast)(), int astprm, int *netid, void (*link_down)(), void (*before_ast)())

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
#include <strroutines.h>
#include <servershr.h>
#include <stdlib.h>
#include <string.h>

#define MIN(a,b) (((a) < (b)) ? (a) : (b))

int ServerDispatchCommand(int efn, char *server, char *cli, char *command,
                        void (*ast)(), void *astprm, int *retstatus, int *netid, void (*link_down)(), void (*before_ast)())
{ 
  int status;
  int size = sizeof(MdsDclCommandMsg) + strlen(command);
  MdsDclCommandMsg *msg = malloc(size);
  int tablen = MIN(sizeof(msg->cli)-1,strlen(cli));
  strncpy(msg->cli,cli,tablen);
  msg->cli[tablen] = 0;
  strcpy(msg->command,command);
  ServerSetLinkDownHandler(link_down);
  status = ServerSendMessage(efn, 0, server, mdsdcl_command, size, (char *)msg, retstatus, ast, astprm, before_ast, netid);
  free(msg);
  return status;
}
