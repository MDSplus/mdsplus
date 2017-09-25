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
/****************************************************************

   status         = scsi_io(int scsiDevice,
                            int direction
                            unsigned char *scsi_cmd,
                            unsigned char *scsi_cmd_len,
                            unsigned char *user_buffer,
                            unsigned char *user_buflen,
                            unsigned char *sense_data_buffer,
                            unsigned char sense_data_buflen,
                            unsigned char *sense_data_retlen,
                            int           *transfer_bytcnt)

   scsi_io returns scsi status or -1 if some other error.

   Arguments:

     scsiDevice        - index of scsi device (0, 1, 2, 3,...)
     direction         - 0=control (no data), 1=read, 2=write
     scsi_cmd          - buffer containing scsi command
     scsi_cmd_len      - length of scsi_cmd in bytes
     user_buffer       - user buffer to receive data (read) or 
                         containing data to write (write)
     user_buflen       - size of user buffer in bytes
     sense_data_buffer - buffer to receive sense data
     sense_data_buflen - size of sense data buffer in bytes
     sense_data_retlen - address of unsigned char to receive number of
                         sense data bytes returned
     transfer_bytcnt   - address of int to receive number of bytes transfered.

   This routine is based on the linux sg driver version 3

*******************************************************************/
#include <stdio.h>
#include <scsi/scsi.h>
#include <scsi/sg.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#define ROUTINE_NAME "scsi_io"

#ifndef SG_FLAG_MMAP_IO
#define SG_FLAG_MMAP_IO 4
#endif

#define MIN_MAXBUF 65536
#define SEM_ID 80420
static int FDS[10] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
static int MAXBUF[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static int BUFFSIZE[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static int OpenScsi(int scsiDevice, char **buff_out);

int SGSetMAXBUF(int scsiDevice, int new)
{
  int old = -1;
  char *bufptr;
  int fd = -1;
  if (scsiDevice >= 0 || scsiDevice <= 9) {
    old = MAXBUF[scsiDevice];
    if (new >= MIN_MAXBUF) {
      int pagesize = getpagesize();
      int reqsize = ((new + pagesize - 1) / pagesize) * pagesize;
      MAXBUF[scsiDevice] = reqsize;
      if (FDS[scsiDevice] != -1) {
	close(FDS[scsiDevice]);
	FDS[scsiDevice] = -1;
      }
      fd = OpenScsi(scsiDevice, &bufptr);
      if ((fd != -1) && (BUFFSIZE[scsiDevice] != MAXBUF[scsiDevice]))
	fprintf(stderr, "Did not get requested buffer size, requested = %d, got = %d\n",
		MAXBUF[scsiDevice], BUFFSIZE[scsiDevice]);
    }
  }
  return old;
}

int SGGetMAXBUF(int scsiDevice)
{
  if (scsiDevice >= 0 || scsiDevice <= 9) {
    char *bufptr;
    OpenScsi(scsiDevice, &bufptr);
    return BUFFSIZE[scsiDevice];
  } else
    return -1;
}

static int OpenScsi(int scsiDevice, char **buff_out)
{
  char *buff = 0;
  static char *BUFFS[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  int fd = -1;
  if (scsiDevice >= 0 || scsiDevice <= 9) {
    if (FDS[scsiDevice] >= 0) {
      fd = FDS[scsiDevice];
      buff = BUFFS[scsiDevice];
    } else {
      char devnam[9];
      sprintf(devnam, "/dev/sg%d", scsiDevice);
      if ((fd = open(devnam, O_RDWR)) < 0) {
	char msg[512];
	sprintf(msg, "%s(): Error opening device \"%s\"", ROUTINE_NAME, devnam);
	perror(msg);
      } else {
	int reqsize;
	int pagesize = getpagesize();
	if (MAXBUF[scsiDevice] < MIN_MAXBUF) {
	  char *env = getenv("CAMAC_BUFFSIZE");
	  if (env)
	    MAXBUF[scsiDevice] = atoi(env);
	  if (MAXBUF[scsiDevice] < MIN_MAXBUF)
	    MAXBUF[scsiDevice] = MIN_MAXBUF;
	}
 try_again:
	reqsize = ((MAXBUF[scsiDevice] + pagesize - 1) / pagesize) * pagesize;
	BUFFSIZE[scsiDevice] = 0;
	if ((ioctl(fd, SG_SET_RESERVED_SIZE, &reqsize) < 0) ||
	    (ioctl(fd, SG_GET_RESERVED_SIZE, &BUFFSIZE[scsiDevice]) < 0) ||
	    (BUFFSIZE[scsiDevice] < MIN_MAXBUF)) {
	  fprintf(stderr, "%s(): could NOT allocate %d byte kernel buffer, max is %d\n",
		  ROUTINE_NAME, reqsize, BUFFSIZE[scsiDevice]);
	  close(fd);
	  fd = -1;
	} else {
	  buff = mmap(0, BUFFSIZE[scsiDevice], (PROT_READ | PROT_WRITE), MAP_SHARED, fd, 0);
	  if (buff == (char *)-1) {
	    fprintf(stderr, "%s(): error in mmap to scsi buffer, buffer = %d bytes\n", ROUTINE_NAME,
		    BUFFSIZE[scsiDevice]);
	    perror("mmap error");
	    if (MAXBUF[scsiDevice] != MIN_MAXBUF) {
	      MAXBUF[scsiDevice] = MIN_MAXBUF;
	      goto try_again;
	    } else {
	      close(fd);
	      fd = -1;
	    }
	  } else
	    BUFFS[scsiDevice] = buff;
	  FDS[scsiDevice] = fd;
	}
      }
    }
  } else {
    fprintf(stderr, "%s: scsi device number (%d) is out of range, must be between 0 and 9\n",
	    ROUTINE_NAME, scsiDevice);
  }
  *buff_out = buff;
  return fd;
}

int scsi_lock(int scsiDevice, int lock)
{
  static int semId = 0;
  int status = 0;
  struct sembuf psembuf;
  if (!semId) {
    semId = semget(SEM_ID, 10, 0);
    if (semId == -1) {
      if (errno == ENOENT) {
	int status;
	semId = semget(SEM_ID, 10, IPC_CREAT | 0x1ff);
	if (semId == -1) {
	  perror("Error creating locking semaphore");
	  semId = 0;
	} else {
	  int i;
	  union semun {
	    int val;
	  } arg;
	  arg.val = 1;
	  for (i = 0; i < 10; i++) {
	    status = semctl(semId, i, SETVAL, arg);
	    if (status == -1)
	      perror("Error accessing locking semaphore");
	  }
	}
      } else
	perror("Error accessing locking semaphore");
    }
  }
  psembuf.sem_num = scsiDevice;
  psembuf.sem_op = lock ? -1 : 1;
  psembuf.sem_flg = SEM_UNDO;
  status = semop(semId, &psembuf, 1);
  if (status == -1)
    perror("Error locking CAMAC system");
  return (status == 0);
}

int scsi_io(int scsiDevice, int direction, unsigned char *cmdp,
	    unsigned char cmd_len, char *buffer, unsigned int buflen,
	    unsigned char *sbp, unsigned char mx_sb_len,
	    unsigned char *sb_out_len, int *transfer_len)
{
  char *buf;
  int fd;
  int bytes_transfered = -1;
  int status = -1;
  int timeout = buflen * .02;
  struct sg_io_hdr sghdr;
  if (sb_out_len != 0)
    *sb_out_len = 0;
  fd = OpenScsi(scsiDevice, &buf);
  if (fd >= 0) {
    sghdr.interface_id = 'S';
    int use_mmap = buflen <= (unsigned int)MAXBUF[scsiDevice];
    //    if (buflen > MAXBUF[scsiDevice])
    //{
    //  fprintf( stderr, "%s(): buffer size (%d) too large, must be less than %d\n",ROUTINE_NAME,buflen,MAXBUF[scsiDevice]);
    //}
    //else
    //{
    switch (direction) {
    case 0:
      sghdr.dxfer_direction = SG_DXFER_NONE;
      break;
    case 1:
      sghdr.dxfer_direction = SG_DXFER_FROM_DEV;
      break;
    case 2:
      sghdr.dxfer_direction = SG_DXFER_TO_DEV;
      if (use_mmap)
	memcpy(buf, buffer, buflen);
      break;
    }
    sghdr.cmd_len = cmd_len;
    sghdr.mx_sb_len = mx_sb_len;
    sghdr.iovec_count = 0;
    sghdr.dxfer_len = buflen;
    sghdr.dxferp = buffer;
    sghdr.cmdp = cmdp;
    sghdr.sbp = sbp;
    sghdr.timeout = timeout > 10000 ? timeout : 10000;
    sghdr.flags = use_mmap ? SG_FLAG_MMAP_IO : 0;
    sghdr.pack_id = 0;
    if (ioctl(fd, SG_IO, &sghdr) >= 0) {
      status = sghdr.masked_status;
      if (512 - (buflen % 512) == (unsigned int)sghdr.resid)
	bytes_transfered = buflen;
      else
	bytes_transfered = buflen - sghdr.resid;
      if (bytes_transfered > (signed int)buflen)
	bytes_transfered = buflen;
      if ((direction == 1) && (bytes_transfered > 0) && use_mmap)
	memcpy(buffer, buf, bytes_transfered);
      if ((sghdr.sb_len_wr == 0) && (mx_sb_len > 0)) {
	int len = 0;
	unsigned char sense_cmd[] = { 3, 0, 0, 0, mx_sb_len, 0 };
	scsi_io(scsiDevice, 1, sense_cmd, sizeof(sense_cmd), (char *)sbp, mx_sb_len, 0, 0, 0, &len);
	if (sb_out_len != 0)
	  *sb_out_len = (unsigned char)len;
      } else if (sb_out_len != 0)
	*sb_out_len = sghdr.sb_len_wr;
    } else
      perror("ioctl error performing SCSI I/O");
  } else {
    fprintf(stderr, "Error opening scsi device\n");
  }
  if (transfer_len != 0)
    *transfer_len = bytes_transfered;
  if (status == -1)
    fprintf(stderr, "scsi_io is returning a -1 status\n");
  return status;
}
