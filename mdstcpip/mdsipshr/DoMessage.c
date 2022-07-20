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
#include <status.h>
#include <stdlib.h>

#include "../mdsip_connections.h"
#include <pthread_port.h>
#include <libroutines.h>

/// returns true if message cleanup is handled
extern int ProcessMessage(Connection *, Message *);
////////////////////////////////////////////////////////////////////////////////
//  DoMessage  /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int ConnectionDoMessage(Connection *connection)
{
  if (!connection)
    return 0; // will cause tunnel to terminate
  int err;
  INIT_AND_FREE_ON_EXIT(Message *, message);
  err = 1;
  int status = MDSplusFATAL;
  message = GetMdsMsgTOC(connection, &status, -1);
  err = !(message && STATUS_OK && ProcessMessage(connection, message));
  FREE_IF(message, err);
  return !err;
}

int DoMessage(int id)
{
  Connection *connection = FindConnectionWithLock(id, CON_ACTIVITY);
  int ok = ConnectionDoMessage(connection);
  UnlockConnection(connection);
  if (!ok)
    CloseConnection(id);
  return ok;
}
