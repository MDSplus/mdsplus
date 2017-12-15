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
#include "mdsobjectswrp.h"
#include <string>
#include <cstring>

void fillErrorCluster(MgErr code, const char *source, const char *message, ErrorCluster *error)
{
	if (code) {
		std::string errMsg(source);
		errMsg += ".<ERR>.";
		errMsg += message;
		errMsg += ".";

		int32 errMsgLen = static_cast<int32>(errMsg.size());

		error->status = LVBooleanTrue;
		error->code = static_cast<int32>(code);
		if(NumericArrayResize((int32)uB, (int32)1, reinterpret_cast<UHandle *>(&(error->source)), errMsgLen + sizeof(int32)) == noErr)
		{
			MoveBlock(errMsg.c_str(), LStrBuf(*error->source), errMsgLen);
			(*error->source)->cnt = errMsgLen;
		}
	}
}

/*

void fillErrorCluster(MgErr code, const char *source, const char *message, ErrorCluster *error)

{
	if (code)
	{
		char *errMsg = new char[strlen(source) + strlen(message) + strlen("<ERR>..")+1];
		strcpy(errMsg, source);
		strcat(errMsg, ".");
		strcat(errMsg, "<ERR>");
		strcat(errMsg, message);
		strcat(errMsg, ".");
		int32 errMsgLen = static_cast<int32>(strlen(errMsg));
		error->status = LVBooleanTrue;
		error->code = static_cast<int32>(code);
		NumericArrayResize(uB, 1, reinterpret_cast<UHandle *>(&(error->source)), sizeof(int32) + errMsgLen);
		MoveBlock(reinterpret_cast<uChar *>(errMsg), LStrBuf(*error->source), errMsgLen);
		//strncpy((char *)((*(error->source))->str), reinterpret_cast<const char *>(errMsg), errMsgLen);
		(*(error->source))->cnt = errMsgLen;
		delete[] errMsg;
	}

}

*/
/*
typedef struct {
	LVBoolean status;
	int32 code;
	LStrHandle source;
} ErrorCluster;
*/



namespace MDSplus {

template<class T>
static void safeCall(char const * src, ErrorCluster * error, T f) {
	MgErr errorCode = noErr;
	char const * errorMessage = (char *)"";

	try {
		f();
		fillErrorCluster(errorCode, src, errorMessage, error);
	} catch (MdsException const & e) {
		errorCode = bogusError;
		errorMessage = e.what();
		fillErrorCluster(errorCode, src, errorMessage, error);
	}

//	fillErrorCluster(errorCode, src, errorMessage, error); errorMessage reters to an exception out of scope???
}

struct Lambda {
	virtual void operator()() = 0;
	virtual ~Lambda() {}
};

static void deleteLvData(void ** data) {
	deleteData(reinterpret_cast<Data *>(*data));
}

template<class T>
struct Constructor: public Lambda {
	Constructor(void ** ptr):
		ptr(*ptr) {}
	void * & ptr;
	void operator()() {
		ptr = reinterpret_cast<void *>(new T());
	}
};

template <class T, class V>
struct ScalarConstructor: public Constructor<Scalar> {
	ScalarConstructor(void ** ptr, V value):
		Constructor(ptr), value(value) {}
	V value;
	void operator()() {
		ptr = reinterpret_cast<void *>(new T(value));
	}
};

template<class T, class V>
static void scalarConstructor(char const * src, ErrorCluster * error, void ** ptr, V value) {
	safeCall(src, error, ScalarConstructor<T, V>(ptr, value));
}

template <class T, class V>
struct ArrayConstructor: public Constructor<Array> {
	ArrayConstructor(void ** ptr, V inPtr, const LIntArrHdl dims):
		Constructor(ptr), inPtr(inPtr), dims(dims) {}
	V inPtr;
	const LIntArrHdl dims;
	void operator()() {
//WRONG. Make LabVIEW crash!!!!	ptr = reinterpret_cast<void *>(new T((*inPtr)->elt, (*dims)->dimSize), (*dims)->elt);
		ptr = reinterpret_cast<void *>(new T((*inPtr)->elt, (*dims)->dimSize, reinterpret_cast<int *>((*dims)->elt)));
	}
};

template<class T, class V>
static void arrayConstructor(char const * src, ErrorCluster * error, void ** ptr, V inPtr, const LIntArrHdl dims) {
	safeCall(src, error, ArrayConstructor<T, V>(ptr, inPtr, dims));
}

template<class T, class V>
static void arrayConstructor(char const * src, ErrorCluster * error, void ** ptr, V inPtr) {
	LIntArr dims;
	dims.dimSize = 1;
// WRONG. make labVIEW crash!!!	dims.elt[1] = (*inPtr)->dimSize;
	dims.elt[0] = (*inPtr)->dimSize;
//WRONG. make LabVIEW crash !!! LIntArr * dimPtr;
	LIntArr * dimPtr = &dims;
	safeCall(src, error, ArrayConstructor<T, V>(ptr, inPtr, &dimPtr));
}

// FIXME: If this doesn't work, add in another template param to cast to instead of
// FIXME: casting to Data directly (like Float32)
// FIXME: If this does work, then every derived type can just call the data function
// FIXME: for instance, mdsplus_float32_getInt can just call mdsplus_data_getInt
template<class T>
static T getScalar(void const * b, T (Data::*getX)(), char const * src, ErrorCluster * error) {
	MgErr errorCode = noErr;
	char const * errorMessage = (char *)"";

	T x = 0;
	try {
		Data * data = reinterpret_cast<Data *>(const_cast<void *>(b));
		x = (data->*getX)();
		fillErrorCluster(errorCode, src, errorMessage, error);
	} catch ( const MdsException & e) {
		errorCode = bogusError;
		errorMessage = e.what();
		fillErrorCluster(errorCode, src, errorMessage, error);
	}
//	fillErrorCluster(errorCode, src, errorMessage, error);

	return x;
}

/********************************************************************************************************
												ARRAY
 ********************************************************************************************************/
extern "C" {

EXPORT void mdsplus_array_constructor(void **lvArrayPtrOut, ErrorCluster *error) {
	safeCall(__func__, error, Constructor<Array>(lvArrayPtrOut));
}

EXPORT void mdsplus_array_destructor(void **lvArrayPtr) {
	deleteLvData(lvArrayPtr);
}

EXPORT void mdsplus_array_getByteArray(const void *lvArrayPtr, LByteArrHdl lvByteArrHdlOut, ErrorCluster *error)

{

	Array *arrayPtr = NULL;

	char *byteArrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const *errorMessage = (char *)"";

	try

	{

		arrayPtr = reinterpret_cast<Array *>(const_cast<void *>(lvArrayPtr));

		int byteArrLen = 0;

		byteArrOut = arrayPtr->getByteArray(&byteArrLen);

		errorCode = NumericArrayResize(iB, 1, reinterpret_cast<UHandle *>(&lvByteArrHdlOut), static_cast<int32>(byteArrLen));

		if (!errorCode) {

			for (int i = 0; i < byteArrLen; i++)

				(*lvByteArrHdlOut)->elt[i] = static_cast<int8>(byteArrOut[i]);

			(*lvByteArrHdlOut)->dimSize = static_cast<int32>(byteArrLen);

		}

		else

			errorMessage = (char *)"NumericArrayResize error";

		deleteNativeArray(byteArrOut);

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
	}

	catch (const MdsException & e)

	{

		errorCode = bogusError;

		errorMessage = e.what();

	      fillErrorCluster(errorCode, errorSource, errorMessage, error);
	}

//	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_array_getDoubleArray(const void *lvArrayPtr, LDblArrHdl lvDblArrHdlOut, ErrorCluster *error)

{

	Array *arrayPtr = NULL;

	double *doubleArrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const *errorMessage = (char *)"";

	try

	{

		arrayPtr = reinterpret_cast<Array *>(const_cast<void *>(lvArrayPtr));

		int doubleArrLen = 0;

		doubleArrOut = arrayPtr->getDoubleArray(&doubleArrLen);

		errorCode = NumericArrayResize(fD, 1, reinterpret_cast<UHandle *>(&lvDblArrHdlOut), static_cast<int32>(doubleArrLen));

		if (!errorCode) {

			for (int i = 0; i < doubleArrLen; i++)

				(*lvDblArrHdlOut)->elt[i] = doubleArrOut[i];

			(*lvDblArrHdlOut)->dimSize = static_cast<int32>(doubleArrLen);

		}

		else

			errorMessage = (char *)"NumericArrayResize error";

		deleteNativeArray(doubleArrOut);

	}

	catch (const MdsException & e)

	{

		errorCode = bogusError;

		errorMessage = e.what();

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}

}



EXPORT void mdsplus_array_getElementAt(const void *lvArrayPtr, void **lvDataPtrOut, int dimIn, ErrorCluster *error)

{

	Array *arrayPtr = NULL;

	Data *dataPtrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const *errorMessage = (char *)"";

	try

	{

		arrayPtr = reinterpret_cast<Array *>(const_cast<void *>(lvArrayPtr));

		dataPtrOut = arrayPtr->getElementAt(dimIn);

		*lvDataPtrOut = reinterpret_cast<void *>(dataPtrOut);

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
	}

	catch (const MdsException & e)

	{


		errorCode = bogusError;

		errorMessage = e.what();
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
	}

//	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_array_getElementAt_dims(const void *lvArrayPtr, void **lvDataPtrOut, const LIntArrHdl lvIntArrHdlIn, ErrorCluster *error)

{

	Array *arrayPtr = NULL;

	Data *dataPtrOut = NULL;

	int *intArr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const *errorMessage = (char *)"";

	try

	{

		int intArrLen = static_cast<int>((*lvIntArrHdlIn)->dimSize);

		intArr = new int[intArrLen];

		for (int i = 0; i < intArrLen; i++)

			intArr[i] = static_cast<int>((*lvIntArrHdlIn)->elt[i]);

		arrayPtr = reinterpret_cast<Array *>(const_cast<void *>(lvArrayPtr));

		dataPtrOut = arrayPtr->getElementAt(intArr, intArrLen);

		delete[] intArr;

		*lvDataPtrOut = reinterpret_cast<void *>(dataPtrOut);

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
	}

	catch (const MdsException & e)

	{

		errorCode = bogusError;

		errorMessage = e.what();
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
	}

//	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_array_getFloatArray(const void *lvArrayPtr, LFltArrHdl lvFltArrHdlOut, ErrorCluster *error)

{

	Array *arrayPtr = NULL;

	float *floatArrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const *errorMessage = (char *)"";

	try

	{

		arrayPtr = reinterpret_cast<Array *>(const_cast<void *>(lvArrayPtr));

		int floatArrLen = 0;

		floatArrOut = arrayPtr->getFloatArray(&floatArrLen);

		errorCode = NumericArrayResize(fS, 1, reinterpret_cast<UHandle *>(&lvFltArrHdlOut), static_cast<int32>(floatArrLen));

		if (!errorCode) {

			for (int i = 0; i < floatArrLen; i++)

				(*lvFltArrHdlOut)->elt[i] = floatArrOut[i];

			(*lvFltArrHdlOut)->dimSize = static_cast<int32>(floatArrLen);

		}

		else

			errorMessage = (char *)"NumericArrayResize error";

		deleteNativeArray(floatArrOut);

	}

	catch (const MdsException & e)

	{

		errorCode = bogusError;

		errorMessage = e.what();

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_array_getInfo(const void *lvArrayPtr, char *clazzOut, char *dtypeOut, short *lengthOut, LIntArrHdl lvIntArrHdlOut, ErrorCluster *error)

{

	Array *arrayPtr = NULL;

	char nDims = 0;

	int *dims = NULL;

	void *ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;
	
	char const * errorMessage = "";

	try

	{

		arrayPtr = reinterpret_cast<Array *>(const_cast<void *>(lvArrayPtr));

		arrayPtr->getInfo(clazzOut, dtypeOut, lengthOut, &nDims, &dims, &ptr);

		errorCode = NumericArrayResize(iL, 1, reinterpret_cast<UHandle *>(&lvIntArrHdlOut), static_cast<int32>(nDims));

		if (!errorCode) {

			for (int i = 0; i < nDims; i++)

				(*lvIntArrHdlOut)->elt[i] = static_cast<int32>(dims[i]);

			(*lvIntArrHdlOut)->dimSize = static_cast<int32>(nDims);

		}

		else

			errorMessage = (char *)"NumericArrayResize error";

		// ptr is not returned

		deleteNativeArray(dims);

	}

	catch (const MdsException & mdsE)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(mdsE.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_array_getIntArray(const void *lvArrayPtr, LIntArrHdl lvIntArrHdlOut, ErrorCluster *error)

{

	Array *arrayPtr = NULL;

	int *intArrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		arrayPtr = reinterpret_cast<Array *>(const_cast<void *>(lvArrayPtr));

		int intArrLen = 0;

		intArrOut = arrayPtr->getIntArray(&intArrLen);

		errorCode = NumericArrayResize(iL, 1, reinterpret_cast<UHandle *>(&lvIntArrHdlOut), static_cast<int32>(intArrLen));

		if (!errorCode) {

			for (int i = 0; i < intArrLen; i++)

				(*lvIntArrHdlOut)->elt[i] = static_cast<int32>(intArrOut[i]);

			(*lvIntArrHdlOut)->dimSize = static_cast<int32>(intArrLen);

		}

		else

			errorMessage = (char *)"NumericArrayResize error";

		deleteNativeArray(intArrOut);

	}

	catch (const MdsException & mdsE)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(mdsE.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_array_getLongArray(const void *lvArrayPtr, LLngArrHdl lvLngArrHdlOut, ErrorCluster *error)

{

	Array *arrayPtr = NULL;

	int64_t *longArrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		arrayPtr = reinterpret_cast<Array *>(const_cast<void *>(lvArrayPtr));

		int longArrLen = 0;

		longArrOut = arrayPtr->getLongArray(&longArrLen);

		errorCode = NumericArrayResize(iQ, 1, reinterpret_cast<UHandle *>(&lvLngArrHdlOut), static_cast<int32>(longArrLen));

		if (!errorCode) {

			for (int i = 0; i < longArrLen; i++)

				(*lvLngArrHdlOut)->elt[i] = static_cast<int64>(longArrOut[i]);

			(*lvLngArrHdlOut)->dimSize = static_cast<int32>(longArrLen);

		}

		else

			errorMessage = (char *)"NumericArrayResize error";

		deleteNativeArray(longArrOut);

	}

	catch (const MdsException & mdsE)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(mdsE.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}



	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_array_getShape(const void *lvArrayPtr, LIntArrHdl lvIntArrHdlOut, ErrorCluster *error)

{

	Array *arrayPtr = NULL;

	int *intArrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		arrayPtr = reinterpret_cast<Array *>(const_cast<void *>(lvArrayPtr));

		int intArrLen = 0;

		intArrOut = arrayPtr->getShape(&intArrLen);

		errorCode = NumericArrayResize(iL, 1, reinterpret_cast<UHandle *>(&lvIntArrHdlOut), static_cast<int32>(intArrLen));

		if (!errorCode) {

			for (int i = 0; i < intArrLen; i++)

				(*lvIntArrHdlOut)->elt[i] = static_cast<int32>(intArrOut[i]);

			(*lvIntArrHdlOut)->dimSize = static_cast<int32>(intArrLen);

		}

		else

			errorMessage = (char *)"NumericArrayResize error";

		deleteNativeArray(intArrOut);

	}

	catch (const MdsException & mdsE)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(mdsE.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_array_getShortArray(const void *lvArrayPtr, LShtArrHdl lvShtArrHdlOut, ErrorCluster *error)

{

	Array *arrayPtr = NULL;

	short *shortArrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		arrayPtr = reinterpret_cast<Array *>(const_cast<void *>(lvArrayPtr));

		int shortArrLen = 0;

		shortArrOut = arrayPtr->getShortArray(&shortArrLen);

		errorCode = NumericArrayResize(iW, 1, reinterpret_cast<UHandle *>(&lvShtArrHdlOut), static_cast<int32>(shortArrLen));

		if (!errorCode) {

			for (int i = 0; i < shortArrLen; i++)

				(*lvShtArrHdlOut)->elt[i] = static_cast<int16>(shortArrOut[i]);

			(*lvShtArrHdlOut)->dimSize = static_cast<int32>(shortArrLen);

		}

		else

			errorMessage = (char *)"NumericArrayResize error";

		deleteNativeArray(shortArrOut);

	}

	catch (const MdsException & mdsE)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(mdsE.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_array_getSize(const void *lvArrayPtr, int *sizeOut, ErrorCluster *error)

{

	Array *arrayPtr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		arrayPtr = reinterpret_cast<Array *>(const_cast<void *>(lvArrayPtr));

		*sizeOut = arrayPtr->getSize();

	}

	catch (const MdsException & mdsE)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(mdsE.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}

EXPORT void mdsplus_array_getStringArray(const void *lvArrayPtr, LStrArrHdl lvStrArrHdlOut, ErrorCluster *error) {
	MgErr errorCode = noErr;
	char const * errorSource = __func__;
	char const * errorMessage = (char *)"";

	char **stringArrOut = NULL;
	int stringArrLen = 0;

	try {
		Array * arrayPtr = reinterpret_cast<Array *>(const_cast<void *>(lvArrayPtr));
		stringArrOut = arrayPtr->getStringArray(&stringArrLen);

		// checks whether the size of a pointer is 32 bit or 64 bit depending upon the system architecture
		int32 typeCode = (sizeof(LStrHandle) > sizeof(int32)) ? iQ : iL;

		// resizes string array
		errorCode = NumericArrayResize(typeCode, 1, reinterpret_cast<UHandle *>(&lvStrArrHdlOut), static_cast<int32>(stringArrLen));

		if (!errorCode) {
			int i = 0;

			// creates LStrHandle strings and fills them with the stringArrOut corrispondent strings
			while (!errorCode && i < stringArrLen) {
				LStrHandle currLStrHdl = reinterpret_cast<LStrHandle>(DSNewHandle(sizeof(LStrHandle)));

				errorCode = currLStrHdl == NULL;

				if (!errorCode) {
					std::string currStr(stringArrOut[i]);

					errorCode = NumericArrayResize(uB, 1, reinterpret_cast<UHandle *>(&currLStrHdl), currStr.size() + sizeof(int32));

					if (!errorCode) {
						MoveBlock(currStr.c_str(), LStrBuf(*currLStrHdl), currStr.size());
						(*currLStrHdl)->cnt = currStr.size();
						(*lvStrArrHdlOut)->elm[i++] = currLStrHdl;
					} else
						errorMessage = (char *)"NumericArrayResize error";
				} else
					errorMessage = (char *)"DSNewHandle error";
			}

			// keeps only well allocated string handles, till the ith string
			(*lvStrArrHdlOut)->dimSize = static_cast<int32>(i);
		} else
			errorMessage = (char *)"NumericArrayResize error";

	} catch (const MdsException & e) {
		errorCode = bogusError;
		errorMessage = e.what();
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	// free memory
	for (int i = 0; i < stringArrLen; ++i)
		deleteNativeArray(stringArrOut[i]);
	deleteNativeArray(stringArrOut);

	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_array_setElementAt(const void *lvArrayPtr, int dimIn, const void *lvDataPtrIn, ErrorCluster *error)

{

	Array *arrayPtr = NULL;

	Data *dataPtrIn = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		arrayPtr = reinterpret_cast<Array *>(const_cast<void *>(lvArrayPtr));

		dataPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvDataPtrIn));

		arrayPtr->setElementAt(dimIn, dataPtrIn);

	}

	catch (const MdsException & mdsE)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(mdsE.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_array_setElementAt_dims(const void *lvArrayPtr, const LIntArrHdl lvIntArrHdlIn, const void *lvDataPtrIn, ErrorCluster *error)

{

	Array *arrayPtr = NULL;

	Data *dataPtrIn = NULL;

	int *intArr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		int intArrLen = static_cast<int>((*lvIntArrHdlIn)->dimSize);

		intArr = new int[intArrLen];

		for (int i = 0; i < intArrLen; i++)

			intArr[i] = static_cast<int>((*lvIntArrHdlIn)->elt[i]);

		arrayPtr = reinterpret_cast<Array *>(const_cast<void *>(lvArrayPtr));

		dataPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvDataPtrIn));

		arrayPtr->setElementAt(intArr, intArrLen, dataPtrIn);

		delete[] intArr;

	}

	catch (const MdsException & mdsE)

	{


		errorCode = bogusError;

		errorMessage = const_cast<char *>(mdsE.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}

/********************************************************************************************************
												COMPOUND
 ********************************************************************************************************/

EXPORT void mdsplus_compound_constructor(void **lvCompoundPtrOut, ErrorCluster *error) {
	safeCall(__func__, error, Constructor<Compound>(lvCompoundPtrOut));
}

EXPORT void mdsplus_compound_destructor(void **lvCompoundPtr) {
	deleteLvData(lvCompoundPtr);
}

/********************************************************************************************************

												DATA

 ********************************************************************************************************/



EXPORT void mdsplus_data_compile(void **lvDataPtrOut, const char *exprIn, ErrorCluster *error)

{

	Data *dataPtrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		dataPtrOut = compile(const_cast<char *>(exprIn));

		*lvDataPtrOut = reinterpret_cast<void *>(dataPtrOut);

	}

	catch (const MdsException & mdsE)

	{


		errorCode = bogusError;

		errorMessage = const_cast<char *>(mdsE.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_data_compile_tree(void **lvDataPtrOut, const char *exprIn, const void *lvTreePtrIn, ErrorCluster *error)

{

	Data *dataPtrOut = NULL;

	Tree *treePtrIn = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		treePtrIn = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtrIn));

		dataPtrOut = compile(const_cast<char *>(exprIn), treePtrIn);

		*lvDataPtrOut = reinterpret_cast<void *>(dataPtrOut);

	}

	catch (const MdsException & mdsE)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(mdsE.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_data_data(const void *lvDataPtr, void **lvDataPtrOut, ErrorCluster *error)

{

	Data *dataPtr = NULL;

	Data *dataPtrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		dataPtr = reinterpret_cast<Data *>(const_cast<void *>(lvDataPtr));

		dataPtrOut = dataPtr->data();

		*lvDataPtrOut = reinterpret_cast<void *>(dataPtrOut);

	}

	catch (const MdsException & mdsE)

	{


		errorCode = bogusError;

		errorMessage = const_cast<char *>(mdsE.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_data_decompile(const void *lvDataPtr, LStrHandle lvStrHdlOut, ErrorCluster *error)

{

	Data *dataPtr = NULL;

	char *strOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		dataPtr = reinterpret_cast<Data *>(const_cast<void *>(lvDataPtr));

		strOut = dataPtr->decompile();

		std::size_t strOutLen = std::strlen(strOut);

		errorCode = NumericArrayResize(uB, 1, reinterpret_cast<UHandle *>(&lvStrHdlOut), strOutLen+sizeof(int32));

		if (!errorCode)

		{

			MoveBlock(reinterpret_cast<uChar *>(strOut), LStrBuf(*lvStrHdlOut), strOutLen);

			(*lvStrHdlOut)->cnt = static_cast<int32>(strOutLen);

		}

		else

			errorMessage = (char *)"NumericArrayResize error";

		deleteNativeArray(strOut);

	}

	catch (const MdsException & mdsE)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(mdsE.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_data_deserialize(void **lvDataPtrOut, const char *serializedIn, ErrorCluster *error)

{

	Data *dataPtrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		dataPtrOut = deserialize(const_cast<char *>(serializedIn));

		*lvDataPtrOut = reinterpret_cast<void *>(dataPtrOut);

	}

	catch (const MdsException & mdsE)

	{


		errorCode = bogusError;

		errorMessage = const_cast<char *>(mdsE.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_data_deserialize_data(void **lvDataPtrOut, const void *lvDataPtrIn, ErrorCluster *error)

{

	Data *dataPtrOut = NULL;

	Data *dataPtrIn = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		dataPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvDataPtrIn));

		dataPtrOut = deserialize(dataPtrIn);

		*lvDataPtrOut = reinterpret_cast<void *>(dataPtrOut);

	}

	catch (const MdsException & mdsE)

	{


		errorCode = bogusError;

		errorMessage = const_cast<char *>(mdsE.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}

EXPORT void mdsplus_data_destructor(void **lvDataPtr)
{
	deleteLvData(lvDataPtr);
}

EXPORT void mdsplus_data_evaluate(const void *lvDataPtr, void **lvDataPtrOut, ErrorCluster *error)

{

	Data *dataPtr = NULL;

	Data *dataPtrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;
	
	char const * errorMessage = ""; 

	try

	{

		dataPtr = reinterpret_cast<Data *>(const_cast<void *>(lvDataPtr));

		dataPtrOut = dataPtr->evaluate();

		*lvDataPtrOut = reinterpret_cast<void *>(dataPtrOut);

	}

	catch (const MdsException & mdsE)

	{


		errorCode = bogusError;

		errorMessage = const_cast<char *>(mdsE.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_data_execute(void **lvDataPtrOut, const char *exprIn, ErrorCluster *error)

{

	Data *dataPtrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		dataPtrOut = execute(const_cast<char *>(exprIn));

		*lvDataPtrOut = reinterpret_cast<void *>(dataPtrOut);

	}

	catch (const MdsException & mdsE)

	{


		errorCode = bogusError;

		errorMessage = const_cast<char *>(mdsE.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_data_execute_tree(void **lvDataPtrOut, const char *exprIn, const void *lvTreePtrIn, ErrorCluster *error)

{

	Data *dataPtrOut = NULL;

	Tree *treePtrIn = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		treePtrIn = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtrIn));

		dataPtrOut = execute(const_cast<char *>(exprIn), treePtrIn);

		*lvDataPtrOut = reinterpret_cast<void *>(dataPtrOut);

	}

	catch (const MdsException & mdsE)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(mdsE.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_data_getByte(const void *lvDataPtr, char *byteOut, ErrorCluster *error) {
	*byteOut = getScalar<char>(lvDataPtr, &Data::getByte, __func__, error);
}

EXPORT void mdsplus_data_getByteArray(const void *lvDataPtr, LByteArrHdl lvByteArrHdlOut, ErrorCluster *error)

{

	Data *dataPtr = NULL;

	char *byteArrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = (char *)"";

	try

	{

		dataPtr = reinterpret_cast<Data *>(const_cast<void *>(lvDataPtr));

		int byteArrLen = 0;

		byteArrOut = dataPtr->getByteArray(&byteArrLen);

		errorCode = NumericArrayResize(iB, 1, reinterpret_cast<UHandle *>(&lvByteArrHdlOut), static_cast<int32>(byteArrLen));

		if (!errorCode) {

			for (int i = 0; i < byteArrLen; i++)

				(*lvByteArrHdlOut)->elt[i] = static_cast<int8>(byteArrOut[i]);

			(*lvByteArrHdlOut)->dimSize = static_cast<int32>(byteArrLen);

		}

		else

			errorMessage = (char *)"NumericArrayResize error";

		deleteNativeArray(byteArrOut);

	} catch (const MdsException & e) {


		errorCode = bogusError;

		errorMessage = e.what();

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}

EXPORT void mdsplus_data_getDouble(const void *lvDataPtr, double *doubleOut, ErrorCluster *error) {
	*doubleOut = getScalar<double>(lvDataPtr, &Data::getDouble, __func__, error);
}

EXPORT void mdsplus_data_getDoubleArray(const void *lvDataPtr, LDblArrHdl lvDblArrHdlOut, ErrorCluster *error)

{

	Data *dataPtr = NULL;

	double *doubleArrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = (char *)"";

	try

	{

		dataPtr = reinterpret_cast<Data *>(const_cast<void *>(lvDataPtr));

		int doubleArrLen = 0;

		doubleArrOut = dataPtr->getDoubleArray(&doubleArrLen);

		errorCode = NumericArrayResize(fD, 1, reinterpret_cast<UHandle *>(&lvDblArrHdlOut), static_cast<int32>(doubleArrLen));

		if (!errorCode) {

			for (int i = 0; i < doubleArrLen; i++)

				(*lvDblArrHdlOut)->elt[i] = doubleArrOut[i];

			(*lvDblArrHdlOut)->dimSize = static_cast<int32>(doubleArrLen);

		}

		else

			errorMessage = (char *)"NumericArrayResize error";

		deleteNativeArray(doubleArrOut);

	} catch (const MdsException & e) {

		errorCode = bogusError;

		errorMessage = e.what();

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}

EXPORT void mdsplus_data_getError(const void *lvDataPtr, void **lvDataPtrOut, ErrorCluster *error) {
	*lvDataPtrOut = reinterpret_cast<void *>(getScalar<Data *>(lvDataPtr, &Data::getError, __func__, error));
}

EXPORT void mdsplus_data_getFloat(const void *lvDataPtr, float *floatOut, ErrorCluster *error) {
	*floatOut = getScalar<float>(lvDataPtr, &Data::getFloat, __func__, error);
}

EXPORT void mdsplus_data_getFloatArray(const void *lvDataPtr, LFltArrHdl lvFltArrHdlOut, ErrorCluster *error)

{

	Data *dataPtr = NULL;

	float *floatArrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = (char *)"";

	try

	{

		dataPtr = reinterpret_cast<Data *>(const_cast<void *>(lvDataPtr));

		int floatArrLen = 0;

		floatArrOut = dataPtr->getFloatArray(&floatArrLen);

		errorCode = NumericArrayResize(fS, 1, reinterpret_cast<UHandle *>(&lvFltArrHdlOut), static_cast<int32>(floatArrLen));

		if (!errorCode) {

			for (int i = 0; i < floatArrLen; i++)

				(*lvFltArrHdlOut)->elt[i] = floatArrOut[i];

			(*lvFltArrHdlOut)->dimSize = static_cast<int32>(floatArrLen);

		}

		else

			errorMessage = (char *)"NumericArrayResize error";

		deleteNativeArray(floatArrOut);

	} catch (const MdsException & e) {


		errorCode = bogusError;

		errorMessage = e.what();

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}

EXPORT void mdsplus_data_getHelp(const void *lvDataPtr, void **lvDataPtrOut, ErrorCluster *error) {
	*lvDataPtrOut = reinterpret_cast<void *>(getScalar<Data *>(lvDataPtr, &Data::getHelp, __func__, error));
}

EXPORT void mdsplus_data_getInfo(const void *lvDataPtr, char *clazzOut, char *dtypeOut, short *lengthOut, LIntArrHdl lvIntArrHdlOut, ErrorCluster *error)

{

	Data *dataPtr = NULL;

	char nDims = 0;

	int *dims = NULL;

	void *ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = (char *)"";

	try

	{

		dataPtr = reinterpret_cast<Data *>(const_cast<void *>(lvDataPtr));

		dataPtr->getInfo(clazzOut, dtypeOut, lengthOut, &nDims, &dims, &ptr);

		errorCode = NumericArrayResize(iL, 1, reinterpret_cast<UHandle *>(&lvIntArrHdlOut), static_cast<int32>(nDims));

		if (!errorCode) {

			for (int i = 0; i < nDims; i++)

				(*lvIntArrHdlOut)->elt[i] = static_cast<int32>(dims[i]);

			(*lvIntArrHdlOut)->dimSize = static_cast<int32>(nDims);

		}

		else

			errorMessage = (char *)"NumericArrayResize error";

		// ptr is not returned

		deleteNativeArray(dims);

	}

	catch (const MdsException &e)

	{


		errorCode = bogusError;

		errorMessage = e.what();

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_data_getInt(const void *lvDataPtr, int *intOut, ErrorCluster *error) {
	*intOut = getScalar<int>(lvDataPtr, &Data::getInt, __func__, error);
}

EXPORT void mdsplus_data_getIntArray(const void *lvDataPtr, LIntArrHdl lvIntArrHdlOut, ErrorCluster *error)

{

	Data *dataPtr = NULL;

	int *intArrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = (char *)"";

	try

	{

		dataPtr = reinterpret_cast<Data *>(const_cast<void *>(lvDataPtr));

		int intArrLen = 0;

		intArrOut = dataPtr->getIntArray(&intArrLen);

		errorCode = NumericArrayResize(iL, 1, reinterpret_cast<UHandle *>(&lvIntArrHdlOut), static_cast<int32>(intArrLen));

		if (!errorCode) {

			for (int i = 0; i < intArrLen; i++)

				(*lvIntArrHdlOut)->elt[i] = static_cast<int32>(intArrOut[i]);

			(*lvIntArrHdlOut)->dimSize = static_cast<int32>(intArrLen);

		}

		else

			errorMessage = (char *)"NumericArrayResize error";

		deleteNativeArray(intArrOut);

	} catch (const MdsException & e) {

		errorCode = bogusError;

		errorMessage = e.what();

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_data_getLong(const void *lvDataPtr, int64_t *longOut, ErrorCluster *error) {
	*longOut = getScalar<int64_t>(lvDataPtr, &Data::getLong, __func__, error);
}

EXPORT void mdsplus_data_getLongArray(const void *lvDataPtr, LLngArrHdl lvLngArrHdlOut, ErrorCluster *error)

{

	Data *dataPtr = NULL;

	int64_t *longArrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = (char *)"";

	try

	{

		dataPtr = reinterpret_cast<Data *>(const_cast<void *>(lvDataPtr));

		int longArrLen = 0;

		longArrOut = dataPtr->getLongArray(&longArrLen);

		errorCode = NumericArrayResize(iQ, 1, reinterpret_cast<UHandle *>(&lvLngArrHdlOut), static_cast<int32>(longArrLen));

		if (!errorCode) {

			for (int i = 0; i < longArrLen; i++)

				(*lvLngArrHdlOut)->elt[i] = static_cast<int64>(longArrOut[i]);

			(*lvLngArrHdlOut)->dimSize = static_cast<int32>(longArrLen);

		}

		else

			errorMessage = (char *)"NumericArrayResize error";

		deleteNativeArray(longArrOut);

	} catch (const MdsException & e) {

		errorCode = bogusError;

		errorMessage = e.what();

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_data_getShape(const void *lvDataPtr, LIntArrHdl lvIntArrHdlOut, ErrorCluster *error)

{

	Data *dataPtr = NULL;

	int *intArrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = (char *)"";

	try

	{

		dataPtr = reinterpret_cast<Data *>(const_cast<void *>(lvDataPtr));

		int intArrLen = 0;

		intArrOut = dataPtr->getShape(&intArrLen);

		errorCode = NumericArrayResize(iL, 1, reinterpret_cast<UHandle *>(&lvIntArrHdlOut), static_cast<int32>(intArrLen));

		if (!errorCode) {

			for (int i = 0; i < intArrLen; i++)

				(*lvIntArrHdlOut)->elt[i] = intArrOut[i];

			(*lvIntArrHdlOut)->dimSize = static_cast<int32>(intArrLen);

		}

		else

			errorMessage = (char *)"NumericArrayResize error";

		deleteNativeArray(intArrOut);

	} catch (const MdsException & e) {

		errorCode = bogusError;

		errorMessage = e.what();

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}

EXPORT void mdsplus_data_getShort(const void *lvDataPtr, short *shortOut, ErrorCluster *error) {
	*shortOut = getScalar<short>(lvDataPtr, &Data::getShort, __func__, error);
}

EXPORT void mdsplus_data_getShortArray(const void *lvDataPtr, LShtArrHdl lvShtArrHdlOut, ErrorCluster *error)

{

	Data *dataPtr = NULL;

	short *shortArrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = (char *)"";

	try

	{

		dataPtr = reinterpret_cast<Data *>(const_cast<void *>(lvDataPtr));

		int shortArrLen = 0;

		shortArrOut = dataPtr->getShortArray(&shortArrLen);

		errorCode = NumericArrayResize(iW, 1, reinterpret_cast<UHandle *>(&lvShtArrHdlOut), static_cast<int32>(shortArrLen));

		if (!errorCode)

		{

			for (int i = 0; i < shortArrLen; i++)

				(*lvShtArrHdlOut)->elt[i] = static_cast<int16>(shortArrOut[i]);

			(*lvShtArrHdlOut)->dimSize = static_cast<int32>(shortArrLen);

		}

		else

			errorMessage = (char *)"NumericArrayResize error";

		deleteNativeArray(shortArrOut);

	} catch (const MdsException & e) {

		errorCode = bogusError;

		errorMessage = e.what();

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}

EXPORT void mdsplus_data_getSize(const void *lvDataPtr, int *sizeOut, ErrorCluster *error) {
	*sizeOut = getScalar<int>(lvDataPtr, &Data::getSize, __func__, error);
}

EXPORT void mdsplus_data_getString(const void *lvDataPtr, LStrHandle lvStrHdlOut, ErrorCluster *error)

{

	Data *dataPtr = NULL;

	char *strOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = (char *)"";

	try

	{

		dataPtr = reinterpret_cast<Data *>(const_cast<void *>(lvDataPtr));

		strOut = dataPtr->getString();

		std::size_t strOutLen = std::strlen(strOut);

		errorCode = NumericArrayResize(uB, 1, reinterpret_cast<UHandle *>(&lvStrHdlOut), strOutLen+sizeof(int32));

		if (!errorCode)

		{

			MoveBlock(reinterpret_cast<uChar *>(strOut), LStrBuf(*lvStrHdlOut), strOutLen);

			(*lvStrHdlOut)->cnt = strOutLen;

		}

		else

			errorMessage = (char *)"NumericArrayResize error";

		deleteNativeArray(strOut);

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = e.what();
  
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}

EXPORT void mdsplus_data_getStringArray(const void *lvDataPtr, LStrArrHdl lvStrArrHdlOut, ErrorCluster *error)

{

	Data *dataPtr = NULL;

	char **stringArrOut = NULL;

	int stringArrLen = 0;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = (char *)"";

	try

	{

		dataPtr = reinterpret_cast<Data *>(const_cast<void *>(lvDataPtr));

		stringArrOut = dataPtr->getStringArray(&stringArrLen);

		// checks whether the size of a pointer is 32 bit or 64 bit depending upon the system architecture

		int32 typeCode = (sizeof(LStrHandle) > sizeof(int32)) ? iQ : iL;

		// resizes string array

		errorCode = NumericArrayResize(typeCode, 1, reinterpret_cast<UHandle *>(&lvStrArrHdlOut), static_cast<int32>(stringArrLen));

		if (!errorCode)

		{

			int i = 0;

			// creates LStrHandle strings and fills them with the stringArrOut corrispondent strings

			while (!errorCode && i < stringArrLen)

			{

				char *currStr = stringArrOut[i];

				std::size_t currStrLen = std::strlen(currStr);

				LStrHandle currLStrHdl = reinterpret_cast<LStrHandle>(DSNewHandle(sizeof(LStrHandle)));

				errorCode = currLStrHdl == NULL;

				if (!errorCode)

				{

					errorCode = NumericArrayResize(uB, 1, reinterpret_cast<UHandle *>(&currLStrHdl), currStrLen+sizeof(int32));

					if (!errorCode)

					{

						MoveBlock(reinterpret_cast<uChar *>(currStr), LStrBuf(*currLStrHdl), currStrLen);

						(*currLStrHdl)->cnt = currStrLen;

						(*lvStrArrHdlOut)->elm[i++] = currLStrHdl;

					}

					else

						errorMessage = (char *)"NumericArrayResize error";

				}

				else

					errorMessage = (char *)"DSNewHandle error";

			}

			// keeps only well allocated string handles, till the ith string

			(*lvStrArrHdlOut)->dimSize = static_cast<int32>(i);

		}

		else

			errorMessage = (char *)"NumericArrayResize error";

		// frees memory

		for (int i = 0; i < stringArrLen; i++)

			deleteNativeArray(stringArrOut[i]);

		deleteNativeArray(stringArrOut);

	} catch (const MdsException & e) {


		errorCode = bogusError;

		errorMessage = e.what();

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}

EXPORT void mdsplus_data_getUnits(const void *lvDataPtr, void **lvDataPtrOut, ErrorCluster *error) {
	*lvDataPtrOut = reinterpret_cast<void *>(getScalar<Data *>(lvDataPtr, &Data::getUnits, __func__, error));
}

EXPORT void mdsplus_data_getValidation(const void *lvDataPtr, void **lvDataPtrOut, ErrorCluster *error) {
	*lvDataPtrOut = reinterpret_cast<void *>(getScalar<Data *>(lvDataPtr, &Data::getValidation, __func__, error));
}

EXPORT void mdsplus_data_plot(const void *lvDataPtr, ErrorCluster *error)

{

	Data *dataPtr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = (char *)"";

	try

	{

		dataPtr = reinterpret_cast<Data *>(const_cast<void *>(lvDataPtr));

		dataPtr->plot();

	} catch (const MdsException & e) {

		errorCode = bogusError;

		errorMessage = e.what();

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}

EXPORT void mdsplus_data_serialize(const void *lvDataPtr, LByteArrHdl lvByteArrHdlOut, ErrorCluster *error)

{

	Data *dataPtr = NULL;

	char *byteArrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = (char *)"";

	try

	{

		dataPtr = reinterpret_cast<Data *>(const_cast<void *>(lvDataPtr));

		int byteArrLen = 0;

		byteArrOut = dataPtr->serialize(&byteArrLen);

		errorCode = NumericArrayResize(iB, 1, reinterpret_cast<UHandle *>(&lvByteArrHdlOut), static_cast<int32>(byteArrLen));

		if (!errorCode) {

			for (int i = 0; i < byteArrLen; i++)

				(*lvByteArrHdlOut)->elt[i] = static_cast<int8>(byteArrOut[i]);

			(*lvByteArrHdlOut)->dimSize = static_cast<int32>(byteArrLen);

		}

		else

			errorMessage = (char *)"NumericArrayResize error";

		deleteNativeArray(byteArrOut);

	} catch (const MdsException & e) {


		errorCode = bogusError;

		errorMessage = e.what();

		fillErrorCluster(errorCode, errorSource, errorMessage, error);

		return;

	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}

static void setAccessory(char const * src, ErrorCluster * error, void * outPtr, void const * inPtr, void (Data::*setX)(Data *)) {
	MgErr errorCode = noErr;
	char const * errorMessage = (char *)"";

	try {
		Data * data = reinterpret_cast<Data *>(outPtr);
		(data->*setX)(reinterpret_cast<Data *>(const_cast<void *>(inPtr)));
	} catch (const MdsException & e) {
		errorCode = bogusError;
		errorMessage = e.what();
		fillErrorCluster(errorCode, src, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, src, errorMessage, error);
}

EXPORT void mdsplus_data_setError(void *lvDataPtr, const void *lvDataPtrIn, ErrorCluster *error) {
	setAccessory(__func__, error, lvDataPtr, lvDataPtrIn, &Data::setError);
}

EXPORT void mdsplus_data_setHelp(void *lvDataPtr, const void *lvDataPtrIn, ErrorCluster *error) {
	setAccessory(__func__, error, lvDataPtr, lvDataPtrIn, &Data::setHelp);
}

EXPORT void mdsplus_data_setUnits(void *lvDataPtr, const void *lvDataPtrIn, ErrorCluster *error) {
	setAccessory(__func__, error, lvDataPtr, lvDataPtrIn, &Data::setUnits);
}

EXPORT void mdsplus_data_setValidation(void *lvDataPtr, const void *lvDataPtrIn, ErrorCluster *error) {
	setAccessory(__func__, error, lvDataPtr, lvDataPtrIn, &Data::setValidation);
}

/********************************************************************************************************
												EMPTY
 ********************************************************************************************************/

EXPORT void mdsplus_empty_destructor(void **lvEmptyPtr) {
	deleteLvData(lvEmptyPtr);
}

/********************************************************************************************************
												FLOAT32
 ********************************************************************************************************/

EXPORT void mdsplus_float32_constructor(void **lvFloat32PtrOut, float valIn, ErrorCluster *error) {
	scalarConstructor<Float32>(__func__, error, lvFloat32PtrOut, valIn);
}

EXPORT void mdsplus_float32_destructor(void **lvFloat32Ptr) {
	deleteLvData(lvFloat32Ptr);
}

EXPORT void mdsplus_float32_getByte(const void *lvFloat32Ptr, char *byteOut, ErrorCluster *error) {
	*byteOut = getScalar<char>(lvFloat32Ptr, &Data::getByte, __func__, error);
}

EXPORT void mdsplus_float32_getDouble(const void *lvFloat32Ptr, double *doubleOut, ErrorCluster *error) {
	*doubleOut = getScalar<double>(lvFloat32Ptr, &Data::getDouble, __func__, error);
}

EXPORT void mdsplus_float32_getFloat(const void *lvFloat32Ptr, float *floatOut, ErrorCluster *error) {
	*floatOut = getScalar<float>(lvFloat32Ptr, &Data::getFloat, __func__, error);
}

EXPORT void mdsplus_float32_getInt(const void *lvFloat32Ptr, int *intOut, ErrorCluster *error) {
	*intOut = getScalar<int>(lvFloat32Ptr, &Data::getInt, __func__, error);
}

EXPORT void mdsplus_float32_getLong(const void *lvFloat32Ptr, int64_t *longOut, ErrorCluster *error) {
	*longOut = getScalar<int64_t>(lvFloat32Ptr, &Data::getLong, __func__, error);
}

EXPORT void mdsplus_float32_getShort(const void *lvFloat32Ptr, short *shortOut, ErrorCluster *error) {
	*shortOut = getScalar<short>(lvFloat32Ptr, &Data::getShort, __func__, error);
}

/********************************************************************************************************
												FLOAT32ARRAY
 ********************************************************************************************************/

EXPORT void mdsplus_float32array_constructor(void **lvFloat32ArrayPtrOut, const LFltArrHdl lvFltArrHdlIn, ErrorCluster *error) {
	arrayConstructor<Float32Array>(__func__, error, lvFloat32ArrayPtrOut, lvFltArrHdlIn);
}

EXPORT void mdsplus_float32array_constructor_dims(void **lvFloat32ArrayPtrOut, const LFltArrHdl lvFltArrHdlIn, const LIntArrHdl lvIntArrHdlIn, ErrorCluster *error) {
	arrayConstructor<Float32Array>(__func__, error, lvFloat32ArrayPtrOut, lvFltArrHdlIn, lvIntArrHdlIn);
}

EXPORT void mdsplus_float32array_destructor(void **lvFloat32ArrayPtr) {
	deleteLvData(lvFloat32ArrayPtr);
}

/********************************************************************************************************
												FLOAT64
 ********************************************************************************************************/

EXPORT void mdsplus_float64_constructor(void **lvFloat64PtrOut, double valIn, ErrorCluster *error) {
	scalarConstructor<Float64>(__func__, error, lvFloat64PtrOut, valIn);
}

EXPORT void mdsplus_float64_destructor(void **lvFloat64Ptr) {
	deleteLvData(lvFloat64Ptr);
}

EXPORT void mdsplus_float64_getByte(const void *lvFloat64Ptr, char *byteOut, ErrorCluster *error) {
	*byteOut = getScalar<char>(lvFloat64Ptr, &Data::getByte, __func__, error);
}

EXPORT void mdsplus_float64_getDouble(const void *lvFloat64Ptr, double *doubleOut, ErrorCluster *error) {
	*doubleOut = getScalar<double>(lvFloat64Ptr, &Data::getDouble, __func__, error);
}

EXPORT void mdsplus_float64_getFloat(const void *lvFloat64Ptr, float *floatOut, ErrorCluster *error) {
	*floatOut = getScalar<float>(lvFloat64Ptr, &Data::getFloat, __func__, error);
}

EXPORT void mdsplus_float64_getInt(const void *lvFloat64Ptr, int *intOut, ErrorCluster *error) {
	*intOut = getScalar<int>(lvFloat64Ptr, &Data::getInt, __func__, error);
}

EXPORT void mdsplus_float64_getLong(const void *lvFloat64Ptr, int64_t *longOut, ErrorCluster *error) {
	*longOut = getScalar<int64_t>(lvFloat64Ptr, &Data::getLong, __func__, error);
}

EXPORT void mdsplus_float64_getShort(const void *lvFloat64Ptr, short *shortOut, ErrorCluster *error) {
	*shortOut = getScalar<short>(lvFloat64Ptr, &Data::getShort, __func__, error);
}

/********************************************************************************************************
												FLOAT64ARRAY
 ********************************************************************************************************/

EXPORT void mdsplus_float64array_constructor(void **lvFloat64ArrayPtrOut, const LDblArrHdl lvDblArrHdlIn, ErrorCluster *error) {
	arrayConstructor<Float64Array>(__func__, error, lvFloat64ArrayPtrOut, lvDblArrHdlIn);
}

EXPORT void mdsplus_float64array_constructor_dims(void **lvFloat64ArrayPtrOut, const LDblArrHdl lvDblArrHdlIn, const LIntArrHdl lvIntArrHdlIn, ErrorCluster *error) {
	arrayConstructor<Float64Array>(__func__, error, lvFloat64ArrayPtrOut, lvDblArrHdlIn, lvIntArrHdlIn);
}

EXPORT void mdsplus_float64array_destructor(void **lvFloat64ArrayPtr)
{
	deleteLvData(lvFloat64ArrayPtr);
}

/********************************************************************************************************
												INT16
 ********************************************************************************************************/

EXPORT void mdsplus_int16_constructor(void **lvInt16PtrOut, short valIn, ErrorCluster *error) {
	scalarConstructor<Int16>(__func__, error, lvInt16PtrOut, valIn);
}

EXPORT void mdsplus_int16_destructor(void **lvInt16Ptr) {
	deleteLvData(lvInt16Ptr);
}

EXPORT void mdsplus_int16_getByte(const void *lvInt16Ptr, char *byteOut, ErrorCluster *error)

{

	Int16 *int16Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		int16Ptr = reinterpret_cast<Int16 *>(const_cast<void *>(lvInt16Ptr));

		*byteOut = int16Ptr->getByte();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_int16_getDouble(const void *lvInt16Ptr, double *doubleOut, ErrorCluster *error)

{

	Int16 *int16Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		int16Ptr = reinterpret_cast<Int16 *>(const_cast<void *>(lvInt16Ptr));

		*doubleOut = int16Ptr->getDouble();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_int16_getFloat(const void *lvInt16Ptr, float *floatOut, ErrorCluster *error)

{

	Int16 *int16Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;
	
	char const * errorMessage = ""; 

	try

	{

		int16Ptr = reinterpret_cast<Int16 *>(const_cast<void *>(lvInt16Ptr));

		*floatOut = int16Ptr->getFloat();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_int16_getInt(const void *lvInt16Ptr, int *intOut, ErrorCluster *error)

{

	Int16 *int16Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		int16Ptr = reinterpret_cast<Int16 *>(const_cast<void *>(lvInt16Ptr));

		*intOut = int16Ptr->getInt();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_int16_getLong(const void *lvInt16Ptr, int64_t *longOut, ErrorCluster *error)

{

	Int16 *int16Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		int16Ptr = reinterpret_cast<Int16 *>(const_cast<void *>(lvInt16Ptr));

		*longOut = int16Ptr->getLong();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_int16_getShort(const void *lvInt16Ptr, short *shortOut, ErrorCluster *error)

{

	Int16 *int16Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		int16Ptr = reinterpret_cast<Int16 *>(const_cast<void *>(lvInt16Ptr));

		*shortOut = int16Ptr->getShort();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



/********************************************************************************************************

												INT16ARRAY

 ********************************************************************************************************/



EXPORT void mdsplus_int16array_constructor(void **lvInt16ArrayPtrOut, const LShtArrHdl lvShtArrHdlIn, ErrorCluster *error)

{

	Int16Array *int16ArrayPtrOut = NULL;

	short *int16Arr = NULL;	

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		int int16ArrLen = static_cast<int>((*lvShtArrHdlIn)->dimSize);

		int16Arr = new short[int16ArrLen];

		for (int i = 0; i < int16ArrLen; i++)

			int16Arr[i] = static_cast<short>((*lvShtArrHdlIn)->elt[i]);

		int16ArrayPtrOut = new Int16Array(int16Arr, int16ArrLen);

		delete[] int16Arr;

		*lvInt16ArrayPtrOut = reinterpret_cast<void *>(int16ArrayPtrOut);

	}

	catch (const MdsException &e)

	{

		delete[] int16Arr;

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_int16array_constructor_dims(void **lvInt16ArrayPtrOut, const LShtArrHdl lvShtArrHdlIn, const LIntArrHdl lvIntArrHdlIn, ErrorCluster *error)

{

	Int16Array *int16ArrayPtrOut = NULL;

	short *int16Arr = NULL;

	int *intArr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		int int16ArrLen = static_cast<int>((*lvShtArrHdlIn)->dimSize);

		int16Arr = new short[int16ArrLen];

		for (int i = 0; i < int16ArrLen; i++)

			int16Arr[i] = static_cast<short>((*lvShtArrHdlIn)->elt[i]);

		int intArrLen = static_cast<int>((*lvIntArrHdlIn)->dimSize);

		intArr = new int[intArrLen];

		for (int i = 0; i < intArrLen; i++)

			intArr[i] = static_cast<int>((*lvIntArrHdlIn)->elt[i]);

		int16ArrayPtrOut = new Int16Array(int16Arr, intArrLen, intArr);

		delete[] int16Arr;

		delete[] intArr;

		*lvInt16ArrayPtrOut = reinterpret_cast<void *>(int16ArrayPtrOut);

	}

	catch (const MdsException &e)

	{

		delete[] int16Arr;

		delete[] intArr;


		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}

EXPORT void mdsplus_int16array_destructor(void **lvInt16ArrayPtr)
{
	deleteLvData(lvInt16ArrayPtr);
}

/********************************************************************************************************
												INT32
 ********************************************************************************************************/

EXPORT void mdsplus_int32_constructor(void **lvInt32PtrOut, int valIn, ErrorCluster *error) {
	scalarConstructor<Int32>(__func__, error, lvInt32PtrOut, valIn);
}

EXPORT void mdsplus_int32_destructor(void **lvInt32Ptr) {
	deleteLvData(lvInt32Ptr);
}

EXPORT void mdsplus_int32_getByte(const void *lvInt32Ptr, char *byteOut, ErrorCluster *error)

{

	Int32 *int32Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		int32Ptr = reinterpret_cast<Int32 *>(const_cast<void *>(lvInt32Ptr));

		*byteOut = int32Ptr->getByte();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_int32_getDouble(const void *lvInt32Ptr, double *doubleOut, ErrorCluster *error)

{

	Int32 *int32Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		int32Ptr = reinterpret_cast<Int32 *>(const_cast<void *>(lvInt32Ptr));

		*doubleOut = int32Ptr->getDouble();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_int32_getFloat(const void *lvInt32Ptr, float *floatOut, ErrorCluster *error)

{

	Int32 *int32Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		int32Ptr = reinterpret_cast<Int32 *>(const_cast<void *>(lvInt32Ptr));

		*floatOut = int32Ptr->getFloat();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_int32_getInt(const void *lvInt32Ptr, int *intOut, ErrorCluster *error)

{

	Int32 *int32Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		int32Ptr = reinterpret_cast<Int32 *>(const_cast<void *>(lvInt32Ptr));

		*intOut = int32Ptr->getInt();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_int32_getLong(const void *lvInt32Ptr, int64_t *longOut, ErrorCluster *error)

{

	Int32 *int32Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		int32Ptr = reinterpret_cast<Int32 *>(const_cast<void *>(lvInt32Ptr));

		*longOut = int32Ptr->getLong();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_int32_getShort(const void *lvInt32Ptr, short *shortOut, ErrorCluster *error)

{

	Int32 *int32Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		int32Ptr = reinterpret_cast<Int32 *>(const_cast<void *>(lvInt32Ptr));

		*shortOut = int32Ptr->getShort();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



/********************************************************************************************************

												INT32ARRAY

 ********************************************************************************************************/



EXPORT void mdsplus_int32array_constructor(void **lvInt32ArrayPtrOut, const LIntArrHdl lvIntArrHdlIn, ErrorCluster *error)

{

	Int32Array *int32ArrayPtrOut = NULL;

	int *int32Arr = NULL;	

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		int int32ArrLen = static_cast<int>((*lvIntArrHdlIn)->dimSize);

		int32Arr = new int[int32ArrLen];

		for (int i = 0; i < int32ArrLen; i++)

			int32Arr[i] = static_cast<int>((*lvIntArrHdlIn)->elt[i]);

		int32ArrayPtrOut = new Int32Array(int32Arr, int32ArrLen);

		delete[] int32Arr;

		*lvInt32ArrayPtrOut = reinterpret_cast<void *>(int32ArrayPtrOut);

	}

	catch (const MdsException &e)

	{

		delete[] int32Arr;

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_int32array_constructor_dims(void **lvInt32ArrayPtrOut, const LIntArrHdl lvIntArrHdlIn, const LIntArrHdl lvIntArrHdlDimIn, ErrorCluster *error)

{

	Int32Array *int32ArrayPtrOut = NULL;

	int *int32Arr = NULL;

	int *intArr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		int int32ArrLen = static_cast<int>((*lvIntArrHdlIn)->dimSize);

		int32Arr = new int[int32ArrLen];

		for (int i = 0; i < int32ArrLen; i++)

			int32Arr[i] = static_cast<int>((*lvIntArrHdlIn)->elt[i]);

		int intArrLen = static_cast<int>((*lvIntArrHdlDimIn)->dimSize);

		intArr = new int[intArrLen];

		for (int i = 0; i < intArrLen; i++)

			intArr[i] = static_cast<int>((*lvIntArrHdlDimIn)->elt[i]);

		int32ArrayPtrOut = new Int32Array(int32Arr, intArrLen, intArr);

		delete[] int32Arr;

		delete[] intArr;

		*lvInt32ArrayPtrOut = reinterpret_cast<void *>(int32ArrayPtrOut);

	}

	catch (const MdsException &e)

	{

		delete[] int32Arr;

		delete[] intArr;

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}

EXPORT void mdsplus_int32array_destructor(void **lvInt32ArrayPtr)
{
	deleteLvData(lvInt32ArrayPtr);
}

/********************************************************************************************************
												INT64
 ********************************************************************************************************/

EXPORT void mdsplus_int64_constructor(void **lvInt64PtrOut, int64_t valIn, ErrorCluster *error) {
	scalarConstructor<Int64>(__func__, error, lvInt64PtrOut, valIn);
}

EXPORT void mdsplus_int64_destructor(void **lvInt64Ptr) {
	deleteLvData(lvInt64Ptr);
}

EXPORT void mdsplus_int64_getByte(const void *lvInt64Ptr, char *byteOut, ErrorCluster *error)

{

	Int64 *int64Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		int64Ptr = reinterpret_cast<Int64 *>(const_cast<void *>(lvInt64Ptr));

		*byteOut = int64Ptr->getByte();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_int64_getDouble(const void *lvInt64Ptr, double *doubleOut, ErrorCluster *error)

{

	Int64 *int64Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		int64Ptr = reinterpret_cast<Int64 *>(const_cast<void *>(lvInt64Ptr));

		*doubleOut = int64Ptr->getDouble();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_int64_getFloat(const void *lvInt64Ptr, float *floatOut, ErrorCluster *error)

{

	Int64 *int64Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		int64Ptr = reinterpret_cast<Int64 *>(const_cast<void *>(lvInt64Ptr));

		*floatOut = int64Ptr->getFloat();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_int64_getInt(const void *lvInt64Ptr, int *intOut, ErrorCluster *error)

{

	Int64 *int64Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;
	
	char const * errorMessage = ""; 

	try

	{

		int64Ptr = reinterpret_cast<Int64 *>(const_cast<void *>(lvInt64Ptr));

		*intOut = int64Ptr->getInt();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_int64_getLong(const void *lvInt64Ptr, int64_t *longOut, ErrorCluster *error)

{

	Int64 *int64Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		int64Ptr = reinterpret_cast<Int64 *>(const_cast<void *>(lvInt64Ptr));

		*longOut = int64Ptr->getLong();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_int64_getShort(const void *lvInt64Ptr, short *shortOut, ErrorCluster *error)

{

	Int64 *int64Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		int64Ptr = reinterpret_cast<Int64 *>(const_cast<void *>(lvInt64Ptr));

		*shortOut = int64Ptr->getShort();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



/********************************************************************************************************

												INT64ARRAY

 ********************************************************************************************************/



EXPORT void mdsplus_int64array_constructor(void **lvInt64ArrayPtrOut, const LLngArrHdl lvLngArrHdlIn, ErrorCluster *error)

{

	Int64Array *int64ArrayPtrOut = NULL;

	int64_t *int64Arr = NULL;	

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		int int64ArrLen = static_cast<int>((*lvLngArrHdlIn)->dimSize);

		int64Arr = new int64_t[int64ArrLen];

		for (int i = 0; i < int64ArrLen; i++)

			int64Arr[i] = static_cast<int64_t>((*lvLngArrHdlIn)->elt[i]);

		int64ArrayPtrOut = new Int64Array(int64Arr, int64ArrLen);

		delete[] int64Arr;

		*lvInt64ArrayPtrOut = reinterpret_cast<void *>(int64ArrayPtrOut);

	}

	catch (const MdsException &e)

	{

		delete[] int64Arr;


		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_int64array_constructor_dims(void **lvInt64ArrayPtrOut, const LLngArrHdl lvLngArrHdlIn, const LIntArrHdl lvIntArrHdlIn, ErrorCluster *error)

{

	Int64Array *int64ArrayPtrOut = NULL;

	int64_t *int64Arr = NULL;

	int *intArr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		int int64ArrLen = static_cast<int>((*lvLngArrHdlIn)->dimSize);

		int64Arr = new int64_t[int64ArrLen];

		for (int i = 0; i < int64ArrLen; i++)

			int64Arr[i] = static_cast<int64_t>((*lvLngArrHdlIn)->elt[i]);

		int intArrLen = static_cast<int>((*lvIntArrHdlIn)->dimSize);

		intArr = new int[intArrLen];

		for (int i = 0; i < intArrLen; i++)

			intArr[i] = static_cast<int>((*lvIntArrHdlIn)->elt[i]);

		int64ArrayPtrOut = new Int64Array(int64Arr, intArrLen, intArr);

		delete[] int64Arr;

		delete[] intArr;

		*lvInt64ArrayPtrOut = reinterpret_cast<void *>(int64ArrayPtrOut);

	}

	catch (const MdsException &e)

	{

		delete[] int64Arr;

		delete[] intArr;


		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}

EXPORT void mdsplus_int64array_destructor(void **lvInt64ArrayPtr)
{
	deleteLvData(lvInt64ArrayPtr);
}

/********************************************************************************************************

												INT8

 ********************************************************************************************************/

EXPORT void mdsplus_int8_constructor(void **lvInt8PtrOut, char valIn, ErrorCluster *error) {
	scalarConstructor<Int8>(__func__, error, lvInt8PtrOut, valIn);
}

EXPORT void mdsplus_int8_destructor(void **lvInt8Ptr) {
	deleteLvData(lvInt8Ptr);
}

EXPORT void mdsplus_int8_getByte(const void *lvInt8Ptr, char *byteOut, ErrorCluster *error)

{

	Int8 *int8Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		int8Ptr = reinterpret_cast<Int8 *>(const_cast<void *>(lvInt8Ptr));

		*byteOut = int8Ptr->getByte();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_int8_getDouble(const void *lvInt8Ptr, double *doubleOut, ErrorCluster *error)

{

	Int8 *int8Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		int8Ptr = reinterpret_cast<Int8 *>(const_cast<void *>(lvInt8Ptr));

		*doubleOut = int8Ptr->getDouble();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_int8_getFloat(const void *lvInt8Ptr, float *floatOut, ErrorCluster *error)

{

	Int8 *int8Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		int8Ptr = reinterpret_cast<Int8 *>(const_cast<void *>(lvInt8Ptr));

		*floatOut = int8Ptr->getFloat();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_int8_getInt(const void *lvInt8Ptr, int *intOut, ErrorCluster *error)

{

	Int8 *int8Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		int8Ptr = reinterpret_cast<Int8 *>(const_cast<void *>(lvInt8Ptr));

		*intOut = int8Ptr->getInt();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_int8_getLong(const void *lvInt8Ptr, int64_t *longOut, ErrorCluster *error)

{

	Int8 *int8Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		int8Ptr = reinterpret_cast<Int8 *>(const_cast<void *>(lvInt8Ptr));

		*longOut = int8Ptr->getLong();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_int8_getShort(const void *lvInt8Ptr, short *shortOut, ErrorCluster *error)

{

	Int8 *int8Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		int8Ptr = reinterpret_cast<Int8 *>(const_cast<void *>(lvInt8Ptr));

		*shortOut = int8Ptr->getShort();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



/********************************************************************************************************

												INT8ARRAY

 ********************************************************************************************************/



EXPORT void mdsplus_int8array_constructor(void **lvInt8ArrayPtrOut, const LByteArrHdl lvByteArrHdlIn, ErrorCluster *error)

{

	Int8Array *int8ArrayPtrOut = NULL;

	char *int8Arr = NULL;	

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		int int8ArrLen = static_cast<int>((*lvByteArrHdlIn)->dimSize);

		int8Arr = new char[int8ArrLen];

		for (int i = 0; i < int8ArrLen; i++)

			int8Arr[i] = static_cast<char>((*lvByteArrHdlIn)->elt[i]);

		int8ArrayPtrOut = new Int8Array(int8Arr, int8ArrLen);

		delete[] int8Arr;

		*lvInt8ArrayPtrOut = reinterpret_cast<void *>(int8ArrayPtrOut);

	}

	catch (const MdsException &e)

	{

		delete[] int8Arr;

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_int8array_constructor_dims(void **lvInt8ArrayPtrOut, const LByteArrHdl lvByteArrHdlIn, const LIntArrHdl lvIntArrHdlIn, ErrorCluster *error)

{

	Int8Array *int8ArrayPtrOut = NULL;

	char *int8Arr = NULL;

	int *intArr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		int int8ArrLen = static_cast<int>((*lvByteArrHdlIn)->dimSize);

		int8Arr = new char[int8ArrLen];

		for (int i = 0; i < int8ArrLen; i++)

			int8Arr[i] = static_cast<char>((*lvByteArrHdlIn)->elt[i]);

		int intArrLen = static_cast<int>((*lvIntArrHdlIn)->dimSize);

		intArr = new int[intArrLen];

		for (int i = 0; i < intArrLen; i++)

			intArr[i] = static_cast<int>((*lvIntArrHdlIn)->elt[i]);

		int8ArrayPtrOut = new Int8Array(int8Arr, intArrLen, intArr);

		delete[] int8Arr;

		delete[] intArr;

		*lvInt8ArrayPtrOut = reinterpret_cast<void *>(int8ArrayPtrOut);

	}

	catch (const MdsException &e)

	{

		delete[] int8Arr;

		delete[] intArr;

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}

EXPORT void mdsplus_int8array_destructor(void **lvInt8ArrayPtr)
{
	deleteLvData(lvInt8ArrayPtr);
}

/********************************************************************************************************

												RANGE

 ********************************************************************************************************/



EXPORT void mdsplus_range_constructor(void **lvRangePtrOut, const void *lvBeginDataPtrIn, const void *lvEndingDataPtrIn, const void *lvDeltaValDataPtrIn, ErrorCluster *error)

{

	Range *rangePtrOut = NULL;

	Data *beginDataPtrIn = NULL;

	Data *endingDataPtrIn = NULL;

	Data *deltaValDataPtrIn = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		beginDataPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvBeginDataPtrIn));

		endingDataPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvEndingDataPtrIn));

		deltaValDataPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvDeltaValDataPtrIn));

		rangePtrOut = new Range(beginDataPtrIn, endingDataPtrIn, deltaValDataPtrIn);

		*lvRangePtrOut = reinterpret_cast<void *>(rangePtrOut);

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}

EXPORT void mdsplus_range_destructor(void **lvRangePtr, ErrorCluster *error)
{
	deleteLvData(lvRangePtr);
}

EXPORT void mdsplus_range_getBegin(const void *lvRangePtr, void **lvDataPtrOut, ErrorCluster *error)

{

	Range *rangePtr = NULL;

	Data *dataPtrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = ""; 

	try

	{

		rangePtr = reinterpret_cast<Range *>(const_cast<void *>(lvRangePtr));

		dataPtrOut = rangePtr->getBegin();

		*lvDataPtrOut = reinterpret_cast<void *>(dataPtrOut);

	}

	catch (const MdsException &e)

	{


		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}





EXPORT void mdsplus_range_getEnding(const void *lvRangePtr, void **lvDataPtrOut, ErrorCluster *error)

{

	Range *rangePtr = NULL;

	Data *dataPtrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		rangePtr = reinterpret_cast<Range *>(const_cast<void *>(lvRangePtr));

		dataPtrOut = rangePtr->getEnding();

		*lvDataPtrOut = reinterpret_cast<void *>(dataPtrOut);

	}

	catch (const MdsException &e)

	{


		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_range_getDeltaVal(const void *lvRangePtr, void **lvDataPtrOut, ErrorCluster *error)

{

	Range *rangePtr = NULL;

	Data *dataPtrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		rangePtr = reinterpret_cast<Range *>(const_cast<void *>(lvRangePtr));

		dataPtrOut = rangePtr->getDeltaVal();

		*lvDataPtrOut = reinterpret_cast<void *>(dataPtrOut);

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}

EXPORT void mdsplus_range_setBegin(const void *lvRangePtr, const void *lvDataPtrIn, ErrorCluster *error)

{

	Range *rangePtr = NULL;

	Data *dataPtrIn = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		rangePtr = reinterpret_cast<Range *>(const_cast<void *>(lvRangePtr));

		dataPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvDataPtrIn));

		rangePtr->setBegin(dataPtrIn);

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}





EXPORT void mdsplus_range_setEnding(const void *lvRangePtr, const void *lvDataPtrIn, ErrorCluster *error)

{

	Range *rangePtr = NULL;

	Data *dataPtrIn = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		rangePtr = reinterpret_cast<Range *>(const_cast<void *>(lvRangePtr));

		dataPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvDataPtrIn));

		rangePtr->setEnding(dataPtrIn);

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_range_setDeltaVal(const void *lvRangePtr, const void *lvDataPtrIn, ErrorCluster *error)

{

	Range *rangePtr = NULL;

	Data *dataPtrIn = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		rangePtr = reinterpret_cast<Range *>(const_cast<void *>(lvRangePtr));

		dataPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvDataPtrIn));

		rangePtr->setDeltaVal(dataPtrIn);

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



/********************************************************************************************************

												SIGNAL

 ********************************************************************************************************/



EXPORT void mdsplus_signal_constructor(void **lvSignalPtrOut, const void *lvDataPtrIn, const void *lvRawPtrIn, const void *lvDim0PtrIn, ErrorCluster *error)

{

	Signal *signalPtrOut = NULL;

	Data *dataPtrIn = NULL;

	Data *rawPtrIn = NULL;

	Data *dim0PtrIn = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		dataPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvDataPtrIn));

		rawPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvRawPtrIn));

		dim0PtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvDim0PtrIn));

		signalPtrOut = new Signal(dataPtrIn, rawPtrIn, dim0PtrIn);

		*lvSignalPtrOut = reinterpret_cast<void *>(signalPtrOut);

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}

EXPORT void mdsplus_signal_destructor(void **lvSignalPtr, ErrorCluster *error)
{
	deleteLvData(lvSignalPtr);
}

EXPORT void mdsplus_signal_getData(const void *lvSignalPtr, void **lvDataPtrOut, ErrorCluster *error)

{

	Signal *signalPtr = NULL;

	Data *dataPtrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		signalPtr = reinterpret_cast<Signal *>(const_cast<void *>(lvSignalPtr));

		dataPtrOut = signalPtr->getData();

		*lvDataPtrOut = reinterpret_cast<void *>(dataPtrOut);

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}





EXPORT void mdsplus_signal_getRaw(const void *lvSignalPtr, void **lvRawPtrOut, ErrorCluster *error)

{

	Signal *signalPtr = NULL;

	Data *rawPtrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		signalPtr = reinterpret_cast<Signal *>(const_cast<void *>(lvSignalPtr));

		rawPtrOut = signalPtr->getRaw();

		*lvRawPtrOut = reinterpret_cast<void *>(rawPtrOut);

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_signal_getDim(const void *lvSignalPtr, void **lvDim0PtrOut, ErrorCluster *error)

{

	Signal *signalPtr = NULL;

	Data *dim0PtrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		signalPtr = reinterpret_cast<Signal *>(const_cast<void *>(lvSignalPtr));

		dim0PtrOut = signalPtr->getDimensionAt(0);

		*lvDim0PtrOut = reinterpret_cast<void *>(dim0PtrOut);

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}

EXPORT void mdsplus_signal_setData(const void *lvSignalPtr, const void *lvDataPtrIn, ErrorCluster *error)

{

	Signal *signalPtr = NULL;

	Data *dataPtrIn = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		signalPtr = reinterpret_cast<Signal *>(const_cast<void *>(lvSignalPtr));

		dataPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvDataPtrIn));

		signalPtr->setData(dataPtrIn);

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}





EXPORT void mdsplus_signal_setRaw(const void *lvSignalPtr, const void *lvRawPtrIn, ErrorCluster *error)

{

	Signal *signalPtr = NULL;

	Data *rawPtrIn = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		signalPtr = reinterpret_cast<Signal *>(const_cast<void *>(lvSignalPtr));

		rawPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvRawPtrIn));

		signalPtr->setRaw(rawPtrIn);

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_signal_setDim(const void *lvSignalPtr, const void *lvDim0PtrIn, ErrorCluster *error)

{

	Signal *signalPtr = NULL;

	Data *dim0PtrIn = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		signalPtr = reinterpret_cast<Signal *>(const_cast<void *>(lvSignalPtr));

		dim0PtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvDim0PtrIn));

		signalPtr->setDimensionAt(dim0PtrIn, 0);

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}

/********************************************************************************************************
												SCALAR
 ********************************************************************************************************/

EXPORT void mdsplus_scalar_constructor(void **lvScalarPtrOut, ErrorCluster *error) {
	safeCall(__func__, error, Constructor<Scalar>(lvScalarPtrOut));
}

EXPORT void mdsplus_scalar_destructor(void **lvScalarPtr) {
	deleteLvData(lvScalarPtr);
}

EXPORT void mdsplus_scalar_getInfo(const void *lvScalarPtr, char *clazzOut, char *dtypeOut, short *lengthOut, LIntArrHdl lvIntArrHdlOut, ErrorCluster *error)

{

	Scalar *scalarPtr = NULL;

	char nDims = 0;

	int *dims = NULL;

	void *ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		scalarPtr = reinterpret_cast<Scalar *>(const_cast<void *>(lvScalarPtr));

		scalarPtr->getInfo(clazzOut, dtypeOut, lengthOut, &nDims, &dims, &ptr);

		errorCode = NumericArrayResize(iL, 1, reinterpret_cast<UHandle *>(&lvIntArrHdlOut), static_cast<int32>(nDims));

		if (!errorCode) {

			for (int i = 0; i < nDims; i++)

				(*lvIntArrHdlOut)->elt[i] = static_cast<int32>(dims[i]);

			(*lvIntArrHdlOut)->dimSize = static_cast<int32>(nDims);

		}

		else

			errorMessage = (char *)"NumericArrayResize error";

		// ptr is not returned

		deleteNativeArray(dims);

	}

	catch (const MdsException &e)

	{


		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}

/********************************************************************************************************
												STRING
 ********************************************************************************************************/

EXPORT void mdsplus_string_constructor(void **lvStringPtrOut, const char *valIn, ErrorCluster *error) {
	scalarConstructor<String>(__func__, error, lvStringPtrOut, valIn);
}

EXPORT void mdsplus_string_constructor_len(void **lvStringPtrOut, const LUByteArrHdl lvUByteArrHdlIn, ErrorCluster *error) {
	arrayConstructor<String>(__func__, error, lvStringPtrOut, lvUByteArrHdlIn);
}

EXPORT void mdsplus_string_destructor(void **lvStringPtr) {
	deleteLvData(lvStringPtr);
}

struct StringEquals: public Lambda {
	StringEquals(void * in1Ptr, void * in2Ptr, LVBoolean * & outPtr):
		in1Ptr(in1Ptr), in2Ptr(in2Ptr), outPtr(outPtr) {}
	void * in1Ptr;
	void * in2Ptr;
	LVBoolean * & outPtr;

	void operator()() {
		if (reinterpret_cast<String *>(in1Ptr)->equals(reinterpret_cast<Data *>(in2Ptr)))
			*outPtr = TRUE;
		else
			*outPtr = FALSE;
	}
};

EXPORT void mdsplus_string_equals(const void *lvStringPtr, LVBoolean *equalsOut, const void *lvDataPtrIn, ErrorCluster *error) {
	safeCall(__func__, error, StringEquals(const_cast<void *>(lvStringPtr), const_cast<void *>(lvDataPtrIn), equalsOut));
}

EXPORT void mdsplus_string_getString(const void *lvStringPtr, LStrHandle lvStrHdlOut, ErrorCluster *error)

{

	String *stringPtr = NULL;

	char *strOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		stringPtr = reinterpret_cast<String *>(const_cast<void *>(lvStringPtr));

		strOut = stringPtr->getString();

		std::size_t strOutLen = std::strlen(strOut);

		errorCode = NumericArrayResize(uB, 1, reinterpret_cast<UHandle *>(&lvStrHdlOut), strOutLen+sizeof(int32));

		if (!errorCode)

		{

			MoveBlock(reinterpret_cast<uChar *>(strOut), LStrBuf(*lvStrHdlOut), strOutLen);

			(*lvStrHdlOut)->cnt = strOutLen;

		}

		else

			errorMessage = (char *)"NumericArrayResize error";

		deleteNativeArray(strOut);

	}

	catch (const MdsException &e)

	{


		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



/********************************************************************************************************

												STRINGARRAY

 ********************************************************************************************************/



EXPORT void mdsplus_stringarray_constructor(void **lvStringArrayPtrOut, const LStrArrHdl lvLStrArrHdlIn, ErrorCluster *error)

{

	StringArray *stringArrayPtrOut = NULL;

	char **stringArr = NULL;

	int stringArrLen = 0;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		stringArrLen = static_cast<int>((*lvLStrArrHdlIn)->dimSize);

		stringArr = new char *[stringArrLen];

		LStrHandle currLStrHdl = NULL;

		char *currStr = NULL;

		int currStrLen = 0;

		for (int i = 0; i < stringArrLen; i++)

		{

			currLStrHdl = (*lvLStrArrHdlIn)->elm[i];

			currStrLen = static_cast<int>((*currLStrHdl)->cnt);

			currStr = new char[currStrLen + 1];

			for (int j = 0; j < currStrLen; j++)

				currStr[j] = static_cast<char>((*currLStrHdl)->str[j]);

			currStr[currStrLen] = 0;

			stringArr[i] = currStr;

		}

		stringArrayPtrOut = new StringArray(stringArr, stringArrLen);

		for (int i = 0; i < stringArrLen; i++)

			delete[] stringArr[i];

		delete[] stringArr;

		*lvStringArrayPtrOut = reinterpret_cast<void *>(stringArrayPtrOut);

	}

	catch (const MdsException &e)

	{


		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_stringarray_constructor_stringLen(void **lvStringArrayPtrOut, const char *dataIn, int nStringsIn, int stringLenIn, ErrorCluster *error)

{

	StringArray *stringArrayPtrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		stringArrayPtrOut = new StringArray(const_cast<char *>(dataIn), nStringsIn, stringLenIn);

		*lvStringArrayPtrOut = reinterpret_cast<void *>(stringArrayPtrOut);

	}

	catch (const MdsException &e)

	{


		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}

EXPORT void mdsplus_stringarray_destructor(void **lvStringArrayPtr)
{
	deleteLvData(lvStringArrayPtr);
}

/********************************************************************************************************
												UINT16
 ********************************************************************************************************/

EXPORT void mdsplus_uint16_constructor(void **lvUint16PtrOut, unsigned short valIn, ErrorCluster *error) {
	scalarConstructor<Uint16>(__func__, error, lvUint16PtrOut, valIn);
}

EXPORT void mdsplus_uint16_destructor(void **lvUint16Ptr) {
	deleteLvData(lvUint16Ptr);
}

EXPORT void mdsplus_uint16_getByte(const void *lvUint16Ptr, char *byteOut, ErrorCluster *error)

{

	Uint16 *uint16Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		uint16Ptr = reinterpret_cast<Uint16 *>(const_cast<void *>(lvUint16Ptr));

		*byteOut = uint16Ptr->getByte();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_uint16_getDouble(const void *lvUint16Ptr, double *doubleOut, ErrorCluster *error)

{

	Uint16 *uint16Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		uint16Ptr = reinterpret_cast<Uint16 *>(const_cast<void *>(lvUint16Ptr));

		*doubleOut = uint16Ptr->getDouble();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_uint16_getFloat(const void *lvUint16Ptr, float *floatOut, ErrorCluster *error)

{

	Uint16 *uint16Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		uint16Ptr = reinterpret_cast<Uint16 *>(const_cast<void *>(lvUint16Ptr));

		*floatOut = uint16Ptr->getFloat();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_uint16_getInt(const void *lvUint16Ptr, int *intOut, ErrorCluster *error)

{

	Uint16 *uint16Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		uint16Ptr = reinterpret_cast<Uint16 *>(const_cast<void *>(lvUint16Ptr));

		*intOut = uint16Ptr->getInt();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_uint16_getLong(const void *lvUint16Ptr, int64_t *longOut, ErrorCluster *error)

{

	Uint16 *uint16Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		uint16Ptr = reinterpret_cast<Uint16 *>(const_cast<void *>(lvUint16Ptr));

		*longOut = uint16Ptr->getLong();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_uint16_getShort(const void *lvUint16Ptr, short *shortOut, ErrorCluster *error)

{

	Uint16 *uint16Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		uint16Ptr = reinterpret_cast<Uint16 *>(const_cast<void *>(lvUint16Ptr));

		*shortOut = uint16Ptr->getShort();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



/********************************************************************************************************

												UINT16ARRAY

 ********************************************************************************************************/



EXPORT void mdsplus_uint16array_constructor(void **lvUint16ArrayPtrOut, const LUShtArrHdl lvUShtArrHdlIn, ErrorCluster *error)

{

	Uint16Array *uint16ArrayPtrOut = NULL;

	unsigned short *uint16Arr = NULL;	

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		int uint16ArrLen = static_cast<int>((*lvUShtArrHdlIn)->dimSize);

		uint16Arr = new unsigned short[uint16ArrLen];

		for (int i = 0; i < uint16ArrLen; i++)

			uint16Arr[i] = static_cast<unsigned short>((*lvUShtArrHdlIn)->elt[i]);

		uint16ArrayPtrOut = new Uint16Array(uint16Arr, uint16ArrLen);

		delete[] uint16Arr;

		*lvUint16ArrayPtrOut = reinterpret_cast<void *>(uint16ArrayPtrOut);

	}

	catch (const MdsException &e)

	{

		delete[] uint16Arr;


		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_uint16array_constructor_dims(void **lvUint16ArrayPtrOut, const LUShtArrHdl lvUShtArrHdlIn, const LIntArrHdl lvIntArrHdlIn, ErrorCluster *error)

{

	Uint16Array *uint16ArrayPtrOut = NULL;

	unsigned short *uint16Arr = NULL;

	int *intArr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		int uint16ArrLen = static_cast<int>((*lvUShtArrHdlIn)->dimSize);

		uint16Arr = new unsigned short[uint16ArrLen];

		for (int i = 0; i < uint16ArrLen; i++)

			uint16Arr[i] = static_cast<unsigned short>((*lvUShtArrHdlIn)->elt[i]);

		int intArrLen = static_cast<int>((*lvIntArrHdlIn)->dimSize);

		intArr = new int[intArrLen];

		for (int i = 0; i < intArrLen; i++)

			intArr[i] = static_cast<int>((*lvIntArrHdlIn)->elt[i]);

		uint16ArrayPtrOut = new Uint16Array(uint16Arr, intArrLen, intArr);

		delete[] uint16Arr;

		delete[] intArr;

		*lvUint16ArrayPtrOut = reinterpret_cast<void *>(uint16ArrayPtrOut);

	}

	catch (const MdsException &e)

	{

		delete[] uint16Arr;

		delete[] intArr;


		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}

EXPORT void mdsplus_uint16array_destructor(void **lvUint16ArrayPtr)
{
	deleteLvData(lvUint16ArrayPtr);
}

/********************************************************************************************************
												UINT32
 ********************************************************************************************************/

EXPORT void mdsplus_uint32_constructor(void **lvUint32PtrOut, unsigned int valIn, ErrorCluster *error) {
	scalarConstructor<Uint32>(__func__, error, lvUint32PtrOut, valIn);
}

EXPORT void mdsplus_uint32_destructor(void **lvUint32Ptr) {
	deleteLvData(lvUint32Ptr);
}

EXPORT void mdsplus_uint32_getByte(const void *lvUint32Ptr, char *byteOut, ErrorCluster *error)

{

	Uint32 *uint32Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		uint32Ptr = reinterpret_cast<Uint32 *>(const_cast<void *>(lvUint32Ptr));

		*byteOut = uint32Ptr->getByte();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_uint32_getDouble(const void *lvUint32Ptr, double *doubleOut, ErrorCluster *error)

{

	Uint32 *uint32Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		uint32Ptr = reinterpret_cast<Uint32 *>(const_cast<void *>(lvUint32Ptr));

		*doubleOut = uint32Ptr->getDouble();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_uint32_getFloat(const void *lvUint32Ptr, float *floatOut, ErrorCluster *error)

{

	Uint32 *uint32Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		uint32Ptr = reinterpret_cast<Uint32 *>(const_cast<void *>(lvUint32Ptr));

		*floatOut = uint32Ptr->getFloat();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_uint32_getInt(const void *lvUint32Ptr, int *intOut, ErrorCluster *error)

{

	Uint32 *uint32Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		uint32Ptr = reinterpret_cast<Uint32 *>(const_cast<void *>(lvUint32Ptr));

		*intOut = uint32Ptr->getInt();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_uint32_getLong(const void *lvUint32Ptr, int64_t *longOut, ErrorCluster *error)

{

	Uint32 *uint32Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		uint32Ptr = reinterpret_cast<Uint32 *>(const_cast<void *>(lvUint32Ptr));

		*longOut = uint32Ptr->getLong();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_uint32_getShort(const void *lvUint32Ptr, short *shortOut, ErrorCluster *error)

{

	Uint32 *uint32Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		uint32Ptr = reinterpret_cast<Uint32 *>(const_cast<void *>(lvUint32Ptr));

		*shortOut = uint32Ptr->getShort();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



/********************************************************************************************************

												UINT32ARRAY

 ********************************************************************************************************/



EXPORT void mdsplus_uint32array_constructor(void **lvUint32ArrayPtrOut, const LUIntArrHdl lvUIntArrHdlIn, ErrorCluster *error)

{

	Uint32Array *uint32ArrayPtrOut = NULL;

	unsigned int *uint32Arr = NULL;	

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		int uint32ArrLen = static_cast<int>((*lvUIntArrHdlIn)->dimSize);

		uint32Arr = new unsigned int[uint32ArrLen];

		for (int i = 0; i < uint32ArrLen; i++)

			uint32Arr[i] = static_cast<unsigned int>((*lvUIntArrHdlIn)->elt[i]);

		uint32ArrayPtrOut = new Uint32Array(uint32Arr, uint32ArrLen);

		delete[] uint32Arr;

		*lvUint32ArrayPtrOut = reinterpret_cast<void *>(uint32ArrayPtrOut);

	}

	catch (const MdsException &e)

	{

		delete[] uint32Arr;


		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_uint32array_constructor_dims(void **lvUint32ArrayPtrOut, const LUIntArrHdl lvUIntArrHdlIn, const LIntArrHdl lvIntArrHdlDimIn, ErrorCluster *error)

{

	Uint32Array *uint32ArrayPtrOut = NULL;

	unsigned int *uint32Arr = NULL;

	int *intArr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		int uint32ArrLen = static_cast<int>((*lvUIntArrHdlIn)->dimSize);

		uint32Arr = new unsigned int[uint32ArrLen];

		for (int i = 0; i < uint32ArrLen; i++)

			uint32Arr[i] = static_cast<unsigned int>((*lvUIntArrHdlIn)->elt[i]);

		int intArrLen = static_cast<int>((*lvIntArrHdlDimIn)->dimSize);

		intArr = new int[intArrLen];

		for (int i = 0; i < intArrLen; i++)

			intArr[i] = static_cast<int>((*lvIntArrHdlDimIn)->elt[i]);

		uint32ArrayPtrOut = new Uint32Array(uint32Arr, intArrLen, intArr);

		delete[] uint32Arr;

		delete[] intArr;

		*lvUint32ArrayPtrOut = reinterpret_cast<void *>(uint32ArrayPtrOut);

	}

	catch (const MdsException &e)

	{

		delete[] uint32Arr;

		delete[] intArr;

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}

EXPORT void mdsplus_uint32array_destructor(void **lvUint32ArrayPtr)
{
	deleteLvData(lvUint32ArrayPtr);
}

/********************************************************************************************************
												UINT64
 ********************************************************************************************************/

EXPORT void mdsplus_uint64_constructor(void **lvUint64PtrOut, uint64_t valIn, ErrorCluster *error) {
	scalarConstructor<Uint64>(__func__, error, lvUint64PtrOut, valIn);
}

EXPORT void mdsplus_uint64_destructor(void **lvUint64Ptr) {
	deleteLvData(lvUint64Ptr);
}

EXPORT void mdsplus_uint64_getByte(const void *lvUint64Ptr, char *byteOut, ErrorCluster *error)

{

	Uint64 *uint64Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		uint64Ptr = reinterpret_cast<Uint64 *>(const_cast<void *>(lvUint64Ptr));

		*byteOut = uint64Ptr->getByte();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_uint64_getDate(const void *lvUint64Ptr, LStrHandle lvStrHdlOut, ErrorCluster *error)

{

	Uint64 *uint64Ptr = NULL;

	char *strOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		uint64Ptr = reinterpret_cast<Uint64 *>(const_cast<void *>(lvUint64Ptr));

		strOut = uint64Ptr->getDate();

		std::size_t strOutLen = std::strlen(strOut);

		errorCode = NumericArrayResize(uB, 1, reinterpret_cast<UHandle *>(&lvStrHdlOut), strOutLen+sizeof(int32));

		if (!errorCode)

		{

			MoveBlock(reinterpret_cast<uChar *>(strOut), LStrBuf(*lvStrHdlOut), strOutLen);

			(*lvStrHdlOut)->cnt = strOutLen;

		}

		else

			errorMessage = (char *)"NumericArrayResize error";

		deleteNativeArray(strOut);

	}

	catch (const MdsException &e)

	{


		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_uint64_getDouble(const void *lvUint64Ptr, double *doubleOut, ErrorCluster *error)

{

	Uint64 *uint64Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		uint64Ptr = reinterpret_cast<Uint64 *>(const_cast<void *>(lvUint64Ptr));

		*doubleOut = uint64Ptr->getDouble();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_uint64_getFloat(const void *lvUint64Ptr, float *floatOut, ErrorCluster *error)

{

	Uint64 *uint64Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		uint64Ptr = reinterpret_cast<Uint64 *>(const_cast<void *>(lvUint64Ptr));

		*floatOut = uint64Ptr->getFloat();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_uint64_getInt(const void *lvUint64Ptr, int *intOut, ErrorCluster *error)

{

	Uint64 *uint64Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		uint64Ptr = reinterpret_cast<Uint64 *>(const_cast<void *>(lvUint64Ptr));

		*intOut = uint64Ptr->getInt();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_uint64_getLong(const void *lvUint64Ptr, int64_t *longOut, ErrorCluster *error)

{

	Uint64 *uint64Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		uint64Ptr = reinterpret_cast<Uint64 *>(const_cast<void *>(lvUint64Ptr));

		*longOut = uint64Ptr->getLong();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_uint64_getShort(const void *lvUint64Ptr, short *shortOut, ErrorCluster *error)

{

	Uint64 *uint64Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		uint64Ptr = reinterpret_cast<Uint64 *>(const_cast<void *>(lvUint64Ptr));

		*shortOut = uint64Ptr->getShort();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



/********************************************************************************************************

												UINT64ARRAY

 ********************************************************************************************************/



EXPORT void mdsplus_uint64array_constructor(void **lvUint64ArrayPtrOut, const LULngArrHdl lvULngArrHdlIn, ErrorCluster *error)

{

	Uint64Array *uint64ArrayPtrOut = NULL;

	uint64_t *uint64Arr = NULL;	

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		int uint64ArrLen = static_cast<int>((*lvULngArrHdlIn)->dimSize);

		uint64Arr = new uint64_t[uint64ArrLen];

		for (int i = 0; i < uint64ArrLen; i++)

			uint64Arr[i] = static_cast<uint64_t>((*lvULngArrHdlIn)->elt[i]);

		uint64ArrayPtrOut = new Uint64Array(uint64Arr, uint64ArrLen);

		delete[] uint64Arr;

		*lvUint64ArrayPtrOut = reinterpret_cast<void *>(uint64ArrayPtrOut);

	}

	catch (const MdsException &e)

	{

		delete[] uint64Arr;


		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_uint64array_constructor_dims(void **lvUint64ArrayPtrOut, const LULngArrHdl lvULngArrHdlIn, const LIntArrHdl lvIntArrHdlIn, ErrorCluster *error)

{

	Uint64Array *uint64ArrayPtrOut = NULL;

	uint64_t *uint64Arr = NULL;

	int *intArr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		int uint64ArrLen = static_cast<int>((*lvULngArrHdlIn)->dimSize);

		uint64Arr = new uint64_t[uint64ArrLen];

		for (int i = 0; i < uint64ArrLen; i++)

			uint64Arr[i] = static_cast<uint64_t>((*lvULngArrHdlIn)->elt[i]);

		int intArrLen = static_cast<int>((*lvIntArrHdlIn)->dimSize);

		intArr = new int[intArrLen];

		for (int i = 0; i < intArrLen; i++)

			intArr[i] = static_cast<int>((*lvIntArrHdlIn)->elt[i]);

		uint64ArrayPtrOut = new Uint64Array(uint64Arr, intArrLen, intArr);

		delete[] uint64Arr;

		delete[] intArr;

		*lvUint64ArrayPtrOut = reinterpret_cast<void *>(uint64ArrayPtrOut);

	}

	catch (const MdsException &e)

	{

		delete[] uint64Arr;

		delete[] intArr;


		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}

EXPORT void mdsplus_uint64array_destructor(void **lvUint64ArrayPtr)
{
	deleteLvData(lvUint64ArrayPtr);
}

/********************************************************************************************************
												UINT8
 ********************************************************************************************************/

EXPORT void mdsplus_uint8_constructor(void **lvUint8PtrOut, unsigned char valIn, ErrorCluster *error) {
	scalarConstructor<Uint8>(__func__, error, lvUint8PtrOut, valIn);
}

EXPORT void mdsplus_uint8_destructor(void **lvUint8Ptr) {
	deleteLvData(lvUint8Ptr);
}

EXPORT void mdsplus_uint8_getByte(const void *lvUint8Ptr, char *byteOut, ErrorCluster *error)

{

	Uint8 *uint8Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		uint8Ptr = reinterpret_cast<Uint8 *>(const_cast<void *>(lvUint8Ptr));

		*byteOut = uint8Ptr->getByte();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_uint8_getDouble(const void *lvUint8Ptr, double *doubleOut, ErrorCluster *error)

{

	Uint8 *uint8Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		uint8Ptr = reinterpret_cast<Uint8 *>(const_cast<void *>(lvUint8Ptr));

		*doubleOut = uint8Ptr->getDouble();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_uint8_getFloat(const void *lvUint8Ptr, float *floatOut, ErrorCluster *error)

{

	Uint8 *uint8Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		uint8Ptr = reinterpret_cast<Uint8 *>(const_cast<void *>(lvUint8Ptr));

		*floatOut = uint8Ptr->getFloat();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_uint8_getInt(const void *lvUint8Ptr, int *intOut, ErrorCluster *error)

{

	Uint8 *uint8Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		uint8Ptr = reinterpret_cast<Uint8 *>(const_cast<void *>(lvUint8Ptr));

		*intOut = uint8Ptr->getInt();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_uint8_getLong(const void *lvUint8Ptr, int64_t *longOut, ErrorCluster *error)

{

	Uint8 *uint8Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		uint8Ptr = reinterpret_cast<Uint8 *>(const_cast<void *>(lvUint8Ptr));

		*longOut = uint8Ptr->getLong();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_uint8_getShort(const void *lvUint8Ptr, short *shortOut, ErrorCluster *error)

{

	Uint8 *uint8Ptr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		uint8Ptr = reinterpret_cast<Uint8 *>(const_cast<void *>(lvUint8Ptr));

		*shortOut = uint8Ptr->getShort();

	}

	catch (const MdsException &e)

	{

		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



/********************************************************************************************************

												UINT8ARRAY

 ********************************************************************************************************/



EXPORT void mdsplus_uint8array_constructor(void **lvUint8ArrayPtrOut, const LUByteArrHdl lvUByteArrHdlIn, ErrorCluster *error)

{

	Uint8Array *uint8ArrayPtrOut = NULL;

	unsigned char *uint8Arr = NULL;	

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		int uint8ArrLen = static_cast<int>((*lvUByteArrHdlIn)->dimSize);

		uint8Arr = new unsigned char[uint8ArrLen];

		for (int i = 0; i < uint8ArrLen; i++)

			uint8Arr[i] = static_cast<unsigned char>((*lvUByteArrHdlIn)->elt[i]);

		uint8ArrayPtrOut = new Uint8Array(uint8Arr, uint8ArrLen);

		delete[] uint8Arr;

		*lvUint8ArrayPtrOut = reinterpret_cast<void *>(uint8ArrayPtrOut);

	}

	catch (const MdsException &e)

	{

		delete[] uint8Arr;


		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_uint8array_constructor_dims(void **lvUint8ArrayPtrOut, const LUByteArrHdl lvUByteArrHdlIn, const LIntArrHdl lvIntArrHdlIn, ErrorCluster *error)

{

	Uint8Array *uint8ArrayPtrOut = NULL;

	unsigned char *uint8Arr = NULL;

	int *intArr = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		int uint8ArrLen = static_cast<int>((*lvUByteArrHdlIn)->dimSize);

		uint8Arr = new unsigned char[uint8ArrLen];

		for (int i = 0; i < uint8ArrLen; i++)

			uint8Arr[i] = static_cast<unsigned char>((*lvUByteArrHdlIn)->elt[i]);

		int intArrLen = static_cast<int>((*lvIntArrHdlIn)->dimSize);

		intArr = new int[intArrLen];

		for (int i = 0; i < intArrLen; i++)

			intArr[i] = static_cast<int>((*lvIntArrHdlIn)->elt[i]);

		uint8ArrayPtrOut = new Uint8Array(uint8Arr, intArrLen, intArr);

		delete[] uint8Arr;

		delete[] intArr;

		*lvUint8ArrayPtrOut = reinterpret_cast<void *>(uint8ArrayPtrOut);

	}

	catch (const MdsException &e)

	{

		delete[] uint8Arr;

		delete[] intArr;


		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}



EXPORT void mdsplus_uint8array_deserialize(const void *lvUint8ArrayPtr, void **lvDataPtrOut, ErrorCluster *error)

{

	Uint8Array *uint8ArrayPtr = NULL;

	Data *dataPtrOut = NULL;

	MgErr errorCode = noErr;

	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try

	{

		uint8ArrayPtr = reinterpret_cast<Uint8Array *>(const_cast<void *>(lvUint8ArrayPtr));

		dataPtrOut = uint8ArrayPtr->deserialize();

		*lvDataPtrOut = reinterpret_cast<void *>(dataPtrOut);

	}

	catch (const MdsException &e)

	{


		errorCode = bogusError;

		errorMessage = const_cast<char *>(e.what());

		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);

}

EXPORT void mdsplus_uint8array_destructor(void **lvUint8ArrayPtr)
{
	deleteLvData(lvUint8ArrayPtr);
}

////////////////////////////////TEMPORARY TEST ROUTINES///////////////////////////


EXPORT void prova1()
{
  std::cout << "CIAO SONO PROVA1\n";
}

EXPORT void prova2()
{
  try {
    MDSplus::Tree *tree = new Tree("CACCA", -1);
  }catch(MdsException &exc){std::cout << exc.what() << std::endl;}
  std::cout << "CIAO SONO PROVA2\n";
}

EXPORT void prova3(int inInt, int *outInt, ErrorCluster *error)
{
  MgErr errorCode = noErr;
  const char *errorSource = __FUNCTION__;
  char const * errorMessage = "";
  *outInt = inInt;
  fillErrorCluster(errorCode, errorSource, errorMessage, error);
}


EXPORT void prova4(int inInt, int *outInt, ErrorCluster *error)
{
  MgErr errorCode = noErr;
  const char *errorSource = __FUNCTION__;
  char const * errorMessage = ""; 
  int i;
  try {
    MDSplus::Tree *tree = new Tree("CACCA", -1);
  }catch(const MdsException &exc)
  { 
      i = 0;
  }
 
  *outInt = inInt;
  fillErrorCluster(errorCode, errorSource, errorMessage, error);
}
EXPORT void prova41(int inInt, int *outInt, ErrorCluster *error)
{
  MgErr errorCode = noErr;
  const char *errorSource = __FUNCTION__;
  char const * errorMessage = "";
  int i;
  try {
    MDSplus::Tree *tree = new Tree("CACCA", -1);
  }catch(const MdsException &exc)
  { 
      i = 0;
  }
 
  *outInt = inInt;
  errorMessage = (char *)"CAZZACICCIA";	
  errorCode = bogusError;
  fillErrorCluster(errorCode, errorSource, errorMessage, error);
}
EXPORT void prova40(int inInt, int *outInt, ErrorCluster *error)
{
  MgErr errorCode = noErr;
  const char *errorSource = __FUNCTION__;
  char const * errorMessage = ""; 
  int i;
  *outInt = inInt;
  errorMessage = (char *)"CAZZACICCIA";	
  errorCode = bogusError;
  fillErrorCluster(errorCode, errorSource, errorMessage, error);
}
EXPORT void prova42(int inInt, int *outInt, ErrorCluster *error)
{
  MgErr errorCode = noErr;
  const char *errorSource = __FUNCTION__;
  char const * errorMessage = ""; 
  int i;
  try {
    MDSplus::Tree *tree = new Tree("CACCA", -1);
  }catch(const MdsException &exc)
  { 
      errorMessage = (char *)"CAZZACICCIA";	
      errorCode = bogusError;
   }
 
  *outInt = inInt;
  fillErrorCluster(errorCode, errorSource, errorMessage, error);
}
EXPORT void prova43(int inInt, int *outInt, ErrorCluster *error)
{
  MgErr errorCode = noErr;
  const char *errorSource = __FUNCTION__;
  char const * errorMessage = "";
  int i;
  try {
    MDSplus::Tree *tree = new Tree("CACCA", -1);
  }catch(const MdsException &exc)
  { 
      char *currErr = (char *)exc.what();
      errorMessage = (char *)"CAZZACICCIA";	
      errorCode = bogusError;
   }
 
  *outInt = inInt;
  fillErrorCluster(errorCode, errorSource, errorMessage, error);
}
EXPORT void prova44(int inInt, int *outInt, ErrorCluster *error)
{
  MgErr errorCode = noErr;
  const char *errorSource = __FUNCTION__;
//  
  char const * errorMessage = ""; 
  int i;
  try {
    MDSplus::Tree *tree = new Tree("CACCA", -1);
  }catch(const MdsException &exc)
  {
//      char *currErr = (char *)exc.what();
//      errorMessage = strdup(currErr);
      errorCode = bogusError;
   }

  *outInt = inInt;
//  fillErrorCluster(errorCode, errorSource, errorMessage, error);
  fillErrorCluster(errorCode, errorSource, "", error);
}

EXPORT void prova45(int inInt, int *outInt, ErrorCluster *error)
{
  MgErr errorCode = noErr;
  const char *errorSource = __FUNCTION__;
  char const * errorMessage = ""; 
  int i;
  try {
    MDSplus::Tree *tree = new Tree("CACCA", -1);
  }catch(const MdsException &exc)
  { 
      char *currErr = (char *)exc.what();
      errorMessage = strdup(currErr);
      errorCode = bogusError;
   }
 
  *outInt = inInt;
  fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void prova5(int inInt, int *outInt, ErrorCluster *error)
{
  MgErr errorCode = noErr;
  const char *errorSource = __FUNCTION__;
  char const * errorMessage = ""; 
  int i;
  try {
    MDSplus::Tree *tree = new Tree("CACCA", -1);
  }catch(const MdsException &exc)
  { 
     fillErrorCluster(bogusError, errorSource, exc.what(), error);
     return;
   }
 
  *outInt = inInt;
  fillErrorCluster(errorCode, errorSource, errorMessage, error);
}
 
EXPORT void prova6(int inInt, int *outInt, ErrorCluster *error)
{
  MgErr errorCode = noErr;
  const char *errorSource = __FUNCTION__;
  char const * errorMessage = ""; 
  try {
    MDSplus::Data *d = new Int32(123);
  }catch(const MdsException &exc)
  {
      errorCode = bogusError;
      errorMessage = const_cast<char *>(exc.what());

  }
}

EXPORT void prova7(int inInt, int *outInt, ErrorCluster *error)
{
  MgErr errorCode = noErr;
  const char *errorSource = __FUNCTION__;
  char const * errorMessage = ""; 
  try {
    MDSplus::Tree *tree = new Tree("test", -1);
  }catch(const MdsException &exc)
  {
      errorCode = bogusError;
      errorMessage = const_cast<char *>(exc.what());

  }

  *outInt = inInt;
  fillErrorCluster(errorCode, errorSource, errorMessage, error);
}
 

EXPORT void prova8()
{
    Int64 *timeInsertedData = new Int64(0);
    Data *retTimeStr = executeWithArgs("date_time(0)", 0);
   std::cout << "FATTO EXECUTE WITH ARGS" << std::endl;
   std::cout << "FATTO EXECUTE WITH ARGS" << (void *)retTimeStr << std::endl;

    std::cout << retTimeStr->getString() << std::endl;
} 
 
EXPORT void prova9()
{
    Int64 *timeInsertedData = new Int64(0);
    Data *retTimeStr = executeWithArgs("date_time($)", 1, timeInsertedData);
   std::cout << "FATTO EXECUTE WITH ARGS" << std::endl;

    std::cout << retTimeStr->getString() << std::endl;
} 
EXPORT void prova10()
{
    Int32 *timeInsertedData = new Int32(0);
    Data *retTimeStr = executeWithArgs("date_time($)", 1, timeInsertedData);

    std::cout << retTimeStr->getString() << std::endl;
} 

EXPORT void prova11()
{
    try {
      Tree * tree = new Tree("test", 1);
      TreeNode *n = tree->getNode("SIG");
      n->deleteData();
      Data *start = new Float32(0);
      Data *end = new Float32(9);
      Data *delta = new Float32(1);
      Data *dim = new Range(start, end, delta);
      float *floatArr = new float[10];
      for(int i = 0; i < 10; i++)
	floatArr[i] = i;
      Data *arrD = new Float32Array(floatArr, 10);
      n->makeSegment(start, end, dim, (Array *)arrD);
    }catch(MdsException &exc)
    {
      std::cout << "ERRORE: " << exc.what() << std::endl;
    }
    std::cout << "FINITO CACCONA www\n";
}

 
}//End Extern "C"
}//End Namespace MDSplus


