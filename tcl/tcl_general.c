#include        "tclsysdef.h"

/***********************************************************************
* TCL_GENERAL.C --
*
* General-purpose routines ...
* History:
*  15-Jan-1998  TRG  Create.
*
************************************************************************/



	/***************************************************************
	 * TclShowDefault:
	 ***************************************************************/
int   TclShowDefault()		/* Returns: status			*/
   {
    char  *p;
    int nid;
    TreeGetDefaultNid(&nid);
    if (p = TreeGetPath(nid))
       {
        TclTextOut(p);
        TreeFree(p);
       }
    return(p ? 1 : 0);
   }



	/*****************************************************************
	 * TclBeginConglomerate:
	 *****************************************************************/
/*int   TclBeginConglomerate()	/* Returns: status			*/
/*   {
/*    int   size;
/*    int   sts;
/*    static DYNAMIC_DESCRITOR(dsc_conglomerate);
/*
/*    sts = cli_get_value("CONGLOMERATE",&dsc_conglomerate);
/*    if (~sts & 1)
/*        return(sts);
/*    if (sscanf(dsc_conglomerate.dscA_pointer,"%d",&size) != 1)
/*        return(mdsMsg(TCL_STS_ERROR,"Error interpreting CONGLOMERATE '%s'",
/*            dsc_conglomerate.dscA_pointer));
/*
/*    sts = TreeStartConglomerate(size);
/*    return(sts);
/*   }
/*
/*
/*
/*	/****************************************************************
/*
/*	 * TclEndConglomerate:
/*	 ****************************************************************/
/*int   TclEndConglomerate()	/* Return: status			*/
/*   {
/*    return(TclEndConglomerate());
/*   }					/*  */
