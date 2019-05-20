#include "NiFpga_FAU_cRIO_FPGA.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mdsobjects.h>
#include <termios.h>
#include <fcntl.h>

#define TICK_40MHz 40000000.
#define TICK_10MHz 10000000.
#define TICK_5MHz 5000000.
#define TICK_1MHz 100000.

//Maximum number of digitai IO 64
#define NUM_DIO 32
#define MIN_SEGMENT_ELEM 10000
#define MAX_FPGA_READ_BUF_SIZE 20000

extern "C" 
{
NiFpga_Status crioFauInit(NiFpga_Session *session, size_t FifoDepthSize);
NiFpga_Status closeFauFpgaSession(NiFpga_Session session);
NiFpga_Status setFauAcqParam(NiFpga_Session session, uint16_t PTEmask, uint32_t PTEfrequency, uint16_t tickFreqCode);
NiFpga_Status startFauFpga(NiFpga_Session session);
int readFauFifoData(NiFpga_Session session, uint8_t *data, double *time, double tickPeriod, size_t maxSamp, size_t nDio, uint8_t *stopFlag);
int fauSaveAcqData(NiFpga_Session session, double tickPeriod, double trigTime, size_t maxSamp, size_t nDio, void *treePtr, void *dataNidPtr, uint8_t *stopFlag);
int  fauQueuedAcqData(NiFpga_Session session, void *fauList, uint8_t *data, double *time, double tickPeriod, size_t maxSamp, size_t nDio, void *treePtr, void *dataNidPtr, uint8_t *stopFlag);
NiFpga_Status getFauAcqState(NiFpga_Session session, uint16_t *acqState);
uint16_t IsFauFIFOOverflow(NiFpga_Session session);
NiFpga_Status startFauAcquisition(NiFpga_Session session);
NiFpga_Status pauseFauAcquisition(NiFpga_Session session);
}



