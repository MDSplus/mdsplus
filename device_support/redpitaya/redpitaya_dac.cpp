#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <rfx_dac.h>
#include <signal.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
 
extern "C" {
  int rpdacInit(int useExtClk, int useExtTrig, double clockFreq, int numPoints1, double *Y1, double *X1, int numPoints2, double *Y2, double *X2);
  void rpdacTrigger();
  void rpdacStop();
}
  
 
static int fd; 
 
//Prepare arrays to be enqueued in FPGA FIFO. It is assumed that both inTimes1 and inTimes2 start with time 0
static int convertInputs(double clockPeriod, int numPoints1, double *inValues1, double *inTimes1, int numPoints2, double *inValues2, double *inTimes2, 
    double *outStart1, double *outStart2, double *stepVals1, double *stepVals2, int *outSteps)
{
    int outIdx = 0, inIdx1 = 0, inIdx2 = 0, numOutPoints, i;
    double outTimes[numPoints1 + numPoints2];
    
    while(inIdx1 < numPoints1 || inIdx2 < numPoints2)
    {
        if(inIdx1 < numPoints1 && inIdx2 < numPoints2)
        {
            if(inTimes1[inIdx1] == inTimes2[inIdx2])
            {
                outStart1[outIdx] = inValues1[inIdx1];
                outStart2[outIdx] = inValues2[inIdx2];
                outTimes[outIdx] = inTimes1[inIdx1]; 
                inIdx1++;
                inIdx2++;
            }
            else if (inTimes1[inIdx1] < inTimes2[inIdx2])
            {
                outStart1[outIdx] = inValues1[inIdx1];
                outStart2[outIdx] = inValues2[inIdx2-1] + (inTimes1[inIdx1] - inTimes2[inIdx2 - 1])*(inValues2[inIdx2] - inValues2[inIdx2-1])/(inTimes2[inIdx2] - inTimes2[inIdx2-1]);
                outTimes[outIdx] = inTimes1[inIdx1];
                inIdx1++;
            }
            else //(inTimes1[inIdx1] > inTimes2[inIdx2])
            {
                outStart2[outIdx] = inValues2[inIdx2];
                outStart1[outIdx] = inValues1[inIdx1-1] + (inTimes2[inIdx2] - inTimes1[inIdx2 - 1])*(inValues1[inIdx1] - inValues1[inIdx1-1])/(inTimes1[inIdx1] - inTimes1[inIdx1-1]);
                outTimes[outIdx] = inTimes2[inIdx2];
                inIdx2++;
            }
        }
        else if(inIdx1 < numPoints1)
        {
            outStart1[outIdx] = inValues1[inIdx1];
            outStart2[outIdx] = outStart2[outIdx - 1];
            outTimes[outIdx] = inTimes1[inIdx1];
            inIdx1++;
        }
        else //(inIdx2 < numPoints2)
        {
            outStart2[outIdx] = inValues2[inIdx2];
            outStart1[outIdx] = outStart1[outIdx - 1];
            outTimes[outIdx] = inTimes2[inIdx2];
            inIdx2++;
        }
        outIdx++;
    }
    
    
    numOutPoints = outIdx;
    for(outIdx = 0; outIdx < numOutPoints - 1; outIdx++)
    {
        outSteps[outIdx] = (int)((outTimes[outIdx + 1] - outTimes[outIdx])/clockPeriod);
        if(outSteps[outIdx] < 1)
            outSteps[outIdx] = 1;
        stepVals1[outIdx] = (outStart1[outIdx + 1] - outStart1[outIdx]);
        stepVals2[outIdx] = (outStart2[outIdx + 1] - outStart2[outIdx]);
    }
    outSteps[numOutPoints - 1] = 2; //Just two clock perios to force last value
    stepVals1[numOutPoints - 1] = 0;
    stepVals2[numOutPoints - 1] = 0;
    for(outIdx = 0; outIdx < numOutPoints; outIdx++)
    {
        outStart1[outIdx] = 8192 - (outStart1[outIdx] * 8192);
        outStart2[outIdx] = 8192 - (outStart2[outIdx] * 8192);
        if(outStart1[outIdx] < 0) 
            outStart1[outIdx] = 0;
        if(outStart2[outIdx] < 0)
            outStart2[outIdx] = 0;
        if(outStart1[outIdx] > 16383) 
            outStart1[outIdx] = 16383;
        if(outStart2[outIdx] > 16383)
            outStart2[outIdx] = 16383;
        stepVals1[outIdx] = -stepVals1[outIdx] * 8192;     
        stepVals2[outIdx] = -stepVals2[outIdx] * 8192;     
    }   
    return numOutPoints;
}
    
int rpdacInit(int useExtClk, int useExtTrig, double clockFreq, int numPoints1, double *Y1, double *X1, int numPoints2, double *Y2, double *X2)
{
    //Make sure that time 0 is included in X1 and X1. If not add point at 0 with the same value of the first point
  double intY1[numPoints1+1], intY2[numPoints2+1], intX1[numPoints1 + 1], intX2[numPoints2 + 1];
  int actPoints1, actPoints2;
  unsigned int clockDivReg = 0, modeReg = 0, commandReg = 0, reg, i;
  unsigned long long currLongVal;

  double stepVals1[numPoints1+numPoints2 + 2], stepVals2[numPoints1+numPoints2 + 2], startVals1[numPoints1+numPoints2 + 2], startVals2[numPoints1+numPoints2 + 2];
  int steps[numPoints1+numPoints2 + 2];
  int numSteps;
   
  if(X1[0] == 0)
  {
      actPoints1 = numPoints1;
      memcpy(intX1, X1, numPoints1 * sizeof(double));
      memcpy(intY1, Y1, numPoints1 * sizeof(double));
  }
  else
  {
      actPoints1 = numPoints1 + 1;
      intX1[0] = 0;
      intY1[0] = Y1[0];
      memcpy(&intX1[1], X1, numPoints1 * sizeof(double));
      memcpy(&intY1[1], Y1, numPoints1 * sizeof(double));
  }
  if(X2[0] == 0)
  {
      actPoints2 = numPoints2;
      memcpy(intX2, X2, numPoints2 * sizeof(double));
      memcpy(intY2, Y2, numPoints2 * sizeof(double));
  }
  else
  {
      actPoints2 = numPoints2 + 1;
      intX2[0] = 0;
      intY2[0] = Y2[0];
      memcpy(&intX2[1], X2, numPoints2 * sizeof(double));
      memcpy(&intY2[1], Y2, numPoints2 * sizeof(double));
  }
  fd = open("/dev/rfx_dac", O_RDWR | O_SYNC);
  if(fd < 0)
  {
      printf("Cannot open device!\n");
      return fd;
  }
  printf("Device Open %d\n", fd);
  
  modeReg = 0;
  if(useExtClk)
  {
      modeReg |= 0x2;
      clockDivReg = 1;
  }
  else
  {
      clockDivReg = (int)(125E6/clockFreq + 0.5);
      printf("clockDivReg: %d\n", clockDivReg); 
  }
  if(useExtTrig)
  {
      modeReg |= 0x1;
  }
  printf("Mode Reg.: %d\n", modeReg);
  
  //clear fifo
  ioctl(fd, RFX_DAC_CLEAR_AXI_FIFO_MM_S_0, 0);
  usleep(10000);
  ioctl(fd,  RFX_DAC_SET_CLOCK_DIVIDE_REG, &clockDivReg);
  usleep(1000);
  // trigger software mode
  ioctl(fd,  RFX_DAC_SET_MODE_REG, &modeReg);
  usleep(1000);

  //stop
  commandReg = 2;
  ioctl(fd,  RFX_DAC_SET_CMD_REG, &commandReg);
  usleep(1000);
  
  //Arm
  commandReg = 4;
  ioctl(fd,  RFX_DAC_SET_CMD_REG, &commandReg);
  usleep(1000);
  

  
  numSteps = convertInputs(1./clockFreq, actPoints1, intY1, intX1, actPoints2, intY2, intX2, startVals1, startVals2, stepVals1, stepVals2, steps);
  

  printf("NUM STEPS:  %d\n", numSteps);
  for(i = 0; i < numSteps; i++)
  {
      printf("startVal1 = %f\tstartVal2 = %f\tstepVal1 = %f\tstepVal2 = %f\tstep = %d\n", startVals1[i], startVals2[i], stepVals1[i], stepVals2[i], steps[i]);
  }



  
  //prepare FIFO 
  for(i = 0; i < numSteps; i++)
  {
    //Start ch 1-2
        reg = (int)(startVals2[i])&0x0000FFFF;
        reg |= ((int)(startVals1[i])<<16)&0xFFFF0000;
        ioctl(fd,  RFX_DAC_SET_AXI_FIFO_MM_S_0_VAL, &reg);
        usleep(1000);
 
    //Num Steps
        reg = (unsigned int)(steps[i]);
       // reg = (unsigned int)(0x0FFFFFFF);
        ioctl(fd,  RFX_DAC_SET_AXI_FIFO_MM_S_0_VAL, &reg);
        usleep(1000);
   
    //Step ch 1
        currLongVal = (long long)((stepVals2[i]/steps[i])*pow(2,50));
        reg = (unsigned int)(currLongVal & 0x00000000FFFFFFFF);
        ioctl(fd,  RFX_DAC_SET_AXI_FIFO_MM_S_0_VAL, &reg);
        usleep(1000);
        reg = (unsigned int)((currLongVal & 0xFFFFFFFF00000000)>> 32);
        ioctl(fd,  RFX_DAC_SET_AXI_FIFO_MM_S_0_VAL, &reg);
        usleep(1000);
 
    //Step ch 2 
        currLongVal = (long long)((stepVals1[i]/steps[i])*pow(2,50));
        reg = (unsigned int)(currLongVal & 0x00000000FFFFFFFF);
        ioctl(fd,  RFX_DAC_SET_AXI_FIFO_MM_S_0_VAL, &reg);
        usleep(1000);
        reg = (unsigned int)((currLongVal & 0xFFFFFFFF00000000)>> 32);
        ioctl(fd,  RFX_DAC_SET_AXI_FIFO_MM_S_0_VAL, &reg);
        usleep(1000);
  }
  
  return fd;  
} 


void rpdacTrigger()
{
    unsigned int commandReg = 1;
    ioctl(fd,  RFX_DAC_SET_CMD_REG, &commandReg);
    usleep(1000);
}

void rpdacStop()
{
    unsigned int commandReg = 2;
    ioctl(fd,  RFX_DAC_SET_CMD_REG, &commandReg);
    usleep(1000);
}
     
    
