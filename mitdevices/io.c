#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef MAX
#define MAX(a,b) ((a) > (b)) ? (a) : (b)
#endif

FILE *FOPEN(const char *fname, const char *mode)
{
  printf("here I am in open\n");
  return fopen(fname, mode);
}
int FCLOSE(FILE *fd)
{
  return fclose(fd);
}
size_t FREAD( void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  return fread(ptr, size, nmemb, stream);
}

size_t FWRITE( void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  return fwrite(ptr, size, nmemb, stream);
}

int FSEEK(FILE *stream, long offset, int whence)
{
  return fseek(stream, offset, whence);
}

int DMARead(short *buffer, const char *fname, int *chan, int *samples, int *active_chans)
{  
  int lstart = 0;
  int lend = *samples;
  int linc = *active_chans;
  unsigned length = lstart+lend*linc*sizeof(short);
  int in, out;
  int fd;
  short *region;
    
  /*  printf("Starting DMARead lstart = %d lend = %d linc = %d, length = %d\n", lstart, lend, linc, length); */
  if ( (fd = open( fname, O_RDWR )) < 0 ) {
        fprintf( stderr, "DMARead: failed to open device \"%s\" - ", fname );
        perror( "" );
        return -1;
  }
  /* printf("about to mmap (null, %d, PROT_READ|PROT_WRITE, MAP_SHARED, %d, 0)\n", length, fd); */
  region = (short *)mmap( NULL, length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0 );

  if ( region == (short *)-1 ){
        perror( "DMARead mmap" );
        return 1;
  }
  for(in=*chan, out=0; out < lend; in+=linc, out++) {
       buffer[out]=region[in];
  }
  /* printf("%d\n", out); */
  munmap((void *)region, length);
  close(fd);
    
  return out;
}
int DMARead2(short *buffer, const char *fname, int *chan, int *samples, int *active_chans, int *start, int *end, int *inc, float *coeffs, int *num_coeffs)
{
  int sample;
  int lstart = 0;
  int lend = *samples;
  int linc = *active_chans;
  unsigned length = lstart+lend*linc*sizeof(short);
  int in, out;
  int fd;
  short *region;
  int i;
    
  /* printf("Starting DMARead lstart = %d lend = %d linc = %d, length = %d\n", lstart, lend, linc, length); */
  if ( (fd = open( fname, O_RDWR )) < 0 ) {
        fprintf( stderr, "DMARead: failed to open device \"%s\" - ", fname );
        perror( "" );
        return -1;
  }
  /* printf("about to mmap (null, %d, PROT_READ|PROT_WRITE, MAP_SHARED, %d, 0)\n", length, fd); */
  region = (short *)mmap( NULL, length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0 );

  if ( region == (short *)-1 ){
        perror( "DMARead mmap" );
        return 1;
  }
  /*
  printf("about to resample start=%d, end=%d, inc=%d, num_coeffs=%d\n", *start, *end, *inc, *num_coeffs);



  printf("the coefs are: ");
  for (i = 0; i< *num_coeffs; i++)
    printf("%f ", coeffs[i]);
  printf("\n");
  */
 
  out=0;
  for(sample = *start; sample < *end; sample += *inc) {
    int i;
    float sam = 0;
    for (i=0; i<*num_coeffs; i++) {
      int idx = (sample +(i-*num_coeffs/2))*linc + *chan;
      if (idx < 0) {
	/*
        printf("front sample = %d i = %d idx = %d ", sample, i, idx);
	*/
	idx = *chan;
	/*
        printf("%d\n", idx);
	*/
      }
      if (idx >= length/2) {
	/*
        printf("back sample = %d i = %d idx = %d ", sample, i, idx);
	*/
	idx = (*samples -1)*linc +*chan;
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
int Convolve(short *outbuf, short *inbuf, int inbuf_len, float *coeffs, int num_coeffs, int inc)
{
  int i,j,idx;
  int sample = 0;
  float total;
  for (i=inc/2; i<inbuf_len; i+=inc) {
    total = 0;
    for (j=0; j<num_coeffs; j++) {
      idx = MAX(0, i+(j-num_coeffs/2));
      total += (inbuf[idx]*coeffs[j]) ;
    }
    outbuf[sample++] = total;
  }
  return sample;
}





