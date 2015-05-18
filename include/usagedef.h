/*  USAGEDEF.H ************************************

Node Usage definitions

**************************************************/

#ifndef USAGEDEF_H

#define TreeUSAGE_ANY       0
#define TreeUSAGE_NONE      1 /******* USAGE_NONE is now USAGE_STRUCTURE *******/
#define TreeUSAGE_STRUCTURE 1
#define TreeUSAGE_ACTION    2
#define TreeUSAGE_DEVICE    3
#define TreeUSAGE_DISPATCH  4
#define TreeUSAGE_NUMERIC   5
#define TreeUSAGE_SIGNAL    6
#define TreeUSAGE_TASK      7
#define TreeUSAGE_TEXT      8
#define TreeUSAGE_WINDOW    9
#define TreeUSAGE_AXIS     10
#define TreeUSAGE_SUBTREE  11
#define TreeUSAGE_COMPOUND_DATA 12
#define TreeUSAGE_MAXIMUM  TreeUSAGE_COMPOUND_DATA
#define TreeUSAGE_SUBTREE_REF 14 /* Runtime only special usage */
#define TreeUSAGE_SUBTREE_TOP 15 /* Runtime only special usage */

#endif
