#include "cRioMFAUfunction.h"
//#include <time.h>
#include <thread>

using namespace MDSplus;

//#define FAU_DEBUG 1

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
#ifdef FAU_DEBUG
    struct timespec before, after;
    long elapsed_nsecs;
#endif

    /* handle connection */
#ifdef FAU_DEBUG
    printf("FAUSaveItem start saving....\n");
#endif
    Tree *tree = new Tree(((Tree *)treePtr)->getName(), ((Tree *)treePtr)->getShot());

    try
    {
#ifdef FAU_DEBUG
      clock_gettime(CLOCK_REALTIME, &before);
#endif
      for (int j = 0; j < nDio; j++)
      {
        TreeNode *currNode = new TreeNode(((int *)dataNids)[j], (Tree *)tree);
#ifdef FAU_DEBUG
        printf("currNode name: %s\n", currNode->getNodeName());
        printf("currNode nid: %d, path: %s\n", currNode->getNid(), currNode->getFullPath());
#endif
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
#ifdef FAU_DEBUG
        if(j == 7 ) printf("j %d ts %lf te %lf size %d\n", j, time[j * currSize],time[ (currSizeA[j] - 1) + j * currSize ],currSizeA[j]);
#endif
      }

      free(currSizeA);
      free(time);
      free(data);

#ifdef FAU_DEBUG   
      clock_gettime(CLOCK_REALTIME, &after);
      elapsed_nsecs = (after.tv_sec - before.tv_sec) * 1000000000 + (after.tv_nsec - before.tv_nsec);
      printf("Elapsed time %0.2f Segment Size %d\n", elapsed_nsecs/1000000000., currSize);
#endif

    }
    catch (const MdsException &exc)
    {
      printf("Class FAUSaveItem: Error saving data  %s\n", exc.what());
    }
  }
};

extern "C" void *FAU_MiticaHandleSave(void *listPtr);

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
    pthread_create(&thread, NULL, FAU_MiticaHandleSave, (void *)this);
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

extern "C" void *FAU_MiticaHandleSave(void *listPtr)
{
  FAUSaveList *list = (FAUSaveList *)listPtr;
  list->executeItems();
  return NULL;
}

extern "C" void FAU_MiticaStartSave(void **retList)
{
  printf("FAU Start Queued Save Thread\n");
  FAUSaveList *fauSaveList = new FAUSaveList;
  fauSaveList->start();
  *retList = (void *)fauSaveList;
}

extern "C" void FAU_MiticaStopSave(void *listPtr)
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

NiFpga_Status crioFauMiticaInit(NiFpga_Session *session, size_t FifoDepthSize)
{
   *session = 0;

  NiFpga_Status status = NiFpga_Status_Success;
  size_t requestedDepth, actualDepth = 0;

  /* opens a session, downloads the bitstream, and runs the FPGA */
  printf("Opening a session... %s \n", NiFpga_M_FAU_cRIO_FPGA_Bitfile);

  NiFpga_MergeStatus(
      &status, NiFpga_Open(NiFpga_M_FAU_cRIO_FPGA_Bitfile,
                                    NiFpga_M_FAU_cRIO_FPGA_Signature, "RIO0",
                                    NiFpga_OpenAttribute_NoRun, session));   /*NiFpga_OpenAttribute_NoRun = 1 ; 0 is RUN*/
  if (NiFpga_IsError(status)) //20260127 fede
  {
    printf("Open error code: %d \n", (int32_t)status);
    printf("Notice: check NiFpga_M_FAU_cRIO_FPGA_Bitfile has absolute path in header file\n"); 
    return -1;
  }
printf("SESSION: %p\n", session);
  //stop all SCTML vi
  //setFauMiticaStopSCTML(*session, 1);
/*  printf("SET GO TO Idle ...\n");
  NiFpga_MergeStatus(
      &status,
      NiFpga_WriteBool(*session, NiFpga_M_FAU_cRIO_FPGA_ControlBool_goToIdle, 1));
    if (NiFpga_IsError(status))
    {
      NiFpga_Close(*session, 0);
      printf("Error setting go to idle\n");
      return -1;
    }
*/
printf("11\n");

  if (NiFpga_IsNotError(status))
  {
    /*OIU*/
    NiFpga_MergeStatus(
        &status,
        NiFpga_ConfigureFifo2(
            *session, NiFpga_M_FAU_cRIO_FPGA_TargetToHostFifoU64_AcquisitionFIFOOIU,
            FifoDepthSize, &actualDepth));
    printf("Host FIFO %d data resize %d status %d\n", actualDepth,
           FifoDepthSize, status);
    if (NiFpga_IsError(status))
    {
      NiFpga_Close(*session, 0);
      printf("Host OIU FIFO data resize error\n");
      return -1;
    }
printf("22\n");

    /*FLS*/
    NiFpga_MergeStatus(
        &status, NiFpga_ConfigureFifo2(
                     *session,
                     NiFpga_M_FAU_cRIO_FPGA_TargetToHostFifoU64_AcquisitionFIFOFLS,
                     FifoDepthSize, &actualDepth));

    if (NiFpga_IsError(status))
    {
      NiFpga_Close(*session, 0);
      printf("Host FLS FIFO data resize error\n");
      return -1;
    }
printf("33\n");

    /*TIME*/
    NiFpga_MergeStatus(
        &status, NiFpga_ConfigureFifo2(
                     *session,
                     NiFpga_M_FAU_cRIO_FPGA_TargetToHostFifoU64_AcquisitionFIFORT,
                     FifoDepthSize, &actualDepth));

    if (NiFpga_IsError(status))
    {
      NiFpga_Close(*session, 0);
      printf("Host TIME FIFO data resize error\n");
      return -1;
    }
  }
  else
  {
    printf("FPGA Open error\n");
    return -1;
  }
printf("44\n");

  return status;
}

NiFpga_Status closeFauMiticaFpgaSession(NiFpga_Session session)
{

  /* CODAC NOTE: NiFpga_Initialize() is deprecated, no longer necessary! */

  NiFpga_Status status = NiFpga_Status_Success;

  /* close the session now that we're done */
  printf("Closing the session...\n");
  status = NiFpga_Status_Success;
  NiFpga_MergeStatus(&status, NiFpga_Close(session, 0));
  if (NiFpga_IsError(status))
  {
      printf("Session close error\n");
      return -1;
  }

  /* CODAC NOTE: NiFpga_Finalize() is deprecated, no longer necessary! */

  return status;
}

NiFpga_Status setFauMiticaAcqParam(NiFpga_Session session, uint8_t hwAcqEnable, uint8_t syncPtpEnable)
{
  NiFpga_Status status = NiFpga_Status_Success;

  NiFpga_MergeStatus(&status,
                     NiFpga_WriteU16(session,
                                     NiFpga_M_FAU_cRIO_FPGA_ControlBool_HwEnaAcq,
                                     hwAcqEnable));  //1=External Enable 0=Internal Enable via NiFpga_M_FAU_cRIO_FPGA_ControlBool_AcqEna 
  if (NiFpga_IsError(status))
  {
    printf("Error Setting Enable Acquisition Mode");
    return -1;
  }

  NiFpga_MergeStatus(&status,
                     NiFpga_WriteU16(session,
                                     NiFpga_M_FAU_cRIO_FPGA_ControlBool_SyncPtpEna,
                                     syncPtpEnable));  //1=External PTP 100Hz signal enabled 0=ptp sync disabled
  if (NiFpga_IsError(status))
  {
    printf("Error Setting PTP sync Mode");
    return -1;
  }
  return status;
}
/*removed fede - 20251203
NiFpga_Status setFauMiticaAcqParam(NiFpga_Session session, uint64_t PTEEnaMask,
                             uint64_t PTECountSlowFastMask,
                             uint32_t PTESlowCount, uint32_t PTEFastCount,
                             uint16_t tickFreqCode)
{
  NiFpga_Status status = NiFpga_Status_Success;
  uint32_t PTEcount;

  // 41640 = 1010 0010 1010 1000

  NiFpga_MergeStatus(&status,
                     NiFpga_WriteU64(session,
                                     NiFpga_M_FAU_cRIO_FPGA_ControlU64_PTEEnaMask,
                                     PTEEnaMask));

  if (NiFpga_IsError(status))
  {
    printf("Error Setting enable channels Pulse Train Mask");
    return -1;
  }

  NiFpga_MergeStatus(
      &status,
      NiFpga_WriteU64(session, NiFpga_M_FAU_cRIO_FPGA_ControlU64_PTEFastSlowMask,
                      PTECountSlowFastMask));

  if (NiFpga_IsError(status))
  {
    printf("Error Setting slow fast channels frequency Pulse Train Mask");
    return -1;
  }

  NiFpga_MergeStatus(
      &status,
      NiFpga_WriteU32(session, NiFpga_M_FAU_cRIO_FPGA_ControlU32_PTEcountSlow,
                      PTESlowCount));

  if (NiFpga_IsError(status))
  {
    printf("Error Setting Pulse Train slow frequency");
    return -1;
  }

  NiFpga_MergeStatus(
      &status,
      NiFpga_WriteU32(session, NiFpga_M_FAU_cRIO_FPGA_ControlU32_PTEcountFast,
                      PTEFastCount));

  if (NiFpga_IsError(status))
  {
    printf("Error Setting Pulse Train fast frequency");
    return -1;
  }

  NiFpga_MergeStatus(&status,
                     NiFpga_WriteU16(session,
                                     NiFpga_M_FAU_cRIO_FPGA_ControlU16_CountFreq,
                                     tickFreqCode));
  if (NiFpga_IsError(status))
  {
    printf("Error Setting time base tick frequency");
    return -1;
  }

  NiFpga_MergeStatus(
      &status,
      NiFpga_WriteBool(session, NiFpga_M_FAU_cRIO_FPGA_ControlBool_goToIdle, 1));

  if (NiFpga_IsError(status))
  {
    printf("Error setting idle state");
    return -1;
  }

  NiFpga_MergeStatus(
      &status,
      NiFpga_WriteBool(session, NiFpga_M_FAU_cRIO_FPGA_ControlBool_AcqEna, 0));

  if (NiFpga_IsError(status))
  {
    printf("Error reset enable acquisition flag");
    return -1;
  }

  return status;
}
*/
/***************************************/

NiFpga_Status startFauMiticaAcquisition(NiFpga_Session session)
{
  NiFpga_Status status = NiFpga_Status_Success;

  printf("startFauAcquisition\n");

  NiFpga_MergeStatus(
      &status,
      NiFpga_WriteBool(session, NiFpga_M_FAU_cRIO_FPGA_ControlBool_AcqEna, 1));

  if (NiFpga_IsError(status))
  {
    printf("Error Setting enable acquisition flag");
    return -1;
  }

  return status;
}

NiFpga_Status pauseFauMiticaAcquisition(NiFpga_Session session)
{
  NiFpga_Status status = NiFpga_Status_Success;

  NiFpga_MergeStatus(
      &status,
      NiFpga_WriteBool(session, NiFpga_M_FAU_cRIO_FPGA_ControlBool_AcqEna, 0));

  if (NiFpga_IsError(status))
  {
    printf("Error Setting enable acquisition flag");
    return -1;
  }

  return status;
}

#define MAX_COUNT 1000
#define MIN_SEGMENT 20000

int fauMiticaQueuedAcqData(NiFpga_Session session, void *fauList, double tickPeriod,
                     double trigTime, size_t maxSamp, size_t nDioFLS, size_t nDioOIU,
                     void *treePtr, void *dataNidPtr, int *stopFlag)
{

  NiFpga_Status status = NiFpga_Status_Success;
  int currSize = 0;
  int *dataNids = (int *)dataNidPtr;
  uint8_t *dataFLS, *dataOIU;
  double *time;
  size_t totalDio = nDioFLS + nDioOIU;

//  int __count = 0;

  if (totalDio > 120 || nDioFLS > 60 || nDioOIU >60)
  {
    printf("Fede\n");
    printf("Invalid arguments: nDioFLS=%zu nDioOIU=%zu totalDIO=%zu \n", nDioFLS, nDioOIU, totalDio);
    return -1;
  }
 
  if (maxSamp < MIN_SEGMENT)
    maxSamp = MIN_SEGMENT;

  dataFLS = (uint8_t *)calloc(maxSamp * nDioFLS, sizeof(uint8_t));
  dataOIU = (uint8_t *)calloc(maxSamp * nDioOIU, sizeof(uint8_t));
  time = (double *) calloc(maxSamp, sizeof(double));

  if (!dataFLS || !dataOIU || !time)
  {
    printf("Memory allocation failed\n");
    free(dataFLS);
    free(dataOIU);
    free(time);
    return -1;
  }

  while ((!*(int *)stopFlag) == 1)
  {

    currSize = readFauMiticaFifoData(session, dataFLS, dataOIU, time, tickPeriod, &maxSamp, nDioFLS, nDioOIU, stopFlag);

//    printf("curr size:%d nDioFLS:%zu nDioFLS:%zu maxSamp:%zu \n", currSize, nDioFLS, nDioOIU, maxSamp);

    if (currSize > 0)
    {
      //Queue buffer allocation
      uint8_t *dataQ = (uint8_t *)calloc(currSize * totalDio, sizeof(uint8_t));
      double  *timeQ = (double *) calloc(currSize * totalDio, sizeof(double));
      int *currSizeA = (int *)    calloc(totalDio, sizeof(int));

      if(!dataQ || !timeQ || !currSizeA)
      {
        printf("Queue allocation failed\n");
        free(dataQ);
        free(timeQ);
        free(currSizeA);
        continue;
      }

      uint8_t b;

     // ===== COMPRIMI FLS (primi nDioFLS canali) =====
     for (int i = 0; i < currSize; i++)
     {
       for (size_t j = 0; j < nDioFLS; j++)
       {
         b = dataFLS[i + j * maxSamp];               
         //if (i == 0 || i == (currSize - 1) || (i >= 1 && dataFLS[(i - 1) + j * maxSamp] != b))
         {
           dataQ[currSizeA[j] + j * currSize] = b;
           timeQ[currSizeA[j] + j * currSize] = time[i] + trigTime;
           currSizeA[j]++;
         }
       }
     }

     // ===== COMPRIMI OIU (successivi nDioOIU canali) =====
     for (int i = 0; i < currSize; i++)
     {
       for (size_t j = 0; j < nDioOIU; j++)
       {
         size_t idx = j + nDioFLS;  // Offset per OIU
         b = dataOIU[i + j * maxSamp];
         //if (i == 0 || i == (currSize - 1) || (i >= 1 && dataOIU[(i - 1) + j * maxSamp] != b))
         {
           dataQ[currSizeA[idx] + idx * currSize] = b;
           timeQ[currSizeA[idx] + idx * currSize] = time[i] + trigTime;
           currSizeA[idx]++;
         }
       }
     }

      ((FAUSaveList *)fauList)->addItem(dataQ, timeQ, trigTime, currSizeA, currSize, totalDio, dataNids, (Tree *)treePtr);
    }
  }

  free(dataFLS);
  free(dataOIU);
  free(time);

  printf("EXIT from fauQueuedAcqData %d \n", (*(int *)stopFlag));

  return 0;
}

int fauMiticaSaveAcqData(NiFpga_Session session, double tickPeriod, double trigTime,
                   size_t maxSamp, size_t nDioFLS, size_t nDioOIU, void *treePtr, void *dataNidPtr,
                   int *stopFlag)
{
  NiFpga_Status status = NiFpga_Status_Success;
  int currSize = 0;
  int *currSizeA;
  uint8_t *dataFLS, *dataOIU, b;
  double *time;
  double *timeDio;
  int *dataNids = (int *)dataNidPtr;
  size_t totalDio = nDioFLS + nDioOIU;

  if(totalDio > 120 || nDioFLS > 60 || nDioOIU > 60)
  {
    printf("Invalid arguments: nDioFLS=%zu nDioOIU=%zu\n", nDioFLS, nDioOIU);
    return -1;
  }

  dataFLS = (uint8_t *)calloc(maxSamp * nDioFLS, sizeof(uint8_t));
  dataOIU = (uint8_t *)calloc(maxSamp * nDioOIU, sizeof(uint8_t));
  time = (double *) calloc(maxSamp, sizeof(double));

  if (!dataFLS || !dataOIU || !time)
  {
    printf("Memory allocation failed\n");
    free(dataFLS);
    free(dataOIU);
    free(time);
    return -1;
  }

  currSize = readFauMiticaFifoData(session, dataFLS, dataOIU, time, tickPeriod, &maxSamp, nDioFLS, nDioOIU, stopFlag);
  if (currSize <= 0)
  {
    free(dataFLS);
    free(dataOIU);
    free(time);
    return currSize;
  }

  printf("fauSaveAcqData %d maxSamp %zu\n", currSize, maxSamp);

  timeDio = (double *)calloc(currSize * totalDio, sizeof(double));
  currSizeA = (int *)calloc(totalDio, sizeof(int));

  // Array temporaneo per dati compressi (riusa lo stesso buffer)
  uint8_t *dataCompressed = (uint8_t *)calloc(maxSamp * totalDio, sizeof(uint8_t));

  if (!timeDio || !currSizeA || !dataCompressed)
  {
    printf("Compression buffer allocation failed\n");
    free(dataFLS);
    free(dataOIU);
    free(time);
    free(timeDio);
    free(currSizeA);
    free(dataCompressed);
    return -1;
  }

  // ===== COMPRIMI FLS (primi nDioFLS canali) =====
  for (int i = 0; i < currSize; i++)
  {
    for (size_t j = 0; j < nDioFLS; j++)
    {
      b = dataFLS[i + j * maxSamp];
      if (i == 0 || i == (currSize - 1) || (i >= 1 && dataFLS[(i - 1) + j * maxSamp] != b))
      {
        dataCompressed[currSizeA[j] + j * maxSamp] = b;
        timeDio[currSizeA[j] + j * currSize] = time[i] + trigTime;
        currSizeA[j]++;
      }
    }
  }

  // ===== COMPRIMI OIU (successivi nDioOIU canali) =====
  for (int i = 0; i < currSize; i++)
  {
    for (size_t j = 0; j < nDioOIU; j++)
    {
      size_t idx = j + nDioFLS;  // Offset per OIU
      b = dataOIU[i + j * maxSamp];
      if (i == 0 || i == (currSize - 1) || (i >= 1 && dataOIU[(i - 1) + j * maxSamp] != b))
      {
        dataCompressed[currSizeA[idx] + idx * maxSamp] = b;
        timeDio[currSizeA[idx] + idx * currSize] = time[i] + trigTime;
        currSizeA[idx]++;
      }
    }
  }

  // ===== SALVA SU MDSPLUS =====
  Float64Array *timeArrData;
  Data *start;
  Data *end;
  Uint8Array *dataArrData;

  TreeNode *currNode[NUM_DIO];
  
  // Apri tutti i nodi
  for (size_t i = 0; i < totalDio; i++)
  {
    try
    {
      currNode[i] = new TreeNode(dataNids[i], (Tree *)treePtr);
      printf("Curr Node to Save: %s", dataNids[i]);
    }
    catch (const MdsException &exc)
    {
      printf("Error collecting data nodes %s\n", exc.what());
      currNode[i] = NULL;
    }
  }

  // Salva tutti i canali
  for (size_t j = 0; j < totalDio; j++)
  {
    if (currNode[j] == NULL || currSizeA[j] == 0)
      continue;

    try
    {
      timeArrData = new Float64Array(&timeDio[j * currSize], currSizeA[j]);
      start = new Float64(timeDio[j * currSize]);
      end = new Float64(timeDio[(currSizeA[j] - 1) + j * currSize]);

      dataArrData = new Uint8Array(&dataCompressed[j * maxSamp], currSizeA[j]);
      
      currNode[j]->makeSegment(start, end, timeArrData, dataArrData);

      delete dataArrData;
      delete timeArrData;
      delete start;
      delete end;
    }
    catch (const MdsException &exc)
    {
      printf("Error saving channel %zu: %s\n", j, exc.what());
    }
    
    delete currNode[j];
  }

  // Cleanup
  free(currSizeA);
  free(timeDio);
  free(dataCompressed);
  free(dataFLS);
  free(dataOIU);
  free(time);

  return currSize;
}

int readFauMiticaFifoData(NiFpga_Session session, uint8_t *dataFLS, uint8_t *dataOIU, double *time,
                    double tickPeriod, size_t *maxSampPtr, size_t nDioFLS, size_t nDioOIU,
                    int *stopFlag)
{
  NiFpga_Status status = NiFpga_Status_Success;
  size_t currSize = 0;
  uint32_t count = 0, noDataCount = 0;
  size_t nElemFLS, nElemOIU, nTime;
  size_t currElem, rElem;
  uint64_t dummy;
  uint16_t acqState;
  size_t maxSamp;

  int __count = 0;

//  uint64_t elem[MAX_FPGA_READ_BUF_SIZE];
  uint64_t elemOIU[MAX_FPGA_READ_BUF_SIZE];
  uint64_t elemFLS[MAX_FPGA_READ_BUF_SIZE];
  uint64_t tickTime[MAX_FPGA_READ_BUF_SIZE];

  if (dataFLS == NULL || dataOIU == NULL || time == NULL || nDioFLS > 60 || nDioOIU > 60 || stopFlag == NULL)   //64=spider fau, on mitica (9+1)x3x2=60
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
    
    // Read available elements in FLS queue
    NiFpga_MergeStatus(
        &status,
        NiFpga_ReadFifoU64(
            session, NiFpga_M_FAU_cRIO_FPGA_TargetToHostFifoU64_AcquisitionFIFOFLS,
            &dummy, 0, 0, &nElemFLS));


    if (NiFpga_IsError(status))
    {
      printf("Read Error FLS FIFO\n");
      return -1;
    }

    // Read available elements in OIU queue
    NiFpga_MergeStatus(
        &status,
        NiFpga_ReadFifoU64(
            session, NiFpga_M_FAU_cRIO_FPGA_TargetToHostFifoU64_AcquisitionFIFOOIU,
            &dummy, 0, 0, &nElemOIU));


    if (NiFpga_IsError(status))
    {
      printf("Read Error OIU FIFO\n");
      return -1;
    }

    // Read available elements in TIME queue
    NiFpga_MergeStatus(
        &status,
        NiFpga_ReadFifoU64(
            session, NiFpga_M_FAU_cRIO_FPGA_TargetToHostFifoU64_AcquisitionFIFORT,
            &dummy, 0, 0, &nTime));


    if (NiFpga_IsError(status))
    {
      printf("Read Error RT FIFO\n");
      return -1;
    }

    // wait if no data available on fifo
    if (nElemFLS == 0 && nElemOIU == 0 && nTime == 0)
    {
      noDataCount++;
      usleep(1000);
      if (getFauMiticaAcqState(session, &acqState) < 0 || acqState == 0)
        break;
      continue;
    }

    // check the min number of elements in the 3 FIFO to avoid error and out of sync
    size_t nElem = nElemFLS;
    if (nElemOIU < nElem) nElem = nElemOIU;
    if (nTime < nElem) nElem = nTime;

    if (nElem == 0)
    {
      noDataCount++;
      usleep(1000);
      if (getFauMiticaAcqState(session, &acqState) < 0 || acqState == 0)
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

    
    nElem -= 1;
    if (nElem > 10) nElem -= 5;
    
    Ho rimosso questa condizione in quanto la CODAC CORE dove operiamo e' la 7.2
    2026 01 26 CT 
    */

    // elements to read
    currElem = (nElem + currSize < maxSamp) ? nElem : maxSamp - currSize;

    //Read FLS data
    rElem = 0;
    NiFpga_MergeStatus(
        &status,
        NiFpga_ReadFifoU64(
            session, 
            NiFpga_M_FAU_cRIO_FPGA_TargetToHostFifoU64_AcquisitionFIFOFLS,
            &elemFLS[currSize], currElem, 500, &rElem));

    if (NiFpga_IsError(status))
    {
      printf("\nNiFpga_ReadFifoU64 Read Error FLS %d elem %zu\n", 
             status, currElem);
      printf("FLSElem %zu OIUElem %zu TimeElem %zu ReadElem %zu Remaining %zu\n", 
             nElemFLS, nElemOIU, nTime, currElem, rElem);
      return -1;
    }

    //Read OIU data
    rElem = 0;
    NiFpga_MergeStatus(
        &status,
        NiFpga_ReadFifoU64(
            session, 
            NiFpga_M_FAU_cRIO_FPGA_TargetToHostFifoU64_AcquisitionFIFOOIU,
            &elemOIU[currSize], currElem, 500, &rElem));

    if (NiFpga_IsError(status))
    {
      printf("\nNiFpga_ReadFifoU64 Read Error OIU %d elem %zu\n", 
             status, currElem);
      printf("FLSElem %zu OIUElem %zu TimeElem %zu ReadElem %zu Remaining %zu\n", 
             nElemFLS, nElemOIU, nTime, currElem, rElem);
      return -1;
    }

    //Read TIME data
    rElem = 0;
    NiFpga_MergeStatus(
        &status,
        NiFpga_ReadFifoU64(
            session, 
            NiFpga_M_FAU_cRIO_FPGA_TargetToHostFifoU64_AcquisitionFIFORT,
            &tickTime[currSize], currElem, 500, &rElem));

    if (NiFpga_IsError(status))
    {
      printf("\nNiFpga_ReadFifoU64 Read Error TIME %d elem %zu\n", 
             status, currElem);
      printf("FLSElem %zu OIUElem %zu TimeElem %zu ReadElem %zu Remaining %zu\n", 
             nElemFLS, nElemOIU, nTime, currElem, rElem);
      return -1;
    }


    usleep(100);
    currSize += currElem;
  }//while loop acq

  if (currSize > 0)
  {
    printf("====== Complete Segment %zu count %u noDataCount %u\n", 
           currSize, count, noDataCount);
    printf("       nDioFLS=%zu nDioOIU=%zu\n", nDioFLS, nDioOIU);

    for (size_t i = 0; i < currSize; i++)
    {
      // Unpack FLS (60 canali da OPIF 1-2)
      for (size_t j = 0; j < nDioFLS; j++)
      {
        dataFLS[i + j * maxSamp] = (elemFLS[i] & (((uint64_t)1) << j)) ? 1 : 0;
      }

      // Unpack OIU (60 canali da OPIF 3-4)
      for (size_t j = 0; j < nDioOIU; j++)
      {
        dataOIU[i + j * maxSamp] = (elemOIU[i] & (((uint64_t)1) << j)) ? 1 : 0;
      }

      // Timestamp comune
      time[i] = (double)(tickTime[i] * tickPeriod);
      printf("tickPeriod=%f\n",tickPeriod);
      printf("tickTime[%d]=%zu \n",i,tickTime[i]);
      printf("time=%f\n",time[i]);
    }
  }

  return (int)currSize;
}



NiFpga_Status startFauMiticaFpga(NiFpga_Session session)
{
  NiFpga_Status status = NiFpga_Status_Success;

  NiFpga_MergeStatus(
      &status,
      NiFpga_WriteBool(session, NiFpga_M_FAU_cRIO_FPGA_ControlBool_AcqEna, 0));

  if (NiFpga_IsError(status))
  {
    printf("Error reset enable acquisition flag");
    return -1;
  }

  // run the FPGA application
  printf("Running the FPGA...\n");
  NiFpga_MergeStatus(&status, NiFpga_Run(session, 0));  //1=return when complete run. In this case never return!!! Use 0.

  if (NiFpga_IsError(status))
  {
    printf("Error running FPGA\n");
    return -1;
  }
  else
    printf("OK running FPGA\n");

  //release stop  SCTML VI
  //setFauMiticaStopSCTML(session, 0);

  return status;
}

NiFpga_Status getFauMiticaAcqState(NiFpga_Session session, uint16_t *acqState)
{

  NiFpga_Status status = NiFpga_Status_Success;

  NiFpga_MergeStatus(&status,
                     NiFpga_ReadU16(session,
                                    NiFpga_M_FAU_cRIO_FPGA_IndicatorU16_AcqState,
                                    acqState));

  if (NiFpga_IsError(status))
  {
    printf("Error reading FAU acquisition state\n");
    return -1;
  }

  return status;
}

uint16_t IsFauMiticaFIFOOverflow(NiFpga_Session session)
{

  NiFpga_Status status = NiFpga_Status_Success;
  NiFpga_Bool fifoOverflowed;

  NiFpga_MergeStatus(
      &status, NiFpga_ReadBool(
                   session, NiFpga_M_FAU_cRIO_FPGA_IndicatorBool_FifoOverflowed,
                   &fifoOverflowed));

  if (NiFpga_IsError(status))
  {
    printf("Error reading FAU FIFO overflow flag\n");
    return 1;
  }

  return fifoOverflowed;
}

NiFpga_Status setFauMiticaStopSCTML(NiFpga_Session session, uint8_t stopSCTML)
{
  printf("StopSCTML value: %d \n", stopSCTML);
  NiFpga_Status status = NiFpga_Status_Success;
  NiFpga_MergeStatus(&status,NiFpga_WriteBool(session, NiFpga_M_FAU_cRIO_FPGA_ControlBool_stopSCTML, (NiFpga_Bool)stopSCTML));

  if (NiFpga_IsError(status))
  {
    NiFpga_Close(session, 0);
    printf("Error in setFauMiticaStopSCTML...\n");
    return -1;
  }
  return status;
}

NiFpga_Status setFauMiticaTestTX1(NiFpga_Session session, uint16_t mode, uint32_t tickCnt, uint16_t op)
{
  NiFpga_Status status = NiFpga_Status_Success;

  NiFpga_MergeStatus(&status,NiFpga_WriteU32(session, NiFpga_M_FAU_cRIO_FPGA_ControlU32_TX1_tk_sp, tickCnt));
  if (NiFpga_IsError(status))
  {
    NiFpga_Close(session, 0);
    printf("Error Setting TX1 tick on setFauMiticaTestTX1...\n");
    return -1;
  }

  NiFpga_MergeStatus(&status,NiFpga_WriteU16(session, NiFpga_M_FAU_cRIO_FPGA_ControlU16_TX1_op, op));
  if (NiFpga_IsError(status))
  {
    NiFpga_Close(session, 0);
    printf("Error Setting TX1 op function on setFauMiticaTestTX1...\n");
    return -1;
  }

  NiFpga_MergeStatus(&status,NiFpga_WriteU16(session, NiFpga_M_FAU_cRIO_FPGA_ControlU16_TX1_mode, mode));
  if (NiFpga_IsError(status))
  {
    printf("Error Setting TX1 mode on setFauMiticaTestTX1...\n");
    return -1;
  }

  return status;
}

NiFpga_Status setFauMiticaTestTX2(NiFpga_Session session, uint16_t mode, uint32_t tickCnt, uint16_t op)
{
  NiFpga_Status status = NiFpga_Status_Success;

  NiFpga_MergeStatus(&status,NiFpga_WriteU32(session, NiFpga_M_FAU_cRIO_FPGA_ControlU32_TX2_tk_sp, tickCnt));
  if (NiFpga_IsError(status))
  {
    NiFpga_Close(session, 0);
    printf("Error Setting TX2 tick on setFauMiticaTestTX2...\n");
    return -1;
  }

  NiFpga_MergeStatus(&status,NiFpga_WriteU16(session, NiFpga_M_FAU_cRIO_FPGA_ControlU16_TX2_op, op));
  if (NiFpga_IsError(status))
  {
    NiFpga_Close(session, 0);
    printf("Error Setting TX2 op function on setFauMiticaTestTX2...\n");
    return -1;
  }

  NiFpga_MergeStatus(&status,NiFpga_WriteU16(session, NiFpga_M_FAU_cRIO_FPGA_ControlU16_TX2_mode, mode));
  if (NiFpga_IsError(status))
  {
    printf("Error Setting TX2 mode on setFauMiticaTestTX2...\n");
    return -1;
  }

  return status;
}

NiFpga_Status setFauMiticaTestTX3(NiFpga_Session session, uint16_t mode, uint32_t tickCnt, uint16_t op)
{
  NiFpga_Status status = NiFpga_Status_Success;

  NiFpga_MergeStatus(&status,NiFpga_WriteU32(session, NiFpga_M_FAU_cRIO_FPGA_ControlU32_TX3_tk_sp, tickCnt));
  if (NiFpga_IsError(status))
  {
    NiFpga_Close(session, 0);
    printf("Error Setting TX3 tick on setFauMiticaTestTX3...\n");
    return -1;
  }

  NiFpga_MergeStatus(&status,NiFpga_WriteU16(session, NiFpga_M_FAU_cRIO_FPGA_ControlU16_TX3_op, op));
  if (NiFpga_IsError(status))
  {
    NiFpga_Close(session, 0);
    printf("Error Setting TX3 op function on setFauMiticaTestTX3...\n");
    return -1;
  }

  NiFpga_MergeStatus(&status,NiFpga_WriteU16(session, NiFpga_M_FAU_cRIO_FPGA_ControlU16_TX3_mode, mode));
  if (NiFpga_IsError(status))
  {
    printf("Error Setting TX1 mode on setFauMiticaTestTX3...\n");
    return -1;
  }

  return status;
}


