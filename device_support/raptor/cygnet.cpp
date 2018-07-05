/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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
static double dSecPerTick = 1E-3;

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
int epixOpen(char *pcConfFile)
{
    int status;
    if(isOpen) return 0;
#ifdef DEBUG
    printf("Opening EPIX(R) PIXCI(R) Frame Grabber,\n");
    printf("using configuration parameters '%s',\n", DRIVERPARMS? DRIVERPARMS: "default");
#endif
    status = pxd_PIXCIopen(DRIVERPARMS, "", pcConfFile); // ConfFile includes exposure time which seems to take precedence over later serial commands
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
        return status;
    }
#ifdef DEBUG
    printFrameInfo();
    printImageInfo();
#endif
    uint32 ticku[2];
    if(pxd_infoSysTicksUnits(ticku) == 0)
    {
        dSecPerTick = (double)ticku[0] / (double)ticku[1] * 1E-6;
#ifdef DEBUG
        printf("Microseconds per tick: %f\n", dSecPerTick * 1E6);
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
void epixStartVideoCapture(int iID)
{
    pxd_goLivePair(iID, 1, 2); // should iID be converted to a unitmap?
#ifdef DEBUG
    printf("Video capture started.\n");
#endif
}

void epixStopVideoCapture(int iID)
{
    pxd_goUnLive(iID); // should iID be converted to a unitmap?
#ifdef DEBUG
    printf("Video capture stopped.\n");
#endif

}

//Capture either a single frame or timeouts.
//Returns 1 on capture and 0 on timeout
//Return the tick count of the last frame or the passed baseTicks in case of timeout

//iID: device iID (1)
//dataNid: nid number of target data node
//timeNid: nid number of associated timestamp node. If -1, the time is put directly in the segmented data, else a reference to the timebase is put
//timeoutMs: timeout in milliseconds
//*treePtr: pointer to Tree object
//*listPtr: pointer to Internal List object for saving frames
//*bufIdx: index of the last captured frame grabber buffer        (initialize with -1 !!!)
//*frameIdx: frame index (from 0 onwards)
//*baseTicks: last 32 bits of system time when the first frame has been taken
//*currtime: returns time since reference t=0
int epixCaptureFrame(int iID, int iFramesNid, double dTriggerTime, int iTimeoutMs, void *pTree, void *pList, int *piBufIdx, int *piFrameIdx, int *piBaseTicks, double *pdCurrTime)
{
    int iUnitMap = 1 << (iID - 1);
    int iLastBufIdx, iCurrTicks;
    int iPixelsRead, iPixelsToRead, iPixelsX, iPixelsY;
    struct timespec waitTime;
    pxbuffer_t  lastbuf = 0;
    iPixelsX = pxd_imageXdim();
    iPixelsY = pxd_imageYdim();
    iPixelsToRead = iPixelsX * iPixelsY;
    waitTime.tv_sec = 0;
    waitTime.tv_nsec = 5000000; //5ms delay
    if(*piBufIdx < 0)
    {
        iLastBufIdx = pxd_capturedBuffer(iUnitMap);  //The first time captureFrame is called
        *piFrameIdx = 0;
    }
    else
        iLastBufIdx = *piBufIdx;
    int iMaxCount = iTimeoutMs/5; //Maximum number of iterations before timing out
    for(int i = 0; i < iMaxCount; i++)
    {
        int iLastCaptured = pxd_capturedBuffer(iUnitMap);
        if(iLastCaptured != iLastBufIdx) //A new frame arrived
        {
            iCurrTicks = pxd_capturedSysTicks(iUnitMap);//internal clock
            if ( *piFrameIdx == 0) //first frame
                *piBaseTicks = iCurrTicks;
            *pdCurrTime = (iCurrTicks - (*piBaseTicks)) * dSecPerTick + dTriggerTime;
            unsigned short *piFrame = new unsigned short[iPixelsToRead];//allocate frame
            iPixelsRead = pxd_readushort(iUnitMap, iLastCaptured, 0, 0, iPixelsX, iPixelsY, piFrame, iPixelsToRead, (char *)"Grey");//get frame
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
            camSaveFrameDirect(piFrame, iPixelsX, iPixelsY, *pdCurrTime, 12, pTree, iFramesNid, -1, *piFrameIdx, pList);
            *piBufIdx = iLastCaptured;
            *piFrameIdx += 1;
            return 1;
        }
        else //No new frame
            nanosleep(&waitTime, NULL);
    }
//If code arrives here timeout occurred
    return 0;
}

int doTransaction(int iID, const char *pcOutBufIn, int iOutBytes, char *pcReadBuf, int iBytesToRead)
{
    int iBytesRead;
    int iUnitMap = 1 << (iID-1);
    struct timespec waitTime;
    static bool isInitialized = false;
    char *pcOutBuf = new char[iOutBytes+1];
    pcOutBuf[iOutBytes] = 0;
    for(int i = 0; i < iOutBytes; i++)
    {
        pcOutBuf[i] = pcOutBufIn[i];
        pcOutBuf[iOutBytes] ^= pcOutBuf[i];
    }
    waitTime.tv_sec = 0;
    waitTime.tv_nsec = 20000000; //20ms
    if(!isInitialized)
    {
        iBytesRead = pxd_serialConfigure(iUnitMap, 0,  115200, 8, 0, 1, 0, 0, 0);
        if (iBytesRead < 0)
        {
            printf("ERROR CONFIGURING SERIAL CAMERALINK PORT\n");
            return iBytesRead; // error
        }
        isInitialized = true;
    }
    nanosleep(&waitTime, NULL);
    iBytesRead = pxd_serialWrite(iUnitMap, 0, pcOutBuf, iOutBytes+1);
    if (iBytesRead < 0)
    {
        printf("ERROR IN SERIAL WRITE\n");
        return iBytesRead; // error
    }
    nanosleep(&waitTime, NULL);
    iBytesRead = pxd_serialRead(iUnitMap, 0, pcReadBuf, iBytesToRead);
    if (iBytesRead < 0)
        printf("ERROR IN SERIAL READ\n");
    else if(iBytesRead != iBytesToRead)
        printf("ERROR IN SERIAL READ: LESS BYTES READ THAN EXPECTED %d %d\n", iBytesRead, iBytesToRead);
    return iBytesRead;
}

short epixGetCMOSTemp(int iID)
{
    const char queryBuf1[] = {0x53, 0xE0, 0x02, 0xF3, 0x7E, 0x50};
    const char queryBuf2[] = {0x53, 0xE0, 0x02, 0xF4, 0x00, 0x50};
    const char queryBuf3[] = {0x53, 0xE0, 0x01, 0x72, 0x50};
    const char queryBuf4[] = {0x53, 0xE1, 0x01, 0x50};
    const char queryBuf5[] = {0x53, 0xE0, 0x02, 0xF3, 0x7F, 0x50};
    const char queryBuf6[] = {0x53, 0xE0, 0x02, 0xF4, 0x00, 0x50};
    const char queryBuf7[] = {0x53, 0xE0, 0x01, 0x73, 0x50};
    const char queryBuf8[] = {0x53, 0xE1, 0x01, 0x50};

    short sTemp = 0;
    char cRetBuf[2];
    doTransaction(iID, queryBuf1, 6, cRetBuf, 1);
    doTransaction(iID, queryBuf2, 6, cRetBuf, 1);
    doTransaction(iID, queryBuf3, 5, cRetBuf, 1);
    doTransaction(iID, queryBuf4, 4, cRetBuf, 2);
    sTemp = (cRetBuf[0] & 0x00FF);
    doTransaction(iID, queryBuf5, 6, cRetBuf, 1);
    doTransaction(iID, queryBuf6, 6, cRetBuf, 1);
    doTransaction(iID, queryBuf7, 5, cRetBuf, 1);
    doTransaction(iID, queryBuf8, 4, cRetBuf, 2);
    sTemp |= (cRetBuf[0] & 0x00FF) << 8;
    return sTemp;
}

short epixGetPCBTemp(int iID)
{
   // const char queryBuf[] = {0x4F, 0x56, 0x50};
    const char queryBuf1[] = {0x53, 0xE0, 0x02, 0x70, 0x00, 0x50};
    const char queryBuf2[] = {0x53, 0xE1, 0x01, 0x50};
    const char queryBuf3[] = {0x53, 0xE0, 0x02, 0x71, 0x00, 0x50};
    const char queryBuf4[] = {0x53, 0xE1, 0x01, 0x50};

    short sTemp = 0;
    char cRetBuf[2];
    //doTransaction(iID, queryBuf, 3, cRetBuf, 2);
    doTransaction(iID, queryBuf1, 6, cRetBuf, 1);
    doTransaction(iID, queryBuf2, 4, cRetBuf, 2);
    sTemp = (cRetBuf[0] & 0x3) << 8;
    doTransaction(iID, queryBuf3, 6, cRetBuf, 1);
    doTransaction(iID, queryBuf4, 4, cRetBuf, 2);
    sTemp |= (cRetBuf[0] & 0x00FF);
    return sTemp;
}

static double getFrameRate(int iID)
{
    const char queryBuf1[] = {0x53, 0xE0, 0x01, 0xDD, 0x50};
    const char queryBuf2[] = {0x53, 0xE1, 0x01, 0x50};
    const char queryBuf3[] = {0x53, 0xE0, 0x01, 0xDE, 0x50};
    const char queryBuf4[] = {0x53, 0xE1, 0x01, 0x50};
    const char queryBuf5[] = {0x53, 0xE0, 0x01, 0xDF, 0x50};
    const char queryBuf6[] = {0x53, 0xE1, 0x01, 0x50};
    const char queryBuf7[] = {0x53, 0xE0, 0x01, 0xE0, 0x50};
    const char queryBuf8[] = {0x53, 0xE1, 0x01, 0x50};

    int iFrameRate = 0;
    char cRetBuf[3];
    doTransaction(iID, queryBuf1, 5, cRetBuf, 2);
    doTransaction(iID, queryBuf2, 4, cRetBuf, 3);
    iFrameRate |= ((cRetBuf[0] & 0x000000FF) << 24);
    doTransaction(iID, queryBuf3, 5, cRetBuf, 2);
    doTransaction(iID, queryBuf4, 4, cRetBuf, 3);
    iFrameRate |= ((cRetBuf[0] & 0x000000FF) << 16);
    doTransaction(iID, queryBuf5, 5, cRetBuf, 2);
    doTransaction(iID, queryBuf6, 4, cRetBuf, 3);
    iFrameRate |= ((cRetBuf[0] & 0x000000FF) << 8);
    doTransaction(iID, queryBuf7, 5, cRetBuf, 2);
    doTransaction(iID, queryBuf8, 4, cRetBuf, 3);
    iFrameRate |= (cRetBuf[0] & 0x000000FF);
    return (double)iFrameRate / 6E7;
}

static void setFrameRate(int iID, double dFrameRate)
{
    int iFrameRate = fabs(6E7 * dFrameRate);
    const char queryBuf1[] = {0x53, 0xE0, 0x02, 0xDD, (iFrameRate & 0xFF000000) >> 24, 0x50};
    const char queryBuf2[] = {0x53, 0xE0, 0x02, 0xDE, (iFrameRate & 0x00FF0000) >> 16, 0x50};
    const char queryBuf3[] = {0x53, 0xE0, 0x02, 0xDF, (iFrameRate & 0x0000FF00) >> 8, 0x50};
    const char queryBuf4[] = {0x53, 0xE0, 0x02, 0xE0, (iFrameRate & 0x000000FF), 0x50};

    char cRetBuf[1];
    doTransaction(iID, queryBuf1, 6, cRetBuf, 1);
    doTransaction(iID, queryBuf2, 6, cRetBuf, 1);
    doTransaction(iID, queryBuf3, 6, cRetBuf, 1);
    doTransaction(iID, queryBuf4, 6, cRetBuf, 1);
}

static double getExposure(int iID)
{
    const char queryBuf1[] = {0x53, 0xE0, 0x01, 0xED, 0x50};
    const char queryBuf2[] = {0x53, 0xE1, 0x01, 0x50};
    const char queryBuf3[] = {0x53, 0xE0, 0x01, 0xEE, 0x50};
    const char queryBuf4[] = {0x53, 0xE1, 0x01, 0x50};
    const char queryBuf5[] = {0x53, 0xE0, 0x01, 0xEF, 0x50};
    const char queryBuf6[] = {0x53, 0xE1, 0x01, 0x50};
    const char queryBuf7[] = {0x53, 0xE0, 0x01, 0xF0, 0x50};
    const char queryBuf8[] = {0x53, 0xE1, 0x01, 0x50};
    const char queryBuf9[] = {0x53, 0xE0, 0x01, 0xF1, 0x50};
    const char queryBuf10[]= {0x53, 0xE1, 0x01, 0x50};

    long lExposure = 0;
    char cRetBuf[2];
    doTransaction(iID, queryBuf1, 5, cRetBuf, 1);
    doTransaction(iID, queryBuf2, 4, cRetBuf, 2);
    lExposure |= ((cRetBuf[0] & 0x00000000000000FFL) << 32);
    doTransaction(iID, queryBuf3, 5, cRetBuf, 1);
    doTransaction(iID, queryBuf4, 4, cRetBuf, 2);
    lExposure |= ((cRetBuf[0] & 0x000000FF) << 24);
    doTransaction(iID, queryBuf5, 5, cRetBuf, 1);
    doTransaction(iID, queryBuf6, 4, cRetBuf, 2);
    lExposure |= ((cRetBuf[0] & 0x000000FF) << 16);
    doTransaction(iID, queryBuf7, 5, cRetBuf, 1);
    doTransaction(iID, queryBuf8, 4, cRetBuf, 2);
    lExposure |= ((cRetBuf[0] & 0x000000FF) << 8);
    doTransaction(iID, queryBuf9, 5, cRetBuf, 1);
    doTransaction(iID, queryBuf10,4, cRetBuf, 2);
    lExposure |= (cRetBuf[0] & 0x000000FF);
    return (double)lExposure / 6E4;
}

static void setExposure(int iID, double dExposureMs)
{
    long lExposure = fabs(6E4 * dExposureMs);//60MHz: 1E6/16.6666 = 6e4
//    long lExposure = fabs(6E7 * dExposure);
    const char queryBuf1[] = {0x53, 0xE0, 0x02, 0xED, (lExposure & 0x000000FF00000000L) >> 32, 0x50};
    const char queryBuf2[] = {0x53, 0xE0, 0x02, 0xEE, (lExposure & 0xFF000000) >> 24, 0x50};
    const char queryBuf3[] = {0x53, 0xE0, 0x02, 0xEF, (lExposure & 0x00FF0000) >> 16, 0x50};
    const char queryBuf4[] = {0x53, 0xE0, 0x02, 0xF0, (lExposure & 0x0000FF00) >> 8, 0x50};
    const char queryBuf5[] = {0x53, 0xE0, 0x02, 0xF1, (lExposure & 0x000000FF), 0x50};

    char cRetBuf[1];
    doTransaction(iID, queryBuf1, 6, cRetBuf, 1);
    doTransaction(iID, queryBuf2, 6, cRetBuf, 1);
    doTransaction(iID, queryBuf3, 6, cRetBuf, 1);
    doTransaction(iID, queryBuf4, 6, cRetBuf, 1);
}

static float getGain(int iID)
{
    const char queryBuf1[] = {0x53, 0xE0, 0x01, 0xD5, 0x50};
    const char queryBuf2[] = {0x53, 0xE1, 0x01, 0x50};
    const char queryBuf3[] = {0x53, 0xE0, 0x01, 0xD6, 0x50};
    const char queryBuf4[] = {0x53, 0xE1, 0x01, 0x50};

    short sGain = 0;
    char cRetBuf[3];
    doTransaction(iID, queryBuf1, 5, cRetBuf, 2);
    doTransaction(iID, queryBuf2, 4, cRetBuf, 3);
    sGain |= ((cRetBuf[0] & 0x00FF) << 8);
    doTransaction(iID, queryBuf3, 5, cRetBuf, 2);
    doTransaction(iID, queryBuf4, 4, cRetBuf, 3);
    sGain |= (cRetBuf[0] & 0x00FF);
    return sGain/512.;
}

static char getTrigMode(int iID)
{
    const char queryBuf1[] = {0x53, 0xE0, 0x01, 0xD4, 0x50};
    const char queryBuf2[] = {0x53, 0xE1, 0x01, 0x50};

    char cRetBuf[3];
    doTransaction(iID, queryBuf1, 5, cRetBuf, 2);
    doTransaction(iID, queryBuf2, 4, cRetBuf, 3);
    return cRetBuf[0];
}

static void setTrigMode(int iID, char cTrigMode)
{
    const char queryBuf1[] = {0x53, 0xE0, 0x02, 0xD4, cTrigMode, 0x50};

    char cRetBuf[1];
    doTransaction(iID, queryBuf1, 6, cRetBuf, 1);
}

static char getBinning(int iID)
{
    const char queryBuf1[] = {0x53, 0xE0, 0x01, 0xDB, 0x50};
    const char queryBuf2[] = {0x53, 0xE1, 0x01, 0x50};

    char cRetBuf[2];
    doTransaction(iID, queryBuf1, 5, cRetBuf, 1);
    doTransaction(iID, queryBuf2, 4, cRetBuf, 2);
    return cRetBuf[0];
}

static short getRoiXSize(int iID)
{
    const char queryBuf1[] = {0x53, 0xE0, 0x01, 0xD7, 0x50};
    const char queryBuf2[] = {0x53, 0xE1, 0x01, 0x50};
    const char queryBuf3[] = {0x53, 0xE0, 0x01, 0xD8, 0x50};
    const char queryBuf4[] = {0x53, 0xE1, 0x01, 0x50};

    short sSize = 0;
    char cRetBuf[2];
    doTransaction(iID, queryBuf1, 5, cRetBuf, 1);
    doTransaction(iID, queryBuf2, 4, cRetBuf, 2);
    sSize |= ((cRetBuf[0] & 0x000F) << 8);
    doTransaction(iID, queryBuf3, 5, cRetBuf, 1);
    doTransaction(iID, queryBuf4, 4, cRetBuf, 2);
    sSize |= (cRetBuf[0] & 0x00FF);
    return sSize;
}

static short getRoiXOffset(int iID)
{
    const char queryBuf1[] = {0x53, 0xE0, 0x01, 0xD9, 0x50};
    const char queryBuf2[] = {0x53, 0xE1, 0x01, 0x50};
    const char queryBuf3[] = {0x53, 0xE0, 0x01, 0xDA, 0x50};
    const char queryBuf4[] = {0x53, 0xE1, 0x01, 0x50};

    short sOffset = 0;
    char cRetBuf[2];
    doTransaction(iID, queryBuf1, 5, cRetBuf, 1);
    doTransaction(iID, queryBuf2, 4, cRetBuf, 2);
    sOffset |= ((cRetBuf[0] & 0x000F) << 8);
    doTransaction(iID, queryBuf3, 5, cRetBuf, 1);
    doTransaction(iID, queryBuf4, 4, cRetBuf, 2);
    sOffset |= (cRetBuf[0] & 0x00FF);
    return sOffset;
}

static short getRoiYSize(int iID)
{
    const char queryBuf1[] = {0x53, 0xE0, 0x02, 0xF3, 0x01, 0x50};
    const char queryBuf2[] = {0x53, 0xE0, 0x02, 0xF4, 0x00, 0x50};
    const char queryBuf3[] = {0x53, 0xE0, 0x01, 0x73, 0x50};
    const char queryBuf4[] = {0x53, 0xE1, 0x01, 0x50};
    const char queryBuf5[] = {0x53, 0xE0, 0x02, 0xF3, 0x02, 0x50};
    const char queryBuf6[] = {0x53, 0xE0, 0x02, 0xF4, 0x00, 0x50};
    const char queryBuf7[] = {0x53, 0xE0, 0x01, 0x73, 0x50};
    const char queryBuf8[] = {0x53, 0xE1, 0x01, 0x50};

    short sSize = 0;
    char cRetBuf[2];
    doTransaction(iID, queryBuf1, 6, cRetBuf, 1);
    doTransaction(iID, queryBuf2, 6, cRetBuf, 1);
    doTransaction(iID, queryBuf3, 5, cRetBuf, 1);
    doTransaction(iID, queryBuf4, 4, cRetBuf, 2);
    sSize |= (cRetBuf[0] & 0x00FF);
    doTransaction(iID, queryBuf5, 6, cRetBuf, 1);
    doTransaction(iID, queryBuf6, 6, cRetBuf, 1);
    doTransaction(iID, queryBuf7, 5, cRetBuf, 1);
    doTransaction(iID, queryBuf8, 4, cRetBuf, 2);
    sSize |= ((cRetBuf[0] & 0x000F) << 8);
    return sSize;
}

static short getRoiYOffset(int iID)
{
    const char queryBuf1[] = {0x53, 0xE0, 0x02, 0xF3, 0x03, 0x50};
    const char queryBuf2[] = {0x53, 0xE0, 0x02, 0xF4, 0x00, 0x50};
    const char queryBuf3[] = {0x53, 0xE0, 0x01, 0x73, 0x50};
    const char queryBuf4[] = {0x53, 0xE1, 0x01, 0x50};
    const char queryBuf5[] = {0x53, 0xE0, 0x02, 0xF3, 0x04, 0x50};
    const char queryBuf6[] = {0x53, 0xE0, 0x02, 0xF4, 0x00, 0x50};
    const char queryBuf7[] = {0x53, 0xE0, 0x01, 0x73, 0x50};
    const char queryBuf8[] = {0x53, 0xE1, 0x01, 0x50};

    short sSize = 0;
    char cRetBuf[2];
    doTransaction(iID, queryBuf1, 6, cRetBuf, 1);
    doTransaction(iID, queryBuf2, 6, cRetBuf, 1);
    doTransaction(iID, queryBuf3, 5, cRetBuf, 1);
    doTransaction(iID, queryBuf4, 4, cRetBuf, 2);
    sSize |= (cRetBuf[0] & 0x00FF);
    doTransaction(iID, queryBuf5, 6, cRetBuf, 1);
    doTransaction(iID, queryBuf6, 6, cRetBuf, 1);
    doTransaction(iID, queryBuf7, 5, cRetBuf, 1);
    doTransaction(iID, queryBuf8, 4, cRetBuf, 2);
    sSize |= ((cRetBuf[0] & 0x000F) << 8);
    return sSize;
}

void epixSetConfiguration(int iID, double dFrameRate, char cTrigMode)
{
    setFrameRate(iID, dFrameRate);
    setTrigMode(iID, cTrigMode);
}

void epixGetConfiguration(int iID, char *pcBinning, short *psRoiXSize, short *psRoiXOffset, short *psRoiYSize, short *psRoiYOffset)
{
    *pcBinning    = getBinning(iID);
    *psRoiXSize   = getRoiXSize(iID);
    *psRoiXOffset = getRoiXOffset(iID);
    *psRoiYSize   = getRoiYSize(iID);
    *psRoiYOffset = getRoiYOffset(iID);
#ifdef DEBUG
    printf("EXPOSURE READ AS %f\n", getExposure(iID));
#endif
}

void epixGetTemp(int iID, float *pfPcbTemp, short *psCmosTemp)
{
    *pfPcbTemp  = epixGetPCBTemp(iID)/16.;
    *psCmosTemp = epixGetCMOSTemp(iID);
}