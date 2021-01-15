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
#include "mpb__decoder_gen.h"
EXPORT int mpb__decoder__add(struct descriptor *name_d_ptr, struct descriptor *dummy_d_ptr __attribute__ ((unused)), int *nid_ptr)
{
  static DESCRIPTOR(library_d, "MIT$DEVICES");
  static DESCRIPTOR(model_d, "MPB__DECODER");
  static DESCRIPTOR_CONGLOM(conglom_d, &library_d, &model_d, 0, 0);
  int usage = TreeUSAGE_DEVICE;
  int curr_nid, old_nid, head_nid, status;
  long int flags = NciM_WRITE_ONCE;
  NCI_ITM flag_itm[] = { {2, NciSET_FLAGS, 0, 0}, {0, 0, 0, 0} };
  char *name_ptr = strncpy(malloc(name_d_ptr->length + 1), name_d_ptr->pointer, name_d_ptr->length);
  flag_itm[0].pointer = (unsigned char *)&flags;
  name_ptr[name_d_ptr->length] = 0;
  status = TreeStartConglomerate(MPB__DECODER_K_CONG_NODES);
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
 ADD_NODE(:NAME, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:COMMENT, TreeUSAGE_TEXT)
#define expr " * : * : 1E-6	"
 ADD_NODE_EXPR(:CLOCK_OUT, TreeUSAGE_AXIS)
#undef expr
      flags |= NciM_WRITE_ONCE;
  status = TreeSetNci(curr_nid, flag_itm);
  ADD_NODE(.CHANNEL_0, TreeUSAGE_STRUCTURE)
 ADD_NODE_INTEGER(.CHANNEL_0:MODE, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_0:P1, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_0:P2, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_0:P3, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_0:P4, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_0:P5, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_0:P6, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
  ADD_NODE(.CHANNEL_0.START_LOW, TreeUSAGE_STRUCTURE)
 ADD_NODE(.CHANNEL_0:EDGES, TreeUSAGE_ANY)
 ADD_NODE(.CHANNEL_0:EDGES_R, TreeUSAGE_ANY)
 ADD_NODE(.CHANNEL_0:EDGES_F, TreeUSAGE_ANY)
 ADD_NODE(.CHANNEL_0:COMMENT, TreeUSAGE_TEXT)
      ADD_NODE(.CHANNEL_1, TreeUSAGE_STRUCTURE)
 ADD_NODE_INTEGER(.CHANNEL_1:MODE, 3, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_1:P1, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_1:P2, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_1:P3, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_1:P4, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_1:P5, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_1:P6, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
  ADD_NODE(.CHANNEL_1.START_LOW, TreeUSAGE_STRUCTURE)
 ADD_NODE(.CHANNEL_1:EDGES, TreeUSAGE_ANY)
 ADD_NODE(.CHANNEL_1:EDGES_R, TreeUSAGE_ANY)
 ADD_NODE(.CHANNEL_1:EDGES_F, TreeUSAGE_ANY)
 ADD_NODE(.CHANNEL_1:COMMENT, TreeUSAGE_TEXT)
      ADD_NODE(.CHANNEL_2, TreeUSAGE_STRUCTURE)
 ADD_NODE(.CHANNEL_2:MODE, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_2:P1, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_2:P2, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_2:P3, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_2:P4, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_2:P5, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_2:P6, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
  ADD_NODE(.CHANNEL_2.START_LOW, TreeUSAGE_STRUCTURE)
 ADD_NODE(.CHANNEL_2:EDGES, TreeUSAGE_ANY)
 ADD_NODE(.CHANNEL_2:EDGES_R, TreeUSAGE_ANY)
 ADD_NODE(.CHANNEL_2:EDGES_F, TreeUSAGE_ANY)
 ADD_NODE(.CHANNEL_2:COMMENT, TreeUSAGE_TEXT)
      ADD_NODE(.CHANNEL_3, TreeUSAGE_STRUCTURE)
 ADD_NODE(.CHANNEL_3:MODE, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_3:P1, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_3:P2, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_3:P3, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_3:P4, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_3:P5, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_3:P6, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
  ADD_NODE(.CHANNEL_3.START_LOW, TreeUSAGE_STRUCTURE)
 ADD_NODE(.CHANNEL_3:EDGES, TreeUSAGE_ANY)
 ADD_NODE(.CHANNEL_3:EDGES_R, TreeUSAGE_ANY)
 ADD_NODE(.CHANNEL_3:EDGES_F, TreeUSAGE_ANY)
 ADD_NODE(.CHANNEL_3:COMMENT, TreeUSAGE_TEXT)
      ADD_NODE(.CHANNEL_4, TreeUSAGE_STRUCTURE)
 ADD_NODE(.CHANNEL_4:MODE, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_4:P1, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_4:P2, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_4:P3, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_4:P4, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_4:P5, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(.CHANNEL_4:P6, TreeUSAGE_ANY)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
  ADD_NODE(.CHANNEL_4.START_LOW, TreeUSAGE_STRUCTURE)
 ADD_NODE(.CHANNEL_4:EDGES, TreeUSAGE_ANY)
 ADD_NODE(.CHANNEL_4:EDGES_R, TreeUSAGE_ANY)
 ADD_NODE(.CHANNEL_4:EDGES_F, TreeUSAGE_ANY)
 ADD_NODE(.CHANNEL_4:COMMENT, TreeUSAGE_TEXT)
#define expr " build_path('\\TSTART')	"
 ADD_NODE_EXPR(:START_TIME, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " ['START']	"
 ADD_NODE_EXPR(:START_EVENT, TreeUSAGE_TEXT)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_ACTION(:INIT_ACTION, INIT, INIT, 50, 0, 0, CAMAC_SERVER, 0)
      status = TreeEndConglomerate();
  if (!(status & 1))
    return status;
  return (TreeSetDefaultNid(old_nid));
}

EXPORT int mpb__decoder__part_name(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)),
			    struct descriptor_d *out_d)
{
  int element = 0, status;
  NCI_ITM nci_list[] = { {4, NciCONGLOMERATE_ELT, 0, 0}, {0, 0, 0, 0} };
  nci_list[0].pointer = (unsigned char *)&element;
  status = TreeGetNci(*(int *)nid_d_ptr->pointer, nci_list);
  if (!(status & 1))
    return status;
  switch (element) {
  case (MPB__DECODER_N_HEAD + 1):
    StrFree1Dx(out_d);
    break;
  case (MPB__DECODER_N_NAME + 1):
 COPY_PART_NAME(:NAME) break;
  case (MPB__DECODER_N_COMMENT + 1):
 COPY_PART_NAME(:COMMENT) break;
  case (MPB__DECODER_N_CLOCK_OUT + 1):
 COPY_PART_NAME(:CLOCK_OUT) break;
  case (MPB__DECODER_N_CHANNEL_0 + 1):
    COPY_PART_NAME(.CHANNEL_0) break;
  case (MPB__DECODER_N_CHANNEL_0_MODE + 1):
 COPY_PART_NAME(.CHANNEL_0:MODE) break;
  case (MPB__DECODER_N_CHANNEL_0_P1 + 1):
 COPY_PART_NAME(.CHANNEL_0:P1) break;
  case (MPB__DECODER_N_CHANNEL_0_P2 + 1):
 COPY_PART_NAME(.CHANNEL_0:P2) break;
  case (MPB__DECODER_N_CHANNEL_0_P3 + 1):
 COPY_PART_NAME(.CHANNEL_0:P3) break;
  case (MPB__DECODER_N_CHANNEL_0_P4 + 1):
 COPY_PART_NAME(.CHANNEL_0:P4) break;
  case (MPB__DECODER_N_CHANNEL_0_P5 + 1):
 COPY_PART_NAME(.CHANNEL_0:P5) break;
  case (MPB__DECODER_N_CHANNEL_0_P6 + 1):
 COPY_PART_NAME(.CHANNEL_0:P6) break;
  case (MPB__DECODER_N_CHANNEL_0_START_LOW + 1):
    COPY_PART_NAME(.CHANNEL_0.START_LOW) break;
  case (MPB__DECODER_N_CHANNEL_0_EDGES + 1):
 COPY_PART_NAME(.CHANNEL_0:EDGES) break;
  case (MPB__DECODER_N_CHANNEL_0_EDGES_R + 1):
 COPY_PART_NAME(.CHANNEL_0:EDGES_R) break;
  case (MPB__DECODER_N_CHANNEL_0_EDGES_F + 1):
 COPY_PART_NAME(.CHANNEL_0:EDGES_F) break;
  case (MPB__DECODER_N_CHANNEL_0_COMMENT + 1):
 COPY_PART_NAME(.CHANNEL_0:COMMENT) break;
  case (MPB__DECODER_N_CHANNEL_1 + 1):
    COPY_PART_NAME(.CHANNEL_1) break;
  case (MPB__DECODER_N_CHANNEL_1_MODE + 1):
 COPY_PART_NAME(.CHANNEL_1:MODE) break;
  case (MPB__DECODER_N_CHANNEL_1_P1 + 1):
 COPY_PART_NAME(.CHANNEL_1:P1) break;
  case (MPB__DECODER_N_CHANNEL_1_P2 + 1):
 COPY_PART_NAME(.CHANNEL_1:P2) break;
  case (MPB__DECODER_N_CHANNEL_1_P3 + 1):
 COPY_PART_NAME(.CHANNEL_1:P3) break;
  case (MPB__DECODER_N_CHANNEL_1_P4 + 1):
 COPY_PART_NAME(.CHANNEL_1:P4) break;
  case (MPB__DECODER_N_CHANNEL_1_P5 + 1):
 COPY_PART_NAME(.CHANNEL_1:P5) break;
  case (MPB__DECODER_N_CHANNEL_1_P6 + 1):
 COPY_PART_NAME(.CHANNEL_1:P6) break;
  case (MPB__DECODER_N_CHANNEL_1_START_LOW + 1):
    COPY_PART_NAME(.CHANNEL_1.START_LOW) break;
  case (MPB__DECODER_N_CHANNEL_1_EDGES + 1):
 COPY_PART_NAME(.CHANNEL_1:EDGES) break;
  case (MPB__DECODER_N_CHANNEL_1_EDGES_R + 1):
 COPY_PART_NAME(.CHANNEL_1:EDGES_R) break;
  case (MPB__DECODER_N_CHANNEL_1_EDGES_F + 1):
 COPY_PART_NAME(.CHANNEL_1:EDGES_F) break;
  case (MPB__DECODER_N_CHANNEL_1_COMMENT + 1):
 COPY_PART_NAME(.CHANNEL_1:COMMENT) break;
  case (MPB__DECODER_N_CHANNEL_2 + 1):
    COPY_PART_NAME(.CHANNEL_2) break;
  case (MPB__DECODER_N_CHANNEL_2_MODE + 1):
 COPY_PART_NAME(.CHANNEL_2:MODE) break;
  case (MPB__DECODER_N_CHANNEL_2_P1 + 1):
 COPY_PART_NAME(.CHANNEL_2:P1) break;
  case (MPB__DECODER_N_CHANNEL_2_P2 + 1):
 COPY_PART_NAME(.CHANNEL_2:P2) break;
  case (MPB__DECODER_N_CHANNEL_2_P3 + 1):
 COPY_PART_NAME(.CHANNEL_2:P3) break;
  case (MPB__DECODER_N_CHANNEL_2_P4 + 1):
 COPY_PART_NAME(.CHANNEL_2:P4) break;
  case (MPB__DECODER_N_CHANNEL_2_P5 + 1):
 COPY_PART_NAME(.CHANNEL_2:P5) break;
  case (MPB__DECODER_N_CHANNEL_2_P6 + 1):
 COPY_PART_NAME(.CHANNEL_2:P6) break;
  case (MPB__DECODER_N_CHANNEL_2_START_LOW + 1):
    COPY_PART_NAME(.CHANNEL_2.START_LOW) break;
  case (MPB__DECODER_N_CHANNEL_2_EDGES + 1):
 COPY_PART_NAME(.CHANNEL_2:EDGES) break;
  case (MPB__DECODER_N_CHANNEL_2_EDGES_R + 1):
 COPY_PART_NAME(.CHANNEL_2:EDGES_R) break;
  case (MPB__DECODER_N_CHANNEL_2_EDGES_F + 1):
 COPY_PART_NAME(.CHANNEL_2:EDGES_F) break;
  case (MPB__DECODER_N_CHANNEL_2_COMMENT + 1):
 COPY_PART_NAME(.CHANNEL_2:COMMENT) break;
  case (MPB__DECODER_N_CHANNEL_3 + 1):
    COPY_PART_NAME(.CHANNEL_3) break;
  case (MPB__DECODER_N_CHANNEL_3_MODE + 1):
 COPY_PART_NAME(.CHANNEL_3:MODE) break;
  case (MPB__DECODER_N_CHANNEL_3_P1 + 1):
 COPY_PART_NAME(.CHANNEL_3:P1) break;
  case (MPB__DECODER_N_CHANNEL_3_P2 + 1):
 COPY_PART_NAME(.CHANNEL_3:P2) break;
  case (MPB__DECODER_N_CHANNEL_3_P3 + 1):
 COPY_PART_NAME(.CHANNEL_3:P3) break;
  case (MPB__DECODER_N_CHANNEL_3_P4 + 1):
 COPY_PART_NAME(.CHANNEL_3:P4) break;
  case (MPB__DECODER_N_CHANNEL_3_P5 + 1):
 COPY_PART_NAME(.CHANNEL_3:P5) break;
  case (MPB__DECODER_N_CHANNEL_3_P6 + 1):
 COPY_PART_NAME(.CHANNEL_3:P6) break;
  case (MPB__DECODER_N_CHANNEL_3_START_LOW + 1):
    COPY_PART_NAME(.CHANNEL_3.START_LOW) break;
  case (MPB__DECODER_N_CHANNEL_3_EDGES + 1):
 COPY_PART_NAME(.CHANNEL_3:EDGES) break;
  case (MPB__DECODER_N_CHANNEL_3_EDGES_R + 1):
 COPY_PART_NAME(.CHANNEL_3:EDGES_R) break;
  case (MPB__DECODER_N_CHANNEL_3_EDGES_F + 1):
 COPY_PART_NAME(.CHANNEL_3:EDGES_F) break;
  case (MPB__DECODER_N_CHANNEL_3_COMMENT + 1):
 COPY_PART_NAME(.CHANNEL_3:COMMENT) break;
  case (MPB__DECODER_N_CHANNEL_4 + 1):
    COPY_PART_NAME(.CHANNEL_4) break;
  case (MPB__DECODER_N_CHANNEL_4_MODE + 1):
 COPY_PART_NAME(.CHANNEL_4:MODE) break;
  case (MPB__DECODER_N_CHANNEL_4_P1 + 1):
 COPY_PART_NAME(.CHANNEL_4:P1) break;
  case (MPB__DECODER_N_CHANNEL_4_P2 + 1):
 COPY_PART_NAME(.CHANNEL_4:P2) break;
  case (MPB__DECODER_N_CHANNEL_4_P3 + 1):
 COPY_PART_NAME(.CHANNEL_4:P3) break;
  case (MPB__DECODER_N_CHANNEL_4_P4 + 1):
 COPY_PART_NAME(.CHANNEL_4:P4) break;
  case (MPB__DECODER_N_CHANNEL_4_P5 + 1):
 COPY_PART_NAME(.CHANNEL_4:P5) break;
  case (MPB__DECODER_N_CHANNEL_4_P6 + 1):
 COPY_PART_NAME(.CHANNEL_4:P6) break;
  case (MPB__DECODER_N_CHANNEL_4_START_LOW + 1):
    COPY_PART_NAME(.CHANNEL_4.START_LOW) break;
  case (MPB__DECODER_N_CHANNEL_4_EDGES + 1):
 COPY_PART_NAME(.CHANNEL_4:EDGES) break;
  case (MPB__DECODER_N_CHANNEL_4_EDGES_R + 1):
 COPY_PART_NAME(.CHANNEL_4:EDGES_R) break;
  case (MPB__DECODER_N_CHANNEL_4_EDGES_F + 1):
 COPY_PART_NAME(.CHANNEL_4:EDGES_F) break;
  case (MPB__DECODER_N_CHANNEL_4_COMMENT + 1):
 COPY_PART_NAME(.CHANNEL_4:COMMENT) break;
  case (MPB__DECODER_N_START_TIME + 1):
 COPY_PART_NAME(:START_TIME) break;
  case (MPB__DECODER_N_START_EVENT + 1):
 COPY_PART_NAME(:START_EVENT) break;
  case (MPB__DECODER_N_INIT_ACTION + 1):
 COPY_PART_NAME(:INIT_ACTION) break;
  default:
    status = TreeILLEGAL_ITEM;
  }
  return status;
}

extern int mpb__decoder___init();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}
#define error(nid,code,code1) {free_xd_array return GenDeviceSignal(nid,code,code1);}

EXPORT int mpb__decoder__init(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InInitStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InInitStruct)

      read_string_error(MPB__DECODER_N_NAME, name, DEV$_BAD_NAME);
  status = mpb__decoder___init(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array
