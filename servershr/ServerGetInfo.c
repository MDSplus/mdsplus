/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*------------------------------------------------------------------------------

                Name:   SERVER$GET_INFO

                Type:   C function

                Author:	TOM FREDIAN

                Date:   17-APR-1992

                Purpose: Find out what server is doing

------------------------------------------------------------------------------

        Call sequence:

int SERVER$GET_INFO(int efn, struct dsc$descriptor *server, struct
dsc$descriptor *response)

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

#include <stdlib.h>
#include <string.h>

#include <mdsshr.h>
#include <ipdesc.h>
#include <servershr.h>
#include <socket_port.h>
#include "servershrp.h"

#include <mdsmsg.h>

extern int GetAnswerInfoTS();
extern int get_addr_port(char *server, uint32_t *addrp, uint16_t *portp);

EXPORT char *ServerGetInfo(int full __attribute__((unused)), char *server_in)
{
  char *cmd = "MdsServerShr->ServerInfo:dsc()";
  char *ans;
  char *ansret;
  short len = 0;
  void *mem = 0;
  char *srv = TranslateLogical(server_in);
  char *server = srv ? srv : server_in;
  uint32_t addr;
  uint16_t port = 0;
  if (get_addr_port(server, &addr, &port))
  {
    free(srv);
    ans = "Could not resolve server";
    len = strlen(ans);
  }
  else
  {
    int conid = ConnectToMds(server);
    free(srv);
    if (conid == INVALID_CONNECTION_ID)
    {
      ans = "Error connecting to server";
      len = strlen(ans);
    }
    else
    {
      pthread_cleanup_push((void *)DisconnectFromMds, (void *)(intptr_t)conid);
      if (IS_NOT_OK(SendArg(conid, (unsigned char)0, (char)DTYPE_CSTRING,
                            (unsigned char)1, (short)strlen(cmd), 0, 0, cmd)))
      {
        ans = "No response from server";
        len = strlen(ans);
      }
      else
      {
        char dtype;
        char ndims;
        int dims[8];
        int numbytes;
        char *reply;
        if (IS_NOT_OK(GetAnswerInfoTS(
                conid, &dtype, &len, &ndims, dims, &numbytes, &reply, &mem, 10)) ||
            (dtype != DTYPE_CSTRING))
        {
          ans = "Invalid response from server";
          len = strlen(ans);
        }
        else
        {
          ans = reply;
        }
      }
      pthread_cleanup_pop(0);
    }
  }
  ansret = strncpy((char *)malloc(len + 1), ans, len);
  free(mem);
  ansret[len] = 0;
  return ansret;
}
