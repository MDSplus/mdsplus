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
#include "a14_gen.h"
EXPORT int a14__add(struct descriptor *name_d_ptr, struct descriptor *dummy_d_ptr __attribute__ ((unused)), int *nid_ptr)
{
  static DESCRIPTOR(library_d, "MIT$DEVICES");
  static DESCRIPTOR(model_d, "A14");
  static DESCRIPTOR_CONGLOM(conglom_d, &library_d, &model_d, 0, 0);
  int usage = TreeUSAGE_DEVICE;
  int curr_nid, old_nid, head_nid, status;
  long int flags = NciM_WRITE_ONCE;
  NCI_ITM flag_itm[] = { {2, NciSET_FLAGS, 0, 0}, {0, 0, 0, 0} };
  char *name_ptr = strncpy(malloc(name_d_ptr->length + 1), name_d_ptr->pointer, name_d_ptr->length);
  flag_itm[0].pointer = (unsigned char *)&flags;
  name_ptr[name_d_ptr->length] = 0;
  status = TreeStartConglomerate(A14_K_CONG_NODES);
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
 ADD_NODE_INTEGER(:CLOCK_DIVIDE, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:EXT_CLOCK_IN, TreeUSAGE_AXIS)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0.	"
 ADD_NODE_EXPR(:STOP_TRIG, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:PTS, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:MODE, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:DIMENSION, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:CLK_POLARITY, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:STR_POLARITY, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:STP_POLARITY, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:GATED, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_1, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_1:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_1:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_2, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_2:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_2:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_3, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_3:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_3:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_4, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_4:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_4:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_5, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_5:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_5:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_6, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_6:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_6:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_ACTION(:INIT_ACTION, INIT, INIT, 50, 0, 0, CAMAC_SERVER, 0)
 ADD_NODE_ACTION(:STORE_ACTION, STORE, STORE, 50, 0, 0, CAMAC_SERVER, 0)
      status = TreeEndConglomerate();
  if (!(status & 1))
    return status;
  return (TreeSetDefaultNid(old_nid));
}

EXPORT int a14__part_name(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)),
		   struct descriptor_d *out_d)
{
  int element = 0, status;
  NCI_ITM nci_list[] = { {4, NciCONGLOMERATE_ELT, 0, 0}, {0, 0, 0, 0} };
  nci_list[0].pointer = (unsigned char *)&element;
  status = TreeGetNci(*(int *)nid_d_ptr->pointer, nci_list);
  if (!(status & 1))
    return status;
  switch (element) {
  case (A14_N_HEAD + 1):
    StrFree1Dx(out_d);
    break;
  case (A14_N_NAME + 1):
 COPY_PART_NAME(:NAME) break;
  case (A14_N_COMMENT + 1):
 COPY_PART_NAME(:COMMENT) break;
  case (A14_N_CLOCK_DIVIDE + 1):
 COPY_PART_NAME(:CLOCK_DIVIDE) break;
  case (A14_N_EXT_CLOCK_IN + 1):
 COPY_PART_NAME(:EXT_CLOCK_IN) break;
  case (A14_N_STOP_TRIG + 1):
 COPY_PART_NAME(:STOP_TRIG) break;
  case (A14_N_PTS + 1):
 COPY_PART_NAME(:PTS) break;
  case (A14_N_MODE + 1):
 COPY_PART_NAME(:MODE) break;
  case (A14_N_DIMENSION + 1):
 COPY_PART_NAME(:DIMENSION) break;
  case (A14_N_CLK_POLARITY + 1):
 COPY_PART_NAME(:CLK_POLARITY) break;
  case (A14_N_STR_POLARITY + 1):
 COPY_PART_NAME(:STR_POLARITY) break;
  case (A14_N_STP_POLARITY + 1):
 COPY_PART_NAME(:STP_POLARITY) break;
  case (A14_N_GATED + 1):
 COPY_PART_NAME(:GATED) break;
  case (A14_N_INPUT_1 + 1):
 COPY_PART_NAME(:INPUT_1) break;
  case (A14_N_INPUT_1_STARTIDX + 1):
 COPY_PART_NAME(INPUT_1:STARTIDX) break;
  case (A14_N_INPUT_1_ENDIDX + 1):
 COPY_PART_NAME(INPUT_1:ENDIDX) break;
  case (A14_N_INPUT_2 + 1):
 COPY_PART_NAME(:INPUT_2) break;
  case (A14_N_INPUT_2_STARTIDX + 1):
 COPY_PART_NAME(INPUT_2:STARTIDX) break;
  case (A14_N_INPUT_2_ENDIDX + 1):
 COPY_PART_NAME(INPUT_2:ENDIDX) break;
  case (A14_N_INPUT_3 + 1):
 COPY_PART_NAME(:INPUT_3) break;
  case (A14_N_INPUT_3_STARTIDX + 1):
 COPY_PART_NAME(INPUT_3:STARTIDX) break;
  case (A14_N_INPUT_3_ENDIDX + 1):
 COPY_PART_NAME(INPUT_3:ENDIDX) break;
  case (A14_N_INPUT_4 + 1):
 COPY_PART_NAME(:INPUT_4) break;
  case (A14_N_INPUT_4_STARTIDX + 1):
 COPY_PART_NAME(INPUT_4:STARTIDX) break;
  case (A14_N_INPUT_4_ENDIDX + 1):
 COPY_PART_NAME(INPUT_4:ENDIDX) break;
  case (A14_N_INPUT_5 + 1):
 COPY_PART_NAME(:INPUT_5) break;
  case (A14_N_INPUT_5_STARTIDX + 1):
 COPY_PART_NAME(INPUT_5:STARTIDX) break;
  case (A14_N_INPUT_5_ENDIDX + 1):
 COPY_PART_NAME(INPUT_5:ENDIDX) break;
  case (A14_N_INPUT_6 + 1):
 COPY_PART_NAME(:INPUT_6) break;
  case (A14_N_INPUT_6_STARTIDX + 1):
 COPY_PART_NAME(INPUT_6:STARTIDX) break;
  case (A14_N_INPUT_6_ENDIDX + 1):
 COPY_PART_NAME(INPUT_6:ENDIDX) break;
  case (A14_N_INIT_ACTION + 1):
 COPY_PART_NAME(:INIT_ACTION) break;
  case (A14_N_STORE_ACTION + 1):
 COPY_PART_NAME(:STORE_ACTION) break;
  default:
    status = TreeILLEGAL_ITEM;
  }
  return status;
}

extern int a14___init();
#define free_xd_array { int i; for(i=0; i<2;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}
#define error(nid,code,code1) {free_xd_array return GenDeviceSignal(nid,code,code1);}

EXPORT int a14__init(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InInitStruct)
      static struct {
    short code;
    int value;
      } clock_divide_t[] = {
    {0, 1}, {1, 2}, {2, 4}, {3, 10}, {4, 20}, {5, 40}, {6, 100}, {0, 0}};
  static struct {
    short code;
    int value;
  } clk_polarity_t[] = {
    {0, 0}, {1, 1}, {0, 0}};
  static struct {
    short code;
    int value;
  } str_polarity_t[] = {
    {0, 0}, {1, 1}, {0, 0}};
  static struct {
    short code;
    int value;
  } stp_polarity_t[] = {
    {0, 0}, {1, 1}, {0, 0}};
  static struct {
    short code;
    int value;
  } gated_t[] = {
    {0, 0}, {1, 1}, {0, 0}};
  struct descriptor_xd work_xd[2];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 2);
  initialize_variables(InInitStruct)

      read_string_error(A14_N_NAME, name, DEV$_BAD_NAME);
  read_integer_error(A14_N_CLOCK_DIVIDE, clock_divide, A14$_BAD_CLK_DIVIDE);
  check_integer_conv_set(clock_divide, clock_divide_convert, clock_divide_t, 7,
			 A14$_BAD_CLK_DIVIDE);
  read_descriptor(A14_N_EXT_CLOCK_IN, ext_clock_in);
  read_integer(A14_N_PTS, pts);
  read_integer_error(A14_N_MODE, mode, A14$_BAD_MODE);
  check_range(mode, 0, 4, A14$_BAD_MODE);
  read_integer_error(A14_N_CLK_POLARITY, clk_polarity, A14$_BAD_CLK_POLARITY);
  check_integer_set(clk_polarity, clk_polarity_t, 2, A14$_BAD_CLK_POLARITY);
  read_integer_error(A14_N_STR_POLARITY, str_polarity, A14$_BAD_STR_POLARITY);
  check_integer_set(str_polarity, str_polarity_t, 2, A14$_BAD_STR_POLARITY);
  read_integer_error(A14_N_STP_POLARITY, stp_polarity, A14$_BAD_STP_POLARITY);
  check_integer_set(stp_polarity, stp_polarity_t, 2, A14$_BAD_STP_POLARITY);
  read_integer_error(A14_N_GATED, gated, A14$_BAD_GATED);
  check_integer_set(gated, gated_t, 2, A14$_BAD_GATED);
  status = a14___init(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array

extern int a14___start();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}

EXPORT int a14__start(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InStartStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InStartStruct)

      read_string_error(A14_N_NAME, name, DEV$_BAD_NAME);
  status = a14___start(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array

extern int a14___stop();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}

EXPORT int a14__stop(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InStopStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InStopStruct)

      read_string_error(A14_N_NAME, name, DEV$_BAD_NAME);
  status = a14___stop(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array

extern int a14___store();
#define free_xd_array { int i; for(i=0; i<3;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}

EXPORT int a14__store(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InStoreStruct)
  struct descriptor_xd work_xd[3];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 3);
  initialize_variables(InStoreStruct)

      read_string_error(A14_N_NAME, name, DEV$_BAD_NAME);
  read_descriptor(A14_N_EXT_CLOCK_IN, ext_clock_in);
  read_descriptor(A14_N_DIMENSION, dimension);
  status = a14___store(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array
