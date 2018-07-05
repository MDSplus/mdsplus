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
#include <utility.h>
#include <ansi_c.h>

//#undef __NISCOPE_HEADER

//#include <ansi_c.h>
//#include <visatype.h>
//#include <stdio.h>
//#include <cvirte.h>
//#include <math.h>
//#include <winsock.h>
//#include "toolbox.h"
//#include <mdslib.h>
#include <niScope.h>
//#undef _VI_FUNC
//#define _VI_FUNC
//#include <ivi.h>
//#include <iviScope.h>
//#include "niScopeObsolete.h" 

// id = ih    같은 뜻이다. 여기서는 ih를 사용 했다.

/* Print the error message */
int NiScope_errorHandler(ViSession ih, ViInt32 code)
{
  ViStatus status;
  ViChar source[MAX_FUNCTION_NAME_SIZE];
  ViChar desc[MAX_ERROR_DESCRIPTION];
  status = niScope_errorHandler(ih, code, source, desc);
  printf("%s\n", desc);
  return status;
}

// NiScope_init  ->  NiScope_close
int NiScope_init(char *dev, ViSession * ih)
{
//      printf("%s\n", dev);
//      ViSession ih;
  ViStatus status;
  status = niScope_init(dev, 1, 1, ih);
  if (status != VI_SUCCESS)
    NiScope_errorHandler(VI_NULL, status);
  return status;
}

int NiScope_initiateacquisition(ViSession ih)
{
  ViStatus status;
  status = niScope_InitiateAcquisition(ih);
  if (status != VI_SUCCESS)
    NiScope_errorHandler(ih, status);
  return status;
}

int NiScope_acquisitionstatus(ViSession ih)
{
  ViStatus status;
  ViInt32 as;

  status = niScope_AcquisitionStatus(ih, &as);
  if (status != VI_SUCCESS)
    NiScope_errorHandler(ih, status);
  switch (as) {
  case NISCOPE_VAL_ACQ_IN_PROGRESS:
    printf("The acquisition is in progress.\n");
    break;
  case NISCOPE_VAL_ACQ_COMPLETE:
    printf("The acquisition is complete.\n");
    break;
  case NISCOPE_VAL_ACQ_STATUS_UNKNOWN:
    printf("The acquisition is in an unknown state.\n");
    break;
  }
  return status;
}

/* Device Setup */
int NiScope_configuretriggerimmediate(ViSession ih)
{
  ViStatus status;
  status = niScope_ConfigureTriggerImmediate(ih);
  if (status != VI_SUCCESS)
    NiScope_errorHandler(ih, status);
  return status;
}

int NiScope_ConfigureChanCharacteristics(ViSession ih, char *dev, ViReal64 * impe)
{
  ViStatus status;
  status = niScope_ConfigureChanCharacteristics(ih, dev, *impe, 0.00);
  if (status != VI_SUCCESS)
    NiScope_errorHandler(ih, status);
//       printf("ID %d\n", impe);
  return status;
}

int NiScope_ConfigureVertical(ViSession ih, char *dev, ViReal64 * range, ViReal64 * offset,
			      ViInt32 coupl, ViReal64 * pa)
{
  //ViBoolean Enable(channel enable true,false)==value -> IVI_ATTR_BASE(1000000),IVI_CLASS_PUBLIC_ATTR_BASE  (IVI_ATTR_BASE + 250000)
  // IVISCOPE_ATTR_CHANNEL_ENABLED  (IVI_CLASS_PUBLIC_ATTR_BASE  + 5L)   ==>total value =1000000+250000+5L = 1250005
  ViStatus status;
  status = niScope_ConfigureVertical(ih, dev, *range, *offset, coupl, *pa, NISCOPE_VAL_TRUE);
  if (status != VI_SUCCESS)
    NiScope_errorHandler(ih, status);
  return status;
}

/*  
#define IVISCOPE_VAL_AC                                         (0L)
#define IVISCOPE_VAL_DC                                         (1L)
#define IVISCOPE_VAL_GND                                        (2L)
*/
//여기서  number record 는 데이터 저장을 한번하고 받을 것인지 여러번 데이터를 받고 한번에 저장을 할것인지 정한다. 그럴때가 가끔 필요하다. 
//우리는 보통 1회에 한번 데이터를 받는다.
int NiScope_ConfigureHorizontalTiming(ViSession ih, ViReal64 * msr, ViInt32 mup)
{
  ViStatus status;
  status = niScope_ConfigureHorizontalTiming(ih, *msr, mup, 50.0, 1, NISCOPE_VAL_TRUE);
  if (status != VI_SUCCESS)
    NiScope_errorHandler(ih, status);
  return status;
}

/* Trigger */
int NiScope_ConfigureTriggerEdge(ViSession ih, char *ts, ViReal64 * level, ViInt32 slope,
				 ViInt32 tc, ViReal64 * delay)
{
  ViStatus status;
  status = niScope_ConfigureTriggerEdge(ih, ts, *level, slope, tc, 0.0, *delay);
  if (status != VI_SUCCESS)
    NiScope_errorHandler(ih, status);
  return status;
}

/* commits to hardware all the parameter settings associated with the task.*/
int NiScope_Commit(ViSession ih)
{
  ViStatus status;
  status = niScope_Commit(ih);
  if (status != VI_SUCCESS)
    NiScope_errorHandler(ih, status);
  return status;
}

/* stored */
//Returns the effective sample rate of the acquired waveform using the current configuration in samples per second.
int NiScope_SampleRate(ViSession ih, ViReal64 * samplerate)
{
  ViStatus status;
  status = niScope_SampleRate(ih, samplerate);
  if (status != VI_SUCCESS)
    NiScope_errorHandler(ih, status);
  return status;
}

//Returns the actual number of points the digitizer acquires for each channel.
// After configuring the digitizer for an acquisition, call this function to determine the size of the waveforms that the digitizer acquires. 
//Allocate a ViReal64 array of this size or greater to pass as the waveformArray parameter of niScope_ReadWaveform and niScope_FetchWaveform
int NiScope_ActualRecordLength(ViSession ih, ViPInt32 recordlength)
{
  ViStatus status;
  status = niScope_ActualRecordLength(ih, recordlength);
  if (status != VI_SUCCESS)
    NiScope_errorHandler(ih, status);
  return status;
}

//the array length can be determined by calling niScope_ActualNumWfms.
//Allows you to declare appropriately-sized waveforms. NI-SCOPE handles the channel list parsing for you. 
int NiScope_ActualNumWfms(ViSession ih, char *dev, ViPInt32 numwfms)
{
  ViStatus status;
  status = niScope_ActualNumWfms(ih, dev, numwfms);
  if (status != VI_SUCCESS)
    NiScope_errorHandler(ih, status);
  return status;
}

//Returns the total available size of an array measurement acquisition.
int NiScope_ActualMeasWfmSize(ViSession ih, ViPInt32 measwaveformsize)
{
  ViStatus status;
  status = niScope_ActualMeasWfmSize(ih, NISCOPE_VAL_MULTIPLY_CHANNELS, measwaveformsize);
  if (status != VI_SUCCESS)
    NiScope_errorHandler(ih, status);
  return status;
}

/* ch0 & ch1 의 Gain 과 Offset 값을 각각 가져 와야 하는데*/

int NiScope_Store(ViSession ih, char *dev, ViInt16 * bindata, ViReal64 * gainch,
		  ViReal64 * offsetch)
// int NiScope_Store (ViSession ih, char* dev, ViReal64 *gainch, ViReal64 *offsetch)
{
  ViInt32 samlen;
  ViStatus status;
  ViReal64 t1, t2;

  struct niScope_wfmInfo info[2];

  status = niScope_ActualRecordLength(ih, &samlen);
  if (status != VI_SUCCESS)
    NiScope_errorHandler(ih, status);

//    bindata =(ViInt32 *) malloc (samlen * sizeof (ViInt32));
  t1 = Timer();
  status = niScope_FetchBinary16(ih, dev, 5.0, samlen, bindata, info);
  t2 = Timer();
  if (status != VI_SUCCESS)
    NiScope_errorHandler(ih, status);
  printf("Elapsed time: %f", t2 - t1);
  gainch[0] = info[0].gain;
  gainch[1] = info[1].gain;
  offsetch[0] = info[0].offset;
  offsetch[1] = info[1].offset;
  return status;
}

/*   C++ 에서 곧바로 Tree에 넣는 부분인데 아직 완성 못한 부분. 이상한 일이~~~
//        error C2664: 'descr' : cannot convert parameter 1 from 'long *' to 'int *'
//        Types pointed to are unrelated; conversion requires reinterpret_cast, C-style cast or function-style cast

int NiScope_Store (int ih, char *nodeName, char* dev)
{
   int i;
   ViInt32 stats, dtype, idesc1, idesc2, idesc3, idesc4,idesc5,idesc6, null=0;
   char pathName[256];
   ViReal64 srate, voltrange, voltresolution;
   ViInt32 *bindata, *bindata1, samlen, samcount, numsam;
   struct niScope_wfmInfo in;
   struct niScope_wfmInfo info;

   voltresolution = 8912; //NI5122 14bits		 여기서는 데이터 값이 계산된 값으로 판단된다. 비트계산 하지 않은 값..
   niScope_SampleRate (ih, &srate);
   niScope_ActualRecordLength (ih, &samlen);
   
   // Data acquisition and Data Put tree   /  
	  samcount = samlen -1;        //
	  srate = 1.0/srate;        // sample rate  --> time

	  dtype = DTYPE_DOUBLE;
      idesc4 = descr(&dtype, &srate, &null);   // time

      dtype = DTYPE_ULONGLONG;
	  idesc3 = descr(&dtype, &samcount, &null);   // samples number

   if (dev="0")
	  bindata = malloc (samlen * sizeof (ViInt32));
	  niScope_FetchBinary32 (ih, "0", 5.0, samlen, bindata, &in);

//	voltage= bindata * gain + in.offset 

	  dtype = DTYPE_DOUBLE;
	  idesc1 = descr(&dtype, &in.gain, &null);  // gain   ch 0
      idesc2 = descr(&dtype, bindata, &samlen, &null);  // binary data ch 0
	  idesc5 = descr(&dtype, &in.offset, &null);  //offset  ch 0
     sprintf (pathName, "%s:AI0:FOO", nodeName);
     stats = MdsPut(pathName,"BUILD_SIGNAL(BUILD_WITH_UNITS(($*$VALUE)+$,'V'),(`$[0:$]),MAKE_DIM(MAKE_WINDOW(0,$,0.0),MAKE_SLOPE(MAKE_WITH_UNITS($,'s'))))", &idesc1, &idesc5, &idesc2, &idesc3, &idesc4, &null);          

   else if (dev="1")
	  bindata1 = malloc (samlen * sizeof (ViInt32));
	  niScope_FetchBinary32 (ih, "1", 5.0, samlen, bindata1, &info);

	  dtype = DTYPE_DOUBLE;
	  idesc1 = descr(&dtype, &info.gain, &null);  // gain   ch 0
      idesc2 = descr(&dtype, bindata, &samlen, &null);  // binary data ch 0
	  idesc5 = descr(&dtype, &info.offset, &null);  //offset  ch 0
	 sprintf (pathName, "%s:AI1:FOO", nodeName);
     stats = MdsPut(pathName,"BUILD_SIGNAL(BUILD_WITH_UNITS(($*$VALUE)+$,'V'),(`$[0:$]),MAKE_DIM(MAKE_WINDOW(0,$,0.0),MAKE_SLOPE(MAKE_WITH_UNITS($,'s'))))", &idesc1, &idesc5, &idesc2, &idesc3, &idesc4, &null);          

   else if (dev="0,1")
	  bindata = malloc (samlen * sizeof (ViInt32));
      bindata1 = malloc (samlen * sizeof (ViInt32));
      niScope_FetchBinary32 (ih, "0", 5.0, samlen, bindata, &in);
	  niScope_FetchBinary32 (ih, "1", 5.0, samlen, bindata1, &info);
	  voltdata = malloc (samlen * sizeof (ViReal64));
	  voltdata1 = malloc (samlen * sizeof (ViReal64));

	  dtype = DTYPE_DOUBLE;
	  idesc1 = descr(&dtype, &in.gain, &null);  // gain   ch 0
      idesc2 = descr(&dtype, bindata, &samlen, &null);  // binary data ch 0
	  idesc5 = descr(&dtype, &in.offset, &null);  //offset  ch 0
     sprintf (pathName, "%s:AI0:FOO", nodeName);
     stats = MdsPut(pathName,"BUILD_SIGNAL(BUILD_WITH_UNITS(($*$VALUE)+$,'V'),(`$[0:$]),MAKE_DIM(MAKE_WINDOW(0,$,0.0),MAKE_SLOPE(MAKE_WITH_UNITS($,'s'))))", &idesc1, &idesc5, &idesc2, &idesc3, &idesc4, &null);          
	 
	  dtype = DTYPE_DOUBLE;
	  idesc1 = descr(&dtype, &info.gain, &null);  // gain   ch 0
      idesc2 = descr(&dtype, bindata, &samlen, &null);  // binary data ch 0
	  idesc5 = descr(&dtype, &info.offset, &null);  //offset  ch 0
	 sprintf (pathName, "%s:AI1:FOO", nodeName);
     stats = MdsPut(pathName,"BUILD_SIGNAL(BUILD_WITH_UNITS(($*$VALUE)+$,'V'),(`$[0:$]),MAKE_DIM(MAKE_WINDOW(0,$,0.0),MAKE_SLOPE(MAKE_WITH_UNITS($,'s'))))", &idesc1, &idesc5, &idesc2, &idesc3, &idesc4, &null);          

    free(voltdata);
	free(voltdata1);
  return 1;
}
*/

int NiScope_close(ViSession ih)
{
  ViStatus status;
  status = niScope_close(ih);
  if (status != VI_SUCCESS)
    NiScope_errorHandler(ih, status);
  return status;
}
