/*------------------------------------------------------------------------------

		Name:   SERVER$STOP_SERVER

		Type:   C function

     		Author:	TOM FREDIAN

		Date:   17-APR-1992

    		Purpose: Stop a server process

------------------------------------------------------------------------------

	Call sequence:

int SERVER$STOP_SERVER( struct dsc$descriptor *server )

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

EXPORT int ServerStopServer(char *server){
  return ServerSendMessage(0, server, SrvStop, 0, 0, 0, 0, 0, 0);
}
