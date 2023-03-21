/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*
    2019 05 14  Acquisition loop interruption when in Transient Recorder Mode is
   called the stop?store
*/

#include <chrono>

#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <semaphore.h>
#include <sys/ioctl.h>

#include <pxi-6259-lib.h>
#include <xseries-lib.h>

#include <tcn.h>

#include <stdlib.h>
#include <stdio.h>
//#include <mdsobjects.h>
#include <vector>
#include <time.h>
#include <unordered_map>
#include <typeinfo>


#include "AsyncStoreManager.h"
#include <nisync-lib.h>



/*
using namespace MDSplus;
using namespace std;
*/

extern "C" void pxi6259_create_ai_conf_ptr(void **confPtr);
extern "C" void pxi6259_free_ai_conf_ptr(void *conf);
extern "C" void pxi6259_ai_polynomial_scaler(int16_t *raw, float *scaled,
                                             uint32_t num_samples, float *coeff,
                                             int n_coeff, int gain);
extern "C" void pxi6259_create_ao_conf_ptr(void **confPtr);
extern "C" void pxi6259_free_ao_conf_ptr(void *conf);


extern "C" void nixseries_free_ai_conf_ptr(void *conf);
extern "C" void nixseries_create_ao_conf_ptr(void **confPtr, int wavePoints);
extern "C" int  nixseries_load_ao_conf_ptr(int ao_fd, xseries_ao_conf_t *ao_conf_ptr);

extern "C" void xseries_create_ai_conf_ptr(void **confPtr,
                                           unsigned int pre_trig_smp,
                                           unsigned int post_trig_smp,
                                           char multi_trigger);
extern "C" void xseries_free_ai_conf_ptr(void *conf);
extern "C" int _xseries_get_device_info(int fd, void *cardInfo);
extern "C" int getErrno();
extern "C" int xseries_set_ai_conf_ptr(int fd, void *conf_ptr);
extern "C" int xseriesReadAndSaveAllChannels(
    int aiFd, int nChan, void *chanFdPtr, int bufSize, int segmentSize,
    int sampleToSkip, int numSamples, void *dataNidPtr, void *gainsPtr,
    void *coeffsNidPtr, int clockNid, float timeIdx0, float period,
    void *treePtr, void *saveListPtr, void *stopAcq, int shot, void *resNitPtr);
extern "C" int xseries_AI_scale(int16_t *raw, float *scaled,
                                uint32_t num_samples, float *coeff);

extern "C" void openTree(char *name, int shot, void **treePtr);
extern "C" void closeTree(void *treePtr);
extern "C" int readAndSave(int chanFd, int bufSize, int segmentSize,
                           int counter, int dataNid, int clockNid,
                           void *treePtr, void *saveList);
extern "C" void readAiConfiguration(int fd);

extern "C" int pxi6259_readAndSaveAllChannels(
    int nChan, void *chanFdPtr, int bufSize, int segmentSize, int sampleToSkip,
    int numSamples, void *dataNidPtr, void *gainsPtr, void *coeffsNidPtr,
    int clockNid, float timeIdx0, float period, void *treePtr,
    void *saveListPtr, void *stopAcq, int shot, void *resNitPtr);

extern "C" void getStopAcqFlag(void **stopAcq);
extern "C" void freeStopAcqFlag(void *stopAcq);
extern "C" void setStopAcqFlag(void *stopAcq);

extern "C" int PXIeTriggerRTSI1(int *devNums, int numChassis);

extern "C" int pxi6368EV_readAndSaveAllChannels(
    int aiFd, int nChan, void *chanMapPtr, void *chanFdPtr, int *isBurst, int *f1Div, int *f2Div,
    double maxDelay, double baseFreq, double *preTimes, double *postTimes,
    double startTime, int bufSize, int segmentSize, char **eventNames,
    void *dataNidPtr, void *treePtr, void *saveListPtr, int clockNid, int shot, int *resNids, 
    void *coeffsNidPtr, void *gainsPtr,  void *stopAcq);

extern "C" int pxi6259EV_readAndSaveAllChannels(
    int nChan, void *chanFdPtr, int *isBurst, int *f1Div, int *f2Div,
    double maxDelay, double baseFreq, double *preTimes, double *postTimes,
    double startTime, int bufSize, int segmentSize, char **eventNames,
    void *dataNidPtr, void *treePtr, void *saveListPtr, void *stopAcq);

extern "C" int64_t NI6683_get_TCN_time();
extern "C" int NI6683_stop(int devFd, int* activeFds, int size);
extern "C" int NI6683_close(int devFd, int* Fds,int size);

void pxi6259_create_ai_conf_ptr(void **confPtr)
{
  pxi6259_ai_conf_t *conf =
      (pxi6259_ai_conf_t *)malloc(sizeof(pxi6259_ai_conf_t));
  *conf = pxi6259_create_ai_conf();
  // printf( "==== aiExportStartTrigSig %d\n", conf->aiExportStartTrigSig);

  *confPtr = (void *)conf;
}

void pxi6259_free_ai_conf_ptr(void *conf) { free((char *)conf); }

void pxi6259_create_ao_conf_ptr(void **confPtr)
{
  pxi6259_ao_conf_t *conf =
      (pxi6259_ao_conf_t *)malloc(sizeof(pxi6259_ao_conf_t));
  *conf = pxi6259_create_ao_conf();
  *confPtr = (void *)conf;
}

void nixseries_free_ai_conf_ptr(void *conf) { free((char *)conf); }

void nixseries_create_ao_conf_ptr(void **confPtr, int wavePoints)
{
printf("nixseries_create_ao_conf_ptr %d \n", wavePoints);
  xseries_ao_conf_t *conf =
      (xseries_ao_conf_t *) malloc(sizeof(xseries_ao_conf_t));
  *conf = xseries_continuous_regenerated_ao(wavePoints);
  *confPtr = (void *)conf;
}

int nixseries_load_ao_conf_ptr(int ao_fd, xseries_ao_conf_t *ao_conf_ptr)
{
   return xseries_load_ao_conf(ao_fd,  *ao_conf_ptr);
}

void pxi6259_free_ao_conf_ptr(void *conf) { free((char *)conf); }

int _xseries_get_device_info(int fd, void *cardInfo)
{
  xseries_device_info_t *data = (xseries_device_info_t *)cardInfo;

  // printf("file descriptor %d\n", fd);
  // printf("card info %x\n", cardInfo);

  if (xseries_get_device_info(fd, data) < 0)
  {
    printf("Error %d %s\n", errno, strerror(errno));
    return -1;
  }

  /*
      printf( "product_id %d\n", data->product_id);
      printf( "device_name %s\n", data->device_name);
      printf( "is_simultaneous %d\n", data->is_simultaneous);
      printf( "is_pcie %d\n", data->is_pcie);
      printf( "adc_number %d\n", data->adc_number);
      printf( "dac_number %d\n", data->dac_number);
      printf( "port0_length %d\n", data->port0_length);
      printf( "serial_number %d\n", data->serial_number);
  */

  return 0;
}

int getErrno() { return errno; }

void xseries_create_ai_conf_ptr(void **confPtr, unsigned int pre_trig_smp,
                                unsigned int post_trig_smp,
                                char multi_trigger)
{
  xseries_ai_conf_t *conf =
      (xseries_ai_conf_t *)malloc(sizeof(xseries_ai_conf_t));

  // Software-timed data acquisition
  //    *conf = xseries_software_timed_ai();int xseries_set_ai_conf_ptr(int fd,
  //    void *conf_ptr)

  if (post_trig_smp > 0)
  {
    if (pre_trig_smp > 0)
    {
      printf("PXI 6368 Ptretrigger analog input acquisition. pre %d post %d \n",
             pre_trig_smp, post_trig_smp);
      *conf = xseries_reference_ai(pre_trig_smp, post_trig_smp);
    }
    else
    {
      if (multi_trigger)
      {
        printf("PXI 6368 Retriggerable finite analog input acquisition. %d \n",
               post_trig_smp);
        *conf = xseries_retriggerable_finite_ai(post_trig_smp);
      }
      else
      {
        printf("PXI 6368 Finite analog input acquisition. %d \n",
               post_trig_smp);
        *conf = xseries_finite_ai(post_trig_smp);
      }
    }
  }
  else
  {
    printf("Continuous analog input acquisition.\n");
    *conf = xseries_continuous_ai();
  }

  *confPtr = (void *)conf;
}

int xseries_set_ai_conf_ptr(int fd, void *conf_ptr)
{
  int retval;

  xseries_ai_conf_t conf;
  memcpy(&conf, conf_ptr, sizeof(xseries_ai_conf_t));

  retval = xseries_load_ai_conf(fd, *(xseries_ai_conf_t *)conf_ptr);
  if (retval)
  {
    printf("PXI 6368 Cannot load AI configuration! %s (%d)\n", strerror(errno),
           errno);
    return -1;
  }

  // wait for descriptors
  sleep(1);

  return 0;
}

void xseries_free_ai_conf_ptr(void *conf) { free((char *)conf); }

void openTree(char *name, int shot, void **treePtr)
{
  try
  {
    Tree *tree = new Tree(name, shot);
    *treePtr = (void *)tree;
    TreeNode *n = tree->getNode("\\TOP");
  }
  catch (const MdsException &exc)
  {
    printf("Cannot open tree %s %d: %s\n", name, shot, exc.what());
  }
}

void closeTree(void *treePtr)
{
  try
  {
    delete ((Tree *)treePtr);
  }
  catch (const MdsException &exc)
  {
    printf("Cannot close tree %s %d: %s\n", exc.what());
  }
}

#define MAX_ITERATIONS 100000
void readAiConfiguration(int fd)
{
  int status;
  pxi6259_ai_conf_t conf;

  status = pxi6259_read_ai_conf(fd, &conf);
  if (status < 0)
    return;

  // printf("FOUTtimebaseSelect %d\n", conf.FOUTtimebaseSelect);
  // printf("FOUTdivider %d\n", conf.FOUTdivider);
  // printf("SlowInternalTimebase %d\n", conf.SlowInternalTimebase);
  // printf("SIsource %d\n", conf.SIsource);

  printf("aiSamplePeriodDivisor %d\n", conf.aiSamplePeriodDivisor);
  printf("aiSampleDelayDivisor %d\n", conf.aiSampleDelayDivisor);
  printf("aiConvertPeriodDivisor %d\n", conf.aiConvertPeriodDivisor);
  printf("aiConvertDelayDivisor %d\n", conf.aiConvertDelayDivisor);
  // printf(" %d", conf.);
}

extern "C" int pxi6259_getCalibrationParams(int chanfd, int range, float *coeff,
                                            int *coeff_num);
int pxi6259_getCalibrationParams(int chanfd, int range, float *coeffVal,
                                 int *coeff_num)
{
  ai_scaling_coefficients_t ai_coefs;
  int32_t i, j;
  int retval;

  retval = pxi6259_get_ai_coefficient(chanfd, &ai_coefs);
  if (retval < 0)
    return retval;

  /*
      retval = ioctl(chanfd, PXI6259_IOC_GET_AI_SCALING_COEF, &ai_coefs);
      if (retval) return retval;


      printf("order %d\n", ai_coefs.order);
      printf("scale_converter_select_t %d\n", ai_coefs.converter);
  */
  for (i = 0; i <= ai_coefs.order; i++)
    coeffVal[i] = ai_coefs.value[i].f;
  *coeff_num = ai_coefs.order + 1;
  /*
          printf("value[%d].f = %e\n", i, ai_coefs.value[i].f);

      printf("mode order %d\n", ai_coefs.mode.order);
      for( i = 0; i < 4; i++)
          printf("mode value[%d].f = %e\n", i, ai_coefs.mode.value[i].f);

      printf("interval.gain   %e\n", ai_coefs.interval.gain.f);
      printf("interval.offset %e\n", ai_coefs.interval.offset.f);
  */

  /*
          for (i=0; i <= ai_coefs.order; i++)
          {
                  coeffVal[i] = (ai_coefs.mode.value[i].f *
     ai_coefs.interval.gain.f); if (i == 0) { coeffVal[i] = ai_coefs.value[i].f
     + ai_coefs.interval.offset.f;
                  }
          }
          *coeff_num = ai_coefs.order+1;
  */
  return retval;
}

/* Gain divider lookup table */
static float gain_divider[] = {
    1.0, 1.0, 2.0, 5.0, 10.0,
    20.0, 50.0, 100.0}; // 1: 10V, 2 :5V, 3: 2V, 4:1V, 5, 500mv, 6, 200mv, 7
                        // :100mv

void pxi6259_ai_polynomial_scaler(int16_t *raw, float *scaled,
                                  uint32_t num_samples, float *coeff, int order,
                                  int gain)
{
  int32_t i, j;
  float rawf;

  for (i = 0; i < num_samples; i++)
  {
    rawf = (float)raw[i];
    scaled[i] = coeff[order];

    for (j = order - 1; j >= 0; j--)
    {
      scaled[i] *= rawf;
      scaled[i] += coeff[j];
    }
    scaled[i] /= gain_divider[gain];
  }
}

#define XSERIES_MAX_BUFSIZE 100000
extern "C" int getCalibrationParams(int chanfd, int range, float *coeff);

void getStopAcqFlag(void **stopAcq)
{
  *stopAcq = (void *)malloc(sizeof(int));
  (*(int *)(*stopAcq)) = 0;
  printf("getStopAcqFlag pointer = %p = %d\n", *stopAcq, (*(int *)stopAcq));
}

void freeStopAcqFlag(void *stopAcq) { free((char *)stopAcq); }

void setStopAcqFlag(void *stopAcq)
{
  // printf("Before setStopAcqFlag pointer = %p = %d\n", stopAcq,
  // *((int*)stopAcq) );
  //(*(int*)stopAcq) = 1;
  // printf("After  setStopAcqFlag pointer = %p = %d\n", stopAcq,
  // *((int*)stopAcq) );
  (*(int *)stopAcq) = 1;
}

// Check calibration channel

/*
 * Kernel does not support floating point directly...
 */

int xseries_AI_scale(int16_t *raw, float *scaled, uint32_t num_samples,
                     float *coeff)
{
  int32_t i, j;
  float rawf;
  /*
      for (j = NUM_AI_SCALING_COEFFICIENTS - 1; j >= 0; j--)
           printf("Coeff[%d] : %e\n", j, coeff[j] );
  */

  for (i = 0; i < num_samples; i++)
  {
    scaled[i] = 0.0;
    rawf = (double)raw[i];
    for (j = NUM_AI_SCALING_COEFFICIENTS - 1; j >= 0; j--)
    {
      scaled[i] *= rawf;
      scaled[i] += coeff[j];
    }
    // printf("ai sample scaled: %d %e \n", raw[i], scaled[i]);
  }
  return 0;
}

/*
inline static void ai_scale(int16_t *raw, float *scaled,
                uint32_t num_samples, xseries_ai_scaling_coef_t *scaling)
{
        int32_t i, j;
        float rawf;
        float val;

        for (i = 0; i < num_samples; i++) {
                scaled[i] = 0.0;
                rawf = (double)raw[i];
                for (j = NUM_AI_SCALING_COEFFICIENTS - 1; j >= 0; j--) {
                    scaled[i] *= rawf;
                    scaled[i] += scaling->c[j].f;
                }
        }
}
*/

int getCalibrationParams(int chanfd, int range, float *coeffVal)
{
  xseries_ai_scaling_coef_t ai_coefs;
  int32_t i, j;
  int retval;

  /*

  Nuova versione 5.0 codac ma sembra non funzionare

       retval = get_ai_scaling_coefficient(chanfd, &ai_coefs, range);
       if (retval) {
             printf("Get ai scaling error %s \n", strerror(errno));
             return retval;
       }
  */

  retval = ioctl(chanfd, XSERIES_IOC_GET_AI_SCALING_COEF, &ai_coefs);
  if (retval)
  {
    printf("Get ai scaling error %s\n", strerror(errno));
    return retval;
  }

  for (i = 0; i < NUM_AI_SCALING_COEFFICIENTS; ++i)
  {
    coeffVal[i] = ai_coefs.cal_info.modes[0].coefficients[i].f *
                  ai_coefs.cal_info.intervals[range].gain.f;
    if (i == 0)
    {
      coeffVal[i] += ai_coefs.cal_info.intervals[range].offset.f;
    }
  }

  /*
      for (j = NUM_AI_SCALING_COEFFICIENTS - 1; j >= 0; j--)
           printf("Coeff[%d] : %e\n",  j, coeffVal[j] );
  */
  return 0;
}

int xseriesReadAndSaveAllChannels(int aiFd, int nChan, void *chanFdPtr,
                                  int bufSize, int segmentSize,
                                  int sampleToSkip, int numSamples,
                                  void *dataNidPtr, void *gainsPtr,
                                  void *coeffsNidPtr, int clockNid,
                                  float timeIdx0, float period, void *treePtr,
                                  void *saveListPtr, void *stopAcq, int shot,
                                  void *resampledNidPtr)
{
  char saveConv = 0; // Acquisition format flags 0 raw data 1 convrted dta
  int skipping = 0;  // Number of samples to not save when start time is > 0

  int sampleToRead = 0;   // Number of sample to read
  int currDataToRead = 0; // Number of current sample to read

  int chan;
  int currReadSamples; // Number of samples read
  SaveList *saveList = (SaveList *)
      saveListPtr;                  // Class to equeu data buffer to save in pulse file
  int *chanFd = (int *)chanFdPtr;   // Channe file descriptor
  int *dataNid = (int *)dataNidPtr; // Channel node identifier
  float *gains = (float *)gainsPtr;
  int *coeffsNid = (int *)coeffsNidPtr;
  int *resampledNid = (int *)resampledNidPtr; // Channel node identifier

  int readCalls[nChan];             // For statistic number of read operation pe channel
  short *buffers_s[nChan]; // Raw data buffer used when not converted data are read
  float *buffers_f[nChan];          // Converted data buffer uesed when converted dta are
                                    // read
  int readChanSmp[nChan];           // Numebr of samples to read from each channel
  int bufReadChanSmp[nChan];        // Number of sample read in the buffer for each
                                    // channel
  float streamGains[nChan];
  float streamOffsets[nChan];
  int channelRead; // Number of channel completely read

  int triggered = 0;         // Module triggered flag
  bool transientRec = false; // transient recorder flag
  char *streamNames[nChan];

  if (bufSize > XSERIES_MAX_BUFSIZE) // Buffer size sets in mdsplus device is
                                     // limited to module limit
    bufSize = XSERIES_MAX_BUFSIZE;

  printf(
      "PXIe 6368 nChan %d bufSize %d segmentSize %d numSamples %d sampleToSkip "
      "%d stopAcq %d Time 0 %e Period %e SelfList %x stoAcqPtr %x\n",
      nChan, bufSize, segmentSize, numSamples, sampleToSkip, *(int *)stopAcq,
      timeIdx0, period, saveListPtr, stopAcq);

  memset(readCalls, 0, sizeof(int) * nChan);
  memset(readChanSmp, 0, sizeof(int) * nChan);

  // stopAcq variable used to receive from phyton device the end of acquisition
  // is also used, when funcrion is colled, to define acq mode continuous 0
  // transient recorder 1
  if ((*(int *)stopAcq) == 1)
    transientRec = true;
  else
    numSamples = segmentSize;

  (*(int *)stopAcq) = 0;

  // Allocate buffer for each channels
  for (chan = 0; chan < nChan; chan++)
  {
    if (saveConv)
      buffers_f[chan] = new float[bufSize];
    else
      buffers_s[chan] = new short[bufSize];

    // Check if resampling
    TreeNode *dataNode = new TreeNode(dataNid[chan], (Tree *)treePtr);
    try
    {
      Data *streamNameData = dataNode->getExtendedAttribute("STREAM_NAME");
      streamNames[chan] = streamNameData->getString();
      deleteData(streamNameData);
      try
      {
        Data *streamGainData = dataNode->getExtendedAttribute("STREAM_GAIN");
        streamGains[chan] = streamGainData->getFloat();
      }
      catch (const MdsException &exc)
      {
        streamGains[chan] = 1;
      }
      try
      {
        Data *streamOffsetData =
            dataNode->getExtendedAttribute("STREAM_OFFSET");
        streamOffsets[chan] = streamOffsetData->getFloat();
      }
      catch (const MdsException &exc)
      {
        streamOffsets[chan] = 0;
      }
    }
    catch (const MdsException &exc)
    {
      streamNames[chan] = NULL;
      streamGains[chan] = 0;
      streamOffsets[chan] = 0;
    }
    delete (dataNode);
  }
  triggered = 0;

  // Read calibration coeffs
  float *coeffs[nChan];
  int numCoeffs[nChan];
  printf("prima TreeNode %p\n", treePtr);
  for (int chanIdx = 0; chanIdx < nChan; chanIdx++)
  {
    try
    {
      TreeNode *rangeNode =
          new TreeNode(coeffsNid[chanIdx], (MDSplus::Tree *)treePtr);
      Data *rangeData = rangeNode->getData();
      coeffs[chanIdx] = rangeData->getFloatArray(&numCoeffs[chanIdx]);
      deleteData(rangeData);
    }
    catch (const MdsException &exc)
    {
      printf("%s\n", exc.what());
    }
  }
  printf("dopo TreeNode\n");
  //////////////////


  //auto currReadTime = std::chrono::high_resolution_clock::now();
  //auto lastReadTime = std::chrono::high_resolution_clock::now();

  xseries_start_ai(aiFd);

  // Start main acquisition loop
  while (!(*(int *)stopAcq))
  {

    int testCount = 0;

    // Check if there are samples to skip common for each channel.
    // sampleToSkip is an argument of the function.
    // ATTENTION : to check with external trigger
    if (!skipping && sampleToSkip > 0)
    {
      printf("PXI 6368 Skipping data %d Nun samples %d\n", sampleToSkip,
             numSamples);
      skipping = numSamples; // save in skipping flag the number of sample to be
                             // acuire for each channel
      numSamples =
          sampleToSkip; // save as numSample to acquire th sample to skip
    }

    // ATTENTION : why dinamic allocation of this buffer
    memset(bufReadChanSmp, 0, sizeof(int) * nChan);



    chan = 0;
    channelRead = 0;
    // Acquisition loop on each channel of numSamples values
    while (channelRead != nChan)
    {
      // Check if for the current channel has been acquired all sample
      // and stop acquisition has not beeen asserted
      if (readChanSmp[chan] == numSamples || (*(int *)stopAcq))
      {
        if (transientRec)
          printf(
              "Complete acquisition for channel %d samples %d stop flag %d \n",
              chan, readChanSmp[chan], (*(int *)stopAcq));

        channelRead++;
        chan = (chan + 1) % nChan;
        continue;
      }

      sampleToRead =
          numSamples - readChanSmp[chan]; // Compute the sample to read for the
                                          // current channel
      if (sampleToRead < bufSize)
        currDataToRead = sampleToRead;
      else
        currDataToRead = bufSize;

/*
      currReadTime = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currReadTime - lastReadTime);
      std::cout << duration.count() << std::endl;
*/

      // read data from device based on acquisition mode converted 1 raw data 0
      // Functions waiting for data or timeout
      if (saveConv)
        // number fo data to red is expresse in samples
        currReadSamples = xseries_read_ai(
            chanFd[chan], &buffers_f[chan][bufReadChanSmp[chan]],
            (currDataToRead - bufReadChanSmp[chan]));
      else
        // number of sample to read must be in byte
        currReadSamples =
            read(chanFd[chan], &buffers_s[chan][bufReadChanSmp[chan]],
                 (currDataToRead - bufReadChanSmp[chan]) << 1);
      
      //lastReadTime = currReadTime;

      readCalls[chan]++;

      // if( chan == 0 && (testCount % 1000000) == 0 )
      // printf("[1] bufReadChanSmp[%d] = %d  currDataToRead %d Request data %d
      // read data %d \n",chan, bufReadChanSmp[chan] , 	currDataToRead,
      // currDataToRead - bufReadChanSmp[chan], currReadSamples >> 1);

      // Check if no data is available
      if (currReadSamples <= 0)
      {
        currReadSamples = 0;
        if (errno == EAGAIN || errno == ENODATA)
        {
          currReadSamples = 0; // No data currently available... Try again
        }
        else
        {
          if (errno == EOVERFLOW)
          {
            printf("PXI 6368 Error reading samples on ai%d: (%d) %s \n", chan,
                   errno, strerror(errno));
            for (chan = 0; chan < nChan; chan++)
              if (saveConv)
                delete (float *)buffers_f[chan];
              else
                delete (short *)buffers_s[chan];
            return -2;
          }
        }
      }

      // If sample are read the module has been triggered
      if (!triggered && currReadSamples > 0)
      {
        triggered = 1;
        printf("6368 TRIGGER!!!\n");
      }

      // Increment current data read
      if (saveConv)
        bufReadChanSmp[chan] += currReadSamples;
      else
        bufReadChanSmp[chan] += (currReadSamples >> 1);

      /*
      if( chan == 0 && (testCount % 1000000) == 0 )
        printf("[2] bufReadChanSmp[%d] = %d  currDataToRead %d \n",chan,
      bufReadChanSmp[chan] , currDataToRead);
      */
      // Enqueue data to store in the pulse file
      if (bufReadChanSmp[chan] == currDataToRead)
      {
        // Check if have been read more than required samples
        if (readChanSmp[chan] + bufReadChanSmp[chan] > numSamples)
          bufReadChanSmp[chan] = numSamples - readChanSmp[chan];

        // Compute the number of samples to complete segment acquisition
        sampleToRead = numSamples - readChanSmp[chan];

        /*
        if( chan == 0 && (testCount % 1000) == 0 )
          printf("[>>>3] bufReadChanSmp[%d] = %d readChanSmp[%d] = %d
        readChanSmp[%d] = %d sampleToRead = %d\n", chan, bufReadChanSmp[chan],
        chan, readChanSmp[chan], chan, readChanSmp[chan], sampleToRead );
        */
        if (!skipping)
        {
          int streamFactor = (int)(0.1 / period);
          if (bufSize > streamFactor)
            streamFactor = bufSize;
          else
          {
            if (streamFactor % bufSize != 0)
              streamFactor = (bufSize + 1) * (streamFactor / bufSize);
          }
          if (resampledNid)
          {
            // std::cout << "bufReadChanSmp[chan]: " << bufReadChanSmp[chan] << ", sampleToRead: " << sampleToRead << ", readChanSmp[chan]" << readChanSmp[chan] << std::endl;
            saveList->addItem(
                ((saveConv) ? (void *)buffers_f[chan]
                            : (void *)buffers_s[chan]),
                bufReadChanSmp[chan], sampleToRead,
                ((saveConv) ? FLOAT : SHORT), segmentSize, readChanSmp[chan],
                dataNid[chan], clockNid, timeIdx0, treePtr, shot, streamFactor,
                streamNames[chan], streamGains[chan], streamOffsets[chan],
                period, gains[chan], coeffs[chan], numCoeffs[chan],
                resampledNid[chan]);
          }
          else
          {
            // std::cout << "bufReadChanSmp[chan]: " <<  bufReadChanSmp[chan] << std::endl;
            saveList->addItem(
                ((saveConv) ? (void *)buffers_f[chan]
                            : (void *)buffers_s[chan]),
                bufReadChanSmp[chan], sampleToRead,
                ((saveConv) ? FLOAT : SHORT), segmentSize, readChanSmp[chan],
                dataNid[chan], clockNid, timeIdx0, treePtr, shot, streamFactor,
                streamNames[chan], streamGains[chan], streamOffsets[chan],
                period, gains[chan], coeffs[chan], numCoeffs[chan]);
          }

          // allocate new buffer to save the next segment
          if (saveConv)
            buffers_f[chan] = new float[bufSize];
          else
            buffers_s[chan] = new short[bufSize];
        }
        // Update the number of samples read
        readChanSmp[chan] += bufReadChanSmp[chan];
        // Reset the the number of sample read for the next segment
        bufReadChanSmp[chan] = 0;
      }
      chan = (chan + 1) % nChan;

      /*
                  //Stop Transien Recoder acquisition
                  if( transientRec && (*(int*)stopAcq) )
                  {
                     printf("STOP Transient Recorder Acquisition\n");
                     break;
                  }
      */
      testCount++;

    } // End Segment acquisition loop for each channel

    // Reset variables for skiping samples
    if (!(*(int *)stopAcq) && skipping)
    {
      // printf("Data to saved %d\n", readChanSmp[0]);
      memset(readChanSmp, 0, sizeof(int) * nChan);

      numSamples = skipping;
      sampleToSkip = 0;
      sampleToRead = 0;
      skipping = 0;
      continue;
    }

    printf("PXI 6368 segment Acquired for all channels\n");
    if (transientRec)
      break;

    numSamples = readChanSmp[0] + segmentSize;

  } // End main acquisition loop

  /*
      for (chan=0; chan < nChan; chan++)
          printf("readCalls[%d] = %d readChanSmp[%d] = %d \n ",chan,
     readCalls[chan]);
  */
  if (readChanSmp[0] == 0)
    return -1;

  printf("PXI 6368 STOP C Acquisition %d\n", *(int *)stopAcq);
  return 1;
}

//=============================
#define PXI6259_MAX_BUFSIZE 50000

int pxi6259_readAndSaveAllChannels(
    int nChan, void *chanFdPtr, int bufSize, int segmentSize, int sampleToSkip,
    int numSamples, void *dataNidPtr, void *gainsPtr, void *coeffsNidPtr,
    int clockNid, float timeIdx0, float period, void *treePtr,
    void *saveListPtr, void *stopAcq, int shot, void *resampledNidPtr)
{
  char saveConv = 0; // Acquisition format flags 0 raw data 1 convrted dta
  int skipping = 0;

  int sampleToRead = 0;
  int currDataToRead = 0;

  int currReadSamples;
  int chan;
  SaveList *saveList = (SaveList *)saveListPtr;
  int *chanFd = (int *)chanFdPtr;
  int *dataNid = (int *)dataNidPtr;
  float *gains = (float *)gainsPtr;
  int *coeffsNid = (int *)coeffsNidPtr;
  int *resampledNid = (int *)resampledNidPtr;
  bool transientRec = false;

  int readCalls[nChan];

  short *buffers_s[nChan];
  float *buffers_f[nChan];
  int readChanSmp[nChan];
  int bufReadChanSmp[nChan];
  int channelRead;
  int triggered = 0;
  char *streamNames[nChan];
  float streamGains[nChan];
  float streamOffsets[nChan];

  if (bufSize > PXI6259_MAX_BUFSIZE)
    bufSize = PXI6259_MAX_BUFSIZE;

  printf("PXI 6259 nChan %d bufSize %d segmentSize %d numSamples %d stopAcq %d "
         "Time 0 %e Period %e\n",
         nChan, bufSize, segmentSize, numSamples, *(int *)stopAcq, timeIdx0,
         period);

  memset(readCalls, 0, sizeof(int) * nChan);
  memset(readChanSmp, 0, sizeof(int) * nChan);

  // Delete first all data nids
  for (int i = 0; i < nChan; i++)
  {
    try
    {
      TreeNode *currNode = new TreeNode(dataNid[i], (Tree *)treePtr);
      currNode->deleteData();
      // Check if resampling
      try
      {
        Data *streamNameData = currNode->getExtendedAttribute("STREAM_NAME");
        streamNames[i] = streamNameData->getString();
        deleteData(streamNameData);
        try
        {
          Data *streamGainData = currNode->getExtendedAttribute("STREAM_GAIN");
          streamGains[i] = streamGainData->getFloat();
        }
        catch (const MdsException &exc)
        {
          streamGains[i] = 1;
        }
        try
        {
          Data *streamOffsetData =
              currNode->getExtendedAttribute("STREAM_OFFSET");
          streamOffsets[i] = streamOffsetData->getFloat();
        }
        catch (const MdsException &exc)
        {
          streamOffsets[i] = 0;
        }
      }
      catch (const MdsException &exc)
      {
        streamNames[i] = NULL;
        streamGains[i] = 0;
        streamOffsets[i] = 0;
      }
      delete currNode;
      if (resampledNid)
      {
        currNode = new TreeNode(resampledNid[i], (Tree *)treePtr);
        currNode->deleteData();
        delete currNode;
      }
    }
    catch (const MdsException &exc)
    {
      printf("Error deleting data nodes\n");
    }
  }

  if ((*(int *)stopAcq) == 1)
    transientRec = true;
  else
    numSamples = segmentSize;

  (*(int *)stopAcq) = 0;

  for (chan = 0; chan < nChan; chan++)
    if (saveConv)
      buffers_f[chan] = new float[bufSize];
    else
      buffers_s[chan] = new short[bufSize];

  triggered = 0;

  // Read calibration coeffs
  float *coeffs[nChan];
  int numCoeffs[nChan];
  for (int chanIdx = 0; chanIdx < nChan; chanIdx++)
  {
    TreeNode *rangeNode =
        new TreeNode(coeffsNid[chanIdx], (MDSplus::Tree *)treePtr);
    Data *rangeData = rangeNode->getData();
    coeffs[chanIdx] = rangeData->getFloatArray(&numCoeffs[chanIdx]);
    deleteData(rangeData);
  }

  //////////////////

  while (!(*(int *)stopAcq))
  {

    // printf("stopAcq %d\n", *(int *)stopAcq );

    if (!skipping && sampleToSkip > 0)
    {
      printf("PXI 6259 Skipping data %d Nun samples %d\n", sampleToSkip,
             numSamples);
      skipping = numSamples;
      numSamples = sampleToSkip;
    }

    memset(bufReadChanSmp, 0, sizeof(int) * nChan);

    chan = 0;
    channelRead = 0;
    while (channelRead != nChan)
    {
      if (readChanSmp[chan] == numSamples || (*(int *)stopAcq))
      {
        channelRead++;
        chan = (chan + 1) % nChan;
        continue;
      }

      currDataToRead = bufSize;
      sampleToRead = numSamples - readChanSmp[chan];
      if (sampleToRead < bufSize)
        currDataToRead = sampleToRead;

      if (saveConv)
        currReadSamples = pxi6259_read_ai(
            chanFd[chan], &buffers_f[chan][bufReadChanSmp[chan]],
            (currDataToRead - bufReadChanSmp[chan]));
      else
        currReadSamples =
            read(chanFd[chan], &buffers_s[chan][bufReadChanSmp[chan]],
                 (currDataToRead - bufReadChanSmp[chan]) << 1);

      readCalls[chan]++;

      // printf("bufReadChanSmp[%d] = %d  currDataToRead %d Request data %d read
      // data %d \n",chan, bufReadChanSmp[chan] , currDataToRead, currDataToRead
      // - bufReadChanSmp[chan], currReadSamples >> 1);

      if (currReadSamples <= 0)
      {
        currReadSamples = 0;
        if (errno == EAGAIN || errno == ENODATA)
        {
          // usleep(50);
          currReadSamples = 0; // No data currently available... Try again
                               // continue;
        }
        else
        {
          if (errno == EOVERFLOW)
          {
            printf("PXI 6259 Error reading samples on ai%d: (%d) %s \n", chan,
                   errno, strerror(errno));

            for (chan = 0; chan < nChan; chan++)
              if (saveConv)
                delete (float *)buffers_f[chan];
              else
                delete (short *)buffers_s[chan];

            return -2;
          }
        }
      }

      if (!triggered && currReadSamples > 0)
      {
        triggered = 1;
        printf("6259 TRIGGER!!!\n");
      }

      if (saveConv)
      {
        // readChanSmp[chan] += currReadSamples;
        bufReadChanSmp[chan] += currReadSamples;
      }
      else
      {
        // readChanSmp[chan] += ( currReadSamples >> 1 );
        bufReadChanSmp[chan] += (currReadSamples >> 1);
      }

      if (bufReadChanSmp[chan] == currDataToRead)
      {
        /*
                        if ( counters[chan] + bufReadChanSmp[chan] > numSamples
           ) bufReadChanSmp[chan] = numSamples - counters[chan]; sampleToRead =
           numSamples - counters[chan];
        */
        if (readChanSmp[chan] + bufReadChanSmp[chan] > numSamples)
          bufReadChanSmp[chan] = numSamples - readChanSmp[chan];
        sampleToRead = numSamples - readChanSmp[chan];

        // printf("bufReadChanSmp[%d] = %d readChanSmp[%d] = %d readChanSmp[%d]
        // = %d sampleToRead = %d\n", chan, bufReadChanSmp[chan], chan,
        // readChanSmp[chan], chan, readChanSmp[chan], sampleToRead );

        if (!skipping)
        {

          int streamFactor = (int)(0.1 / period);
          if (bufSize > streamFactor)
            streamFactor = bufSize;
          else
          {
            if (streamFactor % bufSize != 0)
              streamFactor = (bufSize + 1) * (streamFactor / bufSize);
          }

          if (resampledNid)
            saveList->addItem(
                ((saveConv) ? (void *)buffers_f[chan]
                            : (void *)buffers_s[chan]),
                bufReadChanSmp[chan], sampleToRead,
                ((saveConv) ? FLOAT : SHORT), segmentSize, readChanSmp[chan],
                dataNid[chan], clockNid, timeIdx0, treePtr, shot, streamFactor,
                streamNames[chan], streamGains[chan], streamOffsets[chan],
                period, gains[chan], coeffs[chan], numCoeffs[chan],
                resampledNid[chan]);
          else
            saveList->addItem(
                ((saveConv) ? (void *)buffers_f[chan]
                            : (void *)buffers_s[chan]),
                bufReadChanSmp[chan], sampleToRead,
                ((saveConv) ? FLOAT : SHORT), segmentSize, readChanSmp[chan],
                dataNid[chan], clockNid, timeIdx0, treePtr, shot, streamFactor,
                streamNames[chan], streamGains[chan], streamOffsets[chan],
                period, gains[chan], coeffs[chan], numCoeffs[chan]);

          if (saveConv)
            buffers_f[chan] = new float[bufSize];
          else
            buffers_s[chan] = new short[bufSize];
        }

        // counters[chan] += bufReadChanSmp[chan];
        readChanSmp[chan] += bufReadChanSmp[chan];
        bufReadChanSmp[chan] = 0;
      }

      chan = (chan + 1) % nChan;

      /*
                  //Stop Transien Recoder acquisition
                  if( transientRec && (*(int*)stopAcq) )
                  {
                     printf("STOP Transient Recorder Acquisition\n");
                     break;
                  }
      */
    }

    if (!(*(int *)stopAcq) && skipping)
    {
      // printf("Data to saved %d\n", readChanSmp[0]);
      // memset(counters, 0, sizeof(int) * nChan);
      memset(readChanSmp, 0, sizeof(int) * nChan);

      numSamples = skipping;
      sampleToSkip = 0;
      sampleToRead = 0;
      skipping = 0;
      continue;
    }

    printf("PXI 6259 segment Acquired for all channels %d\n", readChanSmp[0]);
    if (transientRec)
      break;
    /*
            for( chan = 0; chan < nChan; chan++ )
                buffers_s[chan] = new unsigned short[bufSize];
                buffers_f[chan] = new float[bufSize];
    */
    // memset(counters, 0, sizeof(int) * nChan);
    // memset(readChanSmp, 0, sizeof(int) * nChan);
    numSamples = readChanSmp[0] + segmentSize;
  }

  for (chan = 0; chan < nChan; chan++)
    if (saveConv)
      delete (float *)buffers_f[chan];
    else
      delete (short *)buffers_s[chan];

  /*
      for (chan=0; chan < nChan; chan++)
          printf("readCalls[%d] = %d \n ",chan, readCalls[chan]);
  */
  if (readChanSmp[0] == 0)
    return -1;

  printf("STOP C 6259 Acquisition %d\n", *(int *)stopAcq);
  return 1;
}

/**************************************************************************************/
/*********************PROBE TEMPERATURE CONTROL
 * ***************************************/
/**************************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>

extern "C" int configureDigital(int *chanDigitFD, uint32_t deviceNum,
                                uint32_t outChanOnOff);
extern "C" int configureOutput(int *chanOutFD, uint32_t deviceNum,
                               uint32_t outChanRef, uint32_t outChanOnOff);
extern "C" int configureInput(int *chanInFd, uint32_t deviceNum,
                              uint32_t inChan[], double frequency, int numChan);
extern "C" int temperatureCtrlCommand(char *cmd);
extern "C" int temperatureProbeControl(uint32_t boardID, uint32_t *inChan,
                                       int numChan, int aiFdChanIdx,
                                       double freq, int aochan, int dochan,
                                       double tempRef);

#define DEVICE_FILE "/dev/pxi6259"

int configureDigital(int *chanDigitFD, uint32_t deviceNum,
                     uint32_t outChanOnOff)
{
  pxi6259_dio_conf_t dioConfig;
  char filename[256];
  int devFD;

  uint32_t port0Mask = 1 << outChanOnOff;

  // open DIO file descriptor
  sprintf(filename, "%s.%u.dio", DEVICE_FILE, deviceNum);
  devFD = open(filename, O_RDWR);
  if (devFD < 0)
  {
    fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
    return -1;
  }

  // initialize DIO configuration
  dioConfig = pxi6259_create_dio_conf();

  // configure DIO P0.0 ports
  if (pxi6259_add_dio_channel(&dioConfig, DIO_PORT0, port0Mask))
  {
    fprintf(stderr, "Failed to configure DIO port 0!\n");
    return -1;
  }

  // load DIO configuration and let it apply
  if (pxi6259_load_dio_conf(devFD, &dioConfig))
  {
    fprintf(stderr, "Loading DIO configuration failed!\n");
    return -1;
  }

  sprintf(filename, "%s.%u.dio.%u", DEVICE_FILE, deviceNum, 0);
  *chanDigitFD = open(filename, O_RDWR | O_NONBLOCK);
  if (*chanDigitFD < 0)
  {
    fprintf(stderr, "Failed to open port: %s\n", strerror(errno));
    return -1;
  }

  return devFD;
}

int configureOutput(int *chanOutFD, uint32_t deviceNum, uint32_t outChanRef,
                    uint32_t outChanOnOff)
{
  char filename[256];
  int i;
  pxi6259_ao_conf_t aoConfig;
  int devFD;

  // open AO file descriptor
  sprintf(filename, "%s.%u.ao", DEVICE_FILE, deviceNum);
  devFD = open(filename, O_RDWR);
  if (devFD < 0)
  {
    fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
    return -1;
  }

  // initialize AO configuration
  // aoConfig = pxi6259_create_ao_conf();

  // configure AO channel reference
  if (pxi6259_add_ao_channel(&aoConfig, outChanRef, AO_DAC_POLARITY_BIPOLAR))
  {
    fprintf(stderr, "Failed to configure channel %d reference! %s\n",
            outChanRef, strerror(errno));
    return -1;
  }

  // configure AO channel on/off
  if (pxi6259_add_ao_channel(&aoConfig, outChanOnOff,
                             AO_DAC_POLARITY_BIPOLAR))
  {
    fprintf(stderr, "Failed to configure channel %d reference! : %s\n",
            outChanOnOff, strerror(errno));
    return -1;
  }

  // enable signal generation
  if (pxi6259_set_ao_attribute(&aoConfig, AO_SIGNAL_GENERATION,
                               AO_SIGNAL_GENERATION_STATIC))
  {
    fprintf(stderr, "Failed to enable generating static signal!: %s\n",
            strerror(errno));
    return -1;
  }

  // set continuous mode
  if (pxi6259_set_ao_attribute(&aoConfig, AO_CONTINUOUS, 0))
  {
    fprintf(stderr, "Failed to set continuous mode!: %s\n", strerror(errno));
    return -1;
  }

  // load AO configuration and let it apply
  if (pxi6259_load_ao_conf(devFD, &aoConfig))
  {
    fprintf(stderr, "Failed to load output configuration! : %s\n",
            strerror(errno));
    return -1;
  }

  // open file descriptor for each AO channel
  sprintf(filename, "%s.%u.ao.%u", DEVICE_FILE, deviceNum, outChanRef);
  chanOutFD[0] = open(filename, O_RDWR | O_NONBLOCK);
  if (*chanOutFD < 0)
  {
    fprintf(stderr, "Failed to open channel %u: %s\n", outChanRef,
            strerror(errno));
    return -1;
  }

  sprintf(filename, "%s.%u.ao.%u", DEVICE_FILE, deviceNum, outChanOnOff);
  chanOutFD[1] = open(filename, O_RDWR | O_NONBLOCK);
  if (*chanOutFD < 0)
  {
    fprintf(stderr, "Failed to open channel %u: %s\n", outChanOnOff,
            strerror(errno));
    return -1;
  }

  // start AO segment (signal generation)
  if (pxi6259_start_ao(devFD))
  {
    fprintf(stderr, "Failed to start segment!n");
    return -1;
  }

  return devFD;
}

int configureInput(int *chanInFd, uint32_t deviceNum, uint32_t inChan[],
                   double frequency, int numChan)
{
  int diffMapChannel[17] = {-1, 0, 1, 2, 3, 4, 5, 6, 7,
                            16, 17, 18, 19, 20, 21, 22, 23};
  char filename[256];
  int i;
  pxi6259_ai_conf_t aiConfig;
  int devFD;
  int period;

  // open AI file descriptor
  sprintf(filename, "%s.%u.ai", DEVICE_FILE, deviceNum);
  devFD = open(filename, O_RDWR);
  if (devFD < 0)
  {
    fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
    return -1;
  }

  // initialize AI configuration
  aiConfig = pxi6259_create_ai_conf();

  // configure AI channels 0 - 5 V differential
  for (int i = 0; i < numChan; i++)
  {

    if (pxi6259_add_ai_channel(&aiConfig, diffMapChannel[inChan[i]],
                               AI_POLARITY_BIPOLAR, 2,
                               AI_CHANNEL_TYPE_DIFFERENTIAL, 0))
    /*
                    printf("Channel conf %d\n", diffMapChannel[inChan[i]] );
                    if (pxi6259_add_ai_channel(&aiConfig,
       diffMapChannel[inChan[i]], AI_POLARITY_BIPOLAR, 2, AI_CHANNEL_TYPE_RSE,
       0))

    */
    {

      fprintf(stderr, "Failed to configure channel %u\n", inChan[i]);
      return -1;
    }
  }

  // configure AI sampling clock to sample with 2 samples/second
  period = (int)(20000000. / frequency);
  if (pxi6259_set_ai_sample_clk(
          &aiConfig, period, 3, AI_SAMPLE_SELECT_SI_TC,
          AI_SAMPLE_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE))
  {
    fprintf(stderr, "Failed to configure AI sampling clock!\n");
    return -1;
  }

  // load AI configuration and let it apply
  if (pxi6259_load_ai_conf(devFD, &aiConfig))
  {
    fprintf(stderr, "Failed to load input configuration! %s\n",
            strerror(errno));
    return -1;
  }

  // open file descriptor for each AI channel
  for (int i = 0; i < numChan; i++)
  {
    sprintf(filename, "%s.%u.ai.%u", DEVICE_FILE, deviceNum,
            diffMapChannel[inChan[i]]);
    // printf("%s\n", filename);
    chanInFd[i] = open(filename, O_RDWR | O_NONBLOCK);
    if (chanInFd[i] < 0)
    {
      fprintf(stderr, "Failed to open channel %u: %s\n", inChan[i],
              strerror(errno));
      return -1;
    }
  }

  // start AI segment (data acquisition)
  if (pxi6259_start_ai(devFD))
  {
    fprintf(stderr, "Failed to start data acquisition!\n");
    return -1;
  }

  usleep(100);

  return devFD;
}

int temperatureCtrlCommand(char *cmd)
{
  sem_t *semPause_id;
  sem_t *semWake_id;
  int wakeState;
  int i;

  semPause_id = sem_open("PauseControl", O_CREAT, 0666, 0);
  if (semPause_id == SEM_FAILED)
  {
    perror("child sem_open");
    return -1;
  }

  semWake_id = sem_open("WakeControl", O_CREAT, 0666, 0);
  if (semWake_id == SEM_FAILED)
  {
    perror("child sem_open");
    return -1;
  }

  if (strcmp(cmd, "stop") == 0)
  {
    printf("============= STOP Warmer =====================\n");
    if (sem_getvalue(semWake_id, &wakeState) < 0)
    {
      perror("Control sem_getvalue");
      return -1;
    }

    // printf("STOP wake state %d\n", wakeState);
    for (i = 0; i < wakeState; i++)
    {
      if (sem_wait(semWake_id) < 0)
      {
        perror("sem_wait WakeControl");
        return -1;
      }
      /*
                      if( sem_post( semPause_id  ) < 0 )
                      {
                              perror("sem_post PauseControl");
                              return 0;
                      }
      */
    }
  }

  if (strcmp(cmd, "start") == 0 || strcmp(cmd, "exit") == 0)
  {
    if (strcmp(cmd, "start") == 0)
      printf("============= START Warmer ====================\n");

    if (sem_getvalue(semWake_id, &wakeState) < 0)
    {
      perror("Control sem_getvalue");
      return -1;
    }

    // printf("START wake state %d\n", wakeState);
    if (wakeState == 0)
    {
      // printf("START pause state %d\n", wakeState);

      if (sem_post(semPause_id) < 0)
      {
        perror("sem_post PauseControl");
        return -1;
      }
      if (sem_post(semWake_id) < 0)
      {
        perror("sem_post WakeControl");
        return -1;
      }
    }
    if (strcmp(cmd, "exit") == 0)
    {
      printf("============= EXIT Warmer Control =============\n");
      if (sem_post(semWake_id) < 0)
      {
        perror("sem_post WakeControl");
        return -1;
      }
      printf("Close semaphore id\n");

      if (sem_close(semPause_id) < 0)
        perror("Control sem_close");

      if (sem_close(semWake_id) < 0)
        perror("Control sem_close");
    }
  }
  printf("===============================================\n");
  return 0;
}

double calibTCN(double val)
{
  double out;

  double K = 47.513 * 1.e-3 / 5.;

  double C1 = 3.86896e-2;
  double C2 = -1.08267e-6;
  double C3 = 4.70205e-11;
  double C4 = -2.12169e-18;
  double C5 = -1.17272e-19;
  double C6 = 5.39280e-24;
  double C7 = -7.98156e-29;

  val = K * val * 1.e6;
  double val2 = val * val;
  double val3 = val2 * val;
  double val4 = val3 * val;
  double val5 = val4 * val;
  double val6 = val5 * val;
  double val7 = val5 * val;

  out = C1 * val + C2 * val2 + C3 * val3 + C4 * val4 + C5 * val5 + C6 * val6 +
        C7 * val7;

  /*
          scaled[i] = 0.0;
          rawf = (double)raw[i];
          for (j = NUM_AI_SCALING_COEFFICIENTS - 1; j >= 0; j--) {
              scaled[i] *= rawf;
              scaled[i] += scaling->c[j].f;
          }
  */
  return out;
}

int temperatureProbeControl(uint32_t boardID, uint32_t *inChan, int numChan,
                            int aiFdChanIdx, double frequency, int aochan,
                            int dochan, double tempRef)
{

  static char log[5] = "|/-\\";
  static int count = 0;

  uint32_t deviceNum;
  // int numChan = 2;
  // uint32_t inChan[numAiChannle];
  // double    frequency;
  uint32_t outChanRef;
  uint32_t outChanOnOff;
  uint32_t rc;

  int chanInFD[numChan];
  int devInFD;

  int chanOutFD[2];
  int devOutFD;

  int chanDigitFD;
  int devDigitFD;

  TreeNode *node[16];
  TreeNode *errorNode;
  TreeNode *vRefNode;

  int wakeState = 1;
  int error = 0;

  float ZERO_VOLT = 0;
  float FIVE_VOLT = 5.;
  uint8_t ON = 0x01;
  uint8_t OFF = 0x00;

  float setPointT;
  float tempVal;
  float E;
  bool enableTcCheck = true;

  if (aiFdChanIdx < 0)
  {
    aiFdChanIdx = -aiFdChanIdx;
    enableTcCheck = false;
    printf("Disable check on TC1 TC2 temperature difference\n");
  }

  printf("BoardID %d\n", boardID);
  printf("numChan %d\n", numChan);
  for (int i = 0; i < numChan; i++)
  {
    printf("chan %d = %d\n", i, inChan[i]);
  }
  printf("numChan %d\n", numChan);
  printf("FeedBack chan Idx %d\n", inChan[aiFdChanIdx]);
  printf("frequency %e\n", frequency);
  printf("Power supply reference chan %d\n", aochan);
  printf("Rele control digital id %d\n", dochan);
  printf("Temperature set point %e\n", tempRef);

  deviceNum = boardID;
  /*
          inChan[0] = aichan1;
          printf("ch %d\n", inChan[0] );
          inChan[1] = aichan2;
          printf("ch %d\n", inChan[1] );
          frequency = freq;
  */
  outChanRef = aochan;
  outChanOnOff = dochan;

  setPointT = tempRef;

  printf("Set point temperatura %f\n", setPointT);

  sem_t *pauseSem_id;
  sem_t *wakeSem_id;

  pauseSem_id = sem_open("PauseControl", O_CREAT, 0666, 0);
  if (pauseSem_id == SEM_FAILED)
  {
    perror("pause Control sem_open");
    return -1;
  }

  wakeSem_id = sem_open("WakeControl", O_CREAT, 0666, 1);
  if (pauseSem_id == SEM_FAILED)
  {
    perror("wake Control sem_open");
    return -1;
  }

  try
  {
    time_t rawtime;
    struct tm *timeinfo;
    char strShot[256];
    char dataFile[256];
    unsigned long shot;
    Tree *t;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    sprintf(strShot, "%d%0.2d%0.2d", 1900 + timeinfo->tm_year,
            timeinfo->tm_mon + 1, timeinfo->tm_mday);
    shot = strtoul(strShot, NULL, 0);
    sprintf(dataFile, "%s/ipp_tc_trend_%d.datafile",
            getenv("ipp_tc_trend_path"), shot);

    // printf("date %s shot %d str shot %s file %s\n", asctime(timeinfo), shot,
    // strShot, dataFile);

    FILE *fd = fopen(dataFile, "r");
    if (!fd)
    {
      t = new Tree((char *)"ipp_tc_trend", -1);
      t->createPulse(shot);
    }
    else
    {
      fclose(fd);
    }

    t = new Tree((char *)"ipp_tc_trend", shot);

    for (int i = 0; i < numChan; i++)
    {
      char path[256];
      sprintf(path, "\\IPP_TC_TREND::TC%d", inChan[i]);
      printf("NODO %s \n", path);
      node[i] = t->getNode(path);
    }

    errorNode = t->getNode((char *)"\\IPP_TC_TREND::ERROR");
    vRefNode = t->getNode((char *)"\\IPP_TC_TREND::VREF");
  }
  catch (const MdsException &exc)
  {
    printf("%s\n", exc.what());
    return -1;
  }

  while (!error)
  {

    if (!wakeState)
    {
      printf("WAIT\n");
      if (sem_wait(pauseSem_id) < 0)
      {
        perror("Control sem_wait");
        return -1;
      }
      wakeState = 1;
    }

    if ((devInFD = configureInput(chanInFD, deviceNum, inChan, frequency,
                                  numChan)) < 0)
    {
      fprintf(stderr, "Error configure input channel!\n");
      return -1;
    }

    if ((devOutFD = configureOutput(chanOutFD, deviceNum, outChanRef,
                                    outChanOnOff)) < 0)
    {
      fprintf(stderr, "Error configure ouput channel!\n");
      return -1;
    }

    if ((devDigitFD = configureDigital(&chanDigitFD, deviceNum, outChanOnOff)) <
        0)
    {
      fprintf(stderr, "Error configure digital channel!\n");
      return -1;
    }

    printf("START controll\n");

    // Control tc
    while (wakeState)
    {
      int n = 0;
      int i = 0;
      int nChRead;
      int scans_read[16];
      float value[16];
      float vRef;

      /*

                              rc = pxi6259_write_ao(chanOutFD[1], &FIVE_VOLT,
         1); if (rc < 0) { fprintf(stderr, "Failed to write to AO channel
         ON/OFF: %u\n", outChanOnOff); error = 1; goto out;
                              }
      */
      if (write(chanDigitFD, &ON, 4) != 4)
      {
        fprintf(stderr, "Error writing ON to port 0! Error: %s\n",
                strerror(errno));
        error = 1;
        goto out;
      }

      memset(scans_read, 0, sizeof(scans_read));
      nChRead = 0;
      while (nChRead < numChan)
      {
        if (scans_read[i] <= 0)
        {
          // printf("read channel %d n chan %d \n", i, nChRead);
          scans_read[i] = pxi6259_read_ai(chanInFD[i], &value[i], 1);

          if (scans_read[i] < 0)
          {
            if (errno != EAGAIN)
            {
              fprintf(stderr, "Failed while reading channel: %u: %s %d\n",
                      inChan[i], strerror(errno), errno);
              error = 1;
              goto out;
            }
          }
          else
            nChRead++;
        }
        i = (i + 1) % numChan;
      }

      // Start Control algorithm
      // tempVal = 1300./5. * value[aiFdChanIdx];
      tempVal = (float)calibTCN((double)value[aiFdChanIdx]);
      // printf("Temp %f V %f C \n", value[aiFdChanIdx] , tempVal);
      E = setPointT - tempVal;
      if (E >= 0)
      {
        vRef = 0.2 * E;
        if (vRef > 3.0)
          vRef = 3.0;
      }
      else
        vRef = 0.0;

      if (fabs(value[0] - value[1]) > 30. && enableTcCheck)
      {
        fprintf(stderr, "Difference between TC1 and TC2 > than 30 C reset "
                        "reference signal \n");
        vRef = 0.0;
      }

      // vRef =  setPointT ;
      // vRef = value[aiFdChanIdx];

      // End control algoritm

      rc = pxi6259_write_ao(chanOutFD[0], &vRef, 1);
      if (rc < 0)
      {
        fprintf(stderr, "Failed to write to AO channel reference: %u\n",
                outChanRef);
        error = 1;
        goto out;
      }

      struct timeb tb;
      int64_t currTime;
      try
      {
        ftime(&tb);
        Float32 *currData;
        currTime = (int64_t)(tb.time * 1000 + tb.millitm);
        for (int i = 0; i < numChan; i++)
        {
          currData = new Float32(value[i]);
          node[i]->putRow(currData, &currTime);
        }

        currData = new Float32(E);
        errorNode->putRow(currData, &currTime);

        currData = new Float32(vRef);
        vRefNode->putRow(currData, &currTime);
      }
      catch (const MdsException &exc)
      {
        printf("%s\n", exc.what());
        error = 1;
        goto out;
      }

      if (sem_getvalue(wakeSem_id, &wakeState) < 0 || wakeState == 2)
      {
        if (wakeState != 2)
          perror("Control sem_open");
        error = 1;
        goto out;
      }
      /*
                              struct tm * timeinfo;
                              time_t pp =  (time_t)( tb.time );
                              timeinfo = localtime ( &pp );
                              printf(" %x Value[0] %f %s:%d\n\n Wake state %d
         \n", currTime, value[0], asctime(timeinfo), tb.millitm, wakeState);
                              printf("Value[0] = %f  \n", value[0]);
      */
      printf("\r%c", log[count], count);
      fflush(stdout);
      count = (count + 1) % 4;
    }

  out:

    rc = pxi6259_write_ao(chanOutFD[0], &ZERO_VOLT, 1);
    if (rc < 0)
    {
      fprintf(stderr, "Failed to write to AO channel reference: %u\n",
              outChanRef);
      error = 1;
      goto out;
    }
    /*
                    rc = pxi6259_write_ao(chanOutFD[1], &ZERO_VOLT, 1);
                    if (rc < 0) {
                        fprintf(stderr, "Failed to write to AO channel ON/OFF:
       %u\n", outChanOnOff); error = 1; goto out;
                    }
    */
    if (write(chanDigitFD, &OFF, 4) != 4)
    {
      fprintf(stderr, "Error writing OFF to port 0! Error: %s\n",
              strerror(errno));
      error = 1;
      goto out;
    }

    printf("Close open ADC fd\n");

    // stop AI segment
    if (pxi6259_stop_ai(devInFD))
    {
      fprintf(stderr, "Failed to stop data acquisition!\n");
    }

    // close file descriptors
    for (int i = 0; i < numChan; i++)
      close(chanInFD[i]);

    close(devInFD);

    // stop AO segment
    if (pxi6259_stop_ao(devOutFD))
    {
      fprintf(stderr, "Failed to stop generating signal!\n");
    }

    close(chanOutFD[0]);
    close(chanOutFD[1]);
    close(devOutFD);

    // close digital file descriptors
    close(chanDigitFD);
    close(devDigitFD);
  }

  if (sem_unlink("PauseControl") < 0)
    perror("Control sem_close");

  if (sem_unlink("WakeControl") < 0)
    perror("Control sem_close");
  return 0;
}
/**************************************************************************************
Software PXI chassis trigger via RTSI1 bus line.
This function is implemented in PXI 6259 device. One module of device must de
configure with TRIGGER_MODE in SW_RTSI1 the other modules im EXTERNAL_RSTI.
***************************************************************************************/

int PXIeTriggerRTSI1(int *devNums, int numChassis)
{

  pxi6259_dio_conf_t dioConfig[10];
  pxi6259_dio_conf_t dioConfig1;

  char filename[256];
  int devFD[10];

  uint32_t portMask1 = 0x3;
  int portFDs[10];

  uint8_t writeValue1 = 0x02;

  int i;

  for (i = 0; i < numChassis; i++)
  {
    // open DIO file descriptor
    sprintf(filename, "%s.%u.dio", DEVICE_FILE, devNums[i]);
    devFD[i] = open(filename, O_RDWR);
    if (devFD[i] < 0)
    {
      fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
      return -1;
    }

    // initialize DIO configuration
    dioConfig[i] = pxi6259_create_dio_conf();

    // configure DIO ports
    if (pxi6259_add_dio_channel(&dioConfig[i], DIO_PORT1, portMask1))
    {
      fprintf(stderr, "Failed to configure DIO port 0!\n");
      return -1;
    }

    if (pxi6259_load_dio_conf(devFD[i], &dioConfig[i]))
    {
      fprintf(stderr, "Loading DIO configuration failed!\n");
      return -1;
    }

    sprintf(filename, "%s.%u.dio.%u", DEVICE_FILE, devNums[i], 1);
    portFDs[i] = open(filename, O_RDWR | O_NONBLOCK);
    if (portFDs[i] < 0)
    {
      fprintf(stderr, "Failed to open port: %s %s\n", strerror(errno),
              filename);
      return -1;
    }
  }

  for (i = 0; i < numChassis; i++)
  {
    if (write(portFDs[i], &writeValue1, 4) != 4)
    {
      fprintf(stderr, "Error writing to port 1! Error: %s\n", strerror(errno));
    }
    writeValue1 = 0;
    if (write(portFDs[i], &writeValue1, 4) != 4)
    {
      fprintf(stderr, "Error writing to port 1! Error: %s\n", strerror(errno));
    }
    writeValue1 = 2;
  }

  for (i = 0; i < numChassis; i++)
  {
    close(portFDs[i]);
    close(devFD[i]);
  }

  return 1;
}

/**************************************************************************************/
/************* SWEEP WAVEFOR GENERATIO
 * ************************************************/
/**************************************************************************************/

extern "C" uint32_t
generateWaveformOnOneChannel_6368(int selectedCard, int channel, double offset,
                                  double level, int waverate, float *values,
                                  int nPoint, int softwareTrigger);
extern "C" uint32_t
generateWaveformOnOneChannel_6259(int selectedCard, int channel, double offset,
                                  double level, int waverate, float *values,
                                  int nPoint, int softwareTrigger);
extern "C" int stopWaveGeneration();

static void createWaveform(int number_of_samples, double offset, double level,
                           float *buf)
{
  int i;

  // create one complete triangular period in volts

  double m = 4. / number_of_samples;

  for (i = 0; i < number_of_samples / 4; i++)
    buf[i] = (float)(offset + level * m * i);

  for (i = number_of_samples / 4; i < 3 * number_of_samples / 4; i++)
    buf[i] = (float)(offset + level * (-m * i + 2));

  for (i = 3 * number_of_samples / 4; i < number_of_samples; i++)
    buf[i] = (float)(offset + level * (m * i - 4));
}

uint32_t generateWaveformOnOneChannel_6368(int selectedCard, int channel,
                                           double offset, double level,
                                           int waverate, float *values,
                                           int nPoint, int softwareTrigger)
{
  int silent = 0;
  int retval = 0;
  int number_of_channels = 1;
  int number_of_samples = 4000;
  const char *device_name = "/dev/PXIe-6368";
  sem_t *semWaveGen_id;

  uint32_t sampleRate;
  uint32_t update_period_divisor;

  sampleRate = number_of_samples * waverate;
  update_period_divisor = 100000000 / sampleRate;

  if (nPoint == 0)
  {
    printf("Generate default triangular waveform\n");
    number_of_samples = 4000;
  }
  else
  {
    printf("Generate EXPRESSION waveform\n");
    number_of_samples = nPoint;
  }

  sampleRate = number_of_samples * waverate;
  update_period_divisor = 100000000 / sampleRate;

  int ao_fd = 0;
  // int dev_fd = 0;
  int ao_chan_fd[number_of_channels];

  char str[100];
  int i = 0, k = 0;

  double pi;
  double radianPart;

  float *write_array[number_of_channels];
  xseries_ao_conf_t ao_conf;

  for (i = 0; i < number_of_channels; i++)
  {
    write_array[i] = (float *)calloc(1, sizeof(float) * number_of_samples);
  }

  // memset (write_array, 0, sizeof(write_array));

  /* open AO segment file descriptor */
  sprintf(str, "%s.%u.ao", device_name, selectedCard);
  ao_fd = open(str, O_RDWR);
  if (ao_fd <= 0)
  {
    if (!silent)
      printf("Could not open AO segment!\n");
    retval = -1;
    goto out_6368;
  }

  /* Stop the segment */
  xseries_stop_ao(ao_fd);

  /* reset AO segment */
  retval = xseries_reset_ao(ao_fd);
  if (retval)
  {
    if (!silent)
      printf("Error reseting card!\n");
    goto out_6368;
  }

  /*
   * Configure AO segment
   */

  /* Create AO configuration */

  ao_conf = xseries_continuous_regenerated_ao(number_of_samples);

  /* Disable external gating of the sample clock */
  retval = xseries_set_ao_external_gate(
      &ao_conf,
      XSERIES_AO_EXTERNAL_GATE_DISABLED, // No external pause signal
      XSERIES_AO_POLARITY_RISING_EDGE,   // Don't care
      0);                                // Disable
  if (retval)
  {
    if (!silent)
      printf("Error setting external gate!\n");
    goto out_6368;
  }

  if (softwareTrigger)
  {
    /* Program the START1 signal (start trigger) to assert from a software
     * rising edge */

    retval = xseries_set_ao_start_trigger(
        &ao_conf,
        XSERIES_AO_START_TRIGGER_SW_PULSE, // Set the line to software-driven
        XSERIES_AO_POLARITY_RISING_EDGE,   // Make line active on rising...
        1);                                //   ...edge (not high level)
    if (retval)
    {
      if (!silent)
        printf("Error setting start software trigger!\n");
      goto out_6368;
    }
  }
  else
  {

    /* Program the START1 signal (start trigger) to assert from a PFI1 rising
     * edge */

    retval = xseries_set_ao_start_trigger(
        &ao_conf,
        XSERIES_AO_START_TRIGGER_PFI1,   // Set the line to PFI1-driven
        XSERIES_AO_POLARITY_RISING_EDGE, // Make line active on rising...
        1);                              //   ...edge (not high level)
    if (retval)
    {
      if (!silent)
        printf("Error setting start PFI 1 trigger!\n");
      goto out_6368;
    }
  }

  /* Program the Update source */
  retval = xseries_set_ao_update_counter(
      &ao_conf,
      XSERIES_AO_UPDATE_COUNTER_UI_TC,  // Derive the clock line from the Update
                                        // Interval Terminal Count
      XSERIES_AO_POLARITY_RISING_EDGE); // Make the line active on rising edge
  if (retval)
  {
    if (!silent)
      printf("Error setting update counter!\n");
    goto out_6368;
  }

  /* Program the Update Interval counter */
  retval = xseries_set_ao_update_interval_counter(
      &ao_conf,
      XSERIES_OUTTIMER_UPDATE_INTERVAL_COUNTER_TB3, // Source the Update
                                                    // Interval from the
                                                    // internal timebase
      XSERIES_OUTTIMER_POLARITY_RISING_EDGE,        // Make the line active on rising
                                                    // edge
      update_period_divisor,                        // Number of clock intervals between successive
                                                    // updates
      2                                             // Number of clock intervals after the start trigger before the first
                                                    // update
  );
  if (retval)
  {
    if (!silent)
      printf("Error setting update interval counter!\n");
    goto out_6368;
  }

  /* Add channels */
  for (i = 0; i < number_of_channels; i++)
  {
    retval = xseries_add_ao_channel(&ao_conf, i, XSERIES_OUTPUT_RANGE_10V);
    if (retval)
    {
      if (!silent)
        printf("Cannot add AI channel %d to configuration!\n", i);
      goto out_6368;
    }
  }

  /* load configuration to the device */
  retval = xseries_load_ao_conf(ao_fd, ao_conf);
  if (retval)
  {
    if (!silent)
      printf("Cannot load AO configuration! %s (%d)\n", strerror(errno), errno);
    goto out_6368;
  }

  /* wait for the AO devices */
  sleep(1);

  if (nPoint == 0)
  {
    createWaveform(number_of_samples, offset, level, write_array[0]);
  }
  else
  {
    for (i = 0; i < nPoint; i++)
    {
      write_array[0][i] = offset + level * values[i];
      // printf( "Wave %d %f \n", i, write_array[0][i] );
    }
  }

  /* Open channels */
  for (i = 0; i < number_of_channels; i++)
  {
    sprintf(str, "%s.%u.ao.%u", device_name, selectedCard, i);
    ao_chan_fd[i] = open(str, O_RDWR | O_NONBLOCK);
    if (ao_chan_fd[i] < 0)
    {
      if (!silent)
        printf("Cannot add AO channel %d to configuration!\n", i);
      goto out_6368;
    }
  }

  /* Write samples */
  for (i = 0; i < number_of_samples; i++)
  {
    for (k = 0; k < number_of_channels; k++)
    {
      // Write is not blocking !!!
      retval = xseries_write_ao(ao_chan_fd[k], &write_array[k][i], 1);
      // printf("write_array[%d][%d] = %f\n", k, i, write_array[k][i]);
      if (retval == -1 && errno != EAGAIN)
      {
        if (!silent)
          printf("Error writing samples to FIFO buffer!\n");
        goto out_6368;
      }
    }
  }

  /* put segment in started state */
  retval = xseries_start_ao(ao_fd);
  if (retval)
  {
    printf("Cannot start AO segment! %s (%d)\n", strerror(errno), errno);
    goto out_6368;
  }

  if (softwareTrigger)
  {
    /* pulse start trigger */
    retval = xseries_pulse_ao(ao_fd, XSERIES_START_TRIGGER);
    if (retval)
    {
      printf("Error generating start pulse!\n");
      goto out_6368;
    }
  }

  {
    int semVal;

    semWaveGen_id = sem_open("WaveGen", O_CREAT, 0666, 0);
    if (semWaveGen_id == SEM_FAILED)
    {
      perror("WaveGen sem_open");
      return 0;
    }

    if (sem_getvalue(semWaveGen_id, &semVal) < 0)
    {
      perror("SemWaveGen sem_getvalue");
    }

    printf("Waveform generation START\n");
    for (int i = 0; i < semVal + 1; i++)
    {
      if (sem_wait(semWaveGen_id) < 0)
      {
        perror("sem_wait on semWaveGen_id");
      }
    }
  }

out_6368:

  /* stop the segment */
  xseries_stop_ao(ao_fd);

  /* close all used file descriptors */
  for (i = 0; i < number_of_channels; i++)
  {
    close(ao_chan_fd[i]);
  }

  /* reset segment */
  xseries_reset_ao(ao_fd);

  /* close segment's file descriptor */
  close(ao_fd);

  /* close card's file descriptor */
  // close(dev_fd);

  for (i = 0; i < number_of_channels; i++)
  {
    free(write_array[i]);
  }

  printf("Waveform generation STOP\n");

  return retval;
}

#define RESOURCE_NAME_DAQ "/dev/pxi6259"
// uint32_t generateWaveformOnOneChannel_6259(uint8_t selectedCard, uint8_t
// channel, double offset, double level, uint32_t waverate, float *values, int
// nPoint, int softwareTrigger)
uint32_t generateWaveformOnOneChannel_6259(int selectedCard, int channel,
                                           double offset, double level,
                                           int waverate, float *values,
                                           int nPoint, int softwareTrigger)
{
  uint32_t retval = 0;
  pxi6259_ao_conf_t ao_conf;

  int fdConfig = 0;
  int fdChannel = 0;

  uint32_t sampleRate;

  uint32_t periodDivisor;

  uint32_t number_of_samples = 1000;
  uint32_t i;
  char str[32];

  double pi;
  double radianPart;
  sem_t *semWaveGen_id;

  // printf("CARD %d\nChannel %d\nOffset %f\nLevel %f\nfreq %f\nnPoint
  // %d\nTrigger %d\n", selectedCard, channel, offset, level, waverate, nPoint,
  // softwareTrigger); printf("nPoint ==== %d\n", nPoint);

  if (nPoint == 0)
  {
    printf("Generate default triangular waveform\n");
    number_of_samples = 1000;
  }
  else
  {
    printf("Generate EXPRESSION waveform\n");
    number_of_samples = nPoint;
  }

  // printf("number_of_samples ==== %d\n", number_of_samples);

  float scaledWriteArray[number_of_samples];

  sampleRate = number_of_samples * waverate;
  periodDivisor = 20000000 / sampleRate;

  // printf("sampleRate ==== %d\n", sampleRate);
  // printf("periodDivisor ==== %d\n", periodDivisor);

  // get configuration file descriptor
  sprintf(str, "%s.%d.ao", RESOURCE_NAME_DAQ, selectedCard);
  fdConfig = open(str, O_RDWR | O_NONBLOCK);
  if (fdConfig < 0)
  {
    printf("Error Opening Device! fd: %d\n", fdConfig);
    return -1;
  }

  /*
          pi = 4.0 * atan(1.0);
          radianPart = 2*pi / number_of_samples;
          for (i=0; i<number_of_samples; i++) {
                  scaledWriteArray[i] = (float)(7 * sin((double) radianPart*i));
          }
  */

  if (nPoint == 0)
  {
    createWaveform(number_of_samples, offset, level, scaledWriteArray);
  }
  else
  {
    for (i = 0; i < nPoint; i++)
    {
      scaledWriteArray[i] = offset + level * values[i];
    }
  }

  //	retval = pxi6259_reset_ao(fdConfig);
  if (retval)
  {
    printf("err: reset ao. retval: %x\n", retval * -1);
    return -1;
  }

  ao_conf = pxi6259_create_ao_conf();

  if (pxi6259_set_ao_waveform_generation(&ao_conf, 1))
    return -1;

  if (pxi6259_add_ao_channel(&ao_conf, channel, AO_DAC_POLARITY_BIPOLAR))
    return retval;

  if (pxi6259_set_ao_count(&ao_conf, number_of_samples, 1, 1))
    return -1;

  if (pxi6259_set_ao_update_clk(&ao_conf, AO_UPDATE_SOURCE_SELECT_UI_TC,
                                AO_UPDATE_SOURCE_POLARITY_RISING_EDGE,
                                periodDivisor))
    return -1;

  if (softwareTrigger)
  {
    // Program the START1 signal (start trigger) to assert from a software
    // rising edge
    if (pxi6259_set_ao_attribute(&ao_conf, AO_START1_SOURCE_SELECT,
                                 AO_START1_SELECT_PULSE))
    {
      printf("Error setting start software trigger!\n");
      goto out_6259;
    }

    if (pxi6259_set_ao_attribute(&ao_conf, AO_START1_POLARITY_SELECT,
                                 AO_START1_POLARITY_RISING_EDGE))
    {
      printf("Error setting start software trigger!\n");
      goto out_6259;
    }
  }
  else
  {
    // Program the START1 signal (start trigger) to assert from a PFI1 rising
    // edge
    if (pxi6259_set_ao_attribute(&ao_conf, AO_START1_SOURCE_SELECT,
                                 AO_START1_SELECT_PFI1))
    {
      printf("Error setting start PFI1 trigger!\n");
      goto out_6259;
    }

    if (pxi6259_set_ao_attribute(&ao_conf, AO_START1_POLARITY_SELECT,
                                 AO_START1_POLARITY_RISING_EDGE))
    {
      printf("Error setting start PFI1 trigger!\n");
      goto out_6259;
    }
  }

  retval = pxi6259_load_ao_conf(fdConfig, &ao_conf);
  if (retval)
  {
    printf("err: load task. retval: %x\n", retval * -1);
    goto out_6259;
  }

  sleep(1);

  // Open channels
  sprintf(str, "%s.%d.ao.%d", RESOURCE_NAME_DAQ, selectedCard, channel);
  fdChannel = open(str, O_RDWR | O_NONBLOCK);
  if (fdChannel < 0)
  {
    printf("Error Opening Channel! FD: %d\n", fdChannel);
    return -1;
  }

  retval = pxi6259_write_ao(fdChannel, scaledWriteArray, number_of_samples);
  if (retval != number_of_samples)
  {
    printf("err: writing. retval: %d\n", retval);
    goto out_6259;
  }

  retval = pxi6259_start_ao(fdConfig);
  if (retval)
  {
    printf("err: Starting task. retval: %d\n", retval);
    return -1;
  }

  /*
          if(softwareTrigger)
          {
                  // pulse start trigger
                  retval = xseries_pulse_ao(ao_fd, XSERIES_START_TRIGGER);
                  if(retval) {
                          printf("Error generating start pulse!\n");
                          goto out_6368;
                  }
          }
  */
  {
    int semVal;

    semWaveGen_id = sem_open("WaveGen", O_CREAT, 0666, 0);
    if (semWaveGen_id == SEM_FAILED)
    {
      perror("WaveGen sem_open");
      return 0;
    }

    if (sem_getvalue(semWaveGen_id, &semVal) < 0)
    {
      perror("SemWaveGen sem_getvalue");
    }

    printf("Waveform generation START\n");
    for (int i = 0; i < semVal + 1; i++)
    {
      if (sem_wait(semWaveGen_id) < 0)
      {
        perror("sem_wait on semWaveGen_id");
      }
    }
  }

out_6259:
  if (pxi6259_stop_ao(fdConfig))
    printf("err: Stoping task. retval: %x\n", retval * -1);

  close(fdChannel);
  // sleep(1);

  //	retval = pxi6259_reset_ao(fdConfig);
  if (retval)
  {
    printf("err: reset ao. retval: %x\n", retval * -1);
    return -1;
  }
  close(fdConfig);

  /*
          retval = pxi6259_load_ao_conf(fdConfig, &ao_conf);
          if (retval) {
                  printf ("err: load task. retval: %x\n",retval * -1);
                  goto out_6259;
          }

          // Open channels
          sprintf(str,"%s.%d.ao.%d",RESOURCE_NAME_DAQ,selectedCard,channel);
          fdChannel = open(str, O_RDWR | O_NONBLOCK);
          if (fdChannel < 0) {
                  printf ("Error Opening Channel! FD: %d\n",fdChannel);
          }

          for( i = 0; i <  number_of_samples; i++)
              scaledWriteArray[i] = (float)0.;

          retval = pxi6259_write_ao(fdChannel, scaledWriteArray,
     number_of_samples); if (retval != number_of_samples) { printf ("err:
     writing. retval: %d\n",retval);
          }

          retval = pxi6259_start_ao(fdConfig);
          if (retval) {
                  printf ("err: Starting task. retval: %d\n",retval);
          }

          sleep(1);

          if (pxi6259_stop_ao(fdConfig))
                  printf ("err: Stoping task. retval: %x\n",retval * -1);

          close(fdChannel);
          close(fdConfig);
  */
  return 0;
}

int stopWaveGeneration()
{
  sem_t *semWaveGen_id;

  semWaveGen_id = sem_open("WaveGen", O_CREAT, 0666, 0);
  if (semWaveGen_id == SEM_FAILED)
  {
    perror("WaveGen sem_open");
    return 0;
  }

  if (sem_post(semWaveGen_id) < 0)
  {
    perror("sem_post semWaveGen");
    return 0;
  }

  // sem_close(semWaveGen_id);

  return 1;
}

////////////////////////////////////////////////////////////////////////////
//////////////////////////   LAZY TRIGGER STUFF   //////////////////////////
////////////////////////////////////////////////////////////////////////////


#define SAVECONV 0
#define SEGMENT_OP_BEGIN 1
#define SEGMENT_OP_UPDATE 2
#define SEGMENT_OP_PUT 3
#define SEGMENT_OP_MAKE 4
// DEBUG VARIABLES
int buffer_i = 0;
int delayed_i = 0;
int debug_i = 0;

// Support class for enqueueing storage requests, specific for NI6259 EV
// management
class SaveItemEV
{
  int operation; // BEGIN UPDATE PUT MAKE
  MDSplus::Data *startData;
  MDSplus::Data *endData;
  MDSplus::Data *dimData;
  MDSplus::Array *segData;
  //MDSplus::TreeNode *node;
  int nodeNid;
  SaveItemEV *nxt;

public:
  SaveItemEV(int operation, MDSplus::Data *startData, MDSplus::Data *endData,
             MDSplus::Data *dimData, MDSplus::Array *segData,
             MDSplus::TreeNode *node)
  {
    this->operation = operation;
    this->startData = startData;
    this->endData = endData;
    this->dimData = dimData;
    this->segData = segData;
    this->nodeNid = node->getNid();
    this->nxt = 0;
  }

  void setNext(SaveItemEV *itm) { nxt = itm; }

  SaveItemEV *getNext() { return nxt; }

  void save(Tree *tree)
  {

    TreeNode *node = new TreeNode(nodeNid, tree);

    switch (operation)
    {
    case SEGMENT_OP_BEGIN:
      try
      {
        // std::cout << "SAVE ITEM OP_BEGIN START:" << startData
        // << "  END: " << endData << "  DIM: " << dimData << std::endl;
        node->beginSegment(startData, endData, dimData, segData);
        MDSplus::deleteData(startData);
        MDSplus::deleteData(endData);
        MDSplus::deleteData(dimData);
      }
      catch (const MDSplus::MdsException &exc)
      {
        std::cout << "Error in BeginSegment: " << exc.what() << std::endl;
      }
      break;
    case SEGMENT_OP_MAKE:
      try
      {
        // std::cout << "SAVE ITEM OP_MAKE in NODE: " << node << std::endl;
        node->makeSegment(startData, endData, dimData, segData);
        MDSplus::deleteData(startData);
        MDSplus::deleteData(endData);
        MDSplus::deleteData(dimData);
        MDSplus::deleteData(segData);
      }
      catch (const MDSplus::MdsException &exc)
      {
        std::cout << "Error in MakeSegment: " << exc.what() << std::endl;
      }
      break;
    case SEGMENT_OP_UPDATE:
      try
      {
        // std::cout << "SAVE ITEM OP_UPDATE" << std::endl;
        node->updateSegment(startData, endData, dimData);
        MDSplus::deleteData(startData);
        MDSplus::deleteData(endData);
        MDSplus::deleteData(dimData);
      }
      catch (const MDSplus::MdsException &exc)
      {
        std::cout << "Error in UpdateSegment: " << exc.what() << std::endl;
      }
      break;
    case SEGMENT_OP_PUT:
      try
      {
        // std::cout << "SAVE ITEM OP_PUT" << std::endl;
        node->putSegment(segData, -1);
        MDSplus::deleteData(segData);
      }
      catch (const MDSplus::MdsException &exc)
      {
        std::cout << "Error in PutSegment: " << exc.what() << std::endl;
      }
      break;
    }
    delete node;
  }

}; // class SaveItemEV

extern "C" void *handleSaveEV(void *listPtr);

class SaveListEV
{
public:
  pthread_cond_t itemAvailable;
  pthread_t thread;
  bool threadCreated;
  SaveItemEV *saveHead, *saveTail;
  bool stopReq;
  pthread_mutex_t mutex;
  char *expName;
  int shot;

  SaveListEV(const char *expName, int shot)
  {
    int status = pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&itemAvailable, NULL);
    saveHead = saveTail = NULL;
    stopReq = false;
    threadCreated = false;
    printf("\n\n********** *%s*\n\n", expName);
    this->expName = new char[strlen(expName) + 1];
    strcpy(this->expName, expName);
    this->shot = shot;
  }
  void addItem(int operation, MDSplus::Data *startData, MDSplus::Data *endData,
               MDSplus::Data *dimData, MDSplus::Array *segData,
               MDSplus::TreeNode *node)
  {
    SaveItemEV *newItem =
        new SaveItemEV(operation, startData, endData, dimData, segData, node);
    pthread_mutex_lock(&mutex);
    if (saveHead == NULL)
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

    printf("\n\n********%s**\n", expName);
    Tree *tree = new Tree((const char *)expName, shot);

    while (true)
    {
      pthread_mutex_lock(&mutex);
      if (stopReq && saveHead == NULL)
      {
        pthread_mutex_unlock(&mutex);
        pthread_exit(NULL);
      }

      while (saveHead == NULL)
      {
        // std::cout << "SAVE LIST WAIT..." << std::endl;
        pthread_cond_wait(&itemAvailable, &mutex);
        // std::cout << "SAVE LIST CONDITION" << std::endl;
        if (stopReq && saveHead == NULL)
        {
          // std::cout << "SAVE LIST EXIT" << std::endl;

          pthread_mutex_unlock(&mutex);
          pthread_exit(NULL);
        }
      }
      SaveItemEV *currItem = saveHead;
      saveHead = saveHead->getNext();
      pthread_mutex_unlock(&mutex);
      currItem->save(tree);
      delete currItem;
    }
    delete tree;
  }

  void start()
  {
    pthread_create(&thread, NULL, handleSaveEV, (void *)this);
    threadCreated = true;
  }
  void stop()
  {
    stopReq = true;
    pthread_cond_signal(&itemAvailable);
    if (threadCreated)
    {
      pthread_join(thread, NULL);
      printf("SAVE THREAD TERMINATED\n");
    }
  }
};

extern "C" void *handleSaveEV(void *listPtr)
{
  SaveListEV *list = (SaveListEV *)listPtr;
  list->executeItems();

  std::cout << "HANDLE SAVE EV TERMINATED " << std::endl;
  return NULL;
}

extern "C" void startSaveEV(void **retList, const char *expName, int shot)
{
  SaveListEV *saveList = new SaveListEV(expName, shot);
  saveList->start();
  *retList = (void *)saveList;
}

extern "C" void stopSaveEV(void *listPtr)
{
  if (listPtr)
  {
    SaveListEV *list = (SaveListEV *)listPtr;
    list->stop();
    delete list;
  }
}

class BufferHandler
{
  char saveConv = SAVECONV; // Acquisition format flags 0 raw data 1 converted data
  size_t bufSize;
  float *buffer_f;
  short *buffer_s;
  
  size_t bufferIdx, oldestBufferIdx;
  unsigned long sampleCount;

protected:
  MDSplus::Tree *tree;
  MDSplus::TreeNode *rawNode;
  int dataNid;
  SaveList *saveList;

public:
  BufferHandler(MDSplus::Tree *tree, int dataNid,
                double maxDelay, double preTime, double baseFreq,
                SaveList *saveList)
  {
    this->tree = tree;
    this->dataNid = dataNid;
    this->bufSize = (preTime + maxDelay) * baseFreq;
    //printf("==== BUFFER SIZE : %d =====", bufSize);
    if (saveConv){
      this->buffer_f = new float[this->bufSize];
    }
    else{
      this->buffer_s = new short[this->bufSize];
    }
    this->bufferIdx = this->oldestBufferIdx = 0;
    this->sampleCount = 0;
    this->saveList = saveList;
  }
  ~BufferHandler() { 
  delete[] buffer_f;
  delete[] buffer_s;
  }

  void processSample(float sample)
  {
    // std::cout << "CALLING FLOAT PROCESS SAMPLE" << std::endl;
    buffer_f[bufferIdx] = sample;
    sampleCount++; // fill the buffer before elaborating it
    if (sampleCount >= bufSize - 1)
    {
      processSampleDelayed(buffer_f[oldestBufferIdx]);
      oldestBufferIdx = (oldestBufferIdx + 1) % bufSize;
    }
    bufferIdx = (bufferIdx + 1) % bufSize;
  }

  void processSample(void* sample)
  {
    void* ptr;
    // std::cout << "PROCESS SHORT SAMPLE" << std::endl;
    if (!saveConv)
      buffer_s[bufferIdx] = *reinterpret_cast<short*>(sample);
    else
      buffer_f[bufferIdx] = *reinterpret_cast<double*>(sample);

    sampleCount++; // fill the buffer before elaborating it
    if (sampleCount >= bufSize - 1)
    {
      if (!saveConv)
        ptr = &buffer_s[oldestBufferIdx];
      else
        ptr = &buffer_f[oldestBufferIdx];
      processSampleDelayed(ptr);
      oldestBufferIdx = (oldestBufferIdx + 1) % bufSize;
    }
    bufferIdx = (bufferIdx + 1) % bufSize;
  }

  void terminate()
  {
    if (sampleCount > bufSize)
    {
      for (size_t i = 0; i < bufSize - 2; i++)
      {
        if (saveConv){
          // std::cout << "TERMINATING FLOAT SAMPLE DELAYED" << std::endl;
          processSampleDelayed(&buffer_f[oldestBufferIdx]);
        }
        else{
          // std::cout << "TERMINATING SHORT SAMPLE DELAYED" << std::endl;
          processSampleDelayed(&buffer_s[oldestBufferIdx]);
        }
        oldestBufferIdx = (oldestBufferIdx + 1) % bufSize;
      }
      flushBuffer();
    }
  }

  virtual void processSampleDelayed(float sample)
  {
    return;
  }
  virtual void processSampleDelayed(void* sample)
  {
    return;
  }
  virtual void trigger(double trigTime) = 0;
  virtual void flushBuffer() = 0;
};

class ClockBufferHandler : public BufferHandler
{
  double *segBuffer_f;
  short *segBuffer_s;
  MDSplus::Array *initSegData;
  size_t bufIdx;
  char saveConv = SAVECONV;
  int f12Div[2], currDivIdx, divSamples[2];
  double baseFreq;
  int segBufSize, segmentSize, numSegments, numBuffersInSegment, bufferCount, shotSampleCount, switchBufCount;
  double startTime;
  double preTime, postTime;
  long baseSampleCount, currBaseSampleCount, sampleCount;
  unsigned long segBufSampleCount;
  double basePeriod;
  double bufStartTime;
  double minPeriod;
  std::vector<double> bufStartTimes;
  std::vector<double> bufEndTimes;
  std::vector<double> bufPeriods;
  std::vector<double> bufPeriodsResampled;
  std::vector<double> switchTimes;
  bool freqSwitched;
  int clockNid, resampleNid;
  int shot, numCoeffs;
  float curSampleDivider;
  float *coeffs;
  MDSplus::Data *startTimeData;
  MDSplus::Data *endTimeData;
  MDSplus::Data *dimData;
  MDSplus::Data *dimDataResampled;

public:
  ClockBufferHandler(MDSplus::Tree *tree, int dataNid,
                     double maxDelay, int f1Div, int f2Div, double baseFreq,
                     int segBufSize, int segmentSize, double startTime,
                     double preTime, double postTime, SaveList *saveList, int clockNid, int shot, float *coeffs,
                     int numCoeffs, int resampleNid)
      : BufferHandler(tree, dataNid, maxDelay, preTime, baseFreq, saveList)
  {
    this->basePeriod = 1. / baseFreq;
    this->baseFreq = baseFreq;
    this->segBufSize = segBufSize;
    this->segBuffer_f = new double[segBufSize];
    this->segBuffer_s = new short[segBufSize];
    this->numBuffersInSegment = segmentSize / segBufSize;
    this->segmentSize = segBufSize * this->numBuffersInSegment;
    this->f12Div[0] = f1Div;
    this->f12Div[1] = f2Div;
    this->divSamples[0] = 0;
    this->divSamples[1] = 0;
    this->f12Div[1] = f2Div;
    if (f1Div > f2Div)
      this->minPeriod = basePeriod * f2Div;
    else
      this->minPeriod = basePeriod * f1Div;
    this->currDivIdx = 0;
    this->startTime = startTime;
    this->preTime = preTime;
    this->postTime = postTime;
    this->baseSampleCount = this->segBufSampleCount = this->sampleCount = 0;
    this->currBaseSampleCount = -1;
    this->bufStartTime = startTime;
    double bufEndTime = startTime + segmentSize * (this->basePeriod * f12Div[0]);
    bufStartTimes.push_back(startTime);
    bufEndTimes.push_back(bufEndTime);
    bufPeriods.push_back(this->basePeriod * f12Div[0]);
    this->bufferCount = 0;
    this->freqSwitched = false;
    this->shotSampleCount = 0;
    this->clockNid = clockNid;
    this->shot = shot;
    this->coeffs = coeffs;
    this->resampleNid = resampleNid;
    this->numCoeffs = numCoeffs;
    this->numSegments = 0;
    this->switchBufCount = 0;
  }
  ~ClockBufferHandler()
  {
    delete[] segBuffer_f;
    delete[] segBuffer_s;
    MDSplus::deleteData(initSegData);
  }

  void processSampleDelayed(void* sample)
  {
    // Check frequency switch
    double currTime = startTime + basePeriod * baseSampleCount;
    baseSampleCount++;
    currBaseSampleCount++;
    // std::cout << "CurrBase Sample Count: " << currBaseSampleCount<< std::endl;

    if (currBaseSampleCount % f12Div[currDivIdx] == 0)
    {
      divSamples[currDivIdx]++;
      if (!saveConv)
        segBuffer_s[segBufSampleCount++] = *reinterpret_cast<short*>(sample);
      else
        segBuffer_f[segBufSampleCount++] = *reinterpret_cast<double*>(sample);



      if (segBufSampleCount >= segBufSize) // buffer filled
      {
        // std::cout << "STO SALVANDO BUFFER CON COUNTER: " << shotSampleCount << std::endl;
        // std::cout << "BUFFER FILLED FOR: " <<  dataNid << std::endl;

        int sampleToRead = segmentSize - bufferCount * segBufSize;              // how many samples can still be saved before filling the current segment

        if (!saveConv){
          saveList->addItem(segBuffer_s, segBufSize, sampleToRead, 1, segmentSize, 
          shotSampleCount, dataNid, clockNid, startTime, tree, shot, 0, nullptr, 0, 0, bufPeriods[bufPeriods.size()-1], 1., 
          coeffs, numCoeffs, resampleNid);
          segBuffer_s = new short[segmentSize];
          }
        else{  
          saveList->addItem(segBuffer_f, segBufSize, sampleToRead, 2, segmentSize, 
          shotSampleCount, dataNid, clockNid, startTime, tree, shot, 0, nullptr, 0, 0, bufPeriods[bufPeriods.size()-1], 1., 
          coeffs, numCoeffs, resampleNid);
          segBuffer_f = new double[segmentSize];
          }

        sampleCount = 0;                                        // reinitializing the counter of the samples read in the current buffer
        bufferCount++;
        segBufSampleCount = 0;

        if (freqSwitched == true){
          curSampleDivider = double(f12Div[currDivIdx]) / f12Div[(currDivIdx + 1) % 2];
          saveList->addItem(dataNid, tree, startTimeData, endTimeData, dimData, dimDataResampled, resampleNid);
          // std:: cout << "switchBufCount: " << switchBufCount << ", curSampleDivider: "  << curSampleDivider << ", primo termine: "<< (switchBufCount) * 1 / curSampleDivider << ", secodno termine" << (segBufSize -  switchBufCount)* curSampleDivider <<  std::endl;
// std::cout << "SHOT SAMPLE COUNT PRIMA: " << shotSampleCount << std::endl;
          shotSampleCount += (switchBufCount-1) * 1 / curSampleDivider + (segBufSize -  switchBufCount);
          bufPeriodsResampled.clear();
// std::cout << "SHOT SAMPLE COUNT DOPO: " << shotSampleCount << std::endl;    
          
        }
        else
        shotSampleCount += segBufSize;

        if (bufferCount >= numBuffersInSegment) // Need to possibly adjust segment end and
                                                // dimension and create a new segment
        {
          //std::cout << "SEGMENT FILLED FOR " << dataNid << std:: endl;
          numSegments ++;
          bufStartTime = startTime + basePeriod * baseSampleCount;
          // std::cout << "BUF_START_TIME: " << bufStartTime << " startTime: " << startTime << " basePeriod: " << basePeriod << " f12Div[currDivIdx]: " << f12Div[currDivIdx] << " baseSampleCount: " << baseSampleCount << std::endl;
          double bufEndTime = bufStartTime + (segmentSize)*basePeriod * f12Div[currDivIdx];

          bufferCount = 0;
          bufStartTimes.clear();
          bufStartTimes.push_back(bufStartTime);
          bufEndTimes.clear();
          bufEndTimes.push_back(bufEndTime);
          bufPeriods.clear();
          bufPeriods.push_back(basePeriod * f12Div[currDivIdx]);
          
        }
        freqSwitched = false;
      }
    }

    // std::cout << "NOT FILLED" << std::endl;
    if (switchTimes.size() > 0 &&
        switchTimes[0] <= currTime) // frequencySwitched
    {
      std::cout << "FREQUENCY SWITCH at:" << currTime << ", with segBufSampleCount: " << segBufSampleCount << std::endl;
      
      // Handling the counter for the Asynch store manager library
      curSampleDivider = double(f12Div[currDivIdx]) / f12Div[(currDivIdx + 1) % 2];
      switchBufCount = segBufSampleCount;
// std::cout << "SHOT SAMPLE COUNT PRIMA" << shotSampleCount << std::endl;
      shotSampleCount *= curSampleDivider;      
// std::cout << "SHOT SAMPLE COUNT DOPO" << shotSampleCount << std::endl;    

      currDivIdx = (currDivIdx + 1) % 2;

      bufPeriods.push_back(basePeriod * f12Div[currDivIdx]);
      if (currBaseSampleCount % f12Div[currDivIdx] == 0) // A sample at previous frequency has been written at this time
      {
        currBaseSampleCount = 0; // wait a period for the new frequency before saving sample
        bufStartTimes.push_back(switchTimes[0] + basePeriod * f12Div[currDivIdx]);
        bufEndTimes[bufEndTimes.size() - 1] = switchTimes[0] + minPeriod / 2.;
      }
      else
      {
        currBaseSampleCount = -1;                                                      // Next sample is being written
        bufStartTimes.push_back(switchTimes[0] + 2 * basePeriod * f12Div[currDivIdx]); 
        bufEndTimes[bufEndTimes.size() - 1] = switchTimes[0] - minPeriod / 2.;
      }
      // std::cout << "STAMPONE!!! BUFFER COUNT:  " << bufferCount << ", segBufSize: " << segBufSize << ", segBufSampleCount: " << segBufSampleCount << ", basePeriod: " << basePeriod << ", switchTimes[0]: " << switchTimes[0] << ", f12Div[currDivIdx]: " << f12Div[currDivIdx]<< std::endl;
      bufEndTimes.push_back(
          switchTimes[0] + (segmentSize - (bufferCount * segBufSize + segBufSampleCount) - 1) * (basePeriod * f12Div[currDivIdx]) - (basePeriod * f12Div[currDivIdx] / 2.));
      freqSwitched = true;
      switchTimes.erase(switchTimes.begin());

      // USARE UPDATE MINMAX

      startTimeData = new MDSplus::Float64(bufStartTimes[bufStartTimes.size() - 1]);
      endTimeData =
          new MDSplus::Float64(bufEndTimes[bufEndTimes.size() - 1]);
      std::cout << "DEBUG -> END TIME DATA: " << bufEndTimes[bufEndTimes.size() - 1] << std::endl;
      MDSplus::Data *startTimesData =
          new MDSplus::Float64Array(bufStartTimes.data(), bufStartTimes.size());
      MDSplus::Data *endTimesData =
          new MDSplus::Float64Array(bufEndTimes.data(), bufEndTimes.size());
      MDSplus::Data *periodsData =
          new MDSplus::Float64Array(bufPeriods.data(), bufPeriods.size());
      dimData =
          MDSplus::compileWithArgs("build_range($, $, $)", tree, 3,
                                   startTimesData, endTimesData, periodsData);
    
      // Resampling handling
      for (int z = 0; z < bufPeriods.size(); z++){
        bufPeriodsResampled.push_back(bufPeriods[z] * 50);
      }

      // std::cout << "bufPeriods.size(): " <<bufPeriods.size() << ", bufPeriods[0]" << bufPeriods[0] * 50 <<std::endl;
      // std::cout << "bufPeriodsResampled.size(): " <<bufPeriodsResampled.size()  <<std::endl; // << ", bufPeriodsResampled[0]" << bufPeriodsResampled[0]
      // std::cout << " bufPeriodsResampled[0]: " << bufPeriodsResampled.data() << std::endl;

      MDSplus::Data *periodsDataResampled =
          new MDSplus::Float64Array(bufPeriodsResampled.data(), bufPeriodsResampled.size());
      
      dimDataResampled = MDSplus::compileWithArgs("build_range($, $, $)", tree, 3,
                                   startTimesData, endTimesData, periodsDataResampled);
            
      // std::cout << " dimDataResampled: " << dimDataResampled << std::endl;

      // saveList->addItem(dataNid, tree, startTimeData, endTimeData, dimData, resampleNid);

      // std::cout << "UPDATE start: " << startTimesData << std::endl;
      // std::cout << "UPDATE end: " << endTimesData << std::endl;
      // std::cout << "UPDATE dim: " << dimData << std::endl;

      MDSplus::deleteData(periodsData);
      MDSplus::deleteData(startTimesData);
      MDSplus::deleteData(endTimesData);
    }
  }

  virtual void trigger(double trigTime)
  {
    std::cout << "TRIGGER AT: " << trigTime << std::endl;
    double startTime_trig = trigTime - preTime;
    if (startTime_trig < startTime){ // checking if valid trigger
      std::cout << "WARNING: TRIGGER WINDOW START VALUE IS IN THE PAST, SETTING IT TO EXPERIMENT START TIME" << std::endl;
      startTime_trig = startTime;
    }
    if (switchTimes.size() == 0 ||  switchTimes[switchTimes.size() - 1] <= startTime_trig)
      switchTimes.push_back(startTime_trig);
    else
    {
      size_t idx;
      for (idx = switchTimes.size() - 1;
            idx > 0 && switchTimes[idx] >= startTime_trig; idx--)
        ;
      switchTimes.insert(switchTimes.begin() + idx, startTime_trig);
    }
  

    double endTime_trig = trigTime + postTime;
    if (switchTimes[switchTimes.size() - 1] <= endTime_trig)
      switchTimes.push_back(endTime_trig);
    else
    {
      size_t idx;
      for (idx = switchTimes.size() - 1; idx > 0 && switchTimes[idx] >= endTime_trig;
          idx--)
        ;
      switchTimes.insert(switchTimes.begin() + idx, endTime_trig);
    }
  }

  virtual void flushBuffer()
  {
    
    if (segBufSampleCount % 2 != 0)
      segBufSampleCount -=1;
    //std::cout << "FLUSH BUFFER with " << segBufSampleCount << " samples left" <<  std::endl;
    if (this->saveConv){
    int sampleToRead = segmentSize - bufferCount*segBufSize - segBufSampleCount;
       saveList->addItem(segBuffer_f, segBufSampleCount - 1 , sampleToRead, 2, segmentSize, 
        shotSampleCount, dataNid, clockNid, startTime, tree, shot, 0, nullptr, 0, 0, basePeriod, 1., 
        coeffs, numCoeffs);
    }
    else{
      int sampleToRead = segmentSize - bufferCount*segBufSize - segBufSampleCount;
       saveList->addItem(segBuffer_s, segBufSampleCount -1 , sampleToRead, 1, segmentSize, 
        shotSampleCount, dataNid, clockNid, startTime, tree, shot, 0, nullptr, 0, 0, basePeriod, 1., 
        coeffs, numCoeffs);
    }
      
  }
};

class BurstBufferHandler : public BufferHandler
{
  int saveConv = SAVECONV;
  double *segBuffer_f;
  short *segBuffer_s;
  std::vector<double> startTimes;
  int freqDiv, clockNid, shot, numCoeffs, resampleNid;
  double baseFreq;
  int segmentSize, bufSize, bufCount, sampleCount, burstCount, windowSize, windowCount;
  double startTime;
  double basePeriod;
  double segStart;
  double preTime;
  double postTime;
  bool inBurst;
  unsigned long baseSampleCount, currBaseSampleCount;
  float *coeffs;
  float trigTime;

public:
  BurstBufferHandler(MDSplus::Tree *tree, int dataNid,
                     double maxDelay, int freqDiv, double baseFreq,
                     double startTime, double preTime, double postTime,
                     int segmentSize, int bufSize, SaveList *saveList, int clockNid, int shot, float *coeffs,
                     int numCoeffs, int resampleNid)
      : BufferHandler(tree, dataNid, maxDelay, preTime, baseFreq, saveList)
  {
    this->preTime = preTime;
    this->postTime = postTime;
    this->baseFreq = baseFreq;
    this->basePeriod = 1. / baseFreq;
    this->windowSize = (postTime + preTime) * this->baseFreq / freqDiv;
    this->segmentSize = segmentSize;
    if (this->segmentSize > this->windowSize)
      this->segmentSize = this->windowSize;
    this->bufSize = bufSize;
    this->segBuffer_f = new double[this->segmentSize];
    this->segBuffer_s = new short[this->segmentSize];
    this->freqDiv = freqDiv;
    this->startTime = startTime;
    this->inBurst = false;
    this->baseSampleCount = 0;
    this->clockNid = clockNid;
    this->shot = shot;
    this->coeffs = coeffs;
    this->numCoeffs = numCoeffs;
    this->resampleNid = resampleNid;
    this->sampleCount = 0;
    this->bufCount = 0;
  }
  ~BurstBufferHandler()
  {
    std::cout << "DISTRUTTORE BURST" << std::endl;
    delete[] segBuffer_f;
    delete[] segBuffer_s;
    std::cout << "DISTRUTTO" << std::endl;
  }

  virtual void processSampleDelayed(void* sample)
  {
    // Check whether the burst window has been reached
    double currTime = startTime + basePeriod * baseSampleCount;

    // Updating timing counters
    baseSampleCount++;    
    currBaseSampleCount++;

    if (startTimes.size() > 0 && startTimes[0] <= currTime)
    {
      printf("\n\nprocessSampleDelayed startTimes[0] %f \n", startTimes[0]);
      
      if (currTime >= startTimes[0] + postTime)
      {
        printf("WARNING! Trigger time in the past -> Saving the current window data.");
      }

      if (!inBurst) // Trigger considered only if not serving a previous burst
      {
        inBurst = true;
        currBaseSampleCount = 0;
        burstCount = 0; // counts how many samples have been read in the burst
        windowCount = 0; // traces the current position on the burst window
        trigTime = startTimes[0];
      }
      startTimes.erase(startTimes.begin());
    }
    if (inBurst && (currBaseSampleCount % freqDiv == 0))
    {
      if (!saveConv)
        segBuffer_s[sampleCount++] = *reinterpret_cast<short*>(sample);
      else 
        segBuffer_f[sampleCount++] = *reinterpret_cast<double*>(sample);
      windowCount++;

      // BUFFER FILLED, NEED TO SAVE IT
      if (sampleCount >= bufSize)
      {
        //std::cout << "BUFFER FILLED FOR: " << dataNid << std::endl;

        int sampleToRead = segmentSize - bufCount;              // how many samples can still be saved before filling the current segment
        // std::cout << "sampleToRead: " << sampleToRead << ", startTime: " << segStart << ", basePeriod: " << basePeriod << std::endl;
        // std::cout << "SAVING SEGMENT FOR " << dataNid << std::endl;

        if (!saveConv){
          saveList->addItem(segBuffer_s, bufSize, sampleToRead, 1, segmentSize, 
          burstCount, dataNid, clockNid, trigTime, tree, shot, 0, nullptr, 0, 0, basePeriod * freqDiv, 1., 
          coeffs, numCoeffs, resampleNid);
          segBuffer_s = new short[segmentSize];                   // reinitializing the samples buffer to read the next data
        }
        else{
          saveList->addItem(segBuffer_f, bufSize, sampleToRead, 2, segmentSize, 
          burstCount, dataNid, clockNid, trigTime, tree, shot, 0, nullptr, 0, 0, basePeriod * freqDiv, 1., 
          coeffs, numCoeffs, resampleNid);
          segBuffer_f = new double[segmentSize];                   // reinitializing the samples buffer to read the next data
        }

        bufCount += bufSize;                                    // updating the number of samples in the current segment 
        burstCount += bufSize;                                  // updating the total number of samples read
        sampleCount = 0;                                        // reinitializing the counter of the samples read in the current buffer

        if (windowCount >= windowSize)                          // burst window finished
          inBurst = false;

        else if (bufCount == segmentSize)                       // segment filled, reinitializing the counter of the samples in the current segment
          bufCount = 0;
      }

      
      else if (windowCount > windowSize) // Last piece of burst
      {
        // std::cout << "inBurst False, windowCount: " << windowCount << ", windowSize: " << windowSize << std::endl;
        int sampleToRead = segmentSize - sampleCount;
        // std::cout << "sampleToRead: " << sampleToRead << ", startTime: " << segStart << ", basePeriod: " << basePeriod << std::endl;
        std::cout << "SAVING LAST DATA FOR " << dataNid << std::endl;

        if (!saveConv){
          saveList->addItem(segBuffer_s, sampleCount, sampleToRead, 1, segmentSize, 
          burstCount, dataNid, clockNid, trigTime, tree, shot, 0, nullptr, 0, 0, basePeriod, 1., 
          coeffs, numCoeffs, resampleNid);
          segBuffer_s = new short[segmentSize];
        }
        else{
          saveList->addItem(segBuffer_f, sampleCount, sampleToRead, 2, segmentSize, 
          burstCount, dataNid, clockNid, trigTime, tree, shot, 0, nullptr, 0, 0, basePeriod, 1., 
          coeffs, numCoeffs, resampleNid);
          segBuffer_f = new double[segmentSize];
        }
        

        inBurst = false;
      }
    }
  }

  
  
  virtual void trigger(double trigTime)
  {
    double currTime = startTime + basePeriod * baseSampleCount;
    std::cout << "TRIGGER AT: " << trigTime << " PRE TIME " << preTime << " CURR TIME " << currTime << std::endl;
    double startTime = trigTime - preTime;
    size_t idx;
    if (startTimes.size() == 0 ||
        startTimes[startTimes.size() - 1] <= startTime)
      startTimes.push_back(startTime);
    else
    {
      size_t idx;
      for (idx = startTimes.size() - 1; idx > 0 && startTimes[idx] >= startTime;
           idx--)
        ;
      startTimes.insert(startTimes.begin() + idx, startTime);
    }
  }
  virtual void flushBuffer() { std::cout << "FLUSH BUFFERS" << std::endl; }
};

// Eventhandler class waits for the %name event and triggers the associated bufferHandler sending
//the event recieving time
class EventHandler : public Event
{
  BufferHandler *bufHandler;

public:
  EventHandler(char *name, BufferHandler *buf) : Event(name)
  {
    this->bufHandler = buf;
  }
  void run()
  {
    int bufSize;
    MDSplus::Data *evData = getData(); // Get raw data
    double triggerTime = evData->getDouble();
    std::cout << "RECEIVED EVENT " << getName() << " WITH DATA  " << evData
              << "\n";
    bufHandler->trigger(triggerTime);
    MDSplus::deleteData(evData);
  }
};

#define ADC_BUFFER_SIZE 1000
int pxi6259EV_readAndSaveAllChannels(
    int nChan, void *chanFdPtr, int *isBurst, int *f1Div, int *f2Div,
    double maxDelay, double baseFreq, double *preTimes, double *postTimes,
    double startTime, int bufSize, int segmentSize, char **eventNames,
    void *dataNidPtr, void *treePtr, void *saveListPtr, void *stopAcq)
{
  int chan;
  SaveListEV *saveList = (SaveListEV *)saveListPtr;
  int *chanFd = (int *)chanFdPtr;
  int *dataNid = (int *)dataNidPtr;
  BufferHandler **bufferHandlers;
  EventHandler **eventHandlers;
  MDSplus::TreeNode **treeNodes;

  treeNodes = new MDSplus::TreeNode *[nChan];
  // Delete first all data nids
  for (int i = 0; i < nChan; i++)
  {
    try
    {
      treeNodes[i] = new TreeNode(dataNid[i], (Tree *)treePtr);
      treeNodes[i]->deleteData();
    }
    catch (const MdsException &exc)
    {
      printf("Error deleting data nodes\n");
    }
  }

  (*(int *)stopAcq) = 0;

  bufferHandlers = new BufferHandler *[nChan];
  memset(bufferHandlers, 0, sizeof(BufferHandler *) * nChan);
  eventHandlers = new EventHandler *[nChan];
  memset(eventHandlers, 0, sizeof(EventHandler *) * nChan);
  for (chan = 0; chan < nChan; chan++)
  {
    // if (isBurst[chan])
    //   bufferHandlers[chan] = new BurstBufferHandler(
    //       (MDSplus::Tree *)treePtr, treeNodes[chan], maxDelay, f1Div[chan],
    //       baseFreq, startTime, preTimes[chan], postTimes[chan], segmentSize,
    //       saveList);
    // else
    //   bufferHandlers[chan] = new ClockBufferHandler(
    //       (MDSplus::Tree *)treePtr, treeNodes[chan], maxDelay, f1Div[chan],
    //       f2Div[chan], baseFreq, bufSize, segmentSize, startTime,
    //       preTimes[chan], postTimes[chan], saveList);
    if (eventNames[chan][0]) // Empty string is passed for no event
    {
      eventHandlers[chan] =
          new EventHandler(eventNames[chan], bufferHandlers[chan]);
      eventHandlers[chan]->start();
    }
    else
      eventHandlers[chan] = NULL;
  }
  while (!(*(int *)stopAcq))
  {
    float buffer[ADC_BUFFER_SIZE];
    for (chan = 0; chan < nChan; chan++)
    {
      int currReadSamples = read(chanFd[chan], buffer, ADC_BUFFER_SIZE * 2);
      if (currReadSamples <= 0)
      {
        if (errno == EAGAIN || errno == ENODATA)
        {
          usleep(50);
          currReadSamples = 0; // No data currently available... Try again
                               // continue;
        }
        else
        {
          if (errno == EOVERFLOW)
          {
            printf("PXI 6259 Error reading samples on ai%d: (%d) %s \n", chan,
                   errno, strerror(errno));
            for (chan = 0; chan < nChan; chan++)
            {
              if (eventHandlers[chan])
              {
                eventHandlers[chan]->stop();
                delete eventHandlers[chan];
              }
              delete bufferHandlers[chan];
            }
            delete[] eventHandlers;
            delete[] bufferHandlers;
            return -2;
          }
        }
      }
      else
      {
        for (int sampleIdx = 0; sampleIdx < currReadSamples / sizeof(short);
             sampleIdx++)
        {
          bufferHandlers[chan]->processSample(buffer[sampleIdx]);
          //		    std::cout << buffer[sampleIdx] << std::endl;
        }
      }
    }
  }

  for (chan = 0; chan < nChan; chan++)
  {
    if (eventHandlers[chan])
    {
      std::cout << "STOPPING EVENT HANDLER...." << std::endl;
      eventHandlers[chan]->stop();
      std::cout << "STOPPED" << std::endl;
      delete eventHandlers[chan];
    }
    //std::cout << "TERMINATING BUFFER HANDLER...." << std::endl;
    bufferHandlers[chan]->terminate();
    //std::cout << "TERMINATED" << std::endl;
  }
  saveList->stop();

  std::cout << "DELETING eventHandlers" << std::endl;
  delete[] eventHandlers;
  std::cout << "DELETING bufferHandlers" << std::endl;
  //    for( chan = 0; chan < nChan; chan++ )
  //	delete bufferHandlers[chan];
  delete[] bufferHandlers;
  printf("STOP PXI 6259 Acquisition \n");
  return 1;
}

int pxi6368EV_readAndSaveAllChannels(
    int aiFd, int nChan, void *chanMapPtr, void *chanFdPtr, int *isBurst, int *f1Div, int *f2Div,
    double maxDelay, double baseFreq, double *preTimes, double *postTimes,
    double startTime, int bufSize, int segmentSize, char **eventNames,
    void *dataNidPtr, void *treePtr, void *saveListPtr, int clockNid, int shot, int *resNids, 
    void *coeffsNidPtr, void *gainsPtr,  void *stopAcq)
{

  char saveConv = SAVECONV; // Acquisition format flags 0 raw data 1 convrted data

  int sampleToRead = 0; // Number of sample to read
  int currDataToRead = 0; // Number of current sample to read

  int chan;
  int currReadSamples; // Number of samples read
  
  
  SaveList *saveList = (SaveList *)saveListPtr;
  float *gains = (float *)gainsPtr;
  int *coeffsNid = (int *)coeffsNidPtr;
  int *resampledNid = (int *)resNids; // Channel node identifier
  int *chanMap = (int *)chanMapPtr;
  int *chanFd = (int *)chanFdPtr;
  int *dataNid = (int *)dataNidPtr;
  BufferHandler **bufferHandlers;
  EventHandler **eventHandlers;
  MDSplus::TreeNode **treeNodes;
  treeNodes = new MDSplus::TreeNode *[nChan];

  float *coeffs[nChan];
  int numCoeffs[nChan];
  // Delete first all data nids
  for (int i = 0; i < nChan; i++)
  {
    try
    {
      treeNodes[i] = new TreeNode(dataNid[i], (Tree *)treePtr);
      treeNodes[i]->deleteData();
    }
    catch (const MdsException &exc)
    {
      printf("Error deleting data nodes\n");
    }

  }

// std::cout<< "READ AND SAVE 1 " << std::endl;


  (*(int *)stopAcq) = 0;
  // std::cout<< "READ AND SAVE 1.1 " << std::endl;
  bufferHandlers = new BufferHandler *[nChan];
  memset(bufferHandlers, 0, sizeof(BufferHandler *) * nChan);
  eventHandlers = new EventHandler *[nChan];
  memset(eventHandlers, 0, sizeof(EventHandler *) * nChan);
  
  for (chan = 0; chan < nChan; chan++)
  {
    try
    {
      TreeNode *rangeNode =
          new TreeNode(coeffsNid[chan], (MDSplus::Tree *)treePtr);
      Data *rangeData = rangeNode->getData();
      coeffs[chan] = rangeData->getFloatArray(&numCoeffs[chan]);
      deleteData(rangeData);
    }
    catch (const MdsException &exc)
    {
      printf("%s\n", exc.what());
    }
    if (isBurst[chan])
    {
      bufferHandlers[chan] = new BurstBufferHandler(
          (MDSplus::Tree *)treePtr, dataNid[chan], maxDelay, f1Div[chan],
          baseFreq, startTime, preTimes[chan], postTimes[chan], segmentSize, bufSize,
          saveList, clockNid, shot, coeffs[chan], numCoeffs[chan], resampledNid[chan]);
    }
    else
      // Clock buffers for the channels indicating "CONTINUOUS" or "DUAL SPEED"
      bufferHandlers[chan] = new ClockBufferHandler(
          (MDSplus::Tree *)treePtr, dataNid[chan], maxDelay, f1Div[chan],
          f2Div[chan], baseFreq, bufSize, segmentSize, startTime,
          preTimes[chan], postTimes[chan], saveList, clockNid, shot, coeffs[chan], numCoeffs[chan], resampledNid[chan]);
    if (eventNames[chan][0]) // Empty string is passed for no event
    {
      eventHandlers[chan] = new EventHandler(eventNames[chan], bufferHandlers[chan]);
      eventHandlers[chan]->start();
    }
    else
      eventHandlers[chan] = NULL;
  }
// std::cout<< "READ AND SAVE 2 " << std::endl;

  xseries_start_ai(aiFd);
  while (!(*(int *)stopAcq))
  {
    short buffers_s[nChan][bufSize]; // Raw data buffer used when not converted data are read
    float buffers_f[nChan][bufSize];          // Converted data buffer uesed when converted dta are
    // float buffers[nChan][bufSize];  OCCHIO QUI, HO CAMBIATO RISPETTO A PRIMA ***
    
    for (chan = 0; chan < nChan; chan++)
    {
      // Reading samples from the analog input channels
      if (saveConv){
        currReadSamples = xseries_read_ai(chanFd[chan], buffers_f[chan], bufSize);
      }
      else{
        currReadSamples = read(chanFd[chan], buffers_s[chan], bufSize);
      }

      if (currReadSamples <= 0)
      {
        if (errno == EAGAIN || errno == ENODATA)
        {
          usleep(50);
          currReadSamples = 0; // No data currently available... Try again
                               // continue;
        }
        else
        {
          if (errno == EOVERFLOW)
          {
            printf("PXI 6368 Error reading samples on ai%d: (%d) %s \n", chan,
                   errno, strerror(errno));
            for (chan = 0; chan < nChan; chan++)
            {
              printf("%d PXI 6368 Error END overflow %p\n", chan, eventHandlers[chan]);
              if (eventHandlers[chan])
              {
                eventHandlers[chan]->stop();
                delete eventHandlers[chan];
              }
              delete bufferHandlers[chan];
            }
            printf("A PXI 6368 Error END overflow\n");
            delete[] eventHandlers;
            delete[] bufferHandlers;
            printf("PXI 6368 Error END overflow\n");
            return -2;
          }
        }
      }
      else
      {
        // std::cout<< "READ AND SAVE 2.1 " << std::endl;

        if (saveConv){

          for (int sampleIdx = 0; sampleIdx < currReadSamples;  sampleIdx++){
            bufferHandlers[chan]->processSample(&buffers_f[chan][sampleIdx]);
          }
        }
        else{
          for (int sampleIdx = 0; sampleIdx < currReadSamples/sizeof(short);  sampleIdx++){
            bufferHandlers[chan]->processSample(&buffers_s[chan][sampleIdx]);
          }
        }
      }
    }
  }
// std::cout<< "READ AND SAVE 3 " << std::endl;

  for (chan = 0; chan < nChan; chan++)
  {
    if (eventHandlers[chan])
    {
      std::cout << "STOPPING EVENT HANDLER...." << std::endl;
      eventHandlers[chan]->stop();
      std::cout << "STOPPED" << std::endl;
      delete eventHandlers[chan];
    }
    //std::cout << "TERMINATING BUFFER HANDLER...." << std::endl;
    bufferHandlers[chan]->terminate();
    //std::cout << "TERMINATED" << std::endl;
  }
  saveList->stop();

  std::cout << "DELETING eventHandlers" << std::endl;
  delete[] eventHandlers;
  std::cout << "DELETING bufferHandlers" << std::endl;
  //    for( chan = 0; chan < nChan; chan++ )
  //	delete bufferHandlers[chan];
  delete[] bufferHandlers;
  printf("STOP PXI 6368 Acquisition \n");
  return 1;
}

int64_t NI6683_get_TCN_time()
{
  int status;
  hpn_timestamp_t time;

  std::string path = "/etc/opt/codac-6.1/tcn/nisync-6683h.xml";
  const char *cPath = path.c_str();
  // const char* c_cpath = cPath;

  // printf("cPath: %c \n", cPath[1]);

  status = tcn_register_device("/home/mdsplus/luca_tests/nisync-6683h.xml");
  printf("cPath: %s \n", cPath);
  if (status != TCN_SUCCESS)
  {
    fprintf(stderr, "Error registering device tcn device: %s\n", tcn_strerror(status));
  }

  status = tcn_init();
  if (status != TCN_SUCCESS)
  {
    fprintf(stderr, "Error initializing TCN : %s\n", tcn_strerror(status));
  }

  status = tcn_get_raw_time(&time);
  if (status != TCN_SUCCESS)
  {
    fprintf(stderr, "Error getting tcn time: %s\n", tcn_strerror(status));
  }

  status = tcn_finalize();
  if (status != TCN_SUCCESS)
  {
    fprintf(stderr, "Error finalizing tcn device: %s\n", tcn_strerror(status));
  }
  return time;
}

int NI6683_stop(int devFd, int* activeFds, int size)
{
  printf("STOPPING NI6683...\n");

  int status = 0;
  uint32_t count;
  
  status = nisync_abort_all_ftes(devFd);
  if (status == -1) return -1;

  for(int i = 0; i < size; i++){
    printf("Disabling future events for FD %i \n", activeFds[i]);  
    status = nisync_disable_future_time_events(activeFds[i]);
    if (status == -1) {
        printf("Disabling future events for FD %i failed with errno %s\n", activeFds[i], strerror(errno));
        return -1;
    }
    close(activeFds[i]);
   }
  
  // status = nisync_get_num_pending_ftes(devFd, &count);
  // printf("ACTIVE FDEs: %i\n", count);
  
  close(devFd);

printf("STOPPED!\n");
return 0;
}

int NI6683_close(int devFd, int* Fds,int size)
{
  printf("CLOSING NI6683...\n");

  int status = 0;

  for(int i = 0; i < size; ++i){
    printf("Closing FD %i\n", Fds[i]);
    close(Fds[i]);
  }
  // printf("Closing FD %i\n", devFd);
 

  printf("CLOSED!\n");
  return 0;
}



