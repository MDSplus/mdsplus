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
#include "joerger_tr812_gen.h"
EXPORT int joerger_tr812__add(struct descriptor *name_d_ptr, struct descriptor *dummy_d_ptr __attribute__ ((unused)), int *nid_ptr)
{
  static DESCRIPTOR(library_d, "MIT$DEVICES");
  static DESCRIPTOR(model_d, "JOERGER_TR812");
  static DESCRIPTOR_CONGLOM(conglom_d, &library_d, &model_d, 0, 0);
  int usage = TreeUSAGE_DEVICE;
  int curr_nid, old_nid, head_nid, status;
  long int flags = NciM_WRITE_ONCE;
  NCI_ITM flag_itm[] = { {2, NciSET_FLAGS, 0, 0}, {0, 0, 0, 0} };
  char *name_ptr = strncpy(malloc(name_d_ptr->length + 1), name_d_ptr->pointer, name_d_ptr->length);
  flag_itm[0].pointer = (unsigned char *)&flags;
  name_ptr[name_d_ptr->length] = 0;
  status = TreeStartConglomerate(JOERGER_TR812_K_CONG_NODES);
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
#define expr " 0.	"
 ADD_NODE_EXPR(:TRIGGER, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:BURST_MODE, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_MODEL;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:EXT_CLOCK, TreeUSAGE_AXIS)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:PRE_TRIGGER, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:ACT_MEMORY, 8, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:CHANNEL_1, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_1:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_1:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(CHANNEL_1:GAIN, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:CHANNEL_2, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_2:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_2:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(CHANNEL_2:GAIN, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:CHANNEL_3, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_3:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_3:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(CHANNEL_3:GAIN, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:CHANNEL_4, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_4:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_4:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(CHANNEL_4:GAIN, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:CHANNEL_5, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_5:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_5:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(CHANNEL_5:GAIN, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:CHANNEL_6, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_6:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_6:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(CHANNEL_6:GAIN, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:CHANNEL_7, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_7:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_7:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(CHANNEL_7:GAIN, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:CHANNEL_8, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_8:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_8:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(CHANNEL_8:GAIN, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_ACTION(:INIT_ACTION, INIT, INIT, 50, 0, 0, CAMAC_SERVER, 0)
 ADD_NODE_ACTION(:STORE_ACTION, STORE, STORE, 50, 0, 0, CAMAC_SERVER, 0)
      status = TreeEndConglomerate();
  if (!(status & 1))
    return status;
  return (TreeSetDefaultNid(old_nid));
}

EXPORT int joerger_tr812__part_name(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)),
			     struct descriptor_d *out_d)
{
  int element = 0, status;
  NCI_ITM nci_list[] = { {4, NciCONGLOMERATE_ELT, 0, 0}, {0, 0, 0, 0} };
  nci_list[0].pointer = (unsigned char *)&element;
  status = TreeGetNci(*(int *)nid_d_ptr->pointer, nci_list);
  if (!(status & 1))
    return status;
  switch (element) {
  case (JOERGER_TR812_N_HEAD + 1):
    StrFree1Dx(out_d);
    break;
  case (JOERGER_TR812_N_NAME + 1):
 COPY_PART_NAME(:NAME) break;
  case (JOERGER_TR812_N_COMMENT + 1):
 COPY_PART_NAME(:COMMENT) break;
  case (JOERGER_TR812_N_TRIGGER + 1):
 COPY_PART_NAME(:TRIGGER) break;
  case (JOERGER_TR812_N_BURST_MODE + 1):
 COPY_PART_NAME(:BURST_MODE) break;
  case (JOERGER_TR812_N_EXT_CLOCK + 1):
 COPY_PART_NAME(:EXT_CLOCK) break;
  case (JOERGER_TR812_N_PRE_TRIGGER + 1):
 COPY_PART_NAME(:PRE_TRIGGER) break;
  case (JOERGER_TR812_N_ACT_MEMORY + 1):
 COPY_PART_NAME(:ACT_MEMORY) break;
  case (JOERGER_TR812_N_CHANNEL_1 + 1):
 COPY_PART_NAME(:CHANNEL_1) break;
  case (JOERGER_TR812_N_CHANNEL_1_STARTIDX + 1):
 COPY_PART_NAME(CHANNEL_1:STARTIDX) break;
  case (JOERGER_TR812_N_CHANNEL_1_ENDIDX + 1):
 COPY_PART_NAME(CHANNEL_1:ENDIDX) break;
  case (JOERGER_TR812_N_CHANNEL_1_GAIN + 1):
 COPY_PART_NAME(CHANNEL_1:GAIN) break;
  case (JOERGER_TR812_N_CHANNEL_2 + 1):
 COPY_PART_NAME(:CHANNEL_2) break;
  case (JOERGER_TR812_N_CHANNEL_2_STARTIDX + 1):
 COPY_PART_NAME(CHANNEL_2:STARTIDX) break;
  case (JOERGER_TR812_N_CHANNEL_2_ENDIDX + 1):
 COPY_PART_NAME(CHANNEL_2:ENDIDX) break;
  case (JOERGER_TR812_N_CHANNEL_2_GAIN + 1):
 COPY_PART_NAME(CHANNEL_2:GAIN) break;
  case (JOERGER_TR812_N_CHANNEL_3 + 1):
 COPY_PART_NAME(:CHANNEL_3) break;
  case (JOERGER_TR812_N_CHANNEL_3_STARTIDX + 1):
 COPY_PART_NAME(CHANNEL_3:STARTIDX) break;
  case (JOERGER_TR812_N_CHANNEL_3_ENDIDX + 1):
 COPY_PART_NAME(CHANNEL_3:ENDIDX) break;
  case (JOERGER_TR812_N_CHANNEL_3_GAIN + 1):
 COPY_PART_NAME(CHANNEL_3:GAIN) break;
  case (JOERGER_TR812_N_CHANNEL_4 + 1):
 COPY_PART_NAME(:CHANNEL_4) break;
  case (JOERGER_TR812_N_CHANNEL_4_STARTIDX + 1):
 COPY_PART_NAME(CHANNEL_4:STARTIDX) break;
  case (JOERGER_TR812_N_CHANNEL_4_ENDIDX + 1):
 COPY_PART_NAME(CHANNEL_4:ENDIDX) break;
  case (JOERGER_TR812_N_CHANNEL_4_GAIN + 1):
 COPY_PART_NAME(CHANNEL_4:GAIN) break;
  case (JOERGER_TR812_N_CHANNEL_5 + 1):
 COPY_PART_NAME(:CHANNEL_5) break;
  case (JOERGER_TR812_N_CHANNEL_5_STARTIDX + 1):
 COPY_PART_NAME(CHANNEL_5:STARTIDX) break;
  case (JOERGER_TR812_N_CHANNEL_5_ENDIDX + 1):
 COPY_PART_NAME(CHANNEL_5:ENDIDX) break;
  case (JOERGER_TR812_N_CHANNEL_5_GAIN + 1):
 COPY_PART_NAME(CHANNEL_5:GAIN) break;
  case (JOERGER_TR812_N_CHANNEL_6 + 1):
 COPY_PART_NAME(:CHANNEL_6) break;
  case (JOERGER_TR812_N_CHANNEL_6_STARTIDX + 1):
 COPY_PART_NAME(CHANNEL_6:STARTIDX) break;
  case (JOERGER_TR812_N_CHANNEL_6_ENDIDX + 1):
 COPY_PART_NAME(CHANNEL_6:ENDIDX) break;
  case (JOERGER_TR812_N_CHANNEL_6_GAIN + 1):
 COPY_PART_NAME(CHANNEL_6:GAIN) break;
  case (JOERGER_TR812_N_CHANNEL_7 + 1):
 COPY_PART_NAME(:CHANNEL_7) break;
  case (JOERGER_TR812_N_CHANNEL_7_STARTIDX + 1):
 COPY_PART_NAME(CHANNEL_7:STARTIDX) break;
  case (JOERGER_TR812_N_CHANNEL_7_ENDIDX + 1):
 COPY_PART_NAME(CHANNEL_7:ENDIDX) break;
  case (JOERGER_TR812_N_CHANNEL_7_GAIN + 1):
 COPY_PART_NAME(CHANNEL_7:GAIN) break;
  case (JOERGER_TR812_N_CHANNEL_8 + 1):
 COPY_PART_NAME(:CHANNEL_8) break;
  case (JOERGER_TR812_N_CHANNEL_8_STARTIDX + 1):
 COPY_PART_NAME(CHANNEL_8:STARTIDX) break;
  case (JOERGER_TR812_N_CHANNEL_8_ENDIDX + 1):
 COPY_PART_NAME(CHANNEL_8:ENDIDX) break;
  case (JOERGER_TR812_N_CHANNEL_8_GAIN + 1):
 COPY_PART_NAME(CHANNEL_8:GAIN) break;
  case (JOERGER_TR812_N_INIT_ACTION + 1):
 COPY_PART_NAME(:INIT_ACTION) break;
  case (JOERGER_TR812_N_STORE_ACTION + 1):
 COPY_PART_NAME(:STORE_ACTION) break;
  default:
    status = TreeILLEGAL_ITEM;
  }
  return status;
}

extern int joerger_tr812___init();
#define free_xd_array { int i; for(i=0; i<2;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}
#define error(nid,code,code1) {free_xd_array return GenDeviceSignal(nid,code,code1);}

EXPORT int joerger_tr812__init(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InInitStruct)
      static struct {
    short code;
    int value;
  } channel_1_gain_t[] = {
    {0, 1}, {1, 2}, {2, 4}, {3, 8}, {0, 0}};
  static struct {
    short code;
    int value;
  } channel_2_gain_t[] = {
  {0, 1}, {1, 2}, {2, 4}, {3, 8}, {0, 0}};
  static struct {
    short code;
    int value;
  } channel_3_gain_t[] = {
  {0, 1}, {1, 2}, {2, 4}, {3, 8}, {0, 0}};
  static struct {
    short code;
    int value;
  } channel_4_gain_t[] = {
  {0, 1}, {1, 2}, {2, 4}, {3, 8}, {0, 0}};
  static struct {
    short code;
    int value;
  } channel_5_gain_t[] = {
  {0, 1}, {1, 2}, {2, 4}, {3, 8}, {0, 0}};
  static struct {
    short code;
    int value;
  } channel_6_gain_t[] = {
  {0, 1}, {1, 2}, {2, 4}, {3, 8}, {0, 0}};
  static struct {
    short code;
    int value;
  } channel_7_gain_t[] = {
  {0, 1}, {1, 2}, {2, 4}, {3, 8}, {0, 0}};
  static struct {
    short code;
    int value;
  } channel_8_gain_t[] = {
  {0, 1}, {1, 2}, {2, 4}, {3, 8}, {0, 0}};
  struct descriptor_xd work_xd[2];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 2);
  initialize_variables(InInitStruct)

      read_string_error(JOERGER_TR812_N_NAME, name, DEV$_BAD_NAME);
  read_integer_error(JOERGER_TR812_N_PRE_TRIGGER, pre_trigger, JOERGER$_BAD_PRE_TRIGGER);
  check_range(pre_trigger, 0, 7, JOERGER$_BAD_PRE_TRIGGER);
  read_integer_error(JOERGER_TR812_N_ACT_MEMORY, act_memory, JOERGER$_BAD_ACT_MEMORY);
  check_range(act_memory, 1, 8, JOERGER$_BAD_ACT_MEMORY);
  read_integer_error(JOERGER_TR812_N_CHANNEL_1_GAIN, channel_1_gain, JOERGER$_BAD_GAIN);
  check_integer_conv_set(channel_1_gain, channel_1_gain_convert, channel_1_gain_t, 4,
			 JOERGER$_BAD_GAIN);
  read_integer_error(JOERGER_TR812_N_CHANNEL_2_GAIN, channel_2_gain, JOERGER$_BAD_GAIN);
  check_integer_conv_set(channel_2_gain, channel_2_gain_convert, channel_2_gain_t, 4,
			 JOERGER$_BAD_GAIN);
  read_integer_error(JOERGER_TR812_N_CHANNEL_3_GAIN, channel_3_gain, JOERGER$_BAD_GAIN);
  check_integer_conv_set(channel_3_gain, channel_3_gain_convert, channel_3_gain_t, 4,
			 JOERGER$_BAD_GAIN);
  read_integer_error(JOERGER_TR812_N_CHANNEL_4_GAIN, channel_4_gain, JOERGER$_BAD_GAIN);
  check_integer_conv_set(channel_4_gain, channel_4_gain_convert, channel_4_gain_t, 4,
			 JOERGER$_BAD_GAIN);
  read_integer_error(JOERGER_TR812_N_CHANNEL_5_GAIN, channel_5_gain, JOERGER$_BAD_GAIN);
  check_integer_conv_set(channel_5_gain, channel_5_gain_convert, channel_5_gain_t, 4,
			 JOERGER$_BAD_GAIN);
  read_integer_error(JOERGER_TR812_N_CHANNEL_6_GAIN, channel_6_gain, JOERGER$_BAD_GAIN);
  check_integer_conv_set(channel_6_gain, channel_6_gain_convert, channel_6_gain_t, 4,
			 JOERGER$_BAD_GAIN);
  read_integer_error(JOERGER_TR812_N_CHANNEL_7_GAIN, channel_7_gain, JOERGER$_BAD_GAIN);
  check_integer_conv_set(channel_7_gain, channel_7_gain_convert, channel_7_gain_t, 4,
			 JOERGER$_BAD_GAIN);
  read_integer_error(JOERGER_TR812_N_CHANNEL_8_GAIN, channel_8_gain, JOERGER$_BAD_GAIN);
  check_integer_conv_set(channel_8_gain, channel_8_gain_convert, channel_8_gain_t, 4,
			 JOERGER$_BAD_GAIN);
  read_descriptor(JOERGER_TR812_N_EXT_CLOCK, ext_clock);
  status = joerger_tr812___init(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array

extern int joerger_tr812___trigger();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}

EXPORT int joerger_tr812__trigger(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InTriggerStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InTriggerStruct)

      read_string_error(JOERGER_TR812_N_NAME, name, DEV$_BAD_NAME);
  status = joerger_tr812___trigger(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array

extern int joerger_tr812___store();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}

EXPORT int joerger_tr812__store(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InStoreStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InStoreStruct)

      read_string_error(JOERGER_TR812_N_NAME, name, DEV$_BAD_NAME);
  status = joerger_tr812___store(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array

extern int joerger_tr812___partial_store();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}

EXPORT int joerger_tr812__partial_store(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InPartial_storeStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InPartial_storeStruct)

      read_string_error(JOERGER_TR812_N_NAME, name, DEV$_BAD_NAME);
  status = joerger_tr812___partial_store(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array
