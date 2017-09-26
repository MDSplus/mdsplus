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
#include "mdsobjects.h"

#include <mdsplus/mdsplus.h>
#include <mdsplus/AutoPointer.hpp>

#include <cstdio>
#include <iostream>
#include <stdarg.h>
#include <string.h>

//#include "mdstree.h"
using namespace MDSplus;
using namespace std;

#define MAX_ARGS 512

///@{ \ingroup Descrmap

extern "C" {
	// From mdsshr.h. Can't include because of class keyword usage in mdsdescrip.h
	char *MdsGetMsg(int sts);

	void *convertToScalarDsc(int clazz, int dtype, int length, char *ptr);
	void *evaluateData(void *dscPtr, int isEvaluate, int *retStatus);
	void freeDsc(void *dscPtr);
	void *convertFromDsc(void *dscPtr);
	char *decompileDsc(void *dscPtr);
	void *compileFromExprWithArgs(const char *expr, int nArgs, void *args, void *tree, int *retStatus);
	void *convertToArrayDsc(int clazz, int dtype, int length, int l_length, int nDims, int *dims, void *ptr);
	void *convertToCompoundDsc(int clazz, int dtype, int length, void *ptr, int ndescs, void **descs);
	void *convertToApdDsc(int type, int ndescs, void **ptr);
	char * serializeData(void *dsc, int *retSize, void **retDsc);
	void *deserializeData(char const * serialized);

	void * convertToByte(void *dsc); 
	void * convertToShort(void *dsc); 
	void * convertToInt(void *dsc); 
	void * convertToLong(void *dsc); 
	void * convertToByteUnsigned(void *dsc); 
	void * convertToShortUnsigned(void *dsc); 
	void * convertToIntUnsigned(void *dsc); 
	void * convertToLongUnsigned(void *dsc); 
	void * convertToFloat(void *dsc); 
	void * convertToDouble(void *dsc); 
	void * convertToShape(void *dcs);
	void * convertToParameter(void *dsc, void *helpDsc, void *validationDsc);
	void * convertToUnits(void *dsc, void *unitsDsc);
	void * convertToError(void *dsc, void *errorDsc);
	void convertToIEEEFloat(int dtype, int length, void *ptr);
	void convertToIEEEFloatArray(int dtype, int length, int nDims, int *dims, void *ptr);
	void convertTimeToAscii(int64_t *timePtr, char *dateBuf, int bufLen, int *retLen);
}

//////Wrapper functions called by C code to build a Data class instance from a MDSplus descriptor///////////

extern "C"  void *convertDataToDsc(void *data)
{
	return ((Data *)data)->convertToDsc();
}

extern "C"  void *createScalarData(int dtype, int length, char *ptr, Data *unitsData, Data *errorData, 
								  Data *helpData, Data *validationData, Tree *tree)
{
	switch(dtype) {
		case DTYPE_B: return new Int8(*ptr, unitsData, errorData, helpData, validationData);
		case DTYPE_BU: return new Uint8(*(unsigned char *)ptr, unitsData, errorData, helpData, validationData);
		case DTYPE_W: return new Int16(*(short *)ptr, unitsData, errorData, helpData, validationData);
		case DTYPE_WU: return new Uint16(*(unsigned short *)ptr, unitsData, errorData, helpData, validationData);
		case DTYPE_L: return new Int32(*(int *)ptr, unitsData, errorData, helpData, validationData);
		case DTYPE_LU: return new Uint32(*(int *)ptr, unitsData, errorData, helpData, validationData);
		case DTYPE_Q: return new Int64(*(int64_t *)ptr, unitsData, errorData, helpData, validationData);
		case DTYPE_QU: return new Uint64(*(int64_t *)ptr, unitsData, errorData, helpData, validationData);
		case DTYPE_FLOAT: return new Float32(*(float *)ptr, unitsData, errorData, helpData, validationData);
		case DTYPE_F: 
			convertToIEEEFloat(dtype, length, ptr); 
			return new Float32(*(float *)ptr, unitsData, errorData, helpData, validationData);
		case DTYPE_DOUBLE: return new Float64(*(double *)ptr, unitsData, errorData, helpData, validationData);
		case DTYPE_G:
		case DTYPE_D:
			convertToIEEEFloat(dtype, length, ptr); 
			return new Float64(*(double *)ptr, unitsData, errorData, helpData, validationData);
		case DTYPE_FSC: return new Complex32(((float *)ptr)[0], ((float *)ptr)[1], unitsData, errorData, helpData, validationData);
		case DTYPE_FTC: return new Complex64(((double *)ptr)[0], ((double *)ptr)[1], unitsData, errorData, helpData, validationData);
		case DTYPE_T: return new String(ptr, length, unitsData, errorData, helpData, validationData);
		case DTYPE_NID: return new TreeNode(*(int *)ptr, tree, unitsData, errorData, helpData, validationData);
		case DTYPE_PATH: return new TreePath(ptr, length, tree, unitsData, errorData, helpData, validationData);
		case DTYPE_IDENT: return new Ident(ptr, length, unitsData, errorData, helpData, validationData);
	}
	return 0;
}

extern "C"  void *createArrayData(int dtype, int length, int nDims, int *dims, char *ptr, 
								 Data *unitsData, Data *errorData, Data *helpData, Data *validationData)
{
	int revDims[MAX_ARGS];
	for(int i = 0; i < nDims; i++)
		revDims[i] = dims[nDims - i - 1];
	switch(dtype) {
		case DTYPE_B: return new Int8Array(ptr, nDims, revDims, unitsData, errorData, helpData, validationData);
		case DTYPE_BU: return new Uint8Array((unsigned char *)ptr, nDims, revDims, unitsData, errorData, helpData, validationData);
		case DTYPE_W: return new Int16Array((short *)ptr, nDims, revDims, unitsData, errorData, helpData, validationData);
		case DTYPE_WU: return new Uint16Array((unsigned short *)ptr, nDims, revDims, unitsData, errorData, helpData, validationData);
		case DTYPE_L: return new Int32Array((int *)ptr, nDims, revDims, unitsData, errorData, helpData, validationData);
		case DTYPE_LU: return new Uint32Array((unsigned int *)ptr, nDims, revDims, unitsData, errorData, helpData, validationData);
		case DTYPE_Q: return new Int64Array((int64_t *)ptr, nDims, revDims, unitsData, errorData, helpData, validationData);
		case DTYPE_QU: return new Uint64Array((uint64_t *)ptr, nDims, revDims, unitsData, errorData, helpData, validationData);
		case DTYPE_FLOAT: return new Float32Array((float *)ptr, nDims, revDims, unitsData, errorData, helpData, validationData);
		case DTYPE_F: 
			convertToIEEEFloatArray(dtype, length, nDims, dims, ptr);
			return new Float32Array((float *)ptr, nDims, revDims, unitsData, errorData, helpData, validationData);
		case DTYPE_DOUBLE: return new Float64Array((double *)ptr, nDims, revDims, unitsData, errorData, helpData, validationData);
		case DTYPE_G:
		case DTYPE_D:
			convertToIEEEFloatArray(dtype, length, nDims, dims, ptr);
			return new Float64Array((double *)ptr, nDims, revDims, unitsData, errorData, helpData, validationData);
		case DTYPE_FSC: return new Complex32Array((float *)ptr, nDims, revDims, unitsData, errorData, helpData, validationData);
		case DTYPE_FTC: return new Complex64Array((double *)ptr, nDims, revDims, unitsData, errorData, helpData, validationData);
		case DTYPE_T: return new StringArray((char *)ptr, dims[0], length);
	}
	return 0;
}

extern "C"  void *createCompoundData(int dtype, int length, char *ptr, int nDescs, char **descs, 
									Data *unitsData, Data *errorData, Data *helpData, Data *validationData)
{
//printf("CREATE COMPOUND DATA nDescs = %d ptr= %x\n", nDescs, ptr);
  switch (dtype) {
		case DTYPE_SIGNAL: return new Signal(dtype, length, ptr, nDescs, descs, unitsData, errorData, helpData, validationData);
		case DTYPE_DIMENSION: return new Dimension(dtype, length, ptr, nDescs, descs, unitsData, errorData, helpData, validationData);
		case DTYPE_WINDOW: return new Window(dtype, length, ptr, nDescs, descs, unitsData, errorData, helpData, validationData);
		case DTYPE_FUNCTION: return new Function(dtype, length, ptr, nDescs, descs, unitsData, errorData, helpData, validationData);
		case DTYPE_CONGLOM: return new Conglom(dtype, length, ptr, nDescs, descs, unitsData, errorData, helpData, validationData);
		case DTYPE_SLOPE:
		case DTYPE_RANGE: return new Range(dtype, length, ptr, nDescs, descs, unitsData, errorData, helpData, validationData);
		case DTYPE_ACTION: return new Action(dtype, length, ptr, nDescs, descs, unitsData, errorData, helpData, validationData);
		case DTYPE_DISPATCH: return new Dispatch(dtype, length, ptr, nDescs, descs, unitsData, errorData, helpData, validationData);
		case DTYPE_PROGRAM: return new Program(dtype, length, ptr, nDescs, descs, unitsData, errorData, helpData, validationData);
		case DTYPE_ROUTINE: return new Routine(dtype, length, ptr, nDescs, descs, unitsData, errorData, helpData, validationData);
		case DTYPE_PROCEDURE: return new Procedure(dtype, length, ptr, nDescs, descs, unitsData, errorData, helpData, validationData);
		case DTYPE_METHOD: return new Method(dtype, length, ptr, nDescs, descs, unitsData, errorData, helpData, validationData);
		case DTYPE_DEPENDENCY: return new Dependency(dtype, length, ptr, nDescs, descs, unitsData, errorData, helpData, validationData);
		case DTYPE_CONDITION: return new Condition(dtype, length, ptr, nDescs, descs, unitsData, errorData, helpData, validationData);
		case DTYPE_CALL: return new Call(dtype, length, ptr, nDescs, descs, unitsData, errorData, helpData, validationData);
	}
	return 0;
}

extern "C"  void *createApdData(int nData, char **dataPtrs, Data *unitsData, 
							   Data *errorData, Data *helpData, Data *validationData)
{
	return new Apd(nData, (Data **) dataPtrs, unitsData, errorData, helpData, validationData);
}
extern "C"  void *createListData(int nData, char **dataPtrs, Data *unitsData, 
							   Data *errorData, Data *helpData, Data *validationData)
{
	return new List(nData, (Data **) dataPtrs, unitsData, errorData, helpData, validationData);
}
extern "C"  void *createDictionaryData(int nData, char **dataPtrs, Data *unitsData, 
							   Data *errorData, Data *helpData, Data *validationData)
{
	return new Dictionary(nData, (Data **) dataPtrs, unitsData, errorData, helpData, validationData);
}

// end of Descrmap
///@}




////////MdsException implemenmtation /////

/// \brief MdsException::MdsException
/// \param status ref to MdsGetMsg()
MdsException::MdsException(int status): msg(MdsGetMsg(status)) { }

///////////////////Data methods implementation////////////////////////

Data::~Data() {
//	decRefCount();
}

//void *Data::operator new(size_t sz) {
//	return ::operator new(sz);
//}

//void Data::operator delete(void *p) {
//	Data * data = reinterpret_cast<Data *>(p);
////    if(data->units) { data->units->decRefCount(); deleteData(data->units); }
////    if(data->error) { data->error->decRefCount(); deleteData(data->error); }
////    if(data->help) { data->help->decRefCount(); deleteData(data->help); }
////    if(data->validation) { data->validation->decRefCount(); deleteData(data->validation); }
//    //data->propagateDeletion();
//	::operator delete(p);
//}

void MDSplus::deleteData(Data *data) {
	if (data->refCount <= 1) {
        if(data->units) { data->units->decRefCount(); deleteData(data->units); }
        if(data->error) { data->error->decRefCount(); deleteData(data->error); }
        if(data->help) { data->help->decRefCount(); deleteData(data->help); }
        if(data->validation) { data->validation->decRefCount(); deleteData(data->validation); }
        delete data;
        data = NULL;
    } else {
        data->decRefCount();
    }
}

void Data::decRefCount(Data * d) {
	d->refCount--;
}

void Data::incRefCount(Data * d) {
	d->refCount++;
}

void Data::decRefCount() {
    decRefCount(this);
}

void Data::incRefCount() {
	incRefCount(this);
}

static Data * getMember(Data * member) {
    if (member) member->incRefCount();
    return member;
}

Data * Data::getUnits() {
	return getMember(units);
}

Data * Data::getError() {
	return getMember(error);
}

Data * Data::getHelp() {
	return getMember(help);
}

Data * Data::getValidation() {
	return getMember(validation);
}

static void setMember(Data *&target, Data *member) {
    if(target) { target->decRefCount(); deleteData(target); }
    if(member) {
        target = member;
        target->incRefCount();
    }
}

void Data::setUnits(Data * in) {
    setMember(this->units, in);
}

void Data::setError(Data * in) {
    setMember(this->error, in);
}

void Data::setHelp(Data * in) {
    setMember(this->help, in);
}

void Data::setValidation(Data * in) {
    setMember(this->validation, in);
}

Data *Data::data()
{
	void *dscPtr = convertToDsc();
	int retStatus;
	void *evalPtr = evaluateData(dscPtr, 0, &retStatus);
	if(!(retStatus & 1))
		throw MdsException(retStatus);

	Data *retData = (Data *)convertFromDsc(evalPtr);
	freeDsc(dscPtr);
	freeDsc(evalPtr);
	return retData;
}

Data *Data::evaluate()
{
	void *dscPtr = convertToDsc();
	int retStatus;
	void *evalPtr = evaluateData(dscPtr, 1, &retStatus);
    if( !(retStatus & 1) )
		throw MdsException(retStatus);
	Data *retData = (Data *)convertFromDsc(evalPtr);
	freeDsc(dscPtr);
	freeDsc(evalPtr);
	return retData;
}

char *Data::decompile()
{
	void *dscPtr = convertToDsc();
	char * retStr = decompileDsc(dscPtr);
	freeDsc(dscPtr);
	//Get rid of malloc'd stuff
	char *retDec = new char[strlen(retStr)+1];
	strcpy(retDec, retStr);
	free(retStr);
	return retDec;
}

void *Scalar::convertToDsc()
{
	return completeConversionToDsc(convertToScalarDsc(clazz, dtype, length, ptr));
}

//Make a dymanically allocated copy of the Data instance Tee
Data *Data::clone()
{
	void *dscPtr = convertToDsc();
	Data *retData = (Data *)convertFromDsc(dscPtr);
	freeDsc(dscPtr);
	return retData;
}

int * Data::getShape(int *numDim)
{
	void *dscPtr = convertToDsc();
	void *retDsc = convertToShape(dscPtr);
	Data *retData = (Data *)convertFromDsc(retDsc);
	if(!retData || retData->clazz != CLASS_S)
		throw MdsException("Cannot compute shape");
	freeDsc(dscPtr);
	freeDsc(retDsc);
	
	int *res = retData->getIntArray(numDim);
	deleteData(retData);
	return res;
}

/// \ingroup Descrmap
///
Data * Data::getData(int classType, int dataType) {
	void *dscPtr = convertToDsc();
	void *retDsc = NULL;
	switch(dataType)  
	{
	  case DTYPE_B: 
	    retDsc = convertToByte(dscPtr); 
	    break;
	  case DTYPE_BU:
	    retDsc = convertToByteUnsigned(dscPtr); 
	    break;
	  case DTYPE_W:
	    retDsc = convertToShort(dscPtr); 
	    break;
	  case DTYPE_WU:
	    retDsc = convertToShortUnsigned(dscPtr);
	    break;
	  case DTYPE_L:
	    retDsc = convertToInt(dscPtr);
	    break;
	  case DTYPE_LU:
	    retDsc = convertToIntUnsigned(dscPtr);
	    break;
	  case DTYPE_Q:
	    retDsc = convertToLong(dscPtr);
	    break;
	  case DTYPE_QU:
	    retDsc = convertToLongUnsigned(dscPtr);
	    break;
	  case DTYPE_FLOAT:
	    retDsc = convertToFloat(dscPtr);
	    break;
	  case DTYPE_DOUBLE:
	    retDsc = convertToDouble(dscPtr);
	    break;
	}
	Data *retData = (Data *)convertFromDsc(retDsc);
	freeDsc(dscPtr);
	freeDsc(retDsc);
	if(!retData || retData->clazz != classType)
		throw MdsException("Cannot convert to desired type");

	return retData;
}

Data * Data::getArrayData(int dtype) {
	return getData(CLASS_A, dtype);
}

Data * Data::getScalarData(int dtype) {
	return getData(CLASS_S, dtype);
}

template<class T>
static std::vector<T> getArray(T * data, int size) {
	std::vector<T> v(data, data+size);
	delete[] data;
	return v;
}

void MDSplus::deleteNativeArray(char * array){delete []array;}
void MDSplus::deleteNativeArray(unsigned char * array){delete []array;}
void MDSplus::deleteNativeArray(short * array){delete []array;}
void MDSplus::deleteNativeArray(unsigned short * array){delete []array;}
void MDSplus::deleteNativeArray(int * array){delete []array;}
void MDSplus::deleteNativeArray(unsigned int * array){delete []array;}
void MDSplus::deleteNativeArray(int64_t * array){delete []array;}
void MDSplus::deleteNativeArray(uint64_t * array){delete []array;}
void MDSplus::deleteNativeArray(float * array){delete []array;}
void MDSplus::deleteNativeArray(double * array){delete []array;}
void MDSplus::deleteNativeArray(char ** array){delete []array;}
void MDSplus::deleteNativeArray(MDSplus::Data ** array){delete []array;}

char *Data::getByteArray(int *numElements)
{
	AutoPointer<Data> array(getArrayData(DTYPE_B));
	return array.ptr->getByteArray(numElements);
}

std::vector<char> Data::getByteArray()
{
	int numElements;
	char *retData = getByteArray(&numElements);
	return getArray(retData, numElements);
}

unsigned char *Data::getByteUnsignedArray(int *numElements)
{
	AutoPointer<Data> array(getArrayData(DTYPE_BU));
	return array.ptr->getByteUnsignedArray(numElements);
}

std::vector<unsigned char> Data::getByteUnsignedArray()
{
	int numElements;
	unsigned char *retData = getByteUnsignedArray(&numElements);
	return getArray(retData, numElements);
}

short * Data::getShortArray(int *numElements)
{
	AutoPointer<Data> array(getArrayData(DTYPE_W));
	return array.ptr->getShortArray(numElements);
}

std::vector<short> Data::getShortArray()
{
	int numElements;
	short *retData = getShortArray(&numElements);
	return getArray(retData, numElements);
}

unsigned short * Data::getShortUnsignedArray(int *numElements)
{
	AutoPointer<Data> array(getArrayData(DTYPE_WU));
	return array.ptr->getShortUnsignedArray(numElements);
}

std::vector<unsigned short> Data::getShortUnsignedArray()
{
	int numElements;
	unsigned short *retData = getShortUnsignedArray(&numElements);
	return getArray(retData, numElements);
}

int * Data::getIntArray(int *numElements)
{
	AutoPointer<Data> array(getArrayData(DTYPE_L));
	return array.ptr->getIntArray(numElements);
}

std::vector<int> Data::getIntArray()
{
	int numElements;
	int *retData = getIntArray(&numElements);
	return getArray(retData, numElements);
}

unsigned int * Data::getIntUnsignedArray(int *numElements)
{
	AutoPointer<Data> array(getArrayData(DTYPE_LU));
	return array.ptr->getIntUnsignedArray(numElements);
}

std::vector<unsigned int> Data::getIntUnsignedArray()
{
	int numElements;
	unsigned int *retData = getIntUnsignedArray(&numElements);
	return getArray(retData, numElements);
}

int64_t * Data::getLongArray(int *numElements)
{
	AutoPointer<Data> array(getArrayData(DTYPE_Q));
	return array.ptr->getLongArray(numElements);
}

std::vector<int64_t> Data::getLongArray()
{
	int numElements;
	int64_t *retData = getLongArray(&numElements);
	return getArray(retData, numElements);
}

uint64_t * Data::getLongUnsignedArray(int *numElements)
{
	AutoPointer<Data> array(getArrayData(DTYPE_QU));
	return array.ptr->getLongUnsignedArray(numElements);
}

std::vector<uint64_t> Data::getLongUnsignedArray()
{
	int numElements;
	uint64_t *retData = getLongUnsignedArray(&numElements);
	return getArray(retData, numElements);
}

float * Data::getFloatArray(int *numElements)
{
	AutoPointer<Data> array(getArrayData(DTYPE_FLOAT));
	return array.ptr->getFloatArray(numElements);
}

std::vector<float> Data::getFloatArray()
{
	int numElements;
	float *retData = getFloatArray(&numElements);
	return getArray(retData, numElements);
}

double * Data::getDoubleArray(int *numElements)
{
	AutoPointer<Data> array(getArrayData(DTYPE_DOUBLE));
	return array.ptr->getDoubleArray(numElements);
}

std::vector<double> Data::getDoubleArray()
{
	int numElements;
	double *retData = getDoubleArray(&numElements);
	return getArray(retData, numElements);
}

std::vector<std::complex<double> > Data::getComplexArray()
{
	int numElements;
	std::complex<double> *retData = getComplexArray(&numElements);
	return getArray(retData, numElements);
}

char * Data::serialize(int *size)
{
	void *dscPtr = convertToDsc();
	void *retDsc;
	int retSize;
	char *serialized = serializeData(dscPtr, &retSize, &retDsc);
	if(!serialized)
	{
		freeDsc(dscPtr);
		throw MdsException("Cannot serialize Data object");
	}
	char *retSerialized = new char[retSize];
	memcpy(retSerialized, serialized, retSize);
	freeDsc(dscPtr);
	freeDsc(retDsc);
	*size = retSize;
	return retSerialized;
}

char Data::getByte()
{
	AutoPointer<Data> scalar(getScalarData(DTYPE_B));
	return scalar.ptr->getByte();
}

short Data::getShort()
{
	AutoPointer<Data> scalar(getScalarData(DTYPE_W));
	return scalar.ptr->getShort();
}

int Data::getInt()
{
	AutoPointer<Data> scalar(getScalarData(DTYPE_L));
	return scalar.ptr->getInt();
}

int64_t Data::getLong()
{
	AutoPointer<Data> scalar(getScalarData(DTYPE_Q));
	return scalar.ptr->getLong();
}

unsigned char Data::getByteUnsigned()
{
	AutoPointer<Data> scalar(getScalarData(DTYPE_BU));
	return scalar.ptr->getByteUnsigned();
}

unsigned short Data::getShortUnsigned()
{
	AutoPointer<Data> scalar(getScalarData(DTYPE_WU));
	return scalar.ptr->getShortUnsigned();
}

unsigned int Data::getIntUnsigned()
{
	AutoPointer<Data> scalar(getScalarData(DTYPE_LU));
	return scalar.ptr->getIntUnsigned();
}

uint64_t Data::getLongUnsigned()
{
	AutoPointer<Data> scalar(getScalarData(DTYPE_QU));
	return scalar.ptr->getLongUnsigned();
}

float Data::getFloat()
{
	AutoPointer<Data> scalar(getScalarData(DTYPE_FLOAT));
	return scalar.ptr->getFloat();
}

double Data::getDouble()
{
	AutoPointer<Data> scalar(getScalarData(DTYPE_DOUBLE));
	return scalar.ptr->getDouble();
}

Data * Data::getDimensionAt(int dimIdx UNUSED_ARGUMENT) {
	return executeWithArgs("DIM_OF($)", 1, this);
}






////////////////////////////////////////////////////////////////////////////////
// TDI BINDINGS  ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///@{ \ingroup TDIbind


///
/// \return new instanced Data representing compiled script
/// 
/// ref to \ref MDSplus::compileWithArgs
Data * MDSplus::compile(const char *expr) {
	return compileWithArgs(expr, 0);
}

///
///
Data * MDSplus::compileWithArgs(const char *expr, int nArgs ...) {
	struct Lambda {
		std::vector<void *> args;
		~Lambda() {
			std::for_each(args.begin(), args.end(), freeDsc);
		}
	} lambda;

	std::vector<void *> & args = lambda.args;
	va_list v;
	va_start(v, nArgs);
	for(int i = 0; i < nArgs; ++i)
		args.push_back((va_arg(v, Data *))->convertToDsc());
	va_end(v);

	int status;
    Data * res;
    try {
        AutoPointer<Tree> actTree(getActiveTree());
        res = (Data *)compileFromExprWithArgs(expr, nArgs, &args[0], actTree.ptr, &status);
    } catch(MdsException &exc)
    {
        res = (Data *)compileFromExprWithArgs(expr, nArgs, &args[0], 0, &status);
    }

	if(!(status & 1))
		throw MdsException(status);
	return res;
}

///
/// \brief MDSplus::compile
/// \param expr TDI expression to evaluate
/// \param tree Tree target pointer
/// \return new instanced Data representing compiled script
///
Data * MDSplus::compile(const char *expr, Tree *tree) {
	return  compileWithArgs(expr, tree, 0);
}

///
/// \brief MDSplus::compileWithArgs compile TDI expression with variadic argument
/// \param expr TDI expression to evaluate
/// \param tree Tree node pointer
/// \param nArgs number of arguments of expression
/// \return new instanced Data representing compiled script
///
Data * MDSplus::compileWithArgs(const char *expr, Tree *tree, int nArgs ...) {
		int i;
		void *args[MAX_ARGS];

		va_list v;
		va_start(v, nArgs);
		for(i = 0; i < nArgs; i++)
		{
			Data *currArg = va_arg(v, Data *);
			args[i] = currArg->convertToDsc();
		}
        if(tree)
            setActiveTree(tree);
		int status;
		Data *res = (Data *)compileFromExprWithArgs(expr, nArgs, (void *)args, tree, &status);
		for(i = 0; i < nArgs; i++)
		    freeDsc(args[i]);
		if(!(status & 1))
			throw MdsException(status);
		return res;
}


Data * MDSplus::execute(const char *expr) {
	return executeWithArgs(expr, 0);
}

Data * MDSplus::executeWithArgs(const char *expr, int nArgs ...) {
		void *args[MAX_ARGS];

		va_list v;
		va_start(v, nArgs);
		
		for(int i = 0; i < nArgs; i++)
		{
			Data *currArg = va_arg(v, Data *);
			args[i] = currArg->convertToDsc();
		}
		int status;
		Tree *actTree = 0;
		try {
			actTree = getActiveTree();
		}catch(MdsException const & exc){actTree = 0;}
		Data *compData = (Data *)compileFromExprWithArgs(expr, nArgs, (void *)args, actTree, &status);
		if(!(status & 1))
			throw MdsException(status);
		Data *evalData = compData->data();
		deleteData(compData);
		for(int i = 0; i < nArgs; i++)
		    freeDsc(args[i]);
		if(actTree)
		    delete(actTree);

		return evalData;
}

Data * MDSplus::execute(const char *expr, Tree *tree) {
	return executeWithArgs(expr, tree, 0);
}

Data * MDSplus::executeWithArgs(const char *expr, Tree *tree, int nArgs ...) {
		void *args[MAX_ARGS];

		va_list v;
		va_start(v, nArgs);
		for(int i = 0; i < nArgs; i++)
		{
			Data *currArg = va_arg(v, Data *);
			args[i] = currArg->convertToDsc();
		}
		if(tree)
			setActiveTree(tree);
		int status;
		Data *compData = (Data *)compileFromExprWithArgs((char *)expr, nArgs, (void *)args, tree, &status);
		if(!(status & 1))
			throw MdsException(status);
		if(!compData)
			throw MdsException("Cannot compile expression");

		Data *evalData = compData->data();
		deleteData(compData);
		for(int i = 0; i < nArgs; i++)
		    freeDsc(args[i]);
		return evalData;
}

/*
Data * MDSplus::executeWithArgs(const char *expr, Data **dataArgs, int nArgs)
	{
		void *args[MAX_ARGS];
		int i;
		int actArgs = (nArgs < MAX_ARGS)?nArgs:MAX_ARGS;
		for(i = 0; i < actArgs; i++)
			args[nArgs++] = dataArgs[i]->convertToDsc();

		int status;
		Data *compData = (Data *)compileFromExprWithArgs(expr, actArgs, (void *)args, 0, &status);
		if(!(status & 1))
			throw MdsException(status);
		if(!compData)
		{
			char *msg = new char[20 + strlen(expr)];
			sprintf(msg, "Cannot compile %s", expr);
			MdsException *exc = new MdsException(msg);
			delete [] msg;
			throw exc;
		}
		Data *evalData = compData->data();
		deleteData (compData);
		for(i = 0; i < actArgs; i++)
		    freeDsc(args[i]);

		return evalData;
}
*/

//Complete Conversion to Dsc by condsidering help, units and error

void * Data::completeConversionToDsc(void *dsc) {
	void *retDsc = dsc;

	if(help || validation)
	{
		void *helpDsc = (help)?help->convertToDsc():0;
		void *validationDsc = (validation)?validation->convertToDsc():0;
		void *oldDsc = retDsc;
		retDsc = convertToParameter(retDsc, helpDsc, validationDsc);
		if(helpDsc) freeDsc(helpDsc);
		if(validationDsc) freeDsc(validationDsc);
		if(oldDsc) freeDsc(oldDsc);
	}
		 
	if(error)
	{
		void *errorDsc = error->convertToDsc();
		void *oldDsc = retDsc;
		retDsc = convertToError(retDsc, errorDsc);
		if(errorDsc) freeDsc(errorDsc);
		if(oldDsc) freeDsc(oldDsc);
	}
		
	if(units)
	{
		void *unitsDsc = units->convertToDsc();
		void *oldDsc = retDsc;
		retDsc = convertToUnits(retDsc, unitsDsc);
		if(unitsDsc) freeDsc(unitsDsc);
		if(oldDsc) freeDsc(oldDsc);
	}

	return retDsc;
}

// TDIbind end
///@}


void Data::plot()
{
		Data *dim = getDimensionAt(0);
		Scope *scope = new Scope("");
		scope->plot(this, dim);
		scope->show();
		delete scope;
        deleteData(dim);
}





int *Array::getShape(int *numDims)
{
	int *retDims = new int[nDims];
	*numDims = nDims;
	for(int i = 0; i < nDims; i++)
		retDims[i] = dims[i];
	return retDims;
}

//Array Methods setElementAt and getElementAt
Data * Array::getElementAt(int *getDims, int getNumDims)
{
	//Check Dimensionality
	if(getNumDims > nDims)
		throw MdsException("Invalid passed dimensions in Array::getElementAt");

	for(int i = 0; i < getNumDims; ++i)
		if(getDims[i] < 0 || getDims[i] >= dims[i])
			throw MdsException("Invalid passed dimensions in Array::getElementAt");

	//Prepare actual row dimensions
	std::vector<int> rowDims(nDims);
	rowDims[nDims - 1] = 1;
	for(int i = nDims - 2; i >= 0; --i)
		rowDims[i] = rowDims[i+1] * dims[i+1];

	//Compute startIdx of selected data portion
	int startIdx = 0;
	for(int i = 0; i < getNumDims; ++i)
		startIdx += getDims[i] * rowDims[i];

	if(getNumDims == nDims) //return a scalar
		return (Data *)createScalarData(dtype, length, ptr+(startIdx * length), 0,0,0,0, 0);
	//Otherwise return an array
	return (Data *)createArrayData(dtype, length, nDims - getNumDims, &dims[getNumDims], ptr+(startIdx * length), 0,0,0,0);
}


Array *Array::getSubArray(int startDim, int nSamples)
{
	int i;

	//Check Dimensionality
	if(startDim + nSamples > dims[0])
		throw MdsException("Invalid passed dimensions in Array::getSubArray");

	//Prepare actual row dimensions
	int rowItems = 1;
	for(i = nDims - 2; i >= 0; i--)
		rowItems *= dims[i+1];

	//Compute startIdx of selected data portion
	int startIdx = startDim * rowItems;
	int *newDims = new int[nDims];
	for(i = 1; i < nDims; i++)
		newDims[i] = dims[i];
	newDims[0] = nSamples;

	Array *retArr = (Array *)createArrayData(dtype, length, nDims, newDims, ptr+(startIdx * length), 0,0,0,0);
	delete [] newDims;
	return retArr;
}




void Array::setElementAt(int *getDims, int getNumDims, Data *data)
{
	int i;

	Array *arrayData;
	Scalar *scalarData;

	if((data->dtype != dtype) || (data->clazz != CLASS_S && data->clazz != CLASS_A))
		throw MdsException("Invalid data type in Array::setElementAt");

    //Check Dimensionality
	if(getNumDims > nDims)
		throw MdsException("Invalid passed dimensions in Array::setElementAt");
	for(i = 0; i < getNumDims; i++)
	{
		if(getDims[i] < 0 || getDims[i] >= dims[i])
			throw MdsException("Invalid passed dimensions in Array::setElementAt");
	}

	//Prepare actual row dimensions
	int *rowDims = new int[nDims];
	rowDims[nDims - 1] = 1;
	for(i = nDims - 2; i >= 0; i--)
		rowDims[i] = rowDims[i+1] * dims[i+1];

	//Compute startIdx of selected data portion
	int startIdx = 0;
	for(i = 0; i < getNumDims; i++)
		startIdx += getDims[i] * rowDims[i];

	if(data->clazz == CLASS_A)
	{
		arrayData = (Array *)data;
        //If passed data is an array, its dimensions must match the remaining dimensions
		if(arrayData->nDims != nDims - getNumDims)
		{
			delete [] rowDims;
			throw MdsException("Invalid passed dimensions in Array::setElementAt");
		}
		for(i = 0; i < arrayData->nDims; i++)
		{
			if(arrayData->dims[i] != dims[getNumDims + i])
			{
				delete [] rowDims;
				throw MdsException("Invalid passed dimensions in Array::setElementAt");
			}
		}
        //Dimensionality check passed: copy passed Array
		memcpy(ptr + (length * startIdx), arrayData->ptr, rowDims[getNumDims-1] * length);
	}
	else //data->clazz == CLASS_S
	{
		scalarData = (Scalar *)data;
        //Propagate the passed scalar to all the remaining dimensions
		for(i = 0; i < rowDims[getNumDims - 1]; i++)
			memcpy(ptr + ((startIdx + i) * length), scalarData->ptr, length); 
	}
	delete [] rowDims;

}



char *Array::getByteArray(int *numElements)
{
	int size = arsize/length;
	char *retArr = new char[arsize/length];
	for(int i = 0; i < size; i++)
	{
		switch(dtype) {

        case DTYPE_B: retArr[i] = numeric_cast<char>(*(char *)&ptr[i * length]); break;
        case DTYPE_BU: retArr[i] = numeric_cast<char>(*(unsigned char *)&ptr[i * length]); break;
        case DTYPE_W: retArr[i] = numeric_cast<char>(*(short *)&ptr[i * length]); break;
        case DTYPE_WU: retArr[i] = numeric_cast<char>(*(unsigned short *)&ptr[i * length]); break;
        case DTYPE_L: retArr[i] = numeric_cast<char>(*(int *)&ptr[i * length]); break;
        case DTYPE_LU: retArr[i] = numeric_cast<char>(*(unsigned int *)&ptr[i * length]); break;
        case DTYPE_Q: retArr[i] = numeric_cast<char>(*(int64_t *)&ptr[i * length]); break;
        case DTYPE_QU: retArr[i] = numeric_cast<char>(*(int64_t *)&ptr[i * length]); break;
        case DTYPE_FLOAT: retArr[i] = numeric_cast<char>(*(float *)&ptr[i * length]); break;
        case DTYPE_DOUBLE: retArr[i] = numeric_cast<char>(*(double *)&ptr[i * length]); break;
        case DTYPE_FSC:
        case DTYPE_FTC:
            delete [] retArr;
            throw MdsException("getByteArray() not supported for Complex data type");
		}
	}
	*numElements = size;
	return retArr;
}
unsigned char *Array::getByteUnsignedArray(int *numElements)
{
	int size = arsize/length;
	unsigned char *retArr = new unsigned char[arsize/length];
	for(int i = 0; i < size; i++)
	{
		switch(dtype) {
        case DTYPE_B: retArr[i] = numeric_cast<unsigned char>(*(char *)&ptr[i * length]); break;
        case DTYPE_BU: retArr[i] = numeric_cast<unsigned char>(*(unsigned char *)&ptr[i * length]); break;
        case DTYPE_W: retArr[i] = numeric_cast<unsigned char>(*(short *)&ptr[i * length]); break;
        case DTYPE_WU: retArr[i] = numeric_cast<unsigned char>(*(unsigned short *)&ptr[i * length]); break;
        case DTYPE_L: retArr[i] = numeric_cast<unsigned char>(*(int *)&ptr[i * length]); break;
        case DTYPE_LU: retArr[i] = numeric_cast<unsigned char>(*(unsigned int *)&ptr[i * length]); break;
        case DTYPE_Q: retArr[i] = numeric_cast<unsigned char>(*(int64_t *)&ptr[i * length]); break;
        case DTYPE_QU: retArr[i] = numeric_cast<unsigned char>(*(int64_t *)&ptr[i * length]); break;
        case DTYPE_FLOAT: retArr[i] = numeric_cast<unsigned char>(*(float *)&ptr[i * length]); break;
        case DTYPE_DOUBLE: retArr[i] = numeric_cast<unsigned char>(*(double *)&ptr[i * length]); break;
        case DTYPE_FSC:
        case DTYPE_FTC:
            delete [] retArr;
            throw MdsException("getByteUnsignedArray() not supported for Complex data type");
		}
	}
	*numElements = size;
	return retArr;
}
short *Array::getShortArray(int *numElements)
{
	short *retArr = new short[arsize/length];
	int size = arsize/length;
	for(int i = 0; i < size; i++)
		switch(dtype) {

        case DTYPE_B: retArr[i] = numeric_cast<short>(*(char *)&ptr[i * length]); break;
        case DTYPE_BU: retArr[i] = numeric_cast<short>(*(unsigned char *)&ptr[i * length]); break;
        case DTYPE_W: retArr[i] = numeric_cast<short>(*(short *)&ptr[i * length]); break;
        case DTYPE_WU: retArr[i] = numeric_cast<short>(*(unsigned short *)&ptr[i * length]); break;
        case DTYPE_L: retArr[i] = numeric_cast<short>(*(int *)&ptr[i * length]); break;
        case DTYPE_LU: retArr[i] = numeric_cast<short>(*(unsigned int *)&ptr[i * length]); break;
        case DTYPE_Q: retArr[i] = numeric_cast<short>(*(int64_t *)&ptr[i * length]); break;
        case DTYPE_QU: retArr[i] = numeric_cast<short>(*(int64_t *)&ptr[i * length]); break;
        case DTYPE_FLOAT: retArr[i] = numeric_cast<short>(*(float *)&ptr[i * length]); break;
        case DTYPE_DOUBLE: retArr[i] = numeric_cast<short>(*(double *)&ptr[i * length]); break;
        case DTYPE_FSC:
        case DTYPE_FTC:
            delete [] retArr;
            throw MdsException("getShortArray() not supported for Complex data type");
		}
	*numElements = size;
	return retArr;
}
unsigned short *Array::getShortUnsignedArray(int *numElements)
{
	unsigned short *retArr = new unsigned short[arsize/length];
	int size = arsize/length;
	for(int i = 0; i < size; i++)
		switch(dtype) {
        case DTYPE_B: retArr[i] = numeric_cast<unsigned short>(*(char *)&ptr[i * length]); break;
        case DTYPE_BU: retArr[i] = numeric_cast<unsigned short>(*(unsigned char *)&ptr[i * length]); break;
        case DTYPE_W: retArr[i] = numeric_cast<unsigned short>(*(short *)&ptr[i * length]); break;
        case DTYPE_WU: retArr[i] = numeric_cast<unsigned short>(*(unsigned short *)&ptr[i * length]); break;
        case DTYPE_L: retArr[i] = numeric_cast<unsigned short>(*(int *)&ptr[i * length]); break;
        case DTYPE_LU: retArr[i] = numeric_cast<unsigned short>(*(unsigned int *)&ptr[i * length]); break;
        case DTYPE_Q: retArr[i] = numeric_cast<unsigned short>(*(int64_t *)&ptr[i * length]); break;
        case DTYPE_QU: retArr[i] = numeric_cast<unsigned short>(*(int64_t *)&ptr[i * length]); break;
        case DTYPE_FLOAT: retArr[i] = numeric_cast<unsigned short>(*(float *)&ptr[i * length]); break;
        case DTYPE_DOUBLE: retArr[i] = numeric_cast<unsigned short>(*(double *)&ptr[i * length]); break;
        case DTYPE_FSC:
        case DTYPE_FTC:
            delete [] retArr;
            throw MdsException("getShortUnsignedArray() not supported for Complex data type");
		}
	*numElements = size;
	return retArr;
}
int *Array::getIntArray(int *numElements)
{
	int size = arsize/length;
	int *retArr = new int[size];
	for(int i = 0; i < size; i++)
		switch(dtype) {
        case DTYPE_B: retArr[i] = numeric_cast<int>(*(char *)&ptr[i * length]); break;
        case DTYPE_BU: retArr[i] = numeric_cast<int>(*(unsigned char *)&ptr[i * length]); break;
        case DTYPE_W: retArr[i] = numeric_cast<int>(*(short *)&ptr[i * length]); break;
        case DTYPE_WU: retArr[i] = numeric_cast<int>(*(unsigned short *)&ptr[i * length]); break;
        case DTYPE_L: retArr[i] = numeric_cast<int>(*(int *)&ptr[i * length]); break;
        case DTYPE_LU: retArr[i] = numeric_cast<int>(*(unsigned int *)&ptr[i * length]); break;
        case DTYPE_Q: retArr[i] = numeric_cast<int>(*(int64_t *)&ptr[i * length]); break;
        case DTYPE_QU: retArr[i] = numeric_cast<int>(*(int64_t *)&ptr[i * length]); break;
        case DTYPE_FLOAT: retArr[i] = numeric_cast<int>(*(float *)&ptr[i * length]); break;
        case DTYPE_DOUBLE: retArr[i] = numeric_cast<int>(*(double *)&ptr[i * length]); break;
        case DTYPE_FSC:
        case DTYPE_FTC:
            delete [] retArr;
            throw MdsException("getIntArray() not supported for Complex data type");
		}
	*numElements = size;
	return retArr;
}
unsigned int *Array::getIntUnsignedArray(int *numElements)
{
	int size = arsize/length;
	unsigned int *retArr = new unsigned int[size];
	for(int i = 0; i < size; i++)
		switch(dtype) {
        case DTYPE_B: retArr[i] = numeric_cast<unsigned int>(*(char *)&ptr[i * length]); break;
        case DTYPE_BU: retArr[i] = numeric_cast<unsigned int>(*(unsigned char *)&ptr[i * length]); break;
        case DTYPE_W: retArr[i] = numeric_cast<unsigned int>(*(short *)&ptr[i * length]); break;
        case DTYPE_WU: retArr[i] = numeric_cast<unsigned int>(*(unsigned short *)&ptr[i * length]); break;
        case DTYPE_L: retArr[i] = numeric_cast<unsigned int>(*(int *)&ptr[i * length]); break;
        case DTYPE_LU: retArr[i] = numeric_cast<unsigned int>(*(unsigned int *)&ptr[i * length]); break;
        case DTYPE_Q: retArr[i] = numeric_cast<unsigned int>(*(int64_t *)&ptr[i * length]); break;
        case DTYPE_QU: retArr[i] = numeric_cast<unsigned int>(*(int64_t *)&ptr[i * length]); break;
        case DTYPE_FLOAT: retArr[i] = numeric_cast<unsigned int>(*(float *)&ptr[i * length]); break;
        case DTYPE_DOUBLE: retArr[i] = numeric_cast<unsigned int>(*(double *)&ptr[i * length]); break;
        case DTYPE_FSC:
        case DTYPE_FTC:
            delete [] retArr;
            throw MdsException("getIntUnsignedArray() not supported for Complex data type");
		}
	*numElements = size;
	return retArr;
}
int64_t *Array::getLongArray(int *numElements)
{
	int size = arsize/length;
	int64_t *retArr = new int64_t[size];
	for(int i = 0; i < size; i++)
		switch(dtype) {
        case DTYPE_B: retArr[i] = numeric_cast<long>(*(char *)&ptr[i * length]); break;
        case DTYPE_BU: retArr[i] = numeric_cast<long>(*(unsigned char *)&ptr[i * length]); break;
        case DTYPE_W: retArr[i] = numeric_cast<long>(*(short *)&ptr[i * length]); break;
        case DTYPE_WU: retArr[i] = numeric_cast<long>(*(unsigned short *)&ptr[i * length]); break;
        case DTYPE_L: retArr[i] = numeric_cast<long>(*(int *)&ptr[i * length]); break;
        case DTYPE_LU: retArr[i] = numeric_cast<long>(*(unsigned int *)&ptr[i * length]); break;
        case DTYPE_Q: retArr[i] = numeric_cast<long>(*(int64_t *)&ptr[i * length]); break;
        case DTYPE_QU: retArr[i] = numeric_cast<long>(*(int64_t *)&ptr[i * length]); break;
        case DTYPE_FLOAT: retArr[i] = numeric_cast<long>(*(float *)&ptr[i * length]); break;
        case DTYPE_DOUBLE: retArr[i] = numeric_cast<long>(*(double *)&ptr[i * length]); break;
        case DTYPE_FSC:
        case DTYPE_FTC:
            delete [] retArr;
            throw MdsException("getLongArray() not supported for Complex data type");
		}
	*numElements = size;
	return retArr;
}

uint64_t *Array::getLongUnsignedArray(int *numElements)
{
	int size = arsize/length;
	uint64_t *retArr = new uint64_t[size];
	for(int i = 0; i < size; i++)
		switch(dtype) {
        case DTYPE_B: retArr[i] = numeric_cast<unsigned long>(*(char *)&ptr[i * length]); break;
        case DTYPE_BU: retArr[i] = numeric_cast<unsigned long>(*(unsigned char *)&ptr[i * length]); break;
        case DTYPE_W: retArr[i] = numeric_cast<unsigned long>(*(short *)&ptr[i * length]); break;
        case DTYPE_WU: retArr[i] = numeric_cast<unsigned long>(*(unsigned short *)&ptr[i * length]); break;
        case DTYPE_L: retArr[i] = numeric_cast<unsigned long>(*(int *)&ptr[i * length]); break;
        case DTYPE_LU: retArr[i] = numeric_cast<unsigned long>(*(unsigned int *)&ptr[i * length]); break;
        case DTYPE_Q: retArr[i] = numeric_cast<unsigned long>(*(int64_t *)&ptr[i * length]); break;
        case DTYPE_QU: retArr[i] = numeric_cast<unsigned long>(*(int64_t *)&ptr[i * length]); break;
        case DTYPE_FLOAT: retArr[i] = numeric_cast<unsigned long>(*(float *)&ptr[i * length]); break;
        case DTYPE_DOUBLE: retArr[i] = numeric_cast<unsigned long>(*(double *)&ptr[i * length]); break;
        case DTYPE_FSC:
        case DTYPE_FTC:
            delete [] retArr;
            throw MdsException("getLongUnsignedArray() not supported for Complex data type");
		}
	*numElements = size;
	return retArr;
}

float *Array::getFloatArray(int *numElements)
{
	int size = arsize/length;
	float *retArr = new float[size];
	for(int i = 0; i < size; i++)
		switch(dtype) {
        case DTYPE_B: retArr[i] = numeric_cast<float>(*(char *)&ptr[i * length]); break;
        case DTYPE_BU: retArr[i] = numeric_cast<float>(*(unsigned char *)&ptr[i * length]); break;
        case DTYPE_W: retArr[i] = numeric_cast<float>(*(short *)&ptr[i * length]); break;
        case DTYPE_WU: retArr[i] = numeric_cast<float>(*(unsigned short *)&ptr[i * length]); break;
        case DTYPE_L: retArr[i] = numeric_cast<float>(*(int *)&ptr[i * length]); break;
        case DTYPE_LU: retArr[i] = numeric_cast<float>(*(unsigned int *)&ptr[i * length]); break;
        case DTYPE_Q: retArr[i] = numeric_cast<float>(*(int64_t *)&ptr[i * length]); break;
        case DTYPE_QU: retArr[i] = numeric_cast<float>(*(int64_t *)&ptr[i * length]); break;
        case DTYPE_FLOAT: retArr[i] = numeric_cast<float>(*(float *)&ptr[i * length]); break;
        case DTYPE_DOUBLE: retArr[i] = numeric_cast<float>(*(double *)&ptr[i * length]); break;
        case DTYPE_FSC:
        case DTYPE_FTC:
            delete [] retArr;
            throw MdsException("getFloatArray() not supported for Complex data type");
		}
	*numElements = size;
	return retArr;
}
double *Array::getDoubleArray(int *numElements)
{
	int size = arsize/length;
	double *retArr = new double[size];
	for(int i = 0; i < size; i++)
		switch(dtype) {
        case DTYPE_B: retArr[i] = numeric_cast<double>(*(char *)&ptr[i * length]); break;
        case DTYPE_BU: retArr[i] = numeric_cast<double>(*(unsigned char *)&ptr[i * length]); break;
        case DTYPE_W: retArr[i] = numeric_cast<double>(*(short *)&ptr[i * length]); break;
        case DTYPE_WU: retArr[i] = numeric_cast<double>(*(unsigned short *)&ptr[i * length]); break;
        case DTYPE_L: retArr[i] = numeric_cast<double>(*(int *)&ptr[i * length]); break;
        case DTYPE_LU: retArr[i] = numeric_cast<double>(*(unsigned int *)&ptr[i * length]); break;
        case DTYPE_Q: retArr[i] = numeric_cast<double>(*(int64_t *)&ptr[i * length]); break;
        case DTYPE_QU: retArr[i] = numeric_cast<double>(*(int64_t *)&ptr[i * length]); break;
        case DTYPE_FLOAT: retArr[i] = numeric_cast<double>(*(float *)&ptr[i * length]); break;
        case DTYPE_DOUBLE: retArr[i] = numeric_cast<double>(*(double *)&ptr[i * length]); break;
        case DTYPE_FSC:
        case DTYPE_FTC:
            delete [] retArr;
            throw MdsException("getDoubleArray() not supported for Complex data type");
		}
	*numElements = size;
	return retArr;
}

std::complex<double> *Array::getComplexArray(int *numElements)
{
	int size = arsize/length;
	std::complex<double> *retArr = new std::complex<double>[size];
	for(int i = 0; i < size; i++)
		switch(dtype) {
			case DTYPE_FSC:
				retArr[i] = std::complex<double>(((float *)ptr)[2*i], ((float *)ptr)[2*i+1]); break;
			case DTYPE_FTC: 
				retArr[i] = std::complex<double>(((double *)ptr)[2*i], ((double *)ptr)[2*i+1]); break;
            default:
            delete [] retArr;
            throw MdsException("getComplexArray() not supported for non Complex data types");

		}
	*numElements = size;
	return retArr;
}

char **Array::getStringArray(int *numElements)
{
	int size = (length > 0)? arsize/length : 0;
	char **retArr = new char*[size];
	for(int i = 0; i < size; i++)
		switch(dtype) {
			case DTYPE_T: 
				retArr[i] = new char[length+1];
				memcpy(retArr[i], &ptr[i*length], length);
				retArr[i][length] = 0;
				for(int j = length -1; j > 0; j--)
					if(retArr[i][j] == ' ')
						retArr[i][j] = 0;
					else
						break;
				break;
			default: throw MdsException("Unsupported conversion to char ** array");

		}
	*numElements = size;
	return retArr;
}

bool String::equals(Data *data) {
    if(data->clazz != clazz || data->dtype != dtype)
        return false;

    AutoString second(((String *)data)->getString());
    return std::string(ptr) == second.string;
}

char *Uint64::getDate()
{
	char dateBuf[512];
	int bufLen;
	convertTimeToAscii((int64_t *)ptr, dateBuf, 512, &bufLen);
	char *retDate = new char[bufLen+1];
	dateBuf[bufLen] = '\0';
	strcpy(retDate, dateBuf);
	return retDate;
}


void *TreePath::convertToDsc()
{
	return completeConversionToDsc(convertToScalarDsc(clazz, dtype, path.length(), const_cast<char *>(path.c_str())));
}

 void *Array::convertToDsc()
{
	return completeConversionToDsc(convertToArrayDsc(clazz, dtype, length, arsize, nDims, dims, ptr));
}

void *Compound::convertToDsc()
{
	return completeConversionToDsc(convertToCompoundDsc(clazz, dtype, sizeof(short), (void *)&opcode, descs.size(), (void **)(&descs[0])));
}

void *Apd::convertToDsc()
{
	return completeConversionToDsc(convertToApdDsc(dtype, descs.size(), (void **)&descs[0]));
}


Data *MDSplus::deserialize(char const * serialized)
{
	void *dscPtr = deserializeData(serialized);
	if(!dscPtr) throw MdsException("Cannot build Data instance from serialized content");
	Data *retData = (Data *)convertFromDsc(dscPtr);
	freeDsc(dscPtr);
	return retData;
}

Data *MDSplus::deserialize(Data *serializedData)
{
	Uint8Array *serializedArr = (Uint8Array *)serializedData;
	return deserialize((const char *)serializedArr->ptr);
}

/*ostream& operator<<(ostream& output, Data *data)
{
//	output << data->decompile();
//	return output;
	char * str = data->decompile();
	output << str;
	delete[] str;
    return output;
}
*/
std::ostream& MDSplus::operator<<(std::ostream& output, Data *data)
{
//	output << data->decompile();
//	return output;
	char * str = data->decompile();
	output << str;
	delete[] str;
    return output;
}
Data *Uint8Array::deserialize()
{
    return (Data *)deserializeData(ptr);
}

void Scope::show()
{
	char expr[256];
	std::sprintf(expr, "JavaShowWindow(%d, %d, %d, %d, %d)", idx, x, y, width, height);
	Data *ris = execute(expr);
	deleteData(ris);
}
Scope::Scope(const char *name, int x, int y, int width, int height)
{
	std::string expr("JavaNewWindow(" + std::string(name) + ", -1");
	Data *ris = execute(expr.c_str());
	idx = ris->getInt();
	deleteData(ris);
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	show();
}
void Scope::plot(Data *x, Data *y , int row, int col, const char *color)
{
	char expr[256];
	std::sprintf(expr, "JavaReplaceSignal(%d, $1, $2, %d, %d, \"%s\")", idx, row, col, color);
	Data *ris = executeWithArgs(expr, 2, x, y);
	deleteData(ris);
}
void Scope::oplot(Data *x, Data *y , int row, int col, const char *color)
{
	char expr[256];
	std::sprintf(expr, "JavaAddSignal(%d, $1, $2, %d, %d, \"%s\")", idx, row, col, color);
	Data *ris = executeWithArgs(expr, 2, x, y);
	deleteData(ris);
}

