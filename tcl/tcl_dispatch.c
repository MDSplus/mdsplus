#include        <servershr.h>
#include        "tclsysdef.h"
#include        <stdlib.h>
#include        <dbidef.h>
#include        <ncidef.h>
#include        <string.h>

#include <strroutines.h>
extern int TdiData();
extern char *ServerGetInfo(int full, char *server);
extern int ServerFreeDispatchTable();

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

extern void TclTextOut();

extern int ServerFailedEssential();

extern int TdiIdentOf();

	/****************************************************************
	 * TclDispatch_close:
	 ****************************************************************/
int TclDispatch_close(void *ctx)
{
  char *ident=0;

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
int TclDispatch_build(void *ctx)
{
  int sts;
  char *monitor=0;
  void *free_dispatch_table = dispatch_table;

  sts = (cli_get_value(ctx, "MONITOR", &monitor) & 1);
  dispatch_table = 0;
  if (free_dispatch_table)
    ServerFreeDispatchTable(free_dispatch_table);
  sts = ServerBuildDispatchTable(0, monitor, &dispatch_table);
  if (~sts & 1)
    MdsMsg(sts, "Error building table");
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

int TclDispatch(void *ctx)
{
  char *ident=0;
  char *treenode=0;
  int sts;
  int iostatus;
  int nid;
  int waiting = cli_present(ctx, "WAIT") != CLI_STS_NEGATED;
  cli_get_value(ctx, "NODE", &treenode);
  if (!SyncEfnInit)
    InitSyncEfn();
  sts = TreeFindNode(treenode, &nid);
  if (sts & 1) {
    struct descriptor niddsc = { 4, DTYPE_NID, CLASS_S, (char *)0 };
    EMPTYXD(xd);
    struct descriptor ident = {0, DTYPE_T, CLASS_D, 0};
    niddsc.pointer = (char *)&nid;
    sts = TdiIdentOf(&niddsc, &xd MDS_END_ARG);
    if (sts & 1)
      sts = TdiData(&xd, &ident MDS_END_ARG);
    MdsFree1Dx(&xd,0);
    if (sts & 1) {
      static char treename[13];
      static DESCRIPTOR(nullstr, "\0");
      static int shot;
      static DBI_ITM itmlst[] = { {13, DbiNAME, treename, 0},
      {4, DbiSHOTID, &shot, 0},
      {0, 0, 0, 0}
      };
      TreeGetDbi(itmlst);
      StrAppend(&ident, &nullstr);
      sts =
	  ServerDispatchAction(SyncEfn, ident.pointer, treename, shot, nid, 0, 0,
			       waiting ? &iostatus : 0, 0, 0);
      if (sts & 1) {
	if (waiting) {
	  WaitfrEf(SyncEfn);
	  sts = iostatus;
	}
      }
    }
  }
  if (!(sts & 1))
    MdsMsg(sts, "Error dispatching %s", treenode);
  if (treenode)
    free(treenode);
  return sts;
}

	/**************************************************************
	 * TclDispatch_abort_server:
	 * TclDispatch_stop_server:
	 * TclDispatch_start_server:
	 **************************************************************/
int TclDispatch_abort_server(void *ctx)
{
  int sts = 1;
  char *ident=0;

  while ((sts & 1) && (cli_get_value(ctx, "SERVER_NAME", &ident) & 1)) {
    sts = ServerAbortServer(ident, 0);
    free(ident);
  }
  if (~sts & 1)
    MdsMsg(sts, "Error from ServerAbortServer");
  return sts;
}

int TclDispatch_stop_server(void *ctx)
{
  int sts = 1;
  char *ident=0;
  while (sts & 1 && cli_get_value(ctx, "SERVNAM", &ident) & 1) {
    sts = ServerStopServer(ident);
    free(ident);
  }
  if (~sts & 1 && sts != 0)
    MdsMsg(sts, "Error from ServerStopServer");
  else
    sts = 1;
  return sts;
}

int TclDispatch_start_server(void *ctx)
{
  int sts = 1;
  char *ident=0;
  while (sts & 1 && cli_get_value(ctx, "SERVER", &ident) & 1) {
    sts = ServerStartServer(ident);
    free(ident);
  }
  if (~sts & 1)
    MdsMsg(sts, "Error from ServerStartServer");
  return sts;
}

	/***************************************************************
	 * TclDispatch_set_server:
	 ***************************************************************/
int TclDispatch_set_server(void *ctx)
{
  int sts = 1;
  int logqual;
  int statqual;
  int logging = 0;
  char *ident=0;

  logqual = cli_present(ctx, "LOG");
  statqual = cli_present(ctx, "STATISTICS");
  if (logqual == CLI_STS_PRESENT) {
    char *log_type=0;
    cli_get_value(ctx, "LOG", &log_type);
    printf("deal with log types\n");
  }

  while (sts & 1 && cli_get_value(ctx, "SERVER", &ident) & 1) {
    sts = ServerSetLogging(ident, (char)logging);
    if (~sts & 1)
      MdsMsg(sts, "Error from ServerSetLogging for server %s", ident);
    free(ident);
  }
  return sts;
}

	/**************************************************************
	 * TclDispatch_show_server:
	 **************************************************************/
int TclDispatch_show_server(void *ctx)
{
  int sts = 1;
  char *ident=0;
  int output = cli_present(ctx, "OUTPUT") & 1;
  int full = cli_present(ctx, "FULL") & 1;

  while (sts & 1 && cli_get_value(ctx, "SERVER_NAME", &ident) & 1) {
    if (IS_WILD(ident)) {	/* contains wildcard?     */
      void *ctx1 = 0;
      char *server;
      while ((server = ServerFindServers(&ctx1, ident))) {
	if (output) {
	  char *info;
	  char *header = strcpy(malloc(100 + strlen(server)), "Checking server: ");
	  strcat(header, server);
	  TclTextOut(header);
	  free(header);
	  TclTextOut(info = ServerGetInfo(full, server));
	  free(info);
	}
      }
    } else {
      if (output) {
	char *info;
	TclTextOut(info = ServerGetInfo(full, ident));
	free(info);
      }
    }
    free(ident);
  }
  if (~sts & 1)
    MdsMsg(sts, "Error getting server info");
  return sts;
}

	/*****************************************************************
	 * TclDispatch_phase:
	 *****************************************************************/
int TclDispatch_phase(void *ctx)
{
  char *phase = 0;
  char *synch_str = 0;
  char *monitor = 0;
  int synch;
  int sts = 1;
  int noaction = cli_present(ctx, "NOACTION") & 1;
  void (*output_rtn) () = cli_present(ctx, "LOG") & 1 ? TclTextOut : 0;

  cli_get_value(ctx, "MONITOR", &monitor);
  if (!SyncEfnInit)
    InitSyncEfn();
  cli_get_value(ctx, "PHASE", &phase);
  cli_get_value(ctx, "SYNCH", &synch_str);
  sscanf(synch_str, "%d", &synch);
  synch = synch >= 1 ? synch : 1;
  if (dispatch_table)
    sts = ServerDispatchPhase(SyncEfn, dispatch_table,
			      phase, (char)noaction, synch, output_rtn, monitor);
  if (~sts & 1)
    MdsMsg(sts, "Error from ServerDispatchPhase");
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
    MdsMsg(command->sts, "Command failed: '%s'", command->command);
  }
  free(command->command);
  free(command);
  return;
}

int TclDispatch_command(void *ctx)
{
  char *cli = 0;
  char *ident = 0;
  DispatchedCommand *command = malloc(sizeof(DispatchedCommand));
  int sts = 1;

  if (!SyncEfnInit)
    InitSyncEfn();
  cli_get_value(ctx, "SERVER", &ident);
  cli_get_value(ctx, "TABLE", &cli);
  cli_get_value(ctx, "P1", &command->command);
  if (cli_present(ctx, "WAIT") & 1) {
    if (sts & 1) {
      sts = ServerDispatchCommand(SyncEfn, ident,
				  cli, command->command,
				  0, 0, &command->sts, 0);
      if (sts & 1) {
	WaitfrEf(SyncEfn);
	sts = command->sts;
      }
    }
    free(&command->command);
    free(command);
    if (~sts & 1)
      MdsMsg(sts, "Error from ServerDispatchCommand");
  } else {
    sts = ServerDispatchCommand(0, ident, cli,
				command->command, CommandDone, command,
				&command->sts, 0);
    if (~sts & 1) {
      free(&command->command);
      free(command);
      MdsMsg(sts, "Error from ServerDispatchCommand (no-wait)");
    }
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
int TclDispatch_check(void *ctx)
{
  if (ServerFailedEssential(dispatch_table, cli_present(ctx, "RESET") & 1))
    return (MdsMsg(TCL_STS_FAILED_ESSENTIAL, 0));
  else
    return TCL_STS_NORMAL;
}
