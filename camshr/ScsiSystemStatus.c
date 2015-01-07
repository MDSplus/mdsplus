// ScsiSystemStatus.c
//-------------------------------------------------------------------------
//      Stuart Sherman
//      MIT / PSFC
//      Cambridge, MA 02139  USA
//
//      This is a port of the MDSplus system software from VMS to Linux, 
//      specifically:
//                      CAMAC subsystem, ie libCamShr.so and verbs.c for CTS.
//-------------------------------------------------------------------------
//      $Id$
//-------------------------------------------------------------------------
// Thu Sep  6 12:23:10 EDT 2001 -- initial release
// Tue Oct 23 11:43:03 EDT 2001 -- streamlined
//-------------------------------------------------------------------------

//-----------------------------------------------------------
// include files
//-----------------------------------------------------------
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "prototypes.h"

//-----------------------------------------------------------
// local static info
//-----------------------------------------------------------
static char *scsimsg = "Attached devices: none";

int ScsiSystemStatus(void)
{
  char line[80], *pline;
  int scsiSystemStatus = 0;	// assume the worst :(
  FILE *fp, *fopen();

  if (MSGLVL(FUNCTION_NAME))
    printf("ScsiSystemStatus()\n");

  if ((fp = fopen(PROC_FILE, "r")) == NULL) {
    fprintf(stderr, "can't open '%s' for read\n", PROC_FILE);
    scsiSystemStatus = 0;
    goto ScsiSystemStatus_Exit;
  }

  pline = &line[0];
  if ((pline = fgets(line, sizeof(line), fp)) != NULL) {
    if (strncmp(pline, scsimsg, strlen(scsimsg)))
      scsiSystemStatus = 1;	// something is attached
  }

 ScsiSystemStatus_Exit:
  if (fp)			// still open
    fclose(fp);

  return scsiSystemStatus;
}
