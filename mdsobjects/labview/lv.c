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
 * name: lv.c
 *
 * purpose: provide dynamic image activation for LabView entry points
 *   needed by mdsobjects/labview, so that labview does not need to be
 *   installed in order to link this code.
 *
 *   Note: this works only if the LabView include files are available.
 *
 *   Josh Stillerman 10/19/12
 */
#include <mdsplus/mdsconfig.h>
#include <stdint.h>
#include  <platdefines.h>
#include <extcode.h>
#include <fundtypes.h>
#include <mdsdescrip.h>
#include <libroutines.h>
#include <stdio.h>
#include <mdsshr.h>

#pragma pack(1)

static void Initialize();

static DESCRIPTOR(LVMemoryManager_10Name, "LVMemoryManager_10");
static DESCRIPTOR(LVRT_10Name, "LVRT_10");

static DESCRIPTOR(DSNewHandleName, "DSNewHandle");
static DESCRIPTOR(MoveBlockName, "MoveBlock");
static DESCRIPTOR(NumericArrayResizeName, "NumericArrayResize");

void (*LVMoveBlock) () = 0;
UHandle(*LVDSNewHandle) () = 0;
MgErr(*LVNumericArrayResize) () = 0;

EXPORT extern void MoveBlock(const void *src, void *dest, size_t siz)
{
  if (!LVMoveBlock)
    Initialize();
  if (LVMoveBlock)
    LVMoveBlock(src, dest, siz);
}

EXPORT extern UHandle DSNewHandle(size_t siz)
{
  if (!LVDSNewHandle)
    Initialize();
  if (LVDSNewHandle)
    return LVDSNewHandle(siz);
  else
    return (UHandle) - 1;
}

EXPORT MgErr NumericArrayResizeCACCA(int32 a, int32 b, UHandle * h, size_t siz)
{
  printf("CIAO SONO NUMERIC ARRAY RESIZE CACCA\n");
  printf("\n\n\n\n");
  if (!LVNumericArrayResize)
    Initialize();
  if (LVNumericArrayResize)
    return LVNumericArrayResize(a, b, h, siz);
  else
    return -1;
}

EXPORT MgErr NumericArrayResize(int32 a, int32 b, UHandle * h, size_t siz)
{
  printf("CIAO SONO NUMERIC ARRAY RESIZE\n");
  printf("\n\n\n\n");
  if (!LVNumericArrayResize)
    Initialize();
  if (LVNumericArrayResize)
    return LVNumericArrayResize(a, b, h, siz);
  else
    return -1;
}

static void Initialize()
{
  int status;
  printf("CIAO SONO INITIALIZE\n");
  if (!LVDSNewHandle) {
    status = LibFindImageSymbol(&LVMemoryManager_10Name, &DSNewHandleName, &LVDSNewHandle);
    printf("FIND IMAGE SYMBOL status: %d %s\n", status, MdsGetMsg(status));
  }
  if (!LVMoveBlock)
    LibFindImageSymbol(&LVRT_10Name, &MoveBlockName, &LVMoveBlock);
  if (!LVNumericArrayResize)
    LibFindImageSymbol(&LVRT_10Name, &NumericArrayResizeName, &LVNumericArrayResize);
}
