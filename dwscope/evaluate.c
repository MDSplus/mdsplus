/*  CMS REPLACEMENT HISTORY, Element EVALUATE.C */
/*  *41   18-SEP-1997 14:19:37 TWF "Add brief/full messages" */
/*  *40   27-AUG-1997 14:50:44 TWF "Add multiline error messages" */
/*  *39    5-FEB-1996 14:39:54 TWF "Remove stack setting" */
/*  *38    5-OCT-1995 15:29:19 TWF "Fix unix version" */
/*  *37    5-OCT-1995 15:11:45 TWF "Fix unix version" */
/*  *36   26-JAN-1995 13:37:13 TWF "Add dummy" */
/*  *35   26-JAN-1995 11:38:25 TWF "Add dummy" */
/*  *34   26-JAN-1995 11:36:35 TWF "Add dummy" */
/*  *33   26-JAN-1995 11:33:10 TWF "Add dummy" */
/*  *32    5-JAN-1995 16:10:19 TWF "check for _toupper" */
/*  *31    5-JAN-1995 14:15:52 TWF "move ipdesc" */
/*  *30   23-DEC-1994 09:50:21 TWF "Change event handling" */
/*  *29   22-DEC-1994 12:47:42 TWF "Need to flip bits on some architectures" */
/*  *28   22-DEC-1994 12:42:21 TWF "Need to flip bits on some architectures" */
/*  *27   22-DEC-1994 10:44:38 TWF "Fix evaluation of axes" */
/*  *26   22-DEC-1994 10:19:30 TWF "Fix \" */
/*  *25   21-DEC-1994 14:18:54 TWF "multiple events" */
/*  *24   21-DEC-1994 12:07:35 TWF "Add eventcan" */
/*  *23   21-DEC-1994 11:42:08 TWF "Add XtAddInput" */
/*  *22   21-DEC-1994 11:40:11 TWF "Add XtAddInput" */
/*  *21   20-DEC-1994 16:19:15 TWF "Add XtAddInput" */
/*  *20   11-OCT-1994 08:53:51 TWF "Add RPC interface" */
/*  *19    4-OCT-1994 08:00:57 TWF "Unix" */
/*  *18   23-MAR-1994 17:11:48 TWF "Take out no x gbls" */
/*  *17   23-MAR-1994 17:04:39 TWF "New DECW includes" */
/*  *16   15-MAR-1994 13:20:28 TWF "Update only" */
/*  *15   15-MAR-1994 13:18:29 TWF "Update only" */
/*  *14   15-MAR-1994 13:17:31 TWF "Update only" */
/*  *13   15-MAR-1994 13:06:48 TWF "Update only if " */
/*  *12   15-MAR-1994 11:41:19 TWF "Add row col event to evaluate calls" */
/*  *11   15-MAR-1994 11:37:23 TWF "Add row col event to evaluate calls" */
/*  *10   30-SEP-1993 12:14:16 TWF "Fix scalar" */
/*  *9    30-SEP-1993 11:42:25 TWF "Fix for scalars" */
/*  *8    30-SEP-1993 10:32:10 TWF "Draw one point?" */
/*  *7    19-MAY-1993 14:37:46 TWF "Add TREE$SET_STACK_SIZE" */
/*  *6    15-APR-1993 08:21:49 TWF "Fix debug clear" */
/*  *5    24-FEB-1993 17:51:18 TWF "Use DECC" */
/*  *4    10-DEC-1991 13:14:56 TWF "Remove excess lf's" */
/*  *3     2-DEC-1991 09:26:26 TWF "Close database at appropriate times" */
/*  *2    13-NOV-1991 11:18:04 TWF "Do TDI$DATA before TDI$ADJUSTL" */
/*  *1    12-NOV-1991 14:30:38 TWF "DWScope data evaluator for MDSplus" */
/*  CMS REPLACEMENT HISTORY, Element EVALUATE.C */
/*------------------------------------------------------------------------------

		Name:   EVALUATE

		Type:   C function

     		Author:	TOM FREDIAN

		Date:   26-JUN-1990

    		Purpose: Evaluate data for scope program 

------------------------------------------------------------------------------

	Call sequence: 

Boolean EvaluateData(Boolean brief, int row, int col, int idx, Boolean *event, 
                 String database, String shot, String default_node, String x, String y, 
                 XmdsWaveformValStruct *x_ret, XmdsWaveformValStruct *y_ret, String *error);

Boolean EvaluateText(String text, String error_prefix, String *text_ret, String *error);
void CloseDataSources();

------------------------------------------------------------------------------
   Copyright (c) 1990
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

 	Description:


------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xmds/XmdsWaveform.h>
#include <ctype.h>
#ifndef _toupper
# define _toupper(c)	(((c) >= 'a' && (c) <= 'z') ? (c) & 0xDF : (c))
#endif

#define _LOCAL_ACCESS

extern void EventUpdate(XtPointer client_data, int *source, XtInputId *id);

#if defined(__VMS)
#include <descrip.h>
#include <tdescrip.h>
#include <treeshr.h>
#include <str$routines.h>
#include <mdsshr.h>
#include <DXm/DecSpecific.h>
#include <descrip.h>
#include <str$routines.h>
#include <starlet.h>
#define EventEfn 16
extern char *DXmDescToNull ( struct dsc$descriptor_s *desc );
extern int TDI$ADJUSTL();
extern int TDI$EXECUTE();
extern int TDI$DATA();
extern int TDI$F_FLOAT();
extern int TDI$COMPILE();
extern int TDI$DIM_OF();
extern int TDI$DEBUG();

static void ResetErrors()
{
  static int const four = 4;
  static struct dsc$descriptor const clear_messages = {4,DSC$K_DTYPE_L,DSC$K_CLASS_S,(char *)&four};
  static struct dsc$descriptor messages = {0, DSC$K_DTYPE_T, DSC$K_CLASS_D, 0};
  TDI$DEBUG(&clear_messages,&messages);
  str$free1_dx(&messages);
}

static Boolean Error(Boolean brief, String topic, String *error, struct dsc$descriptor_xd *xd1, struct dsc$descriptor_xd *xd2)
{
  if (brief)
    *error = XtNewString(topic);
  else
  {
    static struct dsc$descriptor_d messages = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};
    static int const one = 1;
    static struct dsc$descriptor const get_messages = {4,DSC$K_DTYPE_L,DSC$K_CLASS_S,(char *)&one};
    static $DESCRIPTOR(const lflf,"\n\n");
    struct dsc$descriptor topic_d = {0,DSC$K_DTYPE_T,DSC$K_CLASS_S,0};
    topic_d.dsc$w_length = strlen(topic);
    topic_d.dsc$a_pointer = topic;
    TDI$DEBUG(&get_messages,&messages);
    str$concat(&messages,&topic_d,&lflf,&messages);
    *error = DXmDescToNull((struct dsc$descriptor_s *)&messages);
    str$free1_dx(&messages);
  }
  if (xd1)
    MDS$FREE1_DX(xd1);
  if (xd2)
    MDS$FREE1_DX(xd2);
  return 0;
}

static void DestroyXd(Widget w,struct dsc$descriptor_xd *xd)
{
  MDS$FREE1_DX(xd);
  free(xd);
}

Boolean EvaluateData(Boolean brief, int row, int col, int idx, Boolean *event, 
                 String database, String shot, String default_node, String x, String y, 
                 XmdsWaveformValStruct *x_ret, XmdsWaveformValStruct *y_ret, String *error)
{

#define min(a,b) ( ((a)<(b)) ? (a) : (b) )
#define max(a,b) ( ((a)>(b)) ? (a) : (b) )

  static $DESCRIPTOR(rowv,"_ROW=$");
  static $DESCRIPTOR(colv,"_COLUMN=$");
  static $DESCRIPTOR(idxv,"_INDEX=$");
  static int ival;
  static $DESCRIPTOR_LONG(ival_d,&ival);
  ival = row;
  TDI$EXECUTE(&rowv,&ival_d,&ival_d);
  ival = col;
  TDI$EXECUTE(&colv,&ival_d,&ival_d);
  ival = idx;
  TDI$EXECUTE(&idxv,&ival_d,&ival_d);
  if (strlen(database))
  {
    struct dsc$descriptor database_dsc = {0,DSC$K_DTYPE_T,DSC$K_CLASS_S,0};
    static int shotnum;
    static int one = 1;
    shotnum = 0;
    database_dsc.dsc$w_length = strlen(database);
    database_dsc.dsc$a_pointer = database;
    if (strlen(shot))
    {
      struct dsc$descriptor shot_dsc = {0,DSC$K_DTYPE_T,DSC$K_CLASS_S,0};
      static $DESCRIPTOR_LONG(shotnum_dsc,&shotnum);
      shot_dsc.dsc$w_length = strlen(shot);
      shot_dsc.dsc$a_pointer = shot;
      ResetErrors();
      if (!(TDI$EXECUTE(&shot_dsc,&shotnum_dsc)&1))
        return Error(brief, "Error evaluating shot number", error, 0, 0);
      if (event)
      {
        int i;
        String upper_shot = XtNewString(shot);
        int len = strlen(shot);
        for (i=0;i<len;i++) upper_shot[i] = _toupper(shot[i]);
        *event = shotnum == 0 ? 1 : (strstr(upper_shot,"CURRENT_SHOT") ? 1 : 0);
        XtFree(upper_shot);
        if (! *event) return 1;
      }
    }
/*    TREE$SET_STACK_SIZE(&one); */
    if (!(TREE$OPEN_TREE(&database_dsc,&shotnum)&1)) 
      return Error(1, "Error opening database", error, 0, 0);
  }
  if (strlen(default_node))
  {
    struct dsc$descriptor default_node_dsc = {0,DSC$K_DTYPE_T,DSC$K_CLASS_S,0};
    int nid;
    default_node_dsc.dsc$w_length = strlen(default_node);
    default_node_dsc.dsc$a_pointer = default_node;
    TREE$SET_DEFAULT_NID(&0);
    if (!(TREE$SET_DEFAULT(&default_node_dsc,&nid)&1))
      return Error(1, "Default node not found", error, 0, 0);
  }
  else
    TREE$SET_DEFAULT_NID(&0);
  if (strlen(y))
  {
    struct dsc$descriptor y_dsc = {0,DSC$K_DTYPE_T,DSC$K_CLASS_S,0};
    static $EMPTYXD(sig);
    $EMPTYXD(y_xd);
    y_dsc.dsc$w_length = strlen(y);
    y_dsc.dsc$a_pointer = y;
    ResetErrors();
    if ((TDI$EXECUTE(&y_dsc,&sig) & 1) && (TDI$DATA(sig.dsc$a_pointer,&y_xd) & 1) && (TDI$F_FLOAT(&y_xd,&y_xd) & 1))
    {
      struct dsc$descriptor_a *y_a = (struct dsc$descriptor_a *)y_xd.dsc$a_pointer;
      int count = (y_a->dsc$b_class == DSC$K_CLASS_A) ? y_a->dsc$l_arsize/sizeof(float) : 1;
      if (count >= 1)
      {
        $EMPTYXD(x_xd);
        int status;
        ResetErrors();
        if (strlen(x))
        {
          struct dsc$descriptor x_dsc = {0,DSC$K_DTYPE_T,DSC$K_CLASS_S,0};
          x_dsc.dsc$w_length = strlen(x);
          x_dsc.dsc$a_pointer = x;
          status = (TDI$COMPILE(&x_dsc,&sig) & 1) && (TDI$DATA(&sig,&x_xd) & 1) && (TDI$F_FLOAT(&x_xd,&x_xd) & 1);
        }
        else
          status = (TDI$DIM_OF(&sig,&x_xd) & 1) && (TDI$DATA(&x_xd,&x_xd) & 1) && (TDI$F_FLOAT(&x_xd,&x_xd) & 1);
        if (!(status & 1) && (y_a->dsc$b_class == DSC$K_CLASS_S))
        {
          static int zero=0;
          static $DESCRIPTOR_LONG(zero_d,&zero);
          status = MDS$COPY_DXXD(&zero_d,&x_xd);
        }
        MDS$FREE1_DX(&sig);
        if (status)
        {
          struct dsc$descriptor_a *x_a = (struct dsc$descriptor_a *)x_xd.dsc$a_pointer;
          count = (x_a->dsc$b_class == DSC$K_CLASS_A) ? min(x_a->dsc$l_arsize/sizeof(float),count) : 1;
          if (count >= 1)
          {
            x_ret->size = (x_a->dsc$b_class == DSC$K_CLASS_A) ? x_a->dsc$l_arsize : sizeof(float);
            x_ret->addr = x_a->dsc$a_pointer;
            x_ret->destroy = DestroyXd;
            x_ret->destroy_arg = memcpy(malloc(sizeof(x_xd)),&x_xd,sizeof(x_xd));
            y_ret->size = (y_a->dsc$b_class == DSC$K_CLASS_A) ? y_a->dsc$l_arsize : sizeof(float);
            y_ret->addr = y_a->dsc$a_pointer;
            y_ret->destroy = DestroyXd;
            y_ret->destroy_arg = memcpy(malloc(sizeof(y_xd)),&y_xd,sizeof(y_xd));
          }
          else
            return Error(1, "X-axis contains no points", error, &y_xd, &x_xd);
        }
        else
          return Error(brief, "Error evaluating X-axis", error, &y_xd, &x_xd);
      }
      else
        return Error(1, "Y-axis contains no points", error, &y_xd, 0);
    }
    else
      return Error(brief, "Error evaluating Y-axis", error, &y_xd, 0);
  }
  else
    return Error(1, "", error, 0, 0);
  return 1;
}

Boolean EvaluateText(String text, String error_prefix, String *text_ret, String *error)
{
  Boolean status = 1;
  if (strlen(text))
  {
    struct dsc$descriptor text_dsc = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
    static $EMPTYXD(string_xd);
    static struct dsc$descriptor_d string_d = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};
    text_dsc.dsc$w_length = strlen(text);
    text_dsc.dsc$a_pointer = text;
    ResetErrors();
    if ( (TDI$EXECUTE(&text_dsc,&string_xd)&1) && 
         (TDI$DATA(&string_xd, &string_xd)&1) && 
         (TDI$ADJUSTL(&string_xd,&string_d) & 1))
    {
      str$trim(&string_d,&string_d);
      *text_ret = DXmDescToNull((struct dsc$descriptor_s *)&string_d);
    }
    else
    {
      *text_ret = XtNewString("");
      status = Error(0, error_prefix, error, &string_xd, 0);
    }
    str$free1_dx(&string_d);
    MDS$FREE1_DX(&string_xd);
  }
  else
    *text_ret = XtNewString("");
  return status;
}

void CloseDataSources()
{
  while (TREE$CLOSE_TREE() & 1);
}  

static void  EventAst(Boolean *received)
{
  sys$setef(EventEfn);
  *received = True;
}

void SetupEvent(String event, Boolean *received, int **id)
{
  if (*id)
  {
    MDS$EVENTCAN(*id);
    *id = 0;
  }
  if (strlen(event))
  {
    struct dsc$descriptor event_in = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
    struct dsc$descriptor event_d = {0, DSC$K_DTYPE_T, DSC$K_CLASS_D, 0};
    event_in.dsc$w_length = strlen(event);
    event_in.dsc$a_pointer = event;
    str$trim(&event_d, &event_in);
    MDS$EVENTAST(&event_d, (void (*)(int)) EventAst, received, id);
    str$free1_dx(&event_d);
  }
}

static void DoEventUpdate(XtPointer client_data, int *source, XtInputId *id)
{
  sys$clref(EventEfn);
  EventUpdate(client_data, source, id);
}

void SetupEventInput(XtAppContext app_context)
{
  sys$clref(EventEfn);
  XtAppAddInput(app_context, EventEfn, 0, DoEventUpdate, 0);
}

#elif defined(_LOCAL_ACCESS)
#include <mdsdescrip.h>
#include <mds_stdarg.h>
#include <treeshr.h>
#include <strroutines.h>
#include <mdsshr.h>

extern int TdiAdjustl();
extern int TdiExecute();
extern int TdiData();
extern int TdiFloat();
extern int TdiCompile();
extern int TdiDimOf();
extern int TdiDebug();

static void ResetErrors()
{
  static int const four = 4;
  static struct descriptor const clear_messages = {4,DTYPE_L,CLASS_S,(char *)&four};
  static struct descriptor messages = {0, DTYPE_T, CLASS_D, 0};
  TdiDebug(&clear_messages,&messages MDS_END_ARG);
  StrFree1Dx(&messages);
}

static Boolean Error(Boolean brief, String topic, String *error, struct descriptor_xd *xd1, struct descriptor_xd *xd2)
{
  if (brief)
    *error = XtNewString(topic);
  else
  {
    static struct descriptor_d messages = {0,DTYPE_T,CLASS_D,0};
    static int const one = 1;
    static struct descriptor const get_messages = {4,DTYPE_L,CLASS_S,(char *)&one};
    static DESCRIPTOR(const lflf,"\n\n");
    struct descriptor topic_d = {0,DTYPE_T,CLASS_S,0};
    topic_d.length = strlen(topic);
    topic_d.pointer = topic;
    TdiDebug(&get_messages,&messages MDS_END_ARG);
    StrConcat(&messages,&topic_d,&lflf,&messages,0);
    *error = memcpy(XtMalloc(messages.length+1),messages.pointer,messages.length);
    (*error)[messages.length]='\0';
    StrFree1Dx(&messages);
  }
  if (xd1)
    MdsFree1Dx(xd1,0);
  if (xd2)
    MdsFree1Dx(xd2,0);
  return 0;
}

static void DestroyXd(Widget w,struct descriptor_xd *xd)
{
  MdsFree1Dx(xd,0);
  free(xd);
}

Boolean EvaluateData(Boolean brief, int row, int col, int idx, Boolean *event, 
                 String database, String shot, String default_node, String x, String y, 
                 XmdsWaveformValStruct *x_ret, XmdsWaveformValStruct *y_ret, String *error)
{

#define min(a,b) ( ((a)<(b)) ? (a) : (b) )
#define max(a,b) ( ((a)>(b)) ? (a) : (b) )

  static DESCRIPTOR(rowv,"_ROW=$");
  static DESCRIPTOR(colv,"_COLUMN=$");
  static DESCRIPTOR(idxv,"_INDEX=$");
  static int ival;
  static DESCRIPTOR_LONG(ival_d,&ival);
  ival = row;
  TdiExecute(&rowv,&ival_d,&ival_d MDS_END_ARG);
  ival = col;
  TdiExecute(&colv,&ival_d,&ival_d MDS_END_ARG);
  ival = idx;
  TdiExecute(&idxv,&ival_d,&ival_d MDS_END_ARG);
  if (strlen(database))
  {
    static int shotnum;
    static int one = 1;
    shotnum = 0;
    if (strlen(shot))
    {
      struct descriptor shot_dsc = {0,DTYPE_T,CLASS_S,0};
      static DESCRIPTOR_LONG(shotnum_dsc,&shotnum);
      shot_dsc.length = strlen(shot);
      shot_dsc.pointer = shot;
      ResetErrors();
      if (!(TdiExecute(&shot_dsc,&shotnum_dsc MDS_END_ARG)&1))
        return Error(brief, "Error evaluating shot number", error, 0, 0);
      if (event)
      {
        int i;
        String upper_shot = XtNewString(shot);
        int len = strlen(shot);
        for (i=0;i<len;i++) upper_shot[i] = _toupper(shot[i]);
        *event = shotnum == 0 ? 1 : (strstr(upper_shot,"CURRENT_SHOT") ? 1 : 0);
        XtFree(upper_shot);
        if (! *event) return 1;
      }
    }
    if (!(TreeOpen(database,shotnum, 1)&1)) 
      return Error(1, "Error opening database", error, 0, 0);
  }
  if (strlen(default_node))
  {
    int nid;
    TreeSetDefaultNid(0);
    if (!(TreeSetDefault(default_node,&nid)&1))
      return Error(1, "Default node not found", error, 0, 0);
  }
  else
    TreeSetDefaultNid(0);
  if (strlen(y))
  {
    struct descriptor y_dsc = {0,DTYPE_T,CLASS_S,0};
    static EMPTYXD(sig);
    EMPTYXD(y_xd);
    y_dsc.length = strlen(y);
    y_dsc.pointer = y;
    ResetErrors();
    if ((TdiExecute(&y_dsc,&sig MDS_END_ARG) & 1) && (TdiData(sig.pointer,&y_xd MDS_END_ARG) & 1) && (TdiFloat(&y_xd,&y_xd MDS_END_ARG) & 1))
    {
      struct descriptor_a *y_a = (struct descriptor_a *)y_xd.pointer;
      int count = (y_a->class == CLASS_A) ? y_a->arsize/sizeof(float) : 1;
      if (count >= 1)
      {
        EMPTYXD(x_xd);
        int status;
        ResetErrors();
        if (strlen(x))
        {
          struct descriptor x_dsc = {0,DTYPE_T,CLASS_S,0};
          x_dsc.length = strlen(x);
          x_dsc.pointer = x;
          status = (TdiCompile(&x_dsc,&sig MDS_END_ARG) & 1) && (TdiData(&sig,&x_xd MDS_END_ARG) & 1) && (TdiFloat(&x_xd,&x_xd MDS_END_ARG) & 1);
        }
        else
          status = (TdiDimOf(&sig,&x_xd MDS_END_ARG) & 1) && (TdiData(&x_xd,&x_xd MDS_END_ARG) & 1) && (TdiFloat(&x_xd,&x_xd MDS_END_ARG) & 1);
        if (!(status & 1) && (y_a->class == CLASS_S))
        {
          static int zero=0;
          static DESCRIPTOR_LONG(zero_d,&zero);
          status = MdsCopyDxXd(&zero_d,&x_xd);
        }
        MdsFree1Dx(&sig,0);
        if (status)
        {
          struct descriptor_a *x_a = (struct descriptor_a *)x_xd.pointer;
          count = (x_a->class == CLASS_A) ? min(x_a->arsize/sizeof(float),count) : 1;
          if (count >= 1)
          {
            x_ret->size = (x_a->class == CLASS_A) ? x_a->arsize : sizeof(float);
            x_ret->addr = x_a->pointer;
            x_ret->destroy = DestroyXd;
            x_ret->destroy_arg = memcpy(malloc(sizeof(x_xd)),&x_xd,sizeof(x_xd));
            y_ret->size = (y_a->class == CLASS_A) ? y_a->arsize : sizeof(float);
            y_ret->addr = y_a->pointer;
            y_ret->destroy = DestroyXd;
            y_ret->destroy_arg = memcpy(malloc(sizeof(y_xd)),&y_xd,sizeof(y_xd));
          }
          else
            return Error(1, "X-axis contains no points", error, &y_xd, &x_xd);
        }
        else
          return Error(brief, "Error evaluating X-axis", error, &y_xd, &x_xd);
      }
      else
        return Error(1, "Y-axis contains no points", error, &y_xd, 0);
    }
    else
      return Error(brief, "Error evaluating Y-axis", error, &y_xd, 0);
  }
  else
    return Error(1, "", error, 0, 0);
  return 1;
}

Boolean EvaluateText(String text, String error_prefix, String *text_ret, String *error)
{
  Boolean status = 1;
  if (strlen(text))
  {
    struct descriptor text_dsc = {0, DTYPE_T, CLASS_S, 0};
    static EMPTYXD(string_xd);
    static struct descriptor_d string_d = {0,DTYPE_T,CLASS_D,0};
    text_dsc.length = strlen(text);
    text_dsc.pointer = text;
    ResetErrors();
    if ( (TdiExecute(&text_dsc,&string_xd MDS_END_ARG)&1) && 
         (TdiData(&string_xd, &string_xd MDS_END_ARG)&1) && 
         (TdiAdjustl(&string_xd,&string_d MDS_END_ARG) & 1))
    {
      StrTrim(&string_d,&string_d,0);
      *text_ret = memcpy(XtMalloc(string_d.length+1),string_d.pointer,string_d.length);
      (*text_ret)[string_d.length] = '\0';
    }
    else
    {
      *text_ret = XtNewString("");
      status = Error(0, error_prefix, error, &string_xd, 0);
    }
    StrFree1Dx(&string_d);
    MdsFree1Dx(&string_xd,0);
  }
  else
    *text_ret = XtNewString("");
  return status;
}

void CloseDataSources()
{
  while (TreeClose(NULL,0) & 1);
}  

static void  EventAst(Boolean *received)
{
}

void SetupEvent(String event, Boolean *received, int **id)
{
  if (*id)
  {
  }
  if (strlen(event))
  {
  }
}

static void DoEventUpdate(XtPointer client_data, int *source, XtInputId *id)
{
}

void SetupEventInput(XtAppContext app_context)
{
}

#elif defined(_RPC)

#include <rpc/rpc.h>
#include <MdsRpc/mds_rpc.h>
#include <Xmds/XmdsWaveform.h>
static void Destroy_xy(Widget w, float *ptr)
{
  free(ptr);
}

Boolean EvaluateData(Boolean brief, int row, int col, int idx, Boolean *event, 
                 String database, String shot, String default_node, String x, String y, 
                 XmdsWaveformValStruct *x_ret, XmdsWaveformValStruct *y_ret, String *error)
{

  CLIENT *cl;
  char *rpc_server = getenv("mds_rpc_server");
  mds_scope_xy_res *result;
  mds_scope_xy_inputs rpc_input;
  int status;
  float *x_ptr, *y_ptr;

  rpc_input.brief = brief;
  rpc_input.database = database;
  rpc_input.shot     = shot;
  rpc_input.y_expr   = y;
  rpc_input.x_expr   = x;
  rpc_input.default_node = default_node;

  cl = clnt_create(rpc_server, MDSPLUS, MDSPLUSVERS, "tcp");
  if(cl == NULL)
  {
     *error = clnt_spcreateerror(rpc_server);
     return(0);
  }

  result = mds_scope_xy_value_1(&rpc_input, cl);
  if(result == NULL)
  {
    *error = clnt_sperror(cl, rpc_server);
    return(0);
  }

  switch(result->errno) {
    case 0: x_ptr = malloc(result->mds_scope_xy_res_u.results->x_value.len * sizeof(float));
	    memcpy(x_ptr, result->mds_scope_xy_res_u.results->x_value.ptr, 
				result->mds_scope_xy_res_u.results->x_value.len * sizeof(float) );

 	    x_ret->size = result->mds_scope_xy_res_u.results->x_value.len * sizeof(float);
            x_ret->addr = (caddr_t) x_ptr;
            x_ret->destroy = Destroy_xy;
            x_ret->destroy_arg = (caddr_t) x_ptr;

	    y_ptr = malloc(result->mds_scope_xy_res_u.results->y_value.len * sizeof(float));
	    memcpy(y_ptr, result->mds_scope_xy_res_u.results->y_value.ptr, 
				result->mds_scope_xy_res_u.results->x_value.len * sizeof(float) );
            y_ret->size =  result->mds_scope_xy_res_u.results->y_value.len * sizeof(float);
            y_ret->addr = (caddr_t) y_ptr;
            y_ret->destroy = 0;
            y_ret->destroy = Destroy_xy;
            y_ret->destroy_arg = (caddr_t) y_ptr;

  	    xdr_free(xdr_mds_scope_xy_res, result);
	    status = 1;
	    break;
    case 1: if(result->mds_scope_xy_res_u.results_error->error_len) {
	    	*error = XtCalloc(1, result->mds_scope_xy_res_u.results_error->error_len + 1);
	    	strcpy(*error, result->mds_scope_xy_res_u.results_error->error);
	    } else 
		*error = XtNewString("");
  	    xdr_free(xdr_mds_scope_xy_res, result);
	    status = 0;
            break;  
  }

  clnt_destroy(cl);       

  return(status);
}

Boolean EvaluateText(String text, String error_prefix, String *text_ret, String *error)
{
  CLIENT *cl;  
  char *rpc_server = getenv("mds_rpc_server");
  mds_scope_text_res *result;
  mds_scope_text_inputs rpc_input;
  int status;

  rpc_input.text = text;
  rpc_input.error_prefix = error_prefix;

  cl = clnt_create(rpc_server, MDSPLUS, MDSPLUSVERS, "tcp");
  if(cl == NULL)
  {
     *error = clnt_spcreateerror(rpc_server);
    *text_ret = XtNewString("");
     return(0);
  }

  result = mds_scope_text_value_1(&rpc_input, cl);
  if(result == NULL)
  {
    *error = clnt_sperror(cl, rpc_server);
    *text_ret = XtNewString("");
    return(0);
  }

  switch(result->errno) {
    case 0: if(result->mds_scope_text_res_u.results->len) {
	    	*text_ret = XtCalloc(1, result->mds_scope_text_res_u.results->len + 1);
	    	strcpy(*text_ret, result->mds_scope_text_res_u.results->ptr);
		(*text_ret)[result->mds_scope_text_res_u.results->len] = 0;
	    } else
	         *text_ret = XtNewString("");

	    xdr_free(xdr_mds_scope_text_res, result);
	    status = 1;
	    break;
    case 1: if(result->mds_scope_text_res_u.results_error->len) {
	    	*error = XtCalloc(1, result->mds_scope_text_res_u.results_error->len + 1);
	    	strcpy(*error, result->mds_scope_text_res_u.results_error->ptr);
		(*error)[result->mds_scope_text_res_u.results_error->len] = 0;
	         *text_ret = XtNewString("");
	    } else
		 *error = XtNewString("");

  	    xdr_free(xdr_mds_scope_text_res, result);
	    status = 0;
            break;  
  }

  clnt_destroy(cl);       

  return(status);
}
void CloseDataSources(){}  
void SetupEvent(String event, Boolean *received, int *id){}
void SetupEventInput(XtAppContext app_context){}

#elif defined(_DUMMY_)
#include <math.h>
static Boolean Error(Boolean brief, String topic, String *error, String text, int *dummy)
{
/*
  if (brief)
    *error = XtNewString(topic);
  else {
*/
    if (text) {
      String message = XtMalloc(strlen(topic) + strlen(text) + 5);
      sprintf(message,"%s\n\n%s",topic,text);
      *error = message;
    }
    else {
      String message = XtMalloc(strlen(topic) + 5);
      sprintf(message,"%s\n\n%s",topic);
      *error = message;
    }
/*
  }
*/
  return 0;
}

static void Destroy(Widget w,String ptr)
{
  free(ptr);
}

Boolean EvaluateData(Boolean brief, int row, int col, int idx, Boolean *event, 
                 String database, String shot, String default_node, String x, String y, 
                 XmdsWaveformValStruct *x_ret, XmdsWaveformValStruct *y_ret, String *error)
{
  if (strlen(y)) {
    int count = 10000;
    int i;
    float *xv = (float *)malloc(count * sizeof(float));
    float *yv = (float *)malloc(count * sizeof(float));
    for (i=0;i<count;i++)
    {
      xv[i] = i;
      yv[i] = sin(i/1000.*3.14);
    }
    x_ret->size = count * sizeof(float);
    x_ret->addr = (caddr_t)xv;
    x_ret->destroy = Destroy;
    x_ret->destroy_arg = (caddr_t)xv;
    y_ret->size = count * sizeof(float);
    y_ret->addr = (caddr_t)yv;
    y_ret->destroy = Destroy;
    y_ret->destroy_arg = (caddr_t)yv;
  }
  else
    return Error(1,"",error,NULL,NULL);
  return 1;
}

Boolean EvaluateText(String text, String error_prefix, String *text_ret, String *error)
{
  Boolean status = 1;
  if (strlen(text))
  {
    *text_ret = XtNewString(text);
  }
  else
    *text_ret = XtNewString("");
  return status;
}

void CloseDataSources()
{
}  

static void  EventReceived(Boolean *received)
{
}

void SetupEvent(String event, Boolean *received, int *id)
{
}

static void DoEventUpdate(XtPointer client_data, int *source, XtInputId *id)
{
}

void SetupEventInput(XtAppContext app_context)
{
}


#else
#include <ipdesc.h>

static long Connect()
{
  static long sock;
  if (!sock) {
    sock = ConnectToMds(getenv("mds_host"));
  }
  if (sock == -1)
    exit(1);
  return sock;
}

static long ConnectEvents()
{
  static long sock;
  if (!sock) {
    sock = ConnectToMdsEvents(getenv("mds_host"));
  }
  if (sock == -1)
    exit(1);
  return sock;
}

#define FreeDescrip(x) if (x.ptr != NULL) {free(x.ptr); x.ptr = NULL;}
#define Descrip(name,type,ptr) struct descrip name = {type,0,{0,0,0,0,0,0,0,0},ptr}

static Boolean Error(Boolean brief, String topic, String *error, String text, struct descrip *dsc)
{
/*
  if (brief)
    *error = XtNewString(topic);
  else {
*/
    if (text) {
      String message = XtMalloc(strlen(topic) + strlen(text) + 5);
      sprintf(message,"%s\n\n%s",topic,text);
      *error = message;
    }
    else {
      String message = XtMalloc(strlen(topic) + 5);
      sprintf(message,"%s\n\n%s",topic);
      *error = message;
    }
/*
  }
*/
  if (dsc != NULL)
    if (dsc->ptr != NULL) {
      free(dsc->ptr);
      dsc->ptr = NULL;
    }
  return 0;
}

static int Nelements(struct descrip *in)
{
  if (in->ndims) {
    int num = 1;
    int i;
    for (i=0;i<in->ndims;i++) num *= in->dims[i];
    return num;
  }
  else
    return 1;
}

static void Destroy(Widget w,String ptr)
{
  free(ptr);
}

Boolean EvaluateData(Boolean brief, int row, int col, int idx, Boolean *event, 
                 String database, String shot, String default_node, String x, String y, 
                 XmdsWaveformValStruct *x_ret, XmdsWaveformValStruct *y_ret, String *error)
{
  int shotnum = 0;
  static int ival;
  static Descrip(id, DTYPE_LONG, &ival);
  Descrip(ans,0,NULL);
  int status;
  long sock = Connect();
  ival = row;
  status = MdsValue(sock,"_ROW=$",&id,&ans,0);
  FreeDescrip(ans)
  ival = col;
  status = MdsValue(sock,"_COLUMN=$",&id,&ans,0);
  FreeDescrip(ans)
  ival = idx;
  status = MdsValue(sock,"_INDEX=$",&id,&ans,0);
  FreeDescrip(ans)
  if (strlen(database)) {
    if (strlen(shot)) {
      Descrip(ans,0,NULL);
      Descrip(tmp,0,NULL);
      if (!(MdsValue(sock,"long(execute($))",MakeDescrip(&tmp,DTYPE_CSTRING,0,NULL,shot),&ans,0) & 1))
        return Error(brief, "Error evaluating shot number", error, ans.ptr, &ans);
      shotnum = *(int *)ans.ptr;
      if (event) {
	int i;
	String upper_shot = XtNewString(shot);
	int len = strlen(shot);
	for (i=0;i<len;i++) upper_shot[i] = _toupper(shot[i]);
	*event = shotnum == 0 ? 1 : (strstr(upper_shot,"CURRENT_SHOT") ? 1 : 0);
	XtFree(upper_shot);
	if (! *event) return 1;
      }
    }
    if (!(MdsOpen(sock,database,shotnum) & 1))
      return Error(1, "Error opening database", error, NULL, NULL);
  }
  if (strlen(default_node)) {
    MdsValue(sock,"treeshr->tree$set_default_nid(0)",&ans,0);
    FreeDescrip(ans);
    if (!(MdsSetDefault(sock,default_node) & 1))
      return Error(1, "Default node not found", error, NULL, NULL);
  }
  else {
    MdsValue(sock,"treeshr->tree$set_default_nid(0)",&ans,0);
    FreeDescrip(ans);
  }
  if (strlen(y)) {
    Descrip(yans,0,NULL);
    String yexp = XtMalloc(strlen(y)+100);
    sprintf(yexp,"_y$$dwscope = (%s),f_float(data(_y$$dwscope))",y);
    if (MdsValue(sock,yexp,&yans,0) & 1) {
      int count = Nelements(&yans);
      if (count >= 1) {
	Descrip(xans,0,NULL);
	if (strlen(x)) {
	  String xexp = XtMalloc(strlen(x)+100);
	  sprintf(xexp,"f_float(data(%s))",x);
	  status = MdsValue(sock,xexp,&xans,0);
          XtFree(xexp);
	}
	else
	  status = MdsValue(sock,"f_float(data(dim_of(_y$$dwscope)))",&xans,0);
        if (status & 1) {
	  int xcount = Nelements(&xans);
          if (xcount < count) count = xcount;
          if (count >= 1) {
	    x_ret->size = count * sizeof(float);
	    x_ret->addr = xans.ptr;
	    x_ret->destroy = Destroy;
	    x_ret->destroy_arg = xans.ptr;
	    y_ret->size = count * sizeof(float);
	    y_ret->addr = yans.ptr;
	    y_ret->destroy = Destroy;
	    y_ret->destroy_arg = yans.ptr;
	  }
	  else {
	    FreeDescrip(xans);
	    FreeDescrip(yans);
	    return Error(1, "X-axis contains no points",error,NULL,NULL);
	  }
	}
	else {
	  FreeDescrip(yans);
	  return Error(brief,"Error evaluating X-axis", error, xans.ptr, &xans);
	}
      }
      else
	return Error(1,"Y-axis contains no points", error, NULL, &yans);
    }
    else
      return Error(brief,"Error evaluating Y-axis", error, yans.ptr, &yans);
  }
  else
    return Error(1,"",error,NULL,NULL);
  return 1;
}

Boolean EvaluateText(String text, String error_prefix, String *text_ret, String *error)
{
  Boolean status = 1;
  long sock = Connect();  
  *text_ret = NULL;
     
  if (strlen(text) && sock)
  {
    Descrip(ans,0,NULL);
    Descrip(tmp,0,NULL);
    if (MdsValue(sock,"trim(adjustl(execute($)))",MakeDescrip(&tmp,DTYPE_CSTRING,0,NULL,text),&ans,0) & 1) {
      *text_ret = XtNewString(ans.ptr);
      FreeDescrip(ans);
    }
    else {
      *text_ret = XtNewString("");
      status = Error(0, error_prefix, error, ans.ptr, &ans);
    }
  }
  else
    *text_ret = XtNewString("");
  return status;
}

void CloseDataSources()
{
  long sock = Connect();
  if (sock)
    MdsClose(sock);
}  

static void  EventReceived(Boolean *received)
{
  *received = True;
}

void SetupEvent(String event, Boolean *received, int *id)
{
  if (*id) {
    MdsEventCan(ConnectEvents(), *id);
    *id = 0;
  }
  if (strlen(event)) {
    MdsEventAst(ConnectEvents(), event, EventReceived, received, id);
  }
}

extern int MdsDispatchEvent();

static void DoEventUpdate(XtPointer client_data, int *source, XtInputId *id)
{
  MdsDispatchEvent(ConnectEvents());
  EventUpdate(client_data, source, id);
}

void SetupEventInput(XtAppContext app_context)
{
  XtAppAddInput(app_context, ConnectEvents(), (XtPointer)XtInputExceptMask, DoEventUpdate, 0);
}

#endif /* __VMS */
