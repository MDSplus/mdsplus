#ifndef MDSOBJECTS_H
#define MDSOBJECTS_H

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <exception>
#include <config.h>
#include <ncidef.h>
#include <dbidef.h>
#include <usagedef.h>
#ifdef HAVE_WINDOWS_H
#include <Windows.h>
#else
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <semaphore.h>
#endif


#ifndef HAVE_WINDOWS_H
#include <mdstypes.h>
#endif
#ifdef HAVE_WINDOWS_H
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
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


#ifndef HAVE_WINDOWS_H
#define EXPORT
#else
#define EXPORT __declspec(dllexport)
#endif

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
using namespace std;

namespace MDSplus  {

//Required for handling dynamic memory allocated in a different DLL on windows
//in Debug configuration
EXPORT void deleteNativeArray(char *array);
EXPORT void deleteNativeArray(short *array);
EXPORT void deleteNativeArray(int *array);
EXPORT void deleteNativeArray(long *array);
#if (SIZEOF_LONG != 8)
EXPORT void deleteNativeArray(_int64 *array);
#endif
EXPORT void deleteNativeArray(float *array);
EXPORT void deleteNativeArray(double *array);
EXPORT void deleteNativeArray(char **array);

class Data;
EXPORT void deleteNativeArray(Data **array);
	
	
	
EXPORT void deleteString(char *str);
class Tree;
EXPORT void setActiveTree(Tree *tree);
EXPORT Tree *getActiveTree();
/////Exceptions//////////////

	class EXPORT MdsException
	{
	protected:
		int status;
		char *msg;
	public:
		MdsException(){}
		MdsException(const char *msg)
		{
			this->msg = new char[strlen(msg) + 1];
			strcpy(this->msg, msg);
		}
		MdsException(int status)
		{
			char *currMsg = MdsGetMsg(status);
			this->msg = new char[strlen(currMsg) + 1];
			strcpy(this->msg, currMsg);
		}

		virtual ~MdsException()
		{
			if(msg)
				deleteNativeArray(msg);
		}

		virtual const char* what() const 
		{
			if(msg)
				return msg;
			return MdsGetMsg(status);
		}
		friend ostream & operator << (ostream &outStream, MdsException &exc)
		{
		    return outStream << exc.what();
		}
		
	};




////////////////////Data class//////////////////////////////
class EXPORT Data 
{
		friend EXPORT Data *compile(const char *expr);
		friend EXPORT Data *compileWithArgs(const char *expr, int nArgs ...);
		friend EXPORT Data *compile(const char *expr, Tree *tree);
		friend EXPORT Data *compileWithArgs(const char *expr, Tree *tree, int nArgs ...);
		friend EXPORT Data *execute(const char *expr);
		friend EXPORT Data *executeWithArgs(const char *expr, int nArgs ...);
		friend EXPORT Data *execute(const char *expr, Tree *tree);
		friend EXPORT Data *executeWithArgs(const char *expr, Tree *tree, int nArgs ...);
		friend EXPORT Data *deserialize(char *serialized);
		friend EXPORT Data *deserialize(Data *serialized);
		friend EXPORT void deleteData(Data *);
		virtual void propagateDeletion(){}

		Data *units;
		Data *error;
		Data *validation;
		Data *help;

protected:
		bool changed;
		Data *dataCache;
		virtual bool isImmutable() {return true;}
	
	public: 
		int clazz, dtype;
		virtual bool hasChanged() {return !isImmutable() || changed;}
		void *completeConversionToDsc(void *dsc);
		void setAccessory(Data *units, Data *error, Data *help, Data *validation)
		{
			this->units = units;
			this->error = error;
			this->help = help;
			this->validation = validation;
		}
		
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
		void *operator new(size_t sz);
		void operator delete(void *p);
		virtual bool equals(Data *data) {return false;}
		Data()
		{
			changed = true;
			refCount = 0;
			dataCache = 0;
			units = error = help = validation = 0;
		}
		virtual ~Data()
		{
			//printf("DELETED\n");
		}
		Data *evaluate();
		Data *data();
		char *decompile();

		Data *clone();

		char *	serialize(int *size);

		virtual char getByte(); 
		virtual short getShort();
		virtual int getInt();
		virtual _int64 getLong();
		virtual float getFloat();
		virtual double getDouble(); 
		virtual char * getString(){return decompile();}

		virtual int * getShape(int *numDim);
		virtual char *getByteArray(int *numElements);
		virtual short * getShortArray(int *numElements);

		virtual int * getIntArray(int *numElements);

		virtual _int64 * getLongArray(int *numElements);

		virtual float * getFloatArray(int *numElements);

		virtual double * getDoubleArray(int *numElements);
		virtual char ** getStringArray(int *numElements)
		{
			printf("GET STRING ARRAY DATA !!!!!\n");
			*numElements = 0; 
			return NULL;
		}
		virtual Data *getDimensionAt(int dimIdx);
		virtual int getSize() {return 1;}

		virtual Data *getUnits()
		{
			if(units)
				units->refCount++;
			return units;
		}
		virtual void setUnits(Data *inUnits)
		{
			if(units)
			{
				deleteData(units);
			}
			units = inUnits;
			inUnits->refCount++;
		}
		virtual Data *getError()
		{
			if(error)
				error->refCount++;
			return error;
		}
		virtual void setError(Data *inError)
		{
			if(error)
			{
				deleteData(error);
			}
			error = inError;
			inError->refCount++;
		}
		virtual Data *getHelp()
		{
			if(help)
				help->refCount++;
			return help;
		}
		virtual void setHelp(Data *inHelp)
		{
			if(help)
			{
				deleteData(help);
			}
			help = inHelp;
			inHelp->refCount++;
		}
		virtual Data *getValidation()
		{
			if(validation)
				validation->refCount++;
			return validation;
		}
		virtual void setValidation(Data *inValidation)
		{
			if(validation)
			{
				deleteData(validation);
			}
			validation = inValidation;
			inValidation->refCount++;
		}
		friend ostream & operator << (ostream &outStream, Data *data)
		{
		    return outStream << data->decompile();
		}
		virtual void plot();
	};



	class Empty: public Data
	{
	public:
		Empty(){};
	};



	class EXPORT Scalar: public Data
	{
		public:
		int length;
		char *ptr;

		~Scalar()
		{
			deleteNativeArray(ptr);
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
			clazz = CLASS_S;
			dtype = DTYPE_B;
			length = 1;
			ptr = new char[1];
			ptr[0] = val;
			setAccessory(units, error, help, validation);
		}
		char getByte() {return ptr[0];}
		short getShort() {return (short)ptr[0];}
		int getInt() {return (int)ptr[0];}
		_int64 getLong() {return (_int64)ptr[0];}
		float getFloat() {return (float)ptr[0];}
		double getDouble() {return (double)ptr[0];}
	};

	class  EXPORT Uint8 : public Scalar
	{
	public:
		Uint8(unsigned char val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			clazz = CLASS_S;
			dtype = DTYPE_BU;
			length = 1;
			ptr = new char[1];
			ptr[0] = val;
			setAccessory(units, error, help, validation);
		}
		char getByte() {return ptr[0];}
		short getShort() {return (short)ptr[0];}
		int getInt() {return (int)ptr[0];}
		_int64 getLong() {return (_int64)ptr[0];}
		float getFloat() {return (float)ptr[0];}
		double getDouble() {return (double)ptr[0];}
	};


	class  EXPORT Int16 : public Scalar
	{
	public:
		Int16(short val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			clazz = CLASS_S;
			dtype = DTYPE_W;
			length = 2;
			ptr = new char[2];
			*(short *)ptr = val;
			setAccessory(units, error, help, validation);
		}
		char getByte() {return (char)(*(short *)ptr);}
		short getShort() {return (short)(*(short *)ptr);}
		int getInt() {return (int)(*(short *)ptr);}
		_int64 getLong() {return (_int64)(*(short *)ptr);}
		float getFloat() {return (float)(*(short *)ptr);}
		double getDouble() {return (double)(*(short *)ptr);}
	};

	class  EXPORT Uint16 : public Scalar
	{
	public:
		Uint16(unsigned short val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			clazz = CLASS_S;
			dtype = DTYPE_WU;
			length = 2;
			ptr = new char[2];
			*(unsigned short *)ptr = val;
			setAccessory(units, error, help, validation);
		}
		char getByte() {return (char)(*(short *)ptr);}
		short getShort() {return (short)(*(short *)ptr);}
		int getInt() {return (int)(*(short *)ptr);}
		_int64 getLong() {return (_int64)(*(short *)ptr);}
		float getFloat() {return (float)(*(short *)ptr);}
		double getDouble() {return (double)(*(short *)ptr);}
	};

	class  EXPORT Int32 : public Scalar
	{
	public:
		Int32(int val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			clazz = CLASS_S;
			dtype = DTYPE_L;
			length = sizeof(int);
			ptr = new char[sizeof(int)];
			*(int *)ptr = val;
			setAccessory(units, error, help, validation);
		}
		char getByte() {return (char)(*(int *)ptr);}
		short getShort() {return (short)(*(int *)ptr);}
		int getInt() {return (int)(*(int *)ptr);}
		_int64 getLong() {return (_int64)(*(int *)ptr);}
		float getFloat() {return (float)(*(int *)ptr);}
		double getDouble() {return (double)(*(int *)ptr);}
	};

	class  EXPORT Uint32 : public Scalar
	{
	public:
		Uint32(unsigned int val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			clazz = CLASS_S;
			dtype = DTYPE_LU;
			length = sizeof(int);
			ptr = new char[sizeof(int)];
			*(unsigned int *)ptr = val;
			setAccessory(units, error, help, validation);
		}
		char getByte() {return (char)(*(int *)ptr);}
		short getShort() {return (short)(*(int *)ptr);}
		int getInt() {return (int)(*(int *)ptr);}
		_int64 getLong() {return (_int64)(*(int *)ptr);}
		float getFloat() {return (float)(*(int *)ptr);}
		double getDouble() {return (double)(*(int *)ptr);}
	};

	class EXPORT Int64 : public Scalar
	{
	public:
		Int64(_int64 val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			clazz = CLASS_S;
			dtype = DTYPE_Q;
			length = sizeof(_int64);
			ptr = new char[sizeof(_int64)];
			*(_int64 *)ptr = val;
			setAccessory(units, error, help, validation);
		}
		char getByte() {return (char)(*(_int64 *)ptr);}
		short getShort() {return (short)(*(_int64 *)ptr);}
		int getInt() {return (int)(*(_int64 *)ptr);}
		_int64 getLong() {return (_int64)(*(_int64 *)ptr);}
		float getFloat() {return (float)(*(_int64 *)ptr);}
		double getDouble() {return (double)(*(_int64 *)ptr);}
	};


	class EXPORT Uint64 : public Scalar
	{
	public:
#ifdef HAVE_WINDOWS_H
		Uint64(unsigned _int64 val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			clazz = CLASS_S;
			dtype = DTYPE_QU;
			length = sizeof(_int64);
			ptr = new char[sizeof(_int64)];
			*(unsigned _int64 *)ptr = val;
			setAccessory(units, error, help, validation);
		}
#else
		Uint64(_int64u val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			clazz = CLASS_S;
			dtype = DTYPE_QU;
			length = sizeof(_int64);
			ptr = new char[sizeof(_int64)];
			*(_int64u *)ptr = val;
			setAccessory(units, error, help, validation);
		}
#endif
		char getByte() {return (char)(*(_int64 *)ptr);}
		short getShort() {return (short)(*(_int64 *)ptr);}
		int getInt() {return (int)(*(_int64 *)ptr);}
		_int64 getLong() {return (_int64)(*(_int64 *)ptr);}
		float getFloat() {return (float)(*(_int64 *)ptr);}
		double getDouble() {return (double)(*(_int64 *)ptr);}
		char *getDate();
	};



	class  EXPORT Float32 : public Scalar
	{
	public:
		Float32(float val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			clazz = CLASS_S;
			dtype = DTYPE_FLOAT;
			length = sizeof(float);
			ptr = new char[sizeof(float)];
			*(float *)ptr = val;
			setAccessory(units, error, help, validation);
		}
		char getByte() {return (char)(*(float *)ptr);}
		short getShort() {return (short)(*(float *)ptr);}
		int getInt() {return (int)(*(float *)ptr);}
		_int64 getLong() {return (_int64)(*(float *)ptr);}
		float getFloat() {return (float)(*(float *)ptr);}
		double getDouble() {return (double)(*(float *)ptr);}
	};


	class  EXPORT Float64 : public Scalar
	{
	public:
		Float64(double val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			clazz = CLASS_S;
			dtype = DTYPE_DOUBLE;
			length = sizeof(double);
			ptr = new char[sizeof(double)];
			*(double *)ptr = val;
			setAccessory(units, error, help, validation);
		}
		char getByte() {return (char)(*(double *)ptr);}
		short getShort() {return (short)(*(double *)ptr);}
		int getInt() {return (int)(*(double *)ptr);}
		_int64 getLong() {return (_int64)(*(double *)ptr);}
		float getFloat() {return (float)(*(double *)ptr);}
		double getDouble() {return (double)(*(double *)ptr);}
	};


	class  EXPORT String : public Scalar
	{
	public:
		String(const char *val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			clazz = CLASS_S;
			dtype = DTYPE_T;
			length = strlen(val);
			ptr = new char[length+1];
			ptr[length] = 0;
			memcpy(ptr, val, length);
			setAccessory(units, error, help, validation);
		}
		String(const char *val, int len, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			clazz = CLASS_S;
			dtype = DTYPE_T;
			length = len;
			ptr = new char[length+1];
			memcpy(ptr, val, length);
			ptr[length] = 0;
			setAccessory(units, error, help, validation);
		}

		char *getString()
		{
			char *res = new char[length + 1];
			memcpy(res, ptr, length);
			res[length] = 0;
			return res;
		}
		bool equals(Data *data)
		{
			if(data->clazz != clazz || data->dtype != dtype) return false;
			String *dataStr = (String *)data;
			if (dataStr->length != length) return false;
			return !strncmp(dataStr->ptr, ptr, length);
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
		void setSpecific(char *data, int length, int dtype, int nData)
		{
			clazz = CLASS_A;
			this->dtype = dtype;
			this->length = length;
			arsize = nData * length;
			nDims = 1;
			dims[0] = nData;
			ptr = new char[arsize];
			memcpy(ptr, data, arsize);
		}
		void setSpecific(char *data, int length, int dtype, int nDims, int *dims)
		{
			clazz = CLASS_A;
			this->dtype = dtype;
			this->length = length;
			arsize = length;
			for(int i = 0; i < nDims; i++)
			{
				arsize *= dims[i];
				this->dims[i] = dims[i];
			}
			this->nDims = nDims;
			ptr = new char[arsize];
			memcpy(ptr, data, arsize);
		}
	public:
		Array() {clazz = CLASS_A;}
		~Array()
		{
			deleteNativeArray(ptr);
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

		void setElementAt(int dim, Data *data)
		{
			setElementAt(&dim, 1, data);
		}



		void *convertToDsc();
		void *getArray() {return ptr;}
		char *getByteArray(int *numElements);
		short *getShortArray(int *numElements);
		int *getIntArray(int *numElements);
		_int64 *getLongArray(int *numElements);
		float *getFloatArray(int *numElements);
		double *getDoubleArray(int *numElements);
		virtual char **getStringArray(int *numElements);

	};
	class Int8Array: public Array
	{
	public:
		Int8Array(char *data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific(data, 1, DTYPE_B, nData);
			setAccessory(units, error, help, validation);
		}
		Int8Array(char *data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific(data, 1, DTYPE_B, nDims, dims);
			setAccessory(units, error, help, validation);
		}
	};

	class EXPORT Uint8Array: public Array
	{
	friend Data *MDSplus::deserialize(Data *serializedData);
	public:
		Uint8Array(char *data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific(data, 1, DTYPE_BU, nData);
			setAccessory(units, error, help, validation);
		}
		Uint8Array(char *data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific(data, 1, DTYPE_BU, nDims, dims);
			setAccessory(units, error, help, validation);
		}
		Data* deserialize();
	};

	class Int16Array: public Array
	{
	public:
		Int16Array(short *data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific((char *)data, 2, DTYPE_W, nData);
			setAccessory(units, error, help, validation);
		}
		Int16Array(short *data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific((char *)data, 2, DTYPE_W, nDims, dims);
			setAccessory(units, error, help, validation);
		}
	};
	class Uint16Array: public Array
	{
	public:
		Uint16Array(short *data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific((char *)data, 2, DTYPE_WU, nData);
			setAccessory(units, error, help, validation);
		}
		Uint16Array(short *data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific((char *)data, 2, DTYPE_WU, nDims, dims);
			setAccessory(units, error, help, validation);
		}
	};
	class Int32Array: public Array
	{
	public:
		Int32Array(int *data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific((char *)data, sizeof(int), DTYPE_L, nData);
			setAccessory(units, error, help, validation);
		}
		Int32Array(int *data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific((char *)data, sizeof(int), DTYPE_L, nDims, dims);
			setAccessory(units, error, help, validation);
		}
	};
	class Uint32Array: public Array
	{
	public:
		Uint32Array(int *data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific((char *)data, sizeof(int), DTYPE_LU, nData);
			setAccessory(units, error, help, validation);
		}
		Uint32Array(int *data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific((char *)data, sizeof(int), DTYPE_LU, nDims, dims);
			setAccessory(units, error, help, validation);
		}
	};
	class Int64Array: public Array
	{
	public:
		Int64Array(_int64 *data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific((char *)data, 8, DTYPE_Q, nData);
			setAccessory(units, error, help, validation);
		}
		Int64Array(_int64 *data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific((char *)data, 8, DTYPE_Q, nDims, dims);
			setAccessory(units, error, help, validation);
		}
	};
	class Uint64Array: public Array
	{
	public:
		Uint64Array(_int64 *data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific((char *)data, 8, DTYPE_QU, nData);
			setAccessory(units, error, help, validation);
		}
		Uint64Array(_int64 *data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific((char *)data, 8, DTYPE_QU, nDims, dims);
			setAccessory(units, error, help, validation);
		}
	};
	class Float32Array: public Array
	{
	public:
		Float32Array(float *data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific((char *)data, sizeof(float), DTYPE_FLOAT, nData);
			setAccessory(units, error, help, validation);
		}
		Float32Array(float *data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific((char *)data, sizeof(float), DTYPE_FLOAT, nDims, dims);
			setAccessory(units, error, help, validation);
		}
	};
	class Float64Array: public Array
	{
	public:
		Float64Array(double *data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific((char *)data, sizeof(double), DTYPE_DOUBLE, nData);
			setAccessory(units, error, help, validation);
		}
		Float64Array(double *data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			setSpecific((char *)data, sizeof(double), DTYPE_DOUBLE, nDims, dims);
			setAccessory(units, error, help, validation);
		}
	};
	class StringArray: public Array
	{
	public:
		StringArray(char **data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			//Pad all string to longest 
			int i;
			int maxLen = 0;
			for(i = 0; i < nData; i++)
			{
				if(strlen(data[i]) > (unsigned int)maxLen)
					maxLen = strlen(data[i]);
			}
			char *padData = new char[nData * maxLen];
			for(i = 0; i < nData; i++)
			{
				int currLen = strlen(data[i]);
				memcpy(&padData[i * maxLen], data[i], currLen);
				if(currLen < maxLen)
					memset(&padData[i*maxLen + currLen], ' ', maxLen - currLen);
			}
			setSpecific(padData, maxLen, DTYPE_T, nData);
			setAccessory(units, error, help, validation);
			delete[] padData;
		}
		StringArray(char *data, int nStrings, int stringLen, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0) //For contiuguous
		{
			setSpecific(data, stringLen, DTYPE_T, nStrings);
			setAccessory(units, error, help, validation);
		}
	};

/////////////////////////COMPOUND DATA//////////////////////////////////
	class EXPORT Compound: public Data
	{
	protected:
		int length;
		char *ptr;
		int nDescs;
		Data **descs;
		void incrementRefCounts()
		{
			for(int i = 0; i < nDescs; i++)
			{
				if(descs[i]) descs[i]->refCount++;
			}
		}
		void assignDescAt(Data *data, int idx)
		{
			if(descs[idx])
			{
				deleteData(descs[idx]);
			}
			descs[idx] = data;
			if(data) data->refCount++;
			changed = true;
		}

		bool hasChanged()
		{
			if (changed || !isImmutable()) return true;
			for(int i = 0; i < nDescs; i++)
				if(descs[i] && descs[i]->hasChanged())
					return true;
			return false;
		}
	public:
		Compound() {clazz = CLASS_R;}
		Compound(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			this->dtype = dtype;
			this->length = length;
			if(length > 0)
			{
				this->ptr = new char[length];
				memcpy(this->ptr, ptr, length);
			}
			else
				this->ptr = 0;
			this->nDescs = nDescs;
			if(nDescs > 0)
			{
				this->descs = new Data *[nDescs];
				for(int i = 0; i < nDescs; i++)
				{
					this->descs[i] = (Data *)descs[i];
					if(this->descs[i])
						this->descs[i]->refCount++;
				}
			}
			else
				this->descs = 0;
			setAccessory(units, error, help, validation);
		}

		virtual ~Compound()
		{
			if(length > 0)
				deleteNativeArray(ptr);
			if(nDescs > 0)
				deleteNativeArray(descs);
		}

		virtual void propagateDeletion()
		{
			for(int i = 0; i < nDescs; i++)
			{
				if(descs[i])
				{
					deleteData(descs[i]);
				}
			}
		}



		void *convertToDsc();
	};

	class Signal: public Compound
	{
	public: 
		Signal(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):Compound(dtype, length, ptr, nDescs, descs)
		{
			setAccessory(units, error, help, validation);
		}
		Signal(Data *data, Data *raw, Data *dimension, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_SIGNAL;
			length = 0;
			ptr = 0;
			nDescs = 3;
			descs = new Data *[3];
			descs[0] = data;
			descs[1] = raw;
			descs[2] = dimension;
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}
		Signal(Data *data, Data *raw, Data *dimension1, Data *dimension2, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_SIGNAL;
			length = 0;
			ptr = 0;
			nDescs = 4;
			descs = new Data *[4];
			descs[0] = data;
			descs[1] = raw;
			descs[2] = dimension1;
			descs[3] = dimension2;
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}
		Signal(Data *data, Data *raw, int nDims, Data **dimensions, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_SIGNAL;
			length = 0;
			ptr = 0;
			nDescs = 2 + nDims;
			descs = new Data *[nDescs];
			descs[0] = data;
			descs[1] = raw;
			for(int i = 0; i < nDims; i++)
			{
				descs[2+i] = dimensions[i];
			}
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}
		Data *getData(){return descs[0];}
		Data *getRaw(){return descs[1];}
		Data *getDimension()
		{
			descs[2]->refCount++;
			return descs[2];
		}
		int genNumDimensions() {return nDescs - 2;}
		Data *getDimensionAt(int idx) 
		{	
			if(descs[2+idx]) descs[2 + idx]->refCount++;
			return descs[2 + idx];
		}
		void setData(Data *data) {assignDescAt(data, 0);}
		void setRaw(Data *raw){assignDescAt(raw, 1);}
		void setDimension(Data *dimension) {assignDescAt(dimension, 2);}
		void setDimensionAt(Data *dimension, int idx) {assignDescAt(dimension, 2 + idx);}
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
			length = 0;
			ptr = 0;
			nDescs = 2;
			descs = new Data *[2];
			descs[0] = window;
			descs[1] = axis;
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}
		Data *getWindow(){
			if(descs[0]) descs[0]->refCount++;
			return descs[0];
		}
		Data *getAxis(){
			if(descs[1]) descs[1]->refCount++;
			return descs[1];
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
			length = 0;
			ptr = 0;
			nDescs = 3;
			descs = new Data *[3];
			descs[0] = startidx;
			descs[1] = endidx;
			descs[2] = value_at_idx0;
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}
		Data *getStartIdx(){return descs[0];}
		Data *getEndIdx(){return descs[1];}
		Data *getValueAt0(){return descs[2];}
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
		Function(char opcode, int nargs, Data **args, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_FUNCTION;
			length = 1;
			ptr = new char[1];
			*ptr = opcode;
			nDescs = nargs;
			descs = new Data *[nargs];
			for(int i = 0; i < nargs; i++)
				descs[i] = args[i];
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}
		char getOpcode() { return *ptr;}
		int getNumArguments() { return nDescs;}
		Data *getArgumentAt(int idx) 
		{
			if(descs[idx]) descs[idx]->refCount++;
			return descs[idx];
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
			length = 0;
			ptr = 0;
			nDescs = 4;
			descs = new Data *[4];
			descs[0] = image;
			descs[1] = model;
			descs[2] = name;
			descs[3] = qualifiers;
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}
		Data *getImage()
		{	
			if(descs[0]) descs[0]->refCount++;
			return descs[0];
		}
		Data *getModel()
		{
			if(descs[1])
				descs[1]->refCount++;
			return descs[1];
		}
		Data *getName()
		{
			if(descs[2]) descs[2]->refCount++;
			return descs[2];
		}
		Data *getQualifiers()
		{
			if(descs[3]) descs[3]->refCount++;
			return descs[3];
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
			length = 0;
			ptr = 0;
			nDescs = 3;
			descs = new Data *[3];
			descs[0] = begin;
			descs[1] = ending;
			descs[2] = deltaval;
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}
		Data *getBegin()
		{
			if(descs[0]) descs[0]->refCount++;
			return descs[0];
		}
		Data *getEnding()
		{
			if(descs[1]) descs[1]->refCount++;
			return descs[1];
		}
		Data *getDeltaVal()
		{
			if(descs[2]) descs[2]->refCount++;			
			return descs[2];
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
			length = 0;
			ptr = 0;
			nDescs = 5;
			descs = new Data *[5];
			descs[0] = dispatch;
			descs[1] = task;
			descs[2] = errorlogs;
			descs[3] = completion_message;
			descs[4] = performance;
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}
		Data *getDispatch()
		{
			if(descs[0]) descs[0]->refCount++;
			return descs[0];
		}
		Data *getTask()
		{
			if(descs[1]) descs[1]->refCount++;
			return descs[1];
		}
		Data *getErrorLogs()
		{
			if(descs[2]) descs[2]->refCount++;
			return descs[2];
		}
		Data *getCompletionMessage()
		{
			if(descs[3]) descs[3]->refCount++;
			return descs[3];
		}
		Data *getPerformance()
		{
			if(descs[4]) descs[4]->refCount++;
			return descs[4];
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
			length = 0;
			ptr = 0;
			nDescs = 4;
			descs = new Data *[4];
			descs[0] = ident;
			descs[1] = phase;
			descs[2] = when;
			descs[3] = completion;
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}
		Data *getIdent(){
			if(descs[0]) descs[0]->refCount++;
			return descs[0];
		}
		Data *getPhase(){
			if(descs[1]) descs[1]->refCount++;
			return descs[1];
		}
		Data *getWhen()
		{
			if(descs[2]) descs[2]->refCount++;
			return descs[2];
		}
		Data *getCompletion()
		{
			if(descs[3]) descs[3]->refCount++;
			return descs[3];
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
			length = 0;
			ptr = 0;
			nDescs = 2;
			descs = new Data *[2];
			descs[0] = timeout;
			descs[1] = program;
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}
		Data *getTimeout()
		{
			if(descs[0]) descs[0]->refCount++;
			return descs[0];
		}
		Data *getProgram()
		{
			if(descs[1]) descs[1]->refCount++;
			return descs[1];
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
			length = 0;
			ptr = 0;
			nDescs = 3 + nargs;
			descs = new Data *[nDescs];
			descs[0] = timeout;
			descs[1] = image;
			descs[2] = routine;
			for(int i = 0; i < nargs; i++)
				descs[3+i] = args[i];
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}
		Data *getTimeout()
		{
			if(descs[0]) descs[0]->refCount++;
			return descs[0];
		}
		Data *getImage(){
			if(descs[1]) descs[1]->refCount++;
			return descs[1];
		}
		Data *getRoutine()
		{
			if(descs[2]) descs[2]->refCount++;
			return descs[2];
		}
		Data *getArgumentAt(int idx)
		{
			if(descs[3+idx]) descs[3+idx]->refCount++;
			return descs[3 + idx];
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
			length = 0;
			ptr = 0;
			nDescs = 3 + nargs;
			descs = new Data *[nDescs];
			descs[0] = timeout;
			descs[1] = language;
			descs[2] = procedure;
			for(int i = 0; i < nargs; i++)
				descs[3+i] = args[i];
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}
		Data *getTimeout(){
			if(descs[0]) descs[0]->refCount++;
			return descs[0];
		}
		Data *getLanguage(){
			if(descs[1]) descs[1]->refCount++;
			return descs[1];
		}
		Data *getProcedure()
		{
			if(descs[2]) descs[2]->refCount++;
			return descs[2];
		}
		Data *getArgumentAt(int idx)
		{
			if(descs[3+idx]) descs[3+idx]->refCount++;
			return descs[3 + idx];
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
			length = 0;
			ptr = 0;
			nDescs = 3 + nargs;
			descs = new Data *[nDescs];
			descs[0] = timeout;
			descs[1] = method;
			descs[2] = object;
			for(int i = 0; i < nargs; i++)
				descs[3+i] = args[i];
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}
		Data *getTimeout(){
			if(descs[0]) descs[0]->refCount++;
			return descs[0];
		}
		Data *getMethod()
		{
			if(descs[1]) descs[1]->refCount++;
			return descs[1];
		}
		Data *getObject()
		{
			if(descs[2]) descs[2]->refCount++;
			return descs[2];
		}
		Data *getArgumentAt(int idx)
		{
			if(descs[3+idx]) descs[3+idx]->refCount++;
			return descs[3 + idx];
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
		Dependency(char opcode, Data *arg1, Data *arg2, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_DEPENDENCY;
			length = 1;
			ptr = new char[1];
			*ptr = opcode;
			nDescs = 2;
			descs = new Data *[2];
			descs[0] = arg1;
			descs[1] = arg2;
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}
		char getOpcode(){return *ptr;}
		Data *getArg1(){
			if(descs[0]) descs[0]->refCount++;
			return descs[0];
		}
		Data *getArg2(){
			if(descs[1]) descs[1]->refCount++;
			return descs[1];
		}
		void setOpcode(char opcode)
		{
			*ptr = opcode;
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
		Condition(char opcode, Data *arg, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_CONDITION;
			length = 1;
			ptr = new char[1];
			*ptr = opcode;
			nDescs = 1;
			descs = new Data *[1];
			descs[0] = arg;
			incrementRefCounts();
			setAccessory(units, error, help, validation);
		}
		char getOpcode(){return *ptr;}
		Data *getArg(){
			if(descs[0]) descs[0]->refCount++;
			return descs[0];
		}
		void setOpcode(char opcode)
		{
			*ptr = opcode;
		}
		void setArg(Data *arg) {assignDescAt(arg, 0);}
	};

	class Call: public Compound
	{
	public: 
		Call(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):Compound(dtype, length, ptr, nDescs, descs)
		{
			setAccessory(units, error, help, validation);
		}
		Call(Data *image, Data *routine, int nargs, Data **args, char retType = DTYPE_L, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_CALL;
			length = 1;
			ptr = new char;
			*ptr = retType;
			nDescs = 2 + nargs;
			descs = new Data *[nDescs];
			descs[0] = image;
			descs[1] = routine;
			for(int i = 0; i < nargs; i++)
				descs[2+i] = args[i];
			incrementRefCounts();
		}
		char getRetType() {return *ptr;}
		void setRetType(char retType){*ptr = retType;}
		Data *getImage()
		{
			if(descs[0]) descs[0]->refCount++;
			return descs[0];
		}
		Data *getRoutine(){
			if(descs[1]) descs[1]->refCount++;
			return descs[1];
		}
		Data *getArgumentAt(int idx)
		{
			if(descs[2+idx]) descs[2+idx]->refCount++;
			return descs[2 + idx];
		}
		void setImage(Data *image){assignDescAt(image, 0);}
		void setRoutine(Data *routine) {assignDescAt(routine, 1);}
		void setArgumentAt(Data *argument, int idx) {assignDescAt(argument, 2 + idx);}
	};

/////////////////////APD///////////////////////
#define DSCS_STEP 64
	class EXPORT Apd: public Data
	{
	protected:
		int nDescs;
		int descsSize;
		Data **descs;

		void resize()
		{
			descsSize += DSCS_STEP;
			Data **newDescs = new Data *[descsSize];
			for(int i = 0; i < nDescs; i++)
				newDescs[i] = descs[i];
			if(descs)
				deleteNativeArray(descs);
			descs = newDescs;
		}
		
	public:
		Apd(Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0) 
		{
			clazz = CLASS_APD; 
			dtype = DTYPE_DSC; 
			nDescs = descsSize = 0;
			descs = 0;
			setAccessory(units, error, help, validation);
		}
		Apd(int nData, Data **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			clazz = CLASS_APD; 
			dtype = DTYPE_DSC; 
			nDescs = nData;
			descsSize = DSCS_STEP * (nData/DSCS_STEP + 1);
			this->descs = new Data *[descsSize];
			for(int i = 0; i < nData; i++)
			{
				if(descs[i])
					descs[i]->refCount++;
				this->descs[i] = descs[i];
			}
			setAccessory(units, error, help, validation);
		}

		virtual ~Apd()
		{
			if(descsSize > 0)
			{
				deleteNativeArray(descs);
			}
		}

		virtual void propagateDeletion()
		{
			if(nDescs > 0)
			{
				for(int i = 0; i < nDescs; i++)
				{
					if(descs[i])
					{
						deleteData(descs[i]);
					}
				}
			}
		}

		virtual bool hasChanged()
		{
			if (changed || !isImmutable()) return true;
			for(int i = 0; i < nDescs; i++)
				if(descs[i] && descs[i]->hasChanged())
					return true;
			return false;
		}
		virtual int len()
		{
			return nDescs;
		}

		Data **getDscs()
		{
			return descs;
		}
		void *convertToDsc();
		int getDimension() {return nDescs;}
		Data **getDscArray() {return descs;}
		Data *getDescAt(int idx) 
		{
			if(descs[idx]) descs[idx]->refCount++;
			return descs[idx];
		}
		void setDescAt(int idx, Data *data)
		{
			descs[idx] = data;
			data->refCount++;
			changed = true;
		}
	};

///////////////////LIST///////////////
	class EXPORT List: public Apd
	{

	public:
		List(Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0) 
		{
			clazz = CLASS_APD; 
			dtype = DTYPE_LIST; 
			nDescs = 0;
			setAccessory(units, error, help, validation);
		}
		List(int nData, Data **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			clazz = CLASS_APD; 
			dtype = DTYPE_LIST; 
			nDescs = nData;
			descsSize = DSCS_STEP * (nData/DSCS_STEP + 1);
			this->descs = new Data *[descsSize];
			for(int i = 0; i < nData; i++)
			{
				if(descs[i])
					descs[i]->refCount++;
				this->descs[i] = descs[i];
			}
			setAccessory(units, error, help, validation);
		}

		void append(Data *data)
		{
			if(nDescs == descsSize)
			    resize();
			descs[nDescs++] = data;
			data->refCount++;
		}
		
		void remove(Data *data)
		{
			for(int i = 0; i < nDescs; i++)
			{
				if(descs[i] == data)
				{
					deleteData(descs[i]);
					for(int j = i; j < nDescs-1; j++)
						descs[j] = descs[j+1];
					nDescs--;
					return;
				}
			}
		}

		void remove(int idx)
		{
			if(idx < 0 || idx >= nDescs)
				return;
			deleteData(descs[idx]);
			for(int j = idx; j < nDescs-1; j++)
				descs[j] = descs[j+1];
			nDescs--;
		}




		void insert(int idx, Data *data)
		{
			if(nDescs == descsSize)
			    resize();
			for(int i = nDescs; i > idx; i--)
				descs[i] = descs[i-1];
			descs[idx] = data;
			data->refCount++;
		}

		Data *getElementAt(int idx)
		{
			if(idx < 0 || idx > nDescs) return 0;
			if(descs[idx])
				descs[idx]->refCount++;
			return descs[idx];
		}

	};

/////Dictionary///////
	class EXPORT Dictionary: public Apd
	{

	public:
		Dictionary(Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0) 
		{
			clazz = CLASS_APD; 
			dtype = DTYPE_DICTIONARY; 
			nDescs = 0;
			setAccessory(units, error, help, validation);
		}
		Dictionary(int nData, Data **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			clazz = CLASS_APD; 
			dtype = DTYPE_DICTIONARY; 
			nDescs = nData;
			descsSize = DSCS_STEP * (nData/DSCS_STEP + 1);
			this->descs = new Data *[descsSize];
			for(int i = 0; i < nData; i++)
			{
				if(descs[i])
					descs[i]->refCount++;
				this->descs[i] = descs[i];
			}
			setAccessory(units, error, help, validation);
		}

		Data *getItem(String *strData)
		{
			for(int i = 0; i < nDescs/2; i++)
			{
				if(strData->equals(descs[2*i]))
				{
					if(descs[2*i+1])
						descs[2*i+1]->refCount++;
					return descs[2*i+1];
				}
			}
			return 0;
		}

		void setItem(String *strData, Data *data)
		{
			for(int i = 0; i < nDescs/2; i++)
			{
				if(strData->equals(descs[2*i]))
				{
					deleteData(descs[2*i]);
					descs[2*i] = strData;
					strData->refCount++;
					deleteData(descs[2*i+1]);
					descs[2*i+1] = data;
					data->refCount++;
					return ;
				}
			}
			if(nDescs+2 > descsSize)
				resize();
			descs[nDescs] = strData;
			strData->refCount++;
			descs[nDescs + 1] = data;
			data->refCount++;
			nDescs+=2;
		}

		int len()
		{
			return nDescs/2;
		}
	};




/////////////////////////////////////////////////////////////////

//              Tree Objects

/////////////////////////////////////////////////////////////////


//using namespace std;

	class Tree;
	class TreeNode;
	class TreeNodeArray;


	class  EXPORT TreeNode: public Data
	{
	friend	ostream &operator<<(ostream &stream, TreeNode *treeNode);
	protected:
		Tree *tree;
		int nid;
		virtual bool isCached() {return false;}
		virtual int getCachePolicy() { return 0;}
//From Data
		virtual bool isImmutable() {return false;}
		virtual void *convertToDsc();

		int getFlag(int flagOfs);

		void setFlag(int flagOfs, bool val);

	public:
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
		TreeNode *getNode(char *relPath);
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

		virtual _int64 getTimeInserted();

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

		int getNumSegments();
		void getSegmentLimits(int segmentIdx, Data **start, Data **end);
			
		Array *getSegment(int segIdx);


		void beginTimestampedSegment(Array *initData);
		void makeTimestampedSegment(Array *data, _int64 *times);
		void putTimestampedSegment(Array *data, _int64 *times);

		void putRow(Data *data, _int64 *time, int size = 1024);

		void acceptSegment(Array *data, Data *start, Data *end, Data *times);
		void acceptRow(Data *data, _int64 time, bool isLast);
		StringArray *findTags();

//////////Edit methods////////////////
		TreeNode *addNode(char *name, char *usage);
		void remove(char *name);
		void rename(char *newName);
		void move(TreeNode *parent, char *newName);
		void move(TreeNode *parent);
		TreeNode *addDevice(char *name, char *type);
		void addTag(char *tagName);
		void removeTag(char *tagName);
		void setSubtree(bool isSubtree);

	};



/////////////////End Class TreeTreeNode///////////////
	class EXPORT TreePath : public TreeNode
	{
		int length;
		char *ptr;
	public:
		TreePath(char *val, Tree *tree, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0);
		TreePath(char *val, int len, Tree *tree, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0);
		void *convertToDsc();
		void resolveNid();
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
		void putLastRow(Data *data, _int64 *time);
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
		Tree(void *dbid, char *name, int shot);

	protected:
		Tree(){}
		char *name;
		int shot;
		void *ctx;

	

	public:
		Tree(char *name, int shot);
		Tree(char *name, int shot, char *mode);

		~Tree();

		void *operator new(size_t sz);
		void operator delete(void *p);

		static void lock();
		static void unlock();
		static void setCurrent(char *treeName, int shot);
		static int getCurrent(char *treeName);
		static Tree *create(char *treeName, int shot);
		
		void *getCtx() {return ctx;}
		void edit();
		void write();
		void quit();

		TreeNode *getNode(char *path);
		TreeNode *getNode(TreePath *path);
		TreeNode *getNode(String *path);
		TreeNode *addNode(char *name, char *usage);
		TreeNode *addDevice(char *name, char *type);
		void remove(char *name);

		TreeNodeArray *getNodeWild(char *path, int usageMask);
		TreeNodeArray *getNodeWild(char *path);
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
		void removeTag(char *tagName);
		_int64 getDatafileSize();
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
#include <UnnamedSemaphore.h>
	class EXPORT Event
	{
	UnnamedSemaphore sem;
	protected:
		virtual void connectToEvents();
		virtual void disconnectFromEvents();
	public:
		char *eventName;
		char *eventBuf;
		int eventBufSize;
		int eventId;
		bool waitingEvent;
		_int64 eventTime;
		Event(){sem.initialize(0);}
		Event(char *evName);
		virtual ~Event();
		char *getRaw(int *size)
		{
			*size = eventBufSize;
			return eventBuf;
		}
		Uint64 *getTime()
		{
			return new Uint64(eventTime);
		}
		char *getName() { return eventName;}
	    Data *getData();
		void wait()
		{
			waitingEvent = true;
			sem.wait();
			waitingEvent = false;
		}
		void wait(int secs)
		{
			waitingEvent = true;
			MdsTimeout timeout(secs, 0);
			int status = sem.timedWait(timeout);
			waitingEvent = false;
			if(status)
				throw new MdsException("Timeout Occurred");
		}
		Data *waitData()
		{
			waitingEvent = true;
			sem.wait();
			waitingEvent = false;
			return getData();
		}
		Data *waitData(int secs)
		{
			waitingEvent = true;
			MdsTimeout timeout(secs, 0);
			//timedWait returns 0 if no timeout, 1 otherwise
			int status = sem.timedWait(timeout);
			waitingEvent = false;
			if(status)
				throw new MdsException("Timeout Occurred");
			return getData();
		}
		void abort()
		{
			if(waitingEvent)
				sem.post();
		}
		char *waitRaw(int *size)
		{
			sem.wait();
			return getRaw(size);
		}
		virtual void run() 
		{
			sem.post();
		}
		static void setEvent(char *evName) {setEventRaw(evName, 0, NULL); }
		static void setEventRaw(char *evName, int bufLen, char *buf);
		static void setEvent(char *evName, Data *evData);
//To keep them compatible with python
		static void setevent(char *evName) {setEvent(evName); }
		static void seteventRaw(char *evName, int bufLen, char *buf){setEventRaw(evName, bufLen, buf);}
		static void setevent(char *evName, Data *evData);
	};


	class  EXPORT REvent:public Event
	{
		void *reventId;
	protected:
		virtual void connectToEvents();
		virtual void disconnectFromEvents();
	public:
		REvent(char *evName);
		virtual ~REvent(){}
		static void setEvent(char *evName, Data *data);
		static void setEventRaw(char *evName, int bufLen, char *buf);
		static void setEventAndWait(char *evName, Data *data);
		static void setEventRawAndWait(char *evName, int bufLen, char *buf);
	};



//////////////End Class Event//////////////

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
				deleteData(evalRes);
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
				deleteData(evalRes);
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
#ifdef HAVE_WINDOWS_H
		HANDLE semH;
		static HANDLE globalSemH;
		static bool globalSemHInitialized;
#else
		sem_t semStruct;
		bool semInitialized;
		static pthread_mutex_t globalMutex;
#endif
		int sockId;
		void lock();
		void unlock();
		void lockGlobal();
		void unlockGlobal();
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
	};

	class EXPORT Scope 
	{
		int x, y, width, height;
		int idx;
	public:
		Scope(char *name, int x = 100, int y = 100, int width = 400, int height = 400);
		void plot(Data *x, Data *y , int row = 1, int col = 1, char *color = "black");
		void oplot(Data *x, Data *y , int row = 1, int col = 1, char *color = "black");
		void show();
	};



//////////////Support functions////////
EXPORT void deleteData(Data *);
EXPORT Data *deserialize(char *serialized);
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
}


#endif
