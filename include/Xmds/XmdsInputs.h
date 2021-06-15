/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSINPUTS.H */
/*  *4     3-MAR-1993 13:55:30 JAS "Add prottypes" */
/*  *3    24-FEB-1993 16:07:35 JAS "port to decc" */
/*  *2     5-DEC-1991 17:20:29 TWF "Add widgettonumber" */
/*  *1     5-DEC-1991 17:13:54 TWF "digitizer channel support" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSINPUTS.H */
/*------------------------------------------------------------------------------

                Name:   XmdsInputs

                Type:   C include file

                Author:	JOSH STILLERMAN

                Date:    2-APR-1990

                Purpose: Defines the data structures and routines for
                         generic Input or Channel support.

--------------------------------------------------------------------------------

   Copyright (c) 1990
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.

--------------------------------------------------------------------------------
  Type declarations :                                                         */

#ifndef _XmdsInputs_h
#define _XmdsInputs_h
typedef struct _xmdsInputCtx
{
  int inputs_nid;
  int nodes_per_input;
  int data_offset;
  int start_offset;
  int end_offset;
  int time_idx_offset;
} * XmdsInputCtx;

/*------------------------------------------------------------------------------
  External Entry points                                                       */

#ifdef _NO_PROTO
extern void XmdsInputCreateCallback();
extern void XmdsResetInput();
extern void XmdsPutInputSetup();
extern int XmdsWidgetToNumber();
#else

extern void XmdsInputCreateCallback(Widget w, XmdsInputCtx ctx);
extern void XmdsResetInput(Widget chans_dlog, XmdsInputCtx ctx, int chan);
extern void XmdsPutInputSetup(Widget inputs_dlog, XmdsInputCtx ctx, int chan);
extern int XmdsWidgetToNumber(Widget w, String prefix);

void XmdsInputCreateCallback(Widget w, XmdsInputCtx ctx);
void XmdsResetInput(Widget chans_dlog, XmdsInputCtx ctx, int chan);
void XmdsPutInputSetup(Widget inputs_dlog, XmdsInputCtx ctx, int chan);
int XmdsWidgetToNumber(Widget w, String prefix);
#endif
#endif /* _XmdsInputs_h */
