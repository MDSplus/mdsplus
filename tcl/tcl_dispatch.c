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
#include <dcl.h>
#include <mds_stdarg.h>
#include <mdsdcl_messages.h>
#include <mdsshr.h>
#include <ncidef.h>
#include <pthread_port.h>
#include <servershr.h>
#include <strroutines.h>
#include <tcl_messages.h>
#include <treeshr.h>
#include <mdsmsg.h>

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

#define IS_WILD(T) (strcspn(T, "*%") < strlen(T))

#define INIT_TCLSTATUS INIT_STATUS_AS TclNORMAL

extern int ServerFailedEssential();

extern int TdiIdentOf();
#include "../treeshr/treeshrp.h"
#define DBID_TABLE (((PINO_DATABASE *)TreeDbid())->dispatch_table)
/****************************************************************
 * TclDispatch_close:
 ****************************************************************/
EXPORT int TclDispatch_close(void *ctx, char **error __attribute__((unused)),
                             char **output __attribute__((unused)))
{
  char *ident;
  if (IS_NOT_OK(cli_present(ctx, "SERVER")))
    ServerDispatchClose(DBID_TABLE);
  else
  {
    while (IS_OK(cli_get_value(ctx, "SERVER", &ident)))
    {
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
EXPORT int TclDispatch_build(void *ctx, char **error,
                             char **output __attribute__((unused)))
{
  INIT_TCLSTATUS;
  INIT_AND_FREE_ON_EXIT(char *, monitor);
  cli_get_value(ctx, "MONITOR", &monitor);
  status = ServerBuildDispatchTable(0, monitor, &DBID_TABLE);
  if (STATUS_NOT_OK)
  {
    char *msg = MdsGetMsg(status);
    *error = malloc(strlen(msg) + 100);
    sprintf(*error,
            "Error: problem building dispatch table\n"
            "Error message was: %s\n",
            msg);
  }
  FREE_NOW(monitor);
  return status;
}

static void syncwait(void *syncid_p) { ServerWait(*(int *)syncid_p); }

/***************************************************************
 * TclDispatch:
 ***************************************************************/
/***************************************************************
 * TclDispatch_command:
 ***************************************************************/
typedef struct
{
  char *treenode;
  void *svr;
  void *xd;
  int *sid;
} cln_act_t;
static void cln_act(void *cin)
{
  cln_act_t *c = (cln_act_t *)cin;
  free(c->treenode);
  if (c->svr)
    free_d(c->svr);
  if (c->xd)
    free_xd(c->xd);
  if (c->sid)
  {
    syncwait(c->sid);
    free(c->sid);
  }
}
EXPORT int TclDispatch(void *ctx, char **error,
                       char **output __attribute__((unused)))
{
  int status, sync, IOS;
  cln_act_t c = {0};
  pthread_cleanup_push(cln_act, (void *)&c);
  status = TclNORMAL;
  sync = cli_present(ctx, "WAIT") != MdsdclNEGATED;
  IOS = SsINTERNAL;
  if (IS_NOT_OK(cli_get_value(ctx, "NODE", &c.treenode)))
  {
    status = MdsdclERROR;
    *error = strdup("Error: Missing node path.\n");
    goto cleanup;
  }
  EMPTYXD(xd);
  c.xd = &xd;
  struct descriptor_d svr = {0, DTYPE_T, CLASS_D, 0};
  c.svr = &svr;
  int nid;
  status = TreeFindNode(c.treenode, &nid);
  if (STATUS_OK)
  {
    struct descriptor niddsc = {4, DTYPE_NID, CLASS_S, (char *)&nid};
    status = TdiIdentOf(&niddsc, &xd MDS_END_ARG);
    if (STATUS_OK)
      status = TdiData(&xd, &svr MDS_END_ARG);
    if (STATUS_OK)
    {
      static char treename[13];
      static DESCRIPTOR(nullstr, "\0");
      static int shot;
      static DBI_ITM itmlst[] = {{13, DbiNAME, treename, 0},
                                 {4, DbiSHOTID, &shot, 0},
                                 {0, 0, 0, 0}};
      TreeGetDbi(itmlst);
      StrAppend(&svr, (struct descriptor *)&nullstr);
      int *iostatusp;
      if (sync)
      {
        c.sid = calloc(1, sizeof(int));
        IOS = SsINTERNAL;
        iostatusp = &IOS;
      }
      else
        iostatusp = NULL;
      status = ServerDispatchAction(c.sid, svr.pointer, treename, shot, nid,
                                    NULL, NULL, iostatusp, NULL, NULL, 0);
    }
  }
  if (STATUS_NOT_OK)
  {
    char *msg = MdsGetMsg(status);
    *error = malloc(strlen(msg) + strlen(c.treenode) + 100);
    sprintf(*error,
            "Error dispatching node %s\n"
            "Error message was: %s\n",
            c.treenode, msg);
  }
cleanup:;
  pthread_cleanup_pop(1);
  if (STATUS_OK && sync)
    return IOS;
  return status;
}

/**************************************************************
 * TclDispatch_abort_server:
 * TclDispatch_stop_server:
 * TclDispatch_start_server:
 **************************************************************/
EXPORT int TclDispatch_abort_server(void *ctx, char **error,
                                    char **output __attribute__((unused)))
{
  INIT_TCLSTATUS;
  char *ident;
  while (STATUS_OK && IS_OK(cli_get_value(ctx, "SERVER_NAME", &ident)))
  {
    FREE_ON_EXIT(ident);
    status = ServerAbortServer(ident, 0);
    if (STATUS_NOT_OK && status)
    {
      char *msg = MdsGetMsg(status);
      *error = malloc(strlen(msg) + strlen(ident) + 1000);
      sprintf(*error,
              "Error: Problem aborting server '%s'\n"
              "Error message was: %s\n",
              ident, msg);
    }
    FREE_NOW(ident);
  }
  return status ? status : TclNORMAL;
}

EXPORT int TclDispatch_stop_server(void *ctx, char **error,
                                   char **output __attribute__((unused)))
{
  INIT_TCLSTATUS;
  char *ident;
  while (STATUS_OK && IS_OK(cli_get_value(ctx, "SERVNAM", &ident)))
  {
    FREE_ON_EXIT(ident);
    status = ServerStopServer(ident);
    if (STATUS_NOT_OK && status)
    {
      char *msg = MdsGetMsg(status);
      *error = malloc(strlen(msg) + strlen(ident) + 100);
      sprintf(*error,
              "Error: problem stopping server %s\n"
              "Error message was: %s\n",
              ident, msg);
    }
    FREE_NOW(ident);
  }
  return status ? status : TclNORMAL;
}

EXPORT int TclDispatch_start_server(void *ctx, char **error,
                                    char **output __attribute__((unused)))
{
  INIT_TCLSTATUS;
  char *ident;
  while (STATUS_OK && IS_OK(cli_get_value(ctx, "SERVER", &ident)))
  {
    FREE_ON_EXIT(ident);
    status = ServerStartServer(ident);
    if (STATUS_NOT_OK && status)
    {
      char *msg = MdsGetMsg(status);
      *error = malloc(strlen(msg) + strlen(ident) + 100);
      sprintf(*error,
              "Error: problem starting server %s\n"
              "Error message was: %s\n",
              ident, msg);
    }
    FREE_NOW(ident);
  }
  return status ? status : TclNORMAL;
}

/***************************************************************
 * TclDispatch_set_server:
 ***************************************************************/

static inline int setLogging(void *ctx, int logging, char **error)
{
  char *ident;
  int status;
  status = TclNORMAL;
  while (STATUS_OK && IS_OK(cli_get_value(ctx, "SERVER", &ident)))
  {
    FREE_ON_EXIT(ident);
    status = ServerSetLogging(ident, logging);
    if (STATUS_NOT_OK)
    {
      char *msg = MdsGetMsg(status);
      if (*error)
        *error = realloc(*error,
                         strlen(*error) + strlen(msg) + strlen(ident) + 100);
      else
        *error = malloc(strlen(msg) + strlen(ident) + 100);
      sprintf(*error,
              "Error: Problem setting logging on serv %s\n"
              "Error message was: %s\n",
              ident, msg);
    }
    FREE_NOW(ident);
  }
  return status;
}

EXPORT int TclDispatch_set_server(void *ctx, char **error,
                                  char **output __attribute__((unused)))
{
  int status;
  if (cli_present(ctx, "LOG") == MdsdclPRESENT)
  {
    INIT_AND_FREE_ON_EXIT(char *, log_type);
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
    status = setLogging(ctx, 0, error);
  return status;
}

/**************************************************************
 * TclDispatch_show_server:
 **************************************************************/
EXPORT int TclDispatch_show_server(void *ctx,
                                   char **error __attribute__((unused)),
                                   char **output)
{
  int dooutput = IS_OK(cli_present(ctx, "OUTPUT"));
  int full = IS_OK(cli_present(ctx, "FULL"));
  if (dooutput)
    *output = strdup("");
  char *ident = NULL;
  while (IS_OK(cli_get_value(ctx, "SERVER_NAME", &ident)))
  {
    if (IS_WILD(ident))
    { /* contains wildcard?     */
      void *ctx1 = NULL;
      char *server;
      while ((server = ServerFindServers(&ctx1, ident)))
      {
        tclAppend(output, "Checking server: ");
        tclAppend(output, server);
        tclAppend(output, "\n");
        mdsdclFlushOutput(*output);
        char *info = ServerGetInfo(full, ident);
        if (dooutput)
        {
          tclAppend(output, info);
          tclAppend(output, "\n");
        }
        free(info);
        free(server);
      }
    }
    else
    {
      tclAppend(output, "Checking server: ");
      tclAppend(output, ident);
      tclAppend(output, "\n");
      mdsdclFlushOutput(*output);
      char *info = ServerGetInfo(full, ident);
      if (dooutput)
      {
        tclAppend(output, info);
        tclAppend(output, "\n");
      }
      free(info);
    }
    free(ident);
  }
  return TclNORMAL;
}

static inline void printIt(char *output) { fprintf(stdout, "%s\n", output); }

/*****************************************************************
 * TclDispatch_phase:
 *****************************************************************/
EXPORT int TclDispatch_phase(void *ctx, char **error,
                             char **output __attribute__((unused)))
{
  int status;
  char *phase = NULL;
  char *synch_str = NULL;
  char *monitor = NULL;
  int synch;
  int noaction = IS_OK(cli_present(ctx, "NOACTION"));
  void (*output_rtn)() = IS_OK(cli_present(ctx, "LOG")) ? printIt : 0;
  cli_get_value(ctx, "MONITOR", &monitor);
  cli_get_value(ctx, "PHASE_NAME", &phase);
  cli_get_value(ctx, "SYNCH", &synch_str);
  sscanf(synch_str, "%d", &synch);
  synch = synch >= 1 ? synch : 1;
  if (DBID_TABLE)
    status = ServerDispatchPhase(NULL, DBID_TABLE, phase, (char)noaction, synch,
                                 output_rtn, monitor);
  else
    status = TclNO_DISPATCH_TABLE;
  free(monitor);
  free(synch_str);
  free(phase);
  if (STATUS_NOT_OK)
  {
    char *msg = MdsGetMsg(status);
    *error = malloc(strlen(msg) + 100);
    sprintf(*error,
            "Error: problem dispatching phase\n"
            "Error message was: %s\n",
            msg);
  }
  return status;
}

/**************************************************************
 * TclDispatch_command:
 **************************************************************/
typedef struct
{
  int status;
  char *command;
} DispatchedCommand;

static void CommandDone(DispatchedCommand *command)
{
  if (IS_NOT_OK(command->status))
  {
    char *msg = MdsGetMsg(command->status);
    fprintf(stderr,
            "Error: Command failed - '%s'\n"
            "Error message was: %s\n",
            command->command, msg);
  }
  free(command->command);
  free(command);
}

/***************************************************************
 * TclDispatch_command:
 ***************************************************************/
typedef struct
{
  char *svr;
  char *tab;
  DispatchedCommand *cmd;
  int *sid;
} cln_cmd_t;
static void cln_cmd(void *cin)
{
  cln_cmd_t *c = (cln_cmd_t *)cin;
  free(c->svr);
  free(c->tab);
  if (c->sid)
  {
    syncwait(c->sid);
    free(c->sid);
  }
  else
    free(c->cmd);
}
EXPORT int TclDispatch_command(void *ctx, char **error,
                               char **output __attribute__((unused)))
{
  int status, sync, IOS;
  cln_cmd_t c = {0};
  pthread_cleanup_push(cln_cmd, (void *)&c);
  status = TclNORMAL;
  sync = 0;
  IOS = SsINTERNAL;
  if (IS_NOT_OK(cli_get_value(ctx, "SERVER", &c.svr)))
  {
    status = MdsdclERROR;
    *error = strdup("Error: Missing server ident.\n");
    goto cleanup;
  }
  c.cmd = calloc(1, sizeof(DispatchedCommand));
  cli_get_value(ctx, "TABLE", &c.tab);
  cli_get_value(ctx, "P1", &c.cmd->command);
  if (c.cmd->command)
  {
    sync = IS_OK(cli_present(ctx, "WAIT"));
    int *iostatusp;
    if (sync)
    {
      c.sid = calloc(1, sizeof(int));
      c.cmd->status = IOS;
      iostatusp = &IOS;
    }
    else
    {
      c.cmd->status = ServerPATH_DOWN;
      iostatusp = &c.cmd->status;
    }
    status = ServerDispatchCommand(c.sid, c.svr, c.tab, c.cmd->command,
                                   CommandDone, c.cmd, iostatusp, NULL, 0);
    if (STATUS_NOT_OK)
    {
      MDSMSG("ServerDispatchCommand failed.");
      char *msg = MdsGetMsg(status);
      *error = malloc(100 + strlen(msg));
      sprintf(*error,
              "Error: Problem dispatching async command.\n"
              "Error message was: %s\n",
              msg);
    }
    c.cmd = NULL;
  }
  else
    status = MdsdclMISSING_VALUE;
cleanup:;
  pthread_cleanup_pop(1);
  if (STATUS_OK && sync)
    return IOS;
  return status;
}

/***************************************************************
 * TclDispatch_check:
 ***************************************************************/
EXPORT int TclDispatch_check(void *ctx, char **error,
                             char **output __attribute__((unused)))
{
  if (ServerFailedEssential(DBID_TABLE, cli_present(ctx, "RESET")))
  {
    *error = strdup("Error: A essential action failed!\n");
    return TclFAILED_ESSENTIAL;
  }
  else
    return TclNORMAL;
}
