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
#include <mdsshr.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#ifdef _WIN32
#include <windows.h>
#define syscall(__NR_gettid) GetCurrentThreadId()
#else
#include <sys/syscall.h>
#endif
#include <time.h>

#include <stdarg.h>

#include "testing.h"

static pthread_mutex_t astCount_lock;
static pthread_mutex_t first_lock;
static pthread_mutex_t second_lock;




static int astCount = 0;

void eventAst(void *arg, int len __attribute__ ((unused)), char *buf __attribute__ ((unused))) {
    printf("received event in thread %ld, name=%s\n",
           syscall(__NR_gettid),
           (char *)arg);
    pthread_mutex_lock(&astCount_lock);
    astCount++;
    pthread_mutex_unlock(&astCount_lock);
}




static int first = 0,second = 0;

void eventAstFirst(void *arg, int len __attribute__ ((unused)), char *buf __attribute__ ((unused))) {
    printf("received event in thread %ld, name=%s\n",
           syscall(__NR_gettid),
           (char *)arg);
    pthread_mutex_lock(&first_lock);
    first=1;
    pthread_mutex_unlock(&first_lock);
}

void eventAstSecond(void *arg, int len __attribute__ ((unused)), char *buf __attribute__ ((unused))) {
    printf("received event in thread %ld, name=%s\n",
           syscall(__NR_gettid),
           (char *)arg);
    pthread_mutex_lock(&second_lock);
    second=1;
    pthread_mutex_unlock(&second_lock);
}



static void wait() {
    static const struct timespec tspec = {0,100000000};
    nanosleep(&tspec,0);
}

int main(int argc, char **args)
{
    int status=0;
    int i,iterations,ev_id;
    char *eventname = alloca(100);
    pthread_mutex_init(&astCount_lock, NULL);
    pthread_mutex_init(&first_lock, NULL);
    pthread_mutex_init(&second_lock, NULL);
    
    BEGIN_TESTING(UdpEvents);
#ifdef _WIN32
    SKIP_TEST("Skipping UDP event tests on Windows because of problems with wine and udp.")
#endif
    if (argc < 2) {
        iterations=3;
    } else {
        iterations=atoi(args[1]);
        printf("Doing %d iterations\n",iterations);
    }
    
    for (i=0;i<iterations;i++) {
        sprintf(eventname,"ev_test_%d_%d",i,getpid());

        status = MDSEventAst(eventname, eventAst, eventname, &ev_id);
        TEST0( status%1 );        
        wait();
        status = MDSEvent(eventname,0,0);
        TEST0( status%1 );        
        status = MDSEvent(eventname,0,0);
        TEST0( status%1 );                
        wait();        
        status = MDSEventCan(ev_id);
        TEST0( status%1 );
        wait();
    }
    pthread_mutex_lock(&astCount_lock);
    TEST1(astCount == 2*iterations);
    pthread_mutex_unlock(&astCount_lock);
    

    // Testing two listening events //
    int id1,id2;
    sprintf(eventname, "test_event_%d", getpid());
    status = MDSEventAst(eventname, eventAstFirst, "first", &id1);
    status = MDSEventAst(eventname, eventAstSecond, "second", &id2);        
    wait();
    status = MDSEvent(eventname,0,0);    
    wait();
    pthread_mutex_lock(&first_lock);
    pthread_mutex_lock(&second_lock);
    printf("first = %d, second = %d\n",first,second);
    TEST1(first);
    TEST1(second);
    pthread_mutex_unlock(&first_lock);
    pthread_mutex_unlock(&second_lock);
    status = MDSEventCan(id1);
    status = MDSEventCan(id2);
    
    END_TESTING;
    return (status & 1)==0;
}
