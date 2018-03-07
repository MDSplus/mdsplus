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
#include <fstream>
#include <string>
#include <mdsobjects.h>
#include "testing.h"
#include "testutils/unique_ptr.h"
#ifdef _WIN32
#include <windows.h>
#define setenv(name,val,extra) _putenv_s(name,val)
#endif


using namespace MDSplus;
using namespace testing;

#define NUM_THREADS 8
#define NUM_REPEATS 4
#define MEM_ALLOC 1028

static char** cmds;
static int    ncmd;

void loadCmds(const char* filename){
    std::ifstream file(filename);
    if (!file.is_open()) {
      std::cerr << "File not found: " << filename << "\n";
      exit(1);
    }
    std::string str;
    int memlen = MEM_ALLOC;
    cmds = (char**)malloc(memlen*sizeof(char*));
    ncmd = 0;
    while (std::getline(file, str)){
      if (str[0]=='!' || str.length()==0) continue;
      if ((ncmd+1) > memlen) {
        memlen += MEM_ALLOC;
        cmds = (char**)realloc(cmds,memlen*sizeof(char*));
      }
      cmds[ncmd] = (char*)malloc(str.length()+1);
      strcpy(cmds[ncmd++],str.c_str());
    }
}

int SingleThreadTest(int idx, int repeats){
  int ii = 0,ic = 0;
  setenv("t_tdi_path",".",1);
  delete MDSplus::execute("TreeShr->TreeUsePrivateCtx(1)");
  MDSplus::Int32* shot = new MDSplus::Int32(10*idx+1);
  delete MDSplus::executeWithArgs("_SHOT=$",1,shot);
  delete shot;
  delete MDSplus::execute("_EXPT='T_TDI'");
  int status = -1, err = 0;
  for (; ii<repeats ; ii++) {
    for (;ic<ncmd; ic++) try {
      if (strlen(cmds[ic])==0 || *cmds[ic] == '#') continue;
      status = AutoPointer<Data>(MDSplus::execute(cmds[ic]))->getInt();
      if (!status) {
        std::cerr << "FAILED in cycle " << ii << " >> " << cmds[ic] << "\n";
        err = 1;
      } else if (!(status&1)) throw MDSplus::MdsException(status);
    } catch (MDSplus::MdsException e) {
      std::cerr << "ERROR in cycle " << ii << ":=" << status << " >> " << cmds[ic] << "\n";
      err = 1;
    } catch (...) {
      std::cerr << "Exception in cycle " << ii << " >> " << cmds[ic] << "\n";
      err = 1;
    }
    if (err) break;
  }
  return err;
}

void* ThreadTest(void* args){
  int* err = (int*)args;
  *err = SingleThreadTest(*(int*)args,NUM_REPEATS);
  return NULL;
}


void MultiThreadTest() {
    pthread_t threads[NUM_THREADS];
    pthread_attr_t attr, *attrp;
    if (pthread_attr_init(&attr))
      attrp = NULL;
    else {
      attrp = &attr;
      pthread_attr_setstacksize(&attr, 0x40000);
    }
    int thread_idx, results[NUM_THREADS];
    for (thread_idx = 0 ; thread_idx<NUM_THREADS ; thread_idx++){
      results[thread_idx]=thread_idx;
      if (pthread_create(&threads[thread_idx], attrp, ThreadTest, &results[thread_idx]))
        break;
    }
    if (attrp) pthread_attr_destroy(attrp);
    if (thread_idx<NUM_THREADS) fprintf(stderr,"Could not create all %d threads\n", NUM_THREADS);
    for (thread_idx = 0 ; thread_idx<NUM_THREADS ; thread_idx++)
      pthread_join(threads[thread_idx],NULL);
    for (thread_idx = 0 ; thread_idx<NUM_THREADS ; thread_idx++)
      TEST0(results[thread_idx]);
}


int main(int argc, char *argv[]){
    size_t stksize = -1; {
      pthread_attr_t attr;
      if (!pthread_attr_init(&attr)) {
        pthread_attr_getstacksize(&attr, &stksize);
        pthread_attr_destroy(&attr);
      }
    }
    if (argc>1)
      loadCmds(argv[1]);
    else {
      char* srcdir = std::getenv("srcdir");
      if (!srcdir) loadCmds("./MdsTdiTest.tdi");
      else {
        char* filename = (char*)malloc(strlen(srcdir)+16);
        memcpy(filename,srcdir,strlen(srcdir));
        strcpy(filename+strlen(srcdir),"/MdsTdiTest.tdi");
        loadCmds(filename);
        free(filename);
      }
    }
    int single = 0;
    BEGIN_TESTING(SingleThread);
    setenv("t_tdi_path",".",1);
    TEST0(single=SingleThreadTest(0,1));
    END_TESTING;
    if (single) exit(1);

    if (stksize<0x40000){
      std::cout << " -- [MultiThread] -- Skipped because the default thread stack size is too small (" << stksize/1024 << "kB < 256kB)." << std::flush;
    } else {
      BEGIN_TESTING(MultiThread);
      MultiThreadTest();
      END_TESTING;
    }
    for (;ncmd-->0;) free(cmds[ncmd]);
    free(cmds);
}

