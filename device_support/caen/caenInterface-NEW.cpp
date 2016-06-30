#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#include <CAENVMElib.h>
#include <mdsobjects.h>

using namespace MDSplus;

#define DECIMATION 1

//Support class for enqueueing storage requests
class SaveItem {
    void *segment;
    char dataType;
    int segmentSize;
    int startIdx_c;
    int endIdx_c;
    int segmentCount;
    int dataNid;
    int clockNid;
    int triggerNid;
    void *treePtr;
    SaveItem *nxt;

 public:
    SaveItem(void *segment, int segmentSize, char dataType, int startIdx_c, int endIdx_c, int segmentCount, int dataNid, int clockNid, int triggerNid, void *treePtr)
    {
	this->segment = segment;
	this->segmentSize = segmentSize;
	this->dataType = dataType;
	this->startIdx_c = startIdx_c;
	this->endIdx_c = endIdx_c;
	this->segmentCount = segmentCount;
	this->dataNid = dataNid;
	this->clockNid = clockNid;
	this->treePtr = treePtr;
	this->triggerNid = triggerNid;
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
	TreeNode *triggerNode = new TreeNode(triggerNid, (Tree *)treePtr);


    	Data *startTime;
	Data *endTime;
	Data *dim;
	Data *startIdx = new Int32(startIdx_c);
	Data *endIdx  = new Int32(endIdx_c);
	Data *segCount = new Int32(segmentCount);
    	Data *decimation = new Int32(DECIMATION);


  	startTime = compileWithArgs("$[$] + slope_of($) * $",(Tree *)treePtr, 4, triggerNode, segCount, clockNode, startIdx);
 	endTime = compileWithArgs("$[$] + slope_of($) * $ * $",(Tree *)treePtr, 5, triggerNode, segCount, clockNode, decimation, endIdx);
 	dim = compileWithArgs("build_range($[$]+slope_of($) * $, $[$]+slope_of($) * $ * $, slope_of($) * $ )", (Tree *)treePtr, 
                                       11, triggerNode, segCount, clockNode, startIdx, triggerNode, segCount, 
                                          clockNode, decimation, endIdx, clockNode, decimation);

        try 
        {

	     switch( dataType )
	     {
		case DTYPE_W:
        {
	        short *fBuf = new short[segmentSize];
			memset(fBuf, 0, sizeof(short) * segmentSize);
			Int16Array *fData = new Int16Array((short *)segment, segmentSize);
            printf("Save segment idx %d Path %s \n", segmentCount, dataNode->getPath() );
			dataNode->makeSegment(startTime, endTime, dim, fData);
			delete [] fBuf;
			deleteData(fData);
		}
		break;
		case DTYPE_F:
		{
			float *fBuf = new float[segmentSize];
			memset(fBuf, 0, sizeof(float) * segmentSize);
			Float32Array *fData = new Float32Array((float *)segment, segmentSize);
			dataNode->makeSegment(startTime, endTime, dim, fData);
			delete [] fBuf;
			deleteData(fData);
		}
		break;
	     }
	    free(segment);
	    deleteData(startIdx);
 	    deleteData(endIdx);
	    deleteData(startTime);
	    deleteData(endTime);
	    deleteData(segCount);

	}
 	catch(MdsException *exc) 
 	{
		printf("Cannot put segment: %s\n", exc->what());
 	}
 	delete dataNode;
 	delete clockNode;
 	delete triggerNode;
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
    void addItem(void *segment, int segmentSize, char dataType, int startIdx_c, int endIdx_c, int segmentCount, int dataNid, int clockNid, int triggerNid, void *treePtr)
    {
//printf("add Item\n");
		void *segmentc = calloc(1, segmentSize * sizeof(short));
		//memcpy(segmentc, segment, segmentSize * sizeof(short) );

    
        for( int i = 0, j = 0; i < segmentSize; i += DECIMATION, j++ )
            ((short *)segmentc)[j] = ((short *)segment)[i];

        segmentSize = segmentSize/DECIMATION;
        endIdx_c = endIdx_c / DECIMATION; 

		SaveItem *newItem = new SaveItem(segmentc, segmentSize, dataType, startIdx_c, endIdx_c, segmentCount,  dataNid, clockNid, triggerNid, treePtr);
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


extern "C" void openTree(char *name, int shot, void **treePtr)
{
    try {
	Tree *tree = new Tree(name, shot);
	*treePtr = (void *)tree;
    }
    catch(MdsException *exc) 
    {
	printf("Cannot open tree %s %d: %s\n", name, shot, exc->what());
    }
}


#define MAX_CHANNELS 4

struct segment_struct
{
    int eventSize;
    int boardGroup;
    int counter;
    int time;
    short *data;
};

void onError(const char *msg )
{
    printf("Error : %s", msg);
}



extern "C" int readAndSaveSegments(int32_t handle, int32_t vmeAddress, int numChannels, int nActChans, int segmentSamples, int segmentSize, int startIdx, int endIdx, int pts, int useCounter, int chanMask, int segmentCounter, int *dataNidPtr, int clockNid, int triggerNid, void *treePtr, void *saveListPtr)

{

// Read number of buffers 
    int actSegments = 0;
    int status;
    int prevLastSegIdx, currSegmentIdx;
    struct segment_struct segment;
    short* channels[MAX_CHANNELS];
    SaveList *saveList = (SaveList *)saveListPtr;


    int currStartIdx    = segmentSamples - pts + startIdx;
    int currEndIdx      = segmentSamples - pts + endIdx;
    int currChanSamples = currEndIdx - currStartIdx;

    printf("readAndSaveSegments\n");

    status = CAENVME_ReadCycle(handle, (vmeAddress + 0x812C), &actSegments, cvA32_S_DATA, cvD32);
    if ( status != 0 )
    {
        onError("Error reading number of acquired segments");
        return 0;
    }

    printf("actSegments %d nActChans %d segmentSamples %d currChanSamples %d\n", actSegments, nActChans, segmentSamples, currChanSamples);
    
    short *buff = (short *)calloc( 1, 16 + nActChans * segmentSamples * sizeof(short) );

    for(int chan = 0; chan < numChannels; chan++)
    {	
       channels[chan] = (short *)calloc(1, currChanSamples * actSegments * sizeof(short));
    }
          for( int segmentIdx = 0; segmentIdx < actSegments; segmentIdx++ )
	  {

              int retLen = 0;
    	      //printf("Read CAENVME_FIFOBLTReadCycle %d vmeAddress %d segmentSize %d\n", segmentIdx, vmeAddress, segmentSize);

              status = CAENVME_FIFOBLTReadCycle(handle, vmeAddress, buff, segmentSize,  cvA32_S_DATA, cvD64, &retLen);


              if ( status != 0 )
              {
		          free(buff);
	              onError("ASYNCH: Error reading data segment");
                  return 0;
              }

	          memcpy(&segment, buff, 4 * sizeof(int) );          

              int actSize = 4 * (segment.eventSize & 0x0fffffff);
              int counter = segment.time/2;
              int sizeInInts = (segment.eventSize & 0x0fffffff) - 4;
              int chanSizeInInts = sizeInInts/nActChans;
              int chanSizeInShorts = chanSizeInInts * 2;

    	      printf( "Read actSize %d counter %d sizeInInts %d retLen %d Index %d\n", actSize, counter, sizeInInts, retLen, segment.time % actSize );

	
            for( int  chan = 0; chan < numChannels; chan++)
	    {
                if ( (chanMask & (1 << chan))  != 0 )
		{

		    printf("Seg Idx %d - channels[%d][%d : %d], &buff[%d : %d]\n", segmentIdx, chan, segmentIdx * currChanSamples,  currChanSamples, 16 + chan * chanSizeInShorts + currStartIdx, currChanSamples);
		    memcpy( &channels[chan][segmentIdx * currChanSamples], &buff[16 + chan * chanSizeInShorts + currStartIdx], currChanSamples  * sizeof(short) );
		}
           }
          }

          if ( actSegments > 0 )
	  {

            for( int chan = 0; chan < numChannels; chan++)
	    {
                 if ( (chanMask & (1 << chan)) != 0 )
		 {

		      for( int seg = 0; seg < actSegments; seg++ )
		      {	

                      	  saveList->addItem((void *)&channels[chan][seg * currChanSamples], currChanSamples,  
						DTYPE_W, startIdx, endIdx, (segmentCounter + seg), dataNidPtr[chan], clockNid, triggerNid, treePtr);

		      }
                 }
		free(channels[chan]);
            } 
	}
	return ( segmentCounter + actSegments );
    
}




