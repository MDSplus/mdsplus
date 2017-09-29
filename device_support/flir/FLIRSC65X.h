#include <PvDevice.h>
#include <PvDeviceGEV.h>   //new 9mar2016 for SDK4
#include <PvStream.h>
#include <PvStreamGEV.h>   //new 9mar2016 for SDK4

enum FPS_ENUM     { fps_200, fps_100, fps_50, fps_25, fps_12, fps_6, fps_3 };
enum IRFMT_ENUM   { radiometric, linear10mK, linear100mK };
enum EXPMODE_ENUM { internal_mode, external_mode };


#ifdef __cplusplus 
extern "C" 
{ 
#endif 

extern int flirRadiometricConv(void *frame, int width, int height, void *metaData);

// Wrapper for Python that must see the cpp class as standard C functions

int flirOpen(const char *cameraName, int *camHandle);
int flirClose(int camHandle);
int checkLastOp(int camHandle);
int printAllParameters(int camHandle);
int setExposureMode(int camHandle, EXPMODE_ENUM exposureMode);

int setFrameRate(int camHandle, FPS_ENUM frameRate, int *frameToSkip);
int setFrameRateNew(int camHandle, double frameRate);

int setIrFormat(int camHandle, IRFMT_ENUM irFormat);
int getReadoutArea(int camHandle, int *x, int *y, int *width, int *height);
int setReadoutArea(int camHandle, int x, int y, int width, int height);
int setObjectParameters(int camHandle, double reflectedTemperature, double atmosphericTemperature, 
											double objectDistance, double objectEmissivity, 
											double relativeHumidity, double extOpticsTemperature, 
											double extOpticsTransmission, double estimatedTransmission);
int setMeasurementRange(int camHandle, int measRange);
int getFocusAbsPosition(int camHandle, int *focusPos);
int setFocusAbsPosition(int camHandle, int focusPos);   
int setAcquisitionMode(int camHandle, int storeEnabled, int acqSkipFrameNumber );

int executeAutoFocus(int camHandle);
int setCalibMode(int camHandle, int calMode);
int executeAutoCalib(int camHandle);
int startAcquisition(int camHandle, int *width, int *height, int *payloadSize);
int stopAcquisition(int camHandle);
int getFrame(int camHandle, int *status, void *frame, void *metaData);
int frameConv(int camHandle, unsigned short *frame, int width, int height);
int startFramesAcquisition(int camHandle);
int stopFramesAcquisition(int camHandle);


int setStreamingMode(int camHandle, IRFMT_ENUM irFormat, int streamingEnabled, bool autoAdjustLimit, const char *streamingServer, int streamingPort, int lowLim, int highLim, int adjRoiX, int adjRoiY, int adjRoiW, int adjRoiH, const char *deviceName);

int setTriggerMode(int camHandle, int triggerMode, double burstDuration, int numTrigger );
int softwareTrigger(int camHandle);
int setTreeInfo( int camHandle,  void *treePtr, int framesNid, int timebaseNid, int framesMetadNid, int frame0TimeNid);

void  getLastError(int camHandle, char *msg);

#ifdef __cplusplus 
} 
#endif 



class FLIR_SC65X
{
	private:
		PvDevice *lDevice;		//camera handle
		PvStream *lStream;		//stream handle
		PvBuffer *lBuffers;		//buffer handle
		PvResult lResult;		//result of the latest operation
		PvString ipAddress;		//camera ip address

		int	 x;
		int 	 y;
		int	 width;
		int 	 height;
		int      pixelFormat;           //all pixelFormat supported are in camstreamutils.h
		double	 frameRate;

		int 	 storeEnabled;
		int 	 triggerMode;
                int      startStoreTrg;
		int      autoCalibration;
		int      irFrameFormat;

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
		int      adjRoiX;
		int      adjRoiY;
		int      adjRoiW;
		int      adjRoiH;
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

		//debug
		uint64_t currTime, lastTime, currIdx, lastIdx,  triggered;
		int64_t tickFreq;


	public:
		//camera
                FLIR_SC65X(const char *ipAddress);
                FLIR_SC65X(); 			//new 23 July 2013 for test purposes
                ~FLIR_SC65X();

		//info
                int checkLastOp();
                int printAllParameters();

		//settings
                int setExposureMode(EXPMODE_ENUM exposureMode);
                int setFrameRate(double frameRate);
                int setFrameRate(FPS_ENUM fps, int *frameToSkip);
                int setIrFormat(IRFMT_ENUM irFormat);
                int getReadoutArea(int *x, int *y, int *width, int *height);
                int setReadoutArea(int x, int y, int width, int height);
                int setObjectParameters(double reflectedTemperature, double atmosphericTemperature, 
											double objectDistance, double objectEmissivity, 
											double relativeHumidity, double extOpticsTemperature, 
											double extOpticsTransmission, double estimatedTransmission);	
                int setMeasurementRange(int measRange);	  
                int getFocusAbsPosition(int *focusPos);
                int setFocusAbsPosition(int focusPos);                
                int setCalibMode(int calibMode);

		int setAcquisitionMode( int storeEnabled, int acqSkipFrameNumber );
		int setStreamingMode( IRFMT_ENUM irFormat, int streamingEnabled, bool autoAdjustLimit, const char *streamingServer, int streamingPort, unsigned int lowLim, unsigned int highLim, int adjRoiX, int adjRoiY, int adjRoiW, int adjRoiH,  const char *deviceName);

		int setTriggerMode( int triggerMode, double burstDuration, int numTrigger );
		int setTreeInfo( void *treePtr, int frameNid, int timebaseNid, int framesMetadNid, int frame0TimeNid);


                int executeAutoFocus();
                int executeAutoCalib();

		void getLastError(char *msg);
		void printLastError(const char *format, const char *msg);
			
		//acquisition
                int startAcquisition(int *width, int *height, int *payloadSize);	
                int stopAcquisition();
                int softwareTrigger();
                int getFrame(int *status, void *frame, void *metaData);
                int frameConv(unsigned short *frame, int width, int height);
				int startFramesAcquisition();
				int stopFramesAcquisition();


	protected:

};
