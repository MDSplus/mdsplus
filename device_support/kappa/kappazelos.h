#include <SDK4/SDK4ControlTypes.h>
#include <SDK4/SDK4ZelosTypes.h>

#ifdef __cplusplus
extern "C"
{
#endif

  // camera
  int kappaOpen(const char *cameraName, void **cameraHandle);
  int kappaClose(void *cameraHandle);

  // settings
  int kappaSetColorCoding(void *cameraHandle, SDK4_ENUM_COLORCODING color_coding);
  int kappaSetExposureMode(void *cameraHandle, SDK4_ENUM_EXPOSUREMODE expmode);
  int kappaSetExposure(void *cameraHandle, float expTime);
  int kappaSetAET(void *cameraHandle, SDK4_ENUM_AET aet);
  int kappaSetAutoExposureLevel(void *cameraHandle, uint32_t lev);
  int kappaSetGain(void *cameraHandle, uint32_t gain);
  int kappaSetAGC(void *cameraHandle, SDK4_ENUM_AGC agc);
  int kappaSetSoftTrigger(void *cameraHandle, SDK4_ENUM_SWITCH uOnOff);
  int kappaSetTriggerTimer(void *cameraHandle, uint32_t time);
  int kappaSetSlowScan(void *cameraHandle, SDK4_ENUM_SWITCH uOnOff);
  int kappaGetReadoutAreaLimits(void *cameraHandle, SDK4_ENUM_ROADOMAIN select,
                                int *x, int *y, int *width, int *height);
  int kappaSetReadoutArea(void *cameraHandle, int x, int y, int width,
                          int height);
  int kappaSetBinning(void *cameraHandle, int binHor, int binVer);
  int kappaSetMeasureWindow(void *cameraHandle, int x, int y, int width,
                            int height);

  // info
  int kappaGetIp(void *cameraHandle, char *ip);
  int kappaPrintInfo(void *cameraHandle);

  // acquisition
  int kappaStartAcquisition(void *cameraHandle, void **buffer, int32_t *width,
                            int32_t *height, uint32_t *payloadSize);
  int kappaStopAcquisition(void *cameraHandle, void *buffer);
  int kappaGetFrame(void *cameraHandle, int32_t *status, void *frame);

#ifdef __cplusplus
}
#endif
