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
// Thu Apr 12 15:01:47 EDT 2001 -- added memory map support for SetCrate cmd
//
//#define DEBUG 1
#define	NEED_WARM_N_FUZZY	1

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
		// try to create it
		if( (fd = Creat(CTS_DB_FILE, 0666)) == ERROR ) {
			status = FILE_ERROR;
			goto Assign_Exit;				// we're done  :<
		}
		else
			close(fd);
	}

	// check to see if db file is memory mapped
	if( CTSdbFileIsMapped == FALSE ) {				// is not, so try
		if( map_data_file(CTS_DB) != SUCCESS ) {	// we're dead in the water
			status = MAP_ERROR;
			goto Assign_Exit;
		}
	}

	// get current db file count
	if( (numOfEntries = get_file_count(CTS_DB)) < 0 ) {
		status = FILE_ERROR;
		goto Assign_Exit;
	}

	if( numOfEntries ) {		// 1 or more
		if( lookup_entry(CTS_DB, log_name.pointer) >= 0 ) {	// duplicate !
			fprintf( stderr, "duplicate module name '%s' -- not allowed\n", log_name.pointer );
			status = DUPLICATE;
			goto Assign_Exit;
		}
	}

	// get db file size
	if( (dbFileSize = get_db_file_size(CTS_DB_FILE)) == ERROR ) {
		status = FILE_ERROR;
		goto Assign_Exit;
	}
	dbFileSize /= MODULE_ENTRY;	// .. current maximum number of possible module entries

	// do we need to expand db file?
	if( (dbFileSize == 0) || (numOfEntries == dbFileSize) ) {		// ... yes
		if( expand_db(CTS_DB, numOfEntries) != SUCCESS ) {			// expand ...
			status = EXPAND_ERROR;									// ... failure
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
		status = ASSIGN_ERROR;
		goto Assign_Exit;
	}

#if NEED_WARM_N_FUZZY
	// write to a buffer file for a warm fuzzy ...
	if( (fd = Creat("buffer.db", 0666)) == ERROR ) {
		perror("creat()");
		status = FILE_ERROR;
		goto Assign_Exit;
	}

	rc = write(fd, line, sizeof(line));
	close(fd);
	if( rc != sizeof(line) ) {
		perror("write()");
		status = FILE_ERROR;
	}
#endif

Assign_Exit:			// we're done, so out'a here!
	if( MSGLVL(9) ) {
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

	// check to see if db file exists
	if( check_for_file(CRATE_DB_FILE) != SUCCESS ) {		// does not exist
		fprintf(stderr, "crate.db does not exist\n");
		status = FILE_ERROR;
		goto AutoConfig_Exit;								// we're done  :<
	}

	// get current db file count
	if( (numOfEntries = get_file_count(CRATE_DB)) < 0 ) {	// nothing to do ...
		status = FILE_ERROR;
		goto AutoConfig_Exit;								// we're done  :<
	}
	if( MSGLVL(9) )
		printf("found %d entries\n", numOfEntries);

	// open file for read-only
	if( (fp = Fopen(CRATE_DB_FILE, "r")) == NULL ) {
		fprintf( stderr, "crate.db does not exist\n" );
		status = FILE_ERROR;
		goto AutoConfig_Exit;
	}

	pHighwayName = highway_name;	// point to real memory ...

	// loop thru list
	for( i = 0; i < numOfEntries; ++i ) {
		fscanf(fp, "%s", line);								// get a crate.db entry
		sprintf(pHighwayName, "%.6s", line);				// trim it

		if( MSGLVL(9) )
			printf("checking '%s'\n",pHighwayName);

		if( map_scsi_device(pHighwayName) != SUCCESS ) {	// map it if possible
			fprintf( stderr, "error mapping scsi devices\n" );
			status = ERROR;
			goto AutoConfig_Exit;
		}
	}
	// end of for()...

AutoConfig_Exit:
	return status;
} 

//-------------------------------------------------------------------------
// deassign a module
//-------------------------------------------------------------------------
int Deassign()
{
	char			db_tmp[CRATE_NAME_SIZE], line[MODULE_ENTRY], name[32], temp[32];
	int 			i, index, j, modules_to_deassign, modulesDeassigned, numOfEntries; 
	int				status = SUCCESS;
	struct Module_	Mod, *pMod;
	struct t_mod 	*element, *head, *tail;

	static DESCRIPTOR( physical, "PHYSICAL" );

	static DESCRIPTOR( modname_p, "MODULE" );
	static DYNAMIC_DESCRIPTOR( modname );

	pMod = &Mod;							// point to some actual storage
	element = head = tail = NULL;			// initialize list pointers

	// get user data
	cli_get_value( &modname_p, &modname );
	str_upcase( &modname, &modname );

	// make sure db file exists
	if( check_for_file(CTS_DB_FILE) != SUCCESS ) {		// failure
		fprintf( stderr, "db file '%s' does not exist\n", CTS_DB_FILE );
		status = FILE_ERROR;
		goto Deassign_Exit;
	}

	// check to see if db file memory mapped
	if( CTSdbFileIsMapped == FALSE ) {				// is not, so try
		if( map_data_file(CTS_DB) != SUCCESS ) {	// we're dead in the water
			status = MAP_ERROR;
			goto Deassign_Exit;
		}
	}

	// get number of current entries
	if( (numOfEntries = get_file_count(CTS_DB)) == 0 ) {	// no entries in cts db file
		fprintf( stderr, "db file empty, no entries to remove\n" );
		status = DEASSIGN_ERROR;
		goto Deassign_Exit;
	}

	modules_to_deassign = modulesDeassigned = 0;	// initialize counts

	// check if '/physical' switch present
	if( cli_present(&physical) & 1 ) {		// physical module name
		if( MSGLVL(9) )
			printf("Deassign(): physical module: <%s>\n", modname.pointer);

		// look up module name(s). NB! more than one logical name may be
		// assigned to the same, unique physical name.
		for( i = 0; i < numOfEntries; ++i ) {
			parse_cts_db( CTSdb+i, pMod );		// extract info from db

			sprintf( db_tmp, "GK%c%d%02d",
				pMod->adapter, 
				pMod->id, 
				pMod->crate 
				);

			if( strncmp(db_tmp, modname.pointer, CRATE_NAME_SIZE) == EQUAL ) {	// found a match
				if( MSGLVL(9) )
					printf("d(%2d): p->adapter <%c>  id <%d>  crate <%2d>  slot <%2d> ==> <%-.32s>\n",
						i+1, pMod->adapter, pMod->id, pMod->crate, pMod->slot, pMod->name
						);

				// allocate a list element
				if( (element = (struct t_mod *)malloc(sizeof(struct t_mod))) == NULL ) {
					perror("malloc()");
					status = NO_MEMORY;
					goto Deassign_Exit;
				}

				// trim and populate element members
				sprintf(temp, "%-.32s", pMod->name);
				strtok(temp," ");			// trim trailing spaces
				strcpy(element->mod_name,temp);
				element->next = NULL;		// NB! important to mark end of current list

				// adjust list pointers as appropriate
				if( head == NULL )
					tail = head = element;			// first element only
				else
					tail = tail->next = element;	// adjust pointers

				modules_to_deassign++;				// increment count

			} // ... end of search thru db file
		} // ... end of for() loop
	} // end of '/physical' processing
	else {									// logical module name
		if( MSGLVL(9) )
			printf("Deassign(): logical  module: <%s>\n", modname.pointer);

		// allocate a list element
		if( (element = (struct t_mod *)malloc(sizeof(struct t_mod))) == NULL ) {
			perror("malloc()");
			status = NO_MEMORY;
			goto Deassign_Exit;
		}

		// assign values to element members
		strcpy(element->mod_name, modname.pointer);
		element->next = NULL;				// NB! important to mark end of current list

		head = element;						// only one

		modules_to_deassign = 1;			// logical names are unique
	}

	// remove 'em
	if( modules_to_deassign ) {				// at least one
		element = head;						// start at the beginning
		for( i = 0; i < modules_to_deassign; ++i ) {
			if( (index = lookup_entry(CTS_DB, element->mod_name)) >= 0 ) {	// module does exist
				if( remove_entry(CTS_DB, index) != SUCCESS ) {				// removal failed
					status = DEASSIGN_ERROR;
					goto Deassign_Exit;
				}
			}
			else {							// no such module
				fprintf( stderr, "deassign(): entry '%s' not found\n", pMod->name);
				status = DEASSIGN_ERROR;
				goto Deassign_Exit;
			}

			modulesDeassigned++;			// successful
			element = element->next;		// point to next one
		}
	}
	else {
		fprintf( stderr, "deassign(): no entries found\n" );
		status = DEASSIGN_ERROR;
		goto Deassign_Exit;
	}

	if( MSGLVL(9) ) {
		if(modules_to_deassign) {
		printf("+++++++++++++++++++++++++++\n");
			for(element=head; element!= NULL; element=element->next) {
				printf("mod name: '%s'\n", element->mod_name);
			}
		printf("+++++++++++++++++++++++++++\n");
		}
	}

Deassign_Exit:
	// free allocated memory
	element = head;
	while( element != NULL) {
		tail = element->next;
		free(element);
		element = tail;
	}

	if( MSGLVL(9) ) {
		printf("Deassign(%d): ", modulesDeassigned); ShowStatus(status);
	}
	if( modulesDeassigned != modules_to_deassign )	// not all modules deassigned
		status = DEASSIGN_ERROR;
	else
		if( MSGLVL(9) )
			printf("all entries(%d) deassigned !!!\n", modulesDeassigned);

	return status;
}  

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
int Noop()
{ 
	if( MSGLVL(9) )
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

	if( MSGLVL(9) )
		printf("ResetHighway() invoked -- not implemented\n");

	return SUCCESS;
}

//-------------------------------------------------------------------------
// set a crate on-line or off-line
//-------------------------------------------------------------------------
int SetCrate()
{
	int status;

	static DESCRIPTOR( crate_p, "CRATE" );
	static DESCRIPTOR( quiet, "QUIET" );

	static DYNAMIC_DESCRIPTOR( wild );
	static DYNAMIC_DESCRIPTOR( crate );

	static DESCRIPTOR( offq, "OFFLINE" );
	static DESCRIPTOR( onq, "ONLINE" );

	int off = cli_present(&offq) & 1;
	int on  = cli_present(&onq ) & 1;

	cli_get_value( &crate_p, &wild );
	str_upcase( &wild, &wild );

	// check to see if db file exists
	if( check_for_file(CRATE_DB_FILE) != SUCCESS ) {	// does not exist -- nothing to show
		fprintf(stderr, "crate.db does not exist\n");
		status = FILE_ERROR;
		goto SetCrate_Exit;								// we're done  :<
	}

	// check to see if db file memory mapped
	if( CRATEdbFileIsMapped == FALSE ) {			// is not, so try
		if( map_data_file(CRATE_DB) != SUCCESS ) {	// we're dead in the water
			fprintf( stderr, "error memory mapping crate db file\n" );
			status = MAP_ERROR;
			goto SetCrate_Exit;
		}
	}

	status = turn_crate_on_off_line( wild.pointer, 1 );

SetCrate_Exit:
	return SUCCESS;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
#define GREEN	2
#define	RED		1
int ShowCrate()
{
  char			colorENH[9], colorON[9];//, NORMAL[] = "\033[0m";
	char tmp[7];
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

	// user input
	cli_get_value( &crate_p, &wild );
	str_upcase( &wild, &wild );

	// check crate.db status ...
	// check to see if crate db file exists
	if( check_for_file(CRATE_DB_FILE) != SUCCESS ) {	// does not exist -- nothing to show
		fprintf(stderr, "crate.db does not exist\n");
		status = FILE_ERROR;
		goto ShowCrate_Exit;							// we're done  :<
	}

	// check to see if crate db file memory mapped
	if( CRATEdbFileIsMapped == FALSE ) {				// is not, so try
		if( map_data_file(CRATE_DB) != SUCCESS ) {		// we're dead in the water
			fprintf( stderr, "error memory mapping crate.db file\n" );
			status = MAP_ERROR;
			goto ShowCrate_Exit;
		}
	}

	// check cts.db status ...
	// check to see if module db file exists
	if( check_for_file(CTS_DB_FILE) != SUCCESS ) {		// does not exist -- nothing to show
		fprintf(stderr, "cts.db does not exist\n");
		status = FILE_ERROR;
		goto ShowCrate_Exit;							// we're done  :<
	}

	// check to see if module db file memory mapped
	if( CTSdbFileIsMapped == FALSE ) {					// is not, so try
		if( map_data_file(CTS_DB) != SUCCESS ) {		// we're dead in the water
			fprintf( stderr, "error memory mapping cts.db file\n" );
			status = MAP_ERROR;
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
				if( wildcard_match( wild.pointer, pCr8->name, 0,0,0 ) ) {
					moduleFound = FALSE;

					for( j = 0; j < numOfModules; ++j ) {
						parse_cts_db(CTSdb+j, pMod);
						sprintf(tmp, "GK%c%d%02d",
							pMod->adapter,
							pMod->id,
							pMod->crate
							);

						if( strcmp(tmp, pCr8->name) == EQUAL ) {
							if( pMod->slot == 30 ) {	// found a crate controller
								moduleFound = TRUE;
								break;
							}
						}
					} // end of for(modules) ...


					if( moduleFound ) {
						if( ( status = get_crate_status(pCr8->name, &crateStatus) ) == 0 ) {
							online = !(crateStatus & 0x3c00)              ? TRUE  : FALSE;
							sprintf(colorON,  "\033[01;3%dm", (online)    ? GREEN : RED);

							enhanced = (online && (crateStatus & 0x4030)) ? TRUE  : FALSE;
							sprintf(colorENH, "\033[01;3%dm", (enhanced)  ? GREEN : RED);

							if( 1 )
							printf("%s:   %s%c%s   .   .   %s%c%s\n",
								pCr8->name,
								colorON,  (online)   ? '*' : 'X', NORMAL,
								colorENH, (enhanced) ? '*' : '-', NORMAL
								);
#if 0
printf("SC(): pCrateStatus 0x%x\n",crateStatus);
#endif
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
	char	db_tmp[CRATE_NAME_SIZE];
	int 	i, numOfEntries, status = SUCCESS;

	static DESCRIPTOR( physical, "PHYSICAL" );
	static DESCRIPTOR( module_p, "MODULE" );
	static DESCRIPTOR( format_p, "FORMAT" );
	static DESCRIPTOR( blank, " " );

	static DESCRIPTOR( heading1, "  #  Logical Name                     Physical   Comment" );	// header
	static DESCRIPTOR( heading2, "==== ================================ ========== ========================================" );

	static DYNAMIC_DESCRIPTOR( wild );

	struct Module_	Mod, *pMod;

	int format = cli_present(&format_p) & 1;

	cli_get_value( &module_p, &wild );

	if( cli_present(&physical) & 1 ) {	// display in physical module order
printf("ShowModule(): in physical order\n");
	}
	else {								// display in logical module order
printf("ShowModule(): in logical order\n");
	}

	// check to see if db file memory mapped
	if( CTSdbFileIsMapped == FALSE ) {				// is not, so try
		if( map_data_file(CTS_DB) != SUCCESS ) {	// we're dead in the water
			fprintf( stderr, "error memory mapping database file\n" );
			status = MAP_ERROR;
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

			sprintf( db_tmp, "GK%c%d%02d", 
				pMod->adapter,
				pMod->id,
				pMod->crate
				);

			if( wildcard_match( wild.pointer, db_tmp, 0,0,0 ) ) {
				printf( "%3d: %.84s<\n", i+1, (char *)CTSdb+(i * MODULE_ENTRY) );
			}
		}

		printf( "%s\n", heading2.pointer );
	}
	else {
		fprintf( stderr, "db file is empty, no modules to show\n" );
		status = ERROR;
		goto Showmodule_Exit;
	}

Showmodule_Exit:
#ifdef DEBUG
printf("ShowModule(): "); ShowStatus(status);
#endif

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

	// check to see if db file exists
	if( check_for_file(CRATE_DB_FILE) != SUCCESS ) {	// does not exist, yet
		// try to create it
		if( (fd = Creat(CRATE_DB_FILE, 0666)) == ERROR ) {
			status = FILE_ERROR;
			goto AddCrate_Exit;				// we're done  :<
		}
		else
			close(fd);
	}

	// check to see if db file is memory mapped
	if( CRATEdbFileIsMapped == FALSE ) {				// is not, so try
		if( map_data_file(CRATE_DB) != SUCCESS ) {		// we're dead in the water
			status = MAP_ERROR;
			goto AddCrate_Exit;
		}
	}

	// get current db file count
	if( (numOfEntries = get_file_count(CRATE_DB)) == FILE_ERROR ) {
		status = FILE_ERROR;
		goto AddCrate_Exit;
	}

	if( numOfEntries ) {		// 1 or more
		if( lookup_entry(CRATE_DB, phy_name.pointer) >= 0 ) {			// duplicate !
			fprintf( stderr, "duplicate crate name '%.6s' -- not allowed\n", phy_name.pointer );
			status = DUPLICATE;
			goto AddCrate_Exit;
		}
	}

	// get db file size
	if( (dbFileSize = get_db_file_size(CRATE_DB_FILE)) == ERROR ) {
		status = FILE_ERROR;
		goto AddCrate_Exit;
	}
	dbFileSize /= CRATE_ENTRY;	// .. current maximum number of possible crate entries

	// do we need to expand db file?
	if( (dbFileSize == 0) || (numOfEntries == dbFileSize) ) {				// ... yes
		if( (status = expand_db(CRATE_DB, numOfEntries)) != SUCCESS ) {		// expand
			status = EXPAND_ERROR;											// failure
			goto AddCrate_Exit;
		}
	}		// else OK

	// make an entry line, with online and enhanced set as undefined
	sprintf( line, "%-.6s:...:.\n",
		phy_name.pointer
		);

	// add it ...
	if( (status = add_entry(CRATE_DB, line)) != SUCCESS ) {
		status = ASSIGN_ERROR;
		goto AddCrate_Exit;
	}

AddCrate_Exit:
#ifdef DEBUG
printf("AddCrate(): "); ShowStatus(status);
#endif

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

	// make sure db file exists
	if( check_for_file(CRATE_DB_FILE) != SUCCESS ) {	// failure
		fprintf( stderr, "db file '%s' does not exist\n", CRATE_DB_FILE );
		status = FILE_ERROR;
		goto DelCrate_Exit;
	}

	// check to see if db file memory mapped
	if( CRATEdbFileIsMapped == FALSE ) {			// is not, so try
		if( map_data_file(CRATE_DB) != SUCCESS ) {	// we're dead in the water
			status = MAP_ERROR;
			goto DelCrate_Exit;
		}
	}

	// get number of current entries
	if( (numOfEntries = get_file_count(CRATE_DB)) == 0 ) {	// no entries in crate db file
		fprintf( stderr, "db file empty, no entries to remove\n" );
		status = DELCRATE_ERROR;
		goto DelCrate_Exit;
	}

	// try to remove from crate.db
	if( (index = lookup_entry(CRATE_DB, crateName)) >= 0 ) {	// module does exist
		if( remove_entry(CRATE_DB, index) != SUCCESS ) {		// removal failed
			status = DELCRATE_ERROR;
			goto DelCrate_Exit;
		}
	}
	else {		// no such module
		fprintf( stderr, "delcrate(): entry '%s' not found\n", crateName);
		status = DELCRATE_ERROR;
		goto DelCrate_Exit;
	}

DelCrate_Exit:
#ifdef DEBUG
printf("DelCrate() status = %d\n", status);
#endif
	return SUCCESS;
}

//-------------------------------------------------------------------------
// print a help menu for cts functions
//-------------------------------------------------------------------------
int Help()
{
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
