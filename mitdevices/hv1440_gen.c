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
#include "hv1440_gen.h"
EXPORT int hv1440__add(struct descriptor *name_d_ptr, struct descriptor *dummy_d_ptr __attribute__ ((unused)), int *nid_ptr)
{
  static DESCRIPTOR(library_d, "MIT$DEVICES");
  static DESCRIPTOR(model_d, "HV1440");
  static DESCRIPTOR_CONGLOM(conglom_d, &library_d, &model_d, 0, 0);
  int usage = TreeUSAGE_DEVICE;
  int curr_nid, old_nid, head_nid, status;
  long int flags = NciM_WRITE_ONCE;
  NCI_ITM flag_itm[] = { {2, NciSET_FLAGS, 0, 0}, {0, 0, 0, 0} };
  char *name_ptr = strncpy(malloc(name_d_ptr->length + 1), name_d_ptr->pointer, name_d_ptr->length);
  flag_itm[0].pointer = (unsigned char *)&flags;
  name_ptr[name_d_ptr->length] = 0;
  status = TreeStartConglomerate(HV1440_K_CONG_NODES);
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
 ADD_NODE(:COMMENT, TreeUSAGE_TEXT)
#define expr " 1.0	"
 ADD_NODE_EXPR(:RANGE, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:POD_01, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:POD_02, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:POD_03, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:POD_04, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:POD_05, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:POD_06, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:POD_07, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:POD_08, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:POD_09, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:POD_10, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:POD_11, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:POD_12, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:POD_13, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:POD_14, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:POD_15, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:POD_16, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:READOUT, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:FRAME, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_ACTION(:INIT_ACTION, INIT, INIT, 50, 0, 0, CAMAC_SERVER, 0)
 ADD_NODE_ACTION(:STORE_ACTION, STORE, STORE, 50, 0, 0, CAMAC_SERVER, 0)
 ADD_NODE_ACTION(:ON_ACTION, ON, INIT, 50, 0, 0, CAMAC_SERVER, 0)
 ADD_NODE_ACTION(:OFF_ACTION, OFF, STORE, 50, 0, 0, CAMAC_SERVER, 0)
      status = TreeEndConglomerate();
  if (!(status & 1))
    return status;
  return (TreeSetDefaultNid(old_nid));
}

EXPORT int hv1440__part_name(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)),
		      struct descriptor_d *out_d)
{
  int element = 0, status;
  NCI_ITM nci_list[] = { {4, NciCONGLOMERATE_ELT, 0, 0}, {0, 0, 0, 0} };
  nci_list[0].pointer = (unsigned char *)&element;
  status = TreeGetNci(*(int *)nid_d_ptr->pointer, nci_list);
  if (!(status & 1))
    return status;
  switch (element) {
  case (HV1440_N_HEAD + 1):
    StrFree1Dx(out_d);
    break;
  case (HV1440_N_NAME + 1):
 COPY_PART_NAME(:NAME) break;
  case (HV1440_N_COMMENT + 1):
 COPY_PART_NAME(:COMMENT) break;
  case (HV1440_N_RANGE + 1):
 COPY_PART_NAME(:RANGE) break;
  case (HV1440_N_POD_01 + 1):
 COPY_PART_NAME(:POD_01) break;
  case (HV1440_N_POD_02 + 1):
 COPY_PART_NAME(:POD_02) break;
  case (HV1440_N_POD_03 + 1):
 COPY_PART_NAME(:POD_03) break;
  case (HV1440_N_POD_04 + 1):
 COPY_PART_NAME(:POD_04) break;
  case (HV1440_N_POD_05 + 1):
 COPY_PART_NAME(:POD_05) break;
  case (HV1440_N_POD_06 + 1):
 COPY_PART_NAME(:POD_06) break;
  case (HV1440_N_POD_07 + 1):
 COPY_PART_NAME(:POD_07) break;
  case (HV1440_N_POD_08 + 1):
 COPY_PART_NAME(:POD_08) break;
  case (HV1440_N_POD_09 + 1):
 COPY_PART_NAME(:POD_09) break;
  case (HV1440_N_POD_10 + 1):
 COPY_PART_NAME(:POD_10) break;
  case (HV1440_N_POD_11 + 1):
 COPY_PART_NAME(:POD_11) break;
  case (HV1440_N_POD_12 + 1):
 COPY_PART_NAME(:POD_12) break;
  case (HV1440_N_POD_13 + 1):
 COPY_PART_NAME(:POD_13) break;
  case (HV1440_N_POD_14 + 1):
 COPY_PART_NAME(:POD_14) break;
  case (HV1440_N_POD_15 + 1):
 COPY_PART_NAME(:POD_15) break;
  case (HV1440_N_POD_16 + 1):
 COPY_PART_NAME(:POD_16) break;
  case (HV1440_N_READOUT + 1):
 COPY_PART_NAME(:READOUT) break;
  case (HV1440_N_FRAME + 1):
 COPY_PART_NAME(:FRAME) break;
  case (HV1440_N_INIT_ACTION + 1):
 COPY_PART_NAME(:INIT_ACTION) break;
  case (HV1440_N_STORE_ACTION + 1):
 COPY_PART_NAME(:STORE_ACTION) break;
  case (HV1440_N_ON_ACTION + 1):
 COPY_PART_NAME(:ON_ACTION) break;
  case (HV1440_N_OFF_ACTION + 1):
 COPY_PART_NAME(:OFF_ACTION) break;
  default:
    status = TreeILLEGAL_ITEM;
  }
  return status;
}

extern int hv1440___init();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}
#define error(nid,code,code1) {free_xd_array return GenDeviceSignal(nid,code,code1);}

EXPORT int hv1440__init(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InInitStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InInitStruct)

      read_string_error(HV1440_N_NAME, name, DEV$_BAD_NAME);
  read_integer_error(HV1440_N_FRAME, frame, HV1440$_BAD_FRAME);
  read_float_error(HV1440_N_RANGE, range, HV1440$_BAD_RANGE);
  status = hv1440___init(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array

extern int hv1440___store();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}

EXPORT int hv1440__store(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InStoreStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InStoreStruct)

      read_string_error(HV1440_N_NAME, name, DEV$_BAD_NAME);
  read_integer_error(HV1440_N_FRAME, frame, HV1440$_BAD_FRAME);
  read_float_error(HV1440_N_RANGE, range, HV1440$_BAD_RANGE);
  status = hv1440___store(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array

extern int hv1440___on();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}

EXPORT int hv1440__on(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InOnStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InOnStruct)

      read_string_error(HV1440_N_NAME, name, DEV$_BAD_NAME);
  status = hv1440___on(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array

extern int hv1440___off();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}

EXPORT int hv1440__off(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InOffStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InOffStruct)

      read_string_error(HV1440_N_NAME, name, DEV$_BAD_NAME);
  status = hv1440___off(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array
