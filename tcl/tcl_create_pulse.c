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
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif
#include "tcl_p.h"
#include <dbidef.h>
#include <dcl.h>
#include <mdsshr.h>
#include <ncidef.h>
#include <treeshr.h>
#include <usagedef.h>

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
                      , void *astprm, int *retStatus, int *netId, void
(*linkdown_handler) () , void (*before_ast) ()
    );
*/
typedef struct _server
{
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
      {4, NciPARENT_TREE, (unsigned char *)&parent_nid, &retlen}, {0, 0, 0, 0}};
  static NCI_ITM flag_itm[] = {
      {4, NciGET_FLAGS, (unsigned char *)&nci_flags, &retlen}, {0, 0, 0, 0}};
  if (nid != 0)
  {
    for (thisNid = nid; thisNid;)
    {
      TreeGetNci(thisNid, flag_itm);
      if ((nci_flags & NciM_INCLUDE_IN_PULSE) == 0)
      {
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
/**************************************************************
 * TclCreatePulse:
 **************************************************************/
EXPORT int TclCreatePulse(void *ctx, char **error,
                          char **output __attribute__((unused)))
{
  int shot;
  char *asciiShot = 0;
  int status = 1;
  int old_default;
  int conditional = cli_present(ctx, "CONDITIONAL") & 1;
  int dispatch = cli_present(ctx, "DISPATCH") & 1;
  int include = cli_present(ctx, "INCLUDE") & 1;
  int exclude = cli_present(ctx, "EXCLUDE") & 1;
  int nomain = cli_present(ctx, "NOMAIN") & 1;
  int i;
  int sts;
  cli_get_value(ctx, "SHOT", &asciiShot);
  sts = tclStringToShot(asciiShot, &shot, error);
  free(asciiShot);
  if (sts & 1)
  {
    if (include | exclude | nomain | conditional | dispatch)
    {
      int nids[256] = {0};
      int nid;
      int num = 0;
      TreeGetDefaultNid(&old_default);
      TreeSetDefaultNid(0);
      if (include)
      {
        num = !nomain;
        char *nodename = 0;
        while (cli_get_value(ctx, "INCLUDE", &nodename) & 1)
        {
          void *fctx = 0;
          while (TreeFindNodeWild(nodename, &nid, &fctx,
                                  (1 << TreeUSAGE_SUBTREE)) &
                 1)
          {
            if (!conditional || CheckCondition(nid))
              nids[num++] = nid;
          }
          free(nodename);
          TreeFindNodeEnd(&fctx);
        }
      }
      else
      {
        void *fctx = 0;
        while (num < 256 && TreeFindTagWild("TOP", &nid, &fctx))
          if (nid ? (!conditional || CheckCondition(nid)) : !nomain)
            nids[num++] = nid;
      }
      if (exclude)
      {
        char *nodename = 0;
        while (cli_get_value(ctx, "EXCLUDE", &nodename) & 1)
        {
          void *fctx = 0;
          while (TreeFindNodeWild(nodename, &nid, &fctx,
                                  (1 << TreeUSAGE_SUBTREE)) &
                 1)
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
          TreeFindNodeEnd(&fctx);
          free(nodename);
        }
      }
      TreeSetDefaultNid(old_default);
      if (num)
        status = TreeCreatePulseFile(shot, num, nids);
    }
    else
      status = TreeCreatePulseFile(shot, 0, 0);
  }
  else
    status = sts;
  return status;
}
