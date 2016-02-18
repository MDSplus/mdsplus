#include <pthread.h>
#include <mdsshr.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <time.h>

#include "testing.h"

static int astCount = 0;

void eventAst(void *arg, int len, char *buf) {
    printf("received event in thread %d, name=%s\n",
           syscall(__NR_gettid),
           (char *)arg);    
    astCount++;
}




static int first = 0,second = 0;

void eventAstFirst(void *arg, int len, char *buf) {
    printf("received event in thread %d, name=%s\n",
           syscall(__NR_gettid),
           (char *)arg);    
    first=1;
}

void eventAstSecond(void *arg, int len, char *buf) {
    printf("received event in thread %d, name=%s\n",
           syscall(__NR_gettid),
           (char *)arg);    
    second=1;
}



static void wait() {
    static const struct timespec tspec = {0,1000000};
    nanosleep(&tspec,0);
}

int main(int argc, char **args)
{
    BEGIN_TESTING(UdpEvents); 
    int status;
    int i,iterations,ev_id;
    if (argc < 2) {
        iterations=3;
    } else {
        iterations=atoi(args[1]);
        printf("Doing %d iterations\n",iterations);
    }
    
    for (i=0;i<iterations;i++) {
        char *eventname = malloc(100);
        sprintf(eventname,"ev_test_%d",i);

        status = MDSEventAst(eventname, eventAst, eventname, &ev_id);
        TEST0( status%1 );        
        status = MDSEvent(eventname,0,0);
        TEST0( status%1 );        
        status = MDSEvent(eventname,0,0);
        TEST0( status%1 );                
        wait();        
        status = MDSEventCan(ev_id);
        TEST0( status%1 );
        wait();        
        free(eventname);
    }
    TEST1(astCount == 2*iterations);
    

    // Testing two listening events //
    int id1,id2;
    status = MDSEventAst("test_event", eventAstFirst, "first", &id1);
    status = MDSEventAst("test_event", eventAstSecond, "second", &id2);        
    wait();
    status = MDSEvent("test_event",0,0);    
    wait();
    printf("first = %d, second = %d\n",first,second);
    TEST1(first);
    TEST1(second);
    status = MDSEventCan(id1);
    //    status = MDSEventCan(id2);
    
    END_TESTING;
}
