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
#include <mitdevices_msg.h>
#include <mds_gendevice.h>
#include "umccd_gen.h"
EXPORT int umccd__add(struct descriptor *name_d_ptr, struct descriptor *dummy_d_ptr __attribute__ ((unused)), int *nid_ptr)
{
  static DESCRIPTOR(library_d, "MIT$DEVICES");
  static DESCRIPTOR(model_d, "UMCCD");
  static DESCRIPTOR_CONGLOM(conglom_d, &library_d, &model_d, 0, 0);
  int usage = TreeUSAGE_DEVICE;
  int curr_nid, old_nid, head_nid, status;
  long int flags = NciM_WRITE_ONCE;
  NCI_ITM flag_itm[] = { {2, NciSET_FLAGS, 0, 0}, {0, 0, 0, 0} };
  char *name_ptr = strncpy(malloc(name_d_ptr->length + 1), name_d_ptr->pointer, name_d_ptr->length);
  flag_itm[0].pointer = (unsigned char *)&flags;
  name_ptr[name_d_ptr->length] = 0;
  status = TreeStartConglomerate(UMCCD_K_CONG_NODES);
  if (!(status & 1))
    return status;
  status = TreeAddNode(name_ptr, &head_nid, usage);
  if (!(status & 1))
    return status;
  *nid_ptr = head_nid;
  status = TreeSetNci(head_nid, flag_itm);
  status = TreePutRecord(head_nid, (struct descriptor *)&conglom_d, 0);
  if (!(status & 1))
    return status;
  status = TreeGetDefaultNid(&old_nid);
  if (!(status & 1))
    return status;
  status = TreeSetDefaultNid(head_nid);
  if (!(status & 1))
    return status;
 ADD_NODE(:COMMENT, TreeUSAGE_TEXT)
 ADD_NODE(:DATA_FILE, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:GO_FILE, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:EXPOSURE, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:GRATNG_ALIGN, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:WAVELEN_2400, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:TRANS_2400, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:WAVELEN_600, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:TRANS_600, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:ROW_DESCRIP, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:SPECTRA, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:SPECTRA_TIME, TreeUSAGE_AXIS)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
  ADD_NODE(.FIBERS, TreeUSAGE_STRUCTURE)
#define expr " ['PORT','LOCATION','VIEW','NUMBER','FILTER']  "
 ADD_NODE_EXPR(.FIBERS:DIM, TreeUSAGE_TEXT)
#undef expr
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " BUILD_SIGNAL(['A','MID','NORM','1','4'],*,.FIBERS:DIM) "
 ADD_NODE_EXPR(.FIBERS:A, TreeUSAGE_SIGNAL)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " BUILD_SIGNAL(['A','MID','NORM','1','4'],*,.FIBERS:DIM) "
 ADD_NODE_EXPR(.FIBERS:B, TreeUSAGE_SIGNAL)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " BUILD_SIGNAL(['A','MID','NORM','1','4'],*,.FIBERS:DIM) "
 ADD_NODE_EXPR(.FIBERS:C, TreeUSAGE_SIGNAL)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " BUILD_SIGNAL(['A','MID','NORM','1','4'],*,.FIBERS:DIM) "
 ADD_NODE_EXPR(.FIBERS:D, TreeUSAGE_SIGNAL)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " BUILD_SIGNAL(['A','MID','NORM','1','4'],*,.FIBERS:DIM) "
 ADD_NODE_EXPR(.FIBERS:E, TreeUSAGE_SIGNAL)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " BUILD_SIGNAL(['A','MID','NORM','1','4'],*,.FIBERS:DIM) "
 ADD_NODE_EXPR(.FIBERS:F, TreeUSAGE_SIGNAL)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " BUILD_SIGNAL(['A','MID','NORM','1','4'],*,.FIBERS:DIM) "
 ADD_NODE_EXPR(.FIBERS:G, TreeUSAGE_SIGNAL)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " BUILD_SIGNAL(['A','MID','NORM','1','4'],*,.FIBERS:DIM) "
 ADD_NODE_EXPR(.FIBERS:H, TreeUSAGE_SIGNAL)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " BUILD_SIGNAL(['A','MID','NORM','1','4'],*,.FIBERS:DIM) "
 ADD_NODE_EXPR(.FIBERS:I, TreeUSAGE_SIGNAL)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " BUILD_SIGNAL(['A','MID','NORM','1','4'],*,.FIBERS:DIM) "
 ADD_NODE_EXPR(.FIBERS:J, TreeUSAGE_SIGNAL)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " BUILD_SIGNAL(['A','MID','NORM','1','4'],*,.FIBERS:DIM) "
 ADD_NODE_EXPR(.FIBERS:K, TreeUSAGE_SIGNAL)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " .FIBERS:A		"
 ADD_NODE_EXPR(:ROW_1, TreeUSAGE_SIGNAL)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " .FIBERS:B		"
 ADD_NODE_EXPR(:ROW_2, TreeUSAGE_SIGNAL)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " .FIBERS:C		"
 ADD_NODE_EXPR(:ROW_3, TreeUSAGE_SIGNAL)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " .FIBERS:D		"
 ADD_NODE_EXPR(:ROW_4, TreeUSAGE_SIGNAL)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " .FIBERS:E		"
 ADD_NODE_EXPR(:ROW_5, TreeUSAGE_SIGNAL)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " .FIBERS:F		"
 ADD_NODE_EXPR(:ROW_6, TreeUSAGE_SIGNAL)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
  ADD_NODE(.SETUP, TreeUSAGE_STRUCTURE)
 ADD_NODE_INTEGER(.SETUP:NUM_SPECTRA, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(.SETUP:EXPOSURE, 25, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(.SETUP:GRATNG_ALIGN, 24, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(.SETUP:WAVELEN_2400, 2060, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(.SETUP:TRANS_2400, 50, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(.SETUP:WAVELEN_600, 100, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(.SETUP:TRANS_600, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(.SETUP:DAQ_MODE, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
  ADD_NODE(.ANALYSIS, TreeUSAGE_STRUCTURE)
 ADD_NODE(.ANALYSIS:CAL_SHIFT, TreeUSAGE_NUMERIC)
 ADD_NODE(.ANALYSIS:H_D_RATIO, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
  ADD_NODE(.ANALYSIS.BALMER_LINES, TreeUSAGE_STRUCTURE)
 ADD_NODE(.ANALYSIS.BALMER_LINES:N_E_7, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.ANALYSIS.BALMER_LINES:N_E_8, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.ANALYSIS.BALMER_LINES:N_E_9, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.ANALYSIS.BALMER_LINES:N_E_10, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.ANALYSIS:TEMP_LINES, TreeUSAGE_NUMERIC)
 ADD_NODE(.ANALYSIS:TEMPATURE, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.ANALYSIS:FLOW, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.ANALYSIS:MAG_LINES, TreeUSAGE_NUMERIC)
 ADD_NODE(.ANALYSIS:MAG_FIELD, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_ACTION(:INIT_ACTION, REPLACE_THIS, INIT, 50, 0, 0, IDL_SERVER, 0)
 ADD_NODE_ACTION(:STORE_ACTION, REPLACE_THIS, STORE, 50, 0, 0, IDL_SERVER, 0)
 ADD_NODE_ACTION(:ANALY_ACTION, REPLACE_THIS, ANALYSIS, 50, 0, 0, SUBMIT_SERVER, 0)
      status = TreeEndConglomerate();
  if (!(status & 1))
    return status;
  return (TreeSetDefaultNid(old_nid));
}

EXPORT int umccd__part_name(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)),
		     struct descriptor_d *out_d)
{
  int element = 0, status;
  NCI_ITM nci_list[] = { {4, NciCONGLOMERATE_ELT, 0, 0}, {0, 0, 0, 0} };
  nci_list[0].pointer = (unsigned char *)&element;
  status = TreeGetNci(*(int *)nid_d_ptr->pointer, nci_list);
  if (!(status & 1))
    return status;
  switch (element) {
  case (UMCCD_N_HEAD + 1):
    StrFree1Dx(out_d);
    break;
  case (UMCCD_N_COMMENT + 1):
 COPY_PART_NAME(:COMMENT) break;
  case (UMCCD_N_DATA_FILE + 1):
 COPY_PART_NAME(:DATA_FILE) break;
  case (UMCCD_N_GO_FILE + 1):
 COPY_PART_NAME(:GO_FILE) break;
  case (UMCCD_N_EXPOSURE + 1):
 COPY_PART_NAME(:EXPOSURE) break;
  case (UMCCD_N_GRATNG_ALIGN + 1):
 COPY_PART_NAME(:GRATNG_ALIGN) break;
  case (UMCCD_N_WAVELEN_2400 + 1):
 COPY_PART_NAME(:WAVELEN_2400) break;
  case (UMCCD_N_TRANS_2400 + 1):
 COPY_PART_NAME(:TRANS_2400) break;
  case (UMCCD_N_WAVELEN_600 + 1):
 COPY_PART_NAME(:WAVELEN_600) break;
  case (UMCCD_N_TRANS_600 + 1):
 COPY_PART_NAME(:TRANS_600) break;
  case (UMCCD_N_ROW_DESCRIP + 1):
 COPY_PART_NAME(:ROW_DESCRIP) break;
  case (UMCCD_N_SPECTRA + 1):
 COPY_PART_NAME(:SPECTRA) break;
  case (UMCCD_N_SPECTRA_TIME + 1):
 COPY_PART_NAME(:SPECTRA_TIME) break;
  case (UMCCD_N_FIBERS + 1):
    COPY_PART_NAME(.FIBERS) break;
  case (UMCCD_N_FIBERS_DIM + 1):
 COPY_PART_NAME(.FIBERS:DIM) break;
  case (UMCCD_N_FIBERS_A + 1):
 COPY_PART_NAME(.FIBERS:A) break;
  case (UMCCD_N_FIBERS_B + 1):
 COPY_PART_NAME(.FIBERS:B) break;
  case (UMCCD_N_FIBERS_C + 1):
 COPY_PART_NAME(.FIBERS:C) break;
  case (UMCCD_N_FIBERS_D + 1):
 COPY_PART_NAME(.FIBERS:D) break;
  case (UMCCD_N_FIBERS_E + 1):
 COPY_PART_NAME(.FIBERS:E) break;
  case (UMCCD_N_FIBERS_F + 1):
 COPY_PART_NAME(.FIBERS:F) break;
  case (UMCCD_N_FIBERS_G + 1):
 COPY_PART_NAME(.FIBERS:G) break;
  case (UMCCD_N_FIBERS_H + 1):
 COPY_PART_NAME(.FIBERS:H) break;
  case (UMCCD_N_FIBERS_I + 1):
 COPY_PART_NAME(.FIBERS:I) break;
  case (UMCCD_N_FIBERS_J + 1):
 COPY_PART_NAME(.FIBERS:J) break;
  case (UMCCD_N_FIBERS_K + 1):
 COPY_PART_NAME(.FIBERS:K) break;
  case (UMCCD_N_ROW_1 + 1):
 COPY_PART_NAME(:ROW_1) break;
  case (UMCCD_N_ROW_2 + 1):
 COPY_PART_NAME(:ROW_2) break;
  case (UMCCD_N_ROW_3 + 1):
 COPY_PART_NAME(:ROW_3) break;
  case (UMCCD_N_ROW_4 + 1):
 COPY_PART_NAME(:ROW_4) break;
  case (UMCCD_N_ROW_5 + 1):
 COPY_PART_NAME(:ROW_5) break;
  case (UMCCD_N_ROW_6 + 1):
 COPY_PART_NAME(:ROW_6) break;
  case (UMCCD_N_SETUP + 1):
    COPY_PART_NAME(.SETUP) break;
  case (UMCCD_N_SETUP_NUM_SPECTRA + 1):
 COPY_PART_NAME(.SETUP:NUM_SPECTRA) break;
  case (UMCCD_N_SETUP_EXPOSURE + 1):
 COPY_PART_NAME(.SETUP:EXPOSURE) break;
  case (UMCCD_N_SETUP_GRATNG_ALIGN + 1):
 COPY_PART_NAME(.SETUP:GRATNG_ALIGN) break;
  case (UMCCD_N_SETUP_WAVELEN_2400 + 1):
 COPY_PART_NAME(.SETUP:WAVELEN_2400) break;
  case (UMCCD_N_SETUP_TRANS_2400 + 1):
 COPY_PART_NAME(.SETUP:TRANS_2400) break;
  case (UMCCD_N_SETUP_WAVELEN_600 + 1):
 COPY_PART_NAME(.SETUP:WAVELEN_600) break;
  case (UMCCD_N_SETUP_TRANS_600 + 1):
 COPY_PART_NAME(.SETUP:TRANS_600) break;
  case (UMCCD_N_SETUP_DAQ_MODE + 1):
 COPY_PART_NAME(.SETUP:DAQ_MODE) break;
  case (UMCCD_N_ANALYSIS + 1):
    COPY_PART_NAME(.ANALYSIS) break;
  case (UMCCD_N_ANALYSIS_CAL_SHIFT + 1):
 COPY_PART_NAME(.ANALYSIS:CAL_SHIFT) break;
  case (UMCCD_N_ANALYSIS_H_D_RATIO + 1):
 COPY_PART_NAME(.ANALYSIS:H_D_RATIO) break;
  case (UMCCD_N_ANALYSIS_BALMER_LINES + 1):
    COPY_PART_NAME(.ANALYSIS.BALMER_LINES) break;
  case (UMCCD_N_ANALYSIS_BALMER_LINES_N_E_7 + 1):
 COPY_PART_NAME(.ANALYSIS.BALMER_LINES:N_E_7) break;
  case (UMCCD_N_ANALYSIS_BALMER_LINES_N_E_8 + 1):
 COPY_PART_NAME(.ANALYSIS.BALMER_LINES:N_E_8) break;
  case (UMCCD_N_ANALYSIS_BALMER_LINES_N_E_9 + 1):
 COPY_PART_NAME(.ANALYSIS.BALMER_LINES:N_E_9) break;
  case (UMCCD_N_ANALYSIS_BALMER_LINES_N_E_10 + 1):
 COPY_PART_NAME(.ANALYSIS.BALMER_LINES:N_E_10) break;
  case (UMCCD_N_ANALYSIS_TEMP_LINES + 1):
 COPY_PART_NAME(.ANALYSIS:TEMP_LINES) break;
  case (UMCCD_N_ANALYSIS_TEMPATURE + 1):
 COPY_PART_NAME(.ANALYSIS:TEMPATURE) break;
  case (UMCCD_N_ANALYSIS_FLOW + 1):
 COPY_PART_NAME(.ANALYSIS:FLOW) break;
  case (UMCCD_N_ANALYSIS_MAG_LINES + 1):
 COPY_PART_NAME(.ANALYSIS:MAG_LINES) break;
  case (UMCCD_N_ANALYSIS_MAG_FIELD + 1):
 COPY_PART_NAME(.ANALYSIS:MAG_FIELD) break;
  case (UMCCD_N_INIT_ACTION + 1):
 COPY_PART_NAME(:INIT_ACTION) break;
  case (UMCCD_N_STORE_ACTION + 1):
 COPY_PART_NAME(:STORE_ACTION) break;
  case (UMCCD_N_ANALY_ACTION + 1):
 COPY_PART_NAME(:ANALY_ACTION) break;
  default:
    status = TreeILLEGAL_ITEM;
  }
  return status;
}
