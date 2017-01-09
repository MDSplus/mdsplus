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

#include <ipdesc.h>
#include <servershr.h>
#include "servershrp.h"
#include <stdlib.h>
#include <string.h>

extern int ServerConnect();
extern int GetAnswerInfoTS();

EXPORT char *ServerGetInfo(int full __attribute__ ((unused)), char *server)
{
  char *cmd = "MdsServerShr->ServerInfo:dsc()";
  char *ans;
  char *ansret;
  short len = 0;
  void *mem = 0;
  int sock = ServerConnect(server);
  if (sock >= 0) {
    int status =
	SendArg(sock, (unsigned char)0, (char)DTYPE_CSTRING, (unsigned char)1, (short)strlen(cmd),
		0, 0, cmd);
    if (status & 1) {
      char dtype;
      char ndims;
      int dims[8];
      int numbytes;
      char *reply;
      status = GetAnswerInfoTS(sock, &dtype, &len, &ndims, dims, &numbytes, (void **)&reply, &mem);
      if ((status & 1) && (dtype == DTYPE_CSTRING))
	ans = reply;
      else {
	ans = "Invalid response from server";
	len = strlen(ans);
      }
    } else {
      ans = "No response from server";
      len = strlen(ans);
    }
  } else {
    ans = "Error connecting to server";
    len = strlen(ans);
  }
  ansret = strncpy((char *)malloc(len + 1), ans, len);
  if (mem)
    free(mem);
  ansret[len] = 0;
  return (ansret);
}
