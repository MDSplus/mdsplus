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
#include <mdsplus/mdsconfig.h>
#include <string.h>
#include <stdlib.h>
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif
#include <dbidef.h>
#include <ncidef.h>
#include <usagedef.h>
#include <dcl.h>
#include <mdsshr.h>
#include <treeshr.h>

#include "tcl_p.h"

/**********************************************************************
* TCL_CREATE_PULSE.C --
*
* TclCreatePulse:  Create pulse file.
*
* History:
*  02-Feb-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/
/*
extern int ServerCreatePulse(int efn, char *server, char *treenam, int ishot,
		      void (*astRtn) ()
		      , void *astprm, int *retStatus, int *netId, void (*linkdown_handler) ()
		      , void (*before_ast) ()
    );
*/
typedef struct _server {
  char *server;
  int *nid;
  int current_nid;
  int num;
  int netid;
  int status;
  struct _server *next;
} Server;


extern int TdiExecute();

	/****************************************************************
         * CheckCondtion:
         ****************************************************************/
static int CheckCondition(int nid)
{
  int ans = 1;
  int thisNid;
  static int parent_nid;
  static int nci_flags;
  static int retlen;
  static NCI_ITM par_itm[] = {
    {4, NciPARENT_TREE, (unsigned char *)&parent_nid, &retlen}
    , {0, 0, 0, 0}
  };
  static NCI_ITM flag_itm[] = {
    {4, NciGET_FLAGS, (unsigned char *)&nci_flags, &retlen}
    , {0, 0, 0, 0}
  };
  if (nid != 0) {
    for (thisNid = nid; thisNid;) {
      TreeGetNci(thisNid, flag_itm);
      if ((nci_flags & NciM_INCLUDE_IN_PULSE) == 0) {
	ans = 0;
	break;
      }
      parent_nid = 0;
      TreeGetNci(thisNid, par_itm);
      thisNid = parent_nid;
    }
  }
  return ans;
}

	/***************************************************************
         * FreeServerList:
         ***************************************************************/
/*
static int DispatchCreatePulse(Server * s);
static Server *ServerList = 0;
static int Efn1 = 0;
static int Efn2 = 0;
static int Shot;
static int Status;

static void FreeServerList()
{
  Server *s;
  Server *next;
  for (s = ServerList; s; s = next) {
    next = s->next;
    if (s->server)
      free(s->server);
    free(s->nid);
    free(s);
  }
  ServerList = 0;
}
*/
	/****************************************************************
         * NidToTree:
         ****************************************************************/
/*
static char *NidToTree(int nid)
{
  static unsigned char tree[12 + 1];
  static NCI_ITM nci_itmlst[] = { {12, NciNODE_NAME, tree, 0}, {0, 0, 0, 0} };
  static DBI_ITM dbi_itmlst[] = { {12, DbiNAME, tree, 0}, {0, 0, 0, 0} };
  if (nid)
    TreeGetNci(nid, nci_itmlst);
  else
    TreeGetDbi(dbi_itmlst);
  return ((char *)tree);
}
*/
	/**************************************************************
         * PutOnRemainingList:
         **************************************************************/
/*
static void PutOnRemainingList(Server * s)
{
  int i;
  static char fmt[] = "Error creating pulse %s on server %s, will try\
 another server";
  char msg[128];

  sprintf(msg, fmt, NidToTree(s->current_nid), s->server);
  Tcl_TextOut(msg);
  ServerList->nid[ServerList->num++] = s->current_nid;
  for (i = 0; i < s->num; i++)
    ServerList->nid[ServerList->num++] = s->nid[i];
}
*/

	/****************************************************************
         * CreatePulse:
         ****************************************************************/
/*
static void CreatePulse(Server * s)
{
  int status = 1;
  if (s != ServerList) {
    if (!(s->status & 1)) {
      static char fmt[] = "Error creating pulse files, %s pulse\
 not created, status = 0x%08X";
      char msg[128];
      sprintf(msg, fmt, NidToTree(s->current_nid), s->status);
      Tcl_TextOut(msg);
    }

    if (s->num) {
      s->current_nid = s->nid[--s->num];
      if (!(DispatchCreatePulse(s) & 1))
	CreatePulse(ServerList);
    } else if (ServerList->num) {
      if (s->netid) {
	s->nid[0] = ServerList->nid[--ServerList->num];
	s->num = 1;
	CreatePulse(s);
      } else
	CreatePulse(ServerList);
    } else {
      s->netid = 0;
      CreatePulse(ServerList);
    }
  } else
  {
    fprintf(stderr, unixMsg, "ServerList");
  }
  return;
}
*/
	/****************************************************************
         * LinkDown:
         ****************************************************************/
/*
static void LinkDown(int *netid)
{
  if (ServerList) {
    Server *s;
    for (s = ServerList->next; s; s = s->next) {
      if (s->netid == *netid) {
	PutOnRemainingList(s);
	s->netid = 0;
	break;
      }
    }
    CreatePulse(ServerList);
  }
}
*/
	/****************************************************************
         * DispatchCreatePulse:	
         ****************************************************************/
/*
static int DispatchCreatePulse(Server * s)
{
  int status;

  status = ServerCreatePulse(Efn1, s->server, NidToTree(s->current_nid),
			     Shot, CreatePulse, s, &s->status, &s->netid, LinkDown, 0);
  if (~status & 1) {
    PutOnRemainingList(s);
    s->netid = 0;
  }
  return status;
}
*/
	/*****************************************************************
	 * TclCreatePulse_server:
	 *****************************************************************/
/*
int TclCreatePulse_server(void *ctx, char **error, char **output)
{
  Server *s;
  Server **nextptr;
  char *tree=0;
  if (ServerList && ServerList->netid)
    FreeServerList();
  if (!ServerList) {
    ServerList = malloc(sizeof(Server));
    ServerList->server = 0;
    ServerList->nid = malloc(256 * sizeof(int));
    ServerList->num = 0;
    ServerList->netid = 0;
    ServerList->next = 0;
    nextptr = &ServerList->next;
  } else {
    nextptr = &ServerList->next;
    s = ServerList;
    for (; s; nextptr = &s->next, s = s->next) ;
  }
  s = *nextptr = malloc(sizeof(Server));
  s->server = 0;
  s->nid = malloc(256 * sizeof(int));
  s->num = 0;
  s->next = 0;
  s->current_nid = -1;
  s->status = 1;
  s->netid = -1;
  cli_get_value(ctx, "P2", &s->server);
  while (cli_get_value(ctx, "TREE", &tree) & 1) {
    char *tag=strcpy(alloca(strlen(tree)+10),"\\");
    strcat(tag,tree);
    strcat(tag,"::TOP");
    int nid;
    if (TreeFindNode(tag, &nid) & 1)
      s->nid[s->num++] = nid;
    free(tree);
  }
  return 1;
}
*/
	/****************************************************************
         * DistributedCreate:
         ****************************************************************/

/*
static int DistributedCreate(int shot, int *nids, int num, char **error, char **output)
{

  fprintf(stderr, unixMsg, "DistributedCreate");
  return (0);
}
*/
	/**************************************************************
	 * TclCreatePulse:
	 **************************************************************/
EXPORT int TclCreatePulse(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int shot;
  char *asciiShot = 0;
  int status = 1;
  int old_default;
  int conditional = cli_present(ctx, "CONDITIONAL") & 1;
  int dispatch = cli_present(ctx, "DISPATCH") & 1;
  int i;
  int sts;
  cli_get_value(ctx, "SHOT", &asciiShot);
  sts = tclStringToShot(asciiShot, &shot, error);
  if (asciiShot)
    free(asciiShot);
  if (sts & 1) {
    if ((cli_present(ctx, "INCLUDE") | cli_present(ctx, "EXCLUDE") | cli_present(ctx, "NOMAIN") |
	 conditional | dispatch) & 1) {
      int nids[256] = { 0 };
      int nid;
      int num = !(cli_present(ctx, "NOMAIN") & 1);
      TreeGetDefaultNid(&old_default);
      TreeSetDefaultNid(0);
      if (cli_present(ctx, "INCLUDE") & 1) {
	char *nodename = 0;
	while (cli_get_value(ctx, "INCLUDE", &nodename) & 1) {
	  void *ctx = 0;
	  char *wnode = strcpy(alloca(strlen(nodename) + 5), "***.");
	  strcat(wnode, nodename);
	  while (TreeFindNodeWild(nodename, &nid, &ctx, (1 << TreeUSAGE_SUBTREE)) & 1) {
	    if (conditional) {
	      if (CheckCondition(nid))
		nids[num++] = nid;
	    } else
	      nids[num++] = nid;
	  }
	  free(nodename);
	  TreeFindNodeEnd(&ctx);
	}
      } else {
	void *ctx = 0;
	while (TreeFindNodeWild("***.*", &nid, &ctx, (1 << TreeUSAGE_SUBTREE)) & 1)
	  if (conditional) {
	    if (CheckCondition(nid))
	      nids[num++] = nid;
	  } else
	    nids[num++] = nid;
	TreeFindNodeEnd(&ctx);
      }
      if (cli_present(ctx, "EXCLUDE") & 1) {
	char *nodename = 0;
	while (cli_get_value(ctx, "EXCLUDE", &nodename) & 1) {
	  void *ctx = 0;
	  char *wnode = strcpy(alloca(strlen(nodename) + 5), "***.");
	  strcat(wnode, nodename);
	  while (TreeFindNodeWild(nodename, &nid, &ctx, (1 << TreeUSAGE_SUBTREE)) & 1) {
	    for (i = 0; i < num; i++) {
	      if (nids[i] == nid) {
		num--;
		for (; i < num; i++)
		  nids[i] = nids[i + 1];
		break;
	      }
	    }
	  }
	  TreeFindNodeEnd(&ctx);
	  free(wnode);
	  free(nodename);
	}
      }
      TreeSetDefaultNid(old_default);
      if (num) {
	/*      if (dispatch)
	   status = DistributedCreate(shot, nids, num, error, output);
	   else
	 */
	status = TreeCreatePulseFile(shot, num, nids);
      }
    } else
      status = TreeCreatePulseFile(shot, 0, 0);
  } else
    status = sts;
  return status;
}
