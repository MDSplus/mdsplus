#include "AsyncStoreManager.h"

pthread_mutex_t globalMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t segmentMutex = PTHREAD_MUTEX_INITIALIZER;

SaveItem::SaveItem(short *buffer, int segmentSamples, MDSplus::TreeNode *dataNode,MDSplus::TreeNode *trigRecvNode,
                   MDSplus::Data *triggerTime, void *treePtr,
                   double *startTimes, double *endTimes, double freq, int blocksInSegment,
                   char *trigReceived, MDSplus::TreeNode *resampledNode)
{
  this->buffer = buffer;
  this->segmentSamples = segmentSamples;
  this->dataNode = dataNode;
  this->triggerTime = triggerTime;
  this->treePtr = (MDSplus::Tree *)treePtr;
  this->blocksInSegment = blocksInSegment;
  this->startTimes = new double[blocksInSegment];
  this->endTimes = new double[blocksInSegment];
  for (int i = 0; i < blocksInSegment; i++)
  {
    this->startTimes[i] = startTimes[i];
    this->endTimes[i] = endTimes[i];
  }
  this->freq = freq;
  this->resampledNode = resampledNode;
  this->trigReceived = trigReceived;
  this->trigRecvNode = trigRecvNode;
  nxt = 0;
}

void SaveItem::save()
{

  MDSplus::Array *chanData =
      new MDSplus::Int16Array(buffer, segmentSamples);

  MDSplus::Data *timebase;
  MDSplus::Data *startTimeData;
  MDSplus::Data *endTimeData;
  MDSplus::Data *deltaData = new MDSplus::Float64(1. / freq);
  if (blocksInSegment == 1)
  {
    startTimeData = new MDSplus::Float64(startTimes[0]);
    endTimeData = new MDSplus::Float64(endTimes[0]);
  }
  else
  {
    startTimeData = new MDSplus::Float64Array(startTimes, blocksInSegment);
    endTimeData = new MDSplus::Float64Array(endTimes, blocksInSegment);
  }
  timebase = MDSplus::compileWithArgs(
      "make_range($1+$2, $3+$4-$5/2., $6)", treePtr, 6, startTimeData, triggerTime,
      endTimeData, triggerTime, deltaData, deltaData);
  deleteData(startTimeData);
  deleteData(endTimeData);
  deleteData(deltaData);

  MDSplus::Data *startData = new MDSplus::Float64(startTimes[0]);
  MDSplus::Data *endData = new MDSplus::Float64(endTimes[blocksInSegment - 1]);
  MDSplus::Data *startSegData =
      MDSplus::compileWithArgs("$1+$2", treePtr, 2, startData, triggerTime);
  MDSplus::Data *endSegData =
      MDSplus::compileWithArgs("$1+$2", treePtr, 2, endData, triggerTime);
  try
  {
    std::cout << "MAKE SEGMENT  SAMPLES:" << segmentSamples << std::endl;
    dataNode->makeSegment(startSegData, endSegData, timebase, chanData);
  }
  catch (MDSplus::MdsException &exc)
  {
    std::cout << "Error writing segment: " << exc.what() << std::endl;
  }
  
  
   if(trigRecvNode)
   {
     MDSplus::Float64Array *trigFlagTimes = new MDSplus::Float64Array(startTimes, blocksInSegment);
     MDSplus::Int8Array *trigFlags = new MDSplus::Int8Array(trigReceived, blocksInSegment);
   
     std::cout << "MAKE SEGMENT  FLAGS:" << std::endl;
   
     trigRecvNode->makeSegment(startSegData, endSegData, trigFlagTimes, trigFlags);
     MDSplus::deleteData(trigFlagTimes);
     MDSplus::deleteData(trigFlags);
  }


  
  
  
  MDSplus::deleteData(chanData);
  MDSplus::deleteData(timebase);
  MDSplus::deleteData(startData);
  MDSplus::deleteData(endData);
  MDSplus::deleteData(startSegData);
  MDSplus::deleteData(endSegData);
  delete[] buffer;
  delete[] startTimes;
  delete[] endTimes;
  
  
}






SaveList::SaveList()
{
  int status = pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&itemAvailable, NULL);
  saveHead = saveTail = NULL;
  stopReq = false;
  threadCreated = false;
}

void SaveList::addItem(short *buffer, int segmentSamples, MDSplus::TreeNode *dataNode,MDSplus::TreeNode *trigRecvNode,
                       MDSplus::Data *triggerTime, void *treePtr,
                       double *startTimes, double *endTimes, double freq, int blocksInSegment,
                       char *trigReceived, MDSplus::TreeNode *resampledNode)

{
  SaveItem *newItem = new SaveItem(
      buffer, segmentSamples, dataNode, trigRecvNode, triggerTime, treePtr, startTimes, endTimes, freq, blocksInSegment, trigReceived, resampledNode);
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
    }
    SaveItem *currItem = saveHead;
    saveHead = saveHead->getNext();
    pthread_mutex_unlock(&mutex);
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
    pthread_join(thread, NULL);
    printf("SAVE THREAD TERMINATED\n");
  }
}

void *handleSave(void *listPtr)
{
  SaveList *list = (SaveList *)listPtr;
  list->executeItems();
  return NULL;
}
