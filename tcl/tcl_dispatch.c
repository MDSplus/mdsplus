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
#include <stdlib.h>
#include <string.h>

#include <dbidef.h>
#include <ncidef.h>
#include <dcl.h>
#include <strroutines.h>
#include <servershr.h>
#include <mdsshr.h>
#include <mds_stdarg.h>
#include <treeshr.h>
#include <tcl_messages.h>
#include <mdsdcl_messages.h>
#define DEF_FREED
#define DEF_FREEXD
#include <pthread_port.h>

#include "tcl_p.h"

extern int TdiData();

/**********************************************************************
* TCL_DISPATCH.C --
*
* Various "Dispatch" functions:
*    TCL> DISPATCH[/NOWAIT] node
*    TCL> DISPATCH/BUILD[/MONITOR=]
*    TCL> DISPATCH/PHASE[/LOG|NOLOG][/MONITOR=] phase [/NOACT]
*    TCL> DISPATCH/CLOSE [/SERVER=list]
*    TCL> DISPATCH/COMMAND/SERVER=server command
*    TCL> STOP SERVER [server-list]
*
* History:
*  28-Apr-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/

#define IS_WILD(T)   (strcspn(T,"*%") < strlen(T))

#define SYNCINIT int SyncId = 0
#define SYNCPASS &SyncId
#define SYNCWAIT ServerWait(SyncId)

#define INIT_TCLSTATUS INIT_STATUS_AS TclNORMAL

static void *dispatch_table = 0;

extern int ServerFailedEssential();

extern int TdiIdentOf();

/****************************************************************
 * TclDispatch_close:
 ****************************************************************/
EXPORT int TclDispatch_close(void *ctx, char **error __attribute__ ((unused)), char **output __attribute__ ((unused)))
{
  char *ident;
  if (IS_NOT_OK(cli_present(ctx, "SERVER")) && dispatch_table)
    ServerDispatchClose(dispatch_table);
  else {
    while IS_OK(cli_get_value(ctx, "SERVER", &ident)) {
      FREE_ON_EXIT(ident);
      ServerCloseTrees(ident);
      FREE_NOW(ident);
    }
  }
  return TclNORMAL;
}

/**************************************************************
 * TclDispatch_build:
 **************************************************************/
EXPORT int TclDispatch_build(void *ctx, char **error, char **output __attribute__ ((unused))){
  INIT_TCLSTATUS;
  INIT_AND_FREE_ON_EXIT(char*,monitor);
  cli_get_value(ctx, "MONITOR", &monitor);
  if (dispatch_table){
    ServerFreeDispatchTable(dispatch_table);
    dispatch_table = NULL;
  }
  status = ServerBuildDispatchTable(0, monitor, &dispatch_table);
  if STATUS_NOT_OK {
    char *msg = MdsGetMsg(status);
    *error = malloc(strlen(msg) + 100);
    sprintf(*error, "Error: problem building dispatch table\n" "Error message was: %s\n", msg);
  }
  FREE_NOW(monitor);
  return status;
}

/***************************************************************
 * TclDispatch:
 ***************************************************************/
EXPORT int TclDispatch(void *ctx, char **error, char **output __attribute__ ((unused))){
  INIT_TCLSTATUS;
  INIT_AND_FREE_ON_EXIT(char*,treenode);
  int iostatus;
  int nid;
  int waiting = cli_present(ctx, "WAIT") != MdsdclNEGATED;
  if IS_NOT_OK(cli_get_value(ctx, "NODE", &treenode)){
    status = MdsdclERROR;
    *error = strdup("Error: Missing node path.\n");
    goto cleanup;
  }
  status = TreeFindNode(treenode, &nid);
  if STATUS_OK {
    struct descriptor niddsc = { 4, DTYPE_NID, CLASS_S, (char *)0 };
    INIT_AND_FREED_ON_EXIT(DTYPE_T,ident);
    INIT_AND_FREEXD_ON_EXIT(xd);
    niddsc.pointer = (char *)&nid;
    status = TdiIdentOf(&niddsc, &xd MDS_END_ARG);
    if STATUS_OK
      status = TdiData(&xd, &ident MDS_END_ARG);
    FREEXD_NOW(&xd);
    if STATUS_OK {
      static char treename[13];
      static DESCRIPTOR(nullstr, "\0");
      static int shot;
      static DBI_ITM itmlst[] = { {13, DbiNAME, treename, 0},
      {4, DbiSHOTID, &shot, 0},
      {0, 0, 0, 0}
      };
      TreeGetDbi(itmlst);
      StrAppend(&ident, (struct descriptor *)&nullstr);
      SYNCINIT;
      status =
          ServerDispatchAction(SYNCPASS, ident.pointer, treename, shot, nid, NULL, NULL,
               waiting ? &iostatus : NULL, NULL, NULL, 0);
      if STATUS_OK {
        if (waiting) {
          SYNCWAIT;
          status = iostatus;
        }
      }
    }
    FREED_NOW(&ident);
  }
  if STATUS_NOT_OK {
    char *msg = MdsGetMsg(status);
    *error = malloc(strlen(msg) + strlen(treenode) + 100);
    sprintf(*error, "Error dispatching node %s\n" "Error message was: %s\n", treenode, msg);
  }
cleanup: ;
  FREE_NOW(treenode);
  return status;
}

/**************************************************************
 * TclDispatch_abort_server:
 * TclDispatch_stop_server:
 * TclDispatch_start_server:
 **************************************************************/
EXPORT int TclDispatch_abort_server(void *ctx, char **error, char **output __attribute__ ((unused))){
  INIT_TCLSTATUS;
  char *ident;
  while (STATUS_OK && IS_OK(cli_get_value(ctx, "SERVER_NAME", &ident))) {
    FREE_ON_EXIT(ident);
    status = ServerAbortServer(ident, 0);
    if (STATUS_NOT_OK && status) {
      char *msg = MdsGetMsg(status);
      *error = malloc(strlen(msg) + strlen(ident) + 1000);
      sprintf(*error, "Error: Problem aborting server '%s'\n"
	      "Error message was: %s\n", ident, msg);
    }
    FREE_NOW(ident);
  }
  return status ? status : TclNORMAL;
}

EXPORT int TclDispatch_stop_server(void *ctx, char **error, char **output __attribute__ ((unused))){
  INIT_TCLSTATUS;
  char *ident;
  while (STATUS_OK && IS_OK(cli_get_value(ctx, "SERVNAM", &ident))) {
    FREE_ON_EXIT(ident);
    status = ServerStopServer(ident);
    if (STATUS_NOT_OK && status) {
      char *msg = MdsGetMsg(status);
      *error = malloc(strlen(msg) + strlen(ident) + 100);
      sprintf(*error, "Error: problem stopping server %s\n" "Error message was: %s\n", ident, msg);
    }
    FREE_NOW(ident);
  }
  return status ? status : TclNORMAL;
}

EXPORT int TclDispatch_start_server(void *ctx, char **error, char **output __attribute__ ((unused))){
  INIT_TCLSTATUS;
  char *ident;
  while (STATUS_OK && IS_OK(cli_get_value(ctx, "SERVER", &ident))) {
    FREE_ON_EXIT(ident);
    status = ServerStartServer(ident);
    if (STATUS_NOT_OK && status) {
      char *msg = MdsGetMsg(status);
      *error = malloc(strlen(msg) + strlen(ident) + 100);
      sprintf(*error, "Error: problem starting server %s\n" "Error message was: %s\n", ident, msg);
    }
    FREE_NOW(ident);
  }
  return status ? status : TclNORMAL;
}

/***************************************************************
 * TclDispatch_set_server:
 ***************************************************************/

static inline int setLogging(void *ctx, int logging, char **error) {
  char *ident;
  int status;
  status = TclNORMAL;
  while (STATUS_OK && IS_OK(cli_get_value(ctx, "SERVER", &ident))) {
    FREE_ON_EXIT(ident);
    status = ServerSetLogging(ident, logging);
    if STATUS_NOT_OK {
      char *msg = MdsGetMsg(status);
      if (*error)
	*error = realloc(*error, strlen(*error) + strlen(msg) + strlen(ident) + 100);
      else
	*error = malloc(strlen(msg) + strlen(ident) + 100);
      sprintf(*error, "Error: Problem setting logging on serv %s\n"
	      "Error message was: %s\n", ident, msg);
    }
    FREE_NOW(ident);
  }
  return status;
}

EXPORT int TclDispatch_set_server(void *ctx, char **error, char **output __attribute__ ((unused))){
  int status;
  if (cli_present(ctx, "LOG") == MdsdclPRESENT) {
    INIT_AND_FREE_ON_EXIT(char*,log_type);
    int logging;
    cli_get_value(ctx, "LOG", &log_type);
    if (strncasecmp(log_type, "statistics", strlen(log_type)) == 0)
      logging = 2;
    else if (strncasecmp(log_type, "actions", strlen(log_type)) == 0)
      logging = 1;
    else
      logging = 0;
    status = setLogging(ctx, logging, error);
    FREE_NOW(log_type);
  }
  else
    status=setLogging(ctx, 0, error);
  return status;
}

/**************************************************************
 * TclDispatch_show_server:
 **************************************************************/
EXPORT int TclDispatch_show_server(void *ctx, char **error __attribute__ ((unused)), char **output){
  INIT_TCLSTATUS;
  char *ident;
  int dooutput = IS_OK(cli_present(ctx, "OUTPUT"));
  int full = IS_OK(cli_present(ctx, "FULL"));
  if (dooutput)
    *output = strdup("");
  while (STATUS_OK && IS_OK(cli_get_value(ctx, "SERVER_NAME", &ident))) {
    FREE_ON_EXIT(ident);
    if (IS_WILD(ident)) {	/* contains wildcard?     */
      void *ctx1 = NULL;
      char *server = NULL;
      while ((server = ServerFindServers(&ctx1, ident))) {
        FREE_ON_EXIT(server);
	tclAppend(output, "Checking server: ");
	tclAppend(output, server);
	tclAppend(output, "\n");
	mdsdclFlushOutput(*output);
	char *info=NULL;
        FREE_ON_EXIT(info);
	if (dooutput) {
	  tclAppend(output, info = ServerGetInfo(full, server));
	  tclAppend(output, "\n");
	} else
	  info = ServerGetInfo(full, server);
	FREE_NOW(info);
	FREE_NOW(server);
      }
    } else {
      char *info=NULL;
      tclAppend(output, "Checking server: ");
      tclAppend(output, ident);
      tclAppend(output, "\n");
      mdsdclFlushOutput(*output);
      FREE_ON_EXIT(info);
      info = ServerGetInfo(full, ident); 
      if (dooutput) {
	tclAppend(output, info); 
	tclAppend(output, "\n");
      }
      FREE_NOW(info);
    }
    FREE_NOW(ident);
  }
  return status;
}

static void printIt(char *output){
  fprintf(stdout, "%s\n", output);
}

/*****************************************************************
 * TclDispatch_phase:
 *****************************************************************/
EXPORT int TclDispatch_phase(void *ctx, char **error, char **output __attribute__ ((unused))){
  int status;
  INIT_AND_FREE_ON_EXIT(char*,phase);
  INIT_AND_FREE_ON_EXIT(char*,synch_str);
  INIT_AND_FREE_ON_EXIT(char*,monitor);
  int synch;
  int noaction = IS_OK(cli_present(ctx, "NOACTION"));
  void (*output_rtn) () = IS_OK(cli_present(ctx, "LOG")) ? printIt : 0;
  cli_get_value(ctx, "MONITOR", &monitor);
  cli_get_value(ctx, "PHASE_NAME", &phase);
  cli_get_value(ctx, "SYNCH", &synch_str);
  sscanf(synch_str, "%d", &synch);
  synch = synch >= 1 ? synch : 1;
  if (dispatch_table)
    status = ServerDispatchPhase(NULL, dispatch_table,
			      phase, (char)noaction, synch, output_rtn, monitor);
  else
    status = TclNO_DISPATCH_TABLE;
  if STATUS_NOT_OK {
    char *msg = MdsGetMsg(status);
    *error = malloc(strlen(msg) + 100);
    sprintf(*error, "Error: problem dispatching phase\n" "Error message was: %s\n", msg);
  }
  FREE_NOW(monitor);
  FREE_NOW(synch_str);
  FREE_NOW(phase);
  return status;
}

/**************************************************************
 * TclDispatch_command:
 **************************************************************/
typedef struct {
  int status;
  char *command;
} DispatchedCommand;

static void CommandDone(DispatchedCommand * command){
  if IS_NOT_OK(command->status) {
    char *msg = MdsGetMsg(command->status);
    fprintf(stderr, "Error: Command failed - '%s'\n"
	    "Error message was: %s\n", command->command, msg);
  }
  free(command->command);
  free(command);
}

EXPORT int TclDispatch_command(void *ctx, char **error, char **output __attribute__ ((unused))){
  INIT_TCLSTATUS,stat1=ServerPATH_DOWN;
  INIT_AND_FREE_ON_EXIT(char*,ident);
  INIT_AND_FREE_ON_EXIT(char*,cli);
  if IS_NOT_OK(cli_get_value(ctx, "SERVER", &ident)){
    status = MdsdclERROR;
    *error = strdup("Error: Missing server ident.\n");
    goto cleanup;
  }
  cli_get_value(ctx, "TABLE", &cli);
  INIT_AS_AND_FREE_ON_EXIT(DispatchedCommand*,command,calloc(1,sizeof(DispatchedCommand)));
  cli_get_value(ctx, "P1", &command->command);
  if(command->command){
    int sync = IS_OK(cli_present(ctx, "WAIT"));
    if (sync) {
      command->status = MDSplusSUCCESS;
      SYNCINIT;
      status = ServerDispatchCommand(SYNCPASS, ident, cli, command->command, CommandDone, command, &stat1, NULL, 0);
      if STATUS_OK SYNCWAIT;
    } else {
     command->status = ServerPATH_DOWN;
     status = ServerDispatchCommand(0, ident, cli, command->command, CommandDone, command, &command->status, NULL, 0);
    }
    if STATUS_NOT_OK {
      char *msg = MdsGetMsg(status);
      *error = malloc(100 + strlen(msg));
      sprintf(*error, "Error: Problem dispatching async command.\n" "Error message was: %s\n", msg);
    } else if (sync) status = stat1;
  } else {
    status = MdsdclMISSING_VALUE;
    free(command);
  }
  FREE_CANCEL(command);
cleanup: ;
  FREE_NOW(cli);
  FREE_NOW(ident);
  return status;
}

/***************************************************************
 * TclDispatch_check:
 ***************************************************************/
EXPORT int TclDispatch_check(void *ctx, char **error, char **output __attribute__ ((unused))){
  if IS_OK(ServerFailedEssential(dispatch_table, cli_present(ctx, "RESET"))) {
    *error = strdup("Error: A essential action failed!\n");
    return TclFAILED_ESSENTIAL;
  } else
    return TclNORMAL;
}
