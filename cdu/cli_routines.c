#include        <ctype.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        "clisysdef.h"
#ifdef vms
#include        <lib$routines.h>
#include        <ssdef.h>
#endif

/*********************************************************************
* CLI_ROUTINES.C --
*
* Emulation of Digital's CLI routines.
*
* History:
*  19-Jan-1998  TRG  find_entity: disable searching for keywords below
*                     the current level.
*  14-Jan-1998  TRG  Use IS_QUALIFIER_CHARACTER() to check for qual.
*  16-Oct-1997  TRG  Create.
*
**********************************************************************/


#define TRUE     1
#define FALSE    0
#define LEGAL_STATUS(S)  (((S)&1) || (S)==CLI_STS_NEGATED)


extern char  *cliNonblank();
extern int   cliToken();


static struct cduVerb  *currentTable;	/* addr of current table	*/
static struct cduVerb  *currentSyntax;	/* addr of current verb/syntax	*/
static struct cduParam  *currentParameters;	/* current Parameters	*/
static struct cduQualifier  *currentQualifiers; /* current Qualifiers	*/
static long  paramId;			/* current param Id# (1-based)	*/

static struct cmd_struct  *cmdVerb;	/* lookup list			*/
static struct cmd_struct  *cmdParam;	/* lookup list			*/
static struct cmd_struct  *cmdQual;	/* lookup list			*/
static int   cliDebug;			/* "Debug" flag			*/
static int   maxVerbs;			/* max size of cmdVerb[]	*/
static int   maxQualifiers;		/* max size of cmdQual[]	*/
static int   maxParams;			/* max size of cmdParam[]	*/
static int   numVerbs;			/* num of cmdVerb[] entries	*/
static int   numQualifiers;		/* num of cmdQual[] entries	*/
static int   numParams;			/* num of cmdParam[] entries	*/

static DYNAMIC_DESCRIPTOR(dsc_token);
static DYNAMIC_DESCRIPTOR(dsc_cmdline);



	/****************************************************************
	 * init_value:
	 ****************************************************************/
static int   init_value(	/* Return: initial "processing" status	*/
    struct cduValue  *val	/* <m> value structure			*/
   )
   {
    int   i,k;
    int   sts;
    struct cduChain  *chain,*chainNext;
    struct cduKeyword  *key;

    sts = 0;
    if (!val)
        return(0);

		/*-------------------------------------------------------
		 * Free any old values which might be attached ...
		 *------------------------------------------------------*/
    str_free1_dx(&val->val_dsc);
    val->valA_substring = 0;

		/*--------------------------------------------------------
		 * Init keywords ...
		 *-------------------------------------------------------*/
    if (key = val->valA_userType)
       {
        for ( ; key->keyA_name ; key++)
            key->keyL_status = init_value(key->keyA_value);
       }

		/*--------------------------------------------------------
		 * Check for default value.
		 *-------------------------------------------------------*/
    if (val->valA_default)
       {
        str_copy_dx(&val->val_dsc,val->valA_default);
        val->valA_substring = val->val_dsc.dscA_pointer;
        sts = CLI_STS_DEFAULTED;
       }
    return(sts);
   }



#include  "clisysdef.h"

	/****************************************************************
	 * initLookupTable:
	 ****************************************************************/
static int   initLookupTable(	/* Return: num entries in cmd-lookup table*/
    struct cduEntity  *entTop	/* <r> top of entity array		*/
   ,struct cmd_struct  **addrTable /* <m> start of "lookup" table	*/
   )
   {
    int   i,k;
    int   icnt;
    int   maxEntities;
    struct cduEntity  *e;
    struct cduVerb  *v;
    struct cmd_struct  *entLookup;

    if (!entTop)
        return(0);

		/*=====================================================
		 * Count num entries in entTop[] table;
		 * Check size of entLookup[] list ...
		 *====================================================*/
    e = entTop;
    for (icnt=0 ; e->entA_name ; icnt++,e++)
        ;

    entLookup = *addrTable;
    maxEntities = entLookup ? ((int)entLookup[0].cmdA_string-1) : 0;
    if (icnt > maxEntities)
       {			/*..need larger entLookup[] table	*/
        maxEntities = icnt+2;	/*....allow a little extra ..		*/
        if (entLookup)
            free(entLookup);	/*....free old entLookup[]		*/
        entLookup = malloc((maxEntities+1)*sizeof(struct cmd_struct));
        if (!entLookup)
            exit(cli_error(0,"*EXIT* Out of space!"));
        *addrTable = entLookup;
        clear_buffer(entLookup,(maxEntities+1)*sizeof(struct cmd_struct));
        entLookup[0].cmdL_id = 0;
        entLookup[0].cmdA_string = (void *)(maxEntities+1);
       }

		/*======================================================
		 * Fill in the entLookup[] table ...
		 *=====================================================*/
    e = entTop;
    for (i=1 ; i<=icnt ; i++,e++)
       {
        entLookup[i].cmdL_id = i;
        entLookup[i].cmdA_string =
                e->entA_label ? e->entA_label : e->entA_name;
       }
    if (i < maxEntities)
       {
        entLookup[i].cmdL_id = 0;
        entLookup[i].cmdA_string = 0;
       }
    return(icnt);
   }



	/***************************************************************
	 * initEntityTable:
	 ***************************************************************/
static int   initEntityTable(	/* Return: num entities defined		*/
    void  *entTop		/* <r> top of paramList or qualList	*/
   ,struct cmd_struct  **addrTable /* <m> top of entity "lookup" table	*/
   )
   {
    int   i,k;
    int   icnt;
    int   sts;
    struct cduEntity  *e;
    struct cduVerb  *v;

    if (!entTop)
        return(0);

		/*=====================================================
		 * Check size of entLookup[] list ...
		 *====================================================*/
    icnt = initLookupTable(entTop,addrTable);

    e = entTop;
    for (i=1 ; i<=icnt ; i++,e++)
       {
        sts = init_value(e->entA_value);
        e->entL_status =
                (e->entL_flags & ENT_M_DEFAULTED) ? CLI_STS_DEFAULTED : sts;
       }
    return(icnt);
   }



	/*****************************************************************
	 * init_table:
	 *****************************************************************/
static int   init_table(	/* Return: num verbs defined		*/
    struct cduVerb  *table	/* <r> addr of command table		*/
   )
   {
    int   i,k;
    int   icnt;
    struct cmd_struct  *cmd;
    struct cduVerb  *v;

    if (table == currentTable)
        return;

    currentTable = table;

		/*=====================================================
		 * Check size of cmdVerb list ...
		 *====================================================*/
    for (icnt=0,v=table ; v->vrbA_name ; icnt++,v++)
        ;
    if (icnt > maxVerbs)
       {			/*...need larger cmdVerb[] table	*/
        maxVerbs = icnt;
        if (cmdVerb)
            free(cmdVerb);	/*....free old cmdVerb[]		*/
        cmdVerb = malloc((maxVerbs+1)*sizeof(struct cmd_struct));
        if (!cmdVerb)
           {
            fprintf(stderr,"*EXIT* Out of space!\n");
            exit(0);
           }
        clear_buffer(cmdVerb,(maxVerbs+1)*sizeof(struct cmd_struct));
        cmdVerb[0].cmdL_id = 0;
        cmdVerb[0].cmdA_string = (void *)(maxVerbs+1);
       }
		/*======================================================
		 * Initialize cmdVerb[] table ...
		 *=====================================================*/
    clear_buffer(cmdVerb+1,maxVerbs*sizeof(struct cmd_struct));
    for (i=1,v=table ; i<=icnt ; i++,v++)
       {
        cmdVerb[i].cmdL_id = i;
        cmdVerb[i].cmdA_string = v->vrbA_name;
       }
    return;
   }



	/***************************************************************
	 * setCurrentSyntax:
	 * clearCurrentSyntax:
	 ***************************************************************/
static void  setCurrentSyntax(
    int   flag			/* <r> T==>newVerb  F==>newSyntax only	*/
   ,struct cduVerb  *v		/* <r> addr of new syntax struct	*/
   )
   {
    int   i,k;
    struct cduVerb  *oldSyntax;
    struct cduParam  *oldPrms;
    struct cduValue  *val,*oldval;

    oldSyntax = currentSyntax;
    currentSyntax = v;
    if (flag || v->vrbA_parameters)
       {
        numParams = initEntityTable(v->vrbA_parameters,&cmdParam);
        oldPrms = currentParameters;
        currentParameters = v->vrbA_parameters;

        if ((k=paramId) > numParams)
           {
            fprintf(stderr,
                "\nsetCurrentSyntax: *WARN* paramId=%d  numParams=%d\n\n",
                paramId,numParams);
#ifdef vms
            lib$signal(SS$_DEBUG);
#endif
            k = numParams;
           }
        for (i=0 ; i<k ; i++)
           {		/* Copy val_dsc and status for each param ...	*/
            val = currentParameters[i].prmA_value;
            oldval = oldPrms[i].prmA_value;
            str_copy_dx(&val->val_dsc,&oldval->val_dsc);
            currentParameters[i].prmL_status = oldPrms[i].prmL_status;
           }
       }
    if (flag || v->vrbA_qualifiers)
       {
        numQualifiers = initEntityTable(v->vrbA_qualifiers,&cmdQual);
        currentQualifiers = v->vrbA_qualifiers;
       }
    return;
   }

static void  clearCurrentSyntax()
   {
    currentSyntax = 0;
    numParams = 0;
    numQualifiers = 0;
    return;
   }



	/*****************************************************************
	 * readCliValueList:
	 *****************************************************************/
static int   readCliValueList(	/* Return: status			*/
    char  **pp			/* <m> current location in cmd line	*/
   ,struct cduValue  *val	/* <m> the value struct			*/
   )
   {
    int   i,k;
    int   sts;
    char  *p;

    if (!*pp || **pp != '(')
        return(cli_error(CLI_STS_ERROR,"called under false pretenses"));

    p = *pp + 1;		/* start of multi-valued token		*/
    str_free1_dx(&val->val_dsc);
    for ( ; ; )
       {
        sts = cliToken(&p,&dsc_token,val->valL_flags);
        if (~sts & 1)
            break;

        str_append(&val->val_dsc,&dsc_token);
        if (p = nonblank(p))
           {
            if (*p == ')')
                break;
            else if (*p == ',')
                p++, str_append(&val->val_dsc,"\01");
            else if (*p == '+')
                p++, str_append(&val->val_dsc,"\02");
           }
       }
    if (!p || (*p != ')'))
        return(cli_error(CLI_STS_ERROR,"Error in value list")); /* return */
    *pp = cliNonblank(p+1);
    return(1);				/*---------------------> return	*/
   }



	/*****************************************************************
	 * set_value:
	 *****************************************************************/
static long  set_value(		/* Return: CLI_STS_xxxx			*/
    char  **pp			/* <m> current location in cmd line	*/
   ,struct cduValue  *val	/* <m> the value struct			*/
   )
   {
    int   i,k;
    int   keycnt;
    int   opt;
    int   sts;
    char  *p,*p2;
    struct cmd_struct  *cmd;
    struct cduKeyword  *key;
    static DYNAMIC_DESCRIPTOR(dsc_temp);

    if ((val->valL_flags & VAL_M_LIST) && **pp=='(')
        sts = readCliValueList(pp,val);
    else
        sts = cliToken(pp,&val->val_dsc,val->valL_flags);

    if (~sts & 1)
        return(cli_error(CLI_STS_BADLINE,"Invalid data in line"));

    val->valA_substring = val->val_dsc.dscA_pointer;

		/*=======================================================
		 * Keywords ?
		 *======================================================*/
    if (!val->valA_userType)
        return(CLI_STS_PRESENT);	/*--------------- Nope: return	*/

		/*-------------------------------------------------------
		 *...yes: check each input for keyword ...
		 *------------------------------------------------------*/
    cmd = make_lookup_keyword(val->valA_userType);
    p = val->val_dsc.dscA_pointer;
    str_free1_dx(&dsc_temp);
    keycnt = 0;
    for ( ; p ; keycnt++)
       {
        if (keycnt)
           {			/* skip separator character		*/
            if (*p=='\01' || *p=='\02')
               {
                str_append(&dsc_temp,(*p=='\01')?"\01":"\02");
                p++;
               }
            else
               {
                fprintf(stderr,"Illegal separator character!\n");
#ifdef vms
                lib$signal(SS$_DEBUG);
#endif
               }
           }
        opt = cmd_lookup(&p,cmd,0,NOMSG,0);
        if (opt)
           {
            key = val->valA_userType + (opt-1);
            key->keyL_status = CLI_STS_PRESENT;
            str_append(&dsc_temp,key->keyA_name);
            continue;			/* back to top of loop ...	*/
           }

		/*-------------------------------------------------------
		 * Check for negated keyword ...
		 *------------------------------------------------------*/
        if (toupper(p[0])=='N' && toupper(p[1])=='O')
           {
            p += 2;
            opt = cmd_lookup(&p,cmd,0,NOMSG,0);
            if (opt)
               {
                key = val->valA_userType + (opt-1);
                if (!(key->keyL_flags & KEY_M_NEGATABLE))
                   {
                    free(cmd);
                    return(cli_error(CLI_STS_NOTNEGATABLE,"not negatable"));
                   }
                key->keyL_status = CLI_STS_NEGATED;
                str_append(&dsc_temp,key->keyA_name);
                continue;		/* back to top of loop ...	*/
               }
           }

        free(cmd);
        return(CLI_STS_IVKEYW);		/*---> return: Invalid keyword	*/
		/* Note: this can be a legal sts.  For example "DEFINE"'s
		 *  P2 might be the keyword "SERVER" or a macro name.
		 *  The latter would appear as an "invalid keyword".
		 *------------------------------------------------------*/
       }

		/*========================================================
		 * Ok -- all keywords were checked ...
		 *=======================================================*/
    free(cmd);
    str_copy_dx(&val->val_dsc,&dsc_temp);
    str_free1_dx(&dsc_temp);
    if (key->keyA_syntax)
       {
        if (keycnt == 1)
           {
            sts = key->keyL_status;		/* save status		*/
            setCurrentSyntax(FALSE,key->keyA_syntax);
            key->keyL_status = sts;
           }
        else
            fprintf(stderr,"Keyword syntax specified, but keycnt=%d\n",
                keycnt);
       }
    return((keycnt==1) ? key->keyL_status : CLI_STS_PRESENT);
   }



	/****************************************************************
	 * cli_process_qualifier:
	 ****************************************************************/
static long  cli_process_qualifier(	/* Return: status		*/
    char  **pp			/* <m> current pos within cmdstring	*/
   )

   {
    int   i,k;
    int   opt;
    char  *p;
    struct cduQualifier  *qual;
    struct cduVerb  *v;

    v = currentSyntax;
    if (!currentQualifiers)
        return(cli_error(CLI_STS_IVVERB,"No qualifiers defined"));

    opt = cmd_lookup(pp,cmdQual,0,NOMSG,0);
    if (opt)
       {
        qual = currentQualifiers + (opt-1);
        qual->qualL_status = CLI_STS_PRESENT;
       }
    else
       {		/* Not found:  check for negated qualifier ...	*/
        if (toupper(**pp)=='N' && toupper(*(*pp+1))=='O')
           {
            p = *pp + 2;
            opt = cmd_lookup(&p,cmdQual,0,NOMSG,0);
            if (opt > 0)
               {
                qual = currentQualifiers + (opt-1);
                if (qual->qualL_flags & QUAL_M_NONNEGATABLE)
                   {		/* default for qualifiers is NEGATABLE	*/
                    return(cli_error(CLI_STS_BADLINE,"not negatable"));
                   }
                qual->qualL_status = CLI_STS_NEGATED;
                *pp = p;
               }
           }
        if (!opt)
            return(cli_error(CLI_STS_IVVERB,"Unknown qualifier"));
       }

    if (cliDebug)
        printf("--> qualifier %s%s\n",qual->qualA_name,
                (qual->qualL_status==CLI_STS_NEGATED)?" (NEGATED)":"");
    if (qual->qualA_syntax)
        setCurrentSyntax(FALSE,qual->qualA_syntax);
    if (qual->qualA_value && (qual->qualL_status&1) && *pp && **pp=='=')
       {
        (*pp)++;
        qual->qualL_status = set_value(pp,qual->qualA_value);
		/* Note: currentSyntax may have changed inside set_value */
		/* .. we're ignoring this problem for now ...		*/
       }
    return(1);
   }



	/****************************************************************
	 * cli_process_parameter:
	 ****************************************************************/
static long  cli_process_parameter(	/* Return: status		*/
    char  **pp			/* <m> current pos within cmdstring	*/
   )

   {
    int   i,k;
    int   opt;
    int   sts;
    struct cduParam  *prm;
    struct cduVerb  *v;

    v = currentSyntax;
    if (paramId > numParams)
        return(cli_error(CLI_STS_IVVERB,"Too many parameters"));

    prm = currentParameters + (paramId-1);
    if (cliDebug)
        printf("--> parameter %s\n",prm->prmA_name);

    sts = set_value(pp,prm->prmA_value);
		/* Note: currentSyntax may have changed inside set_value */
    prm = currentParameters + (paramId-1);
    prm->prmL_status = sts;

    return(LEGAL_STATUS(prm->prmL_status) ? 1 : 0);
   }


	/***************************************************************
	 * cli_process_verb:
	 ***************************************************************/
static int   cli_process_verb(		/* Return: status		*/
    char  **pp			/* <m> current pos within cmdstring	*/
   ,char  prompt[]		/* <r> prompt string, if req'd		*/
   )
   {
    int   i,k;
    int   sts;
    char  *p;
    struct cduVerb  *v;		/* current syntax			*/

		/*=======================================================
		 * Initialize cmd_lookup tables etc for params and quals
		 *======================================================*/
    v = currentSyntax;
    if (cliDebug)
        printf("\ncmd = %s\n",v->vrbA_name);

    p = *pp;
    sts = CLI_STS_NORMAL;		/* a nice, positive outlook	*/

    for ( ; sts&1 && (p=cliNonblank(p)) ; )
       {
        if (IS_QUALIFIER_CHARACTER(*p))
           {
            if (!isalpha(*(++p)))
                return(CLI_STS_BADLINE);
            sts = cli_process_qualifier(&p);
           }
        else
           {
            paramId++;
            sts = cli_process_parameter(&p);
           }
       }
    *pp = p;			/* set current pos within cmdline	*/
    return(sts);
   }



	/***************************************************************
	 * make_entityString:
	 ***************************************************************/
static void  make_entityString(
    void  *entity		/* <r> dsc or cstring			*/
   ,char  entityString[]	/* <w> string to create			*/
   ,int   maxlen		/* <r> sizeof(entityString)		*/
   )
   {
    int   i,k;
    char  *p;
    struct descriptor  *dsc;

    if (is_cdescr(entity) || is_ddescr(entity))
       {
        dsc = entity;
        p = dsc->dscA_pointer;
        k = dsc->dscW_length;
       }
    else
       {
        p = entity;
        k = strlen(p);
       }
    if (k >= maxlen)
       {
        fprintf(stderr,"\n*ERR* entityString too long!\n\n");
        exit(0);
       }
    strncpy(entityString,p,k);
    entityString[k] = '\0';
    strtrim(entityString,0);
    l2u(entityString,0);

    return;
   }



	/***************************************************************
	 * find_entity:
	 * Find the named entity:  parameter, qualifier, or keyword
	 ***************************************************************/
static struct cduEntity  *find_entity(	/* Return: addr of struct	*/
    char  entity[]			/* <r> entity name <upper case>	*/
   ,struct cduEntity  entityList[]	/* <r> params, quals, or keywds	*/
   )
   {
    int   i,k;
    char  *p;
    char  *t;
    struct cduValue  *val;
    struct cduEntity  *e,*kwd;

    if (!entityList)
        return(0);

    p = strchr(entity,'.');			/* contains '.' ?	*/
    k = p ? (p-entity) : strlen(entity);	/*..set length to look at */
    if (!k)
       {
        fprintf(stderr,"\n*WARN* zero-length entity?\n\n");
        exit(0);			/*-----------------> err: exit	*/
       }

    for (e=entityList ; e->entA_name ; e++)
       {
        t = e->entA_label ? e->entA_label : e->entA_name;
        if (!strncmp(entity,t,k))
            break;

        if (!(e->entL_flags & ENT_M_PARAMETERS))
            continue;
        if (val = e->entA_value)
           {
            if ((t=val->val_dsc.dscA_pointer) && !strcmp(entity,t))
                break;
           }
/*        if (val = e->entA_value)
/*           {				/*...check keywords, if any	*/
/*            if (val->valA_userType)
/*               {
/*                kwd = find_entity(entity,(void *)val->valA_userType);
/*                if (kwd)
/*                   {
/*                    e = kwd;
/*                    break;
/*                   }
/*               }
/*           }					/*  */
       }
    if (!e->entA_name)
        return(0);			/*---------> return: not found	*/

    if (!p)
        return(e);			/*------> return: entity found	*/

			/*----------------------------------------------
			 * entity specifies a sub-entity.  Look for it ...
			 *----------------------------------------------*/
    val = e->entA_value;
    if (!val || !val->valA_userType)
        return(0);			/*---------> return: no keywds	*/

    return(find_entity(p+1,(void *)val->valA_userType));
   }



	/*****************************************************************
	 * cli_debug:
	 *****************************************************************/
int   cli_debug(		/* Returns: new debug status		*/
    int   val			/* <r> new debug status			*/
   )
   {
    return(cliDebug = val);
   }



	/****************************************************************
	 * cli_addr_cmdline_dsc:
	 ****************************************************************/
struct descriptor  *cli_addr_cmdline_dsc()
   {
    return(&dsc_cmdline);
   }



	/*****************************************************************
	 * cli_dcl_parse:
	 *****************************************************************/
int   cli_dcl_parse(		/* Returns: status			*/
    void  *command_string	/* <r:opt> descr or c-string		*/
   ,struct cduVerb  *table	/* <r> addr of command table		*/
   ,int   (*param_routine)()	/* <r:opt> routine to read req'd params	*/
   ,int   (*prompt_routine)()	/* <r:opt> routine to read command string*/
   ,void  *uprompt		/* <r:opt> descr or c-string		*/
   )
   {
    int   i,k;
    int   opt;
    int   sts;
    int   tokenLen;
    char  *p;
    char  prompt[48];
    struct cduParam  *prm;
    struct cduValue  *val;
    struct descriptor  *dsc;
    static DYNAMIC_DESCRIPTOR(dsc_parameter);

    clearCurrentSyntax();
    init_table(table);
    if (!uprompt)
        strcpy(prompt,"Command> ");
    else
       {
        if (is_cdescr(uprompt))
           {
            dsc = uprompt;
            p = dsc->dscA_pointer;
            k = dsc->dscW_length;
           }
        else if (is_ddescr(uprompt))
           {
            dsc = uprompt;
            p = dsc->dscA_pointer;
            k = dsc->dscW_length;
            if (!p && !k)
               {			/* null descriptor		*/
                p = "Command> ";
                k = strlen(p);
              }
           }
        else
           {
            p = uprompt;
            k = strlen(uprompt);
           }
        if (k >= sizeof(prompt))
            k = sizeof(prompt) - 1;
        strncpy(prompt,p,k);
        prompt[k] = '\0';
       }

		/*======================================================
		 * Set 'p' to start of command line ...
		 *=====================================================*/
    if (p = command_string)
       {
        if (is_cdescr(command_string) || is_ddescr(command_string))
           {
            dsc = command_string;
            p = dsc->dscA_pointer;
           }
       }
    if (!p)
       {
        sts = prompt_routine(prompt,&dsc_cmdline);
        if (~sts & 1)
            return(sts);		/*--------------------> return	*/
        p = nonblank(dsc_cmdline.dscA_pointer);
        if (!p)
            return(CLI_STS_NOCOMD);	/*---------> return: blank line	*/
       }

		/*=====================================================
		 * Get the command verb ...
		 *====================================================*/
    opt = cmd_lookup(&p,cmdVerb,0,NOMSG,0);
    if (!opt)
        sts = CLI_STS_IVVERB;
    else
       {
        paramId = 0;
        setCurrentSyntax(TRUE,currentTable+(opt-1));
        sts = cli_process_verb(&p,prompt);
       }
    if (~sts & 1)
        return(sts);			/*---------------> return: err	*/

		/*======================================================
		 * "paramId" indicates number of parameters on line.
		 * Check that all required parameters are present ...
		 *=====================================================*/
    for ( ; (prm=currentParameters+paramId) && prm->prmA_value ; )
       {
        val = prm->prmA_value;
        if (!(val->valL_flags & VAL_M_REQUIRED))
            break;		/* done:  required params come first	*/
        sprintf(prompt,"_%s: ",
            prm->prmA_prompt ? prm->prmA_prompt :
            (prm->prmA_label ? prm->prmA_label : prm->prmA_name));

        for (p=0 ; !p ; )
           {
            sts = param_routine(prompt,&dsc_parameter);
            if (~sts & 1)
                return(sts);		/*--------------------> return	*/
            p = nonblank(dsc_parameter.dscA_pointer);
           }
        str_append(&dsc_cmdline," ");
        str_append(&dsc_cmdline,&dsc_parameter);
        sts = cli_process_verb(&p,"...");
       }
    return(sts);
   }



	/****************************************************************
	 * cli_dispatch:
	 ****************************************************************/
int   cli_dispatch(		/* Return: sts of dispatched routine	*/
    void  *userarg	/* <r> optional user arg, passed to routine	*/
   )
   {
    int   sts;

    if (cliDebug)
       {
        if (!(printf("\nOk to dispatch? <Y> "),yesno(1)))
            return(1);
       }

    if (!currentSyntax->vrbA_routine)
        return(CLI_STS_INVROUT);

    sts = (*currentSyntax->vrbA_routine)(userarg);
    return(sts);
   }



	/****************************************************************
	 * cli_present:
	 ****************************************************************/
int   cli_present(		/* Return: status			*/
    void  *entity		/* <r> entity name: dsc or c-string	*/
   )
   {
    int   i,k;
    char  entityString[128];
    struct cduEntity  *e;
    struct cduValue  *val;

    if (!currentSyntax)
        return(CLI_STS_IVREQTYP);

    make_entityString(entity,entityString,sizeof(entityString));

    e = find_entity(entityString,(void *)currentParameters);
    if (!e)
        e=find_entity(entityString,(void *)currentQualifiers);

    if (!e || !e->entL_status)
        return(CLI_STS_ABSENT);

    return(e->entL_status);
   }



	/****************************************************************
	 * cli_get_value:
	 ****************************************************************/
int   cli_get_value(		/* Return: status			*/
    void  *entity		/* <r> entity name: dsc or c-string	*/
   ,struct descriptor  *dsc_ret	/* <w> return value descriptor		*/
   )
   {
    int   i,k;
    int   sts;
    char  entityString[128];
    char  *p,*p2;
    struct cduEntity  *e;
    struct cduValue  *val;
    static struct descriptor  dsc_util = {
                                0,DSC_K_DTYPE_T,DSC_K_CLASS_S,0};

    if (!currentSyntax)
        return(CLI_STS_IVREQTYP);

    make_entityString(entity,entityString,sizeof(entityString));

		/*-----------------------------------------------------
		 * Check for "special" entities ...
		 *----------------------------------------------------*/
    p = 0;
    if (!strcmp(entityString,"$LINE"))
        p = dsc_cmdline.dscA_pointer;
    else if (!strcmp(entityString,"$VERB"))
        p = currentSyntax->vrbA_name;

    if (p)
       {
        str_copy_dx(dsc_ret,p);
        return(1);			/*------------> return: special	*/
       }

		/*--------------------------------------------------------
		 * Find requested entity and return its value ...
		 *-------------------------------------------------------*/
    e = find_entity(entityString,(void *)currentParameters);
    if (!e)
        e=find_entity(entityString,(void *)currentQualifiers);

    if (!e || !e->entL_status)
        return(CLI_STS_ABSENT);

    val = e->entA_value;
    if (!val)
        return(CLI_STS_IVREQTYP);

/*    if (val->valL_flags & VAL_M_USER_DEFINED_TYPE)
/*        return(CLI_STS_IVREQTYP);			/*  */

    if (!val->valA_substring)
        return(CLI_STS_ABSENT);

			/*------------------------------------------------
			 * Return 'value' string to caller ...
			 *-----------------------------------------------*/
    if (val->valL_flags & VAL_M_LIST)
       {
        p = val->valA_substring;
        p2 = strpbrk(p,"\01\02");	/* find delimter char	*/
        k = p2 ? (p2-p) : strlen(p);
        dsc_util.dscW_length = k;
        dsc_util.dscA_pointer = p;
        sts = str_copy_dx(dsc_ret,&dsc_util);
        val->valA_substring = p2;
       }
    else
       {
        sts = str_copy_dx(dsc_ret,val->valA_substring);
        val->valA_substring = 0;
       }

#ifdef vms
    if (val->valL_flags & VAL_M_DELTATIME)
       {				/* VMS deltatime only ...	*/
        p = strchr(dsc_ret->dscA_pointer,'-');
        if (p)  *p = ' ';		/*..replace '-' with blank	*/

        p = dsc_ret->dscA_pointer;
        k = strlen(p);
        if (k < dsc_ret->dscW_length)
            memset(p+k,' ',dsc_ret->dscW_length - k);
       }
#endif

    if (!val->valA_substring)
        return(1);			/*-------------> return: Normal	*/

    if (*val->valA_substring == '\01')		/* "Comma" delimiter	*/
       {
        val->valA_substring++;
        return(CLI_STS_COMMA);
       }

    if (*val->valA_substring == '\02')		/* "Plus" delimiter	*/
       {
        val->valA_substring++;
        return(CLI_STS_CONCAT);
       }

    return(cli_error(CLI_STS_BADLINE,"cli_get_value error ???"));
   }
