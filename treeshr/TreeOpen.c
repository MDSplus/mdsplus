#include <mdsdescrip.h>
#include <mdsshr.h>
#include <treeshr.h>
#include "treeshrp.h"
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <usagedef.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>

#define __toupper(c) (((c) >= 'a' && (c) <= 'z') ? (c) & 0xDF : (c))
#define __tolower(c) (((c) >= 'A' && (c) <= 'Z') ? (c) | 0x20 : (c))

int treeshr_errno = 0;

extern int MdsEventCan();
static void RemoveBlanksAndUpcase(char *out, char *in);
static int CloseTopTree(PINO_DATABASE *dblist, int call_hook);
static int ConnectTree(PINO_DATABASE *dblist, char *tree, NODE *parent, char *subtree_list);
static int CreateDbSlot(PINO_DATABASE **dblist, char *tree, int shot, int editting);
static int OpenTreefile(char *tree, int shot, TREE_INFO *info, int edit_flag, int *remote_file, void **file_handle);
static int MapFile(void *file_handle, TREE_INFO *info, int edit_flag, int remote_file);
static int ReadTree(TREE_INFO *info, int edit_flag);
static int GetVmForTree(TREE_INFO *info);
static int MapTree(char *tree, int shot, TREE_INFO *info, int edit_flag);
static void SubtreeNodeConnect(NODE *parent, NODE *subtreetop);

void *DBID = NULL;

int TreeClose(char *tree, int shot) { return _TreeClose(&DBID,tree,shot); }
int TreeEditing() { return _TreeEditing(DBID);}
int TreeGetStackSize() { return _TreeGetStackSize(DBID);}
int TreeIsOpen() { return _TreeIsOpen(DBID);}
int TreeOpen(char *tree, int shot, int read_only) { return _TreeOpen(&DBID,tree,shot,read_only);}
int TreeSetStackSize(int size) { return _TreeSetStackSize(&DBID, size);}
void TreeRestoreContext(void *ctx) { _TreeRestoreContext(&DBID,ctx); }
void *TreeSaveContext() { return _TreeSaveContext(DBID);}
int TreeOpenEdit(char *tree, int shot) { return _TreeOpenEdit(&DBID,tree,shot);}

int _TreeOpen(void **dbid, char *tree_in, int shot_in, int read_only_flag)
{
	int       status = TreeFAILURE;
	int       shot;
	char     *subtree_list;
	char     *tree = malloc(strlen(tree_in)+1);

	RemoveBlanksAndUpcase(tree,tree_in);
	if (subtree_list = strchr(tree, ',')) 
		*subtree_list++ = 0;

/**************************************************
 To open a tree we need only to connect up the MAIN
 tree which will in turn link up any subtrees.
**************************************************/

	shot = shot_in ? shot_in : MdsGetCurrentShotid(tree);
	if (shot)
	{
		PINO_DATABASE **dblist = (PINO_DATABASE **)dbid;
		status = CreateDbSlot(dblist, tree, shot, 0);
		if (status == TreeNORMAL)
		{
			if ((status = ConnectTree(*dblist, tree, 0, subtree_list)) == TreeNORMAL ||
				(status == TreeNOTALLSUBS))
			{
				(*dblist)->default_node = (*dblist)->tree_info->root;
				(*dblist)->open = 1;
				(*dblist)->open_readonly = read_only_flag;
				(*dblist)->remote = 0;
			}
			else if ((status = ConnectTreeRemote(*dblist, tree, subtree_list)) == TreeNORMAL ||
				(status == TreeNOTALLSUBS))
			{
				(*dblist)->open = 1;
				(*dblist)->open_readonly = read_only_flag;
				(*dblist)->remote = 1;
			}
			else
			{
				PINO_DATABASE *db;
				for (db = *dblist; db->next; db=db->next);
				if (db)
				{
					db->next = *dblist;
					*dblist = (*dblist)->next;
					db->next->next = 0;
				}
			}
		}
		else if (status == TreeALREADY_OPEN)
			(*dblist)->open_readonly = 0;
	}
	return status;
}

static void RemoveBlanksAndUpcase(char *out, char *in)
{
	while(*in)
	{                          
		char c = *in++;
		if (c != (char)32 && c != (char)9)
			*out++=__toupper(c);
	}
	*out=0;
}

int _TreeClose(void **dbid, char *tree, int shot)
{
	PINO_DATABASE **dblist = (PINO_DATABASE **)dbid;
	int       status;
	PINO_DATABASE *db;
	PINO_DATABASE *prev_db;
	status = TreeNOT_OPEN;
	if (dblist && *dblist)
	{
		if (tree)
		{
			char uptree[13];
			int i;
			int len = strlen(tree);
			for (i=0;i<12 && i<len;i++)
				uptree[i] = __toupper(tree[i]);
			uptree[i]='\0';
			status = TreeNOT_OPEN;
			if (!shot)
				shot = MdsGetCurrentShotid(tree);
			for (prev_db = 0, db = *dblist; db ? db->open : 0; prev_db = db, db = db->next)
			{
				if ((shot == db->shotid) && (strcmp(db->main_treenam, uptree) == 0))
				{
					if (prev_db)
					{
						prev_db->next = db->next;
						db->next = *dblist;
						*dblist = db;
					}
					status = TreeNORMAL;
					break;
				}
			}
		}
		else
			status = TreeNORMAL;
		if (status & 1)
		{
			if ((*dblist)->modified)
			{
				status = TreeWRITEFIRST;
			}
			else if ((*dblist)->open)
			{
				status = CloseTopTree(*dblist,1);
				if (status & 1)
				{
					for (prev_db = 0, db = *dblist; db; prev_db = db, db = db->next);
					if (prev_db && (*dblist)->next)
					{
						db = (*dblist)->next;
						(*dblist)->next = 0;
						prev_db->next = *dblist;
						*dblist = db;
					}
				}
			}
			else
				status = TreeNOT_OPEN;
		}
	}
	return status;
}

static int CloseTopTree(PINO_DATABASE *dblist, int call_hook)
{
	TREE_INFO *local_info = dblist ? dblist->tree_info : 0;
	TREE_INFO *previous_info;
	int       status;

	status = TreeNORMAL;
	if (dblist)
	{
		if (dblist->remote)
		{
			status = CloseTreeRemote(dblist, call_hook);
		}
		else if (local_info)
		{

	/************************************************
     We check the BLOCKID just to make sure that what
     we were passed in indeed was a tree info block.
     This is a safety check because if we just assumed
     the block was correct, mapping and freeing random
     pages of memory would cause serious problems that
     may not show up immediately.
    *************************************************/

			if (local_info->blockid == TreeBLOCKID)
			{

      /******************************************************
       If the edit pointer field of this tree is non-zero,
       the tree has been opened for editting. The edit pointer
       points to a structure containing the number of virtual
       memory pages that were dynamically allocated for each
       section of the tree. If any of the tree sections had
       virtual memory allocated, this memory must be freed.
      *****************************************************/

				if (local_info->edit)
				{
					static int tree_edit_size = sizeof(TREE_EDIT);
					if (local_info->edit->header_pages)
						free(local_info->header);
					if (local_info->edit->node_vm_size)
						free(local_info->node);
					if (local_info->edit->tags_pages)
						free(local_info->tags);
					if (local_info->edit->tag_info_pages)
						free(local_info->tag_info);
					if (local_info->edit->external_pages)
						free(local_info->external);
#ifdef __VMS
					if (local_info->edit->nci_section[0])
						sys$deltva(local_info->edit->nci_section, 0, 0);
					if (local_info->edit->nci_channel)
						sys$dassgn(local_info->edit->nci_channel);
#endif
					free(local_info->edit);
				}

	  /********************************************************
	   For each tree in the linked list, first the pages must be
       remapped to the pagefile using sys$cretva before they can
       be freed for general use. This automatically unmaps the
       tree section file pages. Once the section is unmapped the
       I/O channel can be deassigned. Then the virtual memory
       can be deallocated using lib$free_vm_page.
       Finally the tree information block memory can be released.
      *********************************************************/

				while (local_info)
				{
					if (local_info->blockid == TreeBLOCKID)
					{
						if (local_info->rundown_id)
							MdsEventCan(local_info->rundown_id);
						if (local_info->section_addr[0])
						{
#if defined(__VMS)
							if ((status = sys$cretva(local_info->section_addr, 0, 0)) & 1)
							if ((status = sys$dassgn(local_info->channel)) & 1)
							status = lib$free_vm_page(&local_info->vm_pages, &local_info->vm_addr);
#else
#if defined(__osf__)
							if (local_info->channel)
							{
							  close(local_info->channel);
							  munmap(local_info->section_addr[0],local_info->alq * 512);
							}
#endif
							free(local_info->vm_addr);
#endif
						}
						TreeWait(local_info);
						if (local_info->data_file)
						{
							MdsFree1Dx(local_info->data_file->data,NULL);
#ifdef __VMS
							sys$close(local_info->data_file->$a_fab, 0, 0);
#else
							if (local_info->data_file->get)    fclose(local_info->data_file->get);
							if (local_info->data_file->put)    fclose(local_info->data_file->put);
							if (local_info->data_file->update) fclose(local_info->data_file->update);
#endif
							free(local_info->data_file);
							local_info->data_file = NULL;
						}
						if (local_info->nci_file)
						{
#ifdef __VMS
							sys$close(local_info->nci_file->$a_fab, 0, 0);
#else
							if (local_info->nci_file->get)    fclose(local_info->nci_file->get);
							if (local_info->nci_file->put)    fclose(local_info->nci_file->put);
#endif
							free(local_info->nci_file);
							local_info->nci_file = NULL;
						}
						if (call_hook) 
							TreeCallHook(CloseTree, local_info);
						if (local_info->filespec)
							free(local_info->filespec);
						if (local_info->treenam)
							free(local_info->treenam);
						previous_info = local_info;
						local_info = local_info->next_info;
						free(previous_info);
					}
				}
				dblist->tree_info = 0;
			}
			else
				status = TreeINVTREE;
        }
		if (status & 1)
		{
			dblist->open = 0;
			dblist->open_for_edit = 0;
			dblist->modified = 0;
			dblist->remote = 0;
			free(dblist->experiment);
			free(dblist->main_treenam);
		}
	}
	return status;
}

int _TreeIsOpen(void *dbid)
{
	PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
	return IS_OPEN_FOR_EDIT(dblist) ? TreeOPEN_EDIT : (IS_OPEN(dblist) ? TreeOPEN : TreeNOT_OPEN);
}

int _TreeEditing(void *dbid)
{
	PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
	return IS_OPEN_FOR_EDIT(dblist) ? TreeNORMAL : TreeNOEDIT;
}

static int ConnectTree(PINO_DATABASE *dblist, char *tree, NODE *parent, char *subtree_list)
{
	int       status;
	int       ext_status;
	int       i;
	TREE_INFO *info;
	TREE_INFO *iptr;


/***********************************************
  If the parent's usage is not subtree then
  just return success.
************************************************/

	if (parent && parent->usage!=TreeUSAGE_SUBTREE)
		return TreeNORMAL;

/***********************************************
  If there is a treelist (canditates) then if 
  this tree is not in it then just return 
  success.
************************************************/

	if (subtree_list)
	{
		char *found;
		char *tmp_list = malloc(strlen(subtree_list)+3);
		char *tmp_tree = malloc(strlen(tree)+3);
		int llen = strlen(subtree_list);
		int slen = strlen(tree);
		strcpy(tmp_list,",");
		strcat(tmp_list,subtree_list);
		strcat(tmp_list,",");
		strcpy(tmp_tree,",");
		strcat(tmp_tree,tree);
		strcat(tmp_tree,",");
		found = strstr(tmp_list,tmp_tree);
		free(tmp_list);
		free(tmp_tree);
		if (!found)
			return TreeNORMAL;
	}

/***********************************************
  Get virtual memory for the tree
  information structure and zero the structure.
***********************************************/

  info = malloc(sizeof(TREE_INFO));
  if (info)
  {
	  memset(info,0,sizeof(*info));

   /***********************************************
    Next we map the file and if successful copy
    the tree name (blank filled) into the info block.
    ***********************************************/

	  info->flush = (dblist->shotid == -1);
	  info->treenam = strcpy(malloc(strlen(tree)+1),tree);
	  status = MapTree(tree, dblist->shotid, info, 0);
	  if (status == TreeFAILURE && treeshr_errno == TreeFILE_NOT_FOUND)
	  {
		  status = TreeCallHook(RetrieveTree, info);
		  if (status == TreeNORMAL)
			  status = MapTree(tree, dblist->shotid, info, 0);
	  }
	  if (status == TreeNORMAL)
	  {
		  TreeCallHook(OpenTree, info);

      /**********************************************
       If this is the main tree the root node is the
       first node in the tree. If it is a subtree we
       must make the parent/subtree node connections
       and the referencing node is replaced by
       the root node of the subtree.
      **********************************************/

		  info->root = info->node;
		  if (parent == 0)
			  dblist->tree_info = info;
		  else
		  {
			  SubtreeNodeConnect(parent, info->node);
			  for (iptr = dblist->tree_info; iptr->next_info; iptr = iptr->next_info);
			  iptr->next_info = info;
		  }

      /***********************************************
       For each of the external references (subtrees),
       We make a string descriptor for the subtree
       name using the node name of the referencing node
       and then recursively call this routine to connect
       the subtree(s).
      *************************************************/

		  for (i = 0; i < info->header->externals; i++)
		  {
			  NODE     *external_node = info->node + info->external[i];
			  char *subtree = strncpy(malloc(sizeof(NODE_NAME)+1),external_node->name,sizeof(NODE_NAME));
			  char *blank = strchr(subtree,32);
			  if (blank) *blank = 0;
			  ext_status = ConnectTree(dblist, subtree, external_node, subtree_list);
			  free(subtree);
			  if (!(ext_status & 1))
			  {
				  status = TreeNOTALLSUBS;
				  if (treeshr_errno == TreeCANCEL) 
					  break;
			  }
		  }
	  }
	  else
	  {
		  free(info->treenam);
		  free(info);
	  }
  }
  return status;
}

#define move_to_top(prev_db,db)\
	if (prev_db) \
            {prev_db->next = db->next;\
            db->next = *dblist;\
            *dblist = db;}

static int CreateDbSlot(PINO_DATABASE **dblist, char *tree, int shot, int editting)
{
	int       status;
	PINO_DATABASE *db;
	PINO_DATABASE *prev_db;
	PINO_DATABASE *saved_prev_db;
	PINO_DATABASE *useable_db = 0;
	int       count;
	int	    stack_size = 8;
	enum options
	{
		MOVE_TO_TOP, CLOSE, ERROR_DIRTY, OPEN_NEW
	};
	enum options option = OPEN_NEW;

	if (*dblist) 
		stack_size = (*dblist)->stack_size;
	for (prev_db = 0, db = *dblist; db ? db->open : 0; prev_db = db, db = db->next)
	{
		if (db->shotid == shot)
		{
			if (strcmp(db->main_treenam, tree) == 0)
			{
				if (editting)
				{
					if (db->open_for_edit)
					{
						option = MOVE_TO_TOP;
						break;
					}
					else
					{
						option = CLOSE;
						break;
					}
				}
				else
				{
					if (db->open_for_edit)
					{
						if (db->modified)
						{
							option = ERROR_DIRTY;
							break;
						}
						else
						{
							option = CLOSE;
							break;
						}
					}
					else
					{
						option = MOVE_TO_TOP;
						break;
					}
				}
			}
		}
	}
	switch (option)
	{
	case MOVE_TO_TOP:
		move_to_top(prev_db, db);
		status = TreeALREADY_OPEN;
		break;

	case CLOSE:
		move_to_top(prev_db, db);
		_TreeClose((void **)dblist,0,0);
		status = TreeNORMAL;
		break;
	case ERROR_DIRTY:
		status = TreeFAILURE;
		treeshr_errno = TreeWRITEFIRST;
		break;
	default:

		for (count = 0, prev_db = 0, db = *dblist; db; count++, prev_db = db, db = db->next)
		{
			if (!db->open)
			{
				move_to_top(prev_db, db);
				status = TreeNORMAL;
				break;
			}
			else if (!(db->open_for_edit && db->modified))
			{
				saved_prev_db = prev_db,
				useable_db = db;
			}
		}
		if (!db)
		{
			if (count >= stack_size)
			{
				if (useable_db)
				{
					move_to_top(saved_prev_db, useable_db);
					_TreeClose((void **)dblist,0,0);
					status = TreeNORMAL;
				}
				else
				{
					status = TreeFAILURE;
					treeshr_errno = TreeMAXOPENEDIT;
				}
			}
			else
			{
				db = *dblist;
				*dblist=malloc(sizeof(PINO_DATABASE));
				if (*dblist)
				{
					(*dblist)->tree_info = 0;
					(*dblist)->next = db;
					(*dblist)->open = 0;
					(*dblist)->open_for_edit = 0;
					(*dblist)->modified = 0;
					(*dblist)->experiment = 0;
					(*dblist)->main_treenam = 0;
					status = TreeNORMAL;
				}
			}
		}
	}
	if (status == TreeNORMAL)
	{
		(*dblist)->shotid = shot;
		(*dblist)->setup_info = (shot == -1);
		(*dblist)->experiment = strcpy(malloc(strlen(tree)+1),tree);
		(*dblist)->main_treenam = strcpy(malloc(strlen(tree)+1),tree);
		(*dblist)->stack_size = stack_size;
	}
	return status;
}

int _TreeGetStackSize(void *dbid)
{
	PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
	return dblist->stack_size;
}

int _TreeSetStackSize(void **dbid, int size)
{
	PINO_DATABASE *dblist = *(PINO_DATABASE **)dbid;
	int new_size = size > 0 ? (size < 11 ? size : 10) : 1;
	int old_size = dblist ? dblist->stack_size : 8;
	if (!dblist)
		CreateDbSlot((PINO_DATABASE **)dbid, "", 987654321, 0);
	for (dblist = *(PINO_DATABASE **)dbid; dblist; dblist = dblist->next) dblist->stack_size = new_size;
	if (dblist && dblist->remote) SetStackSizeRemote(dblist,new_size);
	return old_size;
}

#ifdef _MSC_VER
#include <wtypes.h>
#include <winreg.h>
static char *GetRegistryPath(char *pathname)
{
	HKEY regkey1,regkey2,regkey3;
	unsigned char *path = NULL;
	if ( (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE",0,KEY_READ,&regkey1) == ERROR_SUCCESS) &&
		(RegOpenKeyEx(regkey1,"MIT",0,KEY_READ,&regkey2) == ERROR_SUCCESS) &&
		(RegOpenKeyEx(regkey2,"MDSplus",0,KEY_READ,&regkey3) == ERROR_SUCCESS) )
	{
		unsigned long valtype;
		unsigned long valsize;
		if (RegQueryValueEx(regkey3,pathname,0,&valtype,NULL,&valsize) == ERROR_SUCCESS)
		{
			path = malloc(++valsize);
			RegQueryValueEx(regkey3,pathname,0,&valtype,path,&valsize);
		}
	}
	RegCloseKey(regkey1);
	RegCloseKey(regkey2);
	RegCloseKey(regkey3);
	return (char *)path;
}
#endif

static FILE  *OpenOne(TREE_INFO *info, char *tree, int shot, char *type,char *options,char **resnam_out)
{
	FILE *file = 0;
	int len = strlen(tree);
	char tree_lower[13];
	char pathname[32];
	char *path;
	char name[32];
	int i;
	char *resnam = 0;
	for (i=0;i<len && i < 12;i++)
		tree_lower[i] = __tolower(tree[i]);
	tree_lower[i]=0;
	strcpy(pathname,tree_lower);
	strcat(pathname,TREE_PATH_SUFFIX);
#if defined(__VMS)
	path = pathname;
#elif defined(_MSC_VER)
	path = GetRegistryPath(pathname);
#else
	path = getenv(pathname);
#endif
	if (path)
	{
		char *part;
		int pathlen = strlen(path);
		path = strcpy(malloc(pathlen+1),path);
		if (shot < 0)
			sprintf(name,"%s_model",tree_lower);
		else if (shot < 1000)
			sprintf(name,"%s_%03d",tree_lower,shot);
		else
			sprintf(name,"%s_%d",tree_lower,shot);
		for (i=0,part=path;i<pathlen+1 && file==0;i++)
		{
			if (*part == ' ') 
				part++;
			else if ((path[i] == ' ' || path[i] == ';' || path[i] == 0) && strlen(part))
			{
				path[i] = 0;
				resnam = strcpy(malloc(strlen(part)+strlen(name)+strlen(type)+2),part);
				if (strcmp(resnam+strlen(resnam)-1,TREE_PATH_DELIM))
					strcat(resnam,TREE_PATH_DELIM);
				strcat(resnam,name);
				strcat(resnam,type);
#ifdef __osf__
				info->channel = open(resnam,O_RDONLY);
				file = info->channel ? fdopen(info->channel,"rb") : NULL;
#else
				info->channel = 0;
				file = fopen(resnam,"rb");
#endif
				if (file == NULL)
				{
					//		  perror("Error opening tree file");
					free(resnam);
					resnam = NULL;
				}
				part = &path[i+1];
			}
		}
		free(path);
	}
	if (resnam_out)
		*resnam_out = file ? resnam : 0;
	else if (resnam)
		free(resnam);
	return file;
}

static int MapTree(char *tree, int shot, TREE_INFO *info, int edit_flag)
{
	int       status;
	int       remote_file;
	void     *file_handle;


	/******************************************
	First we need to open the tree file.
	If successful, we create and map a global
	section on the tree file.
	*******************************************/

	status = OpenTreefile(tree, shot, info, edit_flag, &remote_file, &file_handle);
	if (status == TreeNORMAL)
		status = MapFile(file_handle, info, edit_flag, remote_file);
	free(file_handle);
	return status;
}

static int OpenTreefile(char *tree, int shot, TREE_INFO *info, int edit_flag, int *remote_file, void **file_handle)
{
	int       status;

#ifndef __VMS

	FILE **file;
	char *resnam;
	*file_handle = malloc(sizeof(FILE *));
	file = (FILE **)*file_handle;
	*file = OpenOne(info, tree, shot, TREE_TREEFILE_TYPE, "r", &resnam);
	if (*file)
	{
		fseek(*file, 0, SEEK_END);
		info->alq = ftell(*file) / 512;
		fseek(*file, 0, SEEK_SET);
		status = TreeNORMAL;
		info->filespec=resnam;
		*remote_file = info->channel == 0;
	}
	else if (errno == ENOENT)
		status = TreeFILE_NOT_FOUND;
	else
		status = TreeFAILURE;

#else

	struct FAB *fab;
	struct NAM *nam;
	struct XABFHC *xab;
	int       i;
	short     length;
	char      name[255];
	int tlen = strlen(tree);
	char     *filnam = malloc(tlen * 2 + 20);
	if (shot < 0)
		sprintf(filnam,"%s$DATA:%s_MODEL",tree,tree);
	else if (shot < 1000)
		sprintf(filnam,"%s$DATA:%s_%03d",tree,tree,shot);
	else
		sprintf(filnam,"%s$DATA:%s_%d",tree,tree,shot);
	*file_handle = malloc(sizeof(struct FAB)+sizeof(struct NAM)+sizeof(struct XAB));
	fab = (struct FAB *)(*file_handle);
	nam = (struct NAM *)(fab + 1);
	xab = (struct XABFHC *)(nam + 1);
	*fab = cc$rms_fab;
	*nam = cc$rms_nam;
	*xab = cc$rms_xabfhc;
	fab->fab$l_dna = "*"TREE_TREEFILE_TYPE;
	fab->fab$b_dns = sizeof("*"TREE_TREEFILE_TYPE);
	fab->fab$b_fac = edit_flag ? FAB$M_PUT : FAB$M_GET;
	fab->fab$l_fna = filnam;
	fab->fab$b_fns = strlen(filnam);
	fab->fab$b_shr = edit_flag ? (FAB$M_SHRGET | FAB$M_UPI) : (FAB$M_SHRGET | FAB$M_SHRPUT | FAB$M_UPI);
	fab->fab$l_fop = FAB$M_UFO;
	fab->fab$l_nam = nam;
	fab->fab$l_xab = (char *) xab;
	nam->nam$b_ess = 255;
	nam->nam$l_esa = name;
	status = sys$open(fab);
	if (status == RMS$_SUPPORT ||	/* DECNET */
		!(fab->fab$l_dev & (DEV$M_ELG | DEV$M_NET))/* NFS */)
	{
		*remote_file = 1;
		fab->fab$l_fop = 0;
		sys$dassgn(fab->fab$l_stv);
		status = sys$open(fab);
	}
	else
		*remote_file = 0;
	if (status & 1)
	{
		info->channel = fab->fab$l_stv;
		info->alq = xab->xab$l_ebk - 1;
		memcpy(info->tree_info_w_fid,nam->nam$w_fid,sizeof(info->tree_info_w_fid));
		memcpy(info->tree_info_t_dvi,nam->nam$t_dvi,sizeof(info->tree_info_t_dvi));
		for (length = 0; length < nam->nam$b_esl; length++)
			if (((char *) nam->nam$l_esa)[length] == ';')
			break;
		info->filespec=strcpy(malloc(length+1),nam->nam$l_esa);
		status = TreeNORMAL;
	}
	else if (status == RMS$_FNF)
		status = TreeFILE_NOT_FOUND;
	free(filnam);
#endif

	return status;
}

static int MapFile(void *file_handle, TREE_INFO *info, int edit_flag, int remote_file)
{
	int       status;

	/********************************************
	First we get virtual memory for the tree to
	be mapped into.
	********************************************/

	status = GetVmForTree(info);
	if (status == TreeNORMAL)
	{
		if (remote_file)
		{

#ifndef __VMS
			fread((void *)info->section_addr[0], 512, info->alq, *(FILE **)file_handle);
			fclose(*(FILE **)file_handle);
			status = 1;
#else

			/******************************************
			Read in tree structure using RMS
			******************************************/
			struct RAB rab;
			rab = cc$rms_rab;
			rab.rab$l_fab = file_handle;
			rab.rab$l_rop = RAB$M_NLK;
			rab.rab$w_usz = 512;
			status = sys$connect(&rab);
			if (status & 1)
			{
				int       i;
				for (i = 0, rab.rab$l_ubf = (char *) info->section_addr[0];
				(i < info->alq) && (status & 1);
				i++, rab.rab$l_ubf += 512)
					status = sys$get(&rab);
			}
			sys$close(file_handle);
			info->channel = 0;

#endif

		}
		else
		{
#if defined(__VMS)
			int       retadr[2];
			/************************************************
			Use file id for the section name.
			If we are not editting create and map a global
			section, readonly.
			If we are editting, create a copy on reference
			writeable private section.
			************************************************/
			status = sys$crmpsc(info->section_addr, retadr, 0, edit_flag == 0 ? SEC$M_GBL : SEC$M_CRF | SEC$M_WRT,
				TreeSectionName(info), 0, 0, info->channel, info->alq, 0, 0, 0);
#elif defined(__osf__)
		        status = mmap(info->section_addr[0],info->alq * 512,PROT_READ | PROT_WRITE, MAP_FILE | MAP_PRIVATE | MAP_FIXED, 
                            info->channel, 0) != (void *)-1;
#endif
                }
		/***********************************************
		If we successfully mapped the file, see if
		we got all of the file and the return addresses
		match what we asked for.
		If ok, fill in the addresses of the various
		parts of the tree based on the information in
		the tree header.
		************************************************/

		if (status & 1)
		{
			info->blockid = TreeBLOCKID;
			info->header = (TREE_HEADER *) info->section_addr[0];
			info->node = (NODE *) (info->section_addr[0] + ((sizeof(TREE_HEADER) + 511) / 512) * 512);
			info->tags = (int *) (((char *)info->node) + ((info->header->nodes * sizeof(NODE) + 511) / 512) * 512);
			info->tag_info = (TAG_INFO *) (((char *)info->tags) + ((info->header->tags * 4 + 511) / 512) * 512);
			info->external = (int *) (((char *)info->tag_info) + ((info->header->tags * sizeof(TAG_INFO) + 511) / 512) * 512);
			TreeEstablishRundownEvent(info);
			status = TreeNORMAL;
		}
		else
		{
#ifdef __VMS
			if (!remote_file)
				sys$dassgn(info->channel);
#endif
			free(info->vm_addr);
		}
	}
	return status;
}

static int GetVmForTree(TREE_INFO *info)
{
	int status;

#if defined(__ALPHA) && defined(__VMS)
	int PAGE_SIZE=64;
#elif defined(__osf__)
	int PAGE_SIZE = sysconf(_SC_PAGE_SIZE)/512;
#else
	int PAGE_SIZE=1;
#endif
#define align(addr)  addr = (((unsigned long)addr) % (PAGE_SIZE * 512)) ? \
	addr - (((unsigned long)addr) % (PAGE_SIZE * 512)) + PAGE_SIZE * 512 : addr

	info->vm_pages = ((info->alq + PAGE_SIZE)/PAGE_SIZE * PAGE_SIZE) + PAGE_SIZE;
	info->vm_addr = malloc(info->vm_pages*512);
	if (info->vm_addr)
	{
		info->section_addr[0] = info->vm_addr;
		align(info->section_addr[0]);
		info->section_addr[1] = info->section_addr[0] + info->alq * 512 - 1;
		align(info->section_addr[1]);
		info->section_addr[1]--;
		status = TreeNORMAL;
	}
	else
		status = TreeFAILURE;
	return status;
}


struct descriptor *TreeSectionName(TREE_INFO *info)
{
#ifndef __VMS
	return 0;
#else
	static const _DESCRIPTOR(fao,"!AC_!4XW!4XW!4XW");
	char sectnam[28];
	struct dsc_descriptor sectnam_d = {0,DTYPE_T,CLASS_S,0};
	sectnam_d.length = sizeof(sectnam);
	sectnam_d.pointer = sectnam;
	sys$fao(&fao,&sectnam_d.length,&sectnam_d,info->dvi,
		info->fid[0],info->fid[1],info->fid[2]);
	return &sectnam_d;
#endif
}

#ifndef __VMS

#define SetPageModifiable(a)
#define SetPageNomodifiable(a)

#else

#ifdef __ALPHA
#define page_size (64 * 512)
#else
#define page_size 512
#endif

static void SetPageModifiable(NODE *node)
{

	char     *begaddr = (char *)node;
	char     *endaddr = ((char *)node) + sizeof(NODE) -1;
	char     *inaddr[2];
	char     *retadr[2];
	char     *page_boundary = ((char *)node) - ((unsigned long) node % page_size);
	int       copysize = (endaddr > (page_boundary + page_size - 1)) ? page_size * 2 : page_size;
	char      *temp_pages = malloc(page_size * 2);
	int       status;
	/********************************
	We need to retain a copy of the
	page to be set modifiable. Next
	we create a new demand zero
	page at that address. And finally
	copy the page back to the new page.
	**********************************/
	inaddr[0] = page_boundary;
	inaddr[1] = page_boundary + copysize - 1;
	memcpy(temp_pages, page_boundary, copysize);
	status = sys$cretva(inaddr, retadr, 0);
	memcpy(page_boundary, temp_pages, copysize);
	free(temp_pages);
	return;
}

static void SetPageNomodifiable(NODE *node)
{
	char     *begaddr = (char *)node;
	char     *endaddr = ((char *)node) + sizeof(NODE) -1;
	char     *inaddr[2];
	char     *retadr[2];
	char     *page_boundary = ((char *)node) - ((unsigned long) node % page_size);
	int       copysize = (endaddr > (page_boundary + page_size - 1)) ? page_size * 2 : page_size;
	inaddr[0] = page_boundary;
	inaddr[1] = page_boundary + copysize - 1;
	sys$setprt(inaddr, 0, 0, PRT$C_UR, 0);	/* Set page user read only */
	return;
}

#endif

static void SubtreeNodeConnect(NODE *parent, NODE *subtreetop)
{
	NODE     *grandparent = parent_of(parent);

	/*************************************************
	We must connect the parent node to its child
	node by modifying the child index of the parent.
	Before this field can be modified we must make
	the page of memory modifiable. After modification
	the page will be set back to readonly.
	*************************************************/

	if (child_of(grandparent) == parent)
	{
		SetPageModifiable(grandparent);
		grandparent->child = link_of(subtreetop, grandparent);
		SetPageNomodifiable(grandparent);
	}
	else
	{
		NODE     *bro;
		for (bro = child_of(grandparent); brother_of(bro) && (brother_of(bro) != parent); bro = brother_of(bro));
		if (brother_of(bro))
		{
			SetPageModifiable(bro);
			bro->brother = link_of(subtreetop, bro);
			SetPageNomodifiable(bro);
		}
	}
	SetPageModifiable(subtreetop);
	memcpy(subtreetop->name, parent->name, sizeof(subtreetop->name));
	subtreetop->parent = link_of(grandparent, subtreetop);
	if (parent->brother)
		subtreetop->brother = link_of(brother_of(parent), subtreetop);
	SetPageNomodifiable(subtreetop);
	return;
}

struct context
{
	char *expt;
	int  shot;
	int  edit;
	int  readonly;
	int  defnid;
};

void *_TreeSaveContext(void *dbid)
{
	PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
	struct context *ctx = NULL;
	if (IS_OPEN(dblist))
	{
		ctx = malloc(sizeof(struct context));
		ctx->expt = strcpy(malloc(strlen(dblist->experiment)+1),dblist->experiment);
		ctx->shot = dblist->shotid;
		ctx->edit = dblist->open_for_edit;
		ctx->readonly = dblist->open_readonly;
		_TreeGetDefaultNid(dbid,&ctx->defnid);
	}
	return (void *)ctx;
}

void _TreeRestoreContext(void **dbid, void *ctxin)
{
	if (ctxin)
	{
		struct context *ctx = (struct context *)ctxin;
		if (ctx->edit)
			_TreeOpenEdit(dbid, ctx->expt, ctx->shot);
		else
			_TreeOpen(dbid, ctx->expt, ctx->shot, ctx->readonly);
		_TreeSetDefaultNid(*dbid, ctx->defnid);
		free(ctx->expt);
		free(ctx);
	}
}

int TreeCloseFiles(TREE_INFO *info)
{
	int       status;
	status = TreeNORMAL;
	if (info)
	{
		if (info->blockid == TreeBLOCKID)
		{
			info->reopen = 0;
			TreeWait(info);
			if (info->data_file)
			{
				MdsFree1Dx(info->data_file->data, NULL);
#ifdef __VMS
				sys$close(info->data_file->fab, 0, 0);
#else
				if (info->data_file->get)    fclose(info->data_file->get);
				if (info->data_file->put)    fclose(info->data_file->put);
				if (info->data_file->update) fclose(info->data_file->update);
#endif
				free(info->data_file);
				info->data_file = NULL;
			}
			if (info->nci_file)
			{
#ifdef __VMS
				sys$close(info->nci_file->$fab, 0, 0);
#else
				if (info->nci_file->get)    fclose(info->nci_file->get);
				if (info->nci_file->put)    fclose(info->nci_file->put);
#endif
				free(info->nci_file);
				info->nci_file = NULL;
			}
		}
		else
			status = TreeINVTREE;
	}
	return status;
}

int       _TreeOpenEdit(void **dbid, char *tree_in, int shot_in)
{
  PINO_DATABASE **dblist = (PINO_DATABASE **)dbid;
  TREE_INFO *info;
  char     *tree = malloc(strlen(tree_in)+1);
  int       shot;
  int       status = TreeFAILURE;

  RemoveBlanksAndUpcase(tree,tree_in);
  shot = shot_in ? shot_in : MdsGetCurrentShotid(tree);
  if (shot)
  {
    PINO_DATABASE **dblist = (PINO_DATABASE **)dbid;
    status = CreateDbSlot(dblist, tree, shot, 1);
    if (status == TreeNORMAL)
    {
      info = (TREE_INFO *)malloc(sizeof(TREE_INFO));
      if (info)
      {
        memset(info,0,sizeof(*info));
        info->flush = ((*dblist)->shotid == -1);
        info->treenam = strcpy(malloc(strlen(tree)+1),tree);
        status = MapTree(tree, (*dblist)->shotid, info, 1);
        if (status == TreeFAILURE && treeshr_errno == TreeFILE_NOT_FOUND)
        {
          status = TreeCallHook(RetrieveTree, info);
          if (status == TreeNORMAL)
              status = MapTree(tree, (*dblist)->shotid, info, 1);
        }
        if (status & 1)
        {
	  TreeCallHook(OpenTreeEdit, info);
          info->edit = (TREE_EDIT *)malloc(sizeof(TREE_EDIT));
  	  if (info->edit)
	  {
	    int       new_tree = 0;
            memset(info->edit, 0, sizeof(TREE_EDIT));
	    (*dblist)->tree_info = info;
	    (*dblist)->open = 1;
	    (*dblist)->open_for_edit = 1;
	    (*dblist)->open_readonly = 0;
	    info->root = info->node;
	    (*dblist)->default_node = info->root;
	  }
	}
	else
	{
          free(info->treenam);
          free(info);
	}
      }
    }
  }
  return status;
}
