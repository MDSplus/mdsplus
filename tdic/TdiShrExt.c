/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*  CMS REPLACEMENT HISTORY, Element TDISHREXT.C */
/*  *3    31-AUG-2000 14:34:34 TWF "Fix conflicting names" */
/*  *2    31-AUG-2000 14:32:24 TWF "Fix conflicting names" */
/*  *1    31-AUG-2000 14:19:52 TWF "remote mdsconnect,mdsvalue,mdsdisconnect
 * support from " */
/*  CMS REPLACEMENT HISTORY, Element TDISHREXT.C */
//#define VERSION "2.2000.08.3"
/* these are a few routines to get a TDI client up for MDSIP
 * They will be integrated into libBpdMdsUnix.so and called dynamically
 * This will have to be compiled with the mdsplus/include in the -I path
 * B.P.DUVAL, March 2000
 */

/*EXAMPLE
dele *.exe;
*, *.obj;
*, *.opt;
*mms / descrip = temp.mms;
4 bpdmdsunix_axp.exe
    define TdiShrExt USER:[DUVAL.IBQ.ECC2.TDIC] bpdmdsunix_axp.exe
    _status = build_call(8, 'TdiShrExt', 'rMdsOpen', ref('crpppc6::'), val(0l))
    _oo = build_call(24, 'TdiShrExt', 'rMdsValue', descr('2+3'), xd([]),
val(0ul)) EXAMPLE*/
/*COMPILE w/ debugging
    export DEBUG = -DDEBUG
    make libBpdMdsUnix.so cp libBpdMdsUnix.so / usr / lib / libBpdMdsUnix.so
ldconfig COMPILE*/
#include <ctype.h>
#include <inttypes.h>
#include <ipdesc.h>
#include <mds_stdarg.h>
#include <mdsdescrip.h>
#include <mdsplus/mdsconfig.h>
#include <mdsshr.h>
#include <status.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern int MdsOpen(int conid, char *tree, int shot);
extern int MdsClose(int conid);
extern int TdiCvt();
extern int GetAnswerInfoTS();
extern int MdsIpFree();
extern int ReuseCheck(char *hostin, char *unique, size_t buflen);

#define LOCAL "local"
#define STRLEN 4096
#define INVALID_ID -1

/* Connection record */
typedef struct _connection
{
  int id;       /* Mark the conid as unopen */
  char *unique; /* Unique connection name */
  int port;
  char serv[STRLEN]; /* Current server */
  struct _connection *next;
} Connection;
/* Variables that stay set between calls */

static int id = INVALID_ID; /* Mark the conid as unopen */
static char serv[STRLEN];   /* Current server */
static EMPTYXD(ans_xd);
static Connection *Connections = NULL;

/* Routine definitions */
int rMdsOpen(char *host, int *shotin);
int rMdsClose();
struct descriptor_xd *rMdsValue(struct descriptor *expression, ...);
int rMdsDisconnect();
int rMdsList();
struct descriptor_xd *rMdsVersion();
struct descriptor_xd *rMdsCurrent();
struct descriptor_xd *rgetenv();

static int AddConnection(char *server);
static int MdsToIp(struct descriptor **tdiarg, short *len);
static int IpToMds(int dtypein);

/* Test routines */
#ifdef DEBUG
static void bTest(struct descriptor *desc);
struct descriptor_xd *bTest1(struct descriptor *in_bufD);
#endif

/* Return the name of the current connection */
EXPORT struct descriptor_xd *rMdsCurrent()
{
  /* Values for changing matrix dimensions */
  struct descriptor_s ans_dsc = {0, 0, CLASS_S, 0};

  ans_dsc.pointer = serv;
  ans_dsc.dtype = DTYPE_T;
  ans_dsc.length = strlen(serv);
  MdsCopyDxXd((struct descriptor *)&ans_dsc,
              &ans_xd); /* Copy the arrival data to xd output */
  return (&ans_xd);
}

/* List Current connections */
EXPORT int rMdsList()
{
  Connection *cptr;
  int i = 0;

  if (!strcmp(serv, LOCAL))
    printf("Current connection is local\n");
  for (cptr = Connections; (cptr != NULL);)
  {
    i++;
    if (cptr->id != INVALID_ID)
    {
      printf("Name[%20s] Id[%s] Connection[%3d]", cptr->serv, cptr->unique,
             (int)cptr->id);
      if (cptr->id == id)
        printf("  <-- active");
    }
    else
      printf("UnUsed slot");
    printf("\n");
    cptr = cptr->next;
  }
  return (i);
}

/* Return the version number to TDI */
EXPORT struct descriptor_xd *rgetenv(char *expression)
{
  char *sptr;
  struct descriptor_s ans_dsc = {0, 0, CLASS_S, 0};

  sptr = getenv(expression);
  ans_dsc.pointer = sptr;
  ans_dsc.dtype = DTYPE_T;
  ans_dsc.length = sptr == NULL ? 0 : strlen(sptr);
  MdsCopyDxXd((struct descriptor *)&ans_dsc,
              &ans_xd); /* Copy the arrival data to xd output */
  return (&ans_xd);
}

/* Return the version number to TDI */
EXPORT struct descriptor_xd *rMdsVersion()
{
  struct descriptor_s ans_dsc = {0, 0, CLASS_S, 0};

  ans_dsc.pointer = VERSION;
  ans_dsc.dtype = DTYPE_T;
  ans_dsc.length = strlen(VERSION);
  MdsCopyDxXd((struct descriptor *)&ans_dsc,
              &ans_xd); /* Copy the arrival data to xd output */
  return (&ans_xd);
}

/* Routine returns conid if valid */
static int AddConnection(char *server)
{
  Connection *cptr;
  int nid;
  char unique[128] = "\0";
  /* Extract the ip and port numbers */
  if ((nid = ReuseCheck(server, unique, 128)) == INVALID_ID)
  {
    printf("hostname [%s] invalid, No Connection\n", server);
    return INVALID_ID;
  }
  /* scan through current list looking for an ip/port pair */
  for (cptr = Connections; (cptr != NULL);)
  {
    if ((cptr->id != INVALID_ID) && (strcmp(unique, cptr->unique) == 0))
    {
#ifdef DEBUG
      printf("mdsopen: Keep conid! name changed from  [%s] to [%s]\n",
             cptr->serv, server);
#endif
      /* found a match, replace string and return conid */
      strcpy(cptr->serv, server);
      return (cptr->id);
    }
    cptr = cptr->next;
  }
  /* See if the connection works */
  if ((nid = ConnectToMds(server)) == INVALID_ID)
  {
    printf("mdsopen: Could not open connection to MDS server\n");
    return (INVALID_ID);
  }
  /* Connection valid, find a slot in the stack */
  for (cptr = Connections; (cptr != NULL);)
  {
    if (cptr->id == INVALID_ID)
    { /* found an empty slot */
#ifdef DEBUG
      printf("Found empty slot\n");
#endif
      break;
    }
    cptr = cptr->next;
  }
  /* If slot in stack empty, create a new one */
  if (cptr == NULL)
  { /* End of List */
#ifdef DEBUG
    printf("Making new Connection\n");
#endif
    cptr = (Connection *)malloc(sizeof(Connection));
    cptr->next = Connections;
    Connections = cptr;
  }
  /* Copy in the connection details */
  cptr->unique = strdup(unique);
  cptr->id = nid;
  strcpy(cptr->serv, server); /* Copy in the name */
  return (nid);
}

/* mds server connect */
EXPORT int rMdsConnect(char *hostin)
{
  char host[STRLEN];
  unsigned int i;
  if (hostin == NULL)
    return (0);
  /* Convert to lower case for comparisons */
  for (i = 0; i < (STRLEN - 1) && i < strlen(hostin); i++)
    host[i] = tolower(hostin[i]);
  host[i] = 0;
  if (!strcmp(host, LOCAL))
  {
    strcpy(serv, LOCAL);
    return INVALID_ID;
  }
  /* If no conid, or server name has changed */
  if ((id == INVALID_ID) || strcmp(host, serv))
  {
    if ((id = AddConnection(hostin)) == INVALID_ID)
    {
      *serv = '\0';
      return id; /* no connection obtained */
    }
    else
    {
      strcpy(serv, host); /* copy new name to memory, keep conid open */
    }
  }
  return (id);
}

/* mdsdisconnect
 * ==================================================================== */
EXPORT int rMdsDisconnect(int all)
{
  int status = 1;
  Connection *cptr;

  if (all)
  {
    for (cptr = Connections; (cptr != NULL); cptr = cptr->next)
    {
      if (cptr->id != INVALID_ID)
      {
        DisconnectFromMds(id);
        cptr->id = INVALID_ID;
        if (cptr->unique)
        {
          free(cptr->unique);
          cptr->unique = 0;
        }
      }
    }
  }
  else if (id != INVALID_ID)
  {
    status = DisconnectFromMds(id);
    for (cptr = Connections; (cptr != NULL);)
    {
      if (cptr->id == id)
      {
        cptr->id = INVALID_ID;
        if (cptr->unique)
        {
          free(cptr->unique);
          cptr->unique = 0;
        }
        break;
      }
      cptr = cptr->next;
    }
  }
  else if (strcmp(serv, "local"))
  {
    printf("mdsdisconnect: warning, communication conid aleady closed\n");
    status = 0;
  }
  id = INVALID_ID;
  *serv = '\0'; /* Clear current server string */
  return (status);
}

/*  ================================================================================
 */
EXPORT struct descriptor_xd *rMdsValue(struct descriptor *expression, ...)
{
  /**** NOTE: NULL terminated argument list expected ****/
  va_list incrmtr;
  int i, j;
  unsigned char nargs;
  int status = 1;
  struct descriptor *tdiarg;
  struct descriptor_a *desca;
  int dims[MAX_DIMS]; /* dimensions of an array */
  char dtype;
  char ndims;
  unsigned char *ptr;
  int num;
  short len;
  int numbytes;
  void *dptr;
  void *mem = 0;
  /* check there is a connection open */
  if (id == INVALID_ID)
  {
    printf("MdsValue: No Socket open\n");
    return (0);
  }
  /* Count the arguments which is needed by SendArg routine*/
  va_start(incrmtr, expression);
  tdiarg = (struct descriptor *)expression;
  for (nargs = 0; tdiarg != NULL; nargs++)
  {
    tdiarg = va_arg(incrmtr, struct descriptor *);
#ifdef DEBUG
    printf("Vararg [%d] for [0x%" PRIxPTR "]\n", nargs, (uintptr_t)tdiarg);
#endif
  }
  /* note minimum 1 arg I/P and 1 arg O/P */
  if (expression == NULL || nargs < 1)
  {
    printf("MdsValue: not enough arguments given [%d]\n", nargs);
    return (0);
  }
  /* Process the list */
  va_start(incrmtr, expression);
#ifdef DEBUG
  printf("nargs  [%d]\n", nargs);
#endif
  tdiarg = expression; /* first in list (corresponding to string) */
                       /* Cycle through the arguments */
  for (i = 0; (i < nargs) && (STATUS_OK); i++)
  {
#ifdef DEBUG
    printf("idx[%d] dtype[%d] nargs[%d]\n", i, tdiarg->dtype, nargs);
    bTest(tdiarg); /* Display the descriptor */
#endif
    /* Make up the correct packet for SendArg */
    switch (tdiarg->class)
    {
    default:
      break;
    case CLASS_S: /* fixed-length descriptor */
      ndims = 0;
      num = 1;
      break;
    case CLASS_A: /* Array descriptor */
      desca = (struct descriptor_a *)tdiarg;
      num = desca->arsize / desca->length;
      ndims = desca->dimct; /* dimensions count */
      if (desca->aflags.coeff)
      {
        array_bounds *bptr =
            (array_bounds *)desca; /* access the array bounds of descriptor_a */
        for (j = 0; j < desca->dimct; j++)
          dims[j] = bptr->m[j]; /* copy in the dimensions */
      }
      else
        dims[0] = num; /* simple vector assignment */
      break;
    }
    /* translate the type from Mds to MdsIp descriptors (dtype copied to avoid
     * conflicts from tdi) */
    if (!(dtype = MdsToIp(&tdiarg, &len)))
      return (0);
    ptr = (unsigned char *)tdiarg->pointer;
#ifdef DEBUG
    printf("SendArg id[%d],idx[%d],dtype[%d],nargs[%d],len[%d],ndims[%d]\n", id,
           i, dtype, nargs, len, ndims);
#endif
    status = SendArg(id, (unsigned char)i, dtype, nargs, len, ndims, dims,
                     (char *)ptr);
    tdiarg = va_arg(incrmtr, struct descriptor *); /* get next in list */
  }
  va_end(incrmtr);
  /* Get the reply ================================================== */
  if (STATUS_OK)
  {
    status = GetAnswerInfoTS(id, &dtype, &len, &ndims, dims, &numbytes, &dptr,
                             &mem);
#ifdef DEBUG
    printf(
        "Reply status[%d],dtype[%d],len[%d],ndims[%d],numbytes[%d],ans[%d]\n",
        status, dtype, len, ndims, numbytes, *(int *)dptr);
#endif
    if (STATUS_OK)
    {
      /* Remap the descriptor types */
      dtype = IpToMds(dtype);
      /* Copy the Josh way ( see his example in MdsRemote.c ) */
      if (ndims == 0)
      {
        struct descriptor_s ans_dsc = {
            0, 0, CLASS_S,
            0}; /* Create scalar descriptor for arrived data */
        ans_dsc.pointer = dptr;
        ans_dsc.dtype = dtype;
        ans_dsc.length = len;
        MdsCopyDxXd((struct descriptor *)&ans_dsc,
                    &ans_xd); /* Copy the arrival data to xd output */
      }
      else
      {
        DESCRIPTOR_A_COEFF(
            a_dsc, 0, 0, 0, MAX_DIMS,
            0); /* Create array descriptor for arrived data */
        int i;
        a_dsc.pointer = dptr;
        a_dsc.dimct = ndims;
        a_dsc.dtype = dtype;
        a_dsc.length = len;
        a_dsc.a0 = dptr;
        a_dsc.arsize = numbytes; /* Use byte count already available from
                                        MdsIp reply */
                                 /*      a_dsc.arsize= len;*/
        for (i = 0; i < ndims; i++)
        {
          a_dsc.m[i] = dims[i];
          /*	 a_dsc.arsize *= dims[i];*/
        }
        MdsCopyDxXd((struct descriptor *)&a_dsc,
                    &ans_xd); /* Copy the arrival data to xd output */
      }
    }
    else
    {
      MdsFree1Dx(&ans_xd, 0);
    }
  }
  else
  {
    MdsFree1Dx(&ans_xd, 0);
    rMdsDisconnect(id);
  }
  if (mem)
    MdsIpFree(mem);
  return (&ans_xd);
}

#ifdef CALLING
_ans = build_call(8, 'BpdMdsUnix', 'rMdsOpen', 'localhost::') _a = build_call(
    24, 'BpdMdsUnix', 'rMdsValue', descr('$'), descr(2. .4), xd([]), val(0ul))
#endif
    /* definitions from ipdesc.h for MdsIp descriptor */
    static int
    MdsToIp(struct descriptor * *tdiarg, short *len)
{
  int dtype;
  short llen;
  static EMPTYXD(xd);

  if (len == NULL)
    len = &llen; /* ensure writing is not invalid */

  switch ((*tdiarg)->dtype)
  {
  case DTYPE_BU: /*      2               byte (unsigned);  8-bit unsigned
                    quantity */
    dtype = DTYPE_UCHAR;
    *len = sizeof(char);
    break;
  case DTYPE_WU: /*      3               word (unsigned);  16-bit unsigned
                    quantity */
    dtype = DTYPE_USHORT;
    *len = sizeof(short);
    break;
  case DTYPE_LU: /*      4               longword (unsigned);  32-bit unsigned
                    quantity */
    dtype = DTYPE_ULONG;
    *len = sizeof(int);
    break;
  case DTYPE_QU: /* 5 quadword (unsigned); 64-bit unsigned quantity */
    dtype = DTYPE_ULONGLONG;
    *len = 8;
    break;
  case DTYPE_B: /*      6               byte integer (signed);  8-bit signed
                   2's-complement integer */
    dtype = DTYPE_CHAR;
    *len = sizeof(char);
    break;
  case DTYPE_W: /*      7               word integer (signed);  16-bit signed
                   2's-complement integer */
    dtype = DTYPE_SHORT;
    *len = sizeof(short);
    break;
  case DTYPE_L: /*      8               longword integer (signed);  32-bit
                   signed 2's-complement integer */
    dtype = DTYPE_LONG;
    *len = sizeof(int);
    break;
  case DTYPE_Q: /*  9   quadword integer (signed); 64-bit signed 2's-complement
                   integer */
    dtype = DTYPE_LONGLONG;
    *len = 8;
    break;
  case DTYPE_T: /*      14              character string;  a single 8-bit
                   character or a sequence of characters */
    dtype = DTYPE_CSTRING;
    *len = (*tdiarg)->length
               ? (*tdiarg)->length
               : ((*tdiarg)->pointer ? strlen((*tdiarg)->pointer) : 0);
    break;
  case DTYPE_F:  /*      10              F_floating;  32-bit single-precision
                    floating point */
  case DTYPE_FS: /*      52              IEEE float basic single S */
    if ((*tdiarg)->dtype != DTYPE_NATIVE_FLOAT)
    {
      static char tmp[4];
      static struct descriptor mold = {4, DTYPE_NATIVE_FLOAT, CLASS_S, tmp};
      TdiCvt(*tdiarg, &mold, &xd MDS_END_ARG);
      *tdiarg = xd.pointer;
    }
    dtype = DTYPE_FLOAT;
    *len = sizeof(float);
    break;
  case DTYPE_D:
  case DTYPE_G:
  case DTYPE_FT: /*      53              IEEE float basic double T */
    if ((*tdiarg)->dtype != DTYPE_NATIVE_DOUBLE)
    {
      static char tmp[8];
      static struct descriptor mold = {8, DTYPE_NATIVE_DOUBLE, CLASS_S, tmp};
      TdiCvt(*tdiarg, &mold, &xd MDS_END_ARG);
      *tdiarg = xd.pointer;
    }
    dtype = DTYPE_DOUBLE;
    *len = sizeof(double);
    break;
  case DTYPE_FC:
  case DTYPE_FSC: /*      54              IEEE float basic single S complex */
    if ((*tdiarg)->dtype != DTYPE_FLOAT_COMPLEX)
    {
      static char tmp[8];
      static struct descriptor mold = {8, DTYPE_FLOAT_COMPLEX, CLASS_S, tmp};
      TdiCvt(*tdiarg, &mold, &xd MDS_END_ARG);
      *tdiarg = xd.pointer;
    }
    dtype = DTYPE_COMPLEX;
    *len = sizeof(float) * 2;
    break;
  case DTYPE_DC:
  case DTYPE_GC:
  case DTYPE_FTC: /*      54              IEEE float basic single S complex */
    if ((*tdiarg)->dtype != DTYPE_DOUBLE_COMPLEX)
    {
      static char tmp[16];
      static struct descriptor mold = {16, DTYPE_DOUBLE_COMPLEX, CLASS_S, tmp};
      TdiCvt(*tdiarg, &mold, &xd MDS_END_ARG);
      *tdiarg = xd.pointer;
    }
    dtype = DTYPE_COMPLEX_DOUBLE;
    *len = sizeof(float) * 2;
    break;

  case DTYPE_Z: /*      0               unspecified */
    dtype = 0;
    *len = 0;
    break;
  default:
    printf("Descriptor type [%d]\n not handled\n", (*tdiarg)->dtype);
    *len = dtype = 0;
  }
  return (dtype);
}

static int IpToMds(int dtypein)
{
  int dtype = DTYPE_Z;
  switch (dtypein)
  {
  case DTYPE_UCHAR:
    dtype = DTYPE_BU;
    break; /*      2                byte (unsigned);  8-bit unsigned quantity */
  case DTYPE_USHORT:
    dtype = DTYPE_WU;
    break; /*      3                word (unsigned);  16-bit unsigned quantity
            */
  case DTYPE_ULONG:
    dtype = DTYPE_LU;
    break; /*      4                longword (unsigned);  32-bit unsigned
              quantity */
  case DTYPE_ULONGLONG:
    dtype = DTYPE_QU;
    break; /*      5    quadword (unsigned); 64-bit unsigned quantity */
  case DTYPE_CHAR:
    dtype = DTYPE_B;
    break; /*      6                byte integer (signed);  8-bit signed
              2's-complement integer */
  case DTYPE_SHORT:
    dtype = DTYPE_W;
    break; /*      7                word integer (signed);  16-bit signed
              2's-complement integer */
  case DTYPE_LONG:
    dtype = DTYPE_L;
    break; /*      8                longword integer (signed);  32-bit signed
              2's-complement integer */
  case DTYPE_LONGLONG:
    dtype = DTYPE_Q;
    break; /*      9    quadword integer (signed);  64-bit signed 2's-complement
              integer */
  case DTYPE_FLOAT:
    dtype = DTYPE_NATIVE_FLOAT;
    break; /*  10    float 32-bit */
  case DTYPE_DOUBLE:
    dtype = DTYPE_NATIVE_DOUBLE;
    break; /* 11    double 64-bit */
  case DTYPE_COMPLEX:
    dtype = DTYPE_FLOAT_COMPLEX;
    break; /* 12    complex 32-bit real and imaginary */
  case DTYPE_COMPLEX_DOUBLE:
    dtype = DTYPE_DOUBLE_COMPLEX;
    break; /* 13    complex 64-bit real and imaginary */
  case DTYPE_CSTRING:
    dtype = DTYPE_T;
    break; /*     14                character string;  a single 8-bit character
              or a sequence of characters */
  }
  return (dtype);
}

#ifdef DEBUG
/* test routines for fun */
static void bTest(struct descriptor *desc)
{
  int i, num;
  int8_t *pntC;
  uint8_t *pntUC;
  int16_t *pntS;
  uint16_t *pntUS;
  int32_t *pntW;
  uint32_t *pntUW;
  int64_t *pntL;
  uint64_t *pntUL;
  float *pntF;
  double *pntD;

  struct descriptor_a *desca;
  /* if an array, print out other info */
  switch (desc->class)
  {
  case CLASS_S: /* fixed-length descriptor */
    printf("got descriptor [%d],[%d],[%d],[%" PRIu64 "]\n", desc->length,
           desc->dtype, desc->class, (uintptr_t)desc->pointer);
    num =
        (desc->pointer == NULL) ? 0 : 1; /* pointer must be non zero to print */
    break;
  case CLASS_A: /* Array descriptor */
    desca = (struct descriptor_a *)desc;
    printf("got descriptor [%d],[%d],[%d],[0x%" PRIxPTR "] [%d][%d][%d][%d]\n",
           desca->length, desca->dtype, desca->class, (uintptr_t)desca->pointer,
           desca->scale, desca->digits, desca->dimct, desca->arsize);
    printf("aflags binscale[%d],redim[%d],column[%d],coeff[%d],bounds[%d]\n",
           desca->aflags.binscale, desca->aflags.redim, desca->aflags.column,
           desca->aflags.coeff, desca->aflags.bounds);
    num = desca->arsize / desca->length;
    if (desca->aflags.coeff)
    {
      int j;
      array_bounds *bptr =
          (array_bounds *)desca; /* access the array bounds of descriptor_a */
      printf("Dims  ");
      for (j = 0; j < desca->dimct; j++)
        printf(",%d[%d]", j, bptr->m[j]);
      printf("\n");
    }
    num = (desca->pointer == NULL) ? 0
                                   : 1; /* pointer must be non zero to print */
    break;
  case CLASS_XD:
    printf("extended Descriptor\n");
    printf("got descriptor [%d],[%d],[%d],[0x%" PRIxPTR "]\n", desc->length,
           desc->dtype, desc->class, (uintptr_t)desc->pointer);
    num = 0;
    break;
  default:
    printf("class [%d] unknown\n", desc->class);
    return;
  }
  if (num)
    switch (desc->dtype)
    {
    case DTYPE_Z: /*      0               unspecified */
      printf("unspecified ($MISSING)");
      break;
    case DTYPE_BU: /*      2               byte (unsigned);  8-bit unsigned
                      quantity */
      pntUC = (uint8_t *)desc->pointer;
      for (i = 0; i++ < num;)
        printf("[%u]", *pntUC++);
      break;
    case DTYPE_WU: /*      3               word (unsigned);  16-bit unsigned
                      quantity */
      pntUS = (uint16_t *)desc->pointer;
      for (i = 0; i++ < num;)
        printf("[%u]", *pntUS++);
      break;
    case DTYPE_LU: /*      4               longword (unsigned);  32-bit unsigned
                      quantity */
      pntUW = (uint32_t *)desc->pointer;
      for (i = 0; i++ < num;)
        printf("[%u]", *pntUW++);
      break;
    case DTYPE_QU: /*      5               quadword (unsigned);  64-bit unsigned
                      quantity */
      pntUL = (uint64_t *)desc->pointer;
      for (i = 0; i++ < num;)
        printf("[%" PRIu64 "]", *pntUL++);
      break;
    case DTYPE_OU: /*      25              octaword (unsigned);  128-bit
                      unsigned quantity */
      break;
    case DTYPE_B: /*      6               byte integer (signed);  8-bit signed
                     2's-complement integer */
      pntC = (int8_t *)desc->pointer;
      for (i = 0; i++ < num;)
        printf("[%d]", *pntC++);
      break;
    case DTYPE_W: /*      7               word integer (signed);  16-bit signed
                     2's-complement integer */
      pntS = (int16_t *)desc->pointer;
      for (i = 0; i++ < num;)
        printf("[%d]", *pntS++);
      break;
    case DTYPE_L: /*      8               longword integer (signed);  32-bit
                     signed 2's-complement integer */
      pntW = (int32_t *)desc->pointer;
      for (i = 0; i++ < num;)
        printf("[%d]", *pntW++);
      break;
    case DTYPE_Q: /*      9               quadword integer (signed);  64-bit
                     signed 2's-complement integer */
      pntL = (int64_t *)desc->pointer;
      for (i = 0; i++ < num;)
        printf("[%" PRId64 "]", *pntL++);
      break;
    case DTYPE_T: /*      14              character string;  a single 8-bit
                     character or a sequence of characters */
      printf("string [%s]", (char *)desc->pointer);
      break;
    case DTYPE_O: /*      26              octaword integer (signed);  128-bit
                     signed 2's-complement integer */
      break;
    case DTYPE_F: /*      10              F_floating;  32-bit single-precision
                     floating point */
      pntF = (float *)desc->pointer;
      for (i = 0; i++ < num;)
        printf("[%f]", *pntF++);
      break;
    case DTYPE_D: /*      11              D_floating;  64-bit double-precision
                     floating point */
      break;
    case DTYPE_G: /*      27              G_floating;  64-bit double-precision
                     floating point */
      break;
    case DTYPE_H: /*      28              H_floating;  128-bit
                     quadruple-precision floating point */
      break;
    case DTYPE_FC: /*      12              F_floating complex */
      break;
    case DTYPE_DC: /*      13              D_floating complex */
      break;
    case DTYPE_GC: /*      29              G_floating complex */
      break;
    case DTYPE_HC: /*      30              H_floating complex */
      break;
    case DTYPE_CIT: /*      31              COBOL Intermediate Temporary */
      break;
    case DTYPE_VT: /*      37              varying character string;  16-bit
                      count, followed by a string */
      break;
    case DTYPE_NU: /*      15              numeric string, unsigned */
      break;
    case DTYPE_NL: /*      16              numeric string, left separate sign */
      break;
    case DTYPE_NLO: /*      17              numeric string, left overpunched
                       sign */
      break;
    case DTYPE_NR: /*      18              numeric string, right separate sign
                    */
      break;
    case DTYPE_NRO: /*      19              numeric string, right overpunched
                       sign */
      break;
    case DTYPE_NZ: /*      20              numeric string, zoned sign */
      break;
    case DTYPE_P: /*      21              packed decimal string */
      break;
    case DTYPE_V: /*      1               aligned bit string */
      break;
    case DTYPE_VU: /*      34              unaligned bit string */
      break;
    case DTYPE_FS: /*      52              IEEE float basic single S */
      pntF = (float *)desc->pointer;
      for (i = 0; i++ < num;)
        printf("[%f]", *pntF++);
      break;
    case DTYPE_FT: /*      53              IEEE float basic double T */
      pntD = (double *)desc->pointer;
      for (i = 0; i++ < num;)
        printf("[%g]", *pntD++);
      break;
    case DTYPE_FSC: /*      54              IEEE float basic single S complex */
      break;
    case DTYPE_FTC: /*      55              IEEE float basic double T complex */
      break;
    }
  else
    printf("num was [%d]", num);
  printf("\n"); /* end of line */
}

/* simple example routine as to how to copy or make a xd return descriptor */
struct descriptor_xd *bTest1(struct descriptor *in_bufD)
{
  /* Values for changing matrix dimensions */
  struct descriptor_s ans_dsc = {0, 0, CLASS_S, 0};
  /* by casting to a descriptor with a_coeff, there is enough room to put in
   * dimensions if requrired */
  DESCRIPTOR_A_COEFF(a_dsc, 0, 0, 0, MAX_DIMS, 0);
  struct descriptor_a *desca;
  /* Copy the incoming vector into the output */

  switch (in_bufD->class)
  {
  case CLASS_S: /* fixed-length descriptor */
    ans_dsc.pointer = in_bufD->pointer;
    ans_dsc.dtype = in_bufD->dtype;
    ans_dsc.length = in_bufD->length;
    MdsCopyDxXd((struct descriptor *)&ans_dsc,
                &ans_xd); /* Copy the arrival data to xd output */
    break;
  case CLASS_A: /* Array descriptor */
    desca = (struct descriptor_a *)in_bufD;
    a_dsc.pointer = desca->pointer; /* pointer to the data */
    a_dsc.dtype = desca->dtype;     /* data type */
    a_dsc.length = desca->length;   /* number of bytes per data element */
    a_dsc.dimct = desca->dimct;     /* number of dimensions used */
    a_dsc.arsize = desca->arsize;   /* total size in bytes */
    a_dsc.aflags = desca->aflags;   /* description flags of extended fields */
    if (desca->aflags.coeff)
    {
      int i;
      array_bounds *bptr =
          (array_bounds *)desca; /* access the array bounds of descriptor_a */
      a_dsc.a0 = desca->pointer; /* pointer to first element of data */
      for (i = 0; i < desca->dimct; i++)
      {
        a_dsc.m[i] = bptr->m[i];
      }
    }
    MdsCopyDxXd((struct descriptor *)&a_dsc,
                &ans_xd); /* Copy the arrival data to xd output */
    break;
  }
  return (&ans_xd);
}

#ifdef CALLING
BpdMdsUnix->bTest1 : dsc(descr(1. .10));
#endif
#endif /* DEBUG */
