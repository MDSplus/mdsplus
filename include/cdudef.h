#ifdef __vms
#ifndef vms
#define vms    1
#endif
#endif

#ifndef CDUDEF_H
#define CDUDEF_H       1

#include        "facility_list.h"
#include        "dasutil.h"

/**********************************************************************
* CDUDEF.H --
*
* Definitions for command-line parser ...
*
* History:
*  09-Feb-1998  TRG  Define ENT_M_xxx, to help in generic processing of
*                     entities.  xxx={PARAMETERS,QUALIFIERS,KEYWORDS}.
*  06-Oct-1997  TRG  Create.
*
***********************************************************************/


#ifdef _CASE_DEFINITIONS		/* in cdu.c only	*/
#define CMD_DEFINE           1
#define CMD_IDENT            2
#define CMD_MODULE           3

#define DEFINE_VERB          1
#define DEFINE_SYNTAX        2
#define DEFINE_TYPE          3

#define VERB_DISALLOW        1
#define VERB_NODISALLOWS     2
#define VERB_IMAGE           3
#define VERB_PARAMETER       4
#define VERB_NOPARAMETERS    5
#define VERB_QUALIFIER       6
#define VERB_NOQUALIFIERS    7
#define VERB_ROUTINE         8
#define VERB_SYNONYM         9

#define PARAM_DEFAULT        1
#define PARAM_LABEL          2
#define PARAM_PROMPT         3
#define PARAM_VALUE          4

#define VAL_CONCATENATE      1
#define VAL_DEFAULT          2
#define VAL_LIST             3
#define VAL_NOCONCATENATE    4
#define VAL_REQUIRED         5
#define VAL_TYPE             6

#define QUAL_BATCH           1
#define QUAL_DEFAULT         2
#define QUAL_LABEL           3
#define QUAL_NEGATABLE       4
#define QUAL_NONNEGATABLE    5
#define QUAL_PLACEMENT       6
#define QUAL_SYNTAX          7
#define QUAL_VALUE           8

#define TYPE_KEYWORD         1

#define KEY_DEFAULT          1
#define KEY_LABEL            2
#define KEY_NEGATABLE        3
#define KEY_NONNEGATABLE     4
#define KEY_SYNTAX           5
#define KEY_VALUE            6

#endif		/* _CASE_DEFINITIONS	*/

#define VERB_M_NODISALLOWS   0x0001
#define VERB_M_NOPARAMETERS  0x0002
#define VERB_M_NOQUALIFIERS  0x0004

#define PARAM_M_DEFAULT    0x0001

#define VAL_M_USER_DEFINED_TYPE 0x0001
#define VAL_M_FILE              0x0002
#define VAL_M_NUMBER            0x0004
#define VAL_M_QUOTED_STRING     0x0008
#define VAL_M_DATETIME          0x0010
#define VAL_M_REST_OF_LINE      0x0020
#define VAL_M_ACL               0		/* Not supported	*/
#define VAL_M_DELTATIME         0x0040		/* Not supported	*/
#define VAL_M_EXPRESSION        0		/* Not supported	*/
#define VAL_M_LIST              0x1000
#define VAL_M_REQUIRED          0x2000

#define QUAL_M_DEFAULT       0x0001
#define QUAL_M_NEGATABLE     0x0002
#define QUAL_M_NONNEGATABLE  0x0004

#define KEY_M_DEFAULT       0x0001
#define KEY_M_NEGATABLE     0x0002
#define KEY_M_NONNEGATABLE  0x0004

#define ENT_M_DEFAULTED     0x0001
#define ENT_M_NEGATABLE     0x0002
#define ENT_M_NONNEGATABLE  0x0004
#define ENT_M_PARAMETERS    0x1000	/* identify type of structure	*/
#define ENT_M_QUALIFIERS    0x2000	/* identify type of structure	*/
#define ENT_M_KEYWORDS      0x4000	/* identify type of structure	*/

		/*========================================================
	 	 * structures ...
		 *=======================================================*/
struct valueClause {
        long  valL_flags;	/* flags: built-in or user-defined type	*/
        char  *valA_default;	/* default value string			*/
        char  *valA_userType;	/* name of  user-defined "type"		*/
       };
struct paramClause  {
        char  *prmA_name;
        long  prmL_flags;
        char  *prmA_label;
        char  *prmA_prompt;
        struct valueClause  *prmA_value;
       };
struct verblist  {
        char  *vrbA_name;
        int   vrbL_flags;
        char  *vrbA_disallow;
        char  *vrbA_image;
        char  *vrbA_routine;
        void  *vrbA_parametersTop;
        void  *vrbA_qualifiersTop;
       };
struct keyword  {
        char  *keyA_name;
        long  keyL_flags;
        char  *keyA_label;
        char  *keyA_syntax;
        struct valueClause  *keyA_value;
       };
struct qualifier  {
        char  *qualA_name;
        long  qualL_flags;
        char  *qualA_label;
        char  *qualA_syntax;
        struct valueClause  *qualA_value;
       };

		/*========================================================
		 * macros ...
		 *=======================================================*/
#define STRING_ALLOC(T)  strcpy(malloc(strlen(T)+1),T)

		/*=======================================================
		 * Function prototypes ...
		 *======================================================*/
void  *findUserTypeByIdx(
    int   idxRequest		/* <r> requested idx number, zero based	*/
   );
int   userTypeHasBeenWritten(	/* Return: 1(T) or 0(F)			*/
    char  uname[]		/* <r> name of "type"			*/
   );
int   okToWriteUserType(	/* Return: 1(T) or 0(F)			*/
    char  uname[]		/* <r> name of "type"			*/
   );
void  markUserTypeWritten(	/* Returns:  void			*/
    char  uname[]		/* <r> name of "type"			*/
   );
int   getUserTypeIdx(		/* Return: idx into userTypeList[]	*/
    char  uname[]		/* <r> name of "type"			*/
   );
struct keyword  *getKeywordStruct(	/* Return: addr of keyword struct */
    char  userType[]			/* <r> name of "user type"	*/
   ,char  uname[]			/* <r> keyword name		*/
   );
struct keyword  *findKeywordByIdx(	/* Returns: addr of struct	*/
    char  typeName[]			/* <r> user "type"		*/
   ,int   keywordIdx		/* <r> keywordIdx of keyword in chain	*/
   );
struct paramClause  *findParamByIdx(	/* Return: addr of paramClause	*/
    struct verblist  *v		/* <r> verb struct		*/
   ,int   paramIdx			/* <r> parameter idx		*/
   );
struct paramClause  *getParamByIdx(	/* Return: addr of paramClause	*/
    struct verblist  *v			/* <r> verb struct		*/
   ,int   paramIdx			/* <r> parameter idx		*/
   );
struct qualifier  *findQualifierByIdx(	/* Return: addr of qualifier str*/
    struct verblist  *v		/* <m> verb struct of "owner"		*/
   ,int   idx			/* <r> index number			*/
   );
struct qualifier  *findQualifier( /* Return: addr of qualifier struct	*/
    struct verblist  *v		/* <m> verb struct of "owner"		*/
   ,char  qualName[]		/* <r> name of qualifier		*/
   );
struct qualifier  *getQualifier( /* Return: addr of qualifier struct	*/
    struct verblist  *v		/* <m> verb struct of "owner"		*/
   ,char  qualName[]		/* <r> name of qualifier		*/
   );
struct verblist  *findVerb(	/* Return: Addr of existing verb	*/
    char  verbName[]		/* <r> name of the verb			*/
   );
struct verblist  *findVerbByIdx( /* Return: Addr of existing verb	*/
    int   idxRequest		/* <r> idx of requested verb		*/
   );
struct verblist  *newVerb(	/* Return: Create new verb; else error	*/
    char  verbName[]		/* <r> name of the verb			*/
   );
struct verblist  *getVerb(	/* Return: Create verb if req'd		*/
    char  verbName[]		/* <r> name of the verb			*/
   );
struct verblist  *findSyntax(	/* Return: Addr of existing syntax	*/
    char  syntaxName[]		/* <r> name of syntax			*/
   );
struct verblist  *findSyntaxByIdx( /* Return: Addr of existing syntax	*/
    int   idxRequest		/* <r> idx of requested verb		*/
   );
struct verblist  *newSyntax(	/* Return: Create new syntax; else error*/
    char  syntaxName[]		/* <r> name of syntax			*/
   );
struct verblist  *getSyntax(	/* Return: Create syntax if req'd	*/
    char  syntaxName[]		/* <r> name of syntax			*/
   );
int   writeProtoUserType(	/* Returns: status			*/
    char  name[]		/* <r> userType name			*/
   );
void  cdu_write(
    char  _module[]		/* <r> name of module -- filename too	*/
   );

#endif		/* CDUDEF_H	*/
