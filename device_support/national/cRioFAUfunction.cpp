#include "cRioFAUfunction.h"
//#include <time.h>
#include <thread>

using namespace MDSplus;

/********************/

// Support class for enqueueing storage requests
class FAUSaveItem
{
  uint8_t *data;
  double *time;
  double trigTime;
  int currSize;
  int *currSizeA;
  int *dataNids;
  Tree *treePtr;
  FAUSaveItem *nxt;
  size_t nDio;
  TreeNode *currNode[NUM_DIO];

  Float64Array *timeArrData;
  Data *start;
  Data *end;
  Uint8Array *dataArrData;

  //    int *currSizeA;
  //    double *timeDio;

public:
  // FAUSaveItem(uint8_t *data, double *time, double trigTime, int currSize,
  // size_t nDio, int *dataNids, Tree *treePtr)
  FAUSaveItem(uint8_t *data, double *time, double trigTime, int *currSizeA,
              int currSize, size_t nDio, int *dataNids, Tree *treePtr)
  {
    this->data = data;
    this->time = time;
    this->trigTime = trigTime;
    this->currSize = currSize;
    this->currSizeA = currSizeA;
    this->nDio = nDio;
    this->dataNids = dataNids;
    this->treePtr = treePtr;
    nxt = 0;
  }

  void setNext(FAUSaveItem *itm) { nxt = itm; }

  FAUSaveItem *getNext() { return nxt; }

  void save()
  {

    //struct timespec before, after;
    //long elapsed_nsecs;

    /* handle connection */


    Tree *tree = new Tree(((Tree *)treePtr)->getName(), ((Tree *)treePtr)->getShot());

    try
    {

      //clock_gettime(CLOCK_REALTIME, &before);

      for (int j = 0; j < nDio; j++)
      {
        TreeNode *currNode =
        new TreeNode(((int *)dataNids)[j], (Tree *)tree);
        timeArrData = new Float64Array(&time[j * currSize], currSizeA[j]);
        start = new Float64(time[j * currSize]);
        end   = new Float64(time[(currSizeA[j] - 1) + j * currSize]);

        dataArrData = new Uint8Array(&data[j * currSize], currSizeA[j]);
        currNode->makeSegment(start, end, timeArrData, dataArrData);

        delete dataArrData;
        delete timeArrData;
        delete start;
        delete end;
        delete currNode;
        // if(j == 7 ) printf("j %d ts %lf te %lf size %d\n", j, time[j *
        // currSize],time[ (currSizeA[j] - 1) + j * currSize ],currSizeA[j]);
      }

      free(currSizeA);
      free(time);
      free(data);

      /*      
      clock_gettime(CLOCK_REALTIME, &after);


      elapsed_nsecs = (after.tv_sec - before.tv_sec) * 1000000000 +
                      (after.tv_nsec - before.tv_nsec);

      printf("Elapsed time %0.2f Segment Size %d\n", elapsed_nsecs/1000000000.,
      currSize);
      */
    }
    catch (const MdsException &exc)
    {
      printf("Class FAUSaveItem: Error saving data  %s\n", exc.what());
    }
  }
};

extern "C" void *FAUhandleSave(void *listPtr);

class FAUSaveList
{
public:
  pthread_t thread;
  bool threadCreated;
  FAUSaveItem *saveHead, *saveTail;
  bool stopReq;
  pthread_mutex_t mutex;
  pthread_cond_t FAUitemAvailable;

public:
  FAUSaveList()
  {
    int status = pthread_mutex_init(&mutex, NULL);
    printf("pthread_mutex_init status %d\n", status);

    status = pthread_cond_init(&FAUitemAvailable, NULL);
    printf("pthread_cond_init status %d %p\n", status, &FAUitemAvailable);

    saveHead = saveTail = NULL;
    stopReq = false;
    threadCreated = false;
  }
  ~FAUSaveList()
  {
    printf("Destructor\n");
  }

  void addItem(uint8_t *value, double *time, double trigTime, int *currSizeA,
               int currSize, size_t nDio, int *dataNids, Tree *treePtr)
  {
    FAUSaveItem *newItem = new FAUSaveItem(value, time, trigTime, currSizeA,
                                           currSize, nDio, dataNids, treePtr);

    pthread_mutex_lock(&mutex);
    if (saveHead == NULL)
      saveHead = saveTail = newItem;
    else
    {
      saveTail->setNext(newItem);
      saveTail = newItem;
    }
    pthread_cond_signal(&FAUitemAvailable);
    pthread_mutex_unlock(&mutex);
  }

  void executeItems()
  {
    int __count = 0;
    printf("Start executeItems\n");

    while (true)
    {
      //printf("Get mutex  executeItems\n");
      pthread_mutex_lock(&mutex);
      if (stopReq && saveHead == NULL)
      {
        printf("Stop executeItems\n");
        pthread_mutex_unlock(&mutex);
        pthread_exit(NULL);
      }

      while (saveHead == NULL)
      {
        //printf("Wait  executeItems %p %p\n", this_id, &FAUitemAvailable);
        pthread_cond_wait(&FAUitemAvailable, &mutex);
        if (stopReq && saveHead == NULL)
        {
          printf("Stop executeItems\n");
          pthread_mutex_unlock(&mutex);
          pthread_exit(NULL);
        }
        printf("Exit Wait executeItems\n");
      }
      FAUSaveItem *currItem = saveHead;
      saveHead = saveHead->getNext();

      pthread_mutex_unlock(&mutex);

      // printf("\rFau flush queue %c", simb[__count++ % 5]);
      // fflush(stdout);

      currItem->save();
      delete currItem;
      // printf("Exit executeItems\n");
    }
  }

  int getItemPending()
  {
    int nItems = 0;
    pthread_mutex_lock(&mutex);
    for (FAUSaveItem *itm = saveHead; itm; itm = itm->getNext(), nItems++)
      ;
    pthread_mutex_unlock(&mutex);
    return nItems;
  }

  void start()
  {
    printf("START SAVE THREAD\n");
    pthread_create(&thread, NULL, FAUhandleSave, (void *)this);
    threadCreated = true;
  }

  void stop()
  {
    printf("STOP SAVE THREAD %p %p\n");
    printf("Pendig Items %d\n", getItemPending());
    stopReq = true;
    pthread_cond_signal(&FAUitemAvailable);
    if (threadCreated)
    {
      printf("Join thread\n");
      pthread_join(thread, NULL);
    }
    printf("EXIT SAVE THREAD\n");
  }
};

extern "C" void *FAUhandleSave(void *listPtr)
{
  FAUSaveList *list = (FAUSaveList *)listPtr;
  list->executeItems();
  return NULL;
}

extern "C" void FAUstartSave(void **retList)
{
  printf("FAU Start Queued Save Thread\n");
  FAUSaveList *fauSaveList = new FAUSaveList;
  fauSaveList->start();
  *retList = (void *)fauSaveList;
}

extern "C" void FAUstopSave(void *listPtr)
{
  printf("FAU Stop Queued Save Thread\n");
  if (listPtr)
  {
    FAUSaveList *list = (FAUSaveList *)listPtr;
    list->stop();
    delete list;
  }
}

/*********************/

NiFpga_Status crioFauInit(NiFpga_Session *session, size_t FifoDepthSize)
{
  NiFpga_Status status = NiFpga_Status_Success;
  NiFpga_Status status1 = NiFpga_Status_Success;
  size_t requestedDepth, actualDepth;

  /* opens a session, downloads the bitstream, and runs the FPGA */
  printf("Opening a session... %s \n", NiFpga_FAU_cRIO_FPGA_Bitfile);

  NiFpga_MergeStatus(
      &status, status = NiFpga_Open(NiFpga_FAU_cRIO_FPGA_Bitfile,
                                    NiFpga_FAU_cRIO_FPGA_Signature, "RIO0",
                                    NiFpga_OpenAttribute_NoRun, session));

  if (NiFpga_IsNotError(status))
  {
    NiFpga_MergeStatus(
        &status,
        NiFpga_ConfigureFifo2(
            *session, NiFpga_FAU_cRIO_FPGA_TargetToHostFifoU64_AcquisitionFIFOR,
            FifoDepthSize, &actualDepth));

    printf("Host FIFO %d data resize %d status %d\n", actualDepth,
           FifoDepthSize, status);

    if (NiFpga_IsError(status))
    {
      NiFpga_Close(*session, 0);
      printf("Host FIFO data resize error\n");
      return -1;
    }

    NiFpga_MergeStatus(
        &status, NiFpga_ConfigureFifo2(
                     *session,
                     NiFpga_FAU_cRIO_FPGA_TargetToHostFifoU64_AcquisitionFIFORT,
                     FifoDepthSize, &actualDepth));

    if (NiFpga_IsError(status))
    {
      NiFpga_Close(*session, 0);
      printf("Host FIFO time resize error\n");
      return -1;
    }

    NiFpga_MergeStatus(&status, NiFpga_Reset(*session));

    if (NiFpga_IsError(status))
    {
      NiFpga_Close(*session, 0);
      printf("VI Reset error\n");
      return -1;
    }
  }
  else
  {
    printf("FPGA Open error\n");
    return -1;
  }
  return status;
}

NiFpga_Status closeFauFpgaSession(NiFpga_Session session)
{

  /* CODAC NOTE: NiFpga_Initialize() is no longer necessary! */
  /* must be called before any other calls */
  // printf("FPGA Initializing...\n");
  // NiFpga_Status status = NiFpga_Initialize();

  NiFpga_Status status = NiFpga_Status_Success;

  printf("Idle Device ...\n");
  NiFpga_MergeStatus(
      &status,
      NiFpga_WriteBool(session, NiFpga_FAU_cRIO_FPGA_ControlBool_goToIdle, 1));

  printf("Stop acquisition...\n");
  NiFpga_MergeStatus(
      &status,
      NiFpga_WriteBool(session, NiFpga_FAU_cRIO_FPGA_ControlBool_AcqEna, 0));

  /* close the session now that we're done */
  printf("Closing the session...\n");
  NiFpga_MergeStatus(&status, NiFpga_Close(session, 0));

  /* CODAC NOTE: NiFpga_Finalize() is no longer necessary! */
  /* must be called after all other calls */
  // printf("Finalizing...\n");
  // NiFpga_MergeStatus(&status, NiFpga_Finalize());

  return status;
}

NiFpga_Status setFauAcqParam(NiFpga_Session session, uint64_t PTEEnaMask,
                             uint64_t PTECountSlowFastMask,
                             uint32_t PTESlowCount, uint32_t PTEFastCount,
                             uint16_t tickFreqCode)
{
  NiFpga_Status status = NiFpga_Status_Success;
  uint32_t PTEcount;

  // 41640 = 1010 0010 1010 1000
  NiFpga_MergeStatus(&status,
                     NiFpga_WriteU64(session,
                                     NiFpga_FAU_cRIO_FPGA_ControlU64_PTEEnaMask,
                                     PTEEnaMask));

  if (NiFpga_IsError(status))
  {
    printf("Error Setting enable channels Pulse Train Mask");
    return -1;
  }

  NiFpga_MergeStatus(
      &status,
      NiFpga_WriteU64(session, NiFpga_FAU_cRIO_FPGA_ControlU64_PTEFastSlowMask,
                      PTECountSlowFastMask));

  if (NiFpga_IsError(status))
  {
    printf("Error Setting slow fast channels frequency Pulse Train Mask");
    return -1;
  }

  NiFpga_MergeStatus(
      &status,
      NiFpga_WriteU32(session, NiFpga_FAU_cRIO_FPGA_ControlU32_PTEcountSlow,
                      PTESlowCount));

  if (NiFpga_IsError(status))
  {
    printf("Error Setting Pulse Train slow frequency");
    return -1;
  }

  NiFpga_MergeStatus(
      &status,
      NiFpga_WriteU32(session, NiFpga_FAU_cRIO_FPGA_ControlU32_PTEcountFast,
                      PTEFastCount));

  if (NiFpga_IsError(status))
  {
    printf("Error Setting Pulse Train fast frequency");
    return -1;
  }

  NiFpga_MergeStatus(&status,
                     NiFpga_WriteU16(session,
                                     NiFpga_FAU_cRIO_FPGA_ControlU16_CountFreq,
                                     tickFreqCode));
  if (NiFpga_IsError(status))
  {
    printf("Error Setting time base tick frequency");
    return -1;
  }

  NiFpga_MergeStatus(
      &status,
      NiFpga_WriteBool(session, NiFpga_FAU_cRIO_FPGA_ControlBool_goToIdle, 1));

  if (NiFpga_IsError(status))
  {
    printf("Error setting idle state");
    return -1;
  }

  NiFpga_MergeStatus(
      &status,
      NiFpga_WriteBool(session, NiFpga_FAU_cRIO_FPGA_ControlBool_AcqEna, 0));

  if (NiFpga_IsError(status))
  {
    printf("Error reset enable acquisition flag");
    return -1;
  }

  return status;
}

/***************************************/

NiFpga_Status startFauAcquisition(NiFpga_Session session)
{
  NiFpga_Status status = NiFpga_Status_Success;

  printf("startFauAcquisition\n");

  NiFpga_MergeStatus(
      &status,
      NiFpga_WriteBool(session, NiFpga_FAU_cRIO_FPGA_ControlBool_AcqEna, 1));

  if (NiFpga_IsError(status))
  {
    printf("Error Setting enable acquisition flag");
    return -1;
  }

  return status;
}

NiFpga_Status pauseFauAcquisition(NiFpga_Session session)
{
  NiFpga_Status status = NiFpga_Status_Success;

  NiFpga_MergeStatus(
      &status,
      NiFpga_WriteBool(session, NiFpga_FAU_cRIO_FPGA_ControlBool_AcqEna, 0));

  if (NiFpga_IsError(status))
  {
    printf("Error Setting enable acquisition flag");
    return -1;
  }

  return status;
}

#define MAX_COUNT 1000
#define MIN_SEGMENT 20000

int fauQueuedAcqData(NiFpga_Session session, void *fauList, double tickPeriod,
                     double trigTime, size_t maxSamp, size_t nDio,
                     void *treePtr, void *dataNidPtr, int *stopFlag)
{

  NiFpga_Status status = NiFpga_Status_Success;
  int currSize = 0;
  int *dataNids = (int *)dataNidPtr;
  uint8_t *data;
  double *time;
  int __count = 0;

  if (nDio > 64)
  {
    printf("Invalid arguments\n");
    return -1;
  }

  if (maxSamp < MIN_SEGMENT)
    maxSamp = MIN_SEGMENT;

  data = (uint8_t *)calloc(maxSamp * nDio, sizeof(uint8_t));
  time = (double *) calloc(maxSamp, sizeof(double));


  while ((!*(int *)stopFlag) == 1)
  {

    currSize = readFauFifoData(session, data, time, tickPeriod, &maxSamp, nDio, stopFlag);

    printf("curr size %d nDio %d maxSamp %d \n", currSize, nDio, maxSamp);
    if (currSize > 0)
    {
      //printf("\rFau Acq %c", simb[__count++ % 5]);
      //fflush(stdout);

      uint8_t b;
      uint8_t *dataQ = (uint8_t *)calloc(currSize * nDio, sizeof(uint8_t));
      double  *timeQ = (double *) calloc(currSize * nDio, sizeof(double));
      int *currSizeA = (int *)    calloc(nDio, sizeof(int));

      for (int i = 0; i < currSize; i++)
      {
        for (int j = 0; j < nDio; j++)
        {

          b = data[i + j * maxSamp];
          if (i == 0 || i == (currSize - 1) || (i >= 1 && data[(i - 1) + j * maxSamp] != b))
          {
            dataQ[currSizeA[j] + j * currSize] = b;
            timeQ[currSizeA[j] + j * currSize] = time[i] + trigTime;
            currSizeA[j]++;
          }
        }
      }
      ((FAUSaveList *)fauList)->addItem(dataQ, timeQ, trigTime, currSizeA, currSize, nDio, dataNids, (Tree *)treePtr);
    }
  }

  free(data);
  free(time);

  printf("EXIT from fauQueuedAcqData %d \n", (*(int *)stopFlag));

  return 0;
}

int fauSaveAcqData(NiFpga_Session session, double tickPeriod, double trigTime,
                   size_t maxSamp, size_t nDio, void *treePtr, void *dataNidPtr,
                   int *stopFlag)
{
  NiFpga_Status status = NiFpga_Status_Success;
  int currSize = 0;
  int *currSizeA;
  uint8_t *data, b;
  double *time;
  double *timeDio;
  int *dataNids = (int *)dataNidPtr;

  data = (uint8_t *)calloc(maxSamp * nDio, sizeof(uint8_t));
  time = (double *) calloc(maxSamp, sizeof(double));

  currSize = readFauFifoData(session, data, time, tickPeriod, &maxSamp, nDio, stopFlag);

  timeDio = (double *)calloc(currSize * nDio, sizeof(double));
  currSizeA = (int *)calloc(nDio, sizeof(int));

  for (int i = 0; i < currSize; i++)
  {
    for (int j = 0; j < nDio; j++)
    {
      b = data[i + j * maxSamp];
      if (i == 0 || i == (currSize - 1) || (i >= 1 && data[(i - 1) + j * maxSamp] != b))
      {
        data[currSizeA[j] + j * maxSamp] = b;
        timeDio[currSizeA[j] + j * currSize] = time[i] + trigTime;
        currSizeA[j]++;
      }
    }
  }

  // currSize = 0;

  if (currSize > 0)
  {
    printf("fauSaveAcqData %d maxSamp %d \n", currSize, maxSamp);

    Float64Array *timeArrData;
    Data *start;
    Data *end;
    Uint8Array *dataArrData;

    TreeNode *currNode[NUM_DIO];
    for (int i = 0; i < NUM_DIO; i++)
    {
      try
      {
        currNode[i] = new TreeNode(dataNids[i], (Tree *)treePtr);
      }
      catch (const MdsException &exc)
      {
        printf("Error collecting data nodes %s\n", exc.what());
      }
    }

    for (int j = 0; j < nDio; j++)
    {
      timeArrData = new Float64Array(&timeDio[j * currSize], currSizeA[j]);
      start = new Float64(timeDio[j * currSize]);
      end = new Float64(timeDio[(currSizeA[j] - 1) + j * currSize]);

      dataArrData = new Uint8Array(&data[j * maxSamp], currSizeA[j]);
   // currNode[j]->beginSegment(start, end, timeArrData, dataArrData);
      currNode[j]->makeSegment(start, end, timeArrData, dataArrData);

      delete dataArrData;
      delete currNode[j];
      delete timeArrData;
      delete start;
      delete end;
      // if(j == 7 ) printf("j %d ts %lf te %lf size %d\n", j, timeDio[j *
      // currSize],timeDio[ (currSizeA[j] - 1) + j * currSize ],currSizeA[j]);
    }
  }

  free(currSizeA);
  free(timeDio);
  free(data);
  free(time);

  return currSize;
}

int readFauFifoData(NiFpga_Session session, uint8_t *data, double *time,
                    double tickPeriod, size_t *maxSampPtr, size_t nDio,
                    int *stopFlag)
{
  NiFpga_Status status = NiFpga_Status_Success;
  size_t currSize = 0;
  uint32_t count = 0, noDataCount = 0;
  size_t nElem, nTime;
  size_t currElem, rElem;
  uint64_t dummy;
  uint16_t acqState;
  size_t maxSamp;

  int __count = 0;

  uint64_t elem[MAX_FPGA_READ_BUF_SIZE];
  uint64_t tickTime[MAX_FPGA_READ_BUF_SIZE];

  if (data == NULL || time == NULL || nDio > 64 || stopFlag == NULL)
  {
    printf("Invalid arguments\n");
    return -1;
  }

  maxSamp = *maxSampPtr;

  noDataCount = 0;
  // while( currSize < maxSamp && count < MAX_COUNT && !( count > MAX_COUNT/2 &&
  // currSize > MIN_SEGMENT ) )
  while (currSize < maxSamp && count < MAX_COUNT && currSize < MIN_SEGMENT && *(int *)stopFlag == 0)
  {

    count++;
/*    
            if( __count % 1 == 0 )
            {
                 printf("+");
                 fflush(stdout);
            }
            __count++;
*/    
    // Read number of elements in the queue
    NiFpga_MergeStatus(
        &status,
        NiFpga_ReadFifoU64(
            session, NiFpga_FAU_cRIO_FPGA_TargetToHostFifoU64_AcquisitionFIFOR,
            &dummy, 0, 0, &nElem));


    if (NiFpga_IsError(status))
    {
      printf("Read Error\n");
      return -1;
    }

    if (nElem == 0)
    {
      noDataCount++;
      usleep(1000);
      // Acquisition isn't enabled
      if (getFauAcqState(session, &acqState) < 0 || acqState == 0)
        break;
      continue;
    }

    /*
    One sample is leave in the FIFO to avoid the error:
    −61219 	NiFpga_Status_ElementsNotPermissibleToBeAcquired

    The number of elements requested must be less than or equal to the number
    of unacquired elements left in the host memory DMA FIFO. There are currently
    fewer unacquired elements left in the FIFO than are being requested.
    Release some acquired elements before acquiring more elements.

    THIS IS REQUIRED ONLY FOR CODAC CORE V5.X

    */
    nElem -= 1;
    if (nElem > 10)
      nElem -= 5;

    NiFpga_MergeStatus(
        &status,
        NiFpga_ReadFifoU64(
            session, NiFpga_FAU_cRIO_FPGA_TargetToHostFifoU64_AcquisitionFIFORT,
            &dummy, 0, 0, &nTime));

    if (NiFpga_IsError(status))
    {
      printf("Read Error\n");
      return -1;
    }

    if (nElem >= nTime)
    {
      // if (nElem > nTime)
      //       printf("Difference nElem %d nTime %d\n", nElem,nTime);
      nElem = nTime;
    }
    /*
            else
                            printf("Difference nElem %d nTime %d\n",
       nElem,nTime); continue;
    */
    //	    if( nElem != nTime )
    //	    {
    //		    printf("diff nElem %d nTime %d\n",nElem,nTime);
    //		}

    // currElem = ( nElem < maxSamp ) ? nElem : maxSamp;
    currElem = (nElem + currSize < maxSamp) ? nElem : maxSamp - currSize;

    // printf("masSmp %d nElem %ld ,nTime %ld, currElem %ld currSize %d\n",
    // maxSamp, nElem, nTime, currElem, currSize);

    rElem = 0;
    NiFpga_MergeStatus(
        &status,
        NiFpga_ReadFifoU64(
            session, NiFpga_FAU_cRIO_FPGA_TargetToHostFifoU64_AcquisitionFIFOR,
            &elem[currSize], currElem, 500,
            &rElem)); // FIFO Remaining Element

    if (NiFpga_IsError(status))
    {
      printf("\nNiFpga_ReadFifoU64 Read Error data %d elem %d\n", status,
             currElem);
      printf("DataElem %d TimeElem %d ReadElem %d Remaining Elem %d \n", nElem,
             nTime, currElem, rElem);
      return -1;
    }

    rElem = 0;
    NiFpga_MergeStatus(
        &status,
        NiFpga_ReadFifoU64(
            session, NiFpga_FAU_cRIO_FPGA_TargetToHostFifoU64_AcquisitionFIFORT,
            &tickTime[currSize],
            currElem, // nTime,
            500, &rElem));

    if (NiFpga_IsError(status))
    {
      printf("\nNiFpga_ReadFifoU64 Read Error time %d elem %d\n ", status,
             currElem);
      printf("DataElem %d TimeElem %d ReadElem %d Remaining Elem %d \n", nElem,
             nTime, currElem, rElem);
      return -1;
    }

    usleep(100);

    currSize += currElem;
  }

  if (currSize > 0)
  {
    // printf("====== Complete Segment %d count %d noDataCount %d\n\n",
    // currSize, count, noDataCount);

    for (int i = 0; i < currSize; i++)
    {

      // if(i < 10 )
      //  printf("%d ",i);
      for (int j = 0; j < nDio; j++)
      {
        data[i + j * maxSamp] = (elem[i] & (((uint64_t)1) << j)) ? 1 : 0;

        // if(i < 10 )
        //   printf("%d", data[i + j * maxSamp] );
      }

      // if(i < 10 )
      //   printf("\n");

      time[i] = (double)(tickTime[i] * tickPeriod);
      // if(i==0) printf("delta %ld %e currSize %ld  count %ld\n ",
      // (tickTime[i+1] - tickTime[i]), time[i], currSize, count);
    }
  }

  return currSize;
}

NiFpga_Status startFauFpga(NiFpga_Session session)
{
  NiFpga_Status status = NiFpga_Status_Success;

  // run the FPGA application
  printf("Running the FPGA...\n");
  NiFpga_MergeStatus(&status, NiFpga_Run(session, 0));

  if (NiFpga_IsError(status))
  {
    printf("Error running FPGA\n");
    return -1;
  }
  else
    printf("OK running FPGA\n");

  return status;
}

NiFpga_Status getFauAcqState(NiFpga_Session session, uint16_t *acqState)
{

  NiFpga_Status status = NiFpga_Status_Success;

  NiFpga_MergeStatus(&status,
                     NiFpga_ReadU16(session,
                                    NiFpga_FAU_cRIO_FPGA_IndicatorU16_AcqState,
                                    acqState));

  if (NiFpga_IsError(status))
  {
    printf("Error reading FAU acquisition state\n");
    return -1;
  }

  return status;
}

uint16_t IsFauFIFOOverflow(NiFpga_Session session)
{

  NiFpga_Status status = NiFpga_Status_Success;
  NiFpga_Bool fifoOverflowed;

  NiFpga_MergeStatus(
      &status, NiFpga_ReadBool(
                   session, NiFpga_FAU_cRIO_FPGA_IndicatorBool_FifoOverflowed,
                   &fifoOverflowed));

  if (NiFpga_IsError(status))
  {
    printf("Error reading FAU FIFO overflow flag\n");
    return 1;
  }

  return fifoOverflowed;
}
