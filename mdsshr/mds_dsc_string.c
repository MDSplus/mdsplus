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
#include        <stdio.h>
#include        <mdsdescrip.h>
#include        <usagedef.h>
#include        <STATICdef.h>
#include		<mdsshr.h>

#define checkString(S)  if (id==S)  return #S ;
#define checkAltStr(A, S)  if (id==A)  return #S ;

EXPORT char *MdsDtypeString(const unsigned char id)
{
  STATIC_THREADSAFE char dtypeString[24];	/* not really threadsafe but should be ok */

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
      checkString(DTYPE_LIST)
      checkString(DTYPE_TUPLE)
      checkString(DTYPE_DICTIONARY)
      checkString(DTYPE_OPAQUE)
      sprintf(dtypeString, "DTYPE_?_0x%02X", id);
  return (dtypeString);
}

EXPORT char *MdsClassString(const unsigned char id)
{
  STATIC_THREADSAFE char classString[24];	/* not really threadsafe but ok */

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
      sprintf(classString, "CLASS_?_0x%02X", id);
  return (classString);
}

EXPORT char *MdsUsageString(const unsigned char id)
{
  STATIC_THREADSAFE char usageString[24];	/* not really threadsafe but should be ok */
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
      checkAltStr(TreeUSAGE_SUBTREE_TOP, TreeUSAGE_SUBTREE)
      checkString(TreeUSAGE_COMPOUND_DATA)
      sprintf(usageString, "TreeUSAGE_?_0x%02X", id);
  return (usageString);
}
