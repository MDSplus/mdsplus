#include       "tcldef.h"
#ifdef vms
#include        <lib$routines.h>
#endif
#include        <mdsdescrip.h>
#include        "nodetouch.h"
#include        "mdsshr.h"
#include        "treeshr.h"

	/*****************************************************************
	 * Structures:
	 *****************************************************************/


	/*****************************************************************
	 * Function prototypes:  (see also tcldef.h)
	 *****************************************************************/
int   TclMsg();			/* args: < sts , fmt , [arg] >		*/
void  TclSetCallbacks(		/* Returns: void  			*/
    void  (*error_out)()	/* <r> addr of error output routine	*/
   ,void  (*text_out)()		/* <r> addr of normal text output routine */
   ,void  (*node_touched)()	/* <r> addro of "node touched" routine	*/
   );
void  TclErrorOut(		/* Returns: void  			*/
    int   status		/* <r> status value to display		*/
   );
void  TclTextOut(		/* Returns: void  			*/
    char  *text			/* <r> text for display			*/
   );
void  TclNodeTouched(		/* Returns: void  			*/
    int nid			/* <r> node id				*/
   ,NodeTouchType type		/* <r> type of "touch"			*/
   );
