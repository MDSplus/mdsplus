
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

#define ROUTINE_NAME "scsi_io"

#ifndef SG_FLAG_MMAP_IO
#define SG_FLAG_MMAP_IO 4
#endif

#define MIN_MAXBUF 65538

static int FDS[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}; 
static int MAXBUF[10] =   {0,0,0,0,0,0,0,0,0,0};
static int BUFFSIZE[10] = {0,0,0,0,0,0,0,0,0,0};
static int OpenScsi(int scsiDevice, char **buff_out);

int SGSetMAXBUF(int scsiDevice, int new)
{
  int old=-1;
  char *bufptr;
  int fd = -1;
  if (scsiDevice >= 0 || scsiDevice <=9)
  {
    old=MAXBUF[scsiDevice];
    if (new >= MIN_MAXBUF)
    {
      int pagesize = getpagesize();
      int reqsize = ((new + pagesize -1) / pagesize) * pagesize;
      MAXBUF[scsiDevice]=reqsize;
      if (FDS[scsiDevice] != -1)
      {
	close(FDS[scsiDevice]);
	FDS[scsiDevice]=-1;
      }
      fd = OpenScsi(scsiDevice,&bufptr);
      if ((fd != -1) && (BUFFSIZE[scsiDevice] != MAXBUF[scsiDevice]))
        fprintf(stderr,"Did not get requested buffer size, requested = %d, got = %d\n",MAXBUF[scsiDevice],BUFFSIZE[scsiDevice]);
    }
  }
  return old;
}

int SGGetMAXBUF(int scsiDevice)
{
  if (scsiDevice >= 0 || scsiDevice <= 9)
  {
    char *bufptr;
    int fd = OpenScsi(scsiDevice,&bufptr);
    return BUFFSIZE[scsiDevice];
  }
  else
    return -1;
}

static int OpenScsi(int scsiDevice, char **buff_out)
{
  char *buff = 0;
  static char *BUFFS[10] = {0,0,0,0,0,0,0,0,0,0};
  int fd = -1;
  if (scsiDevice >= 0 || scsiDevice <= 9)
  {
    if (FDS[scsiDevice] >= 0)
    {
      fd = FDS[scsiDevice];
      buff = BUFFS[scsiDevice];
    }
    else
    {
      char devnam[9];
      sprintf(devnam,"/dev/sg%d", scsiDevice);
      if ( (fd = open(devnam, O_RDWR)) < 0 )
      {
        char msg[512];
        sprintf(msg,"%s(): Error opening device \"%s\"",ROUTINE_NAME,devnam); 
        perror(msg);
      }
      else
      {
        int reqsize;
        int pagesize = getpagesize();
        if (MAXBUF[scsiDevice] < MIN_MAXBUF)
	{
          char *env = getenv("CAMAC_BUFFSIZE");
          if (env) MAXBUF[scsiDevice] = atoi(env);
          if (MAXBUF[scsiDevice] < MIN_MAXBUF)
	    MAXBUF[scsiDevice] = MIN_MAXBUF;
	}
        reqsize = ((MAXBUF[scsiDevice] + pagesize -1) / pagesize) * pagesize;
	if( ( ioctl(fd, SG_SET_RESERVED_SIZE, &reqsize) < 0) ||
	    ( ioctl(fd, SG_GET_RESERVED_SIZE, &BUFFSIZE[scsiDevice]) < 0) ||
	    ( BUFFSIZE[scsiDevice] < MIN_MAXBUF ))
        {
	  fprintf( stderr, "%s(): could NOT allocate %d byte kernel buffer, max is %d\n", ROUTINE_NAME, reqsize, BUFFSIZE[scsiDevice] );
          close(fd);
          fd = -1;
	}
        else
	{
          buff = mmap(0,BUFFSIZE[scsiDevice],(PROT_READ | PROT_WRITE),MAP_SHARED, fd,0);
          if (buff == 0)
	  {
            fprintf( stderr, "%(): error in mmap to scsi buffer, errno = %d\n", ROUTINE_NAME, errno);
            close(fd);
            fd = -1;
          }
          else
            BUFFS[scsiDevice] = buff;
          FDS[scsiDevice] = fd;
        }
      }
    }
  }
  else
  {
    fprintf( stderr, "%(): scsi device number (%d) is out of range, must be between 0 and 9\n", ROUTINE_NAME, scsiDevice);
  }
  *buff_out = buff;
  return fd;
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
  struct sg_io_hdr sghdr = {'S'};
  if (sb_out_len != 0)
    *sb_out_len = 0;
  fd = OpenScsi(scsiDevice,&buf);
  if (fd >= 0)
  {
    if (buflen > MAXBUF[scsiDevice])
    {
      fprintf( stderr, "%s(): buffer size (%d) too large, must be less than %d\n",ROUTINE_NAME,buflen,MAXBUF[scsiDevice]);
    }
    else
    {
      switch (direction)
      {
      case 0: sghdr.dxfer_direction = SG_DXFER_NONE; break;
      case 1: sghdr.dxfer_direction = SG_DXFER_FROM_DEV; break;
      case 2: sghdr.dxfer_direction = SG_DXFER_TO_DEV;
              memcpy(buf,buffer,buflen);
              break;
      }
      sghdr.cmd_len = cmd_len;
      sghdr.mx_sb_len = mx_sb_len;
      sghdr.iovec_count = 0;
      sghdr.dxfer_len = buflen;
      sghdr.dxferp = 0;
      sghdr.cmdp = cmdp;
      sghdr.sbp = sbp;
      sghdr.timeout = 10000;
      sghdr.flags = SG_FLAG_MMAP_IO;
      sghdr.pack_id = 0;
      if (ioctl(fd, SG_IO, &sghdr) >= 0)
      {
        status = sghdr.masked_status;
        bytes_transfered = buflen - sghdr.resid;
        if (bytes_transfered > buflen)
          bytes_transfered = buflen;
        if ((direction == 1) && (bytes_transfered > 0))
          memcpy(buffer,buf,bytes_transfered);
        if ((sghdr.sb_len_wr == 0) && (mx_sb_len > 0))
        {
          int len = 0;
          unsigned char sense_cmd[] = {3,0,0,0,mx_sb_len,0}; 
          scsi_io(scsiDevice, 1, sense_cmd, sizeof(sense_cmd), sbp, mx_sb_len, 0, 0, 0, &len);
          if (sb_out_len != 0)
            *sb_out_len = (unsigned char)len;
        }
        else if (sb_out_len != 0)
          *sb_out_len = sghdr.sb_len_wr;
      }
    }
  }
  if (transfer_len != 0)
    *transfer_len = bytes_transfered;
  return status;
}
