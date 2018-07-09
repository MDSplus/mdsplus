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

		Name:   SERVER$FIND_SERVERS

		Type:   C function

     		Author:	TOM FREDIAN

		Date:   19-MAY-1992

    		Purpose: Find all servers in a cluster

------------------------------------------------------------------------------

	Call sequence:

int SERVER$FIND_SERVERS(int *ctx, struct dsc$descriptor *server )

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
#include <sys/types.h>
#include <dirent.h>
#include <status.h>
#include <string.h>
#include <stdlib.h>
#include <mdsplus/mdsconfig.h>
#include <strroutines.h>

EXPORT char *ServerFindServers(void **ctx, char *wild_match){
  char *ans = NULL;
  DIR *dir = (DIR *) * ctx;
  if (dir == 0) {
    char *serverdir = getenv("MDSIP_SERVER_LOGDIR");
    if (serverdir)
      *ctx = dir = opendir(serverdir);
  }
  if (dir) {
    for (;;) {
      struct dirent *entry = readdir(dir);
      if (entry) {
	char *ans_c = strcpy(malloc(strlen(entry->d_name) + 1), entry->d_name);
	if ((strcmp(ans_c, ".") == 0) || (strcmp(ans_c, "..") == 0))
          continue;
	else {
	  struct descriptor ans_d  = { strlen(ans_c),      DTYPE_T, CLASS_S, ans_c };
	  struct descriptor wild_d = { strlen(wild_match), DTYPE_T, CLASS_S, wild_match };
	  if IS_OK(StrMatchWild(&ans_d, &wild_d)) {
	    ans = ans_c;
            break;
          }
	}
        free(ans_c);
      } else {
	closedir(dir);
        *ctx=0;
        break;
      }
    }
  }
  return ans;
}
