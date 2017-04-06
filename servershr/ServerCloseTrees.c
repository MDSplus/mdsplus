/*------------------------------------------------------------------------------

		Name:   SERVER$CLOSE_TREES

		Type:   C function

     		Author:	TOM FREDIAN

		Date:   17-APR-1992

    		Purpose: Tell server to close all trees that are open

------------------------------------------------------------------------------

	Call sequence:

int SERVER$CLOSE_TREES( struct dsc$descriptor *server )

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

EXPORT int ServerCloseTrees(char *server){
  return ServerSendMessage(0, server, SrvClose, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL);
}
