#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#ifdef vms
#include        <lib$routines.h>
#include        <ssdef.h>
#else
#include        <malloc.h>
#endif

#define _CASE_DEFINITIONS
#include        "cdudef.h"

/**********************************************************************
* CDU.C --
*
* Command-line parser, based on Digital's CDU
*
* History:
*  06-Oct-1997  TRG  Create.
*
***********************************************************************/


extern int   debugCdu;		/* set to 1 for extra printout		*/


#define STS_NOT_SUPPORTED      3	/* option not supported		*/
#define STS_EOF                2
#define STS_ILLEGAL_CMDLINE    4


static struct cmd_struct  cmd0[4] = {
        0,  (void *)(sizeof(cmd0)/sizeof(cmd0[0]))
       ,CMD_DEFINE,      "DEFINE"
       ,CMD_IDENT,       "IDENT"
       ,CMD_MODULE,      "MODULE"
       };
static struct cmd_struct  cmdDefine[4] = {
        0,   (void *)(sizeof(cmdDefine)/sizeof(cmdDefine[0]))
       ,DEFINE_VERB,     "VERB"
       ,DEFINE_SYNTAX,   "SYNTAX"
       ,DEFINE_TYPE,     "TYPE"
       };
static struct cmd_struct  cmdDefineVerb[10] = {
        0,   (void *)(sizeof(cmdDefineVerb)/sizeof(cmdDefineVerb[0]))
       ,VERB_DISALLOW,      "DISALLOW"
       ,VERB_NODISALLOWS,   "NODISALLOWS"
       ,VERB_IMAGE,         "IMAGE"
       ,VERB_PARAMETER,     "PARAMETER"
       ,VERB_NOPARAMETERS,  "NOPARAMETERS"
       ,VERB_QUALIFIER,     "QUALIFIER"
       ,VERB_NOQUALIFIERS,  "NOQUALIFIERS"
       ,VERB_ROUTINE,       "ROUTINE"
       ,VERB_SYNONYM,       "SYNONYM"
       };
static struct cmd_struct  cmdParam[5] = {
        0,   (void *)(sizeof(cmdParam)/sizeof(cmdParam[0]))
       ,PARAM_DEFAULT,      "DEFAULT"
       ,PARAM_LABEL,        "LABEL"
       ,PARAM_PROMPT,       "PROMPT"
       ,PARAM_VALUE,        "VALUE"
       };
static struct cmd_struct  cmdValue[8] = {
        0,   (void *)(sizeof(cmdValue)/sizeof(cmdValue[0]))
       ,VAL_CONCATENATE,    "CONCATENATE"
       ,VAL_DEFAULT,        "DEFAULT"
       ,VAL_LIST,           "LIST"
       ,VAL_NOCONCATENATE,  "NOCONCATENATE"
       ,VAL_REQUIRED,       "REQUIRED"
       ,VAL_TYPE,           "TYPE"
       };
static struct cmd_struct  cmdVerbQual[9] = {
        0,   (void *)(sizeof(cmdVerbQual)/sizeof(cmdVerbQual[0]))
       ,QUAL_BATCH,         "BATCH"
       ,QUAL_DEFAULT,       "DEFAULT"
       ,QUAL_LABEL,         "LABEL"
       ,QUAL_NEGATABLE,     "NEGATABLE"
       ,QUAL_NONNEGATABLE,  "NONNEGATABLE"
       ,QUAL_PLACEMENT,     "PLACEMENT"
       ,QUAL_SYNTAX,        "SYNTAX"
       ,QUAL_VALUE,         "VALUE"
       };
static struct cmd_struct  cmdType[2] = {
        0,   (void *)(sizeof(cmdType)/sizeof(cmdType[0]))
       ,TYPE_KEYWORD,       "KEYWORD"
       };
static struct cmd_struct  cmdKeyword[7] = {
        0,   (void *)(sizeof(cmdKeyword)/sizeof(cmdKeyword[0]))
       ,KEY_DEFAULT,        "DEFAULT"
       ,KEY_LABEL,          "LABEL"
       ,KEY_NEGATABLE,      "NEGATABLE"
       ,KEY_NONNEGATABLE,   "NONNEGATABLE"
       ,KEY_SYNTAX,         "SYNTAX"
       ,KEY_VALUE,          "VALUE"
       };
static struct cmd_struct  paramlist[9] = {
        0,   (void *)(sizeof(paramlist)/sizeof(paramlist[0]))
       ,1,"P1", 2,"P2", 3,"P3", 4,"P4", 5,"P5", 6,"P6", 7,"P7", 8,"P8"
       };
static struct cmd_struct  built_in_types[10] = {
        0,   (void *)(sizeof(built_in_types)/sizeof(built_in_types[0]))
       ,VAL_M_DATETIME,       "$DATETIME"
       ,VAL_M_DELTATIME,       "$DELTATIME"
       ,VAL_M_FILE,           "$FILE"
       ,VAL_M_NUMBER,         "$NUMBER"
       ,VAL_M_QUOTED_STRING,  "$QUOTED_STRING"
       ,VAL_M_REST_OF_LINE,   "$REST_OF_LINE"
       };
static char  *keywordFlags[] = {	/* for bits KEY_M_xxxxxxx	*/
                "DEFAULT","NEGATABLE","NONNEGATABLE"
               };
#define KEYWD_FLAGS_MAX  (sizeof(keywordFlags)/sizeof(keywordFlags[0]))

static char  *qualifierFlags[] = {	/* for bits QUAL_M_xxxxxxx	*/
                "DEFAULT","NEGATABLE","NONNEGATABLE"
               };
#define QUAL_FLAGS_MAX  (sizeof(qualifierFlags)/sizeof(qualifierFlags[0]))

static char  *parameterFlags[] = {	/* for bits PARAM_M_xxxxxxx	*/
                "DEFAULT","NEGATABLE","NONNEGATABLE"
               };
#define PARAM_FLAGS_MAX  (sizeof(parameterFlags)/sizeof(parameterFlags[0]))

static char  *verbFlags[] = {		/* for bits VERB_M_xxxxxx	*/
                "NODISALLOWS","NOPARAMETERS","NOQUALIFIERS"
               };
#define VERB_FLAGS_MAX  (sizeof(verbFlags)/sizeof(verbFlags[0]))


static int   errcnt;		/* Error count -- zero if success	*/
static int   paramId;		/* Param id# for current verb		*/
static int   syntaxCnt;		/* Number of "syntax" definitions	*/
static int   verbCnt;		/* Number of "verb" definitions		*/
static char  identString[20];
static char  moduleName[40];
static char  *psave;
static char  token[256];		/* utility array		*/
static DESCRIPTOR(dsc_token,token);	/*    "      "   descriptor	*/



	/****************************************************************
	 * cdu_lookup:
	 * cduGetString:
	 * cduGetCmd:
	 ****************************************************************/
static int   cdu_lookup(	/* Returns: cmd id#			*/
    char  **p			/* <m> addr in command line		*/
   ,struct cmd_struct  *cmdlist	/* <r> list of commands			*/
   ,char  *parentString		/* <r:opt> not used			*/
   ,int   flags			/* <r:opt> not used			*/
   ,int   *ucontext		/* <m:opt> not used			*/
   )
   {
    int   i,k;
    char  *t;
    char  userString[32];
		/* Note:  this routine takes the same args as cmd_lookup.
		 *   However, it does not extract a token from *p: rather,
		 *   it looks in cmdlist[] for an exact match to the
		 *   entire string *p <upper case>.
		 *   Ptr *p is not modified.  Several params are ignored.
		 *------------------------------------------------------*/

    l2u(userString,*p);
    k = (int)cmdlist[0].cmdA_string;	/* full dim of array		*/

    for (i=1 ; i<k ; i++)
       {
        t = cmdlist[i].cmdA_string;
        if (!t || !strcmp(userString,t))
            break;
       }
    return((t && i<k) ? cmdlist[i].cmdL_id : 0);
   }


static int   cduGetString(
    void  *prompt		/* <r> string or descr: prompt string	*/
   ,struct descriptor *dsc	/* <m> string value returned		*/
   ,char  alphExtensions[]	/* <r> other legal 1st chars		*/
   )
   {
    int   sts;

    dsc->dscA_pointer[0] = '\0';
    for ( ; ; )
       {
        psave = getString_linePtr();
        sts = getString(prompt,dsc,alphExtensions);
        if (!sts)
            return(0);
        if (*dsc->dscA_pointer != '!')
            break;			/* ok--> out of loop		*/

        clearnext(0);
       }
    if (!psave)
        psave = getString_startOfLine();
    return(sts);
   }


static int   cduGetCmd(
    void  *prompt		/* <r> prompt string: char or dsc	*/
   ,struct cmd_struct  cmd[]	/* <r> list of possible commands	*/
   ,char  defalutCmdString[]	/* <r:opt> cmd string for display	*/
   ,int   flags		/* <r> Flags: 1=NoMsg			*/
   )
   {
    int   opt;
    int   sts;
    char  *p;

    sts = cduGetString(prompt,&dsc_token,0);
    if (!sts)
        return(sts);

    p = token;
    opt = cdu_lookup(&p,cmd,defalutCmdString,flags,0);
    if (!opt)
        putNext(token,2);	/* replace at start of input stream	*/

    return(opt);
   }



	/*****************************************************************
	 * notHandlingOption:
	 * illegalCmdline:
	 *  Warning messages for one situation or another ...
	 *****************************************************************/
static int   notHandlingOption(	/* Return: status			*/
    char  *pp			/* <r> start of "problem"		*/
   )
   {
    int   k;
    char  *p;
    char  *startOfCmdline;
    char  fmt[20];

    p = pp;
    startOfCmdline = getString_startOfLine();
    ascToken(&p,&dsc_token,0,0);
    k = pp - startOfCmdline;
    printf("\n-->Not handling option '%s'\n",token);
    printf("   Command line = '%.52s'\n",startOfCmdline);
    if (k>=0 && k<50)
       {
        sprintf(fmt,"%%%ds^^^\n",20+k);
        printf(fmt,"");
       }
    printf("\n");
    return(STS_NOT_SUPPORTED);
   }


static int   illegalCmdline(	/* Return: status			*/
    char  *pp			/* <r> start of "problem"		*/
   ,char  text[]		/* <r:opt> extra error text		*/
   )
   {
    char  *p;
    char  *startOfCmdline;
	/*
    int   k;
    char  fmt[20];
	*/

    startOfCmdline = getString_startOfLine();
    p = startOfCmdline[0] ? startOfCmdline : "<null>";
    printf("\n--> Illegal cmd line: '%.48s'\n",p);
/*
    if (k>=0 && k<48)
       {
        sprintf(fmt,"%%%ds^^^\n",23+k);
        printf(fmt,"");
       }
*/
    if (text)
        printf("    %s\n",text);

    printf("\n");
    errcnt++;
#ifdef  vms
    lib$signal(SS$_DEBUG);
#endif
    clearnext(0);
    return(0);
   }



	/****************************************************************
	 * show_value:
	 * display value struct ...
	 ****************************************************************/
static void  show_value(
    struct valueClause  *val	/* <r> valueClause struct for display	*/
   )
   {
    int   i,k;
    int   mask;
    static char  *maskText[] = {
                     "USER_DEFINED_TYPE","FILE","NUMBER","QUOTED_STRING"
                    ,"DATETIME","REST_OF_LINE","DELTATIME<notSupported>"
                    ,0,0,0,0,0,"LIST","REQUIRED"
                    };

    printf("   Value: Flags=");
    mask = val ? val->valL_flags : 0;
    if (!mask)
        printf("<none>");

    for (i=0,k=0 ; mask ; i++,mask>>=1)
       {
        if (mask & 1)
            printf("%s%s",k++?",":"", maskText[i]?maskText[i]:"???");
       }

    if (val->valA_userType)
        printf("  userType='%s'",val->valA_userType);

    if (val->valA_default)
        printf("  default='%s'",val->valA_default);
    printf("\n");
    return;
   }



	/****************************************************************
	 * show_parameter:
	 ****************************************************************/
static void  show_parameter(	/* informational only			*/
    struct paramClause  *prm	/* <r> parameter to display		*/
   )
   {
    int   i,k;
    int   mask;
    char  *p;

    printf("  %s:",prm->prmA_name);
    if (prm->prmL_flags)
       {
        printf("  Flags=");
        mask = prm->prmL_flags;
        for (i=0,k=0 ; mask ; i++,mask>>=1)
           {
            if (mask & 1)
               {
                p = (i<PARAM_FLAGS_MAX) ? parameterFlags[i] : "???";
                printf("%s%s",k++?",":"", p);
               }
           }
       }

    if (prm->prmA_label)
        printf("  label=%s",prm->prmA_label);
    if (prm->prmA_prompt)
        printf("  prompt=%s",prm->prmA_prompt);

    if (prm->prmA_value)
       {
        if (prm->prmL_flags || prm->prmA_label || prm->prmA_prompt)
            printf("\n    ");
/*        show_value(prm->prmA_value);	/*  */
       }
    else
        printf("\n");
    return;
   }



	/****************************************************************
	 * show_qualifier:
	 ****************************************************************/
static void  show_qualifier(	/* informational only			*/
    struct qualifier  *q	/* <r> qualifier to display		*/
   )
   {
    int   i,k;
    int   mask;
    char  *p;

    printf("  Qualifier %-8s:",q->qualA_name);
    if (!q->qualL_flags)
        printf("  Flags=<none>");
    else
       {
        printf("  Flags=");
        mask = q->qualL_flags;
        for (i=0,k=0 ; mask ; i++,mask>>=1)
           {
            if (mask & 1)
               {
                p = (i<QUAL_FLAGS_MAX) ? qualifierFlags[i] : "???";
                printf("%s%s",k++?",":"", p);
               }
           }
       }

    if (q->qualA_label)
        printf("  label=%s",q->qualA_label);
    if (q->qualA_syntax)
        printf("  syntax=%s",q->qualA_syntax);

    if (q->qualA_value)
       {
        printf("\n    ");
/*        show_value(q->qualA_value);	/*  */
       }
    else
        printf("\n");
    return;
   }



	/****************************************************************
	 * show_type:
	 ****************************************************************/
static void  show_type(
    char  typeName[]		/* <r> typeName to display		*/
   )
   {
    int   i;
    int   idx;
    int   mask;
    char  *p;
    struct keyword  *key;

    printf("\nType: %s\n",typeName);

    for (idx=0 ; key=findKeywordByIdx(typeName,idx) ; idx++)
       {
        printf("    Keyword %-8s:",key->keyA_name);
        if (key->keyA_label)
            printf("  label=%s",key->keyA_label);
        if (key->keyA_syntax)
            printf("  syntax=%s",key->keyA_syntax);

        if (mask = key->keyL_flags)
            printf("  Flags=");
        for ( ; mask ; mask>>=1)
           {
            if (!(mask & 1))
                continue;
            p = (i<KEYWD_FLAGS_MAX) ? keywordFlags[i] : "???";
            printf(" %s",p?p:"unknown");
           }
        printf("\n");
       }
    printf("\n");
    return;
   }



	/****************************************************************
	 * show_verb:
	 ****************************************************************/
static void  show_verb(		/* Informational only			*/
    struct verblist  *v		/* <r> verb for display			*/
   ,int   callerOpt		/* <r> DEFINE_VERB or DEFINE_SYNTAX	*/
   )
   {
    int   i,k;
    int   mask;
    char  *p;

    printf("\n%s %s",
            (callerOpt==DEFINE_VERB)?"Verb":"Syntax",v->vrbA_name);

    if (v->vrbL_flags)
       {
        printf("  Flags=");
        mask = v->vrbL_flags;
        for (i=0,k=0 ; mask ; i++,mask>>=1)
           {
            if (mask & 1)
               {
                p = (i<VERB_FLAGS_MAX) ? verbFlags[i] : "???";
                printf("%s%s",k++?",":"", p);
               }
           }
        printf("\n      ");
       }

    if (v->vrbA_image)
        printf("  image=%s",v->vrbA_image);
    if (v->vrbA_routine)
        printf("  routine=%s",v->vrbA_routine);
    if (v->vrbA_disallow)
        printf("  disallow=%s",v->vrbA_disallow);

/*    printf("\n");
/*    for (paramId=1 ; prm=findParamByIdx(v,paramId) ; paramId++)
/*        show_parameter(prm);
/*
/*    for (i=0 ; q=findQualifierByIdx(v,i) ; i++)
/*        show_qualifier(q);			/*  */

    printf("\n");
    return;
   }



	/***************************************************************
	 * cmd_value:
	 ***************************************************************/
static struct valueClause  *cmd_value()	/* Return: addr of structure	*/
   {
    int   mask;
    int   opt;
    char  cmd[32];
    char  *p;
    char  typeString[32];
    struct valueClause  *val;

    val = malloc(sizeof(struct valueClause));
    clear_buffer(val,sizeof(struct valueClause));

    p = getString_linePtr();
    if (!p || *p!='(')
       {
        return(val);
       }

    bumpString_linePtr();
    for ( ; ; )
       {
        p = getString_linePtr();
        if (p && *p==',')
            bumpString_linePtr();

        if (!cduGetString("    valueOpt",&dsc_token,0))
            return((void *)illegalCmdline(0,0));

        p = strncpy(cmd,token,sizeof(cmd)-1);
        if (*p == ')')
            break;		/*--> end of VALUE(...) spec		*/
        opt = cdu_lookup(&p,cmdValue,0,NOMSG,0);
        if (!opt)
            return((void *)illegalCmdline(0,0));

        switch(opt)
           {
            case VAL_CONCATENATE:
            case VAL_NOCONCATENATE:
                notHandlingOption(psave);
                break;

            case VAL_DEFAULT:
                if (!getEqualsString("=defaultVal",&dsc_token,0))
                   {
                    illegalCmdline(0,"requires default string");
                    return(0);
                   }
                l2u(token,0);
                val->valA_default = STRING_ALLOC(token);
                break;

            case VAL_LIST:
                val->valL_flags |= VAL_M_LIST;
                break;

            case VAL_REQUIRED:
                val->valL_flags |= VAL_M_REQUIRED;
                break;

            case VAL_TYPE:
                if (!getEqualsString("=type",&dsc_token,0))
                    return((void *)illegalCmdline(psave,"unexpected eof"));
                l2un(typeString,token,sizeof(typeString)-1);
                p = typeString;
                mask = cdu_lookup(&p,built_in_types,0,NOMSG,0);
                if (mask)
                    val->valL_flags |= mask;
                else
                   {
                    val->valL_flags |= VAL_M_USER_DEFINED_TYPE;
                    val->valA_userType = STRING_ALLOC(typeString);
                   }
                break;
           }
       }
    return(val);
   }



	/***************************************************************
	 * cmd_verbQualifier:
	 ***************************************************************/
static int   cmd_verbQualifier(		/* Return: status		*/
    struct verblist  *v		/* <m> verblist struct to fill in	*/
   )
   {
    int   opt;
    char  cmd[32];
    char  *p;
    char  qualName[32];
    struct qualifier  *q;

    if (!cduGetString("  Qualifier name",&dsc_token,0))
        return(illegalCmdline(psave,"requires Qualifier name"));

    l2u(qualName,token);
    q = getQualifier(v,qualName);

    for ( ; ; )
       {
        p = getString_linePtr();
        if (p && *p==',')
            bumpString_linePtr();

        if (!cduGetString("  Qualifier option",&dsc_token,0))
            return(0);

        strncpy(cmd,token,sizeof(cmd)-1);

        p = cmd;
        opt = cdu_lookup(&p,cmdVerbQual,0,NOMSG,0);
        if (!opt)
           {
            putNext(cmd,2);		/* replace cmd (atStart)	*/
            return(0);
           }
        switch(opt)
           {
            default:
                return(illegalCmdline(0,0));

            case QUAL_BATCH:
                notHandlingOption(psave);
                break;

            case QUAL_DEFAULT:
                q->qualL_flags |= QUAL_M_DEFAULT;
                break;

            case QUAL_LABEL:
                if (!getEqualsString("  Label-name",&dsc_token,0))
                    return(illegalCmdline(psave,"requires label name"));
                l2u(token,0);
                q->qualA_label = STRING_ALLOC(token);
                break;

            case QUAL_NEGATABLE:
                q->qualL_flags |= QUAL_M_NEGATABLE;
                break;

            case QUAL_NONNEGATABLE:
                q->qualL_flags |= QUAL_M_NONNEGATABLE;
                break;

            case QUAL_SYNTAX:
                if (!getEqualsString("  Syntax-name",&dsc_token,0))
                    return(illegalCmdline(psave,"requires syntax name"));
                l2u(token,0);
                q->qualA_syntax = STRING_ALLOC(token);
                break;

            case QUAL_VALUE:
                q->qualA_value = cmd_value();
                break;
           }
       }
   }



	/***************************************************************
	 * cmd_verbParameter:
	 * Called after DEFINE VERB PARAMETER ...
	 ***************************************************************/
static int   cmd_verbParameter(	/* Return: status			*/
    struct verblist  *v		/* <m> verblist struct to fill in	*/
   )
   {
    int   k;
    int   opt;
    int   sts;
    char  cmd[32];
    char  *p;
    struct paramClause  *prm;

    k = cduGetCmd(0,paramlist,0,NOMSG);
    if (!k)
        return(illegalCmdline(psave,"Requires param number Pn"));

    if (k != paramId+1)
        return(illegalCmdline(psave,"Param id numbers not in order"));
    paramId = k;

    sts = 1;
    prm = getParamByIdx(v,paramId);

    for ( ; ; )
       {
        p = getString_linePtr();
        if (p && *p==',')
            bumpString_linePtr();

        if (!cduGetString("    Parameter option",&dsc_token,0))
           {
            sts = STS_EOF;
            break;			/* out of loop			*/
           }

        strncpy(cmd,token,sizeof(cmd)-1);

        p = cmd;
        opt = cdu_lookup(&p,cmdParam,0,NOMSG,0);
        if (!opt)
           {
            putNext(cmd,2);		/* replace cmd (atStart)	*/
            sts = 0;
            break;			/* out of loop			*/
           }
        switch(opt)
           {
            default:
                return(illegalCmdline(0,0));

            case PARAM_DEFAULT:
                prm->prmL_flags |= PARAM_M_DEFAULT;
                break;

            case PARAM_LABEL:
                if (!getEqualsString(0,&dsc_token,0))
                    return(illegalCmdline(p,"Requires label name"));
                l2u(token,0);
                prm->prmA_label = STRING_ALLOC(token);
                break;

            case PARAM_PROMPT:
                if (!getEqualsString(0,&dsc_token,0))
                    return(illegalCmdline(p,"Requires prompt string"));
                prm->prmA_prompt = STRING_ALLOC(token);
                break;

            case PARAM_VALUE:
                prm->prmA_value = cmd_value();
                break;
           }
       }
    if (!prm->prmA_value)
       {			/* parameter MUST have a value struct!	*/
        prm->prmA_value = malloc(sizeof(struct valueClause));
        clear_buffer(prm->prmA_value,sizeof(struct valueClause));
       }
    return(sts);
   }



	/**************************************************************
	 * cmd_defineVerb:
	 * Called for "DEFINE SYNTAX" and DEFINE VERB" commands ...
	 **************************************************************/
static int   cmd_defineVerb(
    int   callerOpt		/* <r> DEFINE_VERB or DEFINE_SYNTAX	*/
   )
   {
    int   sts;
    int   opt;
    char  cmd[32];
    char  *p;
    char  verbName[32];
    int   define_syntax_flag;
    struct verblist  *v;

    paramId = 0;		/* Initialize paramId for new verb	*/
    define_syntax_flag = (callerOpt == DEFINE_SYNTAX);

    if (!cduGetString("VerbName",&dsc_token,0))
        return(illegalCmdline(psave,"unexpected end of input"));
    l2u(verbName,token);


		/*======================================================
		 * Create entry for new verb or syntax element ...
		 *=====================================================*/
    v = (callerOpt==DEFINE_VERB) ? newVerb(verbName) : newSyntax(verbName);

    for ( ; ; )
       {
        p = getString_linePtr();
        if (p && *p==',')
            bumpString_linePtr();

        if (!cduGetString("  Verb-clause option",&dsc_token,0))
            break;
        strncpy(cmd,token,sizeof(cmd)-1);

        p = cmd;
        opt = cdu_lookup(&p,cmdDefineVerb,"DEFINE VERB",NOMSG,0);
        if (!opt)
           {
            putNext(cmd,2);		/* replace cmd (atStart)	*/
            if (debugCdu)
                show_verb(v,callerOpt);	/*  */
            return(0);
           }
        switch(opt)
           {
            case VERB_DISALLOW:
                sts = debugCdu ? notHandlingOption(0) : STS_NOT_SUPPORTED;
                clearnext(debugCdu);
                break;

            case VERB_NODISALLOWS:
                v->vrbL_flags |= VERB_M_NODISALLOWS;
                break;

            case VERB_NOPARAMETERS:
                v->vrbL_flags |= VERB_M_NOPARAMETERS;
                break;

            case VERB_NOQUALIFIERS:
                v->vrbL_flags |= VERB_M_NOQUALIFIERS;
                break;

            case VERB_IMAGE:
                if (!getFilename(0,&dsc_token))
                    return(illegalCmdline(psave,"Requires image-filename"));
                v->vrbA_image = STRING_ALLOC(token);
                break;

            case VERB_PARAMETER:
                sts = cmd_verbParameter(v);
                break;

            case VERB_QUALIFIER:
                sts = cmd_verbQualifier(v);
                break;

            case VERB_ROUTINE:
                if (!cduGetString("    Routine",&dsc_token,0))
                    return(illegalCmdline(psave,"Requires routine-name"));
                v->vrbA_routine = STRING_ALLOC(token);
                break;

            case VERB_SYNONYM:
                if (define_syntax_flag)
                    return(illegalCmdline(psave,"Not allowed in SYNTAX"));
                if (!cduGetString("    Synonym",&dsc_token,0))
                    return(illegalCmdline(psave,"Requires synonym name"));
                printf("    Synonym = '%s'\n\n",token);
                printf("      *NOTE* NOT HANDLING SYNONYMS AT THIS TIME\n");
                break;
           }
       }
    if (debugCdu)
        show_verb(v,callerOpt);		/* you're here upon eof	*/
    return 1;
   }



	/*****************************************************************
	 * cmd_parseKeywordClauses:
	 *****************************************************************/
static int   cmd_parseKeywordClauses(
    struct keyword  *key	/* <m> keyword struct to fill in	*/
   )
   {
    int   opt;
    char  cmd[32];
    char  *p;

    for ( ; ; )
       {
        p = getString_linePtr();
        if (p && *p==',')
            bumpString_linePtr();

        if (!cduGetString("    Keyword option",&dsc_token,0))
            break;
        strncpy(cmd,token,sizeof(cmd)-1);

        p = cmd;
        opt = cdu_lookup(&p,cmdKeyword,"KEYWORD",NOMSG,0);
        if (!opt)
           {
            putNext(cmd,2);		/* replace cmd (atStart)	*/
            return(0);
           }
        switch(opt)
           {
            default:
                notHandlingOption(psave);
                break;

            case KEY_DEFAULT:
                key->keyL_flags |= KEY_M_DEFAULT;
                break;

            case KEY_LABEL:
                if (!getEqualsString(0,&dsc_token,0))
                    return(illegalCmdline(p,"Requires label name"));
                l2u(token,0);
                key->keyA_label = STRING_ALLOC(token);
                break;

            case KEY_NEGATABLE:
                key->keyL_flags |= KEY_M_NEGATABLE;
                break;

            case KEY_NONNEGATABLE:
                key->keyL_flags |= KEY_M_NONNEGATABLE;
                break;

            case KEY_SYNTAX:
                if (!getEqualsString(0,&dsc_token,0))
                    return(illegalCmdline(p,"Requires syntax name"));
                l2u(token,0);
                key->keyA_syntax = STRING_ALLOC(token);
                break;

            case KEY_VALUE:
                key->keyA_value = cmd_value();
                break;
           }
       }
    return 1;
   }



	/*****************************************************************
	 * cmd_defineType:
	 * A "type" is merely a list of keywords, each with associated
	 * parameters.
	 *****************************************************************/
static int   cmd_defineType()	/* Return: status			*/
   {
    char  cmd[32];
    char  *p;
    char  typeName[32];
    char  keywordName[32];
    struct keyword  *key;

    if (!cduGetString("typeName",&dsc_token,0))
        return(illegalCmdline(psave,"unexpected end of input"));
    l2u(typeName,token);

    for ( ; ; )
       {
        p = getString_linePtr();
        if (p && *p==',')
            bumpString_linePtr();

        if (!cduGetString("  'Type' option (i.e. KEYWORD)",&dsc_token,0))
            break;
        l2un(cmd,token,sizeof(cmd)-1);
        if (strcmp(cmd,"KEYWORD"))
           {
            putNext(cmd,2);		/* replace cmd (atStart)	*/
/*            show_type(typeName);	/*  */
            return(0);
           }

		/*--------------------------------------------------------
		 * Get the keyword name;  get keyword struct ...
		 *-------------------------------------------------------*/
        if (!cduGetString("    Keyword name",&dsc_token,0))
            break;
        l2un(keywordName,token,sizeof(keywordName)-1);

        key = getKeywordStruct(typeName,keywordName);
        key->keyA_name = STRING_ALLOC(keywordName);

        cmd_parseKeywordClauses(key);	/* fill in "key" struct		*/
       }
    return 1;
   }


	/****************************************************************
	 * cmd_define:
	 ****************************************************************/
static int   cmd_define()	/* Return: status			*/
   {
    int   opt;
    int   sts;
    char  cmd[32];
    char  *p;

    if (!cduGetString("'Define' option",&dsc_token,0))
        return(illegalCmdline(psave,"Unexpected end of line"));
    strncpy(cmd,token,sizeof(cmd)-1);

    p = cmd;
    opt = cdu_lookup(&p,cmdDefine,"DEFINE",NOMSG,0);
    if (!opt)
       {
        putNext(cmd,2);		/* replace cmd (atStart)	*/
        return(illegalCmdline(psave,0));
       }

    switch(opt)
       {
        default:
            sts = notHandlingOption(psave);
            break;

        case DEFINE_VERB:
        case DEFINE_SYNTAX:
            sts = cmd_defineVerb(opt);
            break;

        case DEFINE_TYPE:
            sts = cmd_defineType();
            break;
       }
    return(sts);
   }



	/******************************************************************
	 * cmd_ident:
	 ******************************************************************/
static int   cmd_ident()	/* Return: status			*/
   {
    if (!cduGetString("Ident string",&dsc_token,0))
        return(illegalCmdline(0,"Requires IDENT name"));
    strncpy(identString,token,sizeof(identString)-1);
    printf("\nIdent = '%s'\n",identString);
    return(1);
   }



	/******************************************************************
	 * cmd_module:
	 ******************************************************************/
static int   cmd_module()	/* Return: status			*/
   {
    if (!cduGetString("Module name",&dsc_token,0))
        return(illegalCmdline(0,"Requires MODULE name"));
    strncpy(moduleName,token,sizeof(moduleName)-1);
    if (interactive())
       printf("\nModule = '%s'\n",moduleName);
    return(1);
   }



	/****************************************************************
	 * cdu_parse:
	 ****************************************************************/
static int   cdu_parse(		/* Return: status			*/
   )
   {
    int   opt;
    int   sts;
    char  cmd[32];
    char  *p;

    for ( ; ; )
       {
        if (!cduGetString("Enter >  ",&dsc_token,0))
            break;
        strncpy(cmd,token,sizeof(cmd)-1);

        p = cmd;
        opt = cdu_lookup(&p,cmd0,0,NOMSG,0);
        if (!opt)
            return(illegalCmdline(0,0));

        switch(opt)
           {
            default:
                sts = 0;
                printf("Can't handle command line\n");
                break;

            case CMD_DEFINE:
                sts = cmd_define();
                break;

            case CMD_IDENT:
                sts = cmd_ident(p);
                break;

            case CMD_MODULE:
                sts = cmd_module(p);
                break;
           }
       }
    return(sts);
   }



	/*****************************************************************
	 * main:
	 *****************************************************************/
int  main(
    int   argc
   ,char  *argv[]
   )
   {
    int   sts;
    static int   flags;

    termio_init(argc,argv);

    sts = cdu_parse();

    cdu_write(moduleName);
    exit(0);
    return sts;
   }
