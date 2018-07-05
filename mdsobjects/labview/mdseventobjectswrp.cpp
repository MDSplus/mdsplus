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

#include <cstring>

using namespace MDSplus;
using namespace std;
/********************************************************************************************************
												EVENT
 ********************************************************************************************************/

EXPORT void mdsplus_event_constructor(void **lvEventPtrOut, const char *evNameIn, ErrorCluster *error)
{
	Event *eventPtrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = (char *)"";
	try
	{
		eventPtrOut = new Event(const_cast<char *>(evNameIn));
		*lvEventPtrOut = reinterpret_cast<void *>(eventPtrOut);
	}
	catch (const MdsException &e)
	{
		delete eventPtrOut;
		errorCode = bogusError;
		errorMessage = e.what();
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_event_destructor(void **lvEventPtr)
{
	Event *eventPtr = reinterpret_cast<Event *>(*lvEventPtr);
	delete eventPtr;
	*lvEventPtr = NULL;
}
/*
EXPORT void mdsplus_event_abort(const void *lvEventPtr, ErrorCluster *error)
{
	Event *eventPtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = (char *)"";
	try
	{
		eventPtr = reinterpret_cast<Event *>(const_cast<void *>(lvEventPtr));
		eventPtr->abort();
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
*/
EXPORT void mdsplus_event_waitData(const void *lvEventPtr, void **lvDataPtrOut, int *timeoutOccurred, ErrorCluster *error)
{
	MgErr errorCode = noErr;
	char const * errorSource = __func__;
	char const * errorMessage = (char *)"";
	try {
		*timeoutOccurred = 0;
		Event * eventPtr = reinterpret_cast<Event *>(const_cast<void *>(lvEventPtr));
//1 Second timeout
		Data * dataPtrOut = eventPtr->waitData(1);
		*lvDataPtrOut = reinterpret_cast<void *>(dataPtrOut);
	}
	catch (const MdsException & e)
	{
		*timeoutOccurred = 1;
//		errorCode = bogusError;
//		errorMessage = e.what();
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_event_wait(const void *lvEventPtr, int *timeoutOccurred, ErrorCluster *error)
{
	Event *eventPtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = (char *)"";
	*timeoutOccurred = 0;
	try
	{
		eventPtr = reinterpret_cast<Event *>(const_cast<void *>(lvEventPtr));

		// 1 Sec timeout
std::cout << "ASPETTO EVENTO....\n"; 

		eventPtr->wait(1);
std::cout << "EVENTO ARRIVATO....\n"; 
	}
	catch (const MdsException & e)
	{
		*timeoutOccurred = 1;
		//errorCode = bogusError;
		//errorMessage = e.what();
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_event_getName(const void *lvEventPtr, LStrHandle lvStrHdlOut, ErrorCluster *error)
{
	Event *eventPtr = NULL;
	char *strOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = (char *)"";
	try
	{
		eventPtr = reinterpret_cast<Event *>(const_cast<void *>(lvEventPtr));
		strOut = strdup(eventPtr->getName());
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
		deleteNativeArray(strOut);
		errorCode = bogusError;
		errorMessage = e.what();
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_event_waitRaw(const void *lvEventPtr, LByteArrHdl lvByteArrHdlOut, ErrorCluster *error)
{
	Event *eventPtr = NULL;
	char const * byteArrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = (char *)"";
	try
	{
		eventPtr = reinterpret_cast<Event *>(const_cast<void *>(lvEventPtr));
		size_t byteArrLen = 0;
std::cout << "ASPETTO EVENTO RAW ....\n";
		byteArrOut = eventPtr->waitRaw(&byteArrLen);
std::cout << "EVENTO ARRIVATO....\n";
		errorCode = NumericArrayResize(iB, 1, reinterpret_cast<UHandle *>(&lvByteArrHdlOut), static_cast<int32>(byteArrLen));
		if (!errorCode)
		{
			for (size_t i = 0; i < byteArrLen; i++)
				(*lvByteArrHdlOut)->elt[i] = static_cast<int8>(byteArrOut[i]);
			(*lvByteArrHdlOut)->dimSize = static_cast<int32>(byteArrLen);
		}
		else
			errorMessage = (char *)"NumericArrayResize error";
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

EXPORT void mdsplus_event_setEvent(const char *evNameIn, ErrorCluster *error)
{
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = (char *)"";
	try
	{
		Event::setEvent(const_cast<char *>(evNameIn));
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

EXPORT void mdsplus_event_setEvent_data(const char *evNameIn, const void *lvDataPtrIn, ErrorCluster *error)
{
	Data *dataPtrIn = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = (char *)"";
	try
	{
		dataPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvDataPtrIn));
		Event::setEvent(const_cast<char *>(evNameIn), dataPtrIn);
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

EXPORT void mdsplus_event_setEventRaw(const char *evNameIn, LByteArrHdl lvByteArrHdlIn, ErrorCluster *error)
{
	char *int8Arr = NULL;	
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = (char *)"";
	try
	{
		int int8ArrLen = static_cast<int>((*lvByteArrHdlIn)->dimSize);
		int8Arr = new char[int8ArrLen];
		for (int i = 0; i < int8ArrLen; i++)
			int8Arr[i] = static_cast<char>((*lvByteArrHdlIn)->elt[i]);
		Event::setEventRaw(const_cast<char *>(evNameIn), int8ArrLen, int8Arr);
		delete[] int8Arr;
	}
	catch (const MdsException &e)
	{
		delete[] int8Arr;
		errorCode = bogusError;
		errorMessage = e.what();
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

/********************************************************************************************************
												REVENT
 *******************************************************************************************************

EXPORT void mdsplus_revent_constructor(void **lvREventPtrOut, const char *evNameIn, ErrorCluster *error)
{
	REvent *reventPtrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = (char *)"";
	try
	{
		reventPtrOut = new REvent(const_cast<char *>(evNameIn));
		*lvREventPtrOut = reinterpret_cast<void *>(reventPtrOut);
	}
	catch (const MdsException &e)
	{
		delete reventPtrOut;
		errorCode = bogusError;
		errorMessage = e.what();
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_revent_destructor(void **lvREventPtr)
{
	REvent *reventPtr = reinterpret_cast<REvent *>(*lvREventPtr);
	delete reventPtr;
	*lvREventPtr = NULL;
}

EXPORT void mdsplus_revent_getData(const void *lvREventPtr, void **lvDataPtrOut, ErrorCluster *error)
{
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = (char *)"";
	try
	{
		REvent * reventPtr = reinterpret_cast<REvent *>(const_cast<void *>(lvREventPtr));
		Data * dataPtrOut = reventPtr->getData();
		*lvDataPtrOut = reinterpret_cast<void *>(dataPtrOut);
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

EXPORT void mdsplus_revent_getName(const void *lvREventPtr, LStrHandle lvStrHdlOut, ErrorCluster *error)
{
	REvent *reventPtr = NULL;
	char *strOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = (char *)"";
	try
	{
		reventPtr = reinterpret_cast<REvent *>(const_cast<void *>(lvREventPtr));
		strOut = reventPtr->getName();
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
		deleteNativeArray(strOut);
		errorCode = bogusError;
		errorMessage = e.what();
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_revent_getRaw(const void *lvREventPtr, LByteArrHdl lvByteArrHdlOut, ErrorCluster *error)
{
	REvent *reventPtr = NULL;
	char const * byteArrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = (char *)"";
	try
	{
		reventPtr = reinterpret_cast<REvent *>(const_cast<void *>(lvREventPtr));
		size_t byteArrLen = 0;
		byteArrOut = reventPtr->getRaw(&byteArrLen);
		errorCode = NumericArrayResize(iB, 1, reinterpret_cast<UHandle *>(&lvByteArrHdlOut), byteArrLen);
		if (!errorCode)
		{
			for (size_t i = 0; i < byteArrLen; i++)
				(*lvByteArrHdlOut)->elt[i] = static_cast<int8>(byteArrOut[i]);
			(*lvByteArrHdlOut)->dimSize = static_cast<int32>(byteArrLen);
		}
		else
			errorMessage = (char *)"NumericArrayResize error";
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

EXPORT void mdsplus_revent_getTime(const void *lvREventPtr, void **lvUint64PtrOut, ErrorCluster *error)
{
	REvent *reventPtr = NULL;
	Uint64 *uint64PtrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = (char *)"";
	try
	{
		reventPtr = reinterpret_cast<REvent *>(const_cast<void *>(lvREventPtr));
		uint64PtrOut = reventPtr->getTime();
		*lvUint64PtrOut = reinterpret_cast<void *>(uint64PtrOut);
	}
	catch (const MdsException &e)
	{
		deleteData(uint64PtrOut);
		errorCode = bogusError;
		errorMessage = e.what();
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_revent_run(const void *lvREventPtr, ErrorCluster *error)
{
	REvent *reventPtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = (char *)"";
	try
	{
		reventPtr = reinterpret_cast<REvent *>(const_cast<void *>(lvREventPtr));
		reventPtr->run();
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

EXPORT void mdsplus_revent_setEvent(const char *evNameIn, const void *lvDataPtrIn, ErrorCluster *error)
{
	Data *dataPtrIn = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = (char *)"";
	try
	{
		dataPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvDataPtrIn));
		REvent::setEvent(const_cast<char *>(evNameIn), dataPtrIn);
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

EXPORT void mdsplus_revent_setEventAndWait(const char *evNameIn, const void *lvDataPtrIn, ErrorCluster *error)
{
	Data *dataPtrIn = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = (char *)"";
	try
	{
		dataPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvDataPtrIn));
		REvent::setEventAndWait(const_cast<char *>(evNameIn), dataPtrIn);
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

EXPORT void mdsplus_revent_setEventRaw(const char *evNameIn, LByteArrHdl lvByteArrHdlIn, ErrorCluster *error)
{
	char *int8Arr = NULL;	
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = (char *)"";
	try
	{
		int int8ArrLen = static_cast<int>((*lvByteArrHdlIn)->dimSize);
		int8Arr = new char[int8ArrLen];
		for (int i = 0; i < int8ArrLen; i++)
			int8Arr[i] = static_cast<char>((*lvByteArrHdlIn)->elt[i]);
		REvent::setEventRaw(const_cast<char *>(evNameIn), int8ArrLen, int8Arr);
		delete[] int8Arr;
	}
	catch (const MdsException &e)
	{
		delete[] int8Arr;
		errorCode = bogusError;
		errorMessage = e.what();
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_revent_setEventRawAndWait(const char *evNameIn, LByteArrHdl lvByteArrHdlIn, ErrorCluster *error)
{
	char *int8Arr = NULL;	
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = (char *)"";
	try
	{
		int int8ArrLen = static_cast<int>((*lvByteArrHdlIn)->dimSize);
		int8Arr = new char[int8ArrLen];
		for (int i = 0; i < int8ArrLen; i++)
			int8Arr[i] = static_cast<char>((*lvByteArrHdlIn)->elt[i]);
		REvent::setEventRawAndWait(const_cast<char *>(evNameIn), int8ArrLen, int8Arr);
		delete[] int8Arr;
	}
	catch (const MdsException &e)
	{
		delete[] int8Arr;
		errorCode = bogusError;
		errorMessage = e.what();
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}
*/
