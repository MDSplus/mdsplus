/*------------------------------------------------------------------------------

		Name:   SERVER$GET_INFO   

		Type:   C function

     		Author:	TOM FREDIAN

		Date:   17-APR-1992

    		Purpose: Find out what server is doing 

------------------------------------------------------------------------------

	Call sequence: 

int SERVER$GET_INFO(int efn, struct dsc$descriptor *server, struct dsc$descriptor *response)

------------------------------------------------------------------------------
   Copyright (c) 1992
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

 	Description:

Send Ast message to server asking server to tell what it is currently
doing.

------------------------------------------------------------------------------*/

#include <mdsdescrip.h>
#include <mdsserver.h>
#include <servershr.h>
#include <stdlib.h>
#include <string.h>

static int Efn;
static int status;

static void ReturnText(char **response, int *netnid, int *length, Msg *reply)
{
  int text_length = *length - sizeof(Msg) + 1;
  *response = (char *)malloc(text_length + 1);
  strncpy(*response,reply->data,text_length);
  (*response)[text_length] = 0;
}

static void LinkDown(int *link_id)
{
  status = ServerPATH_DOWN;
/*
  sys$setef(Efn);
*/
}

char *ServerGetInfo( int efn, int full, char *server)
{
  char *response;
  int status;
  ShowMsg msg;
  msg.full = full;
  ServerSetLinkDownHandler(LinkDown);
  status = ServerSendMessage(1, server, show, sizeof(msg), (char *)&msg, 0, ReturnText, (void *)&response, 0, 0);
/*
  if (status & 1)
    sys$waitfr(efn);
*/
  ServerSetLinkDownHandler(0);
  return response;
}
