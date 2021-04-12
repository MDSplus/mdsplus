#include <mdsobjects.h>
#include <string.h>
//#include <PvTypes.h>       //new 2019-09-19 for point grey streaming connection
//#include <PvSystem.h>
#include <PvBuffer.h>

#include <PvDevice.h>
#include <PvDeviceGEV.h>   //new 9mar2016 for SDK4
#include <PvStream.h>
#include <PvStreamGEV.h>   //new 9mar2016 for SDK4

using namespace MDSplus;

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */


#ifdef __cplusplus 
extern "C" 
{ 
#endif 

//extern int flirRadiometricConv(void *frame, int width, int height, void *metaData);

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


//int setFrameRate(int camHandle, FPS_ENUM frameRate, int *frameToSkip);
//int setFrameRateNew(int camHandle, double frameRate);
int setFrameRate(int camHandle, double *frameRate);

//int getReadoutArea(int camHandle, int *x, int *y, int *width, int *height);
int setReadoutArea(int camHandle, int x, int y, int width, int height);
  
int setAcquisitionMode(int camHandle, int storeEnabled, int acqSkipFrameNumber );

int setPixelFormat(int camHandle, char *pixelFormat);

int startAcquisition(int camHandle, int *width, int *height, int *payloadSize);
int stopAcquisition(int camHandle);
int getFrame(int camHandle, int *status, void *frame, void *metaData);
//int frameConv(int camHandle, unsigned short *frame, int width, int height);
int startFramesAcquisition(int camHandle);
int stopFramesAcquisition(int camHandle);


int setStreamingMode(int camHandle, int streamingEnabled, bool autoAdjustLimit, const char *streamingServer, int streamingPort, int lowLim, int highLim, int adjRoiX, int adjRoiY, int adjRoiW, int adjRoiH, const char *deviceName);

int setTriggerMode(int camHandle, int triggerMode, double burstDuration, int numTrigger );
int softwareTrigger(int camHandle);
int setTreeInfo( int camHandle,  void *treePtr, int framesNid, int timebaseNid, int framesMetadNid, int frame0TimeNid);

void  getLastError(int camHandle, char *msg);

#ifdef __cplusplus 
} 
#endif 


typedef struct _ptgreyMetaData 
{
    float gain;
    float exposure;
    float internalTemperature;
    int64_t timestamp;
}PTGREYMETADATA;


class PTGREY
{

	class TriggerMdsEvent:public Event
	{
            private:
                PTGREY* ptGray;
	    public:
		TriggerMdsEvent(const char *name, PTGREY* ptGr):Event(name)
                {
                   ptGray = ptGr;
                }

	    void run()
	    {
		size_t bufSize;
		const char *name = getName(); //Get the name of the event
		char *date = getTime()->getDate(); //Get the event reception date in string format
		const char *buf =  getRaw(&bufSize); //Get raw data
		char *str = new char[bufSize+1]; //Make it a string
		memcpy(str, buf, bufSize);
		str[bufSize] = 0;
                //MDSevent trigger is set if camera is in acquisition, Frame store is enabled and camera is not saving frame.
                //An event trigger received during acquisition can reset the trigger count to extend the acquisition
                if(ptGray->acqFlag && ptGray->storeEnabled && ptGray->startStoreTrg == 0)
                {   
                    printf("%s EVENT Trigger Start!!!!\n", (ptGray->ipAddress).GetAscii());
                    ptGray->eventTrigger = 1;
                } else {
                    printf("%s EVENT Trigger Reset!!!!\n", (ptGray->ipAddress).GetAscii());
                    ptGray->eventTrigger = 0;
                }
		printf("%s RECEIVED EVENT %s AT %s WITH DATA %s Event Trig %d (%d,%d,%d)\n",(ptGray->ipAddress).GetAscii() ,name, date, str, ptGray->eventTrigger);
                delete str;
	    }
	};

	private:
		PvDevice *lDevice;		//camera handle
		PvStream *lStream;		//stream handle
		PvBuffer *lBuffers;		//buffer handle
		PvResult lResult;		//result of the latest operation
		PvString ipAddress;		//camera ip address
                TriggerMdsEvent *trigEvent;

		int	 x;
		int 	 y;
		int	 width;
		int 	 height;
		int      pixelFormat;           //all pixelFormat supported are in camstreamutils.h
                int      Bpp;			//Bytes per pixel
		double	 frameRate;
                float    gain;
                float    exposure;
                float    internalTemperature;

		int 	 storeEnabled;
		int 	 triggerMode;
                int      startStoreTrg;
		int      autoCalibration;
		int      irFrameFormat;
                int      eventTrigger; //CT on MDSplus event trigger flag


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

		int	 acqFlag;
		int      acqStopped;
		char     error[512];
		int  	 incompleteFrame;

		//debug
		uint64_t currTime, lastTime, currIdx, lastIdx,  triggered;
		int64_t tickFreq;


	public:
		//camera
                PTGREY(const char *ipAddress);
                PTGREY(); 			//new 23 July 2013 for test purposes
                ~PTGREY();

		//info
                int checkLastOp();
                int printAllParameters();

		//settings
                int readInternalTemperature();
                int setExposure(float exposure);
                int setExposureAuto(char *exposureAuto);
                int setGain(float gain);
                int setGainAuto(char *gainAuto);
                int setGammaEnable(char *gammaEnable);
                int setFrameRate(double &frameRate);
//                int setFrameRate(FPS_ENUM fps, int *frameToSkip);
//                int getReadoutArea(int *x, int *y, int *width, int *height);
                int setReadoutArea(int x, int y, int width, int height);
                int setPixelFormat(char *pixelFormat);

		int setAcquisitionMode(int storeEnabled, int acqSkipFrameNumber);
		int setStreamingMode( int streamingEnabled, bool autoAdjustLimit, const char *streamingServer, int streamingPort, unsigned int lowLim, unsigned int highLim, int adjRoiX, int adjRoiY, int adjRoiW, int adjRoiH,  const char *deviceName);

		int setTriggerMode( int triggerMode, double burstDuration, int numTrigger );
		int setTreeInfo( void *treePtr, int frameNid, int timebaseNid, int framesMetadNid, int frame0TimeNid);

		void getLastError(char *msg);
		void printLastError(const char *format, const char *msg);
			
		//acquisition
                int startAcquisition(int *width, int *height, int *payloadSize);	
                int stopAcquisition();
                int softwareTrigger();
                int getFrame(int *status, void *frame, void *metaData);
  //              int frameConv(unsigned short *frame, int width, int height);
		int startFramesAcquisition();
		int stopFramesAcquisition();


	protected:

};
