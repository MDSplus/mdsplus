#include <PvDevice.h>
#include <PvStream.h>

enum FPS_ENUM     { fps_200, fps_100, fps_50, fps_25, fps_12, fps_6, fps_3 };
enum IRFMT_ENUM   { radiometric, linear10mK, linear100mK };
enum EXPMODE_ENUM { internal_mode, external_mode };


#ifdef __cplusplus 
extern "C" 
{ 
#endif 

// Wrapper for Python that must see the cpp class as standard C functions

int flirOpen(const char *cameraName, int *camHandle);
int flirClose(int camHandle);
int checkLastOp(int camHandle);
int printAllParameters(int camHandle);
int setExposureMode(int camHandle, EXPMODE_ENUM exposureMode);
int setFrameRate(int camHandle, FPS_ENUM fps, int *frameToSkip);
int setIrFormat(int camHandle, IRFMT_ENUM irFormat);
int getReadoutArea(int camHandle, int *x, int *y, int *width, int *height);
int setReadoutArea(int camHandle, int x, int y, int width, int height);
int setObjectParameters(int camHandle, double reflectedTemperature, double atmosphericTemperature, 
											double objectDistance, double objectEmissivity, 
											double relativeHumidity, double extOpticsTemperature, 
											double extOpticsTransmission, double estimatedTransmission);
int setMeasurementRange(int camHandle, int measRange);
int executeAutoFocus(int camHandle);
int setCalibMode(int camHandle, int calMode);
int executeAutoCalib(int camHandle);
int startAcquisition(int camHandle, int *width, int *height, unsigned int *payloadSize);
int stopAcquisition(int camHandle);
int getFrame(int camHandle, int *status, void *frame, void *metaData);
int frameConv(int camHandle, unsigned short *frame, int width, int height, int milliKelvinScale);

#ifdef __cplusplus 
} 
#endif 



class FLIR_SC65X
{
	private:
		PvDevice *lDevice;		//camera handle
		PvStream lStream;		//stream handle
		PvBuffer *lBuffers;		//buffer handle
		PvResult lResult;		//result of the latest operation
		PvString ipAddress;		//camera ip address

	public:
		//camera
		FLIR_SC65X(const char *ipAddress);
		~FLIR_SC65X();

		//info
        int checkLastOp();
		int printAllParameters();

		//settings
		int setExposureMode(EXPMODE_ENUM exposureMode);
		int setFrameRate(FPS_ENUM fps, int *frameToSkip);
		int setIrFormat(IRFMT_ENUM irFormat);
		int getReadoutArea(int *x, int *y, int *width, int *height);
		int setReadoutArea(int x, int y, int width, int height);
		int setObjectParameters(double reflectedTemperature, double atmosphericTemperature, 
											double objectDistance, double objectEmissivity, 
											double relativeHumidity, double extOpticsTemperature, 
											double extOpticsTransmission, double estimatedTransmission);	
		int setMeasurementRange(int measRange);	  
    	int executeAutoFocus();
        int setCalibMode(int calMode);
		int executeAutoCalib();
    	//int getFocusAbsPos();
			
		//acquisition
		int startAcquisition(int *width, int *height, unsigned int *payloadSize);
		int stopAcquisition();
		int getFrame(int *status, void *frame, void *metaData);
        int frameConv(unsigned short *frame, int width, int height, int milliKelvinScale);

	protected:

};
