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

static int SyncEfnInit = 0;
#ifdef vms
static int SyncEfn = 0;
#else
static int SyncId;
static int *SyncEfn = &SyncId;
#endif

static void *dispatch_table = 0;

extern int ServerFailedEssential();

extern int TdiIdentOf();

	/****************************************************************
	 * TclDispatch_close:
	 ****************************************************************/
EXPORT int TclDispatch_close(void *ctx, char **error __attribute__ ((unused)), char **output __attribute__ ((unused)))
{
  char *ident = 0;

  if (!(cli_present(ctx, "SERVER") & 1) && dispatch_table)
    ServerDispatchClose(dispatch_table);
  else {
    while (cli_get_value(ctx, "SERVER", &ident) & 1) {
      ServerCloseTrees(ident);
      free(ident);
    }
  }
  return 1;
}

	/**************************************************************
	 * TclDispatch_build:
	 **************************************************************/
EXPORT int TclDispatch_build(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int sts;
  char *monitor = 0;
  void *free_dispatch_table = dispatch_table;

  sts = (cli_get_value(ctx, "MONITOR", &monitor) & 1);
  dispatch_table = 0;
  if (free_dispatch_table)
    ServerFreeDispatchTable(free_dispatch_table);
  sts = ServerBuildDispatchTable(0, monitor, &dispatch_table);
  if (~sts & 1) {
    char *msg = MdsGetMsg(sts);
    *error = malloc(strlen(msg) + 100);
    sprintf(*error, "Error: problem building dispatch table\n" "Error message was: %s\n", msg);
  }
  if (monitor)
    free(monitor);
  return sts;
}

	/***************************************************************
	 * TclDispatch:
	 ***************************************************************/

static void InitSyncEfn()
{
  SyncEfnInit = 1;
}

static void WaitfrEf(int *id)
{
  ServerWait(*id);
}

EXPORT int TclDispatch(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  char *treenode = 0;
  int sts;
  int iostatus;
  int nid;
  int waiting = cli_present(ctx, "WAIT") != MdsdclNEGATED;
  cli_get_value(ctx, "NODE", &treenode);
  if (!SyncEfnInit)
    InitSyncEfn();
  sts = TreeFindNode(treenode, &nid);
  if (sts & 1) {
    struct descriptor niddsc = { 4, DTYPE_NID, CLASS_S, (char *)0 };
    EMPTYXD(xd);
    struct descriptor_d ident = { 0, DTYPE_T, CLASS_D, 0 };
    niddsc.pointer = (char *)&nid;
    sts = TdiIdentOf(&niddsc, &xd MDS_END_ARG);
    if (sts & 1)
      sts = TdiData(&xd, &ident MDS_END_ARG);
    MdsFree1Dx(&xd, 0);
    if (sts & 1) {
      static char treename[13];
      static DESCRIPTOR(nullstr, "\0");
      static int shot;
      static DBI_ITM itmlst[] = { {13, DbiNAME, treename, 0},
      {4, DbiSHOTID, &shot, 0},
      {0, 0, 0, 0}
      };
      TreeGetDbi(itmlst);
      StrAppend(&ident, (struct descriptor *)&nullstr);
      sts =
	  ServerDispatchAction(SyncEfn, ident.pointer, treename, shot, nid, 0, 0,
			       waiting ? &iostatus : 0, 0, 0);
      if (sts & 1) {
	if (waiting) {
	  WaitfrEf(SyncEfn);
	  sts = iostatus;
	}
      }
      StrFree1Dx(&ident);
    }
  }
  if (!(sts & 1)) {
    char *msg = MdsGetMsg(sts);
    *error = malloc(strlen(msg) + strlen(treenode) + 100);
    sprintf(*error, "Error dispatching node %s\n" "Error message was: %s\n", treenode, msg);
  }
  if (treenode)
    free(treenode);
  return sts;
}

	/**************************************************************
	 * TclDispatch_abort_server:
	 * TclDispatch_stop_server:
	 * TclDispatch_start_server:
	 **************************************************************/
EXPORT int TclDispatch_abort_server(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int sts = 1;
  char *ident = 0;

  while (cli_get_value(ctx, "SERVER_NAME", &ident) & 1) {
    sts = ServerAbortServer(ident, 0);
    if (~sts & 1) {
      char *msg = MdsGetMsg(sts);
      *error = malloc(strlen(msg) + strlen(ident) + 1000);
      sprintf(*error, "Error: Problem aborting server '%s'\n"
	      "Error message was: %s\n", ident, msg);
    }
    free(ident);
  }
  return sts;
}

EXPORT int TclDispatch_stop_server(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int sts = 1;
  char *ident = 0;
  while (sts & 1 && cli_get_value(ctx, "SERVNAM", &ident) & 1) {
    sts = ServerStopServer(ident);
    if (sts & 1) {
      free(ident);
      ident = 0;
    }
  }
  if (~sts & 1 && sts != 0) {
    char *msg = MdsGetMsg(sts);
    *error = malloc(strlen(msg) + strlen(ident) + 100);
    sprintf(*error, "Error: problem stopping server %s\n" "Error message was: %s\n", ident, msg);
    if (ident) {
      free(ident);
      ident = 0;
    }
  }
  return sts;
}

EXPORT int TclDispatch_start_server(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int sts = 1;
  char *ident = 0;
  while (sts & 1 && cli_get_value(ctx, "SERVER", &ident) & 1) {
    sts = ServerStartServer(ident);
    if (sts & 1) {
      free(ident);
      ident = 0;
    }
  }
  if (~sts & 1 && sts != 0) {
    char *msg = MdsGetMsg(sts);
    *error = malloc(strlen(msg) + strlen(ident) + 100);
    sprintf(*error, "Error: problem starting server %s\n" "Error message was: %s\n", ident, msg);
    if (ident) {
      free(ident);
      ident = 0;
    }
  }
  return sts;
}

	/***************************************************************
	 * TclDispatch_set_server:
	 ***************************************************************/
EXPORT int TclDispatch_set_server(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int sts = 1;
  int logqual;
  char logging = 0;
  char *ident = 0;

  logqual = cli_present(ctx, "LOG");
  if (logqual == MdsdclPRESENT) {
    char *log_type = 0;
    cli_get_value(ctx, "LOG", &log_type);
    if (strncasecmp(log_type, "statistics", strlen(log_type)) == 0) {
      logging = 2;
    } else if (strncasecmp(log_type, "actions", strlen(log_type)) == 0) {
      logging = 1;
    } else
      logging = 0;
    free(log_type);
  }
  while (sts & 1 && cli_get_value(ctx, "SERVER", &ident) & 1) {
    sts = ServerSetLogging(ident, logging);
    if (~sts & 1) {
      char *msg = MdsGetMsg(sts);
      if (*error)
	*error = realloc(*error, strlen(*error) + strlen(msg) + strlen(ident) + 100);
      else
	*error = malloc(strlen(msg) + strlen(ident) + 100);
      sprintf(*error, "Error: Problem setting logging on serv %s\n"
	      "Error message was: %s\n", ident, msg);
    }
    free(ident);
  }
  return sts;
}

	/**************************************************************
	 * TclDispatch_show_server:
	 **************************************************************/
EXPORT int TclDispatch_show_server(void *ctx, char **error __attribute__ ((unused)), char **output)
{
  int sts = 1;
  char *ident = 0;
  int dooutput = cli_present(ctx, "OUTPUT") & 1;
  int full = cli_present(ctx, "FULL") & 1;
  if (dooutput)
    *output = strdup("");
  while (sts & 1 && cli_get_value(ctx, "SERVER_NAME", &ident) & 1) {
    char *info = 0;
    if (IS_WILD(ident)) {	/* contains wildcard?     */
      void *ctx1 = 0;
      char *server = 0;
      while ((server = ServerFindServers(&ctx1, ident))) {
	tclAppend(output, "Checking server: ");
	tclAppend(output, server);
	tclAppend(output, "\n");
	mdsdclFlushOutput(*output);
	if (dooutput) {
	  tclAppend(output, info = ServerGetInfo(full, server));
	  tclAppend(output, "\n");
	} else
	  info = ServerGetInfo(full, server);
	if (server)
	  free(server);
	if (info)
	  free(info);
      }
    } else {
      tclAppend(output, "Checking server: ");
      tclAppend(output, ident);
      tclAppend(output, "\n");
      mdsdclFlushOutput(*output);
      if (dooutput) {
	tclAppend(output, info = ServerGetInfo(full, ident));
	tclAppend(output, "\n");
      } else
	info = ServerGetInfo(full, ident);
      if (info)
	free(info);

    }
    free(ident);
  }
  return sts;
}

static void printIt(char *output)
{
  fprintf(stdout, "%s\n", output);
}

	/*****************************************************************
	 * TclDispatch_phase:
	 *****************************************************************/
EXPORT int TclDispatch_phase(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  char *phase = 0;
  char *synch_str = 0;
  char *monitor = 0;
  int synch;
  int sts = 1;
  int noaction = cli_present(ctx, "NOACTION") & 1;
  void (*output_rtn) () = cli_present(ctx, "LOG") & 1 ? printIt : 0;

  cli_get_value(ctx, "MONITOR", &monitor);
  if (!SyncEfnInit)
    InitSyncEfn();
  cli_get_value(ctx, "PHASE_NAME", &phase);
  cli_get_value(ctx, "SYNCH", &synch_str);
  sscanf(synch_str, "%d", &synch);
  synch = synch >= 1 ? synch : 1;
  if (dispatch_table)
    sts = ServerDispatchPhase(SyncEfn, dispatch_table,
			      phase, (char)noaction, synch, output_rtn, monitor);
  else
    *error = strdup("Error: No dispatch table found. Forgot to do DISPATCH/BUILD?\n");
  if (~sts & 1) {
    char *msg = MdsGetMsg(sts);
    *error = malloc(strlen(msg) + 100);
    sprintf(*error, "Error: problem dispatching phase\n" "Error message was: %s\n", msg);
  }
  if (phase)
    free(phase);
  if (synch_str)
    free(synch_str);
  if (monitor)
    free(monitor);
  return sts;
}

	/**************************************************************
	 * TclDispatch_command:
	 **************************************************************/
typedef struct {
  int sts;
  char *command;
} DispatchedCommand;

static void CommandDone(DispatchedCommand * command)
{
  if (!(command->sts & 1)) {
    char *msg = MdsGetMsg(command->sts);
    fprintf(stderr, "Error: Command failed - '%s'\n"
	    "Error message was: %s\n", command->command, msg);
  }
  free(command->command);
  free(command);
  return;
}

EXPORT int TclDispatch_command(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  char *cli = NULL;
  char *ident = NULL;
  DispatchedCommand *command = calloc(1,sizeof(DispatchedCommand));
  int sts = 1;

  if (!SyncEfnInit)
    InitSyncEfn();
  cli_get_value(ctx, "SERVER", &ident);
  cli_get_value(ctx, "TABLE", &cli);
  cli_get_value(ctx, "P1", &command->command);
  if(command->command){
    if (cli_present(ctx, "WAIT") & 1) {
      if (sts & 1) {
        sts = ServerDispatchCommand(SyncEfn, ident, cli, command->command, 0, 0, &command->sts, 0);
        if (sts & 1) {
	  WaitfrEf(SyncEfn);
	  sts = command->sts;
        }
      }
      if (~sts & 1) {
        char *msg = MdsGetMsg(sts);
        *error = malloc(strlen(msg) + 100);
        sprintf(*error, "Error: Problem dispatching command\n" "Error message was: %s\n", msg);
        free(command->command);
        free(command);
      }
    } else {
      sts = ServerDispatchCommand(0, ident, cli,
	command->command, CommandDone, command, &command->sts, 0);
      if (~sts & 1) {
        char *msg = MdsGetMsg(sts);
        *error = malloc(strlen(msg) + 100);
        sprintf(*error, "Error: Problem dispatching command\n" "Error message was: %s\n", msg);
        free(command->command);
        free(command);
      }
    }
  } else {
    sts = MdsdclMISSING_VALUE;
    free(command);
  }
  if (cli)
    free(cli);
  if (ident)
    free(ident);
  return sts;
}

	/***************************************************************
	 * TclDispatch_check:
	 ***************************************************************/
EXPORT int TclDispatch_check(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  if (ServerFailedEssential(dispatch_table, cli_present(ctx, "RESET") & 1)) {
    *error = strdup("Error: A essential action failed!\n");
    return TclFAILED_ESSENTIAL;
  } else
    return TclNORMAL;
}
