/*------------------------------------------------------------------------------

		Name:   MdsGetCurrentShotId

		Type:   C function

		Author:	Thomas W. Fredian

		Date:   11-OCT-1989

		Purpose: Get current shot number

------------------------------------------------------------------------------

	Call sequence:

int MdsGetCurrentShotId(experiment,shot)

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <STATICdef.h>

int MdsGetCurrentShotId()
{
  printf("MdsGetCurrentShotId is obsolete, use TreeGetCurrentShotId in TreeShr\n");
  return 0;
}

int MdsSetCurrentShotId()
{
  printf("MdsSetCurrentShotId is obsolete, use TreeSetCurrentShotId in TreeShr\n");
  return 0;
}
