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

#include <mdsdescrip.h>
#include <mdsserver.h>
#include <servershr.h>
int ServerAbortServer( char *server, int *flush )
{
  DoAbortMsg msg;
  int status;
  ServerSetLinkDownHandler(0);
  msg.flush = *flush;
  status = ServerSendMessage(1, server, doabort, sizeof(msg), (char *)&msg, 0, 0, 0, 0, 0);
  if (status & 1 && msg.flush)
    status = ServerSendMessage(0, server, doabort, sizeof(msg), (char *)&msg, 0, 0, 0, 0, 0);
  return status;
}
