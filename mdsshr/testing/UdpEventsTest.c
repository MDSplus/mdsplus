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
