/*  VAX/DEC CMS REPLACEMENT HISTORY, Element USAGEDEF.H */
/*  *4    27-DEC-1990 10:42:05 TWF "Add COMPOUND_DATA" */
/*  *3    10-AUG-1990 10:08:42 JAS "adding usage subtree" */
/*  *1     2-JAN-1990 12:34:10 TWF "Usage value definitions" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element USAGEDEF.H */
/*  USAGEDEF.H ************************************

Node Usage definitions

**************************************************/

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
