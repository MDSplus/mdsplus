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
/*******************************************************
 This module was generated using mdsshr/gen_messages.py
 To add new status messages modify
 one of the "*_messages.xml" files
 and then do:
     python mdsshr/gen_messages.py
*******************************************************/

#include <mdsplus/mdsconfig.h>

static const char *FAC_DEV = "DEV";
static const char *FAC_RETICON = "RETICON";
static const char *FAC_J221 = "J221";
static const char *FAC_TIMING = "TIMING";
static const char *FAC_B2408 = "B2408";
static const char *FAC_FERA = "FERA";
static const char *FAC_HM650 = "HM650";
static const char *FAC_HV4032 = "HV4032";
static const char *FAC_HV1440 = "HV1440";
static const char *FAC_JOERGER = "JOERGER";
static const char *FAC_U_OF_M = "U_OF_M";
static const char *FAC_IDL = "IDL";
static const char *FAC_B5910A = "B5910A";
static const char *FAC_J412 = "J412";
static const char *FAC_TR16 = "TR16";
static const char *FAC_A14 = "A14";
static const char *FAC_L6810 = "L6810";
static const char *FAC_J_DAC = "J_DAC";
static const char *FAC_INCAA = "INCAA";
static const char *FAC_L8212 = "L8212";
static const char *FAC_MPB = "MPB";
static const char *FAC_L8828 = "L8828";
static const char *FAC_L8818 = "L8818";
static const char *FAC_J_TR612 = "J_TR612";
static const char *FAC_L8206 = "L8206";
static const char *FAC_H912 = "H912";
static const char *FAC_H908 = "H908";
static const char *FAC_DSP2904 = "DSP2904";
static const char *FAC_PY = "PY";
static const char *FAC_DT196B = "DT196B";
static const char *FAC_TREE = "TREE";
static const char *FAC_LIB = "LIB";
static const char *FAC_STR = "STR";
static const char *FAC_MDSPLUS = "MDSPLUS";
static const char *FAC_SS = "SS";
static const char *FAC_TDI = "TDI";
static const char *FAC_APD = "APD";
static const char *FAC_MDSDCL = "MDSDCL";
static const char *FAC_SERVER = "SERVER";
static const char *FAC_CAM = "CAM";
static const char *FAC_TCL = "TCL";


EXPORT int MdsGetStdMsg(int status, const char **fac_out, const char **msgnam_out, const char **text_out) {
    int sts;
    switch (status & (-8)) {

/* DevBAD_ENDIDX */
      case 04737100010:
        {static const char *text="unable to read end index for channel";
        static const char *msgnam="BAD_ENDIDX";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevBAD_FILTER */
      case 04737100020:
        {static const char *text="illegal filter selected";
        static const char *msgnam="BAD_FILTER";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevBAD_FREQ */
      case 04737100030:
        {static const char *text="illegal digitization frequency selected";
        static const char *msgnam="BAD_FREQ";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevBAD_GAIN */
      case 04737100040:
        {static const char *text="illegal gain selected";
        static const char *msgnam="BAD_GAIN";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevBAD_HEADER */
      case 04737100050:
        {static const char *text="unable to read header selection";
        static const char *msgnam="BAD_HEADER";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevBAD_HEADER_IDX */
      case 04737100060:
        {static const char *text="unknown header configuration index";
        static const char *msgnam="BAD_HEADER_IDX";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevBAD_MEMORIES */
      case 04737100070:
        {static const char *text="unable to read number of memory modules";
        static const char *msgnam="BAD_MEMORIES";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevBAD_MODE */
      case 04737100100:
        {static const char *text="illegal mode selected";
        static const char *msgnam="BAD_MODE";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevBAD_NAME */
      case 04737100110:
        {static const char *text="unable to read module name";
        static const char *msgnam="BAD_NAME";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevBAD_OFFSET */
      case 04737100120:
        {static const char *text="illegal offset selected";
        static const char *msgnam="BAD_OFFSET";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevBAD_STARTIDX */
      case 04737100130:
        {static const char *text="unable to read start index for channel";
        static const char *msgnam="BAD_STARTIDX";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevNOT_TRIGGERED */
      case 04737100140:
        {static const char *text="device was not triggered,  check wires and triggering device";
        static const char *msgnam="NOT_TRIGGERED";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevFREQ_TO_HIGH */
      case 04737100150:
        {static const char *text="the frequency is set to high for the requested number of channels";
        static const char *msgnam="FREQ_TO_HIGH";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevINVALID_NOC */
      case 04737100160:
        {static const char *text="the NOC (number of channels) requested is greater than the physical number of channels";
        static const char *msgnam="INVALID_NOC";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevRANGE_MISMATCH */
      case 04737100170:
        {static const char *text="the range specified on the menu doesn't match the range setting on the device";
        static const char *msgnam="RANGE_MISMATCH";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevCAMACERR */
      case 04737100200:
        {static const char *text="Error doing CAMAC IO";
        static const char *msgnam="CAMACERR";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevBAD_VERBS */
      case 04737100210:
        {static const char *text="Error reading interpreter list (:VERBS)";
        static const char *msgnam="BAD_VERBS";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevBAD_COMMANDS */
      case 04737100220:
        {static const char *text="Error reading command list";
        static const char *msgnam="BAD_COMMANDS";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevCAM_ADNR */
      case 04737100230:
        {static const char *text="CAMAC: Address not recognized (2160)";
        static const char *msgnam="CAM_ADNR";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevCAM_ERR */
      case 04737100240:
        {static const char *text="CAMAC: Error reported by crate controler";
        static const char *msgnam="CAM_ERR";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevCAM_LOSYNC */
      case 04737100250:
        {static const char *text="CAMAC: Lost Syncronization error";
        static const char *msgnam="CAM_LOSYNC";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevCAM_LPE */
      case 04737100260:
        {static const char *text="CAMAC: Longitudinal Parity error";
        static const char *msgnam="CAM_LPE";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevCAM_TMO */
      case 04737100270:
        {static const char *text="CAMAC: Highway time out error";
        static const char *msgnam="CAM_TMO";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevCAM_TPE */
      case 04737100300:
        {static const char *text="CAMAC: Transverse Parity error";
        static const char *msgnam="CAM_TPE";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevCAM_STE */
      case 04737100310:
        {static const char *text="CAMAC: Serial Transmission error";
        static const char *msgnam="CAM_STE";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevCAM_DERR */
      case 04737100320:
        {static const char *text="CAMAC: Delayed error from SCC";
        static const char *msgnam="CAM_DERR";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevCAM_SQ */
      case 04737100330:
        {static const char *text="CAMAC: I/O completion with Q = 1";
        static const char *msgnam="CAM_SQ";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevCAM_NOSQ */
      case 04737100340:
        {static const char *text="CAMAC: I/O completion with Q = 0";
        static const char *msgnam="CAM_NOSQ";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevCAM_SX */
      case 04737100350:
        {static const char *text="CAMAC: I/O completion with X = 1";
        static const char *msgnam="CAM_SX";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevCAM_NOSX */
      case 04737100360:
        {static const char *text="CAMAC: I/O completion with X = 0";
        static const char *msgnam="CAM_NOSX";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevINV_SETUP */
      case 04737100370:
        {static const char *text="device was not properly set up";
        static const char *msgnam="INV_SETUP";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevPYDEVICE_NOT_FOUND */
      case 04737100400:
        {static const char *text="Python device class not found.";
        static const char *msgnam="PYDEVICE_NOT_FOUND";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevPY_INTERFACE_LIBRARY_NOT_FOUND */
      case 04737100410:
        {static const char *text="The needed device hardware interface library could not be loaded.";
        static const char *msgnam="PY_INTERFACE_LIBRARY_NOT_FOUND";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevIO_STUCK */
      case 04737100420:
        {static const char *text="I/O to Device is stuck. Check network connection and board status.";
        static const char *msgnam="IO_STUCK";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevUNKOWN_STATE */
      case 04737100430:
        {static const char *text="Device returned unrecognized state string";
        static const char *msgnam="UNKOWN_STATE";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevWRONG_TREE */
      case 04737100440:
        {static const char *text="Attempt to digitizerinto different tree than it was armed with";
        static const char *msgnam="WRONG_TREE";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevWRONG_PATH */
      case 04737100450:
        {static const char *text="Attempt to store digitizer into different path than it was armed with";
        static const char *msgnam="WRONG_PATH";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevWRONG_SHOT */
      case 04737100460:
        {static const char *text="Attempt to store digitizer into different shot than it was armed with";
        static const char *msgnam="WRONG_SHOT";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevOFFLINE */
      case 04737100470:
        {static const char *text="Device is not on line.  Check network connection";
        static const char *msgnam="OFFLINE";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevTRIGGERED_NOT_STORED */
      case 04737100500:
        {static const char *text="Device was triggered but not stored.";
        static const char *msgnam="TRIGGERED_NOT_STORED";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevNO_NAME_SPECIFIED */
      case 04737100510:
        {static const char *text="Device name must be specifed - pleas fill it in.";
        static const char *msgnam="NO_NAME_SPECIFIED";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevBAD_ACTIVE_CHAN */
      case 04737100520:
        {static const char *text="Active channels either not available or invalid";
        static const char *msgnam="BAD_ACTIVE_CHAN";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevBAD_TRIG_SRC */
      case 04737100530:
        {static const char *text="Trigger source either not available or invalid";
        static const char *msgnam="BAD_TRIG_SRC";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevBAD_CLOCK_SRC */
      case 04737100540:
        {static const char *text="Clock source either not available or invalid";
        static const char *msgnam="BAD_CLOCK_SRC";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevBAD_PRE_TRIG */
      case 04737100550:
        {static const char *text="Pre trigger samples either not available or invalid";
        static const char *msgnam="BAD_PRE_TRIG";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevBAD_POST_TRIG */
      case 04737100560:
        {static const char *text="Post trigger samples either not available or invalid";
        static const char *msgnam="BAD_POST_TRIG";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevBAD_CLOCK_FREQ */
      case 04737100570:
        {static const char *text="Clock frequency either not available or invalid";
        static const char *msgnam="BAD_CLOCK_FREQ";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevTRIGGER_FAILED */
      case 04737100600:
        {static const char *text="Device trigger method failed";
        static const char *msgnam="TRIGGER_FAILED";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevERROR_READING_CHANNEL */
      case 04737100610:
        {static const char *text="Error reading data for channel from device";
        static const char *msgnam="ERROR_READING_CHANNEL";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevERROR_DOING_INIT */
      case 04737100620:
        {static const char *text="Error sending ARM command to device";
        static const char *msgnam="ERROR_DOING_INIT";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* ReticonNORMAL */
      case 04737100630:
        {static const char *text="successful completion";
        static const char *msgnam="NORMAL";
        *fac_out = FAC_RETICON;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* ReticonBAD_FRAMES */
      case 04737100640:
        {static const char *text="frame count must be less than or equal to 2048";
        static const char *msgnam="BAD_FRAMES";
        *fac_out = FAC_RETICON;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* ReticonBAD_FRAME_SELECT */
      case 04737100650:
        {static const char *text="frame interval must be 1,2,4,8,16,32 or 64";
        static const char *msgnam="BAD_FRAME_SELECT";
        *fac_out = FAC_RETICON;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* ReticonBAD_NUM_STATES */
      case 04737100660:
        {static const char *text="number of states must be between 1 and 4";
        static const char *msgnam="BAD_NUM_STATES";
        *fac_out = FAC_RETICON;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* ReticonBAD_PERIOD */
      case 04737100670:
        {static const char *text="period must be .5,1,2,4,8,16,32,64,128,256,512,1024,2048,4096 or 8192 msec";
        static const char *msgnam="BAD_PERIOD";
        *fac_out = FAC_RETICON;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* ReticonBAD_PIXEL_SELECT */
      case 04737100700:
        {static const char *text="pixel selection must be an array of 256 boolean values";
        static const char *msgnam="BAD_PIXEL_SELECT";
        *fac_out = FAC_RETICON;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* ReticonDATA_CORRUPTED */
      case 04737100710:
        {static const char *text="data in memory is corrupted or framing error detected, no data stored";
        static const char *msgnam="DATA_CORRUPTED";
        *fac_out = FAC_RETICON;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* ReticonTOO_MANY_FRAMES */
      case 04737100720:
        {static const char *text="over 8192 frame start indicators in data read from memory";
        static const char *msgnam="TOO_MANY_FRAMES";
        *fac_out = FAC_RETICON;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* J221NORMAL */
      case 04737101450:
        {static const char *text="successful completion";
        static const char *msgnam="NORMAL";
        *fac_out = FAC_J221;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* J221INVALID_DATA */
      case 04737101460:
        {static const char *text="ignoring invalid data in channel !SL";
        static const char *msgnam="INVALID_DATA";
        *fac_out = FAC_J221;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* J221NO_DATA */
      case 04737101470:
        {static const char *text="no valid data was found for any channel";
        static const char *msgnam="NO_DATA";
        *fac_out = FAC_J221;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TimingINVCLKFRQ */
      case 04737102270:
        {static const char *text="Invalid clock frequency";
        static const char *msgnam="INVCLKFRQ";
        *fac_out = FAC_TIMING;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TimingINVDELDUR */
      case 04737102300:
        {static const char *text="Invalid pulse delay or duration, must be less than 655 seconds";
        static const char *msgnam="INVDELDUR";
        *fac_out = FAC_TIMING;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TimingINVOUTCTR */
      case 04737102310:
        {static const char *text="Invalid output mode selected";
        static const char *msgnam="INVOUTCTR";
        *fac_out = FAC_TIMING;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TimingINVPSEUDODEV */
      case 04737102320:
        {static const char *text="Invalid pseudo device attached to this decoder channel";
        static const char *msgnam="INVPSEUDODEV";
        *fac_out = FAC_TIMING;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TimingINVTRGMOD */
      case 04737102330:
        {static const char *text="Invalid trigger mode selected";
        static const char *msgnam="INVTRGMOD";
        *fac_out = FAC_TIMING;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TimingNOPSEUDODEV */
      case 04737102340:
        {static const char *text="No Pseudo device attached to this channel ... disabling";
        static const char *msgnam="NOPSEUDODEV";
        *fac_out = FAC_TIMING;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TimingTOO_MANY_EVENTS */
      case 04737102350:
        {static const char *text="More than 16 events used by this decoder";
        static const char *msgnam="TOO_MANY_EVENTS";
        *fac_out = FAC_TIMING;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* B2408NORMAL */
      case 04737103110:
        {static const char *text="successful completion";
        static const char *msgnam="NORMAL";
        *fac_out = FAC_B2408;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* B2408OVERFLOW */
      case 04737103120:
        {static const char *text="Triggers received after overflow";
        static const char *msgnam="OVERFLOW";
        *fac_out = FAC_B2408;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* B2408TRIG_LIM */
      case 04737103130:
        {static const char *text="Trigger limit possibly exceeded";
        static const char *msgnam="TRIG_LIM";
        *fac_out = FAC_B2408;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* FeraNORMAL */
      case 04737103730:
        {static const char *text="successful completion";
        static const char *msgnam="NORMAL";
        *fac_out = FAC_FERA;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* FeraDIGNOTSTRARRAY */
      case 04737103740:
        {static const char *text="The digitizer names must be an array of strings";
        static const char *msgnam="DIGNOTSTRARRAY";
        *fac_out = FAC_FERA;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* FeraNODIG */
      case 04737103750:
        {static const char *text="The digitizer names must be specified";
        static const char *msgnam="NODIG";
        *fac_out = FAC_FERA;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* FeraMEMNOTSTRARRAY */
      case 04737103760:
        {static const char *text="The memory names must be an array of strings";
        static const char *msgnam="MEMNOTSTRARRAY";
        *fac_out = FAC_FERA;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* FeraNOMEM */
      case 04737103770:
        {static const char *text="The memory names must be specified";
        static const char *msgnam="NOMEM";
        *fac_out = FAC_FERA;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* FeraPHASE_LOST */
      case 04737104000:
        {static const char *text="Data phase lost No FERA data stored";
        static const char *msgnam="PHASE_LOST";
        *fac_out = FAC_FERA;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* FeraCONFUSED */
      case 04737104010:
        {static const char *text="Fera Data inconsitant.  Data for this point zered.";
        static const char *msgnam="CONFUSED";
        *fac_out = FAC_FERA;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* FeraOVER_RUN */
      case 04737104020:
        {static const char *text="Possible FERA memory overrun, too many triggers.";
        static const char *msgnam="OVER_RUN";
        *fac_out = FAC_FERA;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* FeraOVERFLOW */
      case 04737104030:
        {static const char *text="Possible FERA data saturated.  Data point zeroed";
        static const char *msgnam="OVERFLOW";
        *fac_out = FAC_FERA;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* Hm650NORMAL */
      case 04737104550:
        {static const char *text="successful completion";
        static const char *msgnam="NORMAL";
        *fac_out = FAC_HM650;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* Hm650DLYCHNG */
      case 04737104560:
        {static const char *text="HM650 requested delay can not be processed by hardware.";
        static const char *msgnam="DLYCHNG";
        *fac_out = FAC_HM650;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* Hv4032NORMAL */
      case 04737105370:
        {static const char *text="successful completion";
        static const char *msgnam="NORMAL";
        *fac_out = FAC_HV4032;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* Hv4032WRONG_POD_TYPE */
      case 04737105400:
        {static const char *text="HV40321A n and p can only be used with the HV4032 device";
        static const char *msgnam="WRONG_POD_TYPE";
        *fac_out = FAC_HV4032;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* Hv1440NORMAL */
      case 04737106210:
        {static const char *text="successful completion";
        static const char *msgnam="NORMAL";
        *fac_out = FAC_HV1440;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* Hv1440WRONG_POD_TYPE */
      case 04737106220:
        {static const char *text="HV1443 can only be used with the HV1440 device";
        static const char *msgnam="WRONG_POD_TYPE";
        *fac_out = FAC_HV1440;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* Hv1440BAD_FRAME */
      case 04737106230:
        {static const char *text="HV1440 could not read the frame";
        static const char *msgnam="BAD_FRAME";
        *fac_out = FAC_HV1440;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* Hv1440BAD_RANGE */
      case 04737106240:
        {static const char *text="HV1440 could not read the range";
        static const char *msgnam="BAD_RANGE";
        *fac_out = FAC_HV1440;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* Hv1440OUTRNG */
      case 04737106250:
        {static const char *text="HV1440 out of range";
        static const char *msgnam="OUTRNG";
        *fac_out = FAC_HV1440;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* Hv1440STUCK */
      case 04737106260:
        {static const char *text="HV1440 not responding with Q";
        static const char *msgnam="STUCK";
        *fac_out = FAC_HV1440;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* JoergerBAD_PRE_TRIGGER */
      case 04737107030:
        {static const char *text="bad pretrigger specified, specify a value of 0,1,2,3,4,5,6 or 7";
        static const char *msgnam="BAD_PRE_TRIGGER";
        *fac_out = FAC_JOERGER;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* JoergerBAD_ACT_MEMORY */
      case 04737107040:
        {static const char *text="bad active memory specified, specify a value of 1,2,3,4,5,6,7 or 8";
        static const char *msgnam="BAD_ACT_MEMORY";
        *fac_out = FAC_JOERGER;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* JoergerBAD_GAIN */
      case 04737107050:
        {static const char *text="bad gain specified, specify a value of 1,2,4 or 8";
        static const char *msgnam="BAD_GAIN";
        *fac_out = FAC_JOERGER;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* U_of_mBAD_WAVE_LENGTH */
      case 04737107650:
        {static const char *text="bad wave length specified, specify value between 0 and 13000";
        static const char *msgnam="BAD_WAVE_LENGTH";
        *fac_out = FAC_U_OF_M;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* U_of_mBAD_SLIT_WIDTH */
      case 04737107660:
        {static const char *text="bad slit width specified, specify value between 0 and 500";
        static const char *msgnam="BAD_SLIT_WIDTH";
        *fac_out = FAC_U_OF_M;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* U_of_mBAD_NUM_SPECTRA */
      case 04737107670:
        {static const char *text="bad number of spectra specified, specify value between 1 and 100";
        static const char *msgnam="BAD_NUM_SPECTRA";
        *fac_out = FAC_U_OF_M;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* U_of_mBAD_GRATING */
      case 04737107700:
        {static const char *text="bad grating type specified, specify value between 1 and 5";
        static const char *msgnam="BAD_GRATING";
        *fac_out = FAC_U_OF_M;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* U_of_mBAD_EXPOSURE */
      case 04737107710:
        {static const char *text="bad exposure time specified, specify value between 30 and 3000";
        static const char *msgnam="BAD_EXPOSURE";
        *fac_out = FAC_U_OF_M;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* U_of_mBAD_FILTER */
      case 04737107720:
        {static const char *text="bad neutral density filter specified, specify value between 0 and 5";
        static const char *msgnam="BAD_FILTER";
        *fac_out = FAC_U_OF_M;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* U_of_mGO_FILE_ERROR */
      case 04737107730:
        {static const char *text="error creating new go file";
        static const char *msgnam="GO_FILE_ERROR";
        *fac_out = FAC_U_OF_M;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* U_of_mDATA_FILE_ERROR */
      case 04737107740:
        {static const char *text="error opening datafile";
        static const char *msgnam="DATA_FILE_ERROR";
        *fac_out = FAC_U_OF_M;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* IdlNORMAL */
      case 04737110470:
        {static const char *text="successful completion";
        static const char *msgnam="NORMAL";
        *fac_out = FAC_IDL;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* IdlERROR */
      case 04737110500:
        {static const char *text="IDL returned a non zero error code";
        static const char *msgnam="ERROR";
        *fac_out = FAC_IDL;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* B5910aBAD_CHAN */
      case 04737111310:
        {static const char *text="error evaluating data for channel !SL";
        static const char *msgnam="BAD_CHAN";
        *fac_out = FAC_B5910A;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* B5910aBAD_CLOCK */
      case 04737111320:
        {static const char *text="invalid internal clock range specified";
        static const char *msgnam="BAD_CLOCK";
        *fac_out = FAC_B5910A;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* B5910aBAD_ITERATIONS */
      case 04737111330:
        {static const char *text="invalid number of iterations specified";
        static const char *msgnam="BAD_ITERATIONS";
        *fac_out = FAC_B5910A;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* B5910aBAD_NOC */
      case 04737111340:
        {static const char *text="invalid number of active channels specified";
        static const char *msgnam="BAD_NOC";
        *fac_out = FAC_B5910A;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* B5910aBAD_SAMPS */
      case 04737111350:
        {static const char *text="number of samples specificed invalid";
        static const char *msgnam="BAD_SAMPS";
        *fac_out = FAC_B5910A;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* J412NOT_SORTED */
      case 04737112130:
        {static const char *text="times specified for J412 module were not sorted";
        static const char *msgnam="NOT_SORTED";
        *fac_out = FAC_J412;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* J412NO_DATA */
      case 04737112140:
        {static const char *text="there were no times specifed for J412 module";
        static const char *msgnam="NO_DATA";
        *fac_out = FAC_J412;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* J412BADCYCLES */
      case 04737112150:
        {static const char *text="The number of cycles must be 1 .. 255";
        static const char *msgnam="BADCYCLES";
        *fac_out = FAC_J412;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* Tr16NORMAL */
      case 04737112750:
        {static const char *text="successful completion";
        static const char *msgnam="NORMAL";
        *fac_out = FAC_TR16;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* Tr16BAD_MEMSIZE */
      case 04737112760:
        {static const char *text="Memory size must be in 128K, 256K, 512k, 1024K";
        static const char *msgnam="BAD_MEMSIZE";
        *fac_out = FAC_TR16;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* Tr16BAD_ACTIVEMEM */
      case 04737112770:
        {static const char *text="Active Mem must be power of 2 8K to 1024K";
        static const char *msgnam="BAD_ACTIVEMEM";
        *fac_out = FAC_TR16;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* Tr16BAD_ACTIVECHAN */
      case 04737113000:
        {static const char *text="Active channels must be in 1,2,4,8,16";
        static const char *msgnam="BAD_ACTIVECHAN";
        *fac_out = FAC_TR16;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* Tr16BAD_PTS */
      case 04737113010:
        {static const char *text="PTS must be power of 2 32 to 1024K";
        static const char *msgnam="BAD_PTS";
        *fac_out = FAC_TR16;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* Tr16BAD_FREQUENCY */
      case 04737113020:
        {static const char *text="Invalid clock frequency";
        static const char *msgnam="BAD_FREQUENCY";
        *fac_out = FAC_TR16;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* Tr16BAD_MASTER */
      case 04737113030:
        {static const char *text="Master must be 0 or 1";
        static const char *msgnam="BAD_MASTER";
        *fac_out = FAC_TR16;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* Tr16BAD_GAIN */
      case 04737113040:
        {static const char *text="Gain must be 1, 2, 4, or 8";
        static const char *msgnam="BAD_GAIN";
        *fac_out = FAC_TR16;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* A14NORMAL */
      case 04737113570:
        {static const char *text="successful completion";
        static const char *msgnam="NORMAL";
        *fac_out = FAC_A14;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* A14BAD_CLK_DIVIDE */
      case 04737113600:
        {static const char *text="Clock divide must be one of 1,2,4,10,20,40, or 100";
        static const char *msgnam="BAD_CLK_DIVIDE";
        *fac_out = FAC_A14;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* A14BAD_MODE */
      case 04737113610:
        {static const char *text="Mode must be in the range of 0 to 4";
        static const char *msgnam="BAD_MODE";
        *fac_out = FAC_A14;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* A14BAD_CLK_POLARITY */
      case 04737113620:
        {static const char *text="Clock polarity must be either 0 (rising) or 1 (falling)";
        static const char *msgnam="BAD_CLK_POLARITY";
        *fac_out = FAC_A14;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* A14BAD_STR_POLARITY */
      case 04737113630:
        {static const char *text="Start polarity must be either 0 (rising) or 1 (falling)";
        static const char *msgnam="BAD_STR_POLARITY";
        *fac_out = FAC_A14;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* A14BAD_STP_POLARITY */
      case 04737113640:
        {static const char *text="Stop polarity must be either 0 (rising) or 1 (falling)";
        static const char *msgnam="BAD_STP_POLARITY";
        *fac_out = FAC_A14;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* A14BAD_GATED */
      case 04737113650:
        {static const char *text="Gated clock must be either 0 (not gated) or 1 (gated)";
        static const char *msgnam="BAD_GATED";
        *fac_out = FAC_A14;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* L6810NORMAL */
      case 04737114410:
        {static const char *text="successful completion";
        static const char *msgnam="NORMAL";
        *fac_out = FAC_L6810;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* L6810BAD_ACTIVECHAN */
      case 04737114420:
        {static const char *text="Active chans must be 1, 2, or 4";
        static const char *msgnam="BAD_ACTIVECHAN";
        *fac_out = FAC_L6810;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* L6810BAD_ACTIVEMEM */
      case 04737114430:
        {static const char *text="Active memory must be power of 2 LE 8192";
        static const char *msgnam="BAD_ACTIVEMEM";
        *fac_out = FAC_L6810;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* L6810BAD_FREQUENCY */
      case 04737114440:
        {static const char *text="Frequency must be in [0, .02, .05, .1, .2, .5, 1, 2, 5, 10, 20, 50, 100,  200, 500, 1000, 2000, 5000]";
        static const char *msgnam="BAD_FREQUENCY";
        *fac_out = FAC_L6810;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* L6810BAD_FULL_SCALE */
      case 04737114450:
        {static const char *text="Full Scale must be in [.4096, 1.024, 2.048, 4.096, 10.24, 25.6, 51.2, 102.4]";
        static const char *msgnam="BAD_FULL_SCALE";
        *fac_out = FAC_L6810;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* L6810BAD_MEMORIES */
      case 04737114460:
        {static const char *text="Memories must 1 .. 16";
        static const char *msgnam="BAD_MEMORIES";
        *fac_out = FAC_L6810;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* L6810BAD_COUPLING */
      case 04737114470:
        {static const char *text="Channel source / coupling must be one of 0 .. 7";
        static const char *msgnam="BAD_COUPLING";
        *fac_out = FAC_L6810;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* L6810BAD_OFFSET */
      case 04737114500:
        {static const char *text="Offset must be between 0 and 255";
        static const char *msgnam="BAD_OFFSET";
        *fac_out = FAC_L6810;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* L6810BAD_SEGMENTS */
      case 04737114510:
        {static const char *text="Number of segments must be 1 .. 1024";
        static const char *msgnam="BAD_SEGMENTS";
        *fac_out = FAC_L6810;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* L6810BAD_TRIG_DELAY */
      case 04737114520:
        {static const char *text="Trigger delay must be between -8 and 247 in units of 8ths of segment size";
        static const char *msgnam="BAD_TRIG_DELAY";
        *fac_out = FAC_L6810;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* J_dacOUTRNG */
      case 04737115230:
        {static const char *text="Joerger DAC Channels out of range.  Bad chans code !XW";
        static const char *msgnam="OUTRNG";
        *fac_out = FAC_J_DAC;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* IncaaBAD_ACTIVE_CHANS */
      case 04737116050:
        {static const char *text="bad active channels selection";
        static const char *msgnam="BAD_ACTIVE_CHANS";
        *fac_out = FAC_INCAA;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* IncaaBAD_MASTER */
      case 04737116060:
        {static const char *text="bad master selection, must be 0 or 1";
        static const char *msgnam="BAD_MASTER";
        *fac_out = FAC_INCAA;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* IncaaBAD_EXT_1MHZ */
      case 04737116070:
        {static const char *text="bad ext 1mhz selection, must be 0 or 1";
        static const char *msgnam="BAD_EXT_1MHZ";
        *fac_out = FAC_INCAA;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* IncaaBAD_PTSC */
      case 04737116100:
        {static const char *text="bad PTSC setting";
        static const char *msgnam="BAD_PTSC";
        *fac_out = FAC_INCAA;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* L8212BAD_HEADER */
      case 04737116670:
        {static const char *text="Invalid header jumper information (e.g. 49414944432)";
        static const char *msgnam="BAD_HEADER";
        *fac_out = FAC_L8212;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* L8212BAD_MEMORIES */
      case 04737116700:
        {static const char *text="Invalid number of memories, must be 1 .. 16";
        static const char *msgnam="BAD_MEMORIES";
        *fac_out = FAC_L8212;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* L8212BAD_NOC */
      case 04737116710:
        {static const char *text="Invalid number of active channels";
        static const char *msgnam="BAD_NOC";
        *fac_out = FAC_L8212;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* L8212BAD_OFFSET */
      case 04737116720:
        {static const char *text="Invalid offset must be one of (0, -2048, -4096)";
        static const char *msgnam="BAD_OFFSET";
        *fac_out = FAC_L8212;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* L8212BAD_PTS */
      case 04737116730:
        {static const char *text="Invalid pts code, must be 0 .. 7";
        static const char *msgnam="BAD_PTS";
        *fac_out = FAC_L8212;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* L8212FREQ_TO_HIGH */
      case 04737116740:
        {static const char *text="Frequency to high for selected number of channels";
        static const char *msgnam="FREQ_TO_HIGH";
        *fac_out = FAC_L8212;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* L8212INVALID_NOC */
      case 04737116750:
        {static const char *text="Invalid number of active channels";
        static const char *msgnam="INVALID_NOC";
        *fac_out = FAC_L8212;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* MpbBADTIME */
      case 04737117510:
        {static const char *text="Could not read time";
        static const char *msgnam="BADTIME";
        *fac_out = FAC_MPB;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* MpbBADFREQ */
      case 04737117520:
        {static const char *text="Could not read frequency";
        static const char *msgnam="BADFREQ";
        *fac_out = FAC_MPB;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* L8828BAD_OFFSET */
      case 04737120330:
        {static const char *text="Offset for L8828 must be between 0 and 255";
        static const char *msgnam="BAD_OFFSET";
        *fac_out = FAC_L8828;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* L8828BAD_PRETRIG */
      case 04737120340:
        {static const char *text="Pre trigger samples for L8828 must be betwwen 0 and 7 eighths";
        static const char *msgnam="BAD_PRETRIG";
        *fac_out = FAC_L8828;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* L8828BAD_ACTIVEMEM */
      case 04737120350:
        {static const char *text="ACTIVEMEM must be beteen 16K and 2M";
        static const char *msgnam="BAD_ACTIVEMEM";
        *fac_out = FAC_L8828;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* L8828BAD_CLOCK */
      case 04737120360:
        {static const char *text="Invalid clock frequency specified.";
        static const char *msgnam="BAD_CLOCK";
        *fac_out = FAC_L8828;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* L8818BAD_OFFSET */
      case 04737121150:
        {static const char *text="Offset for L8828 must be between 0 and 255";
        static const char *msgnam="BAD_OFFSET";
        *fac_out = FAC_L8818;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* L8818BAD_PRETRIG */
      case 04737121160:
        {static const char *text="Pre trigger samples for L8828 must be betwwen 0 and 7 eighths";
        static const char *msgnam="BAD_PRETRIG";
        *fac_out = FAC_L8818;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* L8818BAD_ACTIVEMEM */
      case 04737121170:
        {static const char *text="ACTIVEMEM must be beteen 16K and 2M";
        static const char *msgnam="BAD_ACTIVEMEM";
        *fac_out = FAC_L8818;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* L8818BAD_CLOCK */
      case 04737121200:
        {static const char *text="Invalid clock frequency specified.";
        static const char *msgnam="BAD_CLOCK";
        *fac_out = FAC_L8818;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* J_tr612BAD_ACTMEM */
      case 04737121270:
        {static const char *text="ACTMEM value out of range, must be 0-7 where 0=1/8th and 7 = all";
        static const char *msgnam="BAD_ACTMEM";
        *fac_out = FAC_J_TR612;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* J_tr612BAD_PRETRIG */
      case 04737121300:
        {static const char *text="PRETRIG value out of range, must be 0-7 where 0 = none and 7 = 7/8 pretrigger samples";
        static const char *msgnam="BAD_PRETRIG";
        *fac_out = FAC_J_TR612;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* J_tr612BAD_MODE */
      case 04737121310:
        {static const char *text="MODE value out of range, must be 0 (for normal) or 1 (for burst mode)";
        static const char *msgnam="BAD_MODE";
        *fac_out = FAC_J_TR612;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* J_tr612BAD_FREQUENCY */
      case 04737121320:
        {static const char *text="FREQUENCY value out of range, must be 0-4 where 0=3MHz,1=2MHz,2=1MHz,3=100KHz,4=external";
        static const char *msgnam="BAD_FREQUENCY";
        *fac_out = FAC_J_TR612;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* L8206NODATA */
      case 04737121770:
        {static const char *text="no data has been written to memory";
        static const char *msgnam="NODATA";
        *fac_out = FAC_L8206;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* H912BAD_CLOCK */
      case 04737122110:
        {static const char *text="Bad value specified in INT_CLOCK node, use Setup device to correct";
        static const char *msgnam="BAD_CLOCK";
        *fac_out = FAC_H912;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* H912BAD_BLOCKS */
      case 04737122120:
        {static const char *text="Bad value specified in BLOCKS node, use Setup device to correct";
        static const char *msgnam="BAD_BLOCKS";
        *fac_out = FAC_H912;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* H912BAD_PTS */
      case 04737122130:
        {static const char *text="Bad value specfiied in PTS node, must be an integer value between 1 and 131071";
        static const char *msgnam="BAD_PTS";
        *fac_out = FAC_H912;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* H908BAD_CLOCK */
      case 04737122230:
        {static const char *text="Bad value specified in INT_CLOCK node, use Setup device to correct";
        static const char *msgnam="BAD_CLOCK";
        *fac_out = FAC_H908;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* H908BAD_ACTIVE_CHANS */
      case 04737122240:
        {static const char *text="Bad value specified in ACTIVE_CHANS node, use Setup device to correct";
        static const char *msgnam="BAD_ACTIVE_CHANS";
        *fac_out = FAC_H908;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* H908BAD_PTS */
      case 04737122250:
        {static const char *text="Bad value specfiied in PTS node, must be an integer value between 1 and 131071";
        static const char *msgnam="BAD_PTS";
        *fac_out = FAC_H908;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* Dsp2904CHANNEL_READ_ERROR */
      case 04737122350:
        {static const char *text="Error reading channel";
        static const char *msgnam="CHANNEL_READ_ERROR";
        *fac_out = FAC_DSP2904;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* PyUNHANDLED_EXCEPTION */
      case 04737122470:
        {static const char *text="Python device raised an exception, see log files for more details";
        static const char *msgnam="UNHANDLED_EXCEPTION";
        *fac_out = FAC_PY;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* Dt196bNO_SAMPLES */
      case 04737122610:
        {static const char *text="Module did not acquire any samples";
        static const char *msgnam="NO_SAMPLES";
        *fac_out = FAC_DT196B;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevCANNOT_LOAD_SETTINGS */
      case 04737122640:
        {static const char *text="Error loading settings from XML";
        static const char *msgnam="CANNOT_LOAD_SETTINGS";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevCANNOT_GET_BOARD_STATE */
      case 04737122650:
        {static const char *text="Cannot retrieve state of daq board";
        static const char *msgnam="CANNOT_GET_BOARD_STATE";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevACQCMD_FAILED */
      case 04737122660:
        {static const char *text="Error executing acqcmd on daq board";
        static const char *msgnam="ACQCMD_FAILED";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevACQ2SH_FAILED */
      case 04737122670:
        {static const char *text="Error executing acq2sh command on daq board";
        static const char *msgnam="ACQ2SH_FAILED";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevBAD_PARAMETER */
      case 04737122700:
        {static const char *text="Invalid parameter specified for device";
        static const char *msgnam="BAD_PARAMETER";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevCOMM_ERROR */
      case 04737122710:
        {static const char *text="Error communicating with device";
        static const char *msgnam="COMM_ERROR";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevCAMERA_NOT_FOUND */
      case 04737122720:
        {static const char *text="Could not find specified camera on the network";
        static const char *msgnam="CAMERA_NOT_FOUND";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* DevNOT_A_PYDEVICE */
      case 04737122730:
        {static const char *text="Device is not a python device.";
        static const char *msgnam="NOT_A_PYDEVICE";
        *fac_out = FAC_DEV;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeALREADY_OFF */
      case 01764300050:
        {static const char *text="Node is already OFF";
        static const char *msgnam="ALREADY_OFF";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeALREADY_ON */
      case 01764300060:
        {static const char *text="Node is already ON";
        static const char *msgnam="ALREADY_ON";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeALREADY_OPEN */
      case 01764300070:
        {static const char *text="Tree is already OPEN";
        static const char *msgnam="ALREADY_OPEN";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeALREADY_THERE */
      case 01764300210:
        {static const char *text="Node is already in the tree";
        static const char *msgnam="ALREADY_THERE";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeBADRECORD */
      case 01764300270:
        {static const char *text="Data corrupted: cannot read record";
        static const char *msgnam="BADRECORD";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeBOTH_OFF */
      case 01764300230:
        {static const char *text="Both this node and its parent are off";
        static const char *msgnam="BOTH_OFF";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeBUFFEROVF */
      case 01764300420:
        {static const char *text="Output buffer overflow";
        static const char *msgnam="BUFFEROVF";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeCONGLOMFULL */
      case 01764300440:
        {static const char *text="Current conglomerate is full";
        static const char *msgnam="CONGLOMFULL";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeCONGLOM_NOT_FULL */
      case 01764300450:
        {static const char *text="Current conglomerate is not yet full";
        static const char *msgnam="CONGLOM_NOT_FULL";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeCONTINUING */
      case 01764304540:
        {static const char *text="Operation continuing: note following error";
        static const char *msgnam="CONTINUING";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeDUPTAG */
      case 01764300310:
        {static const char *text="Tag name already in use";
        static const char *msgnam="DUPTAG";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeEDITTING */
      case 01764300620:
        {static const char *text="Tree file open for edit: operation not permitted";
        static const char *msgnam="EDITTING";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeILLEGAL_ITEM */
      case 01764300410:
        {static const char *text="Invalid item code or part number specified";
        static const char *msgnam="ILLEGAL_ITEM";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeILLPAGCNT */
      case 01764300320:
        {static const char *text="Illegal page count, error mapping tree file";
        static const char *msgnam="ILLPAGCNT";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeINVDFFCLASS */
      case 01764300470:
        {static const char *text="Invalid data fmt: only CLASS_S can have data in NCI";
        static const char *msgnam="INVDFFCLASS";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeINVDTPUSG */
      case 01764300610:
        {static const char *text="Attempt to store datatype which conflicts with the designated usage of this node";
        static const char *msgnam="INVDTPUSG";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeINVPATH */
      case 01764300400:
        {static const char *text="Invalid tree pathname specified";
        static const char *msgnam="INVPATH";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeINVRECTYP */
      case 01764300500:
        {static const char *text="Record type invalid for requested operation";
        static const char *msgnam="INVRECTYP";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeINVTREE */
      case 01764300300:
        {static const char *text="Invalid tree identification structure";
        static const char *msgnam="INVTREE";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeMAXOPENEDIT */
      case 01764300330:
        {static const char *text="Too many files open for edit";
        static const char *msgnam="MAXOPENEDIT";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNEW */
      case 01764300030:
        {static const char *text="New tree created";
        static const char *msgnam="NEW";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNMN */
      case 01764300140:
        {static const char *text="No More Nodes";
        static const char *msgnam="NMN";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNMT */
      case 01764300150:
        {static const char *text="No More Tags";
        static const char *msgnam="NMT";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNNF */
      case 01764300160:
        {static const char *text="Node Not Found";
        static const char *msgnam="NNF";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNODATA */
      case 01764300340:
        {static const char *text="No data available for this node";
        static const char *msgnam="NODATA";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNODNAMLEN */
      case 01764300510:
        {static const char *text="Node name too long (12 chars max)";
        static const char *msgnam="NODNAMLEN";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNOEDIT */
      case 01764300360:
        {static const char *text="Tree file is not open for edit";
        static const char *msgnam="NOEDIT";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNOLOG */
      case 01764300650:
        {static const char *text="Experiment pathname (xxx_path) not defined";
        static const char *msgnam="NOLOG";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNOMETHOD */
      case 01764300260:
        {static const char *text="Method not available for this object";
        static const char *msgnam="NOMETHOD";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNOOVERWRITE */
      case 01764300600:
        {static const char *text="Write-once node: overwrite not permitted";
        static const char *msgnam="NOOVERWRITE";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNORMAL */
      case 01764300010:
        {static const char *text="Normal successful completion (deprecated: use TreeSUCCESS)";
        static const char *msgnam="NORMAL";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNOTALLSUBS */
      case 01764300040:
        {static const char *text="Main tree opened but not all subtrees found/or connected";
        static const char *msgnam="NOTALLSUBS";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNOTCHILDLESS */
      case 01764300370:
        {static const char *text="Node must be childless to become subtree reference";
        static const char *msgnam="NOTCHILDLESS";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNOT_IN_LIST */
      case 01764300700:
        {static const char *text="Tree being opened was not in the list";
        static const char *msgnam="NOT_IN_LIST";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNOTMEMBERLESS */
      case 01764300560:
        {static const char *text="Subtree reference can not have members";
        static const char *msgnam="NOTMEMBERLESS";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNOTOPEN */
      case 01764300350:
        {static const char *text="No tree file currently open";
        static const char *msgnam="NOTOPEN";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNOTSON */
      case 01764300570:
        {static const char *text="Subtree reference cannot be a member";
        static const char *msgnam="NOTSON";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNOT_CONGLOM */
      case 01764300540:
        {static const char *text="Head node of conglomerate does not contain a DTYPE_CONGLOM record";
        static const char *msgnam="NOT_CONGLOM";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNOT_OPEN */
      case 01764300250:
        {static const char *text="Tree not currently open";
        static const char *msgnam="NOT_OPEN";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNOWRITEMODEL */
      case 01764300630:
        {static const char *text="Data for this node can not be written into the MODEL file";
        static const char *msgnam="NOWRITEMODEL";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNOWRITESHOT */
      case 01764300640:
        {static const char *text="Data for this node can not be written into the SHOT file";
        static const char *msgnam="NOWRITESHOT";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNO_CONTEXT */
      case 01764300100:
        {static const char *text="There is no active search to end";
        static const char *msgnam="NO_CONTEXT";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeOFF */
      case 01764300240:
        {static const char *text="Node is OFF";
        static const char *msgnam="OFF";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeON */
      case 01764300110:
        {static const char *text="Node is ON";
        static const char *msgnam="ON";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeOPEN */
      case 01764300120:
        {static const char *text="Tree is OPEN (no edit)";
        static const char *msgnam="OPEN";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeOPEN_EDIT */
      case 01764300130:
        {static const char *text="Tree is OPEN for edit";
        static const char *msgnam="OPEN_EDIT";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreePARENT_OFF */
      case 01764300220:
        {static const char *text="Parent of this node is OFF";
        static const char *msgnam="PARENT_OFF";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeREADERR */
      case 01764300670:
        {static const char *text="Error reading record for node";
        static const char *msgnam="READERR";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeREADONLY */
      case 01764300660:
        {static const char *text="Tree was opened with readonly access";
        static const char *msgnam="READONLY";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeRESOLVED */
      case 01764300020:
        {static const char *text="Indirect reference successfully resolved";
        static const char *msgnam="RESOLVED";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeSUCCESS */
      case 01764303100:
        {static const char *text="Operation successful";
        static const char *msgnam="SUCCESS";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeTAGNAMLEN */
      case 01764300520:
        {static const char *text="Tagname too long (max 24 chars)";
        static const char *msgnam="TAGNAMLEN";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeTNF */
      case 01764300170:
        {static const char *text="Tag Not Found";
        static const char *msgnam="TNF";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeTREENF */
      case 01764300200:
        {static const char *text="Tree Not Found";
        static const char *msgnam="TREENF";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeUNRESOLVED */
      case 01764300460:
        {static const char *text="Not an indirect node reference: No action taken";
        static const char *msgnam="UNRESOLVED";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeUNSPRTCLASS */
      case 01764300430:
        {static const char *text="Unsupported descriptor class";
        static const char *msgnam="UNSPRTCLASS";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeUNSUPARRDTYPE */
      case 01764300550:
        {static const char *text="Complex data types not supported as members of arrays";
        static const char *msgnam="UNSUPARRDTYPE";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeWRITEFIRST */
      case 01764300530:
        {static const char *text="Tree has been modified:  write or quit first";
        static const char *msgnam="WRITEFIRST";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeFAILURE */
      case 01764307640:
        {static const char *text="Operation NOT successful";
        static const char *msgnam="FAILURE";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeLOCK_FAILURE */
      case 01764307660:
        {static const char *text="Error locking file, perhaps NFSLOCKING not enabled on this system";
        static const char *msgnam="LOCK_FAILURE";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeFILE_NOT_FOUND */
      case 01764307650:
        {static const char *text="File or Directory Not Found";
        static const char *msgnam="FILE_NOT_FOUND";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeCANCEL */
      case 01764306200:
        {static const char *text="User canceled operation";
        static const char *msgnam="CANCEL";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeUNSUPTHICKOP */
      case 01764306210:
        {static const char *text="Unsupported thick client operation";
        static const char *msgnam="UNSUPTHICKOP";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNOSEGMENTS */
      case 01764307670:
        {static const char *text="No segments exist in this node";
        static const char *msgnam="NOSEGMENTS";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeINVDTYPE */
      case 01764307700:
        {static const char *text="Invalid datatype for data segment";
        static const char *msgnam="INVDTYPE";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeINVSHAPE */
      case 01764307710:
        {static const char *text="Invalid shape for this data segment";
        static const char *msgnam="INVSHAPE";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeINVSHOT */
      case 01764307730:
        {static const char *text="Invalid shot number - must be -1 (model), 0 (current), or Positive";
        static const char *msgnam="INVSHOT";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeINVTAG */
      case 01764307750:
        {static const char *text="Invalid tagname - must begin with alpha followed by 0-22 alphanumeric or underscores";
        static const char *msgnam="INVTAG";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNOPATH */
      case 01764307760:
        {static const char *text="No 'treename'_path or default_tree_path environment variables defined. Cannot locate tree files.";
        static const char *msgnam="NOPATH";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeTREEFILEREADERR */
      case 01764307770:
        {static const char *text="Error reading in tree file contents.";
        static const char *msgnam="TREEFILEREADERR";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeMEMERR */
      case 01764310000:
        {static const char *text="Memory allocation error.";
        static const char *msgnam="MEMERR";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNOCURRENT */
      case 01764310010:
        {static const char *text="No current shot number set for this tree.";
        static const char *msgnam="NOCURRENT";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeFOPENW */
      case 01764310020:
        {static const char *text="Error opening file for read-write.";
        static const char *msgnam="FOPENW";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeFOPENR */
      case 01764310030:
        {static const char *text="Error opening file read-only.";
        static const char *msgnam="FOPENR";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeFCREATE */
      case 01764310040:
        {static const char *text="Error creating new file.";
        static const char *msgnam="FCREATE";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeCONNECTFAIL */
      case 01764310050:
        {static const char *text="Error connecting to remote server.";
        static const char *msgnam="CONNECTFAIL";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNCIWRITE */
      case 01764310060:
        {static const char *text="Error writing node characterisitics to file.";
        static const char *msgnam="NCIWRITE";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeDELFAIL */
      case 01764310070:
        {static const char *text="Error deleting file.";
        static const char *msgnam="DELFAIL";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeRENFAIL */
      case 01764310100:
        {static const char *text="Error renaming file.";
        static const char *msgnam="RENFAIL";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeEMPTY */
      case 01764310110:
        {static const char *text="Empty string provided.";
        static const char *msgnam="EMPTY";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreePARSEERR */
      case 01764310120:
        {static const char *text="Invalid node search string.";
        static const char *msgnam="PARSEERR";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNCIREAD */
      case 01764310130:
        {static const char *text="Error reading node characteristics from file.";
        static const char *msgnam="NCIREAD";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNOVERSION */
      case 01764310140:
        {static const char *text="No version available.";
        static const char *msgnam="NOVERSION";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeDFREAD */
      case 01764310150:
        {static const char *text="Error reading from datafile.";
        static const char *msgnam="DFREAD";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeCLOSEERR */
      case 01764310160:
        {static const char *text="Error closing temporary tree file.";
        static const char *msgnam="CLOSEERR";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeMOVEERROR */
      case 01764310170:
        {static const char *text="Error replacing original treefile with new one.";
        static const char *msgnam="MOVEERROR";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeOPENEDITERR */
      case 01764310200:
        {static const char *text="Error reopening new treefile for write access.";
        static const char *msgnam="OPENEDITERR";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeREADONLY_TREE */
      case 01764310210:
        {static const char *text="Tree is marked as readonly. No write operations permitted.";
        static const char *msgnam="READONLY_TREE";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeWRITETREEERR */
      case 01764310220:
        {static const char *text="Error writing .tree file";
        static const char *msgnam="WRITETREEERR";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TreeNOWILD */
      case 01764310230:
        {static const char *text="No wildcard characters permitted in node specifier";
        static const char *msgnam="NOWILD";
        *fac_out = FAC_TREE;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* LibINSVIRMEM */
      case 05301020:
        {static const char *text="Insufficient virtual memory";
        static const char *msgnam="INSVIRMEM";
        *fac_out = FAC_LIB;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* LibINVARG */
      case 05301060:
        {static const char *text="Invalid argument";
        static const char *msgnam="INVARG";
        *fac_out = FAC_LIB;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* LibINVSTRDES */
      case 05301040:
        {static const char *text="Invalid string descriptor";
        static const char *msgnam="INVSTRDES";
        *fac_out = FAC_LIB;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* LibKEYNOTFOU */
      case 05301370:
        {static const char *text="Key not found";
        static const char *msgnam="KEYNOTFOU";
        *fac_out = FAC_LIB;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* LibNOTFOU */
      case 05301160:
        {static const char *text="Entity not found";
        static const char *msgnam="NOTFOU";
        *fac_out = FAC_LIB;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* LibQUEWASEMP */
      case 05301350:
        {static const char *text="Queue was empty";
        static const char *msgnam="QUEWASEMP";
        *fac_out = FAC_LIB;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* LibSTRTRU */
      case 05300020:
        {static const char *text="String truncated";
        static const char *msgnam="STRTRU";
        *fac_out = FAC_LIB;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* StrMATCH */
      case 011102030:
        {static const char *text="Strings match";
        static const char *msgnam="MATCH";
        *fac_out = FAC_STR;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* StrNOMATCH */
      case 011101010:
        {static const char *text="Strings do not match";
        static const char *msgnam="NOMATCH";
        *fac_out = FAC_STR;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* StrNOELEM */
      case 011101030:
        {static const char *text="Not enough delimited characters";
        static const char *msgnam="NOELEM";
        *fac_out = FAC_STR;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* StrINVDELIM */
      case 011101020:
        {static const char *text="Not enough delimited characters";
        static const char *msgnam="INVDELIM";
        *fac_out = FAC_STR;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* StrSTRTOOLON */
      case 011100160:
        {static const char *text="String too long";
        static const char *msgnam="STRTOOLON";
        *fac_out = FAC_STR;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* MDSplusWARNING */
      case 0200000:
        {static const char *text="Warning";
        static const char *msgnam="WARNING";
        *fac_out = FAC_MDSPLUS;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* MDSplusSUCCESS */
      case 0200010:
        {static const char *text="Success";
        static const char *msgnam="SUCCESS";
        *fac_out = FAC_MDSPLUS;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* MDSplusERROR */
      case 0200020:
        {static const char *text="Error";
        static const char *msgnam="ERROR";
        *fac_out = FAC_MDSPLUS;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* MDSplusFATAL */
      case 0200040:
        {static const char *text="Fatal";
        static const char *msgnam="FATAL";
        *fac_out = FAC_MDSPLUS;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* MDSplusSANDBOX */
      case 0200050:
        {static const char *text="Function disabled for security reasons";
        static const char *msgnam="SANDBOX";
        *fac_out = FAC_MDSPLUS;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* SsSUCCESS */
      case 00:
        {static const char *text="Success";
        static const char *msgnam="SUCCESS";
        *fac_out = FAC_SS;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* SsINTOVF */
      case 02170:
        {static const char *text="Integer overflow";
        static const char *msgnam="INTOVF";
        *fac_out = FAC_SS;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* SsINTERNAL */
      case 0-10:
        {static const char *text="This status is meant for internal use only, you should never have seen this message.";
        static const char *msgnam="INTERNAL";
        *fac_out = FAC_SS;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiBREAK */
      case 01764700010:
        {static const char *text="BREAK was not in DO FOR SWITCH or WHILE";
        static const char *msgnam="BREAK";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiCASE */
      case 01764700020:
        {static const char *text="CASE was not in SWITCH statement";
        static const char *msgnam="CASE";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiCONTINUE */
      case 01764700030:
        {static const char *text="CONTINUE was not in DO FOR or WHILE";
        static const char *msgnam="CONTINUE";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiEXTRANEOUS */
      case 01764700040:
        {static const char *text="Some characters were unused, bad number maybe";
        static const char *msgnam="EXTRANEOUS";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiRETURN */
      case 01764700050:
        {static const char *text="Extraneous RETURN statement, not from a FUN";
        static const char *msgnam="RETURN";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiABORT */
      case 01764700060:
        {static const char *text="Program requested abort";
        static const char *msgnam="ABORT";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiBAD_INDEX */
      case 01764700070:
        {static const char *text="Index or subscript is too small or too big";
        static const char *msgnam="BAD_INDEX";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiBOMB */
      case 01764700100:
        {static const char *text="Bad punctuation, could not compile the text";
        static const char *msgnam="BOMB";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiEXTRA_ARG */
      case 01764700110:
        {static const char *text="Too many arguments for function, watch commas";
        static const char *msgnam="EXTRA_ARG";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiGOTO */
      case 01764700120:
        {static const char *text="GOTO target label not found";
        static const char *msgnam="GOTO";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiINVCLADSC */
      case 01764700130:
        {static const char *text="Storage class not valid, must be scalar or array";
        static const char *msgnam="INVCLADSC";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiINVCLADTY */
      case 01764700140:
        {static const char *text="Invalid mixture of storage class and data type";
        static const char *msgnam="INVCLADTY";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiINVDTYDSC */
      case 01764700150:
        {static const char *text="Storage data type is not valid";
        static const char *msgnam="INVDTYDSC";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiINV_OPC */
      case 01764700160:
        {static const char *text="Invalid operator code in a function";
        static const char *msgnam="INV_OPC";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiINV_SIZE */
      case 01764700170:
        {static const char *text="Number of elements does not match declaration";
        static const char *msgnam="INV_SIZE";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiMISMATCH */
      case 01764700200:
        {static const char *text="Shape of arguments does not match";
        static const char *msgnam="MISMATCH";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiMISS_ARG */
      case 01764700210:
        {static const char *text="Missing argument is required for function";
        static const char *msgnam="MISS_ARG";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiNDIM_OVER */
      case 01764700220:
        {static const char *text="Number of dimensions is over the allowed 8";
        static const char *msgnam="NDIM_OVER";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiNO_CMPLX */
      case 01764700230:
        {static const char *text="There are no complex forms of this function";
        static const char *msgnam="NO_CMPLX";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiNO_OPC */
      case 01764700240:
        {static const char *text="No support for this function, today";
        static const char *msgnam="NO_OPC";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiNO_OUTPTR */
      case 01764700250:
        {static const char *text="An output pointer is required";
        static const char *msgnam="NO_OUTPTR";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiNO_SELF_PTR */
      case 01764700260:
        {static const char *text="No $VALUE is defined for signal or validation";
        static const char *msgnam="NO_SELF_PTR";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiNOT_NUMBER */
      case 01764700270:
        {static const char *text="Value is not a scalar number and must be";
        static const char *msgnam="NOT_NUMBER";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiNULL_PTR */
      case 01764700300:
        {static const char *text="Null pointer where value needed";
        static const char *msgnam="NULL_PTR";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiRECURSIVE */
      case 01764700310:
        {static const char *text="Overly recursive function, calls itself maybe";
        static const char *msgnam="RECURSIVE";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiSIG_DIM */
      case 01764700320:
        {static const char *text="Signal dimension does not match data shape";
        static const char *msgnam="SIG_DIM";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiSYNTAX */
      case 01764700330:
        {static const char *text="Bad punctuation or misspelled word or number";
        static const char *msgnam="SYNTAX";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiTOO_BIG */
      case 01764700340:
        {static const char *text="Conversion of number lost significant digits";
        static const char *msgnam="TOO_BIG";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiUNBALANCE */
      case 01764700350:
        {static const char *text="Unbalanced () [] {} '' \" \" or /**/";
        static const char *msgnam="UNBALANCE";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiUNKNOWN_VAR */
      case 01764700360:
        {static const char *text="Unknown/undefined variable name";
        static const char *msgnam="UNKNOWN_VAR";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiSTRTOOLON */
      case 01764700370:
        {static const char *text="string is too long (greater than 65535)";
        static const char *msgnam="STRTOOLON";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TdiTIMEOUT */
      case 01764700400:
        {static const char *text="task did not complete in alotted time";
        static const char *msgnam="TIMEOUT";
        *fac_out = FAC_TDI;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* ApdAPD_APPEND */
      case 01767200010:
        {static const char *text="First argument must be APD or *";
        static const char *msgnam="APD_APPEND";
        *fac_out = FAC_APD;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* ApdDICT_KEYVALPAIR */
      case 01767200020:
        {static const char *text="A Dictionary requires an even number of elements";
        static const char *msgnam="DICT_KEYVALPAIR";
        *fac_out = FAC_APD;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* ApdDICT_KEYCLS */
      case 01767200030:
        {static const char *text="Keys must be scalar, i.e. CLASS_S";
        static const char *msgnam="DICT_KEYCLS";
        *fac_out = FAC_APD;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* MdsdclSUCCESS */
      case 01000400010:
        {static const char *text="Normal successful completion";
        static const char *msgnam="SUCCESS";
        *fac_out = FAC_MDSDCL;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* MdsdclEXIT */
      case 01000400020:
        {static const char *text="Normal exit";
        static const char *msgnam="EXIT";
        *fac_out = FAC_MDSDCL;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* MdsdclERROR */
      case 01000400030:
        {static const char *text="Unsuccessful execution of command";
        static const char *msgnam="ERROR";
        *fac_out = FAC_MDSDCL;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* MdsdclNORMAL */
      case 01000401450:
        {static const char *text="Normal successful completion";
        static const char *msgnam="NORMAL";
        *fac_out = FAC_MDSDCL;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* MdsdclPRESENT */
      case 01000401460:
        {static const char *text="Entity is present";
        static const char *msgnam="PRESENT";
        *fac_out = FAC_MDSDCL;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* MdsdclIVVERB */
      case 01000401470:
        {static const char *text="No such command";
        static const char *msgnam="IVVERB";
        *fac_out = FAC_MDSDCL;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* MdsdclABSENT */
      case 01000401500:
        {static const char *text="Entity is absent";
        static const char *msgnam="ABSENT";
        *fac_out = FAC_MDSDCL;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* MdsdclNEGATED */
      case 01000401510:
        {static const char *text="Entity is present but negated";
        static const char *msgnam="NEGATED";
        *fac_out = FAC_MDSDCL;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* MdsdclNOTNEGATABLE */
      case 01000401520:
        {static const char *text="Entity cannot be negated";
        static const char *msgnam="NOTNEGATABLE";
        *fac_out = FAC_MDSDCL;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* MdsdclIVQUAL */
      case 01000401530:
        {static const char *text="Invalid qualifier";
        static const char *msgnam="IVQUAL";
        *fac_out = FAC_MDSDCL;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* MdsdclPROMPT_MORE */
      case 01000401540:
        {static const char *text="More input required for command";
        static const char *msgnam="PROMPT_MORE";
        *fac_out = FAC_MDSDCL;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* MdsdclTOO_MANY_PRMS */
      case 01000401550:
        {static const char *text="Too many parameters specified";
        static const char *msgnam="TOO_MANY_PRMS";
        *fac_out = FAC_MDSDCL;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* MdsdclTOO_MANY_VALS */
      case 01000401560:
        {static const char *text="Too many values";
        static const char *msgnam="TOO_MANY_VALS";
        *fac_out = FAC_MDSDCL;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* MdsdclMISSING_VALUE */
      case 01000401570:
        {static const char *text="Qualifier value needed";
        static const char *msgnam="MISSING_VALUE";
        *fac_out = FAC_MDSDCL;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* ServerNOT_DISPATCHED */
      case 01770300010:
        {static const char *text="action not dispatched, depended on failed action";
        static const char *msgnam="NOT_DISPATCHED";
        *fac_out = FAC_SERVER;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* ServerINVALID_DEPENDENCY */
      case 01770300020:
        {static const char *text="action dependency cannot be evaluated";
        static const char *msgnam="INVALID_DEPENDENCY";
        *fac_out = FAC_SERVER;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* ServerCANT_HAPPEN */
      case 01770300030:
        {static const char *text="action contains circular dependency or depends on action which was not dispatched";
        static const char *msgnam="CANT_HAPPEN";
        *fac_out = FAC_SERVER;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* ServerINVSHOT */
      case 01770300040:
        {static const char *text="invalid shot number, cannot dispatch actions in model";
        static const char *msgnam="INVSHOT";
        *fac_out = FAC_SERVER;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* ServerABORT */
      case 01770300060:
        {static const char *text="Server action was aborted";
        static const char *msgnam="ABORT";
        *fac_out = FAC_SERVER;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* ServerPATH_DOWN */
      case 01770300100:
        {static const char *text="Path to server lost";
        static const char *msgnam="PATH_DOWN";
        *fac_out = FAC_SERVER;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* ServerSOCKET_ADDR_ERROR */
      case 01770300110:
        {static const char *text="Cannot obtain ip address socket is bound to.";
        static const char *msgnam="SOCKET_ADDR_ERROR";
        *fac_out = FAC_SERVER;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* ServerINVALID_ACTION_OPERATION */
      case 01770300120:
        {static const char *text="Unknown action operation.";
        static const char *msgnam="INVALID_ACTION_OPERATION";
        *fac_out = FAC_SERVER;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* CamDONE_Q */
      case 01000300010:
        {static const char *text="I/O completed with X=1, Q=1";
        static const char *msgnam="DONE_Q";
        *fac_out = FAC_CAM;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* CamDONE_NOQ */
      case 01000300020:
        {static const char *text="I/O completed with X=1, Q=0";
        static const char *msgnam="DONE_NOQ";
        *fac_out = FAC_CAM;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* CamDONE_NOX */
      case 01000310000:
        {static const char *text="I/O completed with X=0 - probable failure";
        static const char *msgnam="DONE_NOX";
        *fac_out = FAC_CAM;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* CamSERTRAERR */
      case 01000314000:
        {static const char *text="serial transmission error on highway";
        static const char *msgnam="SERTRAERR";
        *fac_out = FAC_CAM;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* CamSCCFAIL */
      case 01000314100:
        {static const char *text="serial crate controller failure";
        static const char *msgnam="SCCFAIL";
        *fac_out = FAC_CAM;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* CamOFFLINE */
      case 01000314150:
        {static const char *text="crate is offline";
        static const char *msgnam="OFFLINE";
        *fac_out = FAC_CAM;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TclNORMAL */
      case 012400010:
        {static const char *text="Normal successful completion";
        static const char *msgnam="NORMAL";
        *fac_out = FAC_TCL;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TclFAILED_ESSENTIAL */
      case 012400020:
        {static const char *text="Essential action failed";
        static const char *msgnam="FAILED_ESSENTIAL";
        *fac_out = FAC_TCL;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

/* TclNO_DISPATCH_TABLE */
      case 012400030:
        {static const char *text="No dispatch table found. Forgot to do DISPATCH/BUILD?";
        static const char *msgnam="NO_DISPATCH_TABLE";
        *fac_out = FAC_TCL;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;

    default: sts = 0;
  }
  if (sts == 0) {
    *fac_out=0;
    *msgnam_out=0;
    *text_out=0;
  }
  return sts;
}