/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
// verbs.c
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

//-------------------------------------------------------------------------
// Thu Apr 12 15:01:47 EDT 2001 -- added memory map support for setcrate cmd
// Wed Sep  5 11:39:41 EDT 2001 -- added dynamic debug printout control
// Thu Sep 13 16:01:19 EDT 2001 -- fixed 'show/crate' code - now works with
//                                                                      power turned off or non-existant highway
// Fri Nov  2 15:56:27 EST 2001 -- 'cleanup'
// Fri Jan 25 15:56:16 EST 2002 -- fixed 'show' using wildcard characters
// Wed Feb 20 12:46:12 EST 2002 -- 'removed' memory leak
// Fri Feb 22 10:56:55 EST 2002 -- restructured 'deassign()'
// Thu Dec  5 12:32:50 EST 2002 -- make call to 'Autoconfig()' after Addcrate
// Mon Dec  9 15:51:05 EST 2002 -- 'ShowCrate()' fixed crate status
//-------------------------------------------------------------------------

#define	NEED_WARM_N_FUZZY	0

//-------------------------------------------------------------------------
// include files
//-------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dcl.h>
#include <mdsdescrip.h>
#include <strroutines.h>

#include <math.h>

#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <errno.h>

#include "common.h"
#include "prototypes.h"
#include "cts_p.h"

//-------------------------------------------------------------------------
// global stuff
//-------------------------------------------------------------------------
char nullStr[] = "(null)";
extern int CTSdbFileIsMapped;
extern int CRATEdbFileIsMapped;
extern struct MODULE *CTSdb;	// pointer to in-memory copy of data file
extern struct CRATE *CRATEdb;

//-------------------------------------------------------------------------
// local things
//-------------------------------------------------------------------------
#define CRATE_NAME_SIZE		sizeof(struct CRATE_NAME)

//-------------------------------------------------------------------------
// function prototypes
//-------------------------------------------------------------------------
extern int cli_get_value();
extern int cli_present();

//-------------------------------------------------------------------------
// helper routine
//-------------------------------------------------------------------------
static void str_upcase(char *str)
{
  size_t i;

  for (i = 0; i < strlen(str); i++)
    str[i] = toupper(str[i]);
}

//-------------------------------------------------------------------------
// assign a new module to CTS database
//-------------------------------------------------------------------------
EXPORT int Assign(void *ctx, char **error, char *output __attribute__ ((unused)))
{
  char line[MODULE_ENTRY + 1];
  int dbFileSize, fd, nullMask, numOfEntries;
  size_t i;
  int status = SUCCESS;		// assume the best
  char *phy_name = 0;
  char *log_name = 0;
  char *comment = 0;

  // get user input
  cli_get_value(ctx, "PHY_NAME", &phy_name);
  str_upcase(phy_name);
  cli_get_value(ctx, "LOG_NAME", &log_name);
  str_upcase(log_name);
  cli_get_value(ctx, "COMMENT", &comment);

  // check to see if db file exists
  if (check_for_file(CTS_DB_FILE) != SUCCESS) {	// does not exist, yet
    // try to creat (sic) it
    if ((fd = Creat(CTS_DB_FILE, 0666)) == ERROR) {
      *error = strdup("Error: cts db file does not exist and error attempting to create one.\n");
      status = FAILURE;		// FILE_ERROR;          [2001.07.12]
      goto Assign_Exit;		// we're done  :<
    } else
      close(fd);
  }
  // check to see if db file is memory mapped
  if (CTSdbFileIsMapped == FALSE) {	// is not, so try
    if (map_data_file(CTS_DB) != SUCCESS) {	// we're dead in the water
      *error = strdup("Error: problem mapping cts db file\n");
      status = FAILURE;		// MAP_ERROR;           [2001.07.12]
      goto Assign_Exit;
    }
  }
  // get current db file count
  if ((numOfEntries = get_file_count(CTS_DB)) < 0) {
    *error = strdup("Error: Error obtaining number of entries. cts db file corrupted?\n");
    status = FAILURE;		// FILE_ERROR;          [2001.07.12]
    goto Assign_Exit;
  }

  if (numOfEntries) {		// 1 or more
    if (lookup_entry(CTS_DB, log_name) >= 0) {	// duplicate !
      *error = malloc(strlen(log_name)+100);
      sprintf(*error, "Error: duplicate module name '%s' -- not allowed\n", log_name);
      
      status = FAILURE;		// DUPLICATE;           [2001.07.12]
      goto Assign_Exit;
    }
  }
  // get db file size
  if ((dbFileSize = get_db_file_size(CTS_DB_FILE)) == ERROR) {
    *error = strdup("Error: problem determining cts db file size. cts db file corrupted?\n");
    status = FAILURE;		// FILE_ERROR;          [2001.07.12]
    goto Assign_Exit;
  }
  dbFileSize /= MODULE_ENTRY;	// .. current maximum number of possible module entries

  // do we need to expand db file?
  if ((dbFileSize == 0) || (numOfEntries == dbFileSize)) {	// ... yes
    if (expand_db(CTS_DB, numOfEntries) != SUCCESS) {	// expand ...
      *error = strdup("Error: problem expanding cts db file\n");
      status = FAILURE;		// EXPAND_ERROR; [2001.07.12]   // ... failure
      goto Assign_Exit;
    }
  }				// else OK

  // create a temporary string
  sprintf(line, "%-32s %-10s %-40s\n", log_name,	// these were entered by the user
	  phy_name, comment ? comment : "");

  // check comment field for null string, ie "(null)"
  nullMask = (1 << strlen(nullStr)) - 1;	// set all mask bits
  for (i = COMMENT_INDEX; i < COMMENT_INDEX + strlen(nullStr); ++i)
    if (line[i] == nullStr[i - COMMENT_INDEX])
      nullMask &= ~(1 << (i - COMMENT_INDEX));	// clear a bit in mask

  if (nullMask == 0)		// all mask bit have been reset, ie matched
    for (i = COMMENT_INDEX; i < COMMENT_INDEX + strlen(nullStr); ++i)
      line[i] = ' ';		// make it spaces

  // add it ...
  if (add_entry(CTS_DB, line) != SUCCESS) {
    *error = strdup("Error: problem adding new line to cts db file\n");
    status = FAILURE;		// ASSIGN_ERROR;                [2001.07.12]
    goto Assign_Exit;
  }
#if NEED_WARM_N_FUZZY
  // write to a buffer file for a warm fuzzy ...
  if ((fd = Creat("buffer.db", 0666)) == ERROR) {
    *error = strdup("Error: problem creating buffer file\n");
    perror("creat()");

    status = FAILURE;		// FILE_ERROR;                  [2001.07.12]
    goto Assign_Exit;
  }

  rc = write(fd, line, sizeof(line));
  close(fd);
  if (rc != sizeof(line)) {
    perror("write()");
    *error = strdup("Error: problem writing to buffer file\n");
    status = FAILURE;		// FILE_ERROR;                  [2001.07.12]
  }
#endif

 Assign_Exit:			// we're done, so out'a here!
  if (phy_name)
    free(phy_name);
  if (log_name)
    free(log_name);
  if (comment)
    free(comment);
  return status;
}

//-------------------------------------------------------------------------
// map generic scsi device names to crate table names 
//-------------------------------------------------------------------------
EXPORT int Autoconfig(void *ctx __attribute__ ((unused)), char **error, char **output __attribute__ ((unused)))
{
  char highway_name[CRATE_NAME_SIZE + 1], *pHighwayName;
  char line[CRATE_ENTRY];
  int i, numOfEntries;
  int status = SUCCESS;		// optimistic
  FILE *fp;

  int j;

  // check to see if db file memory mapped
  if (CRATEdbFileIsMapped == FALSE) {	// is not, so try
    if (map_data_file(CRATE_DB) != SUCCESS) {	// we're dead in the water
      *error = strdup("Error: problem mapping crate db file\n");
      status = MAP_ERROR;
      goto AutoConfig_Exit;	// we're done  :<
    }
  }
  // get current db file count
  if ((numOfEntries = get_file_count(CRATE_DB)) <= 0) {	// nothing to do ...
    status = FILE_ERROR;
    goto AutoConfig_Exit;	// we're done  :<
  }
  // open file for read-only
  if ((fp = Fopen(CRATE_DB_FILE, "r")) == NULL) {
    *error = strdup("Error: crate.db does not exist\n");

    status = FILE_ERROR;
    goto AutoConfig_Exit;
  }

  pHighwayName = highway_name;	// point to real memory ...

  // loop thru list
  for (i = 0; i < numOfEntries; ++i) {
    fscanf(fp, "%s", line);	// get a crate.db entry
    sprintf(pHighwayName, "%.6s", line);	// trim it

    // NB! this is a work-around -- seems necessary for the moment
    for (j = 0; j < 2; j++) {
      if (map_scsi_device(pHighwayName) != SUCCESS) {	// map it if possible
	  *error = malloc(strlen(pHighwayName)+100);
	  sprintf(*error, "Error: problem mapping scsi device '%s'\n", pHighwayName);
	  status = FILE_ERROR;
	  goto AutoConfig_Exit;
      }
    }

  }
  // end of for()...

 AutoConfig_Exit:
  status = SUCCESS;

  return status;
}

//-------------------------------------------------------------------------
// deassign a module
//-------------------------------------------------------------------------
EXPORT int Deassign(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  char db_tmp[64];
  int i, modulesToDeassign, modulesDeassigned, numOfEntries, physical_name;
  int status = SUCCESS;
  struct Module_ Mod, *pMod;
  char *wild = 0;
  struct descriptor pattern = {0, DTYPE_T, CLASS_S, 0};
  struct descriptor wild_d = {0, DTYPE_T, CLASS_S, 0};

  // get user data
  cli_get_value(ctx, "MODULE", &wild);
  wild_d.pointer = wild;
  wild_d.length = strlen(wild);
  StrUpcase(&wild_d, &wild_d);

  physical_name = cli_present(ctx, "PHYSICAL") & 1;

  // check to see if db file memory mapped
  if (CTSdbFileIsMapped == FALSE) {	// is not, so try ...
    if (map_data_file(CTS_DB) != SUCCESS) {	// we're dead in the water
      status = MAP_ERROR;	// [2002.02.21]
      goto Deassign_Exit;
    }
  }
  // get number of current entries
  if ((numOfEntries = get_file_count(CTS_DB)) == 0) {	// no entries in cts db file
    *error = strdup("Error: db file empty, no entries to remove\n");

    status = DEASSIGN_ERROR;	// [2002.02.21]
    goto Deassign_Exit;
  }
  // if we get this far, then there are modules in the database
  modulesToDeassign = modulesDeassigned = 0;	// initialize counts
  pMod = &Mod;			// point to some actual storage

  // first, we need to count the number to deassign ...
  for (i = 0; i < numOfEntries; ++i) {	// scan entire list
    // look up module name(s). NB! more than one logical name may be
    // assigned to the same, unique physical name.
    parse_cts_db(CTSdb + i, pMod);	// extract info from db

    memset(db_tmp, ' ', 32);	// clear out buffer
    db_tmp[33] = '\0';		// ensure buffer 'ends'

    if (physical_name)		// physical name
      sprintf(db_tmp, "GK%c%d%02d:N%d", pMod->adapter + 'A', pMod->id, pMod->crate, pMod->slot);
    else			// logical name 
      sprintf(db_tmp, "%s", pMod->name);

    // prepare for 'wild' match
    pattern.pointer = db_tmp;
    pattern.length = strlen(db_tmp);

    if (StrMatchWild(&pattern, &wild_d) & 1) {
      ++modulesToDeassign;	//
    }
  }				// end of for() loop, all entries checked

  // now actually remove them
  while (modulesToDeassign) {
    for (i = 0; i < get_file_count(CTS_DB); ++i) {
      // look up module name(s). NB! more than one logical name may be
      // assigned to the same, unique physical name.
      parse_cts_db(CTSdb + i, pMod);	// extract info from db

      memset(db_tmp, ' ', 32);	// clear out buffer
      db_tmp[33] = '\0';	// ensure buffer 'ends'

      if (physical_name)	// physical name
	sprintf(db_tmp, "GK%c%d%02d:N%d", pMod->adapter + 'A', pMod->id, pMod->crate, pMod->slot);
      else			// logical name 
	sprintf(db_tmp, "%s", pMod->name);

      // prepare for 'wild' match
      pattern.pointer = db_tmp;
      pattern.length = strlen(db_tmp);
      

      if (StrMatchWild(&pattern, &wild_d) & 1) {
	if (remove_entry(CTS_DB, i) != SUCCESS) {	// removal failed
	  status = DEASSIGN_ERROR;	// [2002.02.21]
	  goto Deassign_Exit;
	} else
	  ++modulesDeassigned;	// keep track of successes :)
      }
    }

    --modulesToDeassign;	// one less to remove ...
  }

 Deassign_Exit:
  if (wild)
    free(wild);

  return status;
  }

//-------------------------------------------------------------------------
// set a crate on-line or off-line
//-------------------------------------------------------------------------
EXPORT int SetCrate(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int status;
  char *cratename;
  void *ctx2 = 0;
  char *wild = 0;

  int on = cli_present(ctx, "ONLINE") & 1;
  int quiet = cli_present(ctx, "QUIET") & 1;

  cli_get_value(ctx, "CRATE", &wild);
  str_upcase(wild);

  // check to see if db file memory mapped
  if (CRATEdbFileIsMapped == FALSE) {	// is not, so try
    if (map_data_file(CRATE_DB) != SUCCESS) {	// we're dead in the water
      *error = strdup("Error: problem mapping crate db file\n");

      status = FAILURE;		// MAP_ERROR;           [2001.07.12]
      goto SetCrate_Exit;
    }
  }

  while (find_crate(wild, &cratename, &ctx2)) {

    status = turn_crate_on_off_line(cratename, (on) ? ON : OFF);
    if (!(status & 1) && !quiet) {
      if (*error == NULL)
	*error = strdup("");
      *error = realloc(*error,strlen(*error)+strlen(cratename)+100);
      sprintf(*error+strlen(*error),"Error: problem turning crate %s %s\n",
	      cratename, on ? "online" : "offline");
    }
    free(cratename);
  }
  find_crate_end(&ctx2);
 SetCrate_Exit:
  if (wild)
    free(wild);
  return SUCCESS;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// show crate status, using crates in 'crate.db' file
//-------------------------------------------------------------------------
EXPORT int ShowCrate(void *ctx, char **error, char **output)
{
  char colorENH[9], colorON[9];
  int enhanced, i, online, moduleFound, numOfCrates, numOfModules;
  int crateStatus;
  int status;
  struct Crate_ Cr8, *pCr8;
  char *wild = 0;
  struct descriptor wild_d = {0, DTYPE_T, CLASS_S, 0};
  struct descriptor crate_d = {0, DTYPE_T, CLASS_S, 0};

  if (ScsiSystemStatus() == 0) {
    status = SUCCESS;		// this is the function's status
    *output = malloc(100);
    sprintf(*output,"scsi system is %sdown!%s\n", RED, NORMAL);
    goto ShowCrate_Exit;
  }

  // user input
  cli_get_value(ctx, "MODULE", &wild);
  wild_d.length=strlen(wild);
  wild_d.pointer = wild;
  str_upcase(wild);

  // check to see if crate db file memory mapped
  if (CRATEdbFileIsMapped == FALSE) {	// is not, so try
    if (map_data_file(CRATE_DB) != SUCCESS) {	// we're dead in the water
	*error = strdup("Error: problem memory mapping crate.db file\n");

      status = FAILURE;		// MAP_ERROR;           [2001.07.12]
      goto ShowCrate_Exit;
    }
  }
  // check to see if module db file memory mapped
  if (CTSdbFileIsMapped == FALSE) {	// is not, so try
    if (map_data_file(CTS_DB) != SUCCESS) {	// we're dead in the water
	*error = strdup("Error: error memory mapping cts.db file\n");

      status = FAILURE;		// MAP_ERROR;           [2001.07.12]
      goto ShowCrate_Exit;
    }
  }
  *output = strdup(" CRATE   ONL LAM PRV ENH\n=======  === === === ===\n");

  pCr8 = &Cr8;			// point to some actual storage

  // get number of crates in db file
  if ((numOfCrates = get_file_count(CRATE_DB)) > 0) {	// possibly something to show
    if ((numOfModules = get_file_count(CTS_DB)) > 0) {	// maybe some crates controllers ..
      for (i = 0; i < numOfCrates; i++) {
	parse_crate_db(CRATEdb + i, pCr8);
	crate_d.length = strlen(pCr8->name);
	crate_d.pointer = pCr8->name;
	if (StrMatchWild(&crate_d,&wild_d)&1) {
	  moduleFound = TRUE;
	  if (moduleFound) {
	    crateStatus = 0;
	    status = get_crate_status(pCr8->name, &crateStatus);
	    if (status == SUCCESS) {
	      //                                                        online = !(crateStatus & 0x3c00)    ? TRUE  : FALSE;                            // [2002.12.09]
	      //                                                        online = !(crateStatus & 0x1000)    ? TRUE  : FALSE;                            // [2002.12.09]
	      online = ((crateStatus & 0x1000) != 0x1000) ? TRUE : FALSE;	// [2002.12.09]
	      if (!crateStatus || crateStatus == 0x3)	// [2001.09.10]                 // [2002.12.09]
		online = FALSE;	// [2002.12.09]
	      sprintf(colorON, "%s", (online) ? GREEN : RED);

	      //                                                        enhanced = (online && (crateStatus & 0x4030)) ? TRUE  : FALSE;          // [2002.12.09]
	      enhanced = (online && (crateStatus & 0x4000)) ? TRUE : FALSE;	// [2002.12.09]
	      sprintf(colorENH, "%s", (enhanced) ? GREEN : RED);
	      *output = realloc(*output,strlen(*output)+strlen(pCr8->name) + 100);
	      sprintf(*output + strlen(*output),"%s:   %s%c%s   .   .   %s%c%s\n",
		     pCr8->name,
		     colorON, (online) ? '*' : 'X', NORMAL,
		     colorENH, (enhanced) ? '*' : '-', NORMAL);
	    }
	  }			// end of if(moduleFound) ...
	  else {
	    *output = realloc(*output,strlen(*output)+strlen(pCr8->name) + 100);
	    sprintf(*output+strlen(*output),"%.6s:   .   .   .   .\n", pCr8->name);
	  }
	}			// end of if(wildcard) ...
      }				// end of for(crates) ...
    }				// crates, but no modules (ie no controllers)
  }
  *output = realloc(*output,strlen(*output)+100);
  sprintf(*output+strlen(*output),"=======  === === === ===\n");	// header

  status = SUCCESS;		// always (???)

 ShowCrate_Exit:
  if (wild)
    free(wild);
  return status;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
EXPORT int ShowModule(void *ctx, char **error, char **output)
{
  char db_tmp[64];		// enough space for a logical name and a cstring terminator     [2002.02.20]
  int i, numOfEntries, status = SUCCESS;


  static const char *heading1 =
    "  #  Logical Name                     Physical   Comment";
  static const char *heading2 =
    "==== ================================ ========== ========================================";

  char *wild = 0;
  struct descriptor wild_d = {0, DTYPE_T, CLASS_S, 0};
  struct Module_ Mod, *pMod;

  struct descriptor pattern;

  int physical_name = cli_present(ctx, "PHYSICAL") & 1;	// 2002.01.16

  cli_get_value(ctx, "MODULE", &wild);
  wild_d.pointer = wild;
  wild_d.length = strlen(wild);
  str_upcase(wild);	// convert to upper case

  // check to see if db file memory mapped
  if (CTSdbFileIsMapped == FALSE) {	// is not, so try
    if (map_data_file(CTS_DB) != SUCCESS) {	// we're dead in the water
	*error = strdup("Error: error memory mapping database file\n");

      status = FAILURE;		// MAP_ERROR;           [2001.07.12]
      goto Showmodule_Exit;
    }
  }
  *output = strdup("");
  // db file now mapped, continue
  pMod = &Mod;			// point to some actual storage
  if ((numOfEntries = get_file_count(CTS_DB)) > 0) {	// something to show
    *output=realloc(*output,strlen(*output)+strlen(heading1)+strlen(heading2)+100);
    sprintf(*output+strlen(*output),"%s\n%s%d\n", heading1,heading2, numOfEntries);
    for (i = 0; i < numOfEntries; i++) {
      parse_cts_db(CTSdb + i, pMod);	// extract info from db

      memset(db_tmp, ' ', 32);
      db_tmp[33] = '\0';

      if (physical_name)	// duh, physical name
	sprintf(db_tmp, "GK%c%d%02d:N%d", pMod->adapter + 'A', pMod->id, pMod->crate, pMod->slot);
      else
	sprintf(db_tmp, "%s", pMod->name);

      pattern.pointer = db_tmp;
      pattern.length = strlen(db_tmp);

      if (StrMatchWild(&pattern, &wild_d) & 1) {
	*output = realloc(*output,strlen(*output)+100);
	sprintf(*output+strlen(*output),"%3d: %.84s<\n", i + 1, (char *)CTSdb + (i * MODULE_ENTRY));
      }

    }				// end of for() loop
    *output=realloc(*output,strlen(*output)+strlen(heading2)+10);
    sprintf(*output + strlen(*output),"%s\n", heading2);
  } else {
    *error = strdup("Error: db file is empty, no modules to show\n");
    status = FAILURE;		// Not necessarily an ERROR;    [2002.02.19]
    goto Showmodule_Exit;
  }

 Showmodule_Exit:
  if (wild)
    free(wild);
  return status;
}

//-------------------------------------------------------------------------
// Add a crate to the crate db
//-------------------------------------------------------------------------
EXPORT int AddCrate(void *ctx, char **error, char **output)
{
  char line[CRATE_ENTRY + 1];
  int dbFileSize, fd, numOfEntries;
  int status = SUCCESS;		// assume the best

  char *phy_name = 0;

  // get user input
  cli_get_value(ctx, "PHY_NAME", &phy_name);
  str_upcase(phy_name);

  // [2002.01.08]
  if (CRATEdbFileIsMapped == FALSE) {	// ... no
    if (check_for_file(CRATE_DB_FILE) != SUCCESS) {	// ... no
      if ((fd = Creat(CRATE_DB_FILE, 0666)) == ERROR) {	// no
	status = FAILURE;
	*error = strdup("Error: Unable to open crate db file\n");
	goto AddCrate_Exit;
      } else
	close(fd);		// yes
    }

    if (map_data_file(CRATE_DB) != SUCCESS) {	// failure :(
      *error = strdup("Error: Unable to map crate db file\n");
      status = MAP_ERROR;
      goto AddCrate_Exit;
    }				// else OK :)
  }
  // get current db file count
  if ((numOfEntries = get_file_count(CRATE_DB)) == FILE_ERROR) {
    *error = strdup("Error: problem getting number of entries in crate db file\n");
    status = FAILURE;		// FILE_ERROR;          [2001.07.12]
    goto AddCrate_Exit;
  }

  if (numOfEntries) {		// 1 or more
    char pname[7];
    sprintf(pname, "%.6s", phy_name);
    if (lookup_entry(CRATE_DB, pname) >= 0) {	// duplicate !
      *error = malloc(strlen(phy_name)+100);
      sprintf(*error, "Error: duplicate crate name '%.6s' -- not allowed\n", phy_name);
      status = FAILURE;		// DUPLICATE;           [2001.07.12]
      goto AddCrate_Exit;
    }
  }
  // get db file size
  if ((dbFileSize = get_db_file_size(CRATE_DB_FILE)) == ERROR) {
    *error = strdup("Error: problem getting crate db file size\n");
    status = FAILURE;		// FILE_ERROR;          [2001.07.12]
    goto AddCrate_Exit;
  }
  dbFileSize /= CRATE_ENTRY;	// .. current maximum number of possible crate entries

  // do we need to expand db file?
  if ((dbFileSize == 0) || (numOfEntries == dbFileSize)) {	// ... yes
    if ((status = expand_db(CRATE_DB, numOfEntries)) != SUCCESS) {	// expand
      *error = strdup("Error: problem expanding crate db file\n");
      status = FAILURE;		// EXPAND_ERROR; [2001.07.12]                                                                                   // failure
      goto AddCrate_Exit;
    }
  }				// else OK

  // make an entry line, with online and enhanced set as undefined
  sprintf(line, "%-.6s:...:.:.:.\n", phy_name);

  // add it ...
  if ((status = add_entry(CRATE_DB, line)) != SUCCESS) {
    *error = strdup("Error: problem adding entry to crate db file\n");
    status = FAILURE;		// ASSIGN_ERROR;                [2001.07.12]
    goto AddCrate_Exit;
  }

 AddCrate_Exit:
  if (status == SUCCESS)	// if everything is OK ...
    Autoconfig(ctx,error,output);		// ... map crate to /dev/sg#

  if (phy_name)
    free(phy_name);
//ShowCrate();
  return status;
}

//-------------------------------------------------------------------------
// Delete a crate from the crate db
//-------------------------------------------------------------------------
EXPORT int DelCrate(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  char crateName[CRATE_NAME_SIZE + 1];
  int index, numOfEntries;
  int status = SUCCESS;

  char *phy_name = 0;
  // get user input
  cli_get_value(ctx, "PHY_NAME", &phy_name);
  str_upcase(phy_name);
  // trim it...
  sprintf(crateName, "%.6s", phy_name);

  // check to see if db file memory mapped
  if (CRATEdbFileIsMapped == FALSE) {	// is not, so try
    if (map_data_file(CRATE_DB) != SUCCESS) {	// we're dead in the water
      *error = strdup("Error: Unable to map crate db file\n");
      status = FAILURE;		// MAP_ERROR;           [2001.07.12]
      goto DelCrate_Exit;
    }
  }
  // get number of current entries
  if ((numOfEntries = get_file_count(CRATE_DB)) == 0) {	// no entries in crate db file
    *error = strdup("Error: db file empty, no entries to remove\n");
    status = FAILURE;		// DELCRATE_ERROR;              [2001.07.12]
    goto DelCrate_Exit;
  }
  // try to remove from crate.db
  if ((index = lookup_entry(CRATE_DB, crateName)) >= 0) {	// module does exist
    if (remove_entry(CRATE_DB, index) != SUCCESS) {	// removal failed
      *error = strdup("Error: problem removing entry from crate db\n");
      status = FAILURE;		// DELCRATE_ERROR;              [2001.07.12]
      goto DelCrate_Exit;
    }
  } else {			// no such module
    *error = malloc(strlen(crateName)+100);
    sprintf(*error, "Error: entry '%s' not found\n", crateName);
    status = FAILURE;		// DELCRATE_ERROR;              [2001.07.12]
    goto DelCrate_Exit;
  }

 DelCrate_Exit:
  if (phy_name)
    free(phy_name);
  return status;
}

//-------------------------------------------------------------------------
// print a help menu for cts functions
//-------------------------------------------------------------------------
/*
int Help() {
  printf("2002.12.11 - a\n");
  printf("CTS usage:   (NB! entries are case insensitive)\n");
  printf("\n");
  printf("ASSIGN\n");
  printf
      ("CTS>  assign physical_name  logical_name    [/comment = \"this is a optional comment\"]\n");
  printf("             GKslnn:Ndd     ...              ... \n");
  printf("e.g.  assign GKC416:N11     HALPHA_AURORA14  /comment = \"AURORA 14 digitizer\"\n");
  printf("               ||||  ||\n");
  printf("               ||--  --\n");
  printf("               || |   |_ CAMAC crate slot(station) number -- NB!  <= 30\n");
  printf("               || |_____ CAMAC crate number (e.g. 0,1,...)\n");
  printf("               ||_______ scsi id            (e.g. 0,1,...)\n");
  printf("               |________ scsi host adapter  (e.g. A,B,...)\n");
  printf("\n");
  printf("DEASSIGN\n");
  printf("CTS> deassign logical_name\n");
  printf("CTS> deassign physical_name /p\n");
  printf("	NB! More than one logical name may be assigned to the same\n");
  printf("		physical name. When a unique logical name is removed,\n");
  printf("		only that module is deassigned. When a physical name is\n");
  printf("		removed, all associated logical names will be removed, too.\n");

  return SUCCESS;
}
*/
