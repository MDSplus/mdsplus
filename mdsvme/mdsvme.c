#include <standards.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <c_asm.h>
#include <mach/alpha/boolean.h>
#include <time.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sched.h>
#include <io/common/devdriver.h>
#include <io/common/handler.h>
#include <io/dec/vme/vbareg.h>
#include <unistd.h>
#include <signal.h>
#include "dmaexreg.h"
#include <stdlib.h>
#include <string.h>

extern int VmeWaitForInterrupt(char *device, int irq, int vector);
extern int VmePioRead(char *device, unsigned long addr, unsigned long mode, int bufsize, void *buffer, int *bytes_read);
extern int VmePioWrite(char *device, unsigned long addr, unsigned long mode, int bufsize, void *buffer, int *bytes_written);

static void SigCatcher(int signo) {}

int VmeWaitForInterrupt(char *device, int irq, int vector)
{
  struct dmaex_ioctl_data data;
  int fd = open("/dev/dmaex0",O_RDWR);
  int status = 0;
  if (fd != -1)
  {
    struct sigaction newsigaction,oldsigaction;
    sigset_t empty_set,new_set;
    newsigaction.sa_handler = SigCatcher;
    sigemptyset(&newsigaction.sa_mask);
    newsigaction.sa_flags = 0;
    sigaction(SIGUSR1,&newsigaction,&oldsigaction);
    sigaction(SIGINT,&newsigaction,&oldsigaction);
    sigemptyset(&new_set);
    sigaddset(&new_set, SIGUSR1);
    sigprocmask(SIG_BLOCK,&new_set,NULL);
    sigemptyset(&empty_set);
    data.data[0] = irq;
    data.data[1] = vector;
    if (ioctl(fd,SET_INT_HANDLER,data.data) != 0)
      perror("error in ioctl SET_INT_HANDLER");
    else
    {
      sigsuspend(&empty_set);
      if (ioctl(fd, CLR_INT_HANDLER,data.data) != 0)
        perror("error in ioctl CLR_INT_HANDLER");
      else
        status = 0;
    }
    close(fd);
  }
  else
    perror("Error opening VME device");
  return status;
}

int VmePioRead(char *device, unsigned long addr, unsigned long mode, int bufsize, void *buffer, int *bytes_read)
{
  struct dmaex_ioctl_data data;
  int fd = open(device,O_RDWR);
  int status = 0;
  *bytes_read = 0;
  if (fd != -1)
  {
    data.data[0] = bufsize;
    data.data[1] = addr;
    data.data[2] = mode;
    if (ioctl(fd,SETUP_VME_FOR_STRATEGY_PIO,data.data) != 0)
      perror("error in ioctl SETUP_VME_FOR_STRATEGY_PIO");
    else
    {
      data.data[0] = PIO_XFER_MODE;
      if (ioctl(fd,SET_STRATEGY_XFER_MODE,data.data) != 0)
        perror("error in ioctl SET_STRATEGY_XFER_MODE");
      else
      {
        memset(buffer,0,bufsize);
        *bytes_read = read(fd,buffer,bufsize);
        if (*bytes_read < 0)
        {
          perror("PIO_XFER_MODE read error");
          status = 0;
        }
        else
          status = 1;
        ioctl(fd, UNMAP_VME_FOR_STRATEGY_PIO, data.data);
      }
    }
    close(fd);
  }
  else
    perror("Error opening VME device");
  return status;
}

int VmePioWrite(char *device, unsigned long addr, unsigned long mode, int bufsize, void *buffer, int *bytes_written)
{
  struct dmaex_ioctl_data data;
  int fd = open(device,O_RDWR);
  int status = 0;
  *bytes_written = 0;
  if (fd != -1)
  {
    data.data[0] = bufsize;
    data.data[1] = addr;
    data.data[2] = mode;
    if (ioctl(fd,SETUP_VME_FOR_STRATEGY_PIO,data.data) != 0)
      perror("error in ioctl SETUP_VME_FOR_STRATEGY_PIO");
    else
    {
      data.data[0] = PIO_XFER_MODE;
      if (ioctl(fd,SET_STRATEGY_XFER_MODE,data.data) != 0)
        perror("error in ioctl SET_STRATEGY_XFER_MODE");
      else
      {
        *bytes_written = write(fd,buffer,bufsize);
        if (*bytes_written < 0)
        {
          perror("PIO_XFER_MODE write error");
          status = 0;
        }
        else
          status = 1;
        ioctl(fd, UNMAP_VME_FOR_STRATEGY_PIO, data.data);
      }
    }
    close(fd);
  }
  else
    perror("Error opening VME device");
  return status;
}

int A14ReadChannel(int crate, int slot, int chan, int bufsize, short *buffer, int *bytes_read)
{
  /*
  int status = VmePioRead("/dev/dmaex0", 0x10700000 + chan * 0x40000, VME_A32_UDATA | VME_D16 | VME_BS_LWORD | VME_DENSE,
                      bufsize, buffer, bytes_read);
  if (status & 1)
  {           
    int i;
    short tmp;
    for (i=0;i<bufsize/sizeof(short);i+=2)
    {
      tmp = buffer[i];
      buffer[i] = buffer[i+1];
      buffer[i+1] = tmp;
    }
  }
  return (status);
  */
  return(VmePioRead("/dev/dmaex0", 0x10700000 + chan * 0x40000, VME_A32_UDATA | VME_D16 | VME_BS_BYTE | VME_DENSE,
                      bufsize, buffer, bytes_read));
}

int A14ReadSetup(int crate, int slot, int *setup)
{
  int bytes_read;
  return (VmePioRead("/dev/dmaex0", 0x10500000, VME_A32_UDATA | VME_D16 | VME_BS_LWORD | VME_DENSE,
                      sizeof(int), setup, &bytes_read));
}

int A14SetSetup(int crate, int slot, int *setup)
{
  int bytes_written;
  return (VmePioWrite("/dev/dmaex0", 0x10500000, VME_A32_UDATA | VME_D16 | VME_BS_LWORD | VME_DENSE,
                      sizeof(int), setup, &bytes_written));
}

int A14Wait(int crate, int slot)
{
  return VmeWaitForInterrupt("/dev/dmaex0",3,0xce);
}

