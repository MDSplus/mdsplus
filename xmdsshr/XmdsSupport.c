/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSSUPPORT.C */
/*  *6    23-JUN-1994 09:54:12 TWF "Motif 1.2" */
/*  *5    10-MAY-1993 14:23:11 TWF "Make it shareable" */
/*  *4     3-MAR-1993 09:39:18 JAS "use prototypes" */
/*  *3    25-FEB-1993 10:50:13 JAS "port to decc" */
/*  *2    16-JAN-1992 18:34:43 JAS "Add a putifChanged" */
/*  *1     5-DEC-1991 15:29:00 TWF "Support modules" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSSUPPORT.C */
/*------------------------------------------------------------------------------

		Name:   XMDSSUPPORT

		Type:   C function

		Author:	JOSH STILLERMAN

		Date:   16-JAN-1992

		Purpose: Support routines for Xmdsxd* widgets

------------------------------------------------------------------------------

	Call sequence:

extern Boolean ConglomerateElt(int nid);
extern int ConglomerateHead(int nid);
extern int DefaultNid(int nid);
extern int NodeParent(int nid);
extern void ResetErrors();
extern void TdiComplain(Widget w);
extern struct dsc$descriptor *TdiGet(int nid);
extern Boolean PutIfChanged(int nid, struct dsc$descriptor_xd *xd);

------------------------------------------------------------------------------
   Copyright (c) 1992
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:


------------------------------------------------------------------------------*/

#include <mdsdescrip.h>
#include <ncidef.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <strroutines.h>
#include <Xmds/XmdsSupport.h>
#include <mds_stdarg.h>

/*------------------------------------------------------------------------------

 External functions or symbols referenced:                                    */

extern int XmdsComplain();
extern int TdiDebug();
/*------------------------------------------------------------------------------

 Subroutines referenced:                                                      */

/*------------------------------------------------------------------------------

 Macros:                                                                      */

/*------------------------------------------------------------------------------

 Global variables:                                                            */

/*------------------------------------------------------------------------------

 Local variables:                                                             */

static struct descriptor_xd empty_xd = {0, DTYPE_DSC, CLASS_XD, 0, 0};

/*------------------------------------------------------------------------------

 Executable:                                                                  */
Boolean ConglomerateElt(int nid)
{
  static unsigned short cong_elt;
  static NCI_ITM lst[] = {{sizeof(cong_elt), NciCONGLOMERATE_ELT, (unsigned char *) &cong_elt, 0}, {0,0,0,0}};
  int status = TreeGetNci(nid, lst);
  if (status & 1)
    return (cong_elt != 0);
  else
    return 0;
}

int ConglomerateHead(int nid)
{
  static unsigned int head_nid;
  static NCI_ITM lst[] = {{sizeof(head_nid), NciCONGLOMERATE_NIDS, (unsigned char *) &head_nid, 0}, {0,0,0,0}};
  int status = TreeGetNci(nid, lst);
  if (status & 1)
    return head_nid;
  else
    return 0;
}

int DefaultNid(int nid)
{
  int ans = -1;
  if (ConglomerateElt(nid))
    ans = NodeParent(ConglomerateHead(nid));
  else
    ans = NodeParent(nid);
  return ans;
}

int NodeParent(int nid)
{
  static int parent_nid;
  static NCI_ITM lst[] = {{sizeof(parent_nid), NciPARENT, (unsigned char *) &parent_nid, 0}, {0,0,0,0}};
  int status = TreeGetNci(nid, lst);
  if (status & 1)
    return parent_nid;
  else
    return 0;
}

void ResetErrors()
{
  static int four = 4;
  static struct descriptor clear_messages = {4,DTYPE_L,CLASS_S,(char *) &four};
  static struct descriptor_d messages = {0,DTYPE_T,CLASS_D,0};
  TdiDebug(&clear_messages,&messages MDS_END_ARG);
  StrFree1Dx(&messages);
}

void TdiComplain(Widget w)
{
  static struct descriptor_d messages = {0,DTYPE_T,CLASS_D,0};
  static DESCRIPTOR(null,"\0");
  static int one = 1;
  static struct descriptor get_messages = {4,DTYPE_L,CLASS_S,(char *) &one};
  TdiDebug(&get_messages,&messages MDS_END_ARG);
  if (messages.length)
  {
    StrAppend(&messages,&null);
    XmdsComplain(w,messages.pointer);
    StrFree1Dx(&messages);
  }
}

struct descriptor *TdiGet(int nid)
{
  struct descriptor_xd *answer = (struct descriptor_xd *) XtMalloc(sizeof(struct descriptor_xd));
  int status;
  *answer = empty_xd;
  status = TreeGetRecord(nid,answer);
  if ((status & 1) == 0)
  {
    XtFree((char *)answer);
    answer = 0;
  }
  return (struct descriptor *) answer;
}

Boolean PutIfChanged(int nid,struct descriptor_xd *xd)
{
  int status = 1;
  if (nid)
  {
    if (xd)
    {
      struct descriptor_xd *old_xd = (struct descriptor_xd *) TdiGet(nid);
      if ((!old_xd && xd->l_length) || !MdsCompareXd((struct descriptor *)xd, (struct descriptor *)old_xd))
	status = TreePutRecord(nid,(struct descriptor *)xd,0);
      if (old_xd)
      {
	MdsFree1Dx(old_xd, 0);
	XtFree((char *)old_xd);
      }
    }
  }
  return status & 1;
}
