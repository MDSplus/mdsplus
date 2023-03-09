#include "NiFpga_MainFPGA_9159.h"
//#include "NiFpga_MainFPGA_9159Simulator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mdsobjects.h>
#include <termios.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>

#include "AsyncStoreManager.h"

#define TICK_40MHz 40000000.
#define TICK_10MHz 10000000.
#define TICK_5MHz 5000000.
#define TICK_1MHz 100000.

#define MAX_COUNT 1000
#define MIN_SEGMENT 20000
#define NUM_SLAVE_CHANNEL 18 // number of channel per cRIO slave
#define NUM_SLAVE 3          // number of cRIO slave

#define NUM_9220_CHANNEL 16 // number of channel of NI 6220 cRio module
#define NUM_9220 2          // number of NI 6220 cRio module

#define MIN_SEGMENT_ELEM 10000
#define MAX_FPGA_READ_BUF_SIZE 40000

#define FPGA_IDLE_STATE 0
#define FPGA_RUN_STATE 1
#define FPGA_ACQ_STATE 2

#define STOP_PLC_CMD 0
#define START_PLC_CMD 1

#define CLOCK_MODE_INTERNAL  1
#define CLOCK_MODE_EXTERNAL 0


static const char *simb = "|/-\\";

extern "C"
{
  NiFpga_Status crioMpagInitFifo(NiFpga_Session session, const char *fifoName, NiFpgaEx_DmaFifo fifoId, size_t fifoDepthSize);
  NiFpga_Status crioMpagResetFifo(NiFpga_Session session, const char *fifoName, NiFpgaEx_DmaFifo fifoId);
  NiFpga_Status crioMpagInit(NiFpga_Session *session, const char *cRioId, size_t fifoDepthSize);
  NiFpga_Status closeMpagFpgaSession(NiFpga_Session session);
  NiFpga_Status setMpagAcqParam(NiFpga_Session session, float freq, uint16_t highStrobeTick, const int32_t *chanAOmap);
  int mpag_readAndSaveAllChannels(NiFpga_Session session, int nChan, int *chanState, int bufSize, int segmentSize, int numSamples,
                                  void *dataNidPtr, uint8_t clockMode, int clockNid, float timeIdx0, float period, void *treePtr,
                                  void *saveListPtr, void *stopAcq, int shot, void *resampledNidPtr);
  int readMpagFifoData(NiFpga_Session session, const char *fifoName, NiFpgaEx_DmaFifo fifoId, uint64_t *data,
                       size_t slaveBufSize, void *stopAcq);
  NiFpga_Status startMpagFpga(NiFpga_Session session);

  NiFpga_Status executeMpagFpgaCommand(const char *cRioId, int command);
}

typedef enum
{
  SLAVE_A,
  SLAVE_B,
  SLAVE_C,
} enum_FPGA;

typedef struct t_struct_FPGA_ACQ
{
  NiFpga_Session session;
  int bufSize;
  int *chanState;
  int *resampledNid;
  int segmentSize;
  int numSamples;
  int *dataNid;
  int clockNid;
  float timeIdx0;
  float period;
  void *treePtr;
  void *saveListPtr;
  void *stopAcq;
  int shot;
  void *resampledNidPtr;
  char **streamNames;
  float *streamGains;
  float *streamOffsets;
  int streamFactor;
  uint8_t clockMode;
  NiFpga_Status retStatus;
} struct_FPGA_ACQ;

typedef struct t_struct_FPGA
{
  enum_FPGA slaveIdx;
  struct_FPGA_ACQ *structFpgaAcq;
  sem_t semThreadStart;
} struct_FPGA;
