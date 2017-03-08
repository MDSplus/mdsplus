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
#ifndef _WIN32
/* DTG: Unix Spec v.2 has <dirent.h> depends on <sys/types.h> */
#include <sys/types.h>
#include <dirent.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <config.h>
#include <strroutines.h>

#ifdef _WIN32
EXPORT char *ServerFindServers(void **ctx __attribute__ ((unused)), char *wild_match __attribute__ ((unused))){
  return 0;
}
#else
EXPORT char *ServerFindServers(void **ctx, char *wild_match){
  char *ans = 0;
  DIR *dir = (DIR *) * ctx;
  if (dir == 0) {
    char *serverdir = getenv("MDSIP_SERVER_LOGDIR");
    if (serverdir)
      *ctx = dir = opendir(serverdir);
  }
  if (dir) {
    while (1) {
      struct dirent *entry = readdir(dir);
      if (entry) {
	char *ans_c = strcpy(malloc(strlen(entry->d_name) + 1), entry->d_name);
	if ((strcmp(ans_c, ".") == 0) || (strcmp(ans_c, "..") == 0))
          continue;
	else {
	  struct descriptor ans_d = { 0, DTYPE_T, CLASS_S, 0 };
	  struct descriptor wild_d = { 0, DTYPE_T, CLASS_S, 0 };
	  ans_d.pointer = ans_c;
	  ans_d.length = strlen(ans_c);
	  wild_d.pointer = wild_match;
	  wild_d.length = strlen(wild_match);
	  if ((StrMatchWild(&ans_d, &wild_d) & 1)) {
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
#endif
