/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*
 * *****************************************************************
 * *                                                               *
 * *    Copyright (c) Digital Equipment Corporation, 1991, 1996    *
 * *                                                               *
 * *   All Rights Reserved.  Unpublished rights  reserved  under   *
 * *   the copyright laws of the United States.                    *
 * *                                                               *
 * *   The software contained on this media  is  proprietary  to   *
 * *   and  embodies  the  confidential  technology  of  Digital   *
 * *   Equipment Corporation.  Possession, use,  duplication  or   *
 * *   dissemination of the software and media is authorized only  *
 * *   pursuant to a valid written license from Digital Equipment  *
 * *   Corporation.                                                *
 * *                                                               *
 * *   RESTRICTED RIGHTS LEGEND   Use, duplication, or disclosure  *
 * *   by the U.S. Government is subject to restrictions  as  set  *
 * *   forth in Subparagraph (c)(1)(ii)  of  DFARS  252.227-7013,  *
 * *   or  in  FAR 52.227-19, as applicable.                       *
 * *                                                               *
 * *****************************************************************
 */
/*
 * HISTORY
 */
/* 
 * (c) Copyright 1989, 1990, 1991, 1992, 1993 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/
/* 
 * Motif Release 1.2.3
*/
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$RCSfile$ $Revision$ $Date$"
#endif
#endif
/*
*  (c) Copyright 1989, 1990, DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASS. */
/*
 * This is the program creates binary databases from WML output.
 */
#include <stdio.h>
#ifndef DEC_MOTIF_BUG_FIX
#define DEC_MOTIF_BUG_FIX
#endif
#ifdef DEC_MOTIF_BUG_FIX
/*
 * DEC_MOTIF_BUG_FIX
 *
 * Part 1 of 1
 * Additional parts: None
 * Tracking info: None
 * Fixed by: RDJ
 * Date: 5/31/95
 *
 * MrmWidget.h is a private header file that doesn't ship with the kit.
 * Since we do ship wmldbcreate.c, we'll insert the contents of
 * MrmWidget.h directly into it at this point so that customers can
 * compile it without problems.
 */
#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>
#include <X11/CompositeP.h>
#include <X11/ConstrainP.h>
#include <X11/ShellP.h>
#include <X11/VendorP.h>
#include <X11/RectObjP.h>
#include <Mrm/MrmAppl.h>
#include <Xm/XmP.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/LabelGP.h>
#include <Xm/LabelP.h>
#include <Xm/BulletinB.h>
#include <Xm/BulletinBP.h>
#include <Xm/RowColumn.h>
#include <Xm/RowColumnP.h>
#include <Xm/ArrowB.h>
#include <Xm/ArrowBG.h>
#include <Xm/ArrowBGP.h>
#include <Xm/ArrowBP.h>
#include <Xm/AtomMgr.h>
#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>
#include <Xm/CascadeBGP.h>
#include <Xm/CascadeBP.h>
#include <Xm/SelectioBP.h>
#include <Xm/SelectioB.h>
#include <Xm/Command.h>
#include <Xm/CommandP.h>
#include <Xm/CutPaste.h>
#include <Xm/CutPasteP.h>
#include <Xm/DialogS.h>
#include <Xm/DialogSP.h>
#include <Xm/DrawingA.h>
#include <Xm/DrawingAP.h>
#include <Xm/DrawnB.h>
#include <Xm/DrawnBP.h>
#include <Xm/FileSB.h>
#include <Xm/FileSBP.h>
#include <Xm/Form.h>
#include <Xm/FormP.h>
#include <Xm/Frame.h>
#include <Xm/FrameP.h>
#include <Xm/List.h>
#include <Xm/ListP.h>
#include <Xm/MainW.h>
#include <Xm/MainWP.h>
#include <Xm/MenuShell.h>
#include <Xm/MenuShellP.h>
#include <Xm/MessageB.h>
#include <Xm/MessageBP.h>
#include <Xm/PanedW.h>
#include <Xm/PanedWP.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/PushBGP.h>
#include <Xm/PushBP.h>
#include <Xm/SashP.h>
#include <Xm/Scale.h>
#include <Xm/ScaleP.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrollBarP.h>
#include <Xm/ScrolledW.h>
#include <Xm/ScrolledWP.h>
#include <Xm/SeparatoG.h>
#include <Xm/SeparatoGP.h>
#include <Xm/Separator.h>
#include <Xm/SeparatorP.h>
#include <Xm/Text.h>
#include <Xm/TextP.h>
#include <Xm/TextInP.h>
#include <Xm/TextOutP.h>
#include <Xm/TextStrSoP.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/ToggleBGP.h>
#include <Xm/ToggleBP.h>
#else
#include <Mrm/MrmWidget.h>
#endif				/* DEC_MOTIF_BUG_FIX */
#include <Xm/Xm.h>
#include <Xm/MwmUtil.h>
#ifdef DEC_MOTIF_EXTENSION
#include <DXm/DXmHelpB.h>
#include <DXm/DXmColor.h>
#include <DXm/DXmPrint.h>
#include <DXm/DXmCSText.h>
#include <DXm/DXmSvn.h>
#endif
/*
 * User supplied widget public.h and/or private.h files are to be added here
 * #ifdef for automatic use in uilwmdcreate script file
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
#include "UilDBDef.h"
#include "UilLexPars.h"
#include "UilSymGen.h"		/* from WML */
#include "UilSymArTy.h"		/* from WML */
#include "UilSymRArg.h"		/* from WML */
#include "UilSymArTa.h"		/* from WML */
#include "UilSymReas.h"		/* from WML */
#include "UilSymCtl.h"		/* from WML */
#include "UilConst.h"		/* from WML */
#include "UilSymNam.h"		/* from WML */
#include "UilSymEnum.h"		/* from WML */
#include "UilSymCSet.h"		/* from WML */
#include "UilUrmClas.h"		/* from WML */
#include "UilKeyTab.h"		/* from WML */
#include "UilSymChCl.h"		/* from WML */
#include "UilSymChTa.h"		/* from WML */
    FILE * bfile, *afile;
int DEBUG = FALSE;
char outfilename[80];
char debugfilename[80];

main(argc, argv)
int argc;
char **argv;
{
  _db_header header;

  strcpy(outfilename, "motif.wmd");
  strcpy(debugfilename, "motif.dbg");

  for (argc--, argv++; argc; argc--, argv++) {
    if (strcmp("-debug", *argv) == 0) {
      DEBUG = TRUE;
    } else if ((strcmp("-o", *argv) == 0)) {
      strcpy(outfilename, argv[1]);
    }
  }

  bfile = fopen(outfilename, "w");
  if (bfile == (FILE *) NULL) {
    printf("\nCouldnt't open %s", outfilename);
    return;
  }
  if (DEBUG) {
    afile = fopen(debugfilename, "w");
    if (afile == (FILE *) NULL) {
      printf("\nCouldn't open %s", debugfilename);
      return;
    }
  }

  emit_globals();

/*
 *   UilConst
 *   UilSymArty
 *   UilSymCset - parts of it
 *   UilSymChCl
 */
  emit_chars(Constraint_Tab);
  emit_chars(Argument_Type_Table_Value);
  emit_chars(Charset_Wrdirection_Table);
  emit_chars(Charset_Parsdirection_Table);
  emit_chars(Charset_Charsize_Table);
  emit_chars(Child_Class_Table);
/*
 *    UilKeyTab
 */
  emit_ints_and_string(Key_Table);
  emit_ints_and_string(Key_Table_Case_Ins);
/*
 *   UilSymArTa
 *   UilSymCtl
 *   UilSymReas
 *   UilSymChTa
 */
  emit_char_table(Allowed_Argument_Table);
  emit_char_table(Allowed_Control_Table);
  emit_char_table(Allowed_Reason_Table);
  emit_char_table(Allowed_Child_Table);
/*
 *   UilSymCset
 *   UilSymNam
 *   UilUrmClas
 */
  emit_length_and_string(Charset_Xmstring_Names_Table);
  emit_length_and_string(Charset_Lang_Names_Table);
  emit_length_and_string(Uil_Widget_Names);
  emit_length_and_string(Uil_Argument_Names);
  emit_length_and_string(Uil_Reason_Names);
  emit_length_and_string(Uil_Enumval_names);
  emit_length_and_string(Uil_Charset_Names);
  emit_length_and_string(Uil_Widget_Funcs);
  emit_length_and_string(Uil_Argument_Toolkit_Names);
  emit_length_and_string(Uil_Reason_Toolkit_Names);
  emit_length_and_string(Uil_Children_Names);
/*
 *   UilSymCset
 *   UilSymEnum
 *   UilSymRArg
 *   UilUrmClas
 */
  emit_shorts(Charset_Lang_Codes_Table);
  emit_shorts(Argument_Enum_Set_Table);
  emit_shorts(Related_Argument_Table);
  emit_shorts(Uil_Gadget_Funcs);
  emit_shorts(Uil_Urm_Nondialog_Class);
  emit_shorts(Uil_Urm_Subtree_Resource);
/*
 *   UilSymEnum
 */
  emit_int_and_table_shorts(Enum_Set_Table);
/*
 *   UilSymEnum
 */
  emit_ints(Enumval_Values_Table);

  exit(0);
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
  globals.uil_max_child = uil_max_child;

  fwrite(&globals, sizeof(_db_globals), 1, bfile);
  if (DEBUG)
    fprintf(afile, "%d %d %d %d %d %d %d %d %d %d ", globals.version,
	    globals.uil_max_arg, globals.uil_max_charset,
	    globals.charset_lang_table_max, globals.uil_max_object,
	    globals.uil_max_reason, globals.uil_max_enumval,
	    globals.uil_max_enumset, globals.key_k_keyword_count, globals.key_k_keyword_max_length);
}

emit_header(header)
_db_header_ptr header;
{

  fwrite(header, sizeof(_db_header), 1, bfile);
  if (DEBUG)
    fprintf(afile,
	    "\n\nTableId=%d, NumEntries=%d, TableSize=%d \n",
	    header->table_id, header->num_items, header->table_size);
}

emit_chars(table_id)
int table_id;
{
  _db_header header;
  unsigned char *ptr;
  int i;

  switch (table_id) {
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
  case Child_Class_Table:
    /*
     * NOTE: The first entry is not used but we copy it anyway
     */
    header.table_size = sizeof(child_class_table_vec);
    header.num_items = header.table_size;
    header.table_id = Child_Class_Table;
    ptr = child_class_table;
    break;
  }

  emit_header(&header);

  fwrite(ptr, header.table_size, 1, bfile);
  if (DEBUG) {
    for (i = 0; i <= header.num_items; i++) {
      fprintf(afile, "%d ", ptr[i]);
    }
  }
}

emit_ints_and_string(table_id)
int table_id;
{
  _db_header header;
  key_keytable_entry_type *table;
  int i;

  switch (table_id) {
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

  fwrite(table, header.table_size, 1, bfile);
  for (i = 0; i < header.num_items; i++) {
    fwrite(table[i].at_name, table[i].b_length + 1, 1, bfile);
    if (DEBUG)
      fprintf(afile, "%d %d %d %d %s", table[i].b_class, table[i].b_subclass,
	      table[i].b_length, table[i].b_token, table[i].at_name);
  }

}

emit_char_table(table_id)
int table_id;
{
  unsigned char **table;
  _db_header header;
  unsigned char *entry_vec;
  int i, j;
  int num_bits = (uil_max_object + 7) / 8;

  switch (table_id) {
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
  case Allowed_Child_Table:
    header.table_size = sizeof(allowed_child_table_vec);
    header.num_items = uil_max_child;
    header.table_id = Allowed_Child_Table;
    table = allowed_child_table;
    break;
  }

  emit_header(&header);

  for (i = 1; i <= header.num_items; i++) {	/* First not used */
    entry_vec = table[i];
    fwrite(entry_vec, sizeof(char) * num_bits, 1, bfile);
    if (DEBUG) {
      for (j = 0; j < num_bits; j++) {
	fprintf(afile, "%d, ", entry_vec[j]);
      }
      fprintf(afile, "\n");
    }
  }
}

emit_length_and_string(table_id)
int table_id;
{
  _db_header header;
  int *lengths;
  char *string_table;
  char **table;
  int i;

  switch (table_id) {
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
    table = uil_widget_names;
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
  case Uil_Children_Names:
    header.table_size = sizeof(uil_child_names_vec);
    header.num_items = uil_max_child;
    header.table_id = Uil_Children_Names;
    table = uil_child_names;
    break;
  }

  emit_header(&header);

  lengths = (int *)malloc(sizeof(int) * (header.num_items + 1));

  for (i = 0; i <= header.num_items; i++) {
    if (table[i] != NULL) {
      /*
       * Add one to the length for the null terminator
       */
      lengths[i] = strlen(table[i]) + 1;
    } else {
      lengths[i] = 0;
    }
    if (DEBUG)
      fprintf(afile, "%d ", lengths[i]);
  }
  fwrite(lengths, sizeof(int) * (header.num_items + 1), 1, bfile);
  for (i = 0; i <= header.num_items; i++) {
    if (lengths[i]) {
      /*
       * assumed lengths[i] = lengths[i] * sizeof(char)
       * Add one for the null terminator
       */
      fwrite(table[i], lengths[i] + 1, 1, bfile);
      if (DEBUG)
	fprintf(afile, "%s ", table[i]);
    }
  }
  free(lengths);
}

emit_shorts(table_id)
int table_id;
{
  _db_header header;
  unsigned short int *ptr;
  int i;

  switch (table_id) {
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

  fwrite(ptr, header.table_size, 1, bfile);
  if (DEBUG) {
    for (i = 0; i < header.num_items; i++) {
      fprintf(afile, "%d ", ptr[i]);
    }
  }
}

emit_int_and_table_shorts(table_id)
int table_id;
{
  _db_header header;
  UilEnumSetDescDef *table;
  int j, i;
  unsigned short int *value_vec;

  switch (table_id) {
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
  fwrite(table, header.table_size, 1, bfile);
  for (i = 0; i <= header.num_items; i++) {	/* first is not used */
    if (table[i].values_cnt) {
      fwrite(table[i].values, sizeof(short) * table[i].values_cnt, 1, bfile);
    }
  }
}

emit_ints(table_id)
int table_id;
{
  _db_header header;
  int *ptr;
  int i;

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

  fwrite(ptr, header.table_size, 1, bfile);
  if (DEBUG) {
    for (i = 0; i < header.num_items; i++) {
      fprintf(afile, "%d ", ptr[i]);
    }
  }
}
