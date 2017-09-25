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
// tunables.h
#ifndef __TUNABLES_H
#define __TUNABLES_H

//#define       CAMSHR_MAJOR            1                                       // library major revision number
//#define       CAMSHR_MINOR            0                                       // library minor revision number
#define	CRATE_DB_FILE		"crate.db"	// CAMAC crate database file
#define	CRATE_DB_INCREMENT	10
#define	CTS_DB_FILE			"cts.db"	// main CTS database file
#define CTS_DB_INCREMENT	10	// block size of db file

#define MAX_SCSI_BUSES 		16	// maximum number of scsi host adapters
#define	MAX_SCSI_IDS		8	// maximum number of scsi ids per host adapter

#define	PROC_FILE			"/proc/scsi/scsi"	// scsi info here

#define	SEMAPHORE_KEY_SEED	"/proc/cpuinfo"	// a file that this process can read

#define	TMP_FILE			"temp.file"	// temporary storage

// environment variable names
#define	DB_DIR				"CTS_DB_DIR"	// environment variable
#define	DEBUG_VAR_NAME		"CTS_DEBUG"	// for debugging purposes

#endif
