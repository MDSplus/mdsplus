#ifndef _DBIDEF_H
#define _DBIDEF_H
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element DBIDEF.H */
/*  *3     1-APR-1991 17:29:45 TWF "Add readonly open" */
/*  *2    16-FEB-1990 10:28:18 TWF "Add itmlst struct" */
/*  *1    28-FEB-1989 14:55:59 TWF "Item codes for TreeGetDbi" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element DBIDEF.H */
/**********************************
 Item list codes for TreeGetDbi
**********************************/

#define DbiEND_OF_LIST 0 /* End of list */
#define DbiNAME 1        /* Experiment name used for open - text string */
#define DbiSHOTID 2      /* Shot identification - longword */
#define DbiMODIFIED                                                                \
  3                        /* True if tree has been modified during edit - boolean \
                            */
#define DbiOPEN_FOR_EDIT 4 /* True if tree is open for edit - boolean */
#define DbiINDEX \
  5                             /* Index of tree to use for subsequent information requests */
#define DbiNUMBER_OPENED 6      /* Number of trees currently open */
#define DbiMAX_OPEN 7           /* Maximum number of tree allowed open at one time */
#define DbiDEFAULT 8            /* NID of default node */
#define DbiOPEN_READONLY 9      /* True if tree has been opened readonly */
#define DbiVERSIONS_IN_MODEL 10 /* True if using versioning in model */
#define DbiVERSIONS_IN_PULSE 11 /* True if using versioning in pulse files */
#define DbiREADONLY 12          /* True if making tree readonly */
#define DbiDISPATCH_TABLE 13    /* Tree dispatch table */

typedef struct dbi_itm
{
  short int buffer_length;
  short int code;
  void *pointer;
  int *return_length_address;
} DBI_ITM;
#endif