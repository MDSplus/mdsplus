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
#include "b5910a_gen.h"
EXPORT int b5910a__add(struct descriptor *name_d_ptr, struct descriptor *dummy_d_ptr __attribute__ ((unused)), int *nid_ptr)
{
  static DESCRIPTOR(library_d, "MIT$DEVICES");
  static DESCRIPTOR(model_d, "B5910A");
  static DESCRIPTOR_CONGLOM(conglom_d, &library_d, &model_d, 0, 0);
  int usage = TreeUSAGE_DEVICE;
  int curr_nid, old_nid, head_nid, status;
  long int flags = NciM_WRITE_ONCE;
  NCI_ITM flag_itm[] = { {2, NciSET_FLAGS, 0, 0}, {0, 0, 0, 0} };
  char *name_ptr = strncpy(malloc(name_d_ptr->length + 1), name_d_ptr->pointer, name_d_ptr->length);
  flag_itm[0].pointer = (unsigned char *)&flags;
  name_ptr[name_d_ptr->length] = 0;
  status = TreeStartConglomerate(B5910A_K_CONG_NODES);
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
#define expr " * : * : .0005			"
 ADD_NODE_EXPR(:INT_CLOCK, TreeUSAGE_AXIS)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:EXT_CLOCK, TreeUSAGE_AXIS)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0.				"
 ADD_NODE_EXPR(:START_TRIG, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:SAMPLES, 8192, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:ITERATIONS, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:NOC, 4, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " B5910A$CHAN(CHANNEL_1) 		"
 ADD_NODE_EXPR(:CHANNEL_1, TreeUSAGE_SIGNAL)
#undef expr
      flags |= NciM_WRITE_ONCE;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_1:PROGRAMMING, TreeUSAGE_SIGNAL)
#define expr " B5910A$CHAN(CHANNEL_2) 		"
 ADD_NODE_EXPR(:CHANNEL_2, TreeUSAGE_SIGNAL)
#undef expr
      flags |= NciM_WRITE_ONCE;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_2:PROGRAMMING, TreeUSAGE_SIGNAL)
#define expr " B5910A$CHAN(CHANNEL_3) 		"
 ADD_NODE_EXPR(:CHANNEL_3, TreeUSAGE_SIGNAL)
#undef expr
      flags |= NciM_WRITE_ONCE;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_3:PROGRAMMING, TreeUSAGE_SIGNAL)
#define expr " B5910A$CHAN(CHANNEL_4) 		"
 ADD_NODE_EXPR(:CHANNEL_4, TreeUSAGE_SIGNAL)
#undef expr
      flags |= NciM_WRITE_ONCE;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_4:PROGRAMMING, TreeUSAGE_SIGNAL)
#define expr " IF_ERROR(INT_CLOCK,EXT_CLOCK)	"
 ADD_NODE_EXPR(:CLOCK_OUT, TreeUSAGE_AXIS)
#undef expr
      flags |= NciM_WRITE_ONCE;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_ACTION(:INIT_ACTION, INIT, INIT, 50, 0, 0, CAMAC_SERVER, 0)
      status = TreeEndConglomerate();
  if (!(status & 1))
    return status;
  return (TreeSetDefaultNid(old_nid));
}

EXPORT int b5910a__part_name(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)),
		      struct descriptor_d *out_d)
{
  int element = 0, status;
  NCI_ITM nci_list[] = { {4, NciCONGLOMERATE_ELT, 0, 0}, {0, 0, 0, 0} };
  nci_list[0].pointer = (unsigned char *)&element;
  status = TreeGetNci(*(int *)nid_d_ptr->pointer, nci_list);
  if (!(status & 1))
    return status;
  switch (element) {
  case (B5910A_N_HEAD + 1):
    StrFree1Dx(out_d);
    break;
  case (B5910A_N_NAME + 1):
 COPY_PART_NAME(:NAME) break;
  case (B5910A_N_COMMENT + 1):
 COPY_PART_NAME(:COMMENT) break;
  case (B5910A_N_INT_CLOCK + 1):
 COPY_PART_NAME(:INT_CLOCK) break;
  case (B5910A_N_EXT_CLOCK + 1):
 COPY_PART_NAME(:EXT_CLOCK) break;
  case (B5910A_N_START_TRIG + 1):
 COPY_PART_NAME(:START_TRIG) break;
  case (B5910A_N_SAMPLES + 1):
 COPY_PART_NAME(:SAMPLES) break;
  case (B5910A_N_ITERATIONS + 1):
 COPY_PART_NAME(:ITERATIONS) break;
  case (B5910A_N_NOC + 1):
 COPY_PART_NAME(:NOC) break;
  case (B5910A_N_CHANNEL_1 + 1):
 COPY_PART_NAME(:CHANNEL_1) break;
  case (B5910A_N_CHANNEL_1_PROGRAMMING + 1):
 COPY_PART_NAME(CHANNEL_1:PROGRAMMING) break;
  case (B5910A_N_CHANNEL_2 + 1):
 COPY_PART_NAME(:CHANNEL_2) break;
  case (B5910A_N_CHANNEL_2_PROGRAMMING + 1):
 COPY_PART_NAME(CHANNEL_2:PROGRAMMING) break;
  case (B5910A_N_CHANNEL_3 + 1):
 COPY_PART_NAME(:CHANNEL_3) break;
  case (B5910A_N_CHANNEL_3_PROGRAMMING + 1):
 COPY_PART_NAME(CHANNEL_3:PROGRAMMING) break;
  case (B5910A_N_CHANNEL_4 + 1):
 COPY_PART_NAME(:CHANNEL_4) break;
  case (B5910A_N_CHANNEL_4_PROGRAMMING + 1):
 COPY_PART_NAME(CHANNEL_4:PROGRAMMING) break;
  case (B5910A_N_CLOCK_OUT + 1):
 COPY_PART_NAME(:CLOCK_OUT) break;
  case (B5910A_N_INIT_ACTION + 1):
 COPY_PART_NAME(:INIT_ACTION) break;
  default:
    status = TreeILLEGAL_ITEM;
  }
  return status;
}

extern int b5910a___init();
#define free_xd_array { int i; for(i=0; i<7;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}
#define error(nid,code,code1) {free_xd_array return GenDeviceSignal(nid,code,code1);}

EXPORT int b5910a__init(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InInitStruct)
  struct descriptor_xd work_xd[7];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 7);
  initialize_variables(InInitStruct)

      read_string_error(B5910A_N_NAME, name, DEV$_BAD_NAME);
  read_integer_error(B5910A_N_NOC, noc, B5910A$_BAD_NOC);
  check_range(noc, 1, 4, B5910A$_BAD_NOC);
  read_descriptor(B5910A_N_INT_CLOCK, int_clock);
  read_integer_error(B5910A_N_ITERATIONS, iterations, B5910A$_BAD_ITERATIONS);
  check_range(iterations, 0, 15, B5910A$_BAD_ITERATIONS);
  read_integer_error(B5910A_N_SAMPLES, samples, B5910A$_BAD_SAMPS);
  check_range(samples, 1, 32768, B5910A$_BAD_SAMPS);
  read_descriptor(B5910A_N_CHANNEL_1, channel_1);
  read_descriptor(B5910A_N_CHANNEL_2, channel_2);
  read_descriptor(B5910A_N_CHANNEL_3, channel_3);
  read_descriptor(B5910A_N_CHANNEL_4, channel_4);
  read_descriptor(B5910A_N_START_TRIG, start_trig);
  status = b5910a___init(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array
