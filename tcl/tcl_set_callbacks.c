#include <sys/time.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <mdsshr.h>
#include <STATICdef.h>

#include "tcl_p.h"


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
STATIC_ROUTINE void (*NodeTouched) ();
STATIC_THREADSAFE char *saved_output = 0;
STATIC_THREADSAFE pthread_mutex_t saved_output_mutex;
STATIC_THREADSAFE int initialized = 0;

	/***************************************************************
	 * TclSetCallbacks:
	 ***************************************************************/
void TclSetCallbacks(		/* Returns: void                        */
		      void (*error_out) ()	/* <r> addr of error output routine */
		      , void (*text_out) ()	/* <r> addr of normal text output routine */
		      , void (*node_touched) ()	/* <r> addro of "node touched" routine      */
    )
{
  if (!initialized) {
    pthread_mutex_init(&saved_output_mutex, 0);
    initialized = 1;
  }
  ErrorOut = error_out;
  TextOut = text_out;
  NodeTouched = node_touched;
}


	/*****************************************************************
	 * TclNodeTouched:
	 *****************************************************************/
void TclNodeTouched(		/* Returns: void                        */
		     int nid	/* <r> node id                          */
		     , NodeTouchType type	/* <r> type of "touch"                  */
    )
{
  if (NodeTouched)
    (*NodeTouched) (nid, type);
}

STATIC_ROUTINE void AppendOut(char *text)
{
  char *msg = text ? text : "";
  size_t len = strlen(msg);
  char *old_saved_output;
  pthread_mutex_lock(&saved_output_mutex);
  old_saved_output = saved_output;
  if (saved_output) {
    saved_output = strcpy((char *)malloc(strlen(old_saved_output) + len + 2), old_saved_output);
    free(old_saved_output);
  } else {
    saved_output = (char *)malloc(len + 2);
    saved_output[0] = '\0';
  }
  strcat(saved_output, msg);
  strcat(saved_output, "\n");
  pthread_mutex_unlock(&saved_output_mutex);
}

STATIC_ROUTINE void StatusOut(int status)
{
  AppendOut(MdsGetMsg(status));
}

void TclSaveOut()
{
  if (!initialized) {
    pthread_mutex_init(&saved_output_mutex, 0);
    initialized = 1;
  }
  pthread_mutex_lock(&saved_output_mutex);
  if (saved_output) {
    free(saved_output);
    saved_output = 0;
  }
  TclSetCallbacks(StatusOut, AppendOut, NodeTouched);
  pthread_mutex_unlock(&saved_output_mutex);
}

int TclOutLen()
{
  int ans;
  pthread_mutex_lock(&saved_output_mutex);
  ans = saved_output ? strlen(saved_output) : 0;
  pthread_mutex_unlock(&saved_output_mutex);
  return ans;
}

int TclGetOut(int free_out, int len_out, char *out)
{
  int len = 0;
  pthread_mutex_lock(&saved_output_mutex);
  if (saved_output) {
    len = strlen(saved_output);
    strncpy(out, saved_output, len_out);
    if (free_out) {
      free(saved_output);
      saved_output = 0;
    }
  }
  pthread_mutex_unlock(&saved_output_mutex);
  return len;
}
