#include        <stdio.h>
#include        <mdsdescrip.h>
#include        <usagedef.h>

#define checkString(S)  if (id==S)  return #S ;


char  *MdsDtypeString( int  id )
   {
    static char  dtypeString[24];

    id = id & 0x00FF;
    checkString(DTYPE_MISSING)
    checkString(DTYPE_IDENT)
    checkString(DTYPE_NID)
    checkString(DTYPE_PATH)
    checkString(DTYPE_PARAM)
    checkString(DTYPE_SIGNAL)
    checkString(DTYPE_DIMENSION)
    checkString(DTYPE_WINDOW)
    checkString(DTYPE_SLOPE)
    checkString(DTYPE_FUNCTION)
    checkString(DTYPE_CONGLOM)
    checkString(DTYPE_RANGE)
    checkString(DTYPE_ACTION)
    checkString(DTYPE_DISPATCH)
    checkString(DTYPE_PROGRAM)
    checkString(DTYPE_ROUTINE)
    checkString(DTYPE_PROCEDURE)
    checkString(DTYPE_METHOD)
    checkString(DTYPE_DEPENDENCY)
    checkString(DTYPE_CONDITION)
    checkString(DTYPE_EVENT)
    checkString(DTYPE_WITH_UNITS)
    checkString(DTYPE_CALL)
    checkString(DTYPE_WITH_ERROR)
    checkString(DTYPE_Z)
    checkString(DTYPE_BU)
    checkString(DTYPE_WU)
    checkString(DTYPE_LU)
    checkString(DTYPE_QU)
    checkString(DTYPE_OU)
    checkString(DTYPE_B)
    checkString(DTYPE_W)
    checkString(DTYPE_L)
    checkString(DTYPE_Q)
    checkString(DTYPE_O)
    checkString(DTYPE_F)
    checkString(DTYPE_D)
    checkString(DTYPE_G)
    checkString(DTYPE_H)
    checkString(DTYPE_FC)
    checkString(DTYPE_DC)
    checkString(DTYPE_GC)
    checkString(DTYPE_HC)
    checkString(DTYPE_CIT)
    checkString(DTYPE_T)
    checkString(DTYPE_VT)
    checkString(DTYPE_NU)
    checkString(DTYPE_NL)
    checkString(DTYPE_NLO)
    checkString(DTYPE_NR)
    checkString(DTYPE_NRO)
    checkString(DTYPE_NZ)
    checkString(DTYPE_P)
    checkString(DTYPE_V)
    checkString(DTYPE_VU)
    checkString(DTYPE_FS)
    checkString(DTYPE_FT)
    checkString(DTYPE_FSC)
    checkString(DTYPE_FTC)
    checkString(DTYPE_ZI)
    checkString(DTYPE_ZEM)
    checkString(DTYPE_DSC)
    checkString(DTYPE_BPV)
    checkString(DTYPE_BLV)
    checkString(DTYPE_ADT)
    sprintf(dtypeString,"DTYPE_?_0x%02X",id);
    return(dtypeString);
   }



char  *MdsClassString( int  id )
   {
    static char  classString[24];

    id = id & 0x00FF;
    checkString(CLASS_XD)
    checkString(CLASS_XS)
    checkString(CLASS_R)
    checkString(CLASS_CA)
    checkString(CLASS_APD)
    checkString(CLASS_S)
    checkString(CLASS_D)
    checkString(CLASS_A)
    checkString(CLASS_P)
    checkString(CLASS_SD)
    checkString(CLASS_NCA)
    checkString(CLASS_VS)
    checkString(CLASS_VSA)
    checkString(CLASS_UBS)
    checkString(CLASS_UBA)
    checkString(CLASS_SB)
    checkString(CLASS_UBSB)
    sprintf(classString,"CLASS_?_0x%02X",id);
    return(classString);
   }

char *MdsUsageString( int id )
{
  static char  usageString[24];
  checkString(TreeUSAGE_ANY)       
  checkString(TreeUSAGE_STRUCTURE)
  checkString(TreeUSAGE_ACTION)
  checkString(TreeUSAGE_DEVICE)
  checkString(TreeUSAGE_DISPATCH)
  checkString(TreeUSAGE_NUMERIC)
  checkString(TreeUSAGE_SIGNAL)
  checkString(TreeUSAGE_TASK)
  checkString(TreeUSAGE_TEXT)
  checkString(TreeUSAGE_WINDOW)
  checkString(TreeUSAGE_AXIS)
  checkString(TreeUSAGE_SUBTREE)
  checkString(TreeUSAGE_COMPOUND_DATA)
  sprintf(usageString,"USAGE_?_0x%02X",id);
  return(usageString);
}
