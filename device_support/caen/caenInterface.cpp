#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#include <CAENVMElib.h>
#include <mdsobjects.h>
using namespace MDSplus;


int caenLibDebug = 0;

//////////////////////////////////////////////////////////////
// Filter Code Definitions
//////////////////////////////////////////////////////////////

// law pas  filter centred around 10MHz
// sampling rate = 100MHz
// gain at 10 MHz is about 1.
#define FILTER_LEN 25
int16_t coeffs[ FILTER_LEN ] =
{ 547, 469, 95,
-872,-2261,
-3518,
-3845,
-2528,
677,
5300,
10204,
13951,
15353,
13951,
10204,
5300,
677,
-2528,
-3845,
-3518,
-2261,
-872,
95,
469,
547};

#define SAMPLES 8000

// maximum number of inputs that can be handled
// in one function call
#define MAX_INPUT_LEN 8000
// maximum length of filter than can be handled
#define MAX_FLT_LEN 63
// buffer to hold all of the input samples
#define BUFFER_LEN (MAX_FLT_LEN - 1 + MAX_INPUT_LEN)
// array to hold input samples
int16_t insamp[ BUFFER_LEN ];
// FIR init
void firFixedInit( void )
{
 memset( insamp, 0, sizeof( insamp ) );
}
// the FIR filter function
void firFixed( int16_t *coeffs,  int16_t *input, int16_t *output, int length, int filterLength )
{
 int32_t acc; // accumulator for MACs
 int16_t *coeffp; // pointer to coefficients
 int16_t *inputp; // pointer to input samples
 int n;
 int k;
 // put the new samples at the high end of the buffer
 memcpy( &insamp[filterLength - 1], input, length * sizeof(int16_t) );
 // apply the filter to each input sample
 for ( n = 0; n < length; n++ ) 
 {
 	// calculate output n
 	coeffp = coeffs;
 	inputp = &insamp[filterLength - 1 + n];
 	// load rounding constant
 	acc = 1 << 14;
 	// perform the multiply-accumulate
 	for ( k = 0; k < filterLength; k++ ) {
 		acc += (int32_t)(*coeffp++) * (int32_t)(*inputp--);
 	}
 	// saturate the result
 	if ( acc > 0x3fffffff ) {
 		acc = 0x3fffffff;
 	} else if ( acc < -0x40000000 ) {
 		acc = -0x40000000;
 	}
 	// convert from Q30 to Q15
 	output[n] = (int16_t)(acc >> 15);
 }
 // shift input samples back in time for next time
 memmove( &insamp[0], &insamp[length],
 (filterLength - 1) * sizeof(int16_t) );
}


//////////////////////////////////////////////////////////////
// Filter Code Definitions End
//////////////////////////////////////////////////////////////


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


  	startTime = compileWithArgs("$[$]+slope_of($)*$",(Tree *)treePtr, 4, triggerNode, segCount, clockNode, startIdx);
 	endTime =   compileWithArgs("$[$]+slope_of($)*$",(Tree *)treePtr, 4, triggerNode, segCount, clockNode, endIdx);
 	dim = compileWithArgs("build_dim(build_window($, $, $[$]), build_slope( slope_of($) ) )", (Tree *)treePtr, 5, startIdx, endIdx, triggerNode, segCount, clockNode);


        try 
        {

	     switch( dataType )
	     {
		case DTYPE_W:
        	{
			Int16Array *sData = new Int16Array((short *)segment, segmentSize);
            		printf("Save short data segment idx %d endIdx_c %d\n", segmentCount, endIdx_c );
			dataNode->makeSegment(startTime, endTime, dim, sData);
			deleteData(sData);
		}
		break;
		case DTYPE_F:
		{
			Float32Array *fData = new Float32Array((float *)segment, segmentSize);
            		printf("Save flat data segment idx %d endIdx_c %d\n", segmentCount, endIdx_c );
			dataNode->makeSegment(startTime, endTime, dim, fData);
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
		pthread_mutex_init(&mutex, NULL);
		pthread_cond_init(&itemAvailable, NULL);
		saveHead = saveTail = NULL;
		stopReq = false;
		threadCreated = false;
    }

    void addItem(void *segment, int segmentSize, char dataType, int startIdx_c, int endIdx_c, int segmentCount, int dataNid, int clockNid, int triggerNid, void *treePtr)
    {
//printf("add Item\n");

		int noFilter = 1;
		void *segmentc = calloc(1, segmentSize * sizeof(short));
		int size;

		if( noFilter )
		{			
			memcpy(segmentc, segment, segmentSize * sizeof(short) );
		}
		else
		{
			 int cIdx = 0, currSize;
			 // initialize the filter
			 firFixedInit();
			 // process all of the samples
			currSize = segmentSize;
			size = SAMPLES;
printf("----Start Filter\n");
			while(currSize > 0)
			{
				size = (size > currSize ? currSize : size);
				firFixed( coeffs, &((int16_t *)segment)[cIdx], &((int16_t *)segmentc)[cIdx], size, FILTER_LEN );
				currSize -= size;
                                cIdx += size;
			}
		}
	
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

extern "C" int readAndSaveSegments(int32_t handle, int32_t vmeAddress, int numChannels, int nActChans, int segmentSamples, int segmentSize, int startIdx, int endIdx, int pts, int useCounter, int chanMask, int segmentCounter, int numSegment, int *dataNidPtr, int clockNid, int triggerNid, void *treePtr, void *saveListPtr)

{
// numSegmens < 0 for Continuous acquisition , numSegmens > 0 for transient recorder acquisition
 
	    int actSegments = 0;
	    int status;
	    struct segment_struct segment;
	    short* channels[MAX_CHANNELS];
	    SaveList *saveList = (SaveList *)saveListPtr;


	    int currStartIdx    = segmentSamples - pts + startIdx;
	    int currEndIdx      = segmentSamples - pts + endIdx;
	    int currChanSamples = currEndIdx - currStartIdx;
           

	 // Read number of buffers 
	    status = CAENVME_ReadCycle(handle, (vmeAddress + 0x812C), &actSegments, cvA32_S_DATA, cvD32);
	    if ( status != 0 )
	    {
		onError("Error reading number of acquired segments");
		return 0;
	    }

	    if ( caenLibDebug ) printf("segmentCounter %d actSegments %d nActChans %d segmentSamples %d currChanSamples %d\n", segmentCounter, actSegments, nActChans, segmentSamples, currChanSamples);
	    
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

    	      if( caenLibDebug ) printf( "Read actSize %d counter %d sizeInInts %d retLen %d Index %d\n", actSize, counter, sizeInInts, retLen, segment.time % actSize );

	
            for( int  chan = 0; chan < numChannels; chan++)
	    {
                if ( (chanMask & (1 << chan))  != 0 )
		{
		    if( caenLibDebug )
			printf("---Seg Idx %d - channels[%d][%d : %d], &buff[%d : %d]\n", segmentIdx, chan, segmentIdx * currChanSamples,  currChanSamples, 
											  16 + chan * chanSizeInShorts + currStartIdx, currChanSamples);

		    memcpy( &channels[chan][segmentIdx * currChanSamples], &buff[16 + chan * chanSizeInShorts + currStartIdx], currChanSamples  * sizeof(short) );
		}
             }
          }

	  free(buff);

          if ( actSegments > 0 )
	  {

            for( int chan = 0; chan < numChannels; chan++)
	    {
                 if ( (chanMask & (1 << chan)) != 0 )
		 {
		      for( int seg = 0; seg < actSegments; seg++ )
		      {	
			  if( numSegment > 0 && segmentCounter + seg > numSegment )
			  {
				if( caenLibDebug )printf("skip seg %d for channel %d\n", (segmentCounter + seg), chan);
				break;
                          }
                      	  saveList->addItem((void *)&channels[chan][seg * currChanSamples], currChanSamples,  
						DTYPE_W, startIdx, endIdx, (segmentCounter + seg), dataNidPtr[chan], clockNid, triggerNid, treePtr);                          
		      }
                 }
		free(channels[chan]);
            } 
	}
       
        if( numSegment > 0 && segmentCounter + actSegments > numSegment )
	    return ( numSegment );
	else
	    return ( segmentCounter + actSegments );
    
}





