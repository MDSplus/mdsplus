/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSOKBUTTONS.H */
/*  *3     3-MAR-1993 14:00:12 JAS "Add prottypes" */
/*  *2    24-FEB-1993 15:16:40 JAS "port to decc" */
/*  *1    24-FEB-1993 15:12:25 JAS "include file of okbuttons" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSOKBUTTONS.H */
/*------------------------------------------------------------------------------

                Name:   XmdsOkButtons.H

                Type:   C include

                Author:	Josh Stillerman

                Date:   24-FEB-1993

                Purpose: Public declarations for  okbutton widget

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
----------------------------------------------------------------------------*/

#ifndef _XmdsOkButtons_H
#define _XmdsOkButtons_H

#ifdef _NO_PROTO
Widget XmdsCreateApplyButton();
Widget XmdsCreateCancelButton();
Widget XmdsCreateOkButton();
Widget XmdsCreateResetButton();
#else
Widget XmdsCreateApplyButton(Widget parent, String name, ArgList args,
                             Cardinal numargs);
Widget XmdsCreateCancelButton(Widget parent, String name, ArgList args,
                              Cardinal numargs);
Widget XmdsCreateOkButton(Widget parent, String name, ArgList args,
                          Cardinal numargs);
Widget XmdsCreateResetButton(Widget parent, String name, ArgList args,
                             Cardinal numargs);
#endif
#endif
