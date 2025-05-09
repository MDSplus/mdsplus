#ifndef _TREESHRP_H
#define _TREESHRP_H

#include <mdsdescrip.h>
#include <mdsplus/mdsconfig.h>
#include <mdstypes.h>
#include <ncidef.h>
#include <pthread_port.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <treeshr_hooks.h>
#include <usagedef.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include "../mdstcpip/mdsIo.h"

/*******************************************
 XNCI reserver names
 *******************************************/
#define XNCI_ATTRIBUTE_NAMES "attributenames"
#define XNCI_SEGMENT_SCALE_NAME "SegmentScale"

/********************************************
  NCI

  Each node has associated with it a fixed
  length characteristics record.  This will
  eventually be the 6th section of the tree file.
  for now they will live in their own file.
 ********************************************/
#define NciM_DATA_IN_ATT_BLOCK 0x01
#define NciV_DATA_IN_ATT_BLOCK 0
#define NciM_ERROR_ON_PUT 0x02
#define NciV_ERROR_ON_PUT 1
#define NciM_DATA_CONTIGUOUS 0x04
#define NciV_DATA_CONTIGUOUS 2
#define NciM_NON_VMS 0x08
#define NciV_NON_VMS 3
#define NciM_EXTENDED_NCI 0x10
#define NciV_EXTENDED_NCI 4
#define NciM_32BIT_UID_NCI 0x20
#define NciV_32BIT_UID_NCI 5

typedef struct nci
{
  unsigned int flags;
  unsigned char flags2;
  unsigned char spare;
  int64_t time_inserted;
  unsigned int owner_identifier;
  class_t class;
  dtype_t dtype;
  l_length_t length;
  unsigned char compression_method;
  unsigned int status;
  union {
    struct
    {
      unsigned char file_level;
      unsigned char file_version;
      unsigned char rfa[6];
      unsigned int record_length;
    } DATA_LOCATION;
    struct
    {
      unsigned char element_length;
      unsigned char data[11];
    } DATA_IN_RECORD;
    struct
    {
      unsigned int error_status;
      unsigned int stv;
    } ERROR_INFO;
  } DATA_INFO;
  unsigned char nci_fill;
} NCI;

#define FACILITIES_PER_EA 8
typedef struct extended_attributes
{
  int64_t next_ea_offset;
  int64_t facility_offset[FACILITIES_PER_EA];
  int facility_length[FACILITIES_PER_EA];
} EXTENDED_ATTRIBUTES;

enum
{
  STANDARD_RECORD_FACILITY = 0,
  SEGMENTED_RECORD_FACILITY = 1,
  NAMED_ATTRIBUTES_FACILITY = 2,
};

#define MAX_DIMS 8
typedef struct segment_header
{
  dtype_t dtype;
  char dimct;
  int dims[MAX_DIMS];
  length_t length;
  int idx;
  int next_row;
  int64_t index_offset;
  int64_t data_offset;
  int64_t dim_offset;
} SEGMENT_HEADER;

typedef struct segment_info
{
  int64_t start;
  int64_t end;
  int64_t start_offset;
  int start_length;
  int64_t end_offset;
  int end_length;
  int64_t dimension_offset;
  int dimension_length;
  int64_t data_offset;
  int rows;
} SEGMENT_INFO;

#define SEGMENTS_PER_INDEX 128
typedef struct segment_index
{
  int64_t previous_offset;
  int first_idx;
  SEGMENT_INFO segment[SEGMENTS_PER_INDEX];
} SEGMENT_INDEX;

#define NAMED_ATTRIBUTES_PER_INDEX 128
#define NAMED_ATTRIBUTE_NAME_SIZE 64
typedef struct named_attribute
{
  char name[NAMED_ATTRIBUTE_NAME_SIZE];
  int64_t offset;
  int length;
} NAMED_ATTRIBUTE;

typedef struct named_attributes_index
{
  int64_t previous_offset;
  NAMED_ATTRIBUTE attribute[NAMED_ATTRIBUTES_PER_INDEX];
} NAMED_ATTRIBUTES_INDEX;

// #define WORDS_BIGENDIAN // use for testing
#ifdef WORDS_BIGENDIAN
#define SWP(out, in, a, b)                \
  ((char *)(out))[a] = ((char *)(in))[b]; \
  ((char *)(out))[b] = ((char *)(in))[a];
#define loadint16(out, in) \
  do                       \
  {                        \
    SWP(out, in, 0, 1);    \
  } while (0)
#define loadint32(out, in) \
  do                       \
  {                        \
    SWP(out, in, 0, 3);    \
    SWP(out, in, 2, 1);    \
  } while (0)
#define loadint64(out, in) \
  do                       \
  {                        \
    SWP(out, in, 0, 7);    \
    SWP(out, in, 2, 5);    \
    SWP(out, in, 4, 3);    \
    SWP(out, in, 6, 1);    \
  } while (0)
#undef SWP
#else
#define loadint16(out, in) memcpy((char *)(out), (char *)(in), 2)
#define loadint32(out, in) memcpy((char *)(out), (char *)(in), 4)
#define loadint64(out, in) memcpy((char *)(out), (char *)(in), 8)
#endif
static inline int16_t swapint16(const void *buf)
{
  int16_t ans;
  loadint16(&ans, buf);
  return ans;
}
static inline int32_t swapint32(const void *buf)
{
  int32_t ans;
  loadint32(&ans, buf);
  return ans;
}
static inline int64_t swapint64(const void *buf)
{
  int64_t ans;
  loadint64(&ans, buf);
  return ans;
}
static inline void getchars(char **buf, void *ans, int n)
{
  memcpy(ans, *buf, n);
  *buf += n;
}
static inline void getint8(char **buf, const void *ans)
{
  *(char *)ans = **buf;
  *buf += sizeof(int8_t);
}
static inline void getint16(char **buf, void *ans)
{
  loadint16(ans, *buf);
  *buf += sizeof(int16_t);
}
static inline void getint32(char **buf, void *ans)
{
  loadint32(ans, *buf);
  *buf += sizeof(int32_t);
}
static inline void getint64(char **buf, void *ans)
{
  loadint64(ans, *buf);
  *buf += sizeof(int64_t);
}
static inline void putchars(char **buf, const void *ans, int n)
{
  memcpy(*buf, ans, n);
  *buf += n;
}
static inline void putint8(char **buf, const void *ans)
{
  **buf = *(char *)ans;
  *buf += sizeof(int8_t);
}
static inline void putint16(char **buf, const void *ans)
{
  loadint16(*buf, ans);
  *buf += sizeof(int16_t);
}
static inline void putint32(char **buf, const void *ans)
{
  loadint32(*buf, ans);
  *buf += sizeof(int32_t);
}
static inline void putint64(char **buf, const void *ans)
{
  loadint64(*buf, ans);
  *buf += sizeof(int64_t);
}

#define bitassign(bool, value, mask) \
  value = (bool) ? (value) | (unsigned)(mask) : (value) & ~(unsigned)(mask)
#define bitassign_c(bool, value, mask)                             \
  value = (unsigned char)(((bool) ? (value) | (unsigned)(mask)     \
                                  : (value) & ~(unsigned)(mask)) & \
                          0xFF)

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
  unsigned int tree : 8;
  unsigned int node : 24;
} NID;
#else
typedef struct nid
{
  unsigned int
      node : 24; /* Node offset of root node of tree this node belongs to */
  unsigned int
      tree : 8; /* Level of tree in chained tree_info blocks 0=main tree */
} NID;
#endif

#define MAX_SUBTREES \
  256 /* since there are only 8 bits of tree number in a nid */
#define DEFAULT_STACK_LIMIT 8

/********************************************
   NODE

2) The second section of a tree file contains
   nodes.
*********************************************/

typedef char NODE_NAME[12];
typedef char TREE_NAME[12];

/*********************************************
 Linkages to other nodes via parent, brother,
 member and child node links are expressed in
 self relative byte offsets. To connect to the
 previous node a link should be set to
 -sizeof(NODE). To connect to the following
 node it should be set to sizeof(NODE) etc.
*********************************************/
#pragma pack(push, 1)
typedef struct node
{
  NODE_NAME name;
  int parent;
  int member;
  int brother;
  int child;
  unsigned char usage;             // packed! is aligned(32bit)
  unsigned short conglomerate_elt; // packed! is not aligned
  char fill;
  int tag_link; /* Index of tag info block pointing to this node (index of first
                   tag is 1) */
} NODE;
#pragma pack(pop)

#ifdef EMPTY_NODE
static NODE empty_node = {
    {'e', 'm', 'p', 't', 'y', ' ', 'n', 'o', 'd', 'e', ' ', ' '},
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0};
#endif

static inline int node_offset(NODE *a, NODE *b)
{
  /* Returns byte offset of two nodes if both pointers are not NULL otherwise 0
   */
  int ans = 0;
  if (a && b)
  {
    ans = (int)((char *)a - (char *)b);
    ans = swapint32(&ans);
  }
  return ans;
}

/********************************************
   TAG_INFO

4) The fourth section of a tree file contains
   tag information blocks. These blocks are
   not sorted but the tag index list (previous
   section) provide a sorted index of the tags.
   The tags are located using a binary search
   of the tag indexes
*********************************************/

typedef char TAG_NAME[24];

typedef struct tag_info
{
  TAG_NAME name;
  int node_idx; /* Node to which this tag is assigned   */
  int tag_link; /* Index of next tag also assigned to this node (index of first
                   tag is 1) */
} TAG_INFO;

/*********************************************
   TREE_HEADER

1) The first section of a tree file is the
   header. The tree header is the first 512
   bytes of a tree file
*********************************************/

typedef struct tree_header
{
  char version; /* Version of tree file format */
  unsigned char sort_children : 1; /* Sort children flag */
  unsigned char sort_members : 1;  /* Sort members  flag */
  unsigned char versions_in_model : 1;
  unsigned char versions_in_pulse : 1;
  unsigned char readonly : 1;
  unsigned char alternate_compression : 1;
  unsigned char : 2;
  char fill1[6];
  int free;      /* First node in free node list (connected by PARENT/CHILD indexes
             */
  int tags;      /* Number of tags defined */
  int externals; /* Number of externals/subtrees referenced */
  int nodes;     /* Number of nodes allocated (both defined and free node)     */
  char fill2[488];
} TREE_HEADER;

/***********************************
Defines RFA type as 6 characters.
Used for moving RFA's around
efficiently.
************************************/
typedef struct
{
  unsigned char rfa[6];
} RFA;

#ifdef RFA_MACROS
#define RfaToSeek(rfa)                  \
  (((*(unsigned int *)rfa - 1) * 512) + \
   (*(unsigned short *)&((char *)rfa)[4] & 0x1ff))
#define SeekToRfa(seek, rfa)                                               \
  {                                                                        \
    *(unsigned int *)rfa = (unsigned int)(seek / 512 + 1);                 \
    *(unsigned short *)&(((char *)rfa)[4]) = (unsigned short)(seek % 512); \
  }
#endif

/****************************************
RECORD_HEADER
VFC portion of file.
***************************************/
#pragma pack(push, 1)
typedef struct record_header
{
  unsigned short rlength; // packed! is aligned
  int node_number;        // packed! is not aligned
  RFA rfa;
} RECORD_HEADER;
#pragma pack(pop)
/*****************************************************
 *     New Search structures
 *****************************************************/
typedef enum yytokentype
{
  TAG_TREE = 258,
  TAG = 259,
  CHILD = 260,
  MEMBER = 261,
  CHILD_OR_MEMBER = 262,
  ANCESTOR = 263,
  ANCESTOR_SEARCH = 264,
  CHILD_SEARCH = 265,
  MEMBER_SEARCH = 266,
  CHILD_OR_MEMBER_SEARCH = 267,
  PARENT = 268,
  EOLL = 269
} YYTOKENTYPE;

typedef struct _st
{
  int search_type;
  char *term;
  struct _st *next;
  NODE *start_node;
} SEARCH_TERM;
typedef struct node_list
{
  NODE *node;
  struct node_list *next;
} NODELIST;

typedef struct
{
  SEARCH_TERM *terms;
  char *wildcard;
  NODE *default_node;
  NODELIST *answers;
} SEARCH_CTX;

/********************************************
   TREE_EDIT

The tree edit structure is used during tree
editting. It keeps track of dynamic memory
allocations for tree expansions
*********************************************/

typedef struct deleted_nid
{
  NID nid;
  struct deleted_nid *next;
} DELETED_NID;

typedef struct tree_edit
{
  int header_pages;
  int node_vm_size;
  int tags_pages;
  int tag_info_pages;
  int external_pages;
  int nci_section[2];
  int nci_channel;
  NCI *nci;
  int first_in_mem;
  short conglomerate_size;
  short conglomerate_index;
  unsigned conglomerate_setup : 1;
  DELETED_NID *deleted_nid_list;
} TREE_EDIT;
#ifdef EMPTY_EDIT
static const TREE_EDIT empty_edit = {0};
#endif

/********************************************
TREE_IO.H
Contains definitions of all structures used
in doing I/O to the tree files.
********************************************/

#define DATAF_C_MAX_RECORD_SIZE (32765u - sizeof(RECORD_HEADER))

typedef enum
{
  TREE_DIRECTORY = 0,     // just directory
  TREE_TREEFILE_TYPE = 1, //".tree"
  TREE_NCIFILE_TYPE = 2,  //".characteristics"
  TREE_DATAFILE_TYPE = 3, //".datafile"
} tree_type_t;
#define TREE_TREEFILE_EXT ".tree"
#define TREE_NCIFILE_EXT ".characteristics"
#define TREE_DATAFILE_EXT ".datafile"

/********************************************
The NCI_FILE structure contains the stuff
needed to do I/O on the node characteristics
file.
*********************************************/

typedef struct nci_file
{
  int get;
  int put;
  NCI nci;
} NCI_FILE;

/**************************************
ASY_NCI
Structure used for passing information
to asynchronous I/O completion routines
***************************************/

typedef struct asy_nci
{
  struct tree_info *info;
  NCI *nci;
} ASY_NCI;

/*******************************************
DATA_FILE structure used for I/O to datafile
********************************************/

typedef struct data_file
{
  unsigned open_for_write : 1;
  unsigned : 7;
  int get;
  int put;
  RECORD_HEADER *record_header;
  ASY_NCI *asy_nci;
  struct descriptor_xd *data;
} DATA_FILE;

/********************************************
    TREE_INFO

The tree info structure contains information
of trees which have been activated. There will
be one tree info block per tree activated.
*********************************************/

typedef struct tree_info
{
  struct tree_info *next_info;       /* Pointer to next tree info block */
  int blockid;                       /* Identifier indicating a valid tree info block    */
  char *treenam;                     /* Tree name                                        */
  int shot;                          /* Shot number                                      */
  int vm_pages;                      /* Number of pages and address of virtual memory    */
  void *vm_addr;                     /*  allocated for mapping the tree file             */
  char *section_addr[2];             /* Beginning and ending addresses of mapped section */
  TREE_HEADER *header;               /* Pointer to file header                           */
  NODE *node;                        /* Pointer to nodes                                 */
  int *tags;                         /* Pointer to tag indexes                           */
  TAG_INFO *tag_info;                /* Pointer to tag information blocks                */
  int *external;                     /* Pointer to external reference node indexes       */
  int channel;                       /* I/O channel to tree file                         */
  int alq;                           /* Blocks allocated to tree file                    */
  int speclen;                       /* length of filespec part defined by local treepath*/
  char *filespec;                    /* Pointer to full file spec of tree file           */
  char dvi[16];                      /* Tree file disk info                              */
  unsigned short tree_info_w_fid[3]; /* Tree file file id */
  unsigned flush : 1;                /* Flush I/O's buffers                              */
  unsigned rundown : 1;              /* Doing rundown                                    */
  unsigned mapped : 1;               /* Tree is mapped into memory                       */
  unsigned has_lock : 1;             /* is privte context                                */
  int rundown_id;                    /* Rundown event id                                 */
  NODE *root;                        /* Pointer to top node                              */
  TREE_EDIT *edit;                   /* Pointer to edit block (if editting the tree      */
  NCI_FILE *nci_file;                /* Pointer to nci file block (if open)              */
  DATA_FILE *data_file;              /* Pointer to a datafile access block               */
  pthread_rwlock_t lock;
} TREE_INFO;

#define RDLOCKINFO(info) \
  if (info->has_lock)    \
  pthread_rwlock_rdlock(&info->lock)
#define WRLOCKINFO(info) \
  if (info->has_lock)    \
  pthread_rwlock_wrlock(&info->lock)
#define UNLOCKINFO(info) \
  if (info->has_lock)    \
  pthread_rwlock_unlock(&info->lock)

/********************************************
   PINO_DATABASE

The PINO_DATABASE structure is used to maintain
a tree context. The pino database structure is
always passed as an argument to tree traversal
routines.
*********************************************/

typedef struct
{
  struct descriptor_xd start;
  struct descriptor_xd end;
  struct descriptor_xd delta;
} timecontext_t;

typedef struct pino_database
{
  struct pino_database *next; /* Link to next database in open list */
  TREE_INFO *tree_info;       /* Pointer to main tree info block */
  NODE *default_node;         /* Pointer to current default node */
  char *experiment;           /* Pointer to experiment descriptor */
  char *main_treenam;         /* Pointer to name used for main tree */
  int shotid;                 /* Shot identification */
  unsigned open : 1;          /* Flag indication block has open tree */
  unsigned open_for_edit : 1; /* Flag indicating tree is open for edit */
  unsigned open_readonly : 1; /* Flag indicating tree open readonly */
  unsigned modified : 1;      /* Flag indicating tree structure modified */
  unsigned setup_info : 1;    /* Flag indicating setup info is being added */
  unsigned remote : 1;        /* Flag indicating tree is on remote system */
  unsigned alternate_compression : 1;
  unsigned : 25;
  int stack_size;
  unsigned fill;
  timecontext_t timecontext;
  int delete_list_vm;
  unsigned char *delete_list;
  void *dispatch_table; /* pointer to dispatch table generated by dispatch/build */
} PINO_DATABASE;

static inline NODE *nid_to_node(PINO_DATABASE *dbid, NID *nid)
{
  TREE_INFO *info;
  unsigned int i;
  for (info = dbid->tree_info, i = 0; (i < nid->tree) && (info != NULL);
       info = info->next_info, i++)
    ;
  return info && (info->header->nodes >= (int)nid->node)
             ? info->node + (int)nid->node
             : (NODE *)0;
}

static inline NODE *parent_of(PINO_DATABASE *dbid, NODE *a)
{
  NODE *ans = 0;
  if (a)
  {
    if (a->usage == TreeUSAGE_SUBTREE_TOP)
    {
      ans = nid_to_node(dbid, (NID *)&a->parent);
    }
    else if (a->parent)
    {
      ans = (NODE *)((char *)a + swapint32(&a->parent));
    }
  }
  return ans;
}

static inline NODE *member_of(NODE *a)
{
  NODE *ans = 0;
  if (a && a->member)
  {
    ans = (NODE *)((char *)a + swapint32(&a->member));
  }
  return ans;
}
/*
 * Note in certain edit operations dbid is passed as 0,
 * which would cause nid_to_node(dbid)  to fail
 *
 * however, when editing, the usage of the nde will never be
 * TreeUSAGE_SUBTREE_REF, so it will never call nid_to_node
 */
static inline NODE *child_of(PINO_DATABASE *dbid, NODE *a)
{
  NODE *ans = 0;
  if (a && a->child)
  {
    if (a->usage == TreeUSAGE_SUBTREE_REF)
    {
      ans = nid_to_node(dbid, (NID *)&a->child);
    }
    else
    {
      ans = (NODE *)((char *)a + swapint32(&a->child));
    }
  }
  if (ans && ans->usage == TreeUSAGE_SUBTREE_REF)
    ans = nid_to_node(dbid, (NID *)&ans->child);
  return ans;
}

static inline NODE *brother_of(PINO_DATABASE *dbid, NODE *a)
{
  NODE *ans = 0;
  if (a && a->brother)
  {
    if (a->usage == TreeUSAGE_SUBTREE_TOP)
    {
      ans = nid_to_node(dbid, (NID *)&a->brother);
    }
    else
    {
      ans = (NODE *)((char *)a + swapint32(&a->brother));
      if (ans->usage == TreeUSAGE_SUBTREE_REF)
        ans = nid_to_node(dbid, (NID *)&ans->child);
    }
  }
  if (ans && ans->usage == TreeUSAGE_SUBTREE_REF)
    ans = nid_to_node(dbid, (NID *)&ans->child);
  return ans;
}

static inline NODE *descendant_of(PINO_DATABASE *dbid, NODE *a)
{
  NODE *answer = member_of(a);
  return (answer) ? answer : child_of(dbid, a);
}

static inline int is_member(PINO_DATABASE *dblist, NODE *node)
{
  NODE *n = 0;
  if (parent_of(dblist, node))
    for (n = member_of(parent_of(dblist, node)); n && n != node;
         n = brother_of(dblist, n))
      ;
  return n == node;
}

static inline NODE *sibling_of(PINO_DATABASE *dbid, NODE *a)
{
  NODE *answer = brother_of(dbid, a);
  if (!answer)
    if (is_member(dbid, a))
      answer = child_of(dbid, parent_of(dbid, a));
  return answer;
}

/******************************************
Two macros are provided for converting from
a NID to a node address or from a node
address to a NID:

   nid_to_node(pino, nid, node)
   node_to_nid(pino, node, nid)

******************************************/

/*
#define nid_to_node(pino,nid,saved_node) \
     {\
     TREE_INFO *nid_to_node__info;\
     unsigned int nid_to_node__i;\
     for (nid_to_node__info = pino->tree_info,nid_to_node__i = 0; \
          nid_to_node__i < nid->tree; nid_to_node__i++)\
             nid_to_node__info = nid_to_node__info ?
nid_to_node__info->next_info : 0;\
     saved_node = nid_to_node__info && (nid_to_node__info->header->nodes >=
(int)nid->node)\
                      ? nid_to_node__info->node + (int)nid->node : 0;\
     }
*/

static inline int node_to_nid(PINO_DATABASE *dbid, NODE *node, NID *nid_out)
{
  TREE_INFO *info;
  NID nid = {0, 0};
  for (info = dbid->tree_info, nid.tree = 0; info != NULL;
       info = info->next_info, nid.tree++)
  {
    if ((node >= info->node) && (node <= (info->node + info->header->nodes)))
      break;
  }
  if (info)
    nid.node = (unsigned int)((node - info->node) & 0xFFFFFF);
  else
    nid.tree = 0;
  if (nid_out)
    *nid_out = nid;
  return *(int *)&nid;
}

/******************************************
Another useful macro based on nid:

nid_to_tree_nidx(pino, nid, info, nidx)
*******************************************/

#define nid_to_tree_nidx(pino, nid, info, nidx) \
  nidx = nid_to_tree_idx(pino, nid, &info)
static inline int nid_to_tree_idx(PINO_DATABASE *pino, NID *nid,
                                  TREE_INFO **info_out)
{
  unsigned int i;
  TREE_INFO *info = pino->tree_info;
  for (i = 0; info && i < nid->tree; i++)
    info = info->next_info;
  *info_out = info ? (info->header->nodes >= (int)nid->node ? info : 0) : NULL;
  return *info_out ? nid->node : 0;
}
/******************************************
Another useful macro based on nid:

nid_to_tree(pino, nid, info)
*******************************************/

#define nid_to_tree(pino, nid, info) nid_to_tree_idx(pino, nid, &info)

/****************************
Macro's for checking access
to databases
*****************************/

#define IS_OPEN(db) ((db) ? (db)->open : 0)
#define IS_OPEN_FOR_EDIT(db) (IS_OPEN(db) ? (db)->open_for_edit : 0)

#define TREE_PATH_SUFFIX "_path"
#define TREE_DEFAULT_PATH "default_tree_path"
#define TREE_PATH_DELIM "/"
#define TREE_PATH_LIST_DELIM ";"

/************* Prototypes for internal functions *************/
extern int ConnectTreeRemote(PINO_DATABASE *dblist, char const *tree,
                             char *subtree_list, char *);
extern int SetStackSizeRemote(PINO_DATABASE *dblist, int stack_size);

extern int CloseTreeRemote(PINO_DATABASE *dblist, int call_hook);

extern int CreatePulseFileRemote(PINO_DATABASE *dblist, int shotid, int num,
                                 int *nids);

extern int GetNciRemote(PINO_DATABASE *dblist, int nid_in,
                        struct nci_itm *nci_itm);

extern int GetRecordRemote(PINO_DATABASE *dblist, int nid_in,
                           struct descriptor_xd *dsc);

extern char *GetRegistryPath(char *pathname);

extern int FindNodeRemote(PINO_DATABASE *dblist, char const *path, int *outnid);

extern int FindNodeWildRemote(PINO_DATABASE *dblist, char const *path,
                              int *nid_out, void **ctx_inout, int usage_mask);

extern int FindNodeEndRemote(PINO_DATABASE *dblist, void **ctx_in);

extern char *FindNodeTagsRemote(PINO_DATABASE *dblist, int nid_in,
                                void **ctx_ptr);

extern char *AbsPathRemote(PINO_DATABASE *dblist, char const *inpath);

extern int SetDefaultNidRemote(PINO_DATABASE *dblist, int nid);

extern int GetDefaultNidRemote(PINO_DATABASE *dblist, int *nid);
#ifdef _WIN32
#include <windows.h>
#endif
extern int64_t RfaToSeek(uint8_t *rfa);
void SeekToRfa(int64_t seek, uint8_t *rfa);
extern void TreeCallHookFun(char *hookType, char *hookName, ...);
extern int TreeMakeNidsLocal(struct descriptor *dsc_ptr, int nid);
extern int TreeCloseFiles(TREE_INFO *info, int nci, int data);
extern int TreeCopyExtended(PINO_DATABASE *dbid1, PINO_DATABASE *dbid2, int nid,
                            NCI *nci, int compress);
extern int TreeExpandNodes(PINO_DATABASE *db_ptr, int num_fixup,
                           NODE ***fixup_nodes);
extern int TreeFindParent(PINO_DATABASE *dblist, char *path_ptr,
                          NODE **node_ptrptr, char **namedsc_ptr, int *chid);
extern int TreeInsertChild(NODE *parent_ptr, NODE *child_ptr, int sort);
extern int TreeInsertMember(NODE *parent_ptr, NODE *member_ptr, int sort);
extern int TreeIsChild(PINO_DATABASE *db, NODE *node);
extern struct descriptor *TreeSectionName(TREE_INFO *info);
/* extern int TreeFindTag(PINO_DATABASE *db, NODE *node, const char *treename,
 * char **search_string, NODE **node_in_out); */
extern int TreeFindTag(const char *tagnam, const char *treename, int *tagidx);
int _TreeFindTag(PINO_DATABASE *db, NODE *default_node, short treelen,
                 char const *tree, short taglen, const char *tagnam,
                 NODE **nodeptr, int *tagidx);
extern int TreeCallHook(TreeshrHookType operation, TREE_INFO *info, int nid);
extern void _TreeDeleteNodesWrite(void *dbid);
extern void _TreeDeleteNodesDiscard(void *dbid);
extern int TreeGetDatafile(TREE_INFO *info_ptr, uint8_t *rfa, int *buffer_size,
                           char *record, int *retsize, int *nodenum,
                           unsigned char flags);
extern int TreeEstablishRundownEvent(TREE_INFO *info);
extern int TreeGetExtendedAttributes(TREE_INFO *info_ptr, int64_t offset,
                                     EXTENDED_ATTRIBUTES *att);
extern int _TreeGetSegmentedRecord(void *dbid, int nid,
                                   struct descriptor_xd *data);
extern int TreeGetVersionNci(TREE_INFO *info, NCI *nci, NCI *v_nci);
extern DATA_FILE *TreeGetVmDatafile();
extern int TreeReopenDatafile(struct tree_info *info);
extern int TreeReopenNci(struct tree_info *info);
extern int _TreeOpenDatafileR(TREE_INFO *info);
extern int _TreeOpenNciR(TREE_INFO *info);
extern int _TreeOpenNciW(TREE_INFO *info, int tmpfile);
extern int _TreeOpenDatafileW(TREE_INFO *info, int *stv_ptr, int tmpfile);
extern int TreeOpenDatafileR(TREE_INFO *info);
extern int TreeOpenNciR(TREE_INFO *info);
extern int TreeOpenNciW(TREE_INFO *info, int tmpfile);
extern int TreeOpenDatafileW(TREE_INFO *info, int *stv_ptr, int tmpfile);
extern int TreePutExtendedAttributes(TREE_INFO *info_ptr,
                                     EXTENDED_ATTRIBUTES *att, int64_t *offset);
extern void TreeSerializeNciIn(const char *in, NCI *out);
extern void TreeSerializeNciOut(const NCI *in, char *out);
extern int64_t TreeTimeInserted();
extern int TreeSetTemplateNci(NCI *nci);

extern int tree_lock_nci(TREE_INFO *info, int readonly, int nodenum,
                         int *deleted, int *locked);
extern void tree_unlock_nci(TREE_INFO *info, int readonly, int nodenum,
                            int *locked);
extern int tree_get_and_lock_nci(TREE_INFO *info, int nodenum, NCI *nci,
                                 int *locked);
extern int tree_put_nci(TREE_INFO *info, int nodenum, NCI *nci, int *locked);
extern int tree_get_nci(TREE_INFO *info, int nodenum, NCI *nci,
                        unsigned int version, int *locked);

extern int TreeLockDatafile(TREE_INFO *info, int readonly, int64_t where);
extern int TreeUnLockDatafile(TREE_INFO *info, int readonly, int64_t where);

extern int tree_get_dsc(TREE_INFO *info, const int nid, const int64_t offset,
                        const int length, struct descriptor_xd *dsc);
extern int tree_put_dsc(PINO_DATABASE *dbid, TREE_INFO *info, int nid,
                        struct descriptor *dsc, int64_t *offset, int *length,
                        int compress);

extern int MDS_IO_ID(int fd);
extern int MDS_IO_FD(int fd);
#ifdef _WIN32
#ifndef HAVE_PTHREAD_H
#define ssize_t int64_t
typedef int mode_t;
#endif
#endif
extern int MDS_IO_OPEN(char *filename, int options, mode_t mode);
extern int MDS_IO_CLOSE(int fd);
extern off_t MDS_IO_LSEEK(int fd, off_t offset, int whence);
extern ssize_t MDS_IO_WRITE(int fd, void *buff, size_t count);
extern ssize_t MDS_IO_READ(int fd, void *buff, size_t count);
extern int MDS_IO_LOCK(int fd, off_t offset, size_t size, int mode,
                       int *deleted);
extern int MDS_IO_EXISTS(char *filename);
extern int MDS_IO_REMOVE(char *filename);
extern int MDS_IO_RENAME(char *oldname, char *newname);
extern ssize_t MDS_IO_READ_X(int fd, off_t offset, void *buff, size_t count,
                             int *deleted);
extern int MDS_IO_OPEN_ONE(char *filepath_in, char const *treename, int shot,
                           tree_type_t type, int new, int edit_flag,
                           char **fullpath, int *speclen, int *fd_out);

#endif
