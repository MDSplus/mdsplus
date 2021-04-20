// crate.h
// Tue Nov 21 13:32:07 EST 2000
// Wed Jan  3 12:44:10 EST 2001
// Fri Jan 12 10:46:19 EST 2001
// Fri Mar  2 14:18:30 EST 2001
// Thu Mar 15 11:54:09 EST 2001
// Mon Apr 30 13:14:08 EDT 2001 -- added highway type
//==================================
#ifndef __CRATE_H
#define __CRATE_H

//-------------------------------------------------------------------------
// form of data in crate.db:
//              GKB509:001:2:1:1
//      ^^^^^^ ^^^ ^
//      ||||||  |  |||  |  | | \ online (1=online, 0=not)
//              |  |||  |  | \ enhanced (1=enhanced, 0=not)
//              GK||||  |  |      == constants
//                B|||  |  |      == scsi host adapter
//                 5||  |  |      == scsi id
//                  09 / \ |      == camac crate number
//             001 |  == dev/sg#
//                 2  == type, e.g. KineticSystems
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
struct CRATE_NAME
{
  // physical CAMAC crate name
  char prefix[2]; // 'GK'         <<-- fixed characters

  // scsi info
  char Adapter; // 'A'(=scsi0), 'B'(=scsi1), ...
  char Id;      //  0, 1, 2, ...

  // CAMAC info
  char Crate[2]; //  01, 02, ..., 99     (NB! this field is two chars)
};

struct CRATE
{
  struct CRATE_NAME Phys_Name; // e.g. GKA109
  char r1;                     // ':'
  char DSFname[3];             // devive special file name, eg '/dev/sg#', eg 001
  char r2;                     // ':'
  char HwyType;                // e.g. Jorway, KineticSystems
  char r3;                     // ':'
  char enhanced;               // '1'=enhanced '0'=not
  char r4;                     // ':'
  char online;                 // '1'=online '0'=not
  char r5;                     // room for '\n'
};
#define CRATE_ENTRY sizeof(struct CRATE)

//-------------------------------------------------------------------------
// internal structure
typedef struct Crate_
{
  char name[7];
  int device;
  int type;
  int enhanced;
  int online;
} Crate;

#endif
