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
#include <cvirte.h>
#include <math.h>
#include <winsock.h>
#include "toolbox.h"
#include <mdslib.h>
#include <NIDAQmx.h>

#define NBUF (64)
#define NTASK (4)
#define DllExport   __declspec( dllexport )

struct DataChain {
  int16 *data;
  uInt32 num;
  struct DataChain *next;
};

typedef struct DataChain sDataChain;

typedef struct {
  TaskHandle handle;
  sDataChain *start;
  sDataChain *current;
  int32 type;
  uInt32 num, numChannels;
} sTask;

struct {
  unsigned int num;
  sTask *item;
} tasks;

int task_insert(TaskHandle handle)
{
  int i, j, num;

  /* Find the next empty item */
  num = tasks.num;
  for (i = 0; i < num; i++)
    if (tasks.item[i].handle == 0)
      break;
  /* If there is no empty one, make more rooms */
  if (i == num) {
    tasks.num += NTASK;
    tasks.item = realloc(tasks.item, tasks.num * sizeof(sTask));
    for (j = num + 1; j < tasks.num; j++)
      tasks.item[j].handle = 0;
  }
  tasks.item[i].handle = handle;
  return i;
}

int DllExport _nidaq_open(char *name)
{
  int id, status;
  char buf[256];
  TaskHandle handle;
  sTask *pTask;
  sDataChain *pData;

  status = DAQmxLoadTask(name, &handle);
  if (status) {
    DAQmxGetErrorString(status, buf, 256);
    printf("%s\n", buf);
    return status;
  }
  id = task_insert(handle);
  pTask = tasks.item + id;

  /* # of channels */
  DAQmxGetReadAttribute(handle, DAQmx_Read_NumChans, &pTask->numChannels);

  DAQmxGetTimingAttribute(handle, DAQmx_SampTimingType, &pTask->type);
  if (pTask->type == DAQmx_Val_OnDemand) {
    pData = malloc(sizeof(sDataChain));
    pData->data = malloc(NBUF * pTask->numChannels * sizeof(short));
    pData->num = 0;
    pData->next = 0;
    pTask->current = pData;
    pTask->start = pData;
    pTask->num = 0;
  }

  return id;
}

int DllExport _nidaq_close(int id)
{
  sTask *pTask;

  pTask = tasks.item + id;
  DAQmxClearTask(pTask->handle);
  pTask->handle = 0;

  return 1;
}

int DllExport _nidaq_init(int id)
{
  return DAQmxStartTask(tasks.item[id].handle);
}

int DllExport _nidaq_read(int id)
{
  int32 samplesRead;
  sTask *pTask;
  sDataChain *pData, *pNext;

  pTask = tasks.item + id;
  if (pTask->type != DAQmx_Val_OnDemand)
    return 0;
  pData = pTask->current;
  /* if the data is full, make another chain */
  if (pData->num == NBUF) {
    pNext = malloc(sizeof(sDataChain));
    pNext->data = malloc(NBUF * pTask->numChannels * sizeof(short));
    pNext->num = 0;
    pNext->next = 0;
    pData->next = pNext;
    pTask->current = pNext;
    pTask->num++;
    pData = pNext;
  }
  DAQmxReadBinaryI16(pTask->handle, DAQmx_Val_Auto, 10.0, DAQmx_Val_GroupByScanNumber,
		     pData->data + pData->num * pTask->numChannels,
		     (NBUF - pData->num) * pTask->numChannels, &samplesRead, NULL);
  pData->num += samplesRead;

  return pTask->num;
}

int DllExport _nidaq_store(int id, char *nodeName, int simultaneous)
{
  char channelName[256], pathName[256];
  int16 *data;
  int32 stats, samplesRead, idesc, idesc1, idesc2, idesc3, idesc4, idesc5, idesc6, dtype, null = 0;
  uInt32 i, arraySize, nPacket, packetSize;
  uInt64 numSamples;
  float64 convRate, dt, range, resolution, period;
  sTask *pTask;
  sDataChain *pData, *pNext;

  pTask = tasks.item + id;
  if (simultaneous) {
    dt = 0.0;
  } else {
    DAQmxGetTimingAttribute(pTask->handle, DAQmx_AIConv_Rate, &convRate);
    dt = 1.0 / convRate;
  }

  if (pTask->type == DAQmx_Val_OnDemand) {
    pData = pTask->current;
    samplesRead = pTask->num * NBUF + pData->num;
    arraySize = pTask->numChannels * samplesRead;
    data = (int16 *) malloc(arraySize * sizeof(int16));
    nPacket = NBUF * pTask->numChannels;
    packetSize = nPacket * sizeof(int16);
    pNext = pTask->start;
    for (i = 0; i < pTask->num; i++) {
      pData = pNext;
      memcpy(data + i * nPacket, pData->data, packetSize);
      pNext = pData->next;
      free(pData->data);
      free(pData);
    }
    memcpy(data + i * nPacket, pNext->data, pNext->num * pTask->numChannels * sizeof(int16));
    free(pNext->data);
    free(pNext);
  } else {
    /* # of samples */
    DAQmxGetTimingAttribute(pTask->handle, DAQmx_SampQuant_SampPerChan, &numSamples);
    arraySize = pTask->numChannels * numSamples;
    data = (int16 *) malloc(arraySize * sizeof(int16));
    DAQmxReadBinaryI16(pTask->handle, DAQmx_Val_Auto, 10.0, DAQmx_Val_GroupByScanNumber, data,
		       arraySize, &samplesRead, NULL);
  }
  /* Get the ADC resolution 12bits=NI6071,NI6115, 16bits=NI6143, 14bits but packed from bit 15=NI6133 */
  DAQmxGetChanAttribute(pTask->handle, NULL, DAQmx_AI_Resolution, &resolution);
  if (resolution == 14.)
    resolution = 16.;
  resolution = ldexp(1.0, (int)resolution - 1);

  /* Sampling time */
  DAQmxGetTimingAttribute(pTask->handle, DAQmx_SampClk_Rate, &period);
  period = 1.0 / period;	//Rate -> Time

  dtype = DTYPE_SHORT;
  idesc1 = descr(&dtype, data, &arraySize, &null);

  dtype = DTYPE_ULONG;
  idesc2 = descr(&dtype, &i, &null);

  idesc3 = descr(&dtype, &pTask->numChannels, &null);

  dtype = DTYPE_ULONGLONG;
  numSamples = samplesRead - 1;
  idesc4 = descr(&dtype, &numSamples, &null);

  dtype = DTYPE_DOUBLE;
  idesc = descr(&dtype, &range, &null);
  idesc5 = descr(&dtype, &convRate, &null);
  idesc6 = descr(&dtype, &period, &null);
  for (i = 0; i < pTask->numChannels; i++) {
    /* Translate a channel name to a node name */
    DAQmxGetNthTaskChannel(pTask->handle, i + 1, channelName, 256);
    sprintf(pathName, "%s:%s:FOO", nodeName, channelName);

    DAQmxGetChanAttribute(pTask->handle, channelName, DAQmx_AI_Max, &range);
    range /= resolution;

    convRate = i * dt;
    stats =
	MdsPut(pathName,
	       "BUILD_SIGNAL(BUILD_WITH_UNITS($*$VALUE,'V'),(`$[$:*:($)]),MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,'s'))))",
	       &idesc, &idesc1, &idesc2, &idesc3, &idesc4, &idesc5, &idesc6, &null);
  }
  free(data);

  DAQmxStopTask(pTask->handle);
  return 1;
}

int __stdcall DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  switch (fdwReason) {
  case DLL_PROCESS_ATTACH:
    if (InitCVIRTE(hinstDLL, 0, 0) == 0)
      return 0;			/* out of memory */
    break;
  case DLL_PROCESS_DETACH:
    free(tasks.item);
    CloseCVIRTE();
    break;
  }

  return 1;
}

int __stdcall DllEntryPoint(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  /* Included for compatibility with Borland */

  return DllMain(hinstDLL, fdwReason, lpvReserved);
}
