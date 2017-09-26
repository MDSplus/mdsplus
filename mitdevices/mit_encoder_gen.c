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
#include "mit_encoder_gen.h"
EXPORT int mit_encoder__add(struct descriptor *name_d_ptr, struct descriptor *dummy_d_ptr __attribute__ ((unused)), int *nid_ptr)
{
  static DESCRIPTOR(library_d, "MIT$DEVICES");
  static DESCRIPTOR(model_d, "MIT_ENCODER");
  static DESCRIPTOR_CONGLOM(conglom_d, &library_d, &model_d, 0, 0);
  int usage = TreeUSAGE_DEVICE;
  int curr_nid, old_nid, head_nid, status;
  long int flags = NciM_WRITE_ONCE;
  NCI_ITM flag_itm[] = { {2, NciSET_FLAGS, 0, 0}, {0, 0, 0, 0} };
  char *name_ptr = strncpy(malloc(name_d_ptr->length + 1), name_d_ptr->pointer, name_d_ptr->length);
  flag_itm[0].pointer = (unsigned char *)&flags;
  name_ptr[name_d_ptr->length] = 0;
  status = TreeStartConglomerate(MIT_ENCODER_K_CONG_NODES);
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
 ADD_NODE_ACTION(:INIT_ACTION, INIT, INIT, 50, 0, 0, CAMAC_SERVER, 0)
 ADD_NODE(:CHANNEL_1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_1:EVENT, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:CHANNEL_2, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_2:EVENT, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:CHANNEL_3, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_3:EVENT, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:CHANNEL_4, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_4:EVENT, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:CHANNEL_5, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_5:EVENT, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:CHANNEL_6, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_6:EVENT, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:SOFT_CHANNEL, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(SOFT_CHANNEL:EVENT, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
  status = TreeEndConglomerate();
  if (!(status & 1))
    return status;
  return (TreeSetDefaultNid(old_nid));
}

EXPORT int mit_encoder__part_name(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)),
			   struct descriptor_d *out_d)
{
  int element = 0, status;
  NCI_ITM nci_list[] = { {4, NciCONGLOMERATE_ELT, 0, 0}, {0, 0, 0, 0} };
  nci_list[0].pointer = (unsigned char *)&element;
  status = TreeGetNci(*(int *)nid_d_ptr->pointer, nci_list);
  if (!(status & 1))
    return status;
  switch (element) {
  case (MIT_ENCODER_N_HEAD + 1):
    StrFree1Dx(out_d);
    break;
  case (MIT_ENCODER_N_NAME + 1):
 COPY_PART_NAME(:NAME) break;
  case (MIT_ENCODER_N_COMMENT + 1):
 COPY_PART_NAME(:COMMENT) break;
  case (MIT_ENCODER_N_INIT_ACTION + 1):
 COPY_PART_NAME(:INIT_ACTION) break;
  case (MIT_ENCODER_N_CHANNEL_1 + 1):
 COPY_PART_NAME(:CHANNEL_1) break;
  case (MIT_ENCODER_N_CHANNEL_1_EVENT + 1):
 COPY_PART_NAME(CHANNEL_1:EVENT) break;
  case (MIT_ENCODER_N_CHANNEL_2 + 1):
 COPY_PART_NAME(:CHANNEL_2) break;
  case (MIT_ENCODER_N_CHANNEL_2_EVENT + 1):
 COPY_PART_NAME(CHANNEL_2:EVENT) break;
  case (MIT_ENCODER_N_CHANNEL_3 + 1):
 COPY_PART_NAME(:CHANNEL_3) break;
  case (MIT_ENCODER_N_CHANNEL_3_EVENT + 1):
 COPY_PART_NAME(CHANNEL_3:EVENT) break;
  case (MIT_ENCODER_N_CHANNEL_4 + 1):
 COPY_PART_NAME(:CHANNEL_4) break;
  case (MIT_ENCODER_N_CHANNEL_4_EVENT + 1):
 COPY_PART_NAME(CHANNEL_4:EVENT) break;
  case (MIT_ENCODER_N_CHANNEL_5 + 1):
 COPY_PART_NAME(:CHANNEL_5) break;
  case (MIT_ENCODER_N_CHANNEL_5_EVENT + 1):
 COPY_PART_NAME(CHANNEL_5:EVENT) break;
  case (MIT_ENCODER_N_CHANNEL_6 + 1):
 COPY_PART_NAME(:CHANNEL_6) break;
  case (MIT_ENCODER_N_CHANNEL_6_EVENT + 1):
 COPY_PART_NAME(CHANNEL_6:EVENT) break;
  case (MIT_ENCODER_N_SOFT_CHANNEL + 1):
 COPY_PART_NAME(:SOFT_CHANNEL) break;
  case (MIT_ENCODER_N_SOFT_CHANNEL_EVENT + 1):
 COPY_PART_NAME(SOFT_CHANNEL:EVENT) break;
  default:
    status = TreeILLEGAL_ITEM;
  }
  return status;
}

extern int mit_encoder___init();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}
#define error(nid,code,code1) {free_xd_array return GenDeviceSignal(nid,code,code1);}

EXPORT int mit_encoder__init(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InInitStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InInitStruct)

      read_string_error(MIT_ENCODER_N_NAME, name, DEV$_BAD_NAME);
  status = mit_encoder___init(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array

extern int mit_encoder___trigger();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}

EXPORT int mit_encoder__trigger(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InTriggerStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InTriggerStruct)

      read_string_error(MIT_ENCODER_N_NAME, name, DEV$_BAD_NAME);
  status = mit_encoder___trigger(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array

extern int mit_encoder___set_event();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}

int mit_encoder___set_event(struct descriptor *nid_d_ptr __attribute__ ((unused)), InSet_eventStruct * in_ptr)
{
  declare_variables(InSet_eventStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InSet_eventStruct)

      read_string_error(MIT_ENCODER_N_NAME, name, DEV$_BAD_NAME);
  build_results_with_xd_and_return(1);
}

#undef free_xd_array
