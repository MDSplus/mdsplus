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
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include "acq32ioctl.h"
#include <mdsplus/mdsconfig.h>
#ifndef MAX
#define MAX(a,b) ((a) > (b)) ? (a) : (b)
#endif
#ifndef MIN
#define MIN(a,b) ((a) < (b)) ? (a) : (b)
#endif

#define MAX_CHUNK_SIZE 1024*1024

EXPORT int OPEN(const char *pathname, int flags)
{
  int fd = open(pathname, flags);
  if (fd == -1) {
    char msg[132];
    sprintf(msg, "error opening %s", pathname);
    perror(msg);
  }
  return fd;
}

EXPORT int READ(int fd, void *buf, size_t count)
{
  int bytes = 0;
  int bytes_to_read = count;
  while ((bytes_to_read > 0) && (bytes >= 0)) {
    bytes = read(fd, buf, bytes_to_read);
    if (bytes < 0) {
      char msg[132];
      sprintf(msg, "READ returned zero, quit at %zd\n", count - bytes_to_read);
      perror(msg);
      break;

    }
    bytes_to_read -= bytes;
    buf += bytes;
  }
  return count - bytes_to_read;
}

EXPORT int CLOSE(int fd)
{
  return close(fd);
}

EXPORT FILE *FOPEN(const char *fname, const char *mode)
{
  return fopen(fname, mode);
}

EXPORT int FCLOSE(FILE * fd)
{
  return fclose(fd);
}

EXPORT size_t FREAD(void *ptr, size_t size, size_t nmemb, FILE * stream)
{
  unsigned int samples_to_read = nmemb;
  int chunk_size;
  int this_chunk = 0;

  while ((samples_to_read != 0) && (this_chunk >= 0)) {

    chunk_size = MIN(MAX_CHUNK_SIZE / size, samples_to_read);
    this_chunk = fread(ptr, size, chunk_size, stream);

    if (this_chunk <= 0) {
      fprintf(stderr, "fread returned zero, quit at %zd\n", nmemb - samples_to_read);
      break;
    }

    ptr += this_chunk * size;
    samples_to_read -= this_chunk;
  }

  return nmemb - samples_to_read;
}

/* size_t FREAD( void *ptr, size_t size, size_t nmemb, FILE *stream) */
/* { */
/*   int samples_to_read = nmemb; */
/*   int chunk_size; */
/*   int this_chunk=0; */
/*   while ((samples_to_read > 0) && (this_chunk >= 0)) { */
/*     chunk_size = MIN(MAX_CHUNK_SIZE/size, samples_to_read); */
/*     this_chunk = fread(ptr, size, chunk_size, stream); */
/*     ptr += this_chunk*size; */
/*     samples_to_read -= this_chunk; */
/*   } */
/*   return ((this_chunk==0) ? nmemb : this_chunk); */
/* } */

EXPORT size_t FWRITE(void *ptr, size_t size, size_t nmemb, FILE * stream)
{
  return fwrite(ptr, size, nmemb, stream);
}

EXPORT int FSEEK(FILE * stream, long offset, int whence)
{
  return fseek(stream, offset, whence);
}

EXPORT int DMARead(short *buffer, const char *fname, int *chan, int *samples, int *active_chans)
{
  int lstart = 0;
  int lend = *samples;
  int linc = *active_chans;
  unsigned length = lstart + lend * linc * sizeof(short);
  int in, out;
  int fd;
  short *region;

  /*  printf("Starting DMARead lstart = %d lend = %d linc = %d, length = %d\n", lstart, lend, linc, length); */
  if ((fd = open(fname, O_RDWR)) < 0) {
    fprintf(stderr, "DMARead: failed to open device \"%s\" - ", fname);
    perror("");
    return -1;
  }
  /* printf("about to mmap (null, %d, PROT_READ|PROT_WRITE, MAP_SHARED, %d, 0)\n", length, fd); */
  region = (short *)mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  if (region == (short *)-1) {
    perror("DMARead mmap");
    return 1;
  }
  for (in = *chan, out = 0; out < lend; in += linc, out++) {
    buffer[out] = region[in];
  }
  /* printf("%d\n", out); */
  munmap((void *)region, length);
  close(fd);

  return out;
}

EXPORT int DMARead2(short *buffer, const char *fname, int *chan, int *samples, int *active_chans,
	     int *start, int *end, int *inc, float *coeffs, int *num_coeffs)
{
  int sample;
  int lstart = 0;
  int lend = *samples;
  int linc = *active_chans;
  unsigned length = lstart + lend * linc * sizeof(short);
  int out;
  int fd;
  short *region;
  //int i;

  /* printf("Starting DMARead lstart = %d lend = %d linc = %d, length = %d\n", lstart, lend, linc, length); */
  if ((fd = open(fname, O_RDWR)) < 0) {
    fprintf(stderr, "DMARead: failed to open device \"%s\" - ", fname);
    perror("");
    return -1;
  }
  /* printf("about to mmap (null, %d, PROT_READ|PROT_WRITE, MAP_SHARED, %d, 0)\n", length, fd); */
  region = (short *)mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  if (region == (short *)-1) {
    perror("DMARead mmap");
    return 1;
  }
  /*
     printf("about to resample start=%d, end=%d, inc=%d, num_coeffs=%d\n", *start, *end, *inc, *num_coeffs);

     printf("the coefs are: ");
     for (i = 0; i< *num_coeffs; i++)
     printf("%f ", coeffs[i]);
     printf("\n");
   */

  out = 0;
  for (sample = *start; sample < *end; sample += *inc) {
    int i;
    float sam = 0;
    for (i = 0; i < *num_coeffs; i++) {
      int idx = (sample + (i - *num_coeffs / 2)) * linc + *chan;
      if (idx < 0) {
	/*
	   printf("front sample = %d i = %d idx = %d ", sample, i, idx);
	 */
	idx = *chan;
	/*
	   printf("%d\n", idx);
	 */
      }
      if (idx >= ((int)(length / 2))) {
	/*
	   printf("back sample = %d i = %d idx = %d ", sample, i, idx);
	 */
	idx = (*samples - 1) * linc + *chan;
	/*
	   printf("%d\n", idx);
	 */
      }
      sam += (region[idx] * coeffs[i]);
      /*
         printf("idx = %d src= %d ans = %f ", idx, (int)region[idx], sam);
       */
    }
    /* printf("|"); */
    buffer[out++] = (int)sam;
  }

  printf("%d\n", out);
  munmap((void *)region, length);
  close(fd);

  return out;
}

EXPORT int DMARead3(short *buffer, const char *fname, int *start, int *end, int *inc, float *coeffs,
	     int *num_coeffs)
{
  int samples_to_go;
  int linc = (*num_coeffs <= 1) ? *inc : 1;
  int samples = (*end - *start + 1) / linc;
  unsigned length = samples * sizeof(short);
  int fd;
  short *region;
  struct READ_LOCALBUF_DESCR buffer_def;
  int rc;
  int idx, cidx;
  float sam;

  /*
     printf("Starting DMARead3 start = %d end = %d linc = %d\n", *start, *end, linc);
   */
  if ((fd = open(fname, O_RDONLY)) < 0) {
    fprintf(stderr, "DMARead: failed to open device \"%s\" - ", fname);
    perror("");
    return -1;
  }
  /*
     printf("about to mmap (null, %d, PROT_READ, MAP_SHARED, %d, 0)\n", length, fd);
   */
  region = (short *)mmap(NULL, length, PROT_READ, MAP_SHARED, fd, 0);

  if (region == (short *)-1) {
    perror("DMARead mmap");
    return 1;
  }
  buffer_def.istart = *start;
  buffer_def.istride = linc;
  buffer_def.buffer = (short *)region;
  for (samples_to_go = length / 2; samples_to_go > 0; samples_to_go -= buffer_def.nsamples) {
    buffer_def.nsamples = MIN(samples_to_go, MAX_CHUNK_SIZE);
    /*
       printf("about to ioctl istart = %d, istride= %d, nsamples = %d \n", 
       buffer_def.istart, buffer_def.istride, buffer_def.nsamples);
     */
    rc = ioctl(fd, ACQ32_IOREAD_LOCALBUF, &buffer_def);
    /*
       printf("back from ioctl *num_coeffs=%d\n", *num_coeffs);
     */
    if (rc < 0) {
      printf("got back %d from ioctl\n", rc);
      perror("call to ioctl");
    }
    buffer_def.buffer += buffer_def.nsamples;
    buffer_def.istart += buffer_def.nsamples * buffer_def.istride;
  }
  if (*num_coeffs > 1) {
    for (idx = 0; idx < (*end - *start + 1) / *inc; idx++) {
      sam = 0.;
      /*      printf("."); */
      for (cidx = 0; cidx < *num_coeffs; cidx++) {
	int iidx = MIN(MAX(idx * *inc + cidx - *num_coeffs / 2, 0), *end - 1);
	sam += region[iidx] * coeffs[cidx];
      }
      buffer[idx] = (int)sam;
    }
    /*  printf("\n"); */
  } else {
    memcpy(buffer, region, samples * sizeof(short));
  }
  /*
     printf("about to umap...\n");
   */
  rc = munmap((void *)region, length);
  if (rc != 0) {
    printf("error unmapping %d\n", length);
    perror("munmap");
  }
  close(fd);
  return (*end - *start + 1) / *inc;
}

EXPORT int Convolve(short *outbuf, short *inbuf, int inbuf_len, float *coeffs, int num_coeffs, int inc)
{
  int i, j, idx;
  int sample = 0;
  float total;
  for (i = inc / 2; i < inbuf_len; i += inc) {
    total = 0;
    for (j = 0; j < num_coeffs; j++) {
      idx = MAX(0, i + (j - num_coeffs / 2));
      total += (inbuf[idx] * coeffs[j]);
    }
    outbuf[sample++] = total;
  }
  return sample;
}
