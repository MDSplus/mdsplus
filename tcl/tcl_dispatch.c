#include        "tclsysdef.h"
#include        <servershr.h>
#include        <stdlib.h>
#include        <dbidef.h>
#include        <ncidef.h>

#ifdef vms
#include        <lib$routines.h>
#include        <ssdef.h>
#include        <starlet.h>
#define TdiIdentOf    TDI$IDENT_OF
#endif

extern char *ServerGetInfo(int full, char *server);

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

static void  *dispatch_table = 0;

extern void TclTextOut();

extern int ServerFailedEssential();

extern int TdiIdentOf();

	/****************************************************************
	 * TclDispatch_close:
	 ****************************************************************/
int TclDispatch_close()
   {
    static DYNAMIC_DESCRIPTOR(ident);

    if (!(cli_present("SERVER") & 1) && dispatch_table)
        ServerDispatchClose(dispatch_table);
    else
       {
        while (cli_get_value("SERVER",&ident) & 1)
            ServerCloseTrees(ident.dscA_pointer);
       }
    return 1;
   }



	/**************************************************************
	 * TclDispatch_build:
	 **************************************************************/
int TclDispatch_build()
   {
    int sts;
    static DYNAMIC_DESCRIPTOR(dsc_monitor);
    char  *monitor;
    void  *free_dispatch_table = dispatch_table;

    sts = (cli_get_value("MONITOR",&dsc_monitor) & 1);
    monitor = (sts&1) ? dsc_monitor.dscA_pointer : 0;
    dispatch_table = 0;
    if (free_dispatch_table)
        ServerFreeDispatchTable(free_dispatch_table);
    sts = ServerBuildDispatchTable(0,monitor,&dispatch_table);
    if (~sts & 1)
        MdsMsg(sts,"Error building table");
    return sts;
   }



	/***************************************************************
	 * TclDispatch:
	 ***************************************************************/

static void InitSyncEfn()
{
#ifdef vms
    lib$get_ef(&SyncEfn);
#endif
    SyncEfnInit = 1;
}
  

#ifdef vms
static void WaitfrEf(int efn) {sys$waitfr(SyncEfn);}
#else
static void WaitfrEf(int *id)
{
  ServerWait(*id);
}
#endif

int TclDispatch()
   {
    static DYNAMIC_DESCRIPTOR(ident);
    static DYNAMIC_DESCRIPTOR(treenode);
    int sts;
    int iostatus;
    int nid;
    int waiting = cli_present("WAIT") != CLI_STS_NEGATED;
    cli_get_value("NODE",&treenode);
    l2u(treenode.dscA_pointer,0);
    if (!SyncEfnInit) InitSyncEfn();
    sts = TreeFindNode(treenode.dscA_pointer,&nid);
    if (sts & 1)
       {
        static struct descriptor  niddsc =
                {4, DTYPE_NID, CLASS_S, (char *)0};
        niddsc.dscA_pointer = (char *) &nid;
        sts = TdiIdentOf(&niddsc,&ident MDS_END_ARG);
        if (sts & 1)
           {
            static char treename[13];
            static DESCRIPTOR(nullstr,"\0");
            static int shot;
            static DBI_ITM itmlst[] =
                           {{13,DbiNAME,treename,0},
                            {4,DbiSHOTID,&shot,0},
                            {0,0,0,0}};
            TreeGetDbi(itmlst);
            StrAppend(&ident,&nullstr);
            sts = ServerDispatchAction(SyncEfn,ident.dscA_pointer
                        ,treename,shot,nid,0,0
                        ,waiting ? &iostatus:0,0);
            if (sts & 1)
              {
              if (waiting)
                {
                WaitfrEf(SyncEfn);
                sts = iostatus;
		}
              }
           }
       }
    if (!(sts & 1))
        MdsMsg(sts,"Error dispatching %s",treenode.dscA_pointer);
    return sts;
   }



	/**************************************************************
	 * TclDispatch_abort_server:
	 * TclDispatch_stop_server:
	 * TclDispatch_start_server:
	 **************************************************************/
int TclDispatch_abort_server()
   {
    int sts = 1;
    static DYNAMIC_DESCRIPTOR(ident);

    while ((sts & 1) && (cli_get_value("SERVER_NAME",&ident) & 1))
        sts = ServerAbortServer(ident.dscA_pointer,0);
    if (~sts & 1)
        MdsMsg(sts,"Error from ServerAbortServer");
    return sts;
   }


int TclDispatch_stop_server()
   {
    int sts = 1;
    static DYNAMIC_DESCRIPTOR(ident);
    while (sts & 1 && cli_get_value("SERVNAM",&ident) & 1)
        sts = ServerStopServer(ident.dscA_pointer);
    if (~sts & 1)
        MdsMsg(sts,"Error from ServerStopServer");
    return sts;
   }


int TclDispatch_start_server()
   {
    int sts = 1;
    static DYNAMIC_DESCRIPTOR(ident);
    while (sts & 1 && cli_get_value("SERVER",&ident) & 1)
        sts = ServerStartServer(ident.dscA_pointer);
    if (~sts & 1)
        MdsMsg(sts,"Error from ServerStartServer");
    return sts;
   }



	/***************************************************************
	 * TclDispatch_set_server:
	 ***************************************************************/
int TclDispatch_set_server()
   {
    int sts = 1;
    int logqual;
    int statqual;
    int logging = 0;
    static DYNAMIC_DESCRIPTOR(ident);

    logqual = cli_present("LOG");
    statqual = cli_present("STATISTICS");
    if (logqual == CLI_STS_PRESENT)
       {
        static DYNAMIC_DESCRIPTOR(log_type);
        cli_get_value("LOG",&log_type);
        cli_get_value(&log_type,&log_type);
        if (log_type.dscA_pointer)
            sscanf(log_type.dscA_pointer,"%d",&logging);
        else
           {
            MdsMsg(0,"TclDispatch_set_server: error getting log_type");
           }
       }

    while (sts & 1 && cli_get_value("SERVER",&ident) & 1)
       {
        sts = ServerSetLogging(ident.dscA_pointer,(char)logging);
       }
    if (~sts & 1)
        MdsMsg(sts,"Error from ServerSetLogging for server %s",
            ident.dscA_pointer);
    return sts;
   }



	/**************************************************************
	 * TclDispatch_show_server:
	 **************************************************************/
int TclDispatch_show_server()
   {
    int sts = 1;
    static DYNAMIC_DESCRIPTOR(ident);
    int output = cli_present("OUTPUT") & 1;
    int full = cli_present("FULL") & 1;

    while (sts & 1 && cli_get_value("SERVER_NAME",&ident) & 1)
       {
        if (IS_WILD(ident.dscA_pointer))	/* contains wildcard?	*/
           {
            static DYNAMIC_DESCRIPTOR(server);
            void  *ctx = 0;
            while (0 /* ServerFindServers(&ctx,ident.dscA_pointer,&server) & 1 */)
               {
                if (output)
		{
                    char *info;
                    TclTextOut(info=ServerGetInfo(full,server.dscA_pointer));
                    free(info);
                }
               }
            str_free1_dx(&server);
           }
        else
           {
            if (output)
	    {
                char *info;
                TclTextOut(info=ServerGetInfo(full,ident.dscA_pointer));
                free(info);
            }
           }
       }
    str_free1_dx(&ident);
    if (~sts & 1)
        MdsMsg(sts,"Error getting server info");
    return sts;
   }



	/*****************************************************************
	 * TclDispatch_phase:
	 *****************************************************************/
int TclDispatch_phase()
   {
    static DYNAMIC_DESCRIPTOR(dsc_phase);
    static DYNAMIC_DESCRIPTOR(synch_str);
    static DYNAMIC_DESCRIPTOR(monitor_str);
    char  *monitor;
    int synch;
    int sts = 1;
    int noaction = cli_present("NOACTION") & 1;
    void (*output_rtn) () = cli_present("LOG") & 1 ? TclTextOut : 0;

    monitor = (cli_get_value("MONITOR",&monitor_str) & 1) ?
                                     monitor_str.dscA_pointer : 0;
    if (!SyncEfnInit) InitSyncEfn();
    cli_get_value("PHASE",&dsc_phase);
    cli_get_value("SYNCH",&synch_str);
    sscanf(synch_str.dscA_pointer,"%d",&synch);
    synch = synch >= 1 ? synch : 1;
    if (dispatch_table)
        sts = ServerDispatchPhase(SyncEfn,dispatch_table,
                dsc_phase.dscA_pointer,noaction,synch,output_rtn,monitor);
    if (~sts & 1)
        MdsMsg(sts,"Error from ServerDispatchPhase");
    return sts;
   }



	/**************************************************************
	 * TclDispatch_command:
	 **************************************************************/
typedef struct
   {
    int sts;
    struct descriptor  command;
   }  DispatchedCommand;

static void CommandDone(DispatchedCommand *command)
   {
    if (!(command->sts & 1))
       {
        MdsMsg(command->sts,"Command failed: '%s'",
            command->command.dscA_pointer);
       }
    str_free1_dx(&command->command);
    free(command);
    return;
   }


int TclDispatch_command()
   {
    static DYNAMIC_DESCRIPTOR(cli);
    DispatchedCommand *command = malloc(sizeof(DispatchedCommand));
    static DYNAMIC_DESCRIPTOR(ident);
    int sts = 1;
    int length;

    if (!SyncEfnInit) InitSyncEfn();
    cli_get_value("SERVER",&ident);
    cli_get_value("TABLE",&cli);
    command->command.dscW_length = 0;
    command->command.dscB_dtype = DTYPE_T;
    command->command.dscB_class = CLASS_D;
    command->command.dscA_pointer = 0;
    cli_get_value("P1",&command->command);
    if (cli_present("WAIT") & 1)
       {
        if (sts & 1)
           {
            sts = ServerDispatchCommand(SyncEfn,ident.dscA_pointer,
                    cli.dscA_pointer,command->command.dscA_pointer,
                    0,0,&command->sts,0);
            if (sts & 1)
               {
                    WaitfrEf(SyncEfn);
                    sts = command->sts;
               }
           }
        str_free1_dx(&command->command);
        free(command);
        if (~sts & 1)
            MdsMsg(sts,"Error from ServerDispatchCommand");
       }
    else
       {
        sts = ServerDispatchCommand(0,ident.dscA_pointer,cli.dscA_pointer,
                  command->command.dscA_pointer,CommandDone,command,
                  &command->sts,0);
        if (~sts & 1)
           {
            str_free1_dx(&command->command);
            free(command);
            MdsMsg(sts,"Error from ServerDispatchCommand (no-wait)");
           }
       }
    return sts;
   }



	/***************************************************************
	 * TclDispatch_check:
	 ***************************************************************/
int TclDispatch_check()
   {
    if (ServerFailedEssential(dispatch_table,cli_present("RESET")&1))
        return(MdsMsg(TCL_STS_FAILED_ESSENTIAL,0));
    else
        return TCL_STS_NORMAL;
   }
