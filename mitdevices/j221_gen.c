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
#include "j221_gen.h"
extern int j221___add(int *nid);
EXPORT int j221__add(struct descriptor *name_d_ptr, struct descriptor *dummy_d_ptr __attribute__ ((unused)), int *nid_ptr)
{
  static DESCRIPTOR(library_d, "MIT$DEVICES");
  static DESCRIPTOR(model_d, "J221");
  static DESCRIPTOR_CONGLOM(conglom_d, &library_d, &model_d, 0, 0);
  int usage = TreeUSAGE_DEVICE;
  int curr_nid, old_nid, head_nid, status;
  long int flags = NciM_WRITE_ONCE;
  NCI_ITM flag_itm[] = { {2, NciSET_FLAGS, 0, 0}, {0, 0, 0, 0} };
  char *name_ptr = strncpy(malloc(name_d_ptr->length + 1), name_d_ptr->pointer, name_d_ptr->length);
  flag_itm[0].pointer = (unsigned char *)&flags;
  name_ptr[name_d_ptr->length] = 0;
  status = TreeStartConglomerate(J221_K_CONG_NODES);
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
 ADD_NODE(:CLOCK, TreeUSAGE_AXIS)
 ADD_NODE(:START_TRIG, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:CLOCK_DIVIDE, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:OUTPUT_01, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_01:SET_POINTS, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_01:GATES, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_01:TRIGGERS, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(OUTPUT_01:MODE, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:OUTPUT_02, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_02:SET_POINTS, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_02:GATES, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_02:TRIGGERS, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(OUTPUT_02:MODE, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:OUTPUT_03, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_03:SET_POINTS, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_03:GATES, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_03:TRIGGERS, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(OUTPUT_03:MODE, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:OUTPUT_04, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_04:SET_POINTS, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_04:GATES, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_04:TRIGGERS, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(OUTPUT_04:MODE, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:OUTPUT_05, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_05:SET_POINTS, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_05:GATES, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_05:TRIGGERS, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(OUTPUT_05:MODE, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:OUTPUT_06, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_06:SET_POINTS, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_06:GATES, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_06:TRIGGERS, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(OUTPUT_06:MODE, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:OUTPUT_07, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_07:SET_POINTS, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_07:GATES, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_07:TRIGGERS, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(OUTPUT_07:MODE, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:OUTPUT_08, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_08:SET_POINTS, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_08:GATES, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_08:TRIGGERS, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(OUTPUT_08:MODE, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:OUTPUT_09, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_09:SET_POINTS, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_09:GATES, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_09:TRIGGERS, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(OUTPUT_09:MODE, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:OUTPUT_10, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_10:SET_POINTS, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_10:GATES, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_10:TRIGGERS, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(OUTPUT_10:MODE, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:OUTPUT_11, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_11:SET_POINTS, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_11:GATES, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_11:TRIGGERS, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(OUTPUT_11:MODE, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:OUTPUT_12, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_12:SET_POINTS, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_12:GATES, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(OUTPUT_12:TRIGGERS, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(OUTPUT_12:MODE, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_ACTION(:INIT_ACTION, INIT, INIT, 50, 0, 0, CAMAC_SERVER, 0)
      status = j221___add(&head_nid);
  status = TreeEndConglomerate();
  if (!(status & 1))
    return status;
  return (TreeSetDefaultNid(old_nid));
}

EXPORT int j221__part_name(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)),
		    struct descriptor_d *out_d)
{
  int element = 0, status;
  NCI_ITM nci_list[] = { {4, NciCONGLOMERATE_ELT, 0, 0}, {0, 0, 0, 0} };
  nci_list[0].pointer = (unsigned char *)&element;
  status = TreeGetNci(*(int *)nid_d_ptr->pointer, nci_list);
  if (!(status & 1))
    return status;
  switch (element) {
  case (J221_N_HEAD + 1):
    StrFree1Dx(out_d);
    break;
  case (J221_N_NAME + 1):
 COPY_PART_NAME(:NAME) break;
  case (J221_N_COMMENT + 1):
 COPY_PART_NAME(:COMMENT) break;
  case (J221_N_CLOCK + 1):
 COPY_PART_NAME(:CLOCK) break;
  case (J221_N_START_TRIG + 1):
 COPY_PART_NAME(:START_TRIG) break;
  case (J221_N_CLOCK_DIVIDE + 1):
 COPY_PART_NAME(:CLOCK_DIVIDE) break;
  case (J221_N_OUTPUT_01 + 1):
 COPY_PART_NAME(:OUTPUT_01) break;
  case (J221_N_OUTPUT_01_SET_POINTS + 1):
 COPY_PART_NAME(OUTPUT_01:SET_POINTS) break;
  case (J221_N_OUTPUT_01_GATES + 1):
 COPY_PART_NAME(OUTPUT_01:GATES) break;
  case (J221_N_OUTPUT_01_TRIGGERS + 1):
 COPY_PART_NAME(OUTPUT_01:TRIGGERS) break;
  case (J221_N_OUTPUT_01_MODE + 1):
 COPY_PART_NAME(OUTPUT_01:MODE) break;
  case (J221_N_OUTPUT_02 + 1):
 COPY_PART_NAME(:OUTPUT_02) break;
  case (J221_N_OUTPUT_02_SET_POINTS + 1):
 COPY_PART_NAME(OUTPUT_02:SET_POINTS) break;
  case (J221_N_OUTPUT_02_GATES + 1):
 COPY_PART_NAME(OUTPUT_02:GATES) break;
  case (J221_N_OUTPUT_02_TRIGGERS + 1):
 COPY_PART_NAME(OUTPUT_02:TRIGGERS) break;
  case (J221_N_OUTPUT_02_MODE + 1):
 COPY_PART_NAME(OUTPUT_02:MODE) break;
  case (J221_N_OUTPUT_03 + 1):
 COPY_PART_NAME(:OUTPUT_03) break;
  case (J221_N_OUTPUT_03_SET_POINTS + 1):
 COPY_PART_NAME(OUTPUT_03:SET_POINTS) break;
  case (J221_N_OUTPUT_03_GATES + 1):
 COPY_PART_NAME(OUTPUT_03:GATES) break;
  case (J221_N_OUTPUT_03_TRIGGERS + 1):
 COPY_PART_NAME(OUTPUT_03:TRIGGERS) break;
  case (J221_N_OUTPUT_03_MODE + 1):
 COPY_PART_NAME(OUTPUT_03:MODE) break;
  case (J221_N_OUTPUT_04 + 1):
 COPY_PART_NAME(:OUTPUT_04) break;
  case (J221_N_OUTPUT_04_SET_POINTS + 1):
 COPY_PART_NAME(OUTPUT_04:SET_POINTS) break;
  case (J221_N_OUTPUT_04_GATES + 1):
 COPY_PART_NAME(OUTPUT_04:GATES) break;
  case (J221_N_OUTPUT_04_TRIGGERS + 1):
 COPY_PART_NAME(OUTPUT_04:TRIGGERS) break;
  case (J221_N_OUTPUT_04_MODE + 1):
 COPY_PART_NAME(OUTPUT_04:MODE) break;
  case (J221_N_OUTPUT_05 + 1):
 COPY_PART_NAME(:OUTPUT_05) break;
  case (J221_N_OUTPUT_05_SET_POINTS + 1):
 COPY_PART_NAME(OUTPUT_05:SET_POINTS) break;
  case (J221_N_OUTPUT_05_GATES + 1):
 COPY_PART_NAME(OUTPUT_05:GATES) break;
  case (J221_N_OUTPUT_05_TRIGGERS + 1):
 COPY_PART_NAME(OUTPUT_05:TRIGGERS) break;
  case (J221_N_OUTPUT_05_MODE + 1):
 COPY_PART_NAME(OUTPUT_05:MODE) break;
  case (J221_N_OUTPUT_06 + 1):
 COPY_PART_NAME(:OUTPUT_06) break;
  case (J221_N_OUTPUT_06_SET_POINTS + 1):
 COPY_PART_NAME(OUTPUT_06:SET_POINTS) break;
  case (J221_N_OUTPUT_06_GATES + 1):
 COPY_PART_NAME(OUTPUT_06:GATES) break;
  case (J221_N_OUTPUT_06_TRIGGERS + 1):
 COPY_PART_NAME(OUTPUT_06:TRIGGERS) break;
  case (J221_N_OUTPUT_06_MODE + 1):
 COPY_PART_NAME(OUTPUT_06:MODE) break;
  case (J221_N_OUTPUT_07 + 1):
 COPY_PART_NAME(:OUTPUT_07) break;
  case (J221_N_OUTPUT_07_SET_POINTS + 1):
 COPY_PART_NAME(OUTPUT_07:SET_POINTS) break;
  case (J221_N_OUTPUT_07_GATES + 1):
 COPY_PART_NAME(OUTPUT_07:GATES) break;
  case (J221_N_OUTPUT_07_TRIGGERS + 1):
 COPY_PART_NAME(OUTPUT_07:TRIGGERS) break;
  case (J221_N_OUTPUT_07_MODE + 1):
 COPY_PART_NAME(OUTPUT_07:MODE) break;
  case (J221_N_OUTPUT_08 + 1):
 COPY_PART_NAME(:OUTPUT_08) break;
  case (J221_N_OUTPUT_08_SET_POINTS + 1):
 COPY_PART_NAME(OUTPUT_08:SET_POINTS) break;
  case (J221_N_OUTPUT_08_GATES + 1):
 COPY_PART_NAME(OUTPUT_08:GATES) break;
  case (J221_N_OUTPUT_08_TRIGGERS + 1):
 COPY_PART_NAME(OUTPUT_08:TRIGGERS) break;
  case (J221_N_OUTPUT_08_MODE + 1):
 COPY_PART_NAME(OUTPUT_08:MODE) break;
  case (J221_N_OUTPUT_09 + 1):
 COPY_PART_NAME(:OUTPUT_09) break;
  case (J221_N_OUTPUT_09_SET_POINTS + 1):
 COPY_PART_NAME(OUTPUT_09:SET_POINTS) break;
  case (J221_N_OUTPUT_09_GATES + 1):
 COPY_PART_NAME(OUTPUT_09:GATES) break;
  case (J221_N_OUTPUT_09_TRIGGERS + 1):
 COPY_PART_NAME(OUTPUT_09:TRIGGERS) break;
  case (J221_N_OUTPUT_09_MODE + 1):
 COPY_PART_NAME(OUTPUT_09:MODE) break;
  case (J221_N_OUTPUT_10 + 1):
 COPY_PART_NAME(:OUTPUT_10) break;
  case (J221_N_OUTPUT_10_SET_POINTS + 1):
 COPY_PART_NAME(OUTPUT_10:SET_POINTS) break;
  case (J221_N_OUTPUT_10_GATES + 1):
 COPY_PART_NAME(OUTPUT_10:GATES) break;
  case (J221_N_OUTPUT_10_TRIGGERS + 1):
 COPY_PART_NAME(OUTPUT_10:TRIGGERS) break;
  case (J221_N_OUTPUT_10_MODE + 1):
 COPY_PART_NAME(OUTPUT_10:MODE) break;
  case (J221_N_OUTPUT_11 + 1):
 COPY_PART_NAME(:OUTPUT_11) break;
  case (J221_N_OUTPUT_11_SET_POINTS + 1):
 COPY_PART_NAME(OUTPUT_11:SET_POINTS) break;
  case (J221_N_OUTPUT_11_GATES + 1):
 COPY_PART_NAME(OUTPUT_11:GATES) break;
  case (J221_N_OUTPUT_11_TRIGGERS + 1):
 COPY_PART_NAME(OUTPUT_11:TRIGGERS) break;
  case (J221_N_OUTPUT_11_MODE + 1):
 COPY_PART_NAME(OUTPUT_11:MODE) break;
  case (J221_N_OUTPUT_12 + 1):
 COPY_PART_NAME(:OUTPUT_12) break;
  case (J221_N_OUTPUT_12_SET_POINTS + 1):
 COPY_PART_NAME(OUTPUT_12:SET_POINTS) break;
  case (J221_N_OUTPUT_12_GATES + 1):
 COPY_PART_NAME(OUTPUT_12:GATES) break;
  case (J221_N_OUTPUT_12_TRIGGERS + 1):
 COPY_PART_NAME(OUTPUT_12:TRIGGERS) break;
  case (J221_N_OUTPUT_12_MODE + 1):
 COPY_PART_NAME(OUTPUT_12:MODE) break;
  case (J221_N_INIT_ACTION + 1):
 COPY_PART_NAME(:INIT_ACTION) break;
  default:
    status = TreeILLEGAL_ITEM;
  }
  return status;
}

extern int j221___init();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}
#define error(nid,code,code1) {free_xd_array return GenDeviceSignal(nid,code,code1);}

EXPORT int j221__init(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InInitStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InInitStruct)

      read_string_error(J221_N_NAME, name, DEV$_BAD_NAME);
  status = j221___init(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array

extern int j221___trigger();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}

EXPORT int j221__trigger(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InTriggerStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InTriggerStruct)

      read_string_error(J221_N_NAME, name, DEV$_BAD_NAME);
  status = j221___trigger(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array
