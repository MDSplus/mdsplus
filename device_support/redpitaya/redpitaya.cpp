#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <mdsobjects.h>
#include <rpadc_fifo_freq_auto.h>
#include <rpadc_fifo_auto.h>
#include <signal.h>


extern "C" {
void rpadcStream(int fd, char *treeName, int shot, int chan1Nid, int chan2Nid, int triggerNid, 
		int preSamples, int postSamples, int inSegmentSamples, double freq, double freq1, int single);
<<<<<<< HEAD
int rpadcInit(int mode, int clock_mode, int preSamples, int postSamples, int trigFromChanA, 
	      int trigAboveThreshold, int trigThreshold, 
	      int thresholdSamples, int decimation, int event_code);
=======
void rpadcFreqStream(int fd, char *treeName, int shot, int chan1Nid, int chan2Nid, int triggerNid, 
		int preSamples, int postSamples, int inSegmentSamples, double freq, double freq1, int single);
int rpadcInit(int mode, int clock_mode, int preSamples, int postSamples, int trigFromChanA, int trigAboveThreshold, int trigThreshold, 
	       int thresholdSamples, int decimation, int event_code);
int rpadcFreqInit(int mode, int clock_mode, int preSamples, int postSamples, int trigFromChanA, int trigAboveThreshold, int trigThreshold, 
	       int thresholdSamples, int decimation, int nCycles, int highThresholdCh1, int highThresholdCh2, int lowThresholdCh1, int lowThresholdCh2,
	       int calibCh1, int calibCh2);
>>>>>>> 65bdc7acbb0ce9e5905a8f45695ba1556d311f51
int rpadcTrigger(int fd);
void rpadcStop(int fd);
int rpadcFreqTrigger(int fd);
void rpadcFreqStop(int fd);
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
  EXT_EVENT,
  HIGHWAY
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
    unsigned int event_code;          //Trigger event code (if HIGHWAY clock mode)
<<<<<<< HEAD
=======
};

struct rpadc_freq_configuration
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
    unsigned int ncycles; 
    unsigned int high_threshold_ch1; 
    unsigned int high_threshold_ch2; 
    unsigned int low_threshold_ch1; 
    unsigned int low_threshold_ch2; 
    unsigned int calib_ch1; 
    unsigned int calib_ch2; 
>>>>>>> 65bdc7acbb0ce9e5905a8f45695ba1556d311f51
};


static bool stopped = false;

static int deviceFd = 0;

static void writeConfig(int fd, struct rpadc_configuration *config)
{
    struct rpadc_fifo_auto_registers regs;
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
    if(config->clock_mode == EXTERNAL || config->clock_mode == HIGHWAY)
	currVal |= 0x00000040;
    if(config->clock_mode == HIGHWAY)
	currVal |= 0x00000080;
      
    
	
    currVal |= ((config->trig_samples << 8) & 0x0000FF00);
    currVal |= ((config->trig_threshold << 16) & 0xFFFF0000);
    regs.mode_register_enable = 1;
    regs.mode_register = currVal;
    
    regs.pre_register_enable = 1;
    regs.pre_register = config->pre_samples & 0x0000FFFF;
    
    regs.post_register_enable = 1;
    regs.post_register = config->post_samples;
    
    regs.trig_event_code_enable = 1;
    regs.trig_event_code = config->event_code;
    
    regs.decimator_register_enable = 1;

//Decimator IP seems not to work properly is decimation set the firsttime to 1
    regs.decimator_register = 10;
    ioctl(fd, RPADC_FIFO_AUTO_SET_REGISTERS, &regs);
    usleep(10000);

    regs.decimator_register = config->decimation - 1;
printf("MODE REGISTER:%x\n", regs.mode_register);
    ioctl(fd, RPADC_FIFO_AUTO_SET_REGISTERS, &regs);
printf("REGISTRI SCRITTI:%x\n", regs.mode_register);

/*
    if(config->mode == STREAMING)
      dmaBufSize = DMA_STREAMING_SAMPLES * sizeof(unsigned int); 
    else
      dmaBufSize = (config->post_samples + config->pre_samples) * sizeof(unsigned int);

    ioctl(fd, RPADC_DMA_AUTO_ARM_DMA, &dmaBufSize);
*/    
    currVal = 1;
    ioctl(fd, RPADC_FIFO_AUTO_SET_PACKETIZER, &currVal);

    ioctl(fd, RPADC_FIFO_AUTO_CLEAR_DATA_FIFO, NULL);
    //ioctl(fd, RPADC_FIFO_AUTO_FIFO_INT_HALF_SIZE, NULL);
    ioctl(fd, RPADC_FIFO_AUTO_FIFO_INT_FIRST_SAMPLE, NULL);


    
}

static void writeFreqConfig(int fd, struct rpadc_freq_configuration *config)
{
    struct rpadc_fifo_freq_auto_registers regs;
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

//Decimator IP seems not to work properly is decimation set the firsttime to 1
    regs.decimator_register = 10;
    
    regs.ncycles_enable = 1;
    regs.ncycles = config->ncycles;
    regs.high_threshold_ch1_enable = 1;
    regs.high_threshold_ch1 = config->high_threshold_ch1;
    regs.low_threshold_ch1_enable = 1;
    regs.low_threshold_ch1 = config->low_threshold_ch1;
    regs.high_threshold_ch2_enable = 1;
    regs.high_threshold_ch2 = config->high_threshold_ch2;
    regs.low_threshold_ch2_enable = 1;
    regs.low_threshold_ch2 = config->low_threshold_ch2;
    regs.calib_ch1_enable = 1;
    regs.calib_ch1 = config->calib_ch1;
    regs.calib_ch2_enable = 1;
    regs.calib_ch2 = config->calib_ch2;
    
    
    
    
    ioctl(fd, RPADC_FIFO_FREQ_AUTO_SET_REGISTERS, &regs);
    usleep(10000);

    regs.decimator_register = config->decimation - 1;
    ioctl(fd, RPADC_FIFO_FREQ_AUTO_SET_REGISTERS, &regs);
    currVal = 1;
    ioctl(fd, RPADC_FIFO_FREQ_AUTO_SET_PACKETIZER, &currVal);

    ioctl(fd, RPADC_FIFO_FREQ_AUTO_CLEAR_DATA_FIFO, NULL);
    ioctl(fd, RPADC_FIFO_FREQ_AUTO_FIFO_INT_HALF_SIZE, NULL);
    //ioctl(fd, RPADC_FIFO_AUTO_FIFO_INT_FIRST_SAMPLE, NULL);


    
}



static void readConfig(int fd, struct rpadc_configuration *config)
{
    unsigned int currVal;
    struct rpadc_fifo_auto_registers regs; 
    ioctl(fd,  RPADC_FIFO_AUTO_GET_REGISTERS, &regs);
<<<<<<< HEAD
=======
    currVal = regs.mode_register;
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
    config->pre_samples = regs.pre_register;
    config->decimation = regs.decimator_register + 1;
    config->event_code = regs.trig_event_code;
}

static void readFreqConfig(int fd, struct rpadc_freq_configuration *config)
{
    unsigned int currVal;
    struct rpadc_fifo_freq_auto_registers regs; 
    ioctl(fd,  RPADC_FIFO_FREQ_AUTO_GET_REGISTERS, &regs);
>>>>>>> 65bdc7acbb0ce9e5905a8f45695ba1556d311f51
    currVal = regs.mode_register;
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
    config->pre_samples = regs.pre_register;
    config->decimation = regs.decimator_register + 1;
<<<<<<< HEAD
    config->event_code = regs.trig_event_code;
=======
    
    config->ncycles = regs.ncycles;
    config->high_threshold_ch1 = regs.high_threshold_ch1;
    config->low_threshold_ch1 = regs.low_threshold_ch1;
    config->high_threshold_ch2 = regs.high_threshold_ch2;
    config->low_threshold_ch2 = regs.low_threshold_ch2;
    config->calib_ch1 = regs.calib_ch1;
    config->calib_ch2 = regs.calib_ch2;
>>>>>>> 65bdc7acbb0ce9e5905a8f45695ba1556d311f51
}

static void fifoFlush(int fd)
{
    ioctl(fd, RPADC_FIFO_AUTO_FIFO_FLUSH, NULL);
}

static void fifoFlushFreq(int fd)
{
    ioctl(fd, RPADC_FIFO_FREQ_AUTO_FIFO_FLUSH, NULL);
}


static void stopRead(int fd)
{
    ioctl(fd, RPADC_FIFO_AUTO_STOP_READ, NULL);
}
static void stopReadFreq(int fd)
{
    ioctl(fd, RPADC_FIFO_FREQ_AUTO_STOP_READ, NULL);
}

static void adcStop(int fd)
{
    unsigned int val = 0x00000002;
    ioctl(fd, RPADC_FIFO_AUTO_SET_COMMAND_REGISTER, &val);
}
static void adcStopFreq(int fd)
{
    unsigned int val = 0x00000002;
    ioctl(fd, RPADC_FIFO_FREQ_AUTO_SET_COMMAND_REGISTER, &val);
}
/*
static void dmaStop(int fd)
{
    std::cout << "DMA STOP" << fd << std::endl;
    ioctl(fd, RPADC_FIFO_AUTO_STOP_DMA, 0);
}
*/

static void adcArm(int fd, bool isFreq)
{
    unsigned int command = 0;
    std::cout << "ARM  " << fd << std::endl;
<<<<<<< HEAD
    
    
    ioctl(fd, RPADC_FIFO_AUTO_GET_TIME_FIFO_LEN, &command);
    std::cout << "TIME FIFO LEN: " << command << std::endl;
    ioctl(fd, RPADC_FIFO_AUTO_GET_TIME_FIFO_VAL, &command);
     
    
    command = 0x00000001;  //Arm
    ioctl(fd, RPADC_FIFO_AUTO_SET_COMMAND_REGISTER, &command);
=======
    unsigned int command = 0x00000001;  //Arm
    if(isFreq)
	ioctl(fd, RPADC_FIFO_FREQ_AUTO_SET_COMMAND_REGISTER, &command);
    else
	ioctl(fd, RPADC_FIFO_AUTO_SET_COMMAND_REGISTER, &command);
>>>>>>> 65bdc7acbb0ce9e5905a8f45695ba1556d311f51
}
static void adcTrigger(int fd)
{
    unsigned int command = 0x00000004;
    ioctl(fd, RPADC_FIFO_AUTO_SET_COMMAND_REGISTER, &command);
    usleep(1000);
    command = 0;
    ioctl(fd, RPADC_FIFO_AUTO_SET_COMMAND_REGISTER, &command);

//    ioctl(fd, RPADC_FIFO_AUTO_GET_DATA_FIFO_LEN, &command);

//    ioctl(fd, RPADC_FIFO_AUTO_GET_DATA_FIFO_VAL, &command);
 //   ioctl(fd, RPADC_FIFO_AUTO_GET_TIME_FIFO_LEN, &command);
 //   ioctl(fd, RPADC_FIFO_AUTO_GET_TIME_FIFO_VAL, &command);
 
}
static void adcTriggerFreq(int fd)
{
    unsigned int command = 0x00000004;
    ioctl(fd, RPADC_FIFO_FREQ_AUTO_SET_COMMAND_REGISTER, &command);
    usleep(1000);
    command = 0;
    ioctl(fd, RPADC_FIFO_FREQ_AUTO_SET_COMMAND_REGISTER, &command);

    ioctl(fd, RPADC_FIFO_FREQ_AUTO_GET_DATA_FIFO_LEN, &command);
    printf("DATA FIFO LEN: %d\n", command);

    ioctl(fd, RPADC_FIFO_FREQ_AUTO_GET_DATA_FIFO_VAL, &command);
    printf("DATA FIFO VAL: %x\n", command);
    ioctl(fd, RPADC_FIFO_FREQ_AUTO_GET_TIME_FIFO_LEN, &command);
    printf("TIME FIFO LEN: %d\n", command);
    ioctl(fd, RPADC_FIFO_FREQ_AUTO_GET_TIME_FIFO_VAL, &command);
    printf("TIME FIFO LEN: %x\n", command);

}

static void adcClearFifo(int fd)
{
    ioctl(fd, RPADC_FIFO_AUTO_CLEAR_TIME_FIFO, 0);
}
static void adcClearFifoFreq(int fd)
{
    ioctl(fd, RPADC_FIFO_FREQ_AUTO_CLEAR_TIME_FIFO, 0);
}
/*
static void dmaStart(int fd, int cyclic)
{
    ioctl(fd, RPADC_FIFO_AUTO_START_DMA, &cyclic);
}
*/
static void sigHandler(int signo)
{
  if (signo == SIGINT)
  {
      if(deviceFd)
      {
	  stopped = true;
	  adcStop(deviceFd);
	  usleep(1000);
	 // dmaStop(deviceFd);
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
  adcStop(fd);
  usleep(100000);
  fifoFlush(fd);
  stopped = true;
  usleep(100000);
  usleep(100000);
  //dmaStop(fd);
 // stopped = true;
  usleep(100000);
  stopRead(fd);
  usleep(100000);
  close(fd);
} 
<<<<<<< HEAD

void rpadcStream(int fd, char *treeName, int shot, int chan1Nid, int chan2Nid, int triggerNid, 
		int preSamples, int postSamples, int inSegmentSamples, double freq, double freq1, int single)
=======
void rpadcFreqStop(int fd)
{
  adcStopFreq(fd);
  usleep(100000);
  fifoFlushFreq(fd);
  usleep(100000);
  //dmaStop(fd);
  stopped = true;
  stopRead(fd);
  usleep(100000);
  close(fd);
} 
static void rpadcIntStream(int fd, char *treeName, int shot, int chan1Nid, int chan2Nid, int triggerNid, 
		int preSamples, int postSamples, int inSegmentSamples, double freq, double freq1, int single, bool isFreq)
>>>>>>> 65bdc7acbb0ce9e5905a8f45695ba1556d311f51
{
    int segmentSamples;  //True segment dimension
    int blockSamples;    //post samples + pre samples for event streaming, segmentSize for continuous streaming
    int blocksInSegment;  //1 fir cintinous streaming
    unsigned int  *dataSamples;
    double *startTimes, *endTimes;
    unsigned long long prevTime = 0;    
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
	if(segmentSamples > 1000000 && segmentSamples % 1000000 == 0)
            segmentSamples = 1000000;    //If multiple of 10000 store in chunks of 100Ksamples
	else if(segmentSamples > 100000 && segmentSamples % 100000 == 0)
            segmentSamples = 100000;    //If multiple of 10000 store in chunks of 100Ksamples
	else if(segmentSamples > 10000 && segmentSamples % 10000 == 0)
            segmentSamples = 10000;    //If multiple of 10000 store in chunks of 10Ksamples
        else if (segmentSamples > 1000 && segmentSamples % 1000 == 0)
	    segmentSamples = 1000; //If multiple of 10000 store in chunks of 1Ksamples
	//segmentSamples = preSamples + postSamples;
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
    if(isFreq)
    {
	ioctl(fd, RPADC_FIFO_FREQ_AUTO_START_READ, NULL);
    }
    else
    {
	ioctl(fd, RPADC_FIFO_AUTO_START_READ, NULL);
    }
    adcArm(fd, isFreq);
    usleep(1000); 
    int segmentIdx = 0;
<<<<<<< HEAD
    prevTime = 0;
=======
>>>>>>> 65bdc7acbb0ce9e5905a8f45695ba1556d311f51
    while(true)
    {
	for(int currBlock = 0; currBlock < blocksInSegment; currBlock++)
	{
	    unsigned int currSample = 0;
	    while(currSample < blockSamples)
	    {
		int rb = read(fd,&dataSamples[currBlock * blockSamples + currSample], (blockSamples - currSample)*sizeof(int));

		currSample += rb/sizeof(int);

		if(stopped) //May happen when block readout has terminated or in the middle of readout
		{
std::cout << "STOPPED!!!!!!!" << std::endl;
		    if (!(currBlock == 0 && currSample == 0))//if any data has been acquired
		    {
			if(rb > 0)
			{
	//Make sure no data are left
			  while(rb > 0)
			  {
			      rb = read(fd,&dataSamples[currBlock * blockSamples + currSample], (blockSamples - currSample)*sizeof(int));
			      std::cout << "RILETTURA: " << rb << std::endl;
			      if(rb > 0)
				  currSample += rb/sizeof(int);	
			  }
			    
			    if(single)
			    {
				startTimes[0] = (segmentIdx * segmentSamples - preSamples)/freq1;
				endTimes[0] = ((segmentIdx + 1) * segmentSamples - preSamples)/freq1;
				writeSegment(tree, chan1, chan2, trigger, dataSamples, startTimes, endTimes, 
					      currBlock * blockSamples + currSample, 1, freq1);
			    }
			    else //Some data for new window have been read
			    {
				unsigned long long currTime;
				unsigned int time1, time2;
				ioctl(fd, RPADC_FIFO_AUTO_GET_TIME_FIFO_VAL, &time1);
				ioctl(fd, RPADC_FIFO_AUTO_GET_TIME_FIFO_VAL, &time2);
				currTime = (unsigned long long)time1 | (((unsigned long long) time2) << 32);
				startTimes[currBlock] = (currTime - preSamples)/freq;
				endTimes[currBlock] =  (currTime + postSamples - 1)/freq;  //include last sample
//				std::cout << "ULTIMO TIME: " << currTime << std::endl;
				writeSegment(tree, chan1, chan2, trigger, dataSamples, startTimes, endTimes, 
					 currBlock * blockSamples + currSample, currBlock + 1, freq1);
			    }
			}
			else //Some windows have been read before and the segment is partially filled
			{
			    writeSegment(tree, chan1, chan2, trigger, dataSamples, startTimes, endTimes, 
					 currBlock * blockSamples + currSample, currBlock, freq1);
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
		if(rb < 0 && !stopped)
		{
		    printf("DEVICE FIFO OVERFLOW!!!!\n");
		    return;
		}
	    }
	    //Here the block been filled. It may refer to the same window (isSingle)or to a different time window
	    if(preSamples != 0 || postSamples != 0) //not continuous
	    {
		unsigned long long currTime;          
		unsigned int time1, time2;
                if(single)
                {
                    startTimes[0] = (segmentIdx * segmentSamples - preSamples)/freq1;
                    endTimes[0] = ((segmentIdx + 1) * segmentSamples - preSamples)/freq1;
                }
                else //If referring to a new window, the time must be read
                {
		    ioctl(fd, RPADC_FIFO_AUTO_GET_TIME_FIFO_VAL, &time1);
		    ioctl(fd, RPADC_FIFO_AUTO_GET_TIME_FIFO_VAL, &time2);
		    currTime = (unsigned long long)time1 | (((unsigned long long) time2) << 32);
//std::cout << "TIME1: " << time1 << "  TIME2:  " << time2 << "   PREV TIME: " << prevTime << std::endl;
//std::cout << "TIME COUNTER: " << currTime <<  "DELTA:  " << currTime - prevTime << "   " << (currTime - prevTime)/freq << std::endl;
		    prevTime = currTime;
		    if(currBlock == 0)
		      startTimes[currBlock] = ((long long)currTime - preSamples - 1)/freq;
		    else
		      startTimes[currBlock] = ((long long)currTime - preSamples)/freq;
		    endTimes[currBlock] =  (currTime + postSamples - 1 + 0.1)/freq;  //include last sample
	        }
	    }
	}
	if(preSamples == 0 && postSamples == 0)
	{
	    startTimes[0] = segmentIdx * segmentSamples/freq1;
	    endTimes[0] = (segmentIdx + 1) * segmentSamples/freq1;
	}
	segmentIdx++;
	
	writeSegment(tree, chan1, chan2, trigger, dataSamples, startTimes, endTimes, segmentSamples, blocksInSegment,freq1);
    }
}
<<<<<<< HEAD
=======
void rpadcStream(int fd, char *treeName, int shot, int chan1Nid, int chan2Nid, int triggerNid, 
		int preSamples, int postSamples, int inSegmentSamples, double freq, double freq1, int single)
{
    rpadcIntStream(fd, treeName, shot, chan1Nid, chan2Nid, triggerNid,  preSamples, postSamples, inSegmentSamples, freq, freq1, single, false);
}
void rpadcFreqStream(int fd, char *treeName, int shot, int chan1Nid, int chan2Nid, int triggerNid, 
		int preSamples, int postSamples, int inSegmentSamples, double freq, double freq1, int single)
{
    rpadcIntStream(fd, treeName, shot, chan1Nid, chan2Nid, triggerNid,  preSamples, postSamples, inSegmentSamples, freq, freq1, single, true);
}
>>>>>>> 65bdc7acbb0ce9e5905a8f45695ba1556d311f51
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
    printf("\tevent_code: %d\n", config->event_code);
<<<<<<< HEAD
=======
}
static void printFreqConfig(struct rpadc_freq_configuration *config)
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
    printf("\tncycles: %d\n", config->ncycles);
    printf("\thigh_threshold_ch1: %d\n", config->high_threshold_ch1);
    printf("\tlow_threshold_ch1: %d\n", config->low_threshold_ch1);
    printf("\thigh_threshold_ch2: %d\n", config->high_threshold_ch2);
    printf("\tlow_threshold_ch2: %d\n", config->low_threshold_ch2);
    printf("\tcalib_ch1: %d\n", config->calib_ch1);
    printf("\tcalib_ch2: %d\n", config->calib_ch2);
>>>>>>> 65bdc7acbb0ce9e5905a8f45695ba1556d311f51
}


//return either NULL or an error string
int rpadcInit(int mode, int clock_mode, int preSamples, int postSamples, int trigFromChanA, int trigAboveThreshold, int trigThreshold, 
	       int thresholdSamples, int decimation, int event_code)
{
    struct rpadc_configuration inConfig, outConfig;
    int fd = open("/dev/rpadc_fifo_auto", O_RDWR | O_SYNC);
 
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
    inConfig.post_samples = postSamples; //Watch!!!!!
    inConfig.decimation = decimation;
    inConfig.event_code = event_code;
    printConfig(&inConfig);
    writeConfig(fd, &inConfig);
    memset(&outConfig, 0, sizeof(outConfig));
    
    readConfig(fd, &outConfig); 
    printConfig(&outConfig);
//    adcStop(fd);
    //status = ioctl(fd, RFX_RPADC_STOP, 0);
//    sleep(1);
//    dmaStop(fd);
//    sleep(1);
    adcClearFifo(fd);
    usleep(1000);
//    adcArm(fd);
//    usleep(1000);
/*
    if(mode == 2 || mode == 4) //if single
	dmaStart(fd, 1);
//	dmaStart(fd, 0);
    else
	dmaStart(fd, 1);
    usleep(1000);
*/    
    return fd;
}
//return either NULL or an error string
int rpadcFreqInit(int mode, int clock_mode, int preSamples, int postSamples, int trigFromChanA, int trigAboveThreshold, int trigThreshold, 
	       int thresholdSamples, int decimation, int nCycles, int highThresholdCh1, int highThresholdCh2, int lowThresholdCh1, int lowThresholdCh2,
	       int calibCh1, int calibCh2)
{
    struct rpadc_freq_configuration inConfig, outConfig;
printf("APRO FD\n");
    int fd = open("/dev/rpadc_fifo_freq_auto", O_RDWR | O_SYNC);
printf("APERTO FD = %d\n", fd);
 
     if(fd < 0) 
        return -1;
 
     deviceFd = fd;
     
    stopped = true;
    inConfig.mode = (enum rpadc_mode)mode;
    inConfig.clock_mode = (enum rpadc_clock_mode)clock_mode;
    inConfig.trig_samples = thresholdSamples;
    inConfig.trig_above_threshold = trigAboveThreshold;
    inConfig.trig_from_chana = trigFromChanA;
    inConfig.trig_threshold = trigThreshold;
    inConfig.pre_samples = preSamples;
    inConfig.post_samples = postSamples;
    inConfig.decimation = decimation;
    inConfig.ncycles = nCycles;
    inConfig.high_threshold_ch1 = highThresholdCh1;
    inConfig.high_threshold_ch2 = highThresholdCh2;
    inConfig.low_threshold_ch1 = lowThresholdCh1;
    inConfig.low_threshold_ch2 = lowThresholdCh2;
    inConfig.calib_ch1 = calibCh1;
    inConfig.calib_ch2 = calibCh2;
    
    
    printFreqConfig(&inConfig);
    writeFreqConfig(fd, &inConfig);
    memset(&outConfig, 0, sizeof(outConfig));
    
    readFreqConfig(fd, &outConfig); 
    printFreqConfig(&outConfig);
    adcClearFifoFreq(fd);
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

<<<<<<< HEAD
=======
int rpadcFreqTrigger(int fd)
{
    adcTriggerFreq(fd);
//    int status = ioctl(fd, RFX_RPADC_TRIGGER, 0);
    usleep(10);
    return 0;
}
>>>>>>> 65bdc7acbb0ce9e5905a8f45695ba1556d311f51

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
<<<<<<< HEAD
    int fd = rpadcInit(0, 0, preSamples, postSamples, 1, 1, 5000, 2, atoi(argv[1]), 0);
=======
std::cout << "INIZIALIZZO...\n";
    int fd = rpadcInit(0, 0, preSamples, postSamples, 1, 1, 5000, 2, atoi(argv[1]), 0);
std::cout << "INIZIALIZZATO\n";
>>>>>>> 65bdc7acbb0ce9e5905a8f45695ba1556d311f51
    try {
      
      MDSplus::Tree *t = new MDSplus::Tree("redpitaya", -1);      
      t->createPulse(1);
      t = new MDSplus::Tree("redpitaya", 1);
      MDSplus::TreeNode *chan1 = t->getNode("rpadc:chan_a");
      MDSplus::TreeNode *chan2 = t->getNode("rpadc:chan_b");
      MDSplus::TreeNode *trigger = t->getNode("rpadc:trigger");
      rpadcTrigger(fd);
      rpadcStream(fd, (char *)"redpitaya", 1, chan1->getNid(), chan2->getNid(), trigger->getNid(), preSamples, postSamples, 1000, 125E6/atoi(argv[1]), 125E6/atoi(argv[1]), 0);
 
      rpadcStop(fd);
    }catch(MDSplus::MdsException &exc) 
    {
	std::cout << exc.what() << std::endl;
	return 0;
    }
    return 0;
}
  
    
