//#include <PvTypes.h>       //new 2019-09-19 for point grey streaming
// connection #include <PvSystem.h>
#include <PvBuffer.h>

#include <PvDevice.h>
#include <PvDeviceGEV.h> //new 9mar2016 for SDK4
#include <PvStream.h>
#include <PvStreamGEV.h> //new 9mar2016 for SDK4

#ifdef __cplusplus
extern "C" {
#endif

// extern int flirRadiometricConv(void *frame, int width, int height, void
// *metaData);

// Wrapper for Python that must see the cpp class as standard C functions

int PGopen(const char *cameraName, int *camHandle);
int PGclose(int camHandle);
int checkLastOp(int camHandle);
int printAllParameters(int camHandle);
int readInternalTemperature(int camHandle);
int setExposure(int camHandle, float exposure);
int setExposureAuto(int camHandle, char *exposureAuto);
int setGammaEnable(int camHandle, char *gammaEnable);
int setGain(int camHandle, float gain);
int setGainAuto(int camHandle, char *gainAuto);

// int setFrameRate(int camHandle, FPS_ENUM frameRate, int *frameToSkip);
// int setFrameRateNew(int camHandle, double frameRate);
int setFrameRate(int camHandle, double frameRate);

// int getReadoutArea(int camHandle, int *x, int *y, int *width, int *height);
int setReadoutArea(int camHandle, int x, int y, int width, int height);

int setAcquisitionMode(int camHandle, int storeEnabled, int acqSkipFrameNumber);

int setPixelFormat(int camHandle, char *pixelFormat);

int startAcquisition(int camHandle, int *width, int *height, int *payloadSize);
int stopAcquisition(int camHandle);
int getFrame(int camHandle, int *status, void *frame, void *metaData);
// int frameConv(int camHandle, unsigned short *frame, int width, int height);
int startFramesAcquisition(int camHandle);
int stopFramesAcquisition(int camHandle);

int setStreamingMode(int camHandle, int streamingEnabled, bool autoAdjustLimit,
                     const char *streamingServer, int streamingPort, int lowLim,
                     int highLim, int adjRoiX, int adjRoiY, int adjRoiW,
                     int adjRoiH, const char *deviceName);

int setTriggerMode(int camHandle, int triggerMode, double burstDuration,
                   int numTrigger);
int softwareTrigger(int camHandle);
int setTreeInfo(int camHandle, void *treePtr, int framesNid, int timebaseNid,
                int framesMetadNid, int frame0TimeNid);

void getLastError(int camHandle, char *msg);

#ifdef __cplusplus
}
#endif

typedef struct _ptgreyMetaData {
  float gain;
  float exposure;
  float internalTemperature;
  int64_t timestamp;
} PTGREYMETADATA;

class PTGREY {
private:
  PvDevice *lDevice;  // camera handle
  PvStream *lStream;  // stream handle
  PvBuffer *lBuffers; // buffer handle
  PvResult lResult;   // result of the latest operation
  PvString ipAddress; // camera ip address

  int x;
  int y;
  int width;
  int height;
  int pixelFormat; // all pixelFormat supported are in camstreamutils.h
  int Bpp;         // Bytes per pixel
  double frameRate;
  float gain;
  float exposure;
  float internalTemperature;

  int storeEnabled;
  int triggerMode;
  int startStoreTrg;
  int autoCalibration;
  int irFrameFormat;

  int streamingEnabled;
  int streamingSkipFrameNumber;
  char streamingServer[512];
  int streamingPort;
  int autoScale;
  unsigned int lowLim;
  unsigned int highLim;
  unsigned int minLim;
  unsigned int maxLim;
  bool autoAdjustLimit;
  int adjRoiX;
  int adjRoiY;
  int adjRoiW;
  int adjRoiH;
  char deviceName[64];

  int imageMode;
  int acqSkipFrameNumber;
  double burstDuration;
  int numTrigger;

  void *treePtr;
  int framesNid;
  int timebaseNid;
  int framesMetadNid;
  int frame0TimeNid;

  int acqFlag;
  int acqStopped;
  char error[512];
  int incompleteFrame;

  // debug
  uint64_t currTime, lastTime, currIdx, lastIdx, triggered;
  int64_t tickFreq;

public:
  // camera
  PTGREY(const char *ipAddress);
  PTGREY(); // new 23 July 2013 for test purposes
  ~PTGREY();

  // info
  int checkLastOp();
  int printAllParameters();

  // settings
  int readInternalTemperature();
  int setExposure(float exposure);
  int setExposureAuto(char *exposureAuto);
  int setGain(float gain);
  int setGainAuto(char *gainAuto);
  int setGammaEnable(char *gammaEnable);
  int setFrameRate(double frameRate);
  //                int setFrameRate(FPS_ENUM fps, int *frameToSkip);
  //                int getReadoutArea(int *x, int *y, int *width, int *height);
  int setReadoutArea(int x, int y, int width, int height);
  int setPixelFormat(char *pixelFormat);

  int setAcquisitionMode(int storeEnabled, int acqSkipFrameNumber);
  int setStreamingMode(int streamingEnabled, bool autoAdjustLimit,
                       const char *streamingServer, int streamingPort,
                       unsigned int lowLim, unsigned int highLim, int adjRoiX,
                       int adjRoiY, int adjRoiW, int adjRoiH,
                       const char *deviceName);

  int setTriggerMode(int triggerMode, double burstDuration, int numTrigger);
  int setTreeInfo(void *treePtr, int frameNid, int timebaseNid,
                  int framesMetadNid, int frame0TimeNid);

  void getLastError(char *msg);
  void printLastError(const char *format, const char *msg);

  // acquisition
  int startAcquisition(int *width, int *height, int *payloadSize);
  int stopAcquisition();
  int softwareTrigger();
  int getFrame(int *status, void *frame, void *metaData);
  //              int frameConv(unsigned short *frame, int width, int height);
  int startFramesAcquisition();
  int stopFramesAcquisition();

protected:
};
