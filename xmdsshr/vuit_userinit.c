/*  VAX/DEC CMS REPLACEMENT HISTORY, Element VUIT_USERINIT.C */
/*  *1    16-DEC-1991 16:44:47 TWF "Vuit user initialization" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element VUIT_USERINIT.C */
/*
 *++
 *  FACILITY:
 *
 *	VUIT - Visual User Interface Tool
 *
 *	This file provides the hook for VUIT users to link extended
 *	versions of VUIT containing the code for their own widgets. Each
 *	such widget must have call to the function MrmRegisterClass
 *	for each widget which you wish to have available in VUIT. This
 *	registration makes the widget creation function and class record
 *	accessible.
 *
 *	Users who wish to extend VUIT should replace this module with
 *	one defining a VuitUserInitialize procedure containing MrmRegisterClass
 *	calls for each widget they wish to add.
 *
 *	The last section of this file contains instructions for adding a
 *	user-defined widget to VUIT
 *
 */


/*
 *  INCLUDE FILES
 */
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <Xm/XmP.h>



/*
 *  DECLARATIONS
 *
 *	Place declarations of class records and creation functions here,
 *	or include files which provide these declarations
 */


void VuitUserInitialize ()

/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	An example version of the routine VUIT users may replace with
 *	one containing MrmRegisterClass calls for user-defined widgets
 *
 *  FORMAL PARAMETERS:
 *
 *  IMPLICIT INPUTS:
 *
 *      >
 *
 *  IMPLICIT OUTPUTS:
 *
 *      >
 *
 *  FUNCTION VALUE:
 *
 *      >
 *
 *  SIDE EFFECTS:
 *
 *      >
 *
 *  NOTES:
 *
 *	>
 *
 *--
 */

{

/*
 *  Local variables
 */

/*
 * Example: given a new widget class with the following creation
 *	function and class record:
 *		Creation function - CreateMyWidget (should have the
 *			the same semantics and calling sequence as, for
 *			instance, XmCreateLabel).
 *		Creation function name as declared in WML - "CreateMyWidget"
 *		Class record - myWidgetClassRec
 * then the MrmRegisterClass call would be:
    extern Widget CreateMyWidget (Widget Parent,
                                  char *name,
                                  ArgList arglist,
                                  Cardinal argcount);
    externalref <class_record> myWidgetClassRec;
    MrmRegisterClass (0, NULL, "CreateMyWidget", CreateMyWidget,
                      (WidgetClass)&myWidgetClassRec);
*/
   XmdsInitialize();
}



/*
 * The following summarizes the steps you need to take in order to
 * add a user-defined widget to VUIT.
 *
 * VUIT functions by creating instances of the actual widgets you request
 * in its work area. It uses information formatted for the Uil compiler
 * to acquire information about a widget's resources and legal children.
 * In order to make your widget accessible in VUIT, you need to make this
 * information available in a readable form, and create a new version of
 * VUIT containing the code which supports your widget. To do so, you
 * need to take the following steps:
 *	1. Create a Widget Metalanguage (WML) description of your widget and
 *	   its resources. The WML language is available as part of the
 *	   OSF Motif developers package. You should create your description
 *	   as an extended version of the WML description of the Uil
 *	   language you currently use. Its .wml file will help serve as
 *	   a guide for both syntax and contents. Note you must create entries
 *	   for each new resource defined by your widget.
 *	2. Create a Widget Metalanguage Database from your .wml file. The
 *	   result is a binary database (.wmd) file. The facilities for
 *	   doing this are contained in the DECwindows V3 developers package.
 *	   You will use this database both in VUIT and when compiling the
 *	   Uil files you create.
 *	3. Edit your copy of VUIT/vuit.dat to include your widget in the VUIT
 *	   parts box. You will need to include the widget in some category
 *	   in the parts box (or create a new one). Note the following:
 *		- You can specify an X bitmap file for the parts box icon
 *		  by putting the following line in your vuit.dat:
 *			vuit.partbox.<widget_name>.Xbitmapfile: <filename>.xbm
 *		- If you are based on the standard vuit.dat, your widget
 *		  will automatically receive XmNx and XmNy. If it should also
 *		  have XmNwidth and XmNheight, add the following line (see
 *		  for instance XmRadioBox):
 *			vuit.<widget_name>.ArgsNeeded: XmNwidth ...
 *			vuit.<widget_name>.XmNwidth.InitialValue: <value>
 *		  You should add resources as needed.
 *	4. Make your own version of this file (userinit.c), and place a call
 *	   in it to MrmRegisterClass for your widget.
 *	5. Build a new version of VUIT which links in your version of this
 *	   file, and the code which supports your widget.
 *	6. When running VUIT or the Uil compiler, use the command line
 *	   option which loads the binary database file you created in
 *	   step 2:
 *		VMS: $ VUIT/
 *		Ultrix/Unix: vuit -wmd <file>.wmd
 *	   Your widget will be available in the parts box, and will be
 *	   recognized in any Uil file.
 */
