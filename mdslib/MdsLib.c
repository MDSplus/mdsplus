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
#include <mdsdescrip.h>
#define MDSLIB_NO_PROTOS
#include "mdslib.h"
SOCKET mdsSocket = INVALID_SOCKET;
#define NDESCRIP_CACHE 1024
static struct descriptor *descrs[NDESCRIP_CACHE];
#ifndef _CLIENT_ONLY
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

short ArgLen(struct descrip *d);

static int next = 0;
#define MIN(A,B)  ((A) < (B) ? (A) : (B))
#define MAXARGS 32
static char *MdsValueRemoteExpression(char *expression, struct descriptor *dsc);
static struct descrip *MakeIpDescrip(struct descrip *arg, struct descriptor *dsc);
static int dtype_length(struct descriptor *d);
static void MdsValueSet(struct descriptor *outdsc, struct descriptor *indsc, int *length);

static int va_descr(int *dtype, void *data, int *dim1, ...)
{

  /* variable length argument list:
   * (# elements in dim 1), (# elements in dim 2) ... 0, [length of (each) string if DTYPE_CSTRING]
   */

  struct descriptor *dsc;
  int totsize = *dim1;
  int retval;

  if (data == NULL) {
    printf("NULL pointer passed as data pointer\n");
    return -1;
  }

  if (descrs[next])
    free(descrs[next]);

  /* decide what type of descriptor is needed (descriptor, descriptor_a, array_coeff) */

  if (*dim1 == 0) {
    descrs[next] = malloc(sizeof(struct descriptor));
  } else {
    va_list incrmtr;
    int *dim;

    va_start(incrmtr, dim1);
    dim = va_arg(incrmtr, int *);
    if (*dim == 0) {
      descrs[next] = malloc(sizeof(struct descriptor_a));
    } else {
      descrs[next] = malloc(sizeof(array_coeff));
    }
  }

  dsc = descrs[next];

  dsc->dtype = *dtype;
  dsc->pointer = (char *)data;
  dsc->length = 0;
  dsc->length = dtype_length(dsc);	/* must set length after dtype and data pointers are set */

  /*  Convert DTYPE for native access if required.  Do AFTER the call to dtype_length() to
   *  save having to support DTYPE_NATIVE_FLOAT etc. in dtype_length().
   */

  if (mdsSocket == INVALID_SOCKET) {
    switch (dsc->dtype) {
    case DTYPE_FLOAT:
      dsc->dtype = DTYPE_NATIVE_FLOAT;
      break;
    case DTYPE_DOUBLE:
      dsc->dtype = DTYPE_NATIVE_DOUBLE;
      break;
    case DTYPE_COMPLEX:
      dsc->dtype = DTYPE_FLOAT_COMPLEX;
      break;
    case DTYPE_COMPLEX_DOUBLE:
      dsc->dtype = DTYPE_DOUBLE_COMPLEX;
      break;
    default:
      break;
    }
  }

  if (*dim1 == 0) {
    dsc->class = CLASS_S;

    if (dsc->dtype == DTYPE_CSTRING) {	/* && dsc->length == 0)  */
      va_list incrmtr;
      va_start(incrmtr, dim1);
      dsc->length = *va_arg(incrmtr, int *);
    }

  } else {

    va_list incrmtr;

    int ndim = 1;
    int *dim = &ndim;		/*must initialize dim to nonnull so test below passes */

    /* count the number of dimensions beyond the first */

    va_start(incrmtr, dim1);
    for (ndim = 1; *dim != 0; ndim++) {
      dim = va_arg(incrmtr, int *);
    }
    ndim = ndim - 1;		/* ndim is actually the number of dimensions specified */

    /* if requested descriptor is for a DTYPE_CSTRING, then following the null terminated 
     * list of dimensions there will be an int * to the length of each string in the array
     */

    if (dsc->dtype == DTYPE_CSTRING) {	/*  && dsc->length == 0)  */
      dsc->length = *va_arg(incrmtr, int *);
    }

    if (ndim > 1) {
      int i;
      array_coeff *adsc = (array_coeff *) dsc;
      adsc->class = CLASS_A;

      if (ndim > MAXDIM) {
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
      adsc->a0 = adsc->pointer;	/* &&& this will need to be adjusted for native API, as (array lower bound=0) will not be required. */
      adsc->m[0] = *dim1;

      va_start(incrmtr, dim1);
      for (i = 1; i < ndim; i++) {
	adsc->m[i] = *(va_arg(incrmtr, int *));
	totsize = totsize * adsc->m[i];
      }
      for (i = ndim; i < MAXDIM; i++) {
	adsc->m[i] = 0;
      }
      adsc->arsize = totsize * adsc->length;
    } else {
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
      if (ndim < 1)
	printf("(descr.c) WARNING: requested ndim<1, forcing to 1.\n");
    }

  }

  retval = next + 1;
  next++;
  if (next >= NDESCRIP_CACHE)
    next = 0;
  return retval;
}

static int va_descr2(int *dtype, int *dim1, ...)
{

  /* variable length argument list: 
   * (# elements in dim 1), (# elements in dim 2) ... 0, [length of (each) string if DTYPE_CSTRING]
   */

  struct descriptor *dsc;
  int totsize = *dim1;
  int retval;

  if (descrs[next])
    free(descrs[next]);

  /* decide what type of descriptor is needed (descriptor, descriptor_a, array_coeff) */

  if (*dim1 == 0) {
    descrs[next] = malloc(sizeof(struct descriptor));
  } else {
    va_list incrmtr;
    int *dim;

    va_start(incrmtr, dim1);
    dim = va_arg(incrmtr, int *);
    if (*dim == 0) {
      descrs[next] = malloc(sizeof(struct descriptor_a));
    } else {
      descrs[next] = malloc(sizeof(array_coeff));
    }
  }

  dsc = descrs[next];

  dsc->dtype = *dtype;

  dsc->pointer = 0;
  dsc->length = 0;
  dsc->length = dtype_length(dsc);	/* must set length after dtype and data pointers are set */

  /*  Convert DTYPE for native access if required.  Do AFTER the call to dtype_length() to
   *  save having to support DTYPE_NATIVE_FLOAT etc. in dtype_length().
   */

  if (mdsSocket == INVALID_SOCKET) {
    switch (dsc->dtype) {
    case DTYPE_FLOAT:
      dsc->dtype = DTYPE_NATIVE_FLOAT;
      break;
    case DTYPE_DOUBLE:
      dsc->dtype = DTYPE_NATIVE_DOUBLE;
      break;
    case DTYPE_COMPLEX:
      dsc->dtype = DTYPE_FLOAT_COMPLEX;
      break;
    case DTYPE_COMPLEX_DOUBLE:
      dsc->dtype = DTYPE_DOUBLE_COMPLEX;
      break;
    default:
      break;
    }
  }

  if (*dim1 == 0) {
    dsc->class = CLASS_S;

    if (dsc->dtype == DTYPE_CSTRING) {	/* && dsc->length == 0)  */
      va_list incrmtr;
      va_start(incrmtr, dim1);
      dsc->length = *va_arg(incrmtr, int *);
    }

  } else {

    va_list incrmtr;

    int ndim = 1;
    int *dim = &ndim;		/*must initialize dim to nonnull so test below passes */

    /* count the number of dimensions beyond the first */

    va_start(incrmtr, dim1);
    for (ndim = 1; *dim != 0; ndim++) {
      dim = va_arg(incrmtr, int *);
    }
    ndim = ndim - 1;		/* ndim is actually the number of dimensions specified */

    /* if requested descriptor is for a DTYPE_CSTRING, then following the null terminated 
     * list of dimensions there will be an int * to the length of each string in the array
     */

    if (dsc->dtype == DTYPE_CSTRING) {	/*  && dsc->length == 0)  */
      dsc->length = *va_arg(incrmtr, int *);
    }

    if (ndim > 1) {
      int i;
      array_coeff *adsc = (array_coeff *) dsc;
      adsc->class = CLASS_A;

      if (ndim > MAXDIM) {
	ndim = MAXDIM;
	printf("(descr.c) WARNING: requested ndim>MAXDIM, forcing to MAXDIM\n");
      }
      adsc->dimct = ndim;

      adsc->aflags.binscale = 0;
      adsc->aflags.redim = 1;
      adsc->aflags.column = 1;
      adsc->aflags.coeff = 1;
      adsc->aflags.bounds = 0;
      adsc->a0 = adsc->pointer;	/* &&& this will need to be adjusted for native API, as (array lower bound=0) will not be required. */
      adsc->m[0] = *dim1;

      va_start(incrmtr, dim1);
      for (i = 1; i < ndim; i++) {
	adsc->m[i] = *(va_arg(incrmtr, int *));
	totsize = totsize * adsc->m[i];
      }
      for (i = ndim; i < MAXDIM; i++) {
	adsc->m[i] = 0;
      }
      adsc->arsize = totsize * adsc->length;
    } else {
      struct descriptor_a *adsc = (struct descriptor_a *)dsc;
      adsc->class = CLASS_A;
      adsc->arsize = totsize * adsc->length;
      adsc->dimct = 1;
      adsc->aflags.binscale = 0;
      adsc->aflags.redim = 1;
      adsc->aflags.column = 1;
      adsc->aflags.coeff = 0;
      adsc->aflags.bounds = 0;
      if (ndim < 1)
	printf("(descr.c) WARNING: requested ndim<1, forcing to 1.\n");
    }

  }

  retval = next + 1;
  next++;
  if (next >= NDESCRIP_CACHE)
    next = 0;
  return retval;
}

static int va_MdsValue(char *expression, ...)
{
  va_list incrmtr;
  int a_count;
  int i;
  unsigned char nargs;
  struct descriptor *dsc;
  int *length;
  int status = 1;
  int *descnum = &status;	/* initialize to point at non zero value */
#ifndef _CLIENT_ONLY
  EMPTYXD(tmpxd);
  static int clear=0;
  static DESCRIPTOR_LONG(clear_d, &clear);
#endif

  va_start(incrmtr, expression);
  for (a_count = 0; *descnum != 0; a_count++) {
    descnum = va_arg(incrmtr, int *);
  }
  a_count--;			/* subtract one for terminator of argument list */

  length = va_arg(incrmtr, int *);
  if (length) {
    *length = 0;
  }

  if (mdsSocket != INVALID_SOCKET) {	/* CLIENT/SERVER */
    struct descriptor *dscAnswer;
    struct descrip exparg;
    struct descrip *arg = &exparg;
    char *newexpression;

    va_start(incrmtr, expression);
    nargs = a_count - 1;	/* -1 for answer argument */

    /* Get last argument - it is the answer descriptor number */
    for (i = 1; i <= nargs + 1; i++)
      descnum = va_arg(incrmtr, int *);
    dscAnswer = descrs[*descnum - 1];

    /* 
     * Send expression descriptor first.
     * MdsValueRemoteExpression wraps expression with type conversion function.
     * It malloc's space for newexpression that needs to be freed after
     */
    newexpression = MdsValueRemoteExpression(expression, dscAnswer);
    arg = MakeDescrip(&exparg, DTYPE_CSTRING, 0, 0, newexpression);
    status =
	SendArg(mdsSocket, (char)0, arg->dtype, (unsigned char)(nargs + 1), ArgLen(arg), arg->ndims,
		arg->dims, (void *)arg->ptr);
    free(newexpression);

    /* send each argument */

    va_start(incrmtr, expression);
    for (i = 1; i <= nargs && (status & 1); i++) {
      descnum = va_arg(incrmtr, int *);
      if (*descnum > 0) {
	dsc = descrs[*descnum - 1];
	arg = MakeIpDescrip(arg, dsc);
	status =
	    SendArg(mdsSocket, (unsigned char)i, arg->dtype, (char)(nargs + 1), ArgLen(arg),
		    arg->ndims, arg->dims, arg->ptr);
      } else {
	printf("I: %d    BAD DESCRIPTOR!!!\n", i);
      }
    }

    if (status & 1) {
      int numbytes;
      short len;
      void *dptr;
      char *dnew = 0;
      struct descrip exparg;
      struct descrip *arg = &exparg;

      arg = MakeIpDescrip(arg, dscAnswer);
      status =
	  GetAnswerInfo(mdsSocket, &arg->dtype, &len, &arg->ndims, arg->dims, &numbytes, &dptr);

      /**  Make a "regular" descriptor out of the returned IpDescrip.
       **  Cannot use LibCallg to call descr() because it will not be
       **  present for client-only library.
       **/

      if (status & 1) {
	int ansdescr;
	int dims[MAX_DIMS];
	int null = 0;
	int dtype = arg->dtype;
	int dlen = len;

	for (i = 0; i < arg->ndims; i++)
	  dims[i] = (int)arg->dims[i];

	if (arg->dtype == DTYPE_CSTRING && arg->ndims > 0 && dlen != dscAnswer->length) {
	  /** rewrite string array so that it gets copied to answer descriptor dscAnswer correctly **/
	  int i;
	  int nelements = numbytes / len;
	  int s = nelements * dscAnswer->length;
	  dnew = (char *)malloc(s);
	  for (i = 0; i < s; i++)
	    dnew[i] = 32;	/*fill */
	  for (i = 0; i < nelements; i++)
	    memcpy(dnew + (i * dscAnswer->length), (char *)dptr + (i * len),
		   MIN(dscAnswer->length, len));
	  dptr = dnew;
	  dlen = dscAnswer->length;
	}

	switch (arg->ndims) {
	case 0:
	  ansdescr = va_descr(&dtype, dptr, &null, &dlen);
	  break;
	case 1:
	  ansdescr = va_descr(&dtype, dptr, &dims[0], &null, &dlen);
	  break;
	case 2:
	  ansdescr = va_descr(&dtype, dptr, &dims[0], &dims[1], &null, &dlen);
	  break;
	case 3:
	  ansdescr = va_descr(&dtype, dptr, &dims[0], &dims[1], &dims[2], &null, &dlen);
	  break;
	case 4:
	  ansdescr = va_descr(&dtype, dptr, &dims[0], &dims[1], &dims[2], &dims[3], &null, &dlen);
	  break;
	case 5:
	  ansdescr =
	      va_descr(&dtype, dptr, &dims[0], &dims[1], &dims[2], &dims[3], &dims[4], &null,
		       &dlen);
	  break;
	case 6:
	  ansdescr =
	      va_descr(&dtype, dptr, &dims[0], &dims[1], &dims[2], &dims[3], &dims[4], &dims[5],
		       &null, &dlen);
	  break;
	case 7:
	  ansdescr =
	      va_descr(&dtype, dptr, &dims[0], &dims[1], &dims[2], &dims[3], &dims[4], &dims[5],
		       &dims[6], &null, &dlen);
	  break;
	default:
	  status = 0;
	  break;
	}
        if (status & 1)
	  MdsValueSet(dscAnswer, descrs[ansdescr - 1], length);
      }
      if (dnew)
	free(dnew);
    }

  } else
#ifdef _CLIENT_ONLY
  {
    printf("Must ConnectToMds first\n");
    status = 0;
  }
#else
  {
    void *arglist[MAXARGS];
    struct descriptor *dsc;
    struct descriptor dexpression = { 0, DTYPE_T, CLASS_S, 0 };
    EMPTYXD(xd1);
    EMPTYXD(xd2);
    EMPTYXD(xd3);
    int argidx = 1;
    int i;
    dexpression.length = strlen((char *)expression);
    dexpression.pointer = (char *)expression;
    arglist[argidx++] = (void *)&dexpression;
    va_start(incrmtr, expression);
    for (i = 1; i < a_count; i++) {
      descnum = va_arg(incrmtr, int *);
      dsc = descrs[*descnum - 1];
      arglist[argidx++] = (void *)dsc;
    }
    arglist[argidx++] = (void *)&xd1;
    arglist[argidx++] = MdsEND_ARG;
    *(int *)&arglist[0] = argidx - 1;
    status = LibCallg(arglist, TdiExecute);

    if (status & 1) {

      descnum = va_arg(incrmtr, int *);
      dsc = descrs[*descnum - 1];

      status = TdiData(xd1.pointer, &xd2 MDS_END_ARG);

      if (status & 1 && xd2.pointer != 0 && xd2.pointer->pointer != 0) {
	int templen = (xd2.pointer)->length;
	status = TdiCvt(&xd2, dsc, &xd3 MDS_END_ARG);
	  /**  get string length right if scalar string (if answer descriptor has longer
	   **  length than returned value, then make sure the length is the length of the 
           **  returned value
           **/
	if ((xd3.pointer)->dtype == DTYPE_CSTRING && (xd3.pointer->class != CLASS_A))
	  (xd3.pointer)->length = MIN(templen, (xd3.pointer)->length);
      }

      if (status & 1) {
	MdsValueSet(dsc, xd3.pointer, length);

	MdsFree1Dx(&xd1, NULL);
	MdsFree1Dx(&xd2, NULL);
	MdsFree1Dx(&xd3, NULL);	/* is answerptr still valid after calling this??? */
      }
    }
  }
  TdiDebug(&clear_d, &tmpxd MDS_END_ARG);
  MdsFree1Dx(&tmpxd, 0);
#endif
  return (status);

}

static int va_MdsValue2(char *expression, ...)
{
  va_list incrmtr;
  int a_count;
  int i;
  unsigned char nargs;
  struct descriptor *dsc;
  int *length;
  int status = 1;
  int *descnum = &status;	/* initialize to point at non zero value */

  va_start(incrmtr, expression);
  for (a_count = 0; *descnum != 0; a_count++) {
    descnum = va_arg(incrmtr, int *);
    if (*descnum != 0)
      va_arg(incrmtr, void *);
  }
  a_count--;			/* subtract one for terminator of argument list */

  length = va_arg(incrmtr, int *);
  if (length) {
    *length = 0;
  }

  if (mdsSocket != INVALID_SOCKET) {	/* CLIENT/SERVER */
    struct descriptor *dscAnswer;
    struct descrip exparg;
    struct descrip *arg = &exparg;
    char *newexpression;

    va_start(incrmtr, expression);
    nargs = a_count - 1;	/* -1 for answer argument */

    /* Get last argument - it is the answer descriptor number */
    for (i = 1; i <= (nargs * 2) + 1; i++)
      descnum = va_arg(incrmtr, int *);
    dscAnswer = descrs[*descnum - 1];
    dscAnswer->pointer = va_arg(incrmtr, void *);
    /* 
     * Send expression descriptor first.
     * MdsValueRemoteExpression wraps expression with type conversion function.
     * It malloc's space for newexpression that needs to be freed after
     */
    newexpression = MdsValueRemoteExpression(expression, dscAnswer);
    arg = MakeDescrip(&exparg, DTYPE_CSTRING, 0, 0, newexpression);
    status =
	SendArg(mdsSocket, (char)0, arg->dtype, (unsigned char)(nargs + 1), ArgLen(arg), arg->ndims,
		arg->dims, (void *)arg->ptr);
    free(newexpression);

    /* send each argument */

    va_start(incrmtr, expression);
    for (i = 1; i <= nargs && (status & 1); i++) {
      descnum = va_arg(incrmtr, int *);
      if (*descnum > 0) {
	dsc = descrs[*descnum - 1];
	dsc->pointer = va_arg(incrmtr, void *);
	arg = MakeIpDescrip(arg, dsc);
	status =
	    SendArg(mdsSocket, (unsigned char)i, arg->dtype, (char)(nargs + 1), ArgLen(arg),
		    arg->ndims, arg->dims, arg->ptr);
      } else {
	printf("I: %d    BAD DESCRIPTOR!!!\n", i);
      }
    }

    if (status & 1) {
      int numbytes;
      short len;
      void *dptr;
      char *dnew = 0;
      struct descrip exparg;
      struct descrip *arg = &exparg;

      arg = MakeIpDescrip(arg, dscAnswer);
      status =
	  GetAnswerInfo(mdsSocket, &arg->dtype, &len, &arg->ndims, arg->dims, &numbytes, &dptr);

      /**  Make a "regular" descriptor out of the returned IpDescrip.
       **  Cannot use LibCallg to call descr() because it will not be
       **  present for client-only library.
       **/

      if (status & 1) {
	int ansdescr;
	int dims[MAX_DIMS];
	int null = 0;
	int dtype = arg->dtype;
	int dlen = len;

	for (i = 0; i < arg->ndims; i++)
	  dims[i] = (int)arg->dims[i];

	if (arg->dtype == DTYPE_CSTRING && arg->ndims > 0 && dlen != dscAnswer->length) {
	  /** rewrite string array so that it gets copied to answer descriptor dscAnswer correctly **/
	  int i;
	  int nelements = numbytes / len;
	  int s = nelements * dscAnswer->length;
	  dnew = (char *)malloc(s);
	  for (i = 0; i < s; i++)
	    dnew[i] = 32;	/*fill */
	  for (i = 0; i < nelements; i++)
	    memcpy(dnew + (i * dscAnswer->length), (char *)dptr + (i * len),
		   MIN(dscAnswer->length, len));
	  dptr = dnew;
	  dlen = dscAnswer->length;
	}

	switch (arg->ndims) {
	case 0:
	  ansdescr = va_descr(&dtype, dptr, &null, &dlen);
	  break;
	case 1:
	  ansdescr = va_descr(&dtype, dptr, &dims[0], &null, &dlen);
	  break;
	case 2:
	  ansdescr = va_descr(&dtype, dptr, &dims[0], &dims[1], &null, &dlen);
	  break;
	case 3:
	  ansdescr = va_descr(&dtype, dptr, &dims[0], &dims[1], &dims[2], &null, &dlen);
	  break;
	case 4:
	  ansdescr = va_descr(&dtype, dptr, &dims[0], &dims[1], &dims[2], &dims[3], &null, &dlen);
	  break;
	case 5:
	  ansdescr =
	      va_descr(&dtype, dptr, &dims[0], &dims[1], &dims[2], &dims[3], &dims[4], &null,
		       &dlen);
	  break;
	case 6:
	  ansdescr =
	      va_descr(&dtype, dptr, &dims[0], &dims[1], &dims[2], &dims[3], &dims[4], &dims[5],
		       &null, &dlen);
	  break;
	case 7:
	  ansdescr =
	      va_descr(&dtype, dptr, &dims[0], &dims[1], &dims[2], &dims[3], &dims[4], &dims[5],
		       &dims[6], &null, &dlen);
	  break;
	default:
	  status = 0;
	  break;
	}
	if (status & 1)
	  MdsValueSet(dscAnswer, descrs[ansdescr - 1], length);
      }
      if (dnew)
	free(dnew);
    }

  } else
#ifdef _CLIENT_ONLY
  {
    printf("Must ConnectToMds first\n");
    status = 0;
  }
#else
  {
    void *arglist[MAXARGS];
    struct descriptor *dsc;
    struct descriptor dexpression = { 0, DTYPE_T, CLASS_S, 0 };
    EMPTYXD(xd1);
    EMPTYXD(xd2);
    EMPTYXD(xd3);
    int argidx = 1;
    int i;
    dexpression.length = strlen((char *)expression);
    dexpression.pointer = (char *)expression;
    arglist[argidx++] = (void *)&dexpression;
    va_start(incrmtr, expression);
    for (i = 1; i < a_count; i++) {
      descnum = va_arg(incrmtr, int *);
      dsc = descrs[*descnum - 1];
      dsc->pointer = va_arg(incrmtr, void *);
      arglist[argidx++] = (void *)dsc;
    }
    arglist[argidx++] = (void *)&xd1;
    arglist[argidx++] = MdsEND_ARG;
    *(int *)&arglist[0] = argidx - 1;
    status = LibCallg(arglist, TdiExecute);

    if (status & 1) {

      descnum = va_arg(incrmtr, int *);
      dsc = descrs[*descnum - 1];
      dsc->pointer = va_arg(incrmtr, void *);

      status = TdiData(xd1.pointer, &xd2 MDS_END_ARG);

      if (status & 1 && xd2.pointer) {
	int templen = (xd2.pointer)->length;
	status = TdiCvt(&xd2, dsc, &xd3 MDS_END_ARG);
	  /**  get string length right if scalar string (if answer descriptor has longer
	   **  length than returned value, then make sure the length is the length of the 
           **  returned value
           **/
	if ((xd3.pointer)->dtype == DTYPE_CSTRING && (xd3.pointer->class != CLASS_A))
	  (xd3.pointer)->length = MIN(templen, (xd3.pointer)->length);
      }

      if (status & 1) {
	MdsValueSet(dsc, xd3.pointer, length);

	MdsFree1Dx(&xd1, NULL);
	MdsFree1Dx(&xd2, NULL);
	MdsFree1Dx(&xd3, NULL);	/* is answerptr still valid after calling this??? */
      }
    }
  }
#endif
  return (status);

}

static int va_MdsPut(char *pathname, char *expression, ...)
{
  va_list incrmtr;
  int a_count;
  int i;
  unsigned char nargs;

  struct descriptor *dsc;
  int status = 1;
  int *descnum = &status;	/* initialize to point at non zero value */

  va_start(incrmtr, expression);
  for (a_count = 0; *descnum != 0; a_count++) {
    descnum = va_arg(incrmtr, int *);
  }
  a_count--;			/* subtract one for terminator of argument list */

  if (mdsSocket != INVALID_SOCKET) {	/* CLIENT/SERVER */
    static char *putexpprefix = "TreePut(";
    static char *argplace = "$,";
    char *putexp;
    struct descrip putexparg;
    struct descrip exparg;
    struct descrip *arg;
    unsigned char idx = 0;
    nargs = a_count + 2;	/* +1 for pathname +1 for expression */
    putexp = malloc(strlen(putexpprefix) + (nargs) * strlen(argplace) + 1);
    strcpy(putexp, putexpprefix);
    for (i = 0; i < nargs; i++)
      strcat(putexp, argplace);
    putexp[strlen(putexp) - 1] = ')';

    va_start(incrmtr, expression);

    nargs = nargs + 1;		/* add 1 for putexp  sent first */
    arg = MakeDescrip(&putexparg, DTYPE_CSTRING, 0, 0, putexp);
    status =
	SendArg(mdsSocket, idx++, arg->dtype, nargs, ArgLen(arg), arg->ndims, arg->dims, arg->ptr);
    free(putexp);
    arg = MakeDescrip(&exparg, DTYPE_CSTRING, 0, 0, pathname);
    status =
	SendArg(mdsSocket, idx++, arg->dtype, nargs, ArgLen(arg), arg->ndims, arg->dims, arg->ptr);
    arg = MakeDescrip(&exparg, DTYPE_CSTRING, 0, 0, expression);
    for (i = idx; i < nargs && (status & 1); i++) {
      status =
	  SendArg(mdsSocket, (char)i, arg->dtype, nargs, ArgLen(arg), arg->ndims, arg->dims,
		  arg->ptr);
      descnum = va_arg(incrmtr, int *);
      if (*descnum > 0) {
	dsc = descrs[*descnum - 1];
	arg = MakeIpDescrip(arg, dsc);
      }
    }

    if (status & 1) {
      char dtype;
      int dims[MAX_DIMS];
      char ndims;
      short len;
      int numbytes;
      void *dptr;
      status = GetAnswerInfo(mdsSocket, &dtype, &len, &ndims, dims, &numbytes, &dptr);
      if (status & 1 && dtype == DTYPE_LONG && ndims == 0 && numbytes == sizeof(int))
	memcpy(&status, dptr, numbytes);
    }
  } else
#ifdef _CLIENT_ONLY
  {
    printf("Must ConnectToMds first\n");
    status = 0;
  }
#else
  {

    void *arglist[MAXARGS];
    struct descriptor *dsc;
    struct descriptor dexpression = { 0, DTYPE_T, CLASS_S, 0 };
    EMPTYXD(tmp);
    int argidx = 1;
    int i;
    int nid;

    if ((status = TreeFindNode(pathname, &nid)) & 1) {
      dexpression.length = strlen((char *)expression);
      dexpression.pointer = (char *)expression;
      arglist[argidx++] = (void *)&dexpression;
      va_start(incrmtr, expression);
      for (i = 1; i <= a_count; i++) {
	descnum = va_arg(incrmtr, int *);
	dsc = descrs[*descnum - 1];
	arglist[argidx++] = (void *)dsc;
      }
      arglist[argidx++] = (void *)&tmp;
      arglist[argidx++] = MdsEND_ARG;
      *(int *)&arglist[0] = argidx - 1;

      status = LibCallg(arglist, TdiCompile);

      if (status & 1) {
	if ((status = TreePutRecord(nid, (struct descriptor *)arglist[argidx - 2], 0)) & 1) {
	  TreeWait();
	}
      }
      MdsFree1Dx(&tmp, NULL);
    }

  }
#endif
  return (status);
}

static int va_MdsPut2(char *pathname, char *expression, ...)
{
  va_list incrmtr;
  int a_count;
  int i;
  unsigned char nargs;

  struct descriptor *dsc;
  int status = 1;
  int *descnum = &status;	/* initialize to point at non zero value */

  va_start(incrmtr, expression);
  for (a_count = 0; *descnum != 0; a_count++) {
    descnum = va_arg(incrmtr, int *);
    va_arg(incrmtr, void *);
  }
  a_count--;			/* subtract one for terminator of argument list */

  if (mdsSocket != INVALID_SOCKET) {	/* CLIENT/SERVER */
    static char *putexpprefix = "TreePut(";
    static char *argplace = "$,";
    char *putexp;
    struct descrip putexparg;
    struct descrip exparg;
    struct descrip *arg;
    unsigned char idx = 0;
    nargs = a_count + 2;	/* +1 for pathname +1 for expression */
    putexp = malloc(strlen(putexpprefix) + (nargs) * strlen(argplace) + 1);
    strcpy(putexp, putexpprefix);
    for (i = 0; i < nargs; i++)
      strcat(putexp, argplace);
    putexp[strlen(putexp) - 1] = ')';

    va_start(incrmtr, expression);

    nargs = nargs + 1;		/* add 1 for putexp  sent first */
    arg = MakeDescrip(&putexparg, DTYPE_CSTRING, 0, 0, putexp);
    status =
	SendArg(mdsSocket, idx++, arg->dtype, nargs, ArgLen(arg), arg->ndims, arg->dims, arg->ptr);
    free(putexp);
    arg = MakeDescrip(&exparg, DTYPE_CSTRING, 0, 0, pathname);
    status =
	SendArg(mdsSocket, idx++, arg->dtype, nargs, ArgLen(arg), arg->ndims, arg->dims, arg->ptr);
    arg = MakeDescrip(&exparg, DTYPE_CSTRING, 0, 0, expression);
    for (i = idx; i < nargs && (status & 1); i++) {
      status =
	  SendArg(mdsSocket, (char)i, arg->dtype, nargs, ArgLen(arg), arg->ndims, arg->dims,
		  arg->ptr);
      descnum = va_arg(incrmtr, int *);
      if (*descnum > 0) {
	dsc = descrs[*descnum - 1];
	dsc->pointer = va_arg(incrmtr, void *);
	arg = MakeIpDescrip(arg, dsc);
      }
    }

    if (status & 1) {
      char dtype;
      int dims[MAX_DIMS];
      char ndims;
      short len;
      int numbytes;
      void *dptr;
      status = GetAnswerInfo(mdsSocket, &dtype, &len, &ndims, dims, &numbytes, &dptr);
      if (status & 1 && dtype == DTYPE_LONG && ndims == 0 && numbytes == sizeof(int))
	memcpy(&status, dptr, numbytes);
    }
  } else
#ifdef _CLIENT_ONLY
  {
    printf("Must ConnectToMds first\n");
    status = 0;
  }
#else
  {

    void *arglist[MAXARGS];
    struct descriptor *dsc;
    struct descriptor dexpression = { 0, DTYPE_T, CLASS_S, 0 };
    EMPTYXD(tmp);
    int argidx = 1;
    int i;
    int nid;

    if ((status = TreeFindNode(pathname, &nid)) & 1) {
      dexpression.length = strlen((char *)expression);
      dexpression.pointer = (char *)expression;
      arglist[argidx++] = (void *)&dexpression;
      va_start(incrmtr, expression);
      for (i = 1; i <= a_count; i++) {
	descnum = va_arg(incrmtr, int *);
	dsc = descrs[*descnum - 1];
	dsc->pointer = va_arg(incrmtr, void *);
	arglist[argidx++] = (void *)dsc;
      }
      arglist[argidx++] = (void *)&tmp;
      arglist[argidx++] = MdsEND_ARG;
      *(int *)&arglist[0] = argidx - 1;

      status = LibCallg(arglist, TdiCompile);

      if (status & 1) {
	if ((status = TreePutRecord(nid, (struct descriptor *)arglist[argidx - 2]), 0) & 1) {
	  TreeWait();
	}
      }
      MdsFree1Dx(&tmp, NULL);
    }

  }
#endif
  return (status);
}

static int dtype_length(struct descriptor *d)
{
  short len;

  /*  This function needs to handle the DTYPE values in ipdesc.h as well 
   *  as the "native" DTYPEs, as it is called both for descriptors before
   *  evaluation, and for the answer descriptor, which for local access will
   *  be returned with native DTYPE.
   */

  switch (d->dtype) {
  case DTYPE_UCHAR:
  case DTYPE_CHAR:
    len = sizeof(char);
    break;
  case DTYPE_USHORT:
  case DTYPE_SHORT:
    len = sizeof(short);
    break;
  case DTYPE_ULONG:
  case DTYPE_LONG:
    len = sizeof(int);
    break;
  case DTYPE_ULONGLONG:
  case DTYPE_LONGLONG:
    len = sizeof(int) * 2;
    break;
#if DTYPE_NATIVE_FLOAT != DTYPE_FLOAT
  case DTYPE_NATIVE_FLOAT:
#endif
  case DTYPE_FLOAT:
    len = sizeof(float);
    break;
#if DTYPE_NATIVE_DOUBLE != DTYPE_DOUBLE
  case DTYPE_NATIVE_DOUBLE:
#endif
  case DTYPE_DOUBLE:
    len = sizeof(double);
    break;
#if DTYPE_FLOAT_COMPLEX != DTYPE_COMPLEX
  case DTYPE_FLOAT_COMPLEX:
#endif
  case DTYPE_COMPLEX:
    len = sizeof(float) * 2;
    break;
#if DTYPE_DOUBLE_COMPLEX != DTYPE_COMPLEX_DOUBLE
  case DTYPE_DOUBLE_COMPLEX:
#endif
  case DTYPE_COMPLEX_DOUBLE:
    len = sizeof(double) * 2;
    break;
  case DTYPE_CSTRING:
    len = d->length ? d->length : (d->pointer ? strlen(d->pointer) : 0);
    break;
  default:
    len = 0;
    break;
  }
  return len;
}


#if !defined(_CLIENT_ONLY)
extern EXPORT int *cdescr(int dtype, void *data, ...)
{
  void *arglist[MAXARGS];
  va_list incrmtr;
  int dsc;
  static int status;
  int argidx = 1;
  arglist[argidx++] = (void *)&dtype;
  arglist[argidx++] = (void *)data;

  va_start(incrmtr, data);

  dsc = 1;			/* initialize ok */
  for (; dsc != 0;) {
    dsc = va_arg(incrmtr, int);
    arglist[argidx++] = (void *)&dsc;
  }

  if (dtype == DTYPE_CSTRING) {
    dsc = va_arg(incrmtr, int);
    arglist[argidx++] = (void *)&dsc;
  }
  arglist[argidx++] = MdsEND_ARG;
  *(int *)&arglist[0] = argidx - 1;
  status = LibCallg(arglist, va_descr);
  return (&status);
}
#endif

static struct descrip *MakeIpDescrip(struct descrip *arg, struct descriptor *dsc)
{

  char dtype;
  dtype = dsc->dtype;

  switch (dtype) {
  case DTYPE_NATIVE_FLOAT:
    dtype = DTYPE_FLOAT;
    break;
  case DTYPE_NATIVE_DOUBLE:
    dtype = DTYPE_DOUBLE;
    break;
  case DTYPE_FLOAT_COMPLEX:
    dtype = DTYPE_COMPLEX;
    break;
  case DTYPE_DOUBLE_COMPLEX:
    dtype = DTYPE_COMPLEX_DOUBLE;
    break;
  }

  if (dsc->class == CLASS_S) {
    if (dsc->length)
      arg =
	  MakeDescripWithLength(arg, (char)dtype, (int)dsc->length, (char)0, (int *)0,
				dsc->pointer);
    else
      arg = MakeDescrip(arg, (char)dtype, (char)0, (int *)0, dsc->pointer);
  } else {
    int i;
    array_coeff *adsc = (array_coeff *) dsc;
    int dims[MAXDIM];
    unsigned int num = adsc->arsize / adsc->length;
    unsigned int *m = &num;
    if (adsc->dimct > 1)
      m = adsc->m;
    for (i = 0; i < adsc->dimct; i++)
      dims[i] = m[i];
    for (i = adsc->dimct; i < MAXDIM; i++)
      dims[i] = 0;
    if (dsc->length)
      arg =
	  MakeDescripWithLength(arg, (char)dtype, (int)dsc->length, adsc->dimct, dims,
				adsc->pointer);
    else
      arg = MakeDescrip(arg, (char)dtype, adsc->dimct, dims, adsc->pointer);
  }
  return arg;
}

static int MdsValueLength(struct descriptor *dsc)
{
  int length;
  switch (dsc->class) {
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
  return (length);
}

static char *MdsValueRemoteExpression(char *expression, struct descriptor *dsc)
{

  /* This function will wrap expression in the appropriate type
   * conversion function to ensure that the return value of MdsValue
   * is of the right type.  It is only used for remote MDSplus.
   */

  char *native_float_str, *native_double_str, *native_complex_str, *native_double_complex_str;
  char *newexpression = (char *)malloc(strlen(expression) + 24);

  /*  Determine the native floating/double/complex type of the client, so as to pass the correct
   *  conversion function to the server and thus save extra conversion steps.
   */

  switch (DTYPE_NATIVE_FLOAT) {
  case DTYPE_FS:
    native_float_str = "FS_FLOAT";
    native_complex_str = "FS_COMPLEX";
    break;
  case DTYPE_F:
    native_float_str = "F_FLOAT";
    native_complex_str = "F_COMPLEX";
    break;
  default:
    printf("Unknown DTYPE_NATIVE_FLOAT: %d.  Using FS_FLOAT.\n", DTYPE_NATIVE_FLOAT);
    native_float_str = "FS_FLOAT";
    native_complex_str = "FS_COMPLEX";
    break;
  }

  switch (DTYPE_NATIVE_DOUBLE) {
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
    printf("Unknown DTYPE_NATIVE_DOUBLE: %d.  Using FT_FLOAT.\n", DTYPE_NATIVE_DOUBLE);
    native_double_str = "FT_FLOAT";
    native_double_complex_str = "FT_COMPLEX";
    break;
  }

  /*  The DTYPE of the incoming descriptor will be one of the values in ipdesc.h.
   *  The switch clause below therefore does not need to support NATIVE_FLOAT etc.
   */

  switch (dsc->dtype) {
  case DTYPE_UCHAR:
    strcpy(newexpression, "BYTE_UNSIGNED");
    break;
  case DTYPE_USHORT:
    strcpy(newexpression, "WORD_UNSIGNED");
    break;
  case DTYPE_ULONG:
    strcpy(newexpression, "LONG_UNSIGNED");
    break;
  case DTYPE_ULONGLONG:
    strcpy(newexpression, "QUADWORD_UNSIGNED");
    break;
  case DTYPE_CHAR:
    strcpy(newexpression, "BYTE");
    break;
  case DTYPE_SHORT:
    strcpy(newexpression, "WORD");
    break;
  case DTYPE_LONG:
    strcpy(newexpression, "LONG");
    break;
  case DTYPE_LONGLONG:
    strcpy(newexpression, "QUADWORD");
    break;
  case DTYPE_FLOAT:
    strcpy(newexpression, native_float_str);
    break;
  case DTYPE_DOUBLE:
    strcpy(newexpression, native_double_str);
    break;
  case DTYPE_COMPLEX:
    strcpy(newexpression, native_complex_str);
    break;
  case DTYPE_COMPLEX_DOUBLE:
    strcpy(newexpression, native_double_complex_str);
    break;
  case DTYPE_CSTRING:
    strcpy(newexpression, "TEXT");
    break;
  }

  strcat(newexpression, "((");
  strcat(newexpression, expression);
  strcat(newexpression, "))");
  return newexpression;

}

static void MdsValueMove(int source_length, char *source_array, char fill, int dest_length,
			 char *dest_array)
{
  int i;
  memcpy(dest_array, source_array, MIN(source_length, dest_length));
  for (i = 0; i < dest_length - source_length; i++) {
    dest_array[source_length + i] = fill;
  }
}

static void MdsValueCopy(int dim, int length, char fill, char *in, unsigned int *in_m, char *out,
			 unsigned int *out_m)
{
  unsigned int i;
  int j;
  if (dim == 1)
    MdsValueMove(length * in_m[0], in, fill, length * out_m[0], out);
  else {
    int in_increment = length;
    int out_increment = length;
    for (j = 0; j < dim - 1; j++) {
      in_increment *= in_m[j];
      out_increment *= out_m[j];
    }
    for (i = 0; i < in_m[dim - 1] && i < out_m[dim - 1]; i++)
      MdsValueCopy(dim - 1, length, fill, in + in_increment * i, in_m, out + out_increment * i,
		   out_m);
  }
}

static void MdsValueSet(struct descriptor *outdsc, struct descriptor *indsc, int *length)
{
  char fill;
  if (indsc == 0) {
    if (length)
      *length = 0;
    return;
  }
  fill = (outdsc->dtype == DTYPE_CSTRING) ? 32 : 0;
  if ((indsc->class == CLASS_A) &&
      (outdsc->class == CLASS_A) &&
      (((struct descriptor_a *)outdsc)->dimct > 1) &&
      (((struct descriptor_a *)outdsc)->dimct == ((struct descriptor_a *)indsc)->dimct)) {
    array_coeff *in_a = (array_coeff *) indsc;
    array_coeff *out_a = (array_coeff *) outdsc;
    MdsValueMove(0, 0, fill, MdsValueLength(outdsc), out_a->pointer);
    MdsValueCopy(out_a->dimct, in_a->length, fill, in_a->pointer, in_a->m, out_a->pointer,
		 out_a->m);
  } else {
    MdsValueMove(MdsValueLength(indsc), indsc->pointer, fill, MdsValueLength(outdsc),
		 outdsc->pointer);
  }

  if (length) {
    if (indsc->class == CLASS_A)
      *length = MIN(((struct descriptor_a *)outdsc)->arsize / outdsc->length,
		    ((struct descriptor_a *)indsc)->arsize / indsc->length);
    else {
      if (indsc->dtype == DTYPE_CSTRING) {
	*length = MIN(outdsc->length, indsc->length);
      } else {
	*length = MIN(outdsc->length / dtype_length(outdsc), indsc->length / dtype_length(indsc));
      }
    }
  }
}
#else				/* FORTRAN_ENTRY_POINTS */
extern EXPORT int MdsOpen(char *tree, int *shot)
{
  return ___MdsOpen(tree, shot);
}

static int ___MdsOpen(char *tree, int *shot)
{
  int status = 0;
  if (mdsSocket != INVALID_SOCKET) {

    long answer;
    int length;
    int d1, d2, d3;		/* descriptor numbers passed to MdsValue */
    int dtype_cstring = DTYPE_CSTRING;
    int dtype_long = DTYPE_LONG;
    int null = 0;

    static char *expression = "TreeOpen($,$)";

    length = strlen(tree);
    d1 = va_descr(&dtype_cstring, tree, &null, &length);
    d2 = va_descr(&dtype_long, shot, &null);
    d3 = va_descr(&dtype_long, &answer, &null);

    status = MdsValue(expression, &d1, &d2, &d3, &null, &length);
    if ((status & 1)) {
      return *(int *)&answer;
    } else
      return 0;
  } else
#ifdef _CLIENT_ONLY
  {
    printf("Must ConnectToMds first\n");
    status = 0;
  }
#else
  {
    return TreeOpen(tree, *shot, 0);
  }
#endif
  return status;
}

static SOCKET ___MdsSetSocket(SOCKET *newsocket)
{
  SOCKET oldsocket = mdsSocket;
  mdsSocket = *newsocket;
  return oldsocket;
}

extern EXPORT int MdsSetSocket(SOCKET * newsocket)
{
  return ___MdsSetSocket(newsocket);
}

extern EXPORT int MdsClose(char *tree, int *shot)
{
  return ___MdsClose(tree, shot);
}

static int ___MdsClose(char *tree, int *shot)
{
  int status = 0;
  if (mdsSocket != INVALID_SOCKET) {

    long answer;
    int length;
    int d1, d2, d3;		/* descriptor numbers passed to MdsValue */
    int dtype_cstring = DTYPE_CSTRING;
    int dtype_long = DTYPE_LONG;
    int null = 0;

    static char *expression = "TreeClose($,$)";

    length = strlen(tree);
    d1 = va_descr(&dtype_cstring, tree, &null, &length);
    d2 = va_descr(&dtype_long, shot, &null);
    d3 = va_descr(&dtype_long, &answer, &null);

    status = MdsValue(expression, &d1, &d2, &d3, &null, &length);

    if ((status & 1)) {
      return *(int *)&answer;
    } else
      return 0;
  } else
#ifdef _CLIENT_ONLY
  {
    printf("Must ConnectToMds first\n");
    status = 0;
  }
#else
  {
    return TreeClose(tree, *shot);
  }
#endif
  return status;
}

extern EXPORT int MdsSetDefault(char *node)
{
  return ___MdsSetDefault(node);
}

static int ___MdsSetDefault(char *node)
{
  int status;

  if (mdsSocket != INVALID_SOCKET) {
    char *expression = strcpy((char *)malloc(strlen(node) + 20), "TreeSetDefault('");
    long answer;
    int length = strlen(node);
    int null = 0;
    int dtype_long = DTYPE_LONG;
    int d1 = va_descr(&dtype_long, &answer, &null);
    if (node[0] == '\\')
      strcat(expression, "\\");
    strcat(expression, node);
    strcat(expression, "')");
    status = MdsValue(expression, &d1, &null, &length);
    free(expression);
    if ((status & 1)) {
      return *(int *)&answer;
    } else
      return 0;
  }

  else
#ifdef _CLIENT_ONLY
  {
    printf("Must ConnectToMds first\n");
    status = 0;
  }
#else

  {

    int nid;
    return TreeSetDefault(node, &nid);
  }
#endif
  return status;
}

extern EXPORT void MdsDisconnect()
{
  ___MdsDisconnect();
}

static void ___MdsDisconnect()
{
  DisconnectFromMds(mdsSocket);
  mdsSocket = INVALID_SOCKET;	   /*** SETS GLOBAL VARIABLE mdsSOCKET ***/
}

extern EXPORT SOCKET MdsConnect(char *host)
{
  return ___MdsConnect(host);
}

static SOCKET ___MdsConnect(char *host)
{
  if (mdsSocket != INVALID_SOCKET) {
    MdsDisconnect();
  }
  mdsSocket = ConnectToMds(host);  /*** SETS GLOBAL VARIABLE mdsSOCKET ***/
  return (mdsSocket);
}

/* end FORTRAN_ENTRY_POINTS */
#endif

#if !defined(FORTRAN_ENTRY_POINTS) || defined(descr)
extern EXPORT int descr(int *dtype, void *data, int *dim1, int *dim2, int *dim3, int *dim4, int *dim5, int *dim6,
	  int *dim7, int *dim8, int *dim9)
{
  return va_descr(dtype, data, dim1, dim2, dim3, dim4, dim5, dim6, dim7, dim8, dim9);
}

extern EXPORT int descr2(int *dtype, int *dim1, int *dim2, int *dim3, int *dim4, int *dim5, int *dim6, int *dim7,
	   int *dim8, int *dim9)
{
  return va_descr2(dtype, dim1, dim2, dim3, dim4, dim5, dim6, dim7, dim8, dim9);
}
#endif

#if !defined(FORTRAN_ENTRY_POINTS) || defined(MdsValue)
extern EXPORT int MdsValue(char *expression,
	     int *a00, int *a01, int *a02, int *a03, int *a04, int *a05, int *a06, int *a07,
	     int *a08, int *a09, int *a10, int *a11, int *a12, int *a13, int *a14, int *a15,
	     int *a16, int *a17, int *a18, int *a19, int *a20, int *a21, int *a22, int *a23,
	     int *a24, int *a25, int *a26, int *a27, int *a28, int *a29, int *a30, int *a31,
	     int *a32, int *a33, int *a34, int *a35, int *a36, int *a37, int *a38, int *a39,
	     int *a40, int *a41, int *a42, int *a43, int *a44, int *a45, int *a46, int *a47,
	     int *a48, int *a49, int *a50, int *a51, int *a52, int *a53, int *a54, int *a55,
	     int *a56, int *a57, int *a58, int *a59, int *a60, int *a61, int *a62, int *a63,
	     int *a64, int *a65, int *a66, int *a67, int *a68, int *a69, int *a70, int *a71,
	     int *a72, int *a73, int *a74, int *a75, int *a76, int *a77, int *a78, int *a79,
	     int *a80, int *a81, int *a82, int *a83, int *a84, int *a85, int *a86, int *a87,
	     int *a88, int *a89, int *a90, int *a91, int *a92, int *a93, int *a94, int *a95,
	     int *a96, int *a97, int *a98, int *a99)
{
  return va_MdsValue(expression,
		     a00, a01, a02, a03, a04, a05, a06, a07, a08, a09,
		     a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
		     a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
		     a30, a31, a32, a33, a34, a35, a36, a37, a38, a39,
		     a40, a41, a42, a43, a44, a45, a46, a47, a48, a49,
		     a50, a51, a52, a53, a54, a55, a56, a57, a58, a59,
		     a60, a61, a62, a63, a64, a65, a66, a67, a68, a69,
		     a70, a71, a72, a73, a74, a75, a76, a77, a78, a79,
		     a80, a81, a82, a83, a84, a85, a86, a87, a88, a89,
		     a90, a91, a92, a93, a94, a95, a96, a97, a98, a99);
}

extern EXPORT int MdsValue2(char *expression,
	      int *a00, int *a01, int *a02, int *a03, int *a04, int *a05, int *a06, int *a07,
	      int *a08, int *a09, int *a10, int *a11, int *a12, int *a13, int *a14, int *a15,
	      int *a16, int *a17, int *a18, int *a19, int *a20, int *a21, int *a22, int *a23,
	      int *a24, int *a25, int *a26, int *a27, int *a28, int *a29, int *a30, int *a31,
	      int *a32, int *a33, int *a34, int *a35, int *a36, int *a37, int *a38, int *a39,
	      int *a40, int *a41, int *a42, int *a43, int *a44, int *a45, int *a46, int *a47,
	      int *a48, int *a49, int *a50, int *a51, int *a52, int *a53, int *a54, int *a55,
	      int *a56, int *a57, int *a58, int *a59, int *a60, int *a61, int *a62, int *a63,
	      int *a64, int *a65, int *a66, int *a67, int *a68, int *a69, int *a70, int *a71,
	      int *a72, int *a73, int *a74, int *a75, int *a76, int *a77, int *a78, int *a79,
	      int *a80, int *a81, int *a82, int *a83, int *a84, int *a85, int *a86, int *a87,
	      int *a88, int *a89, int *a90, int *a91, int *a92, int *a93, int *a94, int *a95,
	      int *a96, int *a97, int *a98, int *a99)
{
  return va_MdsValue2(expression,
		      a00, a01, a02, a03, a04, a05, a06, a07, a08, a09,
		      a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
		      a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
		      a30, a31, a32, a33, a34, a35, a36, a37, a38, a39,
		      a40, a41, a42, a43, a44, a45, a46, a47, a48, a49,
		      a50, a51, a52, a53, a54, a55, a56, a57, a58, a59,
		      a60, a61, a62, a63, a64, a65, a66, a67, a68, a69,
		      a70, a71, a72, a73, a74, a75, a76, a77, a78, a79,
		      a80, a81, a82, a83, a84, a85, a86, a87, a88, a89,
		      a90, a91, a92, a93, a94, a95, a96, a97, a98, a99);
}

#endif
#if !defined(FORTRAN_ENTRY_POINTS) || defined(MdsPut)
extern EXPORT int MdsPut(char *node, char *expression,
	   int *a00, int *a01, int *a02, int *a03, int *a04, int *a05, int *a06, int *a07, int *a08,
	   int *a09, int *a10, int *a11, int *a12, int *a13, int *a14, int *a15, int *a16, int *a17,
	   int *a18, int *a19, int *a20, int *a21, int *a22, int *a23, int *a24, int *a25, int *a26,
	   int *a27, int *a28, int *a29, int *a30, int *a31, int *a32, int *a33, int *a34, int *a35,
	   int *a36, int *a37, int *a38, int *a39, int *a40, int *a41, int *a42, int *a43, int *a44,
	   int *a45, int *a46, int *a47, int *a48, int *a49, int *a50, int *a51, int *a52, int *a53,
	   int *a54, int *a55, int *a56, int *a57, int *a58, int *a59, int *a60, int *a61, int *a62,
	   int *a63, int *a64, int *a65, int *a66, int *a67, int *a68, int *a69, int *a70, int *a71,
	   int *a72, int *a73, int *a74, int *a75, int *a76, int *a77, int *a78, int *a79, int *a80,
	   int *a81, int *a82, int *a83, int *a84, int *a85, int *a86, int *a87, int *a88, int *a89,
	   int *a90, int *a91, int *a92, int *a93, int *a94, int *a95, int *a96, int *a97, int *a98,
	   int *a99)
{
  return va_MdsPut(node, expression,
		   a00, a01, a02, a03, a04, a05, a06, a07, a08, a09,
		   a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
		   a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
		   a30, a31, a32, a33, a34, a35, a36, a37, a38, a39,
		   a40, a41, a42, a43, a44, a45, a46, a47, a48, a49,
		   a50, a51, a52, a53, a54, a55, a56, a57, a58, a59,
		   a60, a61, a62, a63, a64, a65, a66, a67, a68, a69,
		   a70, a71, a72, a73, a74, a75, a76, a77, a78, a79,
		   a80, a81, a82, a83, a84, a85, a86, a87, a88, a89,
		   a90, a91, a92, a93, a94, a95, a96, a97, a98, a99);
}

extern EXPORT int MdsPut2(char *node, char *expression,
	    int *a00, int *a01, int *a02, int *a03, int *a04, int *a05, int *a06, int *a07,
	    int *a08, int *a09, int *a10, int *a11, int *a12, int *a13, int *a14, int *a15,
	    int *a16, int *a17, int *a18, int *a19, int *a20, int *a21, int *a22, int *a23,
	    int *a24, int *a25, int *a26, int *a27, int *a28, int *a29, int *a30, int *a31,
	    int *a32, int *a33, int *a34, int *a35, int *a36, int *a37, int *a38, int *a39,
	    int *a40, int *a41, int *a42, int *a43, int *a44, int *a45, int *a46, int *a47,
	    int *a48, int *a49, int *a50, int *a51, int *a52, int *a53, int *a54, int *a55,
	    int *a56, int *a57, int *a58, int *a59, int *a60, int *a61, int *a62, int *a63,
	    int *a64, int *a65, int *a66, int *a67, int *a68, int *a69, int *a70, int *a71,
	    int *a72, int *a73, int *a74, int *a75, int *a76, int *a77, int *a78, int *a79,
	    int *a80, int *a81, int *a82, int *a83, int *a84, int *a85, int *a86, int *a87,
	    int *a88, int *a89, int *a90, int *a91, int *a92, int *a93, int *a94, int *a95,
	    int *a96, int *a97, int *a98, int *a99)
{
  return va_MdsPut2(node, expression,
		    a00, a01, a02, a03, a04, a05, a06, a07, a08, a09,
		    a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
		    a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
		    a30, a31, a32, a33, a34, a35, a36, a37, a38, a39,
		    a40, a41, a42, a43, a44, a45, a46, a47, a48, a49,
		    a50, a51, a52, a53, a54, a55, a56, a57, a58, a59,
		    a60, a61, a62, a63, a64, a65, a66, a67, a68, a69,
		    a70, a71, a72, a73, a74, a75, a76, a77, a78, a79,
		    a80, a81, a82, a83, a84, a85, a86, a87, a88, a89,
		    a90, a91, a92, a93, a94, a95, a96, a97, a98, a99);
}

#endif

#ifndef FORTRAN_ENTRY_POINTS
#define FORTRAN_ENTRY_POINTS

#ifdef _WIN32
static int zero = 0;
static SOCKET ___MdsConnect();
static void ___MdsDisconnect();
static int ___MdsClose();
static SOCKET ___MdsSetSocket();
static int ___MdsSetDefault();
static int ___MdsOpen();
SOCKET WINAPI MdsConnectVB(char *host)
{
  return ___MdsConnect(host);
}

void WINAPI MdsDisconnectVB()
{
  ___MdsDisconnect();
}

int WINAPI MdsCloseVB(char *tree, int *shot)
{
  return ___MdsClose(tree, shot);
}

SOCKET WINAPI MdsSetSocketVB(SOCKET *newsocket)
{
  return ___MdsSetSocket(newsocket);
}

int WINAPI MdsSetDefaultVB(char *node)
{
  return ___MdsSetDefault(node);
}

int WINAPI MdsOpenVB(char *tree, int *shot)
{
  return ___MdsOpen(tree, shot);
}

int WINAPI descr1VB(int *dtype, void *value)
{
  if (*dtype == DTYPE_CSTRING) {
    int len = strlen(value);
    return va_descr(dtype, value, &zero, &len);
  } else
    return va_descr(dtype, value, &zero);
}

int WINAPI descr1VB2(int *dtype, int *len)
{
  return (va_descr2(dtype, &zero, len));
}

int WINAPI descr2VB(int *dtype, int *num, void *value)
{
  if (*dtype == DTYPE_CSTRING) {
    int len = strlen(value);
    return va_descr(dtype, value, num, &zero, &len);
  } else
    return va_descr(dtype, value, num, &zero);
}

int WINAPI descr2VB2(int *dtype, int *num, int *len)
{
  return (va_descr2(dtype, num, &zero, len));
}

int WINAPI descr3VB(int *dtype, int *n1, int *n2, void *value)
{
  if (*dtype == DTYPE_CSTRING) {
    int len = strlen(value);
    return va_descr(dtype, value, n1, n2, &zero, &len);
  } else
    return va_descr(dtype, value, n1, n2, &zero);
}

int WINAPI descr3VB2(int *dtype, int *n1, int *n2, int *len)
{
  return (va_descr2(dtype, n1, n2, &zero, len));
}

int WINAPI descr4VB(int *dtype, int *n1, int *n2, int *n3, void *value)
{
  if (*dtype == DTYPE_CSTRING) {
    int len = strlen(value);
    return va_descr(dtype, value, n1, n2, n3, &zero, &len);
  } else
    return va_descr(dtype, value, n1, n2, n3, &zero);
}

int WINAPI descr4VB2(int *dtype, int *n1, int *n2, int *n3, int *len)
{
  return (va_descr2(dtype, n1, n2, n3, &zero, len));
}

int WINAPI MdsValue1VB(char *expression, int *ansd, int *retlen)
{
  return va_MdsValue(expression, ansd, &zero, retlen);
}

int WINAPI MdsValue1VB2(char *expression, int *ansd, void *value, int *retlen)
{
  return va_MdsValue2(expression, ansd, value, &zero, retlen);
}

int WINAPI MdsValue2VB(char *expression, int *arg1d, int *ansd, int *retlen)
{
  return va_MdsValue(expression, arg1d, ansd, &zero, retlen);
}

int WINAPI MdsValue2VB2(char *expression, int *arg1d, void *arg1v, int *ansd, void *value,
			int *retlen)
{
  return va_MdsValue2(expression, arg1d, arg1v, ansd, value, &zero, retlen);
}

int WINAPI MdsValue3VB(char *expression, int *arg1d, int *arg2d, int *ansd, int *retlen)
{
  return va_MdsValue(expression, arg1d, arg2d, ansd, &zero, retlen);
}

int WINAPI MdsValue3VB2(char *expression, int *arg1d, void *arg1v, int *arg2d, void *arg2v,
			int *ansd, void *value, int *retlen)
{
  return va_MdsValue2(expression, arg1d, arg1v, arg2d, arg2v, ansd, value, &zero, retlen);
}

int WINAPI MdsPut1VB(char *node, char *expression, int *ansd)
{
  return va_MdsPut(node, expression, ansd, &zero);
}

int WINAPI MdsPut1VB2(char *node, char *expression, int *ansd, void *value)
{
  return va_MdsPut2(node, expression, ansd, value, &zero);
}

int WINAPI MdsPut2VB(char *node, char *expression, int *arg1d, int *ansd)
{
  return va_MdsPut(node, expression, arg1d, ansd, &zero);
}

int WINAPI MdsPut2VB2(char *node, char *expression, int *arg1d, void *arg1v, int *ansd, void *value)
{
  return va_MdsPut2(node, expression, arg1d, arg1v, ansd, value, &zero);
}

int WINAPI MdsPut3VB(char *node, char *expression, int *arg1d, int *arg2d, int *ansd)
{
  return va_MdsPut(node, expression, arg1d, arg2d, ansd, &zero);
}

int WINAPI MdsPut3VB2(char *node, char *expression, int *arg1d, void *arg1v, int *arg2d,
		      void *arg2v, int *ansd, void *value)
{
  return va_MdsPut2(node, expression, arg1d, arg1v, arg2d, arg2v, ansd, value, &zero);
}
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

#if defined(__hpux) || defined(__osf__) || defined(__sgi) || defined(__sun) || defined(__linux) || defined(__APPLE__) || defined (_WIN32)
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
int MdsValue();
static SOCKET ___MdsConnect(char *host);
int descr();
static int ___MdsClose(char *tree, int *shot);
static void ___MdsDisconnect();
static int ___MdsOpen(char *tree, int *shot);
static int ___MdsSetDefault(char *node);
static SOCKET ___MdsSetSocket(SOCKET *newsocket);

#ifdef FortranMdsConnect
extern EXPORT SOCKET FortranMdsConnect(char *host)
{
  return ___MdsConnect(host);
}
#endif

#ifdef FortranMdsDisconnect
extern EXPORT void FortranMdsDisconnect()
{
  ___MdsDisconnect();
}
#endif

#ifdef FortranMdsSetSocket
extern EXPORT int FortranMdsSetSocket(int *newsocket)
{
  return (int)___MdsSetSocket((SOCKET*)newsocket);
}
#endif

#ifdef FortranMdsClose
extern EXPORT int FortranMdsClose(char *tree, int *shot)
{
  return ___MdsClose(tree, shot);
}
#endif

#ifdef FortranMdsSetDefault
extern EXPORT int FortranMdsSetDefault(char *node)
{
  return ___MdsSetDefault(node);
}
#endif

#ifdef FortranMdsOpen
extern EXPORT int FortranMdsOpen(char *tree, int *shot)
{
  return ___MdsOpen(tree, shot);
}
#endif

// #include __FILE__
#define _STRINGIZE(s) __STRINGIZE(s)
#define __STRINGIZE(s) #s
#include _STRINGIZE(FILENAME)

#endif
