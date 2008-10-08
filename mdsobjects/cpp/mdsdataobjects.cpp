#include "mdsdata.h"
//#include "mdstree.h"
using namespace MDSobjects;


//////Wrapper functions called by C code to build a Data class instance from a MDSplus descriptor///////////

extern "C" void *convertDataToDsc(void *data)
{
	return ((Data *)data)->convertToDsc();
}

extern "C" void *createScalarData(int dtype, int length, char *ptr, Data *unitsData, Data *errorData, 
								  Data *helpData, Data *validationData)
{
	switch(dtype) {
		case DTYPE_B: return new Int8(*ptr, unitsData, errorData, helpData, validationData);
		case DTYPE_BU: return new Uint8(*(unsigned char *)ptr, unitsData, errorData, helpData, validationData);
		case DTYPE_W: return new Int16(*(short *)ptr, unitsData, errorData, helpData, validationData);
		case DTYPE_WU: return new Uint16(*(unsigned short *)ptr, unitsData, errorData, helpData, validationData);
		case DTYPE_L: return new Int32(*(int *)ptr, unitsData, errorData, helpData, validationData);
		case DTYPE_LU: return new Uint32(*(int *)ptr, unitsData, errorData, helpData, validationData);
		case DTYPE_Q: return new Int64(*(_int64 *)ptr, unitsData, errorData, helpData, validationData);
		case DTYPE_QU: return new Uint64(*(_int64 *)ptr, unitsData, errorData, helpData, validationData);
		case DTYPE_FLOAT: return new Float32(*(float *)ptr, unitsData, errorData, helpData, validationData);
		case DTYPE_DOUBLE: return new Float64(*(double *)ptr, unitsData, errorData, helpData, validationData);
		case DTYPE_T: return new String(ptr, length, unitsData, errorData, helpData, validationData);
		case DTYPE_NID: return new Nid(*(int *)ptr, unitsData, errorData, helpData, validationData);
		case DTYPE_PATH: return new Path(ptr, length, unitsData, errorData, helpData, validationData);
		case DTYPE_EVENT: return new Event(ptr, length, unitsData, errorData, helpData, validationData);
	}
	return 0;
}


extern "C" void *createArrayData(int dtype, int length, int nDims, int *dims, char *ptr, 
								 Data *unitsData, Data *errorData, Data *helpData, Data *validationData)
{
	int revDims[MAX_ARGS];
	for(int i = 0; i < nDims; i++)
		revDims[i] = dims[nDims - i - 1];
	switch(dtype) {
		case DTYPE_B: return new Int8Array(ptr, nDims, revDims, unitsData, errorData, helpData, validationData);
		case DTYPE_BU: return new Uint8Array(ptr, nDims, revDims, unitsData, errorData, helpData, validationData);
		case DTYPE_W: return new Int16Array((short *)ptr, nDims, revDims, unitsData, errorData, helpData, validationData);
		case DTYPE_WU: return new Uint16Array((short *)ptr, nDims, revDims, unitsData, errorData, helpData, validationData);
		case DTYPE_L: return new Int32Array((int *)ptr, nDims, revDims, unitsData, errorData, helpData, validationData);
		case DTYPE_LU: return new Uint32Array((int *)ptr, nDims, revDims, unitsData, errorData, helpData, validationData);
		case DTYPE_Q: return new Int64Array((_int64 *)ptr, nDims, revDims, unitsData, errorData, helpData, validationData);
		case DTYPE_QU: return new Uint64Array((_int64 *)ptr, nDims, revDims, unitsData, errorData, helpData, validationData);
		case DTYPE_FLOAT: return new Float32Array((float *)ptr, nDims, revDims, unitsData, errorData, helpData, validationData);
		case DTYPE_DOUBLE: return new Float64Array((double *)ptr, nDims, revDims, unitsData, errorData, helpData, validationData);
	}
	return 0;
}

extern "C" void *createCompoundData(int dtype, int length, char *ptr, int nDescs, char **descs, 
									Data *unitsData, Data *errorData, Data *helpData, Data *validationData)
{
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

extern "C" void *createApdData(int nData, char **dataPtrs, Data *unitsData, 
							   Data *errorData, Data *helpData, Data *validationData)
{
	return new Apd(nData, (Data **) dataPtrs, unitsData, errorData, helpData, validationData);
}



extern "C" void * convertToByte(void *dsc); 
extern "C" void * convertToShort(void *dsc); 
extern "C" void * convertToInt(void *dsc); 
extern "C" void * convertToLong(void *dsc); 
extern "C" void * convertToFloat(void *dsc); 
extern "C" void * convertToDouble(void *dsc); 
extern "C" void * convertToShape(void *dcs);
extern "C" void * convertToParameter(void *dcs, void *helpDsc, void *validationDsc);
extern "C" void * convertToUnits(void *dcs, void *unitsDsc);
extern "C" void * convertToError(void *dcs, void *errorDsc);

////////////////////Data methods implementation////////////////////////
Data *Data::data()
{
	if(!hasChanged() && !dataCache)
	{
		printf("FATAL: has changed and no data cached\n");
		exit(0);
	}
	if(!hasChanged())
		return dataCache->clone();

	void *dscPtr = convertToDsc();
	void *evalPtr = evaluateData(dscPtr, 0);
	if(dataCache)
		deleteData(dataCache);
	dataCache = (Data *)convertFromDsc(evalPtr);
	freeDsc(dscPtr);
	freeDsc(evalPtr);
	changed = false;
	return dataCache->clone();
}
Data *Data::evaluate()
{
	void *dscPtr = convertToDsc();
	void *evalPtr = evaluateData(dscPtr, 1);
	Data *retData = (Data *)convertFromDsc(evalPtr);
	freeDsc(dscPtr);
	freeDsc(evalPtr);
	return retData;
}


char *Data::decompile()
{
	void *dscPtr = convertToDsc();
	char *dec = decompileDsc(dscPtr);
	char *retStr = new char[strlen(dec)+1];
	strcpy(retStr, dec);
	freeChar(dec);
	return retStr;
}

//Make a dymanically allocated copy of the Data instance Tee
Data *Data::clone()
{
	void *dscPtr = convertToDsc();
	Data *retData = (Data *)convertFromDsc(dscPtr);
	freeDsc(dscPtr);
	return retData;
}


char Data::getByte()
{
	void *dscPtr = convertToDsc();
	void *retDsc = convertToByte(dscPtr);
	Data *retData = (Data *)convertFromDsc(retDsc);
	if(!retData || retData->clazz != CLASS_S)
		throw new DataException(retData->clazz, retData->dtype, "Cannot convert to Byte");
	freeDsc(dscPtr);
	freeDsc(retDsc);
	
	char res = retData->getByte();
	deleteData(retData);
	return res;
}

short Data::getShort()
{
	void *dscPtr = convertToDsc();
	void *retDsc = convertToShort(dscPtr);
	Data *retData = (Data *)convertFromDsc(retDsc);
	if(!retData || retData->clazz != CLASS_S)
		throw new DataException(retData->clazz, retData->dtype, "Cannot convert to Short");
	freeDsc(dscPtr);
	freeDsc(retDsc);
	
	short res = retData->getShort();
	deleteData(retData);
	return res;
}

int Data::getInt()
{
	void *dscPtr = convertToDsc();
	void *retDsc = convertToInt(dscPtr);
	Data *retData = (Data *)convertFromDsc(retDsc);
	if(!retData || retData->clazz != CLASS_S)
		throw new DataException(retData->clazz, retData->dtype, "Cannot convert to Int");
	freeDsc(dscPtr);
	freeDsc(retDsc);
	
	int res = retData->getInt();
	deleteData(retData);
	return res;
}

_int64 Data::getLong()
{
	void *dscPtr = convertToDsc();
	void *retDsc = convertToLong(dscPtr);
	Data *retData = (Data *)convertFromDsc(retDsc);
	if(!retData || retData->clazz != CLASS_S)
		throw new DataException(retData->clazz, retData->dtype, "Cannot convert to Long");
	freeDsc(dscPtr);
	freeDsc(retDsc);
	
	_int64 res = retData->getLong();
	deleteData(retData);
	return res;
}

float Data::getFloat()
{
	void *dscPtr = convertToDsc();
	void *retDsc = convertToFloat(dscPtr);
	Data *retData = (Data *)convertFromDsc(retDsc);
	if(!retData || retData->clazz != CLASS_S)
		throw new DataException(retData->clazz, retData->dtype, "Cannot convert to Float");
	freeDsc(dscPtr);
	freeDsc(retDsc);
	
	float res = retData->getFloat();
	deleteData(retData);
	return res;
}

double Data::getDouble()
{
	void *dscPtr = convertToDsc();
	void *retDsc = convertToDouble(dscPtr);
	Data *retData = (Data *)convertFromDsc(retDsc);
	if(!retData || retData->clazz != CLASS_S)
		throw new DataException(retData->clazz, retData->dtype, "Cannot convert to Double");
	freeDsc(dscPtr);
	freeDsc(retDsc);
	
	double res = retData->getDouble();
	deleteData(retData);
	return res;
}

int * Data::getShape(int *numDim)
{
	void *dscPtr = convertToDsc();
	void *retDsc = convertToShape(dscPtr);
	Data *retData = (Data *)convertFromDsc(retDsc);
	if(!retData || retData->clazz != CLASS_S)
		throw new DataException(retData->clazz, retData->dtype, "Cannot compute shape");
	freeDsc(dscPtr);
	freeDsc(retDsc);
	
	int *res = retData->getIntArray(numDim);
	deleteData(retData);
	return res;
}

char * Data::getByteArray(int *numElements)
{
	void *dscPtr = convertToDsc();
	void *retDsc = convertToByte(dscPtr);
	Data *retData = (Data *)convertFromDsc(retDsc);
	if(!retData || retData->clazz != CLASS_A)
		throw new DataException(retData->clazz, retData->dtype, "Cannot convert to Byte Array");
	freeDsc(dscPtr);
	freeDsc(retDsc);
	
	char *res = retData->getByteArray(numElements);
	deleteData(retData);
	return res;
}

short * Data::getShortArray(int *numElements)
{
	void *dscPtr = convertToDsc();
	void *retDsc = convertToShort(dscPtr);
	Data *retData = (Data *)convertFromDsc(retDsc);
	if(!retData || retData->clazz != CLASS_A)
		throw new DataException(retData->clazz, retData->dtype, "Cannot convert to Short Array");
	freeDsc(dscPtr);
	freeDsc(retDsc);
	
	short *res = retData->getShortArray(numElements);
	deleteData(retData);
	return res;
}

int * Data::getIntArray(int *numElements)
{
	void *dscPtr = convertToDsc();
	void *retDsc = convertToInt(dscPtr);
	Data *retData = (Data *)convertFromDsc(retDsc);
	if(!retData || retData->clazz != CLASS_A)
		throw new DataException(retData->clazz, retData->dtype, "Cannot convert to Int Array");
	freeDsc(dscPtr);
	freeDsc(retDsc);
	
	int *res = retData->getIntArray(numElements);
	deleteData(retData);
	return res;
}

_int64 * Data::getLongArray(int *numElements)
{
	void *dscPtr = convertToDsc();
	void *retDsc = convertToLong(dscPtr);
	Data *retData = (Data *)convertFromDsc(retDsc);
	if(!retData || retData->clazz != CLASS_A)
		throw new DataException(retData->clazz, retData->dtype, "Cannot convert to Long Array");
	freeDsc(dscPtr);
	freeDsc(retDsc);
	
	_int64 *res = retData->getLongArray(numElements);
	deleteData(retData);
	return res;
}

float * Data::getFloatArray(int *numElements)
{
	void *dscPtr = convertToDsc();
	void *retDsc = convertToFloat(dscPtr);
	Data *retData = (Data *)convertFromDsc(retDsc);
	if(!retData || retData->clazz != CLASS_A)
		throw new DataException(retData->clazz, retData->dtype, "Cannot convert to Float Array");
	freeDsc(dscPtr);
	freeDsc(retDsc);
	
	float *res = retData->getFloatArray(numElements);
	deleteData(retData);
	return res;
}

double * Data::getDoubleArray(int *numElements)
{
	void *dscPtr = convertToDsc();
	void *retDsc = convertToDouble(dscPtr);
	Data *retData = (Data *)convertFromDsc(retDsc);
	if(!retData || retData->clazz != CLASS_A)
		throw new DataException(retData->clazz, retData->dtype, "Cannot convert to Double Array");
	freeDsc(dscPtr);
	freeDsc(retDsc);
	
	double *res = retData->getDoubleArray(numElements);
	deleteData(retData);
	return res;
}


//Complete Conversion to Dsc by condsidering help, units and error

void * Data::completeConversionToDsc(void *dsc)
{
	void *retDsc = dsc;


	if(help || validation)
		retDsc = convertToParameter(retDsc, (help)?help->convertToDsc():0, (validation)?validation->convertToDsc():0); 
	if(error)
		retDsc = convertToError(retDsc, error->convertToDsc());
	if(units)
		retDsc = convertToUnits(retDsc, units->convertToDsc());
	return retDsc;
}


//Controlled deletion of dynamic data
EXPORT void MDSobjects::deleteData(Data *data)
{
	if(!data) return;
	data->refCount--;
	if(data->refCount <= 0)
	{
		if(data->dataCache)
			deleteData(data->dataCache);
		if(data->error)
			deleteData(data->error);
		if(data->units)
			deleteData(data->units);
		if(data->help)
			deleteData(data->help);
		if(data->validation)
		deleteData(data->validation);
		data->error = data->units = data->help = data->validation = 0;
		data->propagateDeletion();
		delete data;
	}
}


//Array Methods setElementAt and getElementAt
Data * Array::getElementAt(int *getDims, int getNumDims)
{
	int i;

	//Check Dimensionality
	if(getNumDims > nDims)
		throw new DataException(clazz, dtype, "Invalid passed dimensions in Array::getElementAt"); 
	for(i = 0; i < getNumDims; i++)
	{
		if(getDims[i] < 0 || getDims[i] >= dims[i])
			throw new DataException(clazz, dtype, "Invalid passed dimensions in Array::getElementAt"); 
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

	delete [] rowDims;

	if(getNumDims == nDims) //return a scalar
		return (Data *)createScalarData(dtype, length, ptr+(startIdx * length), 0,0,0,0);
	//Otherwise return an array
	return (Data *)createArrayData(dtype, length, nDims - getNumDims, &dims[getNumDims], ptr+(startIdx * length), 0,0,0,0);
}


void Array::setElementAt(int *getDims, int getNumDims, Data *data)
{
	int i;

	Array *arrayData;
	Scalar *scalarData;

	if((data->dtype != dtype) || (data->clazz != CLASS_S && data->clazz != CLASS_A))
		throw new DataException(clazz, dtype, "Invalid data type in Array::setElementAt"); 
	//Check Dimensionality
	if(getNumDims > nDims)
		throw new DataException(clazz, dtype, "Invalid passed dimensions in Array::getElementAt"); 
	for(i = 0; i < getNumDims; i++)
	{
		if(getDims[i] < 0 || getDims[i] >= dims[i])
			throw new DataException(clazz, dtype, "Invalid passed dimensions in Array::getElementAt"); 
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
			throw new DataException(clazz, dtype, "Invalid passed dimensions in Array::setElementAt"); 
		}
		for(i = 0; i < arrayData->nDims; i++)
		{
			if(arrayData->dims[i] != dims[getNumDims + i])
			{
				delete [] rowDims;
				throw new DataException(clazz, dtype, "Invalid passed dimensions in Array::setElementAt"); 
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
	changed = true;

}




