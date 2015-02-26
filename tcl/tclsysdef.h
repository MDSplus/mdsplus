#include        <config.h>
#include        <mdsdescrip.h>
#include        "nodetouch.h"
#include        "mdsshr.h"
#include        "mds_stdarg.h"
#include        "treeshr.h"
#include        <malloc.h>
#ifdef HAVE_ALLOCA_H
#include        <alloca.h>
#endif
#include        <tcl_messages.h>

#define TCL_STS_NORMAL TclNORMAL
#define TCL_STS_FAILED_ESSENTIAL TclFAILED_ESSENTIAL

	/*****************************************************************
	 * Structures:
	 *****************************************************************/

	/*****************************************************************
	 * Function prototypes:  (see also tcldef.h)
	 *****************************************************************/
//int TclMsg();                 /* args: < sts , fmt , [arg] >          */
void TclSetCallbacks(		/* Returns: void                        */
		      void (*error_out) ()	/* <r> addr of error output routine */
		      , void (*text_out) ()	/* <r> addr of normal text output routine */
		      , void (*node_touched) ()	/* <r> addro of "node touched" routine      */
    );
//void TclErrorOut(             /* Returns: void                        */
//                int status    /* <r> status value to display          */
//    );
//void TclTextOut(              /* Returns: void                        */
//               char *text     /* <r> text for display                 */
//    );
void TclNodeTouched(		/* Returns: void                        */
		     int nid	/* <r> node id                          */
		     , NodeTouchType type	/* <r> type of "touch"                  */
    );

extern void tclUsageToString(char *usage, char **error);
extern int tclStringToShot(char *str, int *shot, char **error);
extern void tclAppend(char **output, char *string);
