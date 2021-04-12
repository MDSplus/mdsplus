/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSXDBOXDIALOG.H */
/*  *3    24-FEB-1993 15:20:24 JAS "port to decc" */
/*  *2    24-FEB-1993 15:18:41 JAS "port to decc" */
/*  *1    24-FEB-1993 15:01:10 JAS "include file for xmdsxdboxdialog widgets" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSXDBOXDIALOG.H */
/*------------------------------------------------------------------------------

                Name:   XmdsXdBoxDialog.H

                Type:   C include

                Author:	Josh Stillerman

                Date:   24-FEB-1993

                Purpose: Public declarations for  XdBox Widgets.


------------------------------------------------------------------------------
   Copyright (c) 1993
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
----------------------------------------------------------------------------*/

#ifndef _XmdsXdBoxDialog_H
#define _XmdsXdBoxDialog_H

#ifdef _NO_PROTO
Widget XmdsCreateXdBoxDialog();
#else
Widget XmdsCreateXdBoxDialog(Widget parent, char *name, ArgList args,
                             Cardinal argcount);
#endif
#endif
