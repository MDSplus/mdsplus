
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
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/mman.h>

#define MAXBUF  100000
#define ROUTINE_NAME "scsi_io"

#ifndef SG_FLAG_MMAP_IO
#define SG_FLAG_MMAP_IO 4
#endif

static int OpenScsi(int scsiDevice, char **buff_out)
{
  char *buff = 0;
  static int FDS[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}; 
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
        int reqsize = MAXBUF;
        int gotsize;
        int pagesize = getpagesize();
        reqsize = ((reqsize + pagesize -1) / pagesize) * pagesize;
	if( ( ioctl(fd, SG_SET_RESERVED_SIZE, &reqsize) < 0) ||
	    ( ioctl(fd, SG_GET_RESERVED_SIZE, &gotsize) < 0) ||
            ( gotsize < reqsize))   
        {
	  fprintf( stderr, "%s(): could NOT allocate %d byte kernel buffer, max is %d\n", ROUTINE_NAME, reqsize, gotsize );
          close(fd);
          fd = -1;
	}
        else
	{
          buff = mmap(0,gotsize,(PROT_READ | PROT_WRITE),MAP_SHARED, fd,0);
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
  if (buflen > MAXBUF)
  {
    fprintf( stderr, "%s(): buffer size (%d) too large, must be less than %d\n",ROUTINE_NAME,buflen,MAXBUF);
  }
  else
  {
    fd = OpenScsi(scsiDevice,&buf);
    if (fd >= 0)
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
