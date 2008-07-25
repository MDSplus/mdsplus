#ifndef MDSDATA_H
#define MDSDATA_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <iostream.h>
#include <exception>

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
typedef unsigned long long _int64;
#define EXPORT
#else
#define EXPORT __declspec(dllexport)
#endif

extern "C" {
	void *convertToScalarDsc(int clazz, int dtype, int length, char *ptr);
	void *evaluateData(void *dscPtr);
	void freeDsc(void *dscPtr);
	void *convertFromDsc(void *dscPtr);
	char *decompileDsc(void *dscPtr);
	char *decompileDsc(void *dscPtr);
	void *compileFromExpr(char *expr);
	void *compileFromExprWithArgs(char *expr, int nArgs, void *args);
	void freeChar(void *);
	void *convertToArrayDsc(int clazz, int dtype, int length, int l_length, int nDims, int *dims, void *ptr);
	void *convertToCompoundDsc(int clazz, int dtype, int length, void *ptr, int ndescs, void **descs);
	void *convertToApdDsc(int ndescs, void **ptr);
}



namespace mdsplus  {

	class DataException: public exception
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
				sprintf(message, "%s  Class = %d Dtype = %d", message, clazz, dtype);
			}
		}
		virtual const char* what() const 
		{
			return message;
		}
		~DataException() {delete[] message;}
	};




	class Data {
	friend Data *compileDataWithArgs(char *expr, ...);

	protected: 
		int clazz, dtype;
	public:
		int refCount;
		virtual void *convertToDsc() = 0;
		Data(){refCount = 0;}
		virtual ~Data()
		{
			//printf("DELETED\n");
		}
		int getClass() { return clazz; }
		int getDtype() { return dtype;}
		bool isArray() 
		{
			if(clazz == CLASS_A) return true;
			return false;
		}
		bool isScalar() 
		{
			if(clazz == CLASS_S) return true;
			return false;
		}
		Data *evaluate()
		{
			void *dscPtr = convertToDsc();
			void *evalPtr = evaluateData(dscPtr);
			Data *retData = (Data *)convertFromDsc(evalPtr);
			freeDsc(dscPtr);
			freeDsc(evalPtr);
		}
		char *decompile()
		{
			void *dscPtr = convertToDsc();
			char *dec = decompileDsc(dscPtr);
			char *retStr = new char[strlen(dec)+1];
			strcpy(retStr, dec);
			freeChar(dec);
			return retStr;
		}
		virtual char getByte() 
		{
			throw new DataException(clazz, dtype, "getByte not supported for this class"); 
			return 0;
		}
		virtual short getShort()
		{
			throw new DataException(clazz, dtype, "getShort not supported for this class"); 
			return 0;
		}
		virtual int getInt()
		{
			throw new DataException(clazz, dtype, "getInt not supported for this class"); 
			return 0;
		}

		virtual _int64 getLong()
		{
			throw new DataException(clazz, dtype, "getLong not supported for this class"); 
			return 0;
		}

		virtual float getFloat() 
		{
			throw new DataException(clazz, dtype, "getFloat not supported for this class"); 
			return 0;
		}

		virtual double getDouble() 
		{
			throw new DataException(clazz, dtype, "getDouble not supported for this class"); 
			return 0;
		}
		virtual char * getString() 
		{
			throw new DataException(clazz, dtype, "getString not supported for this class"); 
			return 0;
		}

		virtual void *getArray() 
		{
			throw new DataException(clazz, dtype, "getArray not supported for this class"); 
			return 0;
		}

		virtual int getNumDims()
		{
			throw new DataException(clazz, dtype, "getNumDims not supported for this class"); 
			return 0;
		}

		virtual void getDims(int *dims)
		{
			throw new DataException(clazz, dtype, "getDims not supported for this class"); 
		}
		virtual char *getByteArray()
		{
			throw new DataException(clazz, dtype, "getByteArray not supported for this class"); 
			return 0;
		}
		virtual short *getShortArray()
		{
			throw new DataException(clazz, dtype, "getShortArray not supported for this class"); 
			return 0;
		}
		virtual int *getIntArray()
		{
			throw new DataException(clazz, dtype, "getIntArray not supported for this class"); 
			return 0;
		}
		virtual _int64 *getLongArray()
		{
			throw new DataException(clazz, dtype, "getLongArray not supported for this class"); 
			return 0;
		}




	};


	Data *compileData(char *expr)
	{
		return (Data *)compileFromExpr(expr);
	}

#define MAX_ARGS 512
	Data *compileDataWithArgs(char *expr, ...)
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



	class ScalarData: public Data
	{
	protected:
		int length;
		bool is_unsigned;
		char *ptr;
	public:
		~ScalarData()
		{
			delete [] ptr;
		}
		 
		void *convertToDsc()
		{
			return convertToScalarDsc(clazz, dtype, length, ptr);
		}
		int getLength() {return length; }
		bool isUnsigned() { return is_unsigned; }

	}; //ScalarData

	class ByteData : public ScalarData
	{
	public:
		ByteData(char val)
		{
			clazz = CLASS_S;
			dtype = DTYPE_B;
			length = 1;
			ptr = new char[1];
			ptr[0] = val;
			is_unsigned = false;
		}
		ByteData(char val, bool is_unsigned)
		{
			clazz = CLASS_S;
			dtype = (is_unsigned)?DTYPE_BU:DTYPE_B;
			length = 1;
			ptr = new char[1];
			ptr[0] = val;
			this->is_unsigned = is_unsigned;
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


	class ShortData : public ScalarData
	{
	public:
		ShortData(short val)
		{
			clazz = CLASS_S;
			dtype = DTYPE_W;
			length = 2;
			ptr = new char[2];
			*(short *)ptr = val;
			is_unsigned = false;
		}
		ShortData(short val, bool is_unsigned)
		{
			clazz = CLASS_S;
			dtype = (is_unsigned)?DTYPE_WU:DTYPE_W;
			length = 2;
			ptr = new char[2];
			*(short *)ptr = val;
			this->is_unsigned = is_unsigned;
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

	class IntData : public ScalarData
	{
	public:
		IntData(int val)
		{
			clazz = CLASS_S;
			dtype = DTYPE_L;
			length = sizeof(int);
			ptr = new char[sizeof(int)];
			*(int *)ptr = val;
			this->is_unsigned = false;
		}
		IntData(int val, bool is_unsigned)
		{
			clazz = CLASS_S;
			dtype = (is_unsigned)?DTYPE_LU:DTYPE_L;
			length = sizeof(int);
			ptr = new char[sizeof(int)];
			*(int *)ptr = val;
			this->is_unsigned = is_unsigned;
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

	class LongData : public ScalarData
	{
	public:
		LongData(_int64 val)
		{
			clazz = CLASS_S;
			dtype = DTYPE_Q;
			length = sizeof(_int64);
			ptr = new char[sizeof(_int64)];
			*(_int64 *)ptr = val;
			this->is_unsigned = false;
		}
		LongData(_int64 val, bool is_unsigned)
		{
			clazz = CLASS_S;
			dtype = (is_unsigned)?DTYPE_QU:DTYPE_Q;
			length = sizeof(_int64);
			ptr = new char[sizeof(_int64)];
			*(_int64 *)ptr = val;
			this->is_unsigned = is_unsigned;
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


	class LongLongData : public ScalarData
	{
	public:
		LongLongData(_int64 *val)
		{
			clazz = CLASS_S;
			dtype = DTYPE_O;
			length = sizeof(_int64);
			ptr = new char[2 * sizeof(_int64)];
			memcpy(ptr, val, 2 * sizeof(_int64));
			this->is_unsigned = false;
		}
		LongLongData(_int64 *val, bool is_unsigned)
		{
			clazz = CLASS_S;
			dtype = DTYPE_O;
			length = sizeof(_int64);
			ptr = new char[2 * sizeof(_int64)];
			memcpy(ptr, val, 2 * sizeof(_int64));
			this->is_unsigned = false;
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


	class FloatData : public ScalarData
	{
	public:
		FloatData(float val)
		{
			clazz = CLASS_S;
			dtype = DTYPE_FLOAT;
			length = sizeof(float);
			ptr = new char[sizeof(float)];
			*(float *)ptr = val;
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


	class NidData : public ScalarData
	{
	public:
		NidData(int val)
		{
			clazz = CLASS_S;
			dtype = DTYPE_NID;
			length = sizeof(int);
			ptr = new char[sizeof(int)];
			*(int *)ptr = val;
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

	class DoubleData : public ScalarData
	{
	public:
		DoubleData(double val)
		{
			clazz = CLASS_S;
			dtype = DTYPE_DOUBLE;
			length = sizeof(double);
			ptr = new char[sizeof(double)];
			*(double *)ptr = val;
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

	class StringData : public ScalarData
	{
	public:
		StringData(char *val)
		{
			clazz = CLASS_S;
			dtype = DTYPE_T;
			length = strlen(val);
			ptr = new char[length];
			memcpy(ptr, val, length);
		}
		StringData(char *val, int len)
		{
			clazz = CLASS_S;
			dtype = DTYPE_T;
			length = len;
			ptr = new char[length];
			memcpy(ptr, val, length);
		}
		char *getString() 
		{
			char *retStr = new char[length + 1];
			memcpy(retStr, ptr, length);
			retStr[length] = 0;
			return retStr;
		}
	};
	class PathData : public ScalarData
	{
	public:
		PathData(char *val)
		{
			clazz = CLASS_S;
			dtype = DTYPE_PATH;
			length = strlen(val);
			ptr = new char[length];
			memcpy(ptr, val, length);
		}
		PathData(char *val, int len)
		{
			clazz = CLASS_S;
			dtype = DTYPE_PATH;
			length = len;
			ptr = new char[length];
			memcpy(ptr, val, length);
		}
		char *getString() 
		{
			char *retStr = new char[length + 1];
			memcpy(retStr, ptr, length);
			retStr[length] = 0;
			return retStr;
		}
	};
	class EventData : public ScalarData
	{
	public:
		EventData(char *val)
		{
			clazz = CLASS_S;
			dtype = DTYPE_EVENT;
			length = strlen(val);
			ptr = new char[length];
			memcpy(ptr, val, length);
		}
		EventData(char *val, int len)
		{
			clazz = CLASS_S;
			dtype = DTYPE_EVENT;
			length = len;
			ptr = new char[length];
			memcpy(ptr, val, length);
		}
		char *getString() 
		{
			char *retStr = new char[length + 1];
			memcpy(retStr, ptr, length);
			retStr[length] = 0;
			return retStr;
		}
	};

////////////////ARRAYS/////////////////////

	class ArrayData: public Data
	{
	protected:
		int length;
		int arsize;
		int nDims;
		int dims[MAX_DIMS];
		char *ptr;
		bool is_unsigned;

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
		ArrayData() {clazz = CLASS_A;}
		~ArrayData()
		{
			delete [] ptr;
		}
		 
		void *convertToDsc()
		{
			return convertToArrayDsc(clazz, dtype, length, arsize, nDims, dims, ptr);
		}
		void *getArray() {return ptr;}
		int getNumDims() {return nDims;}
		void getDims(int *dims) {memcpy(dims, this->dims, sizeof(int) * nDims); }
		char *getByteArray()
		{
			char *retArr = new char[arsize];
			for(int i = 0; i < arsize; i++)
				retArr[i] = *(char *)&ptr[i * length];
			return retArr;
		}
		short *getShortArray()
		{
			short *retArr = new short[arsize];
			int size = arsize/length;
			for(int i = 0; i < size; i++)
				retArr[i] = *(short *)&ptr[i * length];
			return retArr;
		}
		int *getIntArray()
		{
			int *retArr = new int[arsize];
			int size = arsize/length;
			for(int i = 0; i < size; i++)
				retArr[i] = *(int *)&ptr[i * length];
			return retArr;
		}
		_int64 *getLongArray()
		{
			_int64 *retArr = new _int64[arsize];
			int size = arsize/length;
			for(int i = 0; i < size; i++)
				retArr[i] = *(_int64 *)&ptr[i * length];
			return retArr;
		}
		float *getFloatArray()
		{
			float *retArr = new float[arsize];
			int size = arsize/length;
			for(int i = 0; i < size; i++)
				retArr[i] = *(float *)&ptr[i * length];
			return retArr;
		}
		double *getDoubleArray()
		{
			double *retArr = new double[arsize];
			int size = arsize/length;
			for(int i = 0; i < size; i++)
				retArr[i] = *(double *)&ptr[i * length];
			return retArr;
		}

	};
	class ByteArray: public ArrayData
	{
	public:
		ByteArray(char *data, int nData, bool is_unsigned = false)
		{
			setSpecific(data, 1, DTYPE_B, nData);
			this->is_unsigned = is_unsigned;
		}
		ByteArray(char *data, int nDims, int *dims, bool is_unsigned = false)
		{
			setSpecific(data, 1, DTYPE_B, nDims, dims);
			this->is_unsigned = is_unsigned;
		}
	};

	class ShortArray: public ArrayData
	{
	public:
		ShortArray(short *data, int nData, bool is_unsigned = false)
		{
			setSpecific((char *)data, 2, DTYPE_W, nData);
			this->is_unsigned = is_unsigned;
		}
		ShortArray(short *data, int nDims, int *dims, bool is_unsigned = false)
		{
			setSpecific((char *)data, 2, DTYPE_W, nDims, dims);
			this->is_unsigned = is_unsigned;
		}
	};
	class IntArray: public ArrayData
	{
	public:
		IntArray(int *data, int nData, bool is_unsigned = false)
		{
			setSpecific((char *)data, sizeof(int), DTYPE_L, nData);
			this->is_unsigned = is_unsigned;
		}
		IntArray(int *data, int nDims, int *dims, bool is_unsigned = false)
		{
			setSpecific((char *)data, sizeof(int), DTYPE_L, nDims, dims);
			this->is_unsigned = is_unsigned;
		}
	};
	class LongArray: public ArrayData
	{
	public:
		LongArray(_int64 *data, int nData, bool is_unsigned = false)
		{
			setSpecific((char *)data, 8, DTYPE_Q, nData);
			this->is_unsigned = is_unsigned;
		}
		LongArray(_int64 *data, int nDims, int *dims, bool is_unsigned = false)
		{
			setSpecific((char *)data, 8, DTYPE_Q, nDims, dims);
			this->is_unsigned = is_unsigned;
		}
	};
	class FloatArray: public ArrayData
	{
	public:
		FloatArray(float *data, int nData)
		{
			setSpecific((char *)data, sizeof(float), DTYPE_FLOAT, nData);
		}
		FloatArray(float *data, int nDims, int *dims)
		{
			setSpecific((char *)data, sizeof(float), DTYPE_FLOAT, nDims, dims);
		}
	};
	class DoubleArray: public ArrayData
	{
	public:
		DoubleArray(double *data, int nData)
		{
			setSpecific((char *)data, sizeof(double), DTYPE_DOUBLE, nData);
		}
		DoubleArray(double *data, int nDims, int *dims)
		{
			setSpecific((char *)data, sizeof(double), DTYPE_Q, nDims, dims);
		}
	};
	class StringArray: public ArrayData
	{
	public:
		StringArray(char **data, int nData)
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
		}
	};

/////////////////////////COMPOUND DATA//////////////////////////////////
	class CompoundData: public Data
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
				descs[idx]->refCount--;
				if(descs[idx]->refCount == 0)
					delete descs[idx];
			}
			descs[idx] = data;
			if(data) data->refCount++;
		}


	public:
		CompoundData() {clazz = CLASS_R;}
		CompoundData(int dtype, int length, char *ptr, int nDescs, char **descs)
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
					this->descs[i]->refCount++;
				}
			}
			else
				this->descs = 0;
		}

		virtual ~CompoundData()
		{
			if(length > 0)
				delete ptr;
			for(int i = 0; i < nDescs; i++)
			{
				if(descs[i])
				{
					descs[i]->refCount--;
					if(descs[i]->refCount == 0)
						delete descs[i];
				}
			}
		}
			
		void *convertToDsc()
		{
			return convertToCompoundDsc(clazz, dtype, length, ptr, nDescs, (void **)descs);
		}
	};
	class ParamData: public CompoundData
	{
	public: 
		ParamData(int dtype, int length, char *ptr, int nDescs, char **descs):CompoundData(dtype, length, ptr, nDescs, descs)
		{}
		ParamData(Data *value, Data *help, Data *validation)
		{
			dtype = DTYPE_PARAM;
			length = 0;
			ptr = 0;
			nDescs = 3;
			descs = new Data *[3];
			descs[0] = value;
			descs[1] = help;
			descs[2] = validation;
			incrementRefCounts();
		}
		Data *getValue(){return descs[0];}
		Data *getHelp(){return descs[1];}
		Data *getValidation(){return descs[2];}
		void setValue(Data *value) {assignDescAt(value, 0);}
		void setHelp(Data *help){assignDescAt(help, 1);}
		void setValidation(Data *validation) {assignDescAt(validation, 2);}
	};


	class SignalData: public CompoundData
	{
	public: 
		SignalData(int dtype, int length, char *ptr, int nDescs, char **descs):CompoundData(dtype, length, ptr, nDescs, descs)
		{}
		SignalData(Data *data, Data *raw, Data *dimension)
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
		}
		SignalData(Data *data, Data *raw, Data *dimension1, Data *dimension2)
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
		}
		SignalData(Data *data, Data *raw, int nDims, Data **dimensions)
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

	class DimensionData: public CompoundData
	{
	public: 
		DimensionData(int dtype, int length, char *ptr, int nDescs, char **descs):CompoundData(dtype, length, ptr, nDescs, descs)
		{}
		DimensionData(Data *window, Data *axis)
		{
			dtype = DTYPE_DIMENSION;
			length = 0;
			ptr = 0;
			nDescs = 2;
			descs = new Data *[2];
			descs[0] = window;
			descs[1] = axis;
			incrementRefCounts();
		}
		Data *getWindow(){return descs[0];}
		Data *getAxis(){return descs[1];}
		void setWindow(Data *window) {assignDescAt(window, 0);}
		void setAxis(Data *axis){assignDescAt(axis, 1);}
	};


	class WindowData: public CompoundData
	{
	public: 
		WindowData(int dtype, int length, char *ptr, int nDescs, char **descs):CompoundData(dtype, length, ptr, nDescs, descs)
		{}
		WindowData(Data *startidx, Data *endidx, Data *value_at_idx0)
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
		}
		Data *getStartIdx(){return descs[0];}
		Data *getEndIdx(){return descs[1];}
		Data *getValueAt0(){return descs[2];}
		void setStartIdx(Data *startidx) {assignDescAt(startidx, 0);}
		void setEndIdx(Data *endidx){assignDescAt(endidx, 1);}
		void setValueAt0(Data *value_at_idx0) {assignDescAt(value_at_idx0, 2);}
	};


	class SlopeData: public CompoundData
	{
	public: 
		SlopeData(int dtype, int length, char *ptr, int nDescs, char **descs):CompoundData(dtype, length, ptr, nDescs, descs)
		{}
		SlopeData(Data *slope, Data *begin, Data *ending)
		{
			dtype = DTYPE_SLOPE;
			length = 0;
			ptr = 0;
			nDescs = 3;
			descs = new Data *[3];
			descs[0] = slope;
			descs[1] = begin;
			descs[2] = ending;
			incrementRefCounts();
		}
		Data *getSlope(){return descs[0];}
		Data *getBegin(){return descs[1];}
		Data *getEnding(){return descs[2];}
		void setSlope(Data *slope) {assignDescAt(slope, 0);}
		void setEnding(Data *begin){assignDescAt(begin, 1);}
		void setValueAt0(Data *ending) {assignDescAt(ending, 2);}
	};

	class FunctionData: public CompoundData
	{
	public: 
		FunctionData(int dtype, int length, char *ptr, int nDescs, char **descs):CompoundData(dtype, length, ptr, nDescs, descs)
		{}
		FunctionData(char opcode, int nargs, Data **args)
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
		}
		char getOpcode() { return *ptr;}
		int getNumArguments() { return nDescs;}
		Data *getArgumentAt(int idx) {return descs[idx];}
		void setArgAt(Data *arg, int idx) {assignDescAt(arg, idx);}
	};

	class ConglomData: public CompoundData
	{
	public: 
		ConglomData(int dtype, int length, char *ptr, int nDescs, char **descs):CompoundData(dtype, length, ptr, nDescs, descs)
		{}
		ConglomData(Data *image, Data *model, Data *name, Data *qualifiers)
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

	class RangeData: public CompoundData
	{
	public: 
		RangeData(int dtype, int length, char *ptr, int nDescs, char **descs):CompoundData(dtype, length, ptr, nDescs, descs)
		{}
		RangeData(Data *begin, Data *ending, Data *deltaval)
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
		}
		Data *getBegin(){return descs[0];}
		Data *getEnding(){return descs[1];}
		Data *getDeltaVal(){return descs[2];}
		void setBegin(Data *begin) {assignDescAt(begin, 0);}
		void setEnding(Data *ending){assignDescAt(ending, 1);}
		void setDeltaVal(Data *deltaval) {assignDescAt(deltaval, 2);}
	};

	class ActionData: public CompoundData
	{
	public: 
		ActionData(int dtype, int length, char *ptr, int nDescs, char **descs):CompoundData(dtype, length, ptr, nDescs, descs)
		{}
		ActionData(Data *dispatch, Data *task, Data *errorlogs, Data *completion_message, Data *performance)
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

	class DispatchData: public CompoundData
	{
	public: 
		DispatchData(int dtype, int length, char *ptr, int nDescs, char **descs):CompoundData(dtype, length, ptr, nDescs, descs)
		{}
		DispatchData(Data *ident, Data *phase, Data *when, Data *completion)
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

	class ProgramData: public CompoundData
	{
	public: 
		ProgramData(int dtype, int length, char *ptr, int nDescs, char **descs):CompoundData(dtype, length, ptr, nDescs, descs)
		{}
		ProgramData(Data *timeout, Data *program)
		{
			dtype = DTYPE_PROGRAM;
			length = 0;
			ptr = 0;
			nDescs = 2;
			descs = new Data *[2];
			descs[0] = timeout;
			descs[1] = program;
			incrementRefCounts();
		}
		Data *getTimeout(){return descs[0];}
		Data *getProgram(){return descs[1];}
		void setTimeout(Data *timeout) {assignDescAt(timeout, 0);}
		void setProgram(Data *program){assignDescAt(program, 1);}
	};

	class RoutineData: public CompoundData
	{
	public: 
		RoutineData(int dtype, int length, char *ptr, int nDescs, char **descs):CompoundData(dtype, length, ptr, nDescs, descs)
		{}
		RoutineData(Data *timeout, Data *image, Data *routine, int nargs, Data **args)
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

	class ProcedureData: public CompoundData
	{
	public: 
		ProcedureData(int dtype, int length, char *ptr, int nDescs, char **descs):CompoundData(dtype, length, ptr, nDescs, descs)
		{}
		ProcedureData(Data *timeout, Data *language, Data *procedure, int nargs, Data **args)
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

	class MethodData: public CompoundData
	{
	public: 
		MethodData(int dtype, int length, char *ptr, int nDescs, char **descs):CompoundData(dtype, length, ptr, nDescs, descs)
		{}
		MethodData(Data *timeout, Data *method, Data *object, int nargs, Data **args)
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

	class DependencyData: public CompoundData
	{
	public: 
		DependencyData(int dtype, int length, char *ptr, int nDescs, char **descs):CompoundData(dtype, length, ptr, nDescs, descs)
		{}
		DependencyData(char opcode, Data *arg1, Data *arg2)
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

	class ConditionData: public CompoundData
	{
	public: 
		ConditionData(int dtype, int length, char *ptr, int nDescs, char **descs):CompoundData(dtype, length, ptr, nDescs, descs)
		{}
		ConditionData(char opcode, Data *arg)
		{
			dtype = DTYPE_CONDITION;
			length = 1;
			ptr = new char[1];
			*ptr = opcode;
			nDescs = 1;
			descs = new Data *[1];
			descs[0] = arg;
			incrementRefCounts();
		}
		char getOpcode(){return *ptr;}
		Data *getArg(){return descs[0];}
		void setOpcode(char opcode)
		{
			*ptr = opcode;
		}
		void setArg(Data *arg) {assignDescAt(arg, 0);}
	};

	class WithUnitsData: public CompoundData
	{
	public: 
		WithUnitsData(int dtype, int length, char *ptr, int nDescs, char **descs):CompoundData(dtype, length, ptr, nDescs, descs)
		{}
		WithUnitsData(Data *data, Data *units)
		{
			dtype = DTYPE_WITH_UNITS;
			length = 0;
			ptr = 0;
			nDescs = 2;
			descs = new Data *[2];
			descs[0] = data;
			descs[1] = units;
			incrementRefCounts();
		}
		Data *getData(){return descs[0];}
		Data *getUnits(){return descs[1];}
		void setData(Data *data) {assignDescAt(data, 0);}
		void setUnits(Data *units){assignDescAt(units, 1);}
	};

	class CallData: public CompoundData
	{
	public: 
		CallData(int dtype, int length, char *ptr, int nDescs, char **descs):CompoundData(dtype, length, ptr, nDescs, descs)
		{}
		CallData(Data *image, Data *routine, int nargs, Data **args)
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
	class WithErrorData: public CompoundData
	{
	public: 
		WithErrorData(int dtype, int length, char *ptr, int nDescs, char **descs):CompoundData(dtype, length, ptr, nDescs, descs)
		{}
		WithErrorData(Data *data, Data *error)
		{
			dtype = DTYPE_WITH_ERROR;
			length = 0;
			ptr = 0;
			nDescs = 2;
			descs = new Data *[2];
			descs[0] = data;
			descs[1] = error;
			incrementRefCounts();
		}
		Data *getData(){return descs[0];}
		Data *getError(){return descs[1];}
		void setData(Data *data) {assignDescAt(data, 0);}
		void setUnits(Data *error){assignDescAt(error, 1);}
	};

/////////////////////APD///////////////////////
	class ApdData: public Data
	{
	protected:
		int length;
		int arsize;
		Data **dataPtrs;
		
	public:
		ApdData() 
		{
			clazz = CLASS_APD; 
			dtype = DTYPE_DSC; 
			length = sizeof(void *); 
			arsize = 0;
		}
		ApdData(int nData, Data **dataPtrs)
		{
			clazz = CLASS_APD; 
			dtype = DTYPE_DSC; 
			length = sizeof(void *); 
			arsize = nData * length;
			this->dataPtrs = new Data *[nData];
			for(int i = 0; i < nData; i++)
			{
				if(dataPtrs[i])
					dataPtrs[i]->refCount++;
				this->dataPtrs[i] = dataPtrs[i];
			}
		}

		virtual ~ApdData()
		{
			if(arsize > 0)
			{
				int ndescs = arsize/length;
				for(int i = 0; i < ndescs; i++)
				{
					if(dataPtrs[i])
					{
						dataPtrs[i]->refCount--;
						if(dataPtrs[i]->refCount == 0)
							delete dataPtrs[i];
					}
				}
				delete [] dataPtrs;
			}
		}
		 
		void *convertToDsc()
		{
			return convertToApdDsc(arsize/sizeof(Data *), (void **)dataPtrs);
		}
		int getDimension() {return arsize/sizeof(void *);}
		Data **getDscArray() {return dataPtrs;}
	};





}




	
	
#endif