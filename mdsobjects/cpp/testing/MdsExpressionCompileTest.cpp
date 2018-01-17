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
#include <mdsobjects.h>
#include "testing.h"

using namespace MDSplus;

#define NUM_THREADS 8
#define NUM_REPEATS 8


void* TdiTask(__attribute__((unused )) void* arg){
  int ii;
  for (ii = 0 ; ii<NUM_REPEATS ; ii++) {
    AutoPointer<Data> a = MDSplus::execute("whoami()");
    AutoPointer<Data> b = MDSplus::execute("private _s=PHASE_NAME_LOOKUP(0)");
    AutoPointer<Data> c = MDSplus::execute("public _s=[0:6:2,1:4]"); //TODO: use a python method
    AutoPointer<Data> d = MDSplus::execute("private _s=3;public _s=7;private _s==public _s");
    AutoPointer<Data> e = MDSplus::execute("_s=((1+SQRT(3.)*2-[1.3D0, 3.]/2))");
    AutoPointer<Data> f = MDSplus::execute("_ans=0;MdsShr->MdsSerializeDscOut(xd(1),xd(_ans));byte_unsigned(_ans)");
    AutoPointer<Data> g = MDSplus::execute("SerializeOut(1)");
    AutoPointer<Data> h = MDSplus::execute("_s=SerializeOut(`(1+SQRT(3.)*2-[1.3D0, 3.]/2));SerializeIn(_s)");
    AutoPointer<Data> i = MDSplus::execute("fun test(){return(1);};test()");       //TEST1( i->getInt()==1 );
    AutoPointer<Data> j = MDSplus::execute("fun test(in _i){return(_i);};test(2)");//TEST1( j->getInt()==2 );
  }
  return NULL;
}

void* PubTask(__attribute__((unused )) void* arg){
  int ii;
  for (ii = 0 ; ii<NUM_REPEATS ; ii++) {
    AutoPointer<Data> a = MDSplus::execute("public _s=1");   //TEST1( a->getInt()==1 );
    AutoPointer<Data> b = MDSplus::execute("public _s=1+_s");//TEST1( b->getInt()>1 );
  }
  return NULL;
}

void* TreeTask(__attribute__((unused )) void* arg){
  int ii;
  AutoPointer<Data> aa = MDSplus::execute("TreeShr->TreeUsePrivateCtx(1)");
  for (ii = 0 ; ii<NUM_REPEATS ; ii++) {
    AutoPointer<Data> a = MDSplus::execute("_a=*;ext_function('TreeShr','TreeCtx')");
  }
  return NULL;
}

void* PyTask(__attribute__((unused )) void* arg){
  int ii;
  for (ii = 0 ; ii<NUM_REPEATS ; ii++) {
    AutoPointer<Data> a = MDSplus::execute("MdsDevices()");
  }
  return NULL;
}

void MultiThreadTest(void* (*task)(void*)) {
    pthread_t threads[NUM_THREADS];
    pthread_attr_t attr, *attrp;
    if (pthread_attr_init(&attr))
      attrp = NULL;
    else {
      attrp = &attr;
      pthread_attr_setstacksize(&attr, 0x80000);
    }
    int nt,i;
    for (nt = 0 ; nt<NUM_THREADS ; nt++)
      if (pthread_create(&threads[nt], attrp, task, NULL))
        break;
    if (attrp) pthread_attr_destroy(attrp);
    if (nt<NUM_THREADS) fprintf(stderr,"Could not create all %d threads\n", NUM_THREADS);
    for (i = 0 ; i<nt ; i++)
      pthread_join(threads[i],NULL);
}

int main(int argc UNUSED_ARGUMENT, char *argv[] UNUSED_ARGUMENT)
{
    BEGIN_TESTING(ExpressionCompile);
    AutoPointer<Data> data = MDSplus::compile("[1,2,3]");
    // TODO: adds more tests .. //
    END_TESTING;

    BEGIN_TESTING(GetIdentPutIdent);
    MultiThreadTest(PubTask);
    END_TESTING;

    TEST_TIMEOUT(10);
    BEGIN_TESTING(MultiThreadTdi);
    MultiThreadTest(TdiTask);
    END_TESTING;

    BEGIN_TESTING(MultiThreadTree);
    MultiThreadTest(TreeTask);
    END_TESTING;
    // python tests will not pass any test
    if (getenv("PyLib")&&getenv("TEST_PYTHON")) {
      BEGIN_TESTING(MultiThreadPython);
      MultiThreadTest(PyTask);
      END_TESTING;
    }
}

