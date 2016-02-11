
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

#include <mdsshr.h>
#include <testing.h>


int ev_id;

void eventAst(void *arg, int len, char *buf) {
    printf("received event in thread %d\n",syscall(__NR_gettid));
}

int main()
{
  int status;

  MDSEventAst("test_event", eventAst, 0, &ev_id);
  MDSEvent("test_event",0,0);
  MDSEventCan(ev_id);
  
  MDSEventAst("test_event2", eventAst, 0, &ev_id);
  MDSEvent("test_event2",0,0);
  MDSEventCan(ev_id);

  return 0;
}

