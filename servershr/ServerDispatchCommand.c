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

#include <ipdesc.h>
#include <servershr.h>
#include "servershrp.h"

EXPORT int ServerDispatchCommand(int *id, char *server, char *cli, char *command,
			  void (*ast) (), void *astprm, int *retstatus, pthread_rwlock_t *lock, void (*before_ast) ())
{
  struct descrip p1, p2;
  return ServerSendMessage(id, server, SrvCommand, retstatus, lock, NULL, ast, astprm, before_ast, 2,
			MakeDescrip(&p1, DTYPE_CSTRING, 0, 0, cli),
			MakeDescrip(&p2, DTYPE_CSTRING, 0, 0, command));
}
