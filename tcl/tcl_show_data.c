#include        "tclsysdef.h"
#include        <mds_stdarg.h>
#include        <mdsshr.h>
#include        <usagedef.h>

/**********************************************************************
* TCL_SHOW_DATA.C --
*
* TclShowData:  Show data in mdsPlus node.
*
* History:
*  20-Feb-1998  TRG  Create.  Ported from original mdsPlus code.
*
************************************************************************/


		/*=======================================================
		 * Function prototypes:
		 *======================================================*/
static int CvtDxT(struct descriptor *in_dsc_ptr,int depth);
char  *MdsDtypeString(char);

#ifdef vms
#define TdiOpcodeString  tdi$opcode_string
#endif

extern int TdiOpcodeString();



	/****************************************************************
	 * lib_cvt_dx_dx:
	 * Convert data for display ...
	 ****************************************************************/
static int   lib_cvt_dx_dx(	/* Returns:  status			*/
    struct descriptor  *dsc_in	/* <r> data to convert			*/
   ,struct descriptor  *dsc_out	/* <w> output string			*/
   ,unsigned short *len		/* <w> length of output string		*/
   )
   {
    char   dtype;
    char  string[48];
    void  *p;

    if (!is_cdescr(dsc_out) && !is_ddescr(dsc_out))
        return(MdsMsg(0,"lib_cvt_dx_dx: dsc_out is not a descriptor"));

    p = dsc_in->dscA_pointer;
    dtype = dsc_in->dscB_dtype;

    if (dtype == DTYPE_T)
        return(str_copy_dx(dsc_out,dsc_in));	/*------------> return	*/

    switch(dtype)
       {
        default:
            sprintf(string,"CantDisplay%s",MdsDtypeString(dtype));
            break;
        case DTYPE_B:
        case DTYPE_BU:
            sprintf(string,"0x%02X",*(unsigned char *)p);
            break;
        case DTYPE_NATIVE_FLOAT:
            sprintf(string,"%.8g",*(float *)p);
            break;
        case DTYPE_NATIVE_DOUBLE:
            sprintf(string,"%.14lg",*(double *)p);
            break;
        case DTYPE_L:
            sprintf(string,"%d",*(int *)p);
            break;
        case DTYPE_LU:
            sprintf(string,"%u",*(unsigned int *)p);
            break;
        case DTYPE_W:
            sprintf(string,"%d",*(short *)p);
            break;
        case DTYPE_WU:
            sprintf(string,"%u",*(unsigned short *)p);
            break;
       }
    return(str_copy_dx(dsc_out,string));
   }



	/****************************************************************
	 * TclDtypeString:
	 * Simply a re-formatted version of MdsDtypeString ...
	 ****************************************************************/
static char  *TclDtypeString(	/* Returns:  address of formatted string */
    char  dtype			/* <r> data type code			*/
   )
   {
    static char  string[48];

    sprintf(string,"%-30s: ",MdsDtypeString(dtype));
    return(string);
   }



	/****************************************************************
	 * CvtIdentT:
	 ****************************************************************/
static int CvtIdentT(struct descriptor *in_dsc_ptr,int depth)
   {
    static DYNAMIC_DESCRIPTOR(out_str);

    str_dupl_char(&out_str,depth,' ');
    str_concat(&out_str,&out_str,
        TclDtypeString(in_dsc_ptr->dscB_dtype),in_dsc_ptr->dscA_pointer,0);
    TclTextOut(out_str.dscA_pointer);
    str_free1_dx(&out_str);
    return 1;
   }



	/****************************************************************
	 * CvtNidT:
	 ****************************************************************/
static int CvtNidT(struct descriptor *in_dsc_ptr,int depth)
   {
    int   nid;
    int sts;
    char  *pathname;
    struct descriptor_xd lxd = {0, 0, CLASS_XD, 0, 0};
    static DYNAMIC_DESCRIPTOR(out_str);
    static DYNAMIC_DESCRIPTOR(spaces);
    char  *dstr;

    str_dupl_char(&spaces,depth,' ');
    dstr = TclDtypeString(in_dsc_ptr->dscB_dtype);

    nid = *(int *)in_dsc_ptr->dscA_pointer;
    if (nid == 0)
       {
        str_concat(&out_str,&spaces,dstr,"$VALUE",0);
        TclTextOut(out_str.dscA_pointer);
       }
    else
       {
        if (pathname = TreeGetPath(nid))
           {
            str_concat(&out_str,&spaces,dstr,pathname,0);
            TreeFree(pathname);
            TclTextOut(out_str.dscA_pointer);
            sts = TreeGetRecord(nid,&lxd);
            if (sts & 1)
               {
                sts = CvtDxT((struct descriptor *)(&lxd),depth + 4);
                MdsFree1Dx(&lxd,NULL);
               }
            else
                sts = 1;
           }
        else
           {
            str_concat(&out_str,&spaces,dstr,
                        "***** Bad Nid Reference ********",0);
            TclTextOut(out_str.dscA_pointer);
           }
       }
    str_free1_dx(&out_str);
    str_free1_dx(&spaces);
    return sts;
   }



	/**************************************************************
	 * CvtNumericT:
	 **************************************************************/
static int CvtNumericT(struct descriptor *in_dsc_ptr,int depth)
   {
    int sts;
    char  *dstr;
#define STATIC_STRING_LEN 132
    static char static_chars[STATIC_STRING_LEN];
    static DESCRIPTOR(static_str,static_chars);
    static DYNAMIC_DESCRIPTOR(out_str);
    static DYNAMIC_DESCRIPTOR(spaces);
    static_str.dscW_length = STATIC_STRING_LEN;

    dstr = TclDtypeString(in_dsc_ptr->dscB_dtype);
    str_dupl_char(&spaces,depth,' ');
    sts = lib_cvt_dx_dx(in_dsc_ptr,&static_str,&static_str.dscW_length);
    if (sts & 1)
        str_concat(&out_str,&spaces,dstr,&static_str,0);
    else
        str_concat(&out_str,&spaces,dstr,
                "There is no primative display for this datatype",0);
    TclTextOut(out_str.dscA_pointer);
    str_free1_dx(&out_str);
    str_free1_dx(&spaces);
    return 1;
   }



	/***************************************************************
	 * CvtPathT:
	 ***************************************************************/
static int CvtPathT(struct descriptor *in_dsc_ptr,int depth)
   {
    int sts;
    int ldepth;
    int lnid;
    char  *dstr;
    static DYNAMIC_DESCRIPTOR(out_str);
    static DYNAMIC_DESCRIPTOR(spaces);
    struct descriptor_xd lxd = {0, 0, CLASS_XD, 0, 0};

    str_dupl_char(&spaces,depth,' ');
    dstr = TclDtypeString(in_dsc_ptr->dscB_dtype);
    str_concat(&out_str,&spaces,dstr,in_dsc_ptr->dscA_pointer,0);
    TclTextOut(out_str.dscA_pointer);

    ldepth = depth + 4;
    str_free1_dx(&spaces);
    str_dupl_char(&spaces,ldepth,' ');
    sts = TreeFindNode(in_dsc_ptr->dscA_pointer,&lnid);
    if (sts & 1)
       {
        sts = TreeGetRecord(lnid,&lxd);
        if (sts & 1)
           {
            sts = CvtDxT((struct descriptor *) & lxd,depth + 4);
            MdsFree1Dx(&lxd,NULL);
           }
        else
           {
            str_concat(&out_str,&spaces,"Error reading data record",0);
            TclTextOut(out_str.dscA_pointer);
           }
       }
    else
       {
        str_concat(&out_str,&spaces,"Record not found",0);
        TclTextOut(out_str.dscA_pointer);
       }
    str_free1_dx(&out_str);
    str_free1_dx(&spaces);
    return 1;
   }



	/*************************************************************
	 * CvtDdscT:
	 *************************************************************/
static int CvtDdscT(struct descriptor *in_dsc_ptr,int depth)
   {
    int sts;
    char  *dstr;
    static DYNAMIC_DESCRIPTOR(spaces);
    static DYNAMIC_DESCRIPTOR(out_str);

    if (in_dsc_ptr->dscB_dtype == DTYPE_DSC)
        sts = CvtDxT((struct descriptor *)in_dsc_ptr->dscA_pointer,depth);
    else
       {
        switch (in_dsc_ptr->dscB_dtype)
           {
            case DTYPE_T:
                dstr = TclDtypeString(in_dsc_ptr->dscB_dtype);
                str_dupl_char(&spaces,depth,' ');
                str_concat(&out_str,&spaces,dstr,in_dsc_ptr,0);
                TclTextOut(out_str.dscA_pointer);
                sts = 1;
                break;

            case DTYPE_NID:
                sts = CvtNidT(in_dsc_ptr,depth);
                break;

            case DTYPE_PATH:
                sts = CvtPathT(in_dsc_ptr,depth);
                break;

            case DTYPE_IDENT:
                sts = CvtIdentT(in_dsc_ptr,depth);
                break;

            default:
                sts = CvtNumericT(in_dsc_ptr,depth);
                break;
           }
       }
    str_free1_dx(&out_str);
    str_free1_dx(&spaces);
    return sts;
   }



	/***************************************************************
	 * CvtGenericRT:
	 ***************************************************************/
static int CvtGenericRT(struct descriptor_r *in_dsc_ptr,int depth)
   {

    static DYNAMIC_DESCRIPTOR(spaces);
    static DYNAMIC_DESCRIPTOR(out_str);
    int i;
    int sts = 1;
    int ldepth;

    ldepth = depth + 4;
    for (i = 0; sts && i < in_dsc_ptr->dscB_ndesc; i++)
       {
        if (in_dsc_ptr->dscA_dscptrs[i])
           {
            sts = CvtDxT(in_dsc_ptr->dscA_dscptrs[i],depth + 4);
           }
        else
           {
            str_dupl_char(&spaces,ldepth,' ');
            str_concat(&out_str,&spaces,"*** EMPTY ****",0);
            TclTextOut(out_str.dscA_pointer);
            sts = 1;
           }
       }
    str_free1_dx(&out_str);
    str_free1_dx(&spaces);
    return sts;
   }



	/***************************************************************
	 * CvtFunctionT:
	 ***************************************************************/
static int CvtFunctionT(struct descriptor *in_dsc_ptr,int depth)
   {
    int sts;
    char  *dstr;
    static DYNAMIC_DESCRIPTOR(spaces);
    static struct descriptor_xd ostr = {0, DTYPE_T, CLASS_XD, 0, 0};
    static DYNAMIC_DESCRIPTOR(out_str);
    struct descriptor_s opcode_dsc = {2,DTYPE_WU,CLASS_S,(char *) 0};

    opcode_dsc.dscA_pointer = (char *) in_dsc_ptr->dscA_pointer;
    str_dupl_char(&spaces,depth,' ');
    dstr = TclDtypeString(in_dsc_ptr->dscB_dtype);
    sts = TdiOpcodeString(&opcode_dsc,&ostr MDS_END_ARG);
    if (sts & 1)
       {
        str_concat(&out_str,&spaces,dstr,ostr.dscA_pointer,0);
        TclTextOut(out_str.dscA_pointer);
        sts = CvtGenericRT((struct descriptor_r *) in_dsc_ptr,depth);
       }
    MdsFree1Dx(&ostr,NULL);
    str_free1_dx(&out_str);
    str_free1_dx(&spaces);
    return sts;
   }



	/***************************************************************
	 * CvtRdscT:
	 ***************************************************************/
static int CvtRdscT(struct descriptor *in_dsc_ptr,int depth)
   {
    int sts;
    char  *dstr;
    static DYNAMIC_DESCRIPTOR(spaces);
    static DYNAMIC_DESCRIPTOR(out_str);

    if (in_dsc_ptr->dscB_dtype == DTYPE_FUNCTION)
        sts = CvtFunctionT(in_dsc_ptr,depth);
    else
       {
        str_dupl_char(&spaces,depth,' ');
        dstr = TclDtypeString(in_dsc_ptr->dscB_dtype);
        str_concat(&out_str,&spaces,dstr,0);
        TclTextOut(out_str.dscA_pointer);
        sts = CvtGenericRT((struct descriptor_r *) in_dsc_ptr,depth);
        str_free1_dx(&out_str);
        str_free1_dx(&spaces);
       }
    return sts;
   }



	/****************************************************************
	 * CvtAdscT:
	 ****************************************************************/
static int CvtAdscT(struct descriptor_a *in_dsc_ptr,int depth)
   {
    char  *dstr;
    int *bptr;
    int *lbptr;
    int *ubptr;
    int dim;
#define BOUNDS_LENGTH 32		/* formerly 16	*/
    char bchars[BOUNDS_LENGTH];
    static DYNAMIC_DESCRIPTOR(spaces);
    static DYNAMIC_DESCRIPTOR(out_str);

    str_dupl_char(&spaces,depth,' ');
    dstr = TclDtypeString(in_dsc_ptr->dscB_dtype);
    str_concat(&out_str,&spaces,dstr,"Array [ ",0);
    if (in_dsc_ptr->aflags.dscV_bounds)
       {
        bptr = (int *) ((char *)in_dsc_ptr + sizeof(struct descriptor_a) +
                sizeof(void *) + (in_dsc_ptr->dscB_dimct) * sizeof(int));
        for (dim = 0; dim < in_dsc_ptr->dscB_dimct; dim++)
           {
            lbptr = bptr++;
            ubptr = bptr++;
            sprintf(bchars,"%d:%d%s",*lbptr,*ubptr,
                        (dim < in_dsc_ptr->dscB_dimct - 1) ? "," : "");
            str_append(&out_str,bchars);
           }
       }
    else if (in_dsc_ptr->aflags.dscV_coeff)
       {
        bptr = (int *) ((char *)in_dsc_ptr + sizeof(struct descriptor_a) +
                        sizeof(void *));
        for (dim = 0; dim < in_dsc_ptr->dscB_dimct; dim++)
           {
            sprintf(bchars,"%d%s",*bptr++,
                        (dim < in_dsc_ptr->dscB_dimct - 1) ? "," : "");
            str_append(&out_str,bchars);
           }
       }
    else
       {
        sprintf(bchars,"%d",in_dsc_ptr->dscL_arsize / in_dsc_ptr->dscW_length);
        str_append(&out_str,bchars);
       }
    str_append(&out_str," ]");
    TclTextOut(out_str.dscA_pointer);
    str_free1_dx(&out_str);
    str_free1_dx(&spaces);
    return 1;
   }



	/****************************************************************
	 * CvtDxT:
	 ****************************************************************/
static int CvtDxT(struct descriptor *in_dsc_ptr,int depth)
   {
    int sts;
    static DYNAMIC_DESCRIPTOR(spaces);
    static DYNAMIC_DESCRIPTOR(out_str);

    str_dupl_char(&spaces,depth,' ');
    switch (in_dsc_ptr->dscB_class)
       {
        case CLASS_XD:
        case CLASS_XS:
             {
              if (in_dsc_ptr->dscB_dtype == DTYPE_DSC)
                  sts = CvtDxT((struct descriptor *) in_dsc_ptr->dscA_pointer,depth);
              else
                 {
                  str_concat(&out_str,&spaces,
                          "Invalid Dtype for XD or XS must be DSC",0);
                  TclTextOut(out_str.dscA_pointer);
                 }
             }
          break;

        case CLASS_D:
        case CLASS_S:
            sts = CvtDdscT(in_dsc_ptr,depth);
            break;

        case CLASS_R:
            sts = CvtRdscT(in_dsc_ptr,depth);
            break;

        case CLASS_A:
        case CLASS_CA:
        case CLASS_APD:
            sts = CvtAdscT((struct descriptor_a *) in_dsc_ptr,depth);
            break;
       }

    str_free1_dx(&out_str);
    return sts;
   }



	/****************************************************************
	 * TclShowData:
	 ****************************************************************/
int TclShowData()
   {
    int   nid;
    int   sts;
    int   usageMask;
    char  *pathnam;
    void  *ctx = 0;
    static DYNAMIC_DESCRIPTOR(dsc_nodnam);
    static DYNAMIC_DESCRIPTOR(out_line);
    static struct descriptor_xd data = {0,0,CLASS_XD,0};

    usageMask = -1;
    while (cli_get_value("NODE",&dsc_nodnam) & 1)
       {
        l2u(dsc_nodnam.dscA_pointer,0);
        while (TreeFindNodeWild(dsc_nodnam.dscA_pointer,&nid,&ctx,usageMask) & 1)
           {
            pathnam = TreeGetPath(nid);
            TclTextOut(pathnam);
            TreeFree(pathnam);
            sts = TreeGetRecord(nid,&data);
            if (sts & 1)
               {
                str_free1_dx(&out_line);
                sts = CvtDxT((struct descriptor *) (&data),1);
                if (sts & 1 == 0)
                  TclTextOut("   Error displaying data");
               }
            else
                TclTextOut("   No data found");
           }
        TreeFindNodeEnd(&ctx);
       }
    return 1;
   }
