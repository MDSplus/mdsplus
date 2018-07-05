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
  
EXPORT void mdsplus_connection_getNode(const void *lvConnectionPtr, void **lvTreeNodePtrOut,
				      const char *pathIn, ErrorCluster * error)
{
	MDSplus::TreeNode *treeNodePtrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		MDSplus::Connection * connectionPtr = reinterpret_cast<MDSplus::Connection *>(const_cast<void *>(lvConnectionPtr));
		treeNodePtrOut = connectionPtr->getNode(const_cast<char *>(pathIn));
		*lvTreeNodePtrOut = reinterpret_cast<void *>(treeNodePtrOut);
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
	}
	catch (const MDSplus::MdsException &mdsE)
	{
		errorCode = bogusError;
		*lvTreeNodePtrOut = 0;
		fillErrorCluster(errorCode, errorSource, const_cast<char *>(mdsE.what()), error);
	}
}


