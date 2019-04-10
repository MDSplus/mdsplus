#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <mdsobjects.h>
#include <rpadc_dma_auto.h>
#include <signal.h>


extern "C" {
void rpadcStream(int fd, char *treeName, int shot, int chan1Nid, int chan2Nid, int triggerNid,
		int preSamples, int postSamples, int inSegmentSamples, double freq, int single);
int rpadcInit(int mode, int clock_mode, int preSamples, int postSamples, int trigFromChanA, int trigAboveThreshold, int trigThreshold,
	       int thresholdSamples, int decimation);
int rpadcTrigger(int fd);
void rpadcStop(int fd);
void openTree(char *name, int shot, MDSplus::Tree **treePtr);
}

enum rpadc_mode {
  STREAMING = 0,
  EVENT_STREAMING,
  EVENT_SINGLE,
  TRIGGER_STREAMING,
  TRIGGER_SINGLE
};

enum rpadc_clock_mode {
  INTERNAL = 0,
  TRIG_EXTERNAL,
  EXTERNAL,
  TRIG_EVENT,
  EXT_EVENT
};

#define DMA_STREAMING_SAMPLES 1024

struct rpadc_configuration
{
    enum rpadc_mode mode;
    enum rpadc_clock_mode clock_mode;
    unsigned short trig_threshold;		//Signal level for trigger
    char trig_above_threshold;  //If true, trigger when above threshold, below threshold otherwise
    char trig_from_chana;	//If true, the trigger is derived from channel A
    unsigned char trig_samples;          //Number of samples above/below threshol for validating trigger
    unsigned short pre_samples;		//Number of pre-trigger samples
    unsigned int post_samples;         //Number of post-trigger samples
    unsigned int decimation;		//Decimation factor (base frequency: 125MHz)
};

static bool stopped = false;

static int deviceFd = 0;

static void writeConfig(int fd, struct rpadc_configuration *config)
{
    struct rpadc_dma_auto_registers regs;
    memset(&regs, 0, sizeof(regs));
    unsigned int currVal = 0;
    unsigned int dmaBufSize = 0;
    if(config->mode == STREAMING)
	currVal |= 0x00000001;
    if(config->trig_from_chana)
	currVal |= 0x00000002;
    if(config->trig_above_threshold)
	currVal |= 0x00000004;
    if(config->mode == EVENT_STREAMING || config->mode == EVENT_SINGLE)
	currVal |= 0x00000008;
    if(config->mode == EVENT_STREAMING || config->mode == TRIGGER_STREAMING)
        currVal |= 0x00000010;
    if(config->clock_mode == TRIG_EXTERNAL || config->clock_mode == EXTERNAL)
	currVal |= 0x00000020;
    if(config->clock_mode == EXTERNAL)
	currVal |= 0x00000040;


    currVal |= ((config->trig_samples << 8) & 0x0000FF00);
    currVal |= ((config->trig_threshold << 16) & 0xFFFF0000);
    regs.mode_register_enable = 1;
    regs.mode_register = currVal;

    regs.pre_register_enable = 1;
    regs.pre_register = config->pre_samples & 0x0000FFFF;

    regs.post_register_enable = 1;
    regs.post_register = config->post_samples;


    regs.decimator_register_enable = 1;
    regs.decimator_register = config->decimation - 1;
printf("MODE REGISTER:%x\n", regs.mode_register);
    ioctl(fd, RPADC_DMA_AUTO_SET_REGISTERS, &regs);

    if(config->mode == STREAMING)
      dmaBufSize = DMA_STREAMING_SAMPLES * sizeof(unsigned int);
    else
      dmaBufSize = (config->post_samples + config->pre_samples) * sizeof(unsigned int);

    ioctl(fd, RPADC_DMA_AUTO_ARM_DMA, &dmaBufSize);

    currVal = 1;
    ioctl(fd, RPADC_DMA_AUTO_SET_PACKETIZER, &currVal);

}


static void readConfig(int fd, struct rpadc_configuration *config)
{
    unsigned int currVal;
    struct rpadc_dma_auto_registers regs;
    ioctl(fd,  RPADC_DMA_AUTO_GET_REGISTERS, &regs);

    currVal = regs.mode_register;
printf("MODE REGISTER:%x\n", regs.mode_register);
    if(currVal & 0x00000001)
	config->mode = STREAMING;
    else
    {
      if(currVal & 0x00000008)
      {
	  if(currVal & 0x00000010)
	      config->mode = EVENT_STREAMING;
	  else
	      config->mode = EVENT_SINGLE;
      }
      else
      {
	  if(currVal & 0x00000010)
	      config->mode = TRIGGER_STREAMING;
	  else
	      config->mode = TRIGGER_SINGLE;
      }
    }
    if(currVal & 0x00000020)
    {
	if(currVal & 0x00000040)
	    config->clock_mode = EXTERNAL;
	else
	    config->clock_mode = TRIG_EXTERNAL;
    }
    else
	config->clock_mode = INTERNAL;

    if(currVal &  0x00000002)
	config->trig_from_chana = 1;
    else
	config->trig_from_chana = 0;

    if(currVal &  0x00000004)
	config->trig_above_threshold = 1;
    else
	config->trig_above_threshold = 0;

    config->trig_samples = ((currVal >> 8) & 0x000000FF);
    config->trig_threshold = ((currVal >> 16) & 0x0000FFFF);

    config->post_samples = regs.post_register;
    config->post_samples = regs.pre_register;
    config->decimation = regs.decimator_register + 1;
}

static void adcStop(int fd)
{
    unsigned int val = 0x00000002;
    ioctl(fd, RPADC_DMA_AUTO_SET_COMMAND_REGISTER, &val);
}

static void dmaStop(int fd)
{
    std::cout << "DMA STOP" << fd << std::endl;
    ioctl(fd, RPADC_DMA_AUTO_STOP_DMA, 0);
}

static void adcArm(int fd)
{
    std::cout << "DMA ARM  " << fd << std::endl;
    unsigned int command = 0x00000001;  //Arm
    ioctl(fd, RPADC_DMA_AUTO_SET_COMMAND_REGISTER, &command);
}
static void adcTrigger(int fd)
{
    unsigned int command = 0x00000004;
    ioctl(fd, RPADC_DMA_AUTO_SET_COMMAND_REGISTER, &command);
    usleep(1000);
    command = 0;
    ioctl(fd, RPADC_DMA_AUTO_SET_COMMAND_REGISTER, &command);
}

static void adcClearFifo(int fd)
{
    ioctl(fd, RPADC_DMA_AUTO_CLEAR_TIME_FIFO, 0);
}
static void dmaStart(int fd, int cyclic)
{
    ioctl(fd, RPADC_DMA_AUTO_START_DMA, &cyclic);
}

static void sigHandler(int signo)
{
  if (signo == SIGINT)
  {
      if(deviceFd)
      {
	  stopped = true;
	  adcStop(deviceFd);
	  usleep(1000);
	  dmaStop(deviceFd);
	  std::cout << "ORA ESCO!!!!!\n";
	  exit(0);
      }
  }
}

static void writeSegment(MDSplus::Tree *t, MDSplus::TreeNode *chan1, MDSplus::TreeNode *chan2, MDSplus::Data *triggerTime, unsigned int *dataSamples,
			 double *startTimes, double *endTimes, int segmentSamples, int blocksInSegment, double freq)
{

  short *chan1Samples, *chan2Samples;
std::cout << "WRITE SEGMENT SAMPLES: " << segmentSamples << std::endl;
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


    MDSplus::Data *startData = new MDSplus::Float64(startTimes[0]);
    MDSplus::Data *endData = new MDSplus::Float64(endTimes[blocksInSegment-1]);
    MDSplus::Data *startSegData = MDSplus::compileWithArgs("$1+$2", t, 2, startData, triggerTime);
    MDSplus::Data *endSegData = MDSplus::compileWithArgs("$1+$2", t, 2, endData, triggerTime);
    try {
//std::cout << "MAKE SEGMENT  "<< chan1 << chan1Data << std::endl;
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
// Stop
void rpadcStop(int fd)
{
  stopped = true;
  adcStop(fd);
  usleep(100000);
  dmaStop(fd);
  usleep(100000);
  close(fd);
}
void rpadcStream(int fd, char *treeName, int shot, int chan1Nid, int chan2Nid, int triggerNid,
		int preSamples, int postSamples, int inSegmentSamples, double freq, int single)
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
    if((preSamples == 0 && postSamples == 0)) //eventSamples == 0 means continuous streaming
    {
	blockSamples = segmentSamples = inSegmentSamples;
	blocksInSegment = 1;
    }
    else if(single)
    {
	segmentSamples = preSamples + postSamples;
	blockSamples = segmentSamples;
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
	    unsigned int currSample = 0;
	    while(currSample < blockSamples)
	    {
		int rb = read(fd,&dataSamples[currBlock * blockSamples + currSample], (blockSamples - currSample)*sizeof(int));

		currSample += rb/sizeof(int);
		if(rb > 0)
		  numEvents++;
std::cout << "READ: " << rb << std::endl;
//std::cout << "CURR SAMPLE: " << currSample << "BLOCKS IN SEGMENT: " << blocksInSegment << std::endl;

		if(stopped)
		{
std::cout << "STOPPED!!!!!!!" << std::endl;
		    if (!(currBlock == 0 && currSample == 0))//if any data has been acquired
		    {
			if(rb > 0)
			{
			    if(numEvents > currBlock)
			    {
				unsigned long long currTime;
				unsigned int time1, time2;
				ioctl(fd, RPADC_DMA_AUTO_GET_TIME_FIFO_VAL, &time1);
				ioctl(fd, RPADC_DMA_AUTO_GET_TIME_FIFO_VAL, &time2);
				currTime = (unsigned long long)time1 | (((unsigned long long) time2) << 32);
				startTimes[currBlock] = (currTime - preSamples)/freq;
				endTimes[currBlock] =  (currTime + postSamples - 1)/freq;  //include last sample
				std::cout << "ULTIMO TIME: " << currTime << std::endl;
			    }
			    writeSegment(tree, chan1, chan2, trigger, dataSamples, startTimes, endTimes, currBlock * blockSamples + currSample, numEvents, freq);
			}
		    }
		   // adcStop(fd);
		   // usleep(100000);
		   // dmaStop(fd);
		   // usleep(100000);
		   // close(fd);
		    deviceFd = 0;
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
	    }
	    if(preSamples != 0 || postSamples != 0)
	    {
		unsigned long long currTime;
/*		if(ioctl(fd, RFX_RPADC_LAST_TIME, &currTime) < 0)
		    std::cout << "ERROR READING TIME" << std::endl;
*/
		unsigned int time1, time2;
		ioctl(fd, RPADC_DMA_AUTO_GET_TIME_FIFO_VAL, &time1);
		ioctl(fd, RPADC_DMA_AUTO_GET_TIME_FIFO_VAL, &time2);
		currTime = (unsigned long long)time1 | (((unsigned long long) time2) << 32);

std::cout << "TIME COUNTER: " << currTime <<  "DELTA:  " << currTime - prevTime << "   " << (currTime - prevTime)/freq << std::endl;
		prevTime = currTime;
		startTimes[currBlock] = ((long long)currTime - preSamples)/freq;
		endTimes[currBlock] =  (currTime + postSamples - 1)/freq;  //include last sample
//std::cout << "CURR BLOCK: " << currBlock << "   START TIME: " << startTimes[currBlock] << "  END TIME: " << endTimes[currBlock] << std::endl;

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
    switch (config->mode) {
      case STREAMING:
	  printf("\tmode: STREAMING\n");
	  break;
      case EVENT_STREAMING:
	  printf("\tmode: EVENT_STREAMING\n");
	  break;
      case EVENT_SINGLE:
	  printf("\tmode: EVENT_SINGLE\n");
	  break;
      case TRIGGER_STREAMING:
	  printf("\tmode: TRIGGER_STREAMING\n");
	  break;
      case TRIGGER_SINGLE:
	  printf("\tmode: TRIGGER_SINGLE\n");
	  break;
    }
    switch(config->clock_mode)  {
      case INTERNAL:
	  printf("\tclock_mode: INTERNAL\n");
	  break;
      case TRIG_EXTERNAL:
	  printf("\tclock_mode: TRIG_EXTERNAL\n");
	  break;
      case EXTERNAL:
	  printf("\tclock_mode: EXTERNAL\n");
	  break;
      case TRIG_EVENT:
	  printf("\tclock_mode: TRIG_EVENT\n");
	  break;
      case EXT_EVENT:
	  printf("\tclock_mode: EXT_EVENT\n");
	  break;
    }

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
int rpadcInit(int mode, int clock_mode, int preSamples, int postSamples, int trigFromChanA, int trigAboveThreshold, int trigThreshold,
	       int thresholdSamples, int decimation)
{
    struct rpadc_configuration inConfig, outConfig;
    int fd = open("/dev/rpadc_dma_auto", O_RDWR | O_SYNC);
     if(fd < 0)
        return -1;

     deviceFd = fd;

    stopped = true;
    //status = ioctl(fd, RFX_RPADC_FIFO_INT_HALF_SIZE, 0);
//    status = ioctl(fd, RFX_RPADC_FIFO_INT_FIRST_SAMPLE, 0);
//    usleep(10);
//    status = ioctl(fd, RFX_RPADC_RESET, 0);
//    usleep(10);

    inConfig.mode = (enum rpadc_mode)mode;
    inConfig.clock_mode = (enum rpadc_clock_mode)clock_mode;
    inConfig.trig_samples = thresholdSamples;
    inConfig.trig_above_threshold = trigAboveThreshold;
    inConfig.trig_from_chana = trigFromChanA;
    inConfig.trig_threshold = trigThreshold;
    inConfig.pre_samples = preSamples;
    inConfig.post_samples = postSamples;
    inConfig.decimation = decimation;
    printConfig(&inConfig);
    writeConfig(fd, &inConfig);
    memset(&outConfig, 0, sizeof(outConfig));
    printf("configurato \n");

    readConfig(fd, &outConfig);
    printConfig(&outConfig);
//    adcStop(fd);
    //status = ioctl(fd, RFX_RPADC_STOP, 0);
//    sleep(1);
//    dmaStop(fd);
//    sleep(1);
    adcClearFifo(fd);
    usleep(1000);
    adcArm(fd);
    usleep(1000);
    if(mode == 2 || mode == 4) //if single
	dmaStart(fd, 1);
//	dmaStart(fd, 0);
    else
	dmaStart(fd, 1);
    usleep(1000);

    return fd;
}

int rpadcTrigger(int fd)
{
    adcTrigger(fd);
//    int status = ioctl(fd, RFX_RPADC_TRIGGER, 0);
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
//    int preSamples = 0, postSamples = 30;
 //   if (signal(SIGINT, sigHandler) == SIG_ERR)
 //       std::cout << "\ncan't catch SIGINT\n";

    int preSamples = 100, postSamples = 200;
std::cout << "INIZIALIZZO...\n";
    int fd = rpadcInit(0, 0, preSamples, postSamples, 1, 1, 5000, 2, atoi(argv[1]));
std::cout << "INIZIALIZZATO\n";
    try {

      MDSplus::Tree *t = new MDSplus::Tree("redpitaya", -1);
      t->createPulse(1);
      t = new MDSplus::Tree("redpitaya", 1);
std::cout << "CREATO\n";
      MDSplus::TreeNode *chan1 = t->getNode("rpadc:chan_a");
      MDSplus::TreeNode *chan2 = t->getNode("rpadc:chan_b");
      MDSplus::TreeNode *trigger = t->getNode("rpadc:trigger");
std::cout << "LANCIO TRIGGER\n";
      rpadcTrigger(fd);
std::cout << "LANCIO STREAM\n";
      rpadcStream(fd, (char *)"redpitaya", 1, chan1->getNid(), chan2->getNid(), trigger->getNid(), preSamples, postSamples, 1000, 125E6/atoi(argv[1]), 0);

      rpadcStop(fd);
    }catch(MDSplus::MdsException &exc)
    {
	std::cout << exc.what() << std::endl;
	return 0;
    }
    return 0;
}

