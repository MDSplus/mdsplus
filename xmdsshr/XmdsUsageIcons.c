/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSUSAGEICONS.C */
/*  *6    23-JUN-1994 12:07:36 TWF "Motif 1.2" */
/*  *5    22-FEB-1994 15:18:16 TWF "remove NO_X_GBLS" */
/*  *4     8-APR-1993 11:33:59 TWF "Use MrmPublic (much smaller)" */
/*  *3    25-FEB-1993 11:16:22 JAS "port to decc" */
/*  *2    15-APR-1992 16:22:23 JAS "don't reserve the colors" */
/*  *1    10-APR-1992 15:18:30 JAS "new usage icons code" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSUSAGEICONS.C */
/*------------------------------------------------------------------------------

		Name:   XMDSUSAGEICONS

		Type:   C function

		Author:	TOM FREDIAN

		Date:    5-JAN-1990

		Purpose: Return pointer to MDS Usage Icons
			 Must call XmdsUsageIconsInitialize first!

------------------------------------------------------------------------------

	Call sequence:

Pixmap *XmdsUsageIcons();
void XmdsUsageIconsInitialize(Widget w);

------------------------------------------------------------------------------
   Copyright (c) 1990
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:


------------------------------------------------------------------------------*/

#include <X11/Xlib.h>
#include <Xm/Xm.h>
#include <Mrm/MrmPublic.h>
#include <Xm/Xm.h>
#include <usagedef.h>
typedef int *DRMHierarchy;

void XmdsUsageIconsInitialize(Widget w);
Pixmap *XmdsUsageIcons();
Pixmap *XmdsUsageGrayIcons();

/*------------------------------------------------------------------------------

 External functions or symbols referenced:                                    */

/*------------------------------------------------------------------------------

 Subroutines referenced:                                                      */

/*------------------------------------------------------------------------------

 Macros:                                                                      */

/*------------------------------------------------------------------------------

 Global variables:                                                            */

static Pixmap *icons;
static Pixmap *gray_icons;

/*------------------------------------------------------------------------------

 Local variables:                                                             */

/*------------------------------------------------------------------------------

 Executable:                                                                  */

void XmdsUsageIconsInitialize(Widget w)
{
  static String hierarchy_name[] = {{"DECW$SYSTEM_DEFAULTS:XMDS_USAGE_ICONS.UID"}};
  static MrmHierarchy mrm_hierarchy;
  int usage;
  int usages;
  if (!icons)
  {
    int *TREE$K_MAX_USAGE;
    short int type;
    MrmInitialize();
    MrmOpenHierarchy(1,hierarchy_name,0,&mrm_hierarchy);
    MrmFetchLiteral(mrm_hierarchy,"TREE$K_MAX_USAGE",XtDisplay(w),(void *) &TREE$K_MAX_USAGE,&type);
    usages = *TREE$K_MAX_USAGE + 1;
    icons = (unsigned long *) XtMalloc(sizeof(Pixmap) * usages * 2);
    gray_icons = icons + usages;
    for (usage = 0; usage < usages; usage++)
    {
      char name[] = {'U',0,0,0,0};
      name[1] = '0' + (usage / 10);
      name[2] = '0' + usage - (usage / 10) * 10;
      MrmFetchIconLiteral(mrm_hierarchy,name,XtScreen(w),XtDisplay(w),0,1,&icons[usage]);
      MrmFetchIconLiteral(mrm_hierarchy,name,XtScreen(w),XtDisplay(w),0,1,&gray_icons[usage]);
    }
    MrmCloseHierarchy(mrm_hierarchy);
  }
}

Pixmap *XmdsUsageIcons()
{
  return icons;
}

Pixmap *XmdsUsageGrayIcons()
{
  return gray_icons;
}
