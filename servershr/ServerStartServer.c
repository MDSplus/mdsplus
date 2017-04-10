/*------------------------------------------------------------------------------

		Name:   SERVER$START_SERVER

		Type:   C function

     		Author:	TOM FREDIAN

		Date:   17-APR-1992

    		Purpose: Start a server process

------------------------------------------------------------------------------

	Call sequence:

int SERVER$START_SERVER( struct dsc$descriptor *server )

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

EXPORT int ServerStartServer(char *server){
  return ServerConnect(server);
}
