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
#include "dsp2904_gen.h"
extern int dsp2904___add(int *nid);
EXPORT int dsp2904__add(struct descriptor *name_d_ptr, struct descriptor *dummy_d_ptr __attribute__ ((unused)), int *nid_ptr)
{
  static DESCRIPTOR(library_d, "MIT$DEVICES");
  static DESCRIPTOR(model_d, "DSP2904");
  static DESCRIPTOR_CONGLOM(conglom_d, &library_d, &model_d, 0, 0);
  int usage = TreeUSAGE_DEVICE;
  int curr_nid, old_nid, head_nid, status;
  long int flags = NciM_WRITE_ONCE;
  NCI_ITM flag_itm[] = { {2, NciSET_FLAGS, 0, 0}, {0, 0, 0, 0} };
  char *name_ptr = strncpy(malloc(name_d_ptr->length + 1), name_d_ptr->pointer, name_d_ptr->length);
  flag_itm[0].pointer = (unsigned char *)&flags;
  name_ptr[name_d_ptr->length] = 0;
  status = TreeStartConglomerate(DSP2904_K_CONG_NODES);
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
 ADD_NODE(:TRAQ_NAME, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:TIMER, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:TIMER.GATE, TreeUSAGE_STRUCTURE)
 ADD_NODE(:TIMER.WRAP, TreeUSAGE_STRUCTURE)
 ADD_NODE(:TIMER.BITS_32, TreeUSAGE_STRUCTURE)
 ADD_NODE_INTEGER(TIMER:PRESET, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(TIMER:CHANNEL, 9, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:COUNTER, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:COUNTER.GATE, TreeUSAGE_STRUCTURE)
 ADD_NODE(:COUNTER.WRAP, TreeUSAGE_STRUCTURE)
 ADD_NODE(:COUNTER.BITS_32, TreeUSAGE_STRUCTURE)
 ADD_NODE_INTEGER(COUNTER:PRESET, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(COUNTER:CHANNEL, 10, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
  ADD_NODE(.IDX_DIVIDER, TreeUSAGE_STRUCTURE)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
  ADD_NODE(.TIME_OF_CNT, TreeUSAGE_STRUCTURE)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 102.4E-6  "
 ADD_NODE_EXPR(:RATE, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:CLOCK_OUT, TreeUSAGE_AXIS)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:EXT_CLK_IN, TreeUSAGE_AXIS)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0.0	"
 ADD_NODE_EXPR(:START, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_ACTION(:INIT_ACTION, INIT, INIT, 50, 0, 0, CAMAC_SERVER, 0)
 ADD_NODE_ACTION(:STORE_ACTION, STORE, STORE, 50, 0, 0, CAMAC_SERVER, 0)
      status = dsp2904___add(&head_nid);
  status = TreeEndConglomerate();
  if (!(status & 1))
    return status;
  return (TreeSetDefaultNid(old_nid));
}

EXPORT int dsp2904__part_name(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)),
		       struct descriptor_d *out_d)
{
  int element = 0, status;
  NCI_ITM nci_list[] = { {4, NciCONGLOMERATE_ELT, 0, 0}, {0, 0, 0, 0} };
  nci_list[0].pointer = (unsigned char *)&element;
  status = TreeGetNci(*(int *)nid_d_ptr->pointer, nci_list);
  if (!(status & 1))
    return status;
  switch (element) {
  case (DSP2904_N_HEAD + 1):
    StrFree1Dx(out_d);
    break;
  case (DSP2904_N_NAME + 1):
 COPY_PART_NAME(:NAME) break;
  case (DSP2904_N_COMMENT + 1):
 COPY_PART_NAME(:COMMENT) break;
  case (DSP2904_N_TRAQ_NAME + 1):
 COPY_PART_NAME(:TRAQ_NAME) break;
  case (DSP2904_N_TIMER + 1):
 COPY_PART_NAME(:TIMER) break;
  case (DSP2904_N_TIMER_GATE + 1):
 COPY_PART_NAME(:TIMER.GATE) break;
  case (DSP2904_N_TIMER_WRAP + 1):
 COPY_PART_NAME(:TIMER.WRAP) break;
  case (DSP2904_N_TIMER_BITS_32 + 1):
 COPY_PART_NAME(:TIMER.BITS_32) break;
  case (DSP2904_N_TIMER_PRESET + 1):
 COPY_PART_NAME(TIMER:PRESET) break;
  case (DSP2904_N_TIMER_CHANNEL + 1):
 COPY_PART_NAME(TIMER:CHANNEL) break;
  case (DSP2904_N_COUNTER + 1):
 COPY_PART_NAME(:COUNTER) break;
  case (DSP2904_N_COUNTER_GATE + 1):
 COPY_PART_NAME(:COUNTER.GATE) break;
  case (DSP2904_N_COUNTER_WRAP + 1):
 COPY_PART_NAME(:COUNTER.WRAP) break;
  case (DSP2904_N_COUNTER_BITS_32 + 1):
 COPY_PART_NAME(:COUNTER.BITS_32) break;
  case (DSP2904_N_COUNTER_PRESET + 1):
 COPY_PART_NAME(COUNTER:PRESET) break;
  case (DSP2904_N_COUNTER_CHANNEL + 1):
 COPY_PART_NAME(COUNTER:CHANNEL) break;
  case (DSP2904_N_IDX_DIVIDER + 1):
    COPY_PART_NAME(.IDX_DIVIDER) break;
  case (DSP2904_N_TIME_OF_CNT + 1):
    COPY_PART_NAME(.TIME_OF_CNT) break;
  case (DSP2904_N_RATE + 1):
 COPY_PART_NAME(:RATE) break;
  case (DSP2904_N_CLOCK_OUT + 1):
 COPY_PART_NAME(:CLOCK_OUT) break;
  case (DSP2904_N_EXT_CLK_IN + 1):
 COPY_PART_NAME(:EXT_CLK_IN) break;
  case (DSP2904_N_START + 1):
 COPY_PART_NAME(:START) break;
  case (DSP2904_N_INIT_ACTION + 1):
 COPY_PART_NAME(:INIT_ACTION) break;
  case (DSP2904_N_STORE_ACTION + 1):
 COPY_PART_NAME(:STORE_ACTION) break;
  default:
    status = TreeILLEGAL_ITEM;
  }
  return status;
}

extern int dsp2904___init();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}
#define error(nid,code,code1) {free_xd_array return GenDeviceSignal(nid,code,code1);}

EXPORT int dsp2904__init(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InInitStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InInitStruct)

      read_string_error(DSP2904_N_NAME, name, DEV$_BAD_NAME);
  status = dsp2904___init(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array

extern int dsp2904___store();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}

EXPORT int dsp2904__store(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InStoreStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InStoreStruct)

      read_string_error(DSP2904_N_TRAQ_NAME, traq_name, DEV$_BAD_NAME);
  status = dsp2904___store(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array
