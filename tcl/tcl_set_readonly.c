#include <string.h>
#include <stdlib.h>

#include <mds_stdarg.h>
#include <dbidef.h>
#include <dcl.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <mdsdcl_messages.h>

/***************************************************************
 * TclSetVersions:
 **************************************************************/

EXPORT int TclSetReadonly(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int status = 1;

  /*--------------------------------------------------------
   * Executable ...
   *-------------------------------------------------------*/
  status = TreeSetDbiItm(DbiREADONLY, (cli_present(ctx, "OFF")&1)==0);
  if (!(status & 1)) {
    char *msg = MdsGetMsg(status);
    *error = malloc(strlen(msg)+100);
    sprintf(*error,"Error: problem setting tree readonly\n"
	    "Error message was: %s\n",msg);
  }
  return status;
}
