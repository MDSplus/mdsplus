#ifdef __vms
#ifndef vms
#define vms    1
#endif
#endif

#ifndef __CLIDEF_H
#define __CLIDEF_H   1

#include        "cdudef.h"

/**********************************************************************
* CLIDEF_H.H --
*
* Status codes, etc.
*
***********************************************************************/


#define CLI_STS(N)   (CLI_FACILITY<<16)+N

#define CLI_STS_NORMAL     CLI_STS(1)	/* ok				*/
#define CLI_STS_COMMA      CLI_STS(3)	/* return val terminated by comma*/
#define CLI_STS_CONCAT     CLI_STS(5)	/* --not used--			*/
#define CLI_STS_PRESENT    CLI_STS(7)	/* entity is present		*/
#define CLI_STS_DEFAULTED  CLI_STS(9)	/* not present, but defaulted	*/

#define CLI_STS_IVVERB     CLI_STS(2)	/* invalid verb			*/
#define CLI_STS_NOCOMD     CLI_STS(4)	/* null input			*/
#define CLI_STS_EOF        CLI_STS(6)	/* eof				*/
#define CLI_STS_INVROUT    CLI_STS(8)	/* invalid routine		*/
#define CLI_STS_BADLINE    CLI_STS(10)	/* generic "badline" status	*/
#define CLI_STS_IVKEYW     CLI_STS(12)	/* invalid keyword		*/
#define CLI_STS_ABSENT     CLI_STS(14)	/* val not present / no more vals*/
#define CLI_STS_UNDSYM     CLI_STS(16)	/* undefined symbol??		*/
#define CLI_STS_IVREQTYP   CLI_STS(18)	/* invalid request type ?	*/
#define CLI_STS_NEGATED    CLI_STS(20)	/* entity is present+negated	*/
#define CLI_STS_NOTNEGATABLE CLI_STS(22) /* entity is not negatable	*/
#define CLI_STS_ERROR      CLI_STS(24)	/* error in subroutine		*/

#ifdef CREATE_STS_TEXT
static struct stsText  cli_stsText[] = {
        STS_TEXT(CLI_STS_NORMAL)
       ,STS_TEXT(CLI_STS_COMMA)
       ,STS_TEXT(CLI_STS_CONCAT)
       ,STS_TEXT(CLI_STS_PRESENT)
       ,STS_TEXT(CLI_STS_DEFAULTED)
       ,STS_TEXT(CLI_STS_IVVERB)
       ,STS_TEXT(CLI_STS_NOCOMD)
       ,STS_TEXT(CLI_STS_EOF)
       ,STS_TEXT(CLI_STS_INVROUT)
       ,STS_TEXT(CLI_STS_BADLINE)
       ,STS_TEXT(CLI_STS_IVKEYW)
       ,STS_TEXT(CLI_STS_ABSENT)
       ,STS_TEXT(CLI_STS_UNDSYM)
       ,STS_TEXT(CLI_STS_IVREQTYP)
       ,STS_TEXT(CLI_STS_NEGATED)
       ,STS_TEXT(CLI_STS_NOTNEGATABLE)
       ,STS_TEXT(CLI_STS_ERROR)
       };
#endif

	/*===============================================================
	 * Predefined structs:
	 *==============================================================*/
struct  cduVerb;


	/*===============================================================
	 * User-callable functions:
	 *==============================================================*/
int   cli_dcl_parse(		/* Returns: status			*/
    void  *command_string	/* <r:opt> descr or c-string		*/
   ,struct cduVerb  *table	/* <r> addr of command table		*/
   ,int   (*param_routine)()	/* <r:opt> routine to read req'd params	*/
   ,int   (*prompt_routine)()	/* <r:opt> routine to read command string*/
   ,void  *uprompt		/* <r:opt> descr or c-string		*/
   );
int   cli_dispatch(		/* Return: sts of dispatched routine	*/
    void  *userarg	/* <r> optional user arg, passed to routine	*/
   );
int   cli_present(		/* Return: status			*/
    void  *entity		/* <r> entity name: dsc or c-string	*/
   );
int   cli_get_value(		/* Return: status			*/
    void  *entity		/* <r> entity name: dsc or c-string	*/
   ,struct descriptor  *dsc_ret	/* <w> return value descriptor	*/
   );

#endif        /* __CLIDEF_H	*/
