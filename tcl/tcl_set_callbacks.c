#include        "tclsysdef.h"

/*--------------------------------------------------------------------------

		Name:   TclSetCallbacks

		Type:   C function

		Author:	JOSH STILLERMAN

		Date:   10-FEB-1993

		Purpose: implement callbacks to applications that need to
			 know about node operations.

--------------------------------------------------------------------------

	Call sequence:


--------------------------------------------------------------------------
   Copyright (c) 1993
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:


-------------------------------------------------------------------------*/

static void (*ErrorOut) ();
static void (*TextOut) ();
static void (*NodeTouched) ();

	/***************************************************************
	 * TclSetCallbacks:
	 ***************************************************************/
void TclSetCallbacks(		/* Returns: void			*/
    void (*error_out) ()	/* <r> addr of error output routine	*/
   ,void (*text_out) ()		/* <r> addr of normal text output routine */
   ,void (*node_touched) ()	/* <r> addro of "node touched" routine	*/
   )
   {
    ErrorOut = error_out;
    TextOut = text_out;
    NodeTouched = node_touched;
   }

	/*****************************************************************
	 * TclErrorOut:
	 *****************************************************************/
void TclErrorOut(		/* Returns: void			*/
    int   status		/* <r> status value to display		*/
   )
   {
    if (ErrorOut)
        (*ErrorOut) (status);
   }



	/****************************************************************
	 * TclTextOut:
	 ****************************************************************/
void TclTextOut(		/* Returns: void			*/
    char  *text			/* <r> text for display			*/
   )
   {
    char  *p;
    struct descriptor  *dsc;

    if (TextOut)
        (*TextOut) (text);
    else
        printf("%s\n",text?text:"");
   }



	/*****************************************************************
	 * TclNodeTouched:
	 *****************************************************************/
void TclNodeTouched(		/* Returns: void			*/
    int nid			/* <r> node id				*/
   ,NodeTouchType type		/* <r> type of "touch"			*/
   )
   {
    if (NodeTouched)
      (*NodeTouched) (nid,type);
   }
