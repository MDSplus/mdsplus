#include <pthread.h>
#include <mdsshr.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <time.h>

void eventAst(void *arg, int len, char *buf) {
    printf("received event in thread %d, name=%s\n",
           syscall(__NR_gettid),
           (char *)arg);
}

#define WAIT nanosleep(&tspec,0)
int main(int argc, char **args)
{
  struct timespec tspec = {0,1000000};
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
    MDSEventAst(eventname, eventAst, eventname, &ev_id);
    MDSEvent(eventname,0,0);
    WAIT;
    MDSEventCan(ev_id);
    WAIT;
    free(eventname);
  }
  return 0;
}
