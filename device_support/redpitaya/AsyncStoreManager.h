#ifndef ASYNC_STORE_MANAGER
#define ASYNC_STORE_MANAGER

#define SHORT 1
#define FLOAT 2

#include <mdsobjects.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unordered_map>
#include <vector>

class SaveItem
{

private:
  short *buffer;
  int segmentSamples;
  MDSplus::Tree *treePtr;
  SaveItem *nxt;
  MDSplus::Data *triggerTime;
  MDSplus::TreeNode *dataNode;
  MDSplus::TreeNode *resampledNode;
  double *startTimes, *endTimes;
  double freq;
  int blocksInSegment;
  
public:
  
  SaveItem(short *buffer, int segmentSamples, MDSplus::TreeNode *dataNode, 
                   MDSplus::Data *triggerTime, void *treePtr, 
                  double *startTimes, double *endTimes, double freq, int blocksInSegment,
                  MDSplus::TreeNode *resampledNode = NULL);

  void setNext(SaveItem *itm) { nxt = itm; }
  SaveItem *getNext() { return nxt; }

  MDSplus::Tree *getTree() { return (MDSplus::Tree *)treePtr; }
  void save();
};

class SaveList
{
public:
  pthread_cond_t itemAvailable;
  pthread_t thread;
  bool threadCreated;
  SaveItem *saveHead, *saveTail;
  bool stopReq;
  pthread_mutex_t mutex;

private:
  int getQueueLen()
  {
    pthread_mutex_lock(&mutex);
    int len = 0;
    SaveItem *currItem = saveHead;
    while (currItem)
    {
      len++;
      currItem = currItem->getNext();
    }
    pthread_mutex_unlock(&mutex);
    return len;
  }

public:
  SaveList();

  void addItem(short *buffer, int segmentSamples, MDSplus::TreeNode *dataNode,
               MDSplus::Data *triggerTime, void *treePtr, 
               double *startTimes, double *endTimes, double freq, int blocksInSegment,
               MDSplus::TreeNode *resampledNode = NULL);

  void executeItems();
  void start();
  void stop();
};

extern "C" void *handleSave(void *listPtr);
extern "C" void startSave(void **retList);
extern "C" void stopSave(void *listPtr);

#endif
