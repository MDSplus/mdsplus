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
#define setenv(name,val,extra) _putenv_s(name,val)
#endif

using namespace testing;
using namespace MDSplus;


#define TEST_SEGMENT_FLOAT(node,seg,test) do{\
  unique_ptr<Array> signal = node->getSegment(seg);\
  int length = 0;\
  float* array = signal->getFloatArray(&length);\
  try{\
  for (int i = 0 ; i<length ; i++)\
    if (!(test)) TEST1(test);\
  } catch (MDSplus::MdsException) {\
    delete[] array;\
    throw;\
  }\
  delete[] array;\
}while(0);

void putSegment(){
    float times[1000];
    float data[1000];
    for(int i = 0; i < 1000; i++){
      times[i] = i*100;
      data[i] = i*10;
    }
    {
      unique_ptr<Tree>     t   = new MDSplus::Tree("t_treeseg",1,"NEW");
      unique_ptr<TreeNode> n[] = {t->addNode("PS0","SIGNAL"),t->addNode("PS1","SIGNAL")};
      {
	unique_ptr<Float32Array> dim   = new MDSplus::Float32Array(times, 1000);
	unique_ptr<Array>        dat   = (MDSplus::Array*)MDSplus::execute("ZERO([1000],1e0)");
	unique_ptr<Float32>      start = new MDSplus::Float32(0);
	unique_ptr<Float32>      end   = new MDSplus::Float32(999);
	n[0]->beginSegment(start, end, dim, dat);
	n[1]->beginSegment(start, end, dim, dat);
      }
      TEST_SEGMENT_FLOAT(n[0],0,array[i]==0.);
      TEST_SEGMENT_FLOAT(n[1],0,array[i]==0.);
      for(int i = 0; i < 10; i++){
        unique_ptr<Float32Array> chunk = new MDSplus::Float32Array(&data[i*100], 100);
        n[0]->putSegment(chunk, -1);
        n[1]->putSegment(chunk, -1);
      }
      TEST_SEGMENT_FLOAT(n[0],0,array[i]==data[i]);
      TEST_SEGMENT_FLOAT(n[1],0,array[i]==data[i]);
    }
}

void BlockAndRows(){
  unique_ptr<Tree>     t = new MDSplus::Tree("t_treeseg",1,"NEW");
  unique_ptr<TreeNode> n = t->addNode("BAR","SIGNAL");
  t->write();
  {
    int d[2] = {0,7};  unique_ptr<Int32Array> s = new Int32Array(d,2);
    n->beginTimestampedSegment(s);
  }
  TEST1(AutoString(unique_ptr<Data>(n->getData())->decompile()).string == "Build_Signal(0, [], *, [])");
  // beginning adding row increments next_row to 1
  {
    int  d[1] = {1};    unique_ptr<Int32Array> s = new Int32Array(d,1);
    int64_t t= -1;
    n->putRow(s,&t,10);
  }
  TEST1(AutoString(unique_ptr<Data>(n->getData()    )->decompile()).string == "Build_Signal(0, [1], *, [-1Q])");
  TEST1(AutoString(unique_ptr<Data>(n->getSegment(0))->decompile()).string == "[1]");
  /**************************************************************
   beginning a new block set next_row back to 0 of the new block
   the previous block is assumed to be full as the tailing zero could be valid data
   **************************************************************/
  {
    int d[2] = {0,0};  unique_ptr<Int32Array> s = new Int32Array(d,2);
    n->beginTimestampedSegment(s);
  }
  TEST1(AutoString(unique_ptr<Data>(n->getData()    )->decompile()).string == "Build_Signal(0, [1,7], *, [-1Q,0Q])");
  TEST1(AutoString(unique_ptr<Data>(n->getSegment(0))->decompile()).string == "[1,7]");
  n->setSegmentScale(unique_ptr<Data>(compile("$VALUE*2")));
  TEST1(AutoString(unique_ptr<Data>(n->getSegmentScale())->decompile()).string == "$VALUE * 2");
  TEST1(AutoString(unique_ptr<Data>(n->getData()    )->decompile()).string == "Build_Signal(0, $VALUE * 2, [1,7], [-1Q,0Q])");
  std::vector<int> data = n->getIntArray();
  TEST1(data[0]==2);
  TEST1(data[1]==14);
}


#define TEST(prcedure) do{BEGIN_TESTING(prcedure); prcedure(); END_TESTING;}while(0)
int main(int argc UNUSED_ARGUMENT, char *argv[] UNUSED_ARGUMENT){
    setenv("t_treeseg_path",".",1);
    TEST(putSegment);
    TEST(BlockAndRows);
}
