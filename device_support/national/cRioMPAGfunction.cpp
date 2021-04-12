#include "cRioMPAGfunction.h"

int DEBUG = 0;

NiFpga_Status crioMpagInitFifo(NiFpga_Session session, const char *fifoName,
                               NiFpgaEx_DmaFifo fifoId, size_t fifoDepthSize) {
  NiFpga_Status status = NiFpga_Status_Success;
  size_t actualDepth;

  if (DEBUG == 1)
    return status;

  NiFpga_MergeStatus(
      &status,
      NiFpga_ConfigureFifo2(session, fifoId, fifoDepthSize, &actualDepth));

  printf("Host %s actual size %d data resize %d status %d\n", fifoName,
         actualDepth, fifoDepthSize, status);
  if (NiFpga_IsError(status)) {
    printf("Host %s data fifo depth resize error\n", fifoName);
    return status;
  }

  /*
          status = NiFpga_StartFifo(session,  fifoId );
          if ( NiFpga_IsError(status) ) {
                  printf("Host %s start fifo error\n", fifoName);
                  return status;
          }

          status = crioMpagResetFifo(session, fifoName, fifoId);
          if ( NiFpga_IsError(status) ) {
                  return status;
          }
  */
  return status;
}

NiFpga_Status crioMpagResetFifo(NiFpga_Session session, const char *fifoName,
                                NiFpgaEx_DmaFifo fifoId) {
  NiFpga_Status status = NiFpga_Status_Success;
  size_t nElem;
  size_t rElem;
  int16_t dummy;
  int16_t *dummyElem;

  if (DEBUG == 1)
    return status;

  // Read elements in the queue
  NiFpga_MergeStatus(&status,
                     NiFpga_ReadFifoI16(session, fifoId, &dummy, 0, 0, &nElem));

  if (NiFpga_IsError(status)) {
    NiFpga_Close(session, 0);
    printf("Host %s error reading num elements in fifo\n", fifoName);
    return status;
  }

  printf("Num elements in %s %d\n", fifoName, nElem);

  if (nElem > 0) {

    dummyElem = (int16_t *)calloc(nElem, sizeof(int16_t));

    NiFpga_MergeStatus(&status,
                       NiFpga_ReadFifoI16(session, fifoId, dummyElem, nElem, 0,
                                          &rElem)); // FIFO Remaining Element
    if (NiFpga_IsError(status)) {
      NiFpga_Close(session, 0);
      printf("Host %s error reading fifo\n", fifoName);
      return status;
    }

    printf("Remaining elements in %s %d\n", fifoName, rElem);

    free(dummyElem);
  }

  return status;
}

NiFpga_Status crioMpagInit(NiFpga_Session *session, const char *cRioId,
                           size_t fifoDepthSize) {
  NiFpga_Status status = NiFpga_Status_Success;

  if (DEBUG == 1)
    return status;

  /* opens a session, downloads the bitstream, and runs the FPGA */
  printf("cRio %d Opening a session... %s \n", cRioId,
         NiFpga_MainFPGA_9159_Bitfile);

  NiFpga_MergeStatus(
      &status, status = NiFpga_Open(NiFpga_MainFPGA_9159_Bitfile,
                                    NiFpga_MainFPGA_9159_Signature, cRioId,
                                    NiFpga_OpenAttribute_NoRun, session));

  if (NiFpga_IsNotError(status)) {

    status = crioMpagInitFifo(
        *session, "FIFO A", NiFpga_MainFPGA_9159_TargetToHostFifoI16_FIFOTOPC_A,
        fifoDepthSize);
    if (NiFpga_IsError(status)) {
      NiFpga_Close(*session, 0);
      return -1;
    }

    status = crioMpagInitFifo(
        *session, "FIFO B", NiFpga_MainFPGA_9159_TargetToHostFifoI16_FIFOTOPC_B,
        fifoDepthSize);
    if (NiFpga_IsError(status)) {
      NiFpga_Close(*session, 0);
      return -1;
    }

    status = crioMpagInitFifo(
        *session, "FIFO C", NiFpga_MainFPGA_9159_TargetToHostFifoI16_FIFOTOPC_C,
        fifoDepthSize);
    if (NiFpga_IsError(status)) {
      NiFpga_Close(*session, 0);
      return -1;
    }

    NiFpga_MergeStatus(&status, NiFpga_Reset(*session));

    if (NiFpga_IsError(status)) {
      NiFpga_Close(*session, 0);
      printf("VI Reset error\n");
      return -1;
    }

  } else {
    printf("FPGA Open error : %d\n", status);
    return -1;
  }
  return status;
}

NiFpga_Status closeMpagFpgaSession(NiFpga_Session session) {

  /* CODAC NOTE: NiFpga_Initialize() is no longer necessary! */
  /* must be called before any other calls */
  // printf("FPGA Initializing...\n");
  // NiFpga_Status status = NiFpga_Initialize();

  NiFpga_Status status = NiFpga_Status_Success;

  printf("Stop Acquisition ...\n");

  if (DEBUG == 1)
    return status;

  NiFpga_MergeStatus(
      &status,
      NiFpga_WriteBool(session, NiFpga_MainFPGA_9159_ControlBool_Stop, 1));

  usleep(200000);

  status = NiFpga_StopFifo(session,
                           NiFpga_MainFPGA_9159_TargetToHostFifoI16_FIFOTOPC_A);
  if (NiFpga_IsError(status)) {
    printf("Host SLAVE A stop fifo error\n");
  }

  status = NiFpga_StopFifo(session,
                           NiFpga_MainFPGA_9159_TargetToHostFifoI16_FIFOTOPC_B);
  if (NiFpga_IsError(status)) {
    printf("Host SLAVE B stop fifo error\n");
  }

  status = NiFpga_StopFifo(session,
                           NiFpga_MainFPGA_9159_TargetToHostFifoI16_FIFOTOPC_C);
  if (NiFpga_IsError(status)) {
    printf("Host SLAVE C stop fifo error\n");
  }

  /* close the session now that we're done */
  printf("Closing the session...\n");
  NiFpga_MergeStatus(&status, NiFpga_Close(session, 0));

  /* CODAC NOTE: NiFpga_Finalize() is no longer necessary! */
  /* must be called after all other calls */
  // printf("Finalizing...\n");
  // NiFpga_MergeStatus(&status, NiFpga_Finalize());

  return status;
}

#define FPGA_FREQ_40MHz 40000000

NiFpga_Status setMpagAcqParam(NiFpga_Session session, uint8_t clockMode,
                              float freq, uint16_t highStrobeTick,
                              const int32_t *chanAOmap) {
  NiFpga_Status status = NiFpga_Status_Success;
  uint16_t acqPeriod;

  if (DEBUG == 1)
    return status;

  /*
     ForceLocal = Internal Clock = clockMode ( 1 Internal - 0 External)
     External Clock Force Local = 0 Module 7 ch0 = low; Clock from Module 7 ch 1
  */
  NiFpga_MergeStatus(
      &status,
      NiFpga_WriteBool(session, NiFpga_MainFPGA_9159_ControlBool_ForceLocal,
                       clockMode));

  if (NiFpga_IsError(status)) {
    printf("Error setting closk mode");
    return -1;
  }

  acqPeriod = int(1.e6 / freq); // micro seconds

  NiFpga_MergeStatus(
      &status,
      NiFpga_WriteU16(session,
                      NiFpga_MainFPGA_9159_ControlU16_FrameStrobePerioduSec,
                      acqPeriod));

  if (NiFpga_IsError(status)) {
    printf("Error Setting slave acquisition frequency");
    return -1;
  }

  NiFpga_MergeStatus(
      &status,
      NiFpga_WriteU16(
          session,
          NiFpga_MainFPGA_9159_ControlU16_HighPhaseFrameStrobeDurationTicks,
          highStrobeTick));

  if (NiFpga_IsError(status)) {
    printf("Error Setting high strobe Tick count");
    return -1;
  }
  /*
      NiFpga_MergeStatus(&status, NiFpga_WriteArrayI32(session,
                                                       NiFpga_MainFPGA_9159_ControlArrayI32_IndexArray,
                                                       chanAOmap,
                                                       16));


      if ( NiFpga_IsError(status) ) {
          printf("Error Setting Analog Output signals Mapping");
          return -1;
      }
  */

  return status;
}

/************************* Thread Function ************************************/

void *acquisitionThreadFPGA(void *args) {

  NiFpga_Status status = NiFpga_Status_Success;
  int currSize = 0;
  short *buffers_s[NUM_SLAVE_CHANNEL];
  double *time;
  int __count = 0;
  int chan;
  int chanNum = NUM_SLAVE_CHANNEL * NUM_SLAVE;
  int sampleToRead = 0;
  int readElem;
  float dummyCalibCoeff[] = {1., 0., 0., 0.};
  int *readChanSmp;
  int *bufReadChanSmp;
  float gains[NUM_SLAVE_CHANNEL];
  float *coeffs[NUM_SLAVE_CHANNEL];
  int numCoeffs[NUM_SLAVE_CHANNEL];
  NiFpgaEx_DmaFifo fifoId;
  const char *fifoName;
  struct_FPGA *structFpga = (struct_FPGA *)args;
  int slaveIdx = structFpga->slaveIdx;
  struct_FPGA_ACQ *fpgaAcq = ((struct_FPGA *)args)->structFpgaAcq;

  switch (slaveIdx) {
  case SLAVE_A:
    fifoId = NiFpga_MainFPGA_9159_TargetToHostFifoI16_FIFOTOPC_A;
    fifoName = "SLAVE A";
    break;
  case SLAVE_B:
    fifoId = NiFpga_MainFPGA_9159_TargetToHostFifoI16_FIFOTOPC_B;
    fifoName = "SLAVE B";
    break;
  case SLAVE_C:
    fifoId = NiFpga_MainFPGA_9159_TargetToHostFifoI16_FIFOTOPC_C;
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
  int streamFactor = fpgaAcq->streamFactor;
  int slaveDataSamples = bufSize * NUM_SLAVE_CHANNEL;
  int16_t data[bufSize * NUM_SLAVE_CHANNEL];

  readChanSmp = (int *)calloc(NUM_SLAVE_CHANNEL, sizeof(int));
  bufReadChanSmp = (int *)calloc(NUM_SLAVE_CHANNEL, sizeof(int));

  for (int chan = 0; chan < NUM_SLAVE_CHANNEL; chan++) {
    bufReadChanSmp[chan] = bufSize;
    readChanSmp[chan] = 0;
    coeffs[chan] = dummyCalibCoeff;
    numCoeffs[chan] = 4;
    gains[chan] = 1.;
  }

  printf("WAIT  thread %s\n", fifoName);
  sem_wait(&structFpga->semThreadStart);
  printf("START thread %s Stop acq %d\n", fifoName, (*(uint8_t *)stopAcq));

  status = crioMpagResetFifo(session, fifoName, fifoId);

  if (DEBUG) {
    for (int chan = 0; chan < NUM_SLAVE_CHANNEL; chan++) {
      for (int smp = 0; smp < bufSize; smp++) {
        data[chan + smp * NUM_SLAVE_CHANNEL] = (int16_t)(
            (chan + 1) * 1000 * sin(2 * 3.14 * smp / slaveDataSamples * 100.) +
            500 * slaveIdx);
        if (smp % 1000)
          printf(" %d ", data[smp]);
      }
    }
    printf("\n");
  }

  while ((!*(uint8_t *)stopAcq) == 1) {

    readElem = readMpagFifoData(session, fifoName, fifoId, data,
                                slaveDataSamples, stopAcq);
    if (readElem == -1 || readElem != slaveDataSamples) {
      // Report error contition only if system in in acquisizione ande the read
      // data are less than requested
      if (readElem != slaveDataSamples && (!*(uint8_t *)stopAcq) == 1)
        printf("%s Fifo reading samples requested %d read %d\n", fifoName,
               slaveDataSamples, readElem);
      break;
    }

    int chanIdx = 0;
    for (int chan = 0; chan < NUM_SLAVE_CHANNEL; chan++) {
      if (chanState[chan + slaveIdx * NUM_SLAVE_CHANNEL]) {
        buffers_s[chanIdx] = new short[bufSize];
        for (int smp = 0; smp < bufSize; smp++) {
          // if(smp == 0)printf("%s %d buffers_s[%d] = data[%d]\n",fifoName,
          // chanIdx,(chanIdx + smp * NUM_SLAVE_CHANNEL), (chan + smp *
          // NUM_SLAVE_CHANNEL));
          buffers_s[chanIdx][smp] = data[chan + smp * NUM_SLAVE_CHANNEL];
        }
        chanIdx++;
      }
    }

    printf("Data Read from DMA %s OK\n", fifoName);

    chanIdx = 0;
    for (int slaveCh = 0; slaveCh < NUM_SLAVE_CHANNEL; slaveCh++)
    // for( int chan = 0; chan <  nChan; chan++)
    {
      chan = slaveCh + slaveIdx * NUM_SLAVE_CHANNEL;
      if (chanState[chan]) {
        /*
        for(int smp = 0; smp < 10; smp++)
           printf(" %d ", buffers_s[slaveCh][smp] );
        printf("Read chan %d \n", readChanSmp[slaveCh]);
        */
        if (resampledNid)
          saveList->addItem(
              buffers_s[chanIdx], bufReadChanSmp[chanIdx], sampleToRead, SHORT,
              segmentSize, readChanSmp[chanIdx], dataNid[chan], clockNid,
              timeIdx0, treePtr, shot, streamFactor, streamNames[chan],
              streamGains[chan], streamOffsets[chan], period, gains[slaveCh],
              coeffs[slaveCh], numCoeffs[slaveCh], resampledNid[chan]);
        else
          saveList->addItem(
              buffers_s[chanIdx], bufReadChanSmp[chanIdx], sampleToRead, SHORT,
              segmentSize, readChanSmp[chanIdx], dataNid[chan], clockNid,
              timeIdx0, treePtr, shot, streamFactor, streamNames[chan],
              streamGains[chan], streamOffsets[chan], period, gains[slaveCh],
              coeffs[slaveCh], numCoeffs[slaveCh]);

        // Update the number of samples read
        readChanSmp[slaveCh] += bufReadChanSmp[slaveCh];
        chanIdx++;
      }
    }
    numSamples = readChanSmp[0] + segmentSize;
  }
  free(readChanSmp);
  free(bufReadChanSmp);
  printf("EXIT thread %s\n", fifoName);
}

/******************************************************************************/

int mpag_readAndSaveAllChannels(NiFpga_Session session, int nChan,
                                int *chanState, int bufSize, int segmentSize,
                                int numSamples, void *dataNidPtr, int clockNid,
                                float timeIdx0, float period, void *treePtr,
                                void *saveListPtr, void *stopAcq, int shot,
                                void *resampledNidPtr) {

  NiFpga_Status status = NiFpga_Status_Success;
  int currSize = 0;
  int16_t *data;
  double *time;
  int slaveDataSamples;
  int __count = 0;
  int chan;
  int chanNum = NUM_SLAVE_CHANNEL * NUM_SLAVE;
  int sampleToRead = 0;
  int readElem;
  int8_t slaveTimeout[NUM_SLAVE] = {0, 0, 0};

  SaveList *saveList = (SaveList *)
      saveListPtr; // Class to equeu data buffer to save in pulse file
  int *dataNid = (int *)dataNidPtr;           // Channel node identifier
  int *resampledNid = (int *)resampledNidPtr; // Channel node identifier

  float dummyCalibCoeff[] = {1., 0., 0., 0.};
  char *streamNames[nChan];
  float streamGains[nChan];
  float streamOffsets[nChan];
  // Dummy calibration coeffs
  float *coeffs[nChan];
  int numCoeffs[nChan];
  short *buffers_s[nChan];
  int readChanSmp[nChan];
  int bufReadChanSmp[nChan];
  float gains[nChan];

  // Delete first all data nids
  for (int i = 0; i < nChan; i++) {
    try {
      TreeNode *currNode = new TreeNode(dataNid[i], (Tree *)treePtr);
      currNode->deleteData();
      // Check if resampling
      try {
        Data *streamNameData = currNode->getExtendedAttribute("STREAM_NAME");
        streamNames[i] = streamNameData->getString();
        deleteData(streamNameData);
        try {
          Data *streamGainData = currNode->getExtendedAttribute("STREAM_GAIN");
          streamGains[i] = streamGainData->getFloat();
        } catch (MdsException &exc) {
          streamGains[i] = 1;
        }
        try {
          Data *streamOffsetData =
              currNode->getExtendedAttribute("STREAM_OFFSET");
          streamOffsets[i] = streamOffsetData->getFloat();
        } catch (MdsException &exc) {
          streamOffsets[i] = 0;
        }
      } catch (MdsException &exc) {
        streamNames[i] = NULL;
        streamGains[i] = 0;
        streamOffsets[i] = 0;
      }
      delete currNode;
      if (resampledNid) {
        currNode = new TreeNode(resampledNid[i], (Tree *)treePtr);
        currNode->deleteData();
        delete currNode;
      }
    } catch (MdsException &exc) {
      printf("Error deleting data nodes\n");
    }
  }

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
  else {
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
  structFpgaAcq.streamOffsets = streamGains;
  structFpgaAcq.streamFactor = streamFactor;

  /*
      float freq = 0.000025;
      int freqi;
      memcpy(&freqi, &freq, sizeof(freq));

      NiFpga_MergeStatus(&status, NiFpga_WriteI32(session,
                                                   NiFpga_MainFPGA_9159Simulator_ControlSgl_frequency,
                                                   freqi));

      NiFpga_MergeStatus(&status, NiFpga_WriteI32(session,
                                                   NiFpga_MainFPGA_9159Simulator_ControlI32_SinSamp,
                                                   200));

  */
  sem_init(&structFpga[0].semThreadStart, 0, 0);
  sem_init(&structFpga[1].semThreadStart, 0, 0);
  sem_init(&structFpga[2].semThreadStart, 0, 0);
  pthread_t threadSlaveA;
  pthread_t threadSlaveB;
  pthread_t threadSlaveC;
  int err[3];
  int errorBit = 0;

  *(uint8_t *)stopAcq = 0;

  err[0] = pthread_create(&threadSlaveA, NULL, &acquisitionThreadFPGA,
                          &structFpga[0]);
  errorBit |= err[0] ? 1 : 0;
  err[1] = pthread_create(&threadSlaveB, NULL, &acquisitionThreadFPGA,
                          &structFpga[1]);
  errorBit |= err[1] ? 1 << 1 : 0;
  err[2] = pthread_create(&threadSlaveC, NULL, &acquisitionThreadFPGA,
                          &structFpga[2]);
  errorBit |= err[2] ? 1 << 2 : 0;

  if (errorBit) // error on one thread creation must be close other thread
  {
    printf("Error on cgreate thread\n");
    *(uint8_t *)stopAcq = 1;
  }

  usleep(30000);

  startMpagFpga(session);
  sleep(1);
  sem_post(&structFpga[0].semThreadStart);
  sem_post(&structFpga[1].semThreadStart);
  sem_post(&structFpga[2].semThreadStart);

  pthread_join(threadSlaveA, NULL);
  pthread_join(threadSlaveB, NULL);
  pthread_join(threadSlaveC, NULL);
  sem_destroy(&structFpga[0].semThreadStart);
  sem_destroy(&structFpga[1].semThreadStart);
  sem_destroy(&structFpga[2].semThreadStart);

  printf("EXIT from mpagQueuedAcqData %d \n", (!*(int *)stopAcq));

  return 0;
}

int readMpagFifoData(NiFpga_Session session, const char *fifoName,
                     NiFpgaEx_DmaFifo fifoId, int16_t *data,
                     size_t slaveBufSize, void *stopAcq) {
  NiFpga_Status status = NiFpga_Status_Success;
  size_t currSize = 0;
  uint32_t count = 0;
  size_t currElem, rElem, nElem;

  // int __count = 0;

  int16_t *elem;

  if (data == NULL) {
    printf("Invalid arguments\n");
    return -1;
  }

  if (DEBUG == 1) {
    usleep(2000000);
    return slaveBufSize;
  }

  while (currSize < slaveBufSize) {

    count++;

    // Read elements in the queue
    NiFpga_MergeStatus(&status,
                       NiFpga_ReadFifoI16(session, fifoId, data, 0, 0, &nElem));

    if (NiFpga_IsError(status)) {
      printf("Read Error\n");
      return -1;
    }

    // Terminate read from slave if stopAcq is asserted
    if (nElem == 0 && (*(uint8_t *)stopAcq) == 1)
      return -1;

    // Continue to read from slave
    if (nElem == 0)
      continue;

    currElem =
        (nElem + currSize < slaveBufSize) ? nElem : slaveBufSize - currSize;

    rElem = 0;
    NiFpga_MergeStatus(&status,
                       NiFpga_ReadFifoI16(session, fifoId, &data[currSize],
                                          currElem, 200,
                                          // NiFpga_InfiniteTimeout,
                                          &rElem)); // FIFO Remaining Element

    // Time out is not an error condition. Must be acquired bufSize samples per
    // channel
    if (status != -50400 && NiFpga_IsError(status)) { // Timeout -50400
      printf("\nNiFpga_ReadFifoI16 %s Read Error data (status %d) elem %d\n",
             fifoName, status, slaveBufSize);
      return -1;
    }

    // usleep( 100 );
    // printf("\nNiFpga_ReadFifoI16 %s Read data elem %d Remaining %d\n",
    // fifoName, slaveBufSize, rElem);
    // printf("read masSmp %d nElem %ld ,nTime %ld, currElem %ld currSize %d\n",
    // maxSamp, nElem, nTime, currElem, currSize);

    currSize += currElem;
  }

  return currSize;
}

NiFpga_Status startMpagFpga(NiFpga_Session session) {
  NiFpga_Status status = NiFpga_Status_Success;

  if (DEBUG == 1)
    return status;

  // run the FPGA application
  printf("Running the FPGA...\n");
  NiFpga_MergeStatus(&status, NiFpga_Run(session, 0));

  if (NiFpga_IsError(status)) {
    printf("Error running FPGA\n");
    return -1;
  } else
    printf("OK running FPGA\n");

  printf("Start Acquisition ...\n");

  NiFpga_MergeStatus(
      &status,
      NiFpga_WriteBool(session, NiFpga_MainFPGA_9159_ControlBool_Start, 1));

  sleep(1);

  NiFpga_Bool val;
  NiFpga_MergeStatus(
      &status,
      NiFpga_ReadBool(session, NiFpga_MainFPGA_9159_ControlBool_Start, &val));

  printf("Start boolean flag %d\n", val);

  return status;
}

#ifdef OLD_CODE

int mpag_readAndSaveAllChannels(NiFpga_Session session, int nChan,
                                int *chanState, int bufSize, int segmentSize,
                                int numSamples, void *dataNidPtr, int clockNid,
                                float timeIdx0, float period, void *treePtr,
                                void *saveListPtr, void *stopAcq, int shot,
                                void *resampledNidPtr) {

  NiFpga_Status status = NiFpga_Status_Success;
  int currSize = 0;
  int16_t *data;
  double *time;
  int slaveDataSamples;
  int __count = 0;
  int chan;
  int chanNum = NUM_SLAVE_CHANNEL * NUM_SLAVE;
  int sampleToRead = 0;
  int readElem;
  int8_t slaveTimeout[NUM_SLAVE] = {0, 0, 0};

  SaveList *saveList = (SaveList *)
      saveListPtr; // Class to equeu data buffer to save in pulse file
  int *dataNid = (int *)dataNidPtr;           // Channel node identifier
  int *resampledNid = (int *)resampledNidPtr; // Channel node identifier

  float dummyCalibCoeff[] = {1., 0., 0., 0.};
  char *streamNames[nChan];
  float streamGains[nChan];
  float streamOffsets[nChan];
  // Dummy calibration coeffs
  float *coeffs[nChan];
  int numCoeffs[nChan];
  short *buffers_s[nChan];
  int readChanSmp[nChan];
  int bufReadChanSmp[nChan];
  float gains[nChan];

  // Delete first all data nids
  for (int i = 0; i < nChan; i++) {
    try {
      TreeNode *currNode = new TreeNode(dataNid[i], (Tree *)treePtr);
      currNode->deleteData();
      // Check if resampling
      try {
        Data *streamNameData = currNode->getExtendedAttribute("STREAM_NAME");
        streamNames[i] = streamNameData->getString();
        deleteData(streamNameData);
        try {
          Data *streamGainData = currNode->getExtendedAttribute("STREAM_GAIN");
          streamGains[i] = streamGainData->getFloat();
        } catch (MdsException &exc) {
          streamGains[i] = 1;
        }
        try {
          Data *streamOffsetData =
              currNode->getExtendedAttribute("STREAM_OFFSET");
          streamOffsets[i] = streamOffsetData->getFloat();
        } catch (MdsException &exc) {
          streamOffsets[i] = 0;
        }
      } catch (MdsException &exc) {
        streamNames[i] = NULL;
        streamGains[i] = 0;
        streamOffsets[i] = 0;
      }
      delete currNode;
      if (resampledNid) {
        currNode = new TreeNode(resampledNid[i], (Tree *)treePtr);
        currNode->deleteData();
        delete currNode;
      }
    } catch (MdsException &exc) {
      printf("Error deleting data nodes\n");
    }
  }

  for (int chan = 0; chan < nChan; chan++) {
    buffers_s[chan] = new short[bufSize];
    bufReadChanSmp[chan] = bufSize;
    readChanSmp[chan] = 0;
    coeffs[chan] = dummyCalibCoeff;
    numCoeffs[chan] = 4;
    gains[chan] = 1.;
  }

  slaveDataSamples = bufSize * NUM_SLAVE_CHANNEL;
  data = (int16_t *)calloc(slaveDataSamples * NUM_SLAVE, sizeof(int16_t));

  int streamFactor = (int)(0.1 / period);
  if (bufSize > streamFactor)
    streamFactor = bufSize;
  else {
    if (streamFactor % bufSize != 0)
      streamFactor = (bufSize + 1) * (streamFactor / bufSize);
  }

  /**************************************************************/

  status = crioMpagResetFifo(
      session, "FIFO A", NiFpga_MainFPGA_9159_TargetToHostFifoI16_FIFOTOPC_A);
  status = crioMpagResetFifo(
      session, "FIFO B", NiFpga_MainFPGA_9159_TargetToHostFifoI16_FIFOTOPC_B);
  status = crioMpagResetFifo(
      session, "FIFO C", NiFpga_MainFPGA_9159_TargetToHostFifoI16_FIFOTOPC_C);

  /*
      float freq = 0.000025;
      int freqi;
      memcpy(&freqi, &freq, sizeof(freq));

      NiFpga_MergeStatus(&status, NiFpga_WriteI32(session,
                                                   NiFpga_MainFPGA_9159Simulator_ControlSgl_frequency,
                                                   freqi));

      NiFpga_MergeStatus(&status, NiFpga_WriteI32(session,
                                                   NiFpga_MainFPGA_9159Simulator_ControlI32_SinSamp,
                                                   200));

  */
  startMpagFpga(session);
  /**************************************************************/

  while ((!*(uint8_t *)stopAcq) == 1) {

    readElem = readMpagFifoData(
        session, "SLAVE A", NiFpga_MainFPGA_9159_TargetToHostFifoI16_FIFOTOPC_A,
        &data[0], slaveDataSamples);
    if (readElem != -2 && (readElem == -1 || readElem != slaveDataSamples)) {
      if (readElem != slaveDataSamples)
        printf("SLAVE A Fifo reading samples requested %d read %d\n",
               slaveDataSamples, readElem);
      break;
    }
    if (readElem == -2) { // timeout
      printf("SLAVE A Fifo reading timeout\n");
      slaveTimeout[0] = 1;
    } else
      printf("SLAVE A Fifo reading samples requested %d read %d\n",
             slaveDataSamples, readElem);

    readElem = readMpagFifoData(
        session, "SLAVE B", NiFpga_MainFPGA_9159_TargetToHostFifoI16_FIFOTOPC_B,
        &data[slaveDataSamples], slaveDataSamples);
    if (readElem != -2 && (readElem == -1 || readElem != slaveDataSamples)) {
      if (readElem != slaveDataSamples)
        printf("SLAVE B Fifo reading samples requested %d read %d\n",
               slaveDataSamples, readElem);
      break;
    }
    if (readElem == -2) { // timeout
      printf("SLAVE B Fifo reading timeout\n");
      slaveTimeout[1] = 1;
    } else
      printf("SLAVE B Fifo reading samples requested %d read %d\n",
             slaveDataSamples, readElem);

    readElem = readMpagFifoData(
        session, "SLAVE C", NiFpga_MainFPGA_9159_TargetToHostFifoI16_FIFOTOPC_C,
        &data[2 * slaveDataSamples], slaveDataSamples);
    if (readElem != -2 && (readElem == -1 || readElem != slaveDataSamples)) {
      if (readElem != slaveDataSamples)
        printf("SLAVE C Fifo reading samples requested %d read %d\n",
               slaveDataSamples, readElem);
      break;
    }
    if (readElem == -2) { // timeout
      printf("SLAVE C Fifo reading timeout\n");
      slaveTimeout[2] = 1;
    } else
      printf("SLAVE C Fifo reading samples requested %d read %d\n",
             slaveDataSamples, readElem);

    int chanIdx = 0;
    for (int slave = 0; slave < NUM_SLAVE; slave++) {
      if (slaveTimeout[slave])
        continue;
      for (int chan = 0; chan < NUM_SLAVE_CHANNEL; chan++) {
        if (chanState[chan + slave * NUM_SLAVE_CHANNEL]) {
          for (int smp = 0; smp < bufSize; smp++) {
            // if(smp == 0)printf("%d %d buffers_s[%d] = data[%d]\n",slave,
            // chan,(chanIdx + slave * NUM_SLAVE_CHANNEL), (chan + smp *
            // NUM_SLAVE_CHANNEL + slave * slaveDataSamples));
            buffers_s[chanIdx][smp] =
                data[chan + smp * NUM_SLAVE_CHANNEL + slave * slaveDataSamples];
          }
          chanIdx++;
        }
      }
    }

    printf("OK\n");

    for (int slave = 0; slave < NUM_SLAVE; slave++) {
      if (slaveTimeout[slave])
        continue;
      for (int slaveCh = 0; slaveCh < NUM_SLAVE_CHANNEL; slaveCh++)
      // for( int chan = 0; chan <  nChan; chan++)
      {
        chan = slaveCh + slave * NUM_SLAVE_CHANNEL;
        if (resampledNid)
          saveList->addItem(buffers_s[chan], bufReadChanSmp[chan], sampleToRead,
                            SHORT, segmentSize, readChanSmp[chan],
                            dataNid[chan], clockNid, timeIdx0, treePtr, shot,
                            streamFactor, streamNames[chan], streamGains[chan],
                            streamOffsets[chan], period, gains[chan],
                            coeffs[chan], numCoeffs[chan], resampledNid[chan]);
        else
          saveList->addItem(buffers_s[chan], bufReadChanSmp[chan], sampleToRead,
                            SHORT, segmentSize, readChanSmp[chan],
                            dataNid[chan], clockNid, timeIdx0, treePtr, shot,
                            streamFactor, streamNames[chan], streamGains[chan],
                            streamOffsets[chan], period, gains[chan],
                            coeffs[chan], numCoeffs[chan]);

        buffers_s[chan] = new short[bufSize];
        // Update the number of samples read
        readChanSmp[chan] += bufReadChanSmp[chan];
      }
    }
    numSamples = readChanSmp[0] + segmentSize;
  }

  free(data);

  printf("EXIT from mpagQueuedAcqData %d \n", (!*(int *)stopAcq));

  return 0;
}

#endif
