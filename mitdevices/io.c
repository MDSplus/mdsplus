#include <stdio.h>
FILE *FOPEN(const char *fname, const char *mode)
{
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
