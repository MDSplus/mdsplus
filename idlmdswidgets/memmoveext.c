#include <string.h>
void *memmoveext(void *dest, const void *src, size_t n)
{
  return memmove(dest, src, n);
}
