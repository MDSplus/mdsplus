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
#ifndef _MDSSERVER
#define _MDSSERVER
#include <treeshr_hooks.h>

typedef enum { doabort,		  /**** Abort current action or mdsdcl command ***/
  action,			  /**** Execute an action nid in a tree ***/
  close_tree,				  /**** Close open trees ***/
  create_pulse,			      /**** Create pulse files for single tree (no subtrees) ***/
  logging,			  /**** Turn logging on/off ***/
  mdsdcl_command,		  /**** Execute MDSDCL command ***/
  monitor,			  /**** Broadcast messages to action monitors ***/
  show,				  /**** Request current status of server ***/
  stop,				   /**** Stop server ***/
  noop				  /**** Noop used to start server ***/
} MsgType;

typedef enum { none,
  log,
  statistics
} LoggingType;

typedef enum { build_table_begin = 1,
  build_table,
  build_table_end,
  display_checkin,
  dispatched,
  doing,
  done
} MonitorMode;

#ifdef __ALPHA
#pragma member_alignment save
#pragma nomember_alignment
#endif

typedef struct {
  char flush;
} DoAbortMsg;
typedef struct {
  char treename[12];
  int shot;
  int nid;
} ActionMsg;
/* close message has no data */
typedef struct {
  char treename[12];
  int shot;
} CreatePulseMsg;
typedef struct {
  LoggingType type;
} LoggingMsg;
typedef struct {
  char cli[12];
  char command[1];
} MdsDclCommandMsg;
typedef struct {
  char treename[12];
  int shot;
  int phase;
  int nid;
  char on;
  MonitorMode mode;
  char server[32];
  int status;
} MonitorMsg;
typedef struct {
  char full;
} ShowMsg;
/* stop message has no data */

typedef struct {
  char opcode;
  char now;
  char fill1;
  char fill2;
  int status;
  char data[1];
} Msg;

#define MsgOffset(field) ((int)&((Msg *)0)->field)

typedef struct {
  char server[32];
  int nid;
  int phase;
  int sequence;
  int num_references;
  int *referenced_by;
  struct descriptor *condition;
  int status;
  int netid;
  unsigned on:1;
  unsigned done:1;
  unsigned closed:1;
  unsigned dispatched:1;
  unsigned doing:1;
  unsigned recorded:1;
} ActionInfo;

typedef struct {
  int num;
  char tree[12];
  int shot;
  int failed_essential;
  ActionInfo actions[1];
} DispatchTable;

typedef struct {
  int shot;
  union {
    int nid;
    int phase;
  } u;
  int status;
} DispatchEvent;

#ifdef __ALPHA
#pragma member_alignment restore
#endif

#endif
