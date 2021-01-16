#include "AsyncStoreManager.h"

pthread_mutex_t globalMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t segmentMutex = PTHREAD_MUTEX_INITIALIZER;
  
//Support class for enqueueing storage requests
/*
class SaveItem {
    void *buffer;
    char dataType;
    int bufSize;
    int sampleToRead;
    int segmentSize;
    int counter;
    int dataNid;
    int resampledNid;
    int clockNid;
    float timeIdx0;
    void *treePtr;
    SaveItem *nxt;
    char *eventName;
    int streamFactor;
    double period;
    TreeNode *dataNode;
    TreeNode *clockNode;
    TreeNode *resampledNode;
    char *streamName;
    float streamGain;
    float streamOffset;
    int shot;
    int gain;
    int numCoeffs;
    float *coeffs;
 public:
*/
SaveItem::SaveItem(void *buffer, int bufSize, int sampleToRead, char dataType, int segmentSize, int counter, int dataNid, int clockNid, 
                   float timeIdx0, void *treePtr, int shot, int streamFactor, char *streamName, float streamGain, float streamOffset, 
                   double period, float gain, float *coeffs, int numCoeffs, int resampledNid = -1)
    {
	this->buffer = buffer;
	this->dataType = dataType;
	this->bufSize = bufSize;
	this->sampleToRead = sampleToRead;
	this->segmentSize = segmentSize;
	this->counter = counter;
	this->dataNid = dataNid;
	this->resampledNid = resampledNid;
	this->clockNid = clockNid;
	this->timeIdx0 = timeIdx0;
	this->treePtr = treePtr;
	this->shot = shot;
	this->streamFactor = streamFactor;
	this->streamName = streamName;
 	this->streamGain = streamGain;
	this->streamOffset = streamOffset;
	this->period = period;
	this->gain = gain;
	this->numCoeffs = numCoeffs;
	this->coeffs = coeffs;
	nxt = 0;
	dataNode = new TreeNode(dataNid, (Tree *)treePtr);
	clockNode = new TreeNode(clockNid, (Tree *)treePtr);
	resampledNode = NULL;
	if(resampledNid > 0)
	    resampledNode = new TreeNode(resampledNid, (Tree *)treePtr);

    }

/***********
    void setNext(SaveItem *itm)
    {
	nxt = itm;
    }

    SaveItem *getNext()
    {
	return nxt;
    }
    int getShot() { return shot;}
    Tree *getTree() { return(Tree *)treePtr;}
    int getDataNid() {return dataNid;}

 
    vector<string> split(const char *str, char c = ' ')
    {
        vector<string> result;

        do
        {
	    const char *begin = str;

	    while(*str != c && *str)
	        str++;

	    result.push_back(string(begin, str));
        } while (0 != *str++);

        return result;
    }

    void sendChannelSegmentPutEvent(TreeNode *dataNode)
    {
	try
	{
	    char *path;
	    vector<string> tokens;
	    char event[256];
	    char treeStr[64];
		
	    path=dataNode->getPath();
	    tokens = split((const char *)path, ':');
	    sprintf(treeStr,"%s", (tokens[0].substr(1)).data());

	    tokens = split(tokens[tokens.size()-2].data(), '.');
	    sprintf(event,"%s_%s_CH%s", treeStr, tokens[0].data(), (tokens[1].substr(8)).data());
  	    Event::setevent(event);
	}	
	catch(MdsException *exc) 
	{
	    printf("Send Event Error: %s\n", exc->what());
	}
    }
***********************/

void SaveItem::save()
    {
//Streaming stuff

	if(streamName && streamFactor > 0 && (counter % streamFactor) == 0)
	{

	    //Computation of value to be streamed
	    try {
		Data *nidData = new Int32(dataNid);
		int sampleInterval = (int)(0.1/period);
		if(sampleInterval < 1)
		    sampleInterval = 1;
		int numSamples = bufSize / sampleInterval;
		if(numSamples < 1)
		    numSamples = 1;
//printf("NUM SAMPLES: %d  SAMPLE INTERVAL: %d\n", numSamples, sampleInterval);
		float *samples = new float[numSamples];
		float *times = new float[numSamples];
		int actSamples = 0;
		for(int sampleIdx = 0; sampleIdx < numSamples; sampleIdx++)
		{
		    if(sampleIdx * sampleInterval >= bufSize)
		    break;
		    float sample = (float)((dataType == SHORT)?((short *)buffer)[sampleIdx * sampleInterval]:((float *)buffer)[sampleIdx * sampleInterval]);

		    float scaled = coeffs[numCoeffs - 1];
		    for(int c = numCoeffs - 2; c >= 0; c--)
		    {
		        scaled *= sample; 
			scaled += coeffs[c];
		    }
		    scaled /= gain;

		    scaled = scaled * streamGain + streamOffset;
//		    Data *sampleData = new Float32(rawSample);
//		    Data *streamValD = executeWithArgs("NiConvertStream($1,$2)", (Tree *)treePtr, 2, nidData, sampleData);
//		    float sample = streamValD->getFloat(); 
//		    samples[actSamples] = sample;
		    samples[actSamples] = scaled;
//		    deleteData(sampleData);
//		    deleteData(streamValD);
		    times[actSamples] = period * (counter + actSamples * sampleInterval) + timeIdx0;
		    actSamples++; 
		}
		//printf("STREAM %d %s %d %f %f\n", shot, streamName, actSamples, times[0], samples[0]);
	    	EventStream::send(shot, streamName, actSamples, times, samples, false);
	    	//EventStream::send(shot, streamName, (float)(period * counter + timeIdx0), sample);
		delete[] samples;
		delete [] times;

	    }catch(MdsException &exc)
	    {
		printf("Cannot convert stream sample: %s\n", exc.what());
	    }
	}
	    


//////////////////////
	//printf("Counter = %d Sample to read = %d\n", counter, sampleToRead );	
	//if((counter % segmentSize) == 0 || ((int)(counter / segmentSize) * segmentSize) < counter + bufSize )
	if( (counter % segmentSize) == 0 )
	{
           //Create Segment
	    Data *startIdx = new Int32(counter);
	    Data *endIdx;
            //In Transient Record acquisition mode must set
            //the last segment with the correct size
	    if( sampleToRead > 0 && sampleToRead  < segmentSize )
	        segmentSize = sampleToRead;

	    endIdx = new Int32(counter + segmentSize - 1);
                   
	    Data *startTime;
	    Data *endTime;
	    Data *dim;
            Tree *tree = (Tree *)treePtr;
	    if( timeIdx0 != timeIdx0 ) //is a NaN float
	    {
		//printf("Configuration for gclock\n");
		//printf("---------------- time at idx 0 NAN\n");
                /*
                startTime = tree->tdiCompile("begin_of($)+slope_of($)*$", clockNode, clockNode,startIdx);
                endTime = tree->tdiCompile("begin_of($)+slope_of($)*$", clockNode, clockNode, endIdx);
	    	dim = tree->tdiCompile("build_range(begin_of($)+slope_of($)*$, begin_of($)+slope_of($)*($), slope_of($))", 
	       				clockNode, clockNode, startIdx, clockNode, clockNode, endIdx, clockNode);
                */
		startTime = compileWithArgs("NIADCClockSegment($1, $2, $3, 0, 'start_time')",(Tree *)treePtr, 3, clockNode, startIdx, endIdx);
	    	endTime   = compileWithArgs("NIADCClockSegment($1, $2, $3, 0, 'end_time')",  (Tree *)treePtr, 3, clockNode, startIdx, endIdx);
	    	dim       = compileWithArgs("NIADCClockSegment($1, $2, $3, 0, 'dim')",       (Tree *)treePtr, 3, clockNode, startIdx, endIdx);



	    }
	    else
	    {    
	    	Data *timeAtIdx0 = new Float32(timeIdx0);
                /*
	    	startTime = tree->tdiCompile("$+slope_of($)*$", timeAtIdx0, clockNode, startIdx);
	    	endTime = tree->tdiCompile("$+slope_of($)*$",timeAtIdx0,  clockNode, endIdx);
	    	dim = tree->tdiCompile("build_range($+slope_of($)*$, $+slope_of($)*($), slope_of($))", timeAtIdx0, clockNode,
                                        startIdx, timeAtIdx0, clockNode, endIdx, clockNode);
                */
		startTime = compileWithArgs("NIADCClockSegment($1, $2, $3, $4, 'start_time')",(Tree *)treePtr, 4, clockNode, startIdx, endIdx, timeAtIdx0);
	    	endTime   = compileWithArgs("NIADCClockSegment($1, $2, $3, $4, 'end_time')",  (Tree *)treePtr, 4, clockNode, startIdx, endIdx, timeAtIdx0);
	    	dim       = compileWithArgs("NIADCClockSegment($1, $2, $3, $4, 'dim')",       (Tree *)treePtr, 4, clockNode, startIdx, endIdx, timeAtIdx0);


	    }
	    switch( dataType )
	    {
		case SHORT:
		{
		    short *fBuf = new short[segmentSize];
		    memset(fBuf, 0, sizeof(short) * segmentSize);
		    Int16Array *fData = new Int16Array((short *)fBuf, segmentSize);
		    pthread_mutex_lock(&segmentMutex);
		    try {
		    	if(resampledNode)
			    dataNode->beginSegmentMinMax(startTime, endTime, dim, fData, resampledNode, 100);
			    //dataNode->beginSegment(startTime, endTime, dim, fData);
		    	else
			    dataNode->beginSegment(startTime, endTime, dim, fData);
		    }  catch(MdsException &exc) {
			printf("BEGIN SEGMENT FAILED FOR NODE %s: %s\n", dataNode->getFullPath(), exc.what());
		    }
		    pthread_mutex_unlock(&segmentMutex);
		    delete [] fBuf;
		    deleteData(fData);
		}
		break;
		case FLOAT:
		{
		    float *fBuf = new float[segmentSize];
		    memset(fBuf, 0, sizeof(float) * segmentSize);
		    Float32Array *fData = new Float32Array((float *)fBuf, segmentSize);
		    pthread_mutex_lock(&segmentMutex);
		    try {
		    	if(resampledNode)
			    dataNode->beginSegmentMinMax(startTime, endTime, dim, fData, resampledNode, 100);
			    //dataNode->beginSegment(startTime, endTime, dim, fData);
		        else
			    dataNode->beginSegment(startTime, endTime, dim, fData);
		    }  catch(MdsException &exc) {
			printf("BEGIN SEGMENT FAILED FOR NODE %s: %s\n", dataNode->getFullPath(), exc.what());
		    }
		    pthread_mutex_unlock(&segmentMutex);
		    delete [] fBuf;
		    deleteData(fData);
		}
		break;
	    }
	    deleteData(startIdx);
	    deleteData(endIdx);
	    deleteData(startTime);
	    deleteData(endTime);


       	}	

	try 
	{
	    switch( dataType )
	    {
		case SHORT:
		{
//printf("Short Save data %s counter %d\n", dataNode->getPath(), counter );
			Int16Array *data = new Int16Array((short *)buffer, bufSize);

			pthread_mutex_lock(&segmentMutex);
			try  {
			    if(resampledNode)
			        dataNode->putSegmentMinMax(data, -1, resampledNode, 100);
			        //dataNode->putSegment(data, -1);
			    else
			        dataNode->putSegment(data, -1);
			} catch(MdsException &exc)
			{
			    printf("PUT SEGMENT FAILED FOR NODE: %s: %s\n", dataNode->getFullPath(), exc.what());
			}
			pthread_mutex_unlock(&segmentMutex);
			deleteData(data);
 			delete[] (short *)buffer;
		}
		break;
		case FLOAT:
		{
//printf("Float Save data %s counter %d\n", dataNode->getPath(), counter );
			Float32Array *data = new Float32Array((float *)buffer, bufSize);
			pthread_mutex_lock(&segmentMutex);
			try  {
			    if(resampledNode)
			        dataNode->putSegmentMinMax(data, -1, resampledNode, 100);
			        //dataNode->putSegment(data, -1);
			    else
			        dataNode->putSegment(data, -1);
			} catch(MdsException &exc)
			{
			    printf("PUT SEGMENT FAILED FOR NODE: %s: %s\n", dataNode->getFullPath(), exc.what());
			}
			pthread_mutex_unlock(&segmentMutex);
			deleteData(data);
 			delete[] (float *)buffer;
		}
		break;
	    }

	    /* Send Event on Segment update <TreeName>_<DeviceNodeName>_CH<numchannel>*/
	    //sendChannelSegmentPutEvent(dataNode);

	 } 
	 catch(MdsException *exc) 
	 {
	    printf("Cannot put segment: %s\n", exc->what());
	 }
	 delete dataNode;
	 delete clockNode;
    }

/* 

extern "C" void *handleSave(void *listPtr);

class SaveList
{
	public:
		pthread_cond_t itemAvailable;
		pthread_t thread;
		bool threadCreated;
		SaveItem *saveHead, *saveTail;
		bool stopReq;
		pthread_mutex_t mutex;
	public:
*/
SaveList::SaveList()
    {
		int status = pthread_mutex_init(&mutex, NULL);
		pthread_cond_init(&itemAvailable, NULL);
		saveHead = saveTail = NULL;
		stopReq = false;
		threadCreated = false;
    }
/*
    int getQueueLen()
    {
        pthread_mutex_lock(&mutex);
	int len = 0;
        SaveItem *currItem = saveHead;
	while(currItem)
	{
	    len++;
	    currItem = currItem->getNext();
	}
        pthread_mutex_unlock(&mutex);
	return len;
    }
**/
void SaveList::addItem(void *buffer, int bufSize, int sampleToRead, char dataType, int segmentSize, int counter, int dataNid,
                       int clockNid, float trigTime, void *treePtr, int shot, int streamFactor, char *streamName, 
                       float streamGain, float streamOffset, double period, float gain, float *coeffs, int numCoeffs, 
                       int resampledNid)

    {



		SaveItem *newItem = new SaveItem(buffer, bufSize, sampleToRead, dataType, segmentSize, counter, dataNid, clockNid, trigTime, treePtr, 
                                                 shot, streamFactor, streamName, streamGain, streamOffset, period, gain, coeffs, numCoeffs, resampledNid);
		pthread_mutex_lock(&mutex);

		if(saveHead == NULL)
			saveHead = saveTail = newItem;
		else
		{
			saveTail->setNext(newItem);
			saveTail = newItem;
		}
		pthread_cond_signal(&itemAvailable);
		pthread_mutex_unlock(&mutex);

#ifdef DEBUG_QUEUE
		reportQueueLen(dataNid, treePtr, getQueueLen(), shot);
#endif

    }
void SaveList::executeItems()
    {
		while(true)
		{
			pthread_mutex_lock(&mutex);
			if(stopReq && saveHead == NULL)
			{
			    pthread_mutex_unlock(&mutex);
			    pthread_exit(NULL);
			}

			while(saveHead == NULL)
			{
			    pthread_cond_wait(&itemAvailable, &mutex);
			    if(stopReq && saveHead == NULL)
			    {
				    pthread_mutex_unlock(&mutex);
				    pthread_exit(NULL);
			    }
	    /*
			    int nItems = 0;
			    for(SaveItem *itm = saveHead; itm; itm = itm->getNext(), nItems++);
			    if(nItems > 2) printf("THREAD ACTIVATED: %d items pending\n", nItems);
	    */
			}
			SaveItem *currItem = saveHead;
			saveHead = saveHead->getNext();

	/*
			int nItems = 0;
			for(SaveItem *itm = saveHead; itm; itm = itm->getNext(), nItems++);
			if(nItems > 2) printf("THREAD ACTIVATED: %d items pending\n", nItems);
	*/
			pthread_mutex_unlock(&mutex);

			reportQueueLen(currItem->getDataNid(), currItem->getTree(), getQueueLen(), currItem->getShot());

			currItem->save();
			delete currItem;
		}
    }
void SaveList::start()
    {
		pthread_create(&thread, NULL, handleSave, (void *)this);
		threadCreated = true;
    }
void SaveList::stop()
    {
		stopReq = true;
		pthread_cond_signal(&itemAvailable);
		if(threadCreated)
		{	
			pthread_join(thread, NULL);
			printf("SAVE THREAD TERMINATED\n");
		}
    }




//Queue length streaming support 
void reportQueueLen(int dataNid, void *treePtr, int len, int shot)
{
    static long prevTimeMs = 0;
    static int prevShot = -1;
    static unordered_map<int, long> prevTimes;  
    static long startTimeMs;   
    long currTimeMs;
    struct timespec retTime;
    int debug = 0;
    
    clock_gettime(CLOCK_REALTIME, &retTime);
    currTimeMs = retTime.tv_sec*1000+retTime.tv_nsec/1000000;

    pthread_mutex_lock(&globalMutex);
    if(shot != prevShot)
    {
	prevTimes.clear();
	prevShot = shot;
	startTimeMs = currTimeMs;
    }

 
    if (prevTimes.count(dataNid) > 0 && currTimeMs <= prevTimes[dataNid])
    {
	pthread_mutex_unlock(&globalMutex);
	return;
    }
    prevTimes[dataNid] = currTimeMs;
    pthread_mutex_unlock(&globalMutex);

    TreeNode *currNode = new TreeNode(dataNid, (Tree *)treePtr);
    while (currNode->getNid() > 0 && strcmp(currNode->getUsage(), "DEVICE"))
    {
	TreeNode *prevNode = currNode;
	currNode = currNode->getParent();
	delete prevNode;
    }
    if(currNode->getNid() == 0)
    {
	if (debug) std::cout << "INTERNAL ERROR: wrong data nid to reportQueueLen" << std::endl;
	return;
    }
    StringArray *tags = currNode->findTags();
    if(tags->getSize() < 1)
    {
	if (debug) std::cout << "WARNING: no tags for data node  to reportQueueLen" << std::endl;
	return;
    }
    String *tagData = (String *)tags->getElementAt(0);
    deleteData(tags);
    char *tag = tagData->getString();
    deleteData(tagData);
    char *buf = new char[strlen(tag)+7];
    sprintf(buf, "%s_QUEUE", tag);
    delete [] tag;
    EventStream::send(shot, buf, (float)((currTimeMs-startTimeMs)/1000.), (float)len);
    delete [] buf;
}



void *handleSave(void *listPtr)
{
    SaveList *list = (SaveList *)listPtr;
    list->executeItems();
    return NULL;
}

void startSave(void **retList)
{
    SaveList *saveList = new SaveList;
    saveList->start();
    *retList = (void *)saveList;
}

void stopSave(void *listPtr)
{
    if(listPtr) 
    {
        SaveList *list = (SaveList *)listPtr;
        list->stop();
        delete list;
    }
}
