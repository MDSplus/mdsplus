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
#include <treeshr.h>
#include "treeshrp.h"
#include <ncidef.h>
#include <string.h>

#ifdef __VMS
#include <fab.h>
#include <rab.h>
#include <nam.h>
#include <xab.h>
#include <lib$routines.h>
#include <starlet.h>
#include <starlet.h>
#pragma extern_model save
#pragma extern_model globalvalue
extern RMS$_WER;
#pragma extern_model restore
#endif /* VMS */

extern void *DBID;

int TreeSetNci(int nid, NCI_ITM *nci_itm_ptr) { return _TreeSetNci(DBID, nid, nci_itm_ptr);}
int TreeFlushOff(int nid) { return _TreeFlushOff(DBID, nid);}
int TreeFlushReset(int nid) { return _TreeFlushReset(DBID, nid);}
int TreeTurnOn(int nid) { return _TreeTurnOn(DBID, nid);}
int TreeTurnOff(int nid) { return _TreeTurnOff(DBID, nid);}

int TreeGetNciLw(TREE_INFO *info, int node_num, NCI *nci);

static int OpenNciW(TREE_INFO *info);

int       _TreeSetNci(void *dbid, int nid_in, NCI_ITM *nci_itm_ptr)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID *nid_ptr = (NID *)&nid_in;
  int       status;
  int       node_number;
  TREE_INFO *tree_info;
  NCI_ITM  *itm_ptr;
  NCI       nci;
  NODE     *node_ptr;
/*------------------------------------------------------------------------------

 Executable:
*/
  if (!(IS_OPEN(dblist)))
    return TreeNOTOPEN;
  if (dblist->open_readonly)
      return TreeREADONLY;
  nid_to_tree_nidx(dblist, nid_ptr, tree_info, node_number);
  if (!tree_info)
    return TreeNNF;
  if (tree_info->reopen)
    TreeCloseFiles(tree_info);
  status = TreeGetNciLw(tree_info, node_number, &nci);
  if (status & 1)
  {
    for (itm_ptr = nci_itm_ptr; itm_ptr->code != NciEND_OF_LIST && status & 1; itm_ptr++)
    {
      switch (itm_ptr->code)
      {

       case NciSET_FLAGS:
	nci.NCI_FLAG_WORD.flags |= *(unsigned int *) itm_ptr->pointer;
	break;
       case NciCLEAR_FLAGS:
	nci.NCI_FLAG_WORD.flags &= ~(*(unsigned int *) itm_ptr->pointer);
	break;
       case NciSTATUS:
	nci.status = *(unsigned int *) itm_ptr->pointer;
	break;
       default:
	status = TreeILLEGAL_ITEM;
	break;
      }
    }
    if (status & 1)
      status = TreePutNci(tree_info, node_number, &nci, 1);
  }
  return status;
}

int _TreeFlushOff(void *dbid, int nid)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID *nid_ptr = (NID *)nid;
  int       node_number;
  TREE_INFO *tree_info;
  if (!(IS_OPEN(dblist)))
    return TreeNOTOPEN;
  nid_to_tree_nidx(dblist, nid_ptr, tree_info, node_number);
  if (!tree_info)
    return TreeNNF;
  tree_info->flush=0;
  return 1;
}

int _TreeFlushReset(void *dbid,  int nid)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID *nid_ptr = (NID *)nid;
  int       node_number;
  TREE_INFO *tree_info;
  if (!(IS_OPEN(dblist)))
    return TreeNOTOPEN;
  nid_to_tree_nidx(dblist, nid_ptr, tree_info, node_number);
  if (!tree_info)
    return TreeNNF;
  if ((tree_info->flush == 0) && (dblist->shotid == -1)) {
    tree_info->flush = 1;
    TreeWait(tree_info);
  }
  return 1;
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

int TreeGetNciLw(TREE_INFO *info, int node_num, NCI *nci)
{
  int       status;
  int       nci_num;
  status = 1;
  TreeWait(info);

/******************************************
  If the tree is not open for edit then
  if the characteristic file is not open
  (or not open for write)
  open the characteristic file for writting
  if OK so far then
  fill in the rab and read the record
******************************************/

  if ((info->edit == 0) || (node_num < info->edit->first_in_mem))
  {
#ifdef __VMS
    if ((info->nci_file == 0) ? 1 : ((info->nci_file->fab->fab$b_fac & FAB$M_UPD) == 0))
#else /* __VMS */
    if ((info->nci_file == 0) || (info->nci_file->put == 0))
#endif
      status = OpenNciW(info);
    if (status & 1)
    {
      nci_num = node_num + 1;
#ifdef __VMS
      info->nci_file->putrab->rab$l_kbf = (char *) &nci_num;
      info->nci_file->putrab->rab$l_rop &= !RAB$M_ASY;
      info->nci_file->putrab->rab$l_ubf = (char *) nci;
      status = sys$get(info->nci_file->putrab, 0, 0);
      if (status == RMS$_WER)
        status = info->nci_file->putrab->rab$l_stv;
#else /* __VMS */
      fseek(info->nci_file->put,node_num * sizeof(struct nci),SEEK_SET);
      status = fread(nci,sizeof(struct nci),1,info->nci_file->put) == 1;
#endif /* __VMS */
    }
  }
  else
  {
  /********************************************
   Otherwise the tree is open for edit so
   the characteristics are just a memory reference
   away.
  *********************************************/

    memcpy(nci, info->edit->nci +
	      node_num - info->edit->first_in_mem, sizeof(struct nci));
  }

  return status;
}

/*------------------------------------------------------------------------------

		Name: OpenNciW

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
static int OpenNciW(TREE_INFO *info)
{
  int       status;

#ifdef __VMS
  static char *characteristics_name = MtNnciFileType;
  static    $DESCRIPTOR(ncif_buffering, "NCIF");
  struct XABFHC xab;
#endif

/****************************************************
  If there is no characterisitics file block allocated then
    Allocate one
*****************************************************/

  if (info->nci_file == 0)
  {
    status = (info->nci_file = (struct nci_file *)malloc(NCI_FILE_VM_SIZE)) != NULL;
    if (status & 1)
    {
#ifdef __VMS
    /********************************************
      Open the file for PUT & UPDATE with
      a namblock.  If there is a problem free the
      memory and return.
    *********************************************/

      memset(info->nci_file,0, NCI_FILE_VM_SIZE);
      xab = cc$rms_xabfhc;
      *info->nci_file->fab = cc$rms_fab;
      info->nci_file->fab->fab$l_xab = (char *) &xab;
      info->nci_file->fab->fab$l_nam = info->nci_file->nam;
      info->nci_file->fab->fab$l_dna = info->filespec->dsc$a_pointer;
      info->nci_file->fab->fab$b_dns = info->filespec->dsc$w_length;
      info->nci_file->fab->fab$l_fna = characteristics_name;
      info->nci_file->fab->fab$b_fns = strlen(characteristics_name);
      info->nci_file->fab->fab$b_rfm = FAB$C_FIX;
      info->nci_file->fab->fab$b_shr = FAB$M_SHRGET | FAB$M_SHRPUT | FAB$M_MSE | FAB$M_SHRUPD;
      info->nci_file->fab->fab$b_fac = FAB$M_GET | FAB$M_UPD;
      info->nci_file->fab->fab$l_fop = FAB$M_DFW;
      *info->nci_file->nam = cc$rms_nam;
      status = sys$open(info->nci_file->fab, 0, 0);
#else /* __VMS */
      size_t len = strlen(info->filespec)-4;
      char *filename = strncpy(malloc(len+16),info->filespec,len);
      filename[len]='\0';
      strcat(filename,"characteristics");
      memset(info->nci_file,0, NCI_FILE_VM_SIZE);
      info->nci_file->get = fopen(filename,"rb");
      status = (info->nci_file->get == NULL) ? TreeFAILURE : TreeNORMAL;
      if (status & 1)
      {
	setbuf(info->nci_file->get,0);
        info->nci_file->put = fopen(filename,"r+b");
        status = (info->nci_file->put == NULL) ? TreeFAILURE : TreeNORMAL;
      }
#endif /* __VMS */
    }
  }
  else
  {
  /*******************************************
    Else the file was open for read access so
    close it and re-open it for write access.
  *******************************************/
#ifdef __VMS
    status = sys$close(info->nci_file->fab, 0, 0);
    if (status & 1)
    {
      info->nci_file->fab->fab$l_xab = (char *) &xab;
      info->nci_file->fab->fab$l_fop |= (FAB$M_DFW | FAB$M_NAM);
      info->nci_file->fab->fab$b_fac = FAB$M_GET | FAB$M_UPD;
      status = sys$open(info->nci_file->fab, 0, 0);
    }
#else /* __VMS */
    size_t len = strlen(info->filespec)-4;
    char *filename = strncpy(malloc(len+16),info->filespec,len);
    filename[len]='\0';
    strcat(filename,"characteristics");
    info->nci_file->put = fopen(filename,"r+b");
    status = (info->nci_file->put == NULL) ? TreeFAILURE : TreeNORMAL;
#endif /* __VMS */
  }
  if (status & 1)
  {
    if (info->edit)
    {
#ifdef __VMS
      info->edit->first_in_mem = ((xab.xab$l_ebk - 1) * 512 + xab.xab$w_ffb) / sizeof(NCI);
      if ((info->edit->first_in_mem * sizeof(NCI)) != ((xab.xab$l_ebk - 1) * 512 + xab.xab$w_ffb))
      {
	status = sys$close(info->nci_file->fab, 0, 0);
	if (status & 1)
	{
	  info->nci_file->fab->fab$l_fop |= (FAB$M_DFW | FAB$M_NAM);
	  info->nci_file->fab->fab$b_fac |= (FAB$M_TRN | FAB$M_PUT);
	  status = sys$open(info->nci_file->fab, 0, 0);
	  if (status & 1)
	  {
	    struct RAB rab;
	    NCI       nci;
	    int       last_record = info->header->tree_hdr$l_nodes - 1;
            rab = cc$rms_rab;
	    rab.rab$l_fab = info->nci_file->fab;
	    rab.rab$l_rop = RAB$M_TPT;
	    rab.rab$b_ksz = 4;
	    rab.rab$l_kbf = (char *) &last_record;
	    rab.rab$b_rac = RAB$C_KEY;
	    rab.rab$w_usz = sizeof(NCI);
	    rab.rab$w_rsz = sizeof(NCI);
	    rab.rab$l_ubf = (char *) &nci;
	    rab.rab$l_rbf = (char *) &nci;
	    status = sys$connect(&rab, 0, 0);
	    if (status & 1)
	    {
	      status = sys$get(&rab, 0, 0);
	      if (status & 1)
	      {
		rab.rab$b_rac = RAB$C_SEQ;
		status = sys$put(&rab, 0, 0);
		if (status & 1)
		  return OpenNciW(info);
	      }
	    }
	  }
	}
	if (!(status & 1))
	  return status;
      }
#endif /* __VMS */
    }
  /**********************************************
   Set up the RABs for buffered reads and writes
   and CONNECT it.
   If there is a problem then close it, free the
   memory and return.
  **********************************************/
#ifdef __VMS
    *info->nci_file->getrab = cc$rms_rab;
    info->nci_file->getrab->rab$l_fab = info->nci_file->fab;
    info->nci_file->getrab->rab$l_rop = RAB$M_WAT;
    info->nci_file->getrab->rab$b_ksz = 4;
    info->nci_file->getrab->rab$b_rac = RAB$C_KEY;
    info->nci_file->getrab->rab$w_usz = sizeof(NCI);
    GetBuffering((struct dsc$descriptor *)&ncif_buffering, info->nci_file->getrab);
    status = sys$connect(info->nci_file->getrab, 0, 0);
    if (status & 1)
    {
      *info->nci_file->putrab = *info->nci_file->getrab;
      info->nci_file->putrab->rab$l_rop = RAB$M_WBH | RAB$M_WAT;
      status = sys$connect(info->nci_file->putrab, 0, 0);
    }
    if (~status & 1)
    {
      sys$close(info->nci_file->fab, 0, 0);
      free(info->nci_file);
      info->nci_file = NULL;
    }
#endif /* __VMS */
  }
  else
    {
      free(info->nci_file);
      info->nci_file = NULL;
    }
  if (status & 1)
    TreeCallHook(OpenNCIFileWrite, info);
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

#ifdef __VMS
static void flush_it(struct RAB *rab)
{
  sys$flush(rab);
}
#endif /* __VMS */

int TreePutNci(TREE_INFO *info, int node_num, NCI *nci, int flush)
{
  int       status;
  int       nci_num;
  status = 1;
/***************************************
  If the tree is not open for edit
****************************************/

  if ((info->edit == 0) ||
      (node_num < info->edit->first_in_mem))
  {

  /**********************
   Update the NCI record
  ***********************/

#ifdef __VMS
    info->nci_file->nci = *nci;
    info->nci_file->putrab->rab$l_rop |= RAB$M_ASY;
    info->nci_file->putrab->rab$l_rbf = (char *)(&info->nci_file->nci);
    if (flush && info->flush)
      status = sys$update(info->nci_file->putrab, flush_it, flush_it);
    else
      status = sys$update(info->nci_file->putrab, 0, 0);
    if (status == RMS$_WER)
      status = info->nci_file->$a_putrab->rab$l_stv;
#else /* __VMS */
    fseek(info->nci_file->put,sizeof(struct nci) * node_num, SEEK_SET);
    status = fwrite(nci, sizeof(struct nci), 1, info->nci_file->put) == 1;
    fflush(info->nci_file->put);
#endif /* __VMS */
  }

/****************************
  Else it is open for edit so
  the characteristics are just a
  memory reference away.
*****************************/

  else
    *(info->edit->nci +
      node_num - info->edit->first_in_mem) = *nci;
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
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID *nid = (NID *)&nid_in;
  int       status;
  int       node_num;
  TREE_INFO *info;
  NCI       nci;
  NODE     *node;
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  nid_to_tree_nidx(dblist, nid, info, node_num);
  if (!info)
    return TreeNNF;
  status = TreeGetNciLw(info, node_num, &nci);
  if (~status & 1)
    return status;
  if (nci.NCI_FLAG_WORD.NCI_FLAGS.state)
  {
    nci.NCI_FLAG_WORD.NCI_FLAGS.state = 0;
    status = TreePutNci(info, node_num, &nci, 0);
    if (~status & 1)
      return status;
    if (!nci.NCI_FLAG_WORD.NCI_FLAGS.parent_state)
    {
      nid_to_node(dblist, nid, node);
      if (node->child)
	status = SetParentState(dblist, child_of(node), 0);
      if (node->member)
	status = SetParentState(dblist, member_of(node), 0);
    }
    else
      status = TreePARENT_OFF;
#ifdef __VMS
    if (info->flush && info->nci_file)
    {
      if (info->nci_file->putrab)
      {
	sys$wait(info->nci_file->putrab);
	sys$flush(info->nci_file->putrab);
      }
    }
#endif /* __VMS */
  }
  else
  {
    status = UnlockNci(info, node_num);
    if (status & 1)
      status = TreeALREADY_ON;
  }
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
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID 	    *nid = (NID *)&nid_in;
  int       status;
  int       node_num;
  TREE_INFO *info;
  NCI       nci;
  int       i;
  NODE     *node;
  if (!IS_OPEN(dblist))
    return TreeNOT_OPEN;
  nid_to_tree_nidx(dblist, nid, info, node_num);
  if (!info)
    return TreeNNF;
  status = TreeGetNciLw(info, node_num, &nci);
  if (~status & 1)
    return status;
  if (!nci.NCI_FLAG_WORD.NCI_FLAGS.state)
  {
    nci.NCI_FLAG_WORD.NCI_FLAGS.state = 1;
    status = TreePutNci(info, node_num, &nci, 0);
    if (~status & 1)
      return status;
    if (!nci.NCI_FLAG_WORD.NCI_FLAGS.parent_state)
    {
      nid_to_node(dblist, nid, node);
      if (node->child)
	status = SetParentState(dblist, child_of(node), 1);
      if (node->member)
	status = SetParentState(dblist, member_of(node), 1);
    }
#ifdef __VMS
    if (info->flush && info->nci_file)
    {
      if (info->nci_file->putrab)
      {
	sys$wait(info->nci_file->putrab);
	sys$flush(info->nci_file->putrab);
      }
    }
#endif /* __VMS */
  }
  else
  {
    status = UnlockNci(info, node_num);
    if (status & 1)
      status = TreeALREADY_OFF;
  }
  return status;
}

/*------------------------------------------------------------------------------

		Name: UnlockNci

		Type:   C function

		Author:	Josh Stillerman
			MIT Plasma Fusion Center

		Date:    22-MAR-1988

		Purpose: Routine which releases an characteristics record which has been
			 read with a lock.

------------------------------------------------------------------------------

	Call sequence:
	       STATUS = TreeUnlockNci(info_ptr, node_num)

------------------------------------------------------------------------------
   Copyright (c) 1988
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:


+-----------------------------------------------------------------------------*/

static int UnlockNci(TREE_INFO *info, int node_num)
{
  int       status;
  status = 1;
#ifdef __VMS
  if (info->nci_file != 0 && info->nci_file->putrab != 0)
    status = sys$release(info->nci_file->putrab, 0, 0);
  if (status == RMS$_RNL)
    status = 1;
#endif /* __VMS */
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

static int SetParentState(PINO_DATABASE *db, NODE *node, unsigned int state)
{
  int       status;
  NCI       nci;
  NODE     *lnode;
  status = 1;
  for (lnode = node; lnode && (status & 1); lnode = brother_of(lnode))
  {
    status = SetNodeParentState(db, lnode, &nci, state);
    if ((status & 1) && (!nci.NCI_FLAG_WORD.NCI_FLAGS.state) && (lnode->child))
      status = SetParentState(db, child_of(lnode), state);
    if ((status & 1) && (!nci.NCI_FLAG_WORD.NCI_FLAGS.state) && (lnode->member))
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

static int SetNodeParentState(PINO_DATABASE *db, NODE *node, NCI *nci, unsigned int state)
{
  TREE_INFO *info;
  int       node_num;
  int status;
  for (info = db->tree_info; 
       info && ((node < info->node) || (node > (info->node + info->header->nodes))); info = info->next_info);
  if (!info)
    return TreeNNF;
  node_num = node - info->node;
  status = TreeGetNciLw(info, node_num, nci);
  if (status & 1)
  {
    nci->NCI_FLAG_WORD.NCI_FLAGS.parent_state = state;
    status = TreePutNci(info, node_num, nci, 0);
  }
  return status;
}

