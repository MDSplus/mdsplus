#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        "cdudef.h"
#ifdef vms
#include        <ssdef.h>
#endif

/**********************************************************************
* CDU_WRITE.C --
*
* Support routines for CDU emulator ...
*
* History:
*  09-Mar-1998  TRG  When writing prmL_flags, qualL_flags, or keyL_flags,
*                     or-in identifying bit ENT_M_xxx.
*  09-Oct-1997  TRG  Create.
*
***********************************************************************/


extern int   debugCdu;		/* set to 1 for extra printout		*/


static FILE  *fp;
static char  moduleName[32];

static char  fmt_header1[] = "\
#include        \"clisysdef.h\"\n\
\n\
\t\t/* Filename: %s\n\
\t\t * created by %s :  %s\n\
\t\t ********************************************************/\n\n";

static char  fmt_verbs1[] = "struct cduVerb  %s[%d] = {\n";
static char  fmt_verbs2[] = "       %c\"%s\",0x%04X,%s,%s,%s,%s,%s\n";
static char  fmt_syntax1[] =
                "static struct cduVerb  %s = {\t/* Syntax def\t*/\n";
static char  *fmt_syntax2 = fmt_verbs2;

static char  fmt_params1[] =
                "static struct cduParam  %sparams[%d] = {\n";
static char  fmt_params2[] = "       %c\"%s\",0x%04X,%s,%s,%s,0\n";

static char  fmt_qualifiers1[] =
                "static struct cduQualifier %squalifiers[%d] = {\n";
static char  fmt_qualifiers2[] = "       %c\"%s\",0x%04X,%s,%s,%s,0\n";

static char  fmt_value1[] = "static struct cduValue  %svalue = {\n";
static char  fmt_value2[] = "        0x%04X,%s,%s, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0\n";

static char  fmt_protoUserType1[] =
            "static struct cduKeyword  %s[%d];\t/* Prototype only\t*/\n";
static char  fmt_userType1[] =
            "static struct cduKeyword  %s[%d] = {\t/* \"Type\" def\t*/\n";
static char  fmt_userType2[] = "       %c\"%s\",0x%04X,%s,%s,%s,0\n";


static void  writeUserType();
static void  writeSyntax();



	/**************************************************************
	 * writeExtern:
	 **************************************************************/
static void  writeExtern(
    char  name[]		/* <r> name of extern value		*/
   )
   {
    int   i,k;
    char  ucname[32];		/* upper-case version of "name"		*/
    struct stringlink  {
        struct stringlink  *flink;
        char  ucname[1];
       };
    struct stringlink  *p;
    static struct stringlink  *top;

    if (!name || !name[0])
        return;

    l2u(ucname,name);
    for (p=top ; p && strcmp(ucname,p->ucname) ; p=p->flink)
        ;
    if (p)
        return;

		/*======================================================
		 * Write extern def in output file.
		 * Add ucname to stringlink.
		 *=====================================================*/
    fprintf(fp,"extern int   %s();\n",name);	/* case-sensitive name	*/
    p = malloc(sizeof(struct stringlink) + strlen(ucname));
    strcpy(p->ucname,ucname);
    p->flink = top;
    top = p;
    return;
   }



	/**************************************************************
	 * writeValue:
	 **************************************************************/
static void  writeValue(
    char  parentPrefix[]	/* <r> string to identify "parent"	*/
   ,struct valueClause  *val	/* <r> addr of value struct		*/
   )
   {
    int   i,k;
    char  textDefault[64];

    if (!val)
        return;

    if (val->valL_flags & VAL_M_USER_DEFINED_TYPE)
        writeUserType(val->valA_userType);

    if (val->valA_default)
        sprintf(textDefault,"\"%s\"",val->valA_default);
    else
        strcpy(textDefault,"0");

    fprintf(fp,"\n");
    fprintf(fp,fmt_value1,parentPrefix);
    fprintf(fp,fmt_value2,val->valL_flags,textDefault,
        val->valA_userType ? val->valA_userType : "0");
    fprintf(fp,"       };\n");

    if (debugCdu)
        printf("writeValue: %s\n",parentPrefix);
   }



	/***************************************************************
	 * writeUserType:
	 ***************************************************************/
static void  writeUserType(
    char  name[]		/* <r> userType name			*/
   )
   {
    int   i,k;
    int   icnt;
    char  prefix[32];
    char  textLabel[64],textSyntax[64],textValue[64];
    struct keyword  *key;
    static int  userTypeCnt;

    if (!okToWriteUserType(name))
       {
        if (debugCdu)
            printf("--> userType %s already being written\n",name);
        return;
       }
    userTypeCnt++;

    for (icnt=0 ; key=findKeywordByIdx(name,icnt) ; icnt++)
       {
        sprintf(prefix,"t%02dk%02d",userTypeCnt,icnt+1);

        if (key->keyA_syntax)
            writeSyntax(key->keyA_syntax);
        if (key->keyA_value)
            writeValue(prefix,key->keyA_value);
       }

    fprintf(fp,"\n");
    fprintf(fp,fmt_userType1,name,icnt+1);
    for (icnt=0 ; key=findKeywordByIdx(name,icnt) ; icnt++)
       {
        sprintf(prefix,"t%02dk%02d",userTypeCnt,icnt+1);

        if (key->keyA_label)
            sprintf(textLabel,"\"%s\"",key->keyA_label);
        else
            strcpy(textLabel,"0");
        if (key->keyA_value)
            sprintf(textValue,"&%svalue",prefix);
        else
            strcpy(textValue,"0");
        if (key->keyA_syntax)
            sprintf(textSyntax,"&%s",key->keyA_syntax);
        else
            strcpy(textSyntax,"0");

        fprintf(fp,fmt_userType2, icnt?',':' ',key->keyA_name,
            key->keyL_flags | ENT_M_KEYWORDS,
            textLabel,textSyntax,textValue);
       }
    fprintf(fp,"       ,0\t\t\t\t/* null entry at end\t*/\n");
    fprintf(fp,"       };\n");
    markUserTypeWritten(name);
    return;
   }



	/****************************************************************
	 * writeParams:
	 ****************************************************************/
static void  writeParams(
    char  parentPrefix[]	/* <r> string to identify "parent"	*/
   ,struct verblist  *v		/* <r> the verb				*/
   )
   {
    int   i,k;
    int   icnt;
    char  prefix[32];
    char  textLabel[64],textPrompt[64],textValue[64];
    struct paramClause  *prm;

    if (!v->vrbA_parametersTop)
        return;

    if (debugCdu)
        printf("writeParams: called for verb %s\n",parentPrefix);
    for (icnt=0 ; prm=findParamByIdx(v,icnt+1) ; icnt++)
       {
        sprintf(prefix,"%sp%02d",parentPrefix,icnt+1);
        writeValue(prefix,prm->prmA_value);
       }

    fprintf(fp,"\n");
    fprintf(fp,fmt_params1,parentPrefix,icnt+1);
    for (icnt=0 ; prm=findParamByIdx(v,icnt+1) ; icnt++)
       {
        sprintf(prefix,"%sp%02d",parentPrefix,icnt+1);
        if (prm->prmA_label)
            sprintf(textLabel,"\"%s\"",prm->prmA_label);
        else
            strcpy(textLabel,"0");
        if (prm->prmA_prompt)
            sprintf(textPrompt,"\"%s\"",prm->prmA_prompt);
        else
            strcpy(textPrompt,"0");
        if (prm->prmA_value)
            sprintf(textValue,"&%svalue",prefix);
        else
            strcpy(textValue,"0");

        fprintf(fp,fmt_params2, icnt?',':' ',
            prm->prmA_name,prm->prmL_flags | ENT_M_PARAMETERS,
            textLabel,textPrompt,textValue);
       }
    fprintf(fp,"       ,0\t\t\t\t/* null entry at end\t*/\n");
    fprintf(fp,"       };\n");
    return;
   }



	/****************************************************************
	 * writeQualifiers:
	 ****************************************************************/
static void  writeQualifiers(
    char  parentPrefix[]	/* <r> string to identify "parent"	*/
   ,struct verblist  *v		/* <r> the verb				*/
   )
   {
    int   i,k;
    int   icnt;
    char  prefix[32];
    char  textLabel[64],textSyntax[64],textValue[64];
    struct qualifier  *qual;

    if (!v->vrbA_qualifiersTop)
        return;

    if (debugCdu)
        printf("writeQualifiers: called for %s\n",parentPrefix);
    for (icnt=0 ; qual=findQualifierByIdx(v,icnt) ; icnt++)
       {
        sprintf(prefix,"%sq%02d",parentPrefix,icnt+1);

        if (qual->qualA_syntax)
            writeSyntax(qual->qualA_syntax);
        writeValue(prefix,qual->qualA_value);
       }

    fprintf(fp,"\n");
    fprintf(fp,fmt_qualifiers1,parentPrefix,icnt+1);
    for (icnt=0 ; qual=findQualifierByIdx(v,icnt) ; icnt++)
       {
        sprintf(prefix,"%sq%02d",parentPrefix,icnt+1);
        if (qual->qualA_label)
            sprintf(textLabel,"\"%s\"",qual->qualA_label);
        else
            strcpy(textLabel,"0");
        if (qual->qualA_syntax)
            sprintf(textSyntax,"&%s",qual->qualA_syntax);
        else
            strcpy(textSyntax,"0");
        if (qual->qualA_value)
            sprintf(textValue,"&%svalue",prefix);
        else
            strcpy(textValue,"0");

        fprintf(fp,fmt_qualifiers2, icnt?',':' ',
            qual->qualA_name,qual->qualL_flags | ENT_M_QUALIFIERS,
            textLabel,textSyntax,textValue);
       }
    fprintf(fp,"       ,0\t\t\t\t/* null entry at end\t*/\n");
    fprintf(fp,"       };\n");
    return;
   }



	/****************************************************************
	 * writeSyntax:
	 ****************************************************************/
static void  writeSyntax(
    char  name[]		/* <r> name for user-defined syntax	*/
   )
   {
    int   i,k;
    char  prefix[32];
    char  textParameters[64],textQualifiers[64];
    struct verblist  *v;

    if (!name || !name[0])
        return;
    v = findSyntax(name);
    if (!v)
       {
        printf("*ERR* failed to find syntax '%s'\n",name);
        exit(0);
       }

    if (debugCdu)
        printf("writeSyntax:  name=%s\n",name);

    sprintf(prefix,"%s_",name);	/* prefix for parameters and qualifiers	*/
    writeExtern(v->vrbA_routine);
    writeParams(prefix,v);
    writeQualifiers(prefix,v);

    fprintf(fp,"\n");
    fprintf(fp,fmt_syntax1,name);
    if (v->vrbA_parametersTop)
        sprintf(textParameters,"%sparams",prefix);
    else
        strcpy(textParameters,"0");
    if (v->vrbA_qualifiersTop)
        sprintf(textQualifiers,"%squalifiers",prefix);
    else
        strcpy(textQualifiers,"0");

    fprintf(fp,fmt_syntax2, ' ',
        v->vrbA_name,v->vrbL_flags,
        v->vrbA_disallow ? v->vrbA_disallow : "0",
        v->vrbA_image ? v->vrbA_image : "0",
        v->vrbA_routine ? v->vrbA_routine : "0",
        textParameters,textQualifiers);
    fprintf(fp,"       };\n");
    return;
   }



	/***************************************************************
	 * writeProtoUserType:
	 * Write prototype struct when necessary ...
	 ***************************************************************/
int   writeProtoUserType(	/* Returns: status			*/
    char  name[]		/* <r> userType name			*/
   )
   {
    int   i,k;
    int   icnt;

    for (icnt=0 ; findKeywordByIdx(name,icnt) ; icnt++)
        ;
    fprintf(fp,fmt_protoUserType1,name,icnt+1);
			/* add 1 to account for null element at end	*/
    return(1);
   }



	/****************************************************************
	 * cdu_write:
	 ****************************************************************/
void  cdu_write(
    char  _module[]		/* <r> name of module -- filename too	*/
   )
   {
    int   i,k;
    int   icnt;
    char  filename[128];
    char  *p;
    char  prefix[32];
    char  textParameters[64];
    char  textQualifiers[64];
    char  *userType;
    struct verblist  *v;
    struct keyword  *key;

    if (!findVerbByIdx(0))
       {
        dasmsg(0,"No verbs defined:  output not written");
        return;
       }

		/*=======================================================
		 * Open output file and write it ...
		 *======================================================*/
    strcpy(moduleName,(_module && _module[0]) ? _module : "cdu_file");
    sprintf(filename,"%s.c",moduleName);
    u2l(filename,0);
    fp = fopen(filename,"w");
    if (fp)
        printf("\n--> creating file %s\n\n",filename);
    else
       {
        printf("\n--> failed to open file %s -- using stdout\n\n",filename);
        fp = stdout;
       }
    fprintf(fp,fmt_header1,filename,pgmname(),cdatime(0));

		/*------------------------------------------------------
		 * Write support structs first ...
		 *-----------------------------------------------------*/
    for (icnt=0 ; v=findVerbByIdx(icnt) ; icnt++)
       {
        sprintf(prefix,"v%02d",icnt+1);

        writeExtern(v->vrbA_routine);
        writeParams(prefix,v);
        writeQualifiers(prefix,v);
       }

		/*------------------------------------------------------
		 * write verb list ...
		 *-----------------------------------------------------*/
    fprintf(fp,"\n");
    fprintf(fp,fmt_verbs1,moduleName,icnt+1);

    for (icnt=0 ; v=findVerbByIdx(icnt) ; icnt++)
       {
        sprintf(prefix,"v%02d",icnt+1);

        if (v->vrbA_parametersTop)
            sprintf(textParameters,"%sparams",prefix);
        else
            strcpy(textParameters,"0");
        if (v->vrbA_qualifiersTop)
            sprintf(textQualifiers,"%squalifiers",prefix);
        else
            strcpy(textQualifiers,"0");

        fprintf(fp,fmt_verbs2, icnt?',':' ',
            v->vrbA_name,v->vrbL_flags,
            v->vrbA_disallow ? v->vrbA_disallow : "0",
            v->vrbA_image ? v->vrbA_image : "0",
            v->vrbA_routine ? v->vrbA_routine : "0",
            textParameters,textQualifiers);
       }
    fprintf(fp,"       ,0\t\t\t\t/* null entry at end\t*/\n");
    fprintf(fp,"       };\n");

		/*======================================================
		 * Display all TYPEs (keywords only) ...
		 * Display all SYNTAXes ...
		 *=====================================================*/
    if (interactive())
       {
        for (i=0 ; userType=findUserTypeByIdx(i) ; i++)
           {
            printf("\nUSER TYPE = %s\n",userType);
            for (k=0 ; key=findKeywordByIdx(userType,i) ; i++)
                printf("    %s\n",key->keyA_name);
           }
        if (i > 0)
            printf("\n");

        for (i=0 ; v=findSyntaxByIdx(i) ; i++)
           {
            if (!i)
                printf("\nSYNTAX list\n");

            printf("    %s\n",v->vrbA_name);
           }
        if (i > 0)
            printf("\n");

       }
    return;
   }
