#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        "cdudef.h"
#ifdef vms
#include        <ssdef.h>
#endif

/**********************************************************************
* CDU_UTIL.C --
*
* Support routines for CDU emulator ...
*
* History:
*  09-Oct-1997  TRG  Create.
*
***********************************************************************/


#define MAX_USERTYPES      100
#define CDU_WRITTEN        0x01
#define CDU_REQUESTED      0x02
#define CDU_PROTO_WRITTEN  0x04

struct userTypeList  {
        char  *name;				/* Name of userType	*/
        int   hasBeenWritten;			/* flag			*/
        struct keywordChain  *keywordTop;	/* top of keyword list	*/
       };
struct keywordChain  {
        struct keywordChain  *flink;	/* addr of next keyword ...	*/
        struct keyword   keyword;	/* the keyword struct		*/
       };
struct parameterChain  {
        struct parameterChain  *flink;	/* addr of next param ...	*/
        struct paramClause  param;	/* the parameter struct		*/
       };
struct qualifierChain  {
        struct qualifierChain  *flink;	/* addr of next qualifier ...	*/
        struct qualifier   qualifier;	/* the qualifier struct		*/
       };
struct verbChain  {
        struct verbChain  *flink;	/* addr of next verb ...	*/
        struct verblist  verb;		/* the verb structure		*/
       };

static int   userTypeCnt;	/* Number of "user types" defined	*/
static struct userTypeList  userTypeList[MAX_USERTYPES];



	/**************************************************************
	 * findUserTypeByIdx:
	 **************************************************************/
void  *findUserTypeByIdx(
    int   idxRequest		/* <r> requested idx number, zero based	*/
   )
   {
    if (idxRequest<0 || idxRequest>=userTypeCnt)
        return(0);
    return(userTypeList[idxRequest].name);
   }



	/***************************************************************
	 * findUserTypeIdx:
	 * getUserTypeIdx:
	 * Return addr of "userType" struct.  Create if necessary.
	 ***************************************************************/
int   userTypeHasBeenWritten(	/* Return: 1(T) or 0(F)			*/
    char  uname[]		/* <r> name of "type"			*/
   )
   {
    int   i;
    char  name[32];

    l2u(name,uname);		/* make upper-case version of name	*/
    for (i=0 ; i<userTypeCnt && strcmp(name,userTypeList[i].name) ; i++)
        ;
    if (i == userTypeCnt)
        return(0);
    return((userTypeList[i].hasBeenWritten & CDU_WRITTEN) ? 1 : 0);
   }


int   okToWriteUserType(	/* Return: 1(T) or 0(F)			*/
    char  uname[]		/* <r> name of "type"			*/
   )
   {
    int   i;
    int   hbw;			/* "hasBeenWritten" value		*/
    char  name[32];

    l2u(name,uname);		/* make upper-case version of name	*/
    for (i=0 ; i<userTypeCnt && strcmp(name,userTypeList[i].name) ; i++)
        ;
    if (i == userTypeCnt)
        return(0);

    hbw = userTypeList[i].hasBeenWritten;
    if (hbw)
       {		/* Struct has been requested or actually written */
        if (!(hbw & (CDU_WRITTEN | CDU_PROTO_WRITTEN)))
           {
            if (writeProtoUserType(name) & 1)
                userTypeList[i].hasBeenWritten |= CDU_PROTO_WRITTEN;
           }
        return(0);			/* either requested or written	*/
       }

    userTypeList[i].hasBeenWritten = CDU_REQUESTED;
    return(1);
   }


void  markUserTypeWritten(	/* Returns:  void			*/
    char  uname[]		/* <r> name of "type"			*/
   )
   {
    int   i;
    char  name[32];

    l2u(name,uname);		/* make upper-case version of name	*/
    for (i=0 ; i<userTypeCnt && strcmp(name,userTypeList[i].name) ; i++)
        ;
    if (i == userTypeCnt)
       {
        fprintf(stderr,"\n**INTERNAL ERROR**  User Type not found\n\n");
        exit(0);
       }
    userTypeList[i].hasBeenWritten = CDU_WRITTEN;
    return;
   }


int   getUserTypeIdx(		/* Return: idx into userTypeList[]	*/
    char  uname[]		/* <r> name of "type"			*/
   )
   {
    int   i;
    char  name[32];

    l2u(name,uname);		/* make upper-case version of name	*/
    for (i=0 ; i<userTypeCnt && strcmp(name,userTypeList[i].name) ; i++)
        ;
    if (i < userTypeCnt)
        return(i);

		/*--------------------------------------------------------
		 * Not found:  create new entry ...
		 *-------------------------------------------------------*/
    if (userTypeCnt >= MAX_USERTYPES)
       {
        printf("\n*ERR* too many USER TYPES: max is %d\n\n",MAX_USERTYPES);
        exit(0);
       }
    i = userTypeCnt++;
    userTypeList[i].name = STRING_ALLOC(name);
    userTypeList[i].keywordTop = 0;
    return(i);
   }



	/****************************************************************
	 * getKeywordStruct:
	 ****************************************************************/
struct keyword  *getKeywordStruct(	/* Return: addr of keyword struct */
    char  userType[]			/* <r> name of "user type"	*/
   ,char  uname[]			/* <r> keyword name		*/
   )
   {
    int   idx;
    char  name[32];
    struct keywordChain  *key,*keyB4;

    idx = getUserTypeIdx(userType);
    key = userTypeList[idx].keywordTop;
    for (keyB4=0 ; key ; key=keyB4->flink)
       {
        keyB4 = key;
        if (!strcmp(name,key->keyword.keyA_name))
            break;
       }
    if (key)
        return(&key->keyword);

		/*--------------------------------------------------------
		 * Allocate a new keyword struct ...
		 *-------------------------------------------------------*/
    key = malloc(sizeof(struct keywordChain));
    clear_buffer(key,sizeof(*key));
    if (keyB4)
        keyB4->flink = key;
    else
        userTypeList[idx].keywordTop = key;

    return(&key->keyword);
   }



	/****************************************************************
	 * findKeywordByIdx:
	 ****************************************************************/
struct keyword  *findKeywordByIdx(	/* Returns: addr of struct	*/
    char  typeName[]			/* <r> user "type"		*/
   ,int   keywordIdx		/* <r> keywordIdx of keyword in chain	*/
   )
   {
    int   i,k;
    struct keywordChain  *key;

    k = getUserTypeIdx(typeName);
    key = userTypeList[k].keywordTop;
    for (i=0 ; key && i<keywordIdx ; i++,key=key->flink)
        ;
    return(key ? &key->keyword : 0);
   }



	/****************************************************************
	 * findParamByIdx:
	 * getParamByIdx:
	 ****************************************************************/
struct paramClause  *findParamByIdx(	/* Return: addr of paramClause	*/
    struct verblist  *v		/* <r> verb struct		*/
   ,int   paramIdx			/* <r> parameter idx		*/
   )
   {
    int   i;
    struct parameterChain  *pc;

    if (paramIdx < 1)
        return(0);

    pc = v->vrbA_parametersTop;
    for (i=1 ; pc && i<paramIdx ; i++,pc=pc->flink)
        ;

    return(pc ? &pc->param : 0);
   }


struct paramClause  *getParamByIdx(	/* Return: addr of paramClause	*/
    struct verblist  *v			/* <r> verb struct		*/
   ,int   paramIdx			/* <r> parameter idx		*/
   )
   {
    int   i;
    char  paramName[32];
    struct parameterChain  *pc,*pcB4;

    if (paramIdx < 1)
        return(0);

    pc = v->vrbA_parametersTop;
    pcB4 = 0;
    for (i=1 ; pc && i<paramIdx ; i++,pc=pcB4->flink)
        pcB4 = pc;
    if (i != paramIdx)
       {
        printf("*ERR* %s param #%d out-of-sequence\n\n",
            v->vrbA_name,paramIdx);
        exit(0);
       }

    if (pc)
        return(&pc->param);

		/*=======================================================
		 * Create new parameter struct ...
		 *======================================================*/
    pc = malloc(sizeof(struct parameterChain));
    clear_buffer(pc,sizeof(*pc));
    if (pcB4)
        pcB4->flink = pc;
    else
        v->vrbA_parametersTop = pc;

    sprintf(paramName,"P%d",paramIdx);
    pc->param.prmA_name = STRING_ALLOC(paramName);

    return(&pc->param);
   }



	/***************************************************************
	 * findQualifierByIdx:
	 * findQualifier:
	 * getQualifier:
	 ***************************************************************/
struct qualifier  *findQualifierByIdx(	/* Return: addr of qualifier str*/
    struct verblist  *v		/* <m> verb struct of "owner"		*/
   ,int   idx			/* <r> index number			*/
   )
   {
    int   i;
    struct qualifierChain  *qc;

    qc = v->vrbA_qualifiersTop;
    for (i=0 ; qc && i<idx ; i++,qc=qc->flink)
       ;
    return(qc ? &qc->qualifier : 0);
   }


struct qualifier  *findQualifier( /* Return: addr of qualifier struct	*/
    struct verblist  *v		/* <m> verb struct of "owner"		*/
   ,char  qualName[]		/* <r> name of qualifier		*/
   )
   {
    char  name[32];
    struct qualifierChain  *qc;

    qc = v->vrbA_qualifiersTop;
    for ( ; qc ; qc=qc->flink)
        if (!strcmp(name,qc->qualifier.qualA_name))
            break;
    return(qc ? &qc->qualifier : 0);
   }


struct qualifier  *getQualifier( /* Return: addr of qualifier struct	*/
    struct verblist  *v		/* <m> verb struct of "owner"		*/
   ,char  qualName[]		/* <r> name of qualifier		*/
   )
   {
    char  name[32];		/* upper-case version of qualName[]	*/
    struct qualifierChain  *qc,*qcB4;

    l2u(name,qualName);
    qc = v->vrbA_qualifiersTop;
    qcB4 = 0;
    for ( ; qc ; qc=qcB4->flink)
       {
        qcB4 = qc;
        if (!strcmp(name,qc->qualifier.qualA_name))
            break;
       }
    if (qc)
        return(&qc->qualifier);

		/*======================================================
		 * Create new link in qualifier chain ...
		 *=====================================================*/
    qc = malloc(sizeof(struct qualifierChain));
    clear_buffer(qc,sizeof(*qc));
    if (qcB4)
        qcB4->flink = qc;
    else
        v->vrbA_qualifiersTop = qc;

    qc->qualifier.qualA_name = STRING_ALLOC(name);
    return(&qc->qualifier);
   }



	/*****************************************************************
	 * findVerb:
	 * getVerb:
	 * newVerb:
	 *****************************************************************/
static struct verbChain  *lastVerb;
static struct verbChain  *topVerb;

struct verblist  *findVerb(	/* Return: Addr of existing verb	*/
    char  verbName[]		/* <r> name of the verb			*/
   )
   {
    char  name[32];
    struct verbChain  *vc;

    l2u(name,verbName);
    for (vc=topVerb ; vc ; vc=vc->flink)
        if (!strcmp(name,vc->verb.vrbA_name))
            break;
    return(vc ? &vc->verb : 0);
   }


struct verblist  *findVerbByIdx( /* Return: Addr of existing verb	*/
    int   idxRequest		/* <r> idx of requested verb		*/
   )
   {
    int   i;
    struct verbChain  *vc;

    for (i=0,vc=topVerb ; vc && i<idxRequest ; i++,vc=vc->flink)
        ;
    return(vc ? &vc->verb : 0);
   }


struct verblist  *newVerb(	/* Return: Create new verb; else error	*/
    char  verbName[]		/* <r> name of the verb			*/
   )
   {
    char  name[32];
    struct verbChain  *vc;
    struct verblist  *v;

    v = findVerb(verbName);
    if (v)
       {
        printf("*ERR* newVerb:  verb %s already exists\n");
        exit(0);
       }
    vc = malloc(sizeof(struct verbChain));
    clear_buffer(vc,sizeof(*vc));
    l2u(name,verbName);
    vc->verb.vrbA_name = STRING_ALLOC(name);

    if (lastVerb)
        lastVerb->flink = vc;
    else
        topVerb = vc;
    lastVerb = vc;

    return(&vc->verb);
   }


struct verblist  *getVerb(	/* Return: Create verb if req'd		*/
    char  verbName[]		/* <r> name of the verb			*/
   )
   {
    struct verblist  *v;

    v = findVerb(verbName);
    if (!v)
        v = newVerb(verbName);

    return(v);
   }



	/*****************************************************************
	 * findSyntax:
	 * getSyntax:
	 * newSyntax:
	 *****************************************************************/
static struct verbChain  *lastSyntax;
static struct verbChain  *topSyntax;

struct verblist  *findSyntax(	/* Return: Addr of existing syntax	*/
    char  syntaxName[]		/* <r> name of syntax			*/
   )
   {
    char  name[32];
    struct verbChain  *vc;

    l2u(name,syntaxName);
    for (vc=topSyntax ; vc ; vc=vc->flink)
        if (!strcmp(name,vc->verb.vrbA_name))
            break;
    return(vc ? &vc->verb : 0);
   }


struct verblist  *findSyntaxByIdx( /* Return: Addr of existing syntax	*/
    int   idxRequest		/* <r> idx of requested verb		*/
   )
   {
    int   i;
    struct verbChain  *vc;

    for (i=0,vc=topSyntax ; vc && i<idxRequest ; i++,vc=vc->flink)
        ;
    return(vc ? &vc->verb : 0);
   }


struct verblist  *newSyntax(	/* Return: Create new syntax; else error*/
    char  syntaxName[]		/* <r> name of syntax			*/
   )
   {
    char  name[32];
    struct verbChain  *vc;
    struct verblist  *v;

    v = findSyntax(syntaxName);
    if (v)
       {
        printf("*ERR* newSyntax:  syntax %s already exists\n");
        exit(0);
       }
    vc = malloc(sizeof(struct verbChain));
    clear_buffer(vc,sizeof(*vc));
    l2u(name,syntaxName);
    vc->verb.vrbA_name = STRING_ALLOC(name);

    if (lastSyntax)
        lastSyntax->flink = vc;
    else
        topSyntax = vc;
    lastSyntax = vc;

    return(&vc->verb);
   }


struct verblist  *getSyntax(	/* Return: Create syntax if req'd	*/
    char  syntaxName[]		/* <r> name of syntax			*/
   )
   {
    struct verblist  *v;

    v = findSyntax(syntaxName);
    if (!v)
        v = newSyntax(syntaxName);

    return(v);
   }
