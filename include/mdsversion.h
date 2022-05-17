#include <mdsdescrip.h>

#pragma pack(push)
typedef struct
{
  const int MAJOR;
  const int MINOR;
  const int MICRO;
  const char BRANCH[12];
} mds_version_t;
#pragma pack(pop)
