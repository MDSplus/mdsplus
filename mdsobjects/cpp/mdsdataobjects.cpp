#include "mdsobjects.h"
//#include "mdstree.h"
using namespace MDSplus;

#ifdef HAVE_WINDOWS_H
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#define MAX_ARGS 512



extern "C" {
	char * MdsGetMsg(int status);

	void *convertToScalarDsc(int clazz, int dtype, int length, char *ptr);
	void *evaluateData(void *dscPtr, int isEvaluate, int *retStatus);
	void freeDsc(void *dscPtr);
	void *convertFromDsc(void *dscPtr);
	char *decompileDsc(void *dscPtr);
	char *decompileDsc(void *dscPtr);
	void *compileFromExprWithArgs(const char *expr, int nArgs, void *args, void *tree, int *retStatus);
	void freeChar(void *);
	void *convertToArrayDsc(int clazz, int dtype, int length, int l_length, int nDims, int *dims, void *ptr);
	void *convertToCompoundDsc(int clazz, int dtype, int length, void *ptr, int ndescs, void **descs);
	void *convertToApdDsc(int type, int ndescs, void **ptr);
	char * serializeData(void *dsc, int *retSize, void **retDsc);
	void *deserializeData(char *serialized);

	void * convertToByte(void *dsc); 
	void * convertToShort(void *dsc); 
	void * convertToInt(void *dsc); 
	void * convertToLong(void *dsc); 
	void * convertToFloat(void *dsc); 
	void * convertToDouble(void *dsc); 
	void * convertToShape(void *dcs);
	void * convertToParameter(void *dsc, void *helpDsc, void *validationDsc);
	void * convertToUnits(void *dsc, void *unitsDsc);
	void * convertToError(void *dsc, void *errorDsc);

	void convertTimeToAscii(_int64 *timePtr, char *dateBuf, int bufLen, int *retLen);
}


//////Wrapper functions called by C code to build a Data class instance from a MDSplus descriptor///////////

extern "C" void *convertDataToDsc(void *data)
{
	return ((Data *)data)->convertToDsc();
}

extern "C" void *createScalarData(int dtype, int length, char *ptr, Data *unitsData, Data *errorData, 
								  Data *helpData, Data *validationData, Tree *tree)
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
		case DTYPE_NID: return new TreeNode(*(int *)ptr, tree, unitsData, errorData, helpData, validationData);
		case DTYPE_PATH: return new TreePath(ptr, length, tree, unitsData, errorData, helpData, validationData);
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
		case DTYPE_T: return new StringArray((char *)ptr, dims[0], length);
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
extern "C" void *createListData(int nData, char **dataPtrs, Data *unitsData, 
							   Data *errorData, Data *helpData, Data *validationData)
{
	return new List(nData, (Data **) dataPtrs, unitsData, errorData, helpData, validationData);
}
extern "C" void *createDictionaryData(int nData, char **dataPtrs, Data *unitsData, 
							   Data *errorData, Data *helpData, Data *validationData)
{
	return new Dictionary(nData, (Data **) dataPtrs, unitsData, errorData, helpData, validationData);
}


///////////////////Data methods implementation////////////////////////
EXPORT void *Data::operator new(size_t sz)
{
	return ::operator new(sz);
}
EXPORT void Data::operator delete(void *p)
{
	::operator delete(p);
}

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
	int retStatus;
	void *evalPtr = evaluateData(dscPtr, 0, &retStatus);
	if(!(retStatus & 1))
		throw new MdsException(MdsGetMsg(retStatus));

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
	int retStatus;
	void *evalPtr = evaluateData(dscPtr, 1, &retStatus);
	if(!(retStatus & 1))
		throw new MdsException(MdsGetMsg(retStatus));
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
	freeDsc(dscPtr);
	return retStr;
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
		throw new MdsException("Cannot compute shape");
	freeDsc(dscPtr);
	freeDsc(retDsc);
	
	int *res = retData->getIntArray(numDim);
	deleteData(retData);
	return res;
}

char *Data::getByteArray(int *numElements)
{
	void *dscPtr = convertToDsc();
	void *retDsc = convertToByte(dscPtr);
	Data *retData = (Data *)convertFromDsc(retDsc);
	if(!retData || retData->clazz != CLASS_A)
		throw new MdsException("Cannot convert to Byte Array");
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
		throw new MdsException("Cannot convert to Short Array");
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
		throw new MdsException("Cannot convert to Int Array");
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
		throw new MdsException("Cannot convert to Long Array");
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
		throw new MdsException("Cannot convert to Float Array");
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
		throw new MdsException("Cannot convert to Double Array");
	freeDsc(dscPtr);
	freeDsc(retDsc);
	
	double *res = retData->getDoubleArray(numElements);
	deleteData(retData);
	return res;
}

char *	Data::serialize(int *size)
{
	void *dscPtr = convertToDsc();
	void *retDsc;
	int retSize;
	char *serialized = serializeData(dscPtr, &retSize, &retDsc);
	if(!serialized)
	{
		freeDsc(dscPtr);
		throw new MdsException("Cannot serialize Data object");
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
	void *dscPtr = convertToDsc();
	void *retDsc = convertToByte(dscPtr);
	Data *retData = (Data *)convertFromDsc(retDsc);
	if(!retData || retData->clazz != CLASS_S)
		throw new MdsException("Cannot convert to Byte");
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
		throw new MdsException("Cannot convert to Short");
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
		throw new MdsException("Cannot convert to Int");
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
		throw new MdsException("Cannot convert to Long");
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
		throw new MdsException("Cannot convert to Float");
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
		throw new MdsException("Cannot convert to Double");
	freeDsc(dscPtr);
	freeDsc(retDsc);
	
	double res = retData->getDouble();
	deleteData(retData);
	return res;
}

Data *Data::getDimensionAt(int dimIdx)
{
	return executeWithArgs("DIM_OF($)", 1, this);
}


	EXPORT	Data *MDSplus::compile(const char *expr)
	{
		return compileWithArgs(expr, 0);
	}
	
		
	EXPORT	Data *MDSplus::compileWithArgs(const char *expr, int nArgs ...)
	{
		int i;
		void *args[MAX_ARGS];

		va_list v;
		va_start(v, nArgs);
		for(i = 0; i < nArgs; i++)
		{
			Data *currArg = va_arg(v, Data *);
			args[i] = currArg->convertToDsc();
		}
		int status;
		Data *res =  (Data *)compileFromExprWithArgs(expr, nArgs, (void *)args, getActiveTree(), &status);
		for(i = 0; i < nArgs; i++)
		    freeDsc(args[i]);
		if(!(status & 1))
			throw new MdsException(status);
		return res;

	}
	EXPORT	Data *MDSplus::compile(const char *expr, Tree *tree)
	{
		return  compileWithArgs(expr, tree, 0);
	}
	EXPORT	Data *MDSplus::compileWithArgs(const char *expr, Tree *tree, int nArgs ...)
	{
		int i;
		void *args[MAX_ARGS];

		va_list v;
		va_start(v, nArgs);
		for(i = 0; i < nArgs; i++)
		{
			Data *currArg = va_arg(v, Data *);
			args[i] = currArg->convertToDsc();
		}
		setActiveTree(tree);
		int status;
		Data *res = (Data *)compileFromExprWithArgs(expr, nArgs, (void *)args, tree, &status);
		for(i = 0; i < nArgs; i++)
		    freeDsc(args[i]);
		if(!(status & 1))
			throw new MdsException(status);
		return res;
	}

	EXPORT	Data *MDSplus::execute(const char *expr)
	{
		return executeWithArgs(expr, 0);
	}

	EXPORT	Data *MDSplus::executeWithArgs(const char *expr, int nArgs ...)
	{

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
		}catch(MdsException *exc){actTree = 0;}
		Data *compData = (Data *)compileFromExprWithArgs(expr, nArgs, (void *)args, actTree, &status);
		if(!(status & 1))
			throw new MdsException(status);
		Data *evalData = compData->data();
		deleteData(compData);
		for(int i = 0; i < nArgs; i++)
		    freeDsc(args[i]);
		if(actTree)
			delete(actTree);
		return evalData;
	}

	EXPORT	Data *MDSplus::execute(const char *expr, Tree *tree)
	{
		return executeWithArgs(expr, tree, 0);
	}
	EXPORT	Data *MDSplus::executeWithArgs(const char *expr, Tree *tree, int nArgs ...)
	{
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
			throw new MdsException(status);
		if(!compData)
		{
			char *msg = new char[20 + strlen(expr)];
			sprintf(msg, "Cannot compile %s", expr);
			MdsException *exc = new MdsException(msg);
			delete [] msg;
			throw exc;
		}
		Data *evalData = compData->data();
		deleteData(compData);
		for(int i = 0; i < nArgs; i++)
		    freeDsc(args[i]);
		return evalData;
	}

/*
EXPORT	Data *MDSplus::executeWithArgs(const char *expr, Data **dataArgs, int nArgs)
	{
		void *args[MAX_ARGS];
		int i;
		int actArgs = (nArgs < MAX_ARGS)?nArgs:MAX_ARGS;
		for(i = 0; i < actArgs; i++)
			args[nArgs++] = dataArgs[i]->convertToDsc();

		int status;
		Data *compData = (Data *)compileFromExprWithArgs(expr, actArgs, (void *)args, 0, &status);
		if(!(status & 1))
			throw new MdsException(status);
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

void * Data::completeConversionToDsc(void *dsc)
{
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


//Controlled deletion of dynamic data
EXPORT void MDSplus::deleteData(Data *data)
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
	int i;

	//Check Dimensionality
	if(getNumDims > nDims)
		throw new MdsException("Invalid passed dimensions in Array::getElementAt"); 
	for(i = 0; i < getNumDims; i++)
	{
		if(getDims[i] < 0 || getDims[i] >= dims[i])
			throw new MdsException("Invalid passed dimensions in Array::getElementAt"); 
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
		return (Data *)createScalarData(dtype, length, ptr+(startIdx * length), 0,0,0,0, 0);
	//Otherwise return an array
	return (Data *)createArrayData(dtype, length, nDims - getNumDims, &dims[getNumDims], ptr+(startIdx * length), 0,0,0,0);
}
void Data::plot()
{
		Data *dim = getDimensionAt(0);
		Scope *scope = new Scope("");
		scope->plot(this, dim);
		scope->show();
		delete scope;
		deleteData(dim);
}

void Array::setElementAt(int *getDims, int getNumDims, Data *data)
{
	int i;

	Array *arrayData;
	Scalar *scalarData;

	if((data->dtype != dtype) || (data->clazz != CLASS_S && data->clazz != CLASS_A))
		throw new MdsException("Invalid data type in Array::setElementAt"); 
	//Check Dimensionality
	if(getNumDims > nDims)
		throw new MdsException("Invalid passed dimensions in Array::getElementAt"); 
	for(i = 0; i < getNumDims; i++)
	{
		if(getDims[i] < 0 || getDims[i] >= dims[i])
			throw new MdsException("Invalid passed dimensions in Array::getElementAt"); 
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
			throw new MdsException("Invalid passed dimensions in Array::setElementAt"); 
		}
		for(i = 0; i < arrayData->nDims; i++)
		{
			if(arrayData->dims[i] != dims[getNumDims + i])
			{
				delete [] rowDims;
				throw new MdsException("Invalid passed dimensions in Array::setElementAt"); 
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
char *Array::getByteArray(int *numElements)
{
	int size = arsize/length;
	char *retArr = new char[arsize/length];
	for(int i = 0; i < size; i++)
	{
		switch(dtype) {
			case DTYPE_B: retArr[i] = *(char *)&ptr[i * length]; break;
			case DTYPE_BU: retArr[i] = *(unsigned char *)&ptr[i * length]; break;
			case DTYPE_W: retArr[i] = *(short *)&ptr[i * length]; break;
			case DTYPE_WU: retArr[i] = *(unsigned short *)&ptr[i * length]; break;
			case DTYPE_L: retArr[i] = *(int *)&ptr[i * length]; break;
			case DTYPE_LU: retArr[i] = *(unsigned int *)&ptr[i * length]; break;
			case DTYPE_Q: retArr[i] = *(_int64 *)&ptr[i * length]; break; 
			case DTYPE_QU: retArr[i] = *(_int64 *)&ptr[i * length]; break;
			case DTYPE_FLOAT: retArr[i] = (char)*(float *)&ptr[i * length]; break;
			case DTYPE_DOUBLE: retArr[i] = (char)*(double *)&ptr[i * length]; break;
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
			case DTYPE_B: retArr[i] = *(char *)&ptr[i * length]; break;
			case DTYPE_BU: retArr[i] = *(unsigned char *)&ptr[i * length]; break;
			case DTYPE_W: retArr[i] = *(short *)&ptr[i * length]; break;
			case DTYPE_WU: retArr[i] = *(unsigned short *)&ptr[i * length]; break;
			case DTYPE_L: retArr[i] = *(int *)&ptr[i * length]; break;
			case DTYPE_LU: retArr[i] = *(unsigned int *)&ptr[i * length]; break;
			case DTYPE_Q: retArr[i] = *(_int64 *)&ptr[i * length]; break; 
			case DTYPE_QU: retArr[i] = *(_int64 *)&ptr[i * length]; break;
			case DTYPE_FLOAT: retArr[i] = (short)*(float *)&ptr[i * length]; break;
			case DTYPE_DOUBLE: retArr[i] = (short)*(double *)&ptr[i * length]; break;
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
			case DTYPE_B: retArr[i] = *(char *)&ptr[i * length]; break;
			case DTYPE_BU: retArr[i] = *(unsigned char *)&ptr[i * length]; break;
			case DTYPE_W: retArr[i] = *(short *)&ptr[i * length]; break;
			case DTYPE_WU: retArr[i] = *(unsigned short *)&ptr[i * length]; break;
			case DTYPE_L: retArr[i] = *(int *)&ptr[i * length]; break;
			case DTYPE_LU: retArr[i] = *(unsigned int *)&ptr[i * length]; break;
			case DTYPE_Q: retArr[i] = *(_int64 *)&ptr[i * length]; break; 
			case DTYPE_QU: retArr[i] = *(_int64 *)&ptr[i * length]; break;
			case DTYPE_FLOAT: retArr[i] = (int)*(float *)&ptr[i * length]; break;
			case DTYPE_DOUBLE: retArr[i] = (int)*(double *)&ptr[i * length]; break;
		}
	*numElements = size;
	return retArr;
}
_int64 *Array::getLongArray(int *numElements)
{
	int size = arsize/length;
	_int64 *retArr = new _int64[size];
	for(int i = 0; i < size; i++)
		switch(dtype) {
			case DTYPE_B: retArr[i] = *(char *)&ptr[i * length]; break;
			case DTYPE_BU: retArr[i] = *(unsigned char *)&ptr[i * length]; break;
			case DTYPE_W: retArr[i] = *(short *)&ptr[i * length]; break;
			case DTYPE_WU: retArr[i] = *(unsigned short *)&ptr[i * length]; break;
			case DTYPE_L: retArr[i] = *(int *)&ptr[i * length]; break;
			case DTYPE_LU: retArr[i] = *(unsigned int *)&ptr[i * length]; break;
			case DTYPE_Q: retArr[i] = *(_int64 *)&ptr[i * length]; break; 
			case DTYPE_QU: retArr[i] = *(_int64 *)&ptr[i * length]; break;
			case DTYPE_FLOAT: retArr[i] = (_int64)*(float *)&ptr[i * length]; break;
			case DTYPE_DOUBLE: retArr[i] = (_int64)*(double *)&ptr[i * length]; break;
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
			case DTYPE_B: retArr[i] = *(char *)&ptr[i * length]; break;
			case DTYPE_BU: retArr[i] = *(unsigned char *)&ptr[i * length]; break;
			case DTYPE_W: retArr[i] = *(short *)&ptr[i * length]; break;
			case DTYPE_WU: retArr[i] = *(unsigned short *)&ptr[i * length]; break;
			case DTYPE_L: retArr[i] = *(int *)&ptr[i * length]; break;
			case DTYPE_LU: retArr[i] = *(unsigned int *)&ptr[i * length]; break;
			case DTYPE_Q: retArr[i] = *(_int64 *)&ptr[i * length]; break; 
			case DTYPE_QU: retArr[i] = *(_int64 *)&ptr[i * length]; break;
			case DTYPE_FLOAT: retArr[i] = *(float *)&ptr[i * length]; break;
			case DTYPE_DOUBLE: retArr[i] = *(double *)&ptr[i * length]; break;
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
			case DTYPE_B: retArr[i] = *(char *)&ptr[i * length]; break;
			case DTYPE_BU: retArr[i] = *(unsigned char *)&ptr[i * length]; break;
			case DTYPE_W: retArr[i] = *(short *)&ptr[i * length]; break;
			case DTYPE_WU: retArr[i] = *(unsigned short *)&ptr[i * length]; break;
			case DTYPE_L: retArr[i] = *(int *)&ptr[i * length]; break;
			case DTYPE_LU: retArr[i] = *(unsigned int *)&ptr[i * length]; break;
			case DTYPE_Q: retArr[i] = *(_int64 *)&ptr[i * length]; break; 
			case DTYPE_QU: retArr[i] = *(_int64 *)&ptr[i * length]; break;
			case DTYPE_FLOAT: retArr[i] = *(float *)&ptr[i * length]; break;
			case DTYPE_DOUBLE: retArr[i] = *(double *)&ptr[i * length]; break;
		}
	*numElements = size;
	return retArr;
}

char **Array::getStringArray(int *numElements)
{
printf("ARRAY GET STRING ARRAY\n");
	int size = arsize/length;
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
			default: throw new MdsException("Unsupported conversion to char ** array");

		}
	*numElements = size;
	return retArr;
}

char *Uint64::getDate()
{
	char dateBuf[512];
	int bufLen;
	convertTimeToAscii((_int64 *)ptr, dateBuf, 512, &bufLen);
	char *retDate = new char[bufLen+1];
	dateBuf[bufLen] = 0;
	strcpy(retDate, dateBuf);
	return retDate;
}


EXPORT void *TreePath::convertToDsc()
{
	return completeConversionToDsc(convertToScalarDsc(clazz, dtype, length, ptr));
}

EXPORT void *Array::convertToDsc()
{
	return completeConversionToDsc(convertToArrayDsc(clazz, dtype, length, arsize, nDims, dims, ptr));
}

EXPORT void *Compound::convertToDsc()
{
	return completeConversionToDsc(convertToCompoundDsc(clazz, dtype, length, ptr, nDescs, (void **)descs));
}
EXPORT void *Apd::convertToDsc()
{
	return completeConversionToDsc(convertToApdDsc(dtype, nDescs, (void **)descs));
}


EXPORT Data *MDSplus::deserialize(char *serialized)
{
	void *dscPtr = deserializeData(serialized);
	if(!dscPtr) throw new MdsException("Cannot build Data instance from serialized content");
	Data *retData = (Data *)convertFromDsc(dscPtr);
	freeDsc(dscPtr);
	return retData;
}

EXPORT Data *MDSplus::deserialize(Data *serializedData)
{
	Uint8Array *serializedArr = (Uint8Array *)serializedData;
	return deserialize(serializedArr->ptr);
}

ostream& operator<<(ostream& output, Data *data)
{
	output << data->decompile();
	return output;
}

EXPORT Data *Uint8Array::deserialize()
{
    return (Data *)deserializeData(ptr);
}

EXPORT void Scope::show()
{
	char expr[256];
	sprintf(expr, "JavaShowWindow(%d, %d, %d, %d, %d)", idx, x, y, width, height);
	Data *ris = execute(expr);
	deleteData(ris);
}
EXPORT Scope::Scope(char *name, int x, int y, int width, int height)
{
	char *expr = new char[64+strlen(name)];
	sprintf(expr, "JavaNewWindow(\"%s\", -1)", name);
	Data *ris = execute(expr);
	idx = ris->getInt();
	deleteData(ris);
	delete [] expr;
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	show();
}
EXPORT void Scope::plot(Data *x, Data *y , int row, int col, char *color)
{
	char expr[256];
	sprintf(expr, "JavaReplaceSignal(%d, $1, $2, %d, %d, \"%s\")", idx, row, col, color);
	Data *ris = executeWithArgs(expr, 2, x, y);
	deleteData(ris);
}
EXPORT void Scope::oplot(Data *x, Data *y , int row, int col, char *color)
{
	char expr[256];
	sprintf(expr, "JavaAddSignal(%d, $1, $2, %d, %d, \"%s\")", idx, row, col, color);
	Data *ris = executeWithArgs(expr, 2, x, y);
	deleteData(ris);
}

//Required in Windows Debug configuation to propely de-allocate native arrays and strings
EXPORT void MDSplus::deleteNativeArray(char *array){delete [] array;}
EXPORT void MDSplus::deleteNativeArray(short *array){delete [] array;}
EXPORT void MDSplus::deleteNativeArray(int *array){delete [] array;}
EXPORT void MDSplus::deleteNativeArray(long *array){delete [] array;}
EXPORT void MDSplus::deleteNativeArray(float *array){delete [] array;}
EXPORT void MDSplus::deleteNativeArray(double *array){delete [] array;}
EXPORT void MDSplus::deleteNativeArray(char **array){delete [] array;}
EXPORT void MDSplus::deleteNativeArray(Data **array){delete [] array;}
EXPORT void MDSplus::deleteString(char *str){delete[] str;}

