#include        <stdio.h>
#include        <stdarg.h>
#include        <stdlib.h>
#if defined(vms)
#include        <lib$routines.h>
#include        <time.h>
#include        <unistd.h>
#elif defined(_WIN32)
#include        <io.h>
#define isatty(a) _isatty(a)
#elif defined(HAVE_VXWORKS_H)
#include 	<time.h>
#else
#include        <sys/time.h>
#endif

#define CREATE_STS_TEXT
#include        "mdsdcldef.h"
#include        "clidef.h"
#include        "tcldef.h"
#include        "ccldef.h"
#include        "librtl_messages.h"
#include        "tdimessages.h"
#include        "treeshr.h"
#include        "camdef.h"


		/*========================================================
		 * Static variables ...
		 *=======================================================*/
static int   mdsmsgFlag = 1;		/* 1 for longer "status" string	*/
		/*========================================================
		 * "Define"s and structure definitions ...
		 *=======================================================*/
#define ALREADY_DISPLAYED  0x80000000

#ifndef HAVE_VXWORKS_H
#include        "servershr.h"
#endif
/**********************************************************************
* MDSMSG.C --
*
* Routine for printing status information ...
*
* History:
*  06-Apr-2001  TRG  Start/end each MdsMsg line with '\r', esp for IDL.
*  05-Jan-1998  TRG  Create.
*
************************************************************************/




		/*========================================================
		 * Function prototypes ...
		 *=======================================================*/
extern void StrCopyDx();
#ifndef HAVE_VXWORKS_H
extern int MDSprintf( char *fmt , ... );
extern int MDSfprintf( FILE *fp , char *fmt , ... );
#endif

	/*****************************************************************
	 * getFacility:
	 *****************************************************************/
static int   getFacility(	/* Return: num entries in stsText[]	*/
    int   sts			/* <r> sts value			*/
   ,char  **facilityText	/* <w> name of facility: c-string	*/
   ,struct stsText  **stsText	/* <w> addr of facility's "stsText[]"	*/
   )
   {
    int   facility;
    int   max;

    facility = (sts & ~ALREADY_DISPLAYED) >> 16;
    if (facility == MDSDCL_FACILITY)
       {
        *stsText = mdsdcl_stsText;	/* point to array		*/
        *facilityText = "MDSDCL";
        max = sizeof(mdsdcl_stsText)/sizeof(mdsdcl_stsText[0]);
       }
    else if (facility == CLI_FACILITY)
       {
        *stsText = cli_stsText;		/* point to array		*/
        *facilityText = "CLI";
        max = sizeof(cli_stsText)/sizeof(cli_stsText[0]);
       }
    else if (facility == CCL_FACILITY)
       {
        *stsText = ccl_stsText;		/* point to array		*/
        *facilityText = "CCL";
        max = sizeof(ccl_stsText)/sizeof(ccl_stsText[0]);
       }
#ifndef HAVE_VXWORKS_H
    else if (facility == SERVERSHR_FACILITY)
       {
        *stsText = servershr_stsText;		/* point to array		*/
        *facilityText = "SERVER";
        max = sizeof(servershr_stsText)/sizeof(servershr_stsText[0]);
       }
#endif
    else if (facility == TDI_FACILITY)
       {
        *stsText = tdi_stsText;		/* point to array		*/
        *facilityText = "TDI";
        max = sizeof(tdi_stsText)/sizeof(tdi_stsText[0]);
       }
    else if (facility == TCL_FACILITY)
       {
        *stsText = tcl_stsText;		/* point to array		*/
        *facilityText = "TCL";
        max = sizeof(tcl_stsText)/sizeof(tcl_stsText[0]);
       }
    else if (facility == TREESHR_FACILITY)
       {
        *stsText = treeshr_stsText;	/* point to array		*/
        *facilityText = "TREE";
        max = sizeof(treeshr_stsText)/sizeof(treeshr_stsText[0]);
       }
    else if (facility == LIB_FACILITY)
       {
        *stsText = librtl_stsText;	/* point to array		*/
        *facilityText = "LIB";
        max = sizeof(librtl_stsText)/sizeof(librtl_stsText[0]);
       }
    else if (facility == STRMDS_FACILITY)
       {
        *stsText = strMds_stsText;	/* point to array		*/
        *facilityText = "STRMDS";
        max = sizeof(strMds_stsText)/sizeof(strMds_stsText[0]);
       }
    else if (facility == CAM_FACILITY)
       {
        *stsText = camshr_stsText;      /* point to array               */
        *facilityText = "CAM";
        max = sizeof(camshr_stsText)/sizeof(camshr_stsText[0]);
       }
    else if (facility == SS_FACILITY)
       {
        *stsText = ss_stsText;	/* point to array		*/
        *facilityText = "SS";
        max = sizeof(ss_stsText)/sizeof(ss_stsText[0]);
       }
    else
      max = 0;
    return max;
   }
	/*****************************************************************
	 * MdsGetMsg:
	 *****************************************************************/
char  *MdsGetMsg(	/* Return: addr of "status" string	*/
    int   sts			/* <r> sts value			*/
   )
   {
     static DESCRIPTOR(msg_files,"MDSMSG_PATH:*Msg.*");
     static DESCRIPTOR(getmsg_nam,"getmsg");
     struct descriptor_d filnam = {0, DTYPE_T, CLASS_D, 0};
     int   i;
     char  *facnam, *msgnam, *msgtext;
     int status = 0;
     void *ctx = 0;
     const static char *severity[] = {"W","S","E","I","F","?","?","?"};
     static char text[512];
     int (*getmsg)(int,char **,char **,char **);
     int   max;
     char  *facilityText;
     struct stsText  *stsText;

     if (sts == 1)
     {
       strcat(text,"%SS-S-SUCCESS, Success");
       return text;
     }
     max = getFacility(sts,&facnam,&stsText);
     if (max > 0)
     {
       for (i=0 ; i<max ; i++)
       {
         if ((sts & 0xfffffff8) == (stsText[i].stsL_num & 0xfffffff8))
         {
           char *ptr;
           sprintf(text,"%%%s-%s-%s, %s",facnam,severity[sts&0x7],stsText[i].stsA_name,stsText[i].stsA_text);
           break;
         }
       }
       if (i == max)
         sprintf(text,"%%%s-%s-NOMSG, Message number 0x%08x",facnam,severity[sts&0x7],sts);
       status = 1;
     }
     while (!(status & 1) && (LibFindFile(&msg_files,&filnam,&ctx)&1))
     {
       status = LibFindImageSymbol(&filnam,&getmsg_nam,&getmsg);
       if (status & 1)
       {
         status = (*getmsg)(sts,&facnam,&msgnam,&msgtext);
         if (status & 1)
           sprintf(text,"%%%s-%s-%s, %s",facnam,severity[sts&0x7],msgnam,msgtext);
       }
     }
     LibFindFileEnd(&ctx);
     if (!(status & 1))
       sprintf(text,"%%%NONAME-%s-NOMSG, Message number 0x%08X",severity[sts&0x7],sts);
     return text;
   }


void MdsGetMsgDsc(int status, struct descriptor *out)
{
	static struct descriptor tmp = {0,DTYPE_T,CLASS_S,0};
	tmp.dscA_pointer = MdsGetMsg(status);
	tmp.dscW_length = strlen(tmp.dscA_pointer);
	StrCopyDx(out,&tmp);
	return;
}

	/****************************************************************
	 * setMdsMsgFlag:
	 ****************************************************************/
int   setMdsMsgFlag(		/* Return: old setting of flag		*/
    int   new			/* <r> new setting for flag		*/
   )
   {
    int   old;

    old = mdsmsgFlag;
    mdsmsgFlag = new;
    return(old);
   }



	/*****************************************************************
	 * MdsMsg:
	 *****************************************************************/
int   MdsMsg(			/* Return: sts provided by user		*/
    int   sts			/* <r> status code			*/
   ,char  fmt[]			/* <r> format statement			*/
   , ...			/* <r:opt> arguments to fmt[]		*/
   )
   {
    int   k;
    int   write2stdout;
    char  text[256];
    va_list  ap;		/* arg ptr				*/

    write2stdout = isatty(fileno(stdout)) ^ isatty(fileno(stderr));
    if ((sts & ALREADY_DISPLAYED) && (sts != -1))
        return(sts);

    sprintf(text,"\r%s: ",pgmname());
    k = strlen(text);
    if (fmt)
       {
        va_start(ap,fmt);	/* initialize "ap"			*/
        vsprintf(text+k,fmt,ap);
        if (sts)
           {
            MDSfprintf(stderr,"%s\n\r    sts=%s\n\n\r",text,MdsGetMsg(sts));
            if (write2stdout)
                MDSfprintf(stdout,"%s\n\r    sts=%s\n\n\r",
                    text,MdsGetMsg(sts));
           }
        else
           {
            MDSfprintf(stderr,"%s\n\r",text);
            if (write2stdout)
                MDSfprintf(stdout,"%s\n\r",text);
           }
       }
    else
       {
        MDSfprintf(stderr,"%s:  sts=%s\n\r",text,MdsGetMsg(sts));
        if (write2stdout)
            MDSfprintf(stdout,"%s:  sts=%s\n\r",text,MdsGetMsg(sts));
       }

    return(sts | ALREADY_DISPLAYED);
   }

#ifdef MAIN
void  main()
   {
    MdsMsg(MDSDCL_STS_SUCCESS,0);  MDSprintf("\n");
    MdsMsg(CLI_STS_PRESENT,0);  MDSprintf("\n");
    MdsMsg(CCL_STS_SUCCESS,0);  MDSprintf("\n");
    MdsMsg(TCL_STS_SUCCESS,0);  MDSprintf("\n");
    MdsMsg(TreeNORMAL,0);  MDSprintf("\n");
    MdsMsg(LibNOTFOU,0);  MDSprintf("\n");
    MdsMsg(StrMATCH,0);  MDSprintf("\n");
    MdsMsg(SsINTOVF,0);  MDSprintf("\n");
   }
#endif
