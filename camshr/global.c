// global.c
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

#include "common.h"
#include <config.h>

EXPORT int CTSdbFileIsMapped = FALSE;
EXPORT int CRATEdbFileIsMapped = FALSE;
EXPORT struct MODULE *CTSdb;
EXPORT struct CRATE *CRATEdb;
