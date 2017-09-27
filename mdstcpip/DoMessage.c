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
#include <stdlib.h>
#include <status.h>
#include <libroutines.h>

#include "mdsip_connections.h"

////////////////////////////////////////////////////////////////////////////////
//  DoMessage  /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


int DoMessage(int id)
{
  INIT_STATUS_AS MDSplusERROR;
  Message *(*processMessage) (Connection *, Message *) = 0;
  Connection *c = FindConnection(id, 0);
  if (processMessage == 0) {
    DESCRIPTOR(MdsIpSrvShr, "MdsIpSrvShr");
    DESCRIPTOR(procmsg, "ProcessMessage");
    status = LibFindImageSymbol(&MdsIpSrvShr, &procmsg, &processMessage);
  }
  if (c && processMessage) {
    Message *msgptr = GetMdsMsg(id, &status);
    Message *ans = 0;
    if STATUS_OK {
      ans = (*processMessage) (c, msgptr);
      if (ans) {
          // NOTE: [Andrea] this status is not actually tested for errors //
	status = SendMdsMsg(id, ans, 0);
	free(ans);
      }
    } else
      CloseConnection(id);
    if (msgptr)
      free(msgptr);
  }
  return status;
}

