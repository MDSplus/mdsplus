// ScsiIo.c -- (SI2.c)
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
// Thu Jun 29 09:53:43 EDT 2000
// Mon Jul  3 15:28:01 EDT 2000
// Thu Feb 22 11:52:39 EST 2001
// Mon Jun 25 11:59:47 EDT 2001 -- changed buffers to allocated memory
// Thu Jul 12 10:49:24 EDT 2001	-- fixed return status
// Fri Aug 17 14:58:49 EDT 2001 -- changed calling sequence parameters
// Mon Aug 20 14:47:28 EDT 2001	-- minor cleanup
// Thu Aug 30 11:03:35 EDT 2001	-- fix to jorway non-data command
// Thu Sep 13 11:46:02 EDT 2001	-- fixed memory leak (ie free()) problem
// Mon Oct 15 10:47:35 EDT 2001	-- cleanup
// Mon Oct 22 16:05:23 EDT 2001	-- check/allocate additional kernel buffer space
/********************************************
 * This is the lower level interface to the *
 * generic SCSI driver                      *
 *******************************************/

//                            ...35m... is magenta
#define ROUTINE_NAME	"\033[01;35mScsiIo\033[0m"

//-----------------------------------------------------------
// include files
//-----------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/ioctl.h>

#include <scsi/scsi.h>
#include <scsi/sg.h>

#include "common.h"
#include "prototypes.h"

#define	ItemsPerLine	16

//-----------------------------------------------------------
// local functions
//-----------------------------------------------------------
static void LineBreak( int count );

/***************************************************************
* Process a complete SCSI command. Use generic SCSI interface. *
* NB! Supports Linux SCSI generic packet device driver,        *
*	version 2.1.36 or later. Is backward binary compatible.    *
***************************************************************/
int ScsiIo(	int        scsiDevice,				// eg. "/dev/sg#" where '#' is 0,1,2...
			UserParams *user,					// all needed info
			void       (*ExtractStatusFlags)()	// highway-specific function to extract appropriate status bits
			)
{
	char			 device_name[9];			// eg "/dev/sg#"
	BYTE 			 *scsiCmdBuffer = (BYTE *)NULL;
	BYTE 			 *scsiRetBuffer = (BYTE *)NULL;
	int 		 	 fd;						// file descriptor
	int				 i, rc;						// general purpose
	int				 ask4kernelbuffer;			// [2001.10.22]
	int				 kernelbuffersize;
	int 			 read_size, send_size;
	int 			 status = SUCCESS;			// function return value
	int				 scsiStatus;
	static int 		 PackId = 0;				// persistent, for debug
	struct sg_header *sg_hd;					// generic scsi header
	sigset_t         block_mask;				// to prevent ctrl-C problems

	if( MSGLVL(FUNCTION_NAME) )
		printf( "%s()\n", ROUTINE_NAME );

	if( MSGLVL(DETAILS) ) {
		printf( "scsiDevice:       %d\n", scsiDevice );
		printf( "scsi command:     " );
		for( i = 0; i < user->scsi_descr.command_len; ++i )
			printf( "0x%02x ", *(user->scsi_descr.command + i) );
		printf( "\n" );
		printf( "scsi command len: %d\n", user->scsi_descr.command_len );
		printf( "direction:        %s_CAMAC\n", (user->scsi_descr.direction == TO_CAMAC) ? "TO" : "FROM" );
		if( user->scsi_descr.direction == TO_CAMAC && user->scsi_descr.data_len ) {
			printf( "data:             " );
			for( i = 0; i < user->scsi_descr.data_len; ++i )
				printf( "%s0x%02x ",(i%ItemsPerLine==0 && i) ? "\n                  " : "", *(user->scsi_descr.data + i) );
			printf( "\n" );
		}
		printf( "data len:         %d\n", user->scsi_descr.data_len );
		printf( "sense len:        %d\n", user->scsi_descr.sense_len );
	}

	// clear IOSB
	user->actual_iosb.status      = 0;			// function's status
	user->actual_iosb.bytcnt      = 0;			// data byte count
	user->actual_iosb.scsi_status = 0;			// scsi command status

	// safety checks
	if( !user->scsi_descr.command_len ) { 		// need a command length > 0
		status = FAILURE;
		goto ScsiIo_Exit;
	}

	if( !user->scsi_descr.command  ) { 			// need a command to send
		status = FAILURE;
		goto ScsiIo_Exit;
	}

	// get a file descriptor for SCSI host adapter
	sprintf(device_name, "/dev/sg%d", scsiDevice);
	if( (fd = open(device_name, O_RDWR)) < 0 ) {
		if( MSGLVL(ALWAYS) )
			fprintf( stderr, "%s(): Need read/write permissions for \"%s\".[%d]\n", ROUTINE_NAME, device_name, errno );

		status = NO_PERMISSION;
		goto ScsiIo_Exit;
	}
	if( MSGLVL(DETAILS) )
		printf( "%s(): fd = %d\n", ROUTINE_NAME, fd );

	// check to see if we have enough system resources, ie kernel buffer space
	// calculate size of scsi command buffer
	send_size = sizeof(struct sg_header)     + 
				user->scsi_descr.command_len + 
				((user->scsi_descr.direction == TO_CAMAC  ) ? user->scsi_descr.data_len : 0);

	// calculate size of scsi return buffer
	read_size = sizeof(struct sg_header)     + 
				((user->scsi_descr.direction == FROM_CAMAC) ? user->scsi_descr.data_len : 0);

	// check kernel buffer size
	ask4kernelbuffer = (send_size > read_size) ? send_size : read_size;
	if( ask4kernelbuffer > SG_DEF_RESERVED_SIZE ) {	// currently allocated is too small, so enlarge
		if( MSGLVL(DETAILS) )
			printf( "%s(): trying to allocate %d byte kernel buffer\n", ROUTINE_NAME, ask4kernelbuffer );

		// allocate the buffer
		if( (i = ioctl(fd, SG_SET_RESERVED_SIZE, &ask4kernelbuffer)) == ERROR ) {
			if( MSGLVL(IMPORTANT) )
				printf( "%s(): could NOT allocate %d byte kernel buffer\n", ROUTINE_NAME, ask4kernelbuffer );

			status = NO_MEMORY;
			goto ScsiIo_Exit;
		}

		// check amount allocated
		if( (i = ioctl(fd, SG_GET_RESERVED_SIZE, &kernelbuffersize)) == ERROR ) {
			if( MSGLVL(IMPORTANT) )
				printf( "%s(): could NOT read kernel buffer size\n", ROUTINE_NAME );

			status = NO_MEMORY;
			goto ScsiIo_Exit;
		}
		else {
			if( kernelbuffersize < ask4kernelbuffer ) {
				printf( "%s(): kernel buffer too small [%d]\n", ROUTINE_NAME, errno );

				status = NO_MEMORY;
				goto ScsiIo_Exit;
			}
		}
	}

	if( MSGLVL(DETAILS) )
		printf( "%s(): send_size = %d\n", ROUTINE_NAME, send_size );

	// allocate memory for command buffer
	if( (scsiCmdBuffer = (BYTE *)malloc(send_size)) == NULL ) {
		if( MSGLVL(ALWAYS) )
			fprintf( stderr, "%s(): 'scsiCmdBuffer[]' -- malloc() failed\n", ROUTINE_NAME );

		status = NO_MEMORY;
		goto ScsiIo_Exit;
	}

	// clear out command buffer
	memset(scsiCmdBuffer, 0, send_size);

	// generic SCSI device header stuff comes first (see below)...
	sg_hd              = (struct sg_header *)scsiCmdBuffer;
	sg_hd->reply_len   = sizeof(struct sg_header) + user->scsi_descr.data_len;
	sg_hd->twelve_byte = user->scsi_descr.command_len == 12;
	sg_hd->pack_id     = PackId++;	// not necessary, but helps track commands

	// scsi command comes next ...
	memcpy(scsiCmdBuffer + sizeof(struct sg_header), 
		   user->scsi_descr.command, 
		   user->scsi_descr.command_len
		   );

	// Special Jorway case [2001.08.29]	& [2001.09.21]
	// NB! this is a scsi group 6 command (eg >= 0xC0). the linux
	// 'sg()' driver shows that this is 10 bytes in length, but
	// the Jorway manual shows this as 6 bytes in length. good
	// 'house keeping' zeros the last 4 bytes.
	if( *user->scsi_descr.command == OpCodeCamacNonDataCommand )
		memset(scsiCmdBuffer + sizeof(struct sg_header) + 6, 0, 4);		// clear final 4 bytes

	// data out next (if appropriate) ...
	if( user->scsi_descr.direction == TO_CAMAC && user->scsi_descr.data_len )
		memcpy(scsiCmdBuffer + (sizeof(struct sg_header) + user->scsi_descr.command_len), 
			   user->scsi_descr.data, 
			   user->scsi_descr.data_len
			   );

	// what to send to generic scsi interface:
	// +------------------------------+
	// | sg_header                    |  <<-- 36 bytes           (required)
	// +------------------------------+
	// | scsi command                 |  <<-- 6, 10, or 12 bytes (required)
	// +------------------------------+
	// | output data (if appropriate) |  <<-- as needed
	// +------------------------------+

	if( MSGLVL(DETAILS) ) {
		printf( "%s(): about to write %d bytes to fd = %d", ROUTINE_NAME, send_size, fd );
		for( i = 0; i < send_size; ++i )
			printf( "%s0x%02x ",(i%ItemsPerLine==0) ? "\n" : "", *(scsiCmdBuffer + i) );
		printf( "\n" );
	}

	// send command ...
	// ... but first, block SIGINT (ala ^C)
	sigemptyset(&block_mask);
	sigaddset(&block_mask, SIGINT);
	sigprocmask(SIG_BLOCK, &block_mask, NULL);
	if( MSGLVL(DETAILS) )
		printf( "%s^C is now blocked%s\n", RED, NORMAL );

	// write it ...
	rc = write(fd, scsiCmdBuffer, send_size);
	if( rc < 0 || rc != send_size ) { // some error happened
		if( MSGLVL(ALWAYS) ) {
			fprintf( stderr, "%s(): (generic scsi) write status = %d, SCSI cmd = 0x%x errno = %d\n",
				ROUTINE_NAME,
				rc,
				*(scsiCmdBuffer + sizeof(struct sg_header)),
				errno
				);
			perror("");
		}

		status = FAILURE;
		goto ScsiIo_Exit;
	}

	if( MSGLVL(DETAILS) )
		printf( "%s(): wrote %d bytes, successful!!!\n", ROUTINE_NAME, rc );

	// cleanup allocated buffer -- done with command buffer
	if( scsiCmdBuffer ) {
		free(scsiCmdBuffer);
		scsiCmdBuffer = NULL;

		if( MSGLVL(DETAILS) )
			printf( "%s(): free'd scsiCmdBuffer\n", ROUTINE_NAME );
	}

	// response from generic scsi command:
	// +------------------------------+
	// | sg_header                    |  <<-- 36 bytes (header + sense_buffer(status/error))
	// +------------------------------+
	// | input data (if appropriate)  |  <<-- as appropriate
	// +------------------------------+
	
	// retrieve result
	if( MSGLVL(DETAILS) )
		printf( "%s(): want to read %d bytes", ROUTINE_NAME, read_size );

	// allocate memory for scsi return buffer
	if( (scsiRetBuffer = (BYTE *)malloc(read_size)) == NULL ) {
		if( MSGLVL(ALWAYS) )
			fprintf( stderr, "%s(): 'scsiRetBuffer[]' -- malloc() failed\n", ROUTINE_NAME );

		status = NO_MEMORY;
		goto ScsiIo_Exit;
	}

	// clear out return buffer
	memset(scsiRetBuffer, 0, read_size);

	// actual read
	rc = read(fd, scsiRetBuffer, read_size);

	// release signal block
	sigprocmask(SIG_UNBLOCK, &block_mask, NULL);
	sigdelset(&block_mask, SIGINT);

	if( MSGLVL(DETAILS) )
		printf( ", read() returned %d\n", rc );

	close(fd);

	if( rc < 0 ) {	// all bets are off, nothing returned
		if( MSGLVL(ALWAYS) ) {
			fprintf( stderr, "%s(): read(generic) error: status = %d  errno = %d\n",
				ROUTINE_NAME,
				rc,
				errno
				);
			perror("");
		}

		status = FAILURE;
		goto ScsiIo_Exit;
	}

	// maybe something returned ...
	sg_hd = (struct sg_header *)scsiRetBuffer;					// point into returned array
	if( rc < sizeof(struct sg_header) ) {						// incomplete header
		if( MSGLVL(ALWAYS) ) {
			fprintf( stderr, "%s(): read(generic): returned %d bytes", ROUTINE_NAME, rc );
			for( i = 0; i < rc; ++i )
				printf( "%s0x%02x ", (i%ItemsPerLine==0) ? "\n" : "", *(scsiRetBuffer + i) );
			printf( "\n" );
		}

		status = FAILURE;
		goto ScsiIo_Exit;
	}
	else if( rc < read_size ) {									// partial response
		if( MSGLVL(ALWAYS) ) {
			fprintf( stderr, "%s(): read(generic) status = %d, result = 0x%x, cmd = 0x%x\n",
				ROUTINE_NAME,
				rc,												// read() status
				(sg_hd->target_status & STATUS_MASK) >> 1,		// within sg_header
				*(scsiRetBuffer + sizeof(struct sg_header))		// command byte
				);

			fprintf( stderr, "%s(): read(generic) sense ", ROUTINE_NAME );
			for( i = 0; i < SG_MAX_SENSE; ++i )
				fprintf( stderr, "0x%02x ", *(scsiRetBuffer + 20 + i) );	// offset into sg_header for sense data
			fprintf( stderr, "\n" );
		}

		status = FAILURE;
		goto ScsiIo_Exit;
	}

	// else, we got it all !!!
	if( MSGLVL(DETAILS) ) {
		printf( "%s(): raw read buffer:", ROUTINE_NAME );
		for( i = 0; i < rc; ++i )
			printf( "%s0x%02x ", (i%ItemsPerLine==0) ? "\n" : "", *(scsiRetBuffer + i) );
		printf( "\n" );
	}

	// copy sense data   	[2001.10.10]
	memcpy(user->scsi_descr.sense, &scsiRetBuffer[20], SG_MAX_SENSE);

	// copy return data 	[2001.10.10]
	if( user->scsi_descr.direction == FROM_CAMAC && user->scsi_descr.data_len )
		memcpy(user->scsi_descr.data, &scsiRetBuffer[sizeof(struct sg_header)], user->scsi_descr.data_len);

	// for debug ...
	if( MSGLVL(DETAILS) ) {
		LineBreak( SG_MAX_SENSE );

		// scsi data buffer
		if( user->scsi_descr.direction == FROM_CAMAC && user->scsi_descr.data_len ) {	// only if data exists
			printf( "%s():\n", ROUTINE_NAME );
			printf( "user->scsi_descr.data: " );
			for( i = 0; i < user->scsi_descr.data_len; ++i )
				printf( "%s%s0x%02x%s ", 
					(i%ItemsPerLine==0 && i) ? "\n                       " : "",
					(*(BYTE*)(user->scsi_descr.data + i)) ? YELLOW : NORMAL,
					 *(BYTE*)(user->scsi_descr.data + i),
					NORMAL
					);
			printf( "\n" );
			LineBreak( SG_MAX_SENSE );
		}

		// scsi sense data
		printf( "%s(): user->scsi_descr.sense (raw read buffer)\n", ROUTINE_NAME );
		for( i = 0; i < SG_MAX_SENSE; ++i )
			printf( "%s0x%02x%s ",
				(*(BYTE*)(user->scsi_descr.sense + i)) ? YELLOW : NORMAL,
				 *(BYTE*)(user->scsi_descr.sense + i),
				NORMAL
				);
		printf( "\n" );

		LineBreak( SG_MAX_SENSE );
	}

	// derive SCSI status, returned from driver
	if( MSGLVL(DETAILS) )
		printf( "%s(): sg_hd->target_status 0x%0x ", ROUTINE_NAME, sg_hd->target_status );
	switch( sg_hd->target_status ) {	// this is SCSI status
		case GOOD:						// 0x00
			scsiStatus = GOOD;
			if( MSGLVL(DETAILS) )
				printf( "[%sGOOD%s]", GREEN, NORMAL );

			break;
		case CHECK_CONDITION:			// 0x01	<<-- masked and shifted
			scsiStatus = CHECK_CONDITION;
			if( MSGLVL(DETAILS) )
				printf( "[%sCHECK_CONDITION%s]", RED, NORMAL );

			break;
		case CONDITION_GOOD:			// 0x02
			scsiStatus = GOOD;
			if( MSGLVL(DETAILS) )
				printf( "[%sCONDITION_MET%s]", GREEN, NORMAL );

			break;
		default:
			scsiStatus = CHECK_CONDITION;
			if( MSGLVL(DETAILS) )
				printf( "[%sHWY_STATUS_BAD%s]", RED, NORMAL );

			break;
	}
	if( MSGLVL(DETAILS) )
		printf( "\n" );
	
ScsiIo_Exit:
	// check on status of SIGINT block
	if( !sigprocmask(0, NULL, &block_mask) ) {				// get current mask
		if( sigismember(&block_mask, SIGINT) ) {			// check to see if a member of mask
			if( MSGLVL(DETAILS) )
				printf( "%s^C is blocked -- ", RED );

			sigprocmask(SIG_UNBLOCK, &block_mask, NULL);	// it is, so remove it
			sigdelset(&block_mask, SIGINT);

			if( MSGLVL(DETAILS) )
				printf( "block removed%s\n", NORMAL );
		}
		else
			if( MSGLVL(DETAILS) )
				printf( "%s^C is now UN-blocked%s\n", RED, NORMAL );
	}
	else {
		if( MSGLVL(DETAILS) )
			printf( "%sfailure to get current signal mask%s\n", RED, NORMAL );
	}

	// check on file descriptor
	if( fd > 0 ) { 				// still open, so close it
		if( MSGLVL(DETAILS) )
			printf("%s(): closing fd %d\n", ROUTINE_NAME, fd);
		close(fd);
	}

	// NB! this seems to be order specific -- needs further investigation :(
	// cleanup allocated buffers
	if( scsiCmdBuffer ) {
		free(scsiCmdBuffer);
		scsiCmdBuffer = NULL;

		if( MSGLVL(DETAILS) )
			printf( "%s(): free'd scsiCmdBuffer\n", ROUTINE_NAME );
	}
	if( scsiRetBuffer ) {
		free(scsiRetBuffer);
		scsiRetBuffer = NULL;

		if( MSGLVL(DETAILS) )
			printf( "%s(): free'd scsiRetBuffer\n", ROUTINE_NAME );
	}

	if( status == SUCCESS ) {
		// common for all highway types
		user->actual_iosb.status                   = status;							// this function's status
		user->actual_iosb.scsi_status              = scsiStatus;						// returned from scsi driver
		user->actual_iosb.bytcnt                   = rc - sizeof(struct sg_header);		// 2001.08.15

		if( MSGLVL(DETAILS) ) {
			printf("%s(): user->actual_iosb.status          %d [0x%x] ", 
				ROUTINE_NAME, user->actual_iosb.status, user->actual_iosb.status); 
				ShowStatus(user->actual_iosb.status);
			printf("%s(): user->actual_iosb.scsi_status set %d\n", 
				ROUTINE_NAME, user->actual_iosb.scsi_status);
			printf("%s(): user->actual_iosb.bytcnt set      %d\n", 
				ROUTINE_NAME, user->actual_iosb.bytcnt);
		}

		// translate appropriate highway iosb struct
		ExtractStatusFlags( user );
	}
	else {		// got here because of an error
		printf( "%sERROR%s -- status:[%d] -- %sERROR%s\n", RED, NORMAL, status/2, RED, NORMAL );
	}

	return status;
}

//-----------------------------------------------------------
// for debug use -- visual seperator
//-----------------------------------------------------------
static void LineBreak( int count )
{
	int i;

	for( i = 0; i < count * 5 - 1; ++i )
		printf( "=" );
	printf( "\n" );
}
