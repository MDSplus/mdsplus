#ifdef __vms
#ifndef vms
#define vms    1
#endif
#endif

#ifndef CLISYSDEF_H
#define CLISYSDEF_H   1

#include        "dasutil.h"
#include        "clidef.h"
#include        "cdudef.h"

/************************************************************************
* CLISYSDEF.H --
*
* Data structures for CLI emulation ...
*
* History:
*  16-Aug-2001  TRG  Make IS_COMMENT_CHARACTER be '!' only, not ';'
*  20-Oct-1997  TRG  Create.
*
*************************************************************************/


struct cduValue {
        int   valL_flags;	/* flags: built-in or user-defined type	*/
        char  *valA_default;	/* default value string			*/
        struct cduKeyword  *valA_userType;	/* user-defined "type"	*/
        struct descriptor  val_dsc;		/* "Value" string	*/
        char  *valA_substring;	/* pointer within value string		*/
       };

		/*-------------------------------------------------------
		 * "Entities" are the named objects which the user
		 * can request, via cli_present() and cli_get_value().
		 *
		 * Parameters, Qualifiers, and Keywords are all entities.
		 * They share a common structure shape, generically
		 * represented by the cduEntity structure.
		 *------------------------------------------------------*/
struct cduParam  {
        char  *prmA_name;
        int   prmL_flags;
        char  *prmA_label;
        char  *prmA_prompt;
        struct cduValue  *prmA_value;
        int   prmL_status;		/* processing status		*/
       };
struct cduQualifier  {
        char  *qualA_name;
        int   qualL_flags;
        char  *qualA_label;
        void  *qualA_syntax;
        struct cduValue  *qualA_value;
        int   qualL_status;		/* processing status		*/
       };
struct cduKeyword  {
        char  *keyA_name;
        int   keyL_flags;
        char  *keyA_label;
        void  *keyA_syntax;
        struct cduValue  *keyA_value;
        int   keyL_status;
       };
struct cduEntity  {
        char  *entA_name;
        int   entL_flags;
        char  *entA_label;
        void  *entA_entity_specific;
        struct cduValue  *entA_value;
        int   entL_status;
       };
		/*-------------------------------------------------------
		 * Verb and Syntax share the same structure, cduVerb
		 *------------------------------------------------------*/
struct cduVerb  {
        char  *vrbA_name;
        int   vrbL_flags;
        char  *vrbA_disallow;
        char  *vrbA_image;		/* .exe filename	*/
        int   (*vrbA_routine)();
        struct cduParam  *vrbA_parameters;
        struct cduQualifier  *vrbA_qualifiers;
       };



	/****************************************************************
	 * macros, etc
	 ****************************************************************/
#ifdef vms
#define COMMENT_CHARACTER  '!'
#define CONTINUATION_CHARACTER  '-'
#define QUALIFIER_CHARACTER     '/'
#else
#define COMMENT_CHARACTER  ';'
#define CONTINUATION_CHARACTER  '\\'
#define QUALIFIER_CHARACTER     '-'
#endif

#define IS_COMMENT_CHARACTER(C)       /*((C)=='!' || (C)==';')*/  ((C)=='!')
#define IS_CONTINUATION_CHARACTER(C)  ((C)=='-' || (C)=='\\')
#define IS_QUALIFIER_CHARACTER(C)     ((C)=='/' || (C)=='-')



	/***************************************************************
	 * function prototypes:
	 * Note: user-callable functions appear in clidef.h
	 ***************************************************************/
int   cli_debug(		/* Returns: new debug status		*/
    int   val			/* <r> new debug status			*/
   );
int   cli_error(		/* Display only				*/
    int   sts			/* <r> status value			*/
   ,char  msg[]			/* <r> message to display		*/
   );
int   cliToken(
    char  **pp			/* <m> addr of ptr to char string	*/
   ,struct descriptor *dsc_ret	/* <w> return token here		*/
   ,int   flags			/* <r> cli flags, describing data type	*/
   );
struct cmd_struct  *make_lookup_keyword( /* Return: addr of cmd_struct	*/
    struct cduKeyword  keywordList[]	/* <r> keyword list		*/
   );
struct descriptor  *cli_addr_cmdline_dsc();	/* Return: addr of dsc	*/

#endif		/* CLISYSDEF_H	*/
