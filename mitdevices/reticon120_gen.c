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
#include "reticon120_gen.h"
EXPORT int reticon120__add(struct descriptor *name_d_ptr, struct descriptor *dummy_d_ptr __attribute__ ((unused)), int *nid_ptr)
{
  static DESCRIPTOR(library_d, "MIT$DEVICES");
  static DESCRIPTOR(model_d, "RETICON120");
  static DESCRIPTOR_CONGLOM(conglom_d, &library_d, &model_d, 0, 0);
  int usage = TreeUSAGE_DEVICE;
  int curr_nid, old_nid, head_nid, status;
  long int flags = NciM_WRITE_ONCE;
  NCI_ITM flag_itm[] = { {2, NciSET_FLAGS, 0, 0}, {0, 0, 0, 0} };
  char *name_ptr = strncpy(malloc(name_d_ptr->length + 1), name_d_ptr->pointer, name_d_ptr->length);
  flag_itm[0].pointer = (unsigned char *)&flags;
  name_ptr[name_d_ptr->length] = 0;
  status = TreeStartConglomerate(RETICON120_K_CONG_NODES);
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
 ADD_NODE(:MEMORY_NAME, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0.	"
 ADD_NODE_EXPR(:START_TRIG, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:PIXEL_SELECT, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " _X=PACK(1:256,:PIXEL_SELECT)*4,_X=SORTVAL([_X-3,_X-2,_X-1,_X]),BUILD_WITH_UNITS(SET_RANGE(SIZE(_X),_X),'pixel') "
 ADD_NODE_EXPR(:PIXEL_AXIS, TreeUSAGE_AXIS)
#undef expr
      flags |= NciM_WRITE_ONCE;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:DATA, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:EVENT_LATCH, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:NUM_STATES, 4, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:FRAME_INDEX, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
  ADD_NODE(.STATE_0, TreeUSAGE_STRUCTURE)
#define expr " 8.0	"
 ADD_NODE_EXPR(.STATE_0:PERIOD, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(.STATE_0:FRAMES, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(.STATE_0:FRAME_SELECT, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
  ADD_NODE(.STATE_1, TreeUSAGE_STRUCTURE)
#define expr " 8.0	"
 ADD_NODE_EXPR(.STATE_1:PERIOD, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(.STATE_1:FRAMES, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(.STATE_1:FRAME_SELECT, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
  ADD_NODE(.STATE_2, TreeUSAGE_STRUCTURE)
#define expr " 8.0	"
 ADD_NODE_EXPR(.STATE_2:PERIOD, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(.STATE_2:FRAMES, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(.STATE_2:FRAME_SELECT, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
  ADD_NODE(.STATE_3, TreeUSAGE_STRUCTURE)
#define expr " 8.0	"
 ADD_NODE_EXPR(.STATE_3:PERIOD, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(.STATE_3:FRAMES, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(.STATE_3:FRAME_SELECT, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_ACTION(:INIT_ACTION, INIT, INIT, 50, 0, 0, CAMAC_SERVER, 0)
 ADD_NODE_ACTION(:STORE_ACTION, STORE, STORE, 50, 0, 0, CAMAC_SERVER, 0)
      status = TreeEndConglomerate();
  if (!(status & 1))
    return status;
  return (TreeSetDefaultNid(old_nid));
}

EXPORT int reticon120__part_name(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)),
			  struct descriptor_d *out_d)
{
  int element = 0, status;
  NCI_ITM nci_list[] = { {4, NciCONGLOMERATE_ELT, 0, 0}, {0, 0, 0, 0} };
  nci_list[0].pointer = (unsigned char *)&element;
  status = TreeGetNci(*(int *)nid_d_ptr->pointer, nci_list);
  if (!(status & 1))
    return status;
  switch (element) {
  case (RETICON120_N_HEAD + 1):
    StrFree1Dx(out_d);
    break;
  case (RETICON120_N_NAME + 1):
 COPY_PART_NAME(:NAME) break;
  case (RETICON120_N_COMMENT + 1):
 COPY_PART_NAME(:COMMENT) break;
  case (RETICON120_N_MEMORY_NAME + 1):
 COPY_PART_NAME(:MEMORY_NAME) break;
  case (RETICON120_N_START_TRIG + 1):
 COPY_PART_NAME(:START_TRIG) break;
  case (RETICON120_N_PIXEL_SELECT + 1):
 COPY_PART_NAME(:PIXEL_SELECT) break;
  case (RETICON120_N_PIXEL_AXIS + 1):
 COPY_PART_NAME(:PIXEL_AXIS) break;
  case (RETICON120_N_DATA + 1):
 COPY_PART_NAME(:DATA) break;
  case (RETICON120_N_EVENT_LATCH + 1):
 COPY_PART_NAME(:EVENT_LATCH) break;
  case (RETICON120_N_NUM_STATES + 1):
 COPY_PART_NAME(:NUM_STATES) break;
  case (RETICON120_N_FRAME_INDEX + 1):
 COPY_PART_NAME(:FRAME_INDEX) break;
  case (RETICON120_N_STATE_0 + 1):
    COPY_PART_NAME(.STATE_0) break;
  case (RETICON120_N_STATE_0_PERIOD + 1):
 COPY_PART_NAME(.STATE_0:PERIOD) break;
  case (RETICON120_N_STATE_0_FRAMES + 1):
 COPY_PART_NAME(.STATE_0:FRAMES) break;
  case (RETICON120_N_STATE_0_FRAME_SELECT + 1):
 COPY_PART_NAME(.STATE_0:FRAME_SELECT) break;
  case (RETICON120_N_STATE_1 + 1):
    COPY_PART_NAME(.STATE_1) break;
  case (RETICON120_N_STATE_1_PERIOD + 1):
 COPY_PART_NAME(.STATE_1:PERIOD) break;
  case (RETICON120_N_STATE_1_FRAMES + 1):
 COPY_PART_NAME(.STATE_1:FRAMES) break;
  case (RETICON120_N_STATE_1_FRAME_SELECT + 1):
 COPY_PART_NAME(.STATE_1:FRAME_SELECT) break;
  case (RETICON120_N_STATE_2 + 1):
    COPY_PART_NAME(.STATE_2) break;
  case (RETICON120_N_STATE_2_PERIOD + 1):
 COPY_PART_NAME(.STATE_2:PERIOD) break;
  case (RETICON120_N_STATE_2_FRAMES + 1):
 COPY_PART_NAME(.STATE_2:FRAMES) break;
  case (RETICON120_N_STATE_2_FRAME_SELECT + 1):
 COPY_PART_NAME(.STATE_2:FRAME_SELECT) break;
  case (RETICON120_N_STATE_3 + 1):
    COPY_PART_NAME(.STATE_3) break;
  case (RETICON120_N_STATE_3_PERIOD + 1):
 COPY_PART_NAME(.STATE_3:PERIOD) break;
  case (RETICON120_N_STATE_3_FRAMES + 1):
 COPY_PART_NAME(.STATE_3:FRAMES) break;
  case (RETICON120_N_STATE_3_FRAME_SELECT + 1):
 COPY_PART_NAME(.STATE_3:FRAME_SELECT) break;
  case (RETICON120_N_INIT_ACTION + 1):
 COPY_PART_NAME(:INIT_ACTION) break;
  case (RETICON120_N_STORE_ACTION + 1):
 COPY_PART_NAME(:STORE_ACTION) break;
  default:
    status = TreeILLEGAL_ITEM;
  }
  return status;
}

extern int reticon120___init();
#define free_xd_array { int i; for(i=0; i<3;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}
#define error(nid,code,code1) {free_xd_array return GenDeviceSignal(nid,code,code1);}

EXPORT int reticon120__init(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InInitStruct)
  struct descriptor_xd work_xd[3];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 3);
  initialize_variables(InInitStruct)

      read_string_error(RETICON120_N_NAME, name, DEV$_BAD_NAME);
  read_string_error(RETICON120_N_MEMORY_NAME, memory_name, DEV$_BAD_NAME);
  read_integer_error(RETICON120_N_NUM_STATES, num_states, RETICON$_BAD_NUM_STATES);
  check_range(num_states, 1, 4, RETICON$_BAD_NUM_STATES);
  read_descriptor(RETICON120_N_PIXEL_SELECT, pixel_select);
  status = reticon120___init(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array

extern int reticon120___store();
#define free_xd_array { int i; for(i=0; i<2;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}

EXPORT int reticon120__store(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InStoreStruct)
  struct descriptor_xd work_xd[2];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 2);
  initialize_variables(InStoreStruct)

      read_string_error(RETICON120_N_NAME, name, DEV$_BAD_NAME);
  read_string_error(RETICON120_N_MEMORY_NAME, memory_name, DEV$_BAD_NAME);
  status = reticon120___store(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array

extern int reticon120___trigger();
#define free_xd_array { int i; for(i=0; i<2;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}

EXPORT int reticon120__trigger(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InTriggerStruct)
  struct descriptor_xd work_xd[2];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 2);
  initialize_variables(InTriggerStruct)

      read_string_error(RETICON120_N_NAME, name, DEV$_BAD_NAME);
  read_string_error(RETICON120_N_MEMORY_NAME, memory_name, DEV$_BAD_NAME);
  status = reticon120___trigger(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array
