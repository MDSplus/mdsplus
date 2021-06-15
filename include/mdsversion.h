#include <mdsdescrip.h>

#pragma pack(push)
typedef struct
{
  const int MAJOR;
  const int MINOR;
  const int MICRO;
  const char BRANCH[sizeof("@RELEASE_BRANCH@")];
} mds_version_t;
#pragma pack(pop)

#define CONCAT2(a, b) a##b
#define CONCAT(a, b) CONCAT2(a, b)
#define VERSIONCONST CONCAT(LIBPREFIX, Version)
#define GETRELEASE CONCAT(LIBPREFIX, Release)
#define GETRELEASEDSC CONCAT(LIBPREFIX, ReleaseDsc)
