// xlate_logicalname.c
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

//-------------------------------------------------------------------------
// include files
//-------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "module.h"
#include "prototypes.h"

//-------------------------------------------------------------------------
// translate a logical module name to a physical representation (ie 'Key' format)
//-------------------------------------------------------------------------
// Fri May 25 12:23:23 EDT 2001
// Fri Feb  8 10:42:57 EST 2002	-- fixed type of 'key->scsi_port'
//-------------------------------------------------------------------------
// input: 	logical module name
// output:	status, and modified data referenced by pointer
//-------------------------------------------------------------------------
int xlate_logicalname( char *Name, CamKey *key )
{
	int						i;
	int						status = SUCCESS;	// otpimistic
	struct Module_			Mod;
	extern struct MODULE	*CTSdb;
	extern int				CTSdbFileIsMapped;

	if( MSGLVL(FUNCTION_NAME) )
		printf( "xlate_logicalname()\n" );

	if( strchr(Name, ':') != NULL ) {			// invalid logical name ...
		status = ERROR;							// ... was passed a physical name
		goto Xlate_LogicalName_Exit;
	}

	// check to see if CTS db is memory mapped
	if( CTSdbFileIsMapped == FALSE ) {
		if( map_data_file( CTS_DB ) != SUCCESS ) {
			status = MAP_ERROR;					// not mapped, we're done :<
			goto Xlate_LogicalName_Exit;
		}
	}

	// look up entry in db file
	if( (i = lookup_entry( CTS_DB, Name )) < 0 ) {
		status = NO_DEVICE;
		goto Xlate_LogicalName_Exit;
	}

	parse_cts_db( CTSdb+i, &Mod );				// get info ...

	// load up struct with vital info
	key->scsi_port    = Mod.adapter +'A';		// SCSI host adapter	[2002.02.08]
	key->scsi_address = Mod.id;					// SCSI id
	key->crate        = Mod.crate;				// CAMAC crate number
	key->slot         = Mod.slot;				// CAMAC slot (ie station)

Xlate_LogicalName_Exit:
	if( MSGLVL(DETAILS) ) {
		printf( "xlate(): name['%s'] ==>> HA[%c] id[%d] crate[%02d] slot[%d]\n",
			Name, key->scsi_port, key->scsi_address, key->crate, key->slot
			);
	}

	return status;
}
