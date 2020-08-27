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
#include <pthread.h>
#include <mdsobjects.h>
#include <time.h>
#include <string.h>

#ifdef _MSC_VER
inline int nanosleep(const struct timespec *req, struct timespec *rem __attribute__ ((unused)))
{
	DWORD sleep_ms = ((DWORD)req->tv_sec * 1000) + ((DWORD)req->tv_nsec / 1000000);
	Sleep(sleep_ms);
	return 0;
}
#endif

extern "C" {
EXPORT int registerListener(char *expr, char *tree, int shot);
EXPORT void unregisterListener(int listenerId);
EXPORT void *getNewSamplesSerializedXd();
}

static 	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static bool daemonStarted = false;
static pthread_t thread;

extern  "C" void *monitorStreamInfo(void *);
//static MDSplus::TreeNode *getSegmentedNode(MDSplus::Data *data);
static MDSplus::Data *getNewSamplesSerialized();
//Condition variable for synchronization
static pthread_cond_t availCond;

/////////////////////////////////////////////////////////////////////
/// Class StreamInfo records information related to a given data item in streaming.
/// StreamInfo instances are collected in StreamInfoV vector which is used for both periodi check/update
/// and for data readout. StreamInfo instances share the same condition variable used to signal
/// data availability. New StreamInfo instances are created by subsequent registerListener calls.
/// A client registers first for srteaming by calling resisterListener with the following parameters:
/// - The expression to be evaluated in streaming. This expression will contain at least a segmented node reference
/// - The experiment name
/// - The shot number
/// It returns an integer identifier which is is subsequently used when reading data chunks.
/// When called the first time, method registerListener will activate a thread periodically polling the involved segmented
/// tree nodes in order to detect whether new data are available. The availability of new data for a given data item,
/// is recorded in the corresponding dataAvailable flag, and the associated condition variable is signaled.
/// Clients will call in a loop method getNewSamplesSerialized(). This method checks first the availability of data
/// recorded in  StreamInfo instance. If data are not available in any instance, the calling process waits for the associated
/// condition variable. When awakened, there will for sure at least one data item containing new data.
/// The (set of) signals are returned in the form of a serialized APD data item. There will be as many APD descriptor pair
/// as the number of data items with new data. The first descriptor of the pair will contain the id of the corresponding
/// data item, and the second one will contain a signal describing sampels and time of the new data.
/// \note A call to getNewSamplesSerialized() is a blocking one, i.e. no other remote expression evaluation can be performed
/// in the meantime. This means that no other remote expression evaluation hould be issued for a given mdsip connection.
/// In thic case, after registering for a number of signals, the client should repeatedly call getNewSamplesSerialised().
/// Observe also that it is assumed that the mdsip connection is served
/// by xinetd on the server side, so that a separate process is created for every connection.
/////////////////////////////////////////////////////////////////////
class StreamInfo
{
	MDSplus::Tree *tree;            ///< Tree reference
	MDSplus::TreeNode *segNode;     ///< Reference segmented node, always included in compiledExpr tree
	MDSplus::Data *compiledExpr;    ///< Epression tree including a segmented node
	int lastSegment;				///< Last acxcessed segment number
	int lastNextRow;				///< Last index of next free row in the segment
	bool dataAvailable;				///< New data available flag
	MDSplus::Data *lastTime;		///< Time of the last sample streamed so far

public:
	///
	StreamInfo(MDSplus::Tree *tree, MDSplus::TreeNode *segNode, MDSplus::Data *compiledExpr)
	{
		this->tree = tree;
		this->segNode = segNode;
		this->compiledExpr = compiledExpr;
		lastSegment = 0;
		lastNextRow = 0;
		dataAvailable = false;
		lastTime = NULL;
	}
	~StreamInfo()
	{
		deleteData(compiledExpr);
		if(lastTime)
			deleteData(lastTime);
		delete tree;
	}
	bool isDataAvailable() {return dataAvailable;}
/// Check availability of new data. First the current number of segments is checked.
/// If not changed, the index of the next free row is checked
	void checkDataAvailable()
	{
		char dtype, dimct;
		int dims[MAX_DIMS], nextRow;
	    if(dataAvailable) return;  	//Data previously available
		int numSegments = segNode->getNumSegments();
		if(numSegments == 0)		//No data yet
			return;
		if(numSegments != lastSegment) //First condition: number of segments has increased
		{
			segNode->getSegmentInfo(numSegments-1, &dtype, &dimct, dims, &nextRow);
			lastNextRow = nextRow;
//Do not signal condition in which number of segments has increased but no row has nee writen in the new segment
//			if(numSegments > lastSegment +1 && nextRow > 0)
			if(numSegments > lastSegment +1 || nextRow > 0)
				dataAvailable = true;
			lastSegment = numSegments;
			if(dataAvailable)
				pthread_cond_signal(&availCond); //Awake pending threadd
			return;
		}
		segNode->getSegmentInfo(lastSegment-1, &dtype, &dimct, dims, &nextRow); //Second condition:
		if(nextRow > lastNextRow)  //Second condition: new samples added to current segment
		{
			lastNextRow = nextRow;
			dataAvailable = true;
			pthread_cond_signal(&availCond);
		}
	}

///Get new data for this data item.
	MDSplus::Signal *getNewSamples()
	{
		if(!dataAvailable) return NULL;  //No new data available
		MDSplus::Tree *tree = segNode->getTree();   //Use Time context to retrieve data not sttreamed yet
		tree->setTimeContext(lastTime, NULL, NULL); //Use Time context to retrieve data not sttreamed yet
		MDSplus::Data *segData = segNode->getData();//Read segmented data in order to retrieve dimension
		unsigned char clazz, dtype;
		if(!segData)
		{
			dataAvailable = false;
			return NULL;
		}
	segData->getInfo((char *)&clazz, (char *)&dtype);
		if(clazz != CLASS_R || dtype != DTYPE_SIGNAL)   //Data read from segment must be a signal
		{
			std::cout << "Internal error: non signal returned from segmented node" << std::endl;
		    MDSplus::deleteData(segData);
			dataAvailable = false;
			return NULL;
		}
		MDSplus::Signal *sig = (MDSplus::Signal *)segData;
		MDSplus::Data *sigData;
		MDSplus::Data *dimension = sig->getDimension();   //Get dimension samples
		MDSplus::Data *evalDimension = dimension->data();
		MDSplus::deleteData(dimension);
		if(compiledExpr != NULL) //If an expression is defined, use it to retrieve data samples
			sigData = compiledExpr->data();
		else
			sigData = segData->data(); //Otherwise use segmented data
		MDSplus::deleteData(segData);
		sigData->getInfo((char *)&clazz, (char *)&dtype);
		if(clazz != CLASS_A )     //It must be an array
		{
			std::cout << "Internal error: not an array of samples returned" << std::endl;
		    MDSplus::deleteData(sigData);
			MDSplus::deleteData(evalDimension);
			dataAvailable = false;
			return NULL;
		}
		evalDimension->getInfo((char *)&clazz, (char *)&dtype); //Dimensions must be an aray
		if(clazz != CLASS_A)
		{
			std::cout << "Internal error: not an array of times returned" << std::endl;
		    MDSplus::deleteData(sigData);
			MDSplus::deleteData(evalDimension);
			dataAvailable = false;
			return NULL;
		}
		//Throw first useless sample if not the first chunk.
		MDSplus::Data *retData, *retDim;
		if(lastTime)
		{
			retData = ((MDSplus::Array *)sigData)->getSubArray(1, sigData->getSize() - 1);
			retDim = ((MDSplus::Array *)evalDimension)->getSubArray(1, evalDimension->getSize() - 1);
			deleteData(sigData);
			deleteData(evalDimension);
		}
		else
		{
			retData = sigData;
			retDim = evalDimension;
		}
		if(retDim->getSize() > 0)
		{
			if(lastTime)
				MDSplus::deleteData(lastTime);
		//Record last sample time. It will be used in the next turn a start time in time context
			lastTime = ((MDSplus::Array *)retDim)->getElementAt(retDim->getSize() - 1);
		}
		MDSplus::Signal *retSig = new MDSplus::Signal(retData,NULL, retDim);  //Build a signal from evaluated samples and times
		dataAvailable = false;
		return retSig;
    }
};  //End class StreamInfo

///Retrieve from the passed expression tree the first reference to a segmented node.If none found, return NULL
static MDSplus::TreeNode *getSegmentedNode(MDSplus::Data *data, MDSplus::Tree *tree)
{
	unsigned char dtype, clazz;
	if(!data) return NULL;
	data->getInfo((char *)&clazz, (char *)&dtype);
	switch(clazz)
	{
		case CLASS_S:
			if(dtype == DTYPE_NID && !strcmp(((MDSplus::TreeNode *)data)->getUsage(), "SIGNAL"))
				return (MDSplus::TreeNode *)data;
			if(dtype == DTYPE_PATH)  //Evaluate path data
			{
				try {
					char *pathName = ((MDSplus::TreePath *)data)->getString();
					MDSplus::TreeNode *node = tree->getNode(pathName);
					delete [] pathName;
					if(!strcmp(((MDSplus::TreeNode *)data)->getUsage(), "SIGNAL"))
						return node;
					else
					{
						delete node;
						return NULL;
					}
				}catch(MDSplus::MdsException &exc){return NULL;}
			}
			return NULL;
		case CLASS_A:  //Do not consider APDs here
		case CLASS_D:
			return NULL;
		case CLASS_R:  //Recursive search
			MDSplus::Compound *compound = (MDSplus::Compound *)data;
			for(int i = 0; i < compound->getNumDescs(); i++)
			{
				MDSplus::TreeNode *retNode = getSegmentedNode(compound->getDescAt(i), tree);
				if(retNode) return retNode;
			}
			return NULL;
	}
	return NULL;
}
//The vedtor of StreamInfo instances
static std::vector<StreamInfo *> streamInfoV;

/// Register a new listener
EXPORT int registerListener(char *expr, char *experiment, int shot)
{
	pthread_mutex_lock(&mutex);
	try {
		MDSplus::Tree *tree = new MDSplus::Tree(experiment, shot);
		MDSplus::Data *exprData = MDSplus::compile(expr, tree);
		MDSplus::TreeNode *segNode = getSegmentedNode(exprData, tree);
		if(!segNode)  //A reference segmented node is required
		{
			MDSplus::deleteData(exprData);
			delete tree;
			pthread_mutex_unlock(&mutex);
			return -1;
		}

		StreamInfo *si = new StreamInfo(tree, segNode, exprData);
		streamInfoV.push_back(si);
		if(!daemonStarted)  //Start monitoring thread if not yet started
		{
			pthread_cond_init(&availCond, NULL);
			daemonStarted = true;
			pthread_create(&thread, NULL, monitorStreamInfo, NULL);
		}
		pthread_mutex_unlock(&mutex);
// Return the index in streamInfo array
		return streamInfoV.size() - 1;
	}catch(MDSplus::MdsException &exc)
	{
		pthread_mutex_unlock(&mutex);
		return -1;
	}
}

/// Unregister a data listener. Deallocate corresponding StreamInfo instance but do not remove it from
/// StreamInfoV array in order to preserve index consistence
EXPORT void unregisterListener(int listenerId)
{
  if(listenerId < 0 || (std::size_t)listenerId >= streamInfoV.size())
		return;
	pthread_mutex_lock(&mutex);
	StreamInfo *si = streamInfoV[listenerId];
	if(si) delete si;
	streamInfoV[listenerId] = NULL;
	pthread_mutex_unlock(&mutex);
}


///Retrieve new samples for at least one data item. Possibly wait until new data are available for a data item
///The passed id may refer to any data item sharing the same context (condition variable)
MDSplus::Data *getNewSamplesSerialized()
{

	//First loop: check whether any data is available
	pthread_mutex_lock(&mutex);
	bool dataAvailable = false;
	for(std::size_t idx = 0; idx < streamInfoV.size(); idx++)
	{
		StreamInfo *currSi = streamInfoV[idx];
		if(currSi && currSi->isDataAvailable())
		{
			dataAvailable = true;
			break;
		}
	}
//If no new data available in any data item sharing the same  context, wait the associated condition variable.
	if(!dataAvailable)
	{
		struct timespec waitTime = {0,0};
		int waitSeconds = 5;  //Exit from wait every 5 seconds. This gives a chance to processes created by xinetd to exit
		waitTime.tv_sec = time(NULL)+waitSeconds;
		//pthread_cond_wait(&availCond, &mutex);
		pthread_cond_timedwait(&availCond, &mutex, &waitTime);
	}
	//Second loop: read available data. For sure at leas one item will contain new data
	MDSplus::Apd *retApd = new MDSplus::Apd();
	for(std::size_t idx = 0; idx < streamInfoV.size(); idx++)
	{
		StreamInfo *currSi = streamInfoV[idx];
		if(!currSi || !currSi->isDataAvailable())
			continue;
		MDSplus::Data *newData = currSi->getNewSamples();
		if(newData)
		{
			retApd->appendDesc(new MDSplus::Int32(idx));
			retApd->appendDesc(newData);
			//NOTE newData is not deallocated because APD::appendDesc does not increment reference counter
		}
	}
	//Serialized resulting APD
	int numBytes;
	char *serialized = retApd->serialize(&numBytes);
	deleteData(retApd);
	MDSplus::Int8Array *retArr = new MDSplus::Int8Array(serialized, numBytes);
	delete[] serialized;
	pthread_mutex_unlock(&mutex);
	return retArr;
}


//Returns a XD pointer
EXPORT void *getNewSamplesSerializedXd()
{
	MDSplus::Data *serialized = getNewSamplesSerialized();
	void *descrPtr = serialized->convertToDsc();
	MDSplus::deleteData(serialized);
	return descrPtr;
}


///Periodically check the list of registered data items for new data availability
void *monitorStreamInfo(void *par __attribute__ ((unused)))
{
	struct timespec waitTime;
	waitTime.tv_sec = 0;
	waitTime.tv_nsec = 100000000; //100 ms
//	std::cout << "PARTE MONITOR\n";
	while(true)
	{
		pthread_mutex_lock(&mutex);
		for(std::size_t i = 0; i < streamInfoV.size(); i++)
		{
			if(streamInfoV[i])
				streamInfoV[i]->checkDataAvailable();
		}
		pthread_mutex_unlock(&mutex);
		nanosleep(&waitTime, NULL);
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class StreamEvents provides an alternative streaming solution. It provides a set of methods for sending chuncks of data as MDSplus events. Events will be       //
// recorded by node.js application using ServerSent Events for streaming visualization and/or by registered StreamEvent listeners. 
// The coding of the message exchanged is the following:
// Shot number: ASCII integer
// Channel number: String
// Mode: a single char with the following code:
// - F: times sent as ASCII floats, Samples sent as ASCII floats
// - L: times sent as ASCII int64, Samples sent as ASCII floats
// - A: Same as F: recongnized by stream visualizer to doiplsy in oscilloscope mode instead of a chart
// - B: Times and Samples sent as serialized MDSplus expressions
// Number of elements (for modes F, L, A) or number of bytes of serialized times (for mode B)
// For all modes except B: ASCII times and values
// For mode B only: serialized times followed by serialized values
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
using namespace MDSplus;

EXPORT void EventStream::send(int shot, const char *name, float time, float sample)
{
    char msgBuf[256];
    sprintf(msgBuf, "%d %s F 1 %f %f", shot, name, time, sample);
    //ASCII coding: <shot> <name> [F|L] <numSamples> <xval>[ xval]* <yval>[ <yval>]*  where F and L indicate floating or integer times, respectrively
    Event::setEventRaw("STREAMING", strlen(msgBuf), msgBuf);
}

EXPORT void EventStream::send(int shot, const char *name, uint64_t time, float sample)
{
    char msgBuf[256];
    sprintf(msgBuf, "%d %s L 1 %lu %f", shot, name, (unsigned long)time, sample);
    Event::setEventRaw("STREAMING", strlen(msgBuf), msgBuf);
}

EXPORT void EventStream::send(int shot, const char *name, int numSamples, float *times, float *samples, bool oscilloscopeMode)
{
    char *msgBuf = new char[numSamples * 64 + 256];
    if(oscilloscopeMode)
        sprintf(msgBuf, "%d %s A %d", shot, name, numSamples);
    else
        sprintf(msgBuf, "%d %s F %d", shot, name, numSamples);
    for(int i = 0; i < numSamples; i++)
    {
	sprintf(&msgBuf[strlen(msgBuf)], " %f", times[i]);
    }
    for(int i = 0; i < numSamples; i++)
	sprintf(&msgBuf[strlen(msgBuf)], " %f", samples[i]);
    Event::setEventRaw("STREAMING", strlen(msgBuf), msgBuf);
    delete [] msgBuf;
}

EXPORT void EventStream::send(int shot, const char *name, int numSamples, uint64_t *times, float *samples)
{
    char *msgBuf = new char[numSamples * 64 + 256];
    sprintf(msgBuf, "%d %s L %d", shot, name, numSamples);
    for(int i = 0; i < numSamples; i++)
    {
	sprintf(&msgBuf[strlen(msgBuf)], " %lu", (unsigned long)times[i]);
    }
    for(int i = 0; i < numSamples; i++)
	sprintf(&msgBuf[strlen(msgBuf)], " %f", samples[i]);
    Event::setEventRaw("STREAMING", strlen(msgBuf), msgBuf);
    delete [] msgBuf;
}

EXPORT void EventStream::send(int shot, const char *name, Data *timesD, Data *samplesD)
{
    int nTimesSer = 0, nSamplesSer = 0;
    char *timesSer = timesD->serialize(&nTimesSer);
    char *samplesSer = samplesD->serialize(&nSamplesSer);
    char *msgBuf = new char[nTimesSer +  nSamplesSer + 256];
    char headBuf[256];
    sprintf(headBuf, "%d %s B %d ", shot, name, nTimesSer);
    int headerLen = strlen(headBuf);
    memcpy(msgBuf, headBuf, headerLen);
    memcpy(&msgBuf[headerLen], timesSer, nTimesSer);
    memcpy(&msgBuf[headerLen+nTimesSer], samplesSer, nSamplesSer);
    Event::setEventRaw("STREAMING", headerLen+nTimesSer+nSamplesSer, msgBuf);
    delete [] msgBuf;
    delete[] timesSer;
    delete [] samplesSer;
}


EXPORT void EventStream::run()
{
    const char *eventName = getName(); //Get the name of the event
    if(strcmp(eventName, "STREAMING")) return; //Should neve return
    size_t bufSize;
    const char *buf =  getRaw(&bufSize); //Get raw data
    char *str = new char[bufSize+1]; //Make it a string
    memcpy(str, buf, bufSize);
    str[bufSize] = 0;
    int shot, numSamples;
    char timeFormat[16], name[256];
    //std::cout << "RECEIVED EVENT " << eventName << " WITH DATA  " << str << std::endl;
    int readItems = sscanf(str, "%d %s %s %d", &shot, name, timeFormat, &numSamples);
    if(readItems < 4)
    {
	delete [] str;
	return; //Incorrect message
    }
    //skip to fourth blank
    int len = strlen(str);
    int j = 0;
    for(int i = 0; j < len && i < 4; i++)
    {
	while(j < len && str[j] != ' ')
	    j++;
	if(j == len)
	{
	    delete [] str;
	    return; //Incorrect message
	}
	j++;
    }
    Data *timesD;
    if(timeFormat[0] == 'F')
    {
	float *times = new float[numSamples];
	for(int i = 0; i < numSamples; i++)
	{
	    sscanf(&str[j], "%f", &times[i]);
	    while(j < len && str[j] != ' ')
		j++;
	    if(i < numSamples && j == len)
	    {
		delete [] times;
		delete [] str;
		return; //Incorrect message
	    }
	    j++;
	}
	if(numSamples > 1)
	    timesD = new Float32Array(times, numSamples);
	else
	    timesD = new Float32(times[0]);
	delete [] times;
    }
    else if (timeFormat[0] != 'B')
    {
	uint64_t *times = new uint64_t[numSamples];
	for(int i = 0; i < numSamples; i++)
	{
	    sscanf(&str[j], "%lu", (unsigned long *)&times[i]);
	    while(j < len && str[j] != ' ')
		j++;
	    if(i < numSamples - 1 && j == len)
	    {
		delete [] times;
		delete [] str;
		return; //Incorrect message
	    }
	    j++;
	}
	if(numSamples > 1)
	    timesD = new Uint64Array(times, numSamples);
	else
	    timesD = new Uint64(times[0]);
	delete [] times;
    }
    else //Mode B
    {
	Data *timesD  = deserialize(&str[j]);
	Data *samplesD  = deserialize(&str[j+numSamples]);
	//Expressions already available, do not need any further action
	std::string nameStr(name);
	for(size_t i = 0; i < listeners.size(); i++)
	{
	    if(names[i] == nameStr)
		listeners[i]->dataReceived(samplesD, timesD, shot);
	}
	deleteData(samplesD);
	deleteData(timesD);
	delete[] str;
	return;
    }
    Data *samplesD;
    float *samples = new float[numSamples];
    for(int i = 0; i < numSamples; i++)
    {
	sscanf(&str[j], "%f", &samples[i]);
	while(j < len && str[j] != ' ')
	    j++;
	if(i < numSamples - 1 && j == len)
	{
	    delete [] samples;
	    delete [] str;
	    deleteData(timesD);
	    return; //Incorrect message
	}
	j++;
    }
    if(numSamples > 1)
	samplesD = new Float32Array(samples, numSamples);
    else
	samplesD = new Float32(samples[0]);

    delete [] samples;
    std::string nameStr(name);

    for(size_t i = 0; i < listeners.size(); i++)
    {
	if(names[i] == nameStr)
	    listeners[i]->dataReceived(samplesD, timesD, shot);
    }
    deleteData(samplesD);
    deleteData(timesD);
}


EXPORT void EventStream::registerListener(DataStreamListener *listener, const char *name)
{
    listeners.push_back(listener);
    names.push_back(std::string(name));
}
