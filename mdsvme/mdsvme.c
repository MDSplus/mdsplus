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
#include "vmp_reg.h"
#include <stdlib.h>
#include <string.h>

extern int VmeWaitForInterrupt(char *device, int irq, int vector);
extern int VmePioRead(char *device, unsigned long addr, unsigned long mode, int bufsize,
		      void *buffer, int *bytes_read);
extern int VmePioWrite(char *device, unsigned long addr, unsigned long mode, int bufsize,
		       void *buffer, int *bytes_written);

static void SigCatcher(int signo)
{
}

static int fd1 = 0, fd2 = 0;

int VmeWaitForInterrupt(char *device, int irq, int vector)
{
  struct dmaex_ioctl_data data;
  int fd = open("/dev/dmaex0", O_RDWR);
  int status = 0;
  if (fd != -1) {
    struct sigaction newsigaction, oldsigaction;
    sigset_t empty_set, new_set;
    newsigaction.sa_handler = SigCatcher;
    sigemptyset(&newsigaction.sa_mask);
    newsigaction.sa_flags = 0;
    sigaction(SIGUSR1, &newsigaction, &oldsigaction);
    sigaction(SIGINT, &newsigaction, &oldsigaction);
    sigemptyset(&new_set);
    sigaddset(&new_set, SIGUSR1);
    sigprocmask(SIG_BLOCK, &new_set, NULL);
    sigemptyset(&empty_set);
    data.data[0] = irq;
    data.data[1] = vector;
    if (ioctl(fd, SET_INT_HANDLER, data.data) != 0)
      perror("error in ioctl SET_INT_HANDLER");
    else {
      sigsuspend(&empty_set);
      if (ioctl(fd, CLR_INT_HANDLER, data.data) != 0)
	perror("error in ioctl CLR_INT_HANDLER");
      else
	status = 0;
    }
    close(fd);
  } else
    perror("Error opening VME device");
  return status;
}

int VmePioRead(char *device, unsigned long addr, unsigned long mode, int bufsize, void *buffer,
	       int *bytes_read)
{
  struct dmaex_ioctl_data data;
  int fd = open(device, O_RDWR);
  int status = 0;
  *bytes_read = 0;
  if (fd != -1) {
    data.data[0] = bufsize;
    data.data[1] = addr;
    data.data[2] = mode;
    if (ioctl(fd, SETUP_VME_FOR_STRATEGY_PIO, data.data) != 0)
      perror("error in ioctl SETUP_VME_FOR_STRATEGY_PIO");
    else {
      data.data[0] = PIO_XFER_MODE;
      if (ioctl(fd, SET_STRATEGY_XFER_MODE, data.data) != 0)
	perror("error in ioctl SET_STRATEGY_XFER_MODE");
      else {
	memset(buffer, 0, bufsize);
	*bytes_read = read(fd, buffer, bufsize);
	if (*bytes_read < 0) {
	  perror("PIO_XFER_MODE read error");
	  status = 0;
	} else
	  status = 1;
	ioctl(fd, UNMAP_VME_FOR_STRATEGY_PIO, data.data);
      }
    }
    close(fd);
  } else
    perror("Error opening VME device");
  return status;
}

int VmePioWrite(char *device, unsigned long addr, unsigned long mode, int bufsize, void *buffer,
		int *bytes_written)
{
  struct dmaex_ioctl_data data;
  int fd = open(device, O_RDWR);
  int status = 0;
  *bytes_written = 0;
  if (fd != -1) {
    data.data[0] = bufsize;
    data.data[1] = addr;
    data.data[2] = mode;
    if (ioctl(fd, SETUP_VME_FOR_STRATEGY_PIO, data.data) != 0)
      perror("error in ioctl SETUP_VME_FOR_STRATEGY_PIO");
    else {
      data.data[0] = PIO_XFER_MODE;
      if (ioctl(fd, SET_STRATEGY_XFER_MODE, data.data) != 0)
	perror("error in ioctl SET_STRATEGY_XFER_MODE");
      else {
	*bytes_written = write(fd, buffer, bufsize);
	if (*bytes_written < 0) {
	  perror("PIO_XFER_MODE write error");
	  status = 0;
	} else
	  status = 1;
	ioctl(fd, UNMAP_VME_FOR_STRATEGY_PIO, data.data);
      }
    }
    close(fd);
  } else
    perror("Error opening VME device");
  return status;
}

int PioRead(char *device, unsigned int addr, unsigned int mode, int bufsize, void *buffer)
{
  struct pio_info setVme, getVme;
  int fd = open(device, O_RDWR);
  int status = 0;
  void *pMapAdr;
  if (fd != -1) {
    setVme.pio_addr = addr;
    setVme.pio_size = bufsize;
    setVme.pio_am = mode;
    setVme.pio_access = HANDLE_LONGWORD;
    if (ioctl(fd, VMP_MAP_PIO_ADDR, &setVme) < 0)
      perror("error in ioctl VMP_MAP_PIO_ADDR");
    else {
      memset(buffer, 0, bufsize);
      pMapAdr =
	  (int *)mmap((caddr_t) 0, setVme.pio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
      if (pMapAdr == (int *)-1) {
	printf("### error in mmap\n");
      } else {
	memcpy(buffer, pMapAdr, bufsize);
	if (ioctl(fd, VMP_UNMAP_PIO_ADDR, &setVme) < 0)
	  printf("### error in ioctl-UNMAP\n");
	if (munmap((caddr_t) pMapAdr, setVme.pio_size) < 0)
	  printf("### error in munmap\n");
	status = 1;
      }
    }
    close(fd);
  } else
    perror("Error opening VME device");
  return status;
}

int PioWrite(char *device, unsigned int addr, unsigned int mode, int bufsize, void *buffer)
{
  struct pio_info setVme, getVme;
  int fd = open(device, O_RDWR);
  int status = 0;
  void *pMapAdr;
  if (fd != -1) {
    setVme.pio_addr = addr;
    setVme.pio_size = bufsize;
    setVme.pio_am = mode;
    setVme.pio_access = HANDLE_LONGWORD;
    if (ioctl(fd, VMP_MAP_PIO_ADDR, &setVme) < 0)
      perror("error in ioctl VMP_MAP_PIO_ADDR");
    else {
      pMapAdr =
	  (int *)mmap((caddr_t) 0, setVme.pio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
      if (pMapAdr == (int *)-1) {
	printf("### error in mmap\n");
      } else {
	memcpy(pMapAdr, buffer, bufsize);
	if (ioctl(fd, VMP_UNMAP_PIO_ADDR, &setVme) < 0)
	  printf("### error in ioctl-UNMAP\n");
	if (munmap((caddr_t) pMapAdr, setVme.pio_size) < 0)
	  printf("### error in munmap\n");
	status = 1;
      }
    }
    close(fd);
  } else
    perror("Error opening VME device");
  return status;
}

void termfunc(int dummy)
{
  if (fd1 != 0) {
    ioctl(fd1, VMP_DEL_INTR);
    close(fd1);
  }
  if (fd2 != 0) {
    ioctl(fd2, VMP_DEL_INTR);
    close(fd2);
  }
  exit(1);
}

int WaitForInterrupt(char *device1, int priority1, int vector1, char *device2, int priority2,
		     int vector2)
{
  struct vmpintr_info setIntr;
  fd_set mask;
  int status = 0;
  fd1 = open(device1, O_RDWR);
  if (fd1 != -1) {
    signal(SIGUSR1, SIG_IGN);
    signal(SIGUSR2, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGCLD, SIG_IGN);
    signal(SIGSYS, SIG_IGN);
    signal(SIGIOT, SIG_IGN);
    signal(SIGINT, termfunc);
    signal(SIGSEGV, termfunc);
    signal(SIGBUS, termfunc);
    signal(SIGTERM, termfunc);
    setIntr.priority = priority1;
    setIntr.vector = vector1;
    ioctl(fd1, VMP_ADD_INTR, &setIntr);
    FD_ZERO(&mask);
    FD_SET(fd1, &mask);
    if (strlen(device2) > 0) {
      fd2 = open(device2, O_RDWR);
      setIntr.priority = priority2;
      setIntr.vector = vector2;
      ioctl(fd2, VMP_ADD_INTR, &setIntr);
      FD_SET(fd2, &mask);
    }
    select(FD_SETSIZE, &mask, NULL, NULL, NULL);
    if (FD_ISSET(fd1, &mask))
      status = 1;
    if (fd2 != 0 && FD_ISSET(fd2, &mask))
      status = 2;
    ioctl(fd1, VMP_DEL_INTR);
    close(fd1);
    fd1 = 0;
    if (fd2 != 0) {
      ioctl(fd2, VMP_DEL_INTR);
      close(fd2);
      fd2 = 0;
    }
  } else
    perror("Error opening VME device");
  return status;
}
