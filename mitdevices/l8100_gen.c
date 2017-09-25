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
#include "l8100_gen.h"
EXPORT int l8100__add(struct descriptor *name_d_ptr, struct descriptor *dummy_d_ptr __attribute__ ((unused)), int *nid_ptr)
{
  static DESCRIPTOR(library_d, "MIT$DEVICES");
  static DESCRIPTOR(model_d, "L8100");
  static DESCRIPTOR_CONGLOM(conglom_d, &library_d, &model_d, 0, 0);
  int usage = TreeUSAGE_DEVICE;
  int curr_nid, old_nid, head_nid, status;
  long int flags = NciM_WRITE_ONCE;
  NCI_ITM flag_itm[] = { {2, NciSET_FLAGS, 0, 0}, {0, 0, 0, 0} };
  char *name_ptr = strncpy(malloc(name_d_ptr->length + 1), name_d_ptr->pointer, name_d_ptr->length);
  flag_itm[0].pointer = (unsigned char *)&flags;
  name_ptr[name_d_ptr->length] = 0;
  status = TreeStartConglomerate(L8100_K_CONG_NODES);
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
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_1:MULT, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0B	"
 ADD_NODE_EXPR(INPUT_1:GROUND, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0.	"
 ADD_NODE_EXPR(INPUT_1:OFFSET, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_1:FILTER, TreeUSAGE_TEXT)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
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
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_2:MULT, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0B	"
 ADD_NODE_EXPR(INPUT_2:GROUND, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0.	"
 ADD_NODE_EXPR(INPUT_2:OFFSET, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_2:FILTER, TreeUSAGE_TEXT)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_2:OUTPUT, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_2:FILTER_ON, TreeUSAGE_TEXT)
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

EXPORT int l8100__part_name(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)),
		     struct descriptor_d *out_d)
{
  int element = 0, status;
  NCI_ITM nci_list[] = { {4, NciCONGLOMERATE_ELT, 0, 0}, {0, 0, 0, 0} };
  nci_list[0].pointer = (unsigned char *)&element;
  status = TreeGetNci(*(int *)nid_d_ptr->pointer, nci_list);
  if (!(status & 1))
    return status;
  switch (element) {
  case (L8100_N_HEAD + 1):
    StrFree1Dx(out_d);
    break;
  case (L8100_N_NAME + 1):
 COPY_PART_NAME(:NAME) break;
  case (L8100_N_COMMENT + 1):
 COPY_PART_NAME(:COMMENT) break;
  case (L8100_N_INPUT_1 + 1):
 COPY_PART_NAME(:INPUT_1) break;
  case (L8100_N_INPUT_1_GAIN + 1):
 COPY_PART_NAME(INPUT_1:GAIN) break;
  case (L8100_N_INPUT_1_MULT + 1):
 COPY_PART_NAME(INPUT_1:MULT) break;
  case (L8100_N_INPUT_1_GROUND + 1):
 COPY_PART_NAME(INPUT_1:GROUND) break;
  case (L8100_N_INPUT_1_OFFSET + 1):
 COPY_PART_NAME(INPUT_1:OFFSET) break;
  case (L8100_N_INPUT_1_FILTER + 1):
 COPY_PART_NAME(INPUT_1:FILTER) break;
  case (L8100_N_INPUT_1_OUTPUT + 1):
 COPY_PART_NAME(INPUT_1:OUTPUT) break;
  case (L8100_N_INPUT_1_FILTER_ON + 1):
 COPY_PART_NAME(INPUT_1:FILTER_ON) break;
  case (L8100_N_INPUT_2 + 1):
 COPY_PART_NAME(:INPUT_2) break;
  case (L8100_N_INPUT_2_GAIN + 1):
 COPY_PART_NAME(INPUT_2:GAIN) break;
  case (L8100_N_INPUT_2_MULT + 1):
 COPY_PART_NAME(INPUT_2:MULT) break;
  case (L8100_N_INPUT_2_GROUND + 1):
 COPY_PART_NAME(INPUT_2:GROUND) break;
  case (L8100_N_INPUT_2_OFFSET + 1):
 COPY_PART_NAME(INPUT_2:OFFSET) break;
  case (L8100_N_INPUT_2_FILTER + 1):
 COPY_PART_NAME(INPUT_2:FILTER) break;
  case (L8100_N_INPUT_2_OUTPUT + 1):
 COPY_PART_NAME(INPUT_2:OUTPUT) break;
  case (L8100_N_INPUT_2_FILTER_ON + 1):
 COPY_PART_NAME(INPUT_2:FILTER_ON) break;
  case (L8100_N_INIT_ACTION + 1):
 COPY_PART_NAME(:INIT_ACTION) break;
  case (L8100_N_STORE_ACTION + 1):
 COPY_PART_NAME(:STORE_ACTION) break;
  default:
    status = TreeILLEGAL_ITEM;
  }
  return status;
}

extern int l8100___init();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}
#define error(nid,code,code1) {free_xd_array return GenDeviceSignal(nid,code,code1);}

EXPORT int l8100__init(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InInitStruct)
      static struct {
    short code;
    float value;
  } input_1_gain_t[] = {
    {1, 100.}, {2, 10.}, {4, 1.}, {0, 0}};
  static struct {
    short code;
    float value;
  } input_1_mult_t[] = {
    {1, 1.}, {2, .5}, {4, .2}, {0, 0}};
  static struct {
    short code;
    float value;
  } input_2_gain_t[] = {
    {1, 100.}, {2, 10.}, {4, 1.}, {0, 0}};
  static struct {
    short code;
    float value;
  } input_2_mult_t[] = {
    {1, 1.}, {2, .5}, {4, .2}, {0, 0}};
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InInitStruct)

      read_string_error(L8100_N_NAME, name, DEV$_BAD_NAME);
  read_float_error(L8100_N_INPUT_1_GAIN, input_1_gain, DEV$_BAD_GAIN);
  check_float_conv_set(input_1_gain, input_1_gain_convert, input_1_gain_t, 3, DEV$_BAD_GAIN);
  read_float_error(L8100_N_INPUT_1_MULT, input_1_mult, DEV$_BAD_GAIN);
  check_float_conv_set(input_1_mult, input_1_mult_convert, input_1_mult_t, 3, DEV$_BAD_GAIN);
  read_float_error(L8100_N_INPUT_1_OFFSET, input_1_offset, DEV$_BAD_OFFSET);
  check_range(input_1_offset, -5.5, 5.5, DEV$_BAD_OFFSET);
  read_float_error(L8100_N_INPUT_2_GAIN, input_2_gain, DEV$_BAD_GAIN);
  check_float_conv_set(input_2_gain, input_2_gain_convert, input_2_gain_t, 3, DEV$_BAD_GAIN);
  read_float_error(L8100_N_INPUT_2_MULT, input_2_mult, DEV$_BAD_GAIN);
  check_float_conv_set(input_2_mult, input_2_mult_convert, input_2_mult_t, 3, DEV$_BAD_GAIN);
  read_float_error(L8100_N_INPUT_2_OFFSET, input_2_offset, DEV$_BAD_OFFSET);
  check_range(input_2_offset, -5.5, 5.5, DEV$_BAD_OFFSET);
  status = l8100___init(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array

extern int l8100___store();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}

EXPORT int l8100__store(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InStoreStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InStoreStruct)

      read_string_error(L8100_N_NAME, name, DEV$_BAD_NAME);
  status = l8100___store(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array
