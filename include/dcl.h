#ifndef DCL_H
#define DCL_H


#include <facility_list.h>

	/****************************************************************
	 * Status codes:
	 ****************************************************************/
#define MDSDCL_STS(N)   (MDSDCL_FACILITY<<16)+N

#define MDSDCL_STS_SUCCESS         MDSDCL_STS(1)
#define MDSDCL_STS_INDIRECT_DONE   MDSDCL_STS(0x103)
#define MDSDCL_STS_EXIT            MDSDCL_STS(0x105)

#define MDSDCL_STS_ERROR           MDSDCL_STS(0x202)
#define MDSDCL_STS_INDIRECT_ERROR  MDSDCL_STS(0x304)
#define MDSDCL_STS_INDIRECT_EOF    MDSDCL_STS(0x406)


#ifdef CREATE_STS_TEXT
static struct stsText mdsdcl_stsText[] = {
  STS_TEXT(MDSDCL_STS_SUCCESS, "Normal successful completion")
      , STS_TEXT(MDSDCL_STS_INDIRECT_DONE, "Success from indirect command")

      , STS_TEXT(MDSDCL_STS_ERROR, "Generic MDSDCL Error")
      , STS_TEXT(MDSDCL_STS_INDIRECT_ERROR, "Error from indirect command")
      , STS_TEXT(MDSDCL_STS_INDIRECT_EOF, "End-Of-File in indirect command")
};
#endif
/**********************************************************************
* CLIDEF_H.H --
*
* Status codes, etc.
*
***********************************************************************/

#define CLI_STS(N)   (CLI_FACILITY<<16)+N

#define CLI_STS_NORMAL     CLI_STS(1)	/* ok                           */
#define CLI_STS_COMMA      CLI_STS(3)	/* return val terminated by comma */
#define CLI_STS_CONCAT     CLI_STS(5)	/* --not used--                 */
#define CLI_STS_PRESENT    CLI_STS(7)	/* entity is present            */
#define CLI_STS_DEFAULTED  CLI_STS(9)	/* not present, but defaulted   */

#define CLI_STS_IVVERB     CLI_STS(2)	/* invalid verb                 */
#define CLI_STS_NOCOMD     CLI_STS(4)	/* null input                   */
#define CLI_STS_EOF        CLI_STS(6)	/* eof                          */
#define CLI_STS_INVROUT    CLI_STS(8)	/* invalid routine              */
#define CLI_STS_BADLINE    CLI_STS(10)	/* generic "badline" status     */
#define CLI_STS_IVKEYW     CLI_STS(12)	/* invalid keyword              */
#define CLI_STS_ABSENT     CLI_STS(14)	/* val not present / no more vals */
#define CLI_STS_UNDSYM     CLI_STS(16)	/* undefined symbol??           */
#define CLI_STS_IVREQTYP   CLI_STS(18)	/* invalid request type ?       */
#define CLI_STS_NEGATED    CLI_STS(20)	/* entity is present+negated    */
#define CLI_STS_NOTNEGATABLE CLI_STS(22)	/* entity is not negatable     */
#define CLI_STS_ERROR      CLI_STS(24)	/* error in subroutine          */
#define CLI_STS_IVQUAL     CLI_STS(26)	/* invalid verb qualifier       */
#define CLI_STS_PROMPT_MORE CLI_STS(28)  /* More input needed            */
#define CLI_STS_TOO_MANY_PRMS CLI_STS(30)  /* Too many parameters provided */
#define CLI_STS_TOO_MANY_VALS CLI_STS(32) /* Too many values */
#define CLI_STS_MISSING_VALUE CLI_STS(34) /* Missing value */

#ifdef CREATE_STS_TEXT
static struct stsText cli_stsText[] = {
  STS_TEXT(CLI_STS_NORMAL, "Normal successful completion")
      , STS_TEXT(CLI_STS_COMMA, "Comma-delimited value:  more in list")
      , STS_TEXT(CLI_STS_CONCAT, "Concatenate with next value in list")
      , STS_TEXT(CLI_STS_PRESENT, "Entity is present")
      , STS_TEXT(CLI_STS_DEFAULTED, "Entity is present by default")
      , STS_TEXT(CLI_STS_IVVERB, "Invalid Verb")
      , STS_TEXT(CLI_STS_NOCOMD, "No command on line")
      , STS_TEXT(CLI_STS_EOF, "End of file")
      , STS_TEXT(CLI_STS_INVROUT, "Invalid routine")
      , STS_TEXT(CLI_STS_BADLINE, "Bad input line")
      , STS_TEXT(CLI_STS_IVKEYW, "Invalid keyword")
      , STS_TEXT(CLI_STS_ABSENT, "Entity is absent")
      , STS_TEXT(CLI_STS_UNDSYM, "Undefined symbol")
      , STS_TEXT(CLI_STS_IVREQTYP, "Unsupported request")
      , STS_TEXT(CLI_STS_NEGATED, "Entity is present but negated")
      , STS_TEXT(CLI_STS_NOTNEGATABLE, "Entity cannot be negated")
      , STS_TEXT(CLI_STS_ERROR, "Generic ERROR")
      , STS_TEXT(CLI_STS_IVQUAL, "Invalid verb qualifier")
  , STS_TEXT(CLI_STS_PROMPT_MORE, "More input required for command")
  , STS_TEXT(CLI_STS_TOO_MANY_PRMS, "Too many parameters specified")
  , STS_TEXT(CLI_STS_TOO_MANY_VALS, "Too many values")
  , STS_TEXT(CLI_STS_MISSING_VALUE, "Qualifier value needed")
};
#endif

extern void *mdsdclSetOutputRtn(void (*rtn)());
extern void mdsdclFlushOutput(char *output);
extern int mdsdcl_do_command(char const *command);

#endif
