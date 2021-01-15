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
#include "a3204_gen.h"
EXPORT int a3204__add(struct descriptor *name_d_ptr, struct descriptor *dummy_d_ptr __attribute__ ((unused)), int *nid_ptr)
{
  static DESCRIPTOR(library_d, "MIT$DEVICES");
  static DESCRIPTOR(model_d, "A3204");
  static DESCRIPTOR_CONGLOM(conglom_d, &library_d, &model_d, 0, 0);
  int usage = TreeUSAGE_DEVICE;
  int curr_nid, old_nid, head_nid, status;
  long int flags = NciM_WRITE_ONCE;
  NCI_ITM flag_itm[] = { {2, NciSET_FLAGS, 0, 0}, {0, 0, 0, 0} };
  char *name_ptr = strncpy(malloc(name_d_ptr->length + 1), name_d_ptr->pointer, name_d_ptr->length);
  flag_itm[0].pointer = (unsigned char *)&flags;
  name_ptr[name_d_ptr->length] = 0;
  status = TreeStartConglomerate(A3204_K_CONG_NODES);
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
 ADD_NODE(:INPUT_1, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_1:GAIN, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0.	"
 ADD_NODE_EXPR(INPUT_1:OFFSET, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_1.FILTER, 0)
 ADD_NODE(INPUT_1:OUTPUT, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_1:FILTER_ON, TreeUSAGE_TEXT)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_2, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_2:GAIN, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0.	"
 ADD_NODE_EXPR(INPUT_2:OFFSET, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_2.FILTER, 0)
 ADD_NODE(INPUT_2:OUTPUT, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_2:FILTER_ON, TreeUSAGE_TEXT)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_3, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_3:GAIN, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0.	"
 ADD_NODE_EXPR(INPUT_3:OFFSET, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_3.FILTER, 0)
 ADD_NODE(INPUT_3:OUTPUT, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_3:FILTER_ON, TreeUSAGE_TEXT)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_4, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_4:GAIN, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0.	"
 ADD_NODE_EXPR(INPUT_4:OFFSET, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_4.FILTER, 0)
 ADD_NODE(INPUT_4:OUTPUT, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_4:FILTER_ON, TreeUSAGE_TEXT)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_ACTION(:INIT_ACTION, INIT, INIT, 50, 0, 0, CAMAC_SERVER, 0)
 ADD_NODE_ACTION(:STORE_ACTION, STORE, STORE, 50, 0, 0, CAMAC_SERVER, 0)
      status = TreeEndConglomerate();
  if (!(status & 1))
    return status;
  return (TreeSetDefaultNid(old_nid));
}

EXPORT int a3204__part_name(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)),
		     struct descriptor_d *out_d)
{
  int element = 0, status;
  NCI_ITM nci_list[] = { {4, NciCONGLOMERATE_ELT, 0, 0}, {0, 0, 0, 0} };
  nci_list[0].pointer = (unsigned char *)&element;
  status = TreeGetNci(*(int *)nid_d_ptr->pointer, nci_list);
  if (!(status & 1))
    return status;
  switch (element) {
  case (A3204_N_HEAD + 1):
    StrFree1Dx(out_d);
    break;
  case (A3204_N_NAME + 1):
 COPY_PART_NAME(:NAME) break;
  case (A3204_N_COMMENT + 1):
 COPY_PART_NAME(:COMMENT) break;
  case (A3204_N_INPUT_1 + 1):
 COPY_PART_NAME(:INPUT_1) break;
  case (A3204_N_INPUT_1_GAIN + 1):
 COPY_PART_NAME(INPUT_1:GAIN) break;
  case (A3204_N_INPUT_1_OFFSET + 1):
 COPY_PART_NAME(INPUT_1:OFFSET) break;
  case (A3204_N_INPUT_1_FILTER + 1):
 COPY_PART_NAME(:INPUT_1.FILTER) break;
  case (A3204_N_INPUT_1_OUTPUT + 1):
 COPY_PART_NAME(INPUT_1:OUTPUT) break;
  case (A3204_N_INPUT_1_FILTER_ON + 1):
 COPY_PART_NAME(INPUT_1:FILTER_ON) break;
  case (A3204_N_INPUT_2 + 1):
 COPY_PART_NAME(:INPUT_2) break;
  case (A3204_N_INPUT_2_GAIN + 1):
 COPY_PART_NAME(INPUT_2:GAIN) break;
  case (A3204_N_INPUT_2_OFFSET + 1):
 COPY_PART_NAME(INPUT_2:OFFSET) break;
  case (A3204_N_INPUT_2_FILTER + 1):
 COPY_PART_NAME(:INPUT_2.FILTER) break;
  case (A3204_N_INPUT_2_OUTPUT + 1):
 COPY_PART_NAME(INPUT_2:OUTPUT) break;
  case (A3204_N_INPUT_2_FILTER_ON + 1):
 COPY_PART_NAME(INPUT_2:FILTER_ON) break;
  case (A3204_N_INPUT_3 + 1):
 COPY_PART_NAME(:INPUT_3) break;
  case (A3204_N_INPUT_3_GAIN + 1):
 COPY_PART_NAME(INPUT_3:GAIN) break;
  case (A3204_N_INPUT_3_OFFSET + 1):
 COPY_PART_NAME(INPUT_3:OFFSET) break;
  case (A3204_N_INPUT_3_FILTER + 1):
 COPY_PART_NAME(:INPUT_3.FILTER) break;
  case (A3204_N_INPUT_3_OUTPUT + 1):
 COPY_PART_NAME(INPUT_3:OUTPUT) break;
  case (A3204_N_INPUT_3_FILTER_ON + 1):
 COPY_PART_NAME(INPUT_3:FILTER_ON) break;
  case (A3204_N_INPUT_4 + 1):
 COPY_PART_NAME(:INPUT_4) break;
  case (A3204_N_INPUT_4_GAIN + 1):
 COPY_PART_NAME(INPUT_4:GAIN) break;
  case (A3204_N_INPUT_4_OFFSET + 1):
 COPY_PART_NAME(INPUT_4:OFFSET) break;
  case (A3204_N_INPUT_4_FILTER + 1):
 COPY_PART_NAME(:INPUT_4.FILTER) break;
  case (A3204_N_INPUT_4_OUTPUT + 1):
 COPY_PART_NAME(INPUT_4:OUTPUT) break;
  case (A3204_N_INPUT_4_FILTER_ON + 1):
 COPY_PART_NAME(INPUT_4:FILTER_ON) break;
  case (A3204_N_INIT_ACTION + 1):
 COPY_PART_NAME(:INIT_ACTION) break;
  case (A3204_N_STORE_ACTION + 1):
 COPY_PART_NAME(:STORE_ACTION) break;
  default:
    status = TreeILLEGAL_ITEM;
  }
  return status;
}

extern int a3204___init();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}
#define error(nid,code,code1) {free_xd_array return GenDeviceSignal(nid,code,code1);}

EXPORT int a3204__init(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InInitStruct)
      static struct {
    short code;
    float value;
  } input_1_gain_t[] = {
    {0, .5}, {1, 1.}, {2, 2.}, {3, 4.}, {4, 8.}, {5, 16.},
    {6, 32.}, {7, 64.}, {8, 128.}, {9, 256.}, {10, 512.}, {0, 0}};
  static struct {
    short code;
    float value;
  } input_2_gain_t[] = {
    {0, .5}, {1, 1.}, {2, 2.}, {3, 4.}, {4, 8.}, {5, 16.},
    {6, 32.}, {7, 64.}, {8, 128.}, {9, 256.}, {10, 512.}, {0, 0}};
  static struct {
    short code;
    float value;
  } input_3_gain_t[] = {
    {0, .5}, {1, 1.}, {2, 2.}, {3, 4.}, {4, 8.}, {5, 16.},
    {6, 32.}, {7, 64.}, {8, 128.}, {9, 256.}, {10, 512.}, {0, 0}};
  static struct {
    short code;
    float value;
  } input_4_gain_t[] = {
    {0, .5}, {1, 1.}, {2, 2.}, {3, 4.}, {4, 8.}, {5, 16.},
    {6, 32.}, {7, 64.}, {8, 128.}, {9, 256.}, {10, 512.}, {0, 0}};
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InInitStruct)

      read_string_error(A3204_N_NAME, name, DEV$_BAD_NAME);
  read_float_error(A3204_N_INPUT_1_GAIN, input_1_gain, DEV$_BAD_GAIN);
  check_float_conv_set(input_1_gain, input_1_gain_convert, input_1_gain_t, 11, DEV$_BAD_GAIN);
  read_float(A3204_N_INPUT_1_OFFSET, input_1_offset);
  read_float_error(A3204_N_INPUT_2_GAIN, input_2_gain, DEV$_BAD_GAIN);
  check_float_conv_set(input_2_gain, input_2_gain_convert, input_2_gain_t, 11, DEV$_BAD_GAIN);
  read_float(A3204_N_INPUT_2_OFFSET, input_2_offset);
  read_float_error(A3204_N_INPUT_3_GAIN, input_3_gain, DEV$_BAD_GAIN);
  check_float_conv_set(input_3_gain, input_3_gain_convert, input_3_gain_t, 11, DEV$_BAD_GAIN);
  read_float(A3204_N_INPUT_3_OFFSET, input_3_offset);
  read_float_error(A3204_N_INPUT_4_GAIN, input_4_gain, DEV$_BAD_GAIN);
  check_float_conv_set(input_4_gain, input_4_gain_convert, input_4_gain_t, 11, DEV$_BAD_GAIN);
  read_float(A3204_N_INPUT_4_OFFSET, input_4_offset);
  status = a3204___init(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array

extern int a3204___store();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}

EXPORT int a3204__store(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InStoreStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InStoreStruct)

      read_string_error(A3204_N_NAME, name, DEV$_BAD_NAME);
  status = a3204___store(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array
