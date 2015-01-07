// msg.c
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
// Tue Sep  4 15:55:06 EDT 2001 -- initial version
// Fri Sep 21 10:08:26 EDT 2001 -- compacted
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// include files
//-------------------------------------------------------------------------
#include <stdlib.h>

#include "common.h"

//-----------------------------------------------------------
// MSGLVL() - For debug. Set an environment variable to a
//                      positive integer value. An 'if()' test on MSGLVL(#)
//                      will print sections with that number and lower.
//                      Set DEBUG to 0 or unset it for no debug printpout.
//
//                      To use: e.g.
//                              if( MSGLVL(1) ) printf("debug info ...\n");
//-----------------------------------------------------------
int MSGLVL(int level)
{
  char *c;
  int dbglvl;

  c = getenv(DEBUG_VAR_NAME);	// get debug variable
  if (c) {			// if not NULL ...
    dbglvl = atoi(c);		// convert to numeric
    return (dbglvl >= level) ? TRUE : FALSE;
  } else			// ... is not set
    return FALSE;
}
