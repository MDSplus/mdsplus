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

#include <ipdesc.h>
#include <servershr.h>
#include "servershrp.h"

int ServerMonitorCheckin(char *server, void (*ast)(), void *astprm)
{ 
  struct descrip p1,p2,p3,p4,p5,p6,p7,p8;
  char *cstring = "";
  int zero = 0;
  int mode = MonitorCheckin;
  return ServerSendMessage(0, server, SrvMonitor, 0, 0, ast, astprm, 0, 
                    8,MakeDescrip(&p1,DTYPE_CSTRING,0,0,cstring),
                      MakeDescrip(&p2,DTYPE_LONG,0,0,&zero),
                      MakeDescrip(&p3,DTYPE_LONG,0,0,&zero),
                      MakeDescrip(&p4,DTYPE_LONG,0,0,&zero),
                      MakeDescrip(&p5,DTYPE_LONG,0,0,&zero),
                      MakeDescrip(&p6,DTYPE_LONG,0,0,&mode),
                      MakeDescrip(&p7,DTYPE_CSTRING,0,0,cstring),
                      MakeDescrip(&p8,DTYPE_LONG,0,0,&zero));
}
