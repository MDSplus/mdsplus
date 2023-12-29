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
#include <mdsplus/mdsplus.h>
#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <wspiapi.h>
#else
/* Debian Bug report logs - #639078
 * pommed: FTBFS: fcntl.h:172:8: error: redefinition of 'struct flock'
 *
 * workaround: define flock so linux/fcntl.h will define lx_flock instead
 */
//// alpine does not linux/fcntl.h
// # define flock lx_flock
// # include <linux/fcntl.h>
// # undef flock

#ifndef F_OFD_SETLK
#ifdef _MACOSX
// Open File Descriptor (OFD) locks have existed as a private / undocumented
// feature in MacOS releases since 2015.  For more details, refer to the
// comments at the end of Issue #2599 and the earlier PRs #2163 and #2198.
#define F_OFD_GETLK 92
#define F_OFD_SETLK 90
#define F_OFD_SETLKW 91
#else
// The usual defines for Linux
#define F_OFD_GETLK 36
#define F_OFD_SETLK 37
#define F_OFD_SETLKW 38
#endif
#endif

#endif
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include <dbidef.h>
#include <inttypes.h>
#include <libroutines.h>
#include <mds_stdarg.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <mdstypes.h>
#include <ncidef.h>

#include <treeshr.h>
#include "treeshrp.h"
#include "treethreadstatic.h"

// #define DEBUG
#include <_mdsshr.h>

static inline char *replaceBackslashes(char *filename)
{
  char *ptr;
  while ((ptr = strchr(filename, '\\')) != NULL)
    *ptr = '/';
  return filename;
}

static int remote_connect(char *server)
{
#define CONMSG(TYP, PRI, ...) TYP("Host(conid=%d, links=?, unique='%s'), server='%s'" PRI, conid, unique, server, __VA_ARGS__)
  int conid = -1;
  MDSSHR_LOAD_LIBROUTINE_LOCAL(MdsIpShr, ReuseCheck, return conid, int, (char *, char *, int));
  MDSSHR_LOAD_LIBROUTINE_LOCAL(MdsIpShr, ConnectToMds, return conid, int, (char *));
  char unique[HOST_UNIQUE_SIZE] = "";
  if (ReuseCheck(server, unique, HOST_UNIQUE_SIZE) < 0)
  {
    conid = ConnectToMds(server);
    if (conid == -1)
    {
      CONMSG(MDSWRN, ": %s", "error");
    }
    else
    {
      CONMSG(MDSDBG, ": %s", "new");
    }
  }
  else
  {
    Host *host = NULL;
    TREETHREADSTATIC_INIT;
    for (host = TREE_HOSTLIST; host; host = host->next)
    {
      if (!strncmp(host->unique, unique, HOST_UNIQUE_SIZE))
      {
        conid = host->conid;
        host->links++;
        MDSDBG(HOST_PRI ", server='%s': found", HOST_VAR(host), server);
        return conid;
      }
    }
    conid = ConnectToMds(unique);
    if (conid == -1)
    {
      CONMSG(MDSWRN, ": %s", "error");
    }
    else
    {
      host = malloc(sizeof(Host));
      host->conid = conid;
      host->links = 1;
      strncpy(host->unique, unique, HOST_UNIQUE_SIZE);
      host->next = TREE_HOSTLIST;
      TREE_HOSTLIST = host;
      MDSDBG(HOST_PRI ", server='%s': new", HOST_VAR(host), server);
    }
  }
  return conid;
#undef CONMSG
}

/// remote_disconnect
/// Used to close a connection either forcefully or indirect by reducing the
/// connection counter. If the counter drops to 0, this method shall arm the
/// cleanup cycle. If the cleanup cycle is already armed, it is not required to
/// reset it.
static int remote_disconnect(int conid, int force)
{
  if (conid == -1)
    return TreeSUCCESS;
  TREETHREADSTATIC_INIT;
  Host **phost = &TREE_HOSTLIST, *host = TREE_HOSTLIST;
  for (; host; phost = &host->next, host = host->next)
  {
    if (host->conid == conid)
    {
      host->links--;
      if (host->links > 0 && !force)
      {
        MDSDBG(HOST_PRI " kept", HOST_VAR(host));
      }
      else
      {
        if (force)
        {
          *phost = host->next;
          MDSWRN(HOST_PRI " disconnected", HOST_VAR(host));
          destroy_host(host);
        }
        else
        {
          MDSDBG(HOST_PRI " idle", HOST_VAR(host));
        }
      }
      break;
    }
  }
  return TreeSUCCESS;
}

void TreeCleanupConnections()
{
  TREETHREADSTATIC_INIT;
  Host **phost = &TREE_HOSTLIST, *host = TREE_HOSTLIST;
  for (; host; host = *phost)
  {
    if (host->links <= 0)
    {
      *phost = host->next;
      MDSDBG(HOST_PRI " cleaned up", HOST_VAR(host));
      destroy_host(host);
    }
    else
    {
      phost = &host->next;
    }
  }
}

///////////////////////////////////////////////////////////////////
///////OLD THICK CLIENT////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

#define min(a, b) (((a) < (b)) ? (a) : (b))
struct descrip
{
  char dtype;
  char ndims;
  int dims[MAX_DIMS];
  int length;
  void *ptr;
};

#define STR2DESCRIP(str) \
  (struct descrip) { DTYPE_T, 0, {0}, strlen(str), (char *)str }
static int MdsValue(int conid, char *exp, ...)
{
  MDSSHR_LOAD_LIBROUTINE_LOCAL(MdsIpShr, _MdsValue, return status, int, ());
  int nargs;
  struct descrip *arglist[256];
  VA_LIST_NULL(arglist, nargs, 1, -1, exp);
  struct descrip expd = STR2DESCRIP(exp);
  arglist[0] = &expd;
  return _MdsValue(conid, nargs, arglist, arglist[nargs]);
}

static int MdsValueDsc(int conid, char *exp, ...)
{
  MDSSHR_LOAD_LIBROUTINE_LOCAL(MdsIpShr, MdsIpGetDescriptor, return status, int, ());
  int nargs;
  struct descrip *arglist[256];
  VA_LIST_NULL(arglist, nargs, 0, -1, exp);
  return MdsIpGetDescriptor(conid, exp, nargs, arglist, arglist[nargs]);
}

inline static void MdsIpFree(void *ptr)
{
  // used to free ans.ptr returned by MdsValue
  static void (*mdsIpFree)(void *) = NULL;
  MDSSHR_LOAD_LIBROUTINE(mdsIpFree, MdsIpShr, MdsIpFree, abort());
  mdsIpFree(ptr);
}

inline static void MdsIpFreeDsc(struct descriptor_xd *xd)
{
  // used to free ans.ptr returned by MdsValueDsc
  static void (*mdsIpFreeDsc)(struct descriptor_xd *) = NULL;
  MDSSHR_LOAD_LIBROUTINE(mdsIpFreeDsc, MdsIpShr, MdsIpFreeDsc, abort());
  mdsIpFreeDsc(xd);
}

inline static int tree_open(PINO_DATABASE *dblist, int conid,
                            const char *treearg)
{
  int status;
  struct descrip ans = {0};
  char exp[256];
  sprintf(exp, "TreeShr->TreeOpen(ref($),val(%d),val(0))", dblist->shotid);
  struct descrip tree = STR2DESCRIP(treearg);
  status = MdsValue(conid, exp, &tree, &ans, NULL);
  if (ans.ptr)
  {
    if (STATUS_OK)
      status = (ans.dtype == DTYPE_L) ? *(int *)ans.ptr : TreeFAILURE;
    MdsIpFree(ans.ptr);
  }
  return status;
}

int ConnectTreeRemote(PINO_DATABASE *dblist, char const *tree,
                      char *subtree_list, char *logname)
{
  int conid;
  logname[strlen(logname) - 2] = '\0';
  int status = TreeSUCCESS;
  conid = remote_connect(logname);
  if (conid != -1)
  {
    status = tree_open(dblist, conid, subtree_list ? subtree_list : tree);
    if (STATUS_OK)
    {
      TREE_INFO *info;
      /***********************************************
         Get virtual memory for the tree
         information structure and zero the structure.
        ***********************************************/
      for (info = dblist->tree_info; info && strcmp(tree, info->treenam);
           info = info->next_info)
        ;
      if (!info)
      {
        info = malloc(sizeof(TREE_INFO) + sizeof(TREE_HEADER));
        if (info)
        {
          memset(info, 0, sizeof(*info) + sizeof(TREE_HEADER));
          info->blockid = TreeBLOCKID;
          info->flush = (dblist->shotid == -1);
          info->header = (TREE_HEADER *)&info[1];
          info->treenam = strdup(tree);
          TreeCallHookFun("TreeHook", "OpenTree", tree, dblist->shotid, NULL);
          TreeCallHook(OpenTree, info, 0);
          info->channel = conid;
          dblist->tree_info = info;
          dblist->remote = 1;
          status = TreeSUCCESS;
        }
        else
          status = TreeFILE_NOT_FOUND;
      }
    }
    else
      remote_disconnect(conid, B_FALSE);
  }
  else
    status = TreeCONNECTFAIL;
  return status;
}

int SetStackSizeRemote(PINO_DATABASE *dbid __attribute__((unused)),
                       int stack_size __attribute__((unused)))
{
  return 1;
}

int CloseTreeRemote(PINO_DATABASE *dblist,
                    int call_host __attribute__((unused)))
{
  struct descrip ans = {0};
  struct descrip tree = STR2DESCRIP(dblist->experiment);
  int status;
  char exp[80];
  sprintf(exp, "TreeShr->TreeClose(ref($),val(%d))", dblist->shotid);
  status = MdsValue(dblist->tree_info->channel, exp, &tree, &ans, NULL);
  if (ans.ptr)
  {
    status = (ans.dtype == DTYPE_L) ? *(int *)ans.ptr : 0;
    MdsIpFree(ans.ptr);
  }
  remote_disconnect(dblist->tree_info->channel, 0);
  if (dblist->tree_info)
  {
    free(dblist->tree_info->treenam);
    free(dblist->tree_info);
    dblist->tree_info = NULL;
  }
  if (dblist->experiment)
  {
    free(dblist->experiment);
    dblist->experiment = NULL;
  }
  if (dblist->main_treenam)
  {
    free(dblist->main_treenam);
    dblist->main_treenam = NULL;
  }
  dblist->remote = 0;
  return status;
}

int CreatePulseFileRemote(PINO_DATABASE *dblist, int shot, int num, int *nids)
{
  char exp[80];
  sprintf(exp, "TreeShr->TreeCreatePulseFile(val(%d),val(%d),ref($))", shot,
          num);
  struct descrip arr = {DTYPE_L, 1, {num}, sizeof(int), (void *)nids};
  struct descrip ans = {0};
  int status = MdsValue(dblist->tree_info->channel, exp, &arr, &ans, NULL);
  if (ans.ptr)
  {
    status = (ans.dtype == DTYPE_L) ? *(int *)ans.ptr : 0;
    MdsIpFree(ans.ptr);
  }
  return status;
}

int GetRecordRemote(PINO_DATABASE *dblist, int nid_in,
                    struct descriptor_xd *dsc)
{
  int status;
  char exp[80];
  sprintf(exp, "getnci(%d,'RECORD')", nid_in);
#ifdef STRICT_MDSIPFREE
  EMPTYXD(ans);
  status = MdsValueDsc(dblist->tree_info->channel, exp, &ans, NULL);
  if (STATUS_OK)
    MdsCopyDxXd(&ans, dsc);
  MdsIpFreeDsc(&ans);
#else
  status = MdsValueDsc(dblist->tree_info->channel, exp, dsc, NULL);
#endif
  if (STATUS_OK && !dsc->pointer)
    status = TreeNODATA;
  return status;
}

int LeadingBackslash(char const *path)
{
  size_t i;
  size_t len = strlen(path);
  for (i = 0; i < len; i++)
  {
    if ((path[i] != 32) && (path[i] != 9))
    {
      if (path[i] == '\\')
        return 1;
      else
        return 0;
    }
  }
  return 0;
}

int FindNodeRemote(PINO_DATABASE *dblist, char const *path, int *outnid)
{
  struct descrip ans = {0};
  int status;
  INIT_AND_FREE_ON_EXIT(char *, exp);
  exp = malloc(strlen(path) + 32);
  sprintf(exp, "getnci(%s%s,'nid_number')", path[0] == '-' ? "." : "", path);
  status = MdsValue(dblist->tree_info->channel, exp, &ans, NULL);
  FREE_NOW(exp);
  if (STATUS_OK)
  {
    if (ans.ptr)
      *outnid = *(int *)ans.ptr;
    else
      status = TreeNNF;
  }
  if (ans.ptr)
    MdsIpFree(ans.ptr);
  return status;
}

struct _FindNodeStruct
{
  int *nids;
  int num;
  void *ptr;
};

int FindNodeEndRemote(PINO_DATABASE *dblist __attribute__((unused)),
                      void **ctx_inout)
{
  struct _FindNodeStruct *ctx = (struct _FindNodeStruct *)*ctx_inout;
  if (ctx)
  {
    MdsIpFree(ctx->ptr);
    free(ctx);
    *ctx_inout = 0;
  }
  return 1;
}

int FindNodeWildRemote(PINO_DATABASE *dblist, char const *patharg, int *nid_out,
                       void **ctx_inout, int usage_mask)
{
  int status = TreeSUCCESS;
  struct _FindNodeStruct *ctx = (struct _FindNodeStruct *)*ctx_inout;
  if (!ctx)
  {
    struct descrip ans = {0};
    struct descrip path = STR2DESCRIP(patharg);
    char exp[80];
    sprintf(exp, "TreeFindNodeWild($,%d)", usage_mask);
    status = MdsValue(dblist->tree_info->channel, exp, &path, &ans, NULL);
    if (STATUS_OK)
    {
      if (ans.ptr)
      {
        ctx = malloc(sizeof(struct _FindNodeStruct));
        ctx->nids = ctx->ptr = (int *)ans.ptr;
        ctx->num = ans.dims[0];
        *ctx_inout = (void *)ctx;
      }
      else
        status = TreeNNF;
    }
  }
  if (STATUS_OK)
  {
    if (ctx->num > 0)
    {
      *nid_out = *ctx->nids;
      ctx->num--;
      ctx->nids++;
    }
    else
    {
      FindNodeEndRemote(dblist, ctx_inout);
      status = TreeNMN;
    }
  }
  return status;
}

char *FindNodeTagsRemote(PINO_DATABASE *dblist, int nid_in,
                         void **ctx_ptr __attribute__((unused)))
{
  struct descrip ans = {0};
  char exp[80];
  char *tag = 0;
  sprintf(exp, "TreeFindNodeTags(%d)", nid_in);
  MdsValue(dblist->tree_info->channel, exp, &ans, NULL);
  if (ans.ptr)
  {
    if ((ans.dtype == DTYPE_BU) && (strlen(ans.ptr) > 0))
      tag = strdup(ans.ptr);
    MdsIpFree(ans.ptr);
  }
  return tag;
}

char *AbsPathRemote(PINO_DATABASE *dblist, char const *inpatharg)
{
  char *retans = 0;
  struct descrip ans = {0};
  struct descrip inpath = STR2DESCRIP(inpatharg);
  MdsValue(dblist->tree_info->channel, "TreeAbsPath($)", &inpath, &ans, NULL);
  if (ans.ptr)
  {
    if (ans.dtype == DTYPE_T && (strlen(ans.ptr) > 0))
      retans = strdup(ans.ptr);
    MdsIpFree(ans.ptr);
  }
  return retans;
}

int SetDefaultNidRemote(PINO_DATABASE *dblist, int nid)
{
  struct descrip ans = {0};
  char exp[80];
  int status;
  sprintf(exp, "TreeShr->TreeSetDefaultNid(val(%d))", nid);
  status = MdsValue(dblist->tree_info->channel, exp, &ans, NULL);
  if (ans.ptr)
  {
    status = (ans.dtype == DTYPE_L) ? *(int *)ans.ptr : 0;
    MdsIpFree(ans.ptr);
  }
  return status;
}

int GetDefaultNidRemote(PINO_DATABASE *dblist, int *nid)
{
  struct descrip ans = {0};
  int status = MdsValue(dblist->tree_info->channel,
                        "_=0;TreeShr->TreeGetDefaultNid(ref(_));_", &ans, NULL);
  if (ans.ptr)
  {
    if (ans.dtype == DTYPE_L)
      *nid = *(int *)ans.ptr;
    else if (STATUS_OK)
      status = 0;
    MdsIpFree(ans.ptr);
  }
  return status;
}

typedef struct tag_search
{
  int next_tag;
  TREE_INFO *this_tree_info;
  struct descriptor_d search_tag;
  struct descriptor_d search_tree;
  unsigned char top_match;
  unsigned char remote;
  char answer[1 + sizeof(TREE_NAME) + 2 + sizeof(TAG_NAME) + 1];
  int conid;
  int64_t ctx;
} TAG_SEARCH;

char *FindTagWildRemote(PINO_DATABASE *dblist, const char *wildarg, int *nidout,
                        void **ctx_inout)
{
  TAG_SEARCH **ctx = (TAG_SEARCH **)ctx_inout;
  char exp[256];
  if (!*ctx)
  {
    *ctx = malloc(sizeof(TAG_SEARCH));
    (*ctx)->remote = 1;
    (*ctx)->conid = dblist->tree_info->channel;
    (*ctx)->ctx = 0;
  }
  DESCRIPTOR_FROM_CSTRING(wild, wildarg);
  EMPTYXD(ans);
  sprintf(exp,
          "__a=-1;__b=0x%" PRIx64
          "QU;__c=*;___=TreeShr->TreeFindTagWildDsc(ref($),ref(__a),ref(__b),"
          "xd(__c));execute('deallocate(\"__*\");`list(*,___,__a,__b,__c)')",
          (*ctx)->ctx);
  int status = MdsValueDsc(dblist->tree_info->channel, exp, &wild, &ans, NULL);
  if (STATUS_OK)
  {
    struct descriptor **list = (struct descriptor **)ans.pointer->pointer;
    status = *(int *)list[0]->pointer;
    if (nidout)
      *nidout = *(int *)list[1]->pointer;
    (*ctx)->ctx = *(uint64_t *)list[2]->pointer;
    if ((*ctx)->ctx)
    {
      size_t len = list[3]->length < sizeof((*ctx)->answer)
                       ? list[3]->length
                       : sizeof((*ctx)->answer) - 1;
      memcpy((*ctx)->answer, list[3]->pointer, len);
      (*ctx)->answer[len] = '\0';
    }
    else
      status = TreeFAILURE;
  }
  MdsIpFreeDsc(&ans);
  return STATUS_OK ? (*ctx)->answer : NULL;
}

void FindTagEndRemote(void **ctx_inout)
{
  TAG_SEARCH **ctx = (TAG_SEARCH **)ctx_inout;
  if (*ctx)
  {
    if ((*ctx)->ctx)
    {
      char exp[128];
      sprintf(exp, "TreeShr->TreeFindTagEnd(val(0x%" PRIx64 "QU))",
              (*ctx)->ctx);
      struct descrip ans = {0};
      MdsValue((*ctx)->conid, exp, &ans, NULL);
      if (ans.ptr)
        MdsIpFree(ans.ptr);
    }
    free(*ctx);
    *ctx = NULL;
  }
}

int GetNciRemote(PINO_DATABASE *dblist, int nid_in, struct nci_itm *nci_itm)
{
  int status = TreeSUCCESS;
  NCI_ITM *itm;
  struct descrip ans;
  for (itm = nci_itm; itm->code != NciEND_OF_LIST && STATUS_OK; itm++)
  {
    char *getnci_str = NULL;
    switch (itm->code)
    {
    case NciDEPTH:
      getnci_str = "getnci(%d,'depth')";
      break;
    case NciGET_FLAGS:
      getnci_str = "getnci(%d,'get_flags')";
      break;
    case NciTIME_INSERTED:
      getnci_str = "getnci(%d,'time_inserted')";
      break;
    case NciOWNER_ID:
      getnci_str = "getnci(%d,'owner')";
      break;
    case NciCLASS:
      getnci_str = "getnci(%d,'class')";
      break;
    case NciDTYPE:
      getnci_str = "getnci(%d,'dtype')";
      break;
    case NciLENGTH:
      getnci_str = "getnci(%d,'length')";
      break;
    case NciRLENGTH:
      getnci_str = "getnci(%d,'rlength')";
      break;
    case NciSTATUS:
      getnci_str = "getnci(%d,'status')";
      break;
    case NciDATA_IN_NCI:
      getnci_str = "getnci(%d,'DATA_IN_NCI')";
      break;
    case NciERROR_ON_PUT:
      getnci_str = "getnci(%d,'error_on_put')";
      break;
    case NciIO_STATUS:
      getnci_str = "getnci(%d,'io_status')";
      break;
    case NciIO_STV:
      getnci_str = "getnci(%d,'io_stv')";
      break;
    case NciRFA:
      getnci_str = "getnci(%d,'rfa')";
      break;
    case NciCONGLOMERATE_ELT:
      getnci_str = "getnci(%d,'conglomerate_elt')";
      break;
    case NciPARENT:
      getnci_str = "getnci(getnci(%d,'parent'),'nid_number')";
      break;
    case NciBROTHER:
      getnci_str = "getnci(getnci(%d,'brother'),'nid_number')";
      break;
    case NciMEMBER:
      getnci_str = "getnci(getnci(%d,'member'),'nid_number')";
      break;
    case NciCHILD:
      getnci_str = "getnci(getnci(%d,'child'),'nid_number')";
      break;
    case NciPARENT_RELATIONSHIP:
      getnci_str = "getnci(%d,'parent_relationship')";
      break;
    case NciCONGLOMERATE_NIDS:
      getnci_str = "getnci(getnci(%d,'conglomerate_nids'),'nid_number')";
      break;
    case NciNUMBER_OF_CHILDREN:
      getnci_str = "getnci(%d,'number_of_children')";
      break;
    case NciNUMBER_OF_MEMBERS:
      getnci_str = "getnci(%d,'number_of_members')";
      break;
    case NciNUMBER_OF_ELTS:
      getnci_str = "getnci(%d,'number_of_elts')";
      break;
    case NciCHILDREN_NIDS:
      getnci_str = "getnci(getnci(%d,'children_nids'),'nid_number')";
      break;
    case NciMEMBER_NIDS:
      getnci_str = "getnci(getnci(%d,'member_nids'),'nid_number')";
      break;
    case NciUSAGE:
      getnci_str = "getnci(%d,'usage')";
      break;
    case NciUSAGE_STR:
      getnci_str = "getnci(%d,'usage_str')";
      break;
    case NciNODE_NAME:
      getnci_str = "getnci(%d,'NODE_NAME')";
      break;
    case NciPATH:
      getnci_str = "getnci(%d,'path')";
      break;
    case NciORIGINAL_PART_NAME:
      getnci_str = "getnci(%d,'original_part_name')";
      break;
    case NciFULLPATH:
      getnci_str = "getnci(%d,'fullpath')";
      break;
    case NciMINPATH:
      getnci_str = "getnci(%d,'minpath')";
      break;
    case NciPARENT_TREE:
      getnci_str = "getnci(%d,'parent_tree')";
      break;
    case NciVERSION:
      if (*(int *)itm->pointer == 0)
        continue;
      else
        status = 0;
      break;
    default:
      status = TreeILLEGAL_ITEM;
      break;
    }
    if (STATUS_OK)
    {
      char exp[1024];
      sprintf(exp, getnci_str, nid_in);
      status = MdsValue(dblist->tree_info->channel, exp, &ans, NULL);
      if (STATUS_OK)
      {
        if (ans.ptr && ans.length)
        {
          int length = ans.length * (ans.ndims ? ans.dims[0] : 1);
          if (itm->return_length_address)
            *itm->return_length_address = length;
          if ((ans.dtype == DTYPE_T) && (itm->pointer == 0))
          {
            itm->pointer = memcpy(malloc(length + 1), ans.ptr, length);
            ((char *)itm->pointer)[length] = '\0';
          }
          else
          {
            memcpy(itm->pointer, ans.ptr, min(itm->buffer_length, length));
            /*            if (itm->buffer_length < length) status =
                 * TreeBUFFEROVF; */
          }
          free(ans.ptr);
        }
        else
          status = 0;
      }
    }
  }
  return status;
}

int PutRecordRemote(PINO_DATABASE *dblist, int nid_in, struct descriptor *dsc,
                    int utility_update)
{
  int status;
  EMPTYXD(xd);
  char exp[80];
  if (dsc)
  {
    sprintf(exp, "TreeShr->TreePutRecord(val(%d),xd($),val(%d))", nid_in,
            utility_update);
    status = MdsValueDsc(dblist->tree_info->channel, exp, dsc, &xd, NULL);
  }
  else
  {
    sprintf(exp, "TreeShr->TreePutRecord(val(%d),val(0),val(%d))", nid_in,
            utility_update);
    status = MdsValueDsc(dblist->tree_info->channel, exp, &xd, NULL);
  }
  if (xd.pointer)
  {
    if (xd.pointer->dtype == DTYPE_L)
      status = *(int *)xd.pointer->pointer;
    else if (STATUS_OK)
      status = 0;
  }
  MdsIpFreeDsc(&xd);
  return status;
}

int SetNciItmRemote(PINO_DATABASE *dblist, int nid, int code, int value)
{
  struct descrip ans = {0};
  char exp[80];
  int status;
  sprintf(exp, "TreeShr->TreeSetNciItm(val(%d),val(%d),val(%d))", nid, code,
          value);
  status = MdsValue(dblist->tree_info->channel, exp, &ans, NULL);
  if (ans.ptr)
  {
    status = (ans.dtype == DTYPE_L) ? *(int *)ans.ptr : 0;
    MdsIpFree(ans.ptr);
  }
  return status;
}

int SetDbiItmRemote(PINO_DATABASE *dblist, int code, int value)
{
  struct descrip ans = {0};
  char exp[64];
  sprintf(exp, "TreeShr->TreeSetDbiItm(val(%d),val(%d))", code, value);
  int status = MdsValue(dblist->tree_info->channel, exp, &ans, NULL);
  if (ans.ptr)
  {
    status = (ans.dtype == DTYPE_L) ? *(int *)ans.ptr : 0;
    MdsIpFree(ans.ptr);
  }
  return status;
}

int SetNciRemote(PINO_DATABASE *dblist, int nid, NCI_ITM *nci_itm)
{
  int status = 1;
  NCI_ITM *itm_ptr;
  for (itm_ptr = nci_itm; itm_ptr->code != NciEND_OF_LIST && STATUS_OK;
       itm_ptr++)
  {
    switch (itm_ptr->code)
    {
    case NciSTATUS:
    case NciCLEAR_FLAGS:
    case NciSET_FLAGS:
      status = SetNciItmRemote(dblist, nid, (int)itm_ptr->code,
                               *(int *)itm_ptr->pointer);
      break;
    default:
      status = TreeILLEGAL_ITEM;
      break;
    }
  }
  return status;
}

int SetDbiRemote(PINO_DATABASE *dblist, DBI_ITM *dbi_itm)
{
  int status = 1;
  DBI_ITM *itm_ptr;
  for (itm_ptr = dbi_itm; itm_ptr->code != DbiEND_OF_LIST && STATUS_OK;
       itm_ptr++)
  {
    switch (itm_ptr->code)
    {
    case DbiVERSIONS_IN_MODEL:
    case DbiVERSIONS_IN_PULSE:
      status =
          SetDbiItmRemote(dblist, (int)itm_ptr->code, *(int *)itm_ptr->pointer);
      break;
    default:
      status = TreeILLEGAL_ITEM;
      break;
    }
  }
  return status;
}

int TreeFlushOffRemote(PINO_DATABASE *dblist, int nid)
{
  struct descrip ans = {0};
  char exp[64];
  sprintf(exp, "TreeShr->TreeFlushOff(val(%d))", nid);
  int status = MdsValue(dblist->tree_info->channel, exp, &ans, NULL);
  if (ans.ptr)
  {
    status = (ans.dtype == DTYPE_L) ? *(int *)ans.ptr : 0;
    MdsIpFree(ans.ptr);
  }
  return status;
}

int TreeFlushResetRemote(PINO_DATABASE *dblist, int nid)
{
  struct descrip ans = {0};
  char exp[64];
  int status;
  sprintf(exp, "TreeShr->TreeFlushReset(val(%d))", nid);
  status = MdsValue(dblist->tree_info->channel, exp, &ans, NULL);
  if (ans.ptr)
  {
    status = (ans.dtype == DTYPE_L) ? *(int *)ans.ptr : 0;
    MdsIpFree(ans.ptr);
  }
  return status;
}

int TreeTurnOnRemote(PINO_DATABASE *dblist, int nid)
{
  struct descrip ans = {0};
  char exp[64];
  int status;
  sprintf(exp, "TreeShr->TreeTurnOn(val(%d))", nid);
  status = MdsValue(dblist->tree_info->channel, exp, &ans, NULL);
  if (ans.ptr)
  {
    status = (ans.dtype == DTYPE_L) ? *(int *)ans.ptr : 0;
    MdsIpFree(ans.ptr);
  }
  return status;
}

int TreeTurnOffRemote(PINO_DATABASE *dblist, int nid)
{
  struct descrip ans = {0};
  char exp[64];
  int status;
  sprintf(exp, "TreeShr->TreeTurnOff(val(%d))", nid);
  status = MdsValue(dblist->tree_info->channel, exp, &ans, NULL);
  if (ans.ptr)
  {
    status = (ans.dtype == DTYPE_L) ? *(int *)ans.ptr : 0;
    MdsIpFree(ans.ptr);
  }
  return status;
}

int TreeGetCurrentShotIdRemote(const char *treearg, char *path, int *shot)
{
  int status = TreeFAILURE;
  int conid = remote_connect(path);
  if (conid >= 0)
  {
    struct descrip ans = {0};
    struct descrip tree = STR2DESCRIP(treearg);
    status = MdsValue(conid, "TreeShr->TreeGetCurrentShotId(ref($))", &tree,
                      &ans, NULL);
    remote_disconnect(conid, B_FALSE);
    if (ans.ptr)
    {
      if (ans.dtype == DTYPE_L)
        *shot = *(int *)ans.ptr;
      else
        status = STATUS_OK ? TreeFAILURE : status;
      MdsIpFree(ans.ptr);
    }
  }
  return status;
}

int TreeSetCurrentShotIdRemote(const char *treearg, char *path, int shot)
{
  int status = 0;
  int conid = remote_connect(path);
  if (conid > 0)
  {
    struct descrip ans = {0};
    struct descrip tree = STR2DESCRIP(treearg);
    char exp[64];
    sprintf(exp, "TreeShr->TreeSetCurrentShotId(ref($),val(%d))", shot);
    status = MdsValue(conid, exp, &tree, &ans, NULL);
    remote_disconnect(conid, B_FALSE);
    if (ans.ptr)
    {
      status = (ans.dtype == DTYPE_L) ? *(int *)ans.ptr : 0;
      MdsIpFree(ans.ptr);
    }
  }
  return status;
}

///////////////////////////////////////////////////////////////////
///////DISTRIBUTED AND NEW THICK CLIENT////////////////////////////
///////////////////////////////////////////////////////////////////

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include "../mdstcpip/mdsIo.h"
extern char *TreePath(char const *tree, char *tree_lower_out);
extern void TreePerfWrite(int);
extern void TreePerfRead(int);

typedef struct
{
  int conid;
  int fd;
  int enhanced;
} fdinfo_t;

static struct fd_info_struct
{
  int in_use;
  fdinfo_t i;
} *FDS = 0;
static int ALLOCATED_FDS = 0;

typedef struct iolock_s
{
  int (*io_lock)();
  fdinfo_t fd;
  off_t offset;
  size_t size;
  int *deleted;
} iolock_t;

static void mds_io_unlock(void *in)
{
  iolock_t *l = (iolock_t *)in;
  l->io_lock(l->fd, l->offset, l->size, MDS_IO_LOCK_NONE, l->deleted);
}

#define IO_RDLOCK_FILE(io_lock, fd, offset, size, deleted) \
  io_lock(fd, offset, size, MDS_IO_LOCK_RD, deleted);      \
  iolock_t iolock = {io_lock, fd, offset, size, deleted};  \
  pthread_cleanup_push(mds_io_unlock, &iolock);
#define IO_UNLOCK_FILE() pthread_cleanup_pop(1);

char *ParseFile(char *filename, char **hostpart, char **filepart)
{
  char *tmp = strcpy((char *)malloc(strlen(filename) + 1), filename);
  char *ptr = strstr(tmp, "::");
  if (ptr)
  {
    *hostpart = tmp;
    *filepart = ptr + 2;
    *ptr = (char)0;
  }
  else
  {
    *hostpart = 0;
    *filepart = tmp;
  }
  return tmp;
}

static pthread_mutex_t fds_lock = PTHREAD_MUTEX_INITIALIZER;
#define FDS_LOCK MUTEX_LOCK_PUSH(&fds_lock)
#define FDS_UNLOCK MUTEX_LOCK_POP(&fds_lock)

int ADD_FD(int fd, int conid, int enhanced)
{
  int idx;
  FDS_LOCK;
  for (idx = 0; idx < ALLOCATED_FDS && FDS[idx].in_use; idx++)
    ;
  if (idx == ALLOCATED_FDS)
    FDS = realloc(FDS, sizeof(struct fd_info_struct) * (++ALLOCATED_FDS));
  FDS[idx].in_use = B_TRUE;
  FDS[idx].i.conid = conid;
  FDS[idx].i.fd = fd;
  FDS[idx].i.enhanced = enhanced;
  FDS_UNLOCK;
  return idx + 1;
}

inline static fdinfo_t RM_FD(int idx)
{
  fdinfo_t fdinfo;
  FDS_LOCK;
  if (idx > 0 && idx <= ALLOCATED_FDS && FDS[idx - 1].in_use)
  {
    fdinfo = FDS[idx - 1].i;
    FDS[idx - 1].in_use = B_FALSE;
  }
  else
    fdinfo = (fdinfo_t){-1, -1, -1};
  FDS_UNLOCK;
  return fdinfo;
}

inline static fdinfo_t GET_FD(int idx)
{
  fdinfo_t fdinfo;
  FDS_LOCK;
  if (idx > 0 && idx <= ALLOCATED_FDS && FDS[idx - 1].in_use)
    fdinfo = FDS[idx - 1].i;
  else
    fdinfo = (fdinfo_t){-1, -1, -1};
  FDS_UNLOCK;
  return fdinfo;
}

EXPORT int MDS_IO_ID(int idx)
{
  int id;
  FDS_LOCK;
  id = (idx > 0 && idx <= ALLOCATED_FDS && FDS[idx - 1].in_use)
           ? FDS[idx - 1].i.conid
           : -1;
  FDS_UNLOCK;
  return id;
}

EXPORT int MDS_IO_FD(int idx)
{
  int fd;
  FDS_LOCK;
  fd = (idx > 0 && idx <= ALLOCATED_FDS && FDS[idx - 1].in_use)
           ? FDS[idx - 1].i.fd
           : -1;
  FDS_UNLOCK;
  return fd;
}

static int (*SendArg)() = NULL;
static int (*GetAnswerInfoTS)() = NULL;
static inline int mds_io_request(int conid, mds_io_mode idx, size_t size,
                                 mdsio_t *mdsio, char *din, int *bytes,
                                 char **dout, void **m)
{
  int status;
  char nargs = size / sizeof(int);
  status = SendArg(conid, (int)idx, 0, 0, 0, nargs, mdsio->dims, din);
  if (STATUS_NOT_OK)
  {
    if (idx != MDS_IO_CLOSE_K)
      fprintf(stderr, "Error in SendArg: mode = %d, status = %d\n", idx,
              status);
    remote_disconnect(conid, 1);
  }
  else
  {
    int d[MAX_DIMS];
    status = GetAnswerInfoTS(conid, (char *)d, (short *)d, (char *)d, d, bytes,
                             (void **)dout, m);
    if (STATUS_NOT_OK)
    {
      if (idx != MDS_IO_CLOSE_K)
        fprintf(stderr, "Error in GetAnswerInfoTS: mode = %d, status = %d\n",
                idx, status);
      remote_disconnect(conid, B_FALSE);
    }
  }
  return status;
}

EXPORT int MdsIoRequest(int conid, mds_io_mode idx, size_t size, mdsio_t *mdsio,
                        char *din, int *bytes, char **dout, void **m)
{
  int status = LibFindImageSymbol_C("MdsIpShr", "SendArg", &SendArg);
  if (STATUS_NOT_OK)
    return status;
  status =
      LibFindImageSymbol_C("MdsIpShr", "GetAnswerInfoTS", &GetAnswerInfoTS);
  if (STATUS_NOT_OK)
    return status;
  return mds_io_request(conid, idx, size, mdsio, din, bytes, dout, m);
}

inline static int io_open_request(int conid, int *enhanced, size_t size,
                                  mdsio_t *mdsio, char *filename)
{
  int fd;
  INIT_AND_FREE_ON_EXIT(void *, msg);
  int len;
  char *dout;
  int status = MdsIoRequest(conid, MDS_IO_OPEN_K, size, mdsio, filename, &len,
                            &dout, &msg);
  if (STATUS_OK && sizeof(int) == len)
  {
    fd = *(int *)dout;
    *enhanced = status == 3;
  }
  else
    fd = -1;
  FREE_NOW(msg);
  return fd;
}

inline static int io_open_remote(char *host, char *filename, int options,
                                 mode_t mode, int *conid, int *enhanced)
{
  int fd;
  mdsio_t mdsio = {
      .open = {.length = strlen(filename) + 1, .options = 0, .mode = mode}};
  if (O_CREAT == 0x0200)
  { /* BSD */
    if (options & O_CREAT)
      options = (options & ~O_CREAT) | 0100;
    if (options & O_TRUNC)
      options = (options & ~O_TRUNC) | 01000;
    if (options & O_EXCL)
      options = (options & ~O_EXCL) | 0200;
  }
  if (options & O_CREAT)
    mdsio.open.options |= MDS_IO_O_CREAT;
  if (options & O_TRUNC)
    mdsio.open.options |= MDS_IO_O_TRUNC;
  if (options & O_EXCL)
    mdsio.open.options |= MDS_IO_O_EXCL;
  if (options & O_WRONLY)
    mdsio.open.options |= MDS_IO_O_WRONLY;
  if (options & O_RDONLY)
    mdsio.open.options |= MDS_IO_O_RDONLY;
  if (options & O_RDWR)
    mdsio.open.options |= MDS_IO_O_RDWR;
  if (*conid == -1)
    *conid = remote_connect(host);
  if (*conid != -1)
  {
    fd =
        io_open_request(*conid, enhanced, sizeof(mdsio.open), &mdsio, filename);
    if (fd < 0)
      remote_disconnect(*conid, B_FALSE);
  }
  else
  {
    fd = -1;
    fprintf(stderr, "Error connecting to host /%s/ in io_open_remote\n", host);
  }
  return fd;
}

#ifndef _WIN32
inline static void set_mdsplus_file_protection(const char *filename)
{
  INIT_AND_FREE_ON_EXIT(char *, cmd);
  struct descriptor cmd_d = {0, DTYPE_T, CLASS_S, 0};
  cmd = (char *)malloc(39 + strlen(filename));
  sprintf(cmd, "SetMdsplusFileProtection %s 2> /dev/null", filename);
  cmd_d.length = strlen(cmd);
  cmd_d.pointer = cmd;
  LibSpawn(&cmd_d, 1, 0);
  FREE_NOW(cmd);
}
#endif

EXPORT int MDS_IO_OPEN(char *filename_in, int options, mode_t mode)
{
  int idx;
  INIT_AND_FREE_ON_EXIT(char *, filename);
  INIT_AND_FREE_ON_EXIT(char *, tmp);
  int conid = -1, fd = -1, enhanced = 0;
  filename = replaceBackslashes(strdup(filename_in));
  char *hostpart, *filepart;
  tmp = ParseFile(filename, &hostpart, &filepart);
  if (hostpart)
    fd = io_open_remote(hostpart, filepart, options, mode, &conid, &enhanced);
  else
  {

    fd = open(filename, options | O_BINARY | O_RANDOM, mode);
    MDSDBG("fd=%d, filename='%s'", fd, filename);
#ifndef _WIN32
    if ((fd >= 0) && ((options & O_CREAT) != 0))
      set_mdsplus_file_protection(filename);
#endif
  }
  idx = fd < 0 ? fd : ADD_FD(fd, conid, enhanced);
  FREE_NOW(tmp);
  FREE_NOW(filename);
  return idx;
}

inline static int io_close_remote(int conid, int fd)
{
  int ret;
  INIT_AND_FREE_ON_EXIT(void *, msg);
  int len;
  char *dout;
  mdsio_t mdsio = {.close = {.fd = fd}};
  int status = MdsIoRequest(conid, MDS_IO_CLOSE_K, sizeof(mdsio.close), &mdsio,
                            NULL, &len, &dout, &msg);
  if (STATUS_OK)
    remote_disconnect(conid, B_FALSE);
  if (STATUS_OK && sizeof(int) == len)
  {
    ret = *(int *)dout;
  }
  else
    ret = -1;
  FREE_NOW(msg);
  return ret;
}

EXPORT int MDS_IO_CLOSE(int idx)
{
  fdinfo_t i = RM_FD(idx);
  if (i.fd < 0)
    return -1;
  if (i.conid >= 0)
    return io_close_remote(i.conid, i.fd);
  MDSDBG("I fd=%d", i.fd);
  return close(i.fd);
}

inline static off_t io_lseek_remote(int conid, int fd, off_t offset,
                                    int whence)
{
  off_t ret;
  INIT_AND_FREE_ON_EXIT(void *, msg);
  mdsio_t mdsio = {.lseek = {.fd = fd, .offset = offset, .whence = whence}};
  SWAP_INT_IF_BIGENDIAN(&mdsio.lseek.offset);
  int len;
  char *dout;
  int status = MdsIoRequest(conid, MDS_IO_LSEEK_K, sizeof(mdsio.lseek), &mdsio,
                            NULL, &len, &dout, &msg);
  if (STATUS_OK)
    if (len == sizeof(int32_t))
    {
      ret = (off_t) * (int32_t *)dout;
      fprintf(stderr, "Server return 4 byte offset. Please update MDSplus on "
                      "server if possible.");
    }
    else if (len == sizeof(int64_t))
      ret = (off_t) * (int64_t *)dout;
    else
      ret = -1;
  else
    ret = -1;
  FREE_NOW(msg);
  return ret;
}

EXPORT off_t MDS_IO_LSEEK(int idx, off_t offset, int whence)
{
  fdinfo_t i = GET_FD(idx);
  if (i.fd < 0)
    return -1;
  if (i.conid >= 0)
    return io_lseek_remote(i.conid, i.fd, offset, whence);
  return lseek(i.fd, offset, whence);
}

inline static ssize_t io_write_remote(int conid, int fd, void *buff,
                                      size_t count)
{
  ssize_t ret;
  INIT_AND_FREE_ON_EXIT(void *, msg);
  mdsio_t mdsio = {.write = {.fd = fd, .count = count}};
  int len;
  char *dout;
  int status = MdsIoRequest(conid, MDS_IO_WRITE_K, sizeof(mdsio.write), &mdsio,
                            buff, &len, &dout, &msg);
  if (STATUS_OK)
  {
    if (len == sizeof(int32_t))
      ret = (ssize_t) * (int32_t *)dout;
    else if (len == sizeof(int64_t))
      ret = (ssize_t) * (int64_t *)dout;
    else
      ret = 0;
  }
  else
    ret = 0;
  FREE_NOW(msg);
  return ret;
}

EXPORT ssize_t MDS_IO_WRITE(int idx, void *buff, size_t count)
{
  fdinfo_t i = GET_FD(idx);
  if (i.fd < 0)
    return -1;
  if (count == 0)
    return 0;
  if (i.conid >= 0)
    return io_write_remote(i.conid, i.fd, buff, count);
#ifdef USE_PERF
  TreePerfWrite(count);
#endif
  return write(i.fd, buff, (uint32_t)count);
}

inline static ssize_t io_read_remote(int conid, int fd, void *buff,
                                     size_t count)
{
  ssize_t ret;
  INIT_AND_FREE_ON_EXIT(void *, msg);
  mdsio_t mdsio = {.read = {.fd = fd, .count = count}};
  int len;
  char *dout;
  int status = MdsIoRequest(conid, MDS_IO_READ_K, sizeof(mdsio.read), &mdsio,
                            NULL, &len, &dout, &msg);
  if (STATUS_OK)
  {
    ret = (ssize_t)len;
    memcpy(buff, dout, ret);
  }
  else
    ret = 0;
  FREE_NOW(msg);
  return ret;
}

EXPORT ssize_t MDS_IO_READ(int idx, void *buff, size_t count)
{
  fdinfo_t i = GET_FD(idx);
  if (i.fd < 0)
    return -1;
  if (count == 0)
    return 0;
  if (i.conid >= 0)
    return io_read_remote(i.conid, i.fd, buff, count);
#ifdef USE_PERF
  TreePerfRead(count);
#endif
  return read(i.fd, buff, count);
}

inline static ssize_t io_read_x_remote(int conid, int fd, off_t offset,
                                       void *buff, size_t count, int *deleted)
{
  ssize_t ret;
  INIT_AND_FREE_ON_EXIT(void *, msg);
  mdsio_t mdsio = {.read_x = {.fd = fd, .offset = offset, .count = count}};
  SWAP_INT_IF_BIGENDIAN(&mdsio.read_x.offset);
  int len;
  char *dout;
  int status = MdsIoRequest(conid, MDS_IO_READ_X_K, sizeof(mdsio.read_x),
                            &mdsio, NULL, &len, &dout, &msg);
  if (STATUS_OK)
  {
    if (deleted)
      *deleted = status == 3;
    ret = (ssize_t)len;
    if (ret)
      memcpy(buff, dout, ret);
  }
  else
    ret = -1;
  FREE_NOW(msg);
  return ret;
}
static int io_lock_local(fdinfo_t fdinfo, off_t offset, size_t size,
                         int mode_in, int *deleted);
static int io_lock_remote(fdinfo_t fdinfo, off_t offset, size_t size,
                          int mode_in, int *deleted);
EXPORT ssize_t MDS_IO_READ_X(int idx, off_t offset, void *buff, size_t count,
                             int *deleted)
{
  fdinfo_t i = GET_FD(idx);
  if (deleted)
    *deleted = 0;
  if (i.fd < 0)
    return -1;
  if (count == 0)
    return 0;
  if (i.conid >= 0)
  {
    if (i.enhanced)
      return io_read_x_remote(i.conid, i.fd, offset, buff, count, deleted);
    ssize_t ans;
    IO_RDLOCK_FILE(io_lock_remote, i, offset, count, deleted);
    MDS_IO_LSEEK(i.fd, offset, SEEK_SET);
    ans = MDS_IO_READ(i.fd, buff, (unsigned int)count);
    IO_UNLOCK_FILE();
    return ans;
  }
  ssize_t ans;
  IO_RDLOCK_FILE(io_lock_local, i, offset, count, deleted);
  lseek(i.fd, offset, SEEK_SET);
#ifdef USE_PERF
  TreePerfRead(count);
#endif
  ans = read(i.fd, buff, (unsigned int)count);
  IO_UNLOCK_FILE();
  return ans;
}

inline static int io_lock_remote(fdinfo_t fdinfo, off_t offset, size_t size,
                                 int mode, int *deleted)
{
  int ret;
  INIT_AND_FREE_ON_EXIT(void *, msg);
  mdsio_t mdsio = {
      .lock = {.fd = fdinfo.fd, .offset = offset, .size = size, .mode = mode}};
  SWAP_INT_IF_BIGENDIAN(&mdsio.lock.offset);
  int len;
  char *dout;
  int status = MdsIoRequest(fdinfo.conid, MDS_IO_LOCK_K, sizeof(mdsio.lock),
                            &mdsio, NULL, &len, &dout, &msg);
  if (STATUS_OK && len == sizeof(ret))
  {
    if (deleted)
      *deleted = status == 3;
    ret = *(int *)dout;
  }
  else
    ret = 0;
  FREE_NOW(msg);
  return ret;
}

static int io_lock_local(fdinfo_t fdinfo, off_t offset, size_t size,
                         int mode_in, int *deleted)
{

  MDSDBG("I fd=%d, offset=%" PRIu64 ", size=%" PRIu64 ", mode=%d",
         fdinfo.fd, offset, size, mode_in);
  int fd = fdinfo.fd;
  int err;
  int mode = mode_in & MDS_IO_LOCK_MASK;
  int nowait = mode_in & MDS_IO_LOCK_NOWAIT;
#ifdef _WIN32
  OVERLAPPED overlapped;
  int flags;
  offset = ((offset >= 0) && (nowait == 0)) ? offset : (lseek(fd, 0, SEEK_END));
  overlapped.Offset = (int)(offset & 0xffffffff);
  overlapped.OffsetHigh = (int)(offset >> 32);
  overlapped.hEvent = 0;
  HANDLE h = (HANDLE)_get_osfhandle(fd);
  if (mode > 0)
  {
    flags = ((mode == MDS_IO_LOCK_RD) && (nowait == 0))
                ? 0
                : LOCKFILE_EXCLUSIVE_LOCK;
    if (nowait)
      flags |= LOCKFILE_FAIL_IMMEDIATELY;
    // UnlockFileEx(h, 0, (DWORD) size, 0, &overlapped);
    err = !LockFileEx(h, flags, 0, (DWORD)size, 0, &overlapped);
  }
  else
  {
    err = !UnlockFileEx(h, 0, (DWORD)size, 0, &overlapped);
  }
  if (err)
    MDSDBG("LOCK_ER %d mode=%d, errorcode=%d\n", fd, mode, (int)GetLastError());
  else
    MDSDBG("LOCK_OK %d mode=%d\n", fd, mode);
  if (deleted)
    *deleted = 0;
#else
  struct flock flock;
  struct stat stat;
  flock.l_type = (mode == 0) ? F_UNLCK : ((mode == 1) ? F_RDLCK : F_WRLCK);
  flock.l_whence =
      (mode == 0) ? SEEK_SET : ((offset >= 0) ? SEEK_SET : SEEK_END);
  flock.l_start = (mode == 0) ? 0 : ((offset >= 0) ? offset : 0);
  flock.l_len = (mode == 0) ? 0 : size;
  static int use_ofd_locks = 1; // atomic?
  if (use_ofd_locks == 1)
  {
    flock.l_pid = 0;
    err = fcntl(fd, nowait ? F_OFD_SETLK : F_OFD_SETLKW, &flock);
    if (err != 0 && errno == EINVAL)
    {
      flock.l_pid = getpid();
      err = fcntl(fd, nowait ? F_SETLK : F_SETLKW, &flock);
      if (err == 0)
      {
        MDSWRN("File system does not support OFD locks, file access is not threadsafe");
        use_ofd_locks = 0;
      }
    }
  }
  else
  {
    flock.l_pid = getpid();
    err = fcntl(fd, nowait ? F_SETLK : F_SETLKW, &flock);
  }
  fstat(fd, &stat);
  if (deleted)
    *deleted = stat.st_nlink <= 0;
#endif
  MDSDBG("O fd=%d, offset=%" PRIu64 ", size=%" PRIu64 ", mode=%d, err=%d",
         fdinfo.fd, (uint64_t)offset, (uint64_t)size, mode_in, err);
  return err ? TreeLOCK_FAILURE : TreeSUCCESS;
}

EXPORT int MDS_IO_LOCK(int idx, off_t offset, size_t size, int mode_in,
                       int *deleted)
{
  fdinfo_t fdinfo = GET_FD(idx);
  if (deleted)
    *deleted = 0;
  if (fdinfo.fd < 0)
    return TreeLOCK_FAILURE;
  if (fdinfo.conid >= 0)
    return io_lock_remote(fdinfo, offset, size, mode_in, deleted);
  return io_lock_local(fdinfo, offset, size, mode_in, deleted);
}

inline static int io_exists_remote(char *host, char *filename)
{
  int ret;
  INIT_AND_FREE_ON_EXIT(void *, msg);
  int conid = remote_connect(host);
  if (conid != -1)
  {
    mdsio_t mdsio = {.exists = {.length = strlen(filename) + 1}};
    int len;
    char *dout;
    int status = MdsIoRequest(conid, MDS_IO_EXISTS_K, sizeof(mdsio.exists),
                              &mdsio, filename, &len, &dout, &msg);
    remote_disconnect(conid, B_FALSE);
    if (STATUS_OK && len == sizeof(int))
      ret = *(int *)dout;
    else
      ret = 0;
  }
  else
    ret = 0;
  FREE_NOW(msg);
  return ret;
}

EXPORT int MDS_IO_EXISTS(char *filename_in)
{
  int status;
  INIT_AND_FREE_ON_EXIT(char *, filename);
  INIT_AND_FREE_ON_EXIT(char *, tmp);
  filename = replaceBackslashes(strdup(filename_in));
  struct stat statbuf;
  char *hostpart, *filepart;
  tmp = ParseFile(filename, &hostpart, &filepart);
  if (hostpart)
    status = io_exists_remote(hostpart, filepart);
  else
    status = (stat(filename, &statbuf) == 0);
  FREE_NOW(tmp);
  FREE_NOW(filename);
  return status;
}

inline static int io_remove_remote(char *host, char *filename)
{
  int ret;
  INIT_AND_FREE_ON_EXIT(void *, msg);
  int conid = remote_connect(host);
  if (conid != -1)
  {
    mdsio_t mdsio = {.remove = {.length = strlen(filename) + 1}};
    int len;
    char *dout;
    int status = MdsIoRequest(conid, MDS_IO_REMOVE_K, sizeof(mdsio.remove),
                              &mdsio, filename, &len, &dout, &msg);
    remote_disconnect(conid, B_FALSE);
    if (STATUS_OK && len == sizeof(int))
      ret = *(int *)dout;
    else
      ret = -1;
  }
  else
    ret = -1;
  FREE_NOW(msg);
  return ret;
}

EXPORT int MDS_IO_REMOVE(char *filename_in)
{
  int status;
  INIT_AND_FREE_ON_EXIT(char *, filename);
  INIT_AND_FREE_ON_EXIT(char *, tmp);
  filename = replaceBackslashes(strdup(filename_in));
  char *hostpart, *filepart;
  tmp = ParseFile(filename, &hostpart, &filepart);
  status = hostpart ? io_remove_remote(hostpart, filepart) : remove(filename);
  FREE_NOW(tmp);
  FREE_NOW(filename);
  return status;
}

inline static int io_rename_remote(char *host, char *filename_old,
                                   char *filename_new)
{
  int ret;
  int conid;
  conid = remote_connect(host);
  if (conid != -1)
  {
    INIT_AND_FREE_ON_EXIT(char *, names);
    INIT_AND_FREE_ON_EXIT(void *, msg);
    mdsio_t mdsio = {.rename = {.length = strlen(filename_old) + 1 +
                                          strlen(filename_new) + 1}};
    names = strcpy(malloc(mdsio.length), filename_old);
    strcpy(&names[strlen(filename_old) + 1], filename_new);
    int len;
    char *dout;
    int status = MdsIoRequest(conid, MDS_IO_RENAME_K, sizeof(mdsio.rename),
                              &mdsio, names, &len, &dout, &msg);
    remote_disconnect(conid, B_FALSE);
    if (STATUS_OK && len == sizeof(int))
      ret = *(int *)dout;
    else
      ret = -1;
    FREE_NOW(msg);
    FREE_NOW(names);
  }
  else
    ret = -1;
  return ret;
}

EXPORT int MDS_IO_RENAME(char *filename_old, char *filename_new)
{
  int status;
  INIT_AND_FREE_ON_EXIT(char *, tmp_new);
  INIT_AND_FREE_ON_EXIT(char *, tmp_old);
  char *hostpart_old, *filepart_old, *hostpart_new, *filepart_new;
  tmp_old = ParseFile(filename_old, &hostpart_old, &filepart_old);
  tmp_new = ParseFile(filename_new, &hostpart_new, &filepart_new);
  filename_old = replaceBackslashes(filename_old);
  filename_new = replaceBackslashes(filename_new);
  if (hostpart_old)
  {
    if (hostpart_new && (strcmp(hostpart_old, hostpart_new) == 0))
      status = io_rename_remote(hostpart_old, filepart_old, filepart_new);
    else
      status = -1;
  }
  else
    status = rename(filename_old, filename_new);
  FREE_NOW(tmp_old);
  FREE_NOW(tmp_new);
  return status;
}

///////////////////////////////////////////////////////////////////
/////////NEW OPEN ONE INTERFACE////////////////////////////////////
///////////////////////////////////////////////////////////////////

inline static char *generate_fullpath(char *filepath, char const *treename,
                                      int shot, tree_type_t type)
{
  const char treeext[] = TREE_TREEFILE_EXT;
  const char nciext[] = TREE_NCIFILE_EXT;
  const char dataext[] = TREE_DATAFILE_EXT;
  char *ext;
  switch (type)
  {
  case TREE_TREEFILE_TYPE:
    ext = (char *)treeext;
    break;
  case TREE_NCIFILE_TYPE:
    ext = (char *)nciext;
    break;
  case TREE_DATAFILE_TYPE:
    ext = (char *)dataext;
    break;
  default:
    return strdup(filepath);
  }
  char name[40];
  if (shot > 999)
    sprintf(name, "%s_%d", treename, shot);
  else if (shot > 0)
    sprintf(name, "%s_%03d", treename, shot);
  else // if (shot == -1)
    sprintf(name, "%s_model", treename);
  char *resnam = strcpy(
      malloc(2 + strlen(filepath) + strlen(name) + strlen(ext)), filepath);
  int last = strlen(resnam) - 1;
  if (resnam[last] == '+')
    resnam[last] = '\0';
  else
  {
    if (strcmp(resnam + last, TREE_PATH_DELIM))
      strcat(resnam, TREE_PATH_DELIM);
    strcat(resnam, name);
  }
  strcat(resnam, ext);
  return resnam;
}

inline static int io_open_one_request(int conid, size_t size, mdsio_t *mdsio,
                                      char *data, char *host, int *enhanced,
                                      char **fullpath, int *fd_out)
{
  int status;
  INIT_AND_FREE_ON_EXIT(void *, msg);
  int len;
  int *dout;
  status = MdsIoRequest(conid, MDS_IO_OPEN_ONE_K, size, mdsio, data, &len,
                        (char **)&dout, &msg);
  if (STATUS_OK && len >= 8)
  {
    *enhanced = status == 3;
    status = *(dout++);
    *fd_out = *(dout++);
    if (len > 8)
    {
      *fullpath = malloc(len - 8 + strlen(host) + 3);
      sprintf(*fullpath, "%s::%s", host, (char *)dout);
    }
    else
      *fullpath = NULL;
  }
  else
  {
    *fd_out = -1;
  }
  FREE_NOW(msg);
  return status;
}

static void getOptionsMode(int new, int edit, int *options, int *mode)
{
  if (new)
  {
    *options = O_RDWR | O_CREAT | O_TRUNC;
    *mode = 0664;
  }
  else
  {
    *options = edit ? O_RDWR : O_RDONLY;
    *mode = 0;
  }
}

inline static int io_open_one_remote(char *host, char *filepath,
                                     char const *treename, int shot,
                                     tree_type_t type, int new, int edit,
                                     char **fullpath, int *conid, int *fd,
                                     int *enhanced)
{
  int status;
  static int (*MdsIpGetConnectionVersion)(int) = NULL;
  status = LibFindImageSymbol_C("MdsIpShr", "MdsIpGetConnectionVersion",
                                &MdsIpGetConnectionVersion);
  do
  {
    *conid = remote_connect(host);
    if (*conid != -1)
    {
      if (MdsIpGetConnectionVersion(*conid) < MDSIP_VERSION_OPEN_ONE)
      {
        if (*filepath && !strstr(filepath, "::"))
        {
          INIT_AS_AND_FREE_ON_EXIT(
              char *, tmp, generate_fullpath(filepath, treename, shot, type));
          int options, mode;
          getOptionsMode(new, edit, &options, &mode);
          *fd = io_open_remote(host, tmp, options, mode, conid, enhanced);
          status = *fd == -1 ? TreeFAILURE : TreeSUCCESS;
          if ((*fd >= 0) && edit && (type == TREE_TREEFILE_TYPE))
          {
            if (IS_NOT_OK(io_lock_remote((fdinfo_t){*conid, *fd, *enhanced}, 1, 1,
                                         MDS_IO_LOCK_RD | MDS_IO_LOCK_NOWAIT,
                                         0)))
            {
              status = TreeEDITING;
              *fd = -2;
            }
          }
          if (*fd >= 0)
          {
            *fullpath = malloc(strlen(host) + 3 + strlen(tmp));
            sprintf(*fullpath, "%s::%s", host, tmp);
          }
          FREE_NOW(tmp);
        }
        else
        {
          status = TreeUNSUPTHICKOP;
          remote_disconnect(*conid, B_FALSE);
        }
        break;
      }
      int len = strlen(treename);
      int totlen = strlen(filepath) + len + 2;
      INIT_AS_AND_FREE_ON_EXIT(char *, data, malloc(totlen));
      strcpy(data, treename);
      strcpy(data + len + 1, filepath);
      mdsio_t mdsio = {.open_one = {.length = totlen,
                                    .shot = shot,
                                    .type = (int)type,
                                    .new = new,
                                    .edit = edit}};
      status = io_open_one_request(*conid, sizeof(mdsio.open_one), &mdsio, data,
                                   host, enhanced, fullpath, fd);
      FREE_NOW(data);
      if (*fd < 0)
        remote_disconnect(*conid, B_FALSE);
    }
    else
    {
      fprintf(stderr, "Error connecting to host /%s/ in io_open_one_remote\n",
              host);
      *fd = -1;
    }
  } while (0);
  return status;
}

extern char *MaskReplace(char *, char *, int);
#include <ctype.h>
EXPORT int MDS_IO_OPEN_ONE(char *filepath_in, char const *treename_in, int shot,
                           tree_type_t type, int new, int edit, char **filespec,
                           int *speclen, int *idx)
{
  int status;
  INIT_AND_FREE_ON_EXIT(char *, fullpath);
  status = TreeSUCCESS;
  int enhanced = 0;
  int conid = -1;
  int fd = -1;
  char treename[13];
  char *hostpart, *filepart;
  size_t i;
  char *filepath = NULL;
  if (filepath_in && *filepath_in /*not empty*/)
  {
    for (i = 0; i < 12 && treename_in[i]; ++i)
      treename[i] = tolower(treename_in[i]);
    treename[i] = '\0';
    filepath = strdup(filepath_in);
  }
  else
  {
    char *tmp = TreePath(treename_in, treename);
    if (tmp)
    {
      replaceBackslashes(tmp);
      filepath = MaskReplace(tmp, treename, shot);
      free(tmp);
    }
  }
  if (filepath)
  {
    size_t pathlen = strlen(filepath);
    char *part = filepath;
    for (i = 0; i <= pathlen; i++)
    {
      if (filepath[i] != ';' && filepath[i] != '\0')
        continue;
      while (*part == ' ')
        part++;
      if (*part == '\0')
        break;
      filepath[i] = 0;
      char *tmp = ParseFile(part, &hostpart, &filepart);
      free(fullpath);
      if (hostpart)
      {
        fullpath = NULL;
        status =
            io_open_one_remote(hostpart, filepart, treename, shot, type, new,
                               edit, &fullpath, &conid, &fd, &enhanced);
        if (fd < 0)
        {
          if (status != TreeUNSUPTHICKOP)
            status = TreeSUCCESS;
          conid = -1;
          enhanced = 0;
        }
      }
      else
      {
        fullpath = generate_fullpath(filepart, treename, shot, type);
        int options, mode;
        getOptionsMode(new, edit, &options, &mode);
        fd = open(fullpath, options | O_BINARY | O_RANDOM, mode);
        if (type == TREE_DIRECTORY)
        {
          if (fd != -1)
          {
            close(fd);
            fd = -3;
          }
        }
        else
        {
#ifndef _WIN32
          if ((fd != -1) && new)
            set_mdsplus_file_protection(fullpath);
#endif
          if ((fd != -1) && edit && (type == TREE_TREEFILE_TYPE))
          {
            if (IS_NOT_OK(io_lock_local((fdinfo_t){conid, fd, enhanced}, 1, 1,
                                        MDS_IO_LOCK_RD | MDS_IO_LOCK_NOWAIT, 0)))
            {
              status = TreeEDITING;
              fd = -2;
            }
          }
        }
      }
      free(tmp);
      if (fd != -1)
      {
        if (speclen)
          *speclen = strlen(part);
        if (filespec && fullpath)
        {
          *filespec = fullpath;
          fullpath = NULL;
        }
        break;
      }
      part = &filepath[i + 1];
    }
    free(filepath);
  }
  *idx = fd < 0 ? fd : ADD_FD(fd, conid, enhanced);
  FREE_NOW(fullpath);
  return status;
}
