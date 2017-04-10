/*------------------------------------------------------------------------------

		Name:   SERVER$SET_LOGGING

		Type:   C function

     		Author:	TOM FREDIAN

		Date:   17-APR-1992

    		Purpose: Set logging of server

------------------------------------------------------------------------------

	Call sequence:

int SERVER$SET_LOGGING( struct dsc$descriptor *server, char *logging_mode)

------------------------------------------------------------------------------
   Copyright (c) 1992
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

 	Description:

   Set logging on server:

    0 = none
    1 = log
    2 = log + statistics

------------------------------------------------------------------------------*/

#include <ipdesc.h>
#include <servershr.h>
#include "servershrp.h"

EXPORT int ServerSetLogging(char *server, char logging_mode){
  struct descrip p1;
  return ServerSendMessage(0, server, SrvSetLogging, NULL, NULL, NULL, NULL, NULL, NULL, 1,
			MakeDescrip(&p1, DTYPE_CHAR, 0, 0, &logging_mode));
}
