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
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <stdio.h>
#include <usagedef.h>

#define checkString(S) \
  case (S):            \
    return #S;
#define checkAltStr(A, S) \
  case (A):               \
    return #S;

EXPORT char *MdsDtypeString(const dtype_t id)
{
  static char dtypeString[24]; /* not really threadsafe but should be ok */
  switch (id)
  {
#define DEFINE(name, ...) checkString(DTYPE_##name)
#include <dtypedef.h>
#undef DEFINE
  default:
    sprintf(dtypeString, "DTYPE_?_0x%02X", id);
    return (dtypeString);
  }
}

EXPORT char *MdsClassString(const class_t id)
{
  static char classString[24]; /* not really threadsafe but ok */
  switch (id)
  {
#define DEFINE(name, ...) checkString(CLASS_##name)
#include <classdef.h>
#undef DEFINE
  default:
    sprintf(classString, "CLASS_?_0x%02X", id);
    return (classString);
  }
}

EXPORT char *MdsUsageString(const usage_t id)
{
  static char usageString[24]; /* not really threadsafe but should be ok */
  switch (id)
  {
    checkString(TreeUSAGE_ANY) checkString(TreeUSAGE_STRUCTURE)
        checkString(TreeUSAGE_ACTION) checkString(TreeUSAGE_DEVICE) checkString(
            TreeUSAGE_DISPATCH) checkString(TreeUSAGE_NUMERIC)
            checkString(TreeUSAGE_SIGNAL) checkString(TreeUSAGE_TASK)
                checkString(TreeUSAGE_TEXT) checkString(TreeUSAGE_WINDOW)
                    checkString(TreeUSAGE_AXIS) checkString(TreeUSAGE_SUBTREE)
                        checkAltStr(TreeUSAGE_SUBTREE_REF, TreeUSAGE_SUBTREE)
                            checkAltStr(TreeUSAGE_SUBTREE_TOP,
                                        TreeUSAGE_SUBTREE)
                                checkString(TreeUSAGE_COMPOUND_DATA) default
        : sprintf(usageString, "TreeUSAGE_?_0x%02X", id);
    return usageString;
  }
}
