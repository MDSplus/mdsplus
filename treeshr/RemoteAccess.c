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
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wspiapi.h>
#endif
#include "treeshrp.h"
#include <mdsplus/mdsconfig.h>
#include <mdstypes.h>
#include <pthread.h>
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#endif
#include <STATICdef.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <ncidef.h>
#include <dbidef.h>
#include <libroutines.h>
#ifndef O_BINARY
#define O_BINARY 0x0
#endif

#ifndef O_RANDOM
#define O_RANDOM 0x0
#endif

#ifdef SRB
#define SRB_SOCKET 12345	/* this is the socket value used to indicate that
				   this file is an an SRB file.  Positive so that
				   other MDSPlus checks pass, but a value that
				   otherwise will not occur */
#include "srbUio.h"
#endif

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define MAX_DIMS 7
struct descrip {
  char dtype;
  char ndims;
  int dims[MAX_DIMS];
  int length;
  void *ptr;
};

STATIC_CONSTANT struct descrip empty_ans;


#if !defined(HAVE_PTHREAD_H)
#define pthread_mutex_t int
static void LockMdsShrMutex()
{
}

static void UnlockMdsShrMutex()
{
}
#else
extern void LockMdsShrMutex(pthread_mutex_t *, int *);
extern void UnlockMdsShrMutex(pthread_mutex_t *);
#endif

extern void TreePerfWrite(int);
extern void TreePerfRead(int);

STATIC_ROUTINE int FindImageSymbol(char *name, void **sym)
{
  STATIC_CONSTANT DESCRIPTOR(image, "MdsIpShr");
  struct descriptor symname = { 0, DTYPE_T, CLASS_S, 0 };
  STATIC_CONSTANT int mdslib_library_linked = 0;

/*
Manage name clash for MdsValue (defined both in mdsipshr and mdslib) by forcing library MdsLib to be loaded first.
This does not make harm to treeshr, but avoids to load a wrong symbol when MdsLib's MdsValue is then loaded.
*/
  if (!mdslib_library_linked) {
    STATIC_CONSTANT DESCRIPTOR(dummySymbol, "MdsOpen");
    STATIC_CONSTANT DESCRIPTOR(dummyImage, "MdsLib");
    void *dummySym;
    LibFindImageSymbol(&dummyImage, &dummySymbol, &dummySym);
    mdslib_library_linked = 1;
  }

  symname.length = (unsigned short)strlen(name);
  symname.pointer = name;
  return LibFindImageSymbol(&image, &symname, sym);
}

STATIC_THREADSAFE pthread_mutex_t HostListMutex;
STATIC_THREADSAFE int HostListMutex_initialized = 0;

STATIC_THREADSAFE pthread_mutex_t IOMutex;
STATIC_THREADSAFE int IOMutex_initialized = 0;
#if defined(HAVE_GETADDRINFO) && !defined(GLOBUS)
#ifndef _WIN32
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#endif

STATIC_THREADSAFE struct _host_list {
  void *dbid;
  char *host;
  int socket;
  struct sockaddr_in sockaddr;
  int connections;
  time_t time;
  struct _host_list *next;
} *host_list = 0;

STATIC_ROUTINE int GetAddr(char *host, struct sockaddr_in *sockaddr)
{
  int status;
  struct addrinfo *res;
  struct addrinfo hints;
  STATIC_THREADSAFE struct addr_list {
    char *host;
    struct sockaddr_in sockaddr;
    struct addr_list *next;
  } *AddrList = 0;
  char hostpart[256] = { 0 };
  char portpart[256] = { 0 };
  size_t i;
  struct addr_list *a;
  for (a = AddrList; a; a = a->next) {
    if (strcmp(host, a->host) == 0) {
      memcpy(sockaddr, &a->sockaddr, sizeof(*sockaddr));
      return 0;
    }
  }
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  sscanf(host, "%[^:]:%s", hostpart, portpart);
  if (strlen(portpart) == 0) {
    if (host[0] == '_')
      strcpy(portpart, "mdsips");
    else
      strcpy(portpart, "mdsip");
  }
  for (i = strlen(hostpart); (i > 0) && (hostpart[i - 1] == 32); hostpart[i - 1] = 0, i--) ;
  status = getaddrinfo(hostpart, portpart, &hints, &res);
  if (status == 0) {
    struct addr_list *p = malloc(sizeof(struct addr_list)), *next;
    p->host = strcpy(malloc(strlen(host) + 1), host);
    memset(((struct sockaddr_in *)res->ai_addr)->sin_zero, 0, sizeof(sockaddr->sin_zero));
    memcpy(&p->sockaddr, (struct sockaddr_in *)res->ai_addr, sizeof(struct sockaddr_in));
    memcpy(sockaddr, (struct sockaddr_in *)res->ai_addr, sizeof(struct sockaddr_in));
    p->next = 0;
    freeaddrinfo(res);
    if (AddrList) {
      for (next = AddrList; next->next; next = next->next) ;
      next->next = p;
    } else {
      AddrList = p;
    }
  }
  return status;
}
#else
STATIC_THREADSAFE struct _host_list {
  void *dbid;
  char *host;
  int socket;
  int connections;
  time_t time;
  struct _host_list *next;
} *host_list = 0;
#endif

STATIC_ROUTINE void MdsIpFree(void *ptr)
{
  STATIC_THREADSAFE void (*rtn) (void *) = 0;
  if (rtn == 0) {
    int status = FindImageSymbol("MdsIpFree", (void **)&rtn);
    if (!(status & 1))
      return;
  }
  (*rtn) (ptr);
}

STATIC_ROUTINE int RemoteAccessConnect(char *host, int inc_count, void *dbid)
{
  int host_in_directive;
  struct _host_list *hostchk;
  struct _host_list **nextone;
  STATIC_THREADSAFE int (*rtn) (char *) = 0;
  int socket = -1;
#if defined(HAVE_GETADDRINFO) && !defined(GLOBUS)
  struct sockaddr_in sockaddr;
  int getaddr_status;
#endif
  if (rtn == 0) {
    int status = FindImageSymbol("ConnectToMds", (void **)&rtn);
    if (!(status & 1))
      return -1;
  }
#if defined(HAVE_GETADDRINFO) && !defined(GLOBUS)
  getaddr_status = GetAddr(host, &sockaddr);
#endif
  LockMdsShrMutex(&HostListMutex, &HostListMutex_initialized);
  for (nextone = &host_list, hostchk = host_list; hostchk;
       nextone = &hostchk->next, hostchk = hostchk->next) {
    if (dbid && hostchk->dbid != dbid)
      continue;
#if defined(HAVE_GETADDRINFO) && !defined(GLOBUS)
    host_in_directive = (((getaddr_status == 0) ? memcmp(&sockaddr, &hostchk->sockaddr,
					sizeof(sockaddr)) : strcmp(hostchk->host, host)) == 0);
#else
    host_in_directive = (strcmp(hostchk->host, host) == 0);
#endif
    if (host_in_directive){
      hostchk->time = time(0);
      if (inc_count)
	hostchk->connections++;
      socket = hostchk->socket;
    }
  }
  if (socket == -1) {
    socket = (*rtn) (host);
    if (socket != -1) {
      *nextone = malloc(sizeof(struct _host_list));
      (*nextone)->dbid = dbid;
      (*nextone)->host = strcpy(malloc(strlen(host) + 1), host);
      (*nextone)->socket = socket;
      (*nextone)->connections = inc_count ? 1 : 0;
#if defined(HAVE_GETADDRINFO) && !defined(GLOBUS)
      memcpy(&(*nextone)->sockaddr, &sockaddr, sizeof(sockaddr));
#endif
      (*nextone)->time = time(0);
      (*nextone)->next = 0;
    }
  }
  UnlockMdsShrMutex(&HostListMutex);
  return socket;
}

STATIC_ROUTINE int RemoteAccessDisconnect(int socket, int force)
{
  int status = 1;
  struct _host_list *hostchk;
  struct _host_list *previous;
  STATIC_THREADSAFE int (*rtn) (int) = 0;
  if (rtn == 0) {
    int status = FindImageSymbol("DisconnectFromMds", (void **)&rtn);
    if (!(status & 1))
      return status;
  }
  LockMdsShrMutex(&HostListMutex, &HostListMutex_initialized);
  for (hostchk = host_list; hostchk && hostchk->socket != socket; hostchk = hostchk->next) ;
  if (hostchk) {
    hostchk->connections--;
  }
  previous = 0;
  hostchk = host_list;
  while (hostchk) {
    if (force || (hostchk->connections <= 0 && (hostchk->dbid || ((time(0) - hostchk->time) > 60)))) {
      struct _host_list *next = hostchk->next;
      status = (*rtn) (hostchk->socket);
      free(hostchk->host);
      free(hostchk);
      if (previous)
	previous->next = next;
      else
	host_list = next;
      hostchk = next;
    } else {
      previous = hostchk;
      hostchk = hostchk->next;
    }
  }
  UnlockMdsShrMutex(&HostListMutex);
  return status;
}

STATIC_THREADSAFE int (*MdsValue) () = 0;

STATIC_ROUTINE int MdsValue0(int socket, char *exp, struct descrip *ans)
{
  int status;
  if (MdsValue == 0) {
    status = FindImageSymbol("MdsValue", (void **)&MdsValue);
    if (!(status & 1))
      return status;
  }
  LockMdsShrMutex(&IOMutex, &IOMutex_initialized);
  status = (*MdsValue) (socket, exp, ans, NULL);
  UnlockMdsShrMutex(&IOMutex);
  return status;
}

STATIC_ROUTINE int MdsValue1(int socket, char *exp, struct descrip *arg1, struct descrip *ans)
{
  int status;
  if (MdsValue == 0) {
    status = FindImageSymbol("MdsValue", (void **)&MdsValue);
    if (!(status & 1))
      return status;
  }
  LockMdsShrMutex(&IOMutex, &IOMutex_initialized);
  status = (*MdsValue) (socket, exp, arg1, ans, NULL);
  UnlockMdsShrMutex(&IOMutex);
  return status;
}

int ConnectTreeRemote(PINO_DATABASE * dblist, char *tree, char *subtree_list, char *logname)
{
  int socket;
  logname[strlen(logname) - 2] = '\0';
  int status = TreeNORMAL;
  socket = RemoteAccessConnect(logname, 1, (void *)dblist);
  if (socket != -1) {
    struct descrip ans = empty_ans;
    char *exp = malloc(strlen(subtree_list ? subtree_list : tree) + 100);
    sprintf(exp, "TreeOpen('%s',%d)", subtree_list ? subtree_list : tree, dblist->shotid);
    status = MdsValue0(socket, exp, &ans);
    free(exp);
    status = (status & 1) ? (((ans.dtype == DTYPE_L) && ans.ptr) ? *(int *)ans.ptr : 0) : status;
    if (status & 1) {
      TREE_INFO *info;
      /***********************************************
       Get virtual memory for the tree
       information structure and zero the structure.
      ***********************************************/
      for (info = dblist->tree_info; info && strcmp(tree, info->treenam); info = info->next_info) ;
      if (!info) {
	info = malloc(sizeof(TREE_INFO) + sizeof(TREE_HEADER));
	if (info) {
	  memset(info, 0, sizeof(*info) + sizeof(TREE_HEADER));
	  info->blockid = TreeBLOCKID;
	  info->flush = (dblist->shotid == -1);
	  info->header = (TREE_HEADER *) & info[1];
	  info->treenam = strcpy(malloc(strlen(tree) + 1), tree);
	  TreeCallHook(OpenTree, info, 0);
	  info->channel = socket;
	  dblist->tree_info = info;
	  dblist->remote = 1;
	  status = TreeNORMAL;
	} else
	  status = TreeFILE_NOT_FOUND;
      }
    } else
      RemoteAccessDisconnect(socket, 0);
    if (ans.ptr)
      MdsIpFree(ans.ptr);
  } else
    status = TreeCONNECTFAIL;
  return status;
}

int SetStackSizeRemote(PINO_DATABASE *dbid __attribute__ ((unused)),
		       int stack_size __attribute__ ((unused)))
{
  return 1;
}

int CloseTreeRemote(PINO_DATABASE * dblist, int call_host __attribute__ ((unused)))
{
  struct descrip ans = empty_ans;
  int status;
  char exp[512];
  sprintf(exp, "TreeClose('%s',%d)", dblist->experiment, dblist->shotid);
  status = MdsValue0(dblist->tree_info->channel, exp, &ans);
  if (ans.ptr) {
    status = (ans.dtype == DTYPE_L) ? *(int *)ans.ptr : 0;
    MdsIpFree(ans.ptr);
  }
  RemoteAccessDisconnect(dblist->tree_info->channel, 0);
  if (dblist->tree_info && dblist->tree_info->treenam)
    free(dblist->tree_info->treenam);
  if (dblist->tree_info)
    free(dblist->tree_info);
  dblist->tree_info = 0;
  return status;
}

int CreatePulseFileRemote(PINO_DATABASE * dblist, int shot, int *nids, int num)
{
  struct descrip ans = empty_ans;
  int status;
  char exp[8192];
  int i;
  sprintf(exp, "TreeCreatePulseFile(%d,[", shot);
  for (i = 0; i < num; i++)
    sprintf(&exp[strlen(exp)], "%d,", nids[i]);
  sprintf(&exp[strlen(exp) - 1], "],%d)", num);
  status = MdsValue0(dblist->tree_info->channel, exp, &ans);
  if (ans.ptr) {
    status = (ans.dtype == DTYPE_L) ? *(int *)ans.ptr : 0;
    MdsIpFree(ans.ptr);
  }
  return status;
}

int GetRecordRemote(PINO_DATABASE * dblist, int nid_in, struct descriptor_xd *dsc)
{
  struct descrip ans = empty_ans;
  int status;
  char exp[512];
  sprintf(exp, "_thick_client_rec=getnci(%d,'RECORD'),execute('SerializeOut(`_thick_client_rec)')",
	  nid_in);
  status = MdsValue0(dblist->tree_info->channel, exp, &ans);
  if (status & 1) {
    if (ans.ptr)
      status = MdsSerializeDscIn(ans.ptr, dsc);
    else
      status = TreeNODATA;
  }
  if (ans.ptr)
    MdsIpFree(ans.ptr);
  return status;
}

STATIC_ROUTINE int LeadingBackslash(char const *path)
{
  size_t i;
  size_t len = strlen(path);
  for (i = 0; i < len; i++) {
    if ((path[i] != 32) && (path[i] != 9)) {
      if (path[i] == '\\')
	return 1;
      else
	return 0;
    }
  }
  return 0;
}

int FindNodeRemote(PINO_DATABASE * dblist, char const *path, int *outnid)
{
  struct descrip ans = empty_ans;
  int status;
  char *exp = malloc(strlen(path) + 32);
  sprintf(exp, "getnci(%s%s,'nid_number')", path[0] == '-' ? "." : "", path);
  status = MdsValue0(dblist->tree_info->channel, exp, &ans);

  free(exp);

  if (status & 1) {
    if (ans.ptr)
      *outnid = *(int *)ans.ptr;
    else
      status = TreeNNF;
  }
  if (ans.ptr)
    MdsIpFree(ans.ptr);
  return status;
}

struct _FindNodeStruct {
  int *nids;
  int num;
  int *ptr;
};

int FindNodeEndRemote(PINO_DATABASE * dblist __attribute__ ((unused)), void **ctx_inout)
{
  struct _FindNodeStruct *ctx = (struct _FindNodeStruct *)*ctx_inout;
  if (ctx) {
    MdsIpFree(ctx->ptr);
    free(ctx);
    *ctx_inout = 0;
  }
  return 1;
}

int FindNodeWildRemote(PINO_DATABASE * dblist, char const *path, int *nid_out, void **ctx_inout,
		       int usage_mask)
{
  int status = TreeNORMAL;
  struct _FindNodeStruct *ctx = (struct _FindNodeStruct *)*ctx_inout;
  if (!ctx) {
    struct descrip ans = empty_ans;
    char *exp = malloc(strlen(path) + 50);
    if (LeadingBackslash(path))
      sprintf(exp, "TreeFindNodeWild('\\%s',%d)", path, usage_mask);
    else
      sprintf(exp, "TreeFindNodeWild('%s',%d)", path, usage_mask);
    status = MdsValue0(dblist->tree_info->channel, exp, &ans);

    free(exp);

    if (status & 1) {
      if (ans.ptr) {
	ctx = malloc(sizeof(struct _FindNodeStruct));
	ctx->nids = ctx->ptr = (int *)ans.ptr;
	ctx->num = ans.dims[0];
	*ctx_inout = (void *)ctx;
      } else
	status = TreeNNF;
    }
  }
  if (status & 1) {
    if (ctx->num > 0) {
      *nid_out = *ctx->nids;
      ctx->num--;
      ctx->nids++;
    } else {
      FindNodeEndRemote(dblist, ctx_inout);
      status = TreeNMN;
    }
  }
  return status;
}

char *FindNodeTagsRemote(PINO_DATABASE * dblist, int nid_in, void **ctx_ptr __attribute ((unused)))
{
  struct descrip ans = empty_ans;
  char exp[64];
  char *tag = 0;
  sprintf(exp, "TreeFindNodeTags(%d)", nid_in);
  MdsValue0(dblist->tree_info->channel, exp, &ans);
  if (ans.ptr && (ans.dtype == DTYPE_T) && (strlen(ans.ptr) > 0)) {
    tag = strcpy(malloc(strlen(ans.ptr) + 1), ans.ptr);
    MdsIpFree(ans.ptr);
  }
  return tag;
}

char *AbsPathRemote(PINO_DATABASE * dblist, char const *inpath)
{
  struct descrip ans = empty_ans;
  char *exp = (char *)malloc(strlen(inpath) + 20);
  char *retans = 0;
  if (LeadingBackslash(inpath))
    sprintf(exp, "TreeAbsPath(\"\\%s\")", inpath);
  else
    sprintf(exp, "TreeAbsPath(\"%s\")", inpath);
  MdsValue0(dblist->tree_info->channel, exp, &ans);
  free(exp);
  if (ans.ptr) {
    if (ans.dtype == DTYPE_T && (strlen(ans.ptr) > 0)) {
      retans = strcpy(malloc(strlen(ans.ptr) + 1), ans.ptr);
    }
    MdsIpFree(ans.ptr);
  }
  return retans;
}

int SetDefaultNidRemote(PINO_DATABASE * dblist, int nid)
{
  struct descrip ans = empty_ans;
  char exp[512];
  int status;
  sprintf(exp, "SetDefaultNid(%d)", nid);
  status = MdsValue0(dblist->tree_info->channel, exp, &ans);
  if (ans.ptr) {
    status = (ans.dtype == DTYPE_L) ? *(int *)ans.ptr : 0;
    MdsIpFree(ans.ptr);
  }
  return status;
}

int GetDefaultNidRemote(PINO_DATABASE * dblist, int *nid)
{
  struct descrip ans = empty_ans;
  int status = MdsValue0(dblist->tree_info->channel, "GetDefaultNid()", &ans);
  if (ans.ptr) {
    if (ans.dtype == DTYPE_L)
      *nid = *(int *)ans.ptr;
    else if (status & 1)
      status = 0;
    MdsIpFree(ans.ptr);
  }
  return status;
}

typedef struct tag_search {
  int next_tag;
  TREE_INFO *this_tree_info;
  struct descriptor_d search_tag;
  struct descriptor_d search_tree;
  unsigned char top_match;
  unsigned char remote;
  char *remote_tag;
  int socket;
} TAG_SEARCH;

char *FindTagWildRemote(PINO_DATABASE * dblist, char *wild, int *nidout, void **ctx_inout)
{
  TAG_SEARCH **ctx = (TAG_SEARCH **) ctx_inout;
  struct descrip ans = empty_ans;
  struct descrip nid_ans = empty_ans;
  char exp[512];
  int status;
  int first_time = 0;
  if (*ctx == (TAG_SEARCH *) 0) {
    *ctx = (TAG_SEARCH *) malloc(sizeof(TAG_SEARCH));
    (*ctx)->remote = 1;
    (*ctx)->remote_tag = 0;
    (*ctx)->socket = dblist->tree_info->channel;
    first_time = 1;
  } else if ((*ctx)->remote_tag)
    free((*ctx)->remote_tag);
  sprintf(exp, "TreeFindTagWild(\"%s\",_nid,%s)//\"\\0\"", wild,
	  first_time ? "_remftwctx = 0" : "_remftwctx");
  status = MdsValue0(dblist->tree_info->channel, exp, &ans);
  if (status & 1 && ans.ptr && ans.dtype == DTYPE_T && strlen((char *)ans.ptr) > 0) {
    (*ctx)->remote_tag = strcpy(malloc(strlen(ans.ptr) + 1), ans.ptr);
  } else
    (*ctx)->remote_tag = 0;
  if (ans.ptr)
    MdsIpFree(ans.ptr);
  if (status & 1 && nidout) {
    status = MdsValue0(dblist->tree_info->channel, "_nid", &nid_ans);
    if ((status & 1) && (nid_ans.dtype == DTYPE_L) && (nid_ans.ptr != 0))
      *nidout = *(int *)nid_ans.ptr;
    else
      *nidout = 0;
    if (nid_ans.ptr)
      MdsIpFree(nid_ans.ptr);
  }
  return (*ctx)->remote_tag;
}

void FindTagEndRemote(void **ctx_inout)
{
  TAG_SEARCH **ctx = (TAG_SEARCH **) ctx_inout;
  struct descrip ans = empty_ans;
  if (*ctx != (TAG_SEARCH *) 0) {
    if ((*ctx)->remote_tag)
      MdsIpFree((*ctx)->remote_tag);
    MdsValue0((*ctx)->socket, "TreeFindTagEnd(_remftwctx)", &ans);
    if (ans.ptr)
      MdsIpFree(ans.ptr);
  }
  *ctx = (TAG_SEARCH *) 0;
}

int GetNciRemote(PINO_DATABASE * dblist, int nid_in, struct nci_itm *nci_itm)
{
  int status = TreeNORMAL;
  NCI_ITM *itm;
  struct descrip ans;
  for (itm = nci_itm; itm->code != NciEND_OF_LIST && status & 1; itm++) {
    char *getnci_str = NULL;
    switch (itm->code) {
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
    if (status & 1) {
      char exp[1024];
      sprintf(exp, getnci_str, nid_in);
      status = MdsValue0(dblist->tree_info->channel, exp, &ans);
      if (status & 1) {
	if (ans.ptr && ans.length) {
	  int length = ans.length * (ans.ndims ? ans.dims[0] : 1);
	  if (itm->return_length_address)
	    *itm->return_length_address = length;
	  if ((ans.dtype == DTYPE_T) && (itm->pointer == 0)) {
	    itm->pointer = memcpy(malloc(length + 1), ans.ptr, length);
	    ((char *)itm->pointer)[length] = '\0';
	  } else {
	    memcpy(itm->pointer, ans.ptr, min(itm->buffer_length, length));
/*            if (itm->buffer_length < length) status = TreeBUFFEROVF; */
	  }
	} else
	  status = 0;
      }
    }
  }
  return status;
}

int PutRecordRemote(PINO_DATABASE * dblist, int nid_in, struct descriptor *dsc, int utility_update)
{
  EMPTYXD(out);
  int status = MdsSerializeDscOut(dsc, &out);

  if (status & 1) {
    char exp[512];
    struct descrip ans = empty_ans;
    if (out.pointer) {
      struct descrip data = { DTYPE_B, 1, {0, 0, 0, 0, 0, 0, 0}, 1, 0 };
      sprintf(exp, "TreePutRecord(%d, SerializeIn($), %d)", nid_in, utility_update);
      data.dims[0] = ((struct descriptor_a *)out.pointer)->arsize;
      data.ptr = out.pointer->pointer;
      status = MdsValue1(dblist->tree_info->channel, exp, &data, &ans);
    } else {
      sprintf(exp, "TreePutRecord(%d, *, %d)", nid_in, utility_update);
      status = MdsValue0(dblist->tree_info->channel, exp, &ans);
    }
    if (ans.ptr) {
      if (ans.dtype == DTYPE_L)
	status = *(int *)ans.ptr;
      else if (status & 1)
	status = 0;
      MdsIpFree(ans.ptr);
    }
  }
  MdsFree1Dx(&out, NULL);
  return status;
}

STATIC_ROUTINE int SetNciItmRemote(PINO_DATABASE * dblist, int nid, int code, int value)
{
  struct descrip ans = empty_ans;
  char exp[512];
  int status;
  sprintf(exp, "TreeSetNciItm(%d,%d,%d)", nid, code, value);
  status = MdsValue0(dblist->tree_info->channel, exp, &ans);
  if (ans.ptr) {
    status = (ans.dtype == DTYPE_L) ? *(int *)ans.ptr : 0;
    MdsIpFree(ans.ptr);
  }
  return status;
}

STATIC_ROUTINE int SetDbiItmRemote(PINO_DATABASE * dblist, int code, int value)
{
  struct descrip ans = empty_ans;
  char exp[512];
  int status;
  sprintf(exp, "TreeSetDbiItm(%d,%d)", code, value);
  status = MdsValue0(dblist->tree_info->channel, exp, &ans);
  if (ans.ptr) {
    status = (ans.dtype == DTYPE_L) ? *(int *)ans.ptr : 0;
    MdsIpFree(ans.ptr);
  }
  return status;
}

int SetNciRemote(PINO_DATABASE * dblist, int nid, NCI_ITM * nci_itm)
{
  int status = 1;
  NCI_ITM *itm_ptr;
  for (itm_ptr = nci_itm; itm_ptr->code != NciEND_OF_LIST && status & 1; itm_ptr++) {
    switch (itm_ptr->code) {
    case NciSTATUS:
    case NciCLEAR_FLAGS:
    case NciSET_FLAGS:
      status = SetNciItmRemote(dblist, nid, (int)itm_ptr->code, *(int *)itm_ptr->pointer);
      break;
    default:
      status = TreeILLEGAL_ITEM;
      break;
    }
  }
  return status;
}

int SetDbiRemote(PINO_DATABASE * dblist, DBI_ITM * dbi_itm)
{
  int status = 1;
  DBI_ITM *itm_ptr;
  for (itm_ptr = dbi_itm; itm_ptr->code != DbiEND_OF_LIST && status & 1; itm_ptr++) {
    switch (itm_ptr->code) {
    case DbiVERSIONS_IN_MODEL:
    case DbiVERSIONS_IN_PULSE:
      status = SetDbiItmRemote(dblist, (int)itm_ptr->code, *(int *)itm_ptr->pointer);
      break;
    default:
      status = TreeILLEGAL_ITEM;
      break;
    }
  }
  return status;
}

int TreeFlushOffRemote(PINO_DATABASE * dblist, int nid)
{
  struct descrip ans = empty_ans;
  char exp[512];
  int status;
  sprintf(exp, "TreeFlushOff(%d)", nid);
  status = MdsValue0(dblist->tree_info->channel, exp, &ans);
  if (ans.ptr) {
    status = (ans.dtype == DTYPE_L) ? *(int *)ans.ptr : 0;
    MdsIpFree(ans.ptr);
  }
  return status;
}

int TreeFlushResetRemote(PINO_DATABASE * dblist, int nid)
{
  struct descrip ans = empty_ans;
  char exp[512];
  int status;
  sprintf(exp, "TreeFlushReset(%d)", nid);
  status = MdsValue0(dblist->tree_info->channel, exp, &ans);
  if (ans.ptr) {
    status = (ans.dtype == DTYPE_L) ? *(int *)ans.ptr : 0;
    MdsIpFree(ans.ptr);
  }
  return status;
}

int TreeTurnOnRemote(PINO_DATABASE * dblist, int nid)
{
  struct descrip ans = empty_ans;
  char exp[512];
  int status;
  sprintf(exp, "TreeTurnOn(%d)", nid);
  status = MdsValue0(dblist->tree_info->channel, exp, &ans);
  if (ans.ptr) {
    status = (ans.dtype == DTYPE_L) ? *(int *)ans.ptr : 0;
    MdsIpFree(ans.ptr);
  }
  return status;
}

int TreeTurnOffRemote(PINO_DATABASE * dblist, int nid)
{
  struct descrip ans = empty_ans;
  char exp[512];
  int status;
  sprintf(exp, "TreeTurnOff(%d)", nid);
  status = MdsValue0(dblist->tree_info->channel, exp, &ans);
  if (ans.ptr) {
    status = (ans.dtype == DTYPE_L) ? *(int *)ans.ptr : 0;
    MdsIpFree(ans.ptr);
  }
  return status;
}

int TreeGetCurrentShotIdRemote(char *tree, char *path, int *shot)
{
  int status = 0;
  int channel = RemoteAccessConnect(path, 0, 0);
  if (channel > 0) {
    struct descrip ans = empty_ans;
    char exp[512];
    sprintf(exp, "TreeGetCurrentShot(\"%s\")", tree);
    status = MdsValue0(channel, exp, &ans);
    if (ans.ptr) {
      if (ans.dtype == DTYPE_L)
	*shot = *(int *)ans.ptr;
      else
	status = status & 1 ? 0 : status;
      MdsIpFree(ans.ptr);
    }
  }
  return status;
}

int TreeSetCurrentShotIdRemote(char *tree, char *path, int shot)
{
  int status = 0;
  int channel = RemoteAccessConnect(path, 0, 0);
  if (channel > 0) {
    struct descrip ans = empty_ans;
    char exp[512];
    sprintf(exp, "TreeSetCurrentShot(\"%s\",%d)", tree, shot);
    status = MdsValue0(channel, exp, &ans);
    if (ans.ptr) {
      status = (ans.dtype == DTYPE_L) ? *(int *)ans.ptr : 0;
      MdsIpFree(ans.ptr);
    }
  }
  return status;
}

STATIC_THREADSAFE pthread_mutex_t FdsMutex;
STATIC_THREADSAFE int FdsMutex_initialized = 0;

STATIC_THREADSAFE struct fd_info_struct {
  int in_use;
  int socket;
  int fd;
  int enhanced;
} *FDS = 0;
STATIC_THREADSAFE int ALLOCATED_FDS = 0;

STATIC_ROUTINE char *ParseFile(char *filename, char **hostpart, char **filepart)
{
  char *tmp = strcpy((char *)malloc(strlen(filename) + 1), filename);
  char *ptr = strstr(tmp, "::");
  if (ptr) {
    *hostpart = tmp;
    *filepart = ptr + 2;
    *ptr = (char)0;
  } else {
    *hostpart = 0;
    *filepart = tmp;
  }
  return tmp;
}

#define LOCKFDS  LockMdsShrMutex(&FdsMutex,&FdsMutex_initialized);
#define UNLOCKFDS  UnlockMdsShrMutex(&FdsMutex);
STATIC_ROUTINE int NewFD(int fd, int socket, int enhanced)
{
  int idx;
  LOCKFDS for (idx = 0; idx < ALLOCATED_FDS && FDS[idx].in_use; idx++) ;
  if (idx == ALLOCATED_FDS)
    FDS = realloc(FDS, sizeof(struct fd_info_struct) * (++ALLOCATED_FDS));
  FDS[idx].in_use = 1;
  FDS[idx].socket = socket;
  FDS[idx].fd = fd;
  FDS[idx].enhanced = enhanced;
  UNLOCKFDS return idx + 1;
}

int MDS_IO_SOCKET(int fd)
{
  int ans;
  LOCKFDS ans = (fd > 0 && fd <= ALLOCATED_FDS && FDS[fd - 1].in_use) ? FDS[fd - 1].socket : -1;
  UNLOCKFDS return ans;
}

int MDS_IO_FD(int fd)
{
  int ans;
  LOCKFDS ans = (fd > 0 && fd <= ALLOCATED_FDS && FDS[fd - 1].in_use) ? FDS[fd - 1].fd : -1;
  UNLOCKFDS return ans;
}

STATIC_THREADSAFE int (*MDS_SEND_ARG) () = 0;
STATIC_ROUTINE int SendArg(int socket, unsigned char idx, char dtype, unsigned char nargs,
			   short length, char ndims, int *dims, char *bytes)
{
  if (MDS_SEND_ARG == 0) {
    int status = FindImageSymbol("SendArg", (void **)&MDS_SEND_ARG);
    if STATUS_NOT_OK  return status;
  }
  return (*MDS_SEND_ARG) (socket, idx, dtype, nargs, length, ndims, dims, bytes);
}

STATIC_THREADSAFE int (*MDS_GET_ANSWER_INFO_TS) () = 0;

STATIC_ROUTINE int GetAnswerInfoTS(int sock, char *dtype, short *length, char *ndims, int *dims,
				   int *numbytes, void **dptr, void **m)
{
  if (MDS_GET_ANSWER_INFO_TS == 0) {
    int status = FindImageSymbol("GetAnswerInfoTS", (void **)&MDS_GET_ANSWER_INFO_TS);
    if STATUS_NOT_OK  return status;
  }
  return (*MDS_GET_ANSWER_INFO_TS) (sock, dtype, length, ndims, dims, numbytes, dptr, m);
}

/*
STATIC_THREADSAFE int (*MDS_GET_ANSWER_INFO_TO) () = 0;

STATIC_ROUTINE int GetAnswerInfoTO(int sock, char *dtype, short *length, char *ndims, int *dims,
				   int *numbytes, void **dptr, void **m, int timeout)
{
  if (MDS_GET_ANSWER_INFO_TO == 0) {
    int status = FindImageSymbol("GetAnswerInfoTO", (void **)&MDS_GET_ANSWER_INFO_TO);
    if STATUS_NOT_OK  return status;
  }
  return (*MDS_GET_ANSWER_INFO_TO) (sock, dtype, length, ndims, dims, numbytes, dptr, m, timeout);
}
*/


#define MDS_IO_OPEN_K   1
#define MDS_IO_CLOSE_K  2
#define MDS_IO_LSEEK_K  3
#define MDS_IO_READ_K   4
#define MDS_IO_WRITE_K  5
#define MDS_IO_LOCK_K   6
#define MDS_IO_EXISTS_K 7
#define MDS_IO_REMOVE_K 8
#define MDS_IO_RENAME_K 9
#define MDS_IO_READ_X_K 10

#define MDS_IO_O_CREAT  0x00000040
#define MDS_IO_O_TRUNC  0x00000200
#define MDS_IO_O_EXCL   0x00000080
#define MDS_IO_O_WRONLY 0x00000001
#define MDS_IO_O_RDONLY 0x00004000
#define MDS_IO_O_RDWR   0x00000002

STATIC_ROUTINE int io_open_remote(char *host, char *filename, int options, mode_t mode, int *sock,
				  int *enhanced)
{
  int fd = -1;
  int try_again = 1;
  LockMdsShrMutex(&IOMutex, &IOMutex_initialized);
  while (try_again) {
    *sock = RemoteAccessConnect(host, 1, 0);
    if (*sock != -1) {
      int status;
      int info[3];
      info[0] = (int)strlen(filename) + 1;
      if (O_CREAT == 0x0200) {	/* BSD */
	if (options & O_CREAT)
	  options = (options & ~O_CREAT) | 0100;
	if (options & O_TRUNC)
	  options = (options & ~O_TRUNC) | 01000;
	if (options & O_EXCL)
	  options = (options & ~O_EXCL) | 0200;
      }
      info[1] = (options & O_CREAT ? MDS_IO_O_CREAT : 0) |
	  (options & O_TRUNC ? MDS_IO_O_TRUNC : 0) |
	  (options & O_EXCL ? MDS_IO_O_EXCL : 0) |
	  (options & O_WRONLY ? MDS_IO_O_WRONLY : 0) |
	  (options & O_RDONLY ? MDS_IO_O_RDONLY : 0) | (options & O_RDWR ? MDS_IO_O_RDWR : 0);
      info[2] = (int)mode;
      status = SendArg(*sock, MDS_IO_OPEN_K, 0, 0, 0, sizeof(info) / sizeof(int), info, filename);
      if (status & 1) {
	char dtype;
	short length;
	char ndims;
	int dims[7];
	int numbytes;
	void *dptr;
	void *msg = 0;
	int sts;
	if (((sts =
	      GetAnswerInfoTS(*sock, &dtype, &length, &ndims, dims, &numbytes, &dptr, &msg)) & 1)
	    && (length == sizeof(fd))) {
	  *enhanced = sts == 3;
	  memcpy(&fd, dptr, sizeof(fd));
	  try_again = 0;
	} else
	  fprintf(stderr, "Err in GetAnswerInfoTS in io_open_remote: status = %d, length = %d\n",
		  sts, length);
	if (msg)
	  free(msg);
	if (fd == -1)
	  RemoteAccessDisconnect(*sock, 0);
      } else {
	fprintf(stderr, "Err in SendArg in io_open_remote: status = %d\n", status);
	RemoteAccessDisconnect(*sock, 1);
      }
    } else {
      fprintf(stderr, "Error connecting to host /%s/ in io_open_remote\n", host);
      try_again = 0;
    }
  }
  UnlockMdsShrMutex(&IOMutex);
  return fd;
}

int MDS_IO_OPEN(char *filename, int options, mode_t mode)
{
  int socket = -1;
  char *hostpart, *filepart;
  char *tmp = ParseFile(filename, &hostpart, &filepart);
  int fd;
  int enhanced = 0;
  if (hostpart)
#ifdef SRB
    if (strcmp(hostpart, "SRB") == 0) {
      fd = srbUioOpen(filepart, options, mode);
      socket = SRB_SOCKET;	/* flag to indicate SRB file */
    } else
#endif
      fd = io_open_remote(hostpart, filepart, options, mode, &socket, &enhanced);
  else {
    fd = open(filename, options | O_BINARY | O_RANDOM, mode);
#ifndef _WIN32
    if ((fd != -1) && ((options & O_CREAT) != 0)) {
      struct descriptor cmd_d = { 0, DTYPE_T, CLASS_S, 0 };
      char *cmd = (char *)malloc(39 + strlen(filename));
      if (cmd) {
        sprintf(cmd, "SetMdsplusFileProtection %s 2> /dev/null", filename);
        cmd_d.length = strlen(cmd);
        cmd_d.pointer = cmd;
        LibSpawn(&cmd_d, 1, 0);
        free(cmd);
      }
    }
#endif
  }
  free(tmp);
  if (fd != -1)
    fd = NewFD(fd, socket, enhanced);
  return fd;
}

STATIC_ROUTINE int io_close_remote(int fd)
{
  int ret = -1;
  int info[] = { 0, 0 };
  int sock = FDS[fd - 1].socket;
  int status;
  LockMdsShrMutex(&IOMutex, &IOMutex_initialized);
  info[1] = FDS[fd - 1].fd;
  status = SendArg(sock, MDS_IO_CLOSE_K, 0, 0, 0, sizeof(info) / sizeof(int), info, 0);
  if (status & 1) {
    char dtype;
    short length;
    char ndims;
    int dims[7];
    int numbytes;
    void *dptr;
    void *msg = 0;
    if ((GetAnswerInfoTS(sock, &dtype, &length, &ndims, dims, &numbytes, &dptr, &msg) & 1)
	&& (length == sizeof(ret)))
      memcpy(&ret, dptr, sizeof(ret));
    if (msg)
      free(msg);
    RemoteAccessDisconnect(sock, 0);
  } else
    RemoteAccessDisconnect(sock, 1);
  UnlockMdsShrMutex(&IOMutex);
  return ret;
}

int MDS_IO_CLOSE(int fd)
{
  int status;
  LOCKFDS if (fd > 0 && fd <= ALLOCATED_FDS && FDS[fd - 1].in_use) {
#ifdef SRB
    if (FDS[fd - 1].socket == SRB_SOCKET) {
      status = srbUioClose(FDS[fd - 1].fd);
    } else
#else
    status = (FDS[fd - 1].socket == -1) ? close(FDS[fd - 1].fd) : io_close_remote(fd);
#endif
    FDS[fd - 1].in_use = 0;
    UNLOCKFDS return status;
  } else {
    UNLOCKFDS return -1;
  }
}

STATIC_ROUTINE off_t io_lseek_remote(int fd, off_t offset, int whence)
{
  off_t ret = -1;
  int info[] = { 0, 0, 0, 0, 0 };
  int sock = FDS[fd - 1].socket;
  int status;
  LockMdsShrMutex(&IOMutex, &IOMutex_initialized);
  info[1] = FDS[fd - 1].fd;
  info[4] = whence;
  *(off_t *) (&info[2]) = offset;
#ifdef WORDS_BIGENDIAN
  status = info[2];
  info[2] = info[3];
  info[3] = status;
#endif
  status = SendArg(sock, MDS_IO_LSEEK_K, 0, 0, 0, sizeof(info) / sizeof(int), info, 0);
  if (status & 1) {
    char dtype;
    short length;
    char ndims;
    int dims[7];
    int numbytes;
    void *dptr;
    void *msg = 0;
    if ((GetAnswerInfoTS(sock, &dtype, &length, &ndims, dims, &numbytes, &dptr, &msg) & 1)
	&& ((size_t)length >= sizeof(int))) {
      ret = 0;
      memcpy(&ret, dptr, ((size_t)length > sizeof(ret)) ? sizeof(ret) : (size_t)length);
    }
    if (msg)
      free(msg);
  } else
    RemoteAccessDisconnect(sock, 1);
  UnlockMdsShrMutex(&IOMutex);
  return ret;
}

off_t MDS_IO_LSEEK(int fd, off_t offset, int whence)
{
  off_t pos;
  LOCKFDS if (fd > 0 && fd <= ALLOCATED_FDS && FDS[fd - 1].in_use) {
#ifdef SRB
    if (FDS[fd - 1].socket == SRB_SOCKET) {
      pos = srbUioSeek(FDS[fd - 1].fd, offset, whence);
      UnlockMdsShrMutex(&IOMutex);
      UNLOCKFDS return pos;
    }
#endif
    pos =
	(FDS[fd - 1].socket == -1) ? lseek(FDS[fd - 1].fd, offset, whence) : io_lseek_remote(fd,
											     offset,
											     whence);
    UNLOCKFDS return pos;
  } else {
    UNLOCKFDS return -1;
  }
}

STATIC_ROUTINE ssize_t io_write_remote(int fd, void *buff, size_t count)
{
  ssize_t ret = 0;
  int info[] = { 0, 0 };
  int sock = FDS[fd - 1].socket;
  int status;
  LockMdsShrMutex(&IOMutex, &IOMutex_initialized);
  info[0] = (int)count;
  info[1] = FDS[fd - 1].fd;
  status = SendArg(sock, MDS_IO_WRITE_K, 0, 0, 0, sizeof(info) / sizeof(int), info, buff);
  if (status & 1) {
    char dtype;
    short length;
    char ndims;
    int dims[7];
    int nbytes;
    void *dptr;
    void *msg = 0;
    if ((GetAnswerInfoTS(sock, &dtype, &length, &ndims, dims, &nbytes, &dptr, &msg) & 1)
	&& (nbytes == sizeof(int)))
      ret = (ssize_t) * (int *)dptr;
    if (msg)
      free(msg);
  } else
    RemoteAccessDisconnect(sock, 1);
  UnlockMdsShrMutex(&IOMutex);
  return ret;
}

ssize_t MDS_IO_WRITE(int fd, void *buff, size_t count)
{
  ssize_t ans = -1;
  if (count == 0)
    return 0;
  LOCKFDS if (fd > 0 && fd <= ALLOCATED_FDS && FDS[fd - 1].in_use) {
#ifdef SRB
    if (FDS[fd - 1].socket == SRB_SOCKET) {
      ans = srbUioWrite(FDS[fd - 1].fd, buff, count);
    } else
#endif
    if (FDS[fd - 1].socket == -1) {
#ifdef USE_PERF
      TreePerfWrite(count);
#endif
      ans = (ssize_t) write(FDS[fd - 1].fd, buff, (unsigned int)count);
    } else
      ans = io_write_remote(fd, buff, count);
  }
  UNLOCKFDS return ans;
}

STATIC_ROUTINE ssize_t io_read_remote(int fd, void *buff, size_t count)
{
  ssize_t ret = 0;
  int ret_i;
  int info[] = { 0, 0, 0 };
  int sock = FDS[fd - 1].socket;
  int status;
  LockMdsShrMutex(&IOMutex, &IOMutex_initialized);
  info[1] = FDS[fd - 1].fd;
  info[2] = (int)count;
  status = SendArg(sock, MDS_IO_READ_K, 0, 0, 0, sizeof(info) / sizeof(int), info, 0);
  if (status & 1) {
    char dtype;
    short length;
    char ndims;
    int dims[7];
    void *dptr;
    void *msg = 0;
    if (GetAnswerInfoTS(sock, &dtype, &length, &ndims, dims, &ret_i, &dptr, &msg) & 1) {
      ret = (ssize_t) ret_i;
      if (ret)
	memcpy(buff, dptr, (size_t) ret);
    }
    if (msg)
      free(msg);
  } else
    RemoteAccessDisconnect(sock, 1);
  UnlockMdsShrMutex(&IOMutex);
  return ret;
}

STATIC_ROUTINE ssize_t io_read_x_remote(int fd, off_t offset, void *buff, size_t count,
					int *deleted)
{
  ssize_t ret = -1;
  int info[] = { 0, 0, 0, 0, 0 };
  int sock = FDS[fd - 1].socket;
  int status;
  LockMdsShrMutex(&IOMutex, &IOMutex_initialized);
  info[1] = FDS[fd - 1].fd;
  info[4] = (int)count;
  *(off_t *) (&info[2]) = offset;
#ifdef WORDS_BIGENDIAN
  status = info[2];
  info[2] = info[3];
  info[3] = status;
#endif
  status = SendArg(sock, MDS_IO_READ_X_K, 0, 0, 0, sizeof(info) / sizeof(int), info, 0);
  if (status & 1) {
    char dtype;
    short length;
    char ndims;
    int dims[7];
    void *dptr;
    int ret_i;
    void *msg = 0;
    int sts;
    if ((sts = GetAnswerInfoTS(sock, &dtype, &length, &ndims, dims, &ret_i, &dptr, &msg)) & 1) {
      if (deleted)
	*deleted = sts == 3;
      ret = (ssize_t) ret_i;
      if (ret)
	memcpy(buff, dptr, (size_t) ret);
    }
    if (msg)
      free(msg);
  } else
    RemoteAccessDisconnect(sock, 1);
  UnlockMdsShrMutex(&IOMutex);
  return ret;
}

ssize_t MDS_IO_READ(int fd, void *buff, size_t count)
{
  ssize_t ans = -1;
  if (count == 0)
    return 0;
  LOCKFDS if (fd > 0 && fd <= ALLOCATED_FDS && FDS[fd - 1].in_use) {
#ifdef SRB
    if (FDS[fd - 1].socket == SRB_SOCKET) {
      ans = srbUioRead(FDS[fd - 1].fd, buff, count);
    } else
#endif
    if (FDS[fd - 1].socket == -1) {
#ifdef USE_PERF
      TreePerfRead(count);
#endif
      ans = (ssize_t) read(FDS[fd - 1].fd, buff, (unsigned int)count);
    } else
      ans = (ssize_t) io_read_remote(fd, buff, count);
  }
  UNLOCKFDS return ans;
}

#ifdef SRB
if (FDS[fd - 1].socket == SRB_SOCKET) {
  pos = srbUioSeek(FDS[fd - 1].fd, offset, whence);
  ans = srbUioRead(FDS[fd - 1].fd, buff, count);
} else
#endif

  ssize_t MDS_IO_READ_X(int fd, off_t offset, void *buff, size_t count, int *deleted) {
    ssize_t ans = -1;
    if (count == 0) {
      if (deleted)
	*deleted = 0;
      return 0;
    }
    LOCKFDS if (fd > 0 && fd <= ALLOCATED_FDS && FDS[fd - 1].in_use) {
#ifdef SRB
      if (FDS[fd - 1].socket == SRB_SOCKET) {
	pos = srbUioSeek(FDS[fd - 1].fd, offset, whence);
	ans = srbUioRead(FDS[fd - 1].fd, buff, count);
      } else
#endif
      if (FDS[fd - 1].socket == -1 || (!FDS[fd - 1].enhanced)) {
	LockMdsShrMutex(&IOMutex, &IOMutex_initialized);
	MDS_IO_LOCK(fd, offset, count, MDS_IO_LOCK_RD, deleted);
	MDS_IO_LSEEK(fd, offset, SEEK_SET);
	ans = MDS_IO_READ(fd, buff, count);
	MDS_IO_LOCK(fd, offset, count, MDS_IO_LOCK_NONE, deleted);
	UnlockMdsShrMutex(&IOMutex);
      } else {
	ans = io_read_x_remote(fd, offset, buff, count, deleted);
      }
    }
    UNLOCKFDS return ans;
  }

STATIC_ROUTINE int io_lock_remote(int fd, off_t offset, size_t size, int mode, int *deleted)
{
  int ret = 0;
  int info[] = { 0, 0, 0, 0, 0, 0 };
  int sock = FDS[fd - 1].socket;
  int status;
  LockMdsShrMutex(&IOMutex, &IOMutex_initialized);
  info[1] = FDS[fd - 1].fd;
  info[4] = (int)size;
  info[5] = mode;
  *(off_t *) (&info[2]) = offset;
#ifdef WORDS_BIGENDIAN
  status = info[2];
  info[2] = info[3];
  info[3] = status;
#endif
  status = SendArg(sock, MDS_IO_LOCK_K, 0, 0, 0, sizeof(info) / sizeof(int), info, 0);
  if (status & 1) {
    char dtype;
    short length;
    char ndims;
    int nbytes;
    int dims[7];
    void *dptr;
    void *msg = 0;
    int sts;
    if ((sts = GetAnswerInfoTS(sock, &dtype, &length, &ndims, dims, &nbytes, &dptr, &msg)) & 1) {
      if (deleted)
	*deleted = sts == 3;
      memcpy(&ret, dptr, sizeof(ret));
    }
    if (msg)
      free(msg);
  } else
    RemoteAccessDisconnect(sock, 1);
  UnlockMdsShrMutex(&IOMutex);
  return ret;
}

int MDS_IO_LOCK(int fd, off_t offset, size_t size, int mode_in, int *deleted)
{
  int status = TreeLOCK_FAILURE;
  LOCKFDS if (deleted)
    *deleted = 0;
  if (fd > 0 && fd <= ALLOCATED_FDS && FDS[fd - 1].in_use) {
#ifdef SRB
    if (FDS[fd - 1].socket == SRB_SOCKET) {
      status = srbUioLock(FDS[fd - 1].fd, offset, size, mode_in);
      UNLOCKFDS return (status == 0) ? TreeSUCCESS : TreeLOCK_FAILURE;
    }
#endif
    if (FDS[fd - 1].socket == -1) {
      int mode = mode_in & MDS_IO_LOCK_MASK;
      int nowait = mode_in & MDS_IO_LOCK_NOWAIT;
#ifdef _WIN32
      OVERLAPPED overlapped;
      int flags;
      offset = ((offset >= 0) && (nowait == 0)) ? offset : (lseek(FDS[fd - 1].fd, 0, SEEK_END));
      overlapped.Offset = (int)(offset & 0xffffffff);
      overlapped.OffsetHigh = (int)(offset >> 32);
      overlapped.hEvent = 0;
      if (mode > 0) {
	HANDLE h = (HANDLE) _get_osfhandle(FDS[fd - 1].fd);
	flags = ((mode == MDS_IO_LOCK_RD) && (nowait == 0)) ? 0 : LOCKFILE_EXCLUSIVE_LOCK;
	if (nowait)
	  flags |= LOCKFILE_FAIL_IMMEDIATELY;
	status = UnlockFileEx(h, 0, (DWORD) size, 0, &overlapped);
	status =
	    LockFileEx(h, flags, 0, (DWORD) size, 0,
		       &overlapped) == 0 ? TreeLOCK_FAILURE : TreeNORMAL;
      } else {
	HANDLE h = (HANDLE) _get_osfhandle(FDS[fd - 1].fd);
	status =
	    UnlockFileEx(h, 0, (DWORD) size, 0, &overlapped) == 0 ? TreeLOCK_FAILURE : TreeNORMAL;
      }
#else
      struct flock flock_info;
      struct stat stat;
      flock_info.l_type = (mode == 0) ? F_UNLCK : ((mode == 1) ? F_RDLCK : F_WRLCK);
      flock_info.l_whence = (mode == 0) ? SEEK_SET : ((offset >= 0) ? SEEK_SET : SEEK_END);
      flock_info.l_start = (mode == 0) ? 0 : ((offset >= 0) ? offset : 0);
      flock_info.l_len = (mode == 0) ? 0 : size;
      status =
	  (fcntl(FDS[fd - 1].fd, nowait ? F_SETLK : F_SETLKW, &flock_info) !=
	   -1) ? TreeSUCCESS : TreeLOCK_FAILURE;
      fstat(FDS[fd - 1].fd, &stat);
      if (deleted)
	*deleted = stat.st_nlink <= 0;
#endif
    } else
      status = io_lock_remote(fd, offset, size, mode_in, deleted);
  }
  UNLOCKFDS return status;
}

STATIC_ROUTINE int io_exists_remote(char *host, char *filename)
{
  int ans = 0;
  int sock;
  LockMdsShrMutex(&IOMutex, &IOMutex_initialized);
  sock = RemoteAccessConnect(host, 1, 0);
  if (sock != -1) {
    int info[] = { 0 };
    int status;
    info[0] = (int)strlen(filename) + 1;
    status = SendArg(sock, MDS_IO_EXISTS_K, 0, 0, 0, sizeof(info) / sizeof(int), info, filename);
    if (status & 1) {
      char dtype;
      short length;
      char ndims;
      int dims[7];
      int numbytes;
      void *dptr;
      void *msg = 0;
      if ((GetAnswerInfoTS(sock, &dtype, &length, &ndims, dims, &numbytes, &dptr, &msg) & 1)
	  && (length == sizeof(ans)))
	memcpy(&ans, dptr, sizeof(ans));
      if (msg)
	free(msg);
      RemoteAccessDisconnect(sock, 0);
    } else
      RemoteAccessDisconnect(sock, 1);
  }
  UnlockMdsShrMutex(&IOMutex);
  return ans;
}

int MDS_IO_EXISTS(char *filename)
{
  int status;
  struct stat statbuf;
  char *hostpart, *filepart;
  char *tmp = ParseFile(filename, &hostpart, &filepart);
#ifdef SRB
  if (strcmp(hostpart, "SRB") == 0) {
    status = srbUioStat(filepart, &statbuf);
  } else
#endif
    status = hostpart ? io_exists_remote(hostpart, filepart) : (stat(filename, &statbuf) == 0);
  free(tmp);
  return status;
}

STATIC_ROUTINE int io_remove_remote(char *host, char *filename)
{
  int ans = -1;
  int sock;
  LockMdsShrMutex(&IOMutex, &IOMutex_initialized);
  sock = RemoteAccessConnect(host, 1, 0);
  if (sock != -1) {
    int info[] = { 0 };
    int status;
    info[0] = (int)strlen(filename) + 1;
    status = SendArg(sock, MDS_IO_REMOVE_K, 0, 0, 0, sizeof(info) / sizeof(int), info, filename);
    if (status & 1) {
      char dtype;
      short length;
      char ndims;
      int dims[7];
      int numbytes;
      void *dptr;
      void *msg = 0;
      if ((GetAnswerInfoTS(sock, &dtype, &length, &ndims, dims, &numbytes, &dptr, &msg) & 1)
	  && (length == sizeof(ans)))
	memcpy(&ans, dptr, sizeof(ans));
      if (msg)
	free(msg);
      RemoteAccessDisconnect(sock, 0);
    } else
      RemoteAccessDisconnect(sock, 1);
  }
  UnlockMdsShrMutex(&IOMutex);
  return ans;
}

int MDS_IO_REMOVE(char *filename)
{
  int status;
  char *hostpart, *filepart;
  char *tmp = ParseFile(filename, &hostpart, &filepart);
#ifdef SRB
  if (strcmp(hostpart, "SRB") == 0) {
    status = srbUioUnlink(filepart);
  } else
#endif
    status = hostpart ? io_remove_remote(hostpart, filepart) : remove(filename);
  free(tmp);
  return status;
}

STATIC_ROUTINE int io_rename_remote(char *host, char *filename_old, char *filename_new)
{
  int ans = -1;
  int sock;
  LockMdsShrMutex(&IOMutex, &IOMutex_initialized);
  sock = RemoteAccessConnect(host, 1, 0);
  if (sock != -1) {
    int info[] = { 0 };
    char *names;
    int status;
    info[0] = (int)(strlen(filename_old) + 1 + strlen(filename_new) + 1);
    names = strcpy(malloc(info[0]), filename_old);
    strcpy(&names[strlen(filename_old) + 1], filename_new);
    status = SendArg(sock, MDS_IO_RENAME_K, 0, 0, 0, sizeof(info) / sizeof(int), info, names);
    if (status & 1) {
      char dtype;
      short length;
      char ndims;
      int dims[7];
      int numbytes;
      void *dptr;
      void *msg = 0;
      if ((GetAnswerInfoTS(sock, &dtype, &length, &ndims, dims, &numbytes, &dptr, &msg) & 1)
	  && (length == sizeof(ans)))
	memcpy(&ans, dptr, sizeof(ans));
      if (msg)
	free(msg);
      RemoteAccessDisconnect(sock, 0);
    } else
      RemoteAccessDisconnect(sock, 1);
    free(names);
  }
  UnlockMdsShrMutex(&IOMutex);
  return ans;
}

int MDS_IO_RENAME(char *filename_old, char *filename_new)
{
  int status;
  char *hostpart_old, *filepart_old, *hostpart_new, *filepart_new;
  char *tmp_old = ParseFile(filename_old, &hostpart_old, &filepart_old);
  char *tmp_new = ParseFile(filename_new, &hostpart_new, &filepart_new);
#ifdef SRB
  if (strcmp(hostpart_new, "SRB") == 0) {
    if (strcmp(hostpart_old, hostpart_new) == 0) {
      status = srbUioRename(filepart_old, filepart_new);
    } else {
      status = 1;
    }
    free(tmp_old);
    free(tmp_new);
    return status;
  }
#endif

  if (hostpart_old) {
    if (hostpart_new && (strcmp(hostpart_old, hostpart_new) == 0))
      status = io_rename_remote(hostpart_old, filepart_old, filepart_new);
    else
      status = -1;
  } else {
    status = rename(filename_old, filename_new);
  }
  free(tmp_old);
  free(tmp_new);
  return status;
}
