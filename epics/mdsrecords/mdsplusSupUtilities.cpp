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
#include <stdio.h>
#include <registryFunction.h>
#include <epicsExport.h>
#include <mdsobjects.h>
#include <string.h>
#include <semaphore.h>


#include <dbFldTypes.h>

//#include <epicsTypes.h>
//#include <epicsExport.h>
#define MAX_NODES 10000
#define MAX_CONNECTIONS 100
extern "C" int openMds(char *expName, int shot, int isLocal, char *ipAddr, char *path, unsigned int *nodeId, char *errMsg);
extern "C" int writeMds(int nodeId, double *vals, int dtype, int preTrigger, int nVals, int dim1, int dim2, int dataIdx, double period, double trigger, long epicsTime, char *errMsg, int debug);
//extern "C" int writeMds(int nodeId, double *vals, int nVals, int *dims, int *epicsTime, double *timing, char *errMsg);
extern "C" int registerMdsEvent(char *eventName);
extern "C" void waitMdsEvent(char *eventName, char *buf, int maxLen, int *retLen);
extern "C" int doMdsAction(char *path, int nodeId, char *errMsg);
extern "C" int evaluateExpr(char *expr, int treeIdx, int nBuffers, void **buffers, int *bufDims, int *bufTypes, int maxRetElements, int retType, void *retBuf, int *retElements, char *errMsg, int isLocal);

using namespace MDSplus;
//Generic Lock support
static 	sem_t semStruct;
static bool semInitialized = false;


static void lock() 
{
	if(!semInitialized)
	{
		semInitialized = true;
		int status = sem_init(&semStruct, 0, 1);
		if(status != 0)
			printf("Cannot create lock semaphore");
	}
	sem_wait(&semStruct);
}
static void unlock()
{
	sem_post(&semStruct);
}


////////Class EventHandler is a support class for Mds Event management in devMeSoft for WAVE record.
class EventHandler:Event  //Inherits from MDSplus Event class
{
    sem_t lockSem;
    sem_t sem;
    char *data;
    int dataSize;
    bool dataReady;

    void lock()
    {
	sem_wait(&lockSem);
    }
    void unlock()
    {
	sem_post(&lockSem);
    }

public:

    EventHandler(char *evName):Event(evName)
    {
 	int status = sem_init(&lockSem, 0, 1);
	if(status == -1)
	{
	    printf("FATAL: Cannot initialize semaphore\n");
	    exit(0); //this MUST never happen
	} 
	status = sem_init(&sem, 0, 0);
	if(status == -1)
	{
	    printf("FATAL: Cannot initialize semaphore\n");
	    exit(0); //this MUST never happen
	} 
	dataReady = false;
	dataSize = 0;
	data = 0;
    }

    void postBuffer(char *buf, int size)
    {

	lock();
	if(dataReady) //Previous event message not consumed yet
	{
	    delete []data;
	    data = new char[size];
	    memcpy(data, buf, size);
	    dataSize = size;
	}
	else
	{
	    data = new char[size];
	    memcpy(data, buf, size);
	    sem_post(&sem);
	    dataSize = size;
	    dataReady = true;
	
	}
	unlock();
    }
    char *getBuffer(int *size)
    {
	char *retData;
	lock();
	while(!dataReady)
	{
	    unlock();
	    sem_wait(&sem);
	    lock();
	}
	dataReady = false;
	*size = dataSize;
	retData = data;
	unlock();
	return retData;
    }

    void run() //Called by superclass upon receipt of an event
    {
	size_t size;
//printf("Got Event %s\n", getName());
	char *buf = (char *)getRaw(&size);
	postBuffer(buf, size);
    }
};

//End class EventHandler
struct EventDescriptor{
    char *evName;
    EventHandler *event;
};
#define MAX_EVENTS 1024
static EventDescriptor eventDescrs[MAX_EVENTS];
//return 0 if it is a new event
//return -1 if the same name has already been declared: this is an error condition since only one record per event must be defined per IOC
static int addEvent(char *evName)
{
    int i;
    lock();
    for(i = 0; i < MAX_EVENTS; i++)
    {
	if(eventDescrs[i].evName && !strcmp(evName, eventDescrs[i].evName))
	    return -1; 
	if(!eventDescrs[i].evName) break;
    }
    if(i == MAX_EVENTS) return -1;
    eventDescrs[i].evName = new char[strlen(evName) + 1];
    strcpy(eventDescrs[i].evName, evName);
    eventDescrs[i].event = new EventHandler(evName);
    unlock();
    return 0;
}

static EventHandler *getEvent(char *evName)
{
    int i;
    lock();
    for(i = 0; i < MAX_EVENTS; i++)
    {
	if(eventDescrs[i].evName && !strcmp(evName, eventDescrs[i].evName))
	    break;
    }
    unlock();
    if(i == MAX_EVENTS) return 0; //Error: No event found!!
    return eventDescrs[i].event;
}
    




////////////////////////////

//struct nodeDescr contains information related to a given node.
//For local connection it is the corresponding TreeNode instance
//For Remote connection it is the experiment name and the path (used by connection)
struct nodeDescr {
    Tree *tree;
    TreeNode *node;
    char *path;
    int connectionId; //Index of the corresponding Connection object in Connection table
}; 

struct connectionDescr {
    Connection *connection;
    char *ipAddr;  //IP address for thei connection
    char *experiment; //Currently open experiment 
//NOTE Currently ony one open experiment and shot is supported per Connection
//Therefore this fiedl is not currently used
};

static nodeDescr nodeTable[MAX_NODES];
static connectionDescr connectionTable[MAX_CONNECTIONS];

//When called, a new TreeNode instance is added to nodeTable. Note this is called in record initialization
static int getNodeId(TreeNode *node, char *path, Tree *tree, int connectionId)
{
    int i;
    lock();
    for(i = 0; i < MAX_NODES && nodeTable[i].path != NULL; i++);
    if(i == MAX_NODES)
    {
	unlock();
	printf("NODE TABLE OVERFLOW in mdsput Record initialization!!!!!\n");
	return -1;
    }
    nodeTable[i].node = node;
    nodeTable[i].tree = tree;
    nodeTable[i].connectionId = connectionId;
    if(path)
    {
    	nodeTable[i].path = (char *)malloc(strlen(path)+1);
    	strcpy(nodeTable[i].path, path);
    }
    unlock();
    return i;
}

//Note: no dynamic reconnection currently supported
static int getConnectionId(char *ipAddr, char *exp, int shot)
{
    int i;
    lock();
    for(i = 0; i < MAX_CONNECTIONS && connectionTable[i].ipAddr && strcmp(connectionTable[i].ipAddr, ipAddr);i++);
    if(i == MAX_CONNECTIONS)
    {
	unlock();
	printf("CONNECTION TABLE OVERFLOW in mdsput Record initialization!!!!!\n");
	return -1;
    }
    if(!connectionTable[i].ipAddr)  //First connection to this address
    {
	connectionTable[i].ipAddr = (char *)malloc(strlen(ipAddr) + 1);
	strcpy(connectionTable[i].ipAddr, ipAddr);
	unlock();
	//Let the lengthy connection establishment be done outsize lock
	try {
	    connectionTable[i].connection = new Connection(ipAddr);
	    if(exp && *exp) //May be NULL or empty string if only the connection is required
	        connectionTable[i].connection->openTree(exp, shot);
	}catch(MdsException *exc) 
	{
	    printf("Cannot establish mdsip connection: %s\n", exc->what());
	    connectionTable[i].connection = 0;
	}
    }
    else //A Connection to this address already exists
	unlock();
    return i;
}

static void updateNode(int id, int *isLocal, TreeNode **node, Connection **conn, char **path, Tree **tree)
{
    *node = NULL;
    lock();
    if(id < MAX_NODES)
    {
        *node = nodeTable[id].node;
	if(nodeTable[id].connectionId != -1)
            *conn = connectionTable[nodeTable[id].connectionId].connection;
	else
	    *conn = 0;	  
	*path = nodeTable[id].path;
	*isLocal = nodeTable[id].node != 0;
	*tree = nodeTable[id].tree;
    }
    unlock();
}	

int openMds(char *expName, int shot, int isLocal, char *ipAddr, char *path, unsigned int *nodeId, char *errMsg)
{
    int connId;
    int id;
    Tree *tree;
    printf("OpenMds: %s %d %s %s %s\n", expName, shot, (isLocal)?"Local":"Remote", ipAddr, path);

    if(isLocal)
    {
	TreeNode *node;
    	try {
    	    tree = new Tree(expName, shot);
	    if(path) //path == 0 means that the tree must be only open, no nid searched
	        node = tree->getNode(path);
	    else
		node = 0;
	}
    	catch(MdsException *exc)
    	{
	    printf("Cannot Open tree or find node: %s\n", exc->what());
	    strncpy(errMsg, exc->what(), 40);
	    return 0;
    	}
	id = getNodeId(node, path, tree, -1);
	if(id == -1)
	{
	    strcpy(errMsg, "NODE TABLE OVERFLOW");
	    return 0;
	}
 	*nodeId = id;
    }
    else //Remote connection
    {
	connId = getConnectionId(ipAddr, expName, shot);
	if(connId == -1)
	{
	    strcpy(errMsg, "CANNOT CONNECT TO REMOTE TREE");
	    return 0;
	}
	id = getNodeId(0, path, 0, connId);
	if(id == -1)
	{
	    strcpy(errMsg, "NODE TABLE OVERFLOW");
	    return 0;
	}
	*nodeId = id;
    }
    return 1; //MDSplus success
}		

//int writeMds(int nodeId, int dtype, double *vals, int nVals, int *dims, int *epicsTime, double *timing, char *errMsg)
int writeMds(int nodeId, double *vals, int dtype, int preTriggerSamples, int nVals, int dim1, int dim2, int dataIdx, double period, double trigger, long  epicsTime, char *errMsg, int debug)
{
    if(debug)
    	printf("WriteMds: NodeId: %d, dtype: %d, nVals: %d, dim1: %d, dim2: %d\n, dataIdx: %d, epicsTime: %lX, period: %f, trigger: %f preTrigger samples: %d\n", nodeId, dtype, nVals, dim1, dim2, dataIdx, epicsTime, (float)period, (float)trigger, preTriggerSamples);
    TreeNode *node;
    Tree *tree;
    Connection *conn;
    char *path;
    int isLocal;
    updateNode(nodeId, &isLocal, &node, &conn, &path, &tree);
    if(!node && !conn)
    {
	strcpy(errMsg, "Internal error: node ID not found");
    	return 0;
    }


    if(nVals == 1) //A single data item is being written: in this case EPICS time is considered
    {
	Data *data;
	if(dim1 == 0 && dim2 == 0)
	{
	    switch(dtype) {
		case DBF_CHAR: 
		    data = (Data *)new Int8(((char *)vals)[0]);
		    break;
		case DBF_UCHAR: 
		    data = (Data *)new Uint8(((unsigned char *)vals)[0]);
		    break;
		case DBF_SHORT: 
		    data = (Data *)new Int16(((short *)vals)[0]);
		    break;
		case DBF_USHORT: 
		    data = (Data *)new Uint16(((unsigned short *)vals)[0]);
		    break;
		case DBF_LONG: 
		    data = (Data *)new Int32(((int *)vals)[0]);
		    break;
		case DBF_ULONG: 
		    data = (Data *)new Uint32(((unsigned int *)vals)[0]);
		    break;
		case DBF_FLOAT: 
		    data = (Data *)new Float32(((float *)vals)[0]);
		    break;
		case DBF_DOUBLE: 
		    data = (Data *)new Float64(((double *)vals)[0]);
		    break;
		default:
		    strcpy(errMsg, "Unsupported data type");
		    return 0;
	    }
	}
	else if(dim2 == 0)
	{
	    switch(dtype) {
		case DBF_CHAR: 
		    data = (Data *)new Int8Array((const char *)vals, dim1);
		    break;
		case DBF_UCHAR: 
		    data = (Data *)new Uint8Array((const unsigned char *)vals, dim1);
		    break;
		case DBF_SHORT: 
		    data = (Data *)new Int16Array(( const short *)vals, dim1);
		    break;
		case DBF_USHORT: 
		    data = (Data *)new Uint16Array(( unsigned short *)vals, dim1);
		    break;
		case DBF_LONG: 
		    data = (Data *)new Int32Array((int *)vals, dim1);
		    break;
		case DBF_ULONG: 
		    data = (Data *)new Uint32Array((unsigned int *)vals, dim1);
		    break;
		case DBF_FLOAT: 
		    data = (Data *)new Float32Array((float *)vals, dim1);
		    break;
		case DBF_DOUBLE: 
		    data = (Data *)new Float64Array((double *)vals, dim1);
		    break;
		default:
		    strcpy(errMsg, "Unsupported data type");
		    return 0;
	    }
	}
	else
	{
	    int dims[2];
	    dims[0] = dim1;
	    dims[1] = dim2;
	    switch(dtype) {
		case DBF_CHAR: 
		    data = (Data *)new Int8Array((char *)vals, 2, dims);
		    break;
		case DBF_UCHAR: 
		    data = (Data *)new Uint8Array((unsigned char *)vals, 2, dims);
		    break;
		case DBF_SHORT: 
		    data = (Data *)new Int16Array((short *)vals, 2, dims);
		    break;
		case DBF_USHORT: 
		    data = (Data *)new Uint16Array((unsigned short *)vals, 2, dims);
		    break;
		case DBF_LONG: 
		    data = (Data *)new Int32Array((int *)vals, 2, dims);
		    break;
		case DBF_ULONG: 
		    data = (Data *)new Uint32Array(( unsigned int *)vals, 2, dims);
		    break;
		case DBF_FLOAT: 
		    data = (Data *)new Float32Array((float *)vals, 2, dims);
		    break;
		case DBF_DOUBLE: 
		    data = (Data *)new Float64Array((double *)vals, 2, dims);
		    break;
		default:
		    strcpy(errMsg, "Unsupported data type");
		    return 0;
	    }
	}
	if(isLocal)
	{
	    try {
		node->putRow(data, (int64_t *)&epicsTime);
		deleteData(data);
	    }
    	    catch(MdsException *exc)
    	    {
		strncpy(errMsg, exc->what(), 40);
		return 0;
    	    }
	}
	else
	{
	    char expr[256];
	    Int64 *timeData = new Int64((int64_t)epicsTime);
	    sprintf(expr, "PutRow(\'%s\', 1000, $, $)", path);
	    Data * args[2];
	    args[0] = timeData;
	    args[1] = data;
	    try {
		Data *retData = conn->get(expr, args, 2);
		if(retData) deleteData(retData);
		deleteData(data);
		deleteData(timeData);
	    }
    	    catch(MdsException *exc)
    	    {
		if(debug) printf("ERROR WRITING REMOTE TREE: %s\n", exc->what());
		strncpy(errMsg, exc->what(), 40);
		return 0;
    	    }
	}
    }
    else //Multiple samples: time is now derived from trigger, dataIds and period
    {
	int dims[3];
	int nDims;
	dims[0] = nVals;
	if(dim1 == 0 && dim2 == 0)
	    nDims = 1;
	else if (dim2 == 0)
	{
	    dims[1] = dim1;
	    nDims = 2;
	}
	else
	{
	    dims[1] = dim1;
	    dims[2] = dim2;
	    nDims = 3;
	}
	Array *data;
    	switch(dtype) {
	    case DBF_CHAR: 
		data = new Int8Array((char *)vals, nDims, dims);
		break;
	    case DBF_UCHAR: 
		data = new Uint8Array((unsigned char *)vals, nDims, dims);
		break;
	    case DBF_SHORT: 
		data = new Int16Array((short *)vals, nDims, dims);
		break;
	    case DBF_USHORT: 
		data = new Uint16Array((unsigned short *)vals, nDims, dims);
		break;
	    case DBF_LONG: 
		data = new Int32Array((int *)vals, nDims, dims);
		break;
	    case DBF_ULONG: 
		data = new Uint32Array((unsigned int *)vals, nDims, dims);
		break;
	    case DBF_FLOAT: 
		data = new Float32Array((float *)vals, nDims, dims);
		break;
	    case DBF_DOUBLE: 
		data = new Float64Array((double *)vals, nDims, dims);
		break;
	}
    	Float64 *start = new Float64(trigger - preTriggerSamples * period + period * dataIdx - 0.05);
    	Float64 *end = new Float64(trigger + period * (dataIdx + nVals)+0.05);
    	Data *times = new Range(new Float64(trigger + period * dataIdx), new Float64(trigger + period * (dataIdx + nVals) - 0.05), new Float64(period));
	if(isLocal)
	{
    	    try {
    	    	node->makeSegment(start, end, times, data);
	    	deleteData(data);
	    	deleteData(times);
		deleteData(end);
    	    }
    	    catch(MdsException *exc)
    	    {
	    	strncpy(errMsg, exc->what(), 40);
	    	return 0;
    	    }
	}
	else //Remote connection
	{
	    char expr[256];
	    sprintf(expr, "MakeSegment(\'%s\', $, $, MAKE_RANGE($, $, $), $, -1, %d)", path, nVals);
	    Data *args[6];
	    args[0] = start;
	    args[1] = end;
	    args[2] = new Float64(trigger + period * dataIdx);
	    args[3] = new Float64(trigger + period * (dataIdx + nVals) - 0.1* period); 
	    args[4] = new Float64(period);
	    args[5] = data;
	    try {
		Data *retData = conn->get(expr, args, 6);
		if(retData) deleteData(retData);
		deleteData(data);
		deleteData(times);
		deleteData(args[2]);
		deleteData(args[3]);
		deleteData(args[4]);
		deleteData(end);
	    }
    	    catch(MdsException *exc)
    	    {
		if(debug) printf("ERROR WRITING REMOTE TREE: %s\n", exc->what());
		strncpy(errMsg, exc->what(), 40);
		return 0;
    	    }
	}
    }
    return 1;
}

int evaluateExpr(char *expr, int treeIdx, int nBuffers, void **buffers, int *bufDims, int *bufTypes, int maxRetElements, int retType, void *retBuf, int *outElements, char *errMsg, int isLocal)
{
    int i, retElSize, retElements, dummyLocal;
    TreeNode *node;
    Tree *tree;
    Data *evaluated;
    Connection *conn;
    Data *args[8];
    char *currPath;
    void *currBuf;

//printf("EvaluateExpr: expr: %s %d %d, %d\n", expr, *(int *)buffers[0], bufDims[0], bufTypes[0]);
printf("EvaluateExpr: expr: %s, treeIdx: %d, nBuffers: %d, maxRet: %d dim1: %d dim2: %d\n", expr, treeIdx, nBuffers, maxRetElements, bufDims[0], bufDims[1]);

    try {
    	for (i = 0; i < nBuffers; i++)
    	{
	    if(bufDims[i] > 1)
	    {
	    	switch(bufTypes[i]) {
	    	    case DBF_CHAR: 
		    	args[i] = new Int8Array((char *)buffers[i], bufDims[i]);
		    	break;
	    	    case DBF_UCHAR: 
		    	args[i] = new Uint8Array((unsigned char *)buffers[i], bufDims[i]);
		    	break;
	    	    case DBF_SHORT: 
		    	args[i] = new Int16Array((short *)buffers[i], bufDims[i]);
		    	break;
	    	    case DBF_USHORT: 
		    	args[i] = new Uint16Array((unsigned short *)buffers[i], bufDims[i]);
		    	break;
	    	    case DBF_LONG: 
		    	args[i] = new Int32Array((int *)buffers[i], bufDims[i]);
		    	break;
	    	    case DBF_ULONG: 
		    	args[i] = new Uint32Array((unsigned int *)buffers[i], bufDims[i]);
		    	break;
	    	    case DBF_FLOAT: 
		    	args[i] = new Float32Array((float *)buffers[i], bufDims[i]);
		    	break;
	    	    case DBF_DOUBLE: 
		    	args[i] = new Float64Array((double *)buffers[i], bufDims[i]);
		    	break;
		}
	    }
	    else
	    {
	    	switch(bufTypes[i]) {
	    	    case DBF_CHAR: 
		    	args[i] = new Int8(*((char *)buffers[0]));
		    	break;
	    	    case DBF_UCHAR: 
		    	args[i] = new Uint8(*((char *)buffers[0]));
		    	break;
	    	    case DBF_SHORT: 
		    	args[i] = new Int16(*((short *)buffers[0]));
		    	break;
	    	    case DBF_USHORT: 
		    	args[i] = new Uint16(*((short *)buffers[0]));
		    	break;
	    	    case DBF_LONG: 
		    	args[i] = new Int32(*((int *)buffers[0]));
		    	break;
	    	    case DBF_ULONG: 
		    	args[i] = new Uint32(*((int *)buffers[0]));
		    	break;
	    	    case DBF_FLOAT: 
		    	args[i] = new Float32(*((float *)buffers[0]));
		    	break;
	    	    case DBF_DOUBLE: 
		    	args[i] = new Float64( *((double *)buffers[0]));
		    	break;
		}
	    }
    	}
	if(isLocal)
	{
    	    if(treeIdx != -1) //A Tree was defined, it must be restored
    	    {
     	        updateNode(treeIdx, &isLocal, &node, &conn, &currPath, &tree);
	    	if(nBuffers > 0)
	    	{
	      	    switch(nBuffers) {
			case 1: evaluated = executeWithArgs(expr, tree, nBuffers, args[0]); break;
			case 2: evaluated = executeWithArgs(expr, tree, nBuffers, args[0], args[1]); break;
			case 3: evaluated = executeWithArgs(expr, tree, nBuffers, args[0], args[1], args[2]); break;
			case 4: evaluated = executeWithArgs(expr, tree, nBuffers, args[0], args[1], args[2], args[3]); break;
			case 5: evaluated = executeWithArgs(expr, tree, nBuffers, args[0], args[1], args[2], args[3], args[4]); break;
			case 6: evaluated = executeWithArgs(expr, tree, nBuffers, args[0], args[1], args[2], args[3], args[4], args[5]); break;
			case 7: evaluated = executeWithArgs(expr, tree, nBuffers, args[0], args[1], args[2], args[3], args[4], args[5], args[6]); break;
			case 8: evaluated = executeWithArgs(expr, nBuffers, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]); break;
	      	    }
	    	}
	    	else
		    evaluated = execute(expr, tree);
	    }
	    else //no tree open
	    {
	    	if(nBuffers > 0)
	    	{
	      	    switch(nBuffers) {
			case 1: evaluated = executeWithArgs(expr, nBuffers, args[0]); break;
			case 2: evaluated = executeWithArgs(expr, nBuffers, args[0], args[1]); break;
			case 3: evaluated = executeWithArgs(expr, nBuffers, args[0], args[1], args[2]); break;
			case 4: evaluated = executeWithArgs(expr, nBuffers, args[0], args[1], args[2], args[3]); break;
			case 5: evaluated = executeWithArgs(expr, nBuffers, args[0], args[1], args[2], args[3], args[4]); break;
			case 6: evaluated = executeWithArgs(expr, nBuffers, args[0], args[1], args[2], args[3], args[4], args[5]); break;
			case 7: evaluated = executeWithArgs(expr, nBuffers, args[0], args[1], args[2], args[3], args[4], args[5], args[6]); break;
			case 8: evaluated = executeWithArgs(expr, nBuffers, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]); break;
	      	    }
	    	}
	    	else
		    evaluated = execute(expr);
	    }
	}
	else //remote
	{
     	    updateNode(treeIdx, &dummyLocal, &node, &conn, &currPath, &tree); //Retrieve connection
	    if(!conn)
	    {
		printf("Remote connection not established\n");
		strcpy(errMsg, "Remote connection not established");
		return 0;
	    }

	    if(nBuffers > 0)
	    {
		evaluated = conn->get(expr, args, nBuffers);
		//printf("Valuto : %s %s %d Risultato %s\n", expr, args[0]->decompile(), nBuffers, evaluated->decompile()); 
	    }
	    else
		evaluated = conn->get(expr);
	}
	
	if(evaluated->getSize() == 1) //If scalar
	{
	    retElements = 1;
	    switch(retType) {
	    	case DBF_CHAR: 
	    	case DBF_UCHAR: 
		    *((char *)retBuf) = evaluated->getByte();
		    break;
	    	case DBF_SHORT: 
	    	case DBF_USHORT: 
		    *((short *)retBuf) = evaluated->getShort();
		    break;
	    	case DBF_LONG: 
	    	case DBF_ULONG: 
		    *((int *)retBuf) = evaluated->getInt();
		    break;
	    	case DBF_FLOAT: 
		    *((float *)retBuf) = evaluated->getFloat();
		    break;
	    	case DBF_DOUBLE: 
		    *((double *)retBuf) = evaluated->getDouble();
		    break;
	    }
	}
	else //Array
	{
	    switch(retType) {
	    	case DBF_CHAR: 
	    	case DBF_UCHAR: 
		    currBuf = evaluated->getByteArray(&retElements);
	    	    if(retElements > maxRetElements)
			retElements = maxRetElements;
	      	    memcpy(retBuf, currBuf, retElements);
		    deleteNativeArray((char *)currBuf);
		    break;
	    	case DBF_SHORT: 
	    	case DBF_USHORT: 
		    currBuf = evaluated->getShortArray(&retElements);
	    	    if(retElements > maxRetElements)
			retElements = maxRetElements;
	      	    memcpy(retBuf, currBuf, 2 * retElements);
		    deleteNativeArray((short *)currBuf);
		    break;
	    	case DBF_LONG: 
	    	case DBF_ULONG: 
		    currBuf = evaluated->getIntArray(&retElements);
	    	    if(retElements > maxRetElements)
			retElements = maxRetElements;
	      	    memcpy(retBuf, currBuf, 4 * retElements);
		    deleteNativeArray((int *)currBuf);
		    break;
	    	case DBF_FLOAT: 
		    currBuf = evaluated->getFloatArray(&retElements);
	    	    if(retElements > maxRetElements)
			retElements = maxRetElements;
	      	    memcpy(retBuf, currBuf, 4 * retElements);
		    deleteNativeArray((float *)currBuf);
		    break;
	    	case DBF_DOUBLE: 
		    currBuf = evaluated->getDoubleArray(&retElements);
	    	    if(retElements > maxRetElements)
			retElements = maxRetElements;
	      	    memcpy(retBuf, currBuf, 8 * retElements);
		    deleteNativeArray((double *)currBuf);
		    break;
	    }
	}
	*outElements = retElements;
	deleteData(evaluated);
	for(i = 0; i < nBuffers; i++)
	    deleteData(args[i]);

    }
    catch(MdsException *exc)
    {
	printf("ERROR WRITING REMOTE TREE: %s\n", exc->what());
	strncpy(errMsg, exc->what(), 40);
	return 0;
    }
    return 1;
}


int registerMdsEvent(char *eventName)
{
    return addEvent(eventName);
}
void waitMdsEvent(char *eventName, char *buf, int maxLen, int *retLen)
{
    int size;
    char *evBuf;

    EventHandler *event = getEvent(eventName);
    if(!event)
    {
	printf("EVENT NOT FOUND!!!: %s\n", eventName);
	*retLen = 0;
	return;
    }

    evBuf = event->getBuffer(&size);
    if(size > maxLen)
    {
	printf("Event buffer truncated from %d to %d bytes\n", size, maxLen);
	size = maxLen;
    }
    memcpy(buf, evBuf, size);
    *retLen = size;
    delete[] evBuf;
}

int doMdsAction(char *path, int nodeId, char *errMsg)
{
    TreeNode *node;
    Tree *tree;
    Connection *conn;
    int isLocal;
    char *currPath;
    char *expr = new char[strlen(path) + 16];

    sprintf(expr, "tcl(\'do %s\')", path);
    updateNode(nodeId, &isLocal, &node, &conn, &currPath, &tree);
    if(!node && !conn)
    {
	strcpy(errMsg, "Internal error: node ID not found");
    	return 0;
    }
//Build the data objects
    if(isLocal)
    {
	try {
	    Data *data = compile(expr, node->getTree());
	    Data *resData = data->data();
	    //deleteData(data);
	    
	    
	   // printf("Returned Data: %s\n", (resData)?resData->decompile():"");
	    //if(resData) deleteData(resData);
	}
   	catch(MdsException *exc)
    	{
	    printf("ERROR EXECUTING ACTION: %s\n", exc->what());
	    strncpy(errMsg, exc->what(), 40);
	    return 0;
    	}
    }
    else
    {
	try {
	    Data *resData = conn->get(expr);
	    //printf("Returned Data: %s\n", (resData)?resData->decompile():"");
	    if(resData)deleteData(resData);
	}
   	catch(MdsException *exc)
    	{
	    printf("ERROR EXECUTING REMOTE ACTION: %s\n", exc->what());
	    strncpy(errMsg, exc->what(), 40);
	    return 0;
    	}
    }
    delete [] expr;
    return 1;
}


epicsRegisterFunction(doMdsAction);
epicsRegisterFunction(writeMds);
epicsRegisterFunction(openMds);
epicsRegisterFunction(registerMdsEvent);
epicsRegisterFunction(waitMdsEvent);
epicsRegisterFunction(evaluateExpr);
