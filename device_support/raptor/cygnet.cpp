#define FORMATFILE "/home/mdsplus/cygnet.dat"
#define FORMAT  "default"      // NSTC S-Video on input 1
#define UNITS    1
#define UNITSMAP    ((1<<UNITS)-1)  // shorthand - bitmap of all units
#define DRIVERPARMS ""          // default

//#undef DEBUG


#include <math.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <stdarg.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <poll.h>
#include <time.h>
#include <sys/time.h>

#include <xcliball.h>        // driver function prototypes
#include <cammdsutils.h>    //Camera MDSplus support
#include "cygnet.h"

#ifdef DEBUG
static void printFrameInfo(void);
static void printImageInfo(void);
#endif

static bool isOpen = false;
static double sec_per_tick = 1E-3;

/*
 *  SUPPORT STUFF:
 *
 *  Catch CTRL+C and floating point exceptions so that
 *  once opened, the PIXCI(R) driver and frame grabber
 *  are always closed before exit.
 *  In most environments, this isn't critical; the operating system
 *  advises the PIXCI(R) driver that the program has terminated.
 *  In other environments, such as DOS, the operating system
 *  isn't as helpful and the driver & frame grabber would remain open.
 */
void sigintfunc(int sig)
{
    /*
     * Some languages/environments don't allow
     * use of printf from a signal catcher.
     * Printing the message isn't important anyhow.
     *
    if (sig == SIGINT)
    printf("Break\n");
    if (sig == SIGFPE)
    printf("Float\n");
    */

    pxd_PIXCIclose();
    exit(1);
}

/*
 * Video 'interrupt' callback function.
 */
static int fieldirqcount = 0;
static void videoirqfunc(int sig)
{
    fieldirqcount++;
}

/*
 * Open the XCLIB C Library for use.
 */
int epixOpen(char *confFile, int *iXPixels, int *iYPixels)
{
    int status;
    if(isOpen) return 0;
#ifdef DEBUG
    printf("Opening EPIX(R) PIXCI(R) Frame Grabber,\n");
    printf("using configuration parameters '%s',\n", DRIVERPARMS? DRIVERPARMS: "default");
#endif
    status = pxd_PIXCIopen(DRIVERPARMS, "", confFile); // confFile includes exposure time which seems to take precedence over later serial commands
    if (status >= 0)
    {
#ifdef DEBUG
        printf("Open OK\n");
#endif
        isOpen = true;
    }
    else
    {
        printf("Open Error %d\a\a\n", status);
        pxd_mesgFault(UNITSMAP);
    }
#ifdef DEBUG
    printFrameInfo();
    printImageInfo();
#endif
    *iXPixels = pxd_imageXdim();
    *iYPixels = pxd_imageYdim();
    uint32 ticku[2];
    if(pxd_infoSysTicksUnits(ticku) == 0)
    {
        sec_per_tick = (double)ticku[0] / (double)ticku[1] * 1E-6;
#ifdef DEBUG
        printf("Microseconds per tick: %f\n", sec_per_tick * 1E6);
#endif
    }
    return status;
}

void epixClose(void)
{
    pxd_PIXCIclose();
    isOpen = false;
}

#ifdef DEBUG
/*
 * Report image frame buffer memory size
 */
static void printFrameInfo(void)
{
    printf("Image frame buffer memory size: %.3f Kbytes\n", (double)pxd_infoMemsize(UNITSMAP)/1024);
    printf("Image frame buffers           : %d\n", pxd_imageZdim());
    printf("Number of boards              : %d\n", pxd_infoUnits());
}

/*
 * Report image resolution.
 */
static void printImageInfo(void)
{
    printf("Image resolution:\n");
    printf("xdim           = %d\n", pxd_imageXdim());
    printf("ydim           = %d\n", pxd_imageYdim());
    printf("colors         = %d\n", pxd_imageCdim());
    printf("bits per pixel = %d\n", pxd_imageCdim()*pxd_imageBdim());
}
#endif

/*
 * Capture
 */

void epixStartVideoCapture(int id)
{
    pxd_goLivePair(id, 1, 2); // should id be converted to a unitmap?
#ifdef DEBUG
    printf("Video capture started.\n");
#endif
}

void epixStopVideoCapture(int id)
{
    pxd_goUnLive(id); // should id be converted to a unitmap?
#ifdef DEBUG
    printf("Video capture stopped.\n");
#endif

}

//Capture either a single frame or timeouts.
//Returns 1 on capture and 0 on timeout
//Return the tick count of the last frame or the passed baseTicks in case of timeout

//id: device id (1)
//iXPixels, iYPixels: number of pixels
//dataNid: nid number of target data node
//timeNid: nid number of associated timestamp node. If -1, the time is put directly in the segmented data, else a reference to the timebase is put
//timeoutMs: timeout in milliseconds
//*treePtr: pointer to Tree object
//*listPtr: pointer to Internal List object for saving frames
//*bufIdx: index of the last captured frame grabber buffer        (initialize with -1 !!!)
//*frameIdx: frame index (from 0 onwards)
//*baseTicks: last 32 bits of system time when the first frame has been taken
//*currtime: returns time since reference t=0
int epixCaptureFrame(int iID, int iDataNid, double dTriggerTime, int iTimeOutMs, void *pTree, void *pList, int *piBufIdx, int *piFrameIdx, int *piBaseTicks, double *pdCurrTime)
{
    int iUnitMap = 1 << (iID - 1);
    int iLastBufIdx, iCurrTicks;
    int iPixelsRead, iPixelsToRead, iPixelsX, iPixelsY;
    struct timespec sWaitTime;
    pxbuffer_t  lastbuf = 0;
    iXPixels = pxd_imageXdim();
    iYPixels = pxd_imageYdim();
    iPixelsToRead = iXPixels * iYPixels;
    sWaitTime.tv_sec = 0;
    sWaitTime.tv_nsec = 5000000; //5ms delay
    if(*piBufIdx < 0)
    {
        iLastBufIdx = pxd_capturedBuffer(iUnitMap);  //The first time captureFrame is called
        *piFrameIdx = 0;
    }
    else
        iLastBufIdx = *piBufIdx;
    int iMaxCount = iTimeOutMs/5; //Maximum number of iterations before timing out
    for(int i = 0; i < iMaxCount; i++)
    {
        int iLastCaptured = pxd_capturedBuffer(iUnitMap);
        if(iLastCaptured != iLastBufIdx) //A new frame arrived
        {
            iCurrTicks = pxd_capturedSysTicks(iUnitMap);//internal clock
            if ( *piFrameIdx == 0) //first frame
                *piBaseTicks = iCurrTicks;
            *pdCurrTime = (iCurrTicks - (*piBaseTicks)) * sec_per_tick + dTriggerTime;
            unsigned short *piFrame = new unsigned short[iPixelsToRead];//allocate frame
            iPixelsRead = pxd_readushort(iUnitMap, iLastCaptured, 0, 0, iXPixels, iYPixels, piFrame, iPixelsToRead, (char *)"Grey");//get frame
#ifdef DEBUG
            printf("FRAME %d READ AT TIME %f\n", *piFrameIdx, *pdCurrTime);
#endif
            if(iPixelsRead != iPixelsToRead)
            {
                if (iPixelsRead < 0)
                    printf("pxd_readushort: %s\n", pxd_mesgErrorCode(iPixelsRead));
                else
                    printf("pxd_readushort error: %d != %d\n", iPixelsRead, iPixelsToRead);
                return 0;
            }
            camSaveFrameDirect(piFrame, iXPixels, iYPixels, *pdCurrTime, 12, pTree, iDataNid, -1, *piFrameIdx, pList);
            *piBufIdx = iLastCaptured;
            *piFrameIdx += 1;
            return 1;
        }
        else //No new frame
            nanosleep(&sWaitTime, NULL);
    }
//If code arrives here timeout occurred
    return 0;
}

int doTransaction(int id, char *outBufIn, int outBytes, char *readBuf, int readBytes)
{
    int r;
    int iUnitMap = 1 << (id-1);
    struct timespec waitTime;
    static int initialized = 0;
    char *outBuf = new char[outBytes+1];
    outBuf[outBytes] = 0;
    for(int i = 0; i < outBytes; i++)
    {
        outBuf[i] = outBufIn[i];
        outBuf[outBytes] ^= outBuf[i];
    }

    waitTime.tv_sec = 0;
    waitTime.tv_nsec = 20000000; //20ms
    if(!initialized)
    {
        r = pxd_serialConfigure(iUnitMap, 0,  115200, 8, 0, 1, 0, 0, 0);
        if (r < 0)
        {
            printf("ERROR CONFIGURING SERIAL CAMERALINK PORT\n");
            return r; // error
        }
        initialized = 1;
    }
    nanosleep(&waitTime, NULL);
    r = pxd_serialWrite(iUnitMap, 0, outBuf, outBytes+1);
    if (r < 0)
    {
        printf("ERROR IN SERIAL WRITE\n");
        return r; // error
    }
    nanosleep(&waitTime, NULL);
    r = pxd_serialRead(iUnitMap, 0, readBuf, readBytes);
    if (r < 0)
    {
        printf("ERROR IN SERIAL READ\n");
    }
    if(r != readBytes)
    printf("ERROR IN SERIAL READ: LESS BYTES READ THAN EXPECTED %d %d\n", r, readBytes);
    return r;
}

short epixGetCMOSTemp(int id)
{
    int retCount, r;
    char retBuf[50];
    short temp = 0;

    char queryBuf1[] = {0x53, 0xE0, 0x02, 0xF3, 0x7E, 0x50};
    char queryBuf2[] = {0x53, 0xE0, 0x02, 0xF4, 0x00, 0x50};
    char queryBuf3[] = {0x53, 0xE0, 0x01, 0x72, 0x50};
    char queryBuf4[] = {0x53, 0xE1, 0x01, 0x50};
    char queryBuf5[] = {0x53, 0xE0, 0x02, 0xF3, 0x7F, 0x50};
    char queryBuf6[] = {0x53, 0xE0, 0x02, 0xF4, 0x00, 0x50};
    char queryBuf7[] = {0x53, 0xE0, 0x01, 0x73, 0x50};
    char queryBuf8[] = {0x53, 0xE1, 0x01, 0x50};


    doTransaction(id, queryBuf1, 6, retBuf, 1);
    doTransaction(id, queryBuf2, 6, retBuf, 1);
    doTransaction(id, queryBuf3, 5, retBuf, 1);
    doTransaction(id, queryBuf4, 4, retBuf, 2);
    temp = (retBuf[0] & 0x00FF);
    doTransaction(id, queryBuf5, 6, retBuf, 1);
    doTransaction(id, queryBuf6, 6, retBuf, 1);
    doTransaction(id, queryBuf7, 5, retBuf, 1);
    doTransaction(id, queryBuf8, 4, retBuf, 2);
    temp |= (retBuf[0] & 0x00FF) << 8;
    return temp;
}

short epixGetPCBTemp(int id)
{
    int retCount, r;
    char retBuf[50];
    short temp = 0;

   // char queryBuf[] = {0x4F, 0x56, 0x50};

    char queryBuf1[] = {
    0x53, 0xE0, 0x02, 0x70, 0x00, 0x50};
    char queryBuf2[] = {
    0x53, 0xE1, 0x01, 0x50};
    char queryBuf3[] = {
    0x53, 0xE0, 0x02, 0x71, 0x00, 0x50};
    char queryBuf4[] = {
    0x53, 0xE1, 0x01, 0x50};

    //doTransaction(id, queryBuf, 3, retBuf, 2);
    doTransaction(id, queryBuf1, 6, retBuf, 1);
    doTransaction(id, queryBuf2, 4, retBuf, 2);
    temp = (retBuf[0] & 0x3) << 8;
    doTransaction(id, queryBuf3, 6, retBuf, 1);
    doTransaction(id, queryBuf4, 4, retBuf, 2);
    temp |= (retBuf[0] & 0x00FF);
    return temp;
}

static float getFrameRate(int id)
{
    char retBuf[50];
    int frameRate = 0;

    char queryBuf1[] = {0x53, 0xE0, 0x01, 0xDD, 0x50};
    char queryBuf2[] = {0x53, 0xE1, 0x01, 0x50};
    char queryBuf3[] = {0x53, 0xE0, 0x01, 0xDE, 0x50};
    char queryBuf4[] = {0x53, 0xE1, 0x01, 0x50};
    char queryBuf5[] = {0x53, 0xE0, 0x01, 0xDF, 0x50};
    char queryBuf6[] = {0x53, 0xE1, 0x01, 0x50};
    char queryBuf7[] = {0x53, 0xE0, 0x01, 0xE0, 0x50};
    char queryBuf8[] = {0x53, 0xE1, 0x01, 0x50};

    doTransaction(id, queryBuf1, 5, retBuf, 2);
    doTransaction(id, queryBuf2, 4, retBuf, 3);
    frameRate |= ((retBuf[0] & 0x000000FF) << 24);
    doTransaction(id, queryBuf3, 5, retBuf, 2);
    doTransaction(id, queryBuf4, 4, retBuf, 3);
    frameRate |= ((retBuf[0] & 0x000000FF) << 16);
    doTransaction(id, queryBuf5, 5, retBuf, 2);
    doTransaction(id, queryBuf6, 4, retBuf, 3);
    frameRate |= ((retBuf[0] & 0x000000FF) << 8);
    doTransaction(id, queryBuf7, 5, retBuf, 2);
    doTransaction(id, queryBuf8, 4, retBuf, 3);
    frameRate |= (retBuf[0] & 0x000000FF);
    return 1E9/(frameRate * 16.66);
}

static void setFrameRate(int id, float inFrameRate)
{
    int frameRate = fabs(1E9/(inFrameRate * 16.66));
    char queryBuf1[] = {0x53, 0xE0, 0x02, 0xDD, (frameRate & 0xFF000000) >> 24, 0x50};
    char queryBuf2[] = {0x53, 0xE0, 0x02, 0xDE, (frameRate & 0x00FF0000) >> 16, 0x50};
    char queryBuf3[] = {0x53, 0xE0, 0x02, 0xDF, (frameRate & 0x0000FF00) >> 8, 0x50};
    char queryBuf4[] = {0x53, 0xE0, 0x02, 0xE0, (frameRate & 0x000000FF), 0x50};
    char retBuf[50];
    doTransaction(id, queryBuf1, 6, retBuf, 1);
    doTransaction(id, queryBuf2, 6, retBuf, 1);
    doTransaction(id, queryBuf3, 6, retBuf, 1);
    doTransaction(id, queryBuf4, 6, retBuf, 1);
}

static double getExposure(int id)
{
    char retBuf[50];
    long exposure = 0;

    char queryBuf1[] = {0x53, 0xE0, 0x01, 0xED, 0x50};
    char queryBuf2[] = {0x53, 0xE1, 0x01, 0x50};
    char queryBuf3[] = {0x53, 0xE0, 0x01, 0xEE, 0x50};
    char queryBuf4[] = {0x53, 0xE1, 0x01, 0x50};
    char queryBuf5[] = {0x53, 0xE0, 0x01, 0xEF, 0x50};
    char queryBuf6[] = {0x53, 0xE1, 0x01, 0x50};
    char queryBuf7[] = {0x53, 0xE0, 0x01, 0xF0, 0x50};
    char queryBuf8[] = {0x53, 0xE1, 0x01, 0x50};
    char queryBuf9[] = {0x53, 0xE0, 0x01, 0xF1, 0x50};
    char queryBuf10[] = {0x53, 0xE1, 0x01, 0x50};

    doTransaction(id, queryBuf1, 5, retBuf, 1);
    doTransaction(id, queryBuf2, 4, retBuf, 2);
    exposure |= ((retBuf[0] & 0x00000000000000FFL) << 32);
    doTransaction(id, queryBuf3, 5, retBuf, 1);
    doTransaction(id, queryBuf4, 4, retBuf, 2);
    exposure |= ((retBuf[0] & 0x000000FF) << 24);
    doTransaction(id, queryBuf5, 5, retBuf, 1);
    doTransaction(id, queryBuf6, 4, retBuf, 2);
    exposure |= ((retBuf[0] & 0x000000FF) << 16);
    doTransaction(id, queryBuf7, 5, retBuf, 1);
    doTransaction(id, queryBuf8, 4, retBuf, 2);
    exposure |= ((retBuf[0] & 0x000000FF) << 8);
    doTransaction(id, queryBuf9, 5, retBuf, 1);
    doTransaction(id, queryBuf10, 4, retBuf, 2);
    exposure |= (retBuf[0] & 0x000000FF);
    return exposure * 16.66/1000000.;
}

static void setExposure(int id, float exposureMs)
{
    long exposure = fabs(1E6 * exposureMs/16.66);
//    long exposure = fabs(1E9 * exposureMs/16.66);

    char queryBuf1[] = {0x53, 0xE0, 0x02, 0xED, (exposure & 0x000000FF00000000L) >> 32, 0x50};
    char queryBuf2[] = {0x53, 0xE0, 0x02, 0xEE, (exposure & 0xFF000000) >> 24, 0x50};
    char queryBuf3[] = {0x53, 0xE0, 0x02, 0xEF, (exposure & 0x00FF0000) >> 16, 0x50};
    char queryBuf4[] = {0x53, 0xE0, 0x02, 0xF0, (exposure & 0x0000FF00) >> 8, 0x50};
    char queryBuf5[] = {0x53, 0xE0, 0x02, 0xF1, (exposure & 0x000000FF), 0x50};
    char retBuf[50];
    doTransaction(id, queryBuf1, 6, retBuf, 1);
    doTransaction(id, queryBuf2, 6, retBuf, 1);
    doTransaction(id, queryBuf3, 6, retBuf, 1);
    doTransaction(id, queryBuf4, 6, retBuf, 1);
}

static float getGain(int id)
{
    char retBuf[50];
    short gain = 0;

    char queryBuf1[] = {0x53, 0xE0, 0x01, 0xD5, 0x50};
    char queryBuf2[] = {0x53, 0xE1, 0x01, 0x50};
    char queryBuf3[] = {0x53, 0xE0, 0x01, 0xD6, 0x50};
    char queryBuf4[] = {0x53, 0xE1, 0x01, 0x50};

    doTransaction(id, queryBuf1, 5, retBuf, 2);
    doTransaction(id, queryBuf2, 4, retBuf, 3);
    gain |= ((retBuf[0] & 0x00FF) << 8);
    doTransaction(id, queryBuf3, 5, retBuf, 2);
    doTransaction(id, queryBuf4, 4, retBuf, 3);
    gain |= (retBuf[0] & 0x00FF);
    return gain/512.;
}

static char getTrigMode(int id)
{

    char queryBuf1[] = {0x53, 0xE0, 0x01, 0xD4, 0x50};
    char queryBuf2[] = {0x53, 0xE1, 0x01, 0x50};
    char retBuf[2];

    doTransaction(id, queryBuf1, 5, retBuf, 2);
    doTransaction(id, queryBuf2, 4, retBuf, 3);
    return retBuf[0];
}

static void setTrigMode(int id, char trigMode)
{

    char queryBuf1[] = {0x53, 0xE0, 0x02, 0xD4, trigMode, 0x50};
    char retBuf[2];

    doTransaction(id, queryBuf1, 6, retBuf, 1);
}

static char getBinning(int id)
{

    char queryBuf1[] = {0x53, 0xE0, 0x01, 0xDB, 0x50};
    char queryBuf2[] = {0x53, 0xE1, 0x01, 0x50};
    char retBuf[10];

    doTransaction(id, queryBuf1, 5, retBuf, 1);
    doTransaction(id, queryBuf2, 4, retBuf, 2);
    return retBuf[0];
}

static int getRoiXSize(int id)
{
    char retBuf[50];
    short size = 0;

    char queryBuf1[] = {0x53, 0xE0, 0x01, 0xD7, 0x50};
    char queryBuf2[] = {0x53, 0xE1, 0x01, 0x50};
    char queryBuf3[] = {0x53, 0xE0, 0x01, 0xD8, 0x50};
    char queryBuf4[] = {0x53, 0xE1, 0x01, 0x50};

    doTransaction(id, queryBuf1, 5, retBuf, 1);
    doTransaction(id, queryBuf2, 4, retBuf, 2);
    size |= ((retBuf[0] & 0x000F) << 8);
    doTransaction(id, queryBuf3, 5, retBuf, 1);
    doTransaction(id, queryBuf4, 4, retBuf, 2);
    size |= (retBuf[0] & 0x00FF);
    return (int)size;
}

static int getRoiXOffset(int id)
{
    char retBuf[50];
    short offset = 0;

    char queryBuf1[] = {0x53, 0xE0, 0x01, 0xD9, 0x50};
    char queryBuf2[] = {0x53, 0xE1, 0x01, 0x50};
    char queryBuf3[] = {0x53, 0xE0, 0x01, 0xDA, 0x50};
    char queryBuf4[] = {0x53, 0xE1, 0x01, 0x50};

    doTransaction(id, queryBuf1, 5, retBuf, 1);
    doTransaction(id, queryBuf2, 4, retBuf, 2);
    offset |= ((retBuf[0] & 0x000F) << 8);
    doTransaction(id, queryBuf3, 5, retBuf, 1);
    doTransaction(id, queryBuf4, 4, retBuf, 2);
    offset |= (retBuf[0] & 0x00FF);
    return (int)offset;
}

static int getRoiYSize(int id)
{
    char retBuf[50];
    short size = 0;

    char queryBuf1[] = {0x53, 0xE0, 0x02, 0xF3, 0x01, 0x50};
    char queryBuf2[] = {0x53, 0xE0, 0x02, 0xF4, 0x00, 0x50};
    char queryBuf3[] = {0x53, 0xE0, 0x01, 0x73, 0x50};
    char queryBuf4[] = {0x53, 0xE1, 0x01, 0x50};

    char queryBuf5[] = {0x53, 0xE0, 0x02, 0xF3, 0x02, 0x50};
    char queryBuf6[] = {0x53, 0xE0, 0x02, 0xF4, 0x00, 0x50};
    char queryBuf7[] = {0x53, 0xE0, 0x01, 0x73, 0x50};
    char queryBuf8[] = {0x53, 0xE1, 0x01, 0x50};


    doTransaction(id, queryBuf1, 6, retBuf, 1);
    doTransaction(id, queryBuf2, 6, retBuf, 1);
    doTransaction(id, queryBuf3, 5, retBuf, 1);
    doTransaction(id, queryBuf4, 4, retBuf, 2);
    size |= (retBuf[0] & 0x00FF);

    doTransaction(id, queryBuf5, 6, retBuf, 1);
    doTransaction(id, queryBuf6, 6, retBuf, 1);
    doTransaction(id, queryBuf7, 5, retBuf, 1);
    doTransaction(id, queryBuf8, 4, retBuf, 2);
    size |= ((retBuf[0] & 0x000F) << 8);
    return (int)size;
}

static int getRoiYOffset(int id)
{
    char retBuf[50];
    short size = 0;

    char queryBuf1[] = {0x53, 0xE0, 0x02, 0xF3, 0x03, 0x50};
    char queryBuf2[] = {0x53, 0xE0, 0x02, 0xF4, 0x00, 0x50};
    char queryBuf3[] = {0x53, 0xE0, 0x01, 0x73, 0x50};
    char queryBuf4[] = {0x53, 0xE1, 0x01, 0x50};

    char queryBuf5[] = {0x53, 0xE0, 0x02, 0xF3, 0x04, 0x50};
    char queryBuf6[] = {0x53, 0xE0, 0x02, 0xF4, 0x00, 0x50};
    char queryBuf7[] = {0x53, 0xE0, 0x01, 0x73, 0x50};
    char queryBuf8[] = {0x53, 0xE1, 0x01, 0x50};


    doTransaction(id, queryBuf1, 6, retBuf, 1);
    doTransaction(id, queryBuf2, 6, retBuf, 1);
    doTransaction(id, queryBuf3, 5, retBuf, 1);
    doTransaction(id, queryBuf4, 4, retBuf, 2);
    size |= (retBuf[0] & 0x00FF);

    doTransaction(id, queryBuf5, 6, retBuf, 1);
    doTransaction(id, queryBuf6, 6, retBuf, 1);
    doTransaction(id, queryBuf7, 5, retBuf, 1);
    doTransaction(id, queryBuf8, 4, retBuf, 2);
    size |= ((retBuf[0] & 0x000F) << 8);
    return (int)size;
}

void epixSetConfiguration(int id, float frameRate, char trigMode)
{
    setFrameRate(id, frameRate);
    setTrigMode(id, (char)trigMode);
}

void epixGetConfiguration(int id, int *binning, int *roiXSize, int *roiXOffset, int *roiYSize, int *roiYOffset)
{
    *roiXSize = getRoiXSize(id);
    *roiXOffset = getRoiXOffset(id);
    *roiYSize = getRoiYSize(id);
    *roiYOffset = getRoiYOffset(id);
    *binning = getBinning(id);
#ifdef DEBUG
    printf("EXPOSURE READ AS %f\n", getExposure(id));
#endif
}

void epixGetTemp(int id, float *pcbTemp, short *cmosTemp)
{
    *pcbTemp = epixGetPCBTemp(id)/16.;
    *cmosTemp = epixGetCMOSTemp(id);
}