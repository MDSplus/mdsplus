#ifndef _USAGEDEF_H
#define _USAGEDEF_H
/*  USAGEDEF.H ************************************

Node Usage definitions

**************************************************/
#include <inttypes.h>
#ifdef MDSOBJECTSCPPSHRVS_EXPORTS
// visual studio uses int types for typedef
#define TYPEDEF(bytes) enum
#define ENDDEF(type, name) \
  ;                        \
  typedef type name
#else
#define TYPEDEF(bytes) typedef enum __attribute__((__packed__))
#define ENDDEF(type, name) name
#endif
TYPEDEF(1){TreeUSAGE_ANY = 0, TreeUSAGE_STRUCTURE = 1, TreeUSAGE_ACTION = 2,
           TreeUSAGE_DEVICE = 3, TreeUSAGE_DISPATCH = 4, TreeUSAGE_NUMERIC = 5,
           TreeUSAGE_SIGNAL = 6, TreeUSAGE_TASK = 7, TreeUSAGE_TEXT = 8,
           TreeUSAGE_WINDOW = 9, TreeUSAGE_AXIS = 10, TreeUSAGE_SUBTREE = 11,
           TreeUSAGE_COMPOUND_DATA = 12,
           /* upper limit for valid usage_t */
           TreeUSAGE_MAXIMUM,
           /* Runtime only special usage */
           TreeUSAGE_SUBTREE_REF,
           TreeUSAGE_SUBTREE_TOP} ENDDEF(uint8_t, usage_t);
#undef TYPEDEF
#undef ENDDEF
#define TreeUSAGE_NONE TreeUSAGE_STRUCTURE
#endif
