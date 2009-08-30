// verbs.c
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
// Thu Apr 12 15:01:47 EDT 2001 -- added memory map support for setcrate cmd
// Wed Sep  5 11:39:41 EDT 2001	-- added dynamic debug printout control
// Thu Sep 13 16:01:19 EDT 2001	-- fixed 'show/crate' code - now works with
// 									power turned off or non-existant highway
// Fri Nov  2 15:56:27 EST 2001	-- 'cleanup'
// Fri Jan 25 15:56:16 EST 2002	-- fixed 'show' using wildcard characters
// Wed Feb 20 12:46:12 EST 2002	-- 'removed' memory leak
// Fri Feb 22 10:56:55 EST 2002	-- restructured 'deassign()'
// Thu Dec  5 12:32:50 EST 2002	-- make call to 'Autoconfig()' after Addcrate
// Mon Dec  9 15:51:05 EST 2002	-- 'ShowCrate()' fixed crate status
//-------------------------------------------------------------------------

#define	NEED_WARM_N_FUZZY	0

//-------------------------------------------------------------------------
// include files
//-------------------------------------------------------------------------
#include <mdsdescrip.h>
#include <strroutines.h>
#include <libroutines.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mdsdcldef.h>
#include <clidef.h>

#include <math.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <errno.h>

#include "common.h"
#include "prototypes.h"

#ifdef __toupper
#undef __toupper
#endif
#define __toupper(c) (((c) >= 'a' && (c) <= 'z') ? (c) & 0xDF : (c))

//-------------------------------------------------------------------------
// global stuff
//-------------------------------------------------------------------------
char					nullStr[] = "(null)";
extern int				CTSdbFileIsMapped;
extern int				CRATEdbFileIsMapped;
extern struct MODULE	*CTSdb;		// pointer to in-memory copy of data file
extern struct CRATE		*CRATEdb;

//-------------------------------------------------------------------------
// local things
//-------------------------------------------------------------------------
#define CRATE_NAME_SIZE		sizeof(struct CRATE_NAME)

//-------------------------------------------------------------------------
// function prototypes
//-------------------------------------------------------------------------
extern int  cli_get_value();
extern int  cli_present();

//-------------------------------------------------------------------------
// helper routine
//-------------------------------------------------------------------------
static void str_upcase( struct descriptor *in, struct descriptor *out ) 
{
	int i;

	for( i = 0; i < in->length; i++ )
		out->pointer[i] = __toupper( in->pointer[i] );
}

//-------------------------------------------------------------------------
// assign a new module to CTS database
//-------------------------------------------------------------------------
int Assign()
{
	char	line[MODULE_ENTRY];
	int		dbFileSize, fd, i, nullMask, numOfEntries, rc;
	int		status = SUCCESS;				// assume the best

	static DESCRIPTOR( phy_name_p, "PHY_NAME" );
	static DYNAMIC_DESCRIPTOR( phy_name );

	static DESCRIPTOR( log_name_p, "LOG_NAME" );
	static DYNAMIC_DESCRIPTOR( log_name );

	static DESCRIPTOR( comment_p, "COMMENT" );
	static DYNAMIC_DESCRIPTOR( comment );

	// get user input
	cli_get_value( &phy_name_p, &phy_name );
	str_upcase( &phy_name, &phy_name );

	cli_get_value( &log_name_p, &log_name );
	str_upcase( &log_name, &log_name );

	str_free1_dx(&comment);					// per twf -- clear out comment field
	cli_get_value( &comment_p, &comment );

	// check to see if db file exists
	if( check_for_file(CTS_DB_FILE) != SUCCESS ) {	// does not exist, yet
		// try to creat (sic) it
		if( (fd = Creat(CTS_DB_FILE, 0666)) == ERROR ) {
			status = FAILURE;				// FILE_ERROR;		[2001.07.12]
			goto Assign_Exit;				// we're done  :<
		}
		else
			close(fd);
	}

	// check to see if db file is memory mapped
	if( CTSdbFileIsMapped == FALSE ) {				// is not, so try
		if( map_data_file(CTS_DB) != SUCCESS ) {	// we're dead in the water
			status = FAILURE;		// MAP_ERROR;		[2001.07.12]
			goto Assign_Exit;
		}
	}

	// get current db file count
	if( (numOfEntries = get_file_count(CTS_DB)) < 0 ) {
		status = FAILURE;			// FILE_ERROR;		[2001.07.12]
		goto Assign_Exit;
	}

	if( numOfEntries ) {		// 1 or more
		if( lookup_entry(CTS_DB, log_name.pointer) >= 0 ) {	// duplicate !
			if( MSGLVL(IMPORTANT) )
				fprintf( stderr, "duplicate module name '%s' -- not allowed\n", log_name.pointer );

			status = FAILURE;		// DUPLICATE;		[2001.07.12]
			goto Assign_Exit;
		}
	}

	// get db file size
	if( (dbFileSize = get_db_file_size(CTS_DB_FILE)) == ERROR ) {
		status = FAILURE;			// FILE_ERROR;		[2001.07.12]
		goto Assign_Exit;
	}
	dbFileSize /= MODULE_ENTRY;	// .. current maximum number of possible module entries

	// do we need to expand db file?
	if( (dbFileSize == 0) || (numOfEntries == dbFileSize) ) {		// ... yes
		if( expand_db(CTS_DB, numOfEntries) != SUCCESS ) {			// expand ...
			status = FAILURE;		// EXPAND_ERROR; [2001.07.12]	// ... failure
			goto Assign_Exit;
		}
	}		// else OK

	// create a temporary string
	sprintf( line, "%-32s %-10s %-40s\n", 
		log_name.pointer, 								// these were entered by the user
		phy_name.pointer, 
		comment.pointer
		);

	// check comment field for null string, ie "(null)"
	nullMask = (1 << strlen(nullStr)) - 1;				// set all mask bits
	for( i = COMMENT_INDEX; i < COMMENT_INDEX + strlen(nullStr); ++i )
		if( line[i] == nullStr[i - COMMENT_INDEX] )
			nullMask &= ~(1 << (i - COMMENT_INDEX));	// clear a bit in mask

	if( nullMask == 0 )									// all mask bit have been reset, ie matched
		for( i = COMMENT_INDEX; i < COMMENT_INDEX + strlen(nullStr); ++i ) 
			line[i] = ' ';								// make it spaces

	// add it ...
	if( add_entry(CTS_DB, line) != SUCCESS ) {
		status = FAILURE;		// ASSIGN_ERROR;		[2001.07.12]
		goto Assign_Exit;
	}

#if NEED_WARM_N_FUZZY
	// write to a buffer file for a warm fuzzy ...
	if( (fd = Creat("buffer.db", 0666)) == ERROR ) {
		if( MSGLVL(ALWAYS) )
			perror("creat()");

		status = FAILURE;		// FILE_ERROR;			[2001.07.12]
		goto Assign_Exit;
	}

	rc = write(fd, line, sizeof(line));
	close(fd);
	if( rc != sizeof(line) ) {
		if( MSGLVL(ALWAYS) )
			perror("write()");

		status = FAILURE;		// FILE_ERROR;			[2001.07.12]
	}
#endif

Assign_Exit:			// we're done, so out'a here!
	if( MSGLVL(DETAILS) ) {
		printf("Assign(): "); ShowStatus(status);
	}

	return status;
}

//-------------------------------------------------------------------------
// map generic scsi device names to crate table names 
//-------------------------------------------------------------------------
int Autoconfig()
{
	char	highway_name[CRATE_NAME_SIZE + 1], *pHighwayName;
	char	line[CRATE_ENTRY];
	int		i, numOfEntries;
	int		status = SUCCESS;		// optimistic
	FILE	*fp;

	int j;
	int found, retries;

	// check to see if db file memory mapped
	if( CRATEdbFileIsMapped == FALSE ) {			// is not, so try
		if( map_data_file(CRATE_DB) != SUCCESS ) {	// we're dead in the water
			if( MSGLVL(IMPORTANT) )
				fprintf( stderr, "error memory mapping crate db file\n" );

			status = MAP_ERROR;
			goto AutoConfig_Exit;							// we're done  :<
		}
	}

	// get current db file count
	if( (numOfEntries = get_file_count(CRATE_DB)) <= 0 ) {	// nothing to do ...
		status = FILE_ERROR;
		goto AutoConfig_Exit;								// we're done  :<
	}

	// open file for read-only
	if( (fp = Fopen(CRATE_DB_FILE, "r")) == NULL ) {
		if( MSGLVL(2) )
			fprintf( stderr, "crate.db does not exist\n" );

		status = FILE_ERROR;
		goto AutoConfig_Exit;
	}

	pHighwayName = highway_name;	// point to real memory ...

	// loop thru list
	for( i = 0; i < numOfEntries; ++i ) {
		fscanf(fp, "%s", line);								// get a crate.db entry
		sprintf(pHighwayName, "%.6s", line);				// trim it

		// NB! this is a work-around -- seems necessary for the moment
		for( j = 0; j < 2; j++ ) {
			if( map_scsi_device(pHighwayName) != SUCCESS ) {	// map it if possible
				if( MSGLVL(IMPORTANT) )
					fprintf( stderr, "error mapping scsi device '%s'\n", pHighwayName );
			}
		}

// NB! this is a work-around -- seems necessary for the moment	[2001.11.02] -- see above
//		retries = 2;	// maximum retry count
//		found = FALSE;
//		while( !found && retries-- ) {
//			if( (found = map_scsi_device(pHighwayName)) != SUCCESS ) {	// map it if possible
//				fprintf( stderr, "error mapping scsi device '%s'\n", pHighwayName );
//			}
//		}

	}
	// end of for()...

AutoConfig_Exit:
	status = SUCCESS;

	return status;
} 

//-------------------------------------------------------------------------
// deassign a module
//-------------------------------------------------------------------------
int Deassign()
{
	char			db_tmp[33];
	int 			i, index, modulesToDeassign, modulesDeassigned, numOfEntries, physical_name; 
	int				status = SUCCESS;
	struct Module_	Mod, *pMod;

	static DESCRIPTOR( physical, "PHYSICAL" );

	static DESCRIPTOR( modname_p, "MODULE" );
	static DYNAMIC_DESCRIPTOR( modname );

	static DYNAMIC_DESCRIPTOR( wild );

	struct descriptor pattern;

	// get user data
	cli_get_value( &modname_p, &wild );
	StrUpcase( &wild, &wild );

	physical_name = cli_present(&physical) & 1;

	if( MSGLVL(DETAILS) )
		printf("Deassign(): %s module: <%s>\n", physical_name ? "physical" : "logical", wild.pointer);

	// check to see if db file memory mapped
	if( CTSdbFileIsMapped == FALSE ) {				// is not, so try ...
		if( map_data_file(CTS_DB) != SUCCESS ) {	// we're dead in the water
			status = MAP_ERROR;						// [2002.02.21]
			goto Deassign_Exit;
		}
	}

	// get number of current entries
	if( (numOfEntries = get_file_count(CTS_DB)) == 0 ) {	// no entries in cts db file
		if( MSGLVL(IMPORTANT) )
			fprintf( stderr, "db file empty, no entries to remove\n" );

		status = DEASSIGN_ERROR;					// [2002.02.21]
		goto Deassign_Exit;
	}

	// if we get this far, then there are modules in the database
	modulesToDeassign = modulesDeassigned = 0;	// initialize counts
	pMod = &Mod;				// point to some actual storage

	// first, we need to count the number to deassign ...
	for( i = 0; i < numOfEntries; ++i ) {	// scan entire list
		// look up module name(s). NB! more than one logical name may be
		// assigned to the same, unique physical name.
		parse_cts_db( CTSdb+i, pMod );		// extract info from db

		memset(db_tmp, ' ', 32);	// clear out buffer
		db_tmp[33] = '\0';			// ensure buffer 'ends'

		if( physical_name ) 		// physical name
			sprintf( db_tmp, "GK%c%d%02d:N%d",
				pMod->adapter + 'A', 
				pMod->id, 
				pMod->crate,
				pMod->slot
				);
		else 						// logical name 
			sprintf( db_tmp, "%s", pMod->name );

		// prepare for 'wild' match
		pattern.pointer = db_tmp;
		pattern.length  = strlen(db_tmp);

		if( StrMatchWild( &pattern, &wild ) & 1 ) {
			++modulesToDeassign;	//
		}
	} // end of for() loop, all entries checked

	// now actually remove them
	while( modulesToDeassign ) {
		for( i = 0; i < get_file_count(CTS_DB); ++i ) {
			// look up module name(s). NB! more than one logical name may be
			// assigned to the same, unique physical name.
			parse_cts_db( CTSdb+i, pMod );		// extract info from db

			memset(db_tmp, ' ', 32);	// clear out buffer
			db_tmp[33] = '\0';			// ensure buffer 'ends'

			if( physical_name ) 		// physical name
				sprintf( db_tmp, "GK%c%d%02d:N%d",
					pMod->adapter + 'A', 
					pMod->id, 
					pMod->crate,
					pMod->slot
					);
			else 						// logical name 
				sprintf( db_tmp, "%s", pMod->name );

			// prepare for 'wild' match
			pattern.pointer = db_tmp;
			pattern.length  = strlen(db_tmp);

			if( StrMatchWild( &pattern, &wild ) & 1 ) {
				if( remove_entry(CTS_DB, i) != SUCCESS ) {	// removal failed
					status = DEASSIGN_ERROR;				// [2002.02.21]
					goto Deassign_Exit;
				}
				else 
					++modulesDeassigned;	// keep track of successes :)
			}
		}

		--modulesToDeassign;				// one less to remove ...
	}

Deassign_Exit:
	if( MSGLVL(DETAILS) ) {
		printf("Deassign(%d): ", modulesDeassigned); ShowStatus(status);
	}

	return status;
}  

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
int Noop()
{ 
	if( MSGLVL(DETAILS) )
		printf("Noop() invoked -- not implemented\n");

	return SUCCESS; 
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
int ResetHighway()
{
	int status;

	static DESCRIPTOR( crate_p, "MODULE" );
	static DYNAMIC_DESCRIPTOR( wild );
	static DYNAMIC_DESCRIPTOR( crate );

	cli_get_value( &crate_p, &wild );

	if( MSGLVL(DETAILS) )
		printf("ResetHighway() invoked -- not implemented\n");

	return SUCCESS;
}

//-------------------------------------------------------------------------
// set a crate on-line or off-line
//-------------------------------------------------------------------------
int SetCrate()
{
	int status;
        char *cratename; 
        void *ctx=0;

	static DESCRIPTOR( crate_p, "CRATE" );
	static DESCRIPTOR( quietq, "QUIET" );

	static DYNAMIC_DESCRIPTOR( wild );
	static DYNAMIC_DESCRIPTOR( crate );

	static DESCRIPTOR( offq, "OFFLINE" );
	static DESCRIPTOR( onq, "ONLINE" );

	int off = cli_present(&offq) & 1;
	int on  = cli_present(&onq ) & 1;
        int quiet = cli_present(&quietq) & 1;

	cli_get_value( &crate_p, &wild );
	str_upcase( &wild, &wild );

	// check to see if db file memory mapped
	if( CRATEdbFileIsMapped == FALSE ) {			// is not, so try
		if( map_data_file(CRATE_DB) != SUCCESS ) {	// we're dead in the water
			if( MSGLVL(IMPORTANT) )
				fprintf( stderr, "error memory mapping crate db file\n" );

			status = FAILURE;		// MAP_ERROR;		[2001.07.12]
			goto SetCrate_Exit;
		}
	}
        
        while (find_crate(wild.pointer,&cratename,&ctx))
        {

	  status = turn_crate_on_off_line( cratename , (on) ? ON : OFF );
          if (!(status & 1) && !quiet)
            printf("Error turning crate %s %s\n",cratename,on ? "online" : "offline");
          free(cratename);
        }
        find_crate_end(&ctx);
SetCrate_Exit:
	return SUCCESS;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// show crate status, using crates in 'crate.db' file
//-------------------------------------------------------------------------
int ShowCrate()
{
	char			colorENH[9], colorON[9];
	char tmp[7]; char tmp2[11];
	int				enhanced, i, j, online, moduleFound, numOfCrates, numOfModules;
	int				crateStatus;
	int				status;
	struct Crate_	Cr8, *pCr8;
	struct Module_	Mod, *pMod;

	static DESCRIPTOR( crate_p,  "MODULE" );
	static DESCRIPTOR( heading1, " CRATE   ONL LAM PRV ENH" );
	static DESCRIPTOR( heading2, "=======  === === === ===" );

	static DYNAMIC_DESCRIPTOR( wild );
	static DYNAMIC_DESCRIPTOR( crate );

	if( ScsiSystemStatus() == 0 ) {
		status = SUCCESS;			// this is the function's status
		printf("scsi system is %sdown!%s\n", RED, NORMAL);
		goto ShowCrate_Exit;
	}
	if( MSGLVL(DETAILS) ) printf("scsi system is %sup!%s\n", GREEN, NORMAL);

	// user input
	cli_get_value( &crate_p, &wild );
	str_upcase( &wild, &wild );

	// check to see if crate db file memory mapped
	if( CRATEdbFileIsMapped == FALSE ) {				// is not, so try
		if( map_data_file(CRATE_DB) != SUCCESS ) {		// we're dead in the water
			if( MSGLVL(IMPORTANT) )
				fprintf( stderr, "error memory mapping crate.db file\n" );

			status = FAILURE;		// MAP_ERROR;		[2001.07.12]
			goto ShowCrate_Exit;
		}
	}

	// check to see if module db file memory mapped
	if( CTSdbFileIsMapped == FALSE ) {					// is not, so try
		if( map_data_file(CTS_DB) != SUCCESS ) {		// we're dead in the water
			if( MSGLVL(IMPORTANT) )
				fprintf( stderr, "error memory mapping cts.db file\n" );

			status = FAILURE;		// MAP_ERROR;		[2001.07.12]
			goto ShowCrate_Exit;
		}
	}

	printf( "%s\n", heading1.pointer );					// header
	printf( "%s\n", heading2.pointer );					// header

	pCr8 = &Cr8;										// point to some actual storage
	pMod = &Mod;

	// get number of crates in db file
	if( (numOfCrates = get_file_count(CRATE_DB)) > 0 ) {	// possibly something to show
		if( (numOfModules = get_file_count(CTS_DB)) > 0 ) {	// maybe some crates controllers ..
			for( i = 0; i < numOfCrates; ++i ) {
				parse_crate_db(CRATEdb+i, pCr8);
				if(  wildcard_match( wild.pointer, pCr8->name, 0,0,0 ) ) {
				  //					moduleFound = FALSE;
				  //
				  //					for( j = 0; j < numOfModules; ++j ) {
				  //						parse_cts_db(CTSdb+j, pMod);
				  //						sprintf(tmp, "GK%c%d%02d",
				  //							pMod->adapter + 'A',
				  //							pMod->id,
				  //							pMod->crate
				  //							);
				  //
				  //						if( strcmp(tmp, pCr8->name) == EQUAL ) {
				  //							if( pMod->slot == 30 ) {	// found a crate controller
				  //								moduleFound = TRUE;
				  //								break;
				  //							}
				  //						}
				  //					} // end of for(modules) ...
				  moduleFound = TRUE;
				  if( moduleFound ) {
				    crateStatus = 0;
				    
				    if(MSGLVL(8)) 
				      printf("checking '%s'\n", pCr8->name);
				    
				    status = get_crate_status(pCr8->name, &crateStatus);
				    
				    if( MSGLVL(DETAILS) )
				      printf("gcs(%s) returned %d, crate 0x%x\n", pCr8->name, status, crateStatus);
				    
				    if( status == SUCCESS ) {
				      //							online = !(crateStatus & 0x3c00)    ? TRUE  : FALSE;				// [2002.12.09]
				      //							online = !(crateStatus & 0x1000)    ? TRUE  : FALSE;				// [2002.12.09]
				      online = ((crateStatus & 0x1000) != 0x1000)    ? TRUE  : FALSE;				// [2002.12.09]
				      if( !crateStatus || crateStatus == 0x3 )	// [2001.09.10]			// [2002.12.09]
					online = FALSE;													// [2002.12.09]
				      sprintf(colorON,  "%s", (online)    ? GREEN : RED);
				      
				      //							enhanced = (online && (crateStatus & 0x4030)) ? TRUE  : FALSE;		// [2002.12.09]
				      enhanced = (online && (crateStatus & 0x4000)) ? TRUE  : FALSE;		// [2002.12.09]
				      sprintf(colorENH, "%s", (enhanced)  ? GREEN : RED);
				      
				      printf("%s:   %s%c%s   .   .   %s%c%s",
					     pCr8->name,
					     colorON,  (online)   ? '*' : 'X', NORMAL,
					     colorENH, (enhanced) ? '*' : '-', NORMAL
					     );
				      if( MSGLVL(4) )
					printf( "  0x%04x", crateStatus );
				      printf("\n");
				    }
				  } // end of if(moduleFound) ...
				  else {
				    printf("%.6s:   .   .   .   .\n", pCr8->name);
				  }
				} // end of if(wildcard) ...
			} // end of for(crates) ...
		} // crates, but no modules (ie no controllers)
	}
	printf( "%s\n", heading2.pointer );					// header

	status = SUCCESS;		// always (???)

ShowCrate_Exit:
	return status;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
int ShowModule()
{
	char	db_tmp[33];		// enough space for a logical name and a cstring terminator	[2002.02.20]
	int 	i, numOfEntries, status = SUCCESS;

	static DESCRIPTOR( physical, "PHYSICAL" );
	static DESCRIPTOR( module_p, "MODULE" );
	static DESCRIPTOR( format_p, "FORMAT" );
	static DESCRIPTOR( blank, " " );

	static DESCRIPTOR( heading1, "  #  Logical Name                     Physical   Comment" );	// header
	static DESCRIPTOR( heading2, "==== ================================ ========== ========================================" );

	static DYNAMIC_DESCRIPTOR( wild );
	struct Module_	Mod, *pMod;
	
	struct descriptor pattern;

	int format = cli_present(&format_p) & 1;
	int physical_name = cli_present(&physical) & 1;	// 2002.01.16

	cli_get_value( &module_p, &wild );
	StrUpcase( &wild, &wild );						// convert to upper case

	if( MSGLVL(DETAILS) )
		printf("ShowModule(): in %s order\n", (physical_name) ? "physical" : "logical" );

	// check to see if db file memory mapped
	if( CTSdbFileIsMapped == FALSE ) {				// is not, so try
		if( map_data_file(CTS_DB) != SUCCESS ) {	// we're dead in the water
			if( MSGLVL(IMPORTANT) )
				fprintf( stderr, "error memory mapping database file\n" );

			status = FAILURE;		// MAP_ERROR;		[2001.07.12]
			goto Showmodule_Exit;
		}
	}

	// db file now mapped, continue
	pMod = &Mod;										// point to some actual storage
	if( (numOfEntries = get_file_count(CTS_DB)) > 0 ) {	// something to show
		printf( "%s\n",   heading1.pointer );
		printf( "%s%d\n", heading2.pointer, numOfEntries );
		for( i = 0; i < numOfEntries; ++i ) {
			parse_cts_db( CTSdb+i, pMod );		// extract info from db

			memset(db_tmp, ' ', 32);
			db_tmp[33] = '\0';

			if( physical_name ) 		// duh, physical name
				sprintf( db_tmp, "GK%c%d%02d:N%d", 
					pMod->adapter + 'A', pMod->id, pMod->crate, pMod->slot
					);
			else
				sprintf( db_tmp, "%s", pMod->name );

			pattern.pointer = db_tmp;
			pattern.length  = strlen(db_tmp);

			if( StrMatchWild( &pattern, &wild ) & 1 ) {
//				printf( "%s%3d: %.84s<%s\n", CYAN, i+1, (char *)CTSdb+(i * MODULE_ENTRY), NORMAL );	// fancy, with color
				printf( "%3d: %.84s<\n", i+1, (char *)CTSdb+(i * MODULE_ENTRY) );
			}
		} // end of for() loop

		printf( "%s\n", heading2.pointer );
	}
	else {
		if( MSGLVL(IMPORTANT) )
			fprintf( stderr, "db file is empty, no modules to show\n" );

		status = SUCCESS;		// Not necessarily an ERROR;	[2002.02.19]
		goto Showmodule_Exit;
	}
	
Showmodule_Exit:
	if( MSGLVL(DETAILS) ) {
		printf("ShowModule(): "); ShowStatus(status);
	}

	return status;
}

//-------------------------------------------------------------------------
// Add a crate to the crate db
//-------------------------------------------------------------------------
int AddCrate()
{
	char	line[CRATE_ENTRY + 1];
	int		dbFileSize, fd, numOfEntries;
	int		status = SUCCESS;				// assume the best

	static DESCRIPTOR( phy_name_p, "PHY_NAME" );
	static DYNAMIC_DESCRIPTOR( phy_name );

	// get user input
	str_free1_dx(&phy_name);				// per twf -- clear out field
	cli_get_value( &phy_name_p, &phy_name );
	str_upcase( &phy_name, &phy_name );

	// [2002.01.08]
	if( CRATEdbFileIsMapped == FALSE ) {					// ... no
		if( check_for_file(CRATE_DB_FILE) != SUCCESS ) {	// ... no
			if( (fd = Creat(CRATE_DB_FILE, 0666)) == ERROR ) {	// no
				status = FAILURE;
				goto AddCrate_Exit;
			}
			else
				close(fd);										// yes
		}

		if( map_data_file(CRATE_DB) != SUCCESS ) {			// failure :(
			status = MAP_ERROR;
			goto AddCrate_Exit;
		} 													// else OK :)
	}

	// get current db file count
	if( (numOfEntries = get_file_count(CRATE_DB)) == FILE_ERROR ) {
		status = FAILURE;			// FILE_ERROR;		[2001.07.12]
		goto AddCrate_Exit;
	}

	if( numOfEntries ) {		// 1 or more
		if( lookup_entry(CRATE_DB, phy_name.pointer) >= 0 ) {			// duplicate !
			if( MSGLVL(IMPORTANT) )
				fprintf( stderr, "duplicate crate name '%.6s' -- not allowed\n", phy_name.pointer );

			status = FAILURE;		// DUPLICATE;		[2001.07.12]
			goto AddCrate_Exit;
		}
	}

	// get db file size
	if( (dbFileSize = get_db_file_size(CRATE_DB_FILE)) == ERROR ) {
		status = FAILURE;			// FILE_ERROR;		[2001.07.12]
		goto AddCrate_Exit;
	}
	dbFileSize /= CRATE_ENTRY;	// .. current maximum number of possible crate entries

	// do we need to expand db file?
	if( (dbFileSize == 0) || (numOfEntries == dbFileSize) ) {				// ... yes
		if( (status = expand_db(CRATE_DB, numOfEntries)) != SUCCESS ) {		// expand
			status = FAILURE;	// EXPAND_ERROR; [2001.07.12]											// failure
			goto AddCrate_Exit;
		}
	}		// else OK

	// make an entry line, with online and enhanced set as undefined
	sprintf( line, "%-.6s:...:.:.:.\n",
		phy_name.pointer
		);

	// add it ...
	if( (status = add_entry(CRATE_DB, line)) != SUCCESS ) {
		status = FAILURE;			// ASSIGN_ERROR;		[2001.07.12]
		goto AddCrate_Exit;
	}

AddCrate_Exit:
	if( MSGLVL(DETAILS) ) {
		printf("AddCrate(): "); ShowStatus(status);
	}

	if( status == SUCCESS )			// if everything is OK ...
		Autoconfig();				// ... map crate to /dev/sg#

//ShowCrate();
	return status;
}

//-------------------------------------------------------------------------
// Delete a crate from the crate db
//-------------------------------------------------------------------------
int DelCrate()
{
	char	crateName[CRATE_NAME_SIZE + 1];
	int		index, numOfEntries;
	int		status = SUCCESS;

	static DESCRIPTOR( phy_name_p, "PHY_NAME" );
	static DYNAMIC_DESCRIPTOR( phy_name );

	// get user input
	str_free1_dx(&phy_name);				// per twf -- clear out field
	cli_get_value( &phy_name_p, &phy_name );
	str_upcase( &phy_name, &phy_name );
	// trim it...
	sprintf(crateName, "%.6s", phy_name.pointer);

	// check to see if db file memory mapped
	if( CRATEdbFileIsMapped == FALSE ) {			// is not, so try
		if( map_data_file(CRATE_DB) != SUCCESS ) {	// we're dead in the water
			status = FAILURE;		// MAP_ERROR;		[2001.07.12]
			goto DelCrate_Exit;
		}
	}

	// get number of current entries
	if( (numOfEntries = get_file_count(CRATE_DB)) == 0 ) {	// no entries in crate db file
		if( MSGLVL(IMPORTANT) )
			fprintf( stderr, "db file empty, no entries to remove\n" );

		status = FAILURE;		// DELCRATE_ERROR;		[2001.07.12]
		goto DelCrate_Exit;
	}

	// try to remove from crate.db
	if( (index = lookup_entry(CRATE_DB, crateName)) >= 0 ) {	// module does exist
		if( remove_entry(CRATE_DB, index) != SUCCESS ) {		// removal failed
			status = FAILURE;		// DELCRATE_ERROR;		[2001.07.12]
			goto DelCrate_Exit;
		}
	}
	else {		// no such module
		if( MSGLVL(IMPORTANT) )
			fprintf( stderr, "delcrate(): entry '%s' not found\n", crateName);

		status = FAILURE;		// DELCRATE_ERROR;		[2001.07.12]
		goto DelCrate_Exit;
	}

DelCrate_Exit:
	if( MSGLVL(DETAILS) ) {
		printf("DelCrate() status = %d\n", status);
	}

	return SUCCESS;
}

//-------------------------------------------------------------------------
// print a help menu for cts functions
//-------------------------------------------------------------------------
int Help()
{
printf("2002.12.11 - a\n");
	printf( "CTS usage:   (NB! entries are case insensitive)\n" );
	printf( "\n" );
	printf( "ASSIGN\n" );
	printf( "CTS>  assign physical_name  logical_name    [/comment = \"this is a optional comment\"]\n" );
	printf( "             GKslnn:Ndd     ...              ... \n" );
	printf( "e.g.  assign GKC416:N11     HALPHA_AURORA14  /comment = \"AURORA 14 digitizer\"\n" );
	printf( "               ||||  ||\n" );
	printf( "               ||--  --\n" );
	printf( "               || |   |_ CAMAC crate slot(station) number -- NB!  <= 30\n" );
	printf( "               || |_____ CAMAC crate number (e.g. 0,1,...)\n" );
	printf( "               ||_______ scsi id            (e.g. 0,1,...)\n" );
	printf( "               |________ scsi host adapter  (e.g. A,B,...)\n" );
	printf( "\n" );
	printf( "DEASSIGN\n" );
	printf( "CTS> deassign logical_name\n" );
	printf( "CTS> deassign physical_name /p\n" );
	printf( "	NB! More than one logical name may be assigned to the same\n" );
	printf( "		physical name. When a unique logical name is removed,\n" );
	printf( "		only that module is deassigned. When a physical name is\n" );
	printf( "		removed, all associated logical names will be removed, too.\n" );

	return SUCCESS;
}
