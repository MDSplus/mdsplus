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
#define _LARGEFILE_SOURCE
#define _FILE_OFFSET_BITS 64
#define __USE_FILE_OFFSET64

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define MDS_IO_OPEN_K   1
#define MDS_IO_CLOSE_K  2
#define MDS_IO_LSEEK_K  3
#define MDS_IO_READ_K   4
#define MDS_IO_WRITE_K  5
#define MDS_IO_LOCK_K   6
#define MDS_IO_EXISTS_K 7
#define MDS_IO_REMOVE_K 8
#define MDS_IO_RENAME_K 9
#define MDS_IO_READ_X_K 10

#define MDS_IO_O_CREAT  0x00000040
#define MDS_IO_O_TRUNC  0x00000200
#define MDS_IO_O_EXCL   0x00000080
#define MDS_IO_O_WRONLY 0x00000001
#define MDS_IO_O_RDONLY 0x00004000
#define MDS_IO_O_RDWR   0x00000002

#define MAX_IO_SIZE 1000000

#include "mdsip_connections.h"

struct mdsfile {
  int socket;
  int fd;
};

static float getSeconds(struct timeval *time)
{
  struct timeval now;
  float ans;
  gettimeofday(&now, 0);
  ans = (now.tv_sec + now.tv_usec * 1E-6) - (time->tv_sec + time->tv_usec * 1E-6);
  *time = now;
  return ans;
}

static void printHelp()
{
  printf("Usage: mdscp [--help] [-?] [--streams=n] [--statistics] srcfile dstfile\n\n"
	 "    --streams=n can be used to specify the number of parallel streams between 1 and 32.\n"
	 "    --statistics can be specified to print out timing information.\n"
	 "    file specifications can be used to specify a source or destination host and port. For example:\n\n"
	 "        mdscp --streams=32 myfile.dat myremhost:8000::/mydir/mysubdir/myfile.dat\n");
}

static int doOpen(int streams, char *name, int options, int mode, struct mdsfile *mfile)
{
  char *tmp = strcpy((char *)malloc(strlen(name) + 1), name);
  char *hostpart = tmp;
  char *filepart = strstr(tmp, "::");
  int status;
  if (filepart) {
    int info[3];
    hostpart = tmp;
    filepart[0] = '\0';
    filepart += 2;
    mfile->socket = ConnectToMds(hostpart);
    if (mfile->socket == -1) {
      fprintf(stderr, "Error connecting to host: %s\n", hostpart);
      return -1;
    }
    info[0] = strlen(filepart) + 1;
    if (O_CREAT == 0x0200) {	/* BSD */
      if (options & O_CREAT)
	options = (options & ~O_CREAT) | 0100;
      if (options & O_TRUNC)
	options = (options & ~O_TRUNC) | 01000;
      if (options & O_EXCL)
	options = (options & ~O_EXCL) | 0200;
    }
    info[1] = (options & O_CREAT ? MDS_IO_O_CREAT : 0) |
	(options & O_TRUNC ? MDS_IO_O_TRUNC : 0) |
	(options & O_EXCL ? MDS_IO_O_EXCL : 0) |
	(options & O_WRONLY ? MDS_IO_O_WRONLY : 0) |
	(options & O_RDONLY ? MDS_IO_O_RDONLY : 0) | (options & O_RDWR ? MDS_IO_O_RDWR : 0);
    info[2] = (int)mode;
    status =
	SendArg(mfile->socket, MDS_IO_OPEN_K, 0, 0, 0, sizeof(info) / sizeof(int), info, filepart);
    if (status & 1) {
      char dtype;
      short length;
      char ndims;
      int dims[7];
      int numbytes;
      void *dptr;
      void *msg = 0;
      int sts;
      if (((sts =
	    GetAnswerInfoTS(mfile->socket, &dtype, &length, &ndims, dims, &numbytes, &dptr,
			    &msg)) & 1) && (length == sizeof(mfile->fd))) {
	memcpy(&mfile->fd, dptr, sizeof(mfile->fd));
	status = 0;
      } else {
	printf("Err in GetAnswerInfoTS in io_open_remote: status = %d, length = %d\n", sts, length);
	status = -1;
      }
      if (msg)
	free(msg);
    } else {
      fprintf(stderr, "Error opening file: %s\n", name);
      status = -1;
    }
  } else {
    mfile->socket = -1;
    mfile->fd = open(name, options, mode);
    if (mfile->fd == -1) {
      char *fmt = "Error opening file: %s";
      char *msg = (char *)malloc(strlen(name) + strlen(fmt) + 10);
      sprintf(msg, fmt, name);
      perror(msg);
      free(msg);
      status = -1;
    } else
      status = 0;
  }
  free(tmp);
  return status;
}

off_t getSize(struct mdsfile * file)
{
  if (file->socket == -1) {
    off_t ans = lseek(file->fd, 0, SEEK_END);
    lseek(file->fd, 0, SEEK_SET);
    return ans;
  } else {
    off_t ret = -1;
    int info[] = { 0, 0, 0, 0, 0 };
    int sock = file->socket;
    int status;
    info[1] = file->fd;
    info[4] = SEEK_END;
    *(off_t *) (&info[2]) = 0;
    status = SendArg(sock, MDS_IO_LSEEK_K, 0, 0, 0, sizeof(info) / sizeof(int), info, 0);
    if (status & 1) {
      char dtype;
      unsigned short length;
      char ndims;
      int dims[7];
      int numbytes;
      void *dptr;
      void *msg = 0;
      if ((GetAnswerInfoTS(sock, &dtype, &length, &ndims, dims, &numbytes, &dptr, &msg) & 1)
	  && (length >= sizeof(int))) {
	ret = 0;
	memcpy(&ret, dptr, (length > sizeof(ret)) ? sizeof(ret) : length);
      }
      if (msg)
	free(msg);
      info[4] = SEEK_SET;
    }
    status = SendArg(sock, MDS_IO_LSEEK_K, 0, 0, 0, sizeof(info) / sizeof(int), info, 0);
    if (status & 1) {
      char dtype;
      unsigned short length;
      char ndims;
      int dims[7];
      int numbytes;
      void *dptr;
      void *msg = 0;
      int dumret;
      if ((GetAnswerInfoTS(sock, &dtype, &length, &ndims, dims, &numbytes, &dptr, &msg) & 1)
	  && (length >= sizeof(int))) {
	dumret = 0;
	memcpy(&dumret, dptr, (length > sizeof(dumret)) ? sizeof(dumret) : length);
      }
      if (msg)
	free(msg);
    }
    return ret;
  }
}


off_t doRead(struct mdsfile * file, off_t count, void *buff)
{
  if (file->socket == -1) {
    return read(file->fd, buff, count);
  } else {
    off_t ret = -1;
    int info[] = { 0, 0, 0 };
    int sock = file->socket;
    int status;
    info[1] = file->fd;
    info[2] = count;
    status = SendArg(sock, MDS_IO_READ_K, 0, 0, 0, sizeof(info) / sizeof(int), info, 0);
    if (status & 1) {
      char dtype;
      unsigned short length;
      char ndims;
      int dims[7];
      int numbytes;
      void *dptr;
      void *msg = 0;
      if ((GetAnswerInfoTS(sock, &dtype, &length, &ndims, dims, &numbytes, &dptr, &msg) & 1)) {
	ret = numbytes;
	if (ret)
	  memcpy(buff, dptr, ret);
      }
      if (msg)
	free(msg);
    }
    return ret;
  }
}

static off_t doWrite(struct mdsfile *file, off_t count, void *buff)
{
  if (file->socket == -1) {
    return write(file->fd, buff, count);
  } else {
    off_t ret = -1;
    int info[] = { 0, 0 };
    int sock = file->socket;
    int status;
    info[1] = file->fd;
    info[0] = count;
    status = SendArg(sock, MDS_IO_WRITE_K, 0, 0, 0, sizeof(info) / sizeof(int), info, buff);
    if (status & 1) {
      char dtype;
      unsigned short length;
      char ndims;
      int dims[7];
      int numbytes;
      void *dptr;
      void *msg = 0;
      if ((GetAnswerInfoTS(sock, &dtype, &length, &ndims, dims, &numbytes, &dptr, &msg) & 1)) {
	ret = (off_t) * (int *)dptr;
      }
      if (msg)
	free(msg);
    }
    return ret;
  }
}

static int doClose(struct mdsfile *file)
{
  if (file->socket == -1) {
    return close(file->fd);
  } else {
    int ret = -1;
    int info[] = { 0, 0 };
    int sock = file->socket;
    int status;
    info[1] = file->fd;
    status = SendArg(sock, MDS_IO_CLOSE_K, 0, 0, 0, sizeof(info) / sizeof(int), info, 0);
    if (status & 1) {
      char dtype;
      short length;
      char ndims;
      int dims[7];
      int numbytes;
      void *dptr;
      void *msg = 0;
      if ((GetAnswerInfoTS(sock, &dtype, &length, &ndims, dims, &numbytes, &dptr, &msg) & 1)
	  && (length == sizeof(ret)))
	memcpy(&ret, dptr, sizeof(ret));
      if (msg)
	free(msg);
    }
    return ret;
  }
}

int main(int argc, char **argv)
{
  int c;
  int streams = 1;
  int stats = 0;
  int error = 0;
  int help = 0;
  struct timeval time, timeStart;
  while (1) {
    int this_option_optind = optind ? optind : 1;
    int option_index = 0;
    static struct option long_options[] = {
      {"streams", 1, 0, 0},
      {"statistics", 0, 0, 0},
      {"help", 0, 0, 0},
      {0, 0, 0, 0}
    };
    c = getopt_long(argc, argv, "", long_options, &option_index);
    if (c == -1)
      break;
    switch (c) {
    case 0:
      switch (option_index) {
      case 0:
	streams = atoi(optarg);
	if (streams < 1 || streams > 32) {
	  printf("Invalid number of streams specified.");
	  error = 1;
	}
	break;
      case 1:
	stats = 1;
	break;
      case 2:
	help = 1;
      }
      break;
    default:
      printf("Invalid option specified.");
      error = 1;
    }
  }
  if (error == 0) {
    if (argc - optind != 2) {
      printf("Invalid number of arguments provided.");
      error = 1;
    } else {
      char *srcfile = argv[optind];
      char *destfile = argv[optind + 1];
      off_t inSize;
      void *buff;
      off_t offset = 0;
      struct mdsfile inFile, outFile;
      int status;
      if (stats) {
	gettimeofday(&time, 0);
	timeStart = time;
      }
      status = doOpen(streams, srcfile, O_RDONLY, 0, &inFile);
      if (status == -1) {
	printf("Error opening source file: %s\n", srcfile);
	return 1;
      }
      if (stats)
	printf("Time to open source file: %g\n", getSeconds(&time));
      status = doOpen(streams, destfile, O_WRONLY | O_CREAT, 0777, &outFile);
      if (status == -1) {
	printf("Error opening destination file: %s\n", destfile);
	return 1;
      }
      if (stats)
	printf("Time to open destination file: %g\n", getSeconds(&time));
      inSize = getSize(&inFile);
      if (inSize > 2000) {
	if (streams > 1) {
	  if (inFile.socket != -1)
	    MdsSetStreams(inFile.socket, streams);
	  if (outFile.socket != -1)
	    MdsSetStreams(outFile.socket, streams);
	  if (stats)
	    printf("Time to initialize parallel streams: %g\n", getSeconds(&time));
	}
      }
      buff = malloc(inSize > MAX_IO_SIZE ? MAX_IO_SIZE : inSize);
      while (offset < inSize) {
	off_t bytes_this_time = inSize - offset;
	off_t bytes;
	if (bytes_this_time > MAX_IO_SIZE)
	  bytes_this_time = MAX_IO_SIZE;
	bytes = doRead(&inFile, bytes_this_time, buff);
	if (bytes != bytes_this_time) {
	  fprintf(stderr, "Error reading from source file");
	  return 1;
	}
	bytes = doWrite(&outFile, bytes_this_time, buff);
	if (bytes != bytes_this_time) {
	  fprintf(stderr, "Error writing to destination file");
	  return 1;
	}
	offset += bytes_this_time;
      }
      doClose(&inFile);
      doClose(&outFile);
      if (stats) {
	float secs;
	printf("Time to copy files: %g\n", secs = getSeconds(&time));
	printf("Total time to copy files: %g\n", getSeconds(&timeStart));
	printf("Copy rate is %g MBytes/sec\n", inSize / 1e6 / secs);
      }
    }
  }
  if (error) {
    printf("\n\n");
    printHelp();
    return 1;
  } else if (help) {
    printHelp();
  }
  return 0;
}
