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
#include <errno.h>
#include <string.h>
#include <math.h>
#include <semaphore.h>
#include <sys/ioctl.h>

#include <pxi-6259-lib.h>
#include <xseries-lib.h>

#include <stdlib.h>
#include <stdio.h>
#include <mdsobjects.h>
#include <vector>


using namespace MDSplus;

extern "C" void pxi6259_create_ai_conf_ptr(void **confPtr);
extern "C" void pxi6259_free_ai_conf_ptr(void *conf);

extern "C" void xseries_create_ai_conf_ptr(void **confPtr, unsigned int pre_trig_smp, unsigned int post_trig_smp, char multi_trigger);
extern "C" void xseries_free_ai_conf_ptr(void *conf);
extern "C" int  _xseries_get_device_info(int fd, void *cardInfo);
extern "C" int getErrno();
extern "C" int xseries_set_ai_conf_ptr(int fd,  void *conf_ptr);
extern "C" int xseriesReadAndSaveAllChannels(int nChan, void *chanFdPtr, int bufSize, int segmentSize, int sampleToSkip, int numSamples, void *dataNidPtr, int clockNid, float timeIdx0, float period, void *treePtr, void *saveListPtr, void *stopAcq);
extern "C" int xseries_AI_scale(int16_t *raw, float *scaled, uint32_t num_samples, float * coeff);

extern "C" void openTree(char *name, int shot, void **treePtr);
extern "C" int readAndSave(int chanFd, int bufSize, int segmentSize, int counter, int dataNid, int clockNid, void *treePtr, void *saveList);
extern "C" int readAndSaveAllChannels(int nChan, void *chanFdPtr, int bufSize, int segmentSize, int numSamples, void *dataNidPtr, int clockNid, void *treePtr, void *saveListPtr, void *stopAcq);


extern "C" void readAiConfiguration(int fd);


extern "C" int pxi6259_readAndSaveAllChannels(int nChan, void *chanFdPtr, int bufSize, int segmentSize, int sampleToSkip, int numSamples, void *dataNidPtr, int clockNid, float timeIdx0, float period, void *treePtr, void *saveListPtr, void *stopAcq);

extern "C" int pxi6259EV_readAndSaveAllChannels(int nChan, void *chanFdPtr, int *isBurst, int *f1Div, int *f2Div, double maxDelay, double baseFreq, double *preTimes, double *postTimes, double startTime, int bufSize, int segmentSize, char **eventNames, void *dataNidPtr, void *treePtr, void *saveListPtr, void *stopAcq);



extern "C" void getStopAcqFlag(void **stopAcq);
extern "C" void freeStopAcqFlag(void *stopAcq);
extern "C" void setStopAcqFlag(void *stopAcq);

void pxi6259_create_ai_conf_ptr(void **confPtr)
{
    pxi6259_ai_conf_t *conf = (pxi6259_ai_conf_t *)malloc(sizeof(pxi6259_ai_conf_t));
    *conf = pxi6259_create_ai_conf();
    printf( "==== aiExportStartTrigSig %d\n", conf->aiExportStartTrigSig);

    *confPtr = (void *) conf;
}

void pxi6259_free_ai_conf_ptr(void *conf)
{
    free((char *)conf);
}

int _xseries_get_device_info(int fd, void *cardInfo)
{
    xseries_device_info_t *data = (xseries_device_info_t *)cardInfo;

    //printf("file descriptor %d\n", fd);
    //printf("card info %x\n", cardInfo);

    if ( xseries_get_device_info(fd, data) < 0 )
    {
	printf("Error %d %s\n", errno, strerror(errno));
	return -1;
    }

/*
    printf( "product_id %d\n", data->product_id);
    printf( "device_name %s\n", data->device_name);
    printf( "is_simultaneous %d\n", data->is_simultaneous);
    printf( "is_pcie %d\n", data->is_pcie);
    printf( "adc_number %d\n", data->adc_number);
    printf( "dac_number %d\n", data->dac_number);
    printf( "port0_length %d\n", data->port0_length);
    printf( "serial_number %d\n", data->serial_number);
*/

    return 0;
}

int getErrno()
{
    return errno;
}


void xseries_create_ai_conf_ptr(void **confPtr, unsigned int pre_trig_smp, unsigned int post_trig_smp, char multi_trigger)
{
    xseries_ai_conf_t *conf = (xseries_ai_conf_t *)malloc(sizeof(xseries_ai_conf_t));

//Software-timed data acquisition
//    *conf = xseries_software_timed_ai();int xseries_set_ai_conf_ptr(int fd,  void *conf_ptr)

	if( post_trig_smp > 0 )
	{
	    if( pre_trig_smp > 0)
	    {
		     printf("PXI 6368 Ptretrigger analog input acquisition. pre %d post %d \n", pre_trig_smp , post_trig_smp);
		     *conf = xseries_reference_ai(pre_trig_smp, post_trig_smp);
	    }
	    else
	    {
		    if(multi_trigger)
	            {
			    printf("PXI 6368 Retriggerable finite analog input acquisition. %d \n", post_trig_smp);
			    *conf = xseries_retriggerable_finite_ai(post_trig_smp);
		    }
	            else
	            {
			    printf("PXI 6368 Finite analog input acquisition. %d \n", post_trig_smp);
			    *conf = xseries_finite_ai(post_trig_smp);
	            }
	     }
	}
	else
	{
		printf("Continuous analog input acquisition.\n");
		*conf = xseries_continuous_ai();
	}


	*confPtr = (void *) conf;
}

int xseries_set_ai_conf_ptr(int fd,  void *conf_ptr)
{
    int retval;

    xseries_ai_conf_t conf;
    memcpy(&conf, conf_ptr, sizeof(xseries_ai_conf_t));

    retval = xseries_load_ai_conf(fd, *( xseries_ai_conf_t *)conf_ptr);
	if(retval) {
		printf("PXI 6368 Cannot load AI configuration! %s (%d)\n",strerror(errno),errno);
		return -1;
	}

    // wait for descriptors
    sleep(1);

    return 0;

}

void xseries_free_ai_conf_ptr(void *conf)
{
    free((char *)conf);
}


void openTree(char *name, int shot, void **treePtr)
{
    try {
	Tree *tree = new Tree(name, shot);
	*treePtr = (void *)tree;
    }catch(MdsException *exc)
    {
	printf("Cannot open tree %s %d: %s\n", name, shot, exc->what());
    }
}

#define SHORT 1
#define FLOAT 2


//Support class for enqueueing storage requests
class SaveItem {
    void *buffer;
	char dataType;
    int bufSize;
    int sampleToRead;
    int segmentSize;
    int counter;
    int dataNid;
    int clockNid;
    float timeIdx0;
    void *treePtr;
    SaveItem *nxt;
 public:
    SaveItem(void *buffer, int bufSize, int sampleToRead, char dataType, int segmentSize, int counter, int dataNid, int clockNid, float timeIdx0, void *treePtr)
    {
		this->buffer = buffer;
		this->dataType = dataType;
		this->bufSize = bufSize;
		this->sampleToRead = sampleToRead;
		this->segmentSize = segmentSize;
		this->counter = counter;
		this->dataNid = dataNid;
		this->clockNid = clockNid;
		this->timeIdx0 = timeIdx0;
		this->treePtr = treePtr;
		nxt = 0;
    }

    void setNext(SaveItem *itm)
    {
		nxt = itm;
    }

    SaveItem *getNext()
    {
		return nxt;
    }

    void save()
    {
		TreeNode *dataNode = new TreeNode(dataNid, (Tree *)treePtr);
		TreeNode *clockNode = new TreeNode(clockNid, (Tree *)treePtr);

		//printf("Counter = %d Sample to read = %d\n", counter, sampleToRead );

		//if((counter % segmentSize) == 0 || ((int)(counter / segmentSize) * segmentSize) < counter + bufSize )
		if( (counter % segmentSize) == 0 )
		{
		//Create Segment
			Data *startIdx = new Int32(counter);
			Data *endIdx;
	    //In Transient Record acquisition mode must set
	    //the last segment with the correct size
	    if( sampleToRead > 0 && sampleToRead < segmentSize )
	        endIdx = new Int32(counter + sampleToRead - 1);
	    else
	        endIdx = new Int32(counter + segmentSize);

	 /*
	    Data *startTime = compileWithArgs("$[$]", (Tree *)treePtr, 2, clockNode, startIdx);
			Data *endTime = compileWithArgs("$[$]", (Tree *)treePtr, 2, clockNode, endIdx);
			Data *dim = compileWithArgs("$[$ : $]", (Tree *)treePtr, 3, clockNode, startIdx, endIdx);
	 */
	    Data *startTime;
			Data *endTime;
			Data *dim;

	    if( timeIdx0 != timeIdx0 ) //is a NaN float
	    {
	        //printf("Configuration for gclock\n");
	        //printf("---------------- time at idx 0 NAN\n");

			    startTime = compileWithArgs("begin_of($)+slope_of($)*$",(Tree *)treePtr, 3, clockNode, clockNode,startIdx);
			    endTime = compileWithArgs("begin_of($)+slope_of($)*$",(Tree *)treePtr, 3, clockNode,  clockNode,endIdx);

	    //    dim = compileWithArgs("$", (Tree *)treePtr, 1, clockNode);
			    dim = compileWithArgs("build_range(begin_of($)+slope_of($)*$, begin_of($)+slope_of($)*$, slope_of($))",
	                       (Tree *)treePtr, 7, clockNode, clockNode, startIdx, clockNode, clockNode, endIdx, clockNode);
	    }
	    else
	    {
	        Data *timeAtIdx0 = new Float32(timeIdx0);
			    startTime = compileWithArgs("$+slope_of($)*$",(Tree *)treePtr, 3, timeAtIdx0, clockNode, startIdx);
			    endTime = compileWithArgs("$+slope_of($)*$",(Tree *)treePtr, 3,timeAtIdx0,  clockNode, endIdx);
			    dim = compileWithArgs("build_range($+slope_of($)*$, $+slope_of($)*$, slope_of($))", (Tree *)treePtr,
	                                     7, timeAtIdx0, clockNode, startIdx, timeAtIdx0, clockNode, endIdx, clockNode);
	    }


			switch( dataType )
			{
				case SHORT:
				{
					short *fBuf = new short[segmentSize];
					memset(fBuf, 0, sizeof(short) * segmentSize);
					Int16Array *fData = new Int16Array((short *)fBuf, segmentSize);
					dataNode->beginSegment(startTime, endTime, dim, fData);
					delete [] fBuf;
					deleteData(fData);
				}
				break;
				case FLOAT:
				{
					float *fBuf = new float[segmentSize];
					memset(fBuf, 0, sizeof(float) * segmentSize);
					Float32Array *fData = new Float32Array((float *)fBuf, segmentSize);
					dataNode->beginSegment(startTime, endTime, dim, fData);
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
					Int16Array *data = new Int16Array((short *)buffer, bufSize);
					dataNode->putSegment(data, -1);
					deleteData(data);
					delete[] (short *)buffer;
				}
				break;
				case FLOAT:
				{
					Float32Array *data = new Float32Array((float *)buffer, bufSize);
					dataNode->putSegment(data, -1);
					deleteData(data);
					delete[] (float *)buffer;
				}
				break;
			}
		 }
		 catch(MdsException *exc)
		 {
			printf("Cannot put segment: %s\n", exc->what());
		 }
		 delete dataNode;
		 delete clockNode;
    }

};

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
    SaveList()
    {
		int status = pthread_mutex_init(&mutex, NULL);
		pthread_cond_init(&itemAvailable, NULL);
		saveHead = saveTail = NULL;
		stopReq = false;
		threadCreated = false;
    }
    void addItem(void *buffer, int bufSize, int sampleToRead, char dataType, int segmentSize, int counter, int dataNid, int clockNid, float trigTime, void *treePtr)
    {
		SaveItem *newItem = new SaveItem(buffer, bufSize, sampleToRead, dataType, segmentSize, counter, dataNid, clockNid, trigTime, treePtr);
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
    }
    void executeItems()
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
			currItem->save();
			delete currItem;
		}
    }
    void start()
    {
		pthread_create(&thread, NULL, handleSave, (void *)this);
		threadCreated = true;
    }
    void stop()
    {
		stopReq = true;
		pthread_cond_signal(&itemAvailable);
		if(threadCreated)
		{
			pthread_join(thread, NULL);
			printf("SAVE THREAD TERMINATED\n");
		}
    }
 };


extern "C" void *handleSave(void *listPtr)
{
    SaveList *list = (SaveList *)listPtr;
    list->executeItems();
    return NULL;
}

extern "C" void startSave(void **retList)
{
    SaveList *saveList = new SaveList;
    saveList->start();
    *retList = (void *)saveList;
}

extern "C" void stopSave(void *listPtr)
{
    if(listPtr)
    {
	SaveList *list = (SaveList *)listPtr;
	list->stop();
	delete list;
    }
}

#define PXI6259_MAX_BUFSIZE 1000
#define MAX_ITERATIONS 100000
int readAndSave(int chanFd, int bufSize, int segmentSize, int counter, int dataNid, int clockNid, void *treePtr, void *saveListPtr)
{
    float *buffer = new float[bufSize];
    int readSamples = 0;
    int currReadSamples;
    int currIterations = 0;
    while(readSamples < bufSize)
    {
	if(bufSize - readSamples > PXI6259_MAX_BUFSIZE)
	    currReadSamples = pxi6259_read_ai(chanFd, &buffer[readSamples], PXI6259_MAX_BUFSIZE);
	else
	    currReadSamples = pxi6259_read_ai(chanFd, &buffer[readSamples], bufSize - readSamples);
	if(currReadSamples <=0)
	{
	if( currReadSamples < 0 )
	    printf("pxi6259_read_ai failed %d\n", errno);

	    currReadSamples = 0;
	}
	readSamples += currReadSamples;
	if(currReadSamples == 0)
	{
	    currIterations++;
	    if(currIterations >= MAX_ITERATIONS)
		    return currReadSamples;
	}
    }
    SaveList *saveList = (SaveList *)saveListPtr;
    saveList->addItem(buffer, bufSize, -1, FLOAT, segmentSize, counter, dataNid, clockNid, NAN, treePtr);
    return readSamples;
}

void readAiConfiguration(int fd)
{
    int status;
    pxi6259_ai_conf_t conf;


    status = pxi6259_read_ai_conf(fd, &conf);
    if( status < 0 )
	return;

    //printf("FOUTtimebaseSelect %d\n", conf.FOUTtimebaseSelect);
    //printf("FOUTdivider %d\n", conf.FOUTdivider);
    //printf("SlowInternalTimebase %d\n", conf.SlowInternalTimebase);
    //printf("SIsource %d\n", conf.SIsource);

    printf("aiSamplePeriodDivisor %d\n", conf.aiSamplePeriodDivisor);
    printf("aiSampleDelayDivisor %d\n", conf.aiSampleDelayDivisor);
    printf("aiConvertPeriodDivisor %d\n", conf.aiConvertPeriodDivisor);
    printf("aiConvertDelayDivisor %d\n", conf.aiConvertDelayDivisor);
    //printf(" %d", conf.);

}

int readAndSaveAllChannels(int nChan, void *chanFdPtr, int bufSize, int segmentSize, int numSamples, void *dataNidPtr, int clockNid, void *treePtr, void *saveListPtr, void *stopAcq)
{
    //float *buffer = new float[bufSize];
    float *buffer;
    int   *counters = new int[nChan];

    int readSamples = 0;
    int sampleToRead = 0;

    int currReadSamples;
    int currIterations = 0;
    int chan;
    SaveList *saveList = (SaveList *)saveListPtr;
    int *chanFd = (int *)chanFdPtr;
    int *dataNid =  (int *)dataNidPtr;
    bool allDataSaved;
    bool transientRec = false;

    printf("nChan %d bufSize %d segmentSize %d numSamples %d stopAcq %d\n", nChan, bufSize, segmentSize, numSamples, *(int *)stopAcq );

    memset(counters, 0, sizeof(int) * nChan);

    if( (*(int*)stopAcq) == 1)
	transientRec = false;

    (*(int*)stopAcq) = 0;

    while( !(*(int*)stopAcq) )
    {
	allDataSaved = true;
	//printf("stopAcq %d\n", *(int *)stopAcq );

	for( chan = 0; chan < nChan; chan++)
	{
	    readSamples = 0;
	    currIterations = 0;
	    buffer = new float[bufSize];

	    //printf("chanFd %d dataNid %d counters %d\n", chanFd[chan], dataNid[chan], counters[chan] );
	    //printf("counters %d Num samples %d\n",  counters[chan],  numSamples);

	    if(numSamples < 0 || counters[chan] < numSamples)
	    {
	        while(readSamples < bufSize)
	        {
		        if(bufSize - readSamples > PXI6259_MAX_BUFSIZE)
		            currReadSamples = pxi6259_read_ai(chanFd[chan], &buffer[readSamples], PXI6259_MAX_BUFSIZE);
		        else
		            currReadSamples = pxi6259_read_ai(chanFd[chan], &buffer[readSamples], bufSize - readSamples);
		        if(currReadSamples <=0)
		        {
	                //if( currReadSamples < 0 )
	                //    printf("pxi6259_read_ai failed %d chan %d \n", errno, chan);

		            currReadSamples = 0;
		        }
		        readSamples += currReadSamples;
		        if(currReadSamples == 0)
		        {
		            currIterations++;
		            if(currIterations >= MAX_ITERATIONS)
			    {
				    counters[chan] += currReadSamples;
				    //printf("Max Iteration\n");
	                	    if( transientRec && readSamples == 0 )
	                               return -1;
				    break;
					    //return currReadSamples;
			    }
		        }
	        }
	        //printf("readSamples %d chan %d numSamples %d counters %d\n", readSamples, chan, numSamples, counters[chan]);
	        //printf("readSamples %d counters[%d] = %d readSamples %d\n", readSamples, chan, counters[chan], readSamples );
	        if( readSamples != 0 )
	        {
				//if( numSamples > 0 ) readSamples = 	counters[chan] + readSamples < numSamples ? readSamples : numSamples - counters[chan];
			    if( numSamples > 0 )
	            {
	                if ( counters[chan] + readSamples > numSamples )
	                    readSamples = numSamples - counters[chan];
	                sampleToRead = numSamples - counters[chan];
	            }


	            //printf("readSamples %d counters[%d] = %d readSamples %d\n", readSamples, chan, counters[chan], readSamples );

//                  saveList->addItem(buffer, bufSize, segmentSize, counters[chan], dataNid[chan], clockNid, treePtr);
	            saveList->addItem(buffer, readSamples, sampleToRead, FLOAT, segmentSize, counters[chan], dataNid[chan], clockNid, NAN, treePtr);
	            counters[chan] += readSamples;
	        }
	        allDataSaved = false;
	        //return readSamples;
	    }
	}
	if( allDataSaved )
	    break;
    }
    printf("STOP C Acquisition %d\n", *(int *)stopAcq );
    return 1;
}

extern "C" int  pxi6259_getCalibrationParams(int chanfd, int range, float *coeff);
int pxi6259_getCalibrationParams(int chanfd, int range, float *coeffVal)
{
    ai_scaling_coefficients_t ai_coefs;
    int32_t i, j;
    int retval;

    retval = pxi6259_get_ai_coefficient(chanfd, &ai_coefs);
    //retval = ioctl(chanfd, PXI6259_IOC_GET_AI_SCALING_COEF, &ai_coefs);
    if (retval) return retval;


    printf("order %d\n", ai_coefs.order);
    printf("scale_converter_select_t %d\n", ai_coefs.converter);
    for( i = 0; i < 4; i++)
	printf("value[%d].f = %e\n", i, ai_coefs.value[i].f);

    printf("mode order %d\n", ai_coefs.mode.order);
    for( i = 0; i < 4; i++)
	printf("mode value[%d].f = %e\n", i, ai_coefs.mode.value[i].f);

    printf("interval.gain   %e\n", ai_coefs.interval.gain.f);
    printf("interval.offset %e\n", ai_coefs.interval.offset.f);


/*
    for (i = 0; i < NUM_AI_SCALING_COEFFICIENTS; ++i) {
	coeffVal[i] = ai_coefs.cal_info.modes[0].coefficients[i].f
			* ai_coefs.cal_info.intervals[range].gain.f;
	    if (i == 0) {
		    coeffVal[i] += ai_coefs.cal_info.intervals[range].offset.f;
	    }
	}
*/

/*
    for (j = NUM_AI_SCALING_COEFFICIENTS - 1; j >= 0; j--)
	 printf("Coeff[%d] : %e\n",  j, coeffVal[j] );
*/
}

int pxi6259_readAndSaveAllChannels_OLD(int nChan, void *chanFdPtr, int bufSize, int segmentSize, int numSamples, void *dataNidPtr, int clockNid, void *treePtr, void *saveListPtr, void *stopAcq)
{
    //float *buffer = new float[bufSize];
    //float *buffer;
    short *buffer;
    int   *counters = new int[nChan];

    int readSamples = 0;
    int sampleToRead = 0;

    int currReadSamples;
    int currIterations = 0;
    int chan;
    SaveList *saveList = (SaveList *)saveListPtr;
    int *chanFd = (int *)chanFdPtr;
    int *dataNid =  (int *)dataNidPtr;
    bool allDataSaved;
    bool transientRec = false;

    printf("nChan %d bufSize %d segmentSize %d numSamples %d stopAcq %d\n", nChan, bufSize, segmentSize, numSamples, *(int *)stopAcq );

    memset(counters, 0, sizeof(int) * nChan);

    if( (*(int*)stopAcq) == 1)
	transientRec = false;

    (*(int*)stopAcq) = 0;

    while( !(*(int*)stopAcq) )
    {
	allDataSaved = true;
	//printf("stopAcq %d\n", *(int *)stopAcq );

	for( chan = 0; chan < nChan; chan++)
	{
	    readSamples = 0;
	    currIterations = 0;
	    //buffer = new float[bufSize];
	    buffer = new short[bufSize];


	    //printf("chanFd %d dataNid %d counters %d\n", chanFd[chan], dataNid[chan], counters[chan] );
	    //printf("counters %d Num samples %d\n",  counters[chan],  numSamples);

	    if(numSamples < 0 || counters[chan] < numSamples)
	    {
	        while(readSamples < bufSize)
	        {
		        if(bufSize - readSamples > PXI6259_MAX_BUFSIZE)
		            currReadSamples = read(chanFd[chan], &buffer[readSamples], PXI6259_MAX_BUFSIZE << 1);
		        else
		            currReadSamples = read(chanFd[chan], &buffer[readSamples], ( bufSize - readSamples ) << 1);
		        if(currReadSamples <=0)
		        {
	                //if( currReadSamples < 0 )
	                //    printf("pxi6259_read_ai failed %d chan %d \n", errno, chan);

		            currReadSamples = 0;
		        }
		        readSamples += ( currReadSamples >> 1 );
		        if(currReadSamples == 0)
		        {
		            currIterations++;
		            if(currIterations >= MAX_ITERATIONS)
			    {
				    counters[chan] += (currReadSamples >> 1);
				    //printf("Max Iteration\n");
	                	    if( transientRec && readSamples == 0 )
	                               return -1;
				    break;
					    //return currReadSamples;
			    }
		        }
	        }
	        //printf("readSamples %d chan %d numSamples %d counters %d\n", readSamples, chan, numSamples, counters[chan]);
	        //printf("readSamples %d counters[%d] = %d readSamples %d\n", readSamples, chan, counters[chan], readSamples );
	        if( readSamples != 0 )
	        {
				//if( numSamples > 0 ) readSamples = 	counters[chan] + readSamples < numSamples ? readSamples : numSamples - counters[chan];
			    if( numSamples > 0 )
	            {
	                if ( counters[chan] + readSamples > numSamples )
	                    readSamples = numSamples - counters[chan];
	                sampleToRead = numSamples - counters[chan];
	            }


	            //printf("readSamples %d counters[%d] = %d readSamples %d\n", readSamples, chan, counters[chan], readSamples );

//                  saveList->addItem(buffer, bufSize, segmentSize, counters[chan], dataNid[chan], clockNid, treePtr);
	            saveList->addItem(buffer, readSamples, sampleToRead, SHORT, segmentSize, counters[chan], dataNid[chan], clockNid, NAN, treePtr);
	            counters[chan] += readSamples;
	        }
	        allDataSaved = false;
	        //return readSamples;
	    }
	}
	if( allDataSaved )
	    break;
    }
    printf("STOP 6259 C Acquisition %d\n", *(int *)stopAcq );
    return 1;
}


#define XSERIES_MAX_BUFSIZE 40000
extern "C" int  getCalibrationParams(int chanfd, int range, float *coeff);


void getStopAcqFlag(void **stopAcq)
{
    *stopAcq = (void *)malloc(sizeof(int));
}

void freeStopAcqFlag(void *stopAcq)
{
    free((char *)stopAcq);
}

void setStopAcqFlag(void *stopAcq)
{
    (*(int*)stopAcq) = 1;
}

// Check calibration channel

/*
 * Kernel does not support floating point directly...
 */


int xseries_AI_scale(int16_t *raw, float *scaled, uint32_t num_samples, float * coeff)
{
    int32_t i, j;
    float rawf;
/*
    for (j = NUM_AI_SCALING_COEFFICIENTS - 1; j >= 0; j--)
	 printf("Coeff[%d] : %e\n", j, coeff[j] );
*/

    for (i = 0; i < num_samples; i++) {
	scaled[i] = 0.0;
	rawf = (double)raw[i];
	for (j = NUM_AI_SCALING_COEFFICIENTS - 1; j >= 0; j--) {
	    scaled[i] *= rawf;
	    scaled[i] += coeff[j];
	}
	//printf("ai sample scaled: %d %e \n", raw[i], scaled[i]);
    }
}

/*
inline static void ai_scale(int16_t *raw, float *scaled,
	        uint32_t num_samples, xseries_ai_scaling_coef_t *scaling)
{
	int32_t i, j;
	float rawf;
	float val;

	for (i = 0; i < num_samples; i++) {
	        scaled[i] = 0.0;
	        rawf = (double)raw[i];
	        for (j = NUM_AI_SCALING_COEFFICIENTS - 1; j >= 0; j--) {
	            scaled[i] *= rawf;
	            scaled[i] += scaling->c[j].f;
	        }
	}
}
*/

int getCalibrationParams(int chanfd, int range, float *coeffVal)
{
    xseries_ai_scaling_coef_t ai_coefs;
    int32_t i, j;
    int retval;


/*

Nuova versione 5.0 codac ma sembra non funzionare

     retval = get_ai_scaling_coefficient(chanfd, &ai_coefs, range);
     if (retval) {
	   printf("Get ai scaling error %s \n", strerror(errno));
	   return retval;
     }
*/

     retval = ioctl(chanfd, XSERIES_IOC_GET_AI_SCALING_COEF, &ai_coefs);
     if (retval) {
	   printf("Get ai scaling error %s\n", strerror(errno));
	   return retval;
     }

     for (i = 0; i < NUM_AI_SCALING_COEFFICIENTS; ++i) {
	coeffVal[i] = ai_coefs.cal_info.modes[0].coefficients[i].f
			* ai_coefs.cal_info.intervals[range].gain.f;
	    if (i == 0) {
		    coeffVal[i] += ai_coefs.cal_info.intervals[range].offset.f;
	    }
	}

/*
    for (j = NUM_AI_SCALING_COEFFICIENTS - 1; j >= 0; j--)
	 printf("Coeff[%d] : %e\n",  j, coeffVal[j] );
*/
    return 0;
}




int xseriesReadAndSaveAllChannels(int nChan, void *chanFdPtr, int bufSize, int segmentSize, int sampleToSkip, int numSamples, void *dataNidPtr, int clockNid, float timeIdx0, float period, void *treePtr, void *saveListPtr, void *stopAcq)
{
    char saveConv = 0; // Acquisition format flags 0 raw data 1 convrted dta
    int  skipping = 0; // Number of samples to not save when start time is > 0

    int sampleToRead = 0;   //Number of sample to read
    int currDataToRead = 0; //Number of current sample to read

    int chan;
    int currReadSamples;    //Number of samples read
    SaveList *saveList = (SaveList *)saveListPtr; // Class to equeu data buffer to save in pulse file
    int *chanFd        = (int *)chanFdPtr;        // Channe file descriptor
    int *dataNid       =  (int *)dataNidPtr;      // Channel node identifier


    int             readCalls[nChan]; // For statistic number of read operation pe channel
    unsigned short* buffers_s[nChan]; // Raw data buffer used when not converted data are read
    float*          buffers_f[nChan]; // Converted data buffer uesed when converted dta are read
    int             readChanSmp[nChan]; // Numebr of semples to read from each channel
    int             bufReadChanSmp[nChan]; // Number of sample read in the buffer for each channel
    int             channelRead; // Number of channel completely read

    int triggered = 0;           // Module triggered flag
    bool transientRec  = false;	// transient recorder flag


    if (bufSize > XSERIES_MAX_BUFSIZE)  // Buffer size sets in mdsplus device is limited to module limit
	bufSize = XSERIES_MAX_BUFSIZE;

    printf("PXIe 6368 nChan %d bufSize %d segmentSize %d numSamples %d sampleToSkip %d stopAcq %d Time 0 %e Period %e SelfList %x stoAcqPtr %x\n", nChan, bufSize, segmentSize, numSamples,sampleToSkip, *(int *)stopAcq,  timeIdx0, period, saveListPtr, stopAcq);

    memset(readCalls, 0, sizeof(int) * nChan);
    memset(readChanSmp, 0, sizeof(int) * nChan);

    // stopAcq variable used to receive from phyton device the end of acquisition is also used, when funcrion is colled, to define acq mode continuous 0 transient recorder 1
    if( (*(int*)stopAcq) == 1)
	transientRec = true;
    else
	numSamples = segmentSize;

    (*(int*)stopAcq) = 0;

    //Allocate buffer for each channels
    for( chan = 0; chan < nChan; chan++ )
	if(saveConv)
	    buffers_f[chan] = new float[bufSize];
	else
	    buffers_s[chan] = new unsigned short[bufSize];

    triggered = 0;

    // Start main acquisition loop
    while( !(*(int*)stopAcq) )
    {
	//Check if there are samples to skip common for each channel.
	//sampleToSkip is an argument of the function.
	//ATTENTION : to check with external trigger
	if( !skipping && sampleToSkip > 0 )
	{
	    printf("PXI 6368 Skipping data %d Nun samples %d\n", sampleToSkip, numSamples);
	    skipping = numSamples;        // save in skipping flag the number of sample to be acuire for each channel
	    numSamples = sampleToSkip;    // save as numSample to acquire th sample to skip
	}

	//ATTENTION : why dinamic allocation of this buffer
	memset(bufReadChanSmp, 0, sizeof(int) * nChan);

	chan = 0;
	channelRead = 0;
	//Acquisition loop on each channel of numSamples values
	while( channelRead != nChan )
	{
	    //Check if for the current channel has been acquired all sample
	    //and stop acquisition has not beeen asserted
	    if( readChanSmp[chan] == numSamples || (*(int*)stopAcq) )
	    {
	        channelRead++;
	        chan = (chan + 1) % nChan;
	        continue;
	    }


	    sampleToRead = numSamples - readChanSmp[chan]; // Compute the sample to read for the current channel
	    if( sampleToRead < bufSize )
	        currDataToRead = sampleToRead;
	    else
	        currDataToRead = bufSize;

	    //read data from device based on acquisition mode converted 1 raw data 0
	    //Functions waiting for data or timeout
	    if( saveConv )
	        //number fo data to red is expresse in aamples
	        currReadSamples = xseries_read_ai(chanFd[chan], &buffers_f[chan][bufReadChanSmp[chan]], (currDataToRead - bufReadChanSmp[chan]) );
	    else
		//number of sample to read must be in byte
	        currReadSamples = read(chanFd[chan], &buffers_s[chan][bufReadChanSmp[chan]], ( currDataToRead - bufReadChanSmp[chan] ) << 1 );

	    readCalls[chan]++;

	    //if( chan == 0 )
	    //printf("bufReadChanSmp[%d] = %d  currDataToRead %d Request data %d read data %d \n",chan, bufReadChanSmp[chan] , currDataToRead, currDataToRead - bufReadChanSmp[chan], currReadSamples >> 1);

	    // Check if no data is available
	    if(currReadSamples <=0)
	    {
	        currReadSamples = 0;
	        if (errno == EAGAIN || errno == ENODATA) {
		    currReadSamples = 0; // No data currently available... Try again
	        }
	        else
	        {
	            if (errno == EOVERFLOW )
	            {
			printf("PXI 6368 Error reading samples on ai%d: (%d) %s \n", chan, errno, strerror(errno));
	                for( chan = 0; chan < nChan; chan++ )
	                    if(saveConv)
	                        delete (float *)buffers_f[chan];
	                    else
	                        delete (unsigned short *)buffers_s[chan];
	                return -2;
	            }
	        }
	    }

	    //If sample are read the module has been triggered
	    if ( !triggered && currReadSamples > 0 )
	    {
		triggered = 1;
		printf("6368 TRIGGER!!!\n");
	    }

	    //Increment current data read
	    if( saveConv )
	        bufReadChanSmp[chan] += currReadSamples;
	    else
	        bufReadChanSmp[chan] += ( currReadSamples >> 1 );


//printf("bufReadChanSmp[%d] = %d  currDataToRead %d Request data %d read data %d \n",chan, bufReadChanSmp[chan] , currDataToRead);

	    //Enqueue data to store in the pulse file
	    if( bufReadChanSmp[chan] == currDataToRead )
	    {
		//Check if have been read more than required samples
	        if ( readChanSmp[chan] + bufReadChanSmp[chan] > numSamples )
	            bufReadChanSmp[chan] = numSamples - readChanSmp[chan];

	        //Compute the number of samples to complete segment acquisition
	        sampleToRead = numSamples - readChanSmp[chan];

	        //printf("bufReadChanSmp[%d] = %d readChanSmp[%d] = %d readChanSmp[%d] = %d sampleToRead = %d\n", chan, bufReadChanSmp[chan], chan, readChanSmp[chan], chan, readChanSmp[chan], sampleToRead );

	        if(!skipping)
	        {
	           saveList->addItem(((saveConv) ? (void *)buffers_f[chan] : (void *)buffers_s[chan] ),
	                              bufReadChanSmp[chan], sampleToRead, ((saveConv) ? FLOAT  : SHORT ), segmentSize,
	                              readChanSmp[chan], dataNid[chan], clockNid, timeIdx0, treePtr);

	           //allocate new buffer to save the next segment
	           if( saveConv )
	                buffers_f[chan] = new float[bufSize];
	           else
	                buffers_s[chan] = new unsigned short[bufSize];
	        }
	        //Update the number of samples read
	        readChanSmp[chan] += bufReadChanSmp[chan];
	        //Reset the the number of sample read for the next segment
	        bufReadChanSmp[chan] = 0;
	    }
	    chan = (chan + 1) % nChan;
	}//End Segment acquisition loop for each channel

	//Reset variables for skiping samples
	if( ! (*(int*)stopAcq) && skipping )
	{
	    //printf("Data to saved %d\n", readChanSmp[0]);
	    memset(readChanSmp, 0, sizeof(int) * nChan);

	    numSamples = skipping;
	    sampleToSkip = 0;
	    sampleToRead = 0;
	    skipping = 0;
	    continue;
	}

	printf("PXI 6368 Acquired all channels\n");
	if( transientRec )
	    break;

	numSamples = readChanSmp[0] + segmentSize;

    }//End main acquisition loop


/*
    for (chan=0; chan < nChan; chan++)
	printf("readCalls[%d] = %d readChanSmp[%d] = %d \n ",chan, readCalls[chan]);
*/
    if( readChanSmp[0] == 0 )
	return -1;

    printf("PXI 6368 STOP C Acquisition %d\n", *(int *)stopAcq );
    return 1;
}

//=============================
#define PXI6259_MAX_BUFSIZE_NEW 10000

int pxi6259_readAndSaveAllChannels(int nChan, void *chanFdPtr, int bufSize, int segmentSize, int sampleToSkip, int numSamples, void *dataNidPtr, int clockNid, float timeIdx0, float period, void *treePtr, void *saveListPtr, void *stopAcq)
{
    char saveConv = 1;
    int  skipping  = 0;

    int sampleToRead   = 0;
    int currDataToRead = 0;

    int currReadSamples;
    int chan;
    SaveList *saveList  = (SaveList *)saveListPtr;
    int *chanFd 	= (int *)chanFdPtr;
    int *dataNid 	= (int *)dataNidPtr;
    bool transientRec 	= false;

    int  readCalls[nChan];

    unsigned short* buffers_s[nChan];
    float* 	    buffers_f[nChan];
    int    	    readChanSmp[nChan];
    int    	    bufReadChanSmp[nChan];
    int    	    channelRead;
    int 	    triggered = 0;

    if (bufSize > PXI6259_MAX_BUFSIZE_NEW)
	bufSize = PXI6259_MAX_BUFSIZE_NEW;

    printf("PXI 6259 nChan %d bufSize %d segmentSize %d numSamples %d stopAcq %d Time 0 %e Period %e\n", nChan, bufSize, segmentSize, numSamples, *(int *)stopAcq,  timeIdx0, period);

    memset(readCalls,   0, sizeof(int) * nChan);
    memset(readChanSmp, 0, sizeof(int) * nChan);


    //Delete first all data nids
    for(int i = 0; i < nChan; i++)
    {
	try {
	    TreeNode *currNode = new TreeNode(dataNid[i], (Tree *)treePtr);
	    currNode->deleteData();
	    delete currNode;
	}catch(MdsException &exc)
	{
	    printf("Error deleting data nodes\n");
	}
    }

    if( (*(int*)stopAcq) == 1)
	transientRec = true;
    else
	numSamples = segmentSize;

    (*(int*)stopAcq) = 0;

    for( chan = 0; chan < nChan; chan++ )
	if(saveConv)
	    buffers_f[chan] = new float[bufSize];
	else
	    buffers_s[chan] = new unsigned short[bufSize];

    triggered = 0;
    while( !(*(int*)stopAcq) )
    {

      //printf("stopAcq %d\n", *(int *)stopAcq );

	if( !skipping && sampleToSkip > 0 )
	{
	    printf("PXI 6259 Skipping data %d Nun samples %d\n", sampleToSkip, numSamples);
	    skipping = numSamples;
	    numSamples = sampleToSkip;

	}

	memset(bufReadChanSmp, 0, sizeof(int) * nChan);

	chan = 0;
	channelRead = 0;
	while( channelRead != nChan )
	{
	    if( readChanSmp[chan] == numSamples || (*(int*)stopAcq) )
	    {
	        channelRead++;
	        chan = (chan + 1) % nChan;
	        continue;
	    }

	    currDataToRead = bufSize;
	    sampleToRead = numSamples - readChanSmp[chan];
	    if( sampleToRead < bufSize )
	        currDataToRead = sampleToRead;

	    if( saveConv )
	        currReadSamples = pxi6259_read_ai(chanFd[chan], &buffers_f[chan][bufReadChanSmp[chan]], (currDataToRead - bufReadChanSmp[chan]) );
	    else
	        currReadSamples = read(chanFd[chan], &buffers_s[chan][bufReadChanSmp[chan]], (currDataToRead - bufReadChanSmp[chan]) << 1 );

	    readCalls[chan]++;

	    //printf("bufReadChanSmp[%d] = %d  currDataToRead %d Request data %d read data %d \n",chan, bufReadChanSmp[chan] , currDataToRead, currDataToRead - bufReadChanSmp[chan], currReadSamples >> 1);

	    if(currReadSamples <=0)
	    {
	        currReadSamples = 0;
	        if (errno == EAGAIN || errno == ENODATA) {
	            //usleep(50);
				currReadSamples = 0; // No data currently available... Try again
	            //continue;
	        }
	        else
	        {
	            if (errno == EOVERFLOW )
	            {
			printf("PXI 6259 Error reading samples on ai%d: (%d) %s \n", chan, errno, strerror(errno));

	                for( chan = 0; chan < nChan; chan++ )
	                    if(saveConv)
	                        delete (float *)buffers_f[chan];
	                    else
	                        delete (unsigned short *)buffers_s[chan];

	                return -2;
	            }
	        }
	    }

	    if (!triggered && currReadSamples > 0 )
	    {
		triggered = 1;
		printf("6259 TRIGGER!!!\n");
	    }

	    if( saveConv )
	    {
	        //readChanSmp[chan] += currReadSamples;
	        bufReadChanSmp[chan] += currReadSamples;
	    }
	    else
	    {
	        //readChanSmp[chan] += ( currReadSamples >> 1 );
	        bufReadChanSmp[chan] += ( currReadSamples >> 1 );
	    }


	    if( bufReadChanSmp[chan] == currDataToRead )
	    {
/*
	        if ( counters[chan] + bufReadChanSmp[chan] > numSamples )
	            bufReadChanSmp[chan] = numSamples - counters[chan];
	        sampleToRead = numSamples - counters[chan];
*/
	        if ( readChanSmp[chan] + bufReadChanSmp[chan] > numSamples )
	            bufReadChanSmp[chan] = numSamples - readChanSmp[chan];
	        sampleToRead = numSamples - readChanSmp[chan];

	        //printf("bufReadChanSmp[%d] = %d readChanSmp[%d] = %d readChanSmp[%d] = %d sampleToRead = %d\n", chan, bufReadChanSmp[chan], chan, readChanSmp[chan], chan, readChanSmp[chan], sampleToRead );

	        if(!skipping)
	        {
	           saveList->addItem(((saveConv) ? (void *)buffers_f[chan] : (void *)buffers_s[chan] ),
	                              bufReadChanSmp[chan], sampleToRead, ((saveConv) ? FLOAT  : SHORT ), segmentSize,
	                              readChanSmp[chan], dataNid[chan], clockNid, timeIdx0, treePtr);

	           if( saveConv )
	                buffers_f[chan] = new float[bufSize];
	           else
	                buffers_s[chan] = new unsigned short[bufSize];
	        }

	        //counters[chan] += bufReadChanSmp[chan];
	        readChanSmp[chan] += bufReadChanSmp[chan];
	        bufReadChanSmp[chan] = 0;
	    }
	    chan = (chan + 1) % nChan;
	}

	if( ! (*(int*)stopAcq) && skipping )
	{
	    //printf("Data to saved %d\n", readChanSmp[0]);
	    //memset(counters, 0, sizeof(int) * nChan);
	    memset(readChanSmp, 0, sizeof(int) * nChan);

	    numSamples = skipping;
	    sampleToSkip = 0;
	    sampleToRead = 0;
	    skipping = 0;
	    continue;
	}

	printf("PXI 6259 Acquired all channels %d\n", readChanSmp[0]);
	if( transientRec )
	    break;
/*
	for( chan = 0; chan < nChan; chan++ )
	    buffers_s[chan] = new unsigned short[bufSize];
	    buffers_f[chan] = new float[bufSize];
*/
	//memset(counters, 0, sizeof(int) * nChan);
	//memset(readChanSmp, 0, sizeof(int) * nChan);
	numSamples = readChanSmp[0] + segmentSize;

    }

    for( chan = 0; chan < nChan; chan++ )
	if(saveConv)
	    delete (float *)buffers_f[chan];
	else
	    delete (unsigned short *)buffers_s[chan];

/*
    for (chan=0; chan < nChan; chan++)
	printf("readCalls[%d] = %d \n ",chan, readCalls[chan]);
*/
    if( readChanSmp[0] == 0 )
	return -1;

    printf("STOP C 6259 Acquisition %d\n", *(int *)stopAcq );
    return 1;
}

/**************************************************************************************/
/*********************PROBE TEMPERATURE CONTROL ***************************************/
/**************************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>

extern "C" int configureDigital(int *chanDigitFD, uint32_t deviceNum, uint32_t outChanOnOff);
extern "C" int configureOutput(int *chanOutFD, uint32_t deviceNum, uint32_t outChanRef , uint32_t outChanOnOff);
extern "C" int configureInput(int *chanInFd, uint32_t deviceNum, uint32_t inChan[], double frequency, int numChan);
extern "C" int temperatureCtrlCommand(char *cmd);
extern "C" int temperatureProbeControl(uint32_t boardID, uint32_t *inChan, int numChan, int aiFdChanIdx, double freq, int aochan, int dochan, double tempRef );


#define DEVICE_FILE "/dev/pxi6259"

int configureDigital(int *chanDigitFD, uint32_t deviceNum, uint32_t outChanOnOff)
{
	pxi6259_dio_conf_t dioConfig;
	char filename[256];
	int devFD;

	uint32_t port0Mask = 1 << outChanOnOff;


	// open DIO file descriptor
	sprintf(filename, "%s.%u.dio", DEVICE_FILE, deviceNum);
	devFD = open(filename, O_RDWR);
	if (devFD < 0) {
	        fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
	        return -1;
	}

	// initialize DIO configuration
	dioConfig = pxi6259_create_dio_conf();

	// configure DIO P0.0 ports
	if (pxi6259_add_dio_channel(&dioConfig, DIO_PORT0, port0Mask)) {
	        fprintf(stderr, "Failed to configure DIO port 0!\n");
	        return -1;
	}


	// load DIO configuration and let it apply
	if (pxi6259_load_dio_conf(devFD, &dioConfig)) {
	        fprintf(stderr, "Loading DIO configuration failed!\n");
	        return -1;
	}

	sprintf(filename, "%s.%u.dio.%u", DEVICE_FILE, deviceNum, 0);
	*chanDigitFD = open(filename, O_RDWR | O_NONBLOCK);
	if (*chanDigitFD < 0) {
	        fprintf(stderr, "Failed to open port: %s\n", strerror(errno));
	        return -1;
	}

	return devFD;

}


int configureOutput(int *chanOutFD, uint32_t deviceNum, uint32_t outChanRef , uint32_t outChanOnOff)
{
	char filename[256];
	int i;
	pxi6259_ao_conf_t aoConfig;
	int devFD;

	// open AO file descriptor
	sprintf(filename, "%s.%u.ao", DEVICE_FILE, deviceNum);
	devFD = open(filename, O_RDWR);
	if (devFD < 0) {
	        fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
	        return -1;
	}

	// initialize AO configuration
	//aoConfig = pxi6259_create_ao_conf();

	// configure AO channel reference
	if (pxi6259_add_ao_channel(&aoConfig, outChanRef, AO_DAC_POLARITY_BIPOLAR)) {
	        fprintf(stderr, "Failed to configure channel %d reference! %s\n", outChanRef, strerror(errno));
	        return -1;
	}

	// configure AO channel on/off
	if (pxi6259_add_ao_channel(&aoConfig, outChanOnOff, AO_DAC_POLARITY_BIPOLAR)) {
	        fprintf(stderr, "Failed to configure channel %d reference! : %s\n", outChanOnOff, strerror(errno));
	        return -1;
	}

	// enable signal generation
	if (pxi6259_set_ao_attribute(&aoConfig, AO_SIGNAL_GENERATION, AO_SIGNAL_GENERATION_STATIC)) {
	        fprintf(stderr, "Failed to enable generating static signal!: %s\n", strerror(errno));
	        return -1;
	}

	// set continuous mode
	if (pxi6259_set_ao_attribute(&aoConfig, AO_CONTINUOUS, 0)) {
	        fprintf(stderr, "Failed to set continuous mode!: %s\n", strerror(errno));
	        return -1;
	}

	// load AO configuration and let it apply
	if (pxi6259_load_ao_conf(devFD, &aoConfig)) {
	        fprintf(stderr, "Failed to load output configuration! : %s\n", strerror(errno));
	        return -1;
	}

	// open file descriptor for each AO channel
	sprintf(filename, "%s.%u.ao.%u", DEVICE_FILE, deviceNum, outChanRef);
	chanOutFD[0] = open(filename, O_RDWR | O_NONBLOCK);
	if ( *chanOutFD < 0) {
	        fprintf(stderr, "Failed to open channel %u: %s\n", outChanRef, strerror(errno));
	        return -1;
	}

	sprintf(filename, "%s.%u.ao.%u", DEVICE_FILE, deviceNum, outChanOnOff);
	chanOutFD[1] = open(filename, O_RDWR | O_NONBLOCK);
	if ( *chanOutFD < 0) {
	        fprintf(stderr, "Failed to open channel %u: %s\n", outChanOnOff , strerror(errno));
	        return -1;
	}

	// start AO segment (signal generation)
	if (pxi6259_start_ao(devFD)) {
	        fprintf(stderr, "Failed to start segment!n");
	        return -1;
	}

	return devFD;
}


int configureInput(int *chanInFd, uint32_t deviceNum, uint32_t inChan[], double frequency, int numChan)
{
	int diffMapChannel[16] = {-1,0,1,2,3,4,5,6,7,16,17,18,19,20.21,22,23};
	char filename[256];
	int i;
	pxi6259_ai_conf_t aiConfig;
	int devFD;
	int period;

	// open AI file descriptor
	sprintf(filename, "%s.%u.ai", DEVICE_FILE, deviceNum);
	devFD = open(filename, O_RDWR);
	if (devFD < 0) {
	        fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
	        return -1;
	}

	// initialize AI configuration
	aiConfig = pxi6259_create_ai_conf();

	// configure AI channels 0 - 5 V differential
	for( int i = 0; i < numChan; i++)
	{

		if (pxi6259_add_ai_channel(&aiConfig, diffMapChannel[inChan[i]], AI_POLARITY_BIPOLAR, 2, AI_CHANNEL_TYPE_DIFFERENTIAL, 0))
/*
		printf("Channel conf %d\n", diffMapChannel[inChan[i]] );
		if (pxi6259_add_ai_channel(&aiConfig, diffMapChannel[inChan[i]], AI_POLARITY_BIPOLAR, 2, AI_CHANNEL_TYPE_RSE, 0))

*/		{

			fprintf(stderr, "Failed to configure channel %u\n", inChan[i]);
			return -1;
		}
	}

	// configure AI sampling clock to sample with 2 samples/second
	period = (int) ( 20000000. / frequency );
	if (pxi6259_set_ai_sample_clk(&aiConfig, period, 3, AI_SAMPLE_SELECT_SI_TC, AI_SAMPLE_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE)) {
	        fprintf(stderr, "Failed to configure AI sampling clock!\n");
	        return -1;
	}

	// load AI configuration and let it apply
	if (pxi6259_load_ai_conf(devFD, &aiConfig)) {
	        fprintf(stderr, "Failed to load input configuration! %s\n", strerror(errno) );
	        return -1;
	}

	// open file descriptor for each AI channel
	for( int i = 0; i < numChan; i++)
	{
		sprintf(filename, "%s.%u.ai.%u", DEVICE_FILE, deviceNum, diffMapChannel[inChan[i]]);
		//printf("%s\n", filename);
		chanInFd[i] = open(filename, O_RDWR | O_NONBLOCK);
		if (chanInFd[i] < 0) {
			fprintf(stderr, "Failed to open channel %u: %s\n", inChan[i], strerror(errno));
			return -1;
		}
	}

	// start AI segment (data acquisition)
	if (pxi6259_start_ai(devFD)) {
		fprintf(stderr, "Failed to start data acquisition!\n");
		return -1;
	}

	usleep(100);

	return devFD;

}

int temperatureCtrlCommand(char *cmd)
{
    sem_t * semPause_id;
    sem_t * semWake_id;
    int wakeState;
    int i;

    semPause_id = sem_open("PauseControl", O_CREAT, 0666, 0);
    if(semPause_id == SEM_FAILED) {
	perror("child sem_open");
	return -1;
    }

    semWake_id = sem_open("WakeControl", O_CREAT, 0666, 0);
    if(semWake_id== SEM_FAILED) {
	perror("child sem_open");
	return -1;
    }

    if(strcmp(cmd, "stop") == 0 )
    {
	printf("============= STOP Warmer =====================\n");
	if( sem_getvalue( semWake_id, &wakeState ) < 0 )
	{
		perror("Control sem_getvalue");
		return -1;
	}


	//printf("STOP wake state %d\n", wakeState);
	for( i = 0; i < wakeState; i++)
	{
		if( sem_wait( semWake_id  ) < 0 )
		{
			perror("sem_wait WakeControl");
			return -1;
		}
/*
		if( sem_post( semPause_id  ) < 0 )
		{
			perror("sem_post PauseControl");
			return 0;
		}
*/
	}
    }

    if(strcmp(cmd, "start") == 0 || strcmp(cmd, "exit") == 0 )
    {
	if(strcmp(cmd, "start") == 0)
		printf("============= START Warmer ====================\n");

	if( sem_getvalue( semWake_id, &wakeState ) < 0 )
	{
		perror("Control sem_getvalue");
		return -1;
	}

	//printf("START wake state %d\n", wakeState);
	if ( wakeState == 0 )
	{
		//printf("START pause state %d\n", wakeState);

		if( sem_post( semPause_id  ) < 0 )
		{
			perror("sem_post PauseControl");
			return -1;
		}
		if( sem_post( semWake_id  ) < 0 )
		{
			perror("sem_post WakeControl");
			return -1;
		}
	}
	if( strcmp(cmd, "exit") == 0 )
	{
		printf("============= EXIT Warmer Control =============\n");
		if( sem_post( semWake_id  ) < 0 )
		{
			perror("sem_post WakeControl");
			return -1;
		}
		printf("Close semaphore id\n");

		if( sem_close(semPause_id) < 0 )
			perror("Control sem_close");

		if(sem_close(semWake_id ) < 0 )
			perror("Control sem_close");

	}

    }
    printf("===============================================\n");
    return 0;
}

double calibTCN(double val)
{
	double out;

	double K  = 47.513 * 1.e-3  / 5. ;

	double C1 =  3.86896e-2;
	double C2 = -1.08267e-6;
	double C3 =  4.70205e-11;
	double C4 = -2.12169e-18;
	double C5 = -1.17272e-19;
	double C6 =  5.39280e-24;
	double C7 = -7.98156e-29;

	val = K * val * 1.e6;
	double val2 = val * val;
	double val3 = val2*val;
	double val4 = val3*val;
	double val5 = val4*val;
	double val6 = val5*val;
	double val7 = val5*val;

	out =  C1 * val + C2 * val2 + C3 * val3 + C4 * val4 + C5 * val5 + C6 * val6 + C7 * val7;

/*
	scaled[i] = 0.0;
	rawf = (double)raw[i];
	for (j = NUM_AI_SCALING_COEFFICIENTS - 1; j >= 0; j--) {
	    scaled[i] *= rawf;
	    scaled[i] += scaling->c[j].f;
	}
*/
	return out;
}


int temperatureProbeControl(uint32_t  boardID, uint32_t *inChan, int numChan, int aiFdChanIdx, double frequency, int aochan, int dochan, double tempRef )
{

	static char log[5] = "|/-\\";
	static int count = 0;

	uint32_t deviceNum;
	//int numChan = 2;
	//uint32_t inChan[numAiChannle];
	//double    frequency;
	uint32_t outChanRef;
	uint32_t outChanOnOff;
	uint32_t rc;


	int chanInFD[numChan];
	int devInFD;

	int chanOutFD[2];
	int devOutFD;

	int chanDigitFD;
	int devDigitFD;

	TreeNode *node[16];
	TreeNode *errorNode;
	TreeNode *vRefNode;


	int wakeState = 1;
	int error = 0;

	float ZERO_VOLT = 0;
	float FIVE_VOLT = 5.;
	uint8_t ON  = 0x01;
	uint8_t OFF = 0x00;

	float setPointT;
	float tempVal;
	float E;
	bool enableTcCheck = true;


	if ( aiFdChanIdx < 0 )
	{
	    aiFdChanIdx = -aiFdChanIdx;
	    enableTcCheck = false;
	    printf("Disable check on TC1 TC2 temperature difference\n");
	}

	printf("BoardID %d\n", boardID );
	printf("numChan %d\n", numChan );
	for(int i = 0; i < numChan; i++)
	{
		printf("chan %d = %d\n", i, inChan[i]);
	}
	printf("numChan %d\n", numChan );
	printf("FeedBack chan Idx %d\n", inChan[aiFdChanIdx] );
	printf("frequency %e\n", frequency );
	printf("Power supply reference chan %d\n", aochan );
	printf("Rele control digital id %d\n", dochan );
	printf("Temperature set point %e\n", tempRef );


	deviceNum = boardID;
/*
	inChan[0] = aichan1;
	printf("ch %d\n", inChan[0] );
	inChan[1] = aichan2;
	printf("ch %d\n", inChan[1] );
	frequency = freq;
*/
	outChanRef = aochan;
	outChanOnOff = dochan;

	setPointT = tempRef;



	printf("Set point temperatura %f\n", setPointT );

	sem_t * pauseSem_id;
	sem_t * wakeSem_id;

	pauseSem_id = sem_open("PauseControl", O_CREAT, 0666, 0);
	if(pauseSem_id == SEM_FAILED ) {
		perror("pause Control sem_open");
		return -1;
	}

	wakeSem_id = sem_open("WakeControl", O_CREAT, 0666, 1);
	if(pauseSem_id == SEM_FAILED ) {
		perror("wake Control sem_open");
		return -1;
	}


	try {
		time_t rawtime;
		struct tm * timeinfo;
		char strShot[256];
		char dataFile[256];
		unsigned long shot;
		Tree *t;

		time (&rawtime);
		timeinfo = localtime (&rawtime);

		sprintf(strShot, "%d%0.2d%0.2d", 1900+timeinfo->tm_year, timeinfo->tm_mon+1, timeinfo->tm_mday);
		shot = strtoul(strShot, NULL, 0);
		sprintf(dataFile, "%s/ipp_tc_trend_%d.datafile", getenv("ipp_tc_trend_path"), shot);

		//printf("date %s shot %d str shot %s file %s\n", asctime(timeinfo), shot, strShot, dataFile);

		FILE *fd = fopen(dataFile, "r");
		if( !fd  )
		{
			t = new Tree((char *)"ipp_tc_trend", -1);
			t->createPulse(shot);
		}
		else
		{
			fclose(fd);
		}

		t = new Tree((char *)"ipp_tc_trend", shot);

		for( int i = 0; i < numChan; i++ )
		{
			char path[256];
			sprintf(path, "\\IPP_TC_TREND::TC%d", inChan[i] );
			printf("NODO %s \n", path );
			node[i] = t->getNode(path);
		}


		errorNode = t->getNode((char *)"\\IPP_TC_TREND::ERROR");
		vRefNode = t->getNode((char *)"\\IPP_TC_TREND::VREF");

	}
	catch(MdsException *exc)
	{
		printf("%s\n", exc->what());
		return -1;
	}



	while(!error)
	{

		if( !wakeState )
		{
			printf("WAIT\n");
			if( sem_wait( pauseSem_id ) < 0 )
			{
				perror("Control sem_wait");
				return -1;
			}
			wakeState = 1;
		}

		if ( ( devInFD = configureInput( chanInFD, deviceNum, inChan, frequency, numChan ) ) < 0 ) {
			fprintf(stderr, "Error configure input channel!\n");
			return -1;
		}

		if ( ( devOutFD = configureOutput( chanOutFD, deviceNum, outChanRef, outChanOnOff ) ) < 0 ) {
			fprintf(stderr, "Error configure ouput channel!\n");
			return -1;
		}

		if ( ( devDigitFD = configureDigital( &chanDigitFD, deviceNum, outChanOnOff) ) < 0 ) {
			fprintf(stderr, "Error configure digital channel!\n");
			return -1;
		}

		printf("START controll\n");

		// Control tc
		while (wakeState)
		{
			int n = 0;
			int i = 0;
			int nChRead;
			int scans_read[16];
			float value[16];
			float vRef;

/*

			rc = pxi6259_write_ao(chanOutFD[1], &FIVE_VOLT, 1);
			if (rc < 0) {
			    fprintf(stderr, "Failed to write to AO channel ON/OFF: %u\n", outChanOnOff);
			    error = 1;
			    goto out;
			}
*/
			if (write(chanDigitFD, &ON, 4) != 4) {
			    fprintf(stderr, "Error writing ON to port 0! Error: %s\n", strerror(errno));
			    error = 1;
			    goto out;
			}

			memset( scans_read, 0, sizeof(scans_read) );
			nChRead = 0;
			while( nChRead < numChan )
			{
				if( scans_read[i] <= 0 )
				{
					//printf("read channel %d n chan %d \n", i, nChRead);
					scans_read[i] = pxi6259_read_ai(chanInFD[i], &value[i], 1);

					if (scans_read[i] < 0 )
					{
						if( errno != EAGAIN )
						{
					        	fprintf(stderr, "Failed while reading channel: %u: %s %d\n", inChan[i], strerror(errno), errno);
							error = 1;
					        	goto out;
						}
					}
					else
					      nChRead++;
				}
				i = (i+1) % numChan;
			}


			// Start Control algorithm
			//tempVal = 1300./5. * value[aiFdChanIdx];
			tempVal = (float)calibTCN( (double)value[aiFdChanIdx] );
			//printf("Temp %f V %f C \n", value[aiFdChanIdx] , tempVal);
			E = setPointT - tempVal;
			if( E >= 0)
			{
			   vRef = 0.2 * E;
			   if( vRef > 3.0 )
				 vRef = 3.0;
			}
			else
			   vRef = 0.0;

			if( fabs( value[0] - value[1] ) > 30.  && enableTcCheck )
			{
				fprintf(stderr, "Difference between TC1 and TC2 > than 30 C reset reference signal \n");
				vRef = 0.0;
			}

			//vRef =  setPointT ;
			//vRef = value[aiFdChanIdx];

			// End control algoritm

			rc = pxi6259_write_ao(chanOutFD[0], &vRef, 1);
			if (rc < 0) {
			    fprintf(stderr, "Failed to write to AO channel reference: %u\n", outChanRef);
			    error = 1;
			    goto out;
			}

			struct timeb tb;
			int64_t currTime;
			try
			{
				ftime( &tb);
				Float32 *currData;
				currTime =  (int64_t)(tb.time * 1000 + tb.millitm);
				for(int i = 0; i < numChan; i++)
				{
					currData = new Float32(value[i]);
					node[i]->putRow(currData, &currTime);
				}

				currData = new Float32(E);
				errorNode->putRow(currData, &currTime);

				currData = new Float32(vRef);
				vRefNode->putRow(currData, &currTime);
			}
			catch(MdsException *exc)
			{
				printf("%s\n", exc->what());
				error = 1;
				goto out;
			}

			if( sem_getvalue( wakeSem_id, &wakeState ) < 0 || wakeState == 2 )
			{
				if(wakeState != 2)
				   perror("Control sem_open");
				error = 1;
				goto out;
			}
/*
			struct tm * timeinfo;
			time_t pp =  (time_t)( tb.time );
			timeinfo = localtime ( &pp );
			printf(" %x Value[0] %f %s:%d\n\n Wake state %d \n", currTime, value[0], asctime(timeinfo), tb.millitm, wakeState);
			printf("Value[0] = %f  \n", value[0]);
*/
			printf("\r%c", log[count], count);
			fflush( stdout );
			count = (count+1)%4;

		}

	out:

		rc = pxi6259_write_ao(chanOutFD[0], &ZERO_VOLT, 1);
		if (rc < 0) {
		    fprintf(stderr, "Failed to write to AO channel reference: %u\n", outChanRef);
		    error = 1;
		    goto out;
		}
/*
		rc = pxi6259_write_ao(chanOutFD[1], &ZERO_VOLT, 1);
		if (rc < 0) {
		    fprintf(stderr, "Failed to write to AO channel ON/OFF: %u\n", outChanOnOff);
		    error = 1;
		    goto out;
		}
*/
		if (write(chanDigitFD, &OFF, 4) != 4) {
		    fprintf(stderr, "Error writing OFF to port 0! Error: %s\n", strerror(errno));
		    error = 1;
		    goto out;
		}

		printf("Close open ADC fd\n");

		// stop AI segment
		if (pxi6259_stop_ai(devInFD)) {
			fprintf(stderr, "Failed to stop data acquisition!\n");
		}

		// close file descriptors
		for(int i = 0; i < numChan; i++)
			close(chanInFD[i]);

		close(devInFD);

		// stop AO segment
		if (pxi6259_stop_ao(devOutFD)) {
			fprintf(stderr, "Failed to stop generating signal!\n");
		}


		close(chanOutFD[0]);
		close(chanOutFD[1]);
		close(devOutFD);

		// close digital file descriptors
	        close(chanDigitFD);
		close(devDigitFD);


	}

	if( sem_unlink("PauseControl") < 0 )
		perror("Control sem_close");

	if(sem_unlink("WakeControl") < 0 )
		perror("Control sem_close");

}



/**************************************************************************************/
/************* SWEEP WAVEFOR GENERATIO ************************************************/
/**************************************************************************************/


extern "C" uint32_t generateWaveformOnOneChannel_6368(int selectedCard, int channel, double offset, double level, int waverate, float *values, int nPoint, int softwareTrigger);
extern "C" uint32_t generateWaveformOnOneChannel_6259(int selectedCard, int channel, double offset, double level, int waverate, float *values, int nPoint , int softwareTrigger);
extern "C" int stopWaveGeneration();

static void createWaveform(int number_of_samples, double offset, double level, float *buf)
{
    int i;

    // create one complete triangular period in volts

    double m = 4./number_of_samples;

    for(i = 0; i <  number_of_samples/4; i++)
	    buf[i] = (float)(offset + level * m * i);

    for(i = number_of_samples/4; i <  3 * number_of_samples / 4; i++)
	    buf[i] = (float)(offset + level * (-m * i + 2) );

    for(i = 3 * number_of_samples/ 4; i <  number_of_samples ; i++)
	    buf[i] = (float)(offset + level * (m * i - 4 ));

}


uint32_t generateWaveformOnOneChannel_6368(int selectedCard, int channel, double offset, double level, int waverate, float *values, int nPoint, int softwareTrigger)
{
    int silent = 0;
    int retval = 0;
    int number_of_channels = 1;
    int number_of_samples = 4000;
    const char *device_name = "/dev/PXIe-6368";
    sem_t *semWaveGen_id;

    uint32_t sampleRate;
    uint32_t update_period_divisor;


    sampleRate = number_of_samples * waverate;
    update_period_divisor = 100000000 / sampleRate;



	if( nPoint == 0 )
	{
	    printf ("Generate default triangular waveform\n");
	    number_of_samples = 4000;
	}
	else
	{
	    printf ("Generate EXPRESSION waveform\n");
	    number_of_samples = nPoint;
	}

	sampleRate = number_of_samples * waverate;
	update_period_divisor = 100000000 / sampleRate;

	int ao_fd = 0;
	//int dev_fd = 0;
	int ao_chan_fd[number_of_channels];

	char str[100];
	int i = 0, k = 0;

	double pi;
	double radianPart;

	float *write_array[number_of_channels];
	xseries_ao_conf_t ao_conf;

	for( i = 0; i < number_of_channels; i++ )
	{
		write_array[i] = (float *)calloc( 1, sizeof(float) * number_of_samples );
	}

	//memset (write_array, 0, sizeof(write_array));

	/* open AO segment file descriptor */
	sprintf(str,"%s.%u.ao",device_name,selectedCard);
	ao_fd = open(str, O_RDWR);
	if (ao_fd <= 0) {
		if(!silent) printf("Could not open AO segment!\n");
		retval = -1;
		goto out_6368;
	}

	/* Stop the segment */
	xseries_stop_ao(ao_fd);

	/* reset AO segment */
	retval = xseries_reset_ao(ao_fd);
	if(retval) {
		if(!silent) printf("Error reseting card!\n");
		goto out_6368;
	}

	/*
	* Configure AO segment
	*/

	/* Create AO configuration */


	ao_conf = xseries_continuous_regenerated_ao(number_of_samples);

	/* Disable external gating of the sample clock */
	retval = xseries_set_ao_external_gate(&ao_conf,
			XSERIES_AO_EXTERNAL_GATE_DISABLED,	// No external pause signal
			XSERIES_AO_POLARITY_RISING_EDGE,	// Don't care
			0);					// Disable
	if(retval) {
		if(!silent) printf("Error setting external gate!\n");
		goto out_6368;
	}



	if( softwareTrigger )
	{
		/* Program the START1 signal (start trigger) to assert from a software rising edge */

		retval = xseries_set_ao_start_trigger(&ao_conf,
				XSERIES_AO_START_TRIGGER_SW_PULSE,		// Set the line to software-driven
				XSERIES_AO_POLARITY_RISING_EDGE,	// Make line active on rising...
				1);					//   ...edge (not high level)
		if(retval) {
			if(!silent) printf("Error setting start software trigger!\n");
			goto out_6368;
		}
	}
	else
	{

		/* Program the START1 signal (start trigger) to assert from a PFI1 rising edge */

		retval = xseries_set_ao_start_trigger(&ao_conf,
				XSERIES_AO_START_TRIGGER_PFI1,		// Set the line to PFI1-driven
				XSERIES_AO_POLARITY_RISING_EDGE,	// Make line active on rising...
				1);					//   ...edge (not high level)
		if(retval) {
			if(!silent) printf("Error setting start PFI 1 trigger!\n");
			goto out_6368;
		}

	}


	/* Program the Update source */
	retval = xseries_set_ao_update_counter(&ao_conf,
			XSERIES_AO_UPDATE_COUNTER_UI_TC,	// Derive the clock line from the Update Interval Terminal Count
			XSERIES_AO_POLARITY_RISING_EDGE);	// Make the line active on rising edge
	if(retval)
	{
		if(!silent) printf("Error setting update counter!\n");
		goto out_6368;
	}

	/* Program the Update Interval counter */
	retval = xseries_set_ao_update_interval_counter(&ao_conf,
			XSERIES_OUTTIMER_UPDATE_INTERVAL_COUNTER_TB3,	// Source the Update Interval from the internal timebase
			XSERIES_OUTTIMER_POLARITY_RISING_EDGE,		// Make the line active on rising edge
			update_period_divisor, 				// Number of clock intervals between successive updates
			2						// Number of clock intervals after the start trigger before the first update
			);
	if(retval) {
		if(!silent) printf("Error setting update interval counter!\n");
		goto out_6368;
	}


	/* Add channels */
	for (i=0; i<number_of_channels; i++) {
		retval = xseries_add_ao_channel(&ao_conf, i, XSERIES_OUTPUT_RANGE_10V);
		if(retval) {
			if(!silent) printf("Cannot add AI channel %d to configuration!\n",i);
			goto out_6368;
		}
	}

	/* load configuration to the device */
	retval = xseries_load_ao_conf(ao_fd, ao_conf);
	if(retval) {
		if(!silent) printf("Cannot load AO configuration! %s (%d)\n",strerror(errno),errno);
		goto out_6368;
	}

	/* wait for the AO devices */
	sleep(1);


	if( nPoint == 0 )
	{
	    createWaveform(number_of_samples, offset, level,  write_array[0]);
	}
	else
	{
	    for( i = 0; i < nPoint; i++ )
	    {
		write_array[0][i] = offset + level * values[i];
		//printf( "Wave %d %f \n", i, write_array[0][i] );
	    }
	}

	/* Open channels */
	for (i=0; i<number_of_channels; i++) {
		sprintf(str,"%s.%u.ao.%u",device_name, selectedCard ,i);
		ao_chan_fd[i] = open(str, O_RDWR | O_NONBLOCK);
		if(ao_chan_fd[i] < 0) {
			if(!silent) printf("Cannot add AO channel %d to configuration!\n",i);
			goto out_6368;
		}
	}

	/* Write samples */
       for (i=0; i<number_of_samples; i++) {
		for(k=0; k<number_of_channels; k++) {
			// Write is not blocking !!!
			retval = xseries_write_ao(ao_chan_fd[k], &write_array[k][i], 1);
			//printf("write_array[%d][%d] = %f\n", k, i, write_array[k][i]);
			if(retval == -1 && errno != EAGAIN) {
				if(!silent) printf("Error writing samples to FIFO buffer!\n");
				goto out_6368;
			}
		}
	}

	/* put segment in started state */
	retval = xseries_start_ao(ao_fd);
	if(retval) {
		printf("Cannot start AO segment! %s (%d)\n",strerror(errno),errno);
		goto out_6368;
	}

	if(softwareTrigger)
	{
		/* pulse start trigger */
		retval = xseries_pulse_ao(ao_fd, XSERIES_START_TRIGGER);
		if(retval) {
			printf("Error generating start pulse!\n");
			goto out_6368;
		}
	}

	{
	    int semVal;

	    semWaveGen_id = sem_open("WaveGen", O_CREAT, 0666, 0);
	    if(semWaveGen_id == SEM_FAILED) {
		perror("WaveGen sem_open");
		return 0;
	    }


	    if( sem_getvalue( semWaveGen_id, &semVal ) < 0 )
	    {
		perror("SemWaveGen sem_getvalue");
	    }

	    printf("Waveform generation START\n");
	    for( int i = 0; i < semVal+1; i++ )
	    {
		if( sem_wait( semWaveGen_id ) < 0 )
		{
		     perror("sem_wait on semWaveGen_id");
		}
	    }
	}

out_6368:

	/* stop the segment */
	xseries_stop_ao(ao_fd);

	/* close all used file descriptors */
	for (i=0; i<number_of_channels; i++) {
		close(ao_chan_fd[i]);
	}

	/* reset segment */
	xseries_reset_ao(ao_fd);

	/* close segment's file descriptor */
	close(ao_fd);

	/* close card's file descriptor */
	//close(dev_fd);


	for( i = 0; i < number_of_channels; i++ )
	{
	     free( write_array[i] );
	}

	printf("Waveform generation STOP\n");

	return retval;

}


#define RESOURCE_NAME_DAQ "/dev/pxi6259"
//uint32_t generateWaveformOnOneChannel_6259(uint8_t selectedCard, uint8_t channel, double offset, double level, uint32_t waverate, float *values, int nPoint, int softwareTrigger)
uint32_t generateWaveformOnOneChannel_6259(int selectedCard, int channel, double offset, double level, int waverate, float *values, int nPoint , int softwareTrigger)
{
	uint32_t retval = 0;
	pxi6259_ao_conf_t ao_conf;

	int fdConfig = 0;
	int fdChannel = 0;

	uint32_t sampleRate;

	uint32_t periodDivisor;

	uint32_t number_of_samples = 1000;
	uint32_t i;
	char str[32];

	double pi;
	double radianPart;
	sem_t *semWaveGen_id;


//printf("CARD %d\nChannel %d\nOffset %f\nLevel %f\nfreq %f\nnPoint %d\nTrigger %d\n", selectedCard, channel, offset, level, waverate, nPoint, softwareTrigger);
//printf("nPoint ==== %d\n", nPoint);

	if( nPoint == 0 )
	{
	    printf ("Generate default triangular waveform\n");
	    number_of_samples = 1000;
	}
	else
	{
	    printf ("Generate EXPRESSION waveform\n");
	    number_of_samples = nPoint;
	}

//printf("number_of_samples ==== %d\n", number_of_samples);

	float scaledWriteArray[number_of_samples];

	sampleRate = number_of_samples * waverate;
	periodDivisor = 20000000 / sampleRate;

//printf("sampleRate ==== %d\n", sampleRate);
//printf("periodDivisor ==== %d\n", periodDivisor);


	// get configuration file descriptor
	sprintf(str,"%s.%d.ao",RESOURCE_NAME_DAQ, selectedCard);
	fdConfig = open(str, O_RDWR | O_NONBLOCK);
	if (fdConfig < 0) {
	        printf ("Error Opening Device! fd: %d\n",fdConfig);
	        return -1;
	}


/*
	pi = 4.0 * atan(1.0);
	radianPart = 2*pi / number_of_samples;
	for (i=0; i<number_of_samples; i++) {
	        scaledWriteArray[i] = (float)(7 * sin((double) radianPart*i));
	}
*/


	if( nPoint == 0 )
	{
	    createWaveform(number_of_samples, offset, level,  scaledWriteArray);
	}
	else
	{
	    for( i = 0; i < nPoint; i++ )
	    {
		scaledWriteArray[i] = offset + level * values[i];
	    }
	}

//	retval = pxi6259_reset_ao(fdConfig);
	if (retval) {
	        printf ("err: reset ao. retval: %x\n",retval * -1);
	        return -1;
	}

	ao_conf = pxi6259_create_ao_conf();

	if (pxi6259_set_ao_waveform_generation(&ao_conf, 1))
	        return -1;

	if (pxi6259_add_ao_channel(&ao_conf, channel, AO_DAC_POLARITY_BIPOLAR))
	        return retval;

	if (pxi6259_set_ao_count(&ao_conf, number_of_samples, 1, 1))
	        return -1;

	if (pxi6259_set_ao_update_clk(&ao_conf, AO_UPDATE_SOURCE_SELECT_UI_TC,
	                AO_UPDATE_SOURCE_POLARITY_RISING_EDGE, periodDivisor))
	        return -1;



	if( softwareTrigger )
	{
		// Program the START1 signal (start trigger) to assert from a software rising edge
		if ( pxi6259_set_ao_attribute(&ao_conf, AO_START1_SOURCE_SELECT,  AO_START1_SELECT_PULSE) )
		{
			printf("Error setting start software trigger!\n");
			goto out_6259;
		}

		if ( pxi6259_set_ao_attribute(&ao_conf, AO_START1_POLARITY_SELECT , AO_START1_POLARITY_RISING_EDGE) )
		{
			printf("Error setting start software trigger!\n");
			goto out_6259;
		}


	}
	else
	{
		// Program the START1 signal (start trigger) to assert from a PFI1 rising edge
		if ( pxi6259_set_ao_attribute(&ao_conf, AO_START1_SOURCE_SELECT,  AO_START1_SELECT_PFI1) )
		{
			printf("Error setting start PFI1 trigger!\n");
			goto out_6259;
		}

		if ( pxi6259_set_ao_attribute(&ao_conf, AO_START1_POLARITY_SELECT , AO_START1_POLARITY_RISING_EDGE) )
		{
			printf("Error setting start PFI1 trigger!\n");
			goto out_6259;
		}

	}


	retval = pxi6259_load_ao_conf(fdConfig, &ao_conf);
	if (retval) {
	        printf ("err: load task. retval: %x\n",retval * -1);
	        goto out_6259;
	}

	sleep(1);

	// Open channels
	sprintf(str,"%s.%d.ao.%d",RESOURCE_NAME_DAQ,selectedCard,channel);
	fdChannel = open(str, O_RDWR | O_NONBLOCK);
	if (fdChannel < 0) {
	        printf ("Error Opening Channel! FD: %d\n",fdChannel);
	        return -1;
	}

	retval = pxi6259_write_ao(fdChannel, scaledWriteArray, number_of_samples);
	if (retval != number_of_samples) {
	        printf ("err: writing. retval: %d\n",retval);
	        goto out_6259;
	}

	retval = pxi6259_start_ao(fdConfig);
	if (retval) {
	        printf ("err: Starting task. retval: %d\n",retval);
	        return -1;
	}

/*
	if(softwareTrigger)
	{
		// pulse start trigger
		retval = xseries_pulse_ao(ao_fd, XSERIES_START_TRIGGER);
		if(retval) {
			printf("Error generating start pulse!\n");
			goto out_6368;
		}
	}
*/
	{
	    int semVal;

	    semWaveGen_id = sem_open("WaveGen", O_CREAT, 0666, 0);
	    if(semWaveGen_id == SEM_FAILED) {
		perror("WaveGen sem_open");
		return 0;
	    }


	    if( sem_getvalue( semWaveGen_id, &semVal ) < 0 )
	    {
		perror("SemWaveGen sem_getvalue");
	    }

	    printf("Waveform generation START\n");
	    for( int i = 0; i < semVal+1; i++ )
	    {
		if( sem_wait( semWaveGen_id ) < 0 )
		{
		     perror("sem_wait on semWaveGen_id");
		}
	    }
	}


out_6259:
	if (pxi6259_stop_ao(fdConfig))
	        printf ("err: Stoping task. retval: %x\n",retval * -1);

	close(fdChannel);
	//sleep(1);

//	retval = pxi6259_reset_ao(fdConfig);
	if (retval) {
	        printf ("err: reset ao. retval: %x\n",retval * -1);
	        return -1;
	}
	close(fdConfig);


/*
	retval = pxi6259_load_ao_conf(fdConfig, &ao_conf);
	if (retval) {
	        printf ("err: load task. retval: %x\n",retval * -1);
	        goto out_6259;
	}

	// Open channels
	sprintf(str,"%s.%d.ao.%d",RESOURCE_NAME_DAQ,selectedCard,channel);
	fdChannel = open(str, O_RDWR | O_NONBLOCK);
	if (fdChannel < 0) {
	        printf ("Error Opening Channel! FD: %d\n",fdChannel);
	}

	for( i = 0; i <  number_of_samples; i++)
	    scaledWriteArray[i] = (float)0.;

	retval = pxi6259_write_ao(fdChannel, scaledWriteArray, number_of_samples);
	if (retval != number_of_samples) {
	        printf ("err: writing. retval: %d\n",retval);
	}

	retval = pxi6259_start_ao(fdConfig);
	if (retval) {
	        printf ("err: Starting task. retval: %d\n",retval);
	}

	sleep(1);

	if (pxi6259_stop_ao(fdConfig))
	        printf ("err: Stoping task. retval: %x\n",retval * -1);

	close(fdChannel);
	close(fdConfig);
*/
	return 0;
}

int stopWaveGeneration()
{
    sem_t * semWaveGen_id;

    semWaveGen_id = sem_open("WaveGen", O_CREAT, 0666, 0);
    if(semWaveGen_id == SEM_FAILED) {
	perror("WaveGen sem_open");
	return 0;
    }

    if( sem_post( semWaveGen_id  ) < 0 )
    {
	perror("sem_post semWaveGen");
	return 0;
    }

    //sem_close(semWaveGen_id);

    return 1;

}

////////////////////////////////////////////////////////////////////////////
//////////////////////////    NI6259EV stuff   /////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define SEGMENT_OP_BEGIN 1
#define SEGMENT_OP_UPDATE 2
#define SEGMENT_OP_PUT 3
#define SEGMENT_OP_MAKE 4

//Support class for enqueueing storage requests, specific for NI6259 EV management
class SaveItemEV {
    int operation; //BEGIN UPDATE PUT MAKE
    MDSplus::Data *startData;
    MDSplus::Data *endData;
    MDSplus::Data *dimData;
    MDSplus::Array *segData;
    MDSplus::TreeNode *node;
    SaveItemEV *nxt;
 public:
    SaveItemEV(int operation, MDSplus::Data *startData, MDSplus::Data *endData, MDSplus::Data *dimData, MDSplus::Array *segData, MDSplus::TreeNode *node)
    {
	this->operation = operation;
	this->startData = startData;
	this->endData = endData;
	this->dimData = dimData;
	this->segData = segData;
	this->node = node;
	this->nxt = 0;
    }

    void setNext(SaveItemEV *itm)
    {
	nxt = itm;
    }

    SaveItemEV *getNext()
    {
	return nxt;
    }

    void save()
    {
	switch(operation)
	{
	    case SEGMENT_OP_BEGIN:
		try {
std::cout << "SAVE ITEM OP_BEGIN START:" << startData << "  END: " << endData << "  DIM: " << dimData <<std::endl;
		    node->beginSegment(startData, endData, dimData, segData);
		    MDSplus::deleteData(startData);
		    MDSplus::deleteData(endData);
		    MDSplus::deleteData(dimData);
		} catch(MDSplus::MdsException &exc)
		{
		    std::cout << "Error in BeginSegment: " << exc.what() << std::endl;
		}
//std::cout << "OP_BEGIN FINITO" << std::endl;
		break;
	    case SEGMENT_OP_MAKE:
		try {
std::cout << "SAVE ITEM OP_MAKE" << std::endl;
		    node->makeSegment(startData, endData, dimData, segData);
		    MDSplus::deleteData(startData);
		    MDSplus::deleteData(endData);
		    MDSplus::deleteData(dimData);
		    MDSplus::deleteData(segData);
		} catch(MDSplus::MdsException &exc)
		{
		    std::cout << "Error in MakeSegment: " << exc.what() << std::endl;
		}
		break;
	    case SEGMENT_OP_UPDATE:
		try {
std::cout << "SAVE ITEM OP_UPDATE" << std::endl;
		    node->updateSegment(startData, endData, dimData);
		    MDSplus::deleteData(startData);
		    MDSplus::deleteData(endData);
		    MDSplus::deleteData(dimData);
		} catch(MDSplus::MdsException &exc)
		{
		    std::cout << "Error in UpdateSegment: " << exc.what() << std::endl;
		}
		break;
	    case SEGMENT_OP_PUT:
		try {
std::cout << "SAVE ITEM OP_PUT" << std::endl;
		    node->putSegment(segData, -1);
		    MDSplus::deleteData(segData);
		} catch(MDSplus::MdsException &exc)
		{
		    std::cout << "Error in PutSegment: " << exc.what() << std::endl;
		}
		break;
	    }

    }

}; //class SaveItemEV

extern "C" void *handleSaveEV(void *listPtr);

class SaveListEV
{
    public:
	pthread_cond_t itemAvailable;
	pthread_t thread;
	bool threadCreated;
	SaveItemEV *saveHead, *saveTail;
	bool stopReq;
	pthread_mutex_t mutex;
    SaveListEV()
    {
	int status = pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&itemAvailable, NULL);
	saveHead = saveTail = NULL;
	stopReq = false;
	threadCreated = false;
    }
    void addItem(int operation, MDSplus::Data *startData, MDSplus::Data *endData, MDSplus::Data *dimData, MDSplus::Array *segData, MDSplus::TreeNode *node)
    {
	SaveItemEV *newItem = new SaveItemEV(operation, startData, endData, dimData, segData, node);
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
    }
    void executeItems()
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
std::cout << "SAVE LIST WAIT..." << std::endl;
		pthread_cond_wait(&itemAvailable, &mutex);
std::cout << "SAVE LIST CONDITION" << std::endl;
		if(stopReq && saveHead == NULL)
		{
std::cout << "SAVE LIST EXIT" << std::endl;

		    pthread_mutex_unlock(&mutex);
		    pthread_exit(NULL);
		}
	    }
	    SaveItemEV *currItem = saveHead;
	    saveHead = saveHead->getNext();
	    pthread_mutex_unlock(&mutex);
	    currItem->save();
	    delete currItem;
	}
    }
    void start()
    {
	pthread_create(&thread, NULL, handleSaveEV, (void *)this);
	threadCreated = true;
    }
    void stop()
    {
	stopReq = true;
	pthread_cond_signal(&itemAvailable);
	if(threadCreated)
	{
	    pthread_join(thread, NULL);
	    printf("SAVE THREAD TERMINATED\n");
	}
    }
 };


extern "C" void *handleSaveEV(void *listPtr)
{
    SaveListEV *list = (SaveListEV *)listPtr;
    list->executeItems();

std::cout << "HANDLE SAVE EV TERMINATED " << std::endl;
   return NULL;
}

extern "C" void startSaveEV(void **retList)
{
    SaveListEV *saveList = new SaveListEV;
    saveList->start();
    *retList = (void *)saveList;
}

extern "C" void stopSaveEV(void *listPtr)
{
    if(listPtr)
    {
	SaveListEV *list = (SaveListEV *)listPtr;
	list->stop();
	delete list;
    }
}


class BufferHandler
{
    size_t bufSize;
    short *buffer;
    size_t bufferIdx, oldestBufferIdx;
    unsigned long sampleCount;
  protected:
    MDSplus::Tree *tree;
    MDSplus::TreeNode *rawNode;
    SaveListEV *saveList;

  public:
    BufferHandler(MDSplus::Tree *tree, MDSplus::TreeNode *rawNode, double maxDelay, double preTime, double baseFreq, SaveListEV *saveList)
    {
	this->tree = tree;
	this->rawNode = rawNode;
	this->bufSize = (preTime + maxDelay) * baseFreq;
	this->buffer = new short[this->bufSize];
	this->bufferIdx = this->oldestBufferIdx = 0;
	this->sampleCount = 0;
	this->saveList = saveList;
    }
    ~BufferHandler()
    {
	delete [] buffer;
    }

    void processSample(short sample)
    {
	buffer[bufferIdx] = sample;
	sampleCount++;
	if(sampleCount >= bufSize - 1)
	{
	    processSampleDelayed(buffer[oldestBufferIdx]);
	    oldestBufferIdx = (oldestBufferIdx + 1)%bufSize;
	}
	bufferIdx = (bufferIdx + 1)%bufSize;
    }
    void terminate()
    {
	if(sampleCount > bufSize)
	{
	    for(size_t i = 0; i < bufSize - 2; i++)
	    {
		processSampleDelayed(buffer[oldestBufferIdx]);
		oldestBufferIdx = (oldestBufferIdx + 1)%bufSize;
	    }
	    flushBuffer();
	}
    }

    virtual void processSampleDelayed(short sample) = 0;
    virtual void trigger(double trigTime) = 0;
    virtual void flushBuffer() = 0;
};




class ClockBufferHandler:public BufferHandler
{
    short *segBuffer;
    MDSplus::Array *initSegData;
    size_t bufIdx;

    int f12Div[2], currDivIdx;
    double baseFreq;
    int segBufSize;
    int segmentSize;
    int numBuffersInSegment;
    int bufferCount;
    double startTime;
    double preTime, postTime;
    long baseSampleCount, currBaseSampleCount, sampleCount;
    unsigned long segBufSampleCount;
    double basePeriod;
    double bufStartTime;
    double minPeriod;
    std::vector<double> bufStartTimes;
    std::vector<double> bufEndTimes;
    std::vector<double> bufPeriods;
    std::vector<double> switchTimes;
    bool freqSwitched;

public:
    ClockBufferHandler(MDSplus::Tree *tree, MDSplus::TreeNode *rawNode, double maxDelay, int f1Div, int f2Div,
	double baseFreq, int segBufSize, int segmentSize, double startTime, double preTime,
	double postTime, SaveListEV *saveList):BufferHandler(tree, rawNode, maxDelay, preTime, baseFreq, saveList)
    {
	this->basePeriod = 1./baseFreq;
	this->baseFreq = baseFreq;
	this->segBufSize = segBufSize;
	this->segBuffer = new short[segBufSize];
	this->numBuffersInSegment = segmentSize / segBufSize;
	this->segmentSize = segBufSize * this->numBuffersInSegment;
	this->f12Div[0] = f1Div;
	this->f12Div[1] = f2Div;
	if(f1Div > f2Div)
	    this->minPeriod = basePeriod * f2Div;
	else
	    this->minPeriod = basePeriod * f1Div;
	this->currDivIdx = 0;
	this->startTime = startTime;
	this->preTime = preTime;
	this->postTime = postTime;
	this->baseSampleCount = this->segBufSampleCount = this->sampleCount = 0;
	this->currBaseSampleCount = -1;
	this->bufStartTime = startTime;
	double bufEndTime = startTime + segmentSize * (this->basePeriod * f12Div[0]);
	bufStartTimes.push_back(startTime);
	bufEndTimes.push_back(bufEndTime);
	bufPeriods.push_back(this->basePeriod * f12Div[0]);
//Prepare first segment
	short *initSeg = new short[segmentSize];
	memset(initSeg, 0, sizeof(short) * segmentSize);
	initSegData = new MDSplus::Int16Array(initSeg, segmentSize);
	delete [] initSeg;
	MDSplus::Data *startSegData = new MDSplus::Float64(startTime);
	MDSplus::Data *endSegData = new MDSplus::Float64(bufEndTime);
	MDSplus::Data *periodData = new MDSplus::Float64(this->basePeriod * f12Div[0]);
	MDSplus::Data *dimData = MDSplus::compileWithArgs("build_range($, $, $)", tree, 3, startSegData, endSegData, periodData);
	rawNode->beginSegment(startSegData, endSegData, dimData, initSegData);
	MDSplus::deleteData(startSegData);
	MDSplus::deleteData(endSegData);
	MDSplus::deleteData(periodData);
	MDSplus::deleteData(dimData);
	this->bufferCount = 0;
	this->freqSwitched = false;
    }
    ~ClockBufferHandler()
    {
	delete [] segBuffer;
	MDSplus::deleteData(initSegData);
    }


    void processSampleDelayed(short sample)
    {
	baseSampleCount++;
	currBaseSampleCount++;
//Check whether frequency switched
	double currTime = startTime + basePeriod * baseSampleCount;
static int i = 0;
/*if(i++ % 1000 == 0)
{
    std::cout << currTime << std::endl;
    if(switchTimes.size() > 0)
	std::cout << switchTimes[0] << std::endl;
}
*/
	if(currBaseSampleCount % f12Div[currDivIdx] == 0)
	{
	    segBuffer[segBufSampleCount] = sample;
	    segBufSampleCount++;
	    if(segBufSampleCount >= segBufSize) //buffer filled
	    {
std::cout << "BUFFER FILLED" << std::endl;

		MDSplus::Array *bufferData = new Int16Array(segBuffer, segBufSize);
		saveList->addItem(SEGMENT_OP_PUT, NULL, NULL, NULL, bufferData, rawNode);
		//rawNode->putSegment(bufferData, -1);
		segBufSampleCount = 0;
		bufferCount++;
		if(bufferCount >= numBuffersInSegment) //Need to possibly adjust segment end and dimension and create a new segment
		{
std::cout << "SEGMENT FILLED" << std::endl;
//Prepare next segment
		    bufStartTime = basePeriod * baseSampleCount;
		    double bufEndTime = bufStartTime + segmentSize * basePeriod*f12Div[currDivIdx];
		    MDSplus::Data *startSegData = new MDSplus::Float64(bufStartTime);
		    MDSplus::Data *endSegData = new MDSplus::Float64(bufEndTime);
		    MDSplus::Data *periodData = new MDSplus::Float64(basePeriod * f12Div[currDivIdx]);
		    MDSplus::Data *dimData = MDSplus::compileWithArgs("build_range($, $, $)", tree, 3, startSegData,
			endSegData, periodData);
		    saveList->addItem(SEGMENT_OP_BEGIN, startSegData, endSegData, dimData, initSegData, rawNode);
		    //rawNode->beginSegment(startSegData, endSegData, dimData, initSegData);
		    MDSplus::deleteData(periodData);

		    bufferCount = 0;
		    bufStartTimes.clear();
		    bufStartTimes.push_back(bufStartTime);
		    bufEndTimes.clear();
		    bufEndTimes.push_back(bufEndTime);
		    bufPeriods.clear();
		    bufPeriods.push_back(basePeriod * f12Div[currDivIdx]);
		    freqSwitched = false;
		}
	    }
	}
	if(switchTimes.size() > 0 && switchTimes[0] <= currTime) //frequencySwitched
	{
//std::cout << "FREQUENCY SWITCH " << std::endl;

	    currDivIdx = (currDivIdx + 1)%2;
	    bufPeriods.push_back(basePeriod * f12Div[currDivIdx]);
	    if(currBaseSampleCount % f12Div[currDivIdx] == 0) //A sample at previous frequency has been written at this time
	    {
		currBaseSampleCount = 0; //wait a period for the new frequency before saving sample
		bufStartTimes.push_back(switchTimes[0] + basePeriod * f12Div[currDivIdx]);
	        bufEndTimes[bufEndTimes.size() - 1] = switchTimes[0]+ minPeriod/2.;
	    }
	    else
	    {
		currBaseSampleCount = -1; //Next sample is being written
		bufStartTimes.push_back(switchTimes[0]);
	        bufEndTimes[bufEndTimes.size() - 1] = switchTimes[0]- minPeriod/2.;
	    }



//std::cout << "SEGMENT SIZE: " << segmentSize << "  BUFFER COUNT: " << bufferCount << "  SEG BUF SAMPLE COUNT: " << segBufSampleCount << "  PERIOD: " << basePeriod * f12Div[currDivIdx] << std::endl;
	    bufEndTimes.push_back(switchTimes[0] + (segmentSize - (bufferCount * segBufSize + segBufSampleCount)-1) *
		basePeriod * f12Div[currDivIdx] - basePeriod * f12Div[currDivIdx]/2.);
	    freqSwitched = true;
	    switchTimes.erase(switchTimes.begin());


	    MDSplus::Data *startTimeData  = new MDSplus::Float64(bufStartTimes[0]);
	    MDSplus::Data *endTimeData  = new MDSplus::Float64(bufEndTimes[bufEndTimes.size()]);
	    MDSplus::Data *startTimesData = new MDSplus::Float64Array(bufStartTimes.data(), bufStartTimes.size());
	    MDSplus::Data *endTimesData = new MDSplus::Float64Array(bufEndTimes.data(), bufEndTimes.size());
	    MDSplus::Data *periodsData = new MDSplus::Float64Array(bufPeriods.data(), bufPeriods.size());
	    MDSplus::Data *dimData = MDSplus::compileWithArgs("build_range($, $, $)", tree, 3, startTimesData,
		endTimesData, periodsData);

std::cout << "UPDATE start: " << startTimesData << std::endl;
std::cout << "UPDATE end: " << endTimesData << std::endl;
std::cout << "UPDATE dim: " << dimData << std::endl;

	    saveList->addItem(SEGMENT_OP_UPDATE, startTimeData, endTimeData, dimData, NULL, rawNode);
	    MDSplus::deleteData(periodsData);
	    MDSplus::deleteData(startTimesData);
	    MDSplus::deleteData(endTimesData);



	}

    }  //processSample()
    virtual void trigger(double trigTime)
    {
	double startTime = trigTime - preTime;
	if(switchTimes.size() == 0 || switchTimes[switchTimes.size() - 1] <= startTime)
	    switchTimes.push_back(startTime);
	else
	{
	    size_t idx;
	    for(idx = switchTimes.size() - 1; idx > 0 && switchTimes[idx] >= startTime; idx--);
	    switchTimes.insert(switchTimes.begin() + idx, startTime);
	}
	double endTime = trigTime + postTime;
	if(switchTimes[switchTimes.size() - 1] <= endTime)
	    switchTimes.push_back(endTime);
	else
	{
	    size_t idx;
	    for(idx = switchTimes.size() - 1; idx > 0 && switchTimes[idx] >= endTime; idx--);
	    switchTimes.insert(switchTimes.begin() + idx, endTime);
	}
    }
    virtual void flushBuffer()
    {
std::cout << "FLUSH BUFFER " << segBufSampleCount << std::endl;
	MDSplus::Array *bufferData = new Int16Array(segBuffer, segBufSampleCount);
	saveList->addItem(SEGMENT_OP_PUT, NULL, NULL, NULL, bufferData, rawNode);
    }

};


class BurstBufferHandler:public BufferHandler
{
    short *segBuffer;
    std::vector<double> startTimes;
    int freqDiv;
    double baseFreq;
    int segmentSize, burstCount, windowSize, windowCount;
    double startTime;
    double basePeriod;
    double segStart;
    double preTime;
    bool inBurst;
    unsigned long baseSampleCount, currBaseSampleCount;

public:
    BurstBufferHandler(MDSplus::Tree *tree, MDSplus::TreeNode *rawNode, double maxDelay, int freqDiv,
	double baseFreq, double startTime, double preTime, double postTime, int segmentSize, SaveListEV *saveList):BufferHandler(tree, rawNode, maxDelay, preTime, baseFreq, saveList)
    {
	this->preTime = preTime;
	this->baseFreq = baseFreq;
	this->basePeriod = 1./baseFreq;
	this->windowSize = (postTime + preTime) * this->baseFreq/freqDiv;
	this->segmentSize = segmentSize;
	if(this->segmentSize > this->windowSize)
	    this->segmentSize = this->windowSize;
	this->segBuffer = new short[this->segmentSize];
	this->freqDiv = freqDiv;
	this->startTime = startTime;
	this->inBurst = false;
	this->baseSampleCount = 0;
    }
    ~BurstBufferHandler()
    {
std::cout << "DISTRUTTORE BURST" << std::endl;
	delete [] segBuffer;
std::cout << "DISTRUTTO" << std::endl;
    }

    virtual void processSampleDelayed(short sample)
    {
	baseSampleCount++;
	currBaseSampleCount++;
//Check whether frequency switched
	double currTime = startTime + basePeriod * baseSampleCount;
//static int i = 0;
//if(i++ % 1000 == 0)
//    std::cout << currTime << std::endl;
	if(startTimes.size() > 0 && startTimes[0] <= currTime) //frequencySwitched.
	{
	    if(!inBurst)  //Trigger considered only if not serving a previous burst
	    {
		inBurst = true;
		currBaseSampleCount = 0;
		burstCount = 0;
		windowCount = 0;
		segStart = currTime;
	    }
	    startTimes.erase(startTimes.begin());
	}

	if(inBurst && (currBaseSampleCount % freqDiv == 0))
	{
	    segBuffer[burstCount++] = sample;
	    windowCount++;
	    if(burstCount >= segmentSize)
	    {
		double segEnd = segStart + (segmentSize-1) * basePeriod * freqDiv;
		MDSplus::Data *startSegData = new MDSplus::Float64(segStart);
		MDSplus::Data *endSegData = new MDSplus::Float64(segEnd);
		MDSplus::Data *periodData = new MDSplus::Float64(basePeriod * freqDiv);
		MDSplus::Data *dimData = MDSplus::compileWithArgs("build_range($, $, $)", tree, 3, startSegData, endSegData, periodData);
		MDSplus::Array *segData = new MDSplus::Int16Array(segBuffer, segmentSize);
std::cout << "SEG START: " << startSegData << std::endl;
std::cout << "SEG END: " << endSegData << std::endl;
std::cout << "SEG DIM: " << dimData << std::endl;
		saveList->addItem(SEGMENT_OP_MAKE, startSegData, endSegData, dimData, segData, rawNode);
		MDSplus::deleteData(periodData);
		if(windowCount > windowSize)
		    inBurst = false;
		else //There are still other segments to be stored for this burst
		{
		    burstCount = 0;
		    segStart = currTime;
		}
	    }
	    else if (windowCount > windowSize) //Last piece of burst
	    {
		double segEnd = segStart + (burstCount - 1) * basePeriod * freqDiv;
		MDSplus::Data *startSegData = new MDSplus::Float64(segStart);
		MDSplus::Data *endSegData = new MDSplus::Float64(segEnd);
		MDSplus::Data *periodData = new MDSplus::Float64(basePeriod * freqDiv);
		MDSplus::Data *dimData = MDSplus::compileWithArgs("build_range($, $, $)", tree, 3, startSegData, endSegData, periodData);
		MDSplus::Array *segData = new MDSplus::Int16Array(segBuffer,burstCount);
std::cout << "LAST SEG START: " << startSegData << std::endl;
std::cout << "LAST SEG END: " << endSegData << std::endl;
std::cout << "LAST SEG DIM: " << dimData << std::endl;
		saveList->addItem(SEGMENT_OP_MAKE, startSegData, endSegData, dimData, segData, rawNode);
		MDSplus::deleteData(periodData);
		inBurst = false;
	    }
	}
    }
    virtual void trigger(double trigTime)
    {
	double startTime = trigTime - preTime;
	size_t idx;
	if(startTimes.size() == 0 || startTimes[startTimes.size() - 1] <= startTime)
	    startTimes.push_back(startTime);
	else
	{
	    size_t idx;
	    for(idx = startTimes.size() - 1; idx > 0 && startTimes[idx] >= startTime; idx--);
	    startTimes.insert(startTimes.begin() + idx, startTime);
	}
    }
    virtual void flushBuffer()
    {
std::cout << "FLUSH BUFFERS" << std::endl;
    }
};

class EventHandler:public Event
{
    BufferHandler *bufHandler;
public:
    EventHandler(char *name, BufferHandler *buf):Event(name)
    {
std::cout << "Created event handler for " << name <<std::endl;
	this->bufHandler = buf;
    }
    void run()
    {
	int bufSize;
	MDSplus::Data *evData =  getData(); //Get raw data
	double triggerTime = evData->getDouble();
	std::cout << "RECEIVED EVENT " << getName() << " WITH DATA  " << evData << "\n";
	bufHandler->trigger(triggerTime);
	MDSplus::deleteData(evData);
    }
};


#define ADC_BUFFER_SIZE 1000
int pxi6259EV_readAndSaveAllChannels(int nChan, void *chanFdPtr, int *isBurst, int *f1Div, int *f2Div, double maxDelay, double baseFreq, double *preTimes, double *postTimes, double startTime, int bufSize, int segmentSize, char **eventNames, void *dataNidPtr, void *treePtr, void *saveListPtr, void *stopAcq)
{
    int chan;
    SaveListEV *saveList  = (SaveListEV *)saveListPtr;
    int *chanFd 	= (int *)chanFdPtr;
    int *dataNid 	= (int *)dataNidPtr;
    BufferHandler **bufferHandlers;
    EventHandler **eventHandlers;
    MDSplus::TreeNode **treeNodes;

    treeNodes = new MDSplus::TreeNode *[nChan];
    //Delete first all data nids
    for(int i = 0; i < nChan; i++)
    {
	try {
	    treeNodes[i] = new TreeNode(dataNid[i], (Tree *)treePtr);
	    treeNodes[i]->deleteData();

	}catch(MdsException &exc)
	{
	    printf("Error deleting data nodes\n");
	}
    }

    (*(int*)stopAcq) = 0;

    bufferHandlers = new BufferHandler *[nChan];
    memset(bufferHandlers, 0, sizeof(BufferHandler *) * nChan);
    eventHandlers = new EventHandler *[nChan];
    memset(eventHandlers, 0, sizeof(EventHandler *) * nChan);
   for( chan = 0; chan < nChan; chan++ )
    {
	if(isBurst[chan])
	    bufferHandlers[chan] = new BurstBufferHandler((MDSplus::Tree *)treePtr, treeNodes[chan], maxDelay, f1Div[chan], baseFreq, startTime, preTimes[chan], postTimes[chan], segmentSize, saveList);
	else
	    bufferHandlers[chan] = new ClockBufferHandler((MDSplus::Tree *)treePtr, treeNodes[chan], maxDelay, f1Div[chan], f2Div[chan], baseFreq, bufSize, segmentSize,
		startTime, preTimes[chan], postTimes[chan], saveList);
	if(eventNames[chan][0]) //Empty string is passed for no event
	{
	    eventHandlers[chan] = new EventHandler(eventNames[chan], bufferHandlers[chan]);
	    eventHandlers[chan]->start();
	}
	else
	    eventHandlers[chan] = NULL;
    }
    while( !(*(int*)stopAcq) )
    {
	short buffer[ADC_BUFFER_SIZE];
	for(chan = 0; chan < nChan; chan++)
	{
	    int currReadSamples = read(chanFd[chan], buffer, ADC_BUFFER_SIZE * 2);
	    if(currReadSamples <=0)
	    {
	        if (errno == EAGAIN || errno == ENODATA)
		{
	            usleep(50);
				currReadSamples = 0; // No data currently available... Try again
	            //continue;
	        }
	        else
	        {
	            if (errno == EOVERFLOW )
	            {
			printf("PXI 6259 Error reading samples on ai%d: (%d) %s \n", chan, errno, strerror(errno));
	            	for( chan = 0; chan < nChan; chan++ )
			{
			    if(eventHandlers[chan])
			    {
				eventHandlers[chan]->stop();
				delete eventHandlers[chan];
			    }
			    delete bufferHandlers[chan];
			}
			delete [] eventHandlers;
			delete [] bufferHandlers;
	                return -2;
	            }
	        }
	    }
	    else
	    {
		for(int sampleIdx = 0; sampleIdx < currReadSamples/sizeof(short); sampleIdx++)
		{
		    bufferHandlers[chan]->processSample(buffer[sampleIdx] );
//		    std::cout << buffer[sampleIdx] << std::endl;
		}
	    }
	}
    }


    for( chan = 0; chan < nChan; chan++ )
    {
	if(eventHandlers[chan])
	{
std::cout << "STOPPING EVENT HANDLER...." << std::endl;
	    eventHandlers[chan]->stop();
std::cout << "STOPPED" << std::endl;
	    delete eventHandlers[chan];
	}
	std::cout << "TERMINATING BUFFER HANDLER...." << std::endl;
	bufferHandlers[chan]->terminate();
std::cout << "TERMINATED" << std::endl;
    }
    saveList->stop();


std::cout << "DELETING eventHandlers" << std::endl;
    delete [] eventHandlers;
std::cout << "DELETING bufferHandlers" << std::endl;
//    for( chan = 0; chan < nChan; chan++ )
//	delete bufferHandlers[chan];
    delete [] bufferHandlers;
    printf("STOP PXI 6259 Acquisition \n");
    return 1;
}


