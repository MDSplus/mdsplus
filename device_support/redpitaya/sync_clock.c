#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <rfx_stream.h>
#include <signal.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>

#define COUNT_SIZE 2000000
int checkUpdateFreq(int fd);
void alignFreq(int fd, int iterations);
void alignClock(int fd);
#ifdef CXX
extern "C" {
#endif
void alignFreq(int fd, int iterations);
#ifdef CXX
}
#endif

static void setFrequency(int fd, double reqFreq)
{
    double reqT, K, step;
    unsigned int Kr, C, stepLo, stepHi, reg, K1, K2;
    unsigned long long stepR;
    
    reqT = 1./reqFreq;
    K = 0.25E7 * 25. /reqFreq;  //125MHz clock
//    K = 1E7 * 25. /reqFreq;
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
    printf("K1: %d, K2: %d, C: %u\n", K1, K2, C);
    ioctl(fd, RFX_STREAM_SET_K1_REG, &K1);

    ioctl(fd, RFX_STREAM_SET_K2_REG, &K2);
   ioctl(fd, RFX_STREAM_GET_K1_REG, &K1);

    ioctl(fd, RFX_STREAM_GET_K2_REG, &K2);
    printf("K1: %d, K2: %d\n", K1, K2);

    ioctl(fd, RFX_STREAM_SET_STEP_LO_REG, &stepLo);
  
    ioctl(fd, RFX_STREAM_SET_STEP_HI_REG, &stepHi);
 
    reg = 0;
    ioctl(fd, RFX_STREAM_SET_CMD_REG, &reg);
    usleep(1000);
    reg = 1;
    ioctl(fd, RFX_STREAM_SET_CMD_REG, &reg);
    reg = 0;
    ioctl(fd, RFX_STREAM_SET_CMD_REG, &reg);
}
  
    


static unsigned long long getTime(int fd, double actFrequency)
{
  unsigned int reg, lo, hi = 0, rb, len;
  unsigned long  long time;
  static unsigned long prevCount = 0;
  ioctl(fd, RFX_STREAM_CLEAR_TIMES_FIFO, 0);
  reg = 0;
  ioctl(fd, RFX_STREAM_SET_CMD_REG, &reg);
  reg = 2;
  ioctl(fd, RFX_STREAM_SET_CMD_REG, &reg);
  reg = 0;
  ioctl(fd, RFX_STREAM_SET_CMD_REG, &reg);
  len = -1;
  usleep(10);
  ioctl(fd, RFX_STREAM_GET_TIMES_FIFO_LEN, &len);
 if(len != 2)
        printf("\n\nERRORE ERRORRISSIMO %d\n\n\n", len);
  ioctl(fd, RFX_STREAM_GET_TIMES_FIFO_VAL, &lo);
  ioctl(fd, RFX_STREAM_GET_TIMES_FIFO_VAL, &hi);

  
  
  time = hi;
  time = (time << 32) | lo;
  prevCount = lo;  

  time = round(1E6 * (double)time/actFrequency);
  return time;
}

static void setTime(int fd, unsigned long long timeUs, double actFreq)
{
  unsigned long long fpgaTime, ofsTime, prevOfsTime;
  unsigned int reg = 0, reg1 = 0, reg2 = 0;


  //Reset offset register
  reg = 0; 
  ioctl(fd, RFX_STREAM_SET_TIME_OFS_HI_REG, &reg);
  ioctl(fd, RFX_STREAM_SET_TIME_OFS_LO_REG, &reg);

  fpgaTime = getTime(fd, actFreq);
  ofsTime = round((timeUs - fpgaTime)*actFreq / 1E6);
  reg = ofsTime & 0x00000000FFFFFFFFL;
  ioctl(fd, RFX_STREAM_SET_TIME_OFS_LO_REG, &reg);
  reg = (ofsTime >> 32) & 0x00000000FFFFFFFFL;
  ioctl(fd, RFX_STREAM_SET_TIME_OFS_HI_REG, &reg);
  
  ioctl(fd, RFX_STREAM_GET_TIME_OFS_LO_REG, &reg1);
  ioctl(fd, RFX_STREAM_GET_TIME_OFS_HI_REG, &reg2);
 
  
  
}
  
static int  updateFreq(int fd, unsigned long long *prevTimeUs, unsigned long long *prevFpgaTimeUs, double kp, double ki, double *prevFreq, double actFreq)
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
  
  if(*prevTimeUs)  //The first time only measurements performed, no correction
  {
      elapsedSteps = timeUs - *prevTimeUs;
      elapsedStepsFpga = fpgaTimeUs - *prevFpgaTimeUs;
      stepErr = elapsedStepsFpga - elapsedSteps;
  
      steps = kp*(double)stepErr + ki *(double) totErr;
      //printf("STEPS: %e\n", steps);
      //printf("ELAPSED STEP FPGA: %llu\tELAPSET STEPS: %llu\n", elapsedStepsFpga, elapsedSteps);
      newFreq = 1./(1./ *prevFreq + (steps * 16 * 1E-9)/(2*1E6)); //Attuale cvonteggio fpga
      printf("Step Err: %lld\t Tot err: %lld\tSteps: %d\n", stepErr, totErr, (int)steps);
      setFrequency(fd, actFreq * newFreq/1E6);
  }
  *prevTimeUs = timeUs;
  *prevFpgaTimeUs = fpgaTimeUs;
  *prevFreq = newFreq;
  return 0;
}
  
  
static int openChecked()
{
    int fd;
    fd = open("/dev/rfx_stream", O_RDWR | O_SYNC);
    while(fd < 0)
    {
        sleep(1);
        fd = open("/dev/rfx_stream", O_RDWR | O_SYNC);
    }
    return fd;
}
        
    
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

 
///////////////////////////////////To be called by device
static double prevFreq = 1E6;
static unsigned long long prevTimeUs = 0;
static unsigned long long prevFpgaTimeUs = 0; 
  

int main(int argc, char *argv[])
{
  int i;    
  int fd;
  volatile unsigned int reg;
  
  unsigned long long step, timeUs, fpgaTimeUs;
  struct timeval currTime;
  unsigned long long prevTimeUs;
  unsigned long long prevFpgaTimeUs; 
  double prevFreq ;
  double targetFreq = 1E6;
  
  fd = openChecked(); 
  printf("FD: %d\n", fd);
  printf("ALIGN FREQUENCY\n");
  prevFreq = 1E6;
  prevTimeUs = 0;    
  prevFpgaTimeUs = 0; 
  alignFreq(fd, 60);
  printf("ALIGNED\n");
  alignClock(fd);
  //close(fd);
  while(1)
  {
    checkUpdateFreq(fd);
    sleep(1);
  }
  
    return 0;
}

static double prevTimeS = 0;
#define UPDATE_SECS 1

void alignFreq(int fd, int iterations)
{
  int i;
  double targetFreq = 1E6;
  struct timeval currTime;
  unsigned long long timeUs, fpgaTimeUs;
  setFrequency(fd, targetFreq);
  gettimeofday(&currTime, NULL);
  timeUs = (unsigned long long)currTime.tv_sec * 1000000L + currTime.tv_usec;
  setTime(fd, timeUs, targetFreq);
  gettimeofday(&currTime, NULL);
  timeUs = (unsigned long long)currTime.tv_sec * 1000000L + currTime.tv_usec;
  fpgaTimeUs = getTime(fd,targetFreq);
  prevFreq = 1E6;
  prevFpgaTimeUs = timeUs;
  prevTimeUs = 0;
  for(i = 0; i < iterations; i++) 
  {
    sleep(1);

    updateFreq(fd, &prevTimeUs, &prevFpgaTimeUs, 10., 0,&prevFreq, targetFreq); 
  }
}
  


int checkUpdateFreq(int fd)
{
  unsigned long long timeS;
  struct timeval currTime;
  double targetFreq = 1E6;  
  int status;
  //int fd = openChecked(); 
  gettimeofday(&currTime, NULL);
  timeS = (unsigned long long)currTime.tv_sec;
  if((timeS - prevTimeS) > UPDATE_SECS)
  {
    prevTimeS = timeS;
   prevFreq = getFrequency(fd);
    status = updateFreq(fd, &prevTimeUs, &prevFpgaTimeUs, 6., 5.,&prevFreq, targetFreq); 
    printf("******PREV FREQ: %f   %f\n", prevFreq, getFrequency(fd));
    if(status) return status;
  }
 // close(fd);
  return 0;
}

void alignClock(int fd)
{
  unsigned long long timeUs;
  double targetFreq = 1E6;  
  struct timeval currTime;
  gettimeofday(&currTime, NULL);
  timeUs = (unsigned long long)currTime.tv_sec * 1000000L + currTime.tv_usec;
  setTime(fd, timeUs, targetFreq);
  prevFpgaTimeUs = prevTimeUs = 0;
}
