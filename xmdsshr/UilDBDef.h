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
 * (c) Copyright 1989, 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * Motif Release 1.2
 */
/*   $RCSfile$ $Revision$ $Date$ */

/*
 *  (c) Copyright 1989, 1990, DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASS. */

/*
**++
**  FACILITY:
**
**      User Interface Language Compiler (UIL)
**
**  ABSTRACT:
**
**      This include file defines the interfaces necessry to the binary
**	data base in the WML & UIL compilers.
**
**--
**/

#ifndef UilDBDef_h
#define UilDBDef_h

/***********************************************************************
****************** From UilDef.h ***************************************/
#define _BIT_INDEX(_type) (((unsigned int)(_type)) >> 3)

#define _BIT_MASK(_type) (1 << (((unsigned int)(_type)) & 0x7))

#define _BIT_SET(table_entry_addr, _type) \
  (_BIT_MASK(_type) &                     \
   (((unsigned char *)(table_entry_addr))[_BIT_INDEX(_type)]))

/*******************************************************************
**************** From UilSymDef.h *********************************/
/*
**  Hash table size
*/

#define sym_k_hash_table_limit 127

/*
**  Symbol tags.  These values are used in the tag field of every object header
**  to identify the type of the entry.
*/

#define sym_k_error_entry (127)
/*  An error entry is used when compilation errors are detected in the  */
/*  source program.  Its use reduces cascading errors caused by missing */
/*  information and allows the compilation to continue and thus         */
/*  additional errors may be detected                                   */
#define sym_k_value_entry 1
/*  A value entry contains information about a UIL literal value        */
/*  (integer, string, compound string, xbitmapfile, argument, color,    */
/*  icon, etc.).  The b_type field is used to determine the datatype of */
/*  this value.                                                         */
#define sym_k_name_entry 2
/*  A name entry corresponds to a name (identifier) used in the UIL     */
/*  source.                                                             */
#define sym_k_widget_entry 3
/*  A widget entry represents a widget declaration.  It points off to   */
/*  the arguments, callbacks, and controls lists for the widget.        */
#define sym_k_control_entry 4
/*  A control entry hangs off a list entry and identifes an object      */
/*  (widget or gadget) that is controlled.  It also contains the        */
/*  managed/unmanaged information.                                      */
#define sym_k_forward_ref_entry 5
/*  forward reference entries are a linked listed of objects            */
/*  (widgets/gadgets) that were not defined at the time of the          */
/*  reference.  After all objects have been seen, then this list is     */
/*  traversed and all objects in it should now be defined and the       */
/*  specified pointers can be filled-in.                                */
#define sym_k_external_def_entry 6
/*  An external definition entry identifies a name that has been        */
/*  imported and thus in not declared within this source module.        */
#define sym_k_argument_entry 7
/*  An argument entry hangs off a list entry and identifies an object   */
/*  argument.  It contains informtation about the argument name and     */
/*  argument value.                                                     */
#define sym_k_callback_entry 8
/*  An calllback entry hangs off a list entry and identifies an object  */
/*  callback.  It contains informtation about the callback name,        */
/*  callback procedure, and callback tag.                               */
#define sym_k_module_entry 9
/*  A module entry contains information specified on the module         */
/*  statement.                                                          */
#define sym_k_proc_def_entry 10
/*  A procedure definition entry is created for each procedure listed   */
/*  in a procedure section.  I contains informaion on the number and    */
/*  types of the arguments.                                             */

#define sym_k_proc_ref_entry 11
/*  A procedure reference entry contains information about a the use of */
/*  a procedure (e.g. as a callback).  It also stores the tag to be     */
/*  passed to the procedure.                                            */
#define sym_k_list_entry 12
/*  A list entry is a typed list of other symbol table entries. It      */
/*  contains a count and a pointer to the next entry on the list via    */
/*  the obj_header.az_next field.                                       */
#define sym_k_child_entry 13
/*  A child entry represents a declaration for an                       */
/*  automatically created child.  It points off to                      */
/*  the arguments, callbacks, and controls lists for the child. Since   */
/*  the structures for widget and child are the same, the               */
/*  datastructure for children is really a sym_k_widget_entry_type.     */
#define sym_k_identifier_entry 14
/*  identifier entry tag is used for diagnostic messages only           */
#define sym_k_color_item_entry 15
/*  entry describing a color.                                           */
#define sym_k_gadget_entry 16
/*  A gadget entry represents a gadget declaration.  It points off to   */
/*  the arguments, callbacks, and controls lists for the gadget. Since  */
/*  the structures for widget and gadgets are the same, the             */
/*  datastructure for gadgets is really a sym_k_widget_entry_type.      */
#define sym_k_root_entry 17
/*  An entry used to group all the other entries together.  It is       */
/*  returned in the parse_tree_root field of the compilation descriptor */
/*  when using the callable interface.                                  */
#define sym_k_parent_list_entry 18
/*  This is a list of parents of a widget.  It is used to check for     */
/*  constraints provided by the parent that may be used on this widget. */
#define sym_k_nested_list_entry 19
/*  This entry occurs as a list entry for a nested list. It occupies    */
/*  the correct position in the list for the reference to a list, and   */
/*  points to the actual list entry.                                    */
#define sym_k_include_file_entry 20
/*  An include file entry is used to describe the contents of an        */
/*  include file.  It is needed only maintain information about the     */
/*  source file from which this parse tree was generated.  It is not    */
/*  used by UIL directly.                                               */
#define sym_k_section_entry 21
/*  A section entry is used to describe the contents of an source file. */
/*  It is needed only maintain information about the structure of       */
/*  source file from which this parse tree was generated.  It is not    */
/*  used by UIL directly.                                               */
/*                                                                      */
#define sym_k_def_obj_entry 22
/*  This entry corresponds to the default object clause on the module   */
/*  declaration it is used to store source information about the file   */
/*  from which this parse tree was generated.   It is not used by UIL   */
/*  directly.                                                           */
#define sym_k_UNUSED23_entry 23
#define sym_k_val_forward_ref_entry 24
/*  val forward reference entries are a linked listed of values         */
/*  that were not defined at the time of the                            */
/*  reference.  After all values have been seen, then this list is      */
/*  traversed and all values in it should now be defined and the        */
/*  specified pointers can be filled-in.                                */
#define sym_k_max_entry 24
/*  this is the largest possible value for an entry constant.           */

/*
**    Common attribute masks -- These values are used in the b_flags field of
**    the object_header.
*/

#define sym_m_private (1 << 0)
/*  This item is private to this source module and thus need not be     */
/*  output into the UID file.                                           */
#define sym_m_exported (1 << 1)
/*  This is an exported definition and must be put in the UID file.     */
#define sym_m_imported (1 << 2)
/*  This item is a reference external to this source module and thus    */
/*  will be resolved at runtime by searching the resourec hierarchy.    */
#define sym_m_reference (1 << 3)
#define sym_m_builtin (1 << 4)
/*  This item is builtin as oppose to a userdefined item.               */
#define sym_m_obj_is_gadget (1 << 5)
/*  This object is a gadget as oppose to a widget.                      */

/*
**	Output states - order is important
*/

#define sym_k_not_processed 0
#define sym_k_queued 1
#define sym_k_emitted 2

/*
**	Constants to define compiler-recognized data types. It is
**	important that the names used in .wml language descriptions
**	match these names. These values were once automatically generated
**	by WML. They are now maintained by hand to reduce compiler
**	dependence on WML artifacts.
*/
#define sym_k_any_value 1
#define sym_k_argument_value 2
#define sym_k_asciz_table_value 3
#define sym_k_bool_value 4
#define sym_k_char_8_value 5
#define sym_k_class_rec_name_value 6
#define sym_k_color_value 7
#define sym_k_color_table_value 8
#define sym_k_compound_string_value 9
#define sym_k_float_value 10
#define sym_k_font_value 11
#define sym_k_font_table_value 12
#define sym_k_icon_value 13
#define sym_k_identifier_value 14
#define sym_k_integer_value 15
#define sym_k_integer_table_value 16
#define sym_k_keysym_value 17
#define sym_k_pixmap_value 18
#define sym_k_reason_value 19
#define sym_k_rgb_value 20
#define sym_k_single_float_value 21
#define sym_k_string_table_value 22
#define sym_k_trans_table_value 23
#define sym_k_widget_ref_value 24
#define sym_k_xbitmapfile_value 25
#define sym_k_localized_string_value 26
#define sym_k_wchar_string_value 27
#define sym_k_fontset_value 28
#define sym_k_child_value 29
#define sym_k_max_value 29
#define sym_k_no_value (sym_k_max_value + 1)

/*
 * Error values for all kind of sym_k_... literal set
 */
#define sym_k_error_value 0
#define sym_k_error_object 0
#define sym_k_error_charset 0

/*
 * Literals associated with character sets
 *
 * Character set character sizes
 */
#define sym_k_onebyte_charsize 1
#define sym_k_twobyte_charsize 2
#define sym_k_mixed1_2byte_charsize 3

/*
 * User-defined character set
 */
#define sym_k_userdefined_charset 1

/*
 * XmFONTLIST_DEFAULT_TAG
 */
#define sym_k_fontlist_default_tag 0

/*
**	value sets defining expression operators
*/
#define sym_k_unspecified_op 0
#define sym_k_not_op 1
#define sym_k_unary_plus_op 2
#define sym_k_unary_minus_op 3
#define sym_k_comp_str_op 4
#define sym_k_wchar_str_op 5
#define sym_k_last_unary_op 5

#define sym_k_multiply_op 6
#define sym_k_divide_op 7
#define sym_k_add_op 8
#define sym_k_subtract_op 9
#define sym_k_left_shift_op 10
#define sym_k_right_shift_op 11
#define sym_k_and_op 12
#define sym_k_xor_op 13
#define sym_k_or_op 14
#define sym_k_cat_op 15
#define sym_k_last_binary_op 15
#define sym_k_valref_op                     \
  16 /* az_exp_op1 points to the value node \
        whose value is referenced by this   \
        value node. */
#define sym_k_coerce_op                     \
  17 /* az_exp_op1 points to the value node \
        whose value is to be coerced to the \
        data type of this node. */
#define sym_k_last_special_op 17

/*
 * Structure of an enumeration set entry
 */
typedef struct
{
  short int values_cnt;
  /* number of values in the enuemration set descriptor           */
  unsigned short int *values;
  /* vector of values                                             */
} UilEnumSetDescDef, *UilEnumSetDescDefPtr;

/**********************************************************************
**************** From UilKeyDef.h ************************************/
/*
 *    Token class literals
 */
#define tkn_k_class_argument 1
#define tkn_k_class_charset 2
#define tkn_k_class_color 3
#define tkn_k_class_enumval 4
#define tkn_k_class_font 5
#define tkn_k_class_identifier 6
#define tkn_k_class_keyword 7
#define tkn_k_class_literal 8
#define tkn_k_class_reason 9
#define tkn_k_class_reserved 10
#define tkn_k_class_special 11
#define tkn_k_class_unused 12
#define tkn_k_class_class 13
#define tkn_k_class_child 14

/*
 * Keyword table entry structure
 */
typedef struct
{
  unsigned char b_class;
  unsigned short int b_subclass;
  unsigned char b_length;
  unsigned char b_token;
  char *at_name;
} key_keytable_entry_type;

/*************************************************************************
************************ New Stuff **************************************/

/*
 * Names of the tables put in the binary database
 */

#define Constraint_Tab 1
#define Key_Table 2
#define Key_Table_Case_Ins 3
#define Allowed_Argument_Table 4
#define Argument_Type_Table_Value 5
#define Charset_Xmstring_Names_Table 6
#define Charset_Wrdirection_Table 7
#define Charset_Parsdirection_Table 8
#define Charset_Charsize_Table 9
#define Charset_Lang_Names_Table 10
#define Charset_Lang_Codes_Table 11
#define Allowed_Control_Table 12
#define Enum_Set_Table 13
#define Argument_Enum_Set_Table 14
#define Enumval_Values_Table 15
#define Uil_Widget_Names 16
#define Uil_Argument_Names 17
#define Uil_Reason_Names 18
#define Uil_Enumval_names 19
#define Uil_Charset_Names 20
#define Related_Argument_Table 21
#define Allowed_Reason_Table 22
#define Uil_Widget_Funcs 23
#define Uil_Gadget_Funcs 24
#define Uil_Urm_Nondialog_Class 25
#define Uil_Urm_Subtree_Resource 26
#define Uil_Argument_Toolkit_Names 27
#define Uil_Reason_Toolkit_Names 28
#define Child_Class_Table 29
#define Allowed_Child_Table 30
#define Uil_Children_Names 31

/*
 * Binary Data Base Work
 */
#define DB_Compiled_Version 2

typedef struct _db_globals_struct
{
  int version;
  int uil_max_arg;
  int uil_max_charset;
  int charset_lang_table_max;
  int uil_max_object;
  int uil_max_reason;
  int uil_max_enumval;
  int uil_max_enumset;
  int key_k_keyword_count;
  int key_k_keyword_max_length;
  int uil_max_child;
} _db_globals;

typedef struct _db_header_struct
{
  int table_id;
  int num_items;
  int table_size;
} _db_header, *_db_header_ptr;

#endif /* UilDBDef_h */
/* DON'T ADD STUFF AFTER THIS #endif */
