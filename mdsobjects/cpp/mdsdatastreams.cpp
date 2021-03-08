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
#include <pthread.h>
#include <mdsobjects.h>
#include <time.h>
#include <string.h>
#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
#include <iostream>
#include <vector>
using namespace rapidjson;


#ifdef _MSC_VER
inline int nanosleep(const struct timespec *req,
                     struct timespec *rem __attribute__((unused))) {
  DWORD sleep_ms =
      ((DWORD)req->tv_sec * 1000) + ((DWORD)req->tv_nsec / 1000000);
  Sleep(sleep_ms);
  return 0;
}
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class StreamEvents provides an alternative streaming solution. It provides a
// set of methods for sending chuncks of data as MDSplus events. Stream data are sent as a JSON message
// with the following filds:
// name: name of the stream
// shot: reference shot number  
// timestamp: increasing timestamp
// absolute_time: relative (0) / absolute(1) time reference
// dimension: dimension array [0] means scalar; [n] 1D array with n elementd [n, m] 2D (mxn) array
// times: long for absolute times, float for relative times)
// samples: (float, 1D array) the dimension of the values array is related to the number oftimes and 
//     the declared dimension
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
using namespace MDSplus;

static std::vector<std::string> chanNames;
static  std::vector<uint64_t> chanTimestamps;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static uint64_t getAndIncrementTimestamp(const char *stream)
{
  pthread_mutex_lock(&mutex);
  for (size_t i = 0; i < chanNames.size(); i++)
  {
      if (chanNames[i] == stream)
      {
          uint64_t retTimestamp = chanTimestamps[i];
          chanTimestamps[i]++;
          pthread_mutex_unlock(&mutex);
          return retTimestamp;
      }
  }
  chanNames.push_back(std::string(stream));
  chanTimestamps.push_back(1);
  pthread_mutex_unlock(&mutex);
  return 0;
}


EXPORT void EventStream::send(int shot, const char *name, bool isAbsTime, int nTimes, void *times, 
                              int nSamples, float *samples)
{
  Document d;
  d.SetObject();
  Document::AllocatorType& allocator = d.GetAllocator(); 
  Value timesVal(kArrayType);
  Value samplesVal(kArrayType);
  for(int i = 0; i < nSamples; i++)
    samplesVal.PushBack(samples[i], allocator);
  for(int i  = 0; i < nTimes; i++)
  {
    if(isAbsTime)
      timesVal.PushBack(((uint64_t *)times)[i], allocator);
    else
      timesVal.PushBack(((float *)times)[i], allocator);
  }
  Value timestampVal;
  timestampVal.SetInt(getAndIncrementTimestamp(name));
  Value absTimeVal;
  absTimeVal.SetInt(isAbsTime?1:0);
  Value shotVal;
  shotVal.SetInt(shot);
  Value nameVal;
  nameVal.SetString(name, strlen(name));
  d.AddMember("name", nameVal, allocator);
  d.AddMember("shot", shotVal, allocator);
  d.AddMember("samples", samplesVal, allocator);
  d.AddMember("times", timesVal, allocator);
  d.AddMember("timestamp", timestampVal, allocator);
  d.AddMember("absolute_time", absTimeVal, allocator);
  StringBuffer sb;
  PrettyWriter<StringBuffer> writer(sb);
  d.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
  const char *msgBuf = sb.GetString();
 // Event::setEventRaw("STREAMING", strlen(msgBuf), (char *)msgBuf);
  Event::setEventRaw(name, strlen(msgBuf), (char *)msgBuf);
}

EXPORT void EventStream::send(int shot, const char *name, float time,
                              float sample) 
{
  EventStream::send(shot, name, false, 1, &time, 1, &sample);
}

EXPORT void EventStream::send(int shot, const char *name, uint64_t time,
                              float sample) 
{
  EventStream::send(shot, name, true, 1, &time, 1, &sample);
}

EXPORT void EventStream::send(int shot, const char *name, int numSamples,
                              float *times, float *samples) 
{
  EventStream::send(shot, name, false, numSamples, times, numSamples, samples);
}

EXPORT void EventStream::send(int shot, const char *name, int numSamples,
                              uint64_t *times, float *samples) 
{
  EventStream::send(shot, name, true, numSamples, times, numSamples, samples);
}

EXPORT void EventStream::send(int shot, const char *name, int numTimes,
                              uint64_t *times, int numSamples, float *samples) 
{
  EventStream::send(shot, name, true, numTimes, times, numSamples, samples);
}

EXPORT void EventStream::send(int shot, const char *name, Data *timesD, Data *samplesD) 
{
  bool isAbsTime = (timesD->dtype == DTYPE_Q || timesD->dtype == DTYPE_QU);
  int nTimes, nSamples;
  float *samples, sample;
  bool isTimesScalar = false, isSamplesScalar = false;
  try {
    samples = samplesD->getFloatArray(&nSamples);
  }
  catch(MdsException &exc)
  {
      sample = samplesD->getFloat();
      samples = &sample;
      nSamples = 1;
      isSamplesScalar = true;
  }
  if(isAbsTime)
  {
    uint64_t *times, time;
    try {
      times =  (uint64_t *)timesD->getLongArray(&nTimes);
    }
    catch(MdsException &exc)
    {
      time = timesD->getLong();
      times = &time;
      nTimes = 1;
      isTimesScalar = true;
    }
    EventStream::send(shot, name, true, nTimes, times, nSamples, samples);
    if(!isTimesScalar) delete[] times;
  }
  else
  {
    float *times, time;
    try {
      times = timesD->getFloatArray(&nTimes);
    }
    catch(MdsException &exc)
    {
        time = timesD->getFloat();
        times = &time;
        nTimes = 1;
        isTimesScalar = true;
    }
    EventStream::send(shot, name, false, nTimes, times, nSamples, samples);
    if(!isTimesScalar) delete[] times;
  }
  if(!isSamplesScalar) delete[] samples;
}


EXPORT void EventStream::run() {
//  const char *eventName = getName(); // Get the name of the event
 // if (strcmp(eventName, "STREAMING"))
 //   return; // Should neve return
  size_t bufSize;
  const char *buf = getRaw(&bufSize); // Get raw data
 
  
  if(bufSize == 0) return;
  
  if(buf[0] == '{')  //JSON payload
  {
    char *newPayload = new char[bufSize+1];
    memcpy(newPayload, buf, bufSize);
    newPayload[bufSize] = 0;
    //std::cout << newPayload << std::endl;
    
    
    handleJSONPayload(newPayload);
    delete[] newPayload;
    return;
  }
  char *str = new char[bufSize + 1];  // Make it a string
  memcpy(str, buf, bufSize);
  str[bufSize] = 0;
  int shot, numSamples;
  char timeFormat[16], name[256];
  // std::cout << "RECEIVED EVENT " << eventName << " WITH DATA  " << str <<
  // std::endl;
  int readItems =
      sscanf(str, "%d %s %s %d", &shot, name, timeFormat, &numSamples);
  if (readItems < 4) {
    delete[] str;
    return; // Incorrect message
  }
  // skip to fourth blank
  int len = strlen(str);
  int j = 0;
  for (int i = 0; j < len && i < 4; i++) {
    while (j < len && str[j] != ' ')
      j++;
    if (j == len) {
      delete[] str;
      return; // Incorrect message
    }
    j++;
  }
  Data *timesD;
  if (timeFormat[0] == 'F') {
    float *times = new float[numSamples];
    for (int i = 0; i < numSamples; i++) {
      sscanf(&str[j], "%f", &times[i]);
      while (j < len && str[j] != ' ')
        j++;
      if (i < numSamples && j == len) {
        delete[] times;
        delete[] str;
        return; // Incorrect message
      }
      j++;
    }
    if (numSamples > 1)
      timesD = new Float32Array(times, numSamples);
    else
      timesD = new Float32(times[0]);
    delete[] times;
  } else if (timeFormat[0] != 'B') {
    uint64_t *times = new uint64_t[numSamples];
    for (int i = 0; i < numSamples; i++) {
      sscanf(&str[j], "%lu", (unsigned long *)&times[i]);
      while (j < len && str[j] != ' ')
        j++;
      if (i < numSamples - 1 && j == len) {
        delete[] times;
        delete[] str;
        return; // Incorrect message
      }
      j++;
    }
    if (numSamples > 1)
      timesD = new Uint64Array(times, numSamples);
    else
      timesD = new Uint64(times[0]);
    delete[] times;
  } else // Mode B
  {
    Data *timesD = deserialize(&str[j]);
    Data *samplesD = deserialize(&str[j + numSamples]);
    // Expressions already available, do not need any further action
    std::string nameStr(name);
    for (size_t i = 0; i < listeners.size(); i++) {
      if (names[i] == nameStr)
        listeners[i]->dataReceived(samplesD, timesD, shot);
    }
    deleteData(samplesD);
    deleteData(timesD);
    delete[] str;
    return;
  }
  Data *samplesD;
  float *samples = new float[numSamples];
  for (int i = 0; i < numSamples; i++) {
    sscanf(&str[j], "%f", &samples[i]);
    while (j < len && str[j] != ' ')
      j++;
    if (i < numSamples - 1 && j == len) {
      delete[] samples;
      delete[] str;
      deleteData(timesD);
      return; // Incorrect message
    }
    j++;
  }
  if (numSamples > 1)
    samplesD = new Float32Array(samples, numSamples);
  else
    samplesD = new Float32(samples[0]);

  delete[] samples;
  std::string nameStr(name);

  for (size_t i = 0; i < listeners.size(); i++) {
    if (names[i] == nameStr)
      listeners[i]->dataReceived(samplesD, timesD, shot);
  }
  deleteData(samplesD);
  deleteData(timesD);
}
EXPORT void EventStream::handleJSONPayload(char *payload)
{
  Document d;
  if(d.Parse(payload).HasParseError())
  {
    std::cout << "JSON parse error: " << payload << std::endl;
    return;
  }
  int shot = d["shot"].GetInt();
  int absTime = d["absolute_time"].GetInt();
  
  bool isAbsTime = (absTime == 1);
  const Value &timesVal = d["times"];
  const char *nameStr = d["name"].GetString();
  Data *timesD;
  if(isAbsTime)
  {
      uint64_t *times = new uint64_t[timesVal.Size()];
      for(size_t i = 0; i < timesVal.Size(); i++)
        times[i] = timesVal[i].GetInt();
      if(timesVal.Size() == 1)
        timesD = new Uint64(times[0]);
      else
        timesD = new Uint64Array(times, timesVal.Size());
  }
  else
  {
      float *times = new float[timesVal.Size()];
      for(size_t i = 0; i < timesVal.Size(); i++)
        times[i] = timesVal[i].GetFloat();
      if(timesVal.Size() == 1)
        timesD = new Float32(times[0]);
      else
        timesD = new Float32Array(times, timesVal.Size());
      delete[] times;
  }
  const Value &samplesVal = d["samples"];
  float *samples = new float[samplesVal.Size()];
  for(size_t i = 0; i < samplesVal.Size(); i++)
    samples[i] = samplesVal[i].GetFloat();
  Data *samplesD;
  if(samplesVal.Size() == 1)
    samplesD = new Float32(samples[0]);
  else
    samplesD = new Float32Array(samples, samplesVal.Size());
  delete [] samples;
  for (size_t i = 0; i < listeners.size(); i++) {
    if (names[i] == nameStr)
      listeners[i]->dataReceived(samplesD, timesD, shot);
  }
  deleteData(samplesD);
  deleteData(timesD);
}  

EXPORT void EventStream::registerListener(DataStreamListener *listener,  
                                          const char *inName) {
  listeners.push_back(listener);
  names.push_back(std::string(inName));
}
EXPORT void EventStream::registerListener(DataStreamListener *listener) 
{
  listeners.push_back(listener);
  names.push_back(std::string(name));
}
