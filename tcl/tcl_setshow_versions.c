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

EXPORT int TclSetVersions(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int status = 1;

  /*--------------------------------------------------------
   * Executable ...
   *-------------------------------------------------------*/
  switch (cli_present(ctx, "MODEL")) {
  case MdsdclPRESENT:
    status = TreeSetDbiItm(DbiVERSIONS_IN_MODEL, 1);
    break;
  case MdsdclNEGATED:
    status = TreeSetDbiItm(DbiVERSIONS_IN_MODEL, 0);
    break;
  }
  if (!(status & 1))
    goto error;
  switch (cli_present(ctx, "SHOT")) {
  case MdsdclPRESENT:
    status = TreeSetDbiItm(DbiVERSIONS_IN_PULSE, 1);
    break;
  case MdsdclNEGATED:
    status = TreeSetDbiItm(DbiVERSIONS_IN_PULSE, 0);
    break;
  }
 error:
  if (!(status & 1)) {
    char *msg = MdsGetMsg(status);
    *error = malloc(strlen(msg)+100);
    sprintf(*error,"Error: problem setting versions\n"
	    "Error message was: %s\n",msg);
  }
  return status;
}

EXPORT int TclShowVersions(void *ctx __attribute__ ((unused)), char **error __attribute__ ((unused)), char **output)
{
  int in_model, in_pulse, status;
  DBI_ITM itmlst[] =
      { {4, DbiVERSIONS_IN_MODEL, &in_model, 0}, {4, DbiVERSIONS_IN_PULSE, &in_pulse, 0}, {0, 0, 0,
											   0}
  };
  status = TreeGetDbi(itmlst);
  if (status & 1) {
    *output = malloc(500);
    sprintf(*output, "  Versions are %s in the model file and %s in the shot file.\n",
	    in_model ? "enabled" : "disabled", in_pulse ? "enabled" : "disabled");
  } else {
    char *msg = MdsGetMsg(status);
    *error = malloc(strlen(msg) + 100);
    sprintf(*error, "Error: problem obtaining versions.\n" "Error message was: %s\n", msg);
  }
  return status;
}
