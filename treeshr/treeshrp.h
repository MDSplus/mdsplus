#ifndef _TREESHRP_H
#define _TREESHRP_H

/************ Data structures internal to TREESHR ***********/

#include <stdio.h>
#include <stdlib.h>
#include <mdsdescrip.h>
#include <mdstypes.h>
#include <treeshr_hooks.h>
#include <ncidef.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_VXWORKS_H
#define _int64 long long
#define _int64u long long
#endif

/********************************************
  NCI

  Each node has associated with it a fixed
  length characteristics record.  This will
  eventually be the 6th section of the tree file.
  for now they will live in their own file.
*********************************************/
#define   NciM_DATA_IN_ATT_BLOCK   0x01
#define   NciV_DATA_IN_ATT_BLOCK      0
#define   NciM_ERROR_ON_PUT        0x02
#define   NciV_ERROR_ON_PUT           1
#define   NciM_DATA_CONTIGUOUS     0x04
#define   NciV_DATA_CONTIGUOUS        2
#define   NciM_NON_VMS             0x08
#define   NciV_NON_VMS                3
typedef struct nci
{
  unsigned int  flags;
  unsigned char flags2;
  unsigned char spare;
  _int64        time_inserted;
  unsigned int  owner_identifier;
  unsigned char class;
  unsigned char dtype;
  unsigned int  length;
  unsigned char spare2;
  unsigned int  status;
  union
  {
    struct
    {
      unsigned char file_level;
      unsigned char file_version;
      unsigned char rfa[6];
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

#if defined(__GNUC__) || defined(__APPLE)
#define PACK_ATTR __attribute__ ((__packed__))
#define PACK_START 
#define PACK_STOP
#else
#define PACK_ATTR
#define PACK_START #pragma pack(1)
#define PACK_STOP   #pragma pack(4)
#endif

#if defined(WORDS_BIGENDIAN)

#define LoadShort(in,outp) ((char *)(outp))[0] = ((char *)&in)[1]; ((char *)(outp))[1] = ((char *)&in)[0]
#define LoadInt(in,outp)   ((char *)(outp))[0] = ((char *)&in)[3]; ((char *)(outp))[1] = ((char *)&in)[2]; \
                           ((char *)(outp))[2] = ((char *)&in)[1]; ((char *)(outp))[3] = ((char *)&in)[0]
#define LoadQuad(in,outp)  (outp)[0] = ((char *)&in)[7]; (outp)[1] = ((char *)&in)[6]; \
                           (outp)[2] = ((char *)&in)[5]; (outp)[3] = ((char *)&in)[4]; \
                           (outp)[4] = ((char *)&in)[3]; (outp)[5] = ((char *)&in)[2]; \
                           (outp)[6] = ((char *)&in)[1]; (outp)[7] = ((char *)&in)[0]

#else

#define LoadShort(in,outp) ((char *)(outp))[0] = ((char *)&in)[0]; ((char *)(outp))[1] = ((char *)&in)[1]
#define LoadInt(in,outp)   ((char *)(outp))[0] = ((char *)&in)[0]; ((char *)(outp))[1] = ((char *)&in)[1]; \
                           ((char *)(outp))[2] = ((char *)&in)[2]; ((char *)(outp))[3] = ((char *)&in)[3]
#define LoadQuad(in,outp)  (outp)[0] = ((char *)&in)[0]; (outp)[1] = ((char *)&in)[1]; \
                           (outp)[2] = ((char *)&in)[2]; (outp)[3] = ((char *)&in)[3]; \
                           (outp)[4] = ((char *)&in)[4]; (outp)[5] = ((char *)&in)[5]; \
                           (outp)[6] = ((char *)&in)[6]; (outp)[7] = ((char *)&in)[7]

#endif

#define swapquad(ptr) ( (((_int64)((unsigned char *)ptr)[7]) << 56) | (((_int64)((unsigned char *)ptr)[6]) << 48) | \
                        (((_int64)((unsigned char *)ptr)[5]) << 40) | (((_int64)((unsigned char *)ptr)[4]) << 32) | \
                        (((_int64)((unsigned char *)ptr)[3]) << 24) | (((_int64)((unsigned char *)ptr)[2]) << 16) | \
                        (((_int64)((unsigned char *)ptr)[1]) <<  8) | (((_int64)((unsigned char *)ptr)[0]) ))
#define swapint(ptr) ( (((int)((unsigned char *)(ptr))[3]) << 24) | (((int)((unsigned char *)(ptr))[2]) << 16) | \
                       (((int)((unsigned char *)(ptr))[1]) <<  8) | (((int)((unsigned char *)(ptr))[0]) )) 
#define swapshort(ptr) ( (((int)((unsigned char *)ptr)[1]) << 8) | (((int)((unsigned char *)ptr)[0]) ))

#define bitassign(bool,value,mask) value = (bool) ? (value) | (mask) : (value) & ~(mask)
#define bitassign_c(bool,value,mask) value = (char)((bool) ? (value) | (mask) : (value) & ~(mask))

/*****************************************
  NID

The NID structure is used to identify
nodes in a tree (Node ID). NID's are valid
for invocations of the same MAIN tree and
can be passed between processes.
******************************************/

#ifdef WORDS_BIGENDIAN
typedef struct nid
{
  unsigned tree:8;
  unsigned node:24;
} NID;
#else
typedef struct nid
{
  unsigned node:24;	/* Node offset of root node of tree this node belongs to */
  unsigned tree:8;	/* Level of tree in chained tree_info blocks 0=main tree */
}         NID;
#endif

#define MAX_SUBTREES 256 /* since there are only 8 bits of tree number in a nid */

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

#ifdef HAVE_WINDOWS_H
#pragma pack(1)
#else
PACK_START
#endif
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
  union {
    struct {
      int       member;
      int       brother;
      int       child;
    } TREE_INFO;
    struct {
      struct big_node_linkage *big_linkage PACK_ATTR;
    } LINK_INFO;
  }INFO ;
  unsigned char usage;
  unsigned short conglomerate_elt PACK_ATTR;
  char      fill;
  unsigned int tag_link;	/* Index of tag info block pointing to this node (index of first tag is 1) */
}         NODE;

#ifdef EMPTY_NODE
static NODE empty_node = {{'e', 'm', 'p', 't', 'y', ' ', 'n', 'o', 'd', 'e', ' ', ' '}}; 
#endif

/********************************************
   BIG_NODE_LINKAGE

Nodes generally contain byte offsets to the set 
of adjacent nodes (child, member, brother, parent).
When a subtree is mapped, there is a chance that 
it will be placed in memory at an adress that is 
too far away to be represented by a 32 bit offset.
if this is the case, the the parent pointer of the 
node is set to -1, it is followed in the node 
structure by a pointer to an array of these 
BIG_NODE_LINAGEs which contain the addresses 
(NOT OFFSETS) of the related nodes.To dereference a 
node, if its parent is -1, we search through this 
table to find the corresponding entry 
(there will not be many of them), and use the 
pointer instead of the offset.
*********************************************/
typedef struct big_node_linkage {
  NODE *node;
  NODE *parent;
  NODE *child;
  NODE *member;
  NODE *brother;
} BIG_NODE_LINKAGE;

/*****************************************************
 Macros to perform integer arithmetic of node pointers
 and linkages and end up with node pointers
 *****************************************************/

/* if pointers are more than 32 bits then node offsets can be 
   more than 32 bits */
#if SIZEOF_INT_P == 8
#define parent_of(a)\
  (((a)->parent == -1) ? (a)->INFO.LINK_INFO.big_linkage->parent : (NODE *)((a)->parent  ? (char *)(a) + swapint((char *)&((a)->parent))  : 0))
#define member_of(a)\
  (((a)->parent == -1) ? (a)->INFO.LINK_INFO.big_linkage->member : (NODE *)((a)->INFO.TREE_INFO.member  ? (char *)(a) + swapint((char *)&((a)->INFO.TREE_INFO.member))  : 0))
#define child_of(a)\
  (((a)->parent == -1) ? (a)->INFO.LINK_INFO.big_linkage->child : (NODE *)((a)->INFO.TREE_INFO.child   ? (char *)(a) + swapint((char *)&((a)->INFO.TREE_INFO.child))   : 0))
#define brother_of(a)\
  (((a)->parent == -1) ? (a)->INFO.LINK_INFO.big_linkage->brother : (NODE *)((a)->INFO.TREE_INFO.brother ? (char *)(a) + swapint((char *)&((a)->INFO.TREE_INFO.brother)) : 0))
#define link_it(out,a,b)  out = (int)(((a) != 0) && ((b) != 0)) ? (char *)(a) - (char *)(b) : 0; out = swapint((char *)&out)
#define link_it2(dblist,nodeptr,field,a,b)  \
  if (((char *)(a) - (char *)(b)) >= 2^32) {\
    int i; \
    if (nodeptr->parent != -1) {\
      for (i=0;  (i<(2*MAX_SUBTREES)) && (dblist->big_node_linkage[i].node !=0); i++);\
      dblist->big_node_linkage[i].node = nodeptr;\
      dblist->big_node_linkage[i].parent = parent_of(nodeptr);\
      dblist->big_node_linkage[i].child = child_of(nodeptr);\
      dblist->big_node_linkage[i].member = member_of(nodeptr);\
      dblist->big_node_linkage[i].brother = brother_of(nodeptr);\
      nodeptr->parent = -1;\
      nodeptr->INFO.LINK_INFO.big_linkage=dblist->big_node_linkage+i;\
    }\
    nodeptr->INFO.LINK_INFO.big_linkage->field=(a);\
  } else {\
    nodeptr->INFO.LINK_INFO.big_linkage->field = (int)(((a) != 0) && ((b) != 0)) ? (char *)(a) - (char *)(b) : 0; nodeptr->INFO.LINK_INFO.big_linkage->field = swapint((char *)&nodeptr->INFO.LINK_INFO.big_linkage->field);\
  }
#define link_parent(dblist,nodeptr,a,b)  \
  if (((char *)(a) - (char *)(b)) >= 2^32) {\
    int i; \
    if (nodeptr->parent != -1) {\
      for (i=0;  (i<(2*MAX_SUBTREES)) && (dblist->big_node_linkage[i].node !=0); i++);\
      dblist->big_node_linkage[i].node = nodeptr;\
      dblist->big_node_linkage[i].parent = parent_of(nodeptr);\
      dblist->big_node_linkage[i].child = child_of(nodeptr);\
      dblist->big_node_linkage[i].member = member_of(nodeptr);\
      dblist->big_node_linkage[i].brother = brother_of(nodeptr);\
      nodeptr->parent = -1;\
      nodeptr->INFO.LINK_INFO.big_linkage=dblist->big_node_linkage+i;\
    }\
    nodeptr->INFO.LINK_INFO.big_linkage->parent=(a);\
  } else {\
    nodeptr->parent = (int)(((a) != 0) && ((b) != 0)) ? (char *)(a) - (char *)(b) : 0; nodeptr->parent = swapint((char *)&nodeptr->parent);\
  }
#else
#define parent_of(a)  (NODE *)((a)->parent  ? (char *)(a) + swapint((char *)&((a)->parent))  : 0)
#define member_of(a)  (NODE *)((a)->INFO.TREE_INFO.member  ? (char *)(a) + swapint((char *)&((a)->INFO.TREE_INFO.member))  : 0)
#define child_of(a)   (NODE *)((a)->INFO.TREE_INFO.child   ? (char *)(a) + swapint((char *)&((a)->INFO.TREE_INFO.child))   : 0)
#define brother_of(a) (NODE *)((a)->INFO.TREE_INFO.brother ? (char *)(a) + swapint((char *)&((a)->INFO.TREE_INFO.brother)) : 0)
#define link_it(out,a,b)  out = (int)(((a) != 0) && ((b) != 0)) ? (char *)(a) - (char *)(b) : 0; out = swapint((char *)&out)
#define link_it2(dblist,node,field,a,b)  \
node->field = (int)(((a) != 0) && ((b) != 0)) ? (char *)(a) - (char *)(b) : 0; node->field = swapint((char *)&node->field)
#define link_parent(dblist,nodeptr,a,b)  \
nodeptr->parent = (int)(((a) != 0) && ((b) != 0)) ? (char *)(a) - (char *)(b) : 0; nodeptr->parent = swapint((char *)&nodeptr->parent)
#endif

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
#ifdef _AIX
  unsigned sort_children:1;
  unsigned sort_members:1;
  unsigned versions_in_model:1;
  unsigned versions_in_pulse:1;
  unsigned :4;
#else
  unsigned  char sort_children:1;	/* Sort children flag */
  unsigned  char sort_members:1;	/* Sort members  flag */
  unsigned  char versions_in_model:1;
  unsigned  char versions_in_pulse:1;
  unsigned  char : 4;
#endif
  char      fill1[6];
  int       free;	/* First node in free node list (connected by PARENT/CHILD indexes */
  int       tags;	/* Number of tags defined                                          */
  int       externals;	/* Number of externals/subtrees referenced                         */
  int       nodes;	/* Number of nodes allocated (both defined and free node)          */
  char      fill2[488];
}         TREE_HEADER;

/***********************************
Defines RFA type as 6 characters.
Used for moving RFA's around
efficiently.
************************************/
typedef struct
{
  unsigned char rfa[6] PACK_ATTR;
}         RFA; 

#ifdef RFA_MACROS
#define RfaToSeek(rfa) (((*(unsigned int *)rfa - 1) * 512) + (*(unsigned short *)&((char *)rfa)[4] & 0x1ff))
#define SeekToRfa(seek,rfa) {*(unsigned int *)rfa = (unsigned int)(seek/512 + 1); \
                             *(unsigned short *)&(((char *)rfa)[4]) = (unsigned short)(seek % 512);}
#endif

/****************************************
RECORD_HEADER
VFC portion of file.
***************************************/
typedef struct record_header
{
  unsigned  short rlength PACK_ATTR;
  int       node_number PACK_ATTR;
  RFA       rfa PACK_ATTR;
}         RECORD_HEADER;

#ifdef HAVE_WINDOWS_H
#pragma pack(4)
#else
PACK_STOP
#endif



/*****************************************************
  Search structures
*****************************************************/

#define MAX_SEARCH_LEVELS 32

#ifdef HAVE_VXWORKS_H
#undef NONE
#endif

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



/********************************************
The NCI_FILE structure contains the stuff
needed to do I/O on the node characteristics
file.
*********************************************/

typedef struct nci_file
{
  int get;
  int put;
  NCI  nci;
}  NCI_FILE;

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

typedef struct data_file
{
  unsigned  open_for_write:1;
  unsigned: 7;
  int get;
  int put;
  RECORD_HEADER *record_header;
  ASY_NCI *asy_nci;
  struct descriptor_xd *data;
}         DATA_FILE;

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
  BIG_NODE_LINKAGE big_node_linkage[2*MAX_SUBTREES];  /* We need at most 2 of these
							 per subtree.  The subtree is 
							 either a first child in which
							 case we need to fix up its parent
							 and the node itself's brother offsets,
							 or it is a middle (or end) child and 
							 we need to fix up its previous sibling 
							 and the node itself's parent, and brother
							 pointers. */
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
#define TREE_PATH_DELIM  "/"
#else
#define TREE_PATH_SUFFIX "_path"
#define TREE_PATH_DELIM  "/"
#endif

/************* Prototypes for internal functions *************/
extern int ConnectTreeRemote(PINO_DATABASE *dblist, char *tree, char *subtree_list, char *, int status);
extern int SetStackSizeRemote(PINO_DATABASE *dblist, int stack_size);

extern int CloseTreeRemote(PINO_DATABASE *dblist, int call_hook);

extern int CreatePulseFileRemote(PINO_DATABASE *dblist, int shotid, int *nids, int num);

extern int GetNciRemote(PINO_DATABASE *dblist, int nid_in, struct nci_itm *nci_itm);

extern int GetRecordRemote(PINO_DATABASE *dblist, int nid_in, struct descriptor_xd *dsc);

extern char *GetRegistryPath(char *pathname);

extern int FindNodeRemote(PINO_DATABASE *dblist, char *path, int *outnid);

extern int FindNodeWildRemote(PINO_DATABASE *dblist, char *path, int *nid_out, void **ctx_inout, int usage_mask);

extern int FindNodeEndRemote(PINO_DATABASE *dblist, void **ctx_in);

extern char *FindNodeTagsRemote(PINO_DATABASE *dblist, int nid_in, void **ctx_ptr);

extern char *AbsPathRemote(PINO_DATABASE *dblist, char *inpath);

extern int SetDefaultNidRemote(PINO_DATABASE *dblist, int nid);

extern int GetDefaultNidRemote(PINO_DATABASE *dblist, int *nid);

extern _int64 RfaToSeek(unsigned char *rfa);
void SeekToRfa(_int64 seek, unsigned char *rfa);
extern int SetParentState(PINO_DATABASE *db, NODE *node, unsigned int state);

extern int TreeCloseFiles(TREE_INFO *info);
extern int TreeExpandNodes(PINO_DATABASE *db_ptr, int num_fixup, NODE ***fixup_nodes);
extern int TreeFindParent(PINO_DATABASE *dblist, char *path_ptr, NODE **node_ptrptr, char **namedsc_ptr, SEARCH_TYPE *type_ptr);
extern int TreeGetNciW(TREE_INFO *info, int node_number, NCI *nci,unsigned int version);
extern int TreeGetNciLw(TREE_INFO *info, int node_number, NCI *nci);
extern int TreeInsertChild(NODE *parent_ptr,NODE *child_ptr,int  sort);
extern int TreeInsertMember(NODE *parent_ptr,NODE *member_ptr,int  sort);
extern int TreePutNci(TREE_INFO *info, int node_number, NCI *nci, int flush);
extern int TreeIsChild(NODE *node);
extern struct descriptor *TreeSectionName(TREE_INFO *info);
/* extern int TreeFindTag(PINO_DATABASE *db, NODE *node, char *treename, char **search_string, NODE **node_in_out); */
extern int TreeFindTag(char *tagnam, char *treename, int *tagidx);
int _TreeFindTag(PINO_DATABASE *db, NODE *default_node, short treelen, char *tree, short taglen, char *tagnam, NODE **nodeptr, int *
tagidx);
extern int TreeCallHook(TreeshrHookType operation, TREE_INFO *info,int nid);
extern int TreeEstablishRundownEvent(TREE_INFO *info);
extern DATA_FILE *TreeGetVmDatafile();
extern int TreeOpenNciW(TREE_INFO *info, int tmpfile);
extern int TreeOpenDatafileW(TREE_INFO *info, int *stv_ptr, int tmpfile);
extern void TreeSerializeNciIn(char *in, struct nci *out);
extern void TreeSerializeNciOut(struct nci *in, char *out);
extern int TreeLockNci(TREE_INFO *info, int readonly, int nodenum);
extern int TreeUnLockNci(TREE_INFO *info, int readonly, int nodenum);
extern int TreeLockDatafile(TREE_INFO *info, int readonly, _int64 where);
extern int TreeUnLockDatafile(TREE_INFO *info, int readonly, _int64 where);
extern int MDS_IO_SOCKET(int fd);
extern int MDS_IO_FD(int fd);
#ifdef HAVE_WINDOWS_H
typedef int mode_t;
typedef int ssize_t;
#endif
extern int MDS_IO_OPEN(char *filename, int options, mode_t mode);
extern int MDS_IO_CLOSE(int fd);
extern _int64 MDS_IO_LSEEK(int fd, _int64 offset, int whence);
extern int MDS_IO_WRITE(int fd, void *buff, size_t count);
extern ssize_t MDS_IO_READ(int fd, void *buff, size_t count);
extern int MDS_IO_LOCK(int fd, _int64 offset, int size, int mode);
extern int MDS_IO_EXISTS(char *filename);
extern int MDS_IO_REMOVE(char *filename);
extern int MDS_IO_RENAME(char *oldname, char *newname);


#endif
