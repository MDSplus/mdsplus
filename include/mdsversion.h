#include <mdsdescrip.h>

#pragma pack(push)
typedef struct {
  const int  MAJOR;
  const int  MINOR;
  const int  MICRO;
  const char BRANCH[sizeof("@RELEASE_BRANCH@")];
} mds_version_t;
#pragma pack(pop)

extern const mds_version_t MDSVERSION;
extern const char *MdsRelease();
extern const mdsdsc_t *MdsReleaseDsc();
