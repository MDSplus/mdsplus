
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <mdsdescrip.h>
#include <mdsshr.h>
#include <mds_stdarg.h>

#include <unistd.h>

#include <testing.h>

int ev_id;

pthread_mutex_t mux;

void eventAst(void *arg, int len, char *buf) {
    printf("received\n");
    fflush(stdout);
}

//int main()
//{

//    MDSEventAst("test_event", eventAst, 0, &ev_id);
//    MDSEvent("test_event",0,0);
//    MDSEventCan(ev_id);
    
//    MDSEventAst("test_event2", eventAst, 0, &ev_id);
//    MDSEvent("test_event2",0,0);
//    MDSEventCan(ev_id);

//    return 0;
//}

int main()
{
  int status;
  
//  pthread_mutex_init(&mux,NULL);
  
  {
      status = MDSEventAst("test_event", eventAst, 0, &ev_id);
      printf("status=%d\n",status);
      MDSEvent("test_event",0,0);
//      MDSEventCan(ev_id);
  }
  
  sleep(1);
  {
      status = MDSEventAst("test_event2", eventAst, 0, &ev_id);
      printf("status=%d\n",status);
      MDSEvent("test_event2",0,0);
//      MDSEventCan(ev_id);
  }
//  pthread_mutex_destroy(&mux);
  return 0;
}

