//---------------------------------------------------------
// common.h -- common values for general use
// Fri Dec 29 11:55:33 EST 2000
// Fri Mar  9 13:11:58 EST 2001
// Tue Jul 31 11:54:35 EDT 2001 -- added crate on/off line command
// Mon Aug 20 14:48:14 EDT 2001 -- added symbolic bit flags
// Thu Oct 11 10:58:19 EDT 2001 -- added colors
// Tue Feb  5 15:08:13 EST 2002 -- included 'math.h'
//---------------------------------------------------------
#ifndef __COMMON_H
#define __COMMON_H

#include "macros.h"  // macros
#include "mytypes.h" // define new types
#include <math.h>

//---------------------------------------------------------
// parameters that may be changed to alter system performance
//---------------------------------------------------------
#include "tunables.h"

//---------------------------------------------------------
// constants that should *NOT* need to be changed
//---------------------------------------------------------
#define COMMENT_INDEX 44 // index position of string "(null)" in a comment field
#define EQUAL 0
#define SUCCESS 1
#define FAILURE 0

// MSGLVL() values
enum
{
  ALWAYS = 1, // ... well, almost always -- for debug printout
  IMPORTANT,  // not fatal
  FUNCTION_NAME = 5,
  DETAILS = 9
};
#if 0
#define ALWAYS 1    // ... well, almost always -- for debug printout
#define IMPORTANT 2 // not fatal
#define FUNCTION_NAME 5
#define DETAILS 9
#endif

#define FALSE 0
#define TRUE 1
enum
{
  TO_CAMAC,
  FROM_CAMAC
}; // ie WRITE and READ
enum
{
  STATUS_BAD,
  STATUS_GOOD
};
enum
{
  CTS_DB,
  CRATE_DB
};
enum
{
  OFF,
  ON
}; // crate offline/online command

// semaphore values
#define P_SEMA4 -1
#define V_SEMA4 1

//---------------------------------------------------------
// condition codes for cts verbs
// error conditions (NB! all values are negative)
//---------------------------------------------------------
#if 0
#define ERROR -1           // general status error
#define FILE_ERROR -2      // file doesn't exist
#define DUPLICATE -3       // duplicate module name
#define LOCK_ERROR -4      // unable to lock file
#define MAP_ERROR -5       // db file failed to map to memory
#define COMMIT_ERROR -6    // data did not get added to db
#define UNLOCK_ERROR -7    // unable to unlock file
#define EXPAND_ERROR -8    // failure to expand db file
#define COPY_ERROR -9      // file copy error
#define ASSIGN_ERROR -10   // assign failure
#define DEASSIGN_ERROR -11 // deassign failure
#define DELCRATE_ERROR -12 // delete crate failure
#define NO_MEMORY -13      // malloc() failure
#define NO_DEVICE -14      // no scsi device found
#define QUERY_ERROR -15    // unable to determine highway type
#define NO_PERMISSION -16  // need read/write permission
#endif

// NB! make sure all are EVEN negative values
#if 1
#define ERROR -(1 << 1)           // general status error
#define FILE_ERROR -(2 << 1)      // file doesn't exist
#define DUPLICATE -(3 << 1)       // duplicate module name
#define LOCK_ERROR -(4 << 1)      // unable to lock file
#define MAP_ERROR -(5 << 1)       // db file failed to map to memory
#define COMMIT_ERROR -(6 << 1)    // data did not get added to db
#define UNLOCK_ERROR -(7 << 1)    // unable to unlock file
#define EXPAND_ERROR -(8 << 1)    // failure to expand db file
#define COPY_ERROR -(9 << 1)      // file copy error
#define ASSIGN_ERROR -(10 << 1)   // assign failure
#define DEASSIGN_ERROR -(11 << 1) // deassign failure
#define DELCRATE_ERROR -(12 << 1) // delete crate failure
#define NO_MEMORY -(13 << 1)      // malloc() failure
#define NO_DEVICE -(14 << 1)      // no scsi device found
#define QUERY_ERROR -(15 << 1)    // unable to determine highway type
#define NO_PERMISSION -(16 << 1)  // need read/write permission
#endif

//---------------------------------------------------------
// system specific, generally static
//---------------------------------------------------------
#define HIGHWAY_PARALLEL 0
#define HIGHWAY_SERIAL 1
#define TYPE_UNKNOWN 0

// highway info
#define UNDEFINED 0
#define JORWAY 1
#define KINSYSCO 2
#define JORWAY_OLD 3
#define JORWAY_73A 4

// highway status
#define HWY_GOOD 0
#define HWY_CHECK_CONDITION 2
#define HWY_CONDITION_MET 4
#define HWY_STATUS_BAD -1

// SCC commands
#define ON_LINE 0x1000

// for KineticSystems block transfer
#define HI 2
#define MID 1
#define LO 0

// CAMAC error/status bits
// sense keys                           -- see "ScsiCamac.h"
// additional sense codes       -- see "ScsiCamac.h"

// jorway
// main status register bits
#define SNEX (1 << 3)
#define TO (1 << 5)
#define NO_X (1 << 6)
#define NO_Q (1 << 7)
// serial status register
#define SYNC (1 << 4)
#define LOSYN (1 << 5)
#define TIMOS (1 << 9)
#define RPE (1 << 10)

// kinetic systems -- struct err
#define NOQ (1 << 0)
#define NOX (1 << 1)
#define ADNR (1 << 3)
#define TPE (1 << 4)
#define LPE (1 << 5)
#define ERR (1 << 7)
#define NO_SYNC (1 << 8)
#define TMO (1 << 9)

// colors for output
#include "colors.h"

#endif
