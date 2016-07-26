#include "mdsobjectswrp.h"


EXPORT void mdsplus_connection_constructor(void **lvConnectionPtrOut, const char *ipPortIn, ErrorCluster *error)
{
	MDSplus::Connection *connectionPtrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = (char *)"";
	try
	{
		MDSplus::Connection *connectionPtrOut = new MDSplus::Connection(const_cast<char *>(ipPortIn));
		*lvConnectionPtrOut = reinterpret_cast<void *>(connectionPtrOut);
	}
	catch (const MDSplus::MdsException &e)
	{
		errorCode = bogusError;
		errorMessage = e.what();
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_connection_destructor(void **lvConnectionPtr)
{
	MDSplus::Connection *connectionPtr = reinterpret_cast<MDSplus::Connection *>(*lvConnectionPtr);
	delete connectionPtr;
	*lvConnectionPtr = NULL;
}

EXPORT void mdsplus_connection_getData(const void *lvConnectionPtr, void **lvDataPtrOut, const char *expressionIn, ErrorCluster *error)
{
	MgErr errorCode = noErr;
	char const * errorSource = __func__;
	char const * errorMessage = (char *)"";
	try {
		MDSplus::Connection * connectionPtr = reinterpret_cast<MDSplus::Connection *>(const_cast<void *>(lvConnectionPtr));
		MDSplus::Data * dataPtrOut = connectionPtr->get((char *)expressionIn);
		*lvDataPtrOut = reinterpret_cast<void *>(dataPtrOut);
	}
	catch (const MDSplus::MdsException & e)
	{
		errorCode = bogusError;
		errorMessage = e.what();
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_connection_putData(const void *lvConnectionPtr, const void *lvDataPtrIn, const char *pathIn, ErrorCluster *error)
{
	MDSplus::Data *dataPtrIn = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = (char *)"";
	try
	{
		MDSplus::Connection * connectionPtr = reinterpret_cast<MDSplus::Connection *>(const_cast<void *>(lvConnectionPtr));
		dataPtrIn = reinterpret_cast<MDSplus::Data *>(const_cast<void *>(lvDataPtrIn));
		if(connectionPtr)
		    connectionPtr->put(pathIn, (char *)"$", &dataPtrIn, 1);
	}
	catch (const MDSplus::MdsException & e)
	{
		errorCode = bogusError;
		errorMessage = e.what();
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_connection_openTree(const void *lvConnectionPtr, const char *tree, int shot,  ErrorCluster *error)
{
	MgErr errorCode = noErr;
	char const * errorSource = __func__;
	char const * errorMessage = (char *)"";
	try {
		MDSplus::Connection * connectionPtr = reinterpret_cast<MDSplus::Connection *>(const_cast<void *>(lvConnectionPtr));
		connectionPtr->openTree((char *)tree, shot);
	}
	catch (const MDSplus::MdsException & e)
	{
		errorCode = bogusError;
		errorMessage = e.what();
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_connection_closeTree(const void *lvConnectionPtr,  ErrorCluster *error)
{
	MgErr errorCode = noErr;
	char const * errorSource = __func__;
	char const * errorMessage = (char *)"";
	try {
		MDSplus::Connection * connectionPtr = reinterpret_cast<MDSplus::Connection *>(const_cast<void *>(lvConnectionPtr));
		connectionPtr->closeAllTrees();
	}
	catch (const MDSplus::MdsException & e)
	{
		errorCode = bogusError;
		errorMessage = e.what();
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_connection_putSignal(const void *lvConnectionPtr, const void *lvDataPtrIn, const void *lvTimePtrIn, const char *pathIn, ErrorCluster *error)
{
	MDSplus::Data *dataPtrIn = NULL;
	MDSplus::Data *timePtrIn = NULL;
	MDSplus::Data *args[2];
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = (char *)"";
	try
	{
		MDSplus::Connection * connectionPtr = reinterpret_cast<MDSplus::Connection *>(const_cast<void *>(lvConnectionPtr));
		dataPtrIn = reinterpret_cast<MDSplus::Data *>(const_cast<void *>(lvDataPtrIn));
		timePtrIn = reinterpret_cast<MDSplus::Data *>(const_cast<void *>(lvTimePtrIn));
		args[0] = dataPtrIn;
		args[1] = timePtrIn;
		if(connectionPtr && timePtrIn)
		    connectionPtr->put(pathIn, (char *)"BUILD_SIGNAL($1,*,$2)", args, 2);
	}
	catch (const MDSplus::MdsException & e)
	{
		errorCode = bogusError;
		errorMessage = e.what();
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}


