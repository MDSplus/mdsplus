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
/*
 This header was generated using mdsshr/gen_messages.py
 To add new status messages modify:
     mitdevices_messages.xml
 and then in mdsshr do:
     python gen_messages.py
*/
#pragma once
#include <status.h>

#define DevBAD_ENDIDX            0x277c800a
#define DevBAD_FILTER            0x277c8012
#define DevBAD_FREQ              0x277c801a
#define DevBAD_GAIN              0x277c8022
#define DevBAD_HEADER            0x277c802a
#define DevBAD_HEADER_IDX        0x277c8032
#define DevBAD_MEMORIES          0x277c803a
#define DevBAD_MODE              0x277c8042
#define DevBAD_NAME              0x277c804a
#define DevBAD_OFFSET            0x277c8052
#define DevBAD_STARTIDX          0x277c805a
#define DevNOT_TRIGGERED         0x277c8062
#define DevFREQ_TO_HIGH          0x277c806a
#define DevINVALID_NOC           0x277c8072
#define DevRANGE_MISMATCH        0x277c807a
#define DevCAMACERR              0x277c8082
#define DevBAD_VERBS             0x277c808a
#define DevBAD_COMMANDS          0x277c8092
#define DevCAM_ADNR              0x277c809a
#define DevCAM_ERR               0x277c80a2
#define DevCAM_LOSYNC            0x277c80aa
#define DevCAM_LPE               0x277c80b2
#define DevCAM_TMO               0x277c80ba
#define DevCAM_TPE               0x277c80c2
#define DevCAM_STE               0x277c80ca
#define DevCAM_DERR              0x277c80d2
#define DevCAM_SQ                0x277c80da
#define DevCAM_NOSQ              0x277c80e2
#define DevCAM_SX                0x277c80ea
#define DevCAM_NOSX              0x277c80f2
#define DevINV_SETUP             0x277c80fa
#define DevPYDEVICE_NOT_FOUND    0x277c8102
#define DevPY_INTERFACE_LIBRARY_NOT_FOUND 0x277c810a
#define DevIO_STUCK              0x277c8112
#define DevUNKOWN_STATE          0x277c811a
#define DevWRONG_TREE            0x277c8122
#define DevWRONG_PATH            0x277c812a
#define DevWRONG_SHOT            0x277c8132
#define DevOFFLINE               0x277c813a
#define DevTRIGGERED_NOT_STORED  0x277c8142
#define DevNO_NAME_SPECIFIED     0x277c814a
#define DevBAD_ACTIVE_CHAN       0x277c8152
#define DevBAD_TRIG_SRC          0x277c815a
#define DevBAD_CLOCK_SRC         0x277c8162
#define DevBAD_PRE_TRIG          0x277c816a
#define DevBAD_POST_TRIG         0x277c8172
#define DevBAD_CLOCK_FREQ        0x277c817a
#define DevTRIGGER_FAILED        0x277c8182
#define DevERROR_READING_CHANNEL 0x277c818a
#define DevERROR_DOING_INIT      0x277c8192
#define Reticon$_NORMAL          0x277c8199
#define Reticon$_BAD_FRAMES      0x277c81a2
#define Reticon$_BAD_FRAME_SELECT 0x277c81aa
#define Reticon$_BAD_NUM_STATES  0x277c81b2
#define Reticon$_BAD_PERIOD      0x277c81ba
#define Reticon$_BAD_PIXEL_SELECT 0x277c81c2
#define Reticon$_DATA_CORRUPTED  0x277c81ca
#define Reticon$_TOO_MANY_FRAMES 0x277c81d2
#define J221$_NORMAL             0x277c8329
#define J221$_INVALID_DATA       0x277c8334
#define J221$_NO_DATA            0x277c833a
#define Timing$_INVCLKFRQ        0x277c84ba
#define Timing$_INVDELDUR        0x277c84c2
#define Timing$_INVOUTCTR        0x277c84ca
#define Timing$_INVPSEUDODEV     0x277c84d2
#define Timing$_INVTRGMOD        0x277c84da
#define Timing$_NOPSEUDODEV      0x277c84e3
#define Timing$_TOO_MANY_EVENTS  0x277c84ea
#define B2408$_NORMAL            0x277c8649
#define B2408$_OVERFLOW          0x277c8653
#define B2408$_TRIG_LIM          0x277c865c
#define Fera$_NORMAL             0x277c87d9
#define Fera$_DIGNOTSTRARRAY     0x277c87e2
#define Fera$_NODIG              0x277c87ea
#define Fera$_MEMNOTSTRARRAY     0x277c87f2
#define Fera$_NOMEM              0x277c87fa
#define Fera$_PHASE_LOST         0x277c8802
#define Fera$_CONFUSED           0x277c880c
#define Fera$_OVER_RUN           0x277c8814
#define Fera$_OVERFLOW           0x277c881b
#define Hm650$_NORMAL            0x277c8969
#define Hm650$_DLYCHNG           0x277c8974
#define Hv4032$_NORMAL           0x277c8af9
#define Hv4032$_WRONG_POD_TYPE   0x277c8b02
#define Hv1440$_NORMAL           0x277c8c89
#define Hv1440$_WRONG_POD_TYPE   0x277c8c92
#define Hv1440$_BAD_FRAME        0x277c8c9a
#define Hv1440$_BAD_RANGE        0x277c8ca2
#define Hv1440$_OUTRNG           0x277c8caa
#define Hv1440$_STUCK            0x277c8cb2
#define Joerger$_BAD_PRE_TRIGGER 0x277c8e1a
#define Joerger$_BAD_ACT_MEMORY  0x277c8e22
#define Joerger$_BAD_GAIN        0x277c8e2a
#define U_of_m$_BAD_WAVE_LENGTH  0x277c8faa
#define U_of_m$_BAD_SLIT_WIDTH   0x277c8fb2
#define U_of_m$_BAD_NUM_SPECTRA  0x277c8fba
#define U_of_m$_BAD_GRATING      0x277c8fc2
#define U_of_m$_BAD_EXPOSURE     0x277c8fca
#define U_of_m$_BAD_FILTER       0x277c8fd2
#define U_of_m$_GO_FILE_ERROR    0x277c8fda
#define U_of_m$_DATA_FILE_ERROR  0x277c8fe2
#define Idl$_NORMAL              0x277c9139
#define Idl$_ERROR               0x277c9142
#define B5910a$_BAD_CHAN         0x277c92ca
#define B5910a$_BAD_CLOCK        0x277c92d2
#define B5910a$_BAD_ITERATIONS   0x277c92da
#define B5910a$_BAD_NOC          0x277c92e2
#define B5910a$_BAD_SAMPS        0x277c92ea
#define J412$_NOT_SORTED         0x277c945a
#define J412$_NO_DATA            0x277c9462
#define J412$_BADCYCLES          0x277c946a
#define Tr16$_NORMAL             0x277c95e9
#define Tr16$_BAD_MEMSIZE        0x277c95f2
#define Tr16$_BAD_ACTIVEMEM      0x277c95fa
#define Tr16$_BAD_ACTIVECHAN     0x277c9602
#define Tr16$_BAD_PTS            0x277c960a
#define Tr16$_BAD_FREQUENCY      0x277c9612
#define Tr16$_BAD_MASTER         0x277c961a
#define Tr16$_BAD_GAIN           0x277c9622
#define A14$_NORMAL              0x277c9779
#define A14$_BAD_CLK_DIVIDE      0x277c9782
#define A14$_BAD_MODE            0x277c978a
#define A14$_BAD_CLK_POLARITY    0x277c9792
#define A14$_BAD_STR_POLARITY    0x277c979a
#define A14$_BAD_STP_POLARITY    0x277c97a2
#define A14$_BAD_GATED           0x277c97aa
#define L6810$_NORMAL            0x277c9909
#define L6810$_BAD_ACTIVECHAN    0x277c9912
#define L6810$_BAD_ACTIVEMEM     0x277c991a
#define L6810$_BAD_FREQUENCY     0x277c9922
#define L6810$_BAD_FULL_SCALE    0x277c992a
#define L6810$_BAD_MEMORIES      0x277c9932
#define L6810$_BAD_COUPLING      0x277c993a
#define L6810$_BAD_OFFSET        0x277c9942
#define L6810$_BAD_SEGMENTS      0x277c994a
#define L6810$_BAD_TRIG_DELAY    0x277c9952
#define J_dac$_OUTRNG            0x277c9a9a
#define Incaa$_BAD_ACTIVE_CHANS  0x277c9c2a
#define Incaa$_BAD_MASTER        0x277c9c32
#define Incaa$_BAD_EXT_1MHZ      0x277c9c3a
#define Incaa$_BAD_PTSC          0x277c9c42
#define L8212$_BAD_HEADER        0x277c9dba
#define L8212$_BAD_MEMORIES      0x277c9dc2
#define L8212$_BAD_NOC           0x277c9dca
#define L8212$_BAD_OFFSET        0x277c9dd2
#define L8212$_BAD_PTS           0x277c9dda
#define L8212$_FREQ_TO_HIGH      0x277c9de2
#define L8212$_INVALID_NOC       0x277c9dea
#define Mpb$_BADTIME             0x277c9f4a
#define Mpb$_BADFREQ             0x277c9f52
#define L8828$_BAD_OFFSET        0x277ca0da
#define L8828$_BAD_PRETRIG       0x277ca0e2
#define L8828$_BAD_ACTIVEMEM     0x277ca0ea
#define L8828$_BAD_CLOCK         0x277ca0f2
#define L8818$_BAD_OFFSET        0x277ca26a
#define L8818$_BAD_PRETRIG       0x277ca272
#define L8818$_BAD_ACTIVEMEM     0x277ca27a
#define L8818$_BAD_CLOCK         0x277ca282
#define J_tr612$_BAD_ACTMEM      0x277ca2ba
#define J_tr612$_BAD_PRETRIG     0x277ca2c2
#define J_tr612$_BAD_MODE        0x277ca2ca
#define J_tr612$_BAD_FREQUENCY   0x277ca2d2
#define L8206$_NODATA            0x277ca3fc
#define H912$_BAD_CLOCK          0x277ca44a
#define H912$_BAD_BLOCKS         0x277ca452
#define H912$_BAD_PTS            0x277ca45a
#define H908$_BAD_CLOCK          0x277ca49a
#define H908$_BAD_ACTIVE_CHANS   0x277ca4a2
#define H908$_BAD_PTS            0x277ca4aa
#define Dsp2904$_CHANNEL_READ_ERROR 0x277ca4ea
#define Py$_UNHANDLED_EXCEPTION  0x277ca53a
#define Dt196b$_NO_SAMPLES       0x277ca58a
#define DevCANNOT_LOAD_SETTINGS  0x277ca5a2
#define DevCANNOT_GET_BOARD_STATE 0x277ca5aa
#define DevACQCMD_FAILED         0x277ca5b2
#define DevACQ2SH_FAILED         0x277ca5ba
#define DevBAD_PARAMETER         0x277ca5c2
#define DevCOMM_ERROR            0x277ca5ca
#define DevCAMERA_NOT_FOUND      0x277ca5d2
#define DevNOT_A_PYDEVICE        0x277ca5da
