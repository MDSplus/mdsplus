#include "NiFpga_FAU_cRIO_FPGA.h"
#include <fcntl.h>
#include <mdsobjects.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define TICK_40MHz 40000000.
#define TICK_10MHz 10000000.
#define TICK_5MHz 5000000.
#define TICK_1MHz 100000.

// Maximum number of digitai IO 64
#define NUM_DIO 64
#define MIN_SEGMENT_ELEM 10000
#define MAX_FPGA_READ_BUF_SIZE 40000
const char *simb = "|/-\\";

extern "C"
{
  NiFpga_Status crioFauInit(NiFpga_Session *session, size_t FifoDepthSize);
  NiFpga_Status closeFauFpgaSession(NiFpga_Session session);
  // NiFpga_Status setFauAcqParam(NiFpga_Session session, uint16_t PTEmask,
  // uint32_t PTEfrequency, uint16_t tickFreqCode);
  NiFpga_Status setFauAcqParam(NiFpga_Session session, uint64_t PTEEnaMask,
                               uint64_t PTECountSlowFastMask,
                               uint32_t PTESlowCount, uint32_t PTEFastCount,
                               uint16_t tickFreqCode);
  NiFpga_Status startFauFpga(NiFpga_Session session);
  int readFauFifoData(NiFpga_Session session, uint8_t *data, double *time,
                      double tickPeriod, size_t *maxSamp, size_t nDio,
                      int *stopFlag);
  int fauSaveAcqData(NiFpga_Session session, double tickPeriod, double trigTime,
                     size_t maxSamp, size_t nDio, void *treePtr, void *dataNidPtr,
                     int *stopFlag);
  // int  fauQueuedAcqData(NiFpga_Session session, void *fauList, uint8_t *data,
  // double *time, double tickPeriod, size_t maxSamp, size_t nDio, void *treePtr,
  // void *dataNidPtr, int *stopFlag);
  int fauQueuedAcqData(NiFpga_Session session, void *fauList, double tickPeriod,
                       double trigTime, size_t maxSamp, size_t nDio,
                       void *treePtr, void *dataNidPtr, int *stopFlag);
  NiFpga_Status getFauAcqState(NiFpga_Session session, uint16_t *acqState);
  uint16_t IsFauFIFOOverflow(NiFpga_Session session);
  NiFpga_Status startFauAcquisition(NiFpga_Session session);
  NiFpga_Status pauseFauAcquisition(NiFpga_Session session);
}
