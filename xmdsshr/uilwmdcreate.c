/*  CMS REPLACEMENT HISTORY, Element UILWMDCREATE.C */
/*  *23   10-AUG-1995 09:05:45 TWF "Fix DEC bug" */
/*  *22   25-MAR-1994 14:09:48 TWF "Use system version" */
/*  *21   22-FEB-1994 15:18:01 TWF "remove NO_X_GBLS" */
/*  *20   11-MAR-1993 13:59:24 TWF "remove sys/" */
/*  *19    2-MAR-1993 14:21:43 JAS "port to decc" */
/*  *18    2-MAR-1993 13:54:29 JAS "port to decc" */
/*  *17    2-MAR-1993 12:51:12 JAS "port to decc" */
/*  *16   26-JAN-1993 14:01:20 JAS "change ref to usagedef" */
/*  *15   19-JAN-1993 09:42:34 JAS "adding xmdsdisplay" */
/*  *14   21-SEP-1992 13:21:16 TWF "Upgrade to motif 1.1" */
/*  *13    6-MAY-1992 15:35:08 JAS "forgot one include" */
/*  *12   10-MAR-1992 11:06:55 JAS "need onofftogglebutton include file" */
/*  *11    6-FEB-1992 11:37:29 TWF "Remove XmdsNapplyProc" */
/*  *10    6-FEB-1992 11:22:15 TWF "Add XmdsNapplyProc to ApplyButton" */
/*  *9    28-JAN-1992 16:12:14 TWF "Add NidOptionMenu widge" */
/*  *8    16-JAN-1992 13:34:43 JAS "Adding ExprField" */
/*  *7    16-JAN-1992 11:37:24 JAS "adding Xdbox and xdboxdialog" */
/*  *6     7-JAN-1992 15:15:06 TWF "Add XmdsDigChans" */
/*  *5    18-DEC-1991 10:57:28 TWF "Add Wavedraw widget" */
/*  *4     5-DEC-1991 16:26:24 TWF "Need usagedef" */
/*  *3     5-DEC-1991 15:34:58 TWF "Add Expr widget" */
/*  *2     8-OCT-1991 17:04:36 TWF "Fix include" */
/*  *1     8-OCT-1991 15:41:45 TWF "Template program for generating XMDSUIL.WMD" */
/*  CMS REPLACEMENT HISTORY, Element UILWMDCREATE.C */
#ifdef REV_INFO
#ifndef lint
static char SCCSID[] = "OSF/Motif: %W% %E%";
#endif /* lint */
#endif /* REV_INFO */
/******************************************************************************
*******************************************************************************
*
*  (c) Copyright 1989, 1990, OPEN SOFTWARE FOUNDATION, INC.
*  (c) Copyright 1989, 1990, DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASS.
*  ALL RIGHTS RESERVED
*  
*  	THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED
*  AND COPIED ONLY IN ACCORDANCE WITH THE TERMS OF SUCH LICENSE AND
*  WITH THE INCLUSION OF THE ABOVE COPYRIGHT NOTICE.  THIS SOFTWARE OR
*  ANY OTHER COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE
*  AVAILABLE TO ANY OTHER PERSON.  NO TITLE TO AND OWNERSHIP OF THE
*  SOFTWARE IS HEREBY TRANSFERRED.
*  
*  	THE INFORMATION IN THIS SOFTWARE IS SUBJECT TO CHANGE WITHOUT
*  NOTICE AND SHOULD NOT BE CONSTRUED AS A COMMITMENT BY OPEN SOFTWARE
*  FOUNDATION, INC. OR ITS THIRD PARTY SUPPLIERS  
*  
*  	OPEN SOFTWARE FOUNDATION, INC. AND ITS THIRD PARTY SUPPLIERS,
*  ASSUME NO RESPONSIBILITY FOR THE USE OR INABILITY TO USE ANY OF ITS
*  SOFTWARE .   OSF SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
*  KIND, AND OSF EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES, INCLUDING
*  BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
*  FITNESS FOR A PARTICULAR PURPOSE.
*  
*  Notice:  Notwithstanding any other lease or license that may pertain to,
*  or accompany the delivery of, this computer software, the rights of the
*  Government regarding its use, reproduction and disclosure are as set
*  forth in Section 52.227-19 of the FARS Computer Software-Restricted
*  Rights clause.
*  
*  (c) Copyright 1989, 1990, Open Software Foundation, Inc.  Unpublished - all
*  rights reserved under the Copyright laws of the United States.
*  
*  RESTRICTED RIGHTS NOTICE:  Use, duplication, or disclosure by the
*  Government is subject to the restrictions as set forth in subparagraph
*  (c)(1)(ii) of the Rights in Technical Data and Computer Software clause
*  at DFARS 52.227-7013.
*  
*  Open Software Foundation, Inc.
*  11 Cambridge Center
*  Cambridge, MA   02142
*  (617)621-8700
*  
*  RESTRICTED RIGHTS LEGEND:  This computer software is submitted with
*  "restricted rights."  Use, duplication or disclosure is subject to the
*  restrictions as set forth in NASA FAR SUP 18-52.227-79 (April 1985)
*  "Commercial Computer Software- Restricted Rights (April 1985)."  Open
*  Software Foundation, Inc., 11 Cambridge Center, Cambridge, MA  02142.  If
*  the contract contains the Clause at 18-52.227-74 "Rights in Data General"
*  then the "Alternate III" clause applies.
*  
*  (c) Copyright 1989, 1990, Open Software Foundation, Inc.
*  ALL RIGHTS RESERVED 
*  
*  
* Open Software Foundation is a trademark of The Open Software Foundation, Inc.
* OSF is a trademark of Open Software Foundation, Inc.
* OSF/Motif is a trademark of Open Software Foundation, Inc.
* Motif is a trademark of Open Software Foundation, Inc.
* DEC is a registered trademark of Digital Equipment Corporation
* DIGITAL is a registered trademark of Digital Equipment Corporation
* X Window System is a trademark of the Massachusetts Institute of Technology
*
*******************************************************************************
******************************************************************************/

/*
 * This is the program creates binary databases from WML output.
 */


#include <stdio.h>
#include <sys/file.h>

#ifdef VAX
#include <DECw$Include:Mrm.h> 
#include <DECw$Include:Xm.h> 
#else
#include <Mrm/MrmWidget.h> 
#include <Xm/Xm.h> 
#endif

#ifdef DEC_MOTIF_EXTENSION
#ifdef VAX
#include <DECw$Include:DXmHelpB.h>
#include <DECw$Include:DXmColor.h>
#include <DECw$Include:DXmPrint.h>
#include <DECw$Include:DXmCSText.h>
#include <DECw$Include:DXmSvn.h>
#else
#include <DXm/DXmHelpB.h>
#include <DXm/DXmColor.h>
#include <DXm/DXmPrint.h>
#include <DXm/DXmCSText.h>
#include <DXm/DXmSvn.h>
#endif
#endif

/*
 * User supplied widget public.h and/or private.h files are to be added here
 */
#ifdef DGIT
#include "DGITGobe.h"
#include "DGITNeted.h"
#endif

#ifdef MDS_EXTENSION
#include <usagedef.h>
#include <Xmds/XmdsDigChans.h>
#include <Xmds/XmdsExpr.h>
#include <Xmds/XmdsExprField.h>
#include <Xmds/XmdsNidOptionMenu.h>
#include <Xmds/XmdsOnOffToggleButton.h>
#include <Xmds/XmdsWavedraw.h>
#include <Xmds/XmdsWaveform.h>
#include <Xmds/XmdsXdBox.h>
#include <Xmds/XmdsPath.h>
#include <Xmds/XmdsDisplay.h>
#endif
/*
 * End of User supplied widget files
 */

#ifdef VMS
#include "DECw$Include:UilDBDef.h"
#include "DECw$Include:UilLexPars.h"
#else
#include "UilDBDef.h"
#include "UilLexPars.h"
#endif

#include "UilSymGen.h" /* from WML */
#include "UilSymArTy.h" /* from WML */
#include "UilSymRArg.h" /* from WML */
#include "UilSymArTa.h" /* from WML */
#include "UilSymReas.h" /* from WML */
#include "UilSymCtl.h" /* from WML */
#include "UilConst.h" /* from WML */
#include "UilSymNam.h" /* from WML */
#include "UilSymEnum.h" /* from WML */
#include "UilSymCSet.h" /* from WML */
#include "UilUrmClas.h" /* from WML */
#include "UilKeyTab.h"  /* from WML */

FILE *bfile, *afile;
int DEBUG=FALSE;
char outfilename[80];
char debugfilename[80];

main(argc, argv)
int argc;
char **argv;
{
    _db_header	header;

#ifdef VMS
    strcpy(outfilename, "DECW$UIL_DXM_MOTIF.WMD");
    strcpy(debugfilename, "DECW$UIL_DXM_MOTIF.DBG");
#else
    strcpy(outfilename, "motif.wmd");
    strcpy(debugfilename, "motif.dbg");
#endif


    for (argc--, argv++; argc; argc--, argv++)
	{
	if (strcmp("-debug", *argv) == 0) 
	    {
	    DEBUG=TRUE;
	    }
	else if ((strcmp("-o", *argv) == 0))
		 {
		 strcpy(outfilename, argv[1]);
		 }
	}

    bfile = fopen(outfilename, "w");
    if (DEBUG)
	afile = fopen(debugfilename, "w");

    emit_globals();

/*
 *   UilConst
 *   UilSymArty
 *   UilSymCset - parts of it
 */
    emit_chars(Constraint_Tab);
    emit_chars(Argument_Type_Table_Value);
    emit_chars(Charset_Wrdirection_Table);
    emit_chars(Charset_Parsdirection_Table);
    emit_chars(Charset_Charsize_Table);
/*
 *    UilKeyTab
 */
    emit_ints_and_string (Key_Table);
    emit_ints_and_string (Key_Table_Case_Ins);
/*
 *   UilSymArTa
 *   UilSymCtl
 *   UilSymReas
 */
    emit_char_table (Allowed_Argument_Table);
    emit_char_table (Allowed_Control_Table);
    emit_char_table (Allowed_Reason_Table);
/*
 *   UilSymCset
 *   UilSymNam
 *   UilUrmClas
 */
    emit_length_and_string (Charset_Xmstring_Names_Table);
    emit_length_and_string (Charset_Lang_Names_Table);
    emit_length_and_string (Uil_Widget_Names);
    emit_length_and_string (Uil_Argument_Names);
    emit_length_and_string (Uil_Reason_Names);
    emit_length_and_string (Uil_Enumval_names);
    emit_length_and_string (Uil_Charset_Names);
    emit_length_and_string (Uil_Widget_Funcs);
    emit_length_and_string (Uil_Argument_Toolkit_Names);
    emit_length_and_string (Uil_Reason_Toolkit_Names);
/*
 *   UilSymCset
 *   UilSymEnum
 *   UilSymRArg
 *   UilUrmClas
 */
    emit_shorts (Charset_Lang_Codes_Table);
    emit_shorts (Argument_Enum_Set_Table);
    emit_shorts (Related_Argument_Table);
    emit_shorts (Uil_Gadget_Funcs);
    emit_shorts (Uil_Urm_Nondialog_Class);
    emit_shorts (Uil_Urm_Subtree_Resource);
/*
 *   UilSymEnum
 */
    emit_int_and_table_shorts(Enum_Set_Table);
/*
 *   UilSymEnum
 */
    emit_ints (Enumval_Values_Table);

    exit (0);
}


emit_globals()
{
    _db_globals globals;

    globals.version = DB_Compiled_Version;
    globals.uil_max_arg = uil_max_arg;
    globals.uil_max_charset = uil_max_charset;
    globals.charset_lang_table_max = charset_lang_table_max;
    globals.uil_max_object = uil_max_object;
    globals.uil_max_reason = uil_max_reason;
    globals.uil_max_enumval = uil_max_enumval;
    globals.uil_max_enumset = uil_max_enumset;
    globals.key_k_keyword_count = key_k_keyword_count;
    globals.key_k_keyword_max_length = key_k_keyword_max_length;
/*
    globals.uil_max_child = uil_max_child;
*/
    fwrite (&globals, sizeof (_db_globals), 1, bfile);
    if (DEBUG)
	fprintf(afile, "%d %d %d %d %d %d %d %d %d %d ", globals.version,
		globals.uil_max_arg, globals.uil_max_charset, 
		globals.charset_lang_table_max, globals.uil_max_object,
		globals.uil_max_reason, globals.uil_max_enumval, 
		globals.uil_max_enumset, globals.key_k_keyword_count,
		globals.key_k_keyword_max_length);
    }



emit_header(header)
_db_header_ptr header;
{

    fwrite (header, sizeof(_db_header), 1, bfile);
    if (DEBUG)
	fprintf(afile, 
		"\n\nTableId=%d, NumEntries=%d, TableSize=%d \n",
		 header->table_id, header->num_items, header->table_size);
    }



emit_chars(table_id)
    int	    table_id;
{
    _db_header	    header;
    unsigned char   *ptr;
    int		    i;

    switch (table_id)
	{
	case Constraint_Tab:
	    /*
	     * NOTE: The first entry is not used but we copy it anyway
	     */
	    header.table_size = sizeof(constraint_tab_vec);
	    header.num_items = header.table_size;
	    header.table_id = Constraint_Tab;
	    ptr = constraint_tab;
	    break;
	case Argument_Type_Table_Value:
	    /*
	     * NOTE: The first entry is not used but we copy it anyway
	     */
	    header.table_size = sizeof(argument_type_table_vec);
	    header.num_items = header.table_size;
	    header.table_id = Argument_Type_Table_Value;
	    ptr = argument_type_table;
	    break;
	case Charset_Wrdirection_Table:
	    /*
	     * NOTE: The first entry is not used but we copy it anyway
	     */
	    header.table_size = sizeof(charset_wrdirection_table_vec);
	    header.num_items = header.table_size;
	    header.table_id = Charset_Wrdirection_Table;
	    ptr = charset_writing_direction_table;
	    break;
	case Charset_Parsdirection_Table:
	    /*
	     * NOTE: The first entry is not used but we copy it anyway
	     */
	    header.table_size = sizeof(charset_parsdirection_table_vec);
	    header.num_items = header.table_size;
	    header.table_id = Charset_Parsdirection_Table;
	    ptr = charset_parsing_direction_table;
	    break;
	case Charset_Charsize_Table:
	    /*
	     * NOTE: The first entry is not used but we copy it anyway
	     */
	    header.table_size = sizeof(charset_charsize_table_vec);
	    header.num_items = header.table_size;
	    header.table_id = Charset_Charsize_Table;
	    ptr = charset_character_size_table;
	    break;
	}

    emit_header(&header);

    fwrite (ptr, header.table_size, 1, bfile);  
    if (DEBUG)
	{
	for (i=0; i<=header.num_items; i++)
	    {
	    fprintf(afile, "%d ", ptr[i]);
	    }
	}
}


emit_ints_and_string(table_id)
    int	    table_id;
{
    _db_header		    header;
    key_keytable_entry_type *table;
    int			    i;

    switch (table_id)
	{
	/*
	 * All tables are zero based unless otherwise noted
	 */
	case Key_Table:
	    header.table_size = sizeof(key_table_vec);
	    header.num_items = key_k_keyword_count;
	    header.table_id = Key_Table;
	    table = key_table;
	    break;
	case Key_Table_Case_Ins:
	    header.table_size = sizeof(key_table_case_ins_vec);
	    header.num_items = key_k_keyword_count;
	    header.table_id = Key_Table_Case_Ins;
	    table = key_table_case_ins;
	    break;
	}
    emit_header(&header);

    fwrite (table, header.table_size, 1, bfile);
    for (i=0; i<header.num_items; i++)
        {
	fwrite (table[i].at_name, table[i].b_length + 1, 1, bfile);
	if (DEBUG)
	    fprintf (afile, "%d %d %d %d %s", table[i].b_class, table[i].b_subclass,
		 table[i].b_length, table[i].b_token, table[i].at_name);
	}
	       
}


emit_char_table(table_id)
int	table_id;
{
    unsigned char **table;
    _db_header header;
    unsigned char *entry_vec;
    int i, j;
    int num_bits = (uil_max_object + 7) / 8;

    switch (table_id)
	{
	/*
	 * All tables are 1 based unless otherwise specified
	 */
	case Allowed_Argument_Table:
	    header.table_size = sizeof(allowed_argument_table_vec);
	    header.num_items = uil_max_arg;
	    header.table_id = Allowed_Argument_Table;
	    table = allowed_argument_table;
	    break;
	case Allowed_Control_Table:
	    header.table_size = sizeof(allowed_control_table_vec);
	    header.num_items = uil_max_object;
	    header.table_id = Allowed_Control_Table;
	    table = allowed_control_table;
	    break;
	case Allowed_Reason_Table:
	    header.table_size = sizeof(allowed_reason_table_vec);
	    header.num_items = uil_max_reason;
	    header.table_id = Allowed_Reason_Table;
	    table = allowed_reason_table;
	    break;
	}

    emit_header(&header);

    for (i=1; i<=header.num_items; i++)   /* First not used */
        {
        entry_vec = table[i];
	fwrite (entry_vec, sizeof (char) * num_bits, 1, bfile);
	if (DEBUG)
	    {
	    for (j=0; j<num_bits; j++)
		{
		fprintf (afile, "%d, ", entry_vec[j]);
		}
	    fprintf (afile, "\n");
	    }
        }
}


emit_length_and_string(table_id)
int	table_id;
{
    _db_header	header;
    int		*lengths;
    char	*string_table;
    char	**table;
    int		i;

    switch (table_id)
	{
	/*
	 * all the tables are 1 based unless otherwise documented
	 */
	case Charset_Xmstring_Names_Table:
	    header.table_size = sizeof(charset_xmstring_names_table_vec);
	    header.num_items = uil_max_charset;
	    header.table_id = Charset_Xmstring_Names_Table;
	    table = charset_xmstring_names_table;
	    break;
	case Charset_Lang_Names_Table:
	    /*
	     * This table is 0 based
	     */
	    header.table_size = sizeof(charset_lang_names_table_vec);
	    header.num_items = charset_lang_table_max - 1;
	    header.table_id = Charset_Lang_Names_Table;
	    table = charset_lang_names_table;
	    break;
	case Uil_Widget_Names:
	    header.table_size = sizeof(uil_widget_names_vec);
	    header.num_items = uil_max_object;
	    header.table_id = Uil_Widget_Names;
	    table = uil_widget_names ;
	    break;
	case Uil_Argument_Names:
	    header.table_size = sizeof(uil_argument_names_vec);
	    header.num_items = uil_max_arg;
	    header.table_id = Uil_Argument_Names;
	    table = uil_argument_names;
	    break;
	case Uil_Reason_Names:
	    header.table_size = sizeof(uil_reason_names_vec);
	    header.num_items = uil_max_reason;
	    header.table_id = Uil_Reason_Names;
	    table = uil_reason_names;
	    break;
	case Uil_Enumval_names:
	    header.table_size = sizeof(uil_enumval_names_vec);
	    header.num_items = uil_max_enumval;
	    header.table_id = Uil_Enumval_names;
	    table = uil_enumval_names;
	    break;
	case Uil_Charset_Names:
	    header.table_size = sizeof(uil_charset_names_vec);
	    header.num_items = uil_max_charset;
	    header.table_id = Uil_Charset_Names;
	    table = uil_charset_names;
	    break;
	case Uil_Widget_Funcs:
	    header.table_size = sizeof(uil_widget_funcs_vec);
	    header.num_items = uil_max_object;
	    header.table_id = Uil_Widget_Funcs;
	    table = uil_widget_funcs;
	    break;
	case Uil_Argument_Toolkit_Names:
	    header.table_size = sizeof(uil_argument_toolkit_names_vec);
	    header.num_items = uil_max_arg;
	    header.table_id = Uil_Argument_Toolkit_Names;
	    table = uil_argument_toolkit_names;
	    break;
	case Uil_Reason_Toolkit_Names:
	    header.table_size = sizeof(uil_reason_toolkit_names_vec);
	    header.num_items = uil_max_reason;
	    header.table_id = Uil_Reason_Toolkit_Names;
	    table = uil_reason_toolkit_names;
	    break;
	}

    emit_header(&header);

#ifdef DEC_MOTIF_BUG_FIX
    lengths = (int *) malloc (sizeof (int) * (header.num_items + 1));
#else
    lengths = (int *) malloc (sizeof (int) * header.num_items + 1);
#endif
 
    for (i=0; i<=header.num_items; i++)
	{
	if (table[i] != NULL)
	    {
	    /*
	     * Add one to the length for the null terminator
	     */
	    lengths[i] = strlen(table[i]) + 1;
	    }
	else
	    {
	    lengths[i] = 0;
	    }
	if (DEBUG)
	    fprintf (afile, "%d ", lengths[i]);
	}
#ifdef DEC_MOTIF_BUG_FIX
    fwrite (lengths, sizeof (int) * (header.num_items + 1), 1, bfile);
#else
    fwrite (lengths, sizeof (int) * header.num_items + 1, 1, bfile);
#endif
    for (i=0; i<=header.num_items; i++)
	{
	if (lengths[i])
	    {
	    /*
	     * assumed lengths[i] = lengths[i] * sizeof(char)
	     * Add one for the null terminator
	     */
	    fwrite (table[i], lengths[i] + 1, 1, bfile);
	    if (DEBUG)
		fprintf (afile, "%s ", table[i]);
	    }
	}
    free (lengths);
}


emit_shorts(table_id)
    int	    table_id;
{
    _db_header		header;
    unsigned short int	*ptr;
    int			i;

    switch (table_id)
	{
	/* 
	 * All tables are 1 based unless otherwise noted
	 */
	case Charset_Lang_Codes_Table:
	    /*
	     * 0 based table
	     */
	    header.table_size = sizeof(charset_lang_codes_table_vec);
	    header.num_items = charset_lang_table_max - 1;
	    header.table_id = Charset_Lang_Codes_Table;
	    ptr = charset_lang_codes_table;
	    break;
	case Argument_Enum_Set_Table:
	    header.table_size = sizeof(argument_enumset_table_vec);
	    header.num_items = uil_max_arg;
	    header.table_id = Argument_Enum_Set_Table;
	    ptr = argument_enumset_table;
	    break;
	case Related_Argument_Table:
	    header.table_size = sizeof(related_argument_table_vec);
	    header.num_items = uil_max_arg;
	    header.table_id = Related_Argument_Table;
	    ptr = related_argument_table;
	    break;
	case Uil_Gadget_Funcs:
	    header.table_size = sizeof(uil_gadget_variants_vec);
	    header.num_items = uil_max_object;
	    header.table_id = Uil_Gadget_Funcs;
	    ptr = uil_gadget_variants;
	    break;
	case Uil_Urm_Nondialog_Class:
	    header.table_size = sizeof(uil_urm_nondialog_class_vec);
	    header.num_items = uil_max_object;
	    header.table_id = Uil_Urm_Nondialog_Class;
	    ptr = uil_urm_nondialog_class;
	    break;
	case Uil_Urm_Subtree_Resource:
	    header.table_size = sizeof(uil_urm_subtree_resource_vec);
	    header.num_items = uil_max_object;
	    header.table_id = Uil_Urm_Subtree_Resource;
	    ptr = uil_urm_subtree_resource;
	    break;
	}

    emit_header(&header);

    fwrite (ptr, header.table_size, 1, bfile);  
    if (DEBUG)
	{
	for (i=0; i<header.num_items; i++)
	    {
	    fprintf(afile, "%d ", ptr[i]);
	    }
	}
}


emit_int_and_table_shorts(table_id)
    int	    table_id;
{
    _db_header		header;
    UilEnumSetDescDef	*table;
    int			j, i;
    unsigned short int	*value_vec;

    switch (table_id)
	{
	/*
	 * All tables are 1 based unless otherwise noted
	 */
	case Enum_Set_Table:
	    header.table_size = sizeof(enum_set_table_vec);
	    header.num_items = uil_max_enumset;
	    header.table_id = Enum_Set_Table;
	    table = enum_set_table;
	    break;
	}

    emit_header(&header);
    fwrite (table, header.table_size, 1, bfile);
    for (i=0; i<=header.num_items; i++) /* first is not used */
        {
	if (table[i].values_cnt)
	    {
	    fwrite (table[i].values, sizeof (short) * table[i].values_cnt, 1, bfile);
	    }
        }
}


emit_ints(table_id)
    int	    table_id;
{
    _db_header	header;
    int		*ptr;
    int		i;

    switch (table_id)
	/*
	 * all tables are 1 based unless otherwise noted
	 */
	{
	case Enumval_Values_Table:
	    header.table_size = sizeof(enumval_values_table_vec);
	    header.num_items = uil_max_enumval;
	    header.table_id = Enumval_Values_Table;
	    ptr = enumval_values_table;
	    break;
	}

    emit_header(&header);

    fwrite (ptr, header.table_size, 1, bfile);  
    if (DEBUG)
	{
	for (i=0; i<header.num_items; i++)
	    {
	    fprintf(afile, "%d ", ptr[i]);
	    }
	}
}

