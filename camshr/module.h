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
// module.h
//==================================
#ifndef __MODULE_H
#define __MODULE_H

// structure of CTS_DB_FILE
struct MODULE {
  //--------------------------
  // logical CAMAC module name
  //--------------------------
  char Name[32];		// NB! no spaces allowed, but can use _'s
  char r1;

  //--------------------------
  // physical name
  //--------------------------
  unsigned char r2[2];		// 'GK'         <<-- fixed characters

  //--------------------------
  // scsi info
  //--------------------------
  char Adapter;			// 'A'(=scsi0), 'B'(=scsi1), ...
  char Id;			// 0, 1, 2, ...

  //--------------------------
  // CAMAC info
  //--------------------------
  char Crate[2];		// 01, 02, ..., 99
  unsigned char r3[2];		// 'N:'         <<-- fixed characters
  char Slot[2];			// 1, 2, ..., 30

  unsigned char r4;

  //--------------------------
  // comment
  //--------------------------
  char Comment[41];
};
#define	MODULE_ENTRY	sizeof(struct MODULE)

// internal structure
struct Module_ {
  char name[32];		// 2002.02.07
  int adapter;
  int id;
  int crate;
  int slot;
  char comment[41];		// 2002.02.07
} Module;

// linked list structure
struct t_mod {
  char mod_name[32];		// module logical name
  struct t_mod *next;		// pointer to next element
};

#endif
