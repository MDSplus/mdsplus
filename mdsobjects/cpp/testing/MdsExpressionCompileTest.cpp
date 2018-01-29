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

const char* TdiCmds[] = {
  "whoami()",
  "private _s=PHASE_NAME_LOOKUP(0)",
  "public _s=[0:6:2,1:4]",
  "private _s=3;public _s=7;private _s==public _s",
  "_s=((1+SQRT(3.)*2-[1.3D0, 3.]/2))",
  "_ans=0;MdsShr->MdsSerializeDscOut(xd(1),xd(_ans));byte_unsigned(_ans)",
  "SerializeOut(1)",
  "_s=SerializeOut(`(1+SQRT(3.)*2-[1.3D0, 3.]/2));SerializeIn(_s)",
  "fun test(){return(1);};test()",
  "fun test(in _i){return(_i);};test(2)",
};
const int TdiNCmd = sizeof(TdiCmds)/sizeof(*TdiCmds);

const char* PubCmds[] = {
  "public _s=1",
  "public _s=1+_s",
};
const int PubNCmd = sizeof(PubCmds)/sizeof(*PubCmds);

const char* TreeCmds[] = {
  "_a=*;ext_function('TreeShr','TreeCtx')",
};
const int TreeNCmd = sizeof(TreeCmds)/sizeof(*TreeCmds);

const char* PyCmds[] = {
  "MdsDevices()",
};
const int PyNCmd = sizeof(PyCmds)/sizeof(*PyCmds);



void* Test(void* args){
  int ii = 0,ic = 0;
  int ncmd = *(int*)((void**)args)[0];
  char** cmds = (char**)((void**)args)[1];
  delete MDSplus::execute("TreeShr->TreeUsePrivateCtx(1)");
  try {
    for (; ii<NUM_REPEATS ; ii++)
      for (; ic<ncmd ; ic++)
        delete MDSplus::execute(cmds[ic]);
  } catch (MDSplus::MdsException) {
    std::cerr << "ERROR in cycle " << ii << " >> " << cmds[ic] << "\n";
    throw;
  }
  return NULL;
}

void MultiThreadTest(int ncmd, const char* cmds[]) {
    pthread_t threads[NUM_THREADS];
    pthread_attr_t attr, *attrp;
    if (pthread_attr_init(&attr))
      attrp = NULL;
    else {
      attrp = &attr;
      pthread_attr_setstacksize(&attr, 0x80000);
    }
    void* args[] = {&ncmd,cmds};
    int nt;
    for (nt = 0 ; nt<NUM_THREADS ; nt++)
      if (pthread_create(&threads[nt], attrp, Test, args))
        break;
    if (attrp) pthread_attr_destroy(attrp);
    if (nt<NUM_THREADS) fprintf(stderr,"Could not create all %d threads\n", NUM_THREADS);
    for (; nt-->0;)
      pthread_join(threads[nt],NULL);
}


#define TEST_INT(cmd,value) do{MDSplus::Data* d=MDSplus::execute(cmd);TEST1(d->getInt()==value);delete d;} while(0)
int main(int argc UNUSED_ARGUMENT, char *argv[] UNUSED_ARGUMENT){
    BEGIN_TESTING(ExpressionCompile);
    delete MDSplus::execute("_s=[1,2,3]");
    TEST_INT("public _s = 3",3);
    TEST_INT("deallocate(public _s)",1);
    // TODO: adds more tests .. //
    END_TESTING;

    BEGIN_TESTING(GetIdentPutIdent);
    MultiThreadTest(PubNCmd,PubCmds);
    END_TESTING;

    BEGIN_TESTING(MultiThreadTdi);
    MultiThreadTest(TdiNCmd,TdiCmds);
    END_TESTING;

    BEGIN_TESTING(MultiThreadTree);
    MultiThreadTest(TreeNCmd,TreeCmds);
    END_TESTING;
    // python tests will not pass any test
    if (getenv("PyLib")&&getenv("TEST_PYTHON")) {
      BEGIN_TESTING(MultiThreadPython);
      MultiThreadTest(PyNCmd,PyCmds);
      END_TESTING;
    }
}

