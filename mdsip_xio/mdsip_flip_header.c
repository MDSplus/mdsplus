#include "mdsip.h"

#define FlipBytes(num,ptr) \
{\
  int __i;\
  int __n = num;\
  char *__p = ptr;\
  for (__i=0;__i<__n/2;__i++)\
  {\
    char __tmp = __p[__i];\
    __p[__i] = __p[__n - __i -1];\
    __p[__n - __i - 1] = __tmp;\
  }\
}

void mdsip_flip_header(mdsip_message_header_t *header)
{
  int i;
#ifdef __CRAY
#define Flip32(n) n = ((n >> 24) & 0xff) | ((n >> 8) & 0xff00) | ((n << 8) & 0xff0000) | ((n << 24) & 0xff000000)
#define Flip16(n) n = ((n >> 8) & 0xff) | ((n << 8) & 0xff00)
  Flip32(header->msglen);
  Flip32(header->status);
  Flip16(header->length);
  for (i=0;i<MAX_DIMS;i++) FlipBytes(4,((char *)header->dims)+4*i);
#else
  FlipBytes(4,(char *)&header->msglen);
  FlipBytes(4,(char *)&header->status);
  FlipBytes(2,(char *)&header->length);
  for (i=0;i<MAX_DIMS;i++) FlipBytes(sizeof(header->dims[i]),(char *)&header->dims[i]);
#endif
}
