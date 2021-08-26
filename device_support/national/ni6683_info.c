#include <nisync-lib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

int checkService(const char *command, const char *service, const char *status)
{
  char cmd[100];
  char line[100];
  FILE *f;
  int res = 0;

  sprintf(cmd, command, service);
  f = popen(cmd, "r");
  while (fgets(line, 100, f) != NULL)
    ;
  line[strlen(line) - 1] = 0;
  if (strstr(line, status) != NULL || strcmp(line, status) == 0)
    res = 1;
  else
    res = 0;
  pclose(f);
  return res;
}

int main(int argc, const char **argv)
{
  const nisync_device_type devtype = NISYNC_DEVICE_TYPE_PXI6683H;
  int devnum = 0;
  int resetDevice = 0;
  struct nisync_device_info device_info;
  int fd, rc;

  if (!(argc == 2 || argc == 3))
  {
    printf("Usage:  %s <device number> [reset] \n", argv[0]);
    exit(EXIT_FAILURE);
  }

  devnum = atoi(argv[1]);
  if (argc == 3)
  {
    resetDevice = (strcmp(argv[2], "reset") == 0) ? 1 : 0;
  }

  fd = nisync_open_device(devtype, devnum);

  if (fd < 0)
  {
    fprintf(stderr, "Cannot open device: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  if (resetDevice)
  {
    if (nisync_reset(fd))
    {
      fprintf(stderr, "Failed to rest device : %s\n", strerror(errno));
      rc = EXIT_FAILURE;
      goto cleanup_terminal_fd;
    }
    printf("Waithing 20 sec for device reset\n");
    sleep(20);
  }

  if (nisync_get_device_info(fd, &device_info))
  {
    fprintf(stderr, "Failed to query device info: %s\n", strerror(errno));
    rc = EXIT_FAILURE;
    goto cleanup_terminal_fd;
  }

  printf("driver_version             %s\n", device_info.driver_version);
  printf("model                      %s\n", device_info.model);
  printf("name                       %s\n", device_info.name);
  printf("time_since_sync            %lld\n", (long long)device_info.time_since_sync);
  printf("serial_number              %d\n", device_info.serial_number);
  printf("signature                  %d\n", device_info.signature);
  printf("revision                   %d\n", device_info.revision);
  printf("oldest_compatible_revision %d\n", device_info.oldest_compatible_revision);
  printf("hardware_revision          %d\n", device_info.hardware_revision);

  //printf("\n");

  switch (device_info.sync_state)
  {
  case NISYNC_INACTIVE_OR_NO_MASTER:
    printf("%sNISYNC_INACTIVE_OR_NO_MASTER - Either IEEE-1588v2 synchronization is not currently activated or\nthere has been no communication with a master since synchronization\nwas last activated\n", KRED);

    /*            
            if( !checkService("systemctl is-active %s", "ntpd", "inactive") )
                printf("ntpd service MUST be inactive\n");
    
            if( !checkService("%s-core status", "ptpd", "active") )
                printf("ptpd service MUST be active\n");
*/
    if (!checkService("systemctl is-active %s", "ntpd", "inactive")) //CODAC 6
                                                                     //if( !checkService("service %s status", "ntpd", "stopped") )//CODAC 5
      printf("%sntpd service MUST be stoped\n", KRED);
    else
      printf("%sntpd service is stoped\n", KGRN);

    if (checkService("%s-core status", "ptpd", "inactive")) //CODAC 6
                                                            //if( !checkService("%s-nisync status", "ptpd", "stopped") )//CODAC 5
      printf("%sptpd service MUST be started\n", KRED);
    else
      printf("%sptpd service is running\n", KGRN);

    break;
  case NISYNC_ACTIVE_MASTER:
    printf("%sNISYNC_ACTIVE_MASTER - There is currently communication with a master at regular intervals\n", KGRN);
    break;
  case NISYNC_WITHIN_GMC_OFFSET:
    printf("%sNISYNC_WITHIN_GMC_OFFSET - There is currently communication with a master at regular intervals\nand GMC offset of the board is within GMC threshold\n", KGRN);
    break;
  case NISYNC_LOST_MASTER:
    printf("%sNISYNC_LOST_MASTER - There previously was communication with a master, but we haven't\nheard from any master in at least 5 seconds\n", KRED);
    break;
  }
  printf(KNRM);

cleanup_terminal_fd:
  close(fd);

  return rc;
}
