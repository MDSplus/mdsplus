/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSSHR.H */
/*  *2     3-MAR-1993 15:46:57 JAS "fix rowcolreference" */
/*  *1     3-MAR-1993 15:38:32 JAS "public include for xmdshr" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSSHR.H */
#ifndef __XMDSSHR
#define __XMDSSHR

/******************* Prototypes for XMDSSHR routines *********************/

#ifdef _NO_PROTO

extern Boolean	XmdsApplyAllXds();
extern int	XmdsBells();
extern void	XmdsComplain();
extern int	XmdsDeviceSetup();
extern int	XmdsGetDeviceNid();
extern int	XmdsGetNidBooleanValue();
extern int	XmdsGetNidFloatOption();
extern int	XmdsGetNidIntOption();
extern int	XmdsGetNidLongValue();
extern char	*XmdsGetNidText();
extern int	XmdsGetOptionIdx();
extern void	XmdsInitialize();
extern int	XmdsIsOn();
extern void	XmdsManageWindow();
extern void	XmdsPopupMenuPosition();
extern int	XmdsPutNidScale();
extern int	XmdsPutNidSText();
extern int	XmdsPutNidToggleButton();
extern int	XmdsPutNidValue();
extern void	XmdsRaiseWindow();
extern void	XmdsResetAllXds();
extern void	XmdsSetDeviceNid();
extern void	XmdsSetOptionIdx();
extern int	XmdsSetState();
extern int	XmdsSetSubvalues();
extern Pixmap	*XmdsUsageGrayIcons();
extern Pixmap	*XmdsUsageIcons();
extern void	XmdsUsageIconsInitialize();
extern int	XmdsWidgetToNumber();
extern Boolean	XmdsXdsAreValid();

#else 

extern Boolean	XmdsApplyAllXds(Widget w);
extern int	XmdsBells(int *number_of_bells );
extern void	XmdsComplain(Widget parent,char *string,...);
extern int	XmdsDeviceSetup(Widget parent,int *nid,String uids[],Cardinal num_uids,String ident,MrmRegisterArglist reglist,
		    MrmCount regnum,Widget *widget_return);
extern int	XmdsGetDeviceNid();
extern int	XmdsGetNidBooleanValue(int nid,int defaultVal);
extern int	XmdsGetNidFloatOption(int nid,float *options,int numOptions);
extern int	XmdsGetNidIntOption(int nid,int *options,int numOptions);
extern int	XmdsGetNidLongValue(int nid,int minVal,int maxVal,int defaultVal);
extern char	*XmdsGetNidText(int nid);
extern int	XmdsGetOptionIdx(Widget w);
extern void	XmdsInitialize();
extern int	XmdsIsOn(int nid);
extern void	XmdsManageWindow(Widget w);
extern void	XmdsPopupMenuPosition(Widget w,XButtonEvent *event);
extern int	XmdsPutNidScale(Widget w,int nid,int originalValue);
extern int	XmdsPutNidSText(Widget w,int nid,char *originalValue);
extern int	XmdsPutNidToggleButton(Widget w,int nid,int originalValue);
extern int	XmdsPutNidValue(int nid,unsigned short length,unsigned char dtype,char *pointer,char *originalValue);
extern void	XmdsRaiseWindow(Widget w);
extern void	XmdsResetAllXds(Widget w);
extern void	XmdsSetDeviceNid(int nid);
extern void	XmdsSetOptionIdx(Widget w,int idx);
extern int	XmdsSetState(int nid,Widget w);
extern int	XmdsSetSubvalues(XtPointer record,XtResourceList resources,Cardinal num_resources,ArgList args,Cardinal argcount);
extern Pixmap	*XmdsUsageGrayIcons();
extern Pixmap	*XmdsUsageIcons();
extern void	XmdsUsageIconsInitialize(Widget w);
extern int	XmdsWidgetToNumber(Widget w,String prefix);
extern Boolean	XmdsXdsAreValid(Widget w);

#endif /* else no proto */

#endif /* xmdsshr defined */
