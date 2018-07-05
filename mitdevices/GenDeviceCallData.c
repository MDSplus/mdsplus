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
/*------------------------------------------------------------------------------

	Name:	GEN_DEVICE$CALL_DATA

	Type:   C function

	Author:	Giulio Fregonese

	Date:   23-JAN-1990

	Purpose:    Calls TDI$DATA for INCAA CADF.

--------------------------------------------------------------------------------
	
Work done by 1999 Informatica Ricerca Sviluppo, s.c.r.l.,	
under contract from Istituto Gas Ionizzati del CNR - Padova (Italy)	
	
--------------------------------------------------------------------------------

 Call sequence:

    int status = GEN_DEVICE$CALL_DATA(int			    *mode_ptr;
				 int			    *cur_nid_ptr;
				 struct dsc$descriptor_xd   *setting_d_ptr);

 Input  arguments:

    mode_ptr:		mode of operation;	    (passed by reference)

    curr_nid_ptr:	current nid;		    (passed by reference)

 Output arguments:

    setting_d_ptr	data structure;		    (passed by descriptor)

 Return status:

    same as TREE$GET_RECORD and TDI$DATA

--------------------------------------------------------------------------------

 Description:

Calls TDI$DATA for INCAA CADF.

------------------------------------------------------------------------------*/

#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <mds_stdarg.h>

EXPORT int GenDeviceCallData(int mode, int cur_nid, struct descriptor_xd *setting_d_ptr)
{
/*------------------------------------------------------------------------------

 External functions or symbols referenced:				      */

/*------------------------------------------------------------------------------

 Macros:								      */

/*------------------------------------------------------------------------------

 Global variables:							      */

/*------------------------------------------------------------------------------

 Local variables:							      */

  register int status;
  struct descriptor_xd record_d = { 0, 0, CLASS_XD, 0, 0 };
  struct descriptor_xd temp_xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
  unsigned char type;
  // DESCRIPTOR_RANGE(range_d, 0, 0, 0);	/* for debug */
  DESCRIPTOR_FLOAT(float_d, 0);

/*------------------------------------------------------------------------------

 Executable:								      */

  status = TreeGetRecord(cur_nid, &record_d);
  if (!(status & 1))
    return (status);
  type = record_d.pointer->dtype;
  switch (mode) {
  case DevMODSTR:
    if (type != DTYPE_T)
      status = TdiData(record_d.pointer, setting_d_ptr MDS_END_ARG);
    else
      MdsCopyDxXd((struct descriptor *)&record_d, setting_d_ptr);
    if (~status & 1) {
      if ((setting_d_ptr) && (setting_d_ptr->pointer))
	setting_d_ptr->pointer->class = CLASS_D;
    } else {
      char zero = 0;
      struct descriptor null_str = { 1, DTYPE_T, CLASS_S, 0 };
      null_str.pointer = &zero;
      status = TdiConcat((struct descriptor *)setting_d_ptr, &null_str, setting_d_ptr MDS_END_ARG);
    }
    break;
  case DevMODINT:
    if (type != DTYPE_L)
      status = TdiLong(record_d.pointer, setting_d_ptr MDS_END_ARG);
    else
      MdsCopyDxXd((struct descriptor *)&record_d, setting_d_ptr);
    break;
  case DevMODFLO:
    if (type != DTYPE_F)
      status = TdiCvt(record_d.pointer, &float_d, setting_d_ptr MDS_END_ARG);
    else
      MdsCopyDxXd((struct descriptor *)&record_d, setting_d_ptr);
    break;
  case DevMODSLO:
    status = TdiEvaluate(record_d.pointer, setting_d_ptr MDS_END_ARG);
    if (status & 1) {
      if (((setting_d_ptr->pointer)->dtype != DTYPE_SLOPE)
	  && ((setting_d_ptr->pointer)->dtype != DTYPE_RANGE)) {
	MdsFree1Dx(setting_d_ptr, 0);
	status = DEV$_BADPARAM;
      }
    }
    break;
  case DevMODRAN:
    status = TdiEvaluate(record_d.pointer, &temp_xd MDS_END_ARG);
    if (status & 1) {
      if ((temp_xd.pointer)->dtype != DTYPE_RANGE) {
	MdsFree1Dx(&temp_xd, 0);
	status = DEV$_BADPARAM;
      } else {
	struct descriptor_xd begin_xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
	struct descriptor_xd end_xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
	struct descriptor_xd delta_xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
	DESCRIPTOR_RANGE(range_d, 0, 0, 0);

	status =
	    TdiCvt(((struct descriptor_range *)temp_xd.pointer)->begin, &float_d,
		   &begin_xd MDS_END_ARG);
	status =
	    TdiCvt(((struct descriptor_range *)temp_xd.pointer)->ending, &float_d,
		   &end_xd MDS_END_ARG);
	status =
	    TdiCvt(((struct descriptor_range *)temp_xd.pointer)->deltaval, &float_d,
		   &delta_xd MDS_END_ARG);
	range_d.begin = begin_xd.pointer;
	range_d.ending = end_xd.pointer;
	range_d.deltaval = delta_xd.pointer;
	status = MdsCopyDxXd((struct descriptor *)&range_d, setting_d_ptr);
	MdsFree1Dx(&temp_xd, 0);
	MdsFree1Dx(&begin_xd, 0);
	MdsFree1Dx(&end_xd, 0);
	MdsFree1Dx(&delta_xd, 0);
      }
    }
    break;
  case DevMODRANLON:
    status = TdiEvaluate(record_d.pointer, &temp_xd MDS_END_ARG);
    if (status & 1) {
      if ((temp_xd.pointer)->dtype != DTYPE_RANGE) {
	MdsFree1Dx(&temp_xd, 0);
	status = DEV$_BADPARAM;
      } else {
	struct descriptor_xd begin_xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
	struct descriptor_xd end_xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
	struct descriptor_xd delta_xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
	DESCRIPTOR_RANGE(range_d, 0, 0, 0);

	status =
	    TdiLong(((struct descriptor_range *)temp_xd.pointer)->begin, &begin_xd MDS_END_ARG);
	status = TdiLong(((struct descriptor_range *)temp_xd.pointer)->ending, &end_xd MDS_END_ARG);
	status =
	    TdiLong(((struct descriptor_range *)temp_xd.pointer)->deltaval, &delta_xd MDS_END_ARG);
	range_d.begin = begin_xd.pointer;
	range_d.ending = end_xd.pointer;
	range_d.deltaval = delta_xd.pointer;
	status = MdsCopyDxXd((struct descriptor *)&range_d, setting_d_ptr);
	MdsFree1Dx(&temp_xd, 0);
	MdsFree1Dx(&begin_xd, 0);
	MdsFree1Dx(&end_xd, 0);
	MdsFree1Dx(&delta_xd, 0);
      }
    }
    break;
  default:
    status = DEV$_BADPARAM;
  }
  MdsFree1Dx(&record_d, 0);
  return status;
}
