/*------------------------------------------------------------------------------

		Name:   SERVER$MONITOR_CHECKIN   

		Type:   C function

     		Author:	TOM FREDIAN

		Date:   23-APR-1992

    		Purpose: Checkin routine for action monitor 

------------------------------------------------------------------------------

	Call sequence: 

int SERVER$MONITOR_CHECKIN(struct dsc$descriptor *server, void (*ast)(), int astparam, void (*link_down)())

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

int ServerMonitorCheckin(char *server, void (*ast)(), void *astparam, void (*link_down)())
{
  MonitorMsg msg;
  msg.mode = display_checkin;
  ServerSetLinkDownHandler(link_down);
  return ServerSendMessage(0, 0, server, monitor, sizeof(msg), (char *)&msg, 0, ast, astparam, 0, 0);
}
