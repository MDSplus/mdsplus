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
struct CRATE_NAME {
  // physical CAMAC crate name
  char prefix[2];		// 'GK'         <<-- fixed characters

  // scsi info
  char Adapter;			// 'A'(=scsi0), 'B'(=scsi1), ...
  char Id;			//  0, 1, 2, ...

  // CAMAC info
  char Crate[2];		//  01, 02, ..., 99     (NB! this field is two chars)
};

struct CRATE {
  struct CRATE_NAME Phys_Name;	// e.g. GKA109
  char r1;			// ':'
  char DSFname[3];		// devive special file name, eg '/dev/sg#', eg 001
  char r2;			// ':'
  char HwyType;			// e.g. Jorway, KineticSystems
  char r3;			// ':'
  char enhanced;		// '1'=enhanced '0'=not
  char r4;			// ':'
  char online;			// '1'=online '0'=not
  char r5;			// room for '\n'
};
#define	CRATE_ENTRY	sizeof(struct CRATE)

//-------------------------------------------------------------------------
// internal structure
struct Crate_ {
  char name[7];
  int device;
  int type;
  int enhanced;
  int online;
} Crate;

#endif
