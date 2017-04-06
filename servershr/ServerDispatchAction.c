/*------------------------------------------------------------------------------

		Name:   SERVER$DISPATCH_ACTION

		Type:   C function

     		Author:	TOM FREDIAN

		Date:   17-APR-1992

    		Purpose: Create a single tree pulse file

------------------------------------------------------------------------------

	Call sequence:

int SERVER$DISPATCH_ACTION(int efn, struct dsc$descriptor *server, struct dsc$descriptor *tree, int *shot, int *nid,
                        void (*ast)(), int astprm, int *netid, void (*link_down)(),void (*before_ast)())

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

EXPORT int ServerDispatchAction(int *id, char *server, char *tree, int shot, int nid,
			 void (*ast) (), void *astprm, int *retstatus, pthread_rwlock_t* lock, int *socket,
			 void (*before_ast) ())
{
  struct descrip p1, p2, p3;
  return ServerSendMessage(id, server, SrvAction, retstatus, lock, socket, ast, astprm, before_ast, 3,
			MakeDescrip(&p1, DTYPE_CSTRING, 0, 0, tree),
			MakeDescrip(&p2, DTYPE_LONG,    0, 0, &shot),
			MakeDescrip(&p3, DTYPE_LONG, 0, 0, &nid));
}
