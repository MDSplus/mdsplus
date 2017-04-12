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

#include <ipdesc.h>
#include <servershr.h>
#include "servershrp.h"

EXPORT int ServerCreatePulse(int *id, char *server, char *tree, int shot,
		      void (*ast) (), void *astprm, int *retstatus, pthread_rwlock_t *lock, void (*before_ast) ())
{
  struct descrip p1, p2;
  return ServerSendMessage(id, server, SrvAction, retstatus, lock, NULL, ast, astprm, before_ast, 2,
			MakeDescrip(&p1, DTYPE_CSTRING, 0, 0, tree),
			MakeDescrip(&p2, DTYPE_LONG, 0, 0, &shot));
}
