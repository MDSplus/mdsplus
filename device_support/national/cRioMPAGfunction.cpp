#include "cRioMPAGfunction.h"

int DEBUG = 0;

NiFpga_Status crioMpagInitFifo(NiFpga_Session session, const char *fifoName, NiFpgaEx_DmaFifo fifoId, size_t fifoDepthSize)
{
  NiFpga_Status status = NiFpga_Status_Success;
  size_t actualDepth;

  if (DEBUG == 1)
    return status;

  NiFpga_MergeStatus(&status, NiFpga_ConfigureFifo2(session,
                                                    fifoId,
                                                    fifoDepthSize,
                                                    &actualDepth));

  printf("Host %s actual size %d data resize %d status %d\n", fifoName, actualDepth, fifoDepthSize, status);
  if (NiFpga_IsError(status))
  {
    printf("Host %s data fifo depth resize error\n", fifoName);
    return status;
  }
  return status;
}

NiFpga_Status crioMpagResetFifo(NiFpga_Session session, const char *fifoName, NiFpgaEx_DmaFifo fifoId)
{
  NiFpga_Status status = NiFpga_Status_Success;
  size_t nElem;
  size_t rElem;
  uint64_t dummy;
  uint64_t *dummyElem;
  int numDmaRead;
  int maxDmaRead = 1000;

  if (DEBUG == 1)
    return status;

  //Read elements in the queue
  NiFpga_MergeStatus(&status, NiFpga_ReadFifoU64(session,
                                                 fifoId,
                                                 &dummy,
                                                 0,
                                                 0,
                                                 &nElem));

  if (NiFpga_IsError(status))
  {
    NiFpga_Close(session, NiFpga_CloseAttribute_NoResetIfLastSession);
    printf("Host %s error reading num elements in fifo\n", fifoName);
    return status;
  }

  printf("Num elements in %s %d\n", fifoName, nElem);
  
  numDmaRead = 0;
  while ( nElem > 0 )
  {

    if ( numDmaRead > maxDmaRead )
       break; 

    dummyElem = (uint64_t *)calloc(nElem, sizeof(uint64_t));

    NiFpga_MergeStatus(&status, NiFpga_ReadFifoU64(session,
                                                   fifoId,
                                                   dummyElem,
                                                   nElem,
                                                   0,
                                                   &rElem)); // FIFO Remaining Element
    if (NiFpga_IsError(status))
    {
      NiFpga_Close(session, NiFpga_CloseAttribute_NoResetIfLastSession);
      printf("Host %s error reading fifo\n", fifoName);
      return status;
    }


    free(dummyElem);
    nElem = rElem; //fede 20210312
    numDmaRead++;

    //printf("Remaining elements in %s %d %d\n", fifoName, rElem, numDmaRead);  //20221214

  }

  if ( numDmaRead < maxDmaRead )
  {
     return status;
  } else {
     return -1;
  }
}


NiFpga_Status crioMpagInit(NiFpga_Session *session, const char *cRioId, size_t fifoDepthSize)
{
  NiFpga_Status status = NiFpga_Status_Success;

  if (DEBUG == 1)
    return status;

  /* opens a session, downloads the bitstream, and runs the FPGA */
  printf("cRio %s Opening a session... %s \n", cRioId, NiFpga_MainFPGA_9159_Bitfile);

  NiFpga_MergeStatus(&status, status = NiFpga_Open(NiFpga_MainFPGA_9159_Bitfile,
                                                   NiFpga_MainFPGA_9159_Signature,
                                                   cRioId,
                                                   0, //NiFpga_OpenAttribute_NoRun, //(20201203 recovered)
                                                   session));

  if (NiFpga_IsNotError(status))
  {

    status = startMpagFpga(*session);
    if (NiFpga_IsError(status))
    {
      NiFpga_Close(*session, NiFpga_CloseAttribute_NoResetIfLastSession);
      return -1;
    }

    sleep(1);

    uint16_t fpgaState;
    NiFpga_MergeStatus(&status, NiFpga_ReadU16(*session,
                                               NiFpga_MainFPGA_9159_IndicatorU16_Master_STATUS,
                                               &fpgaState));

    if (NiFpga_IsError(status) || fpgaState == FPGA_IDLE_STATE)
    {
      NiFpga_Close(*session, NiFpga_CloseAttribute_NoResetIfLastSession);
      printf("Error reading AGPS cRIO acquisition state or AGPS cRIO in IDLE state  \n");
      return -1;
    }

    //printf("FPGA state %s\n", fpgaStateStr[fpgaState]);

    printf("FPGA state %d\n", fpgaState);

    status = crioMpagInitFifo(*session, "FIFO A", NiFpga_MainFPGA_9159_TargetToHostFifoU64_FIFOTOPC_A, fifoDepthSize);
    if (NiFpga_IsError(status))
    {
      NiFpga_Close(*session, NiFpga_CloseAttribute_NoResetIfLastSession);
      return -1;
    }

    status = crioMpagInitFifo(*session, "FIFO B", NiFpga_MainFPGA_9159_TargetToHostFifoU64_FIFOTOPC_B, fifoDepthSize);
    if (NiFpga_IsError(status))
    {
      NiFpga_Close(*session, NiFpga_CloseAttribute_NoResetIfLastSession);
      return -1;
    }

    status = crioMpagInitFifo(*session, "FIFO C", NiFpga_MainFPGA_9159_TargetToHostFifoU64_FIFOTOPC_C, fifoDepthSize);
    if (NiFpga_IsError(status))
    {
      NiFpga_Close(*session, NiFpga_CloseAttribute_NoResetIfLastSession);
      return -1;
    }
  }
  else
  {
    printf("FPGA Open error : %d\n", status);
    return -1;
  }
  return status;
}

NiFpga_Status generateMpagFpgaBoolPule(NiFpga_Session session, NiFpga_MainFPGA_9159_ControlBool ctrlAddr, float puleDuration)
{

  NiFpga_Status status = NiFpga_Status_Success;

  NiFpga_MergeStatus(&status, NiFpga_WriteBool(session, ctrlAddr, 0));
  NiFpga_MergeStatus(&status, NiFpga_WriteBool(session, ctrlAddr, 1));
  usleep((int)(puleDuration * 1e6));
  NiFpga_MergeStatus(&status, NiFpga_WriteBool(session, ctrlAddr, 0));

  return status;
}

NiFpga_Status executeMpagFpgaCommand(const char *cRioId, int command)
{
  NiFpga_Status status = NiFpga_Status_Success;
  NiFpga_Session session;

  NiFpga_MergeStatus(&status, status = NiFpga_Open(NiFpga_MainFPGA_9159_Bitfile,
                                                   NiFpga_MainFPGA_9159_Signature,
                                                   cRioId,
                                                   0,
                                                   &session));

  if (NiFpga_IsNotError(status))
  {

    NiFpga_MainFPGA_9159_ControlBool cmd;

    switch (command)
    {
    case START_PLC_CMD:
      cmd = NiFpga_MainFPGA_9159_ControlBool_DEBUG_PLC_START;
      break;
    case STOP_PLC_CMD:
      cmd = NiFpga_MainFPGA_9159_ControlBool_DEBUG_PLC_STOP;
      break;
    default:
      printf("FPGA invlid command\n");
      return -1;
    }
    status = generateMpagFpgaBoolPule(session, cmd, 0.1);
    if (NiFpga_IsError(status))
    {
      printf("Error on start command to cRIO NASTER %d\n", status);
    }
    NiFpga_Close(session, NiFpga_CloseAttribute_NoResetIfLastSession);
  }
  else
  {
    printf("FPGA Open error : %d\n", status);
  }
  return status;
}

NiFpga_Status closeMpagFpgaSession(NiFpga_Session session)
{

  /* CODAC NOTE: NiFpga_Initialize() is no longer necessary! */

  NiFpga_Status status = NiFpga_Status_Success;

  printf("Stop Acquisition ...\n");

  if (DEBUG == 1)
    return status;

  status = generateMpagFpgaBoolPule(session, NiFpga_MainFPGA_9159_ControlBool_STOP_DMA, 0.1);
  if (NiFpga_IsError(status))
  {
    printf("Error on stop DMA link from slave\n");
  }

  status = NiFpga_StopFifo(session, NiFpga_MainFPGA_9159_TargetToHostFifoU64_FIFOTOPC_A);
  if (NiFpga_IsError(status))
  {
    printf("Host SLAVE A stop fifo error\n");
  }

  status = NiFpga_StopFifo(session, NiFpga_MainFPGA_9159_TargetToHostFifoU64_FIFOTOPC_B);
  if (NiFpga_IsError(status))
  {
    printf("Host SLAVE B stop fifo error\n");
  }

  status = NiFpga_StopFifo(session, NiFpga_MainFPGA_9159_TargetToHostFifoU64_FIFOTOPC_C);
  if (NiFpga_IsError(status))
  {
    printf("Host SLAVE C stop fifo error\n");
  }

  /* close the session now that we're done */
  //Session is not Close to mantai FPGA running
  printf("Closing the session...\n");
  NiFpga_MergeStatus(&status, NiFpga_Close(session, NiFpga_CloseAttribute_NoResetIfLastSession));

  /* CODAC NOTE: NiFpga_Finalize() is no longer necessary! */

  return status;
}

#define FPGA_FREQ_40MHz 40000000

NiFpga_Status setMpagAcqParam(NiFpga_Session session, float freq, uint16_t highStrobeTick, const int32_t *chanAOmap)
{
  NiFpga_Status status = NiFpga_Status_Success;
  uint16_t acqPeriod;

  if (DEBUG == 1)
    return status;

  acqPeriod = int(1.e6 / freq); //micro seconds
  NiFpga_MergeStatus(&status, NiFpga_WriteU16(session,
                                              NiFpga_MainFPGA_9159_ControlU16_ACQ_Trigger_PerioduSec,
                                              acqPeriod));

  if (NiFpga_IsError(status))
  {
    printf("Error Setting slave acquisition frequency");
    return -1;
  }

  NiFpga_MergeStatus(&status, NiFpga_WriteU16(session,
                                              NiFpga_MainFPGA_9159_ControlU16_ACQ_Trigger_HighDurationTicks,
                                              highStrobeTick));

  if (NiFpga_IsError(status))
  {
    printf("Error Setting high strobe Tick count");
    return -1;
  }
  return status;
}

/************************* Thread Function ************************************/

void *acquisitionThreadFPGA(void *args)
{

  NiFpga_Status status = NiFpga_Status_Success;
  int currSize = 0;
  int chanNum = NUM_SLAVE_CHANNEL + NUM_9220_CHANNEL * NUM_9220;
  short *buffers_s[NUM_SLAVE_CHANNEL + NUM_9220_CHANNEL * NUM_9220];
  double *time;
  int __count = 0;
  int chanTree;
  int sampleToRead = 0;
  int readElem;
  float dummyCalibCoeff[] = {1., 0., 0., 0.};
  int *readChanSmp;
  int *bufReadChanSmp;
  float gains[NUM_SLAVE_CHANNEL + NUM_9220_CHANNEL * NUM_9220];
  float *coeffs[NUM_SLAVE_CHANNEL + NUM_9220_CHANNEL * NUM_9220];
  int numCoeffs[NUM_SLAVE_CHANNEL + NUM_9220_CHANNEL * NUM_9220];
  NiFpgaEx_DmaFifo fifoId;
  const char *fifoName;
  struct_FPGA *structFpga = (struct_FPGA *)args;
  int slaveIdx = structFpga->slaveIdx;
  struct_FPGA_ACQ *fpgaAcq = ((struct_FPGA *)args)->structFpgaAcq;

  switch (slaveIdx)
  {
  case SLAVE_A:
    fifoId = NiFpga_MainFPGA_9159_TargetToHostFifoU64_FIFOTOPC_A;
    fifoName = "SLAVE A";
    break;
  case SLAVE_B:
    fifoId = NiFpga_MainFPGA_9159_TargetToHostFifoU64_FIFOTOPC_B;
    fifoName = "SLAVE B";
    break;
  case SLAVE_C:
    fifoId = NiFpga_MainFPGA_9159_TargetToHostFifoU64_FIFOTOPC_C;
    fifoName = "SLAVE C";
    break;
  }

  NiFpga_Session session = fpgaAcq->session;
  int bufSize = fpgaAcq->bufSize;
  int *chanState = fpgaAcq->chanState;
  int *resampledNid = fpgaAcq->resampledNid;
  int segmentSize = fpgaAcq->segmentSize;
  int numSamples = fpgaAcq->numSamples;
  int *dataNid = fpgaAcq->dataNid;
  int clockNid = fpgaAcq->clockNid;
  float timeIdx0 = fpgaAcq->timeIdx0;
  float period = fpgaAcq->period;
  void *treePtr = fpgaAcq->treePtr;
  SaveList *saveList = (SaveList *)fpgaAcq->saveListPtr;
  void *stopAcq = fpgaAcq->stopAcq;
  int shot = fpgaAcq->shot;
  void *resampledNidPtr = fpgaAcq->resampledNidPtr;
  char **streamNames = fpgaAcq->streamNames;
  float *streamGains = fpgaAcq->streamGains;
  float *streamOffsets = fpgaAcq->streamOffsets;
  uint8_t clockMode = fpgaAcq->clockMode;

  NiFpga_Bool val;
  NiFpga_MergeStatus(&status, NiFpga_ReadBool(session,
                                              NiFpga_MainFPGA_9159_ControlBool_Start,
                                              &val));
  int streamFactor = fpgaAcq->streamFactor;
  int slaveDataSamples = bufSize * NUM_SLAVE_CHANNEL;
  uint64_t data[bufSize * NUM_SLAVE_CHANNEL];

  readChanSmp = (int *)calloc(NUM_SLAVE_CHANNEL + NUM_9220_CHANNEL * NUM_9220, sizeof(int));
  bufReadChanSmp = (int *)calloc(NUM_SLAVE_CHANNEL + NUM_9220_CHANNEL * NUM_9220, sizeof(int));

  for (int chan = 0; chan < NUM_SLAVE_CHANNEL + NUM_9220_CHANNEL * NUM_9220; chan++)
  {
    bufReadChanSmp[chan] = bufSize;
    readChanSmp[chan] = 0;
    coeffs[chan] = dummyCalibCoeff;
    numCoeffs[chan] = 4;
    gains[chan] = 1.;
  }

  printf("WAIT  thread %s\n", fifoName);
  sem_wait(&structFpga->semThreadStart);
  printf("START thread %s Stop acq %d\n", fifoName, (*(uint8_t *)stopAcq));

  usleep(200000);

  status = crioMpagResetFifo(session, fifoName, fifoId);
  status = NiFpga_Status_Success;//Test 14/02/2023
  if( status == -1 )
  {
    printf("\n\nDMA buffer reset failed for slave index %d. This occurs when the acquisition clock\n is active during initialization. The clock must be active \nafter initialization. The acquisition system must be configured with a Gated Clock.\n", slaveIdx);
    fpgaAcq->retStatus = status;
    pthread_exit(NULL);
  }
  if ( NiFpga_IsError(status) )
  {
    fpgaAcq->retStatus = status;
    pthread_exit(NULL);
  }

  //Configure clock mode 
  if (slaveIdx == SLAVE_A)
  {
    /*
      Only one thread needs to configure the clock mode. 
      The clock mode is defined by default as external
    */
    NiFpga_MergeStatus(&status, NiFpga_WriteBool(session,
                                               NiFpga_MainFPGA_9159_ControlBool_Internal_ACQ_Trigger,
                                               clockMode));
  }

  if (DEBUG)
  {

    int16_t *data16;

    for (int chan = 0; chan < NUM_SLAVE_CHANNEL; chan++)
    {
      for (int smp = 0; smp < bufSize; smp++)
      {
        data16 = (int16_t *)&data[chan + smp * NUM_SLAVE_CHANNEL];

        //data[chan + smp * NUM_SLAVE_CHANNEL] = (int16_t)((chan+1)*1000 * sin(2 * 3.14 * smp / slaveDataSamples * 100.) + 500*slaveIdx);
        data16[0] = (int16_t)((chan + 1) * 1000 * sin(2 * 3.14 * smp / slaveDataSamples * 100.) + 500 * slaveIdx);
        data16[2] = (int16_t)((chan + 1) * 1000 * sin(4 * 3.14 * smp / slaveDataSamples * 100.) + 100);
        data16[3] = (int16_t)((chan + 1) * 1000 * sin(8 * 3.14 * smp / slaveDataSamples * 100.) + 200);

        //if(smp%1000) printf(" %d ", data[smp]);
      }
    }
    printf("\n");
  }

  while ((!*(uint8_t *)stopAcq) == 1)
  {
    readElem = readMpagFifoData(session, fifoName, fifoId, data, slaveDataSamples, stopAcq);
    printf("*");
    fflush(stdout);

    if (DEBUG)
    {
      readElem = slaveDataSamples;
      sleep(2);
      printf("Read elem %d \n", readElem);
    }

    if (readElem == -1 || readElem != slaveDataSamples)
    {
      //Report error contition only if system in in acquisizione ande the read data are less than requested
      if (readElem != slaveDataSamples && (!*(uint8_t *)stopAcq) == 1)
        printf("%s Fifo reading samples requested %d read %d\n", fifoName, slaveDataSamples, readElem);
      break;
    }

    //chanIdx is the counter of active  channels on the system  from 0 to NUM_SLAVE_CHANNEL * NUM_SLAVE + NUM_9220_CHANNEL * NUM_9220
    //chan    is the counter for all channels on the slave   from 0 to NUM_SLAVE_CHANNEL + NUM_9220_CHANNEL * NUM_9220

    int chanIdx = 0;
    int chanIndex = 0;
    int16_t *data16;

    for (int slaveCh = 0; slaveCh < NUM_SLAVE_CHANNEL; slaveCh++)
    {

      if (chanState[slaveCh + slaveIdx * NUM_SLAVE_CHANNEL]) // Node is ON
      {
        buffers_s[chanIdx] = new short[bufSize];
        for (int smp = 0; smp < bufSize; smp++)
        {
          //if(smp == 0)printf("%s %d buffers_s[%d] = data[%d]\n",fifoName, chanIdx,(chanIdx + smp * NUM_SLAVE_CHANNEL), (slaveCh + smp * NUM_SLAVE_CHANNEL));
          //  buffers_s[chanIdx][smp] = data[slaveCh + smp * NUM_SLAVE_CHANNEL];
          buffers_s[chanIdx][smp] = (data[slaveCh + smp * NUM_SLAVE_CHANNEL] >> (16 * 0)) & 0xFFFF;
        }
        if (slaveIdx == SLAVE_A)
        {
          data16 = (int16_t *)&data[slaveCh + 0 * NUM_SLAVE_CHANNEL];
          chanIndex = data16[1];
          if (chanIndex != chanIdx)
            printf("Disallineamento canale atteso %d letto %d \n", chanIdx, chanIndex);
        }
        chanIdx++;
      }
    }

    chanIdx = 0;
    for (int slaveCh = 0; slaveCh < NUM_SLAVE_CHANNEL; slaveCh++)
    {
      chanTree = slaveCh + slaveIdx * NUM_SLAVE_CHANNEL;
      if (chanState[chanTree])
      {

        /*
for(int smp = 0; smp < 10; smp++)
   printf(" %d ", buffers_s[slaveCh][smp] );
printf("Chan Idx %d  %d %d \n",chanIdx, slaveCh, readChanSmp[slaveCh]);
*/

        if (resampledNid)
          saveList->addItem(buffers_s[chanIdx],
                            bufReadChanSmp[slaveCh], sampleToRead, SHORT, segmentSize,
                            readChanSmp[slaveCh], dataNid[chanTree], clockNid, timeIdx0, treePtr, shot, streamFactor, streamNames[chanTree],
                            streamGains[chanTree], streamOffsets[chanTree], period, gains[slaveCh], coeffs[slaveCh],
                            numCoeffs[slaveCh], resampledNid[chanTree]);
        else
          saveList->addItem(buffers_s[chanIdx],
                            bufReadChanSmp[slaveCh], sampleToRead, SHORT, segmentSize,
                            readChanSmp[slaveCh], dataNid[chanTree], clockNid, timeIdx0, treePtr, shot, streamFactor, streamNames[chanTree],
                            streamGains[chanTree], streamOffsets[chanTree], period, gains[slaveCh], coeffs[slaveCh],
                            numCoeffs[slaveCh]);

        //Update the number of samples rearunningd
        readChanSmp[slaveCh] += bufReadChanSmp[slaveCh];
        chanIdx++;
      }
    }

    /********************************************/

    if (slaveIdx == SLAVE_A)
    {
      int ni9220chIdx;

      chanIdx = NUM_SLAVE_CHANNEL;
      ni9220chIdx = NUM_SLAVE * NUM_SLAVE_CHANNEL;
      for (int slaveCh = 0; slaveCh < NUM_9220_CHANNEL; slaveCh++)
      {
        if (chanState[ni9220chIdx])
        {
          buffers_s[chanIdx] = new short[bufSize];
          for (int smp = 0; smp < bufSize; smp++)
          {
            buffers_s[chanIdx][smp] = (data[slaveCh + smp * NUM_SLAVE_CHANNEL] >> (16 * 2)) & 0xFFFF;
          }
          chanIdx++;
        }
        ni9220chIdx++;
        if (chanState[ni9220chIdx])
        {
          buffers_s[chanIdx] = new short[bufSize];
          for (int smp = 0; smp < bufSize; smp++)
          {
            buffers_s[chanIdx][smp] = (data[slaveCh + smp * NUM_SLAVE_CHANNEL] >> (16 * 3)) & 0xFFFF;
          }
          chanIdx++;
        }
        ni9220chIdx++;
      }

      chanIdx = NUM_SLAVE_CHANNEL;
      for (int ch = 0; ch < NUM_9220_CHANNEL * NUM_9220; ch++)
      {
        ni9220chIdx = ch + NUM_SLAVE_CHANNEL;
        chanTree = ch + NUM_SLAVE * NUM_SLAVE_CHANNEL;
        if (chanState[chanTree])
        {

          if (resampledNid)
            saveList->addItem(buffers_s[chanIdx],
                              bufReadChanSmp[ni9220chIdx], sampleToRead, SHORT, segmentSize,
                              readChanSmp[ni9220chIdx], dataNid[chanTree], clockNid, timeIdx0, treePtr, shot, streamFactor, streamNames[chanTree],
                              streamGains[chanTree], streamOffsets[chanTree], period, gains[ni9220chIdx], coeffs[ni9220chIdx],
                              numCoeffs[ni9220chIdx], resampledNid[chanTree]);
          else
            saveList->addItem(buffers_s[chanIdx],
                              bufReadChanSmp[ni9220chIdx], sampleToRead, SHORT, segmentSize,
                              readChanSmp[ni9220chIdx], dataNid[chanTree], clockNid, timeIdx0, treePtr, shot, streamFactor, streamNames[chanTree],
                              streamGains[chanTree], streamOffsets[chanTree], period, gains[ni9220chIdx], coeffs[ni9220chIdx],
                              numCoeffs[ni9220chIdx]);

          //Update the number of samples read
          readChanSmp[ni9220chIdx] += bufReadChanSmp[ni9220chIdx];
          chanIdx++;
        }
      }
    }

    /********************************************/

    //numSamples = readChanSmp[0] + segmentSize;
    numSamples = bufSize + segmentSize;
  }
  printf("\n");

  free(readChanSmp);
  free(bufReadChanSmp);

  //Set clock mode to external 
  if (slaveIdx == SLAVE_A)  
  {
    /*
      Clock mode is set to external to stop data stream 
      generation via DMA. NB the external clock source 
      must be a gated clock and with internal clock there 
      must not be an external clock
    */
    NiFpga_MergeStatus(&status, NiFpga_WriteBool(session,
                                               NiFpga_MainFPGA_9159_ControlBool_Internal_ACQ_Trigger,
                                               CLOCK_MODE_EXTERNAL));
  }
  fpgaAcq->retStatus = status;
  printf("EXIT thread %s\n", fifoName);
  pthread_exit(NULL);
}

/******************************************************************************/

int mpag_readAndSaveAllChannels(NiFpga_Session session, int nChan, int *chanState, int bufSize, int segmentSize,
                                int numSamples, void *dataNidPtr, uint8_t clockMode, int clockNid, float timeIdx0, float period,
                                void *treePtr, void *saveListPtr, void *stopAcq, int shot, void *resampledNidPtr)
{

  NiFpga_Status status = NiFpga_Status_Success;
  int currSize = 0;
  //int64_t *data;
  //double *time;
  //int slaveDataSamples;
  int __count = 0;
  int chan;
  int chanNum = NUM_SLAVE_CHANNEL * NUM_SLAVE + NUM_9220_CHANNEL * NUM_9220;
  int sampleToRead = 0;
  int readElem;
  int8_t slaveTimeout[NUM_SLAVE] = {0, 0, 0};

  SaveList *saveList = (SaveList *)saveListPtr; // Class to equeu data buffer to save in pulse file
  int *dataNid = (int *)dataNidPtr;             // Channel node identifier
  int *resampledNid = (int *)resampledNidPtr;   // Channel node identifier

  float dummyCalibCoeff[] = {1., 0., 0., 0.};
  char *streamNames[nChan];
  float streamGains[nChan];
  float streamOffsets[nChan];

  uint16_t fpgaState;
  NiFpga_MergeStatus(&status, NiFpga_ReadU16(session,
                                             NiFpga_MainFPGA_9159_IndicatorU16_Master_STATUS,
                                             &fpgaState));

  if (NiFpga_IsError(status) || fpgaState == FPGA_IDLE_STATE)
  {
    NiFpga_Close(session, NiFpga_CloseAttribute_NoResetIfLastSession);
    printf("Error reading AGPS cRIO acquisition state or AGPS cRIO in IDLE state\n");
    return -1;
  }

  //Delete first all data nids
  printf("Delete all saved data if present\n");
  TreeNode *currNode;
  for (int i = 0; i < nChan; i++)
  {
    try
    {
      currNode = new TreeNode(dataNid[i], (Tree *)treePtr);
      currNode->deleteData();
      //Check if resampling
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
          Data *streamOffsetData = currNode->getExtendedAttribute("STREAM_OFFSET");
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
      printf("Error deleting data nodes %s : %s\n", currNode->getPath(), exc.what());
    }
  }
  printf("All saved data deleted\n");


  struct_FPGA structFpga[3];
  struct_FPGA_ACQ structFpgaAcq;

  structFpga[0].slaveIdx = SLAVE_A;
  structFpga[1].slaveIdx = SLAVE_B;
  structFpga[2].slaveIdx = SLAVE_C;
  structFpga[0].structFpgaAcq = &structFpgaAcq;
  structFpga[1].structFpgaAcq = &structFpgaAcq;
  structFpga[2].structFpgaAcq = &structFpgaAcq;

  int streamFactor = (int)(0.1 / period);
  if (bufSize > streamFactor)
    streamFactor = bufSize;
  else
  {
    if (streamFactor % bufSize != 0)
      streamFactor = (bufSize + 1) * (streamFactor / bufSize);
  }

  structFpgaAcq.session = session;
  structFpgaAcq.bufSize = bufSize;
  structFpgaAcq.chanState = chanState;
  structFpgaAcq.resampledNid = resampledNid;
  structFpgaAcq.segmentSize = segmentSize;
  structFpgaAcq.numSamples = numSamples;
  structFpgaAcq.dataNid = dataNid;
  structFpgaAcq.clockNid = clockNid;
  structFpgaAcq.timeIdx0 = timeIdx0;
  structFpgaAcq.period = period;
  structFpgaAcq.treePtr = treePtr;
  structFpgaAcq.saveListPtr = saveList;
  structFpgaAcq.stopAcq = stopAcq;
  structFpgaAcq.shot = shot;
  structFpgaAcq.resampledNidPtr = resampledNidPtr;
  structFpgaAcq.streamNames = streamNames;
  structFpgaAcq.streamGains = streamGains;
  structFpgaAcq.streamOffsets = streamOffsets;
  structFpgaAcq.streamFactor = streamFactor;
  structFpgaAcq.clockMode = clockMode; // Clock mode must be passed to the acquisition threads to configure the clock source correctly
  

  /* Enable DMA */
  printf("Enable DMA link from slave\n");
  status = generateMpagFpgaBoolPule(session, NiFpga_MainFPGA_9159_ControlBool_START_DMA, 0.1);
  if (NiFpga_IsError(status))
  {
    printf("Error on enable DMA link from slave\n");
    return -1;
  }

  /* 
     Force clock mode to External
     Even if the clok mode is defined as internal during initialization 
     it is forced to be external. This is necessary to avoid in external 
     clock mode the system immediately starts receiving data via DMA.
  */
  NiFpga_MergeStatus(&status, NiFpga_WriteBool(session,
                                               NiFpga_MainFPGA_9159_ControlBool_Internal_ACQ_Trigger,
                                               CLOCK_MODE_EXTERNAL));

  if (NiFpga_IsError(status))
  {
    printf("Error setting clock mode");
    return -1;
  }

  sem_init(&structFpga[0].semThreadStart, 0, 0);
  sem_init(&structFpga[1].semThreadStart, 0, 0);
  sem_init(&structFpga[2].semThreadStart, 0, 0);
  pthread_t threadSlaveA;
  pthread_t threadSlaveB;
  pthread_t threadSlaveC;
  int err[3];
  int errorBit = 0;

  *(uint8_t *)stopAcq = 0;

  err[0] = pthread_create(&threadSlaveA, NULL, &acquisitionThreadFPGA, &structFpga[0]);
  errorBit |= err[0] ? 1 : 0;
  err[1] = pthread_create(&threadSlaveB, NULL, &acquisitionThreadFPGA, &structFpga[1]);
  errorBit |= err[1] ? 1 << 1 : 0;
  err[2] = pthread_create(&threadSlaveC, NULL, &acquisitionThreadFPGA, &structFpga[2]);
  errorBit |= err[2] ? 1 << 2 : 0;

  printf("errorBit %d\n", errorBit);

  if (errorBit) //error on one thread creation must be close other thread
  {
    printf("Error on acquisition thread creation\n");
    *(uint8_t *)stopAcq = 1;
    return -1;
  }

  usleep(30000);
   
  startMpagFpga(session);
  
  sem_post(&structFpga[0].semThreadStart);
  sem_post(&structFpga[1].semThreadStart);
  sem_post(&structFpga[2].semThreadStart);

  pthread_join(threadSlaveA, NULL);
  pthread_join(threadSlaveB, NULL);
  pthread_join(threadSlaveC, NULL);

  sem_destroy(&structFpga[0].semThreadStart);
  sem_destroy(&structFpga[1].semThreadStart);
  sem_destroy(&structFpga[2].semThreadStart);

  printf("structFpga[0].structFpgaAcq->retStatus %d\n", structFpga[0].structFpgaAcq->retStatus);
  printf("structFpga[1].structFpgaAcq->retStatus %d\n", structFpga[1].structFpgaAcq->retStatus);
  printf("structFpga[2].structFpgaAcq->retStatus %d\n", structFpga[2].structFpgaAcq->retStatus);

  if( structFpga[0].structFpgaAcq->retStatus < 0 ||
      structFpga[1].structFpgaAcq->retStatus < 0 ||
      structFpga[2].structFpgaAcq->retStatus < 0 )
  {
      return -1;
  }


  printf("----- EXIT from mpagQueuedAcqData %d \n", (!*(int *)stopAcq));

  return 0;
}

int readMpagFifoData(NiFpga_Session session, const char *fifoName, NiFpgaEx_DmaFifo fifoId, uint64_t *data, size_t slaveBufSize, void *stopAcq)
{
  NiFpga_Status status = NiFpga_Status_Success;
  size_t currSize = 0;
  uint32_t count = 0;
  size_t currElem, rElem, nElem;

  //int __count = 0;

  int16_t *elem;

  if (data == NULL)
  {
    printf("Invalid arguments\n");
    return -1;
  }

  if (DEBUG == 1)
  {
    usleep(2000000);
    return slaveBufSize;
  }

  while (currSize < slaveBufSize)
  {

    count++;

    //Read elements in the queue
    NiFpga_MergeStatus(&status, NiFpga_ReadFifoU64(session,
                                                   fifoId,
                                                   data,
                                                   0,
                                                   0,
                                                   &nElem));

    if (NiFpga_IsError(status))
    {
      printf("Read Error\n");
      return -1;
    }

    //Terminate read from slave if stopAcq is asserted
    if (nElem == 0 && (*(uint8_t *)stopAcq) == 1)
      return -1;

    //Continue to read from slave
    if (nElem == 0)
      continue;

    currElem = (nElem + currSize < slaveBufSize) ? nElem : slaveBufSize - currSize;

    rElem = 0;
    NiFpga_MergeStatus(&status, NiFpga_ReadFifoU64(session, fifoId,
                                                   &data[currSize],
                                                   currElem,
                                                   200,
                                                   //NiFpga_InfiniteTimeout,
                                                   &rElem)); // FIFO Remaining Element

    //Time out is not an error condition. Must be acquired bufSize samples per channel
    if (status != -50400 && NiFpga_IsError(status))
    { //Timeout -50400
      printf("\nNiFpga_ReadFifoU64 %s Read Error data (status %d) elem %d\n", fifoName, status, slaveBufSize);
      return -1;
    }

    //usleep( 100 );
    //printf("NiFpga_ReadFifoU64 %s Read data elem %d Remaining %d\n", fifoName, slaveBufSize, rElem);
    //printf("read masSmp %d nElem %ld ,nTime %ld, currElem %ld currSize %d\n", maxSamp, nElem, nTime, currElem, currSize);

    currSize += currElem;
  }

  return currSize;
}

NiFpga_Status startMpagFpga(NiFpga_Session session)
{
  NiFpga_Status status = NiFpga_Status_Success;

  if (DEBUG == 1)
    return status;

  // run the FPGA application

  //Test to check resident FPGA
  printf("Running the FPGA...\n");
  NiFpga_MergeStatus(&status, NiFpga_Run(session, 0));

  if (NiFpga_IsError(status))
  {
    printf("Error running FPGA\n");
    return -1;
  }
  else
    printf("OK running FPGA\n");

  printf("Start Acquisition ...\n");
  return status;
}
