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
/*------------------------------------------------------------------------------

		Name: TreeSetNci

		Type:   C function

		Author:	Josh Stillerman
			MIT Plasma Fusion Center
		Date:   19-MAY-1988

		Purpose: Set the Characteristics of a Node.

------------------------------------------------------------------------------

	Call sequence:

  int TreeSetNci(int nid, NCI_ITM *nci_itm_ptr)
  int TreeFlushOff( int nid)
  int TreeFlushReset( int nid)

------------------------------------------------------------------------------
   Copyright (c) 1988
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

+-----------------------------------------------------------------------------*/
#include "treeshrp.h"		/* must be first or off_t wrong */
#include <STATICdef.h>
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#endif
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <treeshr.h>
#include <ncidef.h>

extern void **TreeCtx();

extern int SetNciRemote();
extern int TreeFlushOffRemote();
extern int TreeFlushResetRemote();
extern int TreeTurnOnRemote();
extern int TreeTurnOffRemote();

int TreeSetNci(int nid, NCI_ITM * nci_itm_ptr)
{
  return _TreeSetNci(*TreeCtx(), nid, nci_itm_ptr);
}

int TreeFlushOff(int nid)
{
  return _TreeFlushOff(*TreeCtx(), nid);
}

int TreeFlushReset(int nid)
{
  return _TreeFlushReset(*TreeCtx(), nid);
}

int TreeTurnOn(int nid)
{
  return _TreeTurnOn(*TreeCtx(), nid);
}

int TreeTurnOff(int nid)
{
  return _TreeTurnOff(*TreeCtx(), nid);
}

int TreeGetNciLw(TREE_INFO * info, int node_num, NCI * nci);

static int SetNodeParentState(PINO_DATABASE * db, NODE * node, NCI * nci, unsigned int state);

int _TreeSetNci(void *dbid, int nid_in, NCI_ITM * nci_itm_ptr)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid_ptr = (NID *) & nid_in;
  int status;
  int node_number;
  TREE_INFO *tree_info;
  NCI_ITM *itm_ptr;
  NCI nci;
  int putnci=0;
/*------------------------------------------------------------------------------

 Executable:
*/
  if (!(IS_OPEN(dblist)))
    return TreeNOTOPEN;
  if (dblist->open_readonly)
    return TreeREADONLY;
  if (dblist->remote)
    return SetNciRemote(dbid, nid_in, nci_itm_ptr);
  nid_to_tree_nidx(dblist, nid_ptr, tree_info, node_number);
  if (!tree_info)
    return TreeNNF;
  status = TreeCallHook(dblist, PutNci, nid_in);
  if (status && !(status & 1))
    return status;
  status = TreeGetNciLw(tree_info, node_number, &nci);
  if (status & 1) {
    for (itm_ptr = nci_itm_ptr; itm_ptr->code != NciEND_OF_LIST && status & 1; itm_ptr++) {
      switch (itm_ptr->code) {

      case NciSET_FLAGS:
	nci.flags |= *(unsigned int *)itm_ptr->pointer;
	putnci=1;
	break;
      case NciCLEAR_FLAGS:
	nci.flags &= ~(*(unsigned int *)itm_ptr->pointer);
	putnci=1;
	break;
      case NciSTATUS:
	putnci=1;
	nci.status = *(unsigned int *)itm_ptr->pointer;
	break;
      case NciUSAGE:
	{
	  NODE *node_ptr;

/**************************************************
 First we must check to make sure we are editting
 a valid tree.
**************************************************/

	  if (!(IS_OPEN_FOR_EDIT(dblist)))
	    status = TreeNOEDIT;

	  node_ptr = nid_to_node(dblist, nid_ptr);
	  if (node_ptr->usage != *(unsigned char *)itm_ptr->pointer) {
	    node_ptr->usage = *(unsigned char *)itm_ptr->pointer;
	    dblist->modified = 1;
	  }
	  status = TreeNORMAL;
          break;
	}
      default:
	status = TreeILLEGAL_ITEM;
	break;
      }
    }
    if ((status & 1) && putnci)
      status = TreePutNci(tree_info, node_number, &nci, 1);
    TreeUnLockNci(tree_info, 0, node_number);
  }
  return status;
}

EXPORT int TreeSetNciItm(int nid, int code, int value)
{
  NCI_ITM itm[] = { {0, 0, 0, 0}, {0, 0, 0, 0} };
  itm[0].buffer_length = (short)sizeof(int);
  itm[0].code = (short)code;
  itm[0].pointer = (void *)&value;
  return TreeSetNci(nid, itm);
}

int _TreeFlushOff(void *dbid, int nid)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid_ptr = (NID *) & nid;
  TREE_INFO *tree_info;
  if (!(IS_OPEN(dblist)))
    return TreeNOTOPEN;
  if (dblist->remote)
    return TreeFlushOffRemote(dbid, nid);
  nid_to_tree(dblist, nid_ptr, tree_info);
  if (!tree_info)
    return TreeNNF;
  tree_info->flush = 0;
  return TreeNORMAL;
}

int _TreeFlushReset(void *dbid, int nid)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid_ptr = (NID *) & nid;
  TREE_INFO *tree_info;
  if (!(IS_OPEN(dblist)))
    return TreeNOTOPEN;
  if (dblist->remote)
    return TreeFlushResetRemote(dbid, nid);
  nid_to_tree(dblist, nid_ptr, tree_info);
  if (!tree_info)
    return TreeNNF;
  if ((tree_info->flush == 0) && (dblist->shotid == -1)) {
    tree_info->flush = 1;
    TreeWait(tree_info);
  }
  return TreeNORMAL;
}

/*------------------------------------------------------------------------------

		Name: TreeGetNciLw

		Type:   C function

		Author:	Josh Stillerman
			MIT Plasma Fusion Center

		Date:    22-MAR-1988

		Purpose: Routine which returns an characteristics record for a node
			locking the record so that it can be updated.

------------------------------------------------------------------------------

	Call sequence:
	       STATUS = TreeGetNciLw(info, node_num, nci)

------------------------------------------------------------------------------
   Copyright (c) 1988
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

+-----------------------------------------------------------------------------*/

int TreeGetNciLw(TREE_INFO * info, int node_num, NCI * nci)
{
  int status;
  if (info->header->readonly)
    return TreeREADONLY_TREE;

  /* status = TreeLockNci(info,0,node_num);
     if (!(status & 1)) return status; */
  status = TreeNORMAL;

/******************************************
  If the tree is not open for edit then
  if the characteristic file is not open
  (or not open for write)
  open the characteristic file for writting
  if OK so far then
  fill in the rab and read the record
******************************************/
  if ((info->edit == 0) || (node_num < info->edit->first_in_mem)) {
    int deleted = 1;
    while (status & 1 && deleted) {
      if ((info->nci_file == 0) || (info->nci_file->put == 0))
	status = TreeOpenNciW(info, 0);
      if (status & 1) {
	char nci_bytes[42];
	status = TreeLockNci(info, 0, node_num, &deleted);
	if (status & 1 && deleted) {
	  status = TreeReopenNci(info);
	} else {
	  if (!(status & 1)) {
	    TreeUnLockNci(info, 0, node_num);
	    return status;
	  }
	  MDS_IO_LSEEK(info->nci_file->put, node_num * sizeof(nci_bytes), SEEK_SET);
	  status =
	      (MDS_IO_READ(info->nci_file->put, nci_bytes, sizeof(nci_bytes)) ==
	       sizeof(nci_bytes)) ? TreeNORMAL : TreeFAILURE;
	  if (status == TreeNORMAL)
	    TreeSerializeNciIn(nci_bytes, nci);
	  if (!(status & 1))
	    TreeUnLockNci(info, 0, node_num);
	}
      }
    }
  } else {
  /********************************************
   Otherwise the tree is open for edit so
   the characteristics are just a memory reference
   away.
  *********************************************/
    status = TreeLockNci(info, 0, node_num, 0);
    if (!(status & 1))
      return status;

    memcpy(nci, info->edit->nci + node_num - info->edit->first_in_mem, sizeof(struct nci));
  }

  return status;
}

/*------------------------------------------------------------------------------

		Name: TreeOpenNciW

		Type:   C function

		Author:	Josh Stillerman
			MIT Plasma Fusion Center

		Date:   22-MAR-1988

		Purpose: Open a tree characteristics file for write access.

------------------------------------------------------------------------------

	Call sequence:
		       status = TreeOpenNciW(info_ptr)

------------------------------------------------------------------------------
   Copyright (c) 1988
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

+-----------------------------------------------------------------------------*/
int TreeOpenNciW(TREE_INFO * info, int tmpfile){
  WRLOCKINFO(info);
  int status = _TreeOpenNciW(info,tmpfile);
  UNLOCKINFO(info);
  return status;
}
int _TreeOpenNciW(TREE_INFO * info, int tmpfile)
{
  int status;

/****************************************************
  If there is no characterisitics file block allocated then
    Allocate one
*****************************************************/

  if (!info->nci_file) {
    status =
	((info->nci_file =
	  (struct nci_file *)malloc(sizeof(NCI_FILE))) != NULL) ? TreeNORMAL : TreeFAILURE;
    if STATUS_OK {
      size_t len = strlen(info->filespec) - 4;
#pragma GCC diagnostic push
#if defined __GNUC__ && 800 <= __GNUC__ * 100 + __GNUC_MINOR__
    _Pragma ("GCC diagnostic ignored \"-Wstringop-overflow\"")
#endif
      char *filename = strncpy(malloc(len + 20), info->filespec, len);
#pragma GCC diagnostic pop
      filename[len] = '\0';
      strcat(filename, tmpfile ? "characteristics#" : "characteristics");
      memset(info->nci_file, 0, sizeof(NCI_FILE));
      info->nci_file->get =
	  MDS_IO_OPEN(filename, tmpfile ? O_RDWR | O_CREAT | O_TRUNC | O_EXCL : O_RDONLY, 0664);
      status = (info->nci_file->get == -1) ? TreeFAILURE : TreeNORMAL;
      if (info->nci_file->get == -1)
	info->nci_file->get = 0;
      if (status & 1) {
	info->nci_file->put = MDS_IO_OPEN(filename, O_RDWR, 0);
	status = (info->nci_file->put == -1) ? TreeFAILURE : TreeNORMAL;
	if (info->nci_file->put == -1)
	  info->nci_file->put = 0;
      }
      free(filename);
    }
  } else {
  /*******************************************
    Else the file was open for read access so
    open it for write access.
  *******************************************/
    size_t len = strlen(info->filespec) - 4;
#pragma GCC diagnostic push
#if defined __GNUC__ && 800 <= __GNUC__ * 100 + __GNUC_MINOR__
    _Pragma ("GCC diagnostic ignored \"-Wstringop-overflow\"")
#endif
    char *filename = strncpy(malloc(len + 20), info->filespec, len);
#pragma GCC diagnostic pop
    filename[len] = '\0';
    strcat(filename, tmpfile ? "characteristics#" : "characteristics");
    /********* this will never happen ***********/
    if (info->nci_file->put)
      MDS_IO_CLOSE(info->nci_file->put);
    info->nci_file->put =
	MDS_IO_OPEN(filename, (tmpfile ? O_RDWR | O_CREAT | O_TRUNC : O_RDWR), 0664);
    status = (info->nci_file->put == -1) ? TreeFAILURE : TreeNORMAL;
    if (info->nci_file->put == -1)
      info->nci_file->put = 0;
    free(filename);
  }
  if (status & 1) {
    if (info->edit) {
      info->edit->first_in_mem = (int)MDS_IO_LSEEK(info->nci_file->put, 0, SEEK_END) / 42;
    }
  }
  return status;
}

/*------------------------------------------------------------------------------

		Name: TreePutNci

		Type:   C function

		Author:	Josh Stillerman
			MIT Plasma Fusion Center

		Date:    22-MAR-1988

		Purpose: Routine which puts an attribute record into a
			tree.

------------------------------------------------------------------------------

	Call sequence:
	       STATUS = TreePutNci(info_ptr, node_num, nci_ptr, flush)

------------------------------------------------------------------------------
   Copyright (c) 1988
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

+-----------------------------------------------------------------------------*/

int TreePutNci(TREE_INFO * info, int node_num, NCI * nci, int flush __attribute__ ((unused)))
{
  int status;
  status = TreeNORMAL;
/***************************************
  If the tree is not open for edit
****************************************/

  if ((info->edit == 0) || (node_num < info->edit->first_in_mem)) {

  /**********************
   Update the NCI record
  ***********************/

    status = TreeLockNci(info, 0, node_num, 0);
    if (status & 1) {
      char nci_bytes[42];
      memset(nci_bytes, 0, sizeof(nci_bytes));
      TreeSerializeNciOut(nci, nci_bytes);
      MDS_IO_LSEEK(info->nci_file->put, sizeof(nci_bytes) * node_num, SEEK_SET);
      status =
	  (MDS_IO_WRITE(info->nci_file->put, nci_bytes, sizeof(nci_bytes)) ==
	   sizeof(nci_bytes)) ? TreeNORMAL : TreeFAILURE;
      TreeUnLockNci(info, 0, node_num);
    }
  }

/****************************
  Else it is open for edit so
  the characteristics are just a
  memory reference away.
*****************************/

  else
    memcpy(info->edit->nci + (node_num - info->edit->first_in_mem), nci, sizeof(*nci));
  return status;
}

/*------------------------------------------------------------------------------

		Name: TreeTurnOn

		Type:   C function

		Author:	Josh Stillerman
			MIT Plasma Fusion Center

		Date:   28-MAR-1988

		Purpose: Turn on a sub tree.

------------------------------------------------------------------------------

	Call sequence:
			int TreeTurnOn()
			status = TreeTurnOn(nid);

------------------------------------------------------------------------------
   Copyright (c) 1988
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

+-----------------------------------------------------------------------------*/

int _TreeTurnOn(void *dbid, int nid_in)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid = (NID *) & nid_in;
  int status;
  int node_num;
  TREE_INFO *info;
  NCI nci;
  NODE *node;
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  if (dblist->remote)
    return TreeTurnOnRemote(dbid, nid_in);
  nid_to_tree_nidx(dblist, nid, info, node_num);
  if (!info)
    return TreeNNF;
  status = TreeGetNciLw(info, node_num, &nci);
  if (~status & 1)
    return status;

  if (nci.flags & NciM_STATE) {
    bitassign(0, nci.flags, NciM_STATE);
    status = TreePutNci(info, node_num, &nci, 0);
    if (~status & 1)
      return status;
    if (!(nci.flags & NciM_PARENT_STATE)) {
      node = nid_to_node(dblist, nid);
      if (node->child)
	status = SetParentState(dblist, child_of(dblist, node), 0);
      if (node->member)
	status = SetParentState(dblist, member_of(node), 0);
    } else
      status = TreePARENT_OFF;
  } else {
    status = TreeALREADY_ON;
  }
  status = TreeUnLockNci(info, 0, node_num);
  return status;
}

/*------------------------------------------------------------------------------

		Name: TreeTurnOff

		Type:   C function

		Author:	Josh Stillerman
			MIT Plasma Fusion Center

		Date:   28-MAR-1988

		Purpose: Turn off a sub tree.

------------------------------------------------------------------------------

	Call sequence:
			int TreeTurnOff()
			status = TreeTurnOff(nid);

------------------------------------------------------------------------------
   Copyright (c) 1988
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

+-----------------------------------------------------------------------------*/

int _TreeTurnOff(void *dbid, int nid_in)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid = (NID *) & nid_in;
  int status;
  int node_num;
  TREE_INFO *info;
  NCI nci;
  NODE *node;
  if (!IS_OPEN(dblist))
    return TreeNOT_OPEN;
  if (dblist->remote)
    return TreeTurnOffRemote(dbid, nid_in);
  nid_to_tree_nidx(dblist, nid, info, node_num);
  if (!info)
    return TreeNNF;
  status = TreeGetNciLw(info, node_num, &nci);
  if (~status & 1)
    return status;
  if (!(nci.flags & NciM_STATE)) {
    bitassign(1, nci.flags, NciM_STATE);
    status = TreePutNci(info, node_num, &nci, 0);
    if (~status & 1)
      return status;
    if (!(nci.flags & NciM_PARENT_STATE)) {
      node = nid_to_node(dblist, nid);
      if (node->child)
	status = SetParentState(dblist, child_of(dblist, node), 1);
      if (node->member)
	status = SetParentState(dblist, member_of(node), 1);
    }
  } else {
    status = TreeALREADY_OFF;
  }
  status = TreeUnLockNci(info, 0, node_num);
  return status;
}

/*------------------------------------------------------------------------------

		Name: SetParentState

		Type:   C function

		Author:	Josh Stillerman
			MIT Plasma Fusion Center

		Date:   28-MAR-1988

		Purpose: propagate the parent_inactive status of
		       a node through its' children

------------------------------------------------------------------------------

	Call sequence:

------------------------------------------------------------------------------
   Copyright (c) 1988
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

+-----------------------------------------------------------------------------*/

int SetParentState(PINO_DATABASE * db, NODE * node, unsigned int state)
{
  int status;
  NCI nci;
  NODE *lnode;
  status = TreeNORMAL;
  for (lnode = node; lnode && (status & 1); lnode = brother_of(db, lnode)) {
    status = SetNodeParentState(db, lnode, &nci, state);
    if ((status & 1) && (!(nci.flags & NciM_STATE)) && (lnode->child))
      status = SetParentState(db, child_of(db, lnode), state);
    if ((status & 1) && (!(nci.flags & NciM_STATE)) && (lnode->member))
      status = SetParentState(db, member_of(lnode), state);
  }
  return status;
}

/*------------------------------------------------------------------------------

		Name: SetNodeParentState

		Type:   C function

		Author:	Josh Stillerman
			MIT Plasma Fusion Center

		Date:   28-MAR-1988

		Purpose: Set the state of the PARENT_INACTIVE field of
			a node's attributes.

------------------------------------------------------------------------------

	Call sequence:
			int TREE$SET_NODE_PARENT_STATE()
			status = TREE$SET_NODE_PARENT_STATE(db_ptr, node_ptr, nci_ptr, state)

------------------------------------------------------------------------------
   Copyright (c) 1988
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

+-----------------------------------------------------------------------------*/

static int SetNodeParentState(PINO_DATABASE * db, NODE * node, NCI * nci, unsigned int state)
{
  TREE_INFO *info;
  int node_num;
  int status;
  for (info = db->tree_info;
       info && ((node < info->node) || (node > (info->node + info->header->nodes)));
       info = info->next_info) ;
  if (!info)
    return TreeNNF;
  node_num = (int)(node - info->node);
  status = TreeGetNciLw(info, node_num, nci);
  if (status & 1) {
    bitassign(state, nci->flags, NciM_PARENT_STATE);
    status = TreePutNci(info, node_num, nci, 0);
    TreeUnLockNci(info, 0, node_num);
  }
  return status;
}

extern void LockMdsShrMutex(pthread_mutex_t *, int *);
extern void UnlockMdsShrMutex(pthread_mutex_t *);

STATIC_THREADSAFE pthread_mutex_t NCIMutex;
STATIC_THREADSAFE int NCIMutex_initialized;

int TreeLockNci(TREE_INFO * info, int readonly, int nodenum, int *deleted)
{
  int status=1;
  if (! info->header->readonly)
    status = MDS_IO_LOCK(readonly ? info->nci_file->get : info->nci_file->put,
			 nodenum * 42, 42, readonly ? MDS_IO_LOCK_RD : MDS_IO_LOCK_WRT, deleted);
  LockMdsShrMutex(&NCIMutex, &NCIMutex_initialized);
  return status;
}

int TreeUnLockNci(TREE_INFO * info, int readonly, int nodenum)
{
  int status=1;
  if (! info->header->readonly)
    status = MDS_IO_LOCK(readonly ? info->nci_file->get : info->nci_file->put, nodenum * 42, 42,
			 MDS_IO_LOCK_NONE, 0);
  UnlockMdsShrMutex(&NCIMutex);
  return status;
}

int _TreeSetUsage(void *dbid, int nid_in, unsigned char usage)
{
  NCI_ITM itm[] = { {0, 0, 0, 0}, {0, 0, 0, 0} };
  itm[0].buffer_length = (short)sizeof(usage);
  itm[0].code = (short)NciUSAGE;
  itm[0].pointer = (void *)&usage;
  return _TreeSetNci(dbid, nid_in, itm);
}

int TreeSetUsage(int nid_in, unsigned char usage)
{
  return _TreeSetUsage(*TreeCtx(), nid_in, usage);
}
