// prototypes.h
// Tue Jun 27 13:37:21 EDT 2000
// Tue Jan  2 12:33:24 EST 2001
// Tue Apr  3 09:59:07 EDT 2001
/***************************
 * global prototype() file *
 **************************/
#ifndef __PROTOTYPES_H
#define __PROTOTYPES_H

// needed include file
#include "ScsiCamac.h"
#include "module.h"
#include "crate.h"
#include <mdsplus/mdsconfig.h>

// SCSI CAMAC funtion prototypes
int CamPiow(char *Name,		// CAMAC module name
	    BYTE A,		// address register
	    BYTE F,		// function register
	    void *Data,		// pointer to data buffer
	    BYTE Mem,		// size of memory, 16 or 24 bit
	    TranslatedIosb * iosb	//
    );

int CamPioQrep(char *Name,	// CAMAC module name
	       BYTE A,		// 
	       BYTE F,		//
	       void *Data,	// pointer to data buffer
	       BYTE Mem,	// 
	       TranslatedIosb * iosb	//
    );

//int CamFQstop(                                        [2001.09.13]
int CamFQstopw(char *Name,	// CAMAC module name
	       BYTE A,		// 
	       BYTE F,		//
	       int Count,	//
	       void *Data,	// pointer to data buffer
	       BYTE Mem,	// 
	       TranslatedIosb * iosb	//
    );

//int CamFStop(                                         [2001.09.13]
int CamFStopw(char *Name,	// CAMAC module name
	      BYTE A,		// 
	      BYTE F,		//
	      int Count,	//
	      void *Data,	// pointer to data buffer
	      BYTE Mem,		// 
	      TranslatedIosb * iosb	//
    );

int CamQrep(char *Name,		// CAMAC module name
	    BYTE A,		// 
	    BYTE F,		//
	    int Count,		//
	    void *Data,		// pointer to data buffer
	    BYTE Mem,		// 
	    TranslatedIosb * iosb	//
    );

int CamQscan(char *Name,	// CAMAC module name
	     BYTE A,		// 
	     BYTE F,		//
	     int Count,		//
	     void *Data,	// pointer to data buffer
	     BYTE Mem,		// 
	     TranslatedIosb * iosb	//
    );

int CamQstopw(char *Name,	// CAMAC module name
	      BYTE A,		// 
	      BYTE F,		//
	      int Count,	//
	      void *Data,	// pointer to data buffer
	      BYTE Mem,		// 
	      TranslatedIosb * iosb	//
    );

int CamStop(char *Name,		// CAMAC module name
	    BYTE A,		// 
	    BYTE F,		//
	    int Count,		//
	    void *Data,		// pointer to data buffer
	    BYTE Mem,		// 
	    TranslatedIosb * iosb	//
    );

int lookup_module(char *Name);

int QueryHighwayType(char *serial_driver);

int RequestSense();

int ScsiIo(int scsiDevice,	// eg. "/dev/sg#" where # is 0,1,2...255
	   UserParams * user, void (*ast) ()
    );

//-------------------------------------------------------------------------
int QueryHighwayType(char *serial_driver);
int TestUnitReady(char *serial_driver);

// verbs.c function prototypes
extern EXPORT int add_entry(int dbType, char *newEntry);
int bisearch(int dbType, const void *target, int size,
	     int (*compare) (const void *key1, const void *key2));
extern EXPORT int check_for_file(char *FileName);
int check_sema4();
int commit_entry(int dbType);
int compare_str(const void *key1, const void *key2);
int copy(int dbType, char *in_file, char *out_file, int count);
int create_sema4();
int create_tmp_file(int dbType, int count, char *filename);
extern EXPORT int expand_db(int dbType, int numOfEntries);
int find_scsi_device(char *deviceName);
extern EXPORT int get_crate_status(char *crate_name, int *crate_status);
extern EXPORT int get_db_file_size(char *FileName);
extern EXPORT int get_file_count(int dbType);
extern EXPORT char *get_file_name(char *filename);
int get_scsi_device_number(char *highway_name, int *enhanced, int *online);
int issort(void *data, int size, int esize, int (*compare) (const void *key1, const void *key2));
int lock_file();
extern EXPORT int lookup_entry(int dbType, char *module_name);
extern EXPORT int map_data_file(int dbType);
extern EXPORT int map_scsi_device(char *highway_name);
int MSGLVL(int lvl);
extern EXPORT void parse_crate_db(struct CRATE *in, struct Crate_ *out);
extern EXPORT void parse_cts_db(struct MODULE *in, struct Module_ *out);
int remove_cts_entry(int index);
extern EXPORT int ScsiSystemStatus(void);
extern EXPORT int turn_crate_on_off_line(char *crate_name, int state);
int unlock_file();
int xlate_logicalname(char *Name, CamKey * Key);

//-------------------------------------------------------------------------
// debug prototypes
//-------------------------------------------------------------------------
void dump_db(int count);
void ShowStatus(int statcode);
int scsi_lock(int scsiDevice, int lock);

int scsi_io(int scsiDevice, int direction, unsigned char *cmdp,
	    unsigned char cmd_len, char *buffer, unsigned int buflen,
	    unsigned char *sbp, unsigned char mx_sb_len,
	    unsigned char *sb_out_len, int *transfer_len);
int SGSetMAXBUF(int scsiDevice, int new);
int SGGetMAXBUF(int scsiDevice);

#endif
