/* TO DO: 

  - ensure all data types covered
*/

#include "MdsLib.h"
#ifdef __VMS
extern int MDS$OPEN();
extern int MDS$CLOSE();
extern int MDS$VALUE();
extern int MDS$PUT();
extern int MDS$SET_DEFAULT();
extern int lib$callg();
#else
extern int TdiExecute();
extern int TdiCompile();
extern int TdiData();
#endif
short ArgLen(struct descrip *d);
static EMPTYXD(mdsValueAnswer);

static int next = 0;

#define MAXARGS 32

short dtype_length(struct descriptor *d)
{
  short len;
  switch (d->dtype)
  {
    case DTYPE_CSTRING :  len = d->length ? d->length : (d->pointer ? strlen(d->pointer) : 0); break;
    case DTYPE_UCHAR   :
    case DTYPE_CHAR    :  len = sizeof(char); break;
    case DTYPE_USHORT  :
    case DTYPE_SHORT   :  len = sizeof(short); break;
    case DTYPE_ULONG   :  
    case DTYPE_LONG    :  len = sizeof(long); break;
    case DTYPE_FLOAT   :  len = sizeof(float); break;
    case DTYPE_DOUBLE  :  len = sizeof(double); break;
    case DTYPE_FLOAT_COMPLEX :  len = sizeof(float) * 2; break;
    case DTYPE_DOUBLE_COMPLEX :  len = sizeof(double) * 2; break;
  }
  return len;
}

int descr (int *dtype, void *data, int *dim1, ...)

{

  struct descriptor *dsc = &descrs[next];
  int retval;
  int totsize = *dim1;

  dsc->dtype = *dtype;
  switch (dsc->dtype)
  {
    case DTYPE_F :  
      dsc->dtype = DTYPE_FLOAT;
      break;
    case DTYPE_D :  
      dsc->dtype = DTYPE_DOUBLE;
      break;
    case DTYPE_FC : 
      dsc->dtype = DTYPE_FLOAT_COMPLEX;
      break;
    default : 
      break;
  }
  dsc->pointer = (char *)data;
  dsc->length = dtype_length(dsc);

  if (*dim1 == 0) 
  {
    dsc->class = CLASS_S;
  }
  else 
  {

    va_list incrmtr;

    int ndim=1;
    int *dim = &ndim; /*must initialize dim to nonnull so test below passes */
    
    /* count the number of dimensions beyond the first */
    va_start(incrmtr, dim1);
    for (ndim = 1; *dim != 0; ndim++)
    {
      dim = va_arg(incrmtr, int *);
    }
    ndim = ndim - 1;  /* ndim is actually the number of dimensions specified */

    if (ndim > 1) 
    {
      int i;
      array_coeff *adsc = (array_coeff *)dsc;  /* &&& CAN I DO DYNAMIC NUMBER OF DIMENSIONS HERE??? */
      adsc->class = CLASS_A;
  
      if (ndim > MAXDIM) 
      {
	ndim = MAXDIM;
	printf("(descr.c) WARNING: requested ndim>MAXDIM, forcing to MAXDIM\n");
      }
      adsc->dimct = ndim;

      adsc->aflags.coeff = 1;
      adsc->a0 = adsc->pointer;  /* &&& this will need to be adjusted for native API, as (array lower bound=0) will not be required. */
      adsc->m[0] = *dim1;

      va_start(incrmtr, dim1);
      for (i = 1; i<ndim; i++) 
      {
	adsc->m[i] = *(va_arg(incrmtr, int *));
	totsize = totsize * adsc->m[i];
      }
      for (i = ndim; i<MAXDIM; i++)
      {
	adsc->m[i] = 0;
      }
      adsc->arsize = totsize * adsc->length;
   }
    else 
    {
      struct descriptor_a *adsc = (struct descriptor_a *)dsc;
      adsc->class = CLASS_A;
      adsc->arsize = totsize * adsc->length;
      adsc->dimct = 1;
      if (ndim < 1) printf("(descr.c) WARNING: requested ndim<1, forcing to 1.\n");
    }
      
 

  }
  retval = next+1;
  next++;
  if (next >= NDESCRIP_CACHE) next = 0;
  return retval;
}






void MdsConnect(char *host)
{
  mdsSocket = ConnectToMds(host);  /*** SETS GLOBAL VARIABLE mdsSOCKET ***/
}



struct descrip *MakeIpDescrip(struct descrip *arg, struct descriptor *dsc)
{

  int dtype;

  switch (dsc->dtype)
  {

    case DTYPE_FLOAT : 
      dtype = DTYPE_F;
      break;
    case DTYPE_DOUBLE : 
      dtype = DTYPE_D;
      break;
    case DTYPE_FLOAT_COMPLEX: 
      dtype = DTYPE_FC;
      break;
    default:
      dtype = dsc->dtype;
  }

  if (dsc->class == CLASS_S) 
  {
    arg = MakeDescrip(arg, dtype, 0, 0, dsc->pointer);
  } 
  else 
  {
    int i;
    array_coeff *adsc = (array_coeff *)dsc;  /* &&& CAN I DO DYNAMIC NUMBER OF DIMENSIONS HERE??? */
    if (adsc->dimct > 1) 
    {
      int dims[MAXDIM];
      for (i=0; i<adsc->dimct; i++) dims[i] = adsc->m[i];
      for (i=adsc->dimct; i<MAXDIM; i++) dims[i] = 0;
      arg = MakeDescrip(arg, dtype, adsc->dimct, dims, adsc->pointer);
    }
    else 
    {
      /*** &&& HOW AM I KEEPING TRACK OF N_ELEMENTS() IF THERE IS ONLY ONE DIM?  via LENGTH and ARSIZE??? ***/
    }
  }
  return arg;
}


int MdsValue(char *expression, ...) 
{

  va_list incrmtr;
  int a_count;
  int i;
  unsigned char nargs;
  unsigned char idx;
  int status = 1;
  int *descnum = &status;  /* initialize to point at non zero value */
  struct descriptor *dsc;

#ifdef __VMS
  void *arglist[MAXARGS];
  struct descriptor dexpression = {0,DTYPE_T,CLASS_S,0};
  int argidx = 1;
#endif


  va_start(incrmtr, expression);
  for (a_count = 0; *descnum != 0; a_count++)
  {
    descnum = va_arg(incrmtr, int *);
  }
  a_count--; /* subtract one for terminator of argument list */


#ifdef __VMS  
  dexpression.length = strlen((char *)expression);
  dexpression.pointer = (char *)expression;
  arglist[argidx++] = (void *)&dexpression;
  va_start(incrmtr, expression);
  for (i=1;i <= a_count; i++)
    {
      descnum = va_arg(incrmtr, int *);
      dsc = &descrs[*descnum-1];
      arglist[argidx++] = (void *)dsc;
    }
  *(int *)&arglist[0] = argidx; 
  return lib$callg(arglist,MDS$VALUE);

#else  

  /**************************** REPLACE client/server with regular access ******************/

  if (mdsSocket > 0)   /* CLIENT/SERVER */
  {
    struct descrip exparg;
    struct descrip *arg = &exparg;

    va_start(incrmtr, expression);
    nargs = a_count - 1 + 1;  /* -1 for answer argument, +1 for expression */

    arg = MakeDescrip(&exparg,DTYPE_CSTRING,0,0,expression);
    for (i=0;i<nargs && (status & 1);i++)
    {
      status = SendArg(mdsSocket, i, arg->dtype, nargs, ArgLen(arg), arg->ndims, arg->dims, arg->ptr);
      descnum = va_arg(incrmtr, int *);
      dsc = &descrs[*descnum-1];
      arg = MakeIpDescrip(arg, dsc);

    }

      

    if (status & 1)
    {
      short len;
      int numbytes;
      void *dptr;
      struct descrip exparg;
      struct descrip *arg = &exparg;

      /*      arg = MakeIpDescrip(arg, dsc);  */

      status = GetAnswerInfo(mdsSocket, &arg->dtype, &len, &arg->ndims, arg->dims, &numbytes, &dptr);


      arg->length = len; 
      if (numbytes)
      {
	status = (arg->dtype != DTYPE_CSTRING) || (arg->dtype == dsc->dtype);
	if (status) 
	{
	  switch (arg->dtype)
  	  {

/*	    case DTYPE_UCHAR :     
	      memcpy(dsc->pointer, (uchar *) dptr, numbytes);
	      break;
	    
	    case DTYPE_USHORT : 
	      memcpy(dsc->pointer, (ushort *) dptr, numbytes);
	      break;

  	    case DTYPE_ULONG : 
	      memcpy(dsc->pointer, (ulong *) dptr, numbytes);
	      break;
*/
  	    case DTYPE_CHAR : 
	      memcpy(dsc->pointer, (char *) dptr, numbytes);
	      break;

  	    case DTYPE_CSTRING:
	      memcpy(dsc->pointer, (char *) dptr, numbytes);
	      ((char *)dsc->pointer)[numbytes]=0;
	      break;

  	    case DTYPE_SHORT : 
	      memcpy(dsc->pointer, (short *) dptr, numbytes);
	      break;

  	    case DTYPE_LONG : 
	      memcpy(dsc->pointer, (long *) dptr, numbytes);
	      break;

  	    case DTYPE_F : 
	      memcpy(dsc->pointer, (float *) dptr, numbytes);
	      break;

  	    case DTYPE_D : 
	      memcpy(dsc->pointer, (double *) dptr, numbytes);
	      break;

	    default : 
	      status = 0;
	      break;
	  }
	}
      } 
      else 
      {
	status = 0;
      }
    
    }
  }
  else 
  {
    void *arglist[MAXARGS];
    struct descriptor *dsc;
    struct descriptor dexpression = {0,DTYPE_T,CLASS_S,0};
    EMPTYXD(tmp);
    int argidx = 1;
    int i;
    dexpression.length = strlen((char *)expression);
    dexpression.pointer = (char *)expression;
    arglist[argidx++] = (void *)&dexpression;
    va_start(incrmtr, expression);
    for (i=1;i < a_count; i++)
    {
      descnum = va_arg(incrmtr, int *);
      dsc = &descrs[*descnum-1];
      arglist[argidx++] = (void *)dsc;
    }
    arglist[argidx++] = (void *)&tmp;
    arglist[argidx++] = MdsEND_ARG;
    *(int *)&arglist[0] = argidx; 
    status = LibCallg(arglist,TdiExecute);

    if (status & 1)
    {


      status = TdiData(tmp.pointer,&mdsValueAnswer MDS_END_ARG);

      if (status) 
      {
	int numbytes;
	descnum = va_arg(incrmtr, int *);
	dsc = &descrs[*descnum-1];
        /* TdiCvt(&mdsValueAnswer,&dsc,&mdsValueAnswer MDS_END_ARG); */
	switch ((*mdsValueAnswer.pointer).class)
	{
	  case CLASS_S:
	  case CLASS_D:
	                 numbytes = (*mdsValueAnswer.pointer).length;
			 break;
	  case CLASS_A:
	                 numbytes = (*(struct descriptor_a *)mdsValueAnswer.pointer).arsize;
			 break;
	  default:
	                 status = 0;
	}

	if (status & 1)
	{
	  switch (dsc->dtype)
	  {
	
  	    case DTYPE_CHAR : 
	    case DTYPE_CSTRING: 
	      memcpy(dsc->pointer, (char *) (*mdsValueAnswer.pointer).pointer, numbytes);
	      break;

  	    case DTYPE_SHORT : 
	      memcpy(dsc->pointer, (short *) (*mdsValueAnswer.pointer).pointer, numbytes);
	      break;

  	    case DTYPE_LONG : 
	      memcpy(dsc->pointer, (long *) (*mdsValueAnswer.pointer).pointer, numbytes);
	      break;

  	    case DTYPE_FLOAT : 
	      memcpy(dsc->pointer, (float *) (*mdsValueAnswer.pointer).pointer, numbytes);
	      break;

  	    case DTYPE_DOUBLE : 
	      memcpy(dsc->pointer, (double *) (*mdsValueAnswer.pointer).pointer, numbytes);
	      break;

	    default : 
	      status = 0;
	      break;
	  }
	}
      }

      MdsFree1Dx(&tmp, NULL);
    }
  }
  return(status);
#endif

}


int  MdsPut(char *pathname, char *expression, ...)
{
  va_list incrmtr;
  int a_count;
  int i;
  unsigned char nargs;

#ifdef __VMS
  void *arglist[MAXARGS];
  struct descriptor dexpression = {0,DTYPE_T,CLASS_S,0};
  int argidx = 1;
#endif

  struct descriptor *dsc;
  int status = 1;
  int *descnum = &status;  /* initialize to point at non zero value */

  va_start(incrmtr, expression);
  for (a_count = 0; *descnum != 0; a_count++)
  {
    printf("a_count: %d   descnum: %d \n",a_count,*descnum);
    descnum = va_arg(incrmtr, int *);
  }
  a_count--; /* subtract one for terminator of argument list */


#ifdef __VMS

  dexpression.length = strlen((char *)expression);
  dexpression.pointer = (char *)expression;
  arglist[argidx++] = (void *)&dexpression;
  va_start(incrmtr, expression);
  for (i=1;i <= a_count; i++)
    {
      descnum = va_arg(incrmtr, int *);
      dsc = &descrs[*descnum-1];
      arglist[argidx++] = (void *)dsc;
    }
  *(int *)&arglist[0] = argidx; 
  return lib$callg(arglist,MDS$PUT);

#else

  /**************************** REPLACE client/server with regular access ******************/

  if (mdsSocket > 0)   /* CLIENT/SERVER */
  {
#ifdef _UNIX_SERVER
    static char *putexpprefix = "TreePutRecord(";
    static char *argplace = "$,";
#else
    static char *putexpprefix = "MDSLIB->MDS$PUT(";
    static char *argplace = "descr($),";
#endif
    char *putexp;
    struct descrip putexparg;
    struct descrip exparg;
    struct descrip *arg;
    unsigned char idx=0;
    nargs = a_count + 2; /* +1 for pathname +1 for expression */
    putexp = malloc(strlen(putexpprefix) + (nargs) * strlen(argplace) + 1);
    strcpy(putexp,putexpprefix);
    for (i=0;i<nargs;i++) strcat(putexp,argplace);
    putexp[strlen(putexp)-1] = ')';
    printf("PUT EXPRESSION: -->%s<--\n",putexp);
  
    va_start(incrmtr, expression);

    nargs = nargs + 1; /* add 1 for putexp  sent first */
    arg = MakeDescrip(&putexparg,DTYPE_CSTRING,0,0,putexp);
    status = SendArg(mdsSocket, idx++, arg->dtype, nargs, ArgLen(arg), arg->ndims, arg->dims, arg->ptr);
    free(putexp);
    arg = MakeDescrip(&exparg,DTYPE_CSTRING,0,0,pathname);
    status = SendArg(mdsSocket, idx++, arg->dtype, nargs, ArgLen(arg), arg->ndims, arg->dims, arg->ptr);
    arg = MakeDescrip(&exparg,DTYPE_CSTRING,0,0,expression);
    for (i=idx;i<nargs && (status & 1);i++)
    {
      status = SendArg(mdsSocket, (char)i, arg->dtype, nargs, ArgLen(arg), arg->ndims, arg->dims, arg->ptr);
      descnum = va_arg(incrmtr, int *);
      dsc = &descrs[*descnum-1];
      arg = MakeIpDescrip(arg, dsc);
    }
      
    if (status & 1)
    {
      char dtype;
      int dims[MAX_DIMS];
      char ndims;
      short len;
      int numbytes;
      void *dptr;
      status = GetAnswerInfo(mdsSocket, &dtype, &len, &ndims, dims, &numbytes, &dptr);
      if (status & 1 && dtype == DTYPE_LONG && ndims == 0 && numbytes == sizeof(int))
	memcpy(&status,dptr,numbytes);
    }
  }
  else 
  {

    void *arglist[MAXARGS];
    struct descriptor *dsc;
    struct descriptor dexpression = {0,DTYPE_T,CLASS_S,0};
    EMPTYXD(tmp);
    int argidx = 1;
    int i;
    int nid;

    if ((status = TreeFindNode(pathname, &nid)) & 1)
    {
      dexpression.length = strlen((char *)expression);
      dexpression.pointer = (char *)expression;
      arglist[argidx++] = (void *)&dexpression;
      va_start(incrmtr, expression);
      for (i=1;i <= a_count; i++)
      {
        descnum = va_arg(incrmtr, int *);
        dsc = &descrs[*descnum-1];
        arglist[argidx++] = (void *)dsc;
      }
      arglist[argidx++] = (void *)&tmp;
      arglist[argidx++] = MdsEND_ARG;
      *(int *)&arglist[0] = argidx; 

      status = LibCallg(arglist,TdiCompile);

      if (status & 1)
      {
	if ((status = TreePutRecord(nid, (struct descriptor *)arglist[argidx-2])) & 1)
	{ 
	  TreeWait();
	}
      }
      MdsFree1Dx(&tmp, NULL);
    }

  }
  return(status);
#endif
}


int  MdsOpen(char *tree, int shot)
{

  if (mdsSocket > 0)
  {

    /*    struct descrip treearg;
    struct descrip shotarg;
    struct descrip ansarg;*/

    long answer;
    int status;
    int d1,d2,d3; /* descriptor numbers passed to MdsValue */
    int dtype_cstring = DTYPE_CSTRING;
    int dtype_long = DTYPE_LONG;
    int null = 0;

#ifdef _UNIX_SERVER
    static char *expression = "TreeOpen($,$)";
#else
    static char *expression = "MDSLIB->MDS$OPEN($,$)";
#endif

    d1 = descr(&dtype_cstring,tree,&null);
    d2 = descr(&dtype_long,&shot, &null);
    d3 = descr(&dtype_long,&answer,&null);

    status = MdsValue(expression, &d1, &d2, &d3, &null);
    
    if ((status & 1))
    {
      return *(int *)&answer; 
    } else 
      return 0;  /*&&& should I return status here?  It looks messed up. &&& */
  }
  else 
  {

#ifdef __VMS
    struct descriptor treeexp = {0,DTYPE_T,CLASS_S,0};
    treeexp.length = strlen((char *)tree);
    treeexp.pointer = (char *)tree;
    return MDS$OPEN(&treeexp, &shot);
#else
    return TreeOpen(tree, shot, 0);
#endif

  }

}


int  MdsClose(char *tree, int shot)
{

  if (mdsSocket > 0)
  {


    long answer;
    int status;
    int d1,d2,d3; /* descriptor numbers passed to MdsValue */
    int dtype_cstring = DTYPE_CSTRING;
    int dtype_long = DTYPE_LONG;
    int null = 0;

#ifdef _UNIX_SERVER
    static char *expression = "TreeClose($,$)";
#else
    static char *expression = "MDSLIB->MDS$CLOSE($,$)";
#endif

    d1 = descr(&dtype_cstring,tree,&null);
    d2 = descr(&dtype_long,&shot, &null);
    d3 = descr(&dtype_long,&answer,&null);

    status = MdsValue(expression, &d1, &d2, &d3, &null);
    
    if ((status & 1))
    {
      return *(int *)&answer; 
    } else 
      return 0;  /*&&& should I return status here?  It looks messed up. &&& */
  }
  else 
  {
#ifdef __VMS
    struct descriptor treeexp = {0,DTYPE_T,CLASS_S,0};
    treeexp.length = strlen((char *)tree);
    treeexp.pointer = (char *)tree;
    return MDS$CLOSE(&treeexp, &shot);
#else
    return TreeClose(tree, shot);
#endif
  }


}



int  MdsSetDefault(char *node)
{

  if (mdsSocket > 0) {

#ifdef _UNIX_SERVER
    static char *expression = "TreeSetDefault($)";
#else
    static char *expression = "MDSLIB->MDS$SET_DEFAULT($)";
#endif

    long answer;
    int null = 0;
    int dtype_long = DTYPE_LONG;
    int dtype_cstring = DTYPE_CSTRING;
    int d1 = descr(&dtype_cstring, node, &null);
    int d2 = descr(&dtype_long, &answer, &null);
    int status = MdsValue(expression, &d1, &d2, &null);
    if ((status & 1))
    {
      return *(int *)&answer;
    }  else
      return 0;
  }

  else 

  {

#ifdef __VMS
    struct descriptor defaultexp = {0,DTYPE_T,CLASS_S,0};
    defaultexp.length = strlen((char *)node);
    defaultexp.pointer = (char *)node;
    return MDS$SET_DEFAULT(&defaultexp);
#else
    return TreeSetDefault(node);
#endif
  }

}


