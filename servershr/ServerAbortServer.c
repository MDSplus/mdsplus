/*------------------------------------------------------------------------------

		Name:   SERVER$ABORT_SERVER

		Type:   C function

     		Author:	TOM FREDIAN

		Date:   17-APR-1992

    		Purpose: Abort job that server is currently working on

------------------------------------------------------------------------------

	Call sequence:

int SERVER$ABORT_SERVER( struct dsc$descriptor *server )

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

EXPORT int ServerAbortServer(char *server, int flush)
{
  struct descrip p1;
  return ServerSendMessage(0, server, SrvAbort, NULL, NULL, NULL, NULL, NULL, NULL, 1,
			   MakeDescrip(&p1, DTYPE_LONG, 0, 0, &flush));
}
