#ifndef MDSOBJECTS_H
#define MDSOBJECTS_H
#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#include <mdsplus/mdsplus.h>
#include <mdsplus/ConditionVar.hpp>
#include <mdsplus/Mutex.hpp>
#include <config.h>
#include <dbidef.h>
#include <ncidef.h>
#include <mdstypes.h>
#include <usagedef.h>

#include <algorithm>
#include <complex>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#ifdef MDS_WINDOWS
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <semaphore.h>
#endif
#if __cplusplus >= 201103L
#define NOEXCEPT noexcept
#else
#define NOEXCEPT throw()
#define nullptr NULL
#endif
#define DTYPE_BU 2 
#define DTYPE_WU 3 
#define DTYPE_LU 4 
#define DTYPE_QU 5 
#define DTYPE_OU 25 
#define DTYPE_B 6 
#define DTYPE_W 7 
#define DTYPE_L 8 
#define DTYPE_Q 9 
#define DTYPE_O 26 
#define DTYPE_FLOAT 52 
#define DTYPE_DOUBLE 53 
#define DTYPE_F	10		
#define DTYPE_D	11	
#define DTYPE_G	27	
#define DTYPE_H	28		
#define DTYPE_FSC 54
#define DTYPE_FTC 55
#define DTYPE_T 14 
#define DTYPE_IDENT 191 
#define DTYPE_NID 192 
#define DTYPE_PATH 193 
#define DTYPE_PARAM 194 
#define DTYPE_SIGNAL 195 
#define DTYPE_DIMENSION 196 
#define DTYPE_WINDOW 197 
#define DTYPE_SLOPE 198 
#define DTYPE_FUNCTION 199 
#define DTYPE_CONGLOM 200 
#define DTYPE_RANGE 201 
#define DTYPE_ACTION 202 
#define DTYPE_DISPATCH 203 
#define DTYPE_PROGRAM 204 
#define DTYPE_ROUTINE 205 
#define DTYPE_PROCEDURE 206 
#define DTYPE_METHOD 207 
#define DTYPE_DEPENDENCY 208 
#define DTYPE_CONDITION 209 
#define DTYPE_EVENT 210 
#define DTYPE_WITH_UNITS 211 
#define DTYPE_CALL 212 
#define DTYPE_WITH_ERROR 213 
#define DTYPE_LIST 214
#define DTYPE_TUPLE 215
#define DTYPE_DICTIONARY 216
#define DTYPE_POINTER 51
#define DTYPE_DSC	24		
#define TreeNEGATE_CONDITION 	7
#define TreeIGNORE_UNDEFINED 	8
#define TreeIGNORE_STATUS	9
#define TreeDEPENDENCY_AND	10
#define TreeDEPENDENCY_OR	11
#define CLASS_S 1 
#define CLASS_D 2 
#define CLASS_A 4 
#define CLASS_R 194 
#define CLASS_APD 196 
#define MAX_DIMS 32

extern "C" char *MdsGetMsg(int status);
/*
extern "C" {
	void *convertToScalarDsc(int clazz, int dtype, int length, char *ptr);
	void *evaluateData(void *dscPtr, int isEvaluate, int *status);
	void freeDsc(void *dscPtr);
	void *convertFromDsc(void *dscPtr);
	char *decompileDsc(void *dscPtr);
	char *decompileDsc(void *dscPtr);
	void *compileFromExprWithArgs(char *expr, int nArgs, void *args, void *tree);
	void freeChar(void *);
	void *convertToArrayDsc(int clazz, int dtype, int length, int l_length, int nDims, int *dims, void *ptr);
	void *convertToCompoundDsc(int clazz, int dtype, int length, void *ptr, int ndescs, void **descs);
	void *convertToApdDsc(int type, int ndescs, void **ptr);
	void *deserializeData(char *serialized, int size);
	void * convertToByte(void *dsc); 
	void * convertToShort(void *dsc); 
	void * convertToInt(void *dsc); 
	void * convertToLong(void *dsc); 
	void * convertToFloat(void *dsc); 
	void * convertToDouble(void *dsc); 
	void * convertToShape(void *dcs);
}
*/


namespace MDSplus  {
//Required for handling dynamic memory allocated in a different DLL on windows
//in Debug configuration
//MUST revert to old version. Winodws VS crashes otherwise!! The code MUST be moved from include 
class Tree;
EXPORT void setActiveTree(Tree *tree);
EXPORT Tree *getActiveTree();
/////Exceptions//////////////
class EXPORT MdsException: public std::exception {
public:
	MdsException(const char *msg): msg(msg) { }
	//MdsException(int status): msg(MdsGetMsg(status)) { }
	MdsException(int status);
	virtual ~MdsException() NOEXCEPT { }

	virtual const char* what() const NOEXCEPT {
		return msg;
	}

	friend EXPORT std::ostream & operator << (std::ostream &outStream, MdsException &exc) {
		return outStream << exc.what();
	}

protected:
	const char *msg;
};

////////////////////Data class//////////////////////////////
class Data;
EXPORT void deleteData(Data *);

class EXPORT Data 
{
public:
	Data():
		refCount(1),
		units(nullptr),
		error(nullptr),
		help(nullptr),
		validation(nullptr)
	{
	}

	virtual ~Data();

	void *operator new(size_t sz);
	void operator delete(void *p);

	static void incRefCount(Data * d);
	static void decRefCount(Data * d);
	void incRefCount();
	void decRefCount();

	virtual Data *getUnits();
	virtual Data *getError();
	virtual Data *getHelp();
	virtual Data *getValidation();
	virtual void setUnits(Data *inUnits);
	virtual void setError(Data *inError);
	virtual void setHelp(Data *inHelp);
	virtual void setValidation(Data *inValidation);

private:
		friend EXPORT Data *compile(const char *expr);
		friend EXPORT Data *compileWithArgs(const char *expr, int nArgs ...);
		friend EXPORT Data *compile(const char *expr, Tree *tree);
		friend EXPORT Data *compileWithArgs(const char *expr, Tree *tree, int nArgs ...);
		friend EXPORT Data *execute(const char *expr);
		friend EXPORT Data *executeWithArgs(const char *expr, int nArgs ...);
		friend EXPORT Data *execute(const char *expr, Tree *tree);
		friend EXPORT Data *executeWithArgs(const char *expr, Tree *tree, int nArgs ...);
		friend EXPORT Data *deserialize(char const * serialized);
		friend EXPORT Data *deserialize(Data *serialized);
		virtual void propagateDeletion(){}
	
public:
		int clazz, dtype;
		void *completeConversionToDsc(void *dsc);
		int refCount;
		virtual void getInfo(char *clazz, char *dtype, short *length, char *nDims, int **dims, void **ptr)
		{
			*clazz = this->clazz;
			*dtype = this->dtype;
			*length = 0;
			*nDims = 0;
			*dims = 0;
			*ptr = 0;
		}
		virtual void *convertToDsc() = 0;
		virtual bool equals(Data *data) {return false;}
		Data *evaluate();
		virtual Data *data();
		char *decompile();
		Data *clone();
		char *	serialize(int *size);
		virtual char getByte(); 
		virtual short getShort();
		virtual int getInt();
		virtual int64_t getLong();
		virtual unsigned char getByteUnsigned(); 
		virtual unsigned short getShortUnsigned();
		virtual unsigned int getIntUnsigned();
		virtual uint64_t getLongUnsigned();
		virtual float getFloat();
		virtual double getDouble(); 
		virtual std::complex<double> getComplex() {throw new MdsException("getComplex() not supported for non compelx data types");}
		virtual char * getString(){return decompile();}
		virtual int * getShape(int *numDim);
		virtual char *getByteArray(int *numElements);
		virtual std::vector<char> getByteArray();
		virtual short * getShortArray(int *numElements);
		virtual std::vector<short> getShortArray();
		virtual int * getIntArray(int *numElements);
		virtual std::vector<int> getIntArray();
		virtual int64_t * getLongArray(int *numElements);
		virtual std::vector<int64_t> getLongArray();
		virtual float * getFloatArray(int *numElements);
		virtual std::vector<float> getFloatArray();
		virtual unsigned char *getByteUnsignedArray(int *numElements);
		virtual std::vector<unsigned char> getByteUnsignedArray();
		virtual unsigned short *getShortUnsignedArray(int *numElements);
		virtual std::vector<unsigned  short> getShortUnsignedArray();
		virtual unsigned int *getIntUnsignedArray(int *numElements);
		virtual std::vector<unsigned int> getIntUnsignedArray();
		virtual uint64_t *getLongUnsignedArray(int *numElements);
		virtual std::vector<uint64_t> getLongUnsignedArray();
		virtual double * getDoubleArray(int *numElements);
		virtual std::vector<double> getDoubleArray();
		virtual std::complex<double> * getComplexArray(int *numElements)
		{
			throw MdsException("getComplexArray() not supported for non compelx data types");
		}
		virtual std::vector<std::complex<double> > getComplexArray();
		virtual char ** getStringArray(int *numElements)
		{
			*numElements = 0; 
			return NULL;
		}
		virtual Data *getDimensionAt(int dimIdx);
		virtual int getSize() {return 1;}

		friend EXPORT std::ostream & operator << (std::ostream &outStream, MDSplus::Data *data);

		virtual void plot();

protected:
	virtual bool isImmutable() { return true; }

	void setAccessory(Data *units, Data *error, Data *help, Data *validation) {
		this->units = units;
		this->error = error;
		this->help = help;
		this->validation = validation;
	}

private:
	Data * getData(int classType, int dataType);
	Data * getArrayData(int dataType);
	Data * getScalarData(int dataType);
	Data *units;
	Data *error;
	Data *help;
	Data *validation;
};

class Empty: public Data {
};

	class EXPORT Scalar: public Data
	{
	public:
		Scalar(): length(0), ptr(0) {
			clazz = CLASS_S;
		}

		int length;
		char *ptr;
		virtual ~Scalar()
		{
			delete []ptr;
		}
		 
		virtual void *convertToDsc();
		virtual void getInfo(char *clazz, char *dtype, short *length, char *nDims, int **dims, void **ptr)
		{
			*clazz = this->clazz;
			*dtype = this->dtype;
			*length = this->length;
			*nDims = 0;
			*dims = 0;
			*ptr = this->ptr;
		}
	}; //ScalarData
	class  EXPORT Int8 : public Scalar
	{
	public:
		Int8(char val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_B;
			length = 1;
			ptr = new char[1];
			ptr[0] = val;
			setAccessory(units, error, help, validation);
		}
		char getByte() {return ptr[0];}
		short getShort() {return (short)ptr[0];}
		int getInt() {return (int)ptr[0];}
		int64_t getLong() {return (int64_t)ptr[0];}
		unsigned char getByteUnsigned() {return (unsigned char)ptr[0];}
		unsigned short getShortUnsigned() {return (unsigned short)ptr[0];}
		unsigned int getIntUnsigned() {return (unsigned int)ptr[0];}
		uint64_t getLongUnsigned() {return (uint64_t)ptr[0];}
		float getFloat() {return (float)ptr[0];}
		double getDouble() {return (double)ptr[0];}
	};
	class  EXPORT Uint8 : public Scalar
	{
	public:
		Uint8(unsigned char val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_BU;
			length = 1;
			ptr = new char[1];
			ptr[0] = val;
			setAccessory(units, error, help, validation);
		}
		char getByte() {return ptr[0];}
		short getShort() {return (short)ptr[0];}
		int getInt() {return (int)ptr[0];}
		int64_t getLong() {return (int64_t)ptr[0];}
		unsigned char getByteUnsigned() {return (unsigned char)ptr[0];}
		unsigned short getShortUnsigned() {return (unsigned short)ptr[0];}
		unsigned int getIntUnsigned() {return (unsigned int)ptr[0];}
		uint64_t getLongUnsigned() {return (uint64_t)ptr[0];}
		float getFloat() {return (float)ptr[0];}
		double getDouble() {return (double)ptr[0];}
	};
	class  EXPORT Int16 : public Scalar
	{
	public:
		Int16(short val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_W;
			length = 2;
			ptr = new char[2];
			*(short *)ptr = val;
			setAccessory(units, error, help, validation);
		}
		char getByte() {return (char)(*(short *)ptr);}
		short getShort() {return (short)(*(short *)ptr);}
		int getInt() {return (int)(*(short *)ptr);}
		int64_t getLong() {return (int64_t)(*(short *)ptr);}
		unsigned char getByteUnsigned() {return (unsigned char)(*(short *)ptr);}
		unsigned short getShortUnsigned() {return (unsigned short)(*(short *)ptr);}
		unsigned int getIntUnsigned() {return (unsigned int)(*(short *)ptr);}
		uint64_t getLongUnsigned() {return (uint64_t)(*(short *)ptr);}
		float getFloat() {return (float)(*(short *)ptr);}
		double getDouble() {return (double)(*(short *)ptr);}
	};
	class  EXPORT Uint16 : public Scalar
	{
	public:
		Uint16(unsigned short val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_WU;
			length = 2;
			ptr = new char[2];
			*(unsigned short *)ptr = val;
			setAccessory(units, error, help, validation);
		}
		char getByte() {return (char)(*(short *)ptr);}
		short getShort() {return (short)(*(short *)ptr);}
		int getInt() {return (int)(*(short *)ptr);}
		int64_t getLong() {return (int64_t)(*(short *)ptr);}
		unsigned char getByteUnsigned() {return (unsigned char)(*(unsigned short *)ptr);}
		unsigned short getShortUnsigned() {return (unsigned short)(*(unsigned short *)ptr);}
		unsigned int getIntUnsigned() {return (unsigned int)(*(unsigned short *)ptr);}
		uint64_t getLongUnsigned() {return (uint64_t)(*(unsigned short *)ptr);}
		float getFloat() {return (float)(*(short *)ptr);}
		double getDouble() {return (double)(*(short *)ptr);}
	};
	class  EXPORT Int32 : public Scalar
	{
	public:
		Int32(int val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_L;
			length = sizeof(int);
			ptr = new char[sizeof(int)];
			*(int *)ptr = val;
			setAccessory(units, error, help, validation);
		}
		char getByte() {return (char)(*(int *)ptr);}
		short getShort() {return (short)(*(int *)ptr);}
		int getInt() {return (int)(*(int *)ptr);}
		int64_t getLong() {return (int64_t)(*(int *)ptr);}
		unsigned char getByteUnsigned() {return (unsigned char)(*(int *)ptr);}
		unsigned short getShortUnsigned() {return (unsigned short)(*(int *)ptr);}
		unsigned int getIntUnsigned() {return (unsigned int)(*(int *)ptr);}
		uint64_t getLongUnsigned() {return (uint64_t)(*(int *)ptr);}
		float getFloat() {return (float)(*(int *)ptr);}
		double getDouble() {return (double)(*(int *)ptr);}
	};
	class  EXPORT Uint32 : public Scalar
	{
	public:
		Uint32(unsigned int val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_LU;
			length = sizeof(int);
			ptr = new char[sizeof(int)];
			*(unsigned int *)ptr = val;
			setAccessory(units, error, help, validation);
		}
		char getByte() {return (char)(*(int *)ptr);}
		short getShort() {return (short)(*(int *)ptr);}
		int getInt() {return (int)(*(int *)ptr);}
		int64_t getLong() {return (int64_t)(*(int *)ptr);}
		unsigned char getByteUnsigned() {return (unsigned char)(*(unsigned int *)ptr);}
		unsigned short getShortUnsigned() {return (unsigned short)(*(unsigned int *)ptr);}
		unsigned int getIntUnsigned() {return (unsigned int)(*(unsigned int *)ptr);}
		uint64_t getLongUnsigned() {return (uint64_t)(*(unsigned int *)ptr);}
		float getFloat() {return (float)(*(int *)ptr);}
		double getDouble() {return (double)(*(int *)ptr);}
	};
	class EXPORT Int64 : public Scalar
	{
	public:
		Int64(int64_t val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_Q;
			length = sizeof(int64_t);
			ptr = new char[sizeof(int64_t)];
			*(int64_t *)ptr = val;
			setAccessory(units, error, help, validation);
		}
		char getByte() {return (char)(*(int64_t *)ptr);}
		short getShort() {return (short)(*(int64_t *)ptr);}
		int getInt() {return (int)(*(int64_t *)ptr);}
		int64_t getLong() {return (int64_t)(*(int64_t *)ptr);}
		unsigned char getByteUnsigned() {return (unsigned char)(*(int64_t *)ptr);}
		unsigned short getShortUnsigned() {return (unsigned short)(*(int64_t *)ptr);}
		unsigned int getIntUnsigned() {return (unsigned int)(*(int64_t *)ptr);}
		uint64_t getLongUnsigned() {return (uint64_t)(*(int64_t *)ptr);}
		float getFloat() {return (float)(*(int64_t *)ptr);}
		double getDouble() {return (double)(*(int64_t *)ptr);}
	};
	class EXPORT Uint64 : public Scalar
	{
	public:
		Uint64(uint64_t val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_QU;
			length = sizeof(int64_t);
			ptr = new char[sizeof(int64_t)];
			*(uint64_t *)ptr = val;
			setAccessory(units, error, help, validation);
		}
		unsigned char getByteUnsigned() {return (unsigned char)(*(uint64_t *)ptr);}
		unsigned short getShortUnsigned() {return (unsigned short)(*(uint64_t *)ptr);}
		unsigned int getIntUnsigned() {return (unsigned int)(*(uint64_t *)ptr);}
		uint64_t getLongUnsigned() {return (uint64_t)(*(uint64_t *)ptr);}
		char getByte() {return (char)(*(int64_t *)ptr);}
		short getShort() {return (short)(*(int64_t *)ptr);}
		int getInt() {return (int)(*(int64_t *)ptr);}
		int64_t getLong() {return (int64_t)(*(int64_t *)ptr);}
		float getFloat() {return (float)(*(int64_t *)ptr);}
		double getDouble() {return (double)(*(int64_t *)ptr);}
		char *getDate();
	};
	class  EXPORT Float32 : public Scalar
	{
	public:
		Float32(float val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_FLOAT;
			length = sizeof(float);
			ptr = new char[sizeof(float)];
			*(float *)ptr = val;
			setAccessory(units, error, help, validation);
		}
		char getByte() {return (char)(*(float *)ptr);}
		short getShort() {return (short)(*(float *)ptr);}
		int getInt() {return (int)(*(float *)ptr);}
		int64_t getLong() {return (int64_t)(*(float *)ptr);}
		float getFloat() {return (float)(*(float *)ptr);}
		double getDouble() {return (double)(*(float *)ptr);}
	};
	class  EXPORT Float64 : public Scalar
	{
	public:
		Float64(double val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_DOUBLE;
			length = sizeof(double);
			ptr = new char[sizeof(double)];
			*(double *)ptr = val;
			setAccessory(units, error, help, validation);
		}
		char getByte() {return (char)(*(double *)ptr);}
		short getShort() {return (short)(*(double *)ptr);}
		int getInt() {return (int)(*(double *)ptr);}
		int64_t getLong() {return (int64_t)(*(double *)ptr);}
		float getFloat() {return (float)(*(double *)ptr);}
		double getDouble() {return (double)(*(double *)ptr);}
	};
	class  EXPORT Complex32 : public Scalar
	{
	public:
		Complex32(float re, float im, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_FSC;
			length = 2 * sizeof(float);
			ptr = new char[2*sizeof(float)];
			((float *)ptr)[0] = re;
			((float *)ptr)[1] = im;
			setAccessory(units, error, help, validation);
		}
		float getReal() {return ((float *)ptr)[0];}
		float getImaginary() {return ((float *)ptr)[1];}
		std::complex<double> getComplex() {return std::complex<double>(((float *)ptr)[0], ((float *)ptr)[1]);}
	};
	class  EXPORT Complex64 : public Scalar
	{
	public:
		Complex64(double re, double im, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_FTC;
			length = 2 * sizeof(double);
			ptr = new char[2*sizeof(double)];
			((double *)ptr)[0] = re;
			((double *)ptr)[1] = im;
			setAccessory(units, error, help, validation);
		}
		double getReal() {return ((double *)ptr)[0];}
		double getImaginary() {return ((double *)ptr)[1];}
		std::complex<double> getComplex() {return std::complex<double>(((double *)ptr)[0], ((double *)ptr)[1]);}
	};
	class  EXPORT String : public Scalar
	{
	public:
		String(const char *val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0) {
			init(val, std::string(val).size(), units, error, help, validation);
		}

		String(unsigned char *uval, int len, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0) {
			// FIXME: Hack to handle broken LabView types that use unsigned char (as uint8) instead of char
			// FIXME: Warning: Do not use this constructor in user code
			char * val = reinterpret_cast<char *>(uval);
			init(val, std::string(val).size(), units, error, help, validation);
		}
//GAB  definition in order to avoid breaking LabVIEW
		String(unsigned char *uval, int numDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0) {
			char * val = reinterpret_cast<char *>(uval);
			init(val, std::string(val).size(), units, error, help, validation);
		}

		String(const char *val, int len, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0) {
			init(val, len, units, error, help, validation);
		}

		char *getString()
		{
			char *res = new char[length + 1];
			std::copy(&ptr[0], &ptr[length + 1], res);
			res[length] = 0;
			return res;
		}
		char **getStringArray(int *numElements)
		{
			char **res = new char*[1];
			res[0] = new char[length + 1];
			std::copy(&ptr[0], &ptr[length + 1], res[0]);
			res[0][length] = 0;
			*numElements = 1;
			return res;
		}

		bool equals(Data *data);

	private:
		void init(char const * val, int len, Data * units, Data * error, Data * help, Data * validation) {
			dtype = DTYPE_T;
			length = len;
			ptr = new char[length+1];
			std::copy(&val[0], &val[length], ptr);
			ptr[length] = 0;
			setAccessory(units, error, help, validation);
		}
	};
////////////////ARRAYS/////////////////////
	class EXPORT Array: public Data
	{
	protected:
		int length;
		int arsize;
		int nDims;
		int dims[MAX_DIMS];
		char *ptr;

		void setSpecific(void const * data, int length, int dtype, int nData) {
			setSpecific(data, length, dtype, 1, &nData);
		}

		void setSpecific(void const * data, int length, int dtype, int nDims, int *dims)
		{
			this->dtype = dtype;
			this->nDims = nDims;
			this->length = length;
			arsize = length;
			for(int i = 0; i < nDims; i++)
			{
				arsize *= dims[i];
				this->dims[i] = dims[i];
			}

			char const * dataBytes = reinterpret_cast<char const *>(data);
			ptr = new char[arsize];
			std::copy(&dataBytes[0], &dataBytes[arsize], ptr);
		}
	public:
		Array(): length(0), arsize(0), nDims(0), ptr(0) {
			clazz = CLASS_A;
		}

		~Array() {
//			deleteNativeArray(ptr);
			delete[] ptr;
		}

		virtual int getSize() 
		{
			int retSize = 1;
			for(int i = 0; i < nDims; i++)
				retSize *= dims[i];
			return retSize;
		}
		virtual void getInfo(char *clazz, char *dtype, short *length, char *nDims, int **dims, void **ptr)
		{
			*clazz = this->clazz;
			*dtype = this->dtype;
			*length = this->length;
			*nDims = this->nDims;
			*dims = new int[this->nDims];
			for(int i = 0; i < this->nDims; i++)
			    (*dims)[i] = this->dims[i];
			*ptr = this->ptr;
		}
		virtual int *getShape(int *numDims);
		
		Data *getElementAt(int *getDims, int getNumDims); 
		void setElementAt(int *getDims, int getNumDims, Data *data); 
		Data *getElementAt(int dim)
		{
			return getElementAt(&dim, 1);
		}
		Array *getSubArray(int startDim, int nSamples);
		void setElementAt(int dim, Data *data)
		{
			setElementAt(&dim, 1, data);
		}
		void *convertToDsc();
		void *getArray() {return ptr;}
		char *getByteArray(int *numElements);
		short *getShortArray(int *numElements);
		int *getIntArray(int *numElements);
		int64_t *getLongArray(int *numElements);
		unsigned char *getByteUnsignedArray(int *numElements);
		unsigned short *getShortUnsignedArray(int *numElements);
		unsigned int *getIntUnsignedArray(int *numElements);
		uint64_t *getLongUnsignedArray(int *numElements);
		float *getFloatArray(int *numElements);
		double *getDoubleArray(int *numElements);
		std::complex<double> *getComplexArray(int *numElements);
		virtual char **getStringArray(int *numElements);
	};
	class Int8Array: public Array
	{
	public:
		Int8Array(char const * data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific(data, 1, DTYPE_B, nData);
			setAccessory(units, error, help, validation);
		}
		Int8Array(char const * data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific(data, 1, DTYPE_B, nDims, dims);
			setAccessory(units, error, help, validation);
		}
	};
	class EXPORT Uint8Array: public Array
	{
	friend Data *MDSplus::deserialize(Data *serializedData);
	public:
		Uint8Array(unsigned char const * data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific(data, 1, DTYPE_BU, nData);
			setAccessory(units, error, help, validation);
		}
		Uint8Array(unsigned char const * data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific(data, 1, DTYPE_BU, nDims, dims);
			setAccessory(units, error, help, validation);
		}
		Data* deserialize();
	};
	class Int16Array: public Array
	{
	public:
		Int16Array(short const * data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific(data, 2, DTYPE_W, nData);
			setAccessory(units, error, help, validation);
		}
		Int16Array(short const * data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific(data, 2, DTYPE_W, nDims, dims);
			setAccessory(units, error, help, validation);
		}
	};
	class Uint16Array: public Array
	{
	public:
		Uint16Array(unsigned short const * data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific(data, 2, DTYPE_WU, nData);
			setAccessory(units, error, help, validation);
		}
		Uint16Array(unsigned short const * data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific(data, 2, DTYPE_WU, nDims, dims);
			setAccessory(units, error, help, validation);
		}
	};
	class Int32Array: public Array
	{
	public:
		Int32Array(int const * data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific(data, sizeof(int), DTYPE_L, nData);
			setAccessory(units, error, help, validation);
		}
		Int32Array(int const * data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific(data, sizeof(int), DTYPE_L, nDims, dims);
			setAccessory(units, error, help, validation);
		}
	};
	class Uint32Array: public Array
	{
	public:
		Uint32Array(unsigned int const * data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific(data, sizeof(int), DTYPE_LU, nData);
			setAccessory(units, error, help, validation);
		}
		Uint32Array(unsigned int const * data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific(data, sizeof(int), DTYPE_LU, nDims, dims);
			setAccessory(units, error, help, validation);
		}
	};
	class Int64Array: public Array
	{
	public:
		Int64Array(int64_t const * data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific(data, 8, DTYPE_Q, nData);
			setAccessory(units, error, help, validation);
		}
		Int64Array(int64_t const * data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific(data, 8, DTYPE_Q, nDims, dims);
			setAccessory(units, error, help, validation);
		}
	};
	class Uint64Array: public Array
	{
	public:
		Uint64Array(uint64_t const * data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific(data, 8, DTYPE_QU, nData);
			setAccessory(units, error, help, validation);
		}
		Uint64Array(uint64_t const * data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific(data, 8, DTYPE_QU, nDims, dims);
			setAccessory(units, error, help, validation);
		}
	};
	class Float32Array: public Array
	{
	public:
		Float32Array(float const * data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific(data, sizeof(float), DTYPE_FLOAT, nData);
			setAccessory(units, error, help, validation);
		}
		Float32Array(float const * data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific(data, sizeof(float), DTYPE_FLOAT, nDims, dims);
			setAccessory(units, error, help, validation);
		}
	};
	class Float64Array: public Array
	{
	public:
		Float64Array(double const * data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific(data, sizeof(double), DTYPE_DOUBLE, nData);
			setAccessory(units, error, help, validation);
		}
		Float64Array(double const * data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific(data, sizeof(double), DTYPE_DOUBLE, nDims, dims);
			setAccessory(units, error, help, validation);
		}
	};
	class Complex32Array: public Array
	{
	public:
		Complex32Array(float const * reIm, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific(reIm, 2*sizeof(float), DTYPE_FSC, nData);
			setAccessory(units, error, help, validation);
		}
		Complex32Array(float const * reIm, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific(reIm, 2*sizeof(float), DTYPE_FSC, nDims, dims);
			setAccessory(units, error, help, validation);
		}
	};
	class Complex64Array: public Array
	{
	public:
		Complex64Array(double const * reIm, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific(reIm, 2*sizeof(double), DTYPE_FTC, nData);
			setAccessory(units, error, help, validation);
		}
		Complex64Array(double const * reIm, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific(reIm, 2*sizeof(double), DTYPE_FTC, nDims, dims);
			setAccessory(units, error, help, validation);
		}
	};
	class StringArray: public Array
	{
	public:
		StringArray(char **data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			//Pad all string to longest 
			int maxLen = 0;
			for(int i = 0; i < nData; i++)
				maxLen = std::max(std::string(data[i]).size(), (std::size_t)maxLen);

			char *padData = new char[nData * maxLen];
			for(int i = 0; i < nData; i++) {
				std::size_t currLen = std::string(data[i]).size();
				std::copy(&data[i][0], &data[i][currLen], &padData[i * maxLen]);
				std::fill(&padData[i*maxLen + currLen], &padData[i*maxLen + maxLen], ' ');
			}
			setSpecific(padData, maxLen, DTYPE_T, nData);
			setAccessory(units, error, help, validation);
			delete[] padData;
		}
		StringArray(char const * data, int nStrings, int stringLen, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0) //For contiuguous
		{
			setSpecific(data, stringLen, DTYPE_T, nStrings);
			setAccessory(units, error, help, validation);
		}
	};
/////////////////////////COMPOUND DATA//////////////////////////////////
class EXPORT Compound: public Data {
public:
	Compound() {
		clazz = CLASS_R;
		opcode = 0;
	}

	Compound(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
	{
		switch(length) {
		  case 1:
			opcode = *(char *)ptr;
			break;
		  case 2:
			opcode = *(short *)ptr;
			break;
		  case 4:
			opcode = *(int *)ptr;
			break;
		  default:
			opcode = 0;
		    
		
		}
		if(nDescs > 0) {
			for(int i = 0; i < nDescs; ++i) {
				this->descs.push_back((Data *)descs[i]);
				if (this->descs[i])
					this->descs[i]->incRefCount();
			}
		}

		this->dtype = dtype;
		clazz = CLASS_R;
		setAccessory(units, error, help, validation);
	}

	virtual void propagateDeletion() {
		for(std::size_t i = 0; i < descs.size(); ++i)
		    if (descs[i])
		    {
			descs[i]->decRefCount();
			deleteData(descs[i]);
		    }
	}

	void * convertToDsc();

	virtual ~Compound() 
	{
	    propagateDeletion();
	}

protected:
	short opcode;
	std::vector<Data *> descs;

	void incrementRefCounts() {
		for(std::size_t i = 0; i < descs.size(); ++i)
			if(descs[i])
				descs[i]->refCount++;
	}

	void assignDescAt(Data *data, int idx) {
		if (descs.at(idx))
			deleteData(descs[idx]);

		descs.at(idx) = data;
		if (data)
			data->refCount++;
	}

	Data * getDescAt(std::size_t idx) {
		if (descs.at(idx))
			descs[idx]->incRefCount();
		return descs[idx];
	}

};

class Signal: public Compound {
	public: 
		Signal(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):
			Compound(dtype, length, ptr, nDescs, descs)
		{
			setAccessory(units, error, help, validation);
		}

		Signal(Data *data, Data *raw, Data *dimension, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			descs.push_back(data);
			descs.push_back(raw);
			descs.push_back(dimension);
			init(units, error, help, validation);
		}

		Signal(Data *data, Data *raw, Data *dimension1, Data *dimension2, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			descs.push_back(data);
			descs.push_back(raw);
			descs.push_back(dimension1);
			descs.push_back(dimension2);
			init(units, error, help, validation);
		}

		Signal(Data *data, Data *raw, int nDims, Data **dimensions, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			descs.push_back(data);
			descs.push_back(raw);
			for(int i = 0; i < nDims; i++)
				descs.push_back(dimensions[i]);
			init(units, error, help, validation);
		}

		Data *getData() {
			return getDescAt(0);
		}

		Data *getRaw() {
			return getDescAt(1);
		}

		Data *getDimension() {
			return getDescAt(2);
		}

		Data *getDimensionAt(int idx) {
			return getDescAt(idx + 2);
		}

		int getNumDimensions() {
			return descs.size() - 2;
		}

		void setData(Data *data) {assignDescAt(data, 0);}
		void setRaw(Data *raw){assignDescAt(raw, 1);}
		void setDimension(Data *dimension) {assignDescAt(dimension, 2);}
		void setDimensionAt(Data *dimension, int idx) {assignDescAt(dimension, 2 + idx);}

	private:
		void init(Data * units, Data * error, Data * help, Data * validation) {
			dtype = DTYPE_SIGNAL;
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}
	};
	class Dimension: public Compound
	{
	public: 
		Dimension(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):Compound(dtype, length, ptr, nDescs, descs)
		{
			setAccessory(units, error, help, validation);
		}
		Dimension(Data *window, Data *axis, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_DIMENSION;
			descs.push_back(window);
			descs.push_back(axis);
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}
		Data *getWindow(){
			return getDescAt(0);
		}

		Data *getAxis(){
			return getDescAt(1);
		}

		void setWindow(Data *window) {assignDescAt(window, 0);}
		void setAxis(Data *axis){assignDescAt(axis, 1);}
	};
	class Window: public Compound
	{
	public: 
		Window(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):Compound(dtype, length, ptr, nDescs, descs)
		{
			setAccessory(units, error, help, validation);
		}
		Window(Data *startidx, Data *endidx, Data *value_at_idx0, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_WINDOW;
			descs.push_back(startidx);
			descs.push_back(endidx);
			descs.push_back(value_at_idx0);
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}

		Data *getStartIdx() {
			return getDescAt(0);
		}

		Data *getEndIdx() {
			return getDescAt(1);
		}

		Data *getValueAt0() {
			return getDescAt(2);
		}

		void setStartIdx(Data *startidx) {assignDescAt(startidx, 0);}
		void setEndIdx(Data *endidx){assignDescAt(endidx, 1);}
		void setValueAt0(Data *value_at_idx0) {assignDescAt(value_at_idx0, 2);}
	};
	class Function: public Compound
	{
	public: 
		Function(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):Compound(dtype, length, ptr, nDescs, descs)
		{
			setAccessory(units, error, help, validation);
		}
		Function(short opcode, int nargs, Data **args, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_FUNCTION;
			this->opcode = opcode;
			for(int i = 0; i < nargs; i++)
				descs.push_back(args[i]);
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}

		short getOpcode() {
			return opcode;
		}

		int getNumArguments() { return descs.size(); }
		Data *getArgumentAt(int idx) {
			return getDescAt(idx);
		}

		void setArgAt(Data *arg, int idx) {assignDescAt(arg, idx);}
	};

	class Conglom: public Compound
	{
	public: 
		Conglom(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):Compound(dtype, length, ptr, nDescs, descs)
		{
			setAccessory(units, error, help, validation);
		}
		Conglom(Data *image, Data *model, Data *name, Data *qualifiers, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_CONGLOM;
			descs.push_back(image);
			descs.push_back(model);
			descs.push_back(name);
			descs.push_back(qualifiers);
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}

		Data *getImage() {
			return getDescAt(0);
		}

		Data *getModel() {
			return getDescAt(1);
		}

		Data *getName() {
			return getDescAt(2);
		}

		Data *getQualifiers() {
			return getDescAt(3);
		}

		void setImage(Data *image) {assignDescAt(image, 0);}
		void setModel(Data *model){assignDescAt(model, 1);}
		void setName(Data *name) {assignDescAt(name, 2);}
		void setQualifiers(Data *qualifiers) {assignDescAt(qualifiers, 3);}
	};
	class Range: public Compound
	{
	public: 
		Range(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):Compound(dtype, length, ptr, nDescs, descs)
		{
			setAccessory(units, error, help, validation);
		}
		Range(Data *begin, Data *ending, Data *deltaval, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_RANGE;
			descs.push_back(begin);
			descs.push_back(ending);
			descs.push_back(deltaval);
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}

		Data *getBegin() {
			return getDescAt(0);
		}

		Data *getEnding() {
			return getDescAt(1);
		}

		Data *getDeltaVal() {
			return getDescAt(2);
		}

		void setBegin(Data *begin) {assignDescAt(begin, 0);}
		void setEnding(Data *ending){assignDescAt(ending, 1);}
		void setDeltaVal(Data *deltaval) {assignDescAt(deltaval, 2);}
	};
	class Action: public Compound
	{
	public: 
		Action(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):Compound(dtype, length, ptr, nDescs, descs)
		{
			setAccessory(units, error, help, validation);
		}
		Action(Data *dispatch, Data *task, Data *errorlogs, Data *completion_message, Data *performance, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_ACTION;
			descs.push_back(dispatch);
			descs.push_back(task);
			descs.push_back(errorlogs);
			descs.push_back(completion_message);
			descs.push_back(performance);
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}

		Data *getDispatch() {
			return getDescAt(0);
		}

		Data *getTask() {
			return getDescAt(1);
		}

		Data *getErrorLogs() {
			return getDescAt(2);
		}

		Data *getCompletionMessage() {
			return getDescAt(3);
		}

		Data *getPerformance() {
			return getDescAt(4);
		}

		void setDispatch(Data *dispatch) {assignDescAt(dispatch, 0);}
		void setTask(Data *task){assignDescAt(task, 1);}
		void setErrorLogs(Data *errorlogs){assignDescAt(errorlogs, 2);}
		void setCompletionMessage(Data *completion_message) {assignDescAt(completion_message, 3);}
		void setPerformance(Data *performance){assignDescAt(performance, 4);}
	};

	class Dispatch: public Compound
	{
	public: 
		Dispatch(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):Compound(dtype, length, ptr, nDescs, descs)
		{
			setAccessory(units, error, help, validation);
		}
		Dispatch(Data *ident, Data *phase, Data *when, Data *completion, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_DISPATCH;
			descs.push_back(ident);
			descs.push_back(phase);
			descs.push_back(when);
			descs.push_back(completion);
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}

		Data *getIdent(){
			return getDescAt(0);
		}

		Data *getPhase(){
			return getDescAt(1);
		}

		Data *getWhen() {
			return getDescAt(2);
		}

		Data *getCompletion() {
			return getDescAt(3);
		}

		void setIdent(Data *ident) {assignDescAt(ident, 0);}
		void setPhase(Data *phase){assignDescAt(phase, 1);}
		void setWhen(Data *when) {assignDescAt(when, 2);}
		void setCompletion(Data *completion) {assignDescAt(completion, 3);}
	};

	class Program: public Compound
	{
	protected:
		bool isImmutable() { return false;}
	public: 
		Program(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):Compound(dtype, length, ptr, nDescs, descs)
		{
			setAccessory(units, error, help, validation);		
		}
		Program(Data *timeout, Data *program, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_PROGRAM;
			descs.push_back(timeout);
			descs.push_back(program);
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}

		Data *getTimeout() {
			return getDescAt(0);
		}

		Data *getProgram() {
			return getDescAt(1);
		}

		void setTimeout(Data *timeout) {assignDescAt(timeout, 0);}
		void setProgram(Data *program){assignDescAt(program, 1);}
	};

	class Routine: public Compound
	{
	protected:
		bool isImmutable() { return false;}
	public: 
		Routine(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):Compound(dtype, length, ptr, nDescs, descs)
		{
			setAccessory(units, error, help, validation);
		}
		Routine(Data *timeout, Data *image, Data *routine, int nargs, Data **args, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_ROUTINE;
			descs.push_back(timeout);
			descs.push_back(image);
			descs.push_back(routine);
			for(int i = 0; i < nargs; i++)
				descs.push_back(args[i]);
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}

		Data *getTimeout() {
			return getDescAt(0);
		}

		Data *getImage(){
			return getDescAt(1);
		}

		Data *getRoutine() {
			return getDescAt(2);
		}

		Data *getArgumentAt(int idx) {
			return getDescAt(idx + 3);
		}

		void setTimeout(Data *timeout) {assignDescAt(timeout, 0);}
		void setImage(Data *image){assignDescAt(image, 1);}
		void setRoutine(Data *routine) {assignDescAt(routine, 2);}
		void setArgumentAt(Data *argument, int idx) {assignDescAt(argument, 3 + idx);}
	};

	class Procedure: public Compound
	{
	protected:
		bool isImmutable() { return false;}
	public: 
		Procedure(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):Compound(dtype, length, ptr, nDescs, descs)
		{
			setAccessory(units, error, help, validation);
		}
		Procedure(Data *timeout, Data *language, Data *procedure, int nargs, Data **args, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_PROCEDURE;
			descs.push_back(timeout);
			descs.push_back(language);
			descs.push_back(procedure);
			for(int i = 0; i < nargs; i++)
				descs.push_back(args[i]);
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}

		Data *getTimeout(){
			return getDescAt(0);
		}

		Data *getLanguage(){
			return getDescAt(1);
		}

		Data *getProcedure() {
			return getDescAt(2);
		}

		Data *getArgumentAt(int idx) {
			return getDescAt(idx + 3);
		}

		void setTimeout(Data *timeout) {assignDescAt(timeout, 0);}
		void setLanguage(Data *language){assignDescAt(language, 1);}
		void seProcedure(Data *procedure) {assignDescAt(procedure, 2);}
		void setArgumentAt(Data *argument, int idx) {assignDescAt(argument, 3 + idx);}
	};

	class Method: public Compound
	{
	public: 
		Method(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):Compound(dtype, length, ptr, nDescs, descs)
		{
			setAccessory(units, error, help, validation);
		}
		Method(Data *timeout, Data *method, Data *object, int nargs, Data **args, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_METHOD;
			descs.push_back(timeout);
			descs.push_back(method);
			descs.push_back(object);
			for(int i = 0; i < nargs; i++)
				descs.push_back(args[i]);
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}

		Data *getTimeout(){
			return getDescAt(0);
		}

		Data *getMethod() {
			return getDescAt(1);
		}

		Data *getObject() {
			return getDescAt(2);
		}

		Data *getArgumentAt(int idx) {
			return getDescAt(idx + 3);
		}

		void setTimeout(Data *timeout) {assignDescAt(timeout, 0);}
		void setMethod(Data *method){assignDescAt(method, 1);}
		void setObject(Data *object) {assignDescAt(object, 2);}
		void setArgumentAt(Data *argument, int idx) {assignDescAt(argument, 3 + idx);}
	};

	class Dependency: public Compound
	{
	public: 
		Dependency(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):Compound(dtype, length, ptr, nDescs, descs)
		{
			setAccessory(units, error, help, validation);
		}
		Dependency(short opcode, Data *arg1, Data *arg2, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_DEPENDENCY;
			this->opcode = opcode;
			descs.push_back(arg1);
			descs.push_back(arg2);
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}

		short getOpcode() {
			return opcode;
		}

		Data *getArg1(){
			return getDescAt(0);
		}

		Data *getArg2(){
			return getDescAt(1);
		}

		void setOpcode(short opcode) {
			this->opcode = opcode;
		}

		void setArg1(Data *arg1) {assignDescAt(arg1, 0);}
		void setArg2(Data *arg2) {assignDescAt(arg2, 0);}
	};

	class Condition: public Compound
	{
	public: 
		Condition(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):Compound(dtype, length, ptr, nDescs, descs)
		{
			setAccessory(units, error, help, validation);
		}
		Condition(short opcode, Data *arg, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_CONDITION;
			this->opcode = opcode;
			descs.push_back(arg);
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}

		short getOpcode() {
			return opcode;
		}

		Data *getArg(){
			return getDescAt(0);
		}

		void setOpcode(short opcode) {
			this->opcode = opcode;
		}

		void setArg(Data *arg) {assignDescAt(arg, 0);}
	};

class Call: public Compound {
	public: 
		Call(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):Compound(dtype, length, ptr, nDescs, descs)
		{
			setAccessory(units, error, help, validation);
		}
		Call(Data *image, Data *routine, int nargs, Data **args, char retType = DTYPE_L, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_CALL;
			opcode = retType;
			descs.push_back(image);
			descs.push_back(routine);
			for(int i = 0; i < nargs; i++)
				descs.push_back(args[i]);
			incrementRefCounts();
		}

		char getRetType() {
			return opcode;
		}

		void setRetType(char retType) {
			opcode = retType;
		}

		Data *getImage() {
			return getDescAt(0);
		}

		Data *getRoutine(){
			return getDescAt(1);
		}

		Data *getArgumentAt(int idx) {
			return getDescAt(idx + 2);
		}

		void setImage(Data *image){assignDescAt(image, 0);}
		void setRoutine(Data *routine) {assignDescAt(routine, 1);}
		void setArgumentAt(Data *argument, int idx) {assignDescAt(argument, 2 + idx);}
	};

/////////////////////APD///////////////////////

//NOTE: refCount is not altered by getDescAt(), appendDesc() and setDescAt(). 
// this simplifies the usage of Apd (not intended in any case for the normal user)
///////////////////////////////////////////////
class EXPORT Apd: public Data
{
public:
	Apd(Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0) {
		clazz = CLASS_APD;
		dtype = DTYPE_DSC;
		setAccessory(units, error, help, validation);
	}

	Apd(std::size_t nData, Data **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0) {
		clazz = CLASS_APD;
		dtype = DTYPE_DSC;
		for(std::size_t i = 0; i < nData; ++i) {
			if(descs[i]) descs[i]->incRefCount();
			this->descs.push_back(descs[i]);
		}
		setAccessory(units, error, help, validation);
	}
	
	
	virtual void propagateDeletion() 
	{
	    for(std::size_t i = 0; i < descs.size(); ++i)
	    {
		if (descs[i])
		{
		    descs[i]->decRefCount();
		    deleteData(descs[i]);
		}
	    }
	}


	virtual ~Apd() {
	    propagateDeletion();
	}
/*	    for(size_t i = 0; i < descs.size(); i++)
	    {
		if(descs[i])
		    descs[i]->decRefCount();
	    }
	}
	*/
/*  CANNOT WORK: Some pointers may be empty!!
	virtual ~Apd() {
		std::for_each(descs.begin(), descs.end(), (void (&)(Data *))decRefCount);
	}
*/

	virtual std::size_t len() {
		return descs.size();
	}

	Data ** getDscs() {
		// FIXME: This doesn't increment refCounts.
		return &descs[0];
	}

	std::size_t getDimension() {
		return Apd::len();
	}

	Data ** getDscArray() {
		return getDscs();
	}

	Data * getDescAt(std::size_t i) {
		//descs.at(i)->incRefCount();
		return descs.at(i);
	}

	bool hasDescAt(std::size_t i) {
		return descs.size() > i && descs[i] != NULL;
	}
	void setDescAt(std::size_t i, Data * data) {
		if(descs.size() <= i)
		{
		    for(size_t j = descs.size(); j < i; j++)
			descs.push_back(NULL);
		    descs.push_back(data);
		}
		else
//INSERT AND BEGIN() WORK ONLY IF VECTOR NON EMPTY!!		      
		    descs.insert(descs.begin() + i, data);
		//data->incRefCount();
	} 
	void appendDesc(Data * data)
	{
		descs.push_back(data);
		//if(data) data->incRefCount();
	}
	void *convertToDsc();

protected:
	std::vector<Data*> descs;

};

///////////////////LIST///////////////
class EXPORT List: public Apd
{
public:
	List(Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):
		Apd(units, error, help, validation)
	{
		dtype = DTYPE_LIST;
	}

	List(int nData, Data **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):
		Apd(nData, descs, units, error, help, validation)
	{
		dtype = DTYPE_LIST;
	}

	void append(Data * data) {
		appendDesc(data);
	}
		
	void remove(Data * data) {
		descs.erase(std::remove(descs.begin(), descs.end(), data), descs.end());
		data->decRefCount();
	}

	void remove(std::size_t i) {
		descs.at(i)->decRefCount();
		descs.erase(descs.begin() + i);
	}

	void insert(std::size_t i, Data *data) {
		descs.insert(descs.begin() + i, data);
		data->incRefCount();
	}

	Data * getElementAt(std::size_t i) {
		descs.at(i)->incRefCount();
		return descs[i];
	}
};

/////Dictionary///////
class EXPORT Dictionary: public Apd
{
public:
	Dictionary(Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):
		Apd(units, error, help, validation)
	{
		dtype = DTYPE_DICTIONARY;
	}

	Dictionary(int nData, Data **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):
		Apd(nData, descs, units, error, help, validation)
	{
		dtype = DTYPE_DICTIONARY;
	}

	Data * getItem(String *strData) {
		for(std::size_t i = 0; i < descs.size(); i += 2) {
			if(strData->equals(descs[i])) {
				descs[i+1]->incRefCount();
				return descs[i+1];
			}
		}
		return 0;
	}

	void setItem(String *strData, Data *data) {
		for(std::size_t i = 0; i < descs.size(); i += 2) {
			if(strData->equals(descs[i])) {
				descs[i]->decRefCount();
				descs[i] = strData;
				strData->incRefCount();

				descs[i+1]->decRefCount();
				descs[i+1] = data;
				data->incRefCount();
				return;
			}
		}

		descs.push_back(strData);
		descs.push_back(data);
		strData->incRefCount();
		data->incRefCount();
	}

	std::size_t len() {
		return Apd::len()/2;
	}
};
/////////////////////////////////////////////////////////////////
//              Tree Objects
/////////////////////////////////////////////////////////////////
	class Tree;
	class TreeNode;
	class TreeNodeArray;
	class  EXPORT TreeNode: public Data
	{
		friend	EXPORT std::ostream &operator<<(std::ostream &stream, TreeNode *treeNode)
		{
			stream << treeNode->getFullPathStr();
			return stream;
		}
	protected:
		Tree *tree;
		int nid;
		virtual bool isCached() {return false;}
		virtual int getCachePolicy() { return 0;}
//From Data
		virtual bool isImmutable() {return false;}
		virtual void *convertToDsc();
		bool getFlag(int flagOfs);
		void setFlag(int flagOfs, bool val);
	public:
		virtual Data *data();
		TreeNode(int nid, Tree *tree, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0);
		//Force new and delete in dll for windows
		void *operator new(size_t sz);
		void operator delete(void *p);
		Tree *getTree(){return tree;}
		void setTree(Tree *tree) {this->tree = tree;}
		char *getPath();
		char *getMinPath();
		char *getFullPath();
		char *getNodeName();
		char *getOriginalPartName();
		std::string getPathStr();
		std::string getMinPathStr();
		std::string getFullPathStr();
		std::string getNodeNameStr();
		std::string getOriginalPartNameStr();
		TreeNode *getNode(char const * relPath);
		TreeNode *getNode(String *relPathStr);
		virtual Data *getData();
		virtual void putData(Data *data);
		virtual void deleteData();
		virtual void resolveNid(){}
		int getNid() { return nid;}
		bool isOn();
		void setOn(bool on);
		virtual int getLength();
		virtual int getCompressedLength();
		int getOwnerId();
		int getStatus();
		virtual int64_t getTimeInserted();
		void doMethod(char *method);
		bool isSetup();
		bool isWriteOnce();
		void setWriteOnce(bool flag);
		bool isCompressOnPut();
		void setCompressOnPut(bool flag);
		bool isNoWriteModel();
		void setNoWriteModel(bool flag);
		bool isNoWriteShot();
		void setNoWriteShot(bool flag);
		TreeNode *getParent();
		TreeNode *getBrother();
		TreeNode *getChild();
		TreeNode *getMember();
		int getNumMembers();
		int getNumChildren();
		int getNumDescendants();
		TreeNode **getChildren(int *numChildren);
		TreeNode **getMembers(int *numChildren);
		TreeNode **getDescendants(int *numChildren);
		
		
		const char *getClass();
		const char *getDType();
		const char *getUsage();
		bool isEssential();
		void setEssential(bool flag);
		bool isIncludedInPulse();
		void setIncludedInPulse(bool flag);
		bool isMember();
		bool isChild();
		int getConglomerateElt();
		int getNumElts();
		TreeNodeArray *getConglomerateNodes();
		int getDepth();
		bool containsVersions();
		void beginSegment(Data *start, Data *end, Data *time, Array *initialData);
		void makeSegment(Data *start, Data *end, Data *time, Array *initialData);
		void putSegment(Array *data, int ofs);
		void updateSegment(Data *start, Data *end, Data *time);
		void updateSegment(int idx, Data *start, Data *end, Data *time);
		int getNumSegments();
		void getSegmentLimits(int segmentIdx, Data **start, Data **end);
		Array *getSegment(int segIdx);
		Data *getSegmentDim(int segIdx);
		void getSegmentAndDimension(int segIdx, Array *segment, Data *dimension);
		void beginTimestampedSegment(Array *initData);
		void makeTimestampedSegment(Array *data, int64_t *times);
		void putTimestampedSegment(Array *data, int64_t *times);
		void putRow(Data *data, int64_t *time, int size = 1024);
		void getSegmentInfo(int segIdx, char *dtype, char *dimct, int *dims, int *nextRow);
		void acceptSegment(Array *data, Data *start, Data *end, Data *times);
		void acceptRow(Data *data, int64_t time, bool isLast);
		StringArray *findTags();
//////////Edit methods////////////////
		TreeNode *addNode(char const * name, char const * usage);
		void remove(char const * name);
		void rename(std::string const & newName);
		void move(TreeNode *parent, std::string const & newName);
		void move(TreeNode *parent);
		TreeNode *addDevice(char const * name, char const * type);
		void addTag(std::string const & tagName);
		void removeTag(std::string const & tagName);
		void setSubtree(bool isSubtree);
	};

/////////////////End Class TreeTreeNode///////////////

class EXPORT TreePath: public TreeNode
{
public:
	TreePath(std::string const & path, Tree *tree, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0);
	TreePath(char const * path, int len, Tree *tree, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0);
	void *convertToDsc();
	void resolveNid();

private:
	std::string path;
};

#ifdef CACHEDTREES	
/////////////////CachedTreeNode/////////////////////////////
#define MDS_WRITE_THROUGH 1
#define MDS_WRITE_BACK 2
#define MDS_WRITE_BUFFER 3
#define MDS_WRITE_LAST 4
	class EXPORT CachedTreeNode: public TreeNode
	{
	protected:
		int cachePolicy;
		virtual bool isCached() { return true;}
		virtual int getCachePolicy() { return cachePolicy;}
	public:
		CachedTreeNode(int nid, Tree *tree):TreeNode(nid, tree){cachePolicy = MDS_WRITE_BUFFER;}
		void setCachePolicy(int cachePolicy) {this->cachePolicy = cachePolicy;}
		void flush();
		void putLastRow(Data *data, int64_t *time);
		void terminateSegment();
	};
#endif
////////////////Class TreeNodeArray///////////////////////
	class EXPORT TreeNodeArray
	{
		TreeNode **nodes;
		int numNodes;
	public:
		void *operator new(size_t sz);
		void operator delete(void *p);
		TreeNodeArray(TreeNode **nodes, int numNodes);
		TreeNodeArray(int *nids, int numNodes, Tree *tree);
		~TreeNodeArray();
		StringArray *getPath();
		StringArray *getFullPath();
		Int32Array *getNid();
		Int8Array *isOn();
		void setOn(Int8Array *info);
		Int8Array *isSetup();
		Int8Array *isWriteOnce();
		void setWriteOnce(Int8Array *info);
		Int8Array *isCompressOnPut();
		void setCompressOnPut(Int8Array *info);
		Int8Array *isNoWriteModel();
		void setNoWriteModel(Int8Array *info);
		Int8Array *isNoWriteShot();
		void setNoWriteShot(Int8Array *info);
		Int32Array *getLength();
		Int32Array *getCompressedLength();
		StringArray *getUsage();
		int getNumNodes() { return numNodes;}
		TreeNode *operator[](int idx)
		{
			if(idx >= 0 && idx < numNodes)
				return nodes[idx];
			else
				return 0;
		}
	};
	
	
////////////////Class Tree/////////////////////////////////
	class EXPORT Tree
	{
		friend void setActiveTree(Tree *tree);
		friend Tree *getActiveTree();
		Tree(void *dbid, char const * name, int shot);
	protected:
		Tree(){}
		std::string name;
		int shot;
		void *ctx;
	
	public:
		Tree(char const * name, int shot);
		Tree(char const * name, int shot, char const * mode);
		~Tree();
		void *operator new(size_t sz);
		void operator delete(void *p);
		static void lock();
		static void unlock();
		static void setCurrent(char const * treeName, int shot);
		static int getCurrent(char const * treeName);
		static Tree *create(char const * name, int shot);
		
		void *getCtx() {return ctx;}
		void edit();
		void write();
		void quit();
		TreeNode *getNode(char const *path);
		TreeNode *getNode(TreePath *path);
		TreeNode *getNode(String *path);
		TreeNode *addNode(char const * name, char *usage);
		TreeNode *addDevice(char const * name, char *type);
		void remove(char const *name);
		TreeNodeArray *getNodeWild(char const *path, int usageMask);
		TreeNodeArray *getNodeWild(char const *path);
		void setDefault(TreeNode *treeNode);
		TreeNode *getDefault();
		bool versionsInModelEnabled();
		bool versionsInPulseEnabled();
		bool isModified();
		bool isOpenForEdit();
		bool isReadOnly();
		void setVersionsInModel(bool enable);
		void setVersionsInPulse(bool enable);
		void setViewDate(char *date);
		void setTimeContext(Data *start, Data *end, Data *delta);
		void createPulse(int shot);
		void deletePulse(int shot);
		StringArray *findTags(char *wild);
		void removeTag(char const * tagName);
		int64_t getDatafileSize();
	};
/////////////////End Class Tree /////////////////////
#ifdef CACHEDTREES
/////////////////CachedTree/////////////////////////
#define DEFAULT_CACHE_SIZE 2000000
	class EXPORT CachedTree: public Tree
	{
		bool cacheShared;
		int cacheSize;
	public:
		CachedTree(char *name,int shot,bool shared,int size);
		CachedTree(char *name, int shot);
		virtual ~CachedTree();
		virtual void open();
		virtual void close();
		void configure(bool cacheShared, int cacheSize)
		{
			this->cacheShared = cacheShared;
			this->cacheSize = cacheSize;
		}
		bool isCacheShared() {return cacheShared;}
		int getCacheSize() { return cacheSize;}
		CachedTreeNode *getCachedNode(char *path);
		static void synch();
	};
/////////////////End CachedTree/////////
#endif
/////////////Class Event///////////
class EXPORT Event {
	public:
		char *eventName;
		std::string eventBuf;
		int eventId;
		bool waitingEvent;
		int64_t eventTime;
		Event() {}
		Event(char *evName);
		virtual ~Event();

		char const * getRaw(std::size_t * size) {
			*size = eventBuf.length();
			return eventBuf.c_str();
		}

		Uint64 *getTime() {
			return new Uint64(eventTime);
		}

		char *getName() {
			return eventName;
		}

	    Data *getData();

		void wait() {
			condition.wait();
		}

		void wait(std::size_t secs) {
			if (condition.waitTimeout(secs * 1000) == false)
				throw MdsException("Timeout Occurred");
		}

		Data *waitData() {
			wait();
			return getData();
		}

		Data *waitData(std::size_t secs) {
			wait(secs);
			return getData();
		}

		void abort() {
		}

		char const * waitRaw(std::size_t * size) {
			wait();
			return getRaw(size);
		}

		virtual void run() {
			condition.notify();
		}

		static void setEvent(char *evName) {setEventRaw(evName, 0, NULL); }
		static void setEventRaw(char *evName, int bufLen, char *buf);
		static void setEvent(char *evName, Data *evData);
//To keep them compatible with python
		static void setevent(char *evName) {setEvent(evName); }
		static void seteventRaw(char *evName, int bufLen, char *buf){setEventRaw(evName, bufLen, buf);}
		static void setevent(char *evName, Data *evData);

protected:
	virtual void connectToEvents();
	virtual void disconnectFromEvents();

private:
	ConditionVar condition;
};

///////////////remote data access classes ////////////////
	class Connection;
	class EXPORT GetMany: public List
	{
		Connection *conn;
		Dictionary *evalRes;
	public:
		GetMany(Connection *conn)
		{
			this->conn = conn;
			evalRes = 0;
		}
		~GetMany()
		{
			if(evalRes)
				delete evalRes;
		}
		void append(char *name, char *expr, Data **args, int nArgs);
		void insert(int idx, char *name, char *expr, Data **args, int nArgs);
		void insert(char *beforeName, char *name, char *expr, Data **args, int nArgs);
		void remove(char *name);
		void execute();
		Data *get(char *name);
	};
	class EXPORT PutMany: public List
	{
		Connection *conn;
		Dictionary *evalRes;
	public:
		PutMany(Connection *conn)
		{
			this->conn = conn;
			evalRes = 0;
		}
		~PutMany()
		{
			if(evalRes)
				delete evalRes;
		}
		void append(char *name, char *expr, Data **args, int nArgs);
		void insert(int idx, char *name, char *expr, Data **args, int nArgs);
		void insert(char *beforeName, char *name, char *expr, Data **args, int nArgs);
		void remove(char *name);
		void execute();
		void checkStatus(char *name);
	};

class EXPORT Connection
{
public:
		Connection(char *mdsipAddr);
		~Connection();
		void openTree(char *tree, int shot);
		void closeAllTrees();
		void closeTree(char *tree, int shot)
		{
			closeAllTrees();
		}
		void setDefault(char *path);
		Data *get(const char *expr, Data **args, int nArgs);
		Data *get(const char *expr)
		{
			return get(expr, 0, 0);
		}
		void put(const char *path, char *expr, Data **args, int nArgs);
		PutMany *putMany()
		{
			return new PutMany(this);
		}
		GetMany *getMany()
		{
			return new GetMany(this);
		}

private:
	void lockLocal();
	void unlockLocal();
	void lockGlobal();
	void unlockGlobal();

	int sockId;
	Mutex mutex;
	static Mutex globalMutex;
};
	class EXPORT Scope 
	{
		int x, y, width, height;
		int idx;
	public:
		Scope(const char *name, int x = 100, int y = 100, int width = 400, int height = 400);
		void plot(Data *x, Data *y , int row = 1, int col = 1, const char *color = "black");
		void oplot(Data *x, Data *y , int row = 1, int col = 1, const char *color = "black");
		void show();
	};
//////////////Support functions////////
EXPORT Data *deserialize(char const * serialized);
EXPORT Data *compile(const char *expr);
EXPORT Data *compileWithArgs(const char *expr, int nArgs ...);
EXPORT Data *compile(const char *expr, Tree *tree);
EXPORT Data *compileWithArgs(const char *expr, Tree *tree, int nArgs ...);
EXPORT Data *execute(const char *expr);
EXPORT Data *executeWithArgs(const char *expr, int nArgs ...);
EXPORT Data *execute(const char *expr, Tree *tree);
EXPORT Data *executeWithArgs(const char *expr, Tree *tree, int nArgs ...);
Tree *getActiveTree();
void setActiveTree(Tree *tree);
//Required for handling dynamic memory allocated in a different DLL on windows
//in Debug configuration
EXPORT void deleteTreeNode(TreeNode *node);
EXPORT void deleteTree(Tree *tree);
EXPORT void deleteTreeNodeArray(TreeNodeArray *nodeArray);
EXPORT std::ostream &operator<<(std::ostream &stream, MDSplus::Data *data);


EXPORT void deleteNativeArray(char * array); 
EXPORT void deleteNativeArray(unsigned char * array); 
EXPORT void deleteNativeArray(short * array); 
EXPORT void deleteNativeArray(unsigned short * array); 
EXPORT void deleteNativeArray(int * array); 
EXPORT void deleteNativeArray(unsigned int * array); 
EXPORT void deleteNativeArray(int64_t * array);
EXPORT void deleteNativeArray(uint64_t * array);
EXPORT void deleteNativeArray(float * array); 
EXPORT void deleteNativeArray(double * array); 
EXPORT void deleteNativeArray(char ** array); 
EXPORT void deleteNativeArray(Data ** array); 


}
#endif
