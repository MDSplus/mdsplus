#include <string.h>
#include <stdio.h>
extern int MDSEvent(char *eventname,int len, char *data);

int main(int argc, char **argv)
{
  int status;
  if (argc < 2)
  {
    printf("Usage: %s <event> [optional-data-string]\n",argv[0]);
    status = 0;
  }
  else
  {
    int len = (int)((argc > 2) ? strlen(argv[2]) : 0);
    status = MDSEvent(argv[1],len,argv[2]);
  }
  return(status);
}    
