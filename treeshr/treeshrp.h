#ifndef _TREESHRP_H
#define _TREESHRP_H

/************* Data structures internal to TREESHR ***********/

#include <mdsdescrip.h>
#include <treeshr_hooks.h>

/********************************************
  NCI

  Each node has associated with it a fixed
  length characteristics record.  This will
  eventually be the 6th section of the tree file.
  for now they will live in their own file.
*********************************************/


#ifdef __DECC
#pragma member_alignment save
#pragma nomember_alignment
#define swapint(in) in
#endif

#ifdef __hpux__
#pragma HP_ALIGN NOPADDING PUSH
static int swapint(int in)
{
  int out;
  char *out_c = (char *)&out;
  char *in_c = (char *)&in;
  int i;
  for (i=0;i<4;i++) out_c[3-i] = in_c[i];
  return out;
}
#endif

#ifdef _MSC_VER
#pragma pack(push,enter_include,1)
#define swapint(in) in
#endif

typedef struct nci
{
  union
  {
    unsigned int flags;
    struct
    {
      unsigned  state:1;
      unsigned  parent_state:1;
      unsigned  essential:1;
      unsigned  :3;
      unsigned  setup_information:1;
      unsigned  write_once:1;
      unsigned  compressible:1;
      unsigned  do_not_compress:1;
      unsigned  compress_on_put:1;
      unsigned  nowrite_model:1;
      unsigned  nowrite_shot:1;
      unsigned  path_reference:1;
      unsigned  nid_reference:1;
      unsigned  include_in_pulse:1;
    }         NCI_FLAGS;
  }         NCI_FLAG_WORD;
  unsigned  char data_in_att_block:1;
  unsigned  char error_on_put:1;
  unsigned  char :8;
  unsigned int time_inserted[2];
  unsigned int owner_identifier;
  unsigned char class;
  unsigned char dtype;
  unsigned int length;
  unsigned char :8;
  unsigned int status;
  union
  {
    struct
    {
      unsigned char file_level;
      unsigned char file_version;
      unsigned short rfa[3];
      unsigned int record_length;
    }         DATA_LOCATION;
    struct
    {
      unsigned char element_length;
      unsigned char data[11];
    }         DATA_IN_RECORD;
    struct
    {
      unsigned int error_status;
      unsigned int stv;
    }         ERROR_INFO;
  }         DATA_INFO;
  unsigned char nci_fill;
}         NCI;

/*****************************************
  NID

The NID structure is used to identify
nodes in a tree (Node ID). NID's are valid
for invocations of the same MAIN tree and
can be passed between processes.
******************************************/

typedef struct nid
{
  unsigned node:24;	/* Node offset of root node of tree this node belongs to */
  unsigned tree:8;	/* Level of tree in chained tree_info blocks 0=main tree */
}         NID;
/******************************************
Two macros are provided for converting from
a NID to a node address or from a node
address to a NID:

   nid_to_node(pino, nid, node)
   node_to_nid(pino, node, nid)

******************************************/

#define nid_to_node(pino,nid,saved_node) \
     {\
     TREE_INFO *nid_to_node__info;\
     unsigned int nid_to_node__i;\
     for (nid_to_node__info = pino->tree_info,nid_to_node__i = 0; \
          nid_to_node__i < nid->tree; nid_to_node__i++)\
             nid_to_node__info = nid_to_node__info ? nid_to_node__info->next_info : 0;\
     saved_node = nid_to_node__info && (nid_to_node__info->header->nodes >= (int)nid->node)\
                      ? nid_to_node__info->node + (int)nid->node : 0;\
     }

#define node_to_nid(pino,node_in,nid) \
     {\
     TREE_INFO *node_to_nid__info;\
     nid->tree = 0;\
     for (node_to_nid__info = pino->tree_info; node_to_nid__info != NULL;) \
       if ( (node_in >= node_to_nid__info->node) && \
            (node_in <= (node_to_nid__info->node + \
             node_to_nid__info->header->nodes))) \
       { \
         nid->node = node_in - node_to_nid__info->node; \
         node_to_nid__info = 0; \
       } \
       else \
       { \
         nid->tree++; \
         node_to_nid__info =  node_to_nid__info->next_info; \
       }\
     }
/******************************************
Another useful macro based on nid:

nid_to_tree_nidx(pino, nid, info, nidx)
*******************************************/

#define nid_to_tree_nidx(pino, nid, info, nidx) \
    {\
      unsigned int nid_to_tree_nidx__i;\
      info = pino->tree_info;\
      for (nid_to_tree_nidx__i=0; info ? nid_to_tree_nidx__i < nid->tree : 0; nid_to_tree_nidx__i++) \
               info = info->next_info; \
      info = info->header->nodes >= (int)nid->node ? info : 0; \
      nidx = info ? nid->node : 0; \
    }


/********************************************
   NODE

2) The second section of a tree file contains
   nodes.
*********************************************/

typedef char NODE_NAME[12];
/*********************************************
 Linkages to other nodes via parent, brother,
 member and child node links are expressed in
 self relative byte offsets. To connect to the
 previous node a link should be set to
 -sizeof(NODE). To connect to the following
 node it should be set to sizeof(NODE) etc.
*********************************************/

typedef struct node
{
  NODE_NAME name;
  int       parent;
  int       member;
  int       brother;
  int       child;
  unsigned char usage;
  unsigned short conglomerate_elt;
  char      fill;
  unsigned int tag_link;	/* Index of tag info block pointing to this node (index of first tag is 1) */
}         NODE;
#ifdef EMPTY_NODE
static const NODE empty_node = {{'e', 'm', 'p', 't', 'y', ' ', 'n', 'o', 'd', 'e', ' ', ' '}, sizeof(NODE), 0, 0, -sizeof(NODE), 0, 0, 0, 0};
#endif

/*****************************************************
 Macros to perform integer arithmetic of node pointers
 and linkages and end up with node pointers
 *****************************************************/

#define parent_of(a)  (NODE *)((a)->parent  ? (char *)(a) + swapint((a)->parent)  : 0)
#define member_of(a)  (NODE *)((a)->member  ? (char *)(a) + swapint((a)->member)  : 0)
#define child_of(a)   (NODE *)((a)->child   ? (char *)(a) + swapint((a)->child)   : 0)
#define brother_of(a) (NODE *)((a)->brother ? (char *)(a) + swapint((a)->brother) : 0)
#define link_of(a,b)  swapint((char *)(a) - (char *)(b))

/*****************************************************
  Search structures
*****************************************************/

#define MAX_SEARCH_LEVELS 32

typedef enum search_type
{
  NONE,
  MEMBER_START,
  BROTHER_START,
  SON_BROTHER_TYPE,
  SON_MEMBER_BROTHER_TYPE,
  TAG_TYPE,
  ANCESTOR_TYPE,
  PARENT_TYPE,
  BROTHER_TYPE,
  BROTHER_TYPE_NOWILD,
  TAG_TREE_TYPE,
  MEMBER_TYPE,
  MEMBER_TYPE_NOWILD,
  ASTASTAST_TYPE,
  SON_MEMBER_TYPE,
  SON_MEMBER_TYPE_NOWILD,
  EOL
}         SEARCH_TYPE;

typedef struct node_list
{
  NODE *node;
  struct node_list *next;
}         NODELIST;

typedef struct search_context
{
  int         level;
  SEARCH_TYPE type;
  char        *string;
  short       len;
  char        *tag_tree_name;
  short       tag_tree_name_len;
  NODE        *node;
  NODE        *stop;
  NODELIST    *stack;
}         SEARCH_CONTEXT;


/********************************************
   TAG_INFO

4) The fourth section of a tree file contains
   tag information blocks. These blocks are
   not sorted but the tag index list (previous
   section) provide a sorted index of the tags.
   The tags are located using a binary search
   of the tag indexes
*********************************************/

typedef unsigned char TAG_NAME[24];

typedef struct tag_info
{
  TAG_NAME  name;
  int       node_idx;	/* Node to which this tag is assigned   */
  int       tag_link;	/* Index of next tag also assigned to this node (index of first tag is 1) */
}         TAG_INFO;

/*********************************************
   TREE_HEADER

1) The first section of a tree file is the
   header. The tree header is the first 512
   bytes of a tree file
*********************************************/

typedef struct tree_header
{
  char      version;	/* Version of tree file format */
  unsigned  char sort_children:1;	/* Sort children flag */
  unsigned  char sort_members:1;	/* Sort members  flag */
  unsigned  char : 6;
  char      fill1[6];
  int       free;	/* First node in free node list (connected by PARENT/CHILD indexes */
  int       tags;	/* Number of tags defined                                          */
  int       externals;	/* Number of externals/subtrees referenced                         */
  int       nodes;	/* Number of nodes allocated (both defined and free node)          */
  char      fill2[488];
}         TREE_HEADER;

/********************************************
   TREE_EDIT

The tree edit structure is used during tree
editting. It keeps track of dynamic memory
allocations for tree expansions
*********************************************/

typedef struct tree_edit
{
  int       header_pages;
  int       node_vm_size;
  int       tags_pages;
  int       tag_info_pages;
  int       external_pages;
  int       nci_section[2];
  int       nci_channel;
  NCI       *nci;
  int       first_in_mem;
  short     conglomerate_size;
  short     conglomerate_index;
  unsigned  conglomerate_setup:1;
}         TREE_EDIT;
#ifdef EMPTY_EDIT
static const TREE_EDIT empty_edit;
#endif

/********************************************
TREE_IO.H
Contains definitions of all structures used
in doing I/O to the tree files.
********************************************/

#define DATAF_C_MAX_RECORD_SIZE 32765 - sizeof(RECORD_HEADER)

#define TREE_DATAFILE_TYPE ".datafile"
#define TREE_NCIFILE_TYPE  ".characteristics"
#define TREE_TREEFILE_TYPE ".tree"


#ifndef __VMS

#include <stdio.h>

#else

#include <fab.h>
#include <rab.h>
#include <nam.h>
#include <xab.h>
#include <secdef.h>
#include <devdef.h>
#include <starlet.h>
#include <prtdef.h>

#pragma extern_model save
#pragma extern_model globalvalue
extern RMS$_SUPPORT;
#pragma extern_model restore

#endif

/********************************************
The NCI_FILE structure contains the stuff
needed to do I/O on the node characteristics
file.
*********************************************/

#ifndef __VMS

typedef struct nci_file
{
  FILE *get;
  FILE *put;
  NCI  nci;
}  NCI_FILE;

#define NCI_FILE_VM_SIZE sizeof(NCI_FILE) + 2 * sizeof(FILE *)
#else

typedef struct nci_file
{
  struct NAM *nam;
  struct RAB *getrab;
  struct RAB *putrab;
  struct FAB *fab;
  NCI	     nci;
}         NCI_FILE;

#define NCI_FILE_VM_SIZE sizeof(NCI_FILE) + sizeof(struct NAM) + sizeof(struct RAB) + sizeof(struct RAB) + sizeof(struct FAB)
#endif

/***********************************
Defines RFA type as 6 characters.
Used for moving RFA's around
efficiently.
************************************/
typedef struct
{
  char      rfa[6];
}         RFA;

#define RfaToSeek(rfa) (((*(unsigned int *)rfa - 1) * 512) + (*(unsigned short *)&((char *)rfa)[4] & 0x1ff))
#define SeekToRfa(seek,rfa) {*(unsigned int *)rfa = (unsigned int)(seek/512 + 1); \
                             *(unsigned short *)&(((char *)rfa)[4]) = (unsigned short)(seek % 512);}

/****************************************
RECORD_HEADER
VFC portion of file.
***************************************/
typedef struct record_header
{
  unsigned  short rlength;
  int       node_number;
  RFA       rfa;
}         RECORD_HEADER;


/**************************************
ASY_NCI
Structure used for passing information
to asynchronous I/O completion routines
***************************************/

typedef struct asy_nci
{
  struct tree_info *info;
  NCI       *nci;
}         ASY_NCI;

/*******************************************
DATA_FILE structure used for I/O to datafile
********************************************/

#ifndef __VMS

typedef struct data_file
{
  unsigned  open_for_write:1;
  unsigned: 7;
  FILE *get;
  FILE *put;
  FILE *update;
  RECORD_HEADER *record_header;
  ASY_NCI *asy_nci;
  struct descriptor_xd *data;
}         DATA_FILE;

#else

typedef struct data_file
{
  unsigned  open_for_write:1;
  unsigned: 7;
  struct NAM *nam;
  struct RAB *getrab;
  struct RAB *putrab;
  struct RAB *updaterab;
  struct FAB *fab;
  RECORD_HEADER *record_header;
  ASY_NCI *asy_nci;
  struct descriptor_xd *data;
}         DATA_FILE;

#endif

/********************************************
    TREE_INFO

The tree info structure contains information
of trees which have been activated. There will
be one tree info block per tree activated.
*********************************************/

typedef struct tree_info
{
  int       blockid;/* Identifier indicating a valid tree info block    */
  char      *treenam;	/* Tree name                                        */
  int       shot;       /* Shot number */
  int       vm_pages;	/* Number of pages and address of virtual memory    */
  void      *vm_addr;/*  allocated for mapping the tree file             */
  char      *section_addr[2];	/* Beginning and ending addresses of mapped section */
  TREE_HEADER *header;	/* Pointer to file header                           */
  NODE     *node;	/* Pointer to nodes                                 */
  int      *tags;	/* Pointer to tag indexes                           */
  TAG_INFO *tag_info;	/* Pointer to tag information blocks                */
  int      *external;	/* Pointer to external reference node indexes       */
  int       channel;/* I/O channel to tree file                         */
  int       alq;	/* Blocks allocated to tree file                    */
  char     *filespec;	/* Pointer to full file spec of tree file		  */
  char      dvi[16];	/* Tree file disk info */
  unsigned short tree_info_w_fid[3];	/* Tree file file id                                */
  unsigned  flush:1;/* Flush I/O's buffers                              */
  unsigned  reopen:1;  /* Re-open nci and datafile on next access */
  unsigned  rundown:1; /* Doing rundown */
  unsigned  :13;/* Spare bits */
  int       rundown_id;  /* Rundown event id */
  NODE *root;	/* Pointer to top node                              */
  TREE_EDIT *edit;	/* Pointer to edit block (if editting the tree      */
  NCI_FILE *nci_file;	/* Pointer to nci file block (if open)              */
  DATA_FILE *data_file;	/* Pointer to a datafile access block               */
  struct tree_info *next_info;	/* Pointer to next tree info block                  */
}         TREE_INFO;

/********************************************
   PINO_DATABASE

The PINO_DATABASE structure is used to maintain
a tree context. The pino database structure is
always passed as an argument to tree traversal
routines.
*********************************************/

typedef struct pino_database
{
  TREE_INFO *tree_info;	/* Pointer to main tree info block */
  NODE *default_node;	/* Pointer to current default node */
  char        *experiment;	/* Pointer to experiment descriptor */
  char        *main_treenam;	/* Pointer to name used for main tree */
  int       shotid;	/* Shot identification */
  unsigned  open:1;	/* Flag indication block has open tree */
  unsigned  open_for_edit:1;	/* Flag indicating tree is open for edit */
  unsigned  open_readonly:1;	/* Flag indicating tree open readonly */
  unsigned  modified:1;	/* Flag indicating tree structure modified */
  unsigned  setup_info:1;/* Flag indicating setup info is being added */
  unsigned  remote:1; /* Flag indicating tree is on remote system */

  void      *remote_ctx; /* Pointer to remote system context */

  struct pino_database *next;	/* Link to next database in open list */


  int	    stack_size;
}         PINO_DATABASE;

/****************************
Macro's for checking access
to databases
*****************************/

#define IS_OPEN(db)          ((db) ? (db)->open : 0)
#define IS_OPEN_FOR_EDIT(db) (IS_OPEN(db) ? (db)->open_for_edit : 0)


#if defined(__VMS)
#define TREE_PATH_SUFFIX "$DATA"
#define TREE_PATH_DELIM  ":"
#elif defined(_MSC_VER)
#define TREE_PATH_SUFFIX "_path"
#define TREE_PATH_DELIM  "\\"
#else
#define TREE_PATH_SUFFIX "_path"
#define TREE_PATH_DELIM  "/"
#endif

/************* Prototypes for internal functions *************/
extern int ConnectTreeRemote(PINO_DATABASE *dblist, char *tree, char *subtree_list);
extern int SetStackSizeRemote(PINO_DATABASE *dblist, int stack_size);

extern int CloseTreeRemote(PINO_DATABASE *dblist, int call_hook);

extern int GetNciRemote(PINO_DATABASE *dblist, int nid_in, struct nci_itm *nci_itm);

extern int GetRecordRemote(PINO_DATABASE *dblist, int nid_in, struct descriptor_xd *dsc);

extern int FindNodeRemote(PINO_DATABASE *dblist, char *path, int *outnid);

extern int FindNodeWildRemote(PINO_DATABASE *dblist, char *path, int *nid_out, void **ctx_inout, int usage_mask);

extern int FindNodeEndRemote(PINO_DATABASE *dblist, void **ctx_in);

extern char *FindNodeTagsRemote(PINO_DATABASE *dblist, int nid_in, void **ctx_ptr);

extern char *AbsPathRemote(PINO_DATABASE *dblist, char *inpath);

extern int SetDefaultNidRemote(PINO_DATABASE *dblist, int nid);

extern int GetDefaultNidRemote(PINO_DATABASE *dblist, int *nid);



extern int TreeCloseFiles(TREE_INFO *info);

extern int TreeGetNciW(TREE_INFO *info, int node_number, NCI *nci);
extern int TreeGetNciLw(TREE_INFO *info, int node_number, NCI *nci);
extern int TreePutNci(TREE_INFO *info, int node_number, NCI *nci, int flush);
extern int TreeIsChild(NODE *node);
extern struct descriptor *TreeSectionName(TREE_INFO *info);
extern int TreeFindTag(PINO_DATABASE *db, NODE *node, char *treename, char **search_string, NODE **node_in_out);
extern int TreeCallHook(TreeshrHookType operation, TREE_INFO *info);
extern int TreeEstablishRundownEvent(TREE_INFO *info);
extern DATA_FILE *TreeGetVmDatafile();

#ifdef __DECC
#pragma member_alignment restore
#endif

#ifdef _MSC_VER
#pragma pack(pop,enter_include)
#endif

#ifdef __hpux__
#pragma HP_ALIGN POP
#endif

#endif
