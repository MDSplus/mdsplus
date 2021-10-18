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
#include <stdlib.h>
#include <mdsobjects.h>

#include <treeshr.h>

#include "testing.h"
#include "testutils/unique_ptr.h"
#include "testutils/String.h"
//#include "mdsplus/AutoPointer.hpp"
#ifdef _WIN32
#include <windows.h>
#define setenv(name, val, extra) _putenv_s(name, val)
#endif

using namespace testing;
using namespace MDSplus;

#define TEST_SEGMENT_FLOAT(node, seg, test)           \
  do                                                  \
  {                                                   \
    unique_ptr<Array> signal = node->getSegment(seg); \
    int length = 0;                                   \
    float *array = signal->getFloatArray(&length);    \
    try                                               \
    {                                                 \
      for (int i = 0; i < length; i++)                \
        if (!(test))                                  \
          TEST1(test);                                \
    }                                                 \
    catch (MDSplus::MdsException)                     \
    {                                                 \
      delete[] array;                                 \
      throw;                                          \
    }                                                 \
    delete[] array;                                   \
  } while (0);

void putSegment()
{
  float times[1000];
  float data[1000];
  for (int i = 0; i < 1000; i++)
  {
    times[i] = i * 100;
    data[i] = i * 10;
  }
  {
    unique_ptr<Tree> t = new MDSplus::Tree("t_treeseg", 1, "NEW");
    unique_ptr<TreeNode> n[] = {t->addNode("PS0", "SIGNAL"),
                                t->addNode("PS1", "SIGNAL")};
    {
      unique_ptr<Float32Array> dim = new MDSplus::Float32Array(times, 1000);
      unique_ptr<Array> dat =
          (MDSplus::Array *)MDSplus::execute("ZERO([1000],1e0)");
      unique_ptr<Float32> start = new MDSplus::Float32(0);
      unique_ptr<Float32> end = new MDSplus::Float32(999);
      n[0]->beginSegment(start, end, dim, dat);
      n[1]->beginSegment(start, end, dim, dat);
    }
    TEST_SEGMENT_FLOAT(n[0], 0, array[i] == 0.);
    TEST_SEGMENT_FLOAT(n[1], 0, array[i] == 0.);
    for (int i = 0; i < 10; i++)
    {
      unique_ptr<Float32Array> chunk =
          new MDSplus::Float32Array(&data[i * 100], 100);
      n[0]->putSegment(chunk, -1);
      n[1]->putSegment(chunk, -1);
    }
    TEST_SEGMENT_FLOAT(n[0], 0, array[i] == data[i]);
    TEST_SEGMENT_FLOAT(n[1], 0, array[i] == data[i]);
  }
}

void BlockAndRows()
{
  unique_ptr<Tree> t = new MDSplus::Tree("t_treeseg", 1, "NEW");
  unique_ptr<TreeNode> n = t->addNode("BAR", "SIGNAL");
  t->write();
  {
    int d[2] = {0, 7};
    unique_ptr<Int32Array> s = new Int32Array(d, 2);
    n->beginTimestampedSegment(s);
  }
  TEST1(AutoString(unique_ptr<Data>(n->getData())->decompile()).string ==
        "Build_Signal([], *, [])");
  // beginning adding row increments next_row to 1
  {
    int d[1] = {1};
    unique_ptr<Int32Array> s = new Int32Array(d, 1);
    int64_t t = -1;
    n->putRow(s, &t, 10);
  }
  TEST1(AutoString(unique_ptr<Data>(n->getData())->decompile()).string ==
        "Build_Signal([1], *, [-1Q])");
  TEST1(AutoString(unique_ptr<Data>(n->getSegment(0))->decompile()).string ==
        "[1]");
  /**************************************************************
   beginning a new block set next_row back to 0 of the new block
   the previous block is assumed to be full as the tailing zero could be valid
   data
   **************************************************************/
  {
    int d[2] = {0, 0};
    unique_ptr<Int32Array> s = new Int32Array(d, 2);
    n->beginTimestampedSegment(s);
  }
  TEST1(AutoString(unique_ptr<Data>(n->getData())->decompile()).string ==
        "Build_Signal([1,7], *, [-1Q,0Q])");
  TEST1(AutoString(unique_ptr<Data>(n->getSegment(0))->decompile()).string ==
        "[1,7]");
  n->setSegmentScale(unique_ptr<Data>(compile("$VALUE*2")));
  TEST1(
      AutoString(unique_ptr<Data>(n->getSegmentScale())->decompile()).string ==
      "$VALUE * 2");
  TEST1(AutoString(unique_ptr<Data>(n->getData())->decompile()).string ==
        "Build_Signal($VALUE * 2, [1,7], [-1Q,0Q])");
  AutoData<Data> nData = n->data();
  std::vector<int> data = nData->getIntArray();
  TEST1(data[0] == 2);
  TEST1(data[1] == 14);
}

#define NUM_SEGMENTS 10
#define SEG_SAMPLES 1000000
void makeSegment()
{

  MDSplus::Tree *t = new MDSplus::Tree("t_treeseg", -1, "NEW");
  MDSplus::TreeNode *na = t->addNode("SEG", "SIGNAL");
  delete na;
  na = t->addNode("SEG_RES", "SIGNAL");
  delete na;
  na = t->addNode("SEG1", "SIGNAL");
  delete na;
  na = t->addNode("PSEG", "SIGNAL");
  delete na;
  na = t->addNode("PSEG_RES", "SIGNAL");
  delete na;
  na = t->addNode("PSEG1", "SIGNAL");
  delete na;
  na = t->addNode("MMSEG", "SIGNAL");
  delete na;
  na = t->addNode("MMSEG_RES", "SIGNAL");
  delete na;
  na = t->addNode("MMPSEG", "SIGNAL");
  delete na;
  na = t->addNode("MMPSEG_RES", "SIGNAL");
  delete na;
  t->write();
  delete (t);
  t = new MDSplus::Tree("t_treeseg", -1);
  t->createPulse(1);
  delete (t);
  t = new MDSplus::Tree("t_treeseg", 1);
  MDSplus::TreeNode *n = t->getNode("SEG");
  MDSplus::TreeNode *n1 = t->getNode("SEG1");
  MDSplus::TreeNode *nRes = t->getNode("SEG_RES");
  MDSplus::TreeNode *pn = t->getNode("PSEG");
  MDSplus::TreeNode *pnRes = t->getNode("PSEG_RES");
  MDSplus::TreeNode *pn1 = t->getNode("PSEG1");
  MDSplus::TreeNode *mmn = t->getNode("MMSEG");
  MDSplus::TreeNode *mmnRes = t->getNode("MMSEG_RES");
  MDSplus::TreeNode *mmpn = t->getNode("MMPSEG");
  MDSplus::TreeNode *mmpnRes = t->getNode("MMPSEG_RES");
  for (int segIdx = 0; segIdx < NUM_SEGMENTS; segIdx++)
  {
    float *segData = new float[SEG_SAMPLES];
    for (int i = 0; i < SEG_SAMPLES; i++)
      segData[i] = i;
    MDSplus::Float64 *start = new MDSplus::Float64(segIdx);
    MDSplus::Float64 *end = new MDSplus::Float64(segIdx + 1 - 1. / SEG_SAMPLES);
    MDSplus::Float64 *delta = new MDSplus::Float64(1. / SEG_SAMPLES);
    MDSplus::Data *dim = new MDSplus::Range(start, end, delta);
    MDSplus::Float32Array *data =
        new MDSplus::Float32Array(segData, SEG_SAMPLES);
    n->makeSegmentResampled(start, end, dim, data, nRes, 1000);
    n1->makeSegment(start, end, dim, data);
    deleteData(start);
    deleteData(end);
    deleteData(delta);
    deleteData(dim);
    deleteData(data);
    delete[] segData;
  }
  for (int segIdx = 0; segIdx < NUM_SEGMENTS; segIdx++)
  {
    float *segData = new float[SEG_SAMPLES];
    for (int i = 0; i < SEG_SAMPLES; i++)
      segData[i] = 0;
    MDSplus::Float64 *start = new MDSplus::Float64(segIdx);
    MDSplus::Float64 *end = new MDSplus::Float64(segIdx + 1 - 1. / SEG_SAMPLES);
    MDSplus::Float64 *delta = new MDSplus::Float64(1. / SEG_SAMPLES);
    MDSplus::Data *dim = new MDSplus::Range(start, end, delta);
    MDSplus::Float32Array *data =
        new MDSplus::Float32Array(segData, SEG_SAMPLES);
    pn->beginSegmentResampled(start, end, dim, data, pnRes, 1000);
    pn1->beginSegment(start, end, dim, data);
    for (int i = 0; i < SEG_SAMPLES; i++)
      segData[i] = i;
    for (int i = 0; i < 10; i++)
    {
      MDSplus::Float32Array *putData = new MDSplus::Float32Array(
          &segData[i * SEG_SAMPLES / 10], SEG_SAMPLES / 10);
      pn1->putSegment(putData, -1);
      pn->putSegmentResampled(putData, -1, pnRes, 1000);
      MDSplus::deleteData(putData);
    }
    deleteData(start);
    deleteData(end);
    deleteData(delta);
    deleteData(dim);
    deleteData(data);
    delete[] segData;
  }
  for (int segIdx = 0; segIdx < NUM_SEGMENTS; segIdx++)
  {
    float *segData = new float[SEG_SAMPLES];
    for (int i = 0; i < SEG_SAMPLES; i++)
      segData[i] = i;
    MDSplus::Float64 *start = new MDSplus::Float64(segIdx);
    MDSplus::Float64 *end = new MDSplus::Float64(segIdx + 1 - 1. / SEG_SAMPLES);
    MDSplus::Float64 *delta = new MDSplus::Float64(1. / SEG_SAMPLES);
    MDSplus::Data *dim = new MDSplus::Range(start, end, delta);
    MDSplus::Float32Array *data =
        new MDSplus::Float32Array(segData, SEG_SAMPLES);
    mmn->makeSegmentMinMax(start, end, dim, data, mmnRes, 1000);
    deleteData(start);
    deleteData(end);
    deleteData(delta);
    deleteData(dim);
    deleteData(data);
    delete[] segData;
  }
  for (int segIdx = 0; segIdx < NUM_SEGMENTS; segIdx++)
  {
    float *segData = new float[SEG_SAMPLES];
    for (int i = 0; i < SEG_SAMPLES; i++)
      segData[i] = 0;
    MDSplus::Float64 *start = new MDSplus::Float64(segIdx);
    MDSplus::Float64 *end = new MDSplus::Float64(segIdx + 1 - 1. / SEG_SAMPLES);
    MDSplus::Float64 *delta = new MDSplus::Float64(1. / SEG_SAMPLES);
    MDSplus::Data *dim = new MDSplus::Range(start, end, delta);
    MDSplus::Float32Array *data =
        new MDSplus::Float32Array(segData, SEG_SAMPLES);
    mmpn->beginSegmentMinMax(start, end, dim, data, mmpnRes, 1000);
    for (int i = 0; i < SEG_SAMPLES; i++)
      segData[i] = i;
    for (int i = 0; i < 10; i++)
    {
      MDSplus::Float32Array *putData = new MDSplus::Float32Array(
          &segData[i * SEG_SAMPLES / 10], SEG_SAMPLES / 10);
      mmpn->putSegmentMinMax(putData, -1, mmpnRes, 1000);
      MDSplus::deleteData(putData);
    }
    deleteData(start);
    deleteData(end);
    deleteData(delta);
    deleteData(dim);
    deleteData(data);
    delete[] segData;
  }

  int dataLen;
  int resDataLen;
  MDSplus::Data *retData = n->data();
  float *segData = retData->getFloatArray(&dataLen);
  delete[] segData;
  deleteData(retData);
  retData = nRes->data();
  segData = retData->getFloatArray(&resDataLen);
  delete[] segData;
  deleteData(retData);
  TEST1(dataLen / 1000 == resDataLen);

  retData = pn->data();
  segData = retData->getFloatArray(&dataLen);
  delete[] segData;
  deleteData(retData);
  retData = pnRes->data();
  segData = retData->getFloatArray(&resDataLen);
  delete[] segData;
  deleteData(retData);
  TEST1(dataLen / 1000 == resDataLen);

  retData = mmn->data();
  segData = retData->getFloatArray(&dataLen);
  delete[] segData;
  deleteData(retData);
  retData = mmnRes->data();
  segData = retData->getFloatArray(&resDataLen);
  delete[] segData;
  deleteData(retData);
  TEST1(dataLen / 1000 == resDataLen / 2);

  retData = mmpn->data();
  segData = retData->getFloatArray(&dataLen);
  delete[] segData;
  deleteData(retData);
  retData = mmpnRes->data();
  segData = retData->getFloatArray(&resDataLen);
  delete[] segData;
  deleteData(retData);
  TEST1(dataLen / 1000 == resDataLen / 2);

  delete n;
  delete nRes;
  delete n1;
  delete pn;
  delete pnRes;
  delete pn1;
  delete mmn;
  delete mmnRes;
  delete mmpn;
  delete mmpnRes;
  delete t;
}

#define TEST(prcedure)       \
  do                         \
  {                          \
    BEGIN_TESTING(prcedure); \
    prcedure();              \
    END_TESTING;             \
  } while (0)
int main(int argc __attribute__((unused)),
         char *argv[] __attribute__((unused)))
{
  setenv("t_treeseg_path", ".", 1);
  TEST_TIMEOUT(100);
  TEST(putSegment);
  TEST(BlockAndRows);
  TEST(makeSegment);
}
