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


using namespace MDSplus;
using namespace std;


class SaveItem
{

private:
  bool isUpdate;
  void *buffer;
  char dataType;
  int bufSize;
  int sampleToRead;
  int segmentSize;
  int counter;
  int dataNid;
  int resampledNid;
  int clockNid;
  float timeIdx0;
  void *treePtr;
  SaveItem *nxt;
  char *eventName;
  int streamFactor;
  double period;
  TreeNode *dataNode;
  TreeNode *clockNode;
  TreeNode *resampledNode;
  char *streamName;
  float streamGain;
  float streamOffset;
  int shot;
  int gain;
  int numCoeffs;
  float *coeffs;
  Data *startPtr;
  Data *endPtr;
  Data *dimPtr;
  Data * dimResPtr;

  vector<string> split(const char *str, char c = ' ')
  {
    vector<string> result;

    do
    {
      const char *begin = str;

      while (*str != c && *str)
        str++;

      result.push_back(string(begin, str));
    } while (0 != *str++);

    return result;
  }

  void sendChannelSegmentPutEvent(TreeNode *dataNode)
  {
    try
    {
      char *path;
      vector<string> tokens;
      char event[256];
      char treeStr[64];

      path = dataNode->getPath();
      tokens = split((const char *)path, ':');
      sprintf(treeStr, "%s", (tokens[0].substr(1)).data());

      tokens = split(tokens[tokens.size() - 2].data(), '.');
      sprintf(event, "%s_%s_CH%s", treeStr, tokens[0].data(),
              (tokens[1].substr(8)).data());
      Event::setevent(event);
    }
    catch (const MdsException &exc)
    {
      printf("Send Event Error: %s\n", exc.what());
    }
  }

public:
  SaveItem(void *buffer, int bufSize, int sampleToRead, char dataType,
           int segmentSize, int counter, int dataNid, int clockNid,
           float timeIdx0, void *treePtr, int shot, int streamFactor,
           char *streamName, float streamGain, float streamOffset,
           double period, float gain, float *coeffs, int numCoeffs,
           int resampledNid);

  SaveItem(int dataNid, void *treePtr, MDSplus::Data *startPtr, 
           MDSplus::Data *endPtr, MDSplus::Data *dimPtr, MDSplus::Data *dimResPtr,  int resampledNid);

  void setNext(SaveItem *itm) { nxt = itm; }
  SaveItem *getNext() { return nxt; }

  int getShot() { return shot; }
  Tree *getTree() { return (Tree *)treePtr; }
  int getDataNid() { return dataNid; }
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
  void addItem(void *buffer, int bufSize, int sampleToRead, char dataType,
               int segmentSize, int counter, int dataNid, int clockNid,
               float trigTime, void *treePtr, int shot, int streamFactor,
               char *streamName, float streamGain, float streamOffset,
               double period, float gain, float *coeffs, int numCoeffs,
               int resampledNid = -1);

  void addItem(int dataNid, void *treePtr, MDSplus::Data *startPtr, 
              MDSplus::Data *endPtr, MDSplus::Data *dimPtr, MDSplus::Data *dimResPtr, int resampledNid);

  void executeItems();
  void start();
  void stop();
};

extern "C" void *handleSave(void *listPtr);
extern "C" void startSave(void **retList);
extern "C" void stopSave(void *listPtr);
extern "C" void reportQueueLen(int dataNid, void *treePtr, int len, int shot);

#endif
