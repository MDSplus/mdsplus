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
//#define LEXDEBUG
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <strroutines.h>
#include <tdishr_messages.h>
#include <treeshr.h>
#include "tdirefzone.h"
#include "tdireffunction.h"
#include "tdithreadstatic.h"
extern int tdi_hash();
extern int TdiConvert();

#define NEWLINE 10
#define MAX_TOKEN_LEN  200

typedef struct work {
  uint32_t verify, advance;
} work_t;

typedef struct _svf {
  const work_t *const stoff;
  const struct _svf *const other;
  const int         *const stops;
} svf_t;

const int vstop[] = {
0,	11,0,	1,11,0,	2,0,	8,11,0,	6,7,11,
0,11,0,	11,0,	11,0,	5,11,0,	11,0,	11,0,
6,7,0,	7,0,	7,0,	4,0,	10,0,	-3,0,
5,0,	5,0,	5,0,	7,0,	7,0,	9,0,
3,0,	4,0,	4,5,0,	9,0,	4,0,	4,0,0,
};

const work_t crank[] = {
  {0, 0}, {0, 0}, {1, 3}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {1, 4}, {1, 5},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {1, 6},
  {0, 0}, {1, 7}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {10, 20},
  {1, 3}, {0, 0}, {1, 8}, {1, 9},
  {1, 10}, {1, 11}, {2, 8}, {28, 18},
  {2, 10}, {27, 36}, {18, 18}, {18, 28},
  {36, 37}, {0, 0}, {0, 0}, {1, 12},
  {35, 41}, {0, 0}, {0, 0}, {8, 16},
  {0, 0}, {27, 37}, {1, 7}, {18, 15},
  {0, 0}, {1, 7}, {7, 14}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {7, 15}, {0, 0}, {7, 14}, {7, 14},
  {7, 14}, {7, 14}, {7, 14}, {7, 14},
  {7, 14}, {7, 14}, {7, 14}, {7, 14},
  {7, 15}, {1, 13}, {0, 0}, {0, 0},
  {0, 0}, {2, 13}, {0, 0}, {7, 14},
  {7, 14}, {7, 14}, {7, 14}, {7, 14},
  {7, 14}, {7, 14}, {7, 14}, {7, 14},
  {7, 14}, {7, 14}, {7, 14}, {7, 14},
  {7, 14}, {7, 14}, {7, 14}, {7, 14},
  {7, 14}, {7, 14}, {7, 14}, {7, 14},
  {7, 14}, {7, 14}, {7, 14}, {7, 14},
  {7, 14}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {7, 14}, {0, 0}, {7, 14},
  {7, 14}, {7, 14}, {7, 14}, {7, 14},
  {7, 14}, {7, 14}, {7, 14}, {7, 14},
  {7, 14}, {7, 14}, {7, 14}, {7, 14},
  {7, 14}, {7, 14}, {7, 14}, {7, 14},
  {7, 14}, {7, 14}, {7, 14}, {7, 14},
  {7, 14}, {7, 14}, {7, 14}, {7, 14},
  {7, 14}, {9, 17}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {9, 18}, {0, 0},
  {0, 0}, {9, 19}, {9, 19}, {9, 19},
  {9, 19}, {9, 19}, {9, 19}, {9, 19},
  {9, 19}, {9, 19}, {9, 19}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {9, 17}, {9, 17},
  {9, 17}, {9, 17}, {9, 17}, {9, 17},
  {9, 17}, {9, 17}, {9, 17}, {9, 17},
  {9, 17}, {9, 17}, {9, 17}, {9, 17},
  {9, 17}, {9, 17}, {9, 17}, {9, 17},
  {9, 17}, {9, 17}, {9, 17}, {9, 17},
  {9, 17}, {9, 17}, {9, 17}, {9, 17},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {9, 17}, {0, 0}, {9, 17}, {9, 17},
  {9, 17}, {9, 17}, {9, 17}, {9, 17},
  {9, 17}, {9, 17}, {9, 17}, {9, 17},
  {9, 17}, {9, 17}, {9, 17}, {9, 17},
  {9, 17}, {9, 17}, {9, 17}, {9, 17},
  {9, 17}, {9, 17}, {9, 17}, {9, 17},
  {9, 17}, {9, 17}, {9, 17}, {9, 17},
  {11, 21}, {0, 0}, {11, 22}, {11, 22},
  {11, 22}, {11, 22}, {11, 22}, {11, 22},
  {11, 22}, {11, 22}, {11, 22}, {11, 22},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {11, 23},
  {11, 23}, {11, 23}, {11, 24}, {11, 24},
  {11, 24}, {11, 24}, {11, 24}, {11, 23},
  {11, 23}, {11, 23}, {11, 23}, {11, 23},
  {11, 23}, {11, 23}, {11, 23}, {11, 23},
  {11, 23}, {11, 24}, {11, 24}, {11, 23},
  {11, 24}, {11, 23}, {11, 23}, {11, 23},
  {11, 23}, {0, 0}, {0, 0}, {0, 0},
  {12, 17}, {0, 0}, {0, 0}, {11, 23},
  {11, 23}, {11, 23}, {11, 24}, {11, 24},
  {11, 24}, {11, 24}, {11, 24}, {11, 23},
  {11, 23}, {11, 23}, {11, 23}, {11, 23},
  {11, 23}, {11, 23}, {11, 23}, {11, 23},
  {11, 23}, {11, 24}, {11, 24}, {11, 23},
  {11, 24}, {11, 23}, {11, 23}, {11, 23},
  {11, 23}, {12, 17}, {12, 17}, {12, 17},
  {12, 17}, {12, 17}, {12, 17}, {12, 17},
  {12, 17}, {12, 17}, {12, 17}, {12, 17},
  {12, 17}, {12, 17}, {12, 17}, {12, 17},
  {12, 17}, {12, 17}, {12, 17}, {12, 17},
  {12, 17}, {12, 17}, {12, 17}, {12, 17},
  {12, 17}, {12, 17}, {12, 17}, {0, 0},
  {0, 0}, {0, 0}, {13, 25}, {12, 17},
  {0, 0}, {12, 17}, {12, 17}, {12, 17},
  {12, 17}, {12, 17}, {12, 17}, {12, 17},
  {12, 17}, {12, 17}, {12, 17}, {12, 17},
  {12, 17}, {12, 17}, {12, 17}, {12, 17},
  {12, 17}, {12, 17}, {12, 17}, {12, 17},
  {12, 17}, {12, 17}, {12, 17}, {12, 17},
  {12, 17}, {12, 17}, {12, 17}, {13, 25},
  {13, 25}, {13, 25}, {13, 25}, {13, 25},
  {13, 25}, {13, 25}, {13, 25}, {13, 25},
  {13, 25}, {13, 25}, {13, 25}, {13, 25},
  {13, 25}, {13, 25}, {13, 25}, {13, 25},
  {13, 25}, {13, 25}, {13, 25}, {13, 25},
  {13, 25}, {13, 25}, {13, 25}, {13, 25},
  {13, 25}, {0, 0}, {0, 0}, {0, 0},
  {15, 26}, {13, 25}, {0, 0}, {13, 25},
  {13, 25}, {13, 25}, {13, 25}, {13, 25},
  {13, 25}, {13, 25}, {13, 25}, {13, 25},
  {13, 25}, {13, 25}, {13, 25}, {13, 25},
  {13, 25}, {13, 25}, {13, 25}, {13, 25},
  {13, 25}, {13, 25}, {13, 25}, {13, 25},
  {13, 25}, {13, 25}, {13, 25}, {13, 25},
  {13, 25}, {15, 26}, {15, 26}, {15, 26},
  {15, 26}, {15, 26}, {15, 26}, {15, 26},
  {15, 26}, {15, 26}, {15, 26}, {15, 26},
  {15, 26}, {15, 26}, {15, 26}, {15, 26},
  {15, 26}, {15, 26}, {15, 26}, {15, 26},
  {15, 26}, {15, 26}, {15, 26}, {15, 26},
  {15, 26}, {15, 26}, {15, 26}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {15, 26},
  {0, 0}, {15, 26}, {15, 26}, {15, 26},
  {15, 26}, {15, 26}, {15, 26}, {15, 26},
  {15, 26}, {15, 26}, {15, 26}, {15, 26},
  {15, 26}, {15, 26}, {15, 26}, {15, 26},
  {15, 26}, {15, 26}, {15, 26}, {15, 26},
  {15, 26}, {15, 26}, {15, 26}, {15, 26},
  {15, 26}, {15, 26}, {15, 26}, {16, 27},
  {33, 38}, {33, 38}, {33, 38}, {33, 38},
  {33, 38}, {33, 38}, {33, 38}, {33, 38},
  {33, 38}, {33, 38}, {0, 0}, {16, 27},
  {16, 27}, {16, 27}, {16, 27}, {16, 27},
  {16, 27}, {16, 27}, {16, 27}, {16, 27},
  {16, 27}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {16, 27}, {16, 27}, {16, 27}, {16, 27},
  {16, 27}, {16, 27}, {16, 27}, {16, 27},
  {16, 27}, {16, 27}, {16, 27}, {16, 27},
  {16, 27}, {16, 27}, {16, 27}, {16, 27},
  {16, 27}, {16, 27}, {16, 27}, {16, 27},
  {16, 27}, {16, 27}, {16, 27}, {16, 27},
  {16, 27}, {16, 27}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {16, 27}, {0, 0},
  {16, 27}, {16, 27}, {16, 27}, {16, 27},
  {16, 27}, {16, 27}, {16, 27}, {16, 27},
  {16, 27}, {16, 27}, {16, 27}, {16, 27},
  {16, 27}, {16, 27}, {16, 27}, {16, 27},
  {16, 27}, {16, 27}, {16, 27}, {16, 27},
  {16, 27}, {16, 27}, {16, 27}, {16, 27},
  {16, 27}, {16, 27}, {17, 15}, {0, 0},
  {17, 17}, {17, 17}, {17, 17}, {17, 17},
  {17, 17}, {17, 17}, {17, 17}, {17, 17},
  {17, 17}, {17, 17}, {17, 15}, {19, 19},
  {19, 19}, {19, 19}, {19, 19}, {19, 19},
  {19, 19}, {19, 19}, {19, 19}, {19, 19},
  {19, 19}, {34, 34}, {34, 34}, {34, 34},
  {34, 34}, {34, 34}, {34, 34}, {34, 34},
  {34, 34}, {34, 34}, {34, 34}, {19, 29},
  {19, 29}, {19, 29}, {19, 29}, {19, 29},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {21, 30}, {0, 0},
  {0, 0}, {0, 0}, {19, 29}, {19, 29},
  {0, 0}, {19, 29}, {21, 30}, {21, 30},
  {39, 40}, {39, 40}, {39, 40}, {39, 40},
  {39, 40}, {39, 40}, {39, 40}, {39, 40},
  {39, 40}, {39, 40}, {0, 0}, {19, 29},
  {19, 29}, {19, 29}, {19, 29}, {19, 29},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {21, 30},
  {0, 0}, {21, 30}, {19, 29}, {19, 29},
  {0, 0}, {19, 29}, {0, 0}, {0, 0},
  {21, 30}, {0, 0}, {0, 0}, {21, 0},
  {0, 0}, {21, 31}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {21, 30},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {21, 30}, {0, 0},
  {0, 0}, {21, 32}, {23, 23}, {23, 23},
  {23, 23}, {23, 23}, {23, 23}, {23, 23},
  {23, 23}, {23, 23}, {23, 23}, {23, 23},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {23, 23},
  {23, 23}, {23, 23}, {23, 23}, {23, 23},
  {23, 23}, {23, 23}, {23, 23}, {23, 23},
  {23, 23}, {23, 23}, {23, 23}, {23, 23},
  {23, 23}, {23, 23}, {23, 23}, {23, 23},
  {23, 23}, {23, 23}, {23, 23}, {23, 23},
  {23, 23}, {23, 23}, {23, 23}, {23, 23},
  {23, 23}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {23, 23},
  {23, 23}, {23, 23}, {23, 23}, {23, 23},
  {23, 23}, {23, 23}, {23, 23}, {23, 23},
  {23, 23}, {23, 23}, {23, 23}, {23, 23},
  {23, 23}, {23, 23}, {23, 23}, {23, 23},
  {23, 23}, {23, 23}, {23, 23}, {23, 23},
  {23, 23}, {23, 23}, {23, 23}, {23, 23},
  {23, 23}, {24, 33}, {0, 0}, {24, 33},
  {0, 0}, {0, 0}, {24, 34}, {24, 34},
  {24, 34}, {24, 34}, {24, 34}, {24, 34},
  {24, 34}, {24, 34}, {24, 34}, {24, 34},
  {25, 15}, {0, 0}, {25, 25}, {25, 25},
  {25, 25}, {25, 25}, {25, 25}, {25, 25},
  {25, 25}, {25, 25}, {25, 25}, {25, 25},
  {25, 35}, {26, 15}, {0, 0}, {26, 26},
  {26, 26}, {26, 26}, {26, 26}, {26, 26},
  {26, 26}, {26, 26}, {26, 26}, {26, 26},
  {26, 26}, {26, 15}, {29, 33}, {0, 0},
  {29, 33}, {0, 0}, {0, 0}, {29, 38},
  {29, 38}, {29, 38}, {29, 38}, {29, 38},
  {29, 38}, {29, 38}, {29, 38}, {29, 38},
  {29, 38}, {31, 31}, {31, 31}, {31, 31},
  {31, 31}, {31, 31}, {31, 31}, {31, 31},
  {31, 31}, {31, 31}, {31, 31}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {31, 32}, {31, 32}, {31, 32},
  {31, 32}, {31, 32}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {31, 32}, {31, 32}, {32, 39}, {31, 32},
  {32, 39}, {0, 0}, {0, 0}, {32, 40},
  {32, 40}, {32, 40}, {32, 40}, {32, 40},
  {32, 40}, {32, 40}, {32, 40}, {32, 40},
  {32, 40}, {31, 32}, {31, 32}, {31, 32},
  {31, 32}, {31, 32}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {31, 32}, {31, 32}, {0, 0}, {31, 32},
  {0, 0}};

const svf_t svec[] = {
  {0, 0, 0},
  {crank + -1, 0, 0},
  {crank + -5, svec + 1, 0},
  {crank + 0, 0, vstop + 1},
  {crank + 0, 0, vstop + 3},
  {crank + 0, 0, vstop + 6},
  {crank + 0, 0, vstop + 8},
  {crank + 34, 0, vstop + 11},
  {crank + 1, 0, vstop + 15},
  {crank + 121, 0, vstop + 17},
  {crank + 1, 0, vstop + 19},
  {crank + 198, 0, vstop + 21},
  {crank + 256, 0, vstop + 24},
  {crank + 314, 0, vstop + 26},
  {crank + 0, svec + 7, vstop + 28},
  {crank + 372, 0, 0},
  {crank + 459, 0, 0},
  {crank + 536, svec + 12, vstop + 31},
  {crank + 9, 0, vstop + 33},
  {crank + 547, 0, vstop + 35},
  {crank + 0, 0, vstop + 37},
  {crank + -625, 0, vstop + 39},
  {crank + 0, svec + 11, vstop + 41},
  {crank + 646, 0, vstop + 43},
  {crank + 726, svec + 23, vstop + 45},
  {crank + 738, svec + 13, vstop + 47},
  {crank + 751, svec + 15, vstop + 49},
  {crank + 7, svec + 16, vstop + 51},
  {crank + 6, svec + 15, 0},
  {crank + 767, 0, 0},
  {crank + 0, 0, vstop + 53},
  {crank + 777, 0, vstop + 55},
  {crank + 819, 0, 0},
  {crank + 448, 0, 0},
  {crank + 557, svec + 23, vstop + 57},
  {crank + 2, svec + 15, 0},
  {crank + 10, 0, 0},
  {crank + 0, 0, vstop + 60},
  {crank + 0, svec + 33, vstop + 62},
  {crank + 588, 0, 0},
  {crank + 0, svec + 39, vstop + 64},
  {crank + 0, svec + 12, 0},
  {0, 0, 0}
};
const work_t *const top  = crank + 895;
const svf_t  *const bgin = svec + 1;

const uint32_t match[] = {
  00, 01, 01, 01, 01, 01, 01, 01,
  01, 011, 012, 01, 01, 011, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  011, 01, '"', 01, '$', 01, 01, '"',
  01, 01, 01, '+', 01, '+', '.', 01,
  '0', '0', '0', '0', '0', '0', '0', '0',
  '0', '0', ':', 01, 01, 01, 01, 01,
  01, 'A', 'A', 'A', 'D', 'D', 'D', 'D',
  'D', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
  'A', 'A', 'A', 'D', 'D', 'A', 'D', 'A',
  'A', 'A', 'A', 01, 01, 01, 01, '$',
  01, 'A', 'A', 'A', 'D', 'D', 'D', 'D',
  'D', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
  'A', 'A', 'A', 'D', 'D', 'A', 'D', 'A',
  'A', 'A', 'A', 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  0
};

const char extra[] = {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static void upcase(char *const str, int len){
  char *pc, *const ec = str + len;
  for (pc = str; *pc && pc < ec; ++pc)
    *pc = (char)toupper(*pc);
}

/*      tdiLex
   Lexical analysis to parse tokens for TdiYacc.y.
   Definition section precedes rule section.
   Lex regular expression operators:
   \x "x"               as is unless "" or []
   [xyz] [x-z]  chars x through z
   x?           0 or 1 x
   x*           0 or more x
   x+           1 or more x
   x{4,7}               4 to 7 x
   x|y          either x or y
   (x)          grouping
   x/y          x if y follows
   .            any non-newline char
   ---------------------------------------
   Adjustable table sizes used/default.
   %e   /1000   nodes
   %p   /2500   positions
   %n   /500    6095 transitions
   %k   /1000   packed classes
   %a   /~2000  packed transitions
   %o   /3000   output slots
   Ken Klare, LANL P-4  (c)1989,1990,1991
   NEED to handle 6..8 and 6...8 and 6...8.9, should use spaces.
   Limitations:
   Floating requires (1) digit before exponent
   (2) either decimal point or exponent, (.E3) looks like path.
   (3) 6..8 is 6 .. 8 and not 6. .8, 6...8 is ambiguous?
   (4) prefix + or - on numbers handled elsewhere, a-6 would be tokens a -6.
   Pathname apostrophe required (1) with wildcards (% or *),
   (2) without leading \ . or : (path:member looks like file),
   (3) with son or member starting with number (.7E6 is float not son),
   (4) with up-tree minus requires leading period. .-.over
   Filename double quote required for non-simple names.
   "node::node::device:[--.dir.sub.sub]file.extension;version".
   Input, nlpos, output, pos, unput, and lex defined by include file.
   Floating for exponent but no decimal would fall into integer.
 */

#define UNPUT()		--TDI_REFZONE.a_cur
#define SET_POS()	(lvalPtr->w_ok = TDI_REFZONE.a_cur - TDI_REFZONE.a_begin)
static inline char lex_input(ThreadStatic *const TdiThreadStatic_p) {
  int tchar;
  if (TDI_REFZONE.a_cur < TDI_REFZONE.a_end)
    tchar = *TDI_REFZONE.a_cur++;
  else
    return TDI_REFZONE.a_cur++ == TDI_REFZONE.a_end ? ';' : 0;
  if (tchar==EOF)
    return '\0';
  return (char)tchar;
}
#define INPUT()	lex_input(TdiThreadStatic_p)

/*--------------------------------------------------------
	Remove comment from the Lex input stream.
	Nested comments allowed. Len is not used.
	Limitation:     Not ANSI C standard use of delimiters.
*/
static int lex_comment(
	int len __attribute__ ((unused)),
	char *str __attribute__ ((unused)),
	struct marker *mark_ptr __attribute__ ((unused)),
	ThreadStatic *const TdiThreadStatic_p)
{
  char c, c1;
  int count = 1;
  while (count) {
    if ((c = INPUT()) == '/') {
      if ((c1 = INPUT()) == '*')
	++count;
      else
	UNPUT();
    } else if (c == '*') {
      if ((c1 = INPUT()) == '/')
	--count;
      else
	UNPUT();
    } else if (c == '\0') {
      TDI_REFZONE.l_status = TdiUNBALANCE;
      return 1;
    }
  }
  return 0;
}

/*--------------------------------------------------------
	Convert floating point values with the follow
ing
	syntax to internal representation via descriptors:
	        [sign] decimal [fraction] exponent
	        or [sign] [decimal] fraction [exponent]
	where:  decimal         [+|-] 0-9...
	        fraction        . 0-9...
	        exponent        [E|F|D|G|H|S|T] [+|-] 0-9...
	NEED to size based on exponent range and number of digits.
*/
static const DESCRIPTOR(dfghst_dsc, "DFGHSTVdfghstv");
static const DESCRIPTOR(valid_dsc, "+-.0123456789DEFGHSTV \t");

static int lex_convert_floating(struct descriptor_s *str, struct descriptor_r *out_d) {
  char str_c[64];
  int len = str->length > 63 ? 63 : str->length;
  strncpy(str_c, str->pointer, len);
  str_c[len] = 0;
  if (out_d->length == sizeof(double)) {
    double tmp;
    struct descriptor tmp_d = { sizeof(double), DTYPE_NATIVE_DOUBLE, CLASS_S, 0 };
    tmp_d.pointer = (char *)&tmp;
    tmp = strtod(str_c,NULL);
    return TdiConvert(&tmp_d, out_d);
  } else {
    float tmp;
    struct descriptor tmp_d = { sizeof(float), DTYPE_NATIVE_FLOAT, CLASS_S, 0 };
    tmp_d.pointer = (char *)&tmp;
    sscanf(str_c, "%g", &tmp);
    return TdiConvert(&tmp_d, out_d);
  }
}

static int lex_float(
	int len,
	char *str,
	struct marker *mark_ptr,
	ThreadStatic *const TdiThreadStatic_p) {
  struct descriptor_s str_dsc = { len, DTYPE_T, CLASS_S, str };
  int bad, idx, status, tst, type;
  static const struct {
    length_t length;
    dtype_t  dtype;
  } table[] = {
    {
    4, DTYPE_NATIVE_FLOAT}, {
    8, DTYPE_NATIVE_DOUBLE}, {
    8, DTYPE_D}, {
    8, DTYPE_G}, {
    16, DTYPE_H}, {
    4, DTYPE_F}, {
    4, DTYPE_FS}, {
    8, DTYPE_FT}
  };
  upcase(str, len);
	/*******************
	Find final location.
	*******************/
  bad = StrFindFirstNotInSet((struct descriptor *)&str_dsc, (struct descriptor *)&valid_dsc);
  if (bad > 0)
    str_dsc.length = bad - 1;

	/**********************
	Find special exponents.
	**********************/
  idx = StrFindFirstInSet((struct descriptor *)&str_dsc, (struct descriptor *)&dfghst_dsc);
  if (idx) {
    switch (tst = str[idx - 1]) {
      case 'D':
      type = 1;
      break;
      case 'V':
      type = 2;
      break;
      case 'G':
      type = 3;
      break;
      case 'H':
      type = 4;
      break;
      case 'F':
      type = 5;
      break;
      case 'S':
      type = 6;
      break;
      case 'T':
      type = 7;
      break;
    default:
      type = 0;
      break;
    }
    str[idx - 1] = 'E';
  } else
    type = 0;

  MAKE_S(table[type].dtype, table[type].length, mark_ptr->rptr);

  status = lex_convert_floating(&str_dsc, mark_ptr->rptr);
  if (STATUS_OK && bad > 0 && str[bad - 1] != '\0')
    status = TdiEXTRANEOUS;

  mark_ptr->builtin = -1;
  if STATUS_OK
    return (LEX_VALUE);
  TDI_REFZONE.l_status = status;
  return (LEX_ERROR);
}

/*--------------------------------------------------------
	Recognize some graphic punctuation Lex symbols for YACC.
	Note must be acceptable in written form also: a<=b, a LE b, LE(a,b).
	Binary a<=(b,c) is OK, but unary <=(b,c) should not be.
*/
static int lex_bin_eq(int token, ThreadStatic *const TdiThreadStatic_p) {
  char cx;
  while ((cx = INPUT()) == ' ' || cx == '\t') ;
  if (cx == '=')
    return (LEX_BINEQ);
  UNPUT();
  return token;
}

/*--------------------------------------------------------
	Convert Lex input to identifier name or builtin.
	Clobbers string with upcase. IDENT token returns name.
	Note, Lex strings are NUL terminated.
*/
static inline int lex_ident(
	int len,
	char *str,
	struct marker *mark_ptr,
	ThreadStatic *const TdiThreadStatic_p) {
  int j, token;
  mark_ptr->builtin = -1;
  MAKE_S(DTYPE_T, len, mark_ptr->rptr);
  memcpy(mark_ptr->rptr->pointer, str, mark_ptr->rptr->length);

	/*****************************
	$ marks next compile argument.
	$nnn marks nnn-th argument.
	*****************************/
  if (str[0] == '$') {
    for (j = len; --j > 0;)
      if (str[j] < '0' || str[j] > '9')
	break;
    if (j == 0) {
      mark_ptr->builtin = OPC_$;
      return (LEX_IDENT);
    }
  } else if (str[0] == '_')
    return (LEX_VBL);

	/**********************
	Search of initial list.
	**********************/
  j = tdi_hash(len, str);
  if (j < 0) {
    if (str[0] == '$')
      return (LEX_VBL);
    return (LEX_IDENT);
  }

	/**********************************************
	Note difference of pointers is divided by step.
	Standard function gives number. Token if named.
	**********************************************/
  mark_ptr->builtin = (short)j;
  token = TdiRefFunction[j].token;
  if ((token & LEX_K_NAMED) != 0) {
    token = token & LEX_M_TOKEN;
    if (token == LEX_IN
	|| token == LEX_LAND
	|| token == LEX_LEQ
	|| token == LEX_LEQV || token == LEX_LGE || token == LEX_LOR || token == LEX_MUL)
      return lex_bin_eq(token, TdiThreadStatic_p);
    return token;
  }
  return (LEX_IDENT);
}

/*--------------------------------------------------------
	Convert integer values with the following syntax
	to internal representation via descriptors:
	        [space]...[sign][radix][digit]...[type]
	where:  sign    +       ignored
	                -       negative
	        radix   0B b    binary          digit   0-1
	(digit          0O o    octal                   0-7
	required)       0D d    decimal(float)          0-9
	                0X x    hexadecimal             0-9A-Fa-f
	        type    SB b    signed byte
	                SW w    signed word
	                SL l    signed long
	                SQ q    signed quadword
	                SO o    signed octaword
	                UB bu   unsigned byte
	                UW wu   unsigned word
	                UL lu   unsigned long
	                UQ qu   unsigned quadword
	                UO ou   unsigned octaword
	CAUTION must use unsigned char to avoid sign extend in hex.
	WARNING without following digit B and O radix become byte and octaword.
	WARNING hexadecimal strings must use SB or UB for bytes.

	Limitations:
	Depends on contiguous character set, 0-9 A-F a-f.
	Depends on right-to-left byte assignment.
	Depends on ones- or twos-complement.
	NEED size based on range, thus no overflow.
*/
#define len1 8			/*length of a word in bits */
#define num1 16			/*number of words to accumulate, octaword */

static int lex_integer(
	int len,
	char *str,
	struct marker *mark_ptr,
	ThreadStatic *const TdiThreadStatic_p) {
  const struct {
   length_t length;
   dtype_t  udtype, sdtype;
  } table[] = {
   { 1, DTYPE_BU, DTYPE_B},
   { 2, DTYPE_WU, DTYPE_W},
   { 4, DTYPE_LU, DTYPE_L},
   { 8, DTYPE_QU, DTYPE_Q},
   {16, DTYPE_OU, DTYPE_O},
  };
  INIT_STATUS;
  unsigned char sign;
  char *now = str, *end = &str[len];
  unsigned char *qptr, qq[num1], qtst;
  int carry = 0, radix;
  int length, is_signed, tst, type;

	/******************************
	Remove leading blanks and tabs.
	******************************/
  while (now < end && (*now == ' ' || *now == '\t'))
    ++now;
  upcase(now, len);

	/*********
	Save sign.
	*********/
  if (now >= end)
    sign = '+';
  else if ((sign = *now) == '-' || sign == '+')
    ++now;

	/***************************************************
	Select radix. Must be followed by appropriate digit.
	Leading zero is required in our LEX.
	Watch, 0bu a proper unsigned byte is good and
	0bub a binary with no digits is bad. Reject 0b33.
	***************************************************/
  if (now < end && *now == '0')
    ++now;
  if (now + 1 < end) {
    switch (tst = *now++) {
      case 'B':
      radix = 2;
      break;
      case 'O':
      radix = 8;
      break;
      /*      case 'D' :      radix = 10;     break; */
      case 'X':
      radix = 16;
      break;
    default:
      --now;
      radix = 0;
      break;
    }
    if ((carry = radix) == 0) {
      radix = 10;
    } else {
      if ((tst = *now) >= '0' && tst <= '9')
	carry = tst - '0';
      else if (tst >= 'A' && tst <= 'F')
	carry = tst - 'A' + 10;
      if (carry >= radix)
	--now;
    }
  } else
    radix = 10;

	/**************
	Convert number.
	**************/
  for (qptr = &qq[0]; qptr < &qq[num1]; ++qptr)
    *qptr = 0;
  for (; now < end; ++now) {
    if ((tst = *now) >= '0' && tst <= '9')
      carry = tst - '0';
    else if (tst >= 'A' && tst <= 'F')
      carry = tst - 'A' + 10;
    else
      break;
    if (carry >= radix) {
      carry = 0;
      break;
    }
    for (qptr = &qq[0]; qptr < &qq[num1]; ++qptr) {
      *qptr = (char)(carry += (int)*qptr * radix);
      carry >>= len1;
    }
  }

	/***********************************************************
	Negative numbers do negation until nonzero, then complement.
	Works for 1 or 2's complement, not sign and magnitude.
	Unsigned overflow: carry | sign, signed: carry | wrong sign.
	***********************************************************/
  if (sign == '-') {
    for (qptr = &qq[0]; qptr < &qq[num1]; ++qptr)
      if ((*qptr = (char)(-*qptr)) != 0)
	break;
    for (; ++qptr < &qq[num1];)
      *qptr = (char)(~*qptr);
  }

	/*******************************
	Find output size and signedness.
	*******************************/
  is_signed = -1;
  type = 2;
  if (now < end)
    switch (tst = *now++) {
      case 'U':
      is_signed = 0;
      break;
      case 'S':
      is_signed = 1;
      break;
      case 'B':
      type = 0;
      break;
      case 'W':
      type = 1;
      break;
      case 'L':
      type = 2;
      break;
      case 'Q':
      type = 3;
      break;
      case 'O':
      type = 4;
      break;
    default:
      --now;
      break;
    }

  if (now >= end) {
  } else if (is_signed < 0)
    switch (tst = *now++) {
      case 'U':
      is_signed = 0;
      break;
      case 'S':
      is_signed = 1;
      break;
    default:
      --now;
      break;
  } else
    switch (tst = *now++) {
      case 'B':
      type = 0;
      break;
      case 'W':
      type = 1;
      break;
      case 'L':
      type = 2;
      break;
      case 'Q':
      type = 3;
      break;
      case 'O':
      type = 4;
      break;
    default:
      --now;
      break;
    }

	/*******************************
	Remove trailing blanks and tabs.
	*******************************/
  while (now < end && (*now == ' ' || *now == '\t'))
    ++now;

  length = table[type].length;
  MAKE_S((unsigned char)(is_signed ? table[type].sdtype : table[type].udtype),
	 (unsigned short)length, mark_ptr->rptr);
  mark_ptr->builtin = -1;
  memcpy(mark_ptr->rptr->pointer, (char *)qq, length);

	/*************************
	Check the high order bits.
	*************************/
  if (now < end && *now != '\0')
    status = TdiEXTRANEOUS;

  qtst = (unsigned char)((is_signed && (char)qq[length - 1] < 0) ? -1 : 0);
  if (carry != 0 || (is_signed && ((qtst != 0) ^ (sign == '-'))))
    status = TdiTOO_BIG;
  else
    for (qptr = &qq[length]; qptr < &qq[num1]; ++qptr)
      if (*qptr != qtst)
	status = TdiTOO_BIG;

  if STATUS_OK {
#ifdef WORDS_BIGENDIAN
    int i;
    char *ptr = mark_ptr->rptr->pointer;
    for (i = 0; i < length / 2; i++) {
      char sav = ptr[i];
      ptr[i] = ptr[length - i - 1];
      ptr[length - i - 1] = sav;
    }
#endif
    return (LEX_VALUE);
  }
  TDI_REFZONE.l_status = status;
  return (LEX_ERROR);
}

/*--------------------------------------------------------
	Convert Lex input to NID or absolute PATH.
*/ // used in TdiYacc.c
int tdi_lex_path(
	int len,
	char *str,
	struct marker *mark_ptr,
	ThreadStatic *const TdiThreadStatic_p) {
  int nid, token = LEX_VALUE;
  char *str_l;
  str_l = strncpy((char *)malloc(len + 1), str, len);
  str_l[len] = 0;
  upcase(str_l, len);
  mark_ptr->builtin = -1;
  if (TDI_REFZONE.l_rel_path) {
    MAKE_S(DTYPE_PATH, (unsigned short)len, mark_ptr->rptr);
    memcpy((char *)mark_ptr->rptr->pointer, str, len);
  } else if (TreeFindNode((char *)str_l, &nid) & 1) {
    MAKE_S(DTYPE_NID, (unsigned short)sizeof(nid), mark_ptr->rptr);
    *(int *)mark_ptr->rptr->pointer = nid;
  } else {
    struct descriptor_d abs_dsc = { 0, DTYPE_T, CLASS_D, 0 };
    char *apath = TreeAbsPath((char *)str_l);
    if (apath != NULL) {
      unsigned short alen = (unsigned short)strlen(apath);
      StrCopyR((struct descriptor *)&abs_dsc, &alen, apath);
      TreeFree(apath);
      MAKE_S(DTYPE_PATH, abs_dsc.length, mark_ptr->rptr);
      memcpy((char *)mark_ptr->rptr->pointer, abs_dsc.pointer, abs_dsc.length);
      StrFree1Dx(&abs_dsc);
    } else {
      TDI_REFZONE.l_status = TreeNOT_OPEN;
      token = LEX_ERROR;
    }
  }
  free(str_l);
  return token;
}

/*--------------------------------------------------------
	Remove arrow and trailing punctation.
*/
static inline int lex_point(
	int len,
	char *str,
	struct marker *mark_ptr,
	ThreadStatic *const TdiThreadStatic_p) {
  int lenx = len - 2;
  while (str[lenx + 1] == '.' || str[lenx + 1] == ':')
    --lenx;
  mark_ptr->builtin = -1;
  MAKE_S(DTYPE_T, lenx, mark_ptr->rptr);
  memcpy((char *)mark_ptr->rptr->pointer, &str[2], lenx);
  return LEX_POINT;
}

static inline int lex_punct(
	int len __attribute__ ((unused)),
	char *str,
	struct marker *mark_ptr,
	ThreadStatic *const TdiThreadStatic_p) {
  char c0 = str[0], c1 = INPUT();

  mark_ptr->rptr = 0;

	/********************
	Two graphic operator.
	********************/
  switch (c0) {
  case '!':
    if (c1 == '=') {
      mark_ptr->builtin = OPC_NE;
      return lex_bin_eq(LEX_LEQS, TdiThreadStatic_p);
    }
    break;
  case '&':
    if (c1 == '&') {
      mark_ptr->builtin = OPC_AND;
      return lex_bin_eq(LEX_LANDS, TdiThreadStatic_p);
    }
    break;
  case '*':
    if (c1 == '*') {
      mark_ptr->builtin = OPC_POWER;
      return lex_bin_eq(LEX_POWER, TdiThreadStatic_p);
    }
    break;
  case '+':
    if (c1 == '+') {
      mark_ptr->builtin = OPC_PRE_INC;
      return (LEX_INC);
    }
    break;
  case '-':
    if (c1 == '-') {
      mark_ptr->builtin = OPC_PRE_DEC;
      return (LEX_INC);
    }
    //if (c1 == '>') return (LEX_POINT); never gets here
    break;
  case '.':
    if (c1 == '.') {
      mark_ptr->builtin = OPC_DTYPE_RANGE;
      return (LEX_RANGE);
    }
    break;
  case '/':
    if (c1 == '/') {
      mark_ptr->builtin = OPC_CONCAT;
      return lex_bin_eq(LEX_CONCAT, TdiThreadStatic_p);
    }
    break;
  case '<':
    if (c1 == '<') {
      mark_ptr->builtin = OPC_SHIFT_LEFT;
      return lex_bin_eq(LEX_SHIFT, TdiThreadStatic_p);
    }
    if (c1 == '=') {
      mark_ptr->builtin = OPC_LE;
      return lex_bin_eq(LEX_LGES, TdiThreadStatic_p);
    }
    if (c1 == '>') {
      mark_ptr->builtin = OPC_NE;
      return lex_bin_eq(LEX_LEQS, TdiThreadStatic_p);
    }
    break;
  case '=':
    if (c1 == '=') {
      mark_ptr->builtin = OPC_EQ;
      return lex_bin_eq(LEX_LEQS, TdiThreadStatic_p);
    }
    break;
  case '>':
    if (c1 == '=') {
      mark_ptr->builtin = OPC_GE;
      return lex_bin_eq(LEX_LGES, TdiThreadStatic_p);
    }
    if (c1 == '>') {
      mark_ptr->builtin = OPC_SHIFT_RIGHT;
      return lex_bin_eq(LEX_SHIFT, TdiThreadStatic_p);
    }
    break;
  case '|':
    if (c1 == '|') {
      mark_ptr->builtin = OPC_OR;
      return lex_bin_eq(LEX_LORS, TdiThreadStatic_p);
    }
    break;
  }
  UNPUT();

	/********************
	One graphic operator.
	********************/
  switch (c0) {
  case '!':
    mark_ptr->builtin = OPC_NOT;
    return (LEX_UNARYS);
  case '%':
    mark_ptr->builtin = OPC_MOD;
    return lex_bin_eq(LEX_MULS, TdiThreadStatic_p);
  case '&':
    mark_ptr->builtin = OPC_IAND;
    return lex_bin_eq(LEX_IAND, TdiThreadStatic_p);
  case '*':
    mark_ptr->builtin = OPC_MULTIPLY;
    return lex_bin_eq('*', TdiThreadStatic_p);
  case '+':
    mark_ptr->builtin = OPC_ADD;
    return lex_bin_eq(LEX_ADD, TdiThreadStatic_p);
  case '-':
    mark_ptr->builtin = OPC_SUBTRACT;
    return lex_bin_eq(LEX_ADD, TdiThreadStatic_p);
  case '/':
    mark_ptr->builtin = OPC_DIVIDE;
    return lex_bin_eq(LEX_MULS, TdiThreadStatic_p);
  case ':':
    mark_ptr->builtin = OPC_DTYPE_RANGE;
    return (LEX_RANGE);
  case '<':
    mark_ptr->builtin = OPC_LT;
    return lex_bin_eq(LEX_LGES, TdiThreadStatic_p);
  case '>':
    mark_ptr->builtin = OPC_GT;
    return lex_bin_eq(LEX_LGES, TdiThreadStatic_p);
  case '@':
    mark_ptr->builtin = OPC_PROMOTE;
    return (LEX_PROMO);
  case '^':
    mark_ptr->builtin = OPC_POWER;
    return lex_bin_eq(LEX_POWER, TdiThreadStatic_p);
  case '|':
    mark_ptr->builtin = OPC_IOR;
    return lex_bin_eq(LEX_IOR, TdiThreadStatic_p);
  case '~':
    mark_ptr->builtin = OPC_INOT;
    return (LEX_UNARYS);
  }
  mark_ptr->builtin = -1;
  return (c0);
}

/*--------------------------------------------------------
	C-style text in matching quotes. Strict: must end in quote. Continuation: \ before newline.
	Limitation: Text is ASCII dependent in quotes.
	Limitation: No wide characters. L'\xabc'
	Code all ANSI C escapes.
	On \ followed by non-standard we remove \.
	NEED overflow check on octal and hex. Watch sign extend of char.
*/

static inline int lex_quote(
	int len __attribute__ ((unused)),
	char *str,
	struct marker *mark_ptr,
	ThreadStatic *const TdiThreadStatic_p) {
  char c, c1, *cptr = TDI_REFZONE.a_cur;
  int cur = 0, limit;

  while ((c = INPUT()) && c != str[0])
    if (c == '\\')
      INPUT();
  limit = TDI_REFZONE.a_cur - cptr - 1;
  TDI_REFZONE.a_cur = cptr;
  MAKE_S(DTYPE_T, limit, mark_ptr->rptr);
  mark_ptr->builtin = -1;
  while (cur <= limit) {
    if ((c = INPUT()) == str[0]) {
      mark_ptr->rptr->length = cur;
      return (LEX_TEXT);
    } else if (c == '\\') {
      c = INPUT();
      switch (c) {
      default:
	break;			/*non-standard */
      case '\n':
	continue;		/*continuation ignores end of line */
      case 'n':
	c = '\n';
	break;			/*NL/LF newline */
      case 't':
	c = '\t';
	break;			/*HT    horizontal tab */
      case 'v':
	c = '\v';
	break;			/*VT    vertical tab */
      case 'b':
	c = '\b';
	break;			/*BS    backspace */
      case 'r':
	c = '\r';
	break;			/*CR    carriage return */
      case 'f':
	c = '\f';
	break;			/*FF    formfeed */
      case 'a':
	c = '\007';
	break;			/*BEL   audible alert */
      case '\\':
	break;			/*backslash */
      case '\?':
	break;			/*question mark */
      case '\'':
	break;			/*single quote */
      case '\"':
	break;			/*double quote */
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
	c = c - '0';		/*octal number, 1-3 digits */
	if ((c1 = INPUT()) >= '0' && c1 <= '7')
	  c = (c << 3) | (c1 - '0');
	else
	  UNPUT();
	if ((c1 = INPUT()) >= '0' && c1 <= '7')
	  c = (c << 3) | (c1 - '0');
	else
	  UNPUT();
	break;
      case 'x':
	c = 0;			/*hex number, any number of digits */
	while ((c1 = INPUT())) {
	  if (c1 >= '0' && c1 <= '9')
	    c = (c << 4) | (c1 - '0');
	  else if (c1 >= 'A' && c1 <= 'F')
	    c = (c << 4) | (c1 - 'A' + 10);
	  else if (c1 >= 'a' && c1 <= 'f')
	    c = (c << 4) | (c1 - 'a' + 10);
	  else
	    break;
	}
	UNPUT();
      }
    } else if (c == '\0')
      break;
    mark_ptr->rptr->pointer[cur++] = c;
  }
  TDI_REFZONE.l_status = TdiUNBALANCE;
  return (LEX_ERROR);
}

static inline int lex_back(const int *p, const int m) {
  if (p == 0) return 0;
  while (*p) {
    if (*p++ == m)
      return 1;
  }
  return 0;
}

static int lex_look(int *leng, char *previous, char*const text, const svf_t **lstate, ThreadStatic *const TdiThreadStatic_p) {
  const svf_t *state, **lsp;
  const work_t *t;
  const svf_t *z;
  const work_t *r;
  int ch, first;
  char *lastch;
  /* start off machines */
  first = 1;
  lastch = text;
  for (;;) {
    lsp = lstate;
    state = bgin;
    if (*previous == NEWLINE)
      state++;
    for (;;) {
#ifdef LEXDEBUG
      fprintf(stdout, "state %d ", (int)(state - svec - 1));
#endif
      t = state->stoff;
      if (t == crank && !first) {	/* may not be any transitions */
	z = state->other;
	if (z == 0)
	  break;
	if (z->stoff == crank)
	  break;
      }
      *lastch++ = ch = INPUT();
      if (lastch >= text + (MAX_TOKEN_LEN - 1)) {
	fprintf(stdout, " ! Maximum token length exceeded\n");
	text[MAX_TOKEN_LEN - 1] = 0;
	return 0;
      }
      first = 0;
#define YYOPTIM
#ifdef YYOPTIM
 tryagain:
#endif
#ifdef LEXDEBUG
      fprintf(stdout, "char '%c' (%d)\n",(char)ch,ch);
#endif
      r = t;
      if (t > crank) {
	t = r + ch;
	if (t <= top && t->verify + svec == state) {
	  if (t->advance + svec == svec) {	/* error transitions */
	    --lastch;
	    UNPUT();
	    break;
	  }
	  *lsp++ = state = t->advance + svec;
	  goto contin;
	}
      } else if (t < crank) {	/* r < crank */
	t = r = crank + (crank - t);
#ifdef LEXDEBUG
	fprintf(stdout, "compressed state\n");
#endif
	t = t + ch;
	if (t <= top && t->verify + svec == state) {
	  if (t->advance + svec == svec) {	/* error transitions */
	    --lastch;
	    UNPUT();
	    break;
	  }
	  *lsp++ = state = t->advance + svec;
	  goto contin;
	}
	t = r + match[ch];
#ifdef LEXDEBUG
	fprintf(stdout, "try fall back character %d\n", match[ch]);
#endif
	if (t <= top && t->verify + svec == state) {
	  if (t->advance + svec == svec) {	/* error transition */
	    --lastch;
	    UNPUT();
	    break;
	  }
	  *lsp++ = state = t->advance + svec;
	  goto contin;
	}
      }
      if ((state = state->other)
	  && (t = state->stoff) != crank) {
#ifdef LEXDEBUG
	fprintf(stdout, "fall back to state %d\n", (int)(state - svec - 1));
#endif
	goto tryagain;
      } else {
	--lastch;
	UNPUT();
	break;
      }
 contin:;
#ifdef LEXDEBUG
      fprintf(stdout, "state %ld char '%c' (%d)\n", state - svec - 1, (char)ch,ch);
#endif
    }
#ifdef LEXDEBUG
    {
     int pos = *(lsp - 1) - svec - 1;
     if (pos<0)
       fprintf(stdout, "stopped EOF\n");
     else
       fprintf(stdout, "stopped at %d with '%c' (%d)\n\n", pos, (char)ch,ch);
    }
#endif
    while (lsp-- > lstate) {
      if ((*lsp-svec == 21)) lastch++;
      *lastch-- = 0;
      const int *fnd;
      if (*lsp != 0 && (fnd = (*lsp)->stops) && *fnd > 0) {
	if (extra[*fnd]) {	/* must backup */
	  while (lex_back((*lsp)->stops, -*fnd) != 1 && lsp > lstate) {
	    lsp--;
	    --lastch;
	    UNPUT();
	  }
	}
	*previous = *lastch;
	*leng = lastch - text + 1;
	if (*leng >= (MAX_TOKEN_LEN - 1)) {
	  fprintf(stdout, "Maximum token length exceeded\n");
	  text[MAX_TOKEN_LEN - 1] = 0;
	  return 0;
	}
	text[*leng] = 0;
#ifdef LEXDEBUG
	fprintf(stdout, "match \"%s\" action %d\n",text,*fnd);
#endif
	return (*fnd++);
      }
      --lastch;
      UNPUT();
    }
    if (text[0] == 0)
      return (0);
    *previous = text[0] = INPUT();
    lastch = text;
#ifdef LEXDEBUG
    fprintf(stdout, "\n");
#endif
  }
}

int tdi_lex(struct marker *lvalPtr) {
  GET_TDITHREADSTATIC_P;
  int nstr, leng;
  char previous = NEWLINE;
  char text[MAX_TOKEN_LEN];
  memset(text,127,MAX_TOKEN_LEN);
  const svf_t *lstate[MAX_TOKEN_LEN];
  while ((nstr = lex_look(&leng, &previous, text, lstate, TdiThreadStatic_p)) >= 0) {
    switch (nstr) {
      case 0:
	return 0;
      case 1:
	break;
      case 2:
	break;
      case 3:
	SET_POS();
	return (lex_float(leng, text, lvalPtr, TdiThreadStatic_p));
      case 4:
	SET_POS();
	return (lex_float(leng, text, lvalPtr, TdiThreadStatic_p));
      case 5:
	SET_POS();
	return (lex_integer(leng, text, lvalPtr, TdiThreadStatic_p));
      case 6:
	SET_POS();
	return (lex_ident(leng, text, lvalPtr, TdiThreadStatic_p));
      case 7:
	SET_POS();
	return (tdi_lex_path(leng, text, lvalPtr, TdiThreadStatic_p));
      case 8:
	SET_POS();
	return (lex_quote(leng, text, lvalPtr, TdiThreadStatic_p));
      case 9:
	SET_POS();
	return (lex_point(leng, text, lvalPtr, TdiThreadStatic_p));
      case 10:
	SET_POS();
	if (lex_comment(leng, text, lvalPtr, TdiThreadStatic_p))
	  return (LEX_ERROR);
	else
	  break;
      case 11:
	SET_POS();
	return (lex_punct(leng, text, lvalPtr, TdiThreadStatic_p));
      case -1:
	break;
      default:
	fprintf(stdout, "bad switch look %d", nstr);
    }
  }
  return 0;
}
