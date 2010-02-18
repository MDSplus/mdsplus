// get_crate_status.c
//-------------------------------------------------------------------------
//	Stuart Sherman
//	MIT / PSFC
//	Cambridge, MA 02139  USA
//
//	This is a port of the MDSplus system software from VMS to Linux, 
//	specifically:
//			CAMAC subsystem, ie libCamShr.so and verbs.c for CTS.
//-------------------------------------------------------------------------
//	$Id$
//-------------------------------------------------------------------------

//-----------------------------------------------------------
// include files
//-----------------------------------------------------------
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "crate.h"
#include "prototypes.h"

//-----------------------------------------------------------
// eg. *crate_name == "GKB509"
// Tue Apr  3 16:37:04 EDT 2001
// Fri Aug 24 15:59:54 EDT 2001	-- fixed powered off condition (???)
// Mon Aug 27 11:26:10 EDT 2001 -- re-fixed (?) powered off condition
//-----------------------------------------------------------
int get_crate_status( char *crate_name, int *ptr_crate_status )
{
	char					controller[10];
	short					SCCdata;
	int						status = SUCCESS;		// optimistic ...
	TranslatedIosb 			iosb;

	if( MSGLVL(FUNCTION_NAME) )
		printf( "get_crate_status()\n" );

	// create full crate controller designation
	// NB! all crate controllers reside in station(slot) 30
	sprintf( controller, "%.6s:N30", crate_name );

	// get crate status
	SCCdata = 0;

	status = CamPiow(
					controller,		// serial crate controller name
					0,				// A	--\__ read status register
					1,				// F	--/
					&SCCdata,		// returned status
					16,				// mem == 16-bit data
					&iosb			// *iosb
					);

	*ptr_crate_status = (short)((status & 1) ? SCCdata : 0) & 0x0ffff;

	if( MSGLVL(DETAILS) )
		printf( "gcs(): %.6s  SCCdata = 0x%0x  CamPiow()status = 0x%0x  is %s-LINE\n",
			controller, 
			SCCdata, //*ptr_crate_status,
			status, 
			( *ptr_crate_status & 0x3c00 || *ptr_crate_status & 3 ) ? "off" : "ON"
			);

	return SUCCESS;
}
