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
static char * saved_output = 0;

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

    if (TextOut)
        (*TextOut) (text);
    else
      {
        printf("%s\n",text?text:"");
        fflush(stdout);
      }
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

static void AppendOut(char *text)
{
	char *msg = text ? text : "";
	int len = strlen(msg);
	char *old_saved_output = saved_output;
	if (saved_output)
	{
		saved_output = strcpy((char *)malloc(strlen(old_saved_output)+len+2),old_saved_output);
		free(old_saved_output);
	}
	else
	{
		saved_output = (char *)malloc(len + 2);
		saved_output[0] = '\0';
	}
	strcat(saved_output,msg);
	strcat(saved_output,"\n");
}

void TclSaveOut()
{
	if (saved_output)
	{
		free(saved_output);
		saved_output = 0;
	}
	TclSetCallbacks(AppendOut,AppendOut,NodeTouched);
}

int TclOutLen()
{
	return saved_output ? strlen(saved_output) : 0;
}

int TclGetOut(int free_out, int len_out, char *out)
{
    int len = 0;
	if (saved_output)
	{
	  len = strlen(saved_output);
      strncpy(out,saved_output,len_out);
	  if (free_out)
	  {
		  free(saved_output);
		  saved_output = 0;
	  }
	}
	return len;
}

