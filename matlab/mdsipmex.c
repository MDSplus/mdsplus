/* Version 2.2000.05.1
/* CREATE_MEX_FILE for making this mex and HELPER_FILES below to use mex, BPD, May 2000 */
#pragma comment(lib, "Wsock32.lib")
#ifdef MAKE_DISTRIBUTION
tar - T mdsipmex.T - czf mdsipmex.tar.gz
    mkdir MDSIPMEXTEMP
    cd MDSIPMEXTEMP tar xzf.. / mdsipmex.tar.gz zip - r.. / mdsipmex.zip * cd..rm - rf MDSIPMEXTEMP
#endif
#ifdef CREATE_MEX_FILE
/* make_mdsipmex.m */
    % make_mdsipmex.m
    % make sure.. / mdsplus points to current version of mdsplus distribution
    % to make the debug version, debug = 1;
make_mdsipmex;
%to make the shared version, shared = 1;
make_mdsipmex;
%Note;
if shared
  , install MdsIpShr library or point $LD_LIBRARY_PATH to shareables
      % Basil P.DUVAL, May 2000 MDSPLUS = getenv('MDSPLUS');
if (length(MDSPLUS) == 0)
  disp('shell variable MDSPLUS must point to MDSPLUS distribution before compilation');
end if (exist('debug', 'var')) ;
DEBUG = '-DDEBUG';
else;
DEBUG = '';
end if (~strcmp(computer, 'VMS')) ;
PASSWD = '-DPASSWD';
else;
PASSWD = 'mdsipmex.opt';
end if (~exist('shared', 'var'))
  comm =
      sprintf
      ('mex %s %s mdsipmex.c %s/mdstcpip/mdsipshr.c %s/mdstcpip/mdsiputil.c -I%s/include -I%s/mdstcpip',
       ... DEBUG, PASSWD, MDSPLUS, MDSPLUS, MDSPLUS, MDSPLUS);
else
  comm = sprintf('mex %s %s mdsipmex.c -I%s/include -I%s/mdstcpip -L%s/lib -lMdsIpShr', ...
		 DEBUG, PASSWD, MDSPLUS, MDSPLUS, MDSPLUS);
end disp(comm);
eval(comm);
#endif

#define VERSION "2.2000.05.1"
#ifndef int32
#define int32 int
#endif
/* In this version, calls to mdsipmex is replaced */
/* Example from /usr/local/matlab_real51/extern/examples/mex/mexmakearraypersistent.c
* of how to get persistent memory */
#ifdef VMS
#define MatAlloc(a,b) mxCalloc((a),(b))
/* Matlab5 compiles "as_is" and the uppercase library routines are not found */
#define MdsClose MDSCLOSE
#define MdsOpen MDSOPEN
#define MakeDescrip MAKEDESCRIP
#define MdsValue MDSVALUE

#define DisconnectFromMds DISCONNECTFROMMDS
#define ConnectToMds CONNECTTOMDS
#define SendArg SENDARG
#define GetAnswerInfo GETANSWERINFO
/* Account for differences in V4 FROM V5 */
#ifdef V4
/* local definitions JMM */
typedef struct {
  unsigned long w1;
  unsigned long w2;
} quadword;
#define CONST
typedef char mxChar;
#ifdef HOW_TO_COMPILE
 cc / g_float / inc = matlab: [extern.include]/defi = V4 mdsipmex.c my_cmexg:== @my_cmexg.com
    my_cmexg mdsipmex sock serv
    servip
#endif
/* changes from V4 -> V5 that can be handled with defines */
#define mxArray Matrix
#define mxREAL REAL
#define mxCOMPLEX COMPLEX
#define mxIsChar mxIsString
#define mxCreateDoubleMatrix mxCreateFull
#define USE_FUNCTION_PROTOTYPES
#else				/* else of V4 */
/* V5 defines caused by V4 compatibility */
#define CONST const
#ifdef HOW_TO_COMPILE
mex - v mdsipmex.opt mdsipmex.c mex - DDEBUG - v mdsipmex.opt mdsipmex.c
/* mdsipmex.opt */
 sys$library: mdsipshr_IEEE.exe / share multinet:MULTINET_SOCKET_LIBRARY.EXE / share
    psect_attr = sock, noshr
    psect_attr = serv, noshr psect_attr = servip, noshr
#endif
#endif				/* end of V4 */
/* Default values when not using VMS */
#else				/* Else of VMS */
#define MatAlloc(a,b) mxMalloc((a)*(b))
#define CONST const
#endif				/* End of VMS */

#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

#include "mex.h"
#include <string.h>
#include <mdstypes.h>
/* Include for mds descriptor definition */
#include "ipdesc.h"

#ifndef DEFAULTSERVER
#define DEFAULTSERVER "eltca1.epfl.ch"
#endif
#define SERVERTEST    "::"
#define PORTTEST      ":"
#define STRLEN 128

static void Bmdsopen(int nlhs, mxArray * plhs[], int nrhs, CONST mxArray * prhs[]);
static void Bmdsclose(int nlhs, mxArray * plhs[], int nrhs, CONST mxArray * prhs[]);
static void Bmdsvalue(int nlhs, mxArray * plhs[], int nrhs, CONST mxArray * prhs[]);
static void Bmdsdisconnect(int nlhs, mxArray * plhs[], int nrhs, CONST mxArray * prhs[]);
static void Bmdsput(int nlhs, mxArray * plhs[], int nrhs, CONST mxArray * prhs[]);

SOCKET sock = -1;		/* Mark the socket as unopen */
char serv[STRLEN];		/* Current server */
#define NSERVERIP 6
unsigned char servip[NSERVERIP];
/* Exit Function When the MEX-file is cleared */
void ExitFcn(void)
{
/*  mxDestroyArray(array_ptr);*/
  if (sock != -1) {
    printf("Mdsremote: ExitFcn closing remote server [%s]\n", serv);
    Bmdsdisconnect(0, NULL, 0, NULL);
  } else
    printf("Mdsremote: ExitFcn closing; Nothing to close \n");
}

/* Matlab Function ================================================================ */
void mexFunction(int nlhs, mxArray * plhs[], int nrhs, CONST mxArray * prhs[])
{
  int opt = mxIsChar(prhs[0]) ? 2 : (int)*mxGetPr(prhs[0]);	/* mode */

  switch (opt) {
  case 1:
    Bmdsopen(nlhs, plhs, nrhs, prhs);
    break;
  case 2:
    Bmdsvalue(nlhs, plhs, nrhs, prhs);
    break;
  case 3:
    Bmdsclose(nlhs, plhs, nrhs, prhs);
    break;
  case 4:
    Bmdsdisconnect(nlhs, plhs, nrhs, prhs);
    break;
  case 5:
    Bmdsput(nlhs, plhs, nrhs, prhs);
    break;
  case 6:
    if (nlhs > 0)
      plhs[0] = mxCreateString(VERSION);	/* create string */
    return;
  default:
    break;
  }
}

/*            GetMatReply in suboutine form */
void GetMatReply(int nlhs, mxArray * plhs[])
{
  short vlen;
  int32 numbytes;
  void *vptr;

  int len, i, j;
  struct descrip ans;
  char *cptr;
  unsigned char *ucptr;
  short *sptr;
  unsigned short *usptr;
  int *iptr;
  unsigned int *uiptr;
#ifdef DTYPE_LONGLONG
  int64_t *qptr;
  uint64_t *uqptr;
#endif
  float *fptr;
  double *dptr, *dpout;
  mxChar *mxcP;
  mxArray *mxaP;
  int matdum[MAX_DIMS + 1];
  int status;			/* check status of calls to mdsipshr.exe */

#ifdef DEBUG
  printf("before mdsipmex Call");
#endif
  status = GetAnswerInfo(sock, &ans.dtype, &vlen, &ans.ndims, ans.dims, &numbytes, &vptr);
  ans.length = vlen;
  ans.ptr = numbytes ? vptr : NULL;
#ifdef DEBUG
  printf("... and return\nreply with dtype [%d],ndims[%d] dims[%d,%d][%0lx]\n",
	 ans.dtype, ans.ndims, ans.dims[0], ans.dims[1], ans.ptr);
#endif
/* treat the reply */
  if (ans.ndims && ans.ptr) {	/* get number of elements, assumes ans.ptr exists */
    for (i = ans.ndims, len = 1; i--;)
      len *= ans.dims[i];
  } else {
    if (ans.ptr)
      len = ans.ndims = ans.dims[0] = 1;	/* make it with one dimension with length of 1 */
    else
      len = 0;
  }
/* Matlab ERROR: if ndims ==1 fails... FIX: pretend ndims=2 , bpd, xsl, Jan 98*/
  if (ans.ndims == 1) {
    ans.dims[1] = 1;
    ans.ndims++;
  }
#ifdef DEBUG
  printf("Len comes with [%d]\n", len);
#endif
/* types */
  if (ans.dtype == DTYPE_CSTRING) {
#ifdef DEBUG
    printf("string ndims %d, length %d,%d -> %d,%d\n", ans.ndims, ans.length, len, ans.dims[0],
	   ans.dims[1]);
#endif
#ifdef V4
    len *= ans.length;
    strptr = (char *)MatAlloc(len + 1, sizeof(char));
    strptr[len] = '\0';		/* terminate string */
    memcpy(strptr, ans.ptr, len);	/* copy all data across */
    plhs[0] = mxCreateString((char *)ans.ptr);	/* create string */
    mxFree((void *)strptr);
#else
    if (len == 0) {
      matdum[1] = matdum[0] = 0;
      plhs[0] = mxCreateCharArray(2, matdum);	/* create a cell */
/*       plhs[0] = mxCreateDoubleMatrix(0,0,mxREAL);*/
    } else {
      if (len > 1)
	plhs[0] = mxCreateCellArray(ans.ndims, ans.dims);	/* create char cell array */
      cptr = ans.ptr;
      ans.ndims = 2;
      matdum[0] = 1;
      matdum[1] = ans.length;
      for (i = 0; i < len; i++) {
	mxaP = mxCreateCharArray(ans.ndims, matdum);	/* create a cell */
	mxcP = (mxChar *) mxGetPr(mxaP);	/* pointer to its data */
	for (j = 0; j < ans.length; j++)	/* copy data across */
	  *mxcP++ = *cptr++;
	if (len > 1)
	  mxSetCell(plhs[0], i, mxaP);	/* set cell */
	else
	  plhs[0] = mxaP;	/* single string returned */
      }
    }				/* string or cell array termination */
#endif
  } else {			/* Not DTYPE_CSTRING */
/* output vector */
#ifdef DEBUG
    printf("array [%d],%d [%d,%d]\n", ans.ndims, len, ans.dims[0], ans.dims[1]);
#endif
#ifdef V4
    if (ans.dims[0])		/* Incase dims are zero */
      plhs[0] = mxCreateDoubleMatrix(ans.dims[0], len / ans.dims[0],
				     ans.dtype == DTYPE_COMPLEX ? mxCOMPLEX : mxREAL);
    else
      plhs[0] = mxCreateDoubleMatrix(0, 0, ans.dtype == DTYPE_COMPLEX ? mxCOMPLEX : mxREAL);
#else
    plhs[0] = mxCreateNumericArray(ans.ndims, ans.dims, mxDOUBLE_CLASS,
				   ans.dtype == DTYPE_COMPLEX ? mxCOMPLEX : mxREAL);
#endif
    dpout = mxGetPr(plhs[0]);	/* pointer into matlab array */
    i = len;
    switch (ans.dtype) {
    case DTYPE_UCHAR:
      ucptr = (unsigned char *)ans.ptr;
      while (len--)
	*dpout++ = *ucptr++;
      break;
    case DTYPE_CHAR:
      cptr = (char *)ans.ptr;
      while (len--)
	*dpout++ = *cptr++;
      break;
    case DTYPE_USHORT:
      usptr = (unsigned short *)ans.ptr;
      while (len--)
	*dpout++ = *usptr++;
      break;
    case DTYPE_SHORT:
      sptr = (short *)ans.ptr;
      while (len--)
	*dpout++ = *sptr++;
      break;
    case DTYPE_ULONG:
      uiptr = (unsigned int *)ans.ptr;
      while (len--)
	*dpout++ = *uiptr++;
      break;
    case DTYPE_LONG:
      iptr = (int *)ans.ptr;
      while (len--)
	*dpout++ = *iptr++;
      break;
#ifdef DTYPE_LONGLONG
    case DTYPE_ULONGLONG:
      uqptr = (uint64_t *) ans.ptr;
      while (len--)
	*dpout++ = (double)*uqptr++;
      break;
    case DTYPE_LONGLONG:
      qptr = (int64_t *) ans.ptr;
      while (len--)
	*dpout++ = (double)*qptr++;
      break;
#endif
    case DTYPE_FLOAT:
      fptr = (float *)ans.ptr;
      while (len--)
	*dpout++ = *fptr++;
      break;
    case DTYPE_DOUBLE:
    case 27:
      dptr = (double *)ans.ptr;
      while (len--)
	*dpout++ = *dptr++;
      break;
    case DTYPE_COMPLEX:
      dptr = (double *)ans.ptr;
      while (len--) {
	*dpout++ = *dptr++;
	*dpout++ = *dptr++;

      }
      break;
    default:
      break;
    }				/* Switch on ans.dtype */
/* make ROPRANDS into NaN, JMM circa 1993 */
#ifdef V4
    for (dpout = mxGetPr(plhs[0]); i--; dpout++)
      if (((quadword *) (dpout))->w1 == 0x8000l && ((quadword *) dpout)->w2 == 0x0l) {
	((quadword *) dpout)->w1 = 0xd37affeb;
	((quadword *) dpout)->w2 = 0xe9bd6f4d;
      }
#endif
  }				/* Not a DTYPE_CSTRING */
/* return the status if requested */
  if (nlhs > 1) {
    plhs[1] = mxCreateDoubleMatrix(1, 1, mxREAL);
    *mxGetPr(plhs[1]) = (double)status;
  }
}

#ifdef NEVER
/* conversion from NaN (Matlab4) to ROPRAND, to be used in mdsput ? */
if (((quadword *) dpout)->w1 == 0xd37affeb && ((quadword *) dpout)->w2 == 0xe9bd6f4d) {
  ((quadword *) dpout)->w1 = 0x8000l;
  ((quadword *) dpout)->w2 = 0x0l;
#endif
/*            SendMatValue in suboutine form */
#ifdef V4
  void SendMatValue(CONST mxArray * prhs[], int i, int nrhs, int opt, int conv) {
#else
  void SendMatValue(CONST mxArray * prhs[], int i, int nrhs, int opt) {
#endif
    mxArray *outAr[1], *inAr[2];
    int llen, len, j, k, lmax;
    char *cptr, *strptr, *s2trptr;
    unsigned char *ucptr;
    short *sptr;
    unsigned short *usptr;
    int *iptr;
    unsigned int *uiptr;
#ifdef DTYPE_LONGLONG
    int64_t *qptr;
    uint64_t *uqptr;
#endif
    float *fptr;
    double *dptr;
    int matdum[MAX_DIMS + 1];
    int ndim, *ndimP;
    int status = 1;		/* check status of calls to mdsipshr.exe */
    int alloc;
    mxArray *Tarray;		/* Get data from class */
#ifdef V4
    Tarray = (mxArray *) prhs[i];	/* copy array pointer with CONST cast */
    ndim = 2;
    len = mxGetM(Tarray) * mxGetN(Tarray);
#else
    int conv = 0;
#define CLEN 8
    char convS[CLEN];		/* conversion string */
/* Before you can look at the matrix dimensions, you have to see if it is a class */
/* use the class mdscvt to find if there is a conversion */
    if (mxIsClass(prhs[i], "mdscvt")) {
#ifdef DEBUG
      printf("Was class class mdscvt idx [%d]\n", i);
#endif
      conv = DTYPE_DOUBLE;	/* default was a double */
      Tarray = mxGetField(prhs[i], 0, "t");	/* type element */
      if (Tarray == NULL)
	mexErrMsgTxt("mdsipmex: Conversion type in class mdscvt not given");
      else {
	mxGetString(Tarray, convS, CLEN);
#ifdef DEBUG
	printf("Got value '%s'\n", convS);
#endif
	switch (*convS) {
	case 'd':
	case 'f':
	  conv = DTYPE_FLOAT;
	  break;
	case 'x':
	  conv = DTYPE_DOUBLE;
	  break;
	case 'l':
	  conv = DTYPE_LONG;
	  break;
	case 's':
	  conv = DTYPE_SHORT;
	  break;
	case 'c':
	  conv = DTYPE_CHAR;
	  break;
	case 'L':
	  conv = DTYPE_ULONG;
	  break;
	case 'S':
	  conv = DTYPE_USHORT;
	  break;
	case 'C':
	  conv = DTYPE_UCHAR;
	  break;
#ifdef DTYPE_LONGLONG
	case 'q':
	  conv = DTYPE_LONGLONG;
	  break;
	case 'Q':
	  conv = DTYPE_ULONGLONG;
	  break;
#else
	case 'q':
	case 'Q':
	  mexErrMsgTxt("mdsipmex: Quadword not yet supported");
	  break;
#endif
	default:
	  mexErrMsgTxt("mdsipmex: Conversion in mdscvt class unknown");
	}
      }
      Tarray = mxGetField(prhs[i], 0, "x");
      if (Tarray == NULL)
	mexErrMsgTxt("mdsipmex: No data in mdscvt class");
    } else
      Tarray = (mxArray *) prhs[i];	/* copy array pointer with CONST cast */
    ndimP = (int *)mxGetDimensions(Tarray);
    ndim = mxGetNumberOfDimensions(Tarray);
/* Calculate number of elements */
    for (j = ndim, len = 1; j--;)
      len *= ndimP[j];
#endif				/* V4 */

#ifdef DEBUG
#ifdef V4
    printf("V4 [%d] replacing type [%d]\n", i, mxIsChar(Tarray));
#else
    printf("[%d] replacing type [%d], ndims [%d]", i, mxIsChar(Tarray), ndim);
    if (ndim > 1)
      printf("%d,%d", ndimP[0], ndimP[1]);
    printf("\n");
#endif
#endif				/* of DEBUG */
/* Check type of variable from MATLAB */
    if (mxIsNumeric(Tarray)
#ifdef V4
	&& !mxIsChar(Tarray)
#endif
	) {
/* Check if scalar */
      if (len == 1) {
#ifdef DEBUG
	printf("converting scalar %d,%d\n", mxGetM(Tarray), mxGetN(Tarray));
#endif
	ndim = 0;
	ndimP = matdum;
      } else {			/* Treat Scalar */
#ifdef V4
	ndim = 2;
	matdum[0] = mxGetM(Tarray);
	matdum[1] = mxGetN(Tarray);
	ndimP = matdum;
/* Special case to reduce a matrix (3,1) or (1,3) to a vector */
	if (MIN(ndimP[0], ndimP[1]) == 1) {
	  matdum[0] = MAX(ndimP[0], ndimP[1]);
	  matdum[1] = 0;
	  ndim--;
	}
#else
/* Special case to reduce a matrix (3,1) or (1,3) to a vector */
	if (ndim == 2)
	  if (MIN(ndimP[0], ndimP[1]) == 1) {
	    matdum[0] = MAX(ndimP[0], ndimP[1]);
	    matdum[1] = 0;
	    ndimP = matdum;
	    ndim--;
	  }
#endif				/* end of V4 */
      }				/* End Treat Scalar */
#ifdef DEBUG
      printf("ndim %d, point %lx", ndim, ndimP);
      if (ndim > 1)
	printf("  vals %d %d", ndimP[0], ndimP[1]);
      printf("\n");
#endif

/* Find out what type the variable has */
#ifdef V4
#else				/* get class and data type from matlab */
      if (conv) {
/* if not a normal matlab variable and conv was specified, signal alarm */
	if (!mxIsClass(Tarray, "double"))
	  mexErrMsgTxt("mdsipmex: mdscvt specified but data was not double precision");
      } else if (mxIsClass(Tarray, "double")) {
	conv = -DTYPE_DOUBLE;
#ifdef DEBUG
	printf("Got type 'double'\n");
#endif
      } else if (mxIsClass(Tarray, "char")) {
	conv = -DTYPE_CHAR;
#ifdef DEBUG
	printf("Got type 'char'\n");
#endif
      } else if (mxIsClass(Tarray, "int8")) {
#ifdef DEBUG
	printf("Got type 'int8'\n");
#endif
	conv = -DTYPE_CHAR;
      } else if (mxIsClass(Tarray, "uint8")) {
	conv = -DTYPE_UCHAR;
#ifdef DEBUG
	printf("Got type 'uint8'\n");
#endif
      } else if (mxIsClass(Tarray, "int16")) {
	conv = -DTYPE_SHORT;
#ifdef DEBUG
	printf("Got type 'int16'\n");
#endif
      } else if (mxIsClass(Tarray, "uint16")) {
#ifdef DEBUG
	printf("Got type 'uint16'\n");
#endif
	conv = -DTYPE_USHORT;
      } else if (mxIsClass(Tarray, "int32")) {
#ifdef DEBUG
	printf("Got type 'int32'\n");
#endif
	conv = -DTYPE_LONG;
      } else if (mxIsClass(Tarray, "uint32")) {
#ifdef DEBUG
	printf("Got type 'uint32'\n");
#endif
	conv = -DTYPE_ULONG;
      } else
	mexErrMsgTxt("mdsipmex: Can not handle this matlab data type, YET!");
#endif				/* V4 mdscvt class */
      dptr = mxGetPr(Tarray);	/* pointer to original data */
      strptr = (char *)dptr;	/* default output value */
      alloc = 0;
      llen = len;
      switch (abs(conv)) {
      case DTYPE_UCHAR:
	alloc = sizeof(unsigned char);
	if (conv < 0)
	  break;
	ucptr = (unsigned char *)MatAlloc(len, alloc);
	strptr = (char *)ucptr;
	while (len--)
	  *ucptr++ = (unsigned char)*dptr++;
	break;
      case DTYPE_CHAR:
	alloc = sizeof(char);
	if (conv < 0)
	  break;
	cptr = (char *)MatAlloc(len, alloc);
	strptr = (char *)cptr;
	while (len--)
	  *cptr++ = (char)*dptr++;
	break;
      case DTYPE_USHORT:
	alloc = sizeof(unsigned short);
	if (conv < 0)
	  break;
	usptr = (unsigned short *)MatAlloc(len, alloc);
	strptr = (char *)usptr;
	while (len--)
	  *usptr++ = (unsigned short)*dptr++;
	break;
      case DTYPE_SHORT:
	alloc = sizeof(short);
	if (conv < 0)
	  break;
	sptr = (short *)MatAlloc(len, alloc);
	strptr = (char *)sptr;
	while (len--)
	  *sptr++ = (short)*dptr++;
	break;
#ifdef DTYPE_LONGLONG
      case DTYPE_ULONGLONG:
	alloc = sizeof(uint64_t);
	if (conv < 0)
	  break;
	uqptr = (uint64_t *) MatAlloc(len, alloc);
	strptr = (char *)uqptr;
	while (len--)
	  *uqptr++ = (uint64_t) * dptr++;
	break;
      case DTYPE_LONGLONG:
	alloc = sizeof(int64_t);
	if (conv < 0)
	  break;
	qptr = (int64_t *) MatAlloc(len, alloc);
	strptr = (char *)qptr;
	while (len--)
	  *qptr++ = (int64_t) * dptr++;
	break;
#endif
      case DTYPE_ULONG:
	alloc = sizeof(unsigned int);
	if (conv < 0)
	  break;
	uiptr = (unsigned int *)MatAlloc(len, alloc);
	strptr = (char *)uiptr;
	while (len--)
	  *uiptr++ = (unsigned int)*dptr++;
	break;
      case DTYPE_LONG:
	alloc = sizeof(int);
	if (conv < 0)
	  break;
	iptr = (int *)MatAlloc(len, alloc);
	strptr = (char *)iptr;
	while (len--)
	  *iptr++ = (int)*dptr++;
	break;
      case DTYPE_FLOAT:
	alloc = sizeof(float);
	if (conv < 0)
	  break;
	fptr = (float *)MatAlloc(len, alloc);
	strptr = (char *)fptr;
	while (len--)
	  *fptr++ = (float)*dptr++;
	break;
      case DTYPE_DOUBLE:
	alloc = sizeof(double);
	if (conv < 0)
	  break;
	strptr = (char *)dptr;
/* assume that this does not need conversion */
	break;
      default:
	alloc = sizeof(double);
	conv = DTYPE_DOUBLE;
	break;
      }
#ifdef DEBUG
      {
	int ii;
	for (ii = 0; ii < (llen * alloc); ii++)
	  printf("Byte(%d) = 0x%x\n", ii, strptr[ii]);
      }
#endif
      status = SendArg(sock, (unsigned char)(i - opt), (char)abs(conv), (char)(nrhs - opt)
		       , (short)alloc, (char)ndim, ndimP, strptr);
/* check to see if it points to the original data */
      if (dptr != mxGetPr(Tarray))
	mxFree((void *)strptr);	/* release the memory */

/*      status = SendArg(sock, (unsigned char)(i-opt), DTYPE_DOUBLE, (unsigned char)(nrhs-opt)
		       ,(short)sizeof(double), (char)ndim, ndimP, (char *)mxGetPr(Tarray));*/
    }
    /* End Treat numbers */
    /* text data */
    else if (mxIsChar(Tarray)
#ifdef V4
#else
	     || mxIsCell(Tarray)
#endif
	) {
      if (mxIsChar(Tarray)) {
#ifdef V4
	len = mxGetM(Tarray);	/* number of elements */
	lmax = mxGetN(Tarray);	/* length of each element */
	if (len == 1)
	  ndim = 0;
	else {
	  ndim = 1;		/* array of strings */
	  matdum[0] = len;	/* number of strings */
	  matdum[MAX_DIMS] = lmax;	/* SPECIAL.... copy w_length into last element */
	}
	strptr = (char *)MatAlloc((lmax * len + 1), sizeof(char));
	strptr[lmax * len] = '\0';
	mxGetString(Tarray, strptr, lmax * len + 1);	/* Get the data */
#else
	ndim--;			/* remove second array */
	lmax = ndimP[1];	/* length of each element */
	*matdum = *ndimP;	/* first element gives size */
	for (j = 1; j < ndim; j++)
	  matdum[j] = ndimP[j + 1];	/* copy the other dimensions */
	s2trptr = strptr = (char *)MatAlloc((len + 1), sizeof(char));
#ifdef TRY1
	c2ptr = cptr = (char *)MatAlloc((len + 1), sizeof(char));
	mxGetString(Tarray, cptr, len + 1);	/* Get the data */
	for (k = 0; k < len / lmax; k++)
	  for (j = 0, c2ptr = cptr + k; j < lmax; j++, c2ptr += len / lmax)
	    *s2trptr++ = *c2ptr;
/*	 printf("Enter with [%s]\n",cptr);
	 printf("Leave with [%s]\n",strptr);*/
	mxFree((void *)cptr);	/* release the memory */
#else				/* TRY1 */
#ifndef TRY2
/* use Matlab To permutate */
	inAr[0] = (mxArray *) Tarray;	/* the data (cast eliminates compile error) */
	inAr[1] = mxCreateDoubleMatrix(ndim + 1, 1, mxREAL);	/* for the dimensions */
	dptr = mxGetPr(inAr[1]);	/* pointer to data */
/*	 printf("Ndim [%d] [%d] [%d] [%d]\n",ndim,ndimP[0],ndimP[1],ndimP[2]);*/
	*dptr++ = 2;		/* last index */
	for (j = 0; j <= ndim; j++) {
	  *dptr++ = j + 1;
	  if (!j)
	    j++;		/* skip the 2 */
	}
	mexCallMATLAB(1, outAr, 2, inAr, "permute");
	mxGetString(outAr[0], strptr, len + 1);	/* Get the data */
/* clear the matricies */
	mxDestroyArray(outAr[0]);
	mxDestroyArray(inAr[1]);
#else				/* TRY2 */
	ndim = 0;
	lmax = len;
	mxGetString(Tarray, strptr, len + 1);	/* Get the data */
#endif				/* TRY2 */
#endif				/* TRY1 */
#endif				/* V4 */
      }
#ifdef V4
#else
      else if (mxIsCell(Tarray)) {
	lmax = 0;		/* find max length of a cell */
	for (j = len; j--;) {
	  if (!mxIsChar(mxGetCell(Tarray, j)) || mxGetM(mxGetCell(Tarray, j)) > 1)
	    mexErrMsgTxt("mdsipmex: Cell array can only contain 1D character Arrays");
	  k = mxGetN(mxGetCell(Tarray, j));
	  if (k > lmax)
	    lmax = k;
	}
/* Allow room for all data and extra space for 0 at the end */
	cptr = strptr = (char *)MatAlloc(lmax * len + 1, sizeof(char));
	memset(strptr, ' ', lmax * len);	/* fill with spaces */
	for (j = 0; j < len; j++) {
	  mxGetString(mxGetCell(Tarray, j), cptr, lmax + 1);
/* put back the spaces */
	  cptr[strlen(cptr)] = ' ';
	  cptr += lmax;		/* point to next string */
	}
	for (j = 0; j < ndim; j++)
	  matdum[j] = ndimP[j];	/* copy dimensions of matrix */
	matdum[MAX_DIMS] = lmax;	/* SPECIAL.... copy w_length into last element */
      }
      /* End of CELL data */
      /* Remove a second dimension if 1 to reduce rank in remote server */
      if (ndim == 2 && matdum[ndim - 1] == 1)
	ndim--;
#endif				/* end V4 */
#ifdef DEBUG
      printf("Send String %d,%d,%d,%d,%d,%d,%d,[%s]\n", i - opt, nrhs - opt, DTYPE_CSTRING, lmax,
	     ndim, matdum[0], matdum[1], strptr);
#endif
      status = SendArg(sock, (unsigned char)(i - opt), DTYPE_CSTRING, (unsigned char)(nrhs - opt)
		       , (short)lmax, (char)ndim, matdum, strptr);
      mxFree((void *)strptr);	/* release the memory */
    }				/* end of text data */
#ifdef V4
#else
    else if (mxIsComplex(Tarray))
      mexErrMsgTxt("mdsipmex: Imaginary numbers not wanted");
#endif
    else
      mexErrMsgTxt("mdsipmex: Untreated data type encountered");
  }
/* mdsput ======================================================================*/
  static void Bmdsput(int nlhs, mxArray * plhs[], int nrhs, CONST mxArray * prhs[]) {
    int i;
    int status = 1;		/* check status of calls to mdsipshr.exe */
    int conv = 0;

/* Open server connection if not available */
    if (sock == -1)
      Bmdsopen(0, NULL, 0, NULL);
/*     mexErrMsgTxt("mdsipmex: Connection to MDS server not open");*/

/* Check is string */
    if (!mxIsChar(prhs[1]))
      mexErrMsgTxt("mdsput: Second argument must be a string");
#ifdef V4			/* Get conversion from 3rd element */
    if (!mxIsChar(prhs[2]))
      mexErrMsgTxt("mdsipmex: Third argument must be a string");
    mxGetString(prhs[1], temp, STRLEN);	/* get conversion string */
/* find conversion */
    if (!strcmp(temp, "DTYPE_USHORT"))
      conv = DTYPE_USHORT;
    else if (!strcmp(temp, "DTYPE_ULONG"))
      conv = DTYPE_ULONG;
    else if (!strcmp(temp, "DTYPE_CHAR"))
      conv = DTYPE_CHAR;
    else if (!strcmp(temp, "DTYPE_SHORT"))
      conv = DTYPE_SHORT;
    else if (!strcmp(temp, "DTYPE_LONG"))
      conv = DTYPE_LONG;
    else if (!strcmp(temp, "DTYPE_FLOAT"))
      conv = DTYPE_FLOAT;
/*   else if(!strcmp(temp,"DTYPE_DOUBLE"))  conv = DTYPE_DOUBLE;*/
    else if (!strcmp(temp, "DTYPE_DOUBLE"))
      conv = 0;			/* default from matlab */
    else
      mexErrMsgTxt("mdsput: Conversion request not found");
#endif				/* V4 */

/* Cycle through the commands one at a time */
    for (i = 1; i < nrhs; i++) {
#ifdef V4
      SendMatValue(prhs, i, nrhs, 2, conv);
#else
      SendMatValue(prhs, i, nrhs, 2);
#endif				/* V4 */
    }				/* end of input data */
    GetMatReply(nlhs, plhs);
  }
/* mdsvalue ======================================================================*/
  static void Bmdsvalue(int nlhs, mxArray * plhs[], int nrhs, CONST mxArray * prhs[]) {
    int i;
    int status = 1;		/* check status of calls to mdsipshr.exe */
    int opt = mxIsChar(prhs[0]) ? 0 : 1;	/* Offset */

    if (sock == -1)
      Bmdsopen(0, NULL, 0, NULL);
/*     mexErrMsgTxt("mdsipmex: Connection to MDS server not open");*/

/* Check is string */
    if (!mxIsChar(prhs[opt]))
      mexErrMsgTxt("mdsipmex: First argument must be a string");

/* Cycle through the commands one at a time */
    for (i = opt; i < nrhs; i++) {
#ifdef V4
      SendMatValue(prhs, i, nrhs, opt, 0);
#else
      SendMatValue(prhs, i, nrhs, opt);
#endif
    }				/* end of input data */
    GetMatReply(nlhs, plhs);
  }

/* mdsopen and server connect */
#ifndef _WIN32
#include <netdb.h>
#include <sys/socket.h>
#endif
  static void Bmdsopen(int nlhs, mxArray * plhs[], int nrhs, CONST mxArray * prhs[]) {
    char *server = DEFAULTSERVER;
    char *cptr;
    char trees[STRLEN], temp[STRLEN];
    char *tree = trees;
    int32 shot, status;
    struct hostent *hent;

/* Open server connection if not available */
    if (prhs) {
/* check character string is sent */
      if (nrhs < 3)
	mexErrMsgTxt("mdsopen: Requires two parameters ie. mdsopen('tcv_shot',123)");

      if (mxIsChar(prhs[1])) {
	mxGetString(prhs[1], trees, STRLEN);
      }
/* No, this is not a string array. */
      else
	mexErrMsgTxt("mdsopen: must enter character string for tree");
/* Look for server in tree */
      if ((cptr = strstr(trees, SERVERTEST)) != NULL) {
	*cptr = '\0';		/* terminate the new server string */
	strcpy(temp, trees);	/* copy string into temp */
	server = temp;		/* point server to new string */
	tree = cptr + strlen(SERVERTEST);	/* skip over initial part of string */
/* check to see if a port was requested */
	if ((cptr = strstr(tree, PORTTEST)) != NULL) {
	  strcat(server, cptr);	/* add port to end */
	  *cptr = '\0';
	}
/* If changing server, close current connection */
	if (sock != -1 && strcmp(server, serv)) {
/* check if the server ip is however correct */
	  hent = gethostbyname(server);	/* get ip number for the connection */
	  if ((hent == NULL)
	      || strncmp(*hent->h_addr_list, (char *)servip, MIN(hent->h_length, NSERVERIP))) {
	    printf("mdsopen: WARNING, implicitly closing server [%s]\n", serv);
	    Bmdsdisconnect(0, NULL, 0, NULL);
	  } else {
#ifdef DEBUG
	    printf("mdsopen: Keep socket! name changed from  [%s] to [%s]\n", serv, server);
#endif
	    strcpy(serv, server);	/* copy new name to memory, keep socket open */
	  }
	}
#ifdef DEBUG
	printf("mdsopen: got server [%s] and tree [%s]\n", server, tree);
#endif
      }
    }
/* if connection not open, do so with best server name */
    if (sock == -1) {		/* Connection not open */
#ifdef DEBUG
      printf("Connecting to server [%s]\n", server);
#endif
      if ((sock = ConnectToMds(server)) == -1)
	mexErrMsgTxt("mdsopen: Could not open connection to MDS server");
      else {
	mexAtExit(&ExitFcn);	/* install exit function */
	strcpy(serv, server);	/* copy current server into string */
	hent = gethostbyname(server);	/* get ip number for the connection */
	if (hent != NULL) {
	  strncpy((char *)servip, *hent->h_addr_list, MIN(hent->h_length, NSERVERIP));	/* copy ip name to memory */
#ifdef DEBUG
	  printf("IP [%03d][%03d][%03d][%03d]  [%d]\n", servip[0], servip[1], servip[2], servip[3],
		 hent->h_length);
#endif
	}
      }
    } else
      server = serv;		/* point server name to old string */
#ifdef DEBUG
    printf("socket %lx\n");
#endif
/* Special to open the connection from mdsopen */
    if (plhs == NULL && prhs == NULL)
      return;
/* If string empty, or no shot given (ie string) do not perform mdsopen */
    if (strlen(tree) && !mxIsChar(prhs[2])) {
/* Get Shot number */
      if ((mxGetM(prhs[2]) * mxGetN(prhs[2])) != 1)
	mexErrMsgTxt("mdsopen: shot number is a single number");
      else
	shot = (int)*mxGetPr(prhs[2]);	/* single number */
/* Open the shot and get status */
#ifdef DEBUG
      printf("On server [%s], Opening tree:[%s] shot:[%d]\n", server, tree, shot);
#endif
      status = MdsOpen(sock, tree, shot);
#ifdef DEBUG
      printf("mdsopen: status [%d]\n", status);
#endif
    } else
      status = sock;
/* output to calling code */
    if (nlhs > 0) {
      plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
      *mxGetPr(plhs[0]) = status;
    }
  }
/* mdsclose and server connect */
  static void Bmdsclose(int nlhs, mxArray * plhs[], int nrhs, CONST mxArray * prhs[]) {
    int status;

    if (sock != -1)
      status = MdsClose(sock);
    else {
      printf("mdsclose: warning, communication socket closed\n");
      status = 0;
    }

    if (nlhs > 0) {
      plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
      *mxGetPr(plhs[0]) = status;
    }
  }
/* mdsdisconnect ==================================================================== */
  static void Bmdsdisconnect(int nlhs, mxArray * plhs[], int nrhs, CONST mxArray * prhs[]) {
    int status;

    if (sock != -1) {
      status = DisconnectFromMds(sock);
      sock = -1;
    } else {
      printf("mdsdisconnect: warning, communication socket aleady closed\n");
      status = 0;
    }
    *serv = '\0';		/* Clear current server string */
/* copy status as reply */
    if (nlhs > 0) {
      plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
      *mxGetPr(plhs[0]) = status;
    }
  }
#ifdef HELPER_ROUTINES
/* mdsopen.m */
  function[shoto, status] = mdsopen(server, shot)
 % function to open remote server server % eg:mdsopen('tcv_shot', 12345);
  %defaults to mdsopen('eltca1::tcv_shot', shot);
  %To open a remote server only, use mdsopen(no arguments)
      % If first parameter is not a string, will open 'tcv_shot' tree
      % %Basil P.DUVAL, Oct 1998 if (nargin < 1) ;
  server = '';
  shot = 'none';
  elseif(nargin < 2 & ~isstr(server));
  shot = server;
  server = 'tcv_shot';
  elseif(nargin < 2 & isstr(server));
  shot = 'none';
  elseif(nargin > 2 | ~isstr(server) | isstr(shot));
  error('Incorrect arguments to mdsopen');
  end % open the shot status = mdsipmex(1, server, shot);

  %status = mdsipmex('MDSLIB->MDS$OPEN($,int($))', server, shot);
  %return the open shot number if successful
    if (rem(status, 2) == 1 & nargin > 1)
      shoto = mdsipmex(2, '$SHOT');
    else if (nargout > 0) ;
  shoto =[];
  end end
/* mdsvalue.m */
      function[reply, status] = mdsvalue(str, varargin)
      %[reply, status] = mdsvalue(str, varargin)
      % This is a dummy file to call mdsipmex.mex
##; calls the mex directly; rename mex if you want to use this function
 % eg:mdsvalue('_a=2+3');
 %to avoid a reply eg:mdsvalue('_a=1..1000;$MISSING');
 %If variables are NOT DOUBLE PRECISION, they are put appropriately eg: mdsvalue('_a=$1', uint8(1:10));
 %If variables ARE DOUBLE PRECISION, you may convert before sending(see mdsput) eg: mdsvalue('_a=$1', mdscvt(1:100, 'C'));
  %Adapted Basil P.DUVAL, Sept 1998[reply, status] = mdsipmex(str, varargin {
 :							      }
  );
/* mdsput.m */
  function status = mdsput(node, expr, varargin)

      % MDSPUT puts data in MDS data base % mdsput(node, d1)
      % node = 'target::node', d1 = data % mdsput(node, expr, d1, d2, d3 ...)
      % node = 'target::node', expr = 'sin($1)', d1, d2, d3 ... = data % if d1
    is expressed as class mdscvt, data is translated before sending
	% d1 = mdscvt(data, 'CONVERSION')
	% CONVERSION 'd' = 'G_FLOAT';
  'f' = 'F_FLOAT';
  'q' = 'QUADWORD';
  'l' = 'LONG';
  's' = 'WORD';
  'c' = 'BYTE';
  %'Q' = 'QUADWORD_UNSIGNED';
  'L' = 'LONG_UNSIGNED';
  'S' = 'WORD_UNSIGNED';
  'C' = 'BYTE_UNSIGNED';
  't' = 'TEXT';
  % %Adapted from mdsvalue Basil P.DUVAL, May 2000 % defaults if nargin
    <2, error('Specify the node and the content.'), end if nargin
      <3, varargin {
      1}
  = expr;
  expr = '$1';
 end cmd =['TreePut($1' sprintf(',$%d', 2:length(varargin) + 2) ')'];

  stat = mdsipmex(cmd, node, expr, varargin {
 1:		  end}
  );

  if nargout
    , status = stat;
  end
/* mdsclose.m */
      function status = mdsclose(para)
 % function to mdsclose on remote server % eg:mdsclose(para);
  %if para
    ~=0, perform a disconnect of remote server % Basil P.DUVAL, Oct 1998 % status = mdsremote(3);
  %status = mdsipmex(3);
  %status = mdsipmex('MDSLIB->MDS$CLOSE()');
  if (nargin < 1) ;
  para = 0;
  end if (para)
    status = mdsipmex('TreeClose()');
  else
    status = mdsdisconnect();
  end
/* mdsdisconnect.m */
      function status = mdsdisconnect()
 % function to close communication with remote mds server % eg:mdsdisconnect;
  %Basil P.DUVAL, Oct 1998 % status = mdsremote(4);
  status = mdsipmex(4);
/* mdsversion.m */
  function status = mdsversion()
      % function to return version of mex mdsipmex % Basil P.DUVAL, May 2000 status = mdsipmex(6);
/* to use mdscvt conversion, put a directory @mdscvt in your path and insert the following file */
/* mdscvt.m */
  function y = mdscvt(x, t)

      % MDSCVT local conversion for MDSIP
  %MDSCVT(X, T) prepares array X to be locally converted to type T when used as
	% a $ argument in MDSDATA or MDSPUT.Supported types are 'd' ouble, 'f' loat,
	%'q' uadword, 'l' ong, 's' hort, signed 'c' har, unsigned 'Q' uadword, u 'L' ong,
	%u 'S' hort, u 'C' har and 't' ext.Ex.:MDSDATA('$', MDSCVT(X, 'l')) uses less
      % network bandwidth than MDSDATA('LONG($)', X). if nargin == 0
    y = mdscvt([]);
  elseif isa(x, 'mdscvt')
   y = x;
  else
if nargin
  == 1 switch class
    (x)
	case 'double', t = 'd';
  case 'char', t = 't';
  otherwise, 'Unsupported class.' end end y.x = x;
  y.t = t;
  y = class(y, 'mdscvt');
  end
#endif
#ifdef PASSWD
  int Lgihpwd() {
    return (1);
  }
#endif
