#include <stdio.h>
#include "mdsobjects.h"
#ifdef HAVE_WINDOWS_H
#include <Windows.h>
#else
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <semaphore.h>
#endif
using namespace MDSplus;

extern "C"  void *getManyObj(char *serializedIn);
extern "C"  void *putManyObj(char *serializedIn);
extern "C" void *compileFromExprWithArgs(char *expr, int nArgs, void *args, void *tree);
extern "C" int  SendArg(int sock, unsigned char idx, char dtype, unsigned char nargs, short length, char ndims,
int *dims, char *bytes);
extern "C" int GetAnswerInfoTS(int sock, char *dtype, short *length, char *ndims, int *dims, int *numbytes, void * *dptr, void **m);
extern "C" int MdsOpen(int sock, char *tree, int shot);
extern "C" int MdsSetDefault(int sock, char *node);
extern "C" int MdsClose(int sock);
extern "C" int ConnectToMds(char *host);
extern "C" int ConnectToMdsEvents(char *host);
extern "C" void DisconnectFromMds(int sockId);
extern "C" void FreeMessage(void *m);


#define DTYPE_UCHAR_IP   2
#define DTYPE_USHORT_IP  3
#define DTYPE_ULONG_IP   4
#define DTYPE_ULONGLONG_IP 5
#define DTYPE_CHAR_IP    6
#define DTYPE_SHORT_IP   7
#define DTYPE_LONG_IP    8
#define DTYPE_LONGLONG_IP 9
#define DTYPE_FLOAT_IP   10
#define DTYPE_DOUBLE_IP  11
#define DTYPE_CSTRING_IP 14

static int convertType(int mdsType)
{
	switch(mdsType) {
		case DTYPE_B: return DTYPE_CHAR_IP;
		case DTYPE_BU: return DTYPE_UCHAR_IP;
		case DTYPE_W: return DTYPE_SHORT_IP;
		case DTYPE_WU: return DTYPE_USHORT_IP;
		case DTYPE_L: return DTYPE_LONG_IP;
		case DTYPE_LU: return DTYPE_ULONG_IP;
		case DTYPE_Q: return DTYPE_LONGLONG_IP;
		case DTYPE_QU: return DTYPE_ULONGLONG_IP;
		case DTYPE_FLOAT: return DTYPE_FLOAT_IP;
		case DTYPE_DOUBLE: return DTYPE_DOUBLE_IP;
		case DTYPE_T: return DTYPE_CSTRING_IP;
		default: return -1;
	}
}


void *getManyObj(char *serializedIn)
{
	bool dataReported = false, errorReported = false;
	List *inArgs = (List *)deserialize(serializedIn);
	if(inArgs->clazz != CLASS_APD)// || inArgs->dtype != DTYPE_LIST)
	{
		printf("INTERNAL ERROR: Get Multi did not receive a LIST argument\n");
		return 0;
	}

	int nArgs = inArgs->len();
	String *nameKey = new String("name");
	String *exprKey = new String("exp");
	String *argsKey = new String("args");
	String *valueKey = new String("value");
	String *errorKey = new String("error");
	Dictionary *result = new Dictionary();
	for(int idx = 0; idx < nArgs; idx++)
	{
		Dictionary *currArg = (Dictionary* )inArgs->getElementAt(idx);
		if(currArg->clazz != CLASS_APD)// || currArg->dtype != DTYPE_DICTIONARY)
		{
			printf("INTERNAL ERROR: Get Multi Argument is not a DICTIONARY argument\n");
			return 0;
		}
		String *nameData = (String *)currArg->getItem(nameKey);
		String *exprData = (String *)currArg->getItem(exprKey);
		char *expr = exprData->getString();
		List *argsData = (List *)currArg->getItem(argsKey);
		
		Dictionary *answDict = new Dictionary();
		Data *currAnsw;
		try {
			if(argsData && argsData->len() > 0)
				currAnsw = executeWithArgs(expr, 2, argsData->getDscs(), argsData->len());
			else
				currAnsw = execute(expr);

			answDict->setItem(valueKey, currAnsw);
			dataReported = true;
		}catch(MdsException *exc)
		{
			String *errorData = new String((char *)exc->what());
			answDict->setItem(errorKey, errorData);
			errorReported = true;
		}
		result->setItem(nameData, answDict);
		deleteData(exprData);
		deleteData(nameData);
		delete [] expr;
		deleteData(argsData);
		deleteData(currArg);
	}
	void *resDsc = result->convertToDsc();
	deleteData(nameKey);
	deleteData(exprKey);
	deleteData(argsKey);
	if(!dataReported)
		deleteData(valueKey);
	if(!errorReported)
		deleteData(errorKey);
	deleteData(result);
	deleteData(inArgs);
	return resDsc;
}

void *putManyObj(char *serializedIn)
{
	List *inArgs = (List *)deserialize(serializedIn);
	if(inArgs->clazz != CLASS_APD)// || inArgs->dtype != DTYPE_LIST)
	{
		printf("INTERNAL ERROR: Get Multi did not receive a LIST argument\n");
		return 0;
	}

	int nArgs = inArgs->len();
	String *nodeKey = new String("node");
	String *exprKey = new String("exp");
	String *argsKey = new String("args");
	Dictionary *result = new Dictionary();
	for(int idx = 0; idx < nArgs; idx++)
	{
		Dictionary *currArg = (Dictionary* )inArgs->getElementAt(idx);
		if(currArg->clazz != CLASS_APD)// || currArg->dtype != DTYPE_DICTIONARY)
		{
			printf("INTERNAL ERROR: Get Multi Argument is not a DICTIONARY argument\n");
			return 0;
		}
		String *nodeNameData = (String *)currArg->getItem(nodeKey);
		String *exprData = (String *)currArg->getItem(exprKey);
		char *expr = exprData->getString();
		List *argsData = (List *)currArg->getItem(argsKey);
		int nPutArgs = 0;
		if(argsData)
			nPutArgs = argsData->len();

		try {
			Tree *tree = getActiveTree();
			Data *compiledData = (Data *)compileFromExprWithArgs(expr, nPutArgs, (argsData)?argsData->getDscs():0, tree);
			TreeNode *node = tree->getNode(nodeNameData);
			node->putData(compiledData);
			delete node;
			deleteData(compiledData);
			String *successData = new String("Success");
			result->setItem(nodeNameData, successData);
			delete tree;
		}catch(MdsException *exc)
		{
			String *errorData = new String((char *)exc->what());
			result->setItem(nodeNameData, errorData);
		}
		delete [] expr;
		deleteData(exprData);
		deleteData(argsData);
		deleteData(currArg);
		deleteData(nodeNameData);
	}
	void *resDsc = result->convertToDsc();
	deleteData(nodeKey);
	deleteData(exprKey);
	deleteData(argsKey);
	deleteData(inArgs);
	deleteData(result);
	return resDsc;
}


Connection::Connection(char *mdsipAddr) //mdsipAddr of the form <IP addr>[:<port>]
{
#ifdef HAVE_WINDOWS_H
	semH = CreateSemaphore(NULL, 1, 16, NULL);
#else
	int status = sem_init(&semStruct, 0, 1);
	if(status != 0)
		throw new MdsException("Cannot create lock semaphore");
#endif

	sockId = ConnectToMds(mdsipAddr);
	if(sockId <= 0)
	{
		char currMsg[256];
		strcpy(currMsg, "Cannot connect to ");
		strncpy(&currMsg[strlen(currMsg)], mdsipAddr, 255 - strlen(currMsg));
		throw new MdsException(currMsg);
	}
}
Connection::~Connection()
{
#ifdef HAVE_WINDOWS_H
#else
	sem_destroy(&semStruct);
#endif
	DisconnectFromMds(sockId);
}

#ifdef HAVE_WINDOWS_H
void Connection::lock() 
{
	if(semH)
		WaitForSingleObject(semH, INFINITE);      
}
void Connection::unlock()
{
	if(semH)
		ReleaseSemaphore(semH, 1, NULL);
}
#else
void Connection::lock() 
{
	sem_wait(&semStruct);
}
void Connection::unlock()
{
	sem_post(&semStruct);
}
#endif




void Connection::openTree(char *tree, int shot)
{
	int status = MdsOpen(sockId, tree, shot);
	if(!(status & 1))
		throw new MdsException(status);
}
void Connection::closeAllTrees()
{
	int status = MdsClose(sockId);
	if(!(status & 1))
		throw new MdsException(status);
}
Data *Connection::get(char *expr, Data **args, int nArgs)
{
	char clazz, dtype, nDims;
	short length;
	int *dims;
	int status, argIdx, numBytes;
	void *ptr, *mem = 0;
	char *buf;
	int retDims[MAX_DIMS];
	Data *resData;

	//Check whether arguments are compatible (Scalars or Arrays)
	for(argIdx = 0; argIdx < nArgs; argIdx++)
	{
		args[argIdx]->getInfo(&clazz, &dtype, &length, &nDims, &dims, &ptr);
		if(!ptr)
			throw new MdsException("Invalid argument passed to Connection::get(). Can only be Scalar or Array");
	}
	lock();
	status = SendArg(sockId, 0, DTYPE_CSTRING_IP, nArgs+1, strlen(expr), 0, 0, expr);
	if(!(status & 1))
	{
		unlock();
		throw new MdsException(status);
	}
	for(argIdx = 0; argIdx < nArgs; argIdx++)
	{
		args[argIdx]->getInfo(&clazz, &dtype, &length, &nDims, &dims, &ptr);
		status = SendArg(sockId, argIdx + 1, convertType(dtype), nArgs+1, length, nDims, dims, (char *)ptr);
		if(!(status & 1))
		{
			unlock();
			throw new MdsException(status);
		}
	}
    status = GetAnswerInfoTS(sockId, &dtype, &length, &nDims, retDims, &numBytes, &ptr, &mem);
	unlock();
	if(!(status & 1))
	{
		throw new MdsException(status);
	}
	if(nDims == 0)
	{
		switch(dtype) {
			case DTYPE_CHAR_IP:
				resData = new Int8(*(char *)ptr);
				break;
			case DTYPE_UCHAR_IP:
				resData = new Uint8(*(unsigned char *)ptr);
				break;
			case DTYPE_SHORT_IP:
				resData = new Int16(*(short *)ptr);
				break;
			case DTYPE_USHORT_IP:
				resData = new Uint16(*(unsigned short *)ptr);
				break;
			case DTYPE_LONG_IP:
				resData = new Int32(*(int *)ptr);
				break;
			case DTYPE_ULONG_IP:
				resData = new Uint32(*(unsigned int *)ptr);
				break;
			case DTYPE_LONGLONG_IP:
				resData = new Int64(*(long *)ptr);
				break;
			case DTYPE_ULONGLONG_IP:
				resData = new Uint64(*(unsigned long *)ptr);
				break;
			case DTYPE_FLOAT_IP:
				resData = new Float32(*(float *)ptr);
				break;
			case DTYPE_DOUBLE_IP:
				resData = new Float64(*(double *)ptr);
				break;
			case DTYPE_CSTRING_IP:
				buf = new char[length + 1];
				memcpy(buf, ptr, length);
				buf[length] = 0;
				resData = new String(buf);
				delete[] buf;
				break;
			default: throw new MdsException("Unexpected data type returned by mdsip");
		}
	}
	else //nDims > 0
	{
		switch(dtype) {
			case DTYPE_CHAR_IP:
				resData = new Int8Array((char *)ptr, nDims, retDims);
				break;
			case DTYPE_UCHAR_IP:
				resData = new Uint8Array((char *)ptr, nDims, retDims);
				break;
			case DTYPE_SHORT_IP:
				resData = new Int16Array((short *)ptr, nDims, retDims);
				break;
			case DTYPE_LONG_IP:
				resData = new Int32Array((int *)ptr, nDims, retDims);
				break;
			case DTYPE_ULONG_IP:
				resData = new Uint32Array((int *)ptr, nDims, retDims);
				break;
			case DTYPE_LONGLONG_IP:
				resData = new Int64Array((_int64 *)ptr, nDims, retDims);
				break;
			case DTYPE_ULONGLONG_IP:
				resData = new Uint64Array((_int64 *)ptr, nDims, retDims);
				break;
			case DTYPE_FLOAT_IP:
				resData = new Float32Array((float *)ptr, nDims, retDims);
				break;
			case DTYPE_DOUBLE_IP:
				resData = new Float64Array((double *)ptr, nDims, retDims);
				break;
			default: throw new MdsException("Unexpected data type returned by mdsip");
		}
	}
	if(mem) FreeMessage(mem);
	return resData;	
}

void Connection::put(char *inPath, char *expr, Data **args, int nArgs)
{
	char clazz, dtype, nDims;
	short length;
	int *dims;
	int status, argIdx, numBytes;
	void *ptr, *mem = 0;
	int retDims[MAX_DIMS];
	int varIdx;

	//Double backslashes!!
	char *path = new char[strlen(inPath)+2];
	if(inPath[0] == '\\')
	{
		path[0] = '\\';
		strcpy(&path[1], inPath);
	}
	else
		strcpy(path, inPath);


	//Check whether arguments are compatible (Scalars or Arrays)
	for(argIdx = 0; argIdx < nArgs; argIdx++)
	{
		args[argIdx]->getInfo(&clazz, &dtype, &length, &nDims, &dims, &ptr);
		if(!ptr)
			throw new MdsException("Invalid argument passed to Connection::get(). Can only be Scalar or Array");
	}

	char *putExpr = new char[strlen("TreePut(") + strlen(expr) +strlen(path) + 5 + nArgs * 2 + 2];
	if(nArgs > 0)
		sprintf(putExpr, "TreePut(\'%s\',\'%s\',", path, expr);
	else
		sprintf(putExpr, "TreePut(\'%s\',\'%s\'", path, expr);
	for(varIdx = 0; varIdx < nArgs; varIdx++)
	{
		if(varIdx < nArgs - 1)
			sprintf(&putExpr[strlen(putExpr)], "$,");
		else
			sprintf(&putExpr[strlen(putExpr)], "$");
	}
	sprintf(&putExpr[strlen(putExpr)], ")");
    delete [] path;

	lock();
	status = SendArg(sockId, 0, DTYPE_CSTRING_IP, nArgs+1, strlen(putExpr), 0, 0, putExpr);
	delete[] putExpr;
	if(!(status & 1))
	{
		unlock();
		throw new MdsException(status);
	}
	for(argIdx = 0; argIdx < nArgs; argIdx++)
	{
		args[argIdx]->getInfo(&clazz, &dtype, &length, &nDims, &dims, &ptr);
		status = SendArg(sockId, argIdx + 1, convertType(dtype), nArgs+1, length, nDims, dims, (char *)ptr);
		if(!(status & 1))
		{
			unlock();
			throw new MdsException(status);
		}
	}

    status = GetAnswerInfoTS(sockId, &dtype, &length, &nDims, retDims, &numBytes, &ptr, &mem);
	unlock();
    if (status & 1 && dtype == DTYPE_LONG_IP && nDims == 0 && numBytes == sizeof(int))
      memcpy(&status,ptr,numBytes);
    if (mem) FreeMessage(mem);
	if(!(status & 1))
		throw new MdsException(status);
}

void Connection::setDefault(char *path)
{
	int status = MdsSetDefault(sockId, path);
	if(!(status & 1))
		throw new MdsException(status);
}

void GetMany::insert(int idx, char *name, char *expr, Data **args, int nArgs)
{
	String *nameStr = new String(name);
	String *exprStr = new String(expr);
	Dictionary *dict = new Dictionary();
	dict->setItem(new String("name"), nameStr);
	dict->setItem(new String("exp"), exprStr);
	List *list = new List();
	for(int i = 0; i < nArgs; i++)
		list->append(args[i]);
	dict->setItem(new String("args"), list);

	if(idx >= len())
		List::append(dict);
	else
		List::insert(idx, dict);
}


void GetMany::append(char *name, char *expr, Data **args, int nArgs)
{
	insert(len(), name, expr, args, nArgs);
}

void GetMany::insert(char * beforeName, char *name, char *expr, Data **args, int nArgs)
{
	int nItems = len();
	int idx;
	String *nameKey = new String("name");
	String *nameStr = new String(name);
	for(idx = 0; idx < nItems; idx++)
	{
		Dictionary *currDict = (Dictionary *)getElementAt(idx);
		String *currName = (String *)currDict->getItem(nameKey);
		if(currName->equals(nameStr))
		{
			deleteData(currName);
			deleteData(currDict);
			break;
		}
		deleteData(currName);
		deleteData(currDict);
	}
	insert(idx, name, expr, args, nArgs);
	deleteData(nameKey);
	deleteData(nameStr);
}

void GetMany::remove(char *name)
{
	int nItems = len();
	int idx;
	String *nameKey = new String("name");
	String *nameStr = new String(name);
	for(idx = 0; idx < nItems; idx++)
	{
		Dictionary *currDict = (Dictionary *)getElementAt(idx);
		String *currName = (String *)currDict->getItem(nameKey);
		if(currName->equals(nameStr))
		{
			deleteData(currName);
			deleteData(currDict);
			break;
		}
		deleteData(currName);
		deleteData(currDict);
	}
	deleteData(nameKey);
	deleteData(nameStr);
	List::remove(idx);
}

void GetMany::execute()
{
	int serSize;
	char *ser = serialize(&serSize);

	Data *serData = new Uint8Array(ser, serSize);
	Data *serEvalRes = (Uint8Array *)conn->get("GetManyExecute($)", &serData, 1);
	delete [] ser;
	deleteData(serData);
	evalRes = (Dictionary *)deserialize(serEvalRes);
	deleteData(serEvalRes);
}


Data *GetMany::get(char *name)
{
	if(!evalRes)
		throw new MdsException("Data have not been evaluated yet");
	String *nameStr = new String(name);
	Dictionary *resItem = (Dictionary *)evalRes->getItem(nameStr);
	deleteData(nameStr);
	if(!resItem)
		throw new MdsException("Missing data item in evaluation list");
	String *valueKey = new String("value");

	Data *result = resItem->getItem(valueKey);
	deleteData(valueKey);
	if(result)
	{
		deleteData(resItem);
		return result;
	}
	String *errorKey = new String("error");
	String *error = (String *)resItem->getItem(errorKey);
	deleteData(errorKey);
	deleteData(resItem);
	if(!error)
		throw new MdsException("Unknown Error");
	else
	{
		char *errBuf = error->getString();
		deleteData(error);
		MdsException *exc = new MdsException(errBuf);
		delete [] errBuf;
		throw exc;
	}
	return 0;
}

/////////////////

void PutMany::insert(int idx, char *nodeName, char *expr, Data **args, int nArgs)
{
	String *nodeNameStr = new String(nodeName);
	String *exprStr = new String(expr);
	Dictionary *dict = new Dictionary();
	dict->setItem(new String("node"), nodeNameStr);
	dict->setItem(new String("exp"), exprStr);
	List *list = new List();
	for(int i = 0; i < nArgs; i++)
		list->append(args[i]);
	dict->setItem(new String("args"), list);

	if(idx >= len())
		List::append(dict);
	else
		List::insert(idx, dict);
}


void PutMany::append(char *name, char *expr, Data **args, int nArgs)
{
	insert(len(), name, expr, args, nArgs);
}

void PutMany::insert(char * beforeName, char *nodeName, char *expr, Data **args, int nArgs)
{
	int nItems = len();
	int idx;
	String *nameKey = new String("node");
	String *nodeNameStr = new String(nodeName);
	for(idx = 0; idx < nItems; idx++)
	{
		Dictionary *currDict = (Dictionary *)getElementAt(idx);
		String *currName = (String *)currDict->getItem(nameKey);
		if(currName->equals(nodeNameStr))
		{
			deleteData(currName);
			deleteData(currDict);
			break;
		}
		deleteData(currName);
		deleteData(currDict);
	}
	insert(idx, nodeName, expr, args, nArgs);
	deleteData(nameKey);
	deleteData(nodeNameStr);
}

void PutMany::remove(char *nodeName)
{
	int nItems = len();
	int idx;
	String *nodeKey = new String("node");
	String *nodeNameStr = new String(nodeName);
	for(idx = 0; idx < nItems; idx++)
	{
		Dictionary *currDict = (Dictionary *)getElementAt(idx);
		String *currName = (String *)currDict->getItem(nodeKey);
		if(currName->equals(nodeNameStr))
		{
			deleteData(currName);
			deleteData(currDict);
			break;
		}
		deleteData(currName);
		deleteData(currDict);
	}
	deleteData(nodeKey);
	deleteData(nodeNameStr);
	List::remove(idx);
}

void PutMany::execute()
{
	int serSize;
	char *ser = serialize(&serSize);

	Data *serData = new Uint8Array(ser, serSize);
	Data *serEvalRes = (Uint8Array *)conn->get("PutManyExecute($)", &serData, 1);
	delete [] ser;
	deleteData(serData);
	evalRes = (Dictionary *)deserialize(serEvalRes);
	deleteData(serEvalRes);
}


void PutMany::checkStatus(char *nodeName)
{
	if(!evalRes)
		throw new MdsException("Data have not written yet");
	String *nodeNameStr = new String(nodeName);
	String *resItem = (String *)evalRes->getItem(nodeNameStr);
	deleteData(nodeNameStr);
	if(!resItem)
		throw new MdsException("Missing data item in evaluation list");

	String *successStr = new String("Success");
	if(!successStr->equals(resItem))
	{
		char *errMsg = resItem->getString();
		MdsException *exc = new MdsException(errMsg);
		delete [] errMsg;
		deleteData(successStr);
		deleteData(resItem);
		throw exc;
	}
	deleteData(successStr);
	deleteData(resItem);
}




