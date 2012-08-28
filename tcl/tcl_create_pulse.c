#include	<config.h>
#include        "tclsysdef.h"
#include        <dbidef.h>
#include        <ncidef.h>
#include        <usagedef.h>
#ifdef vms
#include        <starlet.h>
#endif

/**********************************************************************
* TCL_CREATE_PULSE.C --
*
* TclCreatePulse:  Create pulse file.
*
* History:
*  02-Feb-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/


int   ServerCreatePulse(
    int   efn
   ,struct descriptor  *dsc_server
   ,char  *treenam
   ,int   ishot
   ,void  (*astRtn)()
   ,void  *astprm
   ,int   *retStatus
   ,int   *netId
   ,void  (*linkdown_handler)()
   ,void  (*before_ast)()
   );

typedef struct _server
   {
    struct descriptor server;
    int   *nid;
    int   current_nid;
    int   num;
    int   netid;
    int   status;
    struct _server *next;
   }  Server;


static int   DispatchCreatePulse(Server *s);

#ifdef vms
#pragma extern_model __save
#pragma extern_model globalvalue
extern SS$_CONNECFAIL;
#pragma extern_model __restore
#define TdiExecute  TDI$EXECUTE
#else
static char  unixMsg[] = "\n==> unix not yet handling %s <==\n\n";
#endif


extern int   TdiExecute();


	/****************************************************************
         * CheckCondtion:
         ****************************************************************/
static int   CheckCondition(int   nid)
   {
    int   ans = 1;
    int   thisNid;
    static int   parent_nid;
    static int   nci_flags;
    static int   retlen;
    static NCI_ITM par_itm[] = {
            {4,NciPARENT_TREE,(unsigned char *) &parent_nid,&retlen}
           ,{0,0,0,0}
           };
    static NCI_ITM flag_itm[] = {
            {4,NciGET_FLAGS,(unsigned char *) &nci_flags,&retlen}
           ,{0,0,0,0}
           };
    if (nid != 0)
       {
        for (thisNid = nid; thisNid;)
           {
            TreeGetNci(thisNid,flag_itm);
            if ((nci_flags & NciM_INCLUDE_IN_PULSE) == 0)
               {
                ans = 0;
                break;
               }
            parent_nid = 0;
            TreeGetNci(thisNid,par_itm);
            thisNid = parent_nid;
           }
       }
    return ans;
   }



	/***************************************************************
         * FreeServerList:
         ***************************************************************/
static Server *ServerList = 0;
static int   Efn1 = 0;
static int   Efn2 = 0;
static int   Shot;
static int   Status;

static void  FreeServerList()
   {
    Server *s;
    Server *next;
    for (s = ServerList; s; s = next)
       {
        next = s->next;
        str_free1_dx(&s->server);
        free(s->nid);
        free(s);
       }
    ServerList = 0;
   }



	/****************************************************************
         * NidToTree:
         ****************************************************************/
static char  *NidToTree(int   nid)
   {
    static unsigned char tree[12+1];
    static NCI_ITM nci_itmlst[] = {{12, NciNODE_NAME, tree, 0},{0,0,0,0}};
    static DBI_ITM dbi_itmlst[] = {{12, DbiNAME, tree, 0},{0,0,0,0}};
    if (nid)
        TreeGetNci(nid,nci_itmlst);
    else
        TreeGetDbi(dbi_itmlst);
    return((char *)tree);
   }



	/**************************************************************
         * PutOnRemainingList:
         **************************************************************/
static void  PutOnRemainingList(Server *s)
   {
    int   i;
    static char  fmt[] = "Error creating pulse %s on server %s, will try\
 another server";
    char  msg[128];

    sprintf(msg,fmt,NidToTree(s->current_nid),s->server.dscA_pointer);
    TclTextOut(msg);
    ServerList->nid[ServerList->num++] = s->current_nid;
    for (i = 0; i < s->num; i++)
        ServerList->nid[ServerList->num++] = s->nid[i];
   }



	/****************************************************************
         * CreatePulse:
         ****************************************************************/
static void  CreatePulse(Server *s)
   {
    int   status = 1;
    if (s != ServerList)
       {
        if (!(s->status & 1))
           {
            static char  fmt[] = "Error creating pulse files, %s pulse\
 not created, status = 0x%08X";
            char  msg[128];
            sprintf(msg,fmt,NidToTree(s->current_nid),s->status);
            TclTextOut(msg);
           }

        if (s->num)
           {
            s->current_nid = s->nid[--s->num];
            if (!(DispatchCreatePulse(s) & 1))
                CreatePulse(ServerList);
           }
        else if (ServerList->num)
           {
            if (s->netid)
               {
                s->nid[0] = ServerList->nid[--ServerList->num];
                s->num = 1;
                CreatePulse(s);
               }
            else
                CreatePulse(ServerList);
           }
        else
           {
            s->netid = 0;
            CreatePulse(ServerList);
           }
       }
    else
#ifdef vms
       {
        Server *busy_s = ServerList->next;
        for ( ; busy_s && !busy_s->netid ; busy_s = busy_s->next)
            ;
        if (ServerList->num && !busy_s)
           {
            static char fmt[] = "Error creating pulse files, %s pulse not\
 created, no servers available";
            char  msg[128];
            for (i = 0; i < ServerList->num; i++)
               {
                sprintf(msg,fmt,NidToTree(ServerList->nid[i]));
                TclTextOut(msg);
               }
            Status = SS$_CONNECFAIL;
            sys$setef(Efn2);
           }
        else if (!ServerList->num && !busy_s)
           {
            Status = 1;
            sys$setef(Efn2);
           }
       }
#else
       {
        fprintf(stderr,unixMsg,"ServerList");
       }
#endif
    return;
   }



	/****************************************************************
         * LinkDown:
         ****************************************************************/
static void  LinkDown(int   *netid)
   {
    if (ServerList)
       {
        Server *s;
        for (s = ServerList->next; s; s = s->next)
           {
            if (s->netid == *netid)
               {
                PutOnRemainingList(s);
                s->netid = 0;
                break;
               }
           }
        CreatePulse(ServerList);
       }
   }



	/****************************************************************
         * DispatchCreatePulse:	
         ****************************************************************/
static int   DispatchCreatePulse(Server *s)
   {
    int   status;

    status = ServerCreatePulse(Efn1,&s->server,NidToTree(s->current_nid),
                Shot,CreatePulse,s,&s->status,&s->netid,LinkDown,0);
    if (~status & 1)
       {
        PutOnRemainingList(s);
        s->netid = 0;
       }
    return status;
   }



	/*****************************************************************
	 * TclCreatePulse_server:
	 *****************************************************************/
int   TclCreatePulse_server()
   {
    Server *s;
    Server **nextptr;
    static DYNAMIC_DESCRIPTOR(dsc_tree);
    static DYNAMIC_DESCRIPTOR(dsc_emptyString);
    if (ServerList && ServerList->netid)
        FreeServerList();
    if (!ServerList)
       {
        ServerList = malloc(sizeof(Server));
        ServerList->server = dsc_emptyString;
        ServerList->nid = malloc(256 * sizeof(int));
        ServerList->num = 0;
        ServerList->netid = 0;
        ServerList->next = 0;
        nextptr = &ServerList->next;
       }
    else
       {
        nextptr = &ServerList->next;
        s = ServerList;
        for ( ; s ; nextptr=&s->next,s=s->next)
            ;
       }
    s = *nextptr = malloc(sizeof(Server));
    s->server = dsc_emptyString;
    s->nid = malloc(256 * sizeof(int));
    s->num = 0;
    s->next = 0;
    s->current_nid = -1;
    s->status = 1;
    s->netid = -1;
    cli_get_value("P2",&s->server);
    while (cli_get_value("TREE",&dsc_tree) & 1)
       {
        static DYNAMIC_DESCRIPTOR(dsc_tag);
        int   nid;
        str_concat(&dsc_tag,"\\",&dsc_tree,"::TOP",0);
        if (TreeFindNode(dsc_tag.dscA_pointer,&nid) & 1)
          s->nid[s->num++] = nid;
       }
    return 1;
   }



	/****************************************************************
         * DistributedCreate:
         ****************************************************************/
static int   DistributedCreate(int   shot,int   *nids,int   num)
   {

#ifdef vms
    if (shot)
        Shot = shot;
    else
       {
        Shot = TreeGetCurrentShotId(NidToTree(0));
        if (!(status & 1)) return status;
       }

    if (!ServerList || ServerList->netid == -1)
       {
        static char  msg[] = "Use DEFINE SERVER commands prior\
 to attempting to use the /DISPATCH qualifier";
        TclTextOut(msg);
        return 1;
       }
    if (!Efn1)
       {
        lib$get_ef(&Efn1);
        lib$get_ef(&Efn2);
       }
    ServerList->netid = -1;
    for (s = ServerList->next; s; s = s->next)
       {
        int   j;
        for (i = 0; i < s->num; i++)
           {
            for (j = 0; j < num && s->nid[i] != nids[j]; j++);
            if (j < num)
                nids[j] = -1;
            else
                s->nid[i] = -1;
           }
        for (i = 0; i < s->num;)
           {
            if (s->nid[i] == -1)
               {
                s->num--;
                for (j = i; j < s->num; j++)
                    s->nid[j] = s->nid[j + 1];
               }
            else
                i++;
           }
       }
    for (i = 0; i < num; i++)
       {
        if (nids[i] != -1)
            ServerList->nid[ServerList->num++] = nids[i];
       }
    sys$clref(Efn2);
    sys$setast(0);
    for (s = ServerList->next; s; s = s->next)
        CreatePulse(s);
    sys$setast(1);
    sys$waitfr(Efn2);
    lib$free_ef(&Efn1);
    lib$free_ef(&Efn2);
    Efn1 = Efn2 = 0;
    FreeServerList();
    return 1;
#else
    fprintf(stderr,unixMsg,"DistributedCreate");
    return(0);
#endif
   }



	/**************************************************************
	 * TclCreatePulse:
	 **************************************************************/
int   TclCreatePulse()
   {
    static int   shot;
    static DESCRIPTOR_LONG(dsc_shot,&shot);
    static DYNAMIC_DESCRIPTOR(dsc_nodename);
    static DYNAMIC_DESCRIPTOR(dsc_asciiShot);
    int   status = 1;
    int   old_default;
    int   conditional = cli_present("CONDITIONAL") & 1;
    int   dispatch = cli_present("DISPATCH") & 1;
    int   i;
    int   sts;

    cli_get_value("SHOT",&dsc_asciiShot);
#ifdef vms
    dsc_asciiShot.dscB_class = CLASS_S;		/* vms: malloc vs str$	*/
    sts = TdiExecute(&dsc_asciiShot,&dsc_shot MDS_END_ARG);
    dsc_asciiShot.dscB_class = CLASS_D;
#else
    sts = TdiExecute(&dsc_asciiShot,&dsc_shot MDS_END_ARG);
#endif
    if (sts & 1)
       {
        if ((cli_present("INCLUDE") | cli_present("EXCLUDE") | cli_present("NOMAIN") | conditional | dispatch) & 1)
           {
            int   nids[256] = {0};
            int   nid;
            int   num = !(cli_present("NOMAIN") & 1);
            TreeGetDefaultNid(&old_default);
            TreeSetDefaultNid(0);
            if (cli_present("INCLUDE") & 1)
               {
                while (cli_get_value("INCLUDE",&dsc_nodename) & 1)
                   {
                    void  *ctx = 0;
                    str_prefix(&dsc_nodename,"***.");
                    while (TreeFindNodeWild(dsc_nodename.dscA_pointer,&nid,&ctx,(1 << TreeUSAGE_SUBTREE)) & 1)
                       {
                        if (conditional)
                           {
                            if (CheckCondition(nid))
                                nids[num++] = nid;
                           }
                        else
                            nids[num++] = nid;
                       }
                    TreeFindNodeEnd(&ctx);
                   }
               }
            else
               {
                void  *ctx = 0;
                while (TreeFindNodeWild("***.*",&nid,&ctx,(1 << TreeUSAGE_SUBTREE)) & 1)
                  if (conditional)
                     {
                      if (CheckCondition(nid))
                          nids[num++] = nid;
                     }
                  else
                      nids[num++] = nid;
                TreeFindNodeEnd(&ctx);
               }
            if (cli_present("EXCLUDE") & 1)
               {
                while (cli_get_value("EXCLUDE",&dsc_nodename) & 1)
                   {
                    void  *ctx = 0;
                    str_prefix(&dsc_nodename,"***.");
                    while (TreeFindNodeWild(dsc_nodename.dscA_pointer,&nid,&ctx,(1 << TreeUSAGE_SUBTREE)) & 1)
                       {
                        for (i = 0; i < num; i++)
                           {
                            if (nids[i] == nid)
                               {
                                num--;
                                for (; i < num; i++)
                                    nids[i] = nids[i + 1];
                                break;
                               }
                           }
                       }
                    TreeFindNodeEnd(&ctx);
                   }
               }
            TreeSetDefaultNid(old_default);
            if (num)
               {
                if (dispatch)
                    status = DistributedCreate(shot,nids,num);
                else
                    status = TreeCreatePulseFile(shot,num,nids);
               }
           }
        else
            status = TreeCreatePulseFile(shot,0,0);
       }
    else
      status = sts;
#ifdef vms
    if (~status & 1)
        lib$signal(status,0,0);
#endif
    return status;
   }
