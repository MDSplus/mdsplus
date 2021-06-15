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

#include <unistd.h>

#include <mdsobjects.h>
#include <mdsplus/Mutex.hpp>

#include "testing.h"
#include "testutils/testutils.h"
#include "testutils/unique_ptr.h"

using namespace MDSplus;
using namespace testing;
namespace mds = MDSplus;

static void *sendStream(void *streamName)
{
  sleep(1);
  Data *timeD = new Float32(1.0);
  Data *sampleD = new Float32(123.);
  EventStream::send(1, (char *)streamName, timeD, sampleD);
  deleteData(timeD);
  deleteData(sampleD);
  pthread_exit(0);
  return NULL;
}

static void *sendStreamAbs(void *streamName)
{
  sleep(1);
  Data *timeD = new Uint64(1);
  Data *sampleD = new Float32(123.);
  EventStream::send(1, (char *)streamName, timeD, sampleD);
  deleteData(timeD);
  deleteData(sampleD);
  pthread_exit(0);
  return NULL;
}

static void *sendStreamArr(void *streamName)
{
  sleep(1);
  float times[] = {1., 2};
  float samples[] = {10, 11};
  Data *timesD = new Float32Array(times, 2);
  Data *samplesD = new Float32Array(samples, 2);
  EventStream::send(1, (char *)streamName, timesD, samplesD);
  deleteData(timesD);
  deleteData(samplesD);
  pthread_exit(0);
  return NULL;
}

static void *sendStreamAbsArr(void *streamName)
{
  sleep(1);
  uint64_t times[] = {1, 2};
  float samples[] = {10, 11};
  Data *timesD = new Uint64Array(times, 2);
  Data *samplesD = new Float32Array(samples, 2);
  EventStream::send(1, (char *)streamName, timesD, samplesD);
  deleteData(timesD);
  deleteData(samplesD);
  pthread_exit(0);
  return NULL;
}

static void *setevent(void *evname)
{
  sleep(1);
  Event::setEvent((char *)evname);
  // std::cout << "Event set\n" << std::flush;
  pthread_exit(0);
  return NULL;
}

static void *seteventraw(void *args)
{
  sleep(1);
  std::string *str = ((std::string **)args)[1];
  Event::setEventRaw(((char **)args)[0], str->size(), (char *)str->c_str());
  // std::cout << "EventRaw set\n" << std::flush;
  pthread_exit(0);
  return NULL;
}

static void *seteventdata(void *args)
{
  sleep(1);
  Event::setEvent(((char **)args)[0], ((Data **)args)[1]);
  // std::cout << "EventData set\n" << std::flush;
  pthread_exit(0);
  return NULL;
}

class TestListenerScalarRelative : public MDSplus::DataStreamListener
{
  ConditionVar condition;
  float retTime;
  float retSample;
  int retShot;

public:
  TestListenerScalarRelative()
  {
  }
  void dataReceived(MDSplus::Data *samples, MDSplus::Data *times, int shot)
  {
    retTime = times->getFloat();
    retSample = samples->getFloat();
    retShot = shot;
    condition.notify();
  }
  void waitStream()
  {

    if (condition.waitTimeout(10 * 1000) == false)
      throw MdsException("Timeout Occurred");
  }
  float getTime() { return retTime; }
  float getSample() { return retSample; }
  int getShot() { return retShot; }
};
class TestListenerScalarAbsolute : public MDSplus::DataStreamListener
{
  ConditionVar condition;
  uint64_t retTime;
  float retSample;
  int retShot;

public:
  TestListenerScalarAbsolute()
  {
  }
  void dataReceived(MDSplus::Data *samples, MDSplus::Data *times, int shot)
  {
    retTime = (uint64_t)times->getLong();
    retSample = samples->getFloat();
    retShot = shot;
    condition.notify();
  }
  void waitStream()
  {

    if (condition.waitTimeout(10 * 1000) == false)
      throw MdsException("Timeout Occurred");
  }
  uint64_t getTime() { return retTime; }
  float getSample() { return retSample; }
  int getShot() { return retShot; }
};
class TestListenerArrayRelative : public MDSplus::DataStreamListener
{
  ConditionVar condition;
  float *retTimes;
  float *retSamples;
  int retShot, retTimesLen, retSamplesLen;

public:
  TestListenerArrayRelative()
  {
  }
  void dataReceived(MDSplus::Data *samples, MDSplus::Data *times, int shot)
  {
    retTimes = times->getFloatArray(&retTimesLen);
    retSamples = samples->getFloatArray(&retSamplesLen);
    retShot = shot;
    condition.notify();
  }
  void waitStream()
  {

    if (condition.waitTimeout(10 * 1000) == false)
      throw MdsException("Timeout Occurred");
  }
  float *getTimes(int *retTimeLen)
  {
    *retTimeLen = retTimesLen;
    return retTimes;
  }
  float *getSamples(int *retSampleLen)
  {
    *retSampleLen = retSamplesLen;
    return retSamples;
  }
  int getShot() { return retShot; }
};

class TestListenerArrayAbsolute : public MDSplus::DataStreamListener
{
  ConditionVar condition;
  uint64_t *retTimes;
  float *retSamples;
  int retShot, retTimesLen, retSamplesLen;

public:
  TestListenerArrayAbsolute()
  {
  }
  void dataReceived(MDSplus::Data *samples, MDSplus::Data *times, int shot)
  {
    retTimes = (uint64_t *)times->getLongArray(&retTimesLen);
    retSamples = samples->getFloatArray(&retSamplesLen);
    retShot = shot;
    condition.notify();
  }
  void waitStream()
  {

    if (condition.waitTimeout(10 * 1000) == false)
      throw MdsException("Timeout Occurred");
  }
  uint64_t *getTimes(int *retTimeLen)
  {
    *retTimeLen = retTimesLen;
    return retTimes;
  }
  float *getSamples(int *retSampleLen)
  {
    *retSampleLen = retSamplesLen;
    return retSamples;
  }
  int getShot() { return retShot; }
};

int main(int argc __attribute__((unused)),
         char *argv[] __attribute__((unused)))
{
  BEGIN_TESTING(Event);
  pthread_attr_t attr, *attrp;
  if (pthread_attr_init(&attr))
    attrp = NULL;
  else
  {
    attrp = &attr;
    pthread_attr_setstacksize(&attr, 0x100000);
  }
  try
  {
    static char evname[100] = "empty";
    if (strcmp(evname, "empty") == 0)
      sprintf(evname, "event_test_%d", getpid());

    { // NULL EVENT //
      pthread_t thread;
      if (pthread_create(&thread, attrp, setevent, (void *)evname))
        throw std::runtime_error("ERROR: Could not create thread for setevent");
      Event ev(evname);
      std::cout << "Waiting for wait\n"
                << std::flush;
      ev.wait();
      std::cout << "Waiting for thread\n"
                << std::flush;
      pthread_join(thread, NULL);
    }

    { // RAW EVENT //
      static std::string str("test string to be compared");
      void *args[] = {evname, &str};
      pthread_t thread;
      if (pthread_create(&thread, attrp, seteventraw, (void *)args))
        throw std::runtime_error(
            "ERROR: Could not create thread for seteventraw");
      Event ev(evname);
      size_t buf_len = 0;
      std::cout << "Waiting for waitRaw\n"
                << std::flush;
      const char *buf = ev.waitRaw(&buf_len);
      std::cout << "Waiting for thread\n"
                << std::flush;
      pthread_join(thread, NULL);
      TEST1(std::string(str) == std::string(buf));
    }

    { // DATA EVENT //
      static unique_ptr<String> str = new String("test string to be compared");
      void *args[] = {evname, str};
      pthread_t thread;
      if (pthread_create(&thread, attrp, seteventdata, (void *)args))
        throw std::runtime_error(
            "ERROR: Could not create thread for seteventdata");
      Event ev(evname);
      std::cout << "Waiting for waitData\n"
                << std::flush;
      unique_ptr<Data> data = ev.waitData();
      std::cout << "Waiting for thread\n"
                << std::flush;
      pthread_join(thread, NULL);
      TEST1(AutoString(data->getString()).string ==
            AutoString(str->getString()).string);
    }

    MDSplus::EventStream evStreamScalarRelative("EVENT_TEST:[]SCALAR_RELATIVE");
    MDSplus::EventStream evStreamScalarAbsolute("EVENT_TEST:[]SCALAR_ABSOLUTE");
    MDSplus::EventStream evStreamArrayRelative("EVENT_TEST:[]ARRAY_RELATIVE");
    MDSplus::EventStream evStreamArrayAbsolute("EVENT_TEST:[]ARRAY_ABSOLUTE");
    TestListenerScalarRelative testListenerScalarRelative;
    TestListenerScalarAbsolute testListenerScalarAbsolute;
    TestListenerArrayRelative testListenerArrayRelative;
    TestListenerArrayAbsolute testListenerArrayAbsolute;
    evStreamScalarRelative.registerListener(&testListenerScalarRelative);
    evStreamScalarAbsolute.registerListener(&testListenerScalarAbsolute);
    evStreamArrayRelative.registerListener(&testListenerArrayRelative);
    evStreamArrayAbsolute.registerListener(&testListenerArrayAbsolute);
    evStreamScalarRelative.start();
    evStreamScalarAbsolute.start();
    evStreamArrayRelative.start();
    evStreamArrayAbsolute.start();
    { // STREAM SCALAR RELATIVE TIME
      pthread_t thread;
      if (pthread_create(&thread, attrp, sendStream, (void *)"EVENT_TEST:[]SCALAR_RELATIVE"))
        throw std::runtime_error(
            "ERROR: Could not create thread for sendStream");

      //   std::cout << "Waiting for stream\n" << std::flush;
      testListenerScalarRelative.waitStream();
      pthread_join(thread, NULL);
      float retTime = testListenerScalarRelative.getTime();
      float retSample = testListenerScalarRelative.getSample();
      int retShot = testListenerScalarRelative.getShot();
      TEST1(retTime == 1.0);
      TEST1(retSample == 123.0);
      TEST1(retShot == 1);
    }
    { // STREAM SCALAR ABSOLUTE TIME
      pthread_t thread;
      if (pthread_create(&thread, attrp, sendStreamAbs, (void *)"EVENT_TEST:[]SCALAR_ABSOLUTE"))
        throw std::runtime_error(
            "ERROR: Could not create thread for sendStream");

      //     std::cout << "Waiting for stream\n" << std::flush;
      testListenerScalarAbsolute.waitStream();
      pthread_join(thread, NULL);
      uint64_t retTime = testListenerScalarAbsolute.getTime();
      float retSample = testListenerScalarAbsolute.getSample();
      int retShot = testListenerScalarAbsolute.getShot();
      TEST1(retTime == 1);
      TEST1(retSample == 123.0);
      TEST1(retShot == 1);
    }
    { // STREAM ARRAY RELATIVE TIME
      pthread_t thread;
      if (pthread_create(&thread, attrp, sendStreamArr, (void *)"EVENT_TEST:[]ARRAY_RELATIVE"))
        throw std::runtime_error(
            "ERROR: Could not create thread for sendStream");

      // std::cout << "Waiting for stream\n" << std::flush;
      testListenerArrayRelative.waitStream();
      pthread_join(thread, NULL);
      int retTimesSize, retSamplesSize;
      float *retTimes = testListenerArrayRelative.getTimes(&retTimesSize);
      float *retSamples = testListenerArrayRelative.getSamples(&retSamplesSize);
      int retShot = testListenerArrayRelative.getShot();
      TEST1(retTimesSize == 2);
      TEST1(retSamplesSize == 2);
      TEST1(retTimes[0] == 1. && retTimes[1] == 2.)
      TEST1(retSamples[0] == 10. && retSamples[1] == 11.)
      TEST1(retShot == 1);
      delete[] retTimes;
      delete[] retSamples;
    }
    { // STREAM ARRAY ABSOLUTE TIME
      pthread_t thread;
      if (pthread_create(&thread, attrp, sendStreamAbsArr, (void *)"EVENT_TEST:[]ARRAY_ABSOLUTE"))
        throw std::runtime_error(
            "ERROR: Could not create thread for sendStream");

      //    std::cout << "Waiting for stream\n" << std::flush;
      testListenerArrayAbsolute.waitStream();
      pthread_join(thread, NULL);
      int retTimesSize, retSamplesSize;
      uint64_t *retTimes = testListenerArrayAbsolute.getTimes(&retTimesSize);
      float *retSamples = testListenerArrayAbsolute.getSamples(&retSamplesSize);
      int retShot = testListenerArrayAbsolute.getShot();
      TEST1(retTimesSize == 2);
      TEST1(retSamplesSize == 2);
      TEST1(retTimes[0] == 1 && retTimes[1] == 2)
      TEST1(retSamples[0] == 10. && retSamples[1] == 11.)
      TEST1(retShot == 1);
      delete[] retTimes;
      delete[] retSamples;
    }
  }
  catch (...)
  {
    if (attrp)
      pthread_attr_destroy(attrp);
    throw;
  }
  if (attrp)
    pthread_attr_destroy(attrp);
  END_TESTING;
}
