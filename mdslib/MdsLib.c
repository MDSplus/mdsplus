/* TO DO: 

  - ensure all data types covered
*/

#include "mdslib.h"
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
extern int LibCallg();
extern int MdsFree1Dx();
extern int TreeFindNode();
extern int TreePutRecord();
extern int TreeWait();
extern int TreeOpen();
extern int TreeClose();
extern int TreeSetDefault();
#endif
short ArgLen(struct descrip *d);

static int next = 0;
#define MIN(A,B)  ((A) < (B) ? (A) : (B))
#define MAXARGS 32

int dtype_length(struct descriptor *d)
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

  /* variable length argument list: 
   * (# elements in dim 1), (# elements in dim 2) ... 0, [length of (each) string if DTYPE_CSTRING]
   */

     
  struct descriptor *dsc; 
  int totsize = *dim1;
  int retval;

  if (descrs[next]) free(descrs[next]);

  /* decide what type of descriptor is needed (descriptor, descriptor_a, array_coeff) */

  if (*dim1 == 0) 
    {
      descrs[next] = malloc(sizeof(struct descriptor));
    }
  else
    {
      va_list incrmtr;
      int *dim;

      va_start(incrmtr, dim1);
      dim = va_arg(incrmtr, int *);
      if (*dim == 0) 
	{
	  descrs[next] = malloc(sizeof(struct descriptor_a));
	}
      else
	{
	  descrs[next] = malloc(sizeof(array_coeff));
	}
    }

  dsc = descrs[next];

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

    if (dsc->dtype == DTYPE_CSTRING) /* && dsc->length == 0)  */
    {
	 va_list incrmtr;
	 va_start(incrmtr, dim1);
	 dsc->length = *va_arg(incrmtr, int *);
    }


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

    /* if requested descriptor is for a DTYPE_CSTRING, then following the null terminated 
     * list of dimensions there will be an int * to the length of each string in the array
     */

    if (dsc->dtype == DTYPE_CSTRING && dsc->length == 0) 
    {
	 dsc->length = *va_arg(incrmtr, int *);
    }


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


int *cdescr (int dtype, void *data, ...)
{
    void *arglist[MAXARGS];
    va_list incrmtr;
    int dsc;
    int status;
    int argidx=1;
    arglist[argidx++] = (void *)&dtype;
    arglist[argidx++] = (void *)data;

    va_start(incrmtr, data);

    dsc = 1;  /* initialize ok */
    for ( ; dsc != 0; )
    {
      dsc = va_arg(incrmtr, int);
      arglist[argidx++] = (void *)&dsc;
    }

    if (dtype == DTYPE_CSTRING)
      {
	dsc = va_arg(incrmtr, int);
	arglist[argidx++] = (void *)&dsc;
      }
    arglist[argidx++] = MdsEND_ARG;
    *(int *)&arglist[0] = argidx; 
    status = LibCallg(arglist,descr);
    return(&status);
}


void MdsDisconnect()
{
  DisconnectFromMds(mdsSocket);
  mdsSocket = INVALID_SOCKET;      /*** SETS GLOBAL VARIABLE mdsSOCKET ***/
}


SOCKET MdsConnect(char *host)
{
  if (mdsSocket != INVALID_SOCKET)
    {
      MdsDisconnect(); 
    }
  mdsSocket = ConnectToMds(host);  /*** SETS GLOBAL VARIABLE mdsSOCKET ***/
  return(mdsSocket);
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
    arg = MakeDescrip(arg, (char)dtype, (char)0, (int *)0, dsc->pointer);
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
      arg = MakeDescrip(arg, (char)dtype, adsc->dimct, dims, adsc->pointer);
    }
    else 
    {
      /*** &&& HOW AM I KEEPING TRACK OF N_ELEMENTS() IF THERE IS ONLY ONE DIM?  via LENGTH and ARSIZE??? ***/
    }
  }
  return arg;
}


int MdsValueLength(int numbytes, struct descriptor *dsc)
{
  int length;

  switch (dsc->class) 
    {
    case CLASS_S:
    case CLASS_D:
      if (dsc->dtype == DTYPE_CSTRING) 
	{
	  length = MIN(numbytes, dsc->length);
	}
      else
	{
	  length = numbytes / dtype_length(dsc); 
	}
      break;

    case CLASS_A:
      length = numbytes / (*(struct descriptor_a *)dsc).length;
      break;

    default:
      length = 0;
    }
  return length;

}


char *MdsValueRemoteExpression(char *expression, struct descriptor *dsc)
{

  /* This function will wrap expression in the appropriate type
   * conversion function to ensure that the return value of MdsValue
   * is of the right type.  It is only used for remote MDSplus 
   */

  char *newexpression = (char *) malloc(strlen(expression)+13);
				
  switch (dsc->dtype) 
    {
    case DTYPE_CSTRING: strcpy(newexpression,"TEXT"); break;
    case DTYPE_UCHAR  : strcpy(newexpression,"BYTE_UNSIGNED"); break;
    case DTYPE_CHAR   : strcpy(newexpression,"BYTE"); break;
    case DTYPE_USHORT : strcpy(newexpression,"WORD_UNSIGNED"); break;
    case DTYPE_SHORT  : strcpy(newexpression,"WORD"); break;
    case DTYPE_ULONG  : strcpy(newexpression,"ULONG"); break;
    case DTYPE_LONG   : strcpy(newexpression,"LONG"); break;
    case DTYPE_FLOAT  : strcpy(newexpression,"F_FLOAT"); break;
    case DTYPE_DOUBLE : strcpy(newexpression,"DBLE"); break;
    }

  strcat(newexpression, "(");
  strcat(newexpression, expression);
  strcat(newexpression, ")\0");

  return newexpression;

}

void MdsValueSet(struct descriptor *dsc, void *answerptr, int numbytes)
{
  int status;

  status = (numbytes > 0);
  if (status) 
    {
      switch (dsc->class)
	{
	case CLASS_S:
	case CLASS_D:
	  numbytes = MIN(numbytes, dsc->length);
	  break;
	case CLASS_A:
	  numbytes = MIN(numbytes, 
			 ((struct descriptor_a *)dsc)->arsize);
	  break;
	default:
	  status = 0;
	}
    }

  if (status & 1) 
    { 
      memcpy(dsc->pointer, (char *) answerptr, numbytes);
      if (dsc->dtype == DTYPE_CSTRING && numbytes < dsc->length) 
	strcpy(dsc->pointer+numbytes, "\0");
    }
    
}


int MdsValue(char *expression, ...) 
{

  va_list incrmtr;
  int a_count;
  int i;
  unsigned char nargs;
  struct descriptor *dsc;
  int *length;
  int status = 1;
  int *descnum = &status;  /* initialize to point at non zero value */


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

  length = va_arg(incrmtr, int *);
  if (length) 
    {
      *length = 0;
    }

#ifdef __VMS  
  dexpression.length = strlen((char *)expression);
  dexpression.pointer = (char *)expression;
  arglist[argidx++] = (void *)&dexpression;
  va_start(incrmtr, expression);
  for (i=1;i <= a_count; i++)
    {
      descnum = va_arg(incrmtr, int *);
      dsc = descrs[*descnum-1];
      arglist[argidx++] = (void *)dsc;
    }
  if (length) arglist[argidx++] = (void *)length;
  *(int *)&arglist[0] = argidx; 
  return lib$callg(arglist,MDS$VALUE);

#else  


  if (mdsSocket > 0)   /* CLIENT/SERVER */
  {
    struct descriptor *dscAnswer;
    struct descrip exparg;
    struct descrip *arg = &exparg;
    char *newexpression;

    va_start(incrmtr, expression);
    nargs = a_count - 1 ;  /* -1 for answer argument */

    /* Get last argument - it is the answer descriptor number */
    for (i=1;i<=nargs+1; i++) descnum = va_arg(incrmtr, int *);
    dscAnswer = descrs[*descnum - 1];

    /* 
     * Send expression descriptor first.
     * MdsValueRemoteExpression wraps expression with type conversion function.
     * It malloc's space for newexpression that needs to be freed after
     */
    newexpression = MdsValueRemoteExpression(expression,dscAnswer);  
    arg = MakeDescrip(&exparg,DTYPE_CSTRING,0,0,newexpression);
    status = SendArg(mdsSocket, 0, arg->dtype, (char)nargs+1, ArgLen(arg), arg->ndims, arg->dims, arg->ptr);
    free(newexpression); 

    /* send each argument */

    va_start(incrmtr, expression);
    for (i=1;i<=nargs && (status & 1);i++)
    {
      descnum = va_arg(incrmtr, int *);
      if (*descnum > 0)
      {
        dsc = descrs[*descnum-1];
        arg = MakeIpDescrip(arg, dsc);
	status = SendArg(mdsSocket, (unsigned char)i, arg->dtype, (char)nargs+1, ArgLen(arg), arg->ndims, arg->dims, arg->ptr);
      }
      else
	{
	  printf("I: %d    BAD DESCRIPTOR!!!\n",i);
	}

    }


    if (status & 1)
    {
      int numbytes;
      short len;
      void *dptr;
      struct descrip exparg;
      struct descrip *arg = &exparg;

      arg = MakeIpDescrip(arg, dscAnswer);
      status = GetAnswerInfo(mdsSocket, &arg->dtype, &len, &arg->ndims, arg->dims, &numbytes, &dptr);
      if (status & 1) MdsValueSet(dscAnswer, dptr, numbytes);
      if (length && numbytes) {
	if (arg->ndims == 0 ) 
	  {
	    *length = MdsValueLength(numbytes,dscAnswer);
	  }
	else {
	  int i;
	  for (i=0;i<arg->ndims;i++) *length = *length + arg->dims[i];
	}
      }
    }

  }
  else 
  {
    void *arglist[MAXARGS];
    struct descriptor *dsc;
    struct descriptor dexpression = {0,DTYPE_T,CLASS_S,0};
    EMPTYXD(xd1);
    EMPTYXD(xd2);
    EMPTYXD(xd3);
    int argidx = 1;
    int i;
    dexpression.length = strlen((char *)expression);
    dexpression.pointer = (char *)expression;
    arglist[argidx++] = (void *)&dexpression;
    va_start(incrmtr, expression);
    for (i=1;i < a_count; i++)
    {
      descnum = va_arg(incrmtr, int *);
      dsc = descrs[*descnum-1];
      arglist[argidx++] = (void *)dsc;
    }
    arglist[argidx++] = (void *)&xd1;
    arglist[argidx++] = MdsEND_ARG;
    *(int *)&arglist[0] = argidx; 
    status = LibCallg(arglist,TdiExecute);

    if (status & 1)
    {
	 
      descnum = va_arg(incrmtr, int *);
      dsc = descrs[*descnum-1];

      status = TdiData(xd1.pointer,&xd2 MDS_END_ARG);

      if (status & 1) 
	{      
	  status = TdiCvt(&xd2,dsc,&xd3 MDS_END_ARG);
	}
      
      if (status & 1) 
      {
	int numbytes;
	switch ((*xd3.pointer).class)
	{
	  case CLASS_S:
	  case CLASS_D:
	                 numbytes = (*xd3.pointer).length;
			 break;
	  case CLASS_A:
	                 numbytes = (*(struct descriptor_a *)xd3.pointer).arsize;
			 break;
	  default:
	                 status = 0;
	}

	if (status & 1) 
	  {
	    MdsValueSet(dsc, (*xd3.pointer).pointer, numbytes);
	    if (length && numbytes) *length = MdsValueLength(numbytes,xd3.pointer);
	  }
	MdsFree1Dx(&xd1, NULL);
	MdsFree1Dx(&xd2, NULL);
	MdsFree1Dx(&xd3, NULL);  /* is answerptr still valid after calling this??? */
      }
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
      dsc = descrs[*descnum-1];
      arglist[argidx++] = (void *)dsc;
    }
  *(int *)&arglist[0] = argidx; 
  return lib$callg(arglist,MDS$PUT);

#else


  if (mdsSocket > 0)   /* CLIENT/SERVER */
  {
    static char *putexpprefix = "TreePut(";
    static char *argplace = "$,";
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
      if (*descnum > 0)
      {
        dsc = descrs[*descnum-1];
        arg = MakeIpDescrip(arg, dsc);
      }
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
        dsc = descrs[*descnum-1];
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


int  MdsOpen(char *tree, int *shot)
{

  if (mdsSocket > 0)
  {

    long answer;
    int status;
    int length;
    int d1,d2,d3; /* descriptor numbers passed to MdsValue */
    int dtype_cstring = DTYPE_CSTRING;
    int dtype_long = DTYPE_LONG;
    int null = 0;

    static char *expression = "TreeOpen($,$)";

    length = strlen(tree);
    d1 = descr(&dtype_cstring,tree,&null,&length);
    d2 = descr(&dtype_long,shot, &null);
    d3 = descr(&dtype_long,&answer,&null);

    status = MdsValue(expression, &d1, &d2, &d3, &null, &length);
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
    return MDS$OPEN(&treeexp, shot);
#else
    return TreeOpen(tree, *shot, 0);
#endif

  }

}


int  MdsClose(char *tree, int *shot)
{

  if (mdsSocket > 0)
  {


    long answer;
    int length;
    int status;
    int d1,d2,d3; /* descriptor numbers passed to MdsValue */
    int dtype_cstring = DTYPE_CSTRING;
    int dtype_long = DTYPE_LONG;
    int null = 0;

    static char *expression = "TreeClose($,$)";

    length = strlen(tree);
    d1 = descr(&dtype_cstring,tree,&null,&length);
    d2 = descr(&dtype_long,shot, &null);
    d3 = descr(&dtype_long,&answer,&null);

    status = MdsValue(expression, &d1, &d2, &d3, &null, &length);
    
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
    return MDS$CLOSE(&treeexp, shot);
#else
    return TreeClose(tree, *shot);
#endif
  }


}



int  MdsSetDefault(char *node)
{

  if (mdsSocket > 0) {

    static char *expression = "TreeSetDefault($)";

    long answer;
    int length = strlen(node);
    int null = 0;
    int dtype_long = DTYPE_LONG;
    int dtype_cstring = DTYPE_CSTRING;
    int d1 = descr(&dtype_cstring, node, &null,&length);
    int d2 = descr(&dtype_long, &answer, &null);
    int status = MdsValue(expression, &d1, &d2, &null, &length);
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


