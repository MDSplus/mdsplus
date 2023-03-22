#include "AsyncStoreManager.h"
#include<unistd.h>
#include<sys/time.h>

pthread_mutex_t globalMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t segmentMutex = PTHREAD_MUTEX_INITIALIZER;

//#define DEBUG_QUEUE

SaveItem::SaveItem(void *buffer, int bufSize, int sampleToRead, char dataType,
                   int segmentSize, int counter, int dataNid, int clockNid,
                   float timeIdx0, void *treePtr, int shot, int streamFactor,
                   char *streamName, float streamGain, float streamOffset,
                   double period, float gain, float *coeffs, int numCoeffs,
                   int resampledNid = -1)
{
  this->buffer = buffer;
  this->dataType = dataType;
  this->bufSize = bufSize;
  this->sampleToRead = sampleToRead;
  this->segmentSize = segmentSize;
  this->counter = counter;
  this->dataNid = dataNid;
  this->resampledNid = resampledNid;
  this->clockNid = clockNid;
  this->timeIdx0 = timeIdx0;
  this->treePtr = treePtr;
  this->shot = shot;
  this->streamFactor = streamFactor;
  this->streamName = streamName;
  this->streamGain = streamGain;
  this->streamOffset = streamOffset;
  this->period = period;
  this->gain = gain;
  this->numCoeffs = numCoeffs;
  this->coeffs = coeffs;
  nxt = 0;
  isUpdate = false;
}

SaveItem::SaveItem(int dataNid, void *treePtr, MDSplus::Data *startPtr, 
MDSplus::Data *endPtr, MDSplus::Data *dimPtr, MDSplus::Data *dimResPtr,  int resampledNid){
  this->dataNid = dataNid;
  this->treePtr = treePtr;
  this->startPtr = startPtr;
  this->endPtr = endPtr;
  this->dimPtr = dimPtr;
  this->dimResPtr = dimResPtr;
  this->resampledNid = resampledNid;
  nxt = 0;
  isUpdate = true;
}

void SaveItem::save()
{

  // std::cout << "START SAVE" << std::endl;
  //Tree *tree = new Tree(((Tree *)treePtr)->getName(), ((Tree *)treePtr)->getShot());
  Tree *tree = (Tree *)treePtr;
  printf("-");
  fflush(stdout);
  dataNode = new TreeNode(dataNid, tree);
  resampledNode = NULL;
  if (resampledNid > 0)
    resampledNode = new TreeNode(resampledNid, tree);

  if (isUpdate == true)
  {
    std::vector<double> dims = dimPtr->getDoubleArray();
    std::cout << "RES: " << dims.data()[4] << ", " << dims.data()[5] << std::endl;

    pthread_mutex_lock(&segmentMutex);
    dataNode->updateSegment(startPtr, endPtr, dimPtr);
    resampledNode->updateSegment(startPtr, endPtr, dimResPtr);
    pthread_mutex_unlock(&segmentMutex);
    
  }
  else
  {
    int ofs = ceil(timeIdx0 + (counter)*period);
    clockNode = new TreeNode(clockNid, tree);

    // Streaming stuff
    if (streamName && streamFactor > 0 && (counter % streamFactor) == 0)
    {

      // Computation of value to be streamed
      try
      {
        Data *nidData = new Int32(dataNid);
        int sampleInterval = (int)(0.1 / period);
        if (sampleInterval < 1)
          sampleInterval = 1;
        int numSamples = bufSize / sampleInterval;
        if (numSamples < 1)
          numSamples = 1;
        // printf("NUM SAMPLES: %d  SAMPLE INTERVAL: %d\n", numSamples,
        // sampleInterval);
        float *samples = new float[numSamples];
        float *times = new float[numSamples];
        int actSamples = 0;
        for (int sampleIdx = 0; sampleIdx < numSamples; sampleIdx++)
        {
          if (sampleIdx * sampleInterval >= bufSize)
            break;
          float sample =
              (float)((dataType == SHORT)
                          ? ((short *)buffer)[sampleIdx * sampleInterval]
                          : ((float *)buffer)[sampleIdx * sampleInterval]);

          float scaled = coeffs[numCoeffs - 1];
          for (int c = numCoeffs - 2; c >= 0; c--)
          {
            scaled *= sample;
            scaled += coeffs[c];
          }
          scaled /= gain;

          scaled = scaled * streamGain + streamOffset;
          samples[actSamples] = scaled;
          times[actSamples] =
              period * (counter + actSamples * sampleInterval) + timeIdx0;
          actSamples++;
        }
        EventStream::send(shot, streamName, false, actSamples, times, 1, &actSamples, samples);
        delete[] samples;
        delete[] times;
      }
      catch (const MdsException &exc)
      {
        printf("Cannot convert stream sample: %s\n", exc.what());
      }
    }
  // std::cout << "SAVE  1" << std::endl;

    //////////////////////
    // printf("Counter = %d Sample to read = %d SegmentSize = %d  timeIdx0 = %f\n", counter, sampleToRead, segmentSize, timeIdx0 );
    // if((counter % segmentSize) == 0 || ((int)(counter / segmentSize) *
    // segmentSize) < counter + bufSize )
    // if ((counter % segmentSize) == 0 || sampleToRead == 0)
     if (sampleToRead >= segmentSize || counter == 0)
    {
  //  std::cout << "SAVE 2" << std::endl;
    // Create Segment
      Data *startIdx = new Int32(counter);
      Data *endIdx;
      // In Transient Record acquisition mode must set
      // the last segment with the correct size
      if (sampleToRead > 0 && sampleToRead < segmentSize)
        segmentSize = sampleToRead;

      endIdx = new Int32(counter + segmentSize - 1);

      Data *startTime;
      Data *endTime;
      Data *dim;
      if (timeIdx0 != timeIdx0) // is a NaN float
      {
        // printf("Configuration for gclock\n");
        // printf("---------------- time at idx 0 NAN\n");
        startTime =
            compileWithArgs("NIADCClockSegment($1, $2, $3, 0, 'start_time')",
                            tree, 3, clockNode, startIdx, endIdx);
        endTime =
            compileWithArgs("NIADCClockSegment($1, $2, $3, 0, 'end_time')",
                            tree, 3, clockNode, startIdx, endIdx);
        dim = compileWithArgs("NIADCClockSegment($1, $2, $3, 0, 'dim')",
                              tree, 3, clockNode, startIdx, endIdx);
      }
      else
      {
        //  std::cout << "SAVE  3" << std::endl;

        Data *timeAtIdx0 = new Float32(timeIdx0);
        Data *periodData = new Float64(period);
        std::cout << "PERIOD: " << period << std::endl;
        startTime = compileWithArgs(
            "NIADCClockSegment($1, $2, $3, $4, 'start_time', $5)", tree, 5,
            clockNode, startIdx, endIdx, timeAtIdx0, periodData);
        endTime = compileWithArgs("NIADCClockSegment($1, $2, $3, $4, 'end_time', $5)",
                                  tree, 5, clockNode, startIdx, endIdx,
                                  timeAtIdx0, periodData);
        dim = compileWithArgs("NIADCClockSegment($1, $2, $3, $4, 'dim', $5)",
                              tree, 5, clockNode, startIdx, endIdx,
                              timeAtIdx0, periodData);
//  std::cout << "SAVE  4" << std::endl;

/*
        Data *timeAtIdx0 = new Float32(timeIdx0);
        startTime = compileWithArgs(
            "NIADCClockSegment($1, $2, $3, $4, 'start_time')", tree, 4,
            clockNode, startIdx, endIdx, timeAtIdx0);
std::cout << "CHIAMO LA FUN.." << std::endl;
        endTime = compileWithArgs("NIADCClockSegment($1, $2, $3, $4, 'end_time')",
                                  tree, 4, clockNode, startIdx, endIdx,
                                  timeAtIdx0);
std::cout << "CHIAMO LA FUN.." << std::endl;
        dim = compileWithArgs("NIADCClockSegment($1, $2, $3, $4, 'dim')",
                              tree, 4, clockNode, startIdx, endIdx,
                              timeAtIdx0);
std::cout << "CHIAMO LA FUN.." << std::endl;
*/
      }
// std::cout << "SAVE  5" << std::endl;

 //     std::cout << "CURRENTLY SAVING START TIME: " << startTime->getFloat() << ", END TIME: " << endTime->getFloat() << std::endl;
// std::cout << "SAVE  6" << std::endl;
      switch (dataType)
      {
      case SHORT:
      {
        short *fBuf = new short[segmentSize];
        memset(fBuf, 0, sizeof(short) * segmentSize);
        Int16Array *fData = new Int16Array((short *)fBuf, segmentSize);
        pthread_mutex_lock(&segmentMutex);
        try
        {
          if (resampledNode)
            dataNode->beginSegmentMinMax(startTime, endTime, dim, fData,
                                         resampledNode, 100);
          else
            dataNode->beginSegment(startTime, endTime, dim, fData);
        }
        catch (const MdsException &exc)
        {
          printf("BEGIN SEGMENT FAILED FOR NODE %s: %s\n",
                 dataNode->getFullPath(), exc.what());
        }
        pthread_mutex_unlock(&segmentMutex);
        delete[] fBuf;
        deleteData(fData);
      }
      break;
      case FLOAT:
      {
        float *fBuf = new float[segmentSize];
        memset(fBuf, 0, sizeof(float) * segmentSize);
        Float32Array *fData = new Float32Array((float *)fBuf, segmentSize);
        pthread_mutex_lock(&segmentMutex);
        try
        {
          if (resampledNode)
            dataNode->beginSegmentMinMax(startTime, endTime, dim, fData,
                                         resampledNode, 100);
          else
            dataNode->beginSegment(startTime, endTime, dim, fData);
        }
        catch (const MdsException &exc)
        {
          printf("BEGIN SEGMENT FAILED FOR NODE %s: %s\n",
                 dataNode->getFullPath(), exc.what());
        }
        pthread_mutex_unlock(&segmentMutex);
        delete[] fBuf;
        deleteData(fData);
      }
      break;
      }

      deleteData(startIdx);
      deleteData(endIdx);
      deleteData(startTime);
      deleteData(endTime);
    }

    try
    {
      switch (dataType)
      {
      case SHORT:
      {
        // printf("Short Save data %s counter %d\n", dataNode->getPath(), counter);
        Int16Array *data = new Int16Array((short *)buffer, bufSize);

        pthread_mutex_lock(&segmentMutex);
        try
        {
          if (resampledNode)
            dataNode->putSegmentMinMax(data, -1, resampledNode, 100);
          else
            dataNode->putSegment(data, -1);
        }
        catch (const MdsException &exc)
        {
          printf("PUT SEGMENT FAILED FOR NODE: %s: %s\n", dataNode->getFullPath(),
                 exc.what());
        }
        pthread_mutex_unlock(&segmentMutex);
        deleteData(data);
        delete[](short *) buffer;
      }
      break;
      case FLOAT:
      {
        // printf("Float Save data %s counter %d\n", dataNode->getPath(), counter);
        Float32Array *data = new Float32Array((float *)buffer, bufSize);
        pthread_mutex_lock(&segmentMutex);
        try
        {
          if (resampledNode)
            dataNode->putSegmentMinMax(data, -1, resampledNode, 100);
          else
            dataNode->putSegment(data, -1);
        }
        catch (const MdsException &exc)
        {
          printf("PUT SEGMENT FAILED FOR NODE: %s: %s\n", dataNode->getFullPath(),
                 exc.what());
        }
        pthread_mutex_unlock(&segmentMutex);
        deleteData(data);
        delete[](float *) buffer;
      }
      break;
      }

      /* Send Event on Segment update <TreeName>_<DeviceNodeName>_CH<numchannel>*/
      // sendChannelSegmentPutEvent(dataNode);
    }
    catch (const MdsException &exc)
    {
      printf("Cannot put segment: %s\n", exc.what());
    }
    delete clockNode;
  }
  // std::cout << "DELETING DATANODE AND TREE" << std::endl;
  delete dataNode;
  //delete tree;
}

SaveList::SaveList()
{
  int status = pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&itemAvailable, NULL);
  saveHead = saveTail = NULL;
  stopReq = false;
  threadCreated = false;
}

void SaveList::addItem(void *buffer, int bufSize, int sampleToRead,
                       char dataType, int segmentSize, int counter, int dataNid,
                       int clockNid, float trigTime, void *treePtr, int shot,
                       int streamFactor, char *streamName, float streamGain,
                       float streamOffset, double period, float gain,
                       float *coeffs, int numCoeffs, int resampledNid)

{
  SaveItem *newItem = new SaveItem(
      buffer, bufSize, sampleToRead, dataType, segmentSize, counter, dataNid,
      clockNid, trigTime, treePtr, shot, streamFactor, streamName, streamGain,
      streamOffset, period, gain, coeffs, numCoeffs, resampledNid);

  printf("+");
  fflush(stdout);
  
  pthread_mutex_lock(&mutex);

  if (saveHead == NULL){
    saveHead = saveTail = newItem;
  }
  else
  {
    saveTail->setNext(newItem);
    saveTail = newItem;
  }

  pthread_cond_signal(&itemAvailable);
  pthread_mutex_unlock(&mutex);

#ifdef DEBUG_QUEUE
  reportQueueLen(dataNid, treePtr, getQueueLen(), shot);
#endif
}

void SaveList::addItem(int dataNid, void *treePtr, MDSplus::Data *startPtr, 
           MDSplus::Data *endPtr, MDSplus::Data *dimPtr, MDSplus::Data *dimResPtr, int resampledNid)

{
  SaveItem *newItem = new SaveItem(
      dataNid, treePtr, startPtr, endPtr, dimPtr, dimResPtr, resampledNid);

  printf("+");
  fflush(stdout);

  pthread_mutex_lock(&mutex);

  if (saveHead == NULL){
    saveHead = saveTail = newItem;
  }
  else
  {
    saveTail->setNext(newItem);
    saveTail = newItem;
  }

  pthread_cond_signal(&itemAvailable);
  pthread_mutex_unlock(&mutex);

#ifdef DEBUG_QUEUE
  reportQueueLen(dataNid, treePtr, getQueueLen(), 1237);
#endif
}



void SaveList::executeItems()
{
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
      pthread_cond_wait(&itemAvailable, &mutex);
      if (stopReq && saveHead == NULL)
      {
        pthread_mutex_unlock(&mutex);
        pthread_exit(NULL);
      }
      /*
         int nItems = 0;
         for(SaveItem *itm = saveHead; itm; itm = itm->getNext(),
         nItems++); if(nItems > 2) printf("THREAD ACTIVATED: %d items
         pending\n", nItems);
      */
    }
    SaveItem *currItem = saveHead;
    saveHead = saveHead->getNext();

    /*
       int nItems = 0;
       for(SaveItem *itm = saveHead; itm; itm = itm->getNext(),
       nItems++); if(nItems > 2) printf("THREAD ACTIVATED: %d items pending\n",
       nItems);
    */
    pthread_mutex_unlock(&mutex);

#ifdef DEBUG_QUEUE
    reportQueueLen(currItem->getDataNid(), currItem->getTree(), getQueueLen(), currItem->getShot());
#endif
    currItem->save();
    delete currItem;
  }
}
void SaveList::start()
{
  pthread_create(&thread, NULL, handleSave, (void *)this);
  threadCreated = true;
}
void SaveList::stop()
{
  stopReq = true;
  pthread_cond_signal(&itemAvailable);
  if (threadCreated)
  {
    struct timeval t1, t2;
    double elapsedTime;
    gettimeofday(&t1, NULL);
    printf("WAITING SAVE THREAD TERMINATION ....\n");

    pthread_join(thread, NULL);

    gettimeofday(&t2, NULL);
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
    printf("SAVE THREAD TERMINATED termination time %f s\n", elapsedTime/1000);  
  }
}

// Queue length streaming support
void reportQueueLen(int dataNid, void *treePtr, int len, int shot)
{
  static long prevTimeMs = 0;
  static int prevShot = -1;
  static unordered_map<int, long> prevTimes;
  static long startTimeMs;
  long currTimeMs;
  struct timespec retTime;
  int debug = 0;

  clock_gettime(CLOCK_REALTIME, &retTime);
  currTimeMs = retTime.tv_sec * 1000 + retTime.tv_nsec / 1000000;
/*
  pthread_mutex_lock(&globalMutex);
  if (shot != prevShot)
  {
    prevTimes.clear();
    prevShot = shot;
    startTimeMs = currTimeMs;
  }

  if (prevTimes.count(dataNid) > 0 && currTimeMs <= prevTimes[dataNid])
  {
    pthread_mutex_unlock(&globalMutex);
    return;
  }
  prevTimes[dataNid] = currTimeMs;
  pthread_mutex_unlock(&globalMutex);

  TreeNode *currNode = new TreeNode(dataNid, (Tree *)treePtr);
  if (debug)
     std::cout << "INFO: data node reportQueueLen "
               << currNode->getPath() << " " << std::endl;

  while (currNode->getNid() > 0 && strcmp(currNode->getUsage(), "DEVICE"))
  {
    TreeNode *prevNode = currNode;
    currNode = currNode->getParent();
    delete prevNode;
  }
  if (currNode->getNid() == 0)
  {
    if (debug)
      std::cout << "INTERNAL ERROR: wrong data nid to reportQueueLen"
                << std::endl;
    return;
  }
  StringArray *tags = currNode->findTags();
  if (tags->getSize() < 1)
  {
    if (debug)
      std::cout << "WARNING: no tags for data node " << currNode->getPath() << " to reportQueueLen"
                << std::endl;
    return;
  }
  String *tagData = (String *)tags->getElementAt(0);
  deleteData(tags);
  char *tag = tagData->getString();
  deleteData(tagData);
  char *buf = new char[strlen(tag) + 7];
  sprintf(buf, "%s_QUEUE", tag);
*/
  //delete[] tag;
  //EventStream::send(shot, buf, (float)((currTimeMs - startTimeMs) / 1000.), (float)len);
  EventStream::send(shot, "NI6368_QUEUE", (float)((currTimeMs - startTimeMs) / 1000.),
                    (float)len);
  //delete[] buf;
}

void *handleSave(void *listPtr)
{
  SaveList *list = (SaveList *)listPtr;
  list->executeItems();
  return NULL;
}

void startSave(void **retList)
{
  SaveList *saveList = new SaveList;
  saveList->start();
  *retList = (void *)saveList;
}

void stopSave(void *listPtr)
{
  if (listPtr)
  {
    SaveList *list = (SaveList *)listPtr;
    list->stop();
    delete list;
  }
}
