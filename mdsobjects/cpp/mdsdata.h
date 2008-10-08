#ifndef MDSDATA_H
#define MDSDATA_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <iostream.h>
#include <exception>
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

extern "C" {
	void *convertToScalarDsc(int clazz, int dtype, int length, char *ptr);
	void *evaluateData(void *dscPtr, int isEvaluate);
	void freeDsc(void *dscPtr);
	void *convertFromDsc(void *dscPtr);
	char *decompileDsc(void *dscPtr);
	char *decompileDsc(void *dscPtr);
	void *compileFromExprWithArgs(char *expr, int nArgs, void *args);
	void freeChar(void *);
	void *convertToArrayDsc(int clazz, int dtype, int length, int l_length, int nDims, int *dims, void *ptr);
	void *convertToCompoundDsc(int clazz, int dtype, int length, void *ptr, int ndescs, void **descs);
	void *convertToApdDsc(int ndescs, void **ptr);
}



namespace MDSobjects  {


	class EXPORT DataException//: public exception
	{
		char *message;
	public:
		DataException(int clazz, int dtype, char *msg)
		{
			if(!msg)
			{
				message = new char[64];
				sprintf(message, "Class = %d Dtype = %d", clazz, dtype);
			}
			else
			{
				message = new char[strlen(msg) + 64];
				sprintf(message, "%s  Class = %d Dtype = %d", msg, clazz, dtype);
			}
		}
		virtual const char* what() const 
		{
			return message;
		}
		~DataException() {delete[] message;}
	};


////////////////////Data class//////////////////////////////

class EXPORT Data 
{
		friend EXPORT Data *compile(char *expr, ...);
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
		virtual void *convertToDsc() = 0;
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

		virtual char getByte(); 
		virtual short getShort();
		virtual int getInt();
		virtual _int64 getLong();
		virtual float getFloat();
		virtual double getDouble(); 
		virtual char * getString(){return decompile();}
		virtual int *getShape(int *numDim);
		virtual char *getByteArray(int *numElements);
		virtual short *getShortArray(int *numElements);
		virtual int *getIntArray(int *numElements);
		virtual _int64 *getLongArray(int *numElements);
		virtual float *getFloatArray(int *numElements);
		virtual double *getDoubleArray(int *numElements);

		virtual Data *getUnits()
		{
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
	};

#define MAX_ARGS 512
EXPORT	Data *compile(char *expr, ...)
	{
		int nArgs = 0;
		void *args[MAX_ARGS];

		va_list v;
		va_start(v, expr);
		for(int i = 0; i < MAX_ARGS; i++)
		{
			Data *currArg = va_arg(v, Data *);
			if(currArg == 0)
				break;
			args[nArgs++] = currArg->convertToDsc();
		}
		return (Data *)compileFromExprWithArgs(expr, nArgs, (void *)args);
	}

	ostream& operator<<(ostream& output, Data *data)
	{
		output << data->decompile();
		return output;
	}


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
			delete [] ptr;
		}
		 
		void *convertToDsc()
		{
			return completeConversionToDsc(convertToScalarDsc(clazz, dtype, length, ptr));
		}
	}; //ScalarData

	class Int8 : public Scalar
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
		virtual char *getString()
		{
			char *retStr = new char[64];
			sprintf(retStr, "%d", *(char *)ptr);
			return retStr;
		}
	
	};

	class Uint8 : public Scalar
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
		virtual char *getString()
		{
			char *retStr = new char[64];
			sprintf(retStr, "%d", *(char *)ptr);
			return retStr;
		}
	};


	class Int16 : public Scalar
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
		char *getString()
		{
			char *retStr = new char[64];
			sprintf(retStr, "%d", *(short *)ptr);
			return retStr;
		}
	};

	class Uint16 : public Scalar
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
		char *getString()
		{
			char *retStr = new char[64];
			sprintf(retStr, "%d", *(short *)ptr);
			return retStr;
		}
	};

	class Int32 : public Scalar
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
		char *getString()
		{
			char *retStr = new char[64];
			sprintf(retStr, "%d", *(int *)ptr);
			return retStr;
		}
	};

	class Uint32 : public Scalar
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
		char *getString()
		{
			char *retStr = new char[64];
			sprintf(retStr, "%d", *(int *)ptr);
			return retStr;
		}
	};

	class Int64 : public Scalar
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
		char *getString()
		{
			char *retStr = new char[64];
			sprintf(retStr, "%d", *(_int64 *)ptr);
			return retStr;
		}
	};


	class Uint64 : public Scalar
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
		char *getString()
		{
			char *retStr = new char[64];
			sprintf(retStr, "%d", *(_int64 *)ptr);
			return retStr;
		}
	};



	class Float32 : public Scalar
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
		char *getString()
		{
			char *retStr = new char[64];
			sprintf(retStr, "%e", *(float *)ptr);
			return retStr;
		}
	};


	class Float64 : public Scalar
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
		char *getString()
		{
			char *retStr = new char[64];
			sprintf(retStr, "%e", *(double *)ptr);
			return retStr;
		}
	};


	class String : public Scalar
	{
	public:
		String(char *val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			clazz = CLASS_S;
			dtype = DTYPE_T;
			length = strlen(val);
			ptr = new char[length];
			memcpy(ptr, val, length);
			setAccessory(units, error, help, validation);
		}
		String(char *val, int len, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
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
			char *retStr = new char[length + 1];
			memcpy(retStr, ptr, length);
			retStr[length] = 0;
			return retStr;
		}
	};



	class Nid : public Data
	{
		int nid;
	public:
		Nid(int val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			clazz = CLASS_S;
			dtype = DTYPE_NID;
			nid = val;
			setAccessory(units, error, help, validation);
		}
		void *convertToDsc()
		{
			return completeConversionToDsc(convertToScalarDsc(clazz, dtype, sizeof(int), (char *)&nid));
		}
	};

	class Path : public Data
	{
		int length;
		char *ptr;
	public:
		Path(char *val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			clazz = CLASS_S;
			dtype = DTYPE_PATH;
			length = strlen(val);
			ptr = new char[length];
			memcpy(ptr, val, length);
			setAccessory(units, error, help, validation);
		}
		Path(char *val, int len, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			clazz = CLASS_S;
			dtype = DTYPE_PATH;
			length = len;
			ptr = new char[length];
			memcpy(ptr, val, length);
			setAccessory(units, error, help, validation);
		}
		char *getString() 
		{
			char *retStr = new char[length + 1];
			memcpy(retStr, ptr, length);
			retStr[length] = 0;
			return retStr;
		}
		void *convertToDsc()
		{
			return completeConversionToDsc(convertToScalarDsc(clazz, dtype, length, ptr));
		}
	};
	class Event : public Data
	{
		int length;
		char *ptr;
	public:
		Event(char *val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			clazz = CLASS_S;
			dtype = DTYPE_EVENT;
			length = strlen(val);
			ptr = new char[length];
			memcpy(ptr, val, length);
			setAccessory(units, error, help, validation);
		}
		Event(char *val, int len, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			clazz = CLASS_S;
			dtype = DTYPE_EVENT;
			length = len;
			ptr = new char[length];
			memcpy(ptr, val, length);
			setAccessory(units, error, help, validation);
		}
		char *getString() 
		{
			char *retStr = new char[length + 1];
			memcpy(retStr, ptr, length);
			retStr[length] = 0;
			return retStr;
		}
		void *convertToDsc()
		{
			return completeConversionToDsc(convertToScalarDsc(clazz, dtype, length, ptr));
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
			delete [] ptr;
		}
		virtual int *getShape(int *numDims)
		{
			int *retDims = new int[nDims];
			*numDims = nDims;
			return retDims;
		}
		 
		Data *getElementAt(int *getDims, int getNumDims); 
		void setElementAt(int *getDims, int getNumDims, Data *data); 


		void *convertToDsc()
		{
			return completeConversionToDsc(convertToArrayDsc(clazz, dtype, length, arsize, nDims, dims, ptr));
		}
		void *getArray() {return ptr;}
		char *getByteArray(int *numElements)
		{
			char *retArr = new char[arsize];
			for(int i = 0; i < arsize; i++)
				retArr[i] = *(char *)&ptr[i * length];
			*numElements = arsize;
			return retArr;
		}
		short *getShortArray(int *numElements)
		{
			short *retArr = new short[arsize];
			int size = arsize/length;
			for(int i = 0; i < size; i++)
				retArr[i] = *(short *)&ptr[i * length];
			*numElements = arsize;
			return retArr;
		}
		int *getIntArray(int *numElements)
		{
			int *retArr = new int[arsize];
			int size = arsize/length;
			for(int i = 0; i < size; i++)
				retArr[i] = *(int *)&ptr[i * length];
			*numElements = arsize;
			return retArr;
		}
		_int64 *getLongArray(int *numElements)
		{
			_int64 *retArr = new _int64[arsize];
			int size = arsize/length;
			for(int i = 0; i < size; i++)
				retArr[i] = *(_int64 *)&ptr[i * length];
			*numElements = arsize;
			return retArr;
		}
		float *getFloatArray(int *numElements)
		{
			float *retArr = new float[arsize];
			int size = arsize/length;
			for(int i = 0; i < size; i++)
				retArr[i] = *(float *)&ptr[i * length];
			*numElements = arsize;
			return retArr;
		}
		double *getDoubleArray(int *numElements)
		{
			double *retArr = new double[arsize];
			int size = arsize/length;
			for(int i = 0; i < size; i++)
				retArr[i] = *(double *)&ptr[i * length];
			*numElements = arsize;
			return retArr;
		}

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

	class Uint8Array: public Array
	{
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
			setSpecific((char *)data, sizeof(double), DTYPE_Q, nDims, dims);
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
		}
	};

/////////////////////////COMPOUND DATA//////////////////////////////////
	class Compound: public Data
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
				delete []ptr;
			if(nDescs > 0)
				delete [] descs;
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



		void *convertToDsc()
		{
			return completeConversionToDsc(convertToCompoundDsc(clazz, dtype, length, ptr, nDescs, (void **)descs));
		}
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
		Data *getDimension(){return descs[2];}
		int genNumDimensions() {return nDescs - 2;}
		Data *getDimensionAt(int idx) {return descs[2 + idx];}
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
		Data *getWindow(){return descs[0];}
		Data *getAxis(){return descs[1];}
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
		Data *getArgumentAt(int idx) {return descs[idx];}
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
		Data *getImage(){return descs[0];}
		Data *getModel(){return descs[1];}
		Data *getName(){return descs[2];}
		Data *getQualifiers(){return descs[3];}
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
		Data *getBegin(){return descs[0];}
		Data *getEnding(){return descs[1];}
		Data *getDeltaVal(){return descs[2];}
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
		Data *getDispatch(){return descs[0];}
		Data *getTask(){return descs[1];}
		Data *getErrorLogs(){return descs[2];}
		Data *getCompletionMessage(){return descs[3];}
		Data *getPerformance(){return descs[4];}
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
		Data *getIdent(){return descs[0];}
		Data *getPhase(){return descs[1];}
		Data *getWhen(){return descs[2];}
		Data *getCompletion(){return descs[3];}
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
		Data *getTimeout(){return descs[0];}
		Data *getProgram(){return descs[1];}
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
		Data *getTimeout(){return descs[0];}
		Data *getImage(){return descs[1];}
		Data *getRoutine(){return descs[2];}
		Data *getArgumentAt(int idx){return descs[3 + idx];}
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
		Data *getTimeout(){return descs[0];}
		Data *getLanguage(){return descs[1];}
		Data *getProcedure(){return descs[2];}
		Data *getArgumentAt(int idx){return descs[3 + idx];}
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
		Data *getTimeout(){return descs[0];}
		Data *getMethod(){return descs[1];}
		Data *getObject(){return descs[2];}
		Data *getArgumentAt(int idx){return descs[3 + idx];}
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
		Data *getArg1(){return descs[0];}
		Data *getArg2(){return descs[1];}
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
		Data *getArg(){return descs[0];}
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
		Call(Data *image, Data *routine, int nargs, Data **args, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			dtype = DTYPE_CALL;
			length = 0;
			ptr = 0;
			nDescs = 2 + nargs;
			descs = new Data *[nDescs];
			descs[0] = image;
			descs[1] = routine;
			for(int i = 0; i < nargs; i++)
				descs[2+i] = args[i];
			incrementRefCounts();
		}
		Data *getImage(){return descs[0];}
		Data *getRoutine(){return descs[1];}
		Data *getArgumentAt(int idx){return descs[2 + idx];}
		void setImage(Data *image){assignDescAt(image, 0);}
		void setRoutine(Data *routine) {assignDescAt(routine, 1);}
		void setArgumentAt(Data *argument, int idx) {assignDescAt(argument, 2 + idx);}
	};

/////////////////////APD///////////////////////
	class Apd: public Data
	{
	protected:
		int nDescs;
		Data **descs;
		
	public:
		Apd(Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0) 
		{
			clazz = CLASS_APD; 
			dtype = DTYPE_DSC; 
			nDescs = 0;
			setAccessory(units, error, help, validation);
		}
		Apd(int nData, Data **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
		{
			clazz = CLASS_APD; 
			dtype = DTYPE_DSC; 
			nDescs = nData;
			this->descs = new Data *[nData];
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
			if(nDescs > 0)
			{
				delete [] descs;
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

		void *convertToDsc()
		{
			return completeConversionToDsc(convertToApdDsc(nDescs, (void **)descs));
		}
		int getDimension() {return nDescs;}
		Data **getDscArray() {return descs;}
		Data *getDescAt(int idx) {return descs[idx];}
		void setDescAt(int idx, Data *data)
		{
			descs[idx] = data;
			data->refCount++;
			changed = true;
		}
	};





}




	
	
#endif
