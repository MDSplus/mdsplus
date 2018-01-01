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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mdsplus/mdsconfig.h>
#include <mdsplus/mdsplus.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <mds_stdarg.h>
#include <treeshr.h>
#include <libroutines.h>
#include <strroutines.h>
#include <opcopcodes.h>
#include <mdstypes.h>
extern int TreeBeginSegment(int nid, struct descriptor *start, struct descriptor *end,
			    struct descriptor *dim, struct descriptor_a *initialData, int idx);
extern int TreePutRow(int nid, int bufsize, int64_t * timestamp, struct descriptor_a *rowdata);
extern int TdiData();
extern int TdiEvaluate();
extern int TdiDecompile();
extern int TdiCompile();
extern int TdiConvert();

extern void *createScalarData(int dtype, int length, char *ptr, void *unitsData, void *errorData,
			      void *helpData, void *validationData, void *tree);
extern void *createArrayData(int dtype, int length, int nDims, int *dims, char *ptr,
			     void *unitsData, void *errorData, void *helpData,
			     void *validationData);
extern void *createCompoundData(int dtype, int length, char *ptr, int nDescs, char **descs,
				void *unitsData, void *errorData, void *helpData,
				void *validationData);
extern void *createApdData(int nData, char **dataPtr, void *unitsData, void *errorData,
			   void *helpData, void *validationData);
extern void *createListData(int nData, char **dataPtr, void *unitsData, void *errorData,
			    void *helpData, void *validationData);
extern void *createDictionaryData(int nData, char **dataPtr, void *unitsData, void *errorData,
				  void *helpData, void *validationData);
extern void *convertDataToDsc(void *data);
extern void convertTime(int *time, char *retTime);
extern char *serializeData(void *dsc, int *retSize, void **retDsc);
extern void *deserializeData(char const *serialized);

extern void convertTimeToAscii(int64_t * timePtr, char *dateBuf, int bufLen, int *retLen);
extern void *getManyObj(char *serializedIn);
extern void *putManyObj(char *serializedIn);

void *convertToScalarDsc(int clazz, int dtype, int length, char *ptr)
{
  EMPTYXD(emptyXd);
  int status;
  struct descriptor dsc;

  struct descriptor_xd *xdPtr = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
  *xdPtr = emptyXd;

  dsc.class = clazz;
  dsc.dtype = dtype;
  dsc.length = length;
  dsc.pointer = ptr;
  status = MdsCopyDxXd(&dsc, xdPtr);
  if (!(status & 1)) {
    printf("PANIC in convertToScalarDsc: MdsCopyDxXd failed\n");
    exit(0);
  }
  return xdPtr;
}

#define MAX_DIMS 32

void *convertToArrayDsc(int clazz, int dtype, int length, int arsize, int nDims, int *dims,
			void *ptr)
{
  EMPTYXD(emptyXd);
  int status, i;
  DESCRIPTOR_A(arr1Dsc, length, dtype, ptr, arsize);
  DESCRIPTOR_A_COEFF(arrNDsc, length, dtype, 0, MAX_DIMS, arsize);
  struct descriptor_xd *xdPtr = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
  *xdPtr = emptyXd;

  if (nDims == 1) {
    arr1Dsc.class = clazz;
    status = MdsCopyDxXd((struct descriptor *)&arr1Dsc, xdPtr);
    if (!(status & 1)) {
      printf("PANIC in convertToArrayDsc: MdsCopyDxXd failed: %s\n", MdsGetMsg(status));
      exit(0);
    }
    return (void *)xdPtr;
  }
  arrNDsc.dimct = nDims;
  for (i = 0; i < nDims; i++)
    arrNDsc.m[i] = dims[nDims - i - 1];
  arrNDsc.pointer = ptr;

  status = MdsCopyDxXd((struct descriptor *)&arrNDsc, xdPtr);
  if (!(status & 1)) {
    printf("PANIC in convertToArrayDsc: MdsCopyDxXd failed\n");
    exit(0);
  }
  return xdPtr;
}

#define MAX_ARGS 128

#ifdef _MSC_VER
#define UNUSED_ARGUMENT
#else
#define UNUSED_ARGUMENT __attribute__ ((unused))
#endif

void *convertToCompoundDsc(int clazz UNUSED_ARGUMENT, int dtype, int length, void *ptr, int ndescs, void **descs)
{
  EMPTYXD(emptyXd);
  struct descriptor_xd *xds[MAX_ARGS];
  int status, i;
  DESCRIPTOR_R(recDsc, 0, MAX_ARGS);
  struct descriptor_xd *xdPtr = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
  *xdPtr = emptyXd;
  recDsc.dtype = dtype;
  recDsc.length = length;
  recDsc.pointer = ptr;
  recDsc.ndesc = ndescs;
  for (i = 0; i < ndescs; i++) {
    xds[i] = 0;
    if (descs[i]) {
      xds[i] = (struct descriptor_xd *)convertDataToDsc(descs[i]);
      recDsc.dscptrs[i] = xds[i]->pointer;
    } else
      recDsc.dscptrs[i] = 0;
  }
  status = MdsCopyDxXd((struct descriptor *)&recDsc, xdPtr);
  if (!(status & 1)) {
    printf("PANIC in convertToCompoundDsc: MdsCopyDxXd failed\n");
    exit(0);
  }
  for (i = 0; i < ndescs; i++)
    if (xds[i]) {
      MdsFree1Dx(xds[i], 0);
      free((char *)xds[i]);
    }

  return xdPtr;
}

void *convertToApdDsc(int type, int ndescs, void **descs)
{
  EMPTYXD(emptyXd);
  struct descriptor_xd **xds =
      (struct descriptor_xd **)malloc(ndescs * sizeof(struct descriptor_xd *));
  int status, i;
  DESCRIPTOR_APD(apdDsc, DTYPE_DSC, 0, 0);
  struct descriptor_xd *xdPtr = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
  *xdPtr = emptyXd;
  apdDsc.dtype = type;
  apdDsc.arsize = ndescs * sizeof(struct descriptor *);
  apdDsc.pointer = malloc(ndescs * sizeof(struct descriptor *));
  for (i = 0; i < ndescs; i++) {
    xds[i] = 0;
    if (descs[i]) {
      xds[i] = (struct descriptor_xd *)convertDataToDsc(descs[i]);
      ((struct descriptor **)apdDsc.pointer)[i] = xds[i]->pointer;
    } else
      ((struct descriptor **)apdDsc.pointer)[i] = 0;
  }
  status = MdsCopyDxXd((struct descriptor *)&apdDsc, xdPtr);
  if (!(status & 1)) {
    printf("PANIC in convertToApdDsc: MdsCopyDxXd failed\n");
    exit(0);
  }
  free((char *)apdDsc.pointer);
  for (i = 0; i < ndescs; i++)
    if (xds[i]) {
      MdsFree1Dx(xds[i], 0);
      free((char *)xds[i]);
    }
  free((char *)xds);
  return xdPtr;
}

void *evaluateData(void *dscPtr, int isEvaluate, int *retStatus)
{
  EMPTYXD(emptyXd);
  int status;

  struct descriptor_xd *xdPtr = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
  *xdPtr = emptyXd;
  if (isEvaluate)
    status = TdiEvaluate(dscPtr, xdPtr MDS_END_ARG);
  else
    status = TdiData(dscPtr, xdPtr MDS_END_ARG);
  *retStatus = status;
  if (!(status & 1))
    return 0;
  return (void *)xdPtr;
}

void *convertFromDsc(void *ptr, void *tree)
{
  struct descriptor_xd *xdPtr = (struct descriptor_xd *)ptr;
  struct descriptor *dscPtr;
  int i, status;
  void *unitsData = 0;
  void *errorData = 0;
  void *helpData = 0;
  void *validationData = 0;
  struct descriptor_r *dscRPtr;
  EMPTYXD(caXd);
  int isCa = 0;

/*	if(xdPtr->class != CLASS_XD)
	{
		printf("PANIC in convertFromDsc: not an XD\n");
		exit(0);
	}
	dscPtr = xdPtr->pointer;
*/
  while (xdPtr && xdPtr->class == CLASS_XD)
    xdPtr = (struct descriptor_xd *)xdPtr->pointer;

  dscPtr = (struct descriptor *)xdPtr;
  if (!dscPtr)
    return NULL;

  //Check for help, units and error
  dscRPtr = (struct descriptor_r *)dscPtr;
  while (dscRPtr->class == CLASS_R
	 && (dscRPtr->dtype == DTYPE_WITH_ERROR || dscRPtr->dtype == DTYPE_WITH_UNITS
	     || dscRPtr->dtype == DTYPE_PARAM)) {
    if (!errorData && dscRPtr->dtype == DTYPE_WITH_ERROR) {
      errorData = convertFromDsc(dscRPtr->dscptrs[1], tree);
      dscRPtr = (struct descriptor_r *)dscRPtr->dscptrs[0];
    }
    if (!unitsData && dscRPtr->dtype == DTYPE_WITH_UNITS) {
      unitsData = convertFromDsc(dscRPtr->dscptrs[1], tree);
      dscRPtr = (struct descriptor_r *)dscRPtr->dscptrs[0];
    }
    if (dscRPtr->dtype == DTYPE_PARAM) {
      helpData = (dscRPtr->dscptrs[1]) ? convertFromDsc(dscRPtr->dscptrs[1], tree) : 0;
      validationData = (dscRPtr->dscptrs[2]) ? convertFromDsc(dscRPtr->dscptrs[2], tree) : 0;
      dscRPtr = (struct descriptor_r *)dscRPtr->dscptrs[0];
    }
  }
  dscPtr = (struct descriptor *)dscRPtr;

//printf("CONVERTFROMDSC class %d  type %d\n", dscPtr->class, dscPtr->dtype);   

  switch (dscPtr->class) {
  case CLASS_S:
    return createScalarData(dscPtr->dtype, dscPtr->length, dscPtr->pointer, unitsData,
			    errorData, helpData, validationData, tree);
  case CLASS_CA:
    status = TdiData(dscPtr, &caXd MDS_END_ARG);
    if (!(status & 1)) {
      printf("Cannot evaluate CA descriptor\n");
      return NULL;
    }
    isCa = 1;
    MDS_ATTR_FALLTHROUGH

  case CLASS_A:
    {
      ARRAY_COEFF(char, 64) * arrDscPtr;
      if (isCa)
	arrDscPtr = (void *)caXd.pointer;
      else
	arrDscPtr = (void *)dscPtr;
      if (arrDscPtr->dimct > 1) {
	void *res = createArrayData(arrDscPtr->dtype, arrDscPtr->length, arrDscPtr->dimct,
				    (int *)&arrDscPtr->m, arrDscPtr->pointer, unitsData, errorData,
				    helpData, validationData);
	if (isCa)
	  MdsFree1Dx(&caXd, 0);
	return res;
      } else {
	int dims = arrDscPtr->arsize / arrDscPtr->length;
	void *res =
	    createArrayData(arrDscPtr->dtype, arrDscPtr->length, 1, &dims, arrDscPtr->pointer,
			    unitsData, errorData, helpData, validationData);
	if (isCa)
	  MdsFree1Dx(&caXd, 0);
	return res;
      }
    }
  case CLASS_R:
    {
      void *retData;
      char **descs = malloc(dscRPtr->ndesc * sizeof(char *));
      //Remove WITH_UNITS, WITH ERROR, PARAM which are not on the top of the Data tree
      dscRPtr = (struct descriptor_r *)dscPtr;
      if (dscRPtr->dtype == DTYPE_WITH_UNITS || dscRPtr->dtype == DTYPE_WITH_ERROR
	  || dscRPtr->dtype == DTYPE_PARAM) {
	EMPTYXD(currXd);
	MdsCopyDxXd(dscRPtr->dscptrs[0], &currXd);
	retData = convertFromDsc(&currXd, tree);
	MdsFree1Dx(&currXd, 0);
	return retData;
      }
      for (i = 0; i < dscRPtr->ndesc; i++) {
	if (dscRPtr->dscptrs[i]) {
	  EMPTYXD(currXd);
	  MdsCopyDxXd(dscRPtr->dscptrs[i], &currXd);
	  descs[i] = convertFromDsc(&currXd, tree);
	  MdsFree1Dx(&currXd, 0);
	} else
	  descs[i] = 0;
      }
      retData =
	  createCompoundData(dscRPtr->dtype, dscRPtr->length, (char *)dscRPtr->pointer,
			     dscRPtr->ndesc, descs, unitsData, errorData, helpData, validationData);
      free((char *)descs);
      return retData;
    }
  case CLASS_APD:
    {
      struct descriptor_a *arrPtr = (struct descriptor_a *)dscPtr;
      char **descs = malloc(arrPtr->arsize);
      void *retData;
      int size = arrPtr->arsize / arrPtr->length;
      for (i = 0; i < size; i++) {
	if (((char **)arrPtr->pointer)[i]) {
	  EMPTYXD(currXd);
	  MdsCopyDxXd(((struct descriptor **)arrPtr->pointer)[i], &currXd);
	  descs[i] = convertFromDsc(&currXd, tree);
	  MdsFree1Dx(&currXd, 0);
	} else
	  descs[i] = 0;
      }
      switch (dscPtr->dtype) {
      case DTYPE_LIST:
	retData = createListData(size, descs, unitsData, errorData, helpData, validationData);
	break;
      case DTYPE_DICTIONARY:
	retData = createDictionaryData(size, descs, unitsData, errorData, helpData, validationData);
	break;
      default:
	retData = createApdData(size, descs, unitsData, errorData, helpData, validationData);
      }
      free((char *)descs);
      return retData;
    }
  default:
    printf("CONVERSION NOT YET SUPPORTED\n");
    exit(0);
  }
  return (0);
}

void freeDsc(void *dscPtr)
{
  
  struct descriptor_xd *xdPtr = (struct descriptor_xd *)dscPtr;
  if(!dscPtr) return;
  if (xdPtr->class != CLASS_XD) {
    printf("PANIC in convertFromDsc: not an XD\n");
    exit(0);
  }
  MdsFree1Dx(xdPtr, 0);
  free((char *)xdPtr);
}

char *decompileDsc(void *ptr)
{
  int status;
  EMPTYXD(xd);
  char *buf;
  struct descriptor *dscPtr = (struct descriptor *)ptr;

  status = TdiDecompile(dscPtr, &xd MDS_END_ARG);
  if (!(status & 1)) {
    printf("Error decompiling expression: %s\n", MdsGetMsg(status));
    return NULL;
  }

  dscPtr = xd.pointer;
  buf = (char *)malloc(dscPtr->length + 1);
  memcpy(buf, dscPtr->pointer, dscPtr->length);
  buf[dscPtr->length] = 0;
  MdsFree1Dx(&xd, NULL);
  return buf;
}

void *compileFromExprWithArgs(char *expr, int nArgs, void **args, void *tree, int *retStatus)
{
  int varIdx;
  int i, status;
  void *arglist[MAX_ARGS];
  struct descriptor_xd *arglistXd[MAX_ARGS];
  void *data;
  EMPTYXD(xd);
  struct descriptor exprD = { 0, DTYPE_T, CLASS_S, 0 };

  exprD.length = strlen(expr);
  exprD.pointer = (char *)expr;

  arglist[1] = &exprD;
  varIdx = 2;
  for (i = 0; i < nArgs; i++) {
//              arglistXd[i] = convertDataToDsc(args[i]);
    arglistXd[i] = (struct descriptor_xd *)args[i];
    if (arglistXd[i]->l_length > 0)
      arglist[varIdx] = arglistXd[i]->pointer;
    else
      arglist[varIdx] = arglistXd[i];
    varIdx++;
  }
  arglist[varIdx++] = &xd;
  arglist[varIdx++] = MdsEND_ARG;
  *(int *)&arglist[0] = varIdx - 1;

  status = *retStatus = (char *)LibCallg(arglist, TdiCompile) - (char *)0;
  if (!(status & 1))
    return NULL;

  data = convertFromDsc(&xd, tree);

  MdsFree1Dx(&xd, 0);
  //for(i = 0; i < nArgs; i++)
  //      freeDsc(arglistXd[i]);
  return data;
}

void *convertToByte(void *dsc)
{
  int status;
  unsigned short opcode = OpcByte;
  DESCRIPTOR_FUNCTION(funD, &opcode, 1);
  struct descriptor_xd *xdPtr = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
  EMPTYXD(emptyXd);

  *xdPtr = emptyXd;
  funD.arguments[0] = dsc;
  status = TdiData(&funD, xdPtr MDS_END_ARG);
  if (!(status & 1)) {
    free((char *)xdPtr);
    return 0;
  }
  return xdPtr;
}

void *convertToByteUnsigned(void *dsc)
{
  int status;
  unsigned short opcode = OpcByteUnsigned;
  DESCRIPTOR_FUNCTION(funD, &opcode, 1);
  struct descriptor_xd *xdPtr = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
  EMPTYXD(emptyXd);

  *xdPtr = emptyXd;
  funD.arguments[0] = dsc;
  status = TdiData(&funD, xdPtr MDS_END_ARG);
  if (!(status & 1)) {
    free((char *)xdPtr);
    return 0;
  }
  return xdPtr;
}

void *convertToShort(void *dsc)
{
  int status;
  unsigned short opcode = OpcWord;
  DESCRIPTOR_FUNCTION(funD, &opcode, 1);
  struct descriptor_xd *xdPtr = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
  EMPTYXD(emptyXd);

  *xdPtr = emptyXd;
  funD.arguments[0] = dsc;
  status = TdiData(&funD, xdPtr MDS_END_ARG);
  if (!(status & 1)) {
    free((char *)xdPtr);
    return 0;
  }
  return xdPtr;
}

void *convertToShortUnsigned(void *dsc)
{
  int status;
  unsigned short opcode = OpcWordUnsigned;
  DESCRIPTOR_FUNCTION(funD, &opcode, 1);
  struct descriptor_xd *xdPtr = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
  EMPTYXD(emptyXd);

  *xdPtr = emptyXd;
  funD.arguments[0] = dsc;
  status = TdiData(&funD, xdPtr MDS_END_ARG);
  if (!(status & 1)) {
    free((char *)xdPtr);
    return 0;
  }
  return xdPtr;
}

void *convertToInt(void *dsc)
{
  int status;
  unsigned short opcode = OpcLong;
  DESCRIPTOR_FUNCTION(funD, &opcode, 1);
  struct descriptor_xd *xdPtr = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
  EMPTYXD(emptyXd);

  *xdPtr = emptyXd;
  funD.arguments[0] = dsc;
  status = TdiData(&funD, xdPtr MDS_END_ARG);
  if (!(status & 1)) {
    free((char *)xdPtr);
    return 0;
  }
  return xdPtr;
}

void *convertToIntUnsigned(void *dsc)
{
  int status;
  unsigned short opcode = OpcLongUnsigned;
  DESCRIPTOR_FUNCTION(funD, &opcode, 1);
  struct descriptor_xd *xdPtr = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
  EMPTYXD(emptyXd);

  *xdPtr = emptyXd;
  funD.arguments[0] = dsc;
  status = TdiData(&funD, xdPtr MDS_END_ARG);
  if (!(status & 1)) {
    free((char *)xdPtr);
    return 0;
  }
  return xdPtr;
}

void *convertToLong(void *dsc)
{
  int status;
  unsigned short opcode = OpcQuadword;
  DESCRIPTOR_FUNCTION(funD, &opcode, 1);
  struct descriptor_xd *xdPtr = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
  EMPTYXD(emptyXd);

  *xdPtr = emptyXd;
  funD.arguments[0] = dsc;
  status = TdiData(&funD, xdPtr MDS_END_ARG);
  if (!(status & 1)) {
    free((char *)xdPtr);
    return 0;
  }
  return xdPtr;
}

void *convertToLongUnsigned(void *dsc)
{
  int status;
  unsigned short opcode = OpcQuadwordUnsigned;
  DESCRIPTOR_FUNCTION(funD, &opcode, 1);
  struct descriptor_xd *xdPtr = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
  EMPTYXD(emptyXd);

  *xdPtr = emptyXd;
  funD.arguments[0] = dsc;
  status = TdiData(&funD, xdPtr MDS_END_ARG);
  if (!(status & 1)) {
    free((char *)xdPtr);
    return 0;
  }
  return xdPtr;
}

void *convertToFloat(void *dsc)
{
  int status;
  unsigned short opcode = OpcFloat;

  DESCRIPTOR_FUNCTION(funD, &opcode, 1);
  struct descriptor_xd *xdPtr = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
  EMPTYXD(emptyXd);

  *xdPtr = emptyXd;
  funD.arguments[0] = dsc;
  status = TdiData(&funD, xdPtr MDS_END_ARG);
  if (!(status & 1)) {
    free((char *)xdPtr);
    return 0;
  }
  return xdPtr;
}

void *convertToDouble(void *dsc)
{
  int status;
  unsigned short opcode = OpcFT_float;

  DESCRIPTOR_FUNCTION(funD, &opcode, 1);
  struct descriptor_xd *xdPtr = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
  EMPTYXD(emptyXd);

  *xdPtr = emptyXd;
  funD.arguments[0] = dsc;
  status = TdiData(&funD, xdPtr MDS_END_ARG);
  if (!(status & 1)) {
    free((char *)xdPtr);
    return 0;
  }
  return xdPtr;
}

void *convertToShape(void *dsc)
{
  int status;
  unsigned short opcode = OpcShape;
  DESCRIPTOR_FUNCTION(funD, &opcode, 1);
  struct descriptor_xd *xdPtr = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
  EMPTYXD(emptyXd);

  *xdPtr = emptyXd;
  funD.arguments[0] = dsc;
  status = TdiData(&funD, xdPtr MDS_END_ARG);
  if (!(status & 1)) {
    free((char *)xdPtr);
    return 0;
  }
  return xdPtr;
}

void *convertToParameter(void *dsc, void *helpDsc, void *validationDsc)
{
  struct descriptor_xd *retXd;
  EMPTYXD(emptyXd);
  struct descriptor_xd *xd = (struct descriptor_xd *)dsc;
  struct descriptor_xd *helpXd = (struct descriptor_xd *)helpDsc;
  struct descriptor_xd *validationXd = (struct descriptor_xd *)validationDsc;
  DESCRIPTOR_PARAM(paramD, 0, 0, 0);

  if (xd) {
    paramD.value = xd->pointer;
  }
  if (helpXd) {
    paramD.help = helpXd->pointer;
  }
  if (validationXd) {
    paramD.validation = validationXd->pointer;
  }
  retXd = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
  *retXd = emptyXd;
  MdsCopyDxXd((struct descriptor *)&paramD, retXd);
  return retXd;
}

void *convertToUnits(void *dsc, void *unitsDsc)
{
  struct descriptor_xd *retXd;
  EMPTYXD(emptyXd);
  struct descriptor_xd *xd = (struct descriptor_xd *)dsc;
  struct descriptor_xd *unitsXd = (struct descriptor_xd *)unitsDsc;
  DESCRIPTOR_WITH_UNITS(withUnitsD, 0, 0);

  if (xd) {
    withUnitsD.data = xd->pointer;
  }
  if (unitsXd) {
    withUnitsD.units = unitsXd->pointer;
  }
  retXd = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
  *retXd = emptyXd;
  MdsCopyDxXd((struct descriptor *)&withUnitsD, retXd);
  return retXd;
}

void *convertToError(void *dsc, void *errorDsc)
{
  struct descriptor_xd *retXd;
  EMPTYXD(emptyXd);
  struct descriptor_xd *xd = (struct descriptor_xd *)dsc;
  struct descriptor_xd *errorXd = (struct descriptor_xd *)errorDsc;
  DESCRIPTOR_WITH_ERROR(withErrorD, 0, 0);

  if (xd) {
    withErrorD.data = xd->pointer;
  }
  if (errorXd) {
    withErrorD.error = errorXd->pointer;
  }
  retXd = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
  *retXd = emptyXd;
  MdsCopyDxXd((struct descriptor *)&withErrorD, retXd);
  return retXd;
}

char *serializeData(void *dsc, int *retSize, void **retDsc)
{
  int status;
  struct descriptor *dscIn = (struct descriptor *)dsc;
  EMPTYXD(emptyXd);
  struct descriptor_xd *xdPtr;
  struct descriptor_a *arrPtr;

  xdPtr = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
  *xdPtr = emptyXd;

  status = MdsSerializeDscOut(dscIn, xdPtr);
  if (!(status & 1) || !xdPtr->pointer) {
    free((char *)xdPtr);
    return 0;
  }
  arrPtr = (struct descriptor_a *)xdPtr->pointer;
  if (arrPtr->dtype != DTYPE_B && arrPtr->dtype != DTYPE_BU) {
    printf("Fatal error: MdsSerializeDscOut returned a wrong type");
    return 0;
  }
  *retDsc = xdPtr;
  *retSize = arrPtr->arsize;
  return arrPtr->pointer;
}

void *deserializeData(char const *serialized)
{
  EMPTYXD(emptyXd);
  struct descriptor_xd *xdPtr;
  int status;

  xdPtr = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
  *xdPtr = emptyXd;
  status = MdsSerializeDscIn(serialized, xdPtr);
  if (!(status & 1))
    return 0;
  return xdPtr;
}

void convertTimeToAscii(int64_t * timePtr, char *dateBuf, int bufLen, int *retLen)
{
  struct descriptor_d dateDsc = { 0, DTYPE_T, CLASS_D, 0 };
  unsigned short len;
  LibSysAscTim(&len, (struct descriptor *)&dateDsc, (int *)timePtr);
  if (len > bufLen)
    len = bufLen;
  if (len > 0)
    memcpy(dateBuf, dateDsc.pointer, len);
  StrFree1Dx(&dateDsc);
  *retLen = len;
}

int64_t convertAsciiToTime(const char *ascTime)
{
  int64_t time;
//      LibConvertDateString("now", &time);
  LibConvertDateString(ascTime, &time);
  return time;
}

//Conversion from VMS to IEEE float
void convertToIEEEFloatArray(int dtype, int length, int nDims, int *dims, void *ptr)
{
  int status, arsize, i;
  float *fArr;
  DESCRIPTOR_A(inArrD, 0, 0, 0, 0);
  DESCRIPTOR_A(outArrD, 0, 0, 0, 0);

  arsize = 1;
  for (i = 0; i < nDims; i++)
    arsize *= dims[i];
  arsize *= length;

  inArrD.length = outArrD.length = length;
  inArrD.dtype = dtype;
  inArrD.arsize = outArrD.arsize = arsize;
  inArrD.pointer = ptr;
  fArr = (float *)malloc(arsize);
  outArrD.pointer = (char *)fArr;
  if (inArrD.dtype == DTYPE_F)
    outArrD.dtype = DTYPE_FLOAT;
  else				//DTYPE_D or DTYPE_G
    outArrD.dtype = DTYPE_DOUBLE;
  status = TdiConvert(&inArrD, &outArrD);
  if (!(status & 1))
    printf("Internal Error: cannot issue TdiConvert\n");
  //copy back results
  memcpy(ptr, outArrD.pointer, arsize);
  free((char *)fArr);
}

void convertToIEEEFloat(int dtype, int length, void *ptr)
{
  int dims[1] = { 1 };
  convertToIEEEFloatArray(dtype, length, 1, dims, ptr);
}

///////////////mdsip support for Connection class///////////
struct descriptor_xd EXPORT *GetManyExecute(char *serializedIn)
{
  static EMPTYXD(xd);
  struct descriptor_xd *serResult;

  serResult = (struct descriptor_xd *)getManyObj(serializedIn);
  if (serResult->class == CLASS_XD)
    MdsSerializeDscOut(serResult->pointer, &xd);
  else
    MdsSerializeDscOut((struct descriptor *)serResult, &xd);
  freeDsc(serResult);
  return &xd;
}

struct descriptor_xd *PutManyExecute(char *serializedIn)
{
  static EMPTYXD(xd);
  struct descriptor *serResult;

  serResult = (struct descriptor *)putManyObj(serializedIn);

  MdsSerializeDscOut(serResult, &xd);
  freeDsc(serResult);
  return &xd;
}
