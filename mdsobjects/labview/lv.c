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
#include <extcode.h>
/*
 #include <lv_prolog.h>
 #include <lv_epilog.h>
*/
#include <mdsdescrip.h>
#include <libroutines.h>

static void Initialize();

static DESCRIPTOR(LVMemoryManager_10Name,"LVMemoryManager_10");
static DESCRIPTOR(LVRT_10Name,"LVRT_10");

static DESCRIPTOR(DSNewHandleName,"DSNewHandle");
static DESCRIPTOR(MoveBlockName,"MoveBlock");
static DESCRIPTOR(NumericArrayResizeName,"NumericArrayResize");

void (*LVMoveBlock)() = 0;
UHandle (*LVDSNewHandle)() = 0;
MgErr (*LVNumericArrayResize)() = 0;

extern void MoveBlock(const void *src, void *dest, size_t siz)
{
  if (!LVMoveBlock)
    Initialize();
  if (LVMoveBlock)
    LVMoveBlock(src, dest, siz);
}

extern UHandle DSNewHandle(size_t siz)
{
  if (!LVDSNewHandle)
    Initialize();
  if (LVDSNewHandle)
    return LVDSNewHandle(siz);
  else
    return (UHandle)-1;
}
 
MgErr NumericArrayResize(int32 a, int32 b, UHandle* h, size_t siz)
{
  if (!LVNumericArrayResize)
    Initialize();
  if (LVNumericArrayResize)
    return LVNumericArrayResize(a, b, h, siz);
  else
    return -1;
}

static void Initialize()
{
  if (!LVDSNewHandle)
    LibFindImageSymbol(&LVMemoryManager_10Name, &DSNewHandleName, &LVDSNewHandle);
  if (!LVMoveBlock)
    LibFindImageSymbol(&LVRT_10Name, &MoveBlockName, &LVMoveBlock);
  if (!LVNumericArrayResize)
    LibFindImageSymbol(&LVRT_10Name, &NumericArrayResizeName, &LVNumericArrayResize);
}
