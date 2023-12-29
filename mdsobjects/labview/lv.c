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

// Prevents errors when using Clang on macOS with LabVIEW
#ifdef __APPLE__
#ifndef TARGET_OS_IPHONE
#define TARGET_OS_IPHONE 0
#endif
#endif

#include <extcode.h>
#include <fundtypes.h>
#include <libroutines.h>
#include <mdsdescrip.h>
#include <mdsplus/mdsconfig.h>
#include <mdsshr.h>
#include <platdefines.h>
#include <stdint.h>
#include <stdio.h>

#pragma pack(1)

EXPORT extern void MoveBlock(const void *src, void *dest, size_t siz)
{
  void (*LVMoveBlock)() = NULL;
  LibFindImageSymbol_C("LVRT_10", "MoveBlock", &LVMoveBlock);
  if (LVMoveBlock)
    LVMoveBlock(src, dest, siz);
}

EXPORT extern UHandle DSNewHandle(size_t siz)
{
  static UHandle (*LVDSNewHandle)() = NULL;
  LibFindImageSymbol_C("LVMemoryManager_10", "DSNewHandle", &LVDSNewHandle);
  if (LVDSNewHandle)
    return LVDSNewHandle(siz);
  return (UHandle)-1;
}

EXPORT MgErr NumericArrayResize(int32 a, int32 b, UHandle *h, size_t siz)
{
  static MgErr (*LVNumericArrayResize)() = NULL;
  LibFindImageSymbol_C("LVRT_10", "NumericArrayResize", &LVNumericArrayResize);
  if (LVNumericArrayResize)
    return LVNumericArrayResize(a, b, h, siz);
  return -1;
}

EXPORT MgErr NumericArrayResizeCACCA(int32 a, int32 b, UHandle *h, size_t siz)
{
  return NumericArrayResize(a, b, h, siz);
}
