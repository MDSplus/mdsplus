// tunables.h
#ifndef __TUNABLES_H
#define __TUNABLES_H

//#define       CAMSHR_MAJOR            1 // library major revision number
//#define       CAMSHR_MINOR            0 // library minor revision number
#define CRATE_DB_FILE "crate.db" // CAMAC crate database file
#define CRATE_DB_INCREMENT 10
#define CTS_DB_FILE "cts.db" // main CTS database file
#define CTS_DB_INCREMENT 10  // block size of db file

#define MAX_SCSI_BUSES 16 // maximum number of scsi host adapters
#define MAX_SCSI_IDS 8    // maximum number of scsi ids per host adapter

#define PROC_FILE "/proc/scsi/scsi" // scsi info here

#define SEMAPHORE_KEY_SEED "/proc/cpuinfo" // a file that this process can read

#define TMP_FILE "temp.file" // temporary storage

// environment variable names
#define DB_DIR "CTS_DB_DIR"        // environment variable
#define DEBUG_VAR_NAME "CTS_DEBUG" // for debugging purposes

#endif
