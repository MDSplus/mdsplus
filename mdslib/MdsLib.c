#ifndef FORTRAN_ENTRY_POINTS

/*************************************************************************
Note: This file gets loaded twice during compilation. Once to compile
the C entry points and a second time to generate the Fortran specific
entry points for the target platform.  The first part of the file
enclosed by the ifndef FORTRAN_ENTRY_POINTS loads the header files and
defines some utility routines.  Since MdsConnect, MdsDisconnect,
MdsOpen, MdsClose, and MdsSetDefault do not need to handle variable
argument lists, we can just have fortran entry points that calls these
C functions directly. The functions descr, MdsValue and MdsPut have
variable argument lists so a second compilation of these routines is
necessary to create the fortran entry points.  See more notes at
bottom of this file for configuring fortran entry points.
**************************************************************************/
#ifdef __VMS
#include <descrip.h>
#endif
#include "mdslib.h"
SOCKET mdsSocket=INVALID_SOCKET;
#ifndef _CLIENT_ONLY
#ifdef __VMS
extern int MDS$OPEN();
extern int MDS$CLOSE();
extern int MDS$VALUE();
extern int MDS$PUT();
extern int MDS$SET_DEFAULT();
#define TdiExecute TDI$EXECUTE
#define TdiCompile TDI$COMPILE
#define TdiData TDI$DATA
#define LibCallg lib$callg
#define TreeFindNode TREE$FIND_NODE
#define TdiCvt TDI$CVT
#define TreePutRecord TREE$PUT_RECORD
#define TreeWait TREE$WAIT
#else
extern int TreeOpen();
extern int TreeClose();
extern int TreeSetDefault();
#endif
extern int MdsFree1Dx();
extern int TdiExecute();
extern int TdiCompile();
extern int TdiData();
extern int TdiCvt();
extern int LibCallg();
extern int TreeFindNode();
extern int TreePutRecord();
extern int TreeWait();
extern int TdiDebug();
#endif

short ArgLen(struct descrip *d);

static int next = 0;
#define MIN(A,B)  ((A) < (B) ? (A) : (B))
#define MAXARGS 32

static int dtype_length(struct descriptor *d)
{
  short len;

  /*  This function needs to handle the DTYPE values in ipdesc.h as well 
   *  as the "native" DTYPEs, as it is called both for descriptors before
   *  evaluation, and for the answer descriptor, which for local access will
   *  be returned with native DTYPE.
   */

  switch (d->dtype)
  {
    case DTYPE_UCHAR     :
    case DTYPE_CHAR      :  len = sizeof(char); break;
    case DTYPE_USHORT    :
    case DTYPE_SHORT     :  len = sizeof(short); break;
    case DTYPE_ULONG     :  
    case DTYPE_LONG      :  len = sizeof(int); break;
    case DTYPE_ULONGLONG : 
    case DTYPE_LONGLONG  :  len = sizeof(int) * 2; break; 
#if DTYPE_NATIVE_FLOAT != DTYPE_FLOAT
    case DTYPE_NATIVE_FLOAT : 
#endif
    case DTYPE_FLOAT     :  len = sizeof(float); break;
#if DTYPE_NATIVE_DOUBLE != DTYPE_DOUBLE
    case DTYPE_NATIVE_DOUBLE :
#endif
    case DTYPE_DOUBLE    :  len = sizeof(double); break;
#if DTYPE_FLOAT_COMPLEX != DTYPE_COMPLEX
    case DTYPE_FLOAT_COMPLEX :
#endif
    case DTYPE_COMPLEX   :  len = sizeof(float) * 2; break;
#if DTYPE_DOUBLE_COMPLEX != DTYPE_COMPLEX_DOUBLE
    case DTYPE_DOUBLE_COMPLEX :
#endif
    case DTYPE_COMPLEX_DOUBLE: len = sizeof(double) * 2; break;
    case DTYPE_CSTRING :  len = d->length ? d->length : (d->pointer ? strlen(d->pointer) : 0); break;
  }
  return len;
}

#ifdef __VMS
char *DscToCstring(struct dsc$descriptor *dsc)
{
   char *ans = (dsc->dsc$b_dtype == DSC$K_DTYPE_T) ?
               strcpy((char *)malloc(strlen(dsc->dsc$a_pointer)+1),dsc->dsc$a_pointer) :
               strcpy((char *)malloc(strlen((char *)dsc)+1),(char *)dsc);
  int i;
  char *p;
  for (p = ans ? ans + strlen(ans) - 1 : 0; p && *p == ' ' && p >= ans; p--) *p = 0;
  return ans;
}
#endif

#if !defined(_VMS) && !defined(_CLIENT_ONLY)
int *cdescr (int dtype, void *data, ...)
{
    void *arglist[MAXARGS];
    va_list incrmtr;
    int dsc;
    static int status;
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
#ifndef __VMS
    arglist[argidx++] = MdsEND_ARG;
#endif
    *(int *)&arglist[0] = argidx-1; 
    status = LibCallg(arglist,descr);
    return(&status);
}
#endif

static struct descrip *MakeIpDescrip(struct descrip *arg, struct descriptor *dsc)
{

  char dtype;
  dtype = dsc->dtype;

  switch (dtype)
  {
  case DTYPE_NATIVE_FLOAT: dtype = DTYPE_FLOAT; break;
  case DTYPE_NATIVE_DOUBLE: dtype = DTYPE_DOUBLE; break;
  case DTYPE_FLOAT_COMPLEX: dtype = DTYPE_COMPLEX; break;
  case DTYPE_DOUBLE_COMPLEX: dtype = DTYPE_COMPLEX_DOUBLE; break;
  }

  if (dsc->class == CLASS_S) 
  {
    if (dsc->length)
      arg = MakeDescripWithLength(arg, (char)dtype, (int)dsc->length, (char)0, (int *)0, dsc->pointer);
    else
      arg = MakeDescrip(arg, (char)dtype, (char)0, (int *)0, dsc->pointer);
  } 
  else 
  {
    int i;
    array_coeff *adsc = (array_coeff *)dsc;  
    int dims[MAXDIM];
    int num = adsc->arsize/adsc->length;
    int *m = &num;
    if (adsc->dimct > 1) m = adsc->m;
    for (i=0; i<adsc->dimct; i++) dims[i] = m[i];
    for (i=adsc->dimct; i<MAXDIM; i++) dims[i] = 0;
    if (dsc->length)
      arg = MakeDescripWithLength(arg, (char)dtype, (int)dsc->length, adsc->dimct, dims, adsc->pointer);
    else
      arg = MakeDescrip(arg, (char)dtype, adsc->dimct, dims, adsc->pointer);
  }
  return arg;
}

static int MdsValueLength(struct descriptor *dsc)
{
  int length;
  switch (dsc->class)
  {
    case CLASS_S:
    case CLASS_D:
      length = dsc->length;
      break;
    case CLASS_A:
      length = ((struct descriptor_a *)dsc)->arsize;
      break;
    default:
      length = 0;
      break;
  }
  return(length);
}

static char *MdsValueRemoteExpression(char *expression, struct descriptor *dsc)
{

  /* This function will wrap expression in the appropriate type
   * conversion function to ensure that the return value of MdsValue
   * is of the right type.  It is only used for remote MDSplus.
   */

  char *native_float_str, *native_double_str, *native_complex_str, *native_double_complex_str;
  char *newexpression = (char *) malloc(strlen(expression)+24);

  /*  Determine the native floating/double/complex type of the client, so as to pass the correct
   *  conversion function to the server and thus save extra conversion steps.
   */

  switch (DTYPE_NATIVE_FLOAT)
  {
    case DTYPE_FS: 
      native_float_str = "FS_FLOAT"; 
      native_complex_str = "FS_COMPLEX";
      break;
    case DTYPE_F: 
      native_float_str = "F_FLOAT"; 
      native_complex_str = "F_COMPLEX";
      break;
    default: 
      printf("Unknown DTYPE_NATIVE_FLOAT: %d.  Using FS_FLOAT.\n",DTYPE_NATIVE_FLOAT);
      native_float_str = "FS_FLOAT";
      native_complex_str = "FS_COMPLEX";
      break;
  }
    
  switch (DTYPE_NATIVE_DOUBLE)
  {
    case DTYPE_FT: 
      native_double_str = "FT_FLOAT"; 
      native_double_complex_str = "FT_COMPLEX";
      break;
    case DTYPE_D: 
      native_double_str = "D_FLOAT"; 
      native_double_complex_str = "D_COMPLEX";
      break;
    case DTYPE_G:
      native_double_str = "G_FLOAT"; 
      native_double_complex_str = "G_COMPLEX";
      break;
    default: 
      printf("Unknown DTYPE_NATIVE_DOUBLE: %d.  Using FT_FLOAT.\n",DTYPE_NATIVE_DOUBLE);
      native_double_str = "FT_FLOAT";
      native_double_complex_str = "FT_COMPLEX";
      break;
  }

  /*  The DTYPE of the incoming descriptor will be one of the values in ipdesc.h.
   *  The switch clause below therefore does not need to support NATIVE_FLOAT etc.
   */

  switch (dsc->dtype) 
  {
    case DTYPE_UCHAR  : strcpy(newexpression,"BYTE_UNSIGNED"); break;
    case DTYPE_USHORT : strcpy(newexpression,"WORD_UNSIGNED"); break;
    case DTYPE_ULONG  : strcpy(newexpression,"LONG_UNSIGNED"); break;
    case DTYPE_ULONGLONG  : strcpy(newexpression,"QUADWORD_UNSIGNED"); break; 
    case DTYPE_CHAR   : strcpy(newexpression,"BYTE"); break;
    case DTYPE_SHORT  : strcpy(newexpression,"WORD"); break;
    case DTYPE_LONG   : strcpy(newexpression,"LONG"); break;
    case DTYPE_LONGLONG   : strcpy(newexpression,"QUADWORD"); break; 
    case DTYPE_FLOAT  : strcpy(newexpression,native_float_str); break;
    case DTYPE_DOUBLE : strcpy(newexpression,native_double_str); break;  
    case DTYPE_COMPLEX : strcpy(newexpression,native_complex_str); break;  
    case DTYPE_COMPLEX_DOUBLE: strcpy(newexpression,native_double_complex_str); break;  
    case DTYPE_CSTRING: strcpy(newexpression,"TEXT"); break;
  }

  strcat(newexpression, "((");
  strcat(newexpression, expression);
  strcat(newexpression, "))");
  return newexpression;

}

static void MdsValueMove(int source_length, char *source_array, char fill, int dest_length, char *dest_array)
{
  int i;
  memcpy(dest_array, source_array, MIN(source_length, dest_length));
  for (i=0; i<dest_length-source_length; i++)
  {
    dest_array[source_length+i] = fill;
  }
}

static void MdsValueCopy(int dim, int length, char fill, char *in, int *in_m, char *out, int *out_m)
{
  int i;
  if (dim == 1)
    MdsValueMove(length * in_m[0], in, fill, length * out_m[0], out);
  else
  {
    int in_increment = length;
    int out_increment = length;
    for (i=0; i<dim-1; i++)
    {
      in_increment *= in_m[i];
      out_increment *= out_m[i];
    }
    for (i=0; i<in_m[dim-1] && i < out_m[dim-1]; i++)
      MdsValueCopy(dim-1, length, fill, in + in_increment * i, in_m, out + out_increment * i, out_m);
  }
}

static void MdsValueSet(struct descriptor *outdsc, struct descriptor *indsc, int *length)
{
  char fill;
  if (indsc == 0)
  {
    if (length) *length = 0;
    return;
  }
  fill = (outdsc->dtype == DTYPE_CSTRING) ? 32 : 0;
  if ( (indsc->class == CLASS_A) &&
       (outdsc->class == CLASS_A) &&
       (((struct descriptor_a *)outdsc)->dimct > 1) &&
       (((struct descriptor_a *)outdsc)->dimct == ((struct descriptor_a *)indsc)->dimct) )
  {
    array_coeff *in_a = (array_coeff *) indsc;
    array_coeff *out_a = (array_coeff *) outdsc;
    MdsValueMove(0,0,fill,MdsValueLength(outdsc),out_a->pointer);
    MdsValueCopy(out_a->dimct, in_a->length, fill, in_a->pointer, in_a->m, out_a->pointer, out_a->m);
  }
  else
  {
    MdsValueMove(MdsValueLength(indsc), indsc->pointer, fill, MdsValueLength(outdsc), outdsc->pointer);
  }

  if (length) 
  {
    if (indsc->class == CLASS_A) 
      *length = MIN( ((struct descriptor_a *)outdsc)->arsize / outdsc->length, 
		     ((struct descriptor_a *)indsc)->arsize / indsc->length);
    else 
    {
      if (indsc->dtype == DTYPE_CSTRING)
      {
	*length = MIN(outdsc->length, indsc->length);
      }
      else
      {
	*length = MIN(outdsc->length / dtype_length(outdsc), 
		      indsc->length / dtype_length(indsc));
      }
    }
  }
}
#else /* FORTRAN_ENTRY_POINTS */ 
#ifdef __VMS
int  MdsOpen(struct dsc$descriptor *treedsc, int *shot)
{
   char *tree = DscToCstring(treedsc);
#else
int  MdsOpen(char *tree, int *shot)
{
#endif
  return ___MdsOpen(tree,shot);
}

static int  ___MdsOpen(char *tree, int *shot)
{
  if (mdsSocket != INVALID_SOCKET)
  {

    long answer;
    int status;
    int length;
    int d1,d2,d3; /* descriptor numbers passed to MdsValue */
    int dtype_cstring = DTYPE_CSTRING;
    int dtype_long = DTYPE_LONG;
    int null = 0;

#ifdef __VMS
    static $DESCRIPTOR(expressiondsc,"TreeOpen($,$)\0");
    static struct dsc$descriptor *expression = (struct dsc$descriptor *)&expressiondsc;
#else
    static char *expression = "TreeOpen($,$)";
#endif

    length = strlen(tree);
    d1 = descr(&dtype_cstring,tree,&null,&length);
    d2 = descr(&dtype_long,shot, &null);
    d3 = descr(&dtype_long,&answer,&null);

    status = MdsValue(expression, &d1, &d2, &d3, &null, &length);
    if ((status & 1))
    {
      return *(int *)&answer; 
    } else 
      return 0;  
  }
  else 
#ifdef _CLIENT_ONLY
    printf("Must ConnectToMds first\n");
#else
  {

#ifdef __VMS
    struct descriptor treeexp = {0,DTYPE_T,CLASS_S,0};
    int status;
    treeexp.length = strlen((char *)tree);
    treeexp.pointer = (char *)tree;
    status = MDS$OPEN(&treeexp, shot);
    free(tree);
    return status;
#else
    return TreeOpen(tree, *shot, 0);
#endif

  }
#endif
  return 1;
}

static int ___MdsSetSocket(int *newsocket)
{
  int oldsocket = mdsSocket;
  mdsSocket = *newsocket;
  return oldsocket;
}
int MdsSetSocket(int *newsocket) { return ___MdsSetSocket(newsocket);}

#ifdef __VMS
int  MdsClose(struct dsc$descriptor *treedsc, int *shot)
{
  char *tree = DscToCstring(treedsc);
#else
int  MdsClose(char *tree, int *shot)
{
#endif
  return ___MdsClose(tree,shot);
}

static int  ___MdsClose(char *tree, int *shot)
{
  if (mdsSocket != INVALID_SOCKET)
  {


    long answer;
    int length;
    int status;
    int d1,d2,d3; /* descriptor numbers passed to MdsValue */
    int dtype_cstring = DTYPE_CSTRING;
    int dtype_long = DTYPE_LONG;
    int null = 0;

#ifdef __VMS
    static $DESCRIPTOR(expressiondsc,"TreeClose($,$)\0");
    static struct dsc$descriptor *expression = (struct dsc$descriptor *)&expressiondsc;
#else
    static char *expression = "TreeClose($,$)";
#endif

    length = strlen(tree);
    d1 = descr(&dtype_cstring,tree,&null,&length);
    d2 = descr(&dtype_long,shot, &null);
    d3 = descr(&dtype_long,&answer,&null);

    status = MdsValue(expression, &d1, &d2, &d3, &null, &length);
    
#ifdef __VMS
    free(tree);
#endif
    if ((status & 1))
    {
      return *(int *)&answer; 
    } else 
      return 0;  
  }
  else 
#ifdef _CLIENT_ONLY
    printf("Must ConnectToMds first\n");
#else
  {
#ifdef __VMS
    struct descriptor treeexp = {0,DTYPE_T,CLASS_S,0};
    int status;
    treeexp.length = strlen((char *)tree);
    treeexp.pointer = (char *)tree;
    status =  MDS$CLOSE(&treeexp, shot);
    free(tree);
    return status;
#else
    return TreeClose(tree, *shot);
#endif
  }
#endif
  return 1;
}

#ifdef __VMS
int  MdsSetDefault(struct dsc$descriptor *nodedsc)
{
   char *node = DscToCstring(nodedsc);
#else
int  MdsSetDefault(char *node)
{
#endif
  return ___MdsSetDefault(node);
}

static int  ___MdsSetDefault(char *node)
{

  if (mdsSocket != INVALID_SOCKET) {
    char *expression = strcpy((char *)malloc(strlen(node)+20),"TreeSetDefault('");
#ifdef __VMS
    static struct dsc$descriptor expressiondsc = {0,DTYPE_T,CLASS_S,0};
#endif
    long answer;
    int length = strlen(node);
    int null = 0;
    int dtype_long = DTYPE_LONG;
    int d1 = descr(&dtype_long, &answer, &null);
    int status;
    if (node[0] == '\\') strcat(expression,"\\");
    strcat(expression,node);
    strcat(expression,"')");
#ifdef __VMS
    expressiondsc.dsc$w_length = strlen(expression)+1;
    expressiondsc.dsc$a_pointer = expression;
    free(node);
    status = MdsValue(&expressiondsc, &d1, &null, &length);
#else
    status = MdsValue(expression, &d1, &null, &length);
#endif
    free(expression);
    if ((status & 1))
    {
      return *(int *)&answer;
    }  else
      return 0;
  }

  else 
#ifdef _CLIENT_ONLY
    printf("Must ConnectToMds first\n");
#else

  {

#ifdef __VMS
    struct descriptor defaultexp = {0,DTYPE_T,CLASS_S,0};
    int status;
    defaultexp.length = strlen((char *)node);
    defaultexp.pointer = (char *)node;
    status = MDS$SET_DEFAULT(&defaultexp);
    free(node);
    return status;
#else
    int nid;
    return TreeSetDefault(node,&nid);
#endif
  }
#endif
  return 1;
}

void MdsDisconnect(){___MdsDisconnect();}
static void ___MdsDisconnect()
{
  DisconnectFromMds(mdsSocket);
  mdsSocket = INVALID_SOCKET;      /*** SETS GLOBAL VARIABLE mdsSOCKET ***/
}

#ifdef __VMS
SOCKET MdsConnect(struct dsc$descriptor *hostdsc)
{
  char *host = DscToCstring(hostdsc);
#else
SOCKET MdsConnect(char *host)
{
#endif
  return ___MdsConnect(host);
}

static SOCKET ___MdsConnect(char *host)
{
  if (mdsSocket != INVALID_SOCKET)
    {
      MdsDisconnect(); 
    }
  mdsSocket = ConnectToMds(host);  /*** SETS GLOBAL VARIABLE mdsSOCKET ***/
#ifdef __VMS
  free(host);
#endif
  return(mdsSocket);
}

/* end FORTRAN_ENTRY_POINTS */
#endif

#if !defined(FORTRAN_ENTRY_POINTS) || defined(descr)
int descr (int *dtype, void *data, int *dim1, ...)
{

  /* variable length argument list: 
   * (# elements in dim 1), (# elements in dim 2) ... 0, [length of (each) string if DTYPE_CSTRING]
   */

     
  struct descriptor *dsc; 
  int totsize = *dim1;
  int retval;
  
  if (data == NULL)
  {
    printf("NULL pointer passed as data pointer\n");
    return -1;
  }

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

  if ((dsc->dtype == DTYPE_CSTRING) && (((struct descriptor *)data)->dtype == DTYPE_CSTRING))
    dsc->pointer = ((struct descriptor *)data)->pointer;
  else
    dsc->pointer = (char *)data;
  dsc->length = dtype_length(dsc); /* must set length after dtype and data pointers are set */

  /*  Convert DTYPE for native access if required.  Do AFTER the call to dtype_length() to
   *  save having to support DTYPE_NATIVE_FLOAT etc. in dtype_length().
   */
  
  if (mdsSocket == INVALID_SOCKET)
  {
    switch (dsc->dtype)
    {
      case DTYPE_FLOAT :  
        dsc->dtype = DTYPE_NATIVE_FLOAT;
        break;
      case DTYPE_DOUBLE :  
        dsc->dtype = DTYPE_NATIVE_DOUBLE;
        break;
      case DTYPE_COMPLEX : 
        dsc->dtype = DTYPE_FLOAT_COMPLEX;
        break;
      case DTYPE_COMPLEX_DOUBLE : 
        dsc->dtype = DTYPE_DOUBLE_COMPLEX;
        break;
      default : 
        break;
    }
  } 


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

    if (dsc->dtype == DTYPE_CSTRING) /*  && dsc->length == 0)  */
    {
	 dsc->length = *va_arg(incrmtr, int *);
    }


    if (ndim > 1) 
    {
      int i;
      array_coeff *adsc = (array_coeff *)dsc;  
      adsc->class = CLASS_A;
  
      if (ndim > MAXDIM) 
      {
	ndim = MAXDIM;
	printf("(descr.c) WARNING: requested ndim>MAXDIM, forcing to MAXDIM\n");
      }
      adsc->dimct = ndim;
      adsc->scale = 0;
      adsc->digits = 0;
      adsc->aflags.binscale = 0;
      adsc->aflags.redim = 1;
      adsc->aflags.column = 1;
      adsc->aflags.coeff = 1;
      adsc->aflags.bounds = 0;
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
      adsc->scale = 0;
      adsc->digits = 0;
      adsc->aflags.binscale = 0;
      adsc->aflags.redim = 1;
      adsc->aflags.column = 1;
      adsc->aflags.coeff = 0;
      adsc->aflags.bounds = 0;
      if (ndim < 1) printf("(descr.c) WARNING: requested ndim<1, forcing to 1.\n");
    }
      
  }

  retval = next+1;
  next++;
  if (next >= NDESCRIP_CACHE) next = 0;
  return retval;
}
int descr2 (int *dtype, int *dim1, ...)
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

  dsc->pointer = 0;
  dsc->length = dtype_length(dsc); /* must set length after dtype and data pointers are set */

  /*  Convert DTYPE for native access if required.  Do AFTER the call to dtype_length() to
   *  save having to support DTYPE_NATIVE_FLOAT etc. in dtype_length().
   */
  
  if (mdsSocket == INVALID_SOCKET)
  {
    switch (dsc->dtype)
    {
      case DTYPE_FLOAT :  
        dsc->dtype = DTYPE_NATIVE_FLOAT;
        break;
      case DTYPE_DOUBLE :  
        dsc->dtype = DTYPE_NATIVE_DOUBLE;
        break;
      case DTYPE_COMPLEX : 
        dsc->dtype = DTYPE_FLOAT_COMPLEX;
        break;
      case DTYPE_COMPLEX_DOUBLE : 
        dsc->dtype = DTYPE_DOUBLE_COMPLEX;
        break;
      default : 
        break;
    }
  } 


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

    if (dsc->dtype == DTYPE_CSTRING) /*  && dsc->length == 0)  */
    {
	 dsc->length = *va_arg(incrmtr, int *);
    }


    if (ndim > 1) 
    {
      int i;
      array_coeff *adsc = (array_coeff *)dsc;  
      adsc->class = CLASS_A;
  
      if (ndim > MAXDIM) 
      {
	ndim = MAXDIM;
	printf("(descr.c) WARNING: requested ndim>MAXDIM, forcing to MAXDIM\n");
      }
      adsc->dimct = ndim;

      adsc->aflags.binscale = 0;
      adsc->aflags.redim = 1;
      adsc->aflags.column = 1;
      adsc->aflags.coeff = 1;
      adsc->aflags.bounds = 0;
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
      adsc->aflags.binscale = 0;
      adsc->aflags.redim = 1;
      adsc->aflags.column = 1;
      adsc->aflags.coeff = 0;
      adsc->aflags.bounds = 0;
      if (ndim < 1) printf("(descr.c) WARNING: requested ndim<1, forcing to 1.\n");
    }
      
  }

  retval = next+1;
  next++;
  if (next >= NDESCRIP_CACHE) next = 0;
  return retval;
}
#endif

#if !defined(FORTRAN_ENTRY_POINTS) || defined(MdsValue)

#ifdef __VMS
int MdsValue(struct dsc$descriptor *expressiondsc, ...) 
{
#else
int MdsValue(char *expression, ...) 
{
#endif
  va_list incrmtr;
  int a_count;
  int i;
  unsigned char nargs;
  struct descriptor *dsc;
  int *length;
  static int clear=4;
  int status = 1;
  int *descnum = &status;  /* initialize to point at non zero value */


#ifdef __VMS
  char *expression = DscToCstring(expressiondsc);
  va_start(incrmtr, expressiondsc);
#else
  va_start(incrmtr, expression);
#endif
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

  if (mdsSocket != INVALID_SOCKET)   /* CLIENT/SERVER */
  {
    struct descriptor *dscAnswer;
    struct descrip exparg;
    struct descrip *arg = &exparg;
    char *newexpression;

#ifdef __VMS
    va_start(incrmtr, expressiondsc);
#else
    va_start(incrmtr, expression);
#endif
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
#ifdef __VMS
    free(expression);
#endif
    arg = MakeDescrip(&exparg,DTYPE_CSTRING,0,0,newexpression);
    status = SendArg(mdsSocket, (char)0, arg->dtype, (unsigned char)(nargs+1), ArgLen(arg), arg->ndims, arg->dims, (void *)arg->ptr);
    free(newexpression); 

    /* send each argument */

#ifdef __VMS
    va_start(incrmtr, expressiondsc);
#else
    va_start(incrmtr, expression);
#endif
    for (i=1;i<=nargs && (status & 1);i++)
    {
      descnum = va_arg(incrmtr, int *);
      if (*descnum > 0)
      {
        dsc = descrs[*descnum-1];
        arg = MakeIpDescrip(arg, dsc);
	      status = SendArg(mdsSocket, (unsigned char)i, arg->dtype, (char)(nargs+1), ArgLen(arg), arg->ndims, arg->dims, arg->ptr);
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
      char *dnew = 0;
      struct descrip exparg;
      struct descrip *arg = &exparg;

      arg = MakeIpDescrip(arg, dscAnswer);
      status = GetAnswerInfo(mdsSocket, &arg->dtype, &len, &arg->ndims, arg->dims, &numbytes, &dptr);

      /**  Make a "regular" descriptor out of the returned IpDescrip.
       **  Cannot use LibCallg to call descr() because it will not be
       **  present for client-only library.
       **/

      if (status & 1) 
      {
	int ansdescr;
	int dims[MAX_DIMS];
	int null = 0;
	int dtype = arg->dtype;
	int dlen = len;

	for (i=0; i<arg->ndims; i++) dims[i] = (int)arg->dims[i];

	if (arg->dtype == DTYPE_CSTRING && arg->ndims > 0 && dlen != dscAnswer->length)
	{
	  /** rewrite string array so that it gets copied to answer descriptor dscAnswer correctly **/
	  int i;
	  int nelements = numbytes/len;
	  int s = nelements * dscAnswer->length;
	  dnew = (char *) malloc(s);
	  for (i=0; i<s; i++) dnew[i]=32;  /*fill*/
	  for (i=0; i<nelements; i++) memcpy(dnew+(i*dscAnswer->length), (char *) dptr+(i*len), MIN(dscAnswer->length,len));
	  dptr = dnew;
	  dlen = dscAnswer->length;
	}

	switch (arg->ndims)
	{
	  case 0: ansdescr = descr(&dtype, dptr, &null, &dlen); break;
	  case 1: ansdescr = descr(&dtype, dptr, &dims[0], &null, &dlen); break;
	  case 2: ansdescr = descr(&dtype, dptr, &dims[0], &dims[1], &null, &dlen); break;
	  case 3: ansdescr = descr(&dtype, dptr, &dims[0], &dims[1], &dims[2], &null, &dlen); break;
	  case 4: ansdescr = descr(&dtype, dptr, &dims[0], &dims[1], &dims[2], &dims[3], &null, &dlen); break;
	  case 5: ansdescr = descr(&dtype, dptr, &dims[0], &dims[1], &dims[2], &dims[3], &dims[4], &null, &dlen); break;
	  case 6: ansdescr = descr(&dtype, dptr, &dims[0], &dims[1], &dims[2], &dims[3], &dims[4], &dims[5], &null, &dlen); break;
	  case 7: ansdescr = descr(&dtype, dptr, &dims[0], &dims[1], &dims[2], &dims[3], &dims[4], &dims[5], &dims[6], &null, &dlen); break;
	}

	MdsValueSet(dscAnswer, descrs[ansdescr-1], length);
      }
      if (dnew)
        free(dnew);
    }

  }
  else 
#ifdef _CLIENT_ONLY
    printf("Must ConnectToMds first\n");
#else
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
#ifdef __VMS
    va_start(incrmtr, expressiondsc);
#else
    va_start(incrmtr, expression);
#endif
    for (i=1;i < a_count; i++)
    {
      descnum = va_arg(incrmtr, int *);
      dsc = descrs[*descnum-1];
      arglist[argidx++] = (void *)dsc;
    }
    arglist[argidx++] = (void *)&xd1;
#ifndef __VMS
    arglist[argidx++] = MdsEND_ARG;
#endif
    *(int *)&arglist[0] = argidx-1; 
    status = LibCallg(arglist,TdiExecute);

    if (status & 1)
    {
	 
      descnum = va_arg(incrmtr, int *);
      dsc = descrs[*descnum-1];

      status = TdiData(xd1.pointer,&xd2 MDS_END_ARG);

      if (status & 1 && xd2.pointer != 0 && xd2.pointer->pointer != 0) 
	{      
	  int templen = (xd2.pointer)->length;
	  status = TdiCvt(&xd2,dsc,&xd3 MDS_END_ARG);
	  /**  get string length right if scalar string (if answer descriptor has longer
	   **  length than returned value, then make sure the length is the length of the 
           **  returned value
           **/
	  if ((xd3.pointer)->dtype == DTYPE_CSTRING && (xd3.pointer->class != CLASS_A)) 
	    (xd3.pointer)->length = MIN(templen, (xd3.pointer)->length);
	}
      
      if (status & 1) 
      {
	MdsValueSet(dsc, xd3.pointer, length);

	MdsFree1Dx(&xd1, NULL);
	MdsFree1Dx(&xd2, NULL);
	MdsFree1Dx(&xd3, NULL);  /* is answerptr still valid after calling this??? */
      }
    }
  }
  TdiDebug(&clear,0 MDS_END_ARG);
#endif
  return(status);

}
#ifdef __VMS
int MdsValue2(struct dsc$descriptor *expressiondsc, ...) 
{
#else
int MdsValue2(char *expression, ...) 
{
#endif
  va_list incrmtr;
  int a_count;
  int i;
  unsigned char nargs;
  struct descriptor *dsc;
  int *length;
  int status = 1;
  void *ptr = 0;
  int *descnum = &status;  /* initialize to point at non zero value */


#ifdef __VMS
  char *expression = DscToCstring(expressiondsc);
  va_start(incrmtr, expressiondsc);
#else
  va_start(incrmtr, expression);
#endif
  for (a_count = 0; *descnum != 0; a_count++)
  {
    descnum = va_arg(incrmtr, int *);
    if (*descnum != 0)
      ptr = va_arg(incrmtr, void *);
  }
  a_count--; /* subtract one for terminator of argument list */

  length = va_arg(incrmtr, int *);
  if (length) 
    {
      *length = 0;
    }

  if (mdsSocket != INVALID_SOCKET)   /* CLIENT/SERVER */
  {
    struct descriptor *dscAnswer;
    struct descrip exparg;
    struct descrip *arg = &exparg;
    char *newexpression;

#ifdef __VMS
    va_start(incrmtr, expressiondsc);
#else
    va_start(incrmtr, expression);
#endif
    nargs = a_count - 1 ;  /* -1 for answer argument */

    /* Get last argument - it is the answer descriptor number */
    for (i=1;i<=(nargs*2)+1; i++) descnum = va_arg(incrmtr, int *);
    dscAnswer = descrs[*descnum - 1];
    dscAnswer->pointer = va_arg(incrmtr, void *);

    /* 
     * Send expression descriptor first.
     * MdsValueRemoteExpression wraps expression with type conversion function.
     * It malloc's space for newexpression that needs to be freed after
     */
    newexpression = MdsValueRemoteExpression(expression,dscAnswer);  
#ifdef __VMS
    free(expression);
#endif
    arg = MakeDescrip(&exparg,DTYPE_CSTRING,0,0,newexpression);
    status = SendArg(mdsSocket, (char)0, arg->dtype, (unsigned char)(nargs+1), ArgLen(arg), arg->ndims, arg->dims, (void *)arg->ptr);
    free(newexpression); 

    /* send each argument */

#ifdef __VMS
    va_start(incrmtr, expressiondsc);
#else
    va_start(incrmtr, expression);
#endif
    for (i=1;i<=nargs && (status & 1);i++)
    {
      descnum = va_arg(incrmtr, int *);
      if (*descnum > 0)
      {
        dsc = descrs[*descnum-1];
        dsc->pointer = va_arg(incrmtr, void *);
        arg = MakeIpDescrip(arg, dsc);
	      status = SendArg(mdsSocket, (unsigned char)i, arg->dtype, (char)(nargs+1), ArgLen(arg), arg->ndims, arg->dims, arg->ptr);
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
      char *dnew = 0;
      struct descrip exparg;
      struct descrip *arg = &exparg;

      arg = MakeIpDescrip(arg, dscAnswer);
      status = GetAnswerInfo(mdsSocket, &arg->dtype, &len, &arg->ndims, arg->dims, &numbytes, &dptr);

      /**  Make a "regular" descriptor out of the returned IpDescrip.
       **  Cannot use LibCallg to call descr() because it will not be
       **  present for client-only library.
       **/

      if (status & 1) 
      {
	int ansdescr;
	int dims[MAX_DIMS];
	int null = 0;
	int dtype = arg->dtype;
	int dlen = len;

	for (i=0; i<arg->ndims; i++) dims[i] = (int)arg->dims[i];

	if (arg->dtype == DTYPE_CSTRING && arg->ndims > 0 && dlen != dscAnswer->length)
	{
	  /** rewrite string array so that it gets copied to answer descriptor dscAnswer correctly **/
	  int i;
	  int nelements = numbytes/len;
	  int s = nelements * dscAnswer->length;
	  dnew = (char *) malloc(s);
	  for (i=0; i<s; i++) dnew[i]=32;  /*fill*/
	  for (i=0; i<nelements; i++) memcpy(dnew+(i*dscAnswer->length), (char *) dptr+(i*len), MIN(dscAnswer->length,len));
	  dptr = dnew;
	  dlen = dscAnswer->length;
	}

	switch (arg->ndims)
	{
	  case 0: ansdescr = descr(&dtype, dptr, &null, &dlen); break;
	  case 1: ansdescr = descr(&dtype, dptr, &dims[0], &null, &dlen); break;
	  case 2: ansdescr = descr(&dtype, dptr, &dims[0], &dims[1], &null, &dlen); break;
	  case 3: ansdescr = descr(&dtype, dptr, &dims[0], &dims[1], &dims[2], &null, &dlen); break;
	  case 4: ansdescr = descr(&dtype, dptr, &dims[0], &dims[1], &dims[2], &dims[3], &null, &dlen); break;
	  case 5: ansdescr = descr(&dtype, dptr, &dims[0], &dims[1], &dims[2], &dims[3], &dims[4], &null, &dlen); break;
	  case 6: ansdescr = descr(&dtype, dptr, &dims[0], &dims[1], &dims[2], &dims[3], &dims[4], &dims[5], &null, &dlen); break;
	  case 7: ansdescr = descr(&dtype, dptr, &dims[0], &dims[1], &dims[2], &dims[3], &dims[4], &dims[5], &dims[6], &null, &dlen); break;
	}

	MdsValueSet(dscAnswer, descrs[ansdescr-1], length);
      }
      if (dnew)
        free(dnew);
    }

  }
  else 
#ifdef _CLIENT_ONLY
    printf("Must ConnectToMds first\n");
#else
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
#ifdef __VMS
    va_start(incrmtr, expressiondsc);
#else
    va_start(incrmtr, expression);
#endif
    for (i=1;i < a_count; i++)
    {
      descnum = va_arg(incrmtr, int *);
      dsc = descrs[*descnum-1];
      dsc->pointer = va_arg(incrmtr, void *);
      arglist[argidx++] = (void *)dsc;
    }
    arglist[argidx++] = (void *)&xd1;
#ifndef __VMS
    arglist[argidx++] = MdsEND_ARG;
#endif
    *(int *)&arglist[0] = argidx-1; 
    status = LibCallg(arglist,TdiExecute);

    if (status & 1)
    {
	 
      descnum = va_arg(incrmtr, int *);
      dsc = descrs[*descnum-1];
      dsc->pointer = va_arg(incrmtr, void *);

      status = TdiData(xd1.pointer,&xd2 MDS_END_ARG);

      if (status & 1 && xd2.pointer) 
	{      
	  int templen = (xd2.pointer)->length;
	  status = TdiCvt(&xd2,dsc,&xd3 MDS_END_ARG);
	  /**  get string length right if scalar string (if answer descriptor has longer
	   **  length than returned value, then make sure the length is the length of the 
           **  returned value
           **/
	  if ((xd3.pointer)->dtype == DTYPE_CSTRING && (xd3.pointer->class != CLASS_A)) 
	    (xd3.pointer)->length = MIN(templen, (xd3.pointer)->length);
	}
      
      if (status & 1) 
      {
	MdsValueSet(dsc, xd3.pointer, length);

	MdsFree1Dx(&xd1, NULL);
	MdsFree1Dx(&xd2, NULL);
	MdsFree1Dx(&xd3, NULL);  /* is answerptr still valid after calling this??? */
      }
    }
  }
#endif
  return(status);

}
#endif

#if !defined(FORTRAN_ENTRY_POINTS) || defined(MdsPut)
#ifdef __VMS
int  MdsPut(struct dsc$descriptor *pathnamedsc, struct dsc$descriptor *expressiondsc, ...)
{
#else
int  MdsPut(char *pathname, char *expression, ...)
{
#endif
  va_list incrmtr;
  int a_count;
  int i;
  unsigned char nargs;

  struct descriptor *dsc;
  int status = 1;
  int *descnum = &status;  /* initialize to point at non zero value */

#ifdef __VMS
  char *pathname = DscToCstring(pathnamedsc);
  char *expression = DscToCstring(expressiondsc);
  va_start(incrmtr, expressiondsc);
#else
  va_start(incrmtr, expression);
#endif
  for (a_count = 0; *descnum != 0; a_count++)
  {
    descnum = va_arg(incrmtr, int *);
  }
  a_count--; /* subtract one for terminator of argument list */


  if (mdsSocket != INVALID_SOCKET)   /* CLIENT/SERVER */
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
  
#ifdef __VMS
    va_start(incrmtr, expressiondsc);
#else
    va_start(incrmtr, expression);
#endif

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
#ifdef _CLIENT_ONLY
    printf("Must ConnectToMds first\n");
#else
  {

    void *arglist[MAXARGS];
    struct descriptor *dsc;
    struct descriptor dexpression = {0,DTYPE_T,CLASS_S,0};
    EMPTYXD(tmp);
    int argidx = 1;
    int i;
    int nid;

#ifdef __VMS
    struct descriptor pdsc = {strlen(pathname),DTYPE_T, CLASS_S, pathname};
    if ((status = TreeFindNode(&pdsc, &nid)) & 1)
    {
#else
    if ((status = TreeFindNode(pathname, &nid)) & 1)
    {
#endif
      dexpression.length = strlen((char *)expression);
      dexpression.pointer = (char *)expression;
      arglist[argidx++] = (void *)&dexpression;
#ifdef __VMS
      va_start(incrmtr, expressiondsc);
#else
      va_start(incrmtr, expression);
#endif
      for (i=1;i <= a_count; i++)
      {
        descnum = va_arg(incrmtr, int *);
        dsc = descrs[*descnum-1];
        arglist[argidx++] = (void *)dsc;
      }
      arglist[argidx++] = (void *)&tmp;
#ifndef __VMS
      arglist[argidx++] = MdsEND_ARG;
#endif
      *(int *)&arglist[0] = argidx-1; 

      status = LibCallg(arglist,TdiCompile);

      if (status & 1)
      {
#ifdef __VMS
	if ((status = TreePutRecord(&nid, (struct descriptor *)arglist[argidx-1],0)) & 1)
	{ 
#else
	if ((status = TreePutRecord(nid, (struct descriptor *)arglist[argidx-2],0)) & 1)
	{ 
#endif
	  TreeWait();
	}
      }
      MdsFree1Dx(&tmp, NULL);
    }

  }
#endif
#ifdef __VMS
  free(pathname);
  free(expression);
#endif
  return(status);
}
#ifdef __VMS
int  MdsPut2(struct dsc$descriptor *pathnamedsc, struct dsc$descriptor *expressiondsc, ...)
{
#else
int  MdsPut2(char *pathname, char *expression, ...)
{
#endif
  va_list incrmtr;
  int a_count;
  int i;
  unsigned char nargs;
  void *ptr;

  struct descriptor *dsc;
  int status = 1;
  int *descnum = &status;  /* initialize to point at non zero value */

#ifdef __VMS
  char *pathname = DscToCstring(pathnamedsc);
  char *expression = DscToCstring(expressiondsc);
  va_start(incrmtr, expressiondsc);
#else
  va_start(incrmtr, expression);
#endif
  for (a_count = 0; *descnum != 0; a_count++)
  {
    descnum = va_arg(incrmtr, int *);
    ptr = va_arg(incrmtr, void *);
  }
  a_count--; /* subtract one for terminator of argument list */


  if (mdsSocket != INVALID_SOCKET)   /* CLIENT/SERVER */
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
  
#ifdef __VMS
    va_start(incrmtr, expressiondsc);
#else
    va_start(incrmtr, expression);
#endif

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
        dsc->pointer = va_arg(incrmtr, void *);
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
#ifdef _CLIENT_ONLY
    printf("Must ConnectToMds first\n");
#else
  {

    void *arglist[MAXARGS];
    struct descriptor *dsc;
    struct descriptor dexpression = {0,DTYPE_T,CLASS_S,0};
    EMPTYXD(tmp);
    int argidx = 1;
    int i;
    int nid;

#ifdef __VMS
    struct descriptor pdsc = {strlen(pathname),DTYPE_T, CLASS_S, pathname};
    if ((status = TreeFindNode(&pdsc, &nid)) & 1)
    {
#else
    if ((status = TreeFindNode(pathname, &nid)) & 1)
    {
#endif
      dexpression.length = strlen((char *)expression);
      dexpression.pointer = (char *)expression;
      arglist[argidx++] = (void *)&dexpression;
#ifdef __VMS
      va_start(incrmtr, expressiondsc);
#else
      va_start(incrmtr, expression);
#endif
      for (i=1;i <= a_count; i++)
      {
        descnum = va_arg(incrmtr, int *);
        dsc = descrs[*descnum-1];
        dsc->pointer = va_arg(incrmtr, void *);
        arglist[argidx++] = (void *)dsc;
      }
      arglist[argidx++] = (void *)&tmp;
#ifndef __VMS
      arglist[argidx++] = MdsEND_ARG;
#endif
      *(int *)&arglist[0] = argidx-1; 

      status = LibCallg(arglist,TdiCompile);

      if (status & 1)
      {
#ifdef __VMS
	if ((status = TreePutRecord(&nid, (struct descriptor *)arglist[argidx-1]),0) & 1)
	{ 
#else
	if ((status = TreePutRecord(nid, (struct descriptor *)arglist[argidx-2]),0) & 1)
	{ 
#endif
	  TreeWait();
	}
      }
      MdsFree1Dx(&tmp, NULL);
    }

  }
#endif
#ifdef __VMS
  free(pathname);
  free(expression);
#endif
  return(status);
}
#endif 


#ifndef FORTRAN_ENTRY_POINTS
#define FORTRAN_ENTRY_POINTS

#ifdef HAVE_WINDOWS_H
static int zero = 0;
static SOCKET ___MdsConnect();
static void ___MdsDisconnect();
static int ___MdsClose();
static int ___MdsSetSocket();
static int ___MdsSetDefault();
static int ___MdsOpen();
SOCKET WINAPI MdsConnectVB(char *host) { return ___MdsConnect(host);}
void WINAPI MdsDisconnectVB() { ___MdsDisconnect();}
int  WINAPI MdsCloseVB(char *tree, int *shot) { return ___MdsClose(tree,shot);}
int  WINAPI MdsSetSocketVB(int *newsocket) { return ___MdsSetSocket(newsocket);}
int  WINAPI MdsSetDefaultVB(char *node) { return ___MdsSetDefault(node);}
int  WINAPI MdsOpenVB(char *tree, int *shot) { return ___MdsOpen(tree,shot);}
int WINAPI descr1VB(int *dtype, void *value)
{ if (*dtype == DTYPE_CSTRING)
  {
    int len = strlen(value);
    return descr(dtype,value,&zero,&len);
  } else
    return descr(dtype,value,&zero);
}
int WINAPI descr1VB2(int *dtype,int *len) {return (descr2(dtype,&zero,len));}
int WINAPI descr2VB(int *dtype, int *num, void *value)
{ if (*dtype == DTYPE_CSTRING)
  {
    int len = strlen(value);
    return descr(dtype,value,num,&zero,&len);
  } else
    return descr(dtype,value,num,&zero);
}
int WINAPI descr2VB2(int *dtype,int *num,int *len) {return (descr2(dtype,num,&zero,len));}
int WINAPI descr3VB(int *dtype, int *n1, int *n2, void *value)
{ if (*dtype == DTYPE_CSTRING)
  {
    int len = strlen(value);
    return descr(dtype,value,n1,n2,&zero,&len);
  } else
    return descr(dtype,value,n1,n2,&zero);
}
int WINAPI descr3VB2(int *dtype, int *n1, int *n2, int *len) {return (descr2(dtype,n1,n2,&zero,len));}
int WINAPI descr4VB(int *dtype, int *n1, int *n2, int *n3, void *value)
{ if (*dtype == DTYPE_CSTRING)
  {
    int len = strlen(value);
    return descr(dtype,value,n1,n2,n3,&zero,&len);
  } else
    return descr(dtype,value,n1,n2,n3,&zero);
}
int WINAPI descr4VB2(int *dtype, int *n1, int *n2, int *n3,int *len) {return (descr2(dtype,n1,n2,n3,&zero,len));}
int WINAPI MdsValue1VB(char *expression, int *ansd, int *retlen)
 { return MdsValue(expression,ansd,&zero,retlen);}
int WINAPI MdsValue1VB2(char *expression, int *ansd, void *value, int *retlen)
 { return MdsValue2(expression,ansd,value,&zero,retlen);}

int WINAPI MdsValue2VB(char *expression, int *arg1d, int *ansd, int *retlen)
 { return MdsValue(expression,arg1d,ansd,&zero,retlen);}
int WINAPI MdsValue2VB2(char *expression, int *arg1d, void *arg1v, int *ansd, void *value, int *retlen)
 { return MdsValue2(expression,arg1d,arg1v,ansd,value,&zero,retlen);}
int WINAPI MdsValue3VB(char *expression, int *arg1d, int *arg2d, int *ansd, int *retlen)
 { return MdsValue(expression,arg1d,arg2d,ansd,&zero,retlen);}
int WINAPI MdsValue3VB2(char *expression, int *arg1d, void *arg1v, int *arg2d, void *arg2v, int *ansd, void *value, int *retlen)
 { return MdsValue2(expression,arg1d,arg1v,arg2d,arg2v,ansd,value,&zero,retlen);}
int WINAPI MdsPut1VB(char *node, char *expression, int *ansd)
 { return MdsPut(node,expression,ansd,&zero);}
int WINAPI MdsPut1VB2(char *node, char *expression, int *ansd, void *value)
 { return MdsPut2(node,expression,ansd,value,&zero);}
int WINAPI MdsPut2VB(char *node, char *expression, int *arg1d, int *ansd)
 { return MdsPut(node,expression,arg1d,ansd,&zero);}
int WINAPI MdsPut2VB2(char *node, char *expression, int *arg1d, void *arg1v, int *ansd, void *value)
 { return MdsPut2(node,expression,arg1d,arg1v,ansd,value,&zero);}
int WINAPI MdsPut3VB(char *node, char *expression, int *arg1d, int *arg2d, int *ansd)
 { return MdsPut(node,expression,arg1d,arg2d,ansd,&zero);}
int WINAPI MdsPut3VB2(char *node, char *expression, int *arg1d, void *arg1v, int *arg2d, void *arg2v,int *ansd, void *value)
 { return MdsPut2(node,expression,arg1d,arg1v,arg2d,arg2v,ansd,value,&zero);}
#endif

/************************************************************
For each platform, you can define the following macros to
determine the names of the corresponding fortran entry point:

       descr
       FortranMdsConnect
       FortranMdsClose
       FortranMdsDisconnect
       FortranMdsOpen
       FortranMdsSetDefault
       MdsPut
       MdsValue

If any of these entry points share the same name for fortran
and c then donot define the macro.
*************************************************************/

#if defined(__osf__) || defined(__sgi) || defined(__sun) || defined(__linux) || defined(__APPLE__) || defined (HAVE_WINDOWS_H)
#define descr descr_
#define descr2 descr2_
#define FortranMdsConnect mdsconnect_
#define FortranMdsClose mdsclose_
#define FortranMdsDisconnect mdsdisconnect_
#define FortranMdsOpen mdsopen_
#define FortranMdsSetDefault mdssetdefault_
#define FortranMdsSetSocket mdssetsocket_
#define MdsPut mdsput_
#define MdsValue mdsvalue_
#define MdsPut2 mdsput2_
#define MdsValue2 mdsvalue2_
#elif defined(__hpux) || defined(_AIX)
#define FortranMdsConnect mdsconnect
#define FortranMdsClose mdsclose
#define FortranMdsDisconnect mdsdisconnect
#define FortranMdsOpen mdsopen
#define FortranMdsSetDefault mdssetdefault
#define FortranMdsSetSocket  mdssetsocket
#define MdsPut mdsput
#define MdsValue mdsvalue
#define MdsPut2 mdsput2
#define MdsValue2 mdsvalue2
#endif
#ifdef __VMS
int MdsValue(struct dsc$descriptor *expression, ...);
#else
int MdsValue(char *expression, ...);
#endif
static SOCKET ___MdsConnect(char *host);
int descr (int *dtype, void *data, int *dim1, ...);
static int  ___MdsClose(char *tree, int *shot);
static void ___MdsDisconnect();
static int  ___MdsOpen(char *tree, int *shot);
static int  ___MdsSetDefault(char *node);
static int  ___MdsSetSocket(int *newsocket);

#ifdef FortranMdsConnect
SOCKET FortranMdsConnect(char *host) { return ___MdsConnect(host);}
#endif

#ifdef FortranMdsDisconnect
void FortranMdsDisconnect() { ___MdsDisconnect();}
#endif

#ifdef FortranMdsSetSocket
int FortranMdsSetSocket(int *newsocket) { return ___MdsSetSocket(newsocket);}
#endif

#ifdef FortranMdsClose
int  FortranMdsClose(char *tree, int *shot) { return ___MdsClose(tree,shot);}
#endif

#ifdef FortranMdsSetDefault
int  FortranMdsSetDefault(char *node) { return ___MdsSetDefault(node);}
#endif

#ifdef FortranMdsOpen
int  FortranMdsOpen(char *tree, int *shot) { return ___MdsOpen(tree,shot);}
#endif


#include __FILE__
#endif

