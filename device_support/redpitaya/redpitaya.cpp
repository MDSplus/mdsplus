#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <mdsobjects.h>
#include <rfx_stream.h>
#include <signal.h>
#include <AsyncStoreManager.h>

#define COUNT_SIZE 2000000

static void checkUpdateFreq(int fd);
extern "C"
{
  void rpadcStream(int fd, char *treeName, int shot, int chan1Nid, int chan2Nid,
                   int triggerNid, int preSamples, int postSamples,
                   int inSegmentSamples, double freq, double freq1, int single, int absTriggerTimeFromFPGA, int absTriggerNid);
  int rpadcInit(int mode, int clock_mode, int preSamples, int postSamples,
                int trigFromChanA, int trigAboveThreshold, int trigThreshold,
                int thresholdSamples, int decimation, int deadtime);
  int rpadcTrigger(int fd);
  void rpadcStop(int fd);
  void openTree(char *name, int shot, MDSplus::Tree **treePtr);
  void setTriggerTime(unsigned long long triggerTime);
}


enum rpadc_mode
{
  STREAMING = 0,
  EVENT_STREAMING,
  EVENT_SINGLE,
  TRIGGER_STREAMING,
  TRIGGER_SINGLE
};

enum rpadc_clock_mode
{
  INTERNAL = 0,  //Internal clock and trigger timestamping Sampling direct from ADC (125MHz divided for decimation)
  TRIG_EXTERNAL, //Internal clock, but trigger timestamping from external clock
  TRIG_ABS,      //Internal clock, use absolute time for trigger timestamping (however use relative times in the saved signal, a separate node will contain the absolute time of the first trigger)
  EXTERNAL,      //External sampling clock, same clock used for trigger timestamping   
  SYNC   	 //Synchronized (NTP) sampling clock, use absolute time for trigger timestamping (however use relative times in the saved signal, a separate node will contain the absolute time of the first trigger)
};

#define DMA_STREAMING_SAMPLES 1024

struct rpadc_configuration
{
  enum rpadc_mode mode;
  enum rpadc_clock_mode clock_mode;
  unsigned short trig_threshold; // Signal level for trigger
  char trig_above_threshold;     // If true, trigger when above threshold, below
                                 // threshold otherwise
  char trig_from_chana;          // If true, the trigger is derived from channel A
  unsigned char trig_samples;    // Number of samples above/below threshol for
                                 // validating trigger
  unsigned short pre_samples;    // Number of pre-trigger samples
  unsigned int post_samples;     // Number of post-trigger samples
  unsigned int decimation;       // Decimation factor (base frequency: 125MHz if internal clock, 1 MHz if synch clock, Clock frequency if external clock)
  unsigned int deadtime;         // Deadtime count, valis when trigger on level, referred to sample count
};

static bool stopped = false;
static bool isStream = false;

static int deviceFd = 0;
static unsigned long long absTriggerTime = 0;
static bool isFirstTrigger;
void setTriggerTime(unsigned long long triggerTime)
{
  absTriggerTime = triggerTime;
}

static void writeConfig(int fd, struct rpadc_configuration *config)
{
  struct rfx_stream_registers regs;
  memset(&regs, 0, sizeof(regs));
  unsigned int currVal = 0;
  unsigned int auxVal = 0;

  if (config->mode == STREAMING)
    currVal |= 0x00000001;
  if (config->trig_from_chana)
    currVal |= 0x00000002;
  if (config->trig_above_threshold)
    currVal |= 0x00000004;
  if (config->mode == EVENT_STREAMING || config->mode == EVENT_SINGLE)
    currVal |= 0x00000008;
  if (config->mode == EVENT_STREAMING || config->mode == TRIGGER_STREAMING)
    currVal |= 0x00000010;
  switch(config->clock_mode) {
    case  INTERNAL: //Internal clock and trigger timestamping Sampling direct from ADC (125MHz divided for decimation)
      break;
    case TRIG_EXTERNAL: //Internal clock, but trigger timestamping from external clock
      currVal |= 0x00000020;
      auxVal |= 4;
      break;
    case TRIG_ABS:      //Internal clock, use absolute time for trigger timestamping (however use relative times in the saved signal, a separate node will contain the absolute time of the first trigger)  
      currVal |= 0x00000020;
      auxVal |= 0x00000002;
      break;
    case EXTERNAL:      //External sampling clock, same clock used for trigger timestamping
      currVal |= 0x00000020;   
      currVal |= 0x00000040;   
      auxVal |= 0x00000004;
      break;
   case SYNC: //Synchronized (NTP) sampling clock, use absolute time for trigger timestamping (however use relative times in the saved signal, a separate node will contain the absolute time of the first trigger)
      currVal |= 0x00000020;   
      currVal |= 0x00000040;   
      auxVal |= 0x00000001;
      break;
  } 
   
/*   Aux Mode register bits:
--     0: use ext_clock(synchronized) for resampling clock (0) or use sync_clk for resampling clock
--     1: Use relative times (derived from internal counters) (0) or use absolute times(1) 
--     2: Resampler out_clk = sync_clock (0) or sync_ext_clock(1) 
*/
   
  currVal |= ((config->trig_samples << 8) & 0x0000FF00);
  currVal |= ((config->trig_threshold << 16) & 0xFFFF0000);
  regs.mode_register_enable = 1;
  regs.mode_register = currVal;
  regs.aux_mode_reg_enable = 1;
  regs.aux_mode_reg = auxVal;

  regs.pre_register_enable = 1;
  regs.pre_register = config->pre_samples & 0x0000FFFF;

  regs.post_register_enable = 1;
  regs.post_register = config->post_samples;

  regs.decimator_register_enable = 1;
  regs.decimator_register = 10;

  regs.lev_trig_count = 1; // set first count higher than init in VHDL

  regs.deadtime_register_enable = 1;
  regs.deadtime_register = config->deadtime;
  ioctl(fd, RFX_STREAM_SET_REGISTERS, &regs);
  usleep(10000);

  regs.decimator_register = config->decimation - 1;
  printf("MODE REGISTER:%x\n", regs.mode_register);
  ioctl(fd, RFX_STREAM_SET_REGISTERS, &regs);
  printf("REGISTRI SCRITTI:%x\n", regs.mode_register);
  currVal = 1;
  ioctl(fd, RFX_STREAM_SET_PACKETIZER, &currVal);

  ioctl(fd, RFX_STREAM_CLEAR_DATA_FIFO, NULL);
  // ioctl(fd, RFX_STREAM_FIFO_INT_HALF_SIZE, NULL);
  ioctl(fd, RFX_STREAM_FIFO_INT_FIRST_SAMPLE, NULL);
}

static void readConfig(int fd, struct rpadc_configuration *config)
{
  unsigned int currVal, auxVal;
  struct rfx_stream_registers regs;
  ioctl(fd, RFX_STREAM_GET_REGISTERS, &regs);
  currVal = regs.mode_register;
  auxVal = regs.aux_mode_reg;
  if (currVal & 0x00000001)
    config->mode = STREAMING;
  else
  {
    if (currVal & 0x00000008)
    {
      if (currVal & 0x00000010)
        config->mode = EVENT_STREAMING;
      else
        config->mode = EVENT_SINGLE;
    }
    else
    {
      if (currVal & 0x00000010)
        config->mode = TRIGGER_STREAMING;
      else
        config->mode = TRIGGER_SINGLE;
    }
  }
  if (!(currVal & 0x00000020))
  {
    config->clock_mode = INTERNAL;
  }
  else
  {
    if (currVal & 0x00000040)
    {
    	if (auxVal & 0x0000004)
      	  config->clock_mode = EXTERNAL;
        else
          config->clock_mode = SYNC;
    }
    else
    {
    	if (auxVal & 0x0000004)
      	  config->clock_mode = TRIG_EXTERNAL;
        else 
          config->clock_mode = TRIG_ABS;
    }
  }
  
  if (currVal & 0x00000002)
    config->trig_from_chana = 1;
  else
    config->trig_from_chana = 0;

  if (currVal & 0x00000004)
    config->trig_above_threshold = 1;
  else
    config->trig_above_threshold = 0;

  config->trig_samples = ((currVal >> 8) & 0x000000FF);
  config->trig_threshold = ((currVal >> 16) & 0x0000FFFF);

  config->post_samples = regs.post_register;
  config->pre_samples = regs.pre_register;
  config->decimation = regs.decimator_register + 1;
  config->deadtime = regs.deadtime_register;
  
  printf("Mode Register: %x\t Aux Mode Register: %x\n", regs.mode_register, regs.aux_mode_reg);
}

static void fifoFlush(int fd) { ioctl(fd, RFX_STREAM_FIFO_FLUSH, NULL); }

static void stopRead(int fd) { ioctl(fd, RFX_STREAM_STOP_READ, NULL); }

static void adcStop(int fd)
{
  unsigned int val = 0x00000002;
  ioctl(fd, RFX_STREAM_SET_COMMAND_REGISTER, &val);
}
/*
static void dmaStop(int fd)
{
    std::cout << "DMA STOP" << fd << std::endl;
    ioctl(fd, RFX_STREAM_STOP_DMA, 0);
}
*/

static void adcArm(int fd)
{
  unsigned int command = 0;
  std::cout << "ARM  " << fd << std::endl;

  ioctl(fd, RFX_STREAM_GET_TIME_FIFO_LEN, &command);
  ioctl(fd, RFX_STREAM_GET_TIME_FIFO_VAL, &command);

  command = 0x00000001; // Arm
  ioctl(fd, RFX_STREAM_SET_COMMAND_REGISTER, &command);
}
static void adcTrigger(int fd)
{
  unsigned int command = 0x00000004;
  ioctl(fd, RFX_STREAM_SET_COMMAND_REGISTER, &command);
  usleep(1000);
  command = 0;
  ioctl(fd, RFX_STREAM_SET_COMMAND_REGISTER, &command);

  ioctl(fd, RFX_STREAM_GET_DRIVER_BUFLEN, &command);
  //    ioctl(fd, RFX_STREAM_GET_DATA_FIFO_VAL, &command);
  //   ioctl(fd, RFX_STREAM_GET_TIME_FIFO_LEN, &command);
  //   ioctl(fd, RFX_STREAM_GET_TIME_FIFO_VAL, &command);
}

static void adcClearFifo(int fd) { ioctl(fd, RFX_STREAM_CLEAR_TIME_FIFO, 0); }

static void sigHandler(int signo)
{
  if (signo == SIGINT)
  {
    if (deviceFd)
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

static void writeSegment(MDSplus::Tree *t, MDSplus::TreeNode *chan1,
                         MDSplus::TreeNode *chan2, MDSplus::Data *triggerTime,
                         unsigned int *dataSamples, double *startTimes,
                         double *endTimes, int segmentSamples,
                         int blocksInSegment, double freq, SaveList *saveList)
{
  if (segmentSamples == 0)
    return;
  short *chan1Samples, *chan2Samples;
  //std::cout << "WRITE SEGMENT SAMPLES: " << segmentSamples << std::endl;
  chan1Samples = new short[segmentSamples];
  chan2Samples = new short[segmentSamples];

  for (int i = 0; i < segmentSamples; i++)
  {
    chan1Samples[i] = dataSamples[i] & 0x0000ffff;
    chan2Samples[i] = (dataSamples[i] >> 16) & 0x0000ffff;
  }

  saveList->addItem(chan1Samples, segmentSamples, chan1, triggerTime, t,
                    startTimes, endTimes, freq, blocksInSegment);

  saveList->addItem(chan2Samples, segmentSamples, chan2, triggerTime, t,
                    startTimes, endTimes, freq, blocksInSegment);
}
// Stop
void rpadcStop(int fd)
{
  adcStop(fd);
  usleep(100000);
  fifoFlush(fd);
  stopped = true;
  std::cout << "TIRATO SU STOP\n";
  usleep(100000);
  usleep(100000);
  // dmaStop(fd);
  // stopped = true;
  usleep(100000);
  stopRead(fd);
  usleep(100000);
   close(fd);
  std::cout << "CLOSED\n";
}

void rpadcStream(int fd, char *treeName, int shot, int chan1Nid, int chan2Nid,
                 int triggerNid, int preSamples, int postSamples,
                 int inSegmentSamples, double freq, double freq1, int single, int absTriggerTimeFromFPGA, int absTriggerNid)
{

//freq: frequency of the sampling clock
//freq1: frequency used to timestamp triggers

  int segmentSamples;  // True segment dimension
  unsigned int blockSamples;    // post samples + pre samples for event streaming,
                       // segmentSize for continuous streaming
  int blocksInSegment; // 1 fir cintinous streaming
  unsigned int *dataSamples;
  double *startTimes, *endTimes;
  stopped = false;
  unsigned int trig_lev_count = 0;
  unsigned long long firstAbsTriggerTime = 0;
  unsigned long long lastAbsTriggerTime = 0;
  unsigned long long currTime, savedTime;
  
  std::cout << "rpadcStream freq1: " << freq1 << "   FREQ:  " << freq << std::endl;
  
  
  
  
  absTriggerTime = 0; //It will be set by setTriggerTime()
  isFirstTrigger = true;
  
  MDSplus::Tree *tree = new MDSplus::Tree(treeName, shot);
  MDSplus::TreeNode *chan1 = new MDSplus::TreeNode(chan1Nid, tree);
  MDSplus::TreeNode *chan2 = new MDSplus::TreeNode(chan2Nid, tree);
  MDSplus::TreeNode *absTrigger = new MDSplus::TreeNode(absTriggerNid, tree);

  MDSplus::TreeNode *trigger = new MDSplus::TreeNode(triggerNid, tree);
  if ((preSamples == 0 &&
       postSamples == 0)) // eventSamples == 0 means continuous streaming
  {
    blockSamples = segmentSamples = inSegmentSamples;
    blocksInSegment = 1;
    isStream = true;
  }
  else if (single)
  {
    segmentSamples = preSamples + postSamples;
    if (segmentSamples > 1000000 && segmentSamples % 1000000 == 0)
      segmentSamples =
          1000000; // If multiple of 10000 store in chunks of 100Ksamples
    else if (segmentSamples > 100000 && segmentSamples % 100000 == 0)
      segmentSamples =
          100000; // If multiple of 10000 store in chunks of 100Ksamples
    else if (segmentSamples > 10000 && segmentSamples % 10000 == 0)
      segmentSamples =
          10000; // If multiple of 10000 store in chunks of 10Ksamples
    else if (segmentSamples > 1000 && segmentSamples % 1000 == 0)
      segmentSamples = 1000; // If multiple of 10000 store in chunks of
                             // 1Ksamples
    // segmentSamples = preSamples + postSamples;
    blockSamples = segmentSamples;
    blocksInSegment = 1;
  }
  else
  {
    int eventSamples = preSamples + postSamples;
    if (eventSamples > inSegmentSamples)
      inSegmentSamples = eventSamples; // Force consistenct
    segmentSamples = (inSegmentSamples / eventSamples) * eventSamples;
    blockSamples = eventSamples;
    blocksInSegment = segmentSamples / blockSamples;
  }

  std::cout << "blocksInSegment: " << blocksInSegment
            << "   segmentSamples: " << segmentSamples
            << "   BlockSamples: " << blockSamples << std::endl;

  dataSamples = new unsigned int[segmentSamples];
  memset(dataSamples, 0, segmentSamples * sizeof(int));
  startTimes = new double[blocksInSegment];
  endTimes = new double[blocksInSegment];
  ioctl(fd, RFX_STREAM_START_READ, NULL);
  adcArm(fd);
  usleep(1000);
  int segmentIdx = 0;
  SaveList *saveList = new SaveList;
  saveList->start();
  // START WITH A INITIAL VALUE FOR TRIG_LEV_COUNT
  ioctl(fd, RFX_STREAM_GET_LEV_TRIG_COUNT, &trig_lev_count);
  trig_lev_count++;
  ioctl(fd, RFX_STREAM_SET_LEV_TRIG_COUNT, &trig_lev_count);

  struct timeval selWaitTime;
  while (true)
  {
    for (int currBlock = 0; currBlock < blocksInSegment; currBlock++)
    {
      unsigned int currSample = 0;
      bool firstRead = true;
      while (currSample < blockSamples)
      {
//std::cout<<"Reading...\n";
        int rb = read(fd, &dataSamples[currBlock * blockSamples + currSample],
                      (blockSamples - currSample) * sizeof(int));
//if(rb > 0) std::cout << "READ " << rb << std::endl;
        if(rb < 0)
          std::cout << "ULLALA!\n";
        else
        {
          currSample += rb / sizeof(int);
          if(firstRead)
          {
            firstRead = false;
            // signal to FPGA that block has been read -- In this way make sure that no more than 2 burst can be concurrenlty in read
            ioctl(fd, RFX_STREAM_GET_LEV_TRIG_COUNT, &trig_lev_count);
            trig_lev_count++;
            ioctl(fd, RFX_STREAM_SET_LEV_TRIG_COUNT, &trig_lev_count);
          }
        }
        if (stopped) // May happen when block readout has terminated or in the
                     // middle of readout
        {
          std::cout << "STOPPED!!!!!!!" << std::endl;
          if (!(currBlock == 0 && currSample == 0) &&
              !isStream) // if any data has been acquired
          {
            if (rb > 0)
            {
              // Make sure no data are left
              while (rb > 0)
              {
                rb = read(fd,
                          &dataSamples[currBlock * blockSamples + currSample],
                          (blockSamples - currSample) * sizeof(int));
                std::cout << "RILETTURA: " << rb << std::endl;
                if (rb > 0)
                  currSample += rb / sizeof(int);
              }

              if (single)
              {
                startTimes[0] = 
                    (segmentIdx * segmentSamples - preSamples) / freq; 
//                    (segmentIdx * segmentSamples - preSamples) / freq1; Gabriele Dec 2021
                endTimes[0] =
                    ((segmentIdx + 1) * segmentSamples - preSamples) / freq;
//                    ((segmentIdx + 1) * segmentSamples - preSamples) / freq1; Gabriele Dec 2021
                writeSegment(tree, chan1, chan2, trigger, dataSamples,
                             startTimes, endTimes,
                             currBlock * blockSamples + currSample, 1, freq, saveList);
                        //    currBlock * blockSamples + currSample, 1, freq1, saveList); Gabriele Dec 2021
              }
              else // Some data for new window have been read
              {
                unsigned int time1, time2;
                ioctl(fd, RFX_STREAM_GET_TIME_FIFO_VAL, &time1);
                ioctl(fd, RFX_STREAM_GET_TIME_FIFO_VAL, &time2);
                currTime = (unsigned long long)time1 |
                           (((unsigned long long)time2) << 32);
                           
                           
                if(absTriggerTimeFromFPGA)
                {
         std::cout << "TRIGGER TIME: " << currTime << std::endl;
                    if(firstAbsTriggerTime == 0)
                    {
                    	firstAbsTriggerTime = lastAbsTriggerTime = currTime;
                    	currTime = 0;
                    	MDSplus::Data *triggerData = new MDSplus::Uint64(firstAbsTriggerTime);
                     	absTrigger->putData(triggerData);
                    	MDSplus::deleteData(triggerData);
                    }
                    else
                    {
                    	savedTime = currTime;
                    	currTime -= lastAbsTriggerTime;
                    	lastAbsTriggerTime = savedTime;
                    }
                }
                startTimes[currBlock] = currTime/freq1 - preSamples/freq;
//                startTimes[currBlock] = (currTime - preSamples) / freq; Gabriele Dec 2021
                endTimes[currBlock] =
                    currTime/freq1 + (postSamples - 1) / freq; // include last sample
//                    (currTime + postSamples - 1) / freq; // include last sample Gabriele Dec 2021
                writeSegment(tree, chan1, chan2, trigger, dataSamples,
                             startTimes, endTimes,
                             currBlock * blockSamples + currSample,
                             currBlock + 1, freq, saveList);
              }
            }
            else // Some windows have been read before and the segment is
                 // partially filled
            {
              writeSegment(tree, chan1, chan2, trigger, dataSamples, startTimes,
                           endTimes, currBlock * blockSamples + currSample,
                           currBlock, freq, saveList);
            }
          }
          deviceFd = 0;
          saveList->stop();
          delete chan1;
          delete chan2;
          delete trigger;
          return;
        }
        if (rb < 0 && !stopped)
        {
          printf("DEVICE FIFO OVERFLOW!!!!\n");
          return;
        }
      }
      // signal to FPGA that block has been read
      ioctl(fd, RFX_STREAM_GET_LEV_TRIG_COUNT, &trig_lev_count);
      trig_lev_count++;
      ioctl(fd, RFX_STREAM_SET_LEV_TRIG_COUNT, &trig_lev_count);
      // Here the block been filled. It may refer to the same window
      // (isSingle)or to a different time window
      
      unsigned long long currTime;
      unsigned int time1, time2;
      ioctl(fd, RFX_STREAM_GET_TIME_FIFO_VAL, &time1);
      ioctl(fd, RFX_STREAM_GET_TIME_FIFO_VAL, &time2);
      currTime =(unsigned long long)time1 | (((unsigned long long)time2) << 32);
      std::cout << "TRIGGER TIME: " << currTime << std::endl;
      if(absTriggerTimeFromFPGA)
      {
         std::cout << "TRIGGER TIME: " << currTime << std::endl;
      	if(firstAbsTriggerTime == 0)
        {
          firstAbsTriggerTime = lastAbsTriggerTime = currTime;
          currTime = 0;
          MDSplus::Data *triggerData = new MDSplus::Uint64(firstAbsTriggerTime);
          absTrigger->putData(triggerData);
          MDSplus::deleteData(triggerData);
        }
        else
        {
            currTime -= firstAbsTriggerTime;
        }
      }

      if (preSamples != 0 || postSamples != 0) // not continuous
      {
        if (single)
        {
          startTimes[0] = (segmentIdx * segmentSamples - preSamples) / freq;
//          startTimes[0] = (segmentIdx * segmentSamples - preSamples) / freq1; Gabriele Dec 2021
          endTimes[0] =((segmentIdx + 1) * segmentSamples - preSamples) / freq;
//              ((segmentIdx + 1) * segmentSamples - preSamples) / freq1; Gabriele Dec 2021
        }
        else // If referring to a new window, the time must be read
        {
          
          if (currBlock == 0)
          {
            startTimes[currBlock] = ((long long)currTime)/freq1 - (preSamples - 1) / freq;
//                ((long long)currTime - preSamples - 1) / freq; Gabriele Dec 2021 
          }
          else
          {
            startTimes[currBlock] = (long long)currTime/freq1 - preSamples / freq;
//            startTimes[currBlock] = ((long long)currTime - preSamples) / freq; Gabriele Dec 2021
          }
          endTimes[currBlock] = currTime/freq1 + (postSamples - 1 + 0.1) / freq;
 //             (currTime + postSamples - 1 + 0.1) / freq; Gabriele Dec 2021
        }
      }
    }
    if (preSamples == 0 && postSamples == 0)
    {
//      startTimes[0] = segmentIdx * segmentSamples / freq1; Gabriele Dec 2021
//      endTimes[0] = (segmentIdx + 1) * segmentSamples / freq1; Gabriele Dec 2021
      startTimes[0] = segmentIdx * segmentSamples / freq;
      endTimes[0] = (segmentIdx + 1) * segmentSamples / freq;
    }
    segmentIdx++;
    writeSegment(tree, chan1, chan2, trigger, dataSamples, startTimes, endTimes,
                 segmentSamples, blocksInSegment, freq, saveList);
  }
  std::cout << "ULTIMAO RPADCSTREAM\n";
}
static void printConfig(struct rpadc_configuration *config)
{
  printf("CONFIG:\n");
  switch (config->mode)
  {
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
  switch (config->clock_mode)
  {
  case INTERNAL:
    printf("\tclock_mode: INTERNAL\n");
    break;
  case TRIG_EXTERNAL:
    printf("\tclock_mode: TRIG_EXTERNAL\n");
    break;
  case TRIG_ABS:
    printf("\tclock_mode: TRIG_ABS\n");
    break;
  case EXTERNAL:
    printf("\tclock_mode: EXTERNAL\n");
    break;
  case SYNC:
    printf("\tclock_mode: SYNC\n");
    break;
  }

  if (config->trig_above_threshold)
    printf("\ttrig_above_threshold: true\n");
  else
    printf("\ttrig_above_threshold: false\n");

  if (config->trig_from_chana)
    printf("\ttrig_from_chana: true\n");
  else
    printf("\ttrig_from_chana: false\n");

  printf("\ttrig_samples: %d\n", config->trig_samples);
  printf("\ttrig_threshold: %d\n", config->trig_threshold);
  printf("\tpre_samples: %d\n", config->pre_samples);
  printf("\tpost_samples: %d\n", config->post_samples);
  printf("\tdecimation: %d\n", config->decimation);
  printf("\tdeadtime: %d\n", config->deadtime);
}

// return either NULL or an error string
int rpadcInit(int mode, int clock_mode, int preSamples, int postSamples,
              int trigFromChanA, int trigAboveThreshold, int trigThreshold,
              int thresholdSamples, int decimation, int deadtime)
{
  struct rpadc_configuration inConfig, outConfig;
  int fd = open("/dev/rfx_stream", O_RDWR | O_SYNC);

  if (fd < 0)
    return -1;

  deviceFd = fd;

  stopped = true;
  // status = ioctl(fd, RFX_RPADC_FIFO_INT_HALF_SIZE, 0);
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
  inConfig.post_samples = postSamples; // Watch!!!!!
  inConfig.decimation = decimation;
  inConfig.deadtime = deadtime;
  printConfig(&inConfig);
  writeConfig(fd, &inConfig);
  memset(&outConfig, 0, sizeof(outConfig));

  readConfig(fd, &outConfig);
  printConfig(&outConfig);
  adcClearFifo(fd);
  usleep(1000);
  return fd;
}

int rpadcTrigger(int fd)
{
  struct timeval currTime;
  if(isFirstTrigger)
  {
    isFirstTrigger = false;
    if(absTriggerTime == 0)
    {
      gettimeofday(&currTime, NULL);
      absTriggerTime = (unsigned long long)currTime.tv_sec* 1000000L + currTime.tv_usec;
    }
  }
  adcTrigger(fd);
  usleep(10);
  return 0;
}

void openTree(char *name, int shot, MDSplus::Tree **treePtr)
{
  try
  {
    *treePtr = new MDSplus::Tree(name, shot);
  }
  catch (const MDSplus::MdsException &exc)
  {
    *treePtr = 0;
  }
}

////////////////////Clock related stuff





 static double getFrequency(int fd)
{
    double K, step, periodFPGA, period;
    unsigned int Kr, C, stepLo, stepHi, reg, K1, K2;
    unsigned long long stepR;
    ioctl(fd, RFX_STREAM_GET_K1_REG, &K1);

    ioctl(fd, RFX_STREAM_GET_K2_REG, &K2);

    ioctl(fd, RFX_STREAM_GET_STEP_LO_REG, &stepLo);
  
    ioctl(fd, RFX_STREAM_GET_STEP_HI_REG, &stepHi);
    
    stepR = stepHi;
    stepR = (stepR << 32)|stepLo;
    step = (double)stepR/pow(2, 44);

    periodFPGA = 1./125E6;
    period = (periodFPGA * K1)*step + (periodFPGA * K2)*(1.-step);
    return 0.5/period;
}

 
static void setFrequency(int fd, double reqFreq)
{
    double K, step;
    unsigned int Kr, C, stepLo, stepHi, reg, K1, K2;
    unsigned long long stepR;
    
    K = 0.25E7 * 25. /reqFreq;  //125MHz clock
    Kr = round(K);
    if (Kr > K)
    {
      C = round(COUNT_SIZE*(1 - (Kr -K)));
      K1 = Kr;
      K2 = Kr - 1;
    }
    else if (Kr < K)
    {
      C = round(COUNT_SIZE*(1- (K - Kr)));
      K1 = Kr;
      K2 = Kr+1;
    }
    else
    {
        K1 = K2 = K;
        C = COUNT_SIZE/2;
    }
    step = C/(double)COUNT_SIZE;
    step *= pow(2, 44);
    stepR = round(step);
    stepLo = (unsigned int)(stepR & 0xFFFFFFFF);
    stepHi = (unsigned int)((stepR >> 32) & 0xFFFFFFFF);
    printf("K1: %d, K2: %d, C: %u, Step: %lld\n", K1, K2, C, stepR);
    ioctl(fd, RFX_STREAM_SET_K1_REG, &K1);

    ioctl(fd, RFX_STREAM_SET_K2_REG, &K2);

    ioctl(fd, RFX_STREAM_SET_STEP_LO_REG, &stepLo);
  
    ioctl(fd, RFX_STREAM_SET_STEP_HI_REG, &stepHi);
 
    reg = 0;
    ioctl(fd, RFX_STREAM_SET_TIME_COMMAND_REG, &reg);
    usleep(1000);
    reg = 1;
    ioctl(fd, RFX_STREAM_SET_TIME_COMMAND_REG, &reg);
    reg = 0;
    ioctl(fd, RFX_STREAM_SET_TIME_COMMAND_REG, &reg);
}
  
    


static unsigned long long getTime(int fd, double actFrequency)
{
  unsigned int reg, lo, hi = 0, len;
  unsigned long  long time;
  reg = 0;
  ioctl(fd, RFX_STREAM_SET_TIME_COMMAND_REG, &reg);
  reg = 2;
  ioctl(fd, RFX_STREAM_SET_TIME_COMMAND_REG, &reg);
  reg = 0;
  ioctl(fd, RFX_STREAM_SET_TIME_COMMAND_REG, &reg);
  len = -1;
  ioctl(fd, RFX_STREAM_GET_SYNC_FIFO_LEN, &len);
 if(len != 2)
        printf("\n\nERRORE ERRORRISSIMO  %d\n\n\n", len);
  ioctl(fd, RFX_STREAM_GET_SYNC_FIFO_VAL, &lo);
  ioctl(fd, RFX_STREAM_GET_SYNC_FIFO_VAL, &hi);

  
  
  time = hi;
  time = (time << 32) | lo;

  time = round(1E6 * (double)time/actFrequency);
  return time;
}

static void setTime(int fd, unsigned long long timeUs, double actFreq)
{
  unsigned long long fpgaTime, ofsTime;
  unsigned int reg = 0, reg1 = 0, reg2 = 0;


  //Reset offset register
  reg = 0; 
  ioctl(fd, RFX_STREAM_SET_TIME_OFFSET_HI_REG, &reg);
  ioctl(fd, RFX_STREAM_SET_TIME_OFFSET_LO_REG, &reg);

  fpgaTime = getTime(fd, actFreq);
  ofsTime = round((timeUs - fpgaTime)*actFreq / 1E6);
  reg = ofsTime & 0x00000000FFFFFFFFL;
  ioctl(fd, RFX_STREAM_SET_TIME_OFFSET_LO_REG, &reg);
  reg = (ofsTime >> 32) & 0x00000000FFFFFFFFL;
  ioctl(fd, RFX_STREAM_SET_TIME_OFFSET_HI_REG, &reg);
  
  ioctl(fd, RFX_STREAM_GET_TIME_OFFSET_LO_REG, &reg1);
  ioctl(fd, RFX_STREAM_GET_TIME_OFFSET_HI_REG, &reg2);
 
  
  
}
  
static void updateFreq(int fd, unsigned long long *prevTimeUs, unsigned long long *prevFpgaTimeUs, double kp, double ki, double *prevFreq, double actFreq)
{
  struct timeval currTime;
  unsigned long long timeUs, fpgaTimeUs;
  double steps;
  long long stepErr, totErr, elapsedSteps, elapsedStepsFpga;
  double newFreq = *prevFreq;
  gettimeofday(&currTime, NULL);
  timeUs = (unsigned long long)currTime.tv_sec * 1000000L + currTime.tv_usec;
  fpgaTimeUs = getTime(fd, actFreq);
  totErr = fpgaTimeUs - timeUs;
  
  printf("TIME: %lld\tFPGA Time: %lld\n", timeUs, fpgaTimeUs);
  
  if(*prevTimeUs)  //The first time only measurements performed, no correction
  {
      elapsedSteps = timeUs - *prevTimeUs;
      elapsedStepsFpga = fpgaTimeUs - *prevFpgaTimeUs;
      stepErr = elapsedStepsFpga - elapsedSteps;
  
      steps = kp*(double)stepErr + ki *(double) totErr;
      //printf("STEPS: %e\n", steps);
      //printf("ELAPSED STEP FPGA: %llu\tELAPSET STEPS: %llu\n", elapsedStepsFpga, elapsedSteps);
      newFreq = 1./(1./ *prevFreq + (steps * 16 * 1E-9)/(2*1E6)); //Attuale conteggio fpga
      printf("Step Err: %lld\t Tot err: %lld\tSteps: %d\n", stepErr, totErr, (int)steps);
      setFrequency(fd, actFreq * newFreq/1E6);
      *prevFreq = newFreq;
  }
  else
  {
      *prevFreq = getFrequency(fd);
  }
  *prevTimeUs = timeUs;
  *prevFpgaTimeUs = fpgaTimeUs;
}
// 
#define UPDATE_SECS 1
static unsigned long long prevTimeUs = 0;
static unsigned long long prevFpgaTimeUs = 0;
static double prevFreq = 1E6;

void checkUpdateFreq(int fd)
{
  unsigned long long timeS;
  struct timeval currTime;
  double targetFreq = 1E6;  
  static unsigned long prevTimeS = 0;
  gettimeofday(&currTime, NULL);
  timeS = (unsigned long long)currTime.tv_sec;
  if((timeS - prevTimeS) > UPDATE_SECS)
  {
    prevTimeS = timeS;
    updateFreq(fd, &prevTimeUs, &prevFpgaTimeUs, 6., 5.,&prevFreq, targetFreq)   ; 
  }
}

