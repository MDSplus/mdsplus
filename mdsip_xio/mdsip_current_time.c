#include <time.h>
#include <string.h>
char *mdsip_current_time()
{
  static char timestr[30];
  time_t tim = time(0);
  ctime_r(&tim,timestr);
  timestr[strlen(timestr)-1] = 0;
  return timestr;
}
