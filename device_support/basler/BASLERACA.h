// Include files to use the PYLON API.
#include <pylon/PylonIncludes.h>
#include <pylon/gige/PylonGigEIncludes.h>
#include <GenApi/IEnumerationT.h>

// Settings for using Basler GigE cameras.
#include <pylon/gige/BaslerGigEInstantCamera.h>
typedef Pylon::CBaslerGigEInstantCamera Camera_t;
typedef Pylon::CBaslerGigEImageEventHandler ImageEventHandler_t; // Or use Camera_t::ImageEventHandler_t
typedef Pylon::CBaslerGigEGrabResultPtr GrabResultPtr_t; // Or use Camera_t::GrabResultPtr_t
using namespace Basler_GigECameraParams;

using namespace Pylon;   // Namespace for using pylon objects.
using namespace GenApi;  // Namespace for using GenApi objects.

enum FPS_ENUM     { fps_200, fps_100, fps_50, fps_25, fps_12, fps_6, fps_3 };
enum IRFMT_ENUM   { radiometric, linear10mK, linear100mK };
enum EXPMODE_ENUM { internal_mode, external_mode };


#ifdef __cplusplus 
extern "C" 
{ 
#endif 

// Wrapper for Python that must see the cpp class as standard C functions

int baslerOpen(const char *cameraName, int *camHandle);
int baslerClose(int camHandle);
/*
int checkLastOp(int camHandle);
int printAllParameters(int camHandle);
*/
int setExposureAuto(int camHandle, char *exposureAuto);
int setExposure(int camHandle, double exposure);
int setGainAuto(int camHandle, char *gainAuto);
int setGammaEnable(int camHandle, char *gammaEnable);
int setGain(int camHandle, int gain);
int setFrameRate(int camHandle, double frameRate);
/*
int setFrameRateNew(int camHandle, double frameRate);
int setFrameRate(int camHandle, FPS_ENUM frameRate, int *frameToSkip);

int setIrFormat(int camHandle, IRFMT_ENUM irFormat);
*/
//int getReadoutArea(int camHandle, int *x, int *y, int *width, int *height);
int setReadoutArea(int camHandle, int x, int y, int width, int height);
/*
int setObjectParameters(int camHandle, double reflectedTemperature, double atmosphericTemperature, 
											double objectDistance, double objectEmissivity, 
											double relativeHumidity, double extOpticsTemperature, 
											double extOpticsTransmission, double estimatedTransmission);
*/
int setPixelFormat(int camHandle, char *pixelFormat);
/*
int getFocusAbsPosition(int camHandle, int *focusPos);
int setFocusAbsPosition(int camHandle, int focusPos);
*/   
int setAcquisitionMode(int camHandle, int storeEnabled, int acqSkipFrameNumber );
/*
int executeAutoFocus(int camHandle);
int setCalibMode(int camHandle, int calMode);
int executeAutoCalib(int camHandle);
*/
int startAcquisition(int camHandle, int *width, int *height, int *payloadSize);
int stopAcquisition(int camHandle);
int getFrame(int camHandle, int *status, void *frame, void *metaData);

int startFramesAcquisition(int camHandle);
int stopFramesAcquisition(int camHandle);


int setStreamingMode(int camHandle, int streamingEnabled, bool autoAdjustLimit, const char *streamingServer, int streamingPort, int lowLim, int highLim, const char *deviceName);

int setTriggerMode(int camHandle, int triggerMode, double burstDuration, int numTrigger );
int softwareTrigger(int camHandle);
int setTreeInfo( int camHandle,  void *treePtr, int framesNid, int timebaseNid, int framesMetadNid, int frame0TimeNid);

void  getLastError(int camHandle, char *msg);

#ifdef __cplusplus 
} 
#endif 


class BASLER_ACA
{
	private:
                IPylonDevice *pDevice;		//device handle
               // CInstantCamera *pCamera;        //camera handle
                Camera_t *pCamera;
		const char *ipAddress;
/*
		PvDevice *lDevice;		//camera handle
		PvStream *lStream;		//stream handle
		PvBuffer *lBuffers;		//buffer handle
		PvResult lResult;		//result of the latest operation
		PvString ipAddress;		//camera ip address
*/
		int	 x;
		int 	 y;
		int	 width;
		int 	 height;
//		int      irFrameFormat;
		int      pixelFormat;           //all pixelFormat supported are in camstreamutils.h
                int      Bpp;			//Bytes per pixel
		double	 frameRate;

		int 	 storeEnabled;
		int 	 triggerMode;
                int      startStoreTrg;
		int      autoCalibration;

		int	 streamingEnabled;
		int 	 streamingSkipFrameNumber;
		char     streamingServer[512];
		int 	 streamingPort;
		int      autoScale;
		unsigned int lowLim; 
		unsigned int highLim;
		unsigned int minLim; 
		unsigned int maxLim;
		bool     autoAdjustLimit;
		char     deviceName[64];

		int	 imageMode; 	
		int      acqSkipFrameNumber;
		double   burstDuration;
		int      numTrigger;

		void*    treePtr;
		int      framesNid; 
		int      timebaseNid; 
		int      framesMetadNid;
                int      frame0TimeNid;

		int	     acqFlag;
		int      acqStopped;
		char     error[512];
		int  	 incompleteFrame;
         
                int      lastOpRes;


		//debug
		uint64_t currTime, lastTime, currIdx, lastIdx,  triggered;
		int64_t tickFreq;


	public:
		//camera
                BASLER_ACA(const char *ipAddress);
                BASLER_ACA(); 			//new 23 July 2013 for test purposes
                ~BASLER_ACA();

		//info
                int baslerIsConnected();
                int checkLastOp();
 /*               int printAllParameters();
*/
		//settings
                int setExposure(double exposure);
                int setExposureAuto(char *exposureAuto);
                int setGainAuto(char *gainAuto);
                int setGain(int gain);
                int setGammaEnable(char *gammaEnable);
                int setFrameRate(double frameRate);
/*
                int setFrameRate(FPS_ENUM fps, int *frameToSkip);
                int setIrFormat(IRFMT_ENUM irFormat);
                int getReadoutArea(int *x, int *y, int *width, int *height);
*/
                int setReadoutArea(int x, int y, int width, int height);
/*  
              int setObjectParameters(double reflectedTemperature, double atmosphericTemperature, 
											double objectDistance, double objectEmissivity, 
											double relativeHumidity, double extOpticsTemperature, 
											double extOpticsTransmission, double estimatedTransmission);	

                int getFocusAbsPosition(int *focusPos);
                int setFocusAbsPosition(int focusPos);                
                int setCalibMode(int calibMode);
*/
                int setPixelFormat(char *pixelFormat);
		int setAcquisitionMode( int storeEnabled, int acqSkipFrameNumber );
		int setStreamingMode( int streamingEnabled, bool autoAdjustLimit, const char *streamingServer, int streamingPort, unsigned int lowLim, unsigned int highLim, const char *deviceName);
		int setTriggerMode( int triggerMode, double burstDuration, int numTrigger );
		int setTreeInfo( void *treePtr, int frameNid, int timebaseNid, int framesMetadNid, int frame0TimeNid);

/*
                int executeAutoFocus();
                int executeAutoCalib();
*/
		void getLastError(char *msg);
/*		void printLastError(const char *format, const char *msg);
			
		//acquisition
*/
                int startAcquisition(int *width, int *height, int *payloadSize);	
                int stopAcquisition();
                int softwareTrigger();

                int getFrame(int *status, void *frame, void *metaData);

		int startFramesAcquisition();
		int stopFramesAcquisition();


	protected:

};
