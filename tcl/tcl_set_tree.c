#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <mds_stdarg.h>
#include <mdsdescrip.h>
#include <dcl.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <treeshr_messages.h>
#include <mdsdcl_messages.h>

#include "tcl_p.h"

extern int TdiExecute();

int tclStringToShot(char *str, int *shot_out, char **error)
{
  int shot = -2;
  int status = TreeINVSHOT;
  char *endptr;
  if (str && strlen(str) > 0) {
    status = 1;
    /* First try just treating arg as an integer string */
    shot = strtol(str, &endptr, 0);
    if (*endptr != 0) {
      /* Next see if the string "model" was used. */
      if (strcasecmp(str, "model") == 0)
	shot = -1;
      else {
	/* Next see if TDI can make sense of the string */
	DESCRIPTOR_LONG(dsc_shot, &shot);
	struct descriptor str_d = { strlen(str), DTYPE_T, CLASS_S, str };
	status = TdiExecute(&str_d, &dsc_shot MDS_END_ARG);
	if (!(status & 1)) {
	  *error = malloc(strlen(str) + 100);
	  sprintf(*error, "Error: Could not convert shot specified '%s' to a valid shot number.\n",
		  str);
	}
      }
    }
    if ((status & 1) && (shot < -1)) {
      *error = malloc(100);
      sprintf(*error, "Error: Invalid shot number specified - %d\n", shot);
      status = TreeINVSHOT;
    }
  } else
    *error = strdup("Error: Zero length shot string specified\n");
  if (status & 1)
    *shot_out = shot;
  return status;
}

	/***************************************************************
	 * TclSetTree:
	 **************************************************************/
EXPORT int TclSetTree(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int sts = MdsdclIVVERB;
  int shot;
  char *filnam = 0;
  char *asciiShot = 0;

		/*--------------------------------------------------------
		 * Executable ...
		 *-------------------------------------------------------*/
  cli_get_value(ctx, "FILE", &filnam);
  cli_get_value(ctx, "SHOTID", &asciiShot);
  sts = tclStringToShot(asciiShot, &shot, error);
  if (sts & 1) {
    if (cli_present(ctx, "READONLY") & 1)
      sts = TreeOpen(filnam, shot, 1);
    else
      sts = TreeOpen(filnam, shot, 0);
    if (sts & 1)
      TclNodeTouched(0, tree);
    else {
      char *msg = MdsGetMsg(sts);
      *error = malloc(strlen(filnam) + strlen(msg) + 100);
      sprintf(*error, "Error: Failed to open tree '%s' shot %d\n"
	      "Error message was: %s\n", filnam, shot, msg);
    }
  }
  if (filnam)
    free(filnam);
  if (asciiShot)
    free(asciiShot);
  return sts;
}
