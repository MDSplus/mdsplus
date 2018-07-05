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
/*****************************
 * Simulation of a Transient Recorder for the MDSplus Tutorial
 * This code simulates the interface of a Generic transient recorder with the following characteristics:
 *  - 4 Channels
 *  - 16 bit samples, two's complement, input range 10Vpp
 *  - 64 KSamples for each channel
 *  - programmable number of PostTrigger samples
 * The interface defines three routines:
 * initialize(char *name, int clockFreq, int postTriggerSamples)
 * where clockFreq can have the following values:
 *  - 1 -> clock freq. = 1KHz
 *  - 2 -> clock freq. = 5KHz
 *  - 3 -> clock freq. = 10KHz
 *  - 4 -> clock freq. = 50 KHz
 *  - 5 -> clock freq. = 100KHz
 * trigger(char *name)
 * acquire(char *name, short *c1, short *c2 short *c3, short *c4)
 * the routine acquire returns 4  simulated sinusoidal waveform signals at the following frequencies:
 * Channel 1: 10Hz
 * Channel 2: 50 Hz
 * Channel 3: 100 Hz
 * Channel 4: 200Hz
 *
 * The name argument passed to all device routines is not used and simulates the device identifier
 * used in real devices
 */
#include <math.h>
#include <stdio.h>
#define PI 3.14159265358
static int totSamples = 65536;
static double frequency;
static int pts;
static int confOk;
//Return 0 if succesful, -1 if any argument is not correct
int initialize(char *name, int clockFreq, int postTriggerSamples)
{
  printf("INITIALIZE: %s\t%d\t%d\n", name, clockFreq, postTriggerSamples);
  if (postTriggerSamples < 0 || postTriggerSamples >= totSamples) {
    confOk = 0;
    return -1;
  }
  switch (clockFreq) {
  case 1:
    frequency = 1000;
    break;
  case 2:
    frequency = 5000;
    break;
  case 3:
    frequency = 10000;
    break;
  case 4:
    frequency = 50000;
    break;
  case 5:
    frequency = 100000;
    break;
  default:
    confOk = 0;
    return -1;
  }
  confOk = 1;
  return 1;
}

int acquire(char *name, short *c1, short *c2, short *c3, short *c4)
{
  int i;
//      it is assumed that c1,c2,c3,c4 arrays have totSamples elements
  if (!confOk)
    return -1;
  for (i = 0; i < totSamples; i++) {
    c1[i] = (short)(sin(2 * PI * 10 * i / frequency) * 32767);
    c2[i] = (short)(sin(2 * PI * 50 * i / frequency) * 32767);
    c3[i] = (short)(sin(2 * PI * 100 * i / frequency) * 32767);
    c4[i] = (short)(sin(2 * PI * 200 * i / frequency) * 32767);
  }
  return 0;
}
