#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <mdsobjects.h>
#include <rpadc_fifo.h>

extern "C" {
void rpadcStream(int fd, char *treeName, int shot, int chan1Nid, int chan2Nid, int triggerNid, 
		int preSamples, int postSamples, int inSegmentSamples, double freq);
int rpadcInit(int preSamples, int postSamples, int trigFromChanA, int trigAboveThreshold, int trigThreshold, 
	       int thresholdSamples, int decimation);
int rpadcTrigger(int fd);
void rpadcStop(int fd);
void openTree(char *name, int shot, MDSplus::Tree **treePtr);
}

static bool stopped = false;
static void writeSegment(MDSplus::Tree *t, MDSplus::TreeNode *chan1, MDSplus::TreeNode *chan2, MDSplus::Data *triggerTime, unsigned int *dataSamples,
			 double *startTimes, double *endTimes, int segmentSamples, int blocksInSegment, double freq)
{
  
  short *chan1Samples, *chan2Samples;

    chan1Samples = new short[segmentSamples];
    chan2Samples = new short[segmentSamples];
    
    for(int i = 0; i < segmentSamples; i++)
    {
	chan1Samples[i] = dataSamples[i] & 0x0000ffff;
	chan2Samples[i] = (dataSamples[i] >> 16) & 0x0000ffff;
    }
    
    
    MDSplus::Array *chan1Data = new MDSplus::Int16Array(chan1Samples, segmentSamples);
    MDSplus::Array *chan2Data = new MDSplus::Int16Array(chan2Samples, segmentSamples);
    
    MDSplus::Data *timebase;
    
    
    
    MDSplus::Data *startTimeData;
    MDSplus::Data *endTimeData;
    MDSplus::Data *deltaData = new MDSplus::Float64(1/freq);
    if(blocksInSegment == 1)
    {
	startTimeData = new MDSplus::Float64(startTimes[0]);
	endTimeData = new MDSplus::Float64(endTimes[0]);
    }
    else
    {
	startTimeData = new MDSplus::Float64Array(startTimes, blocksInSegment);
	endTimeData = new MDSplus::Float64Array(endTimes, blocksInSegment);
    }
    timebase = MDSplus::compileWithArgs("make_range($1+$2, $3+$4, $5)", t, 5, startTimeData, triggerTime, endTimeData, triggerTime, deltaData);
    deleteData(startTimeData);
    deleteData(endTimeData);
    deleteData(deltaData);
      
//	timebase = new MDSplus::Range(new MDSplus::Float64(startTimes[0]), new MDSplus::Float64(endTimes[0]), new MDSplus::Float64(1/freq));

	
	
    MDSplus::Data *startData = new MDSplus::Float64(startTimes[0]);
    MDSplus::Data *endData = new MDSplus::Float64(endTimes[blocksInSegment-1]);
    MDSplus::Data *startSegData = MDSplus::compileWithArgs("$1+$2", t, 2, startData, triggerTime);
    MDSplus::Data *endSegData = MDSplus::compileWithArgs("$1+$2", t, 2, endData, triggerTime);
/*    std::cout << chan1 << std::endl;
    std::cout << chan2 << std::endl;
    std::cout << timebase << std::endl;
    std::cout << startData << std::endl;
    std::cout << endData << std::endl;
    std::cout << chan1Data << std::endl;
  */
    try {
std::cout << "MAKE SEGMENT  "<< chan1 << std::endl;
        chan1->makeSegment(startSegData, endSegData, timebase, chan1Data);
	chan2->makeSegment(startSegData, endSegData, timebase, chan2Data);
    }catch(MDSplus::MdsException &exc)
    {
	std::cout << "Error writing segment: " << exc.what() << std::endl;
    }
    MDSplus::deleteData(chan1Data);
    MDSplus::deleteData(chan2Data);
    MDSplus::deleteData(timebase);
    MDSplus::deleteData(startData);
    MDSplus::deleteData(endData);
    MDSplus::deleteData(startSegData);
    MDSplus::deleteData(endSegData);
    delete[] chan1Samples;
    delete[] chan2Samples;
 // std::cout << "WRITESEGMENT END" << std::endl;
}

void rpadcStop(int fd)
{
  stopped = true;
  ioctl(fd, RFX_RPADC_STOP, 0);
} 
void rpadcStream(int fd, char *treeName, int shot, int chan1Nid, int chan2Nid, int triggerNid, 
		int preSamples, int postSamples, int inSegmentSamples, double freq)
{
    int segmentSamples;  //True segment dimension
    int blockSamples;    //post samples + pre samples for event streaming, segmentSize for continuous streaming
    int blocksInSegment;  //1 fir cintinous streaming
    unsigned int  *dataSamples;
    double *startTimes, *endTimes;
    int numEvents;
    int prevTime = 0;    
    stopped = false;
    MDSplus::Tree *tree = new MDSplus::Tree(treeName, shot);
    MDSplus::TreeNode *chan1 = new MDSplus::TreeNode(chan1Nid, tree);
    MDSplus::TreeNode *chan2 = new MDSplus::TreeNode(chan2Nid, tree);
    MDSplus::TreeNode *trigger = new MDSplus::TreeNode(triggerNid, tree);
    if(preSamples == 0 && postSamples == 0) //eventSamples == 0 means continuous streaming
    {
	blockSamples = segmentSamples = inSegmentSamples;
	blocksInSegment = 1;
    }
    else
    {
	int eventSamples = preSamples + postSamples;
	if(eventSamples > inSegmentSamples)
	    inSegmentSamples = eventSamples;  //Force consistenct
	segmentSamples = (inSegmentSamples / eventSamples) * eventSamples;
	blockSamples = eventSamples;
	blocksInSegment = segmentSamples/blockSamples;
    }
    
    
    std::cout << "blocksInSegment: " << blocksInSegment << "   segmentSamples: " << segmentSamples << "   BlockSamples: " << blockSamples << std::endl;
    
    
    
    dataSamples = new unsigned int[segmentSamples];
    memset(dataSamples, 0, segmentSamples*sizeof(int));
    startTimes = new double[blocksInSegment];
    endTimes = new double[blocksInSegment];
    int segmentIdx = 0;
    while(true)
    {
	numEvents = 0;
	for(int currBlock = 0; currBlock < blocksInSegment; currBlock++)
	{
	    for(int currSample = 0; currSample < blockSamples;)
	    {
std::cout << "READING " << (blockSamples - currSample) << "CurrSample: " << currSample << "  Block samples: "<< blockSamples << std::endl;
		int rb = read(fd,&dataSamples[currBlock * blockSamples + currSample], (blockSamples - currSample)*sizeof(int));
std::cout << "READ " << rb/sizeof(int) << std::endl;
		if(stopped)
		{
std::cout << "STOPPED!!!!!!!" << std::endl;
		    if(rb > 0)
			writeSegment(tree, chan1, chan2, trigger, dataSamples, startTimes, endTimes, currSample, numEvents, freq);
		    close(fd);
		    delete chan1;
		    delete chan2;
		    delete trigger;
		    return;
		}
		if(rb < 0)
		{
		    printf("DEVICE FIFO OVERFLOW!!!!\n");
		    return;
		}
		currSample += rb/sizeof(int);
	    }
	    if(preSamples != 0 || postSamples != 0)
	    {
		unsigned long long currTime;
		if(ioctl(fd, RFX_RPADC_LAST_TIME, &currTime) < 0)
		    std::cout << "ERROR READING TIME" << std::endl;
std::cout << "TIME COUNTER: " << currTime <<  "DELTA:  " << currTime - prevTime << "   " << (currTime - prevTime)/freq << std::endl;
		prevTime = currTime;
		startTimes[currBlock] = (currTime - preSamples)/freq;
		endTimes[currBlock] =  (currTime + postSamples - 1)/freq;  //include last sample
	    }
	}
	if(preSamples == 0 && postSamples == 0)
	{
	    startTimes[0] = segmentIdx * segmentSamples/freq;
	    endTimes[0] = (segmentIdx + 1) * segmentSamples/freq;
	}
	segmentIdx++;
	
	writeSegment(tree, chan1, chan2, trigger, dataSamples, startTimes, endTimes, segmentSamples, blocksInSegment,freq);
    }
}

static void printConfig(struct rpadc_configuration *config)
{
    printf("CONFIG:\n");
    if(config->mode == STREAMING)
	printf("\tmode: STREAMING\n");
    else
 	printf("\tmode: EVENT_STREAMING\n");
     
    if(config->trig_above_threshold)
	printf("\ttrig_above_threshold: true\n");
    else
 	printf("\ttrig_above_threshold: false\n");

    if(config->trig_from_chana)
	printf("\ttrig_from_chana: true\n");
    else
 	printf("\ttrig_from_chana: false\n");
    
    printf("\ttrig_samples: %d\n", config->trig_samples);
    printf("\ttrig_threshold: %d\n", config->trig_threshold);
    printf("\tpre_samples: %d\n", config->pre_samples);
    printf("\tpost_samples: %d\n", config->post_samples);
    printf("\tdecimation: %d\n", config->decimation);
}



//return either NULL or an error string
int rpadcInit(int preSamples, int postSamples, int trigFromChanA, int trigAboveThreshold, int trigThreshold, 
	       int thresholdSamples, int decimation)
{
    int status;
    struct rpadc_configuration inConfig, outConfig;
    int fd = open("/dev/rpadc_fifo", O_RDWR | O_SYNC);
    if(fd < 0) 
        return -1;
  
    stopped = true;
    sleep(1);
    status = ioctl(fd, RFX_RPADC_STOP, 0);
    usleep(100);
    //status = ioctl(fd, RFX_RPADC_FIFO_INT_HALF_SIZE, 0);
    status = ioctl(fd, RFX_RPADC_FIFO_INT_FIRST_SAMPLE, 0);
    usleep(10);
    status = ioctl(fd, RFX_RPADC_RESET, 0);
    usleep(10);

    if(preSamples == 0 && postSamples == 0)
	inConfig.mode = STREAMING;
    else
        inConfig.mode = EVENT_STREAMING;
    
    inConfig.trig_samples = thresholdSamples;
    inConfig.trig_above_threshold = trigAboveThreshold;
    inConfig.trig_from_chana = trigFromChanA;
    inConfig.trig_threshold = trigThreshold;
    inConfig.pre_samples = preSamples;
    inConfig.post_samples = postSamples;
    inConfig.decimation = decimation;
    printConfig(&inConfig);
    status = ioctl(fd, RFX_RPADC_SET_CONFIG, &inConfig);
    memset(&outConfig, 0, sizeof(outConfig));
    printf("configurato \n");
     
    status = ioctl(fd, RFX_RPADC_GET_CONFIG, &outConfig);
    printf("letto \n");
    printConfig(&outConfig);
    status = ioctl(fd, RFX_RPADC_CLEAR, 0);
    usleep(10);
    status = ioctl(fd, RFX_RPADC_RESET, 0);
    usleep(10);
    status = ioctl(fd, RFX_RPADC_ARM, 0);
    std::cout << "FD: " << fd << std::endl;
    return fd;
}
    
int rpadcTrigger(int fd)
{
    int status = ioctl(fd, RFX_RPADC_TRIGGER, 0);
    usleep(10);
    return 0;
}

void openTree(char *name, int shot, MDSplus::Tree **treePtr)
{
    try {
      *treePtr = new MDSplus::Tree(name, shot);
    }
    catch(MDSplus::MdsException &exc)
    {
	*treePtr = 0;
    }
}

int main(int argc, char *argv[])
{
    int preSamples = 30, postSamples = 50;
//    int preSamples = 0, postSamples = 0;
    int fd = rpadcInit(preSamples, postSamples, 1, 1, 7800, 2, atoi(argv[1]));
    int stopFlag = 0;
    try {
      
      MDSplus::Tree *t = new MDSplus::Tree("test", -1);
      t->createPulse(1);
      t = new MDSplus::Tree("test", 1);
      MDSplus::TreeNode *chan1 = t->getNode("rpadc:chan_a");
      MDSplus::TreeNode *chan2 = t->getNode("rpadc:chan_b");
      MDSplus::TreeNode *trigger = t->getNode("rpadc:trigger");
      rpadcTrigger(fd);
      rpadcStream(fd, "test", 1, chan1->getNid(), chan2->getNid(), trigger->getNid(), preSamples, postSamples, 1000, 125E6/atoi(argv[1]));
  
    }catch(MDSplus::MdsException &exc) 
    {
	std::cout << exc.what() << std::endl;
	return 0;
    }
    return 0;
}
  
    