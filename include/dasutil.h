#include        <stdarg.h>
#include        <stdio.h>
#include        <string.h>
#include        <time.h>

/********************************************************************
* DASUTIL.H --
*
* Prototypes for functions in dasutil library
*
*********************************************************************/

#ifndef DASUTIL_H
#define DASUTIL_H    1
#ifdef __vms
#define vms    1		/* allow VAXC plus strict ANSI standards */
#endif

		/*======================================================
		 * cmd_lookup:
		 *=====================================================*/
#define NOMSG    1		/* "flags" mask, for no errmsg		*/
#define NOWILD   2		/* "flags" mask, ignores wildcards	*/

struct cmd_struct  {
        long  cmdL_id;		/* command id #				*/
        char  *cmdA_string;	/* command text				*/
       };

int cmd_lookup(
	char  **s,		/* <m> Ptr within command string	*/
	struct cmd_struct  cmdlist[], /* <r> Command-list struct	*/
	char  parentString[],	/* <r:opt> for use with error msg	*/
	long  flags,		/* <r:opt> 1=NoErrMsg			*/
	long  *ucontext		/* <m:opt> Allows continuation of "lookup"*/
	);

		/*======================================================
		 * dasmsg, ipcsmsg, etc.:
		 *=====================================================*/
int dasmsg(
        int   sts,	/* <r> Status code: 0=nomsg 1=nomsg(vms only)	*/
        char  fmt[],	/* <r> Format string for sprintf		*/
        ...		/* <r> Parameters, as required by fmt[]		*/
        );
int   das_brkthru(
    /* int   pid,		/* <r> pid of recipient			*/
    /* char  fmt[],		/* <r> Format string, for sprintf	*/
    /* ...			/* <r> Parameters, as required by fmt[]	*/
   );
int   ipcsmsg(
/*        int   sts,	/* <r> Ipcs status code: 0=nomsg		*/
/*        char  fmt[],	/* <r> Format string for sprintf		*/
/*        ...		/* <r> Parameters, as required by fmt[]		*/
        );
char  *ipcsStatusText(
    int   sts		/* <r> ipcs message status			*/
   );

		/*=======================================================
		 * order ...
		 *======================================================*/
int   order(
    int  (*cmp)(),	/* <r> addr of comparison routine for subj array.
			 *     cmp(i,j) returns:
			 *       positive if subj[i] > subj[j]
			 *       zero     if subj[i] == subj[j]
			 *       negative if subj[i] < subj[j]		*/
    int   list[],	/* <m> ordered list				*/
    int   *nlow,	/* <m> idx of current "low" entry		*/
    int   *nhigh,	/* <m> idx of current "high" entry		*/
    int   *num,		/* <m> number of entries already ordered	*/
    int   j3		/* <r> idx of >>new entry<<			*/
   );
int   ordloc(
    void  *subjx,	/* <r> "subject" element:  find its position	*/
    int   (*xcmp)(),	/* <r> comparison routine for subjx vs subject[].
			 *      xcmp(subjx,j) returns:
			 *        positive if subjx > subject[j]
			 *        zero     if subjx == subject[j]
			 *        negative if subjx < subject[j]	*/
    int   list[],	/* <r> ordered list				*/
    int   *nlow,	/* <r> idx of current "low" entry		*/
    int   *nhigh,	/* <r> idx of current "high" entry		*/
    int   *num		/* <r> number of entries already ordered	*/
   );

		/*======================================================
		 * nonblank, etc ...
		 *=====================================================*/
char  *nonblank(
        char *p			/* <r> Addr of start of string		*/
       );
char  *blank(
        char *p			/* <r> Addr of start of string		*/
       );
int   strtrim(
    char  to[]		/* <w/m> Destination string			*/
   ,char  from[]	/* <r:opt> Source string			*/
   );
int    strntrim(
    char  to[]		/* <w/m> Destination string			*/
   ,char  from[]	/* <r:opt> Source string			*/
   ,int   fromLen	/* <r> Length of source string			*/
   );
void  remove_commas(
        char  str[]		/* <m> string to operate upon		*/
       );

char  *l2u(
    char  *p		/* <m> string to modify				*/
   ,char  *t		/* <r:opt> source string			*/
   );
char  *l2un(
    char  to[],		/* <w> destination string			*/
    char  from[],	/* <r> source string				*/
    int   n		/* <r> number of characters in from[]		*/
   );
char  *u2l(
    char  *p		/* <m> string to modify				*/
   ,char  *t		/* <r:opt> source string			*/
   );
char  *u2ln(
    char  to[],		/* <w> destination string			*/
    char  from[],	/* <r> source string				*/
    int   n		/* <r> number of characters in from[]		*/
   );
long  bcd2i(
    unsigned long  bcd	/* <r> bcd val, 4 bits per digit, max 8 digits	*/
   );
long  i2bcd(		/* bcd val, 4 bits per digit, max 8 digits	*/
    unsigned long  ival	/* <r> integer value to convert			*/
   );

int   wildcard_match(
    char  *s,		/* <r> input pattern, may contain wildcards	*/
    char  *t,		/* <r> comparison string			*/
    char  wc,		/* <r:opt> wildcard char			*/
    char  any,		/* <r:opt> "any single char" character		*/
    char  anydig	/* <r:opt> "any single numerical digit" char	*/
   );
int   wc_match(
    char  *s,		/* <r> input pattern, may contain wildcards	*/
    char  *t		/* <r> comparison string			*/
   );

		/*=======================================================
		 * string utility functions ...
		 *======================================================*/
#define DSC_K_DTYPE_T    14
#define DSC_K_CLASS_S    1	/* Static descriptor	*/
#define DSC_K_CLASS_D    2	/* Dynamic descriptor	*/

#ifdef MDSDESCRIP_H_DEFINED
#undef DESCRIPTOR
#else
struct descriptor  {
        short dscW_length;
        char  dscB_dtype,dscB_class;
        char  *dscA_pointer;
       };
#endif
#define DESCRIPTOR(name,string)  struct descriptor  name = \
                {sizeof(string)-1,DSC_K_DTYPE_T,DSC_K_CLASS_S,string}
#define DYNAMIC_DESCRIPTOR(D)  struct descriptor  D = {\
                                      0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0 }
#define ALIGN_MASK(x)  (sizeof(x) - 1)
#define is_cdescr(d)  (((int)(d) & ALIGN_MASK(void *))==0 &&	\
            ((struct descriptor *)(d))->dscB_dtype==DSC_K_DTYPE_T && \
            ((struct descriptor *)(d))->dscB_class==DSC_K_CLASS_S && \
            ((struct descriptor *)(d))->dscW_length<=1024)
#define is_ddescr(d)  (((int)(d) & ALIGN_MASK(void *))==0 &&	\
            ((struct descriptor *)(d))->dscB_dtype==DSC_K_DTYPE_T && \
            ((struct descriptor *)(d))->dscB_class==DSC_K_CLASS_D && \
            ((struct descriptor *)(d))->dscW_length<=1024)
#define dsc_descriptor  descriptor

#define NIL           (void *)0

int   str_copy_dx(			/* Return: status		*/
    struct descriptor  *dsc_ret		/* <w> Destination string	*/
   ,void  *source			/* <r> source: c-string or dsc	*/
   );
int   str_free1_dx(			/* Returns: status		*/
    struct descriptor  *dsc		/* <m> descriptor to free	*/
   );
int   str_trim(				/* Return: status		*/
    struct descriptor  *dsc_ret		/* <w> Destination string	*/
   ,void  *optsrc			/* <r:opt> source: cstring or dsc */
   );
int   str_prefix(
    struct descriptor  *dsc_ret		/* <w> Destination string	*/
   ,void  *prefix			/* <r> Prefix: cstring or dsc	*/
   );
int   str_replace(			/* Return: status		*/
    struct descriptor  *dsc_ret		/* <w> Destination string	*/
   ,void  *source			/* <r> Source: c-string or dsc	*/
   ,int   offsetStart			/* <r> start pos <0-based>	*/
   ,int   offsetEnd			/* <r> end pos <0-based>	*/
   ,void  *replaceString		/* <r> replacement: c-str or dsc*/
   );
int   str_append(			/* Return: status		*/
    struct descriptor  *dsc_ret		/* <w> Destination string	*/
   ,void  *source			/* <r> source: c-string or dsc	*/
   );
char  *str_concat(		/* Returns: ptr to null-terminated string*/
    struct descriptor  *dsc_dest	/* <w> Destination string dsc	*/
   ,...				/* <r> source strings: dsc or c-string	*/
   );
int   str_element(			/* Returns: status		*/
    struct descriptor  *dsc_ret		/* <w> return string		*/
   ,int   ielement			/* <r> element num, 0-based	*/
   ,char  delimiter			/* <r> delimiter character	*/
   ,void  *source			/* <r> source: dsc or c-string	*/
   );
char  *str_dupl_char(			/* Returns: dsc_ret->dscA_pointer */
    struct descriptor  *dsc_ret		/* <w> Destination string	*/
   ,int   icnt				/* <r> duplication count	*/
   ,char  c				/* <r> character to duplicate	*/
   );


		/*=======================================================
		 * token-oriented functions ...
		 *======================================================*/
int   tknlen(
    char  *s,			/* <r> addr of start of token		*/
    char  usrTerminators[]	/* <r:opt> string of "terminator" chars	*/
   );
int   ascToken(
    char  **s		/* <m> Addr of ptr to input string		*/
   ,struct descriptor  *dsc_token	/* <w> return token here	*/
   ,long  *utknLen	/* <w:opt> token length				*/
   ,char  optAlph[]	/* <r:opt> User-defined alph extensions		*/
   );
int   nextToken(
    char  **s,			/* <m> addr of ptr to char string	*/
    struct descriptor  *dsc_token,	/* <w> return token here	*/
    long  *utknLen,		/* <w:opt> token length <longword>	*/
    char  wildcards[],		/* <r:opt> str of acceptable wildcard chrs*/
    char  alphExtensions[]	/* <r:opt> extensions to default alphnum */
   );
int   longToken(
    char  **s,			/* <m> addr of ptr to char string	*/
    struct descriptor  *dsc_token,	/* <w> return token here	*/
    long  *utknLen,		/* <w:opt> token length <longword>	*/
    long  *uval			/* <w:opt> return long value here	*/
   );
int   doubleToken(
    char  **s,			/* <m> addr of ptr to char string	*/
    struct descriptor  *dsc_token,	/* <w> return token here	*/
    long  *utknLen,		/* <w:opt> token length <longword>	*/
    double *uval		/* <w:opt> return double value here	*/
   );
int   ascFilename(
    char  **s			/* <m> addr from which to start search	*/
   ,struct descriptor  *dsc		/* <w> return token here	*/
   ,long  *utknLen		/* <w:opt> length of token		*/
   );
int   deltatimeToken(		/* Returns: status			*/
    char  **pp			/* <m> addr of ptr to char string	*/
   ,struct descriptor *dsc	/* <w> return token here		*/
   ,long  *utknlen		/* <w:opt> token length <longword>	*/
   ,long  *val			/* <w:opt> return deltatime <sec> here	*/
   );
int   equalsAscToken(
    char  **s		/* <m> addr from which to start search		*/
   ,struct descriptor  *dsc		/* <w> return token here	*/
   ,long  *utknLen	/* <w:opt> length of token (2-byte word)	*/
   ,char  optAlph[]	/* <r:opt> User-defined alph extensions		*/
   );
int   equalsLongToken(
    char  **s,			/* <m> addr of ptr to char string	*/
    struct descriptor  *dsc_token,	/* <w> return token here	*/
    long  *utknLen,		/* <w:opt> token length <longword>	*/
    long  *uval			/* <w:opt> return long value here	*/
   );
int   equalsDoubleToken(
    char  **s,			/* <m> addr of ptr to char string	*/
    struct descriptor  *dsc_token,	/* <w> return token here	*/
    long  *utknLen,		/* <w:opt> token length <longword>	*/
    double *uval		/* <w:opt> return double value here	*/
   );
int   equalsAscFilename(
    char  **s		/* <m> addr from which to start search		*/
   ,struct descriptor  *dsc		/* <w> return token here	*/
   ,long  *utknLen	/* <w:opt> length of token			*/
   );

#define clear_buffer(P,N)  memset(P,0,N)

		/*=======================================================
		 * Terminal i/o functions, with read-ahead
		 *======================================================*/
#define SHOW_LINE       1		/* flag for clearnext		*/
#define NOSHOW_LINE     0		/*  " ...			*/

void  termio_init(
    int   argc
   ,char  *argv[]
   );
void  clearnext(
    int   showFlag		/* <r> 1:show deleted line  0:no msg	*/
   );
void  putNext(
    void  *dsc_text		/* <r> string or descr: text to place	*/
   ,int   flag			/* <r> 0:atEnd 1:clear 2:atStart	*/
   );
char  *getString_linePtr();
char  *getString_startOfLine();
void  bumpString_linePtr();
int   getString(
    void  *dsc_prompt		/* <r> string or descr: prompt string	*/
   ,struct descriptor  *dsc_val		/* <m> string returned		*/
   ,char  alphExtensions[]		/* <r:opt> other legal 1st chars */
   );
int   getLine(
    void  *dsc_prompt		/* <r> string or descr: prompt string	*/
   ,struct descriptor  *dsc_val		/* <m> string returned		*/
   );
int   getDouble(
    void  *dsc_prompt		/* <r> string or descr: prompt string	*/
   ,double *val			/* <m> data value			*/
   ,double *valMin		/* <opt:r> lower data limit		*/
   ,double *valMax		/* <opt:r> upper data limit		*/
   ,struct descriptor  *dsc_userFlag	/* <opt:w> 1-char "flag"	*/
   );
int   getLong(
    void  *dsc_prompt		/* <r> string or descr: prompt string	*/
   ,long  *val			/* <m> data value			*/
   ,long  *valMin		/* <opt:r> lower data limit		*/
   ,long  *valMax		/* <opt:r> upper data limit		*/
   ,struct descriptor  *dsc_userFlag	/* <opt:w> 1-char "flag"	*/
   );
int   getFloat(
    void  *dsc_prompt		/* <r> string or descr: prompt string	*/
   ,float *val			/* <m> data value			*/
   ,float *valMin		/* <opt:r> lower data limit		*/
   ,float *valMax		/* <opt:r> upper data limit		*/
   ,struct descriptor  *dsc_userFlag	/* <opt:w> 1-char "flag"	*/
   );
int   getFilename(
    void  *dsc_prompt		/* <r> string or descr: prompt string	*/
   ,struct descriptor  *dsc_val		/* <m> string returned		*/
   );
int   getEqualsLong(
    void  *dsc_prompt		/* <r> string or descr: prompt string	*/
   ,long  *val			/* <m> data value			*/
   ,long  *valMin		/* <opt:r> lower data limit		*/
   ,long  *valMax		/* <opt:r> upper data limit		*/
   ,struct descriptor  *dsc_userFlag	/* <opt:w> 1-char "flag"	*/
   );
int   getEqualsString(
    void  *dsc_prompt		/* <r> string or descr: prompt string	*/
   ,struct descriptor  *dsc_val		/* <m> string returned		*/
   ,char  alphExtensions[]		/* <r:opt> other legal 1st chars */
   );
int   getEqualsFilename(
    void  *dsc_prompt		/* <r> string or descr: prompt string	*/
   ,struct descriptor  *dsc_val		/* <m> string returned		*/
   );
int   getYesno(
    void  *dsc_prompt		/* <r> string or descr: prompt string	*/
   ,int   ynDefault		/* <r> default:  1=Y  0=N		*/
   );
int   getCmd(
    void   *prompt		/* <r> Prompt string (char or dsc)	*/
   ,struct cmd_struct  cmd[]	/* <r> List of possible commands	*/
   ,char  defaultCmd[]		/* <r,opt> Default command string	*/
   ,int   flags			/* <r> Flags: 1=NoMsg			*/
   );

		/*======================================================
		 * General-purpose functions ...
		 *=====================================================*/
char  *getstr();
int   yesno(
    int   v		/* Default response: 0=false  nonZero=true	*/
   );
int   interactive();

int   init_timer();
int   show_timer();

char  *nodename();

char  *pgmname();
void  set_pgmname(
    char  *name
   );
char  *procname(
    int   pid		/* <r:opt> process id				*/
   );
char  *procname_dsc(
    int   pid			/* <r:opt> process id			*/
   ,struct descriptor  *dsc	/* <w> process name returned here	*/
   );

char  *trnlnm(
    char  name[],	/* <r> name to translate (environment variable)	*/
    int   *context	/* <w:opt> for compatability with VMS		*/
   );
char  *machine_name();

char  *cdate(long  usrtime);
char  *cdatime(long  usrtime);
char  *qdatime(long  usrtime[]);		/* vms only		*/
char  *now();
int   asc2time(
    char  ascString[]		/* <r> DateString: dd-mmm-yy[yy] hh:mm:ss */
   ,time_t  *bintim		/* <w> time integer			*/
   );

char  *fgets_with_edit(		/* Returns:  addr of usrline, or NULL	*/
    char  usrline[]		/* <w> user's input buffer		*/
   ,int   maxlen		/* <r> length of input buffer		*/
   ,FILE  *fp			/* <r> file pointer			*/
   ,char  *prompt		/* <r:opt> prompt string		*/
   );

#endif
