/*------------------------------------------------------------------------------

		Name:   SERVER$SET_DETAIL_PROC

		Type:   C function

     		Author:	TOM FREDIAN

		Date:   16-NOV-1993

    		Purpose: Set/Get detail procedure used during show server

------------------------------------------------------------------------------

	Call sequence:

void SERVER$SET_DETAIL_PROC( void (*)(struct dsc$descriptor *) )
void (*)(struct dsc$descriptor *)SERVER$GET_DETAIL_PROC()

------------------------------------------------------------------------------
   Copyright (c) 1993
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

 	Description:

------------------------------------------------------------------------------*/

#include <config.h>

static char *(*DetailProc) () = 0;
EXPORT void ServerSetDetailProc(char *(*detail_proc) ()){
  DetailProc = detail_proc;
}

EXPORT char *(*ServerGetDetailProc()) () {
  return DetailProc;
}
