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
#include "incaa16_gen.h"
EXPORT int incaa16__add(struct descriptor *name_d_ptr, struct descriptor *dummy_d_ptr __attribute__ ((unused)), int *nid_ptr)
{
  static DESCRIPTOR(library_d, "MIT$DEVICES");
  static DESCRIPTOR(model_d, "INCAA16");
  static DESCRIPTOR_CONGLOM(conglom_d, &library_d, &model_d, 0, 0);
  int usage = TreeUSAGE_DEVICE;
  int curr_nid, old_nid, head_nid, status;
  long int flags = NciM_WRITE_ONCE;
  NCI_ITM flag_itm[] = { {2, NciSET_FLAGS, 0, 0}, {0, 0, 0, 0} };
  char *name_ptr = strncpy(malloc(name_d_ptr->length + 1), name_d_ptr->pointer, name_d_ptr->length);
  flag_itm[0].pointer = (unsigned char *)&flags;
  name_ptr[name_d_ptr->length] = 0;
  status = TreeStartConglomerate(INCAA16_K_CONG_NODES);
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
#define expr " 5.0       "
 ADD_NODE_EXPR(:INT_CLK_FRQ, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:CLOCK_IN, TreeUSAGE_AXIS)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:CLOCK_OUT, TreeUSAGE_AXIS)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0.	"
 ADD_NODE_EXPR(:TRIG_IN, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " TRIG_IN   "
 ADD_NODE_EXPR(:TRIG_OUT, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:MASTER, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:EXT_1MHZ, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:PTSC, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:ACTIVE_CHANS, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_01, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_01:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_01:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_02, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_02:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_02:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_03, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_03:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_03:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_04, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_04:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_04:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_05, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_05:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_05:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_06, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_06:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_06:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_07, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_07:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_07:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_08, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_08:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_08:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_09, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_09:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_09:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_10, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_10:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_10:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_11, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_11:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_11:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_12, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_12:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_12:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_13, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_13:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_13:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_14, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_14:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_14:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_15, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_15:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_15:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_16, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_16:STARTIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_16:ENDIDX, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_ACTION(:INIT_ACTION, INIT, INIT, 50, 0, 0, CAMAC_SERVER, 0)
 ADD_NODE_ACTION(:STORE_ACTION, STORE, STORE, 50, 0, 0, CAMAC_SERVER, 0)
      status = TreeEndConglomerate();
  if (!(status & 1))
    return status;
  return (TreeSetDefaultNid(old_nid));
}

EXPORT int incaa16__part_name(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)),
		       struct descriptor_d *out_d)
{
  int element = 0, status;
  NCI_ITM nci_list[] = { {4, NciCONGLOMERATE_ELT, 0, 0}, {0, 0, 0, 0} };
  nci_list[0].pointer = (unsigned char *)&element;
  status = TreeGetNci(*(int *)nid_d_ptr->pointer, nci_list);
  if (!(status & 1))
    return status;
  switch (element) {
  case (INCAA16_N_HEAD + 1):
    StrFree1Dx(out_d);
    break;
  case (INCAA16_N_NAME + 1):
 COPY_PART_NAME(:NAME) break;
  case (INCAA16_N_COMMENT + 1):
 COPY_PART_NAME(:COMMENT) break;
  case (INCAA16_N_INT_CLK_FRQ + 1):
 COPY_PART_NAME(:INT_CLK_FRQ) break;
  case (INCAA16_N_CLOCK_IN + 1):
 COPY_PART_NAME(:CLOCK_IN) break;
  case (INCAA16_N_CLOCK_OUT + 1):
 COPY_PART_NAME(:CLOCK_OUT) break;
  case (INCAA16_N_TRIG_IN + 1):
 COPY_PART_NAME(:TRIG_IN) break;
  case (INCAA16_N_TRIG_OUT + 1):
 COPY_PART_NAME(:TRIG_OUT) break;
  case (INCAA16_N_MASTER + 1):
 COPY_PART_NAME(:MASTER) break;
  case (INCAA16_N_EXT_1MHZ + 1):
 COPY_PART_NAME(:EXT_1MHZ) break;
  case (INCAA16_N_PTSC + 1):
 COPY_PART_NAME(:PTSC) break;
  case (INCAA16_N_ACTIVE_CHANS + 1):
 COPY_PART_NAME(:ACTIVE_CHANS) break;
  case (INCAA16_N_INPUT_01 + 1):
 COPY_PART_NAME(:INPUT_01) break;
  case (INCAA16_N_INPUT_01_STARTIDX + 1):
 COPY_PART_NAME(INPUT_01:STARTIDX) break;
  case (INCAA16_N_INPUT_01_ENDIDX + 1):
 COPY_PART_NAME(INPUT_01:ENDIDX) break;
  case (INCAA16_N_INPUT_02 + 1):
 COPY_PART_NAME(:INPUT_02) break;
  case (INCAA16_N_INPUT_02_STARTIDX + 1):
 COPY_PART_NAME(INPUT_02:STARTIDX) break;
  case (INCAA16_N_INPUT_02_ENDIDX + 1):
 COPY_PART_NAME(INPUT_02:ENDIDX) break;
  case (INCAA16_N_INPUT_03 + 1):
 COPY_PART_NAME(:INPUT_03) break;
  case (INCAA16_N_INPUT_03_STARTIDX + 1):
 COPY_PART_NAME(INPUT_03:STARTIDX) break;
  case (INCAA16_N_INPUT_03_ENDIDX + 1):
 COPY_PART_NAME(INPUT_03:ENDIDX) break;
  case (INCAA16_N_INPUT_04 + 1):
 COPY_PART_NAME(:INPUT_04) break;
  case (INCAA16_N_INPUT_04_STARTIDX + 1):
 COPY_PART_NAME(INPUT_04:STARTIDX) break;
  case (INCAA16_N_INPUT_04_ENDIDX + 1):
 COPY_PART_NAME(INPUT_04:ENDIDX) break;
  case (INCAA16_N_INPUT_05 + 1):
 COPY_PART_NAME(:INPUT_05) break;
  case (INCAA16_N_INPUT_05_STARTIDX + 1):
 COPY_PART_NAME(INPUT_05:STARTIDX) break;
  case (INCAA16_N_INPUT_05_ENDIDX + 1):
 COPY_PART_NAME(INPUT_05:ENDIDX) break;
  case (INCAA16_N_INPUT_06 + 1):
 COPY_PART_NAME(:INPUT_06) break;
  case (INCAA16_N_INPUT_06_STARTIDX + 1):
 COPY_PART_NAME(INPUT_06:STARTIDX) break;
  case (INCAA16_N_INPUT_06_ENDIDX + 1):
 COPY_PART_NAME(INPUT_06:ENDIDX) break;
  case (INCAA16_N_INPUT_07 + 1):
 COPY_PART_NAME(:INPUT_07) break;
  case (INCAA16_N_INPUT_07_STARTIDX + 1):
 COPY_PART_NAME(INPUT_07:STARTIDX) break;
  case (INCAA16_N_INPUT_07_ENDIDX + 1):
 COPY_PART_NAME(INPUT_07:ENDIDX) break;
  case (INCAA16_N_INPUT_08 + 1):
 COPY_PART_NAME(:INPUT_08) break;
  case (INCAA16_N_INPUT_08_STARTIDX + 1):
 COPY_PART_NAME(INPUT_08:STARTIDX) break;
  case (INCAA16_N_INPUT_08_ENDIDX + 1):
 COPY_PART_NAME(INPUT_08:ENDIDX) break;
  case (INCAA16_N_INPUT_09 + 1):
 COPY_PART_NAME(:INPUT_09) break;
  case (INCAA16_N_INPUT_09_STARTIDX + 1):
 COPY_PART_NAME(INPUT_09:STARTIDX) break;
  case (INCAA16_N_INPUT_09_ENDIDX + 1):
 COPY_PART_NAME(INPUT_09:ENDIDX) break;
  case (INCAA16_N_INPUT_10 + 1):
 COPY_PART_NAME(:INPUT_10) break;
  case (INCAA16_N_INPUT_10_STARTIDX + 1):
 COPY_PART_NAME(INPUT_10:STARTIDX) break;
  case (INCAA16_N_INPUT_10_ENDIDX + 1):
 COPY_PART_NAME(INPUT_10:ENDIDX) break;
  case (INCAA16_N_INPUT_11 + 1):
 COPY_PART_NAME(:INPUT_11) break;
  case (INCAA16_N_INPUT_11_STARTIDX + 1):
 COPY_PART_NAME(INPUT_11:STARTIDX) break;
  case (INCAA16_N_INPUT_11_ENDIDX + 1):
 COPY_PART_NAME(INPUT_11:ENDIDX) break;
  case (INCAA16_N_INPUT_12 + 1):
 COPY_PART_NAME(:INPUT_12) break;
  case (INCAA16_N_INPUT_12_STARTIDX + 1):
 COPY_PART_NAME(INPUT_12:STARTIDX) break;
  case (INCAA16_N_INPUT_12_ENDIDX + 1):
 COPY_PART_NAME(INPUT_12:ENDIDX) break;
  case (INCAA16_N_INPUT_13 + 1):
 COPY_PART_NAME(:INPUT_13) break;
  case (INCAA16_N_INPUT_13_STARTIDX + 1):
 COPY_PART_NAME(INPUT_13:STARTIDX) break;
  case (INCAA16_N_INPUT_13_ENDIDX + 1):
 COPY_PART_NAME(INPUT_13:ENDIDX) break;
  case (INCAA16_N_INPUT_14 + 1):
 COPY_PART_NAME(:INPUT_14) break;
  case (INCAA16_N_INPUT_14_STARTIDX + 1):
 COPY_PART_NAME(INPUT_14:STARTIDX) break;
  case (INCAA16_N_INPUT_14_ENDIDX + 1):
 COPY_PART_NAME(INPUT_14:ENDIDX) break;
  case (INCAA16_N_INPUT_15 + 1):
 COPY_PART_NAME(:INPUT_15) break;
  case (INCAA16_N_INPUT_15_STARTIDX + 1):
 COPY_PART_NAME(INPUT_15:STARTIDX) break;
  case (INCAA16_N_INPUT_15_ENDIDX + 1):
 COPY_PART_NAME(INPUT_15:ENDIDX) break;
  case (INCAA16_N_INPUT_16 + 1):
 COPY_PART_NAME(:INPUT_16) break;
  case (INCAA16_N_INPUT_16_STARTIDX + 1):
 COPY_PART_NAME(INPUT_16:STARTIDX) break;
  case (INCAA16_N_INPUT_16_ENDIDX + 1):
 COPY_PART_NAME(INPUT_16:ENDIDX) break;
  case (INCAA16_N_INIT_ACTION + 1):
 COPY_PART_NAME(:INIT_ACTION) break;
  case (INCAA16_N_STORE_ACTION + 1):
 COPY_PART_NAME(:STORE_ACTION) break;
  default:
    status = TreeILLEGAL_ITEM;
  }
  return status;
}

extern int incaa16___init();
#define free_xd_array { int i; for(i=0; i<2;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}
#define error(nid,code,code1) {free_xd_array return GenDeviceSignal(nid,code,code1);}

EXPORT int incaa16__init(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InInitStruct)
      static struct {
    short code;
    int value;
  } active_chans_t[] = {
    {0, 1}, {1, 2}, {2, 4}, {3, 8}, {4, 16}, {0, 0}};
  static struct {
    short code;
    int value;
  } master_t[] = {
    {0, 0}, {1, 1}, {0, 0}};
  static struct {
    short code;
    int value;
  } ext_1mhz_t[] = {
    {0, 0}, {1, 1}, {0, 0}};
  struct descriptor_xd work_xd[2];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 2);
  initialize_variables(InInitStruct)

      read_string_error(INCAA16_N_NAME, name, DEV$_BAD_NAME);
  read_descriptor(INCAA16_N_INT_CLK_FRQ, int_clk_frq);
  read_integer_error(INCAA16_N_ACTIVE_CHANS, active_chans, INCAA$_BAD_ACTIVE_CHANS);
  check_integer_conv_set(active_chans, active_chans_convert, active_chans_t, 5,
			 INCAA$_BAD_ACTIVE_CHANS);
  read_integer_error(INCAA16_N_MASTER, master, INCAA$_BAD_MASTER);
  check_integer_set(master, master_t, 2, INCAA$_BAD_MASTER);
  read_integer_error(INCAA16_N_EXT_1MHZ, ext_1mhz, INCAA$_BAD_EXT_1MHZ);
  check_integer_set(ext_1mhz, ext_1mhz_t, 2, INCAA$_BAD_EXT_1MHZ);
  read_integer_error(INCAA16_N_PTSC, ptsc, INCAA$_BAD_PTSC);
  status = incaa16___init(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array

extern int incaa16___arm();
#define free_xd_array { int i; for(i=0; i<2;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}

EXPORT int incaa16__arm(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InArmStruct)
      static struct {
    short code;
    int value;
  } active_chans_t[] = {
    {0, 1}, {1, 2}, {2, 4}, {3, 8}, {4, 16}, {0, 0}};
  static struct {
    short code;
    int value;
  } master_t[] = {
    {0, 0}, {1, 1}, {0, 0}};
  static struct {
    short code;
    int value;
  } ext_1mhz_t[] = {
    {0, 0}, {1, 1}, {0, 0}};
  struct descriptor_xd work_xd[2];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 2);
  initialize_variables(InArmStruct)

      read_string_error(INCAA16_N_NAME, name, DEV$_BAD_NAME);
  read_descriptor(INCAA16_N_INT_CLK_FRQ, int_clk_frq);
  read_integer_error(INCAA16_N_ACTIVE_CHANS, active_chans, INCAA$_BAD_ACTIVE_CHANS);
  check_integer_conv_set(active_chans, active_chans_convert, active_chans_t, 5,
			 INCAA$_BAD_ACTIVE_CHANS);
  read_integer_error(INCAA16_N_MASTER, master, INCAA$_BAD_MASTER);
  check_integer_set(master, master_t, 2, INCAA$_BAD_MASTER);
  read_integer_error(INCAA16_N_EXT_1MHZ, ext_1mhz, INCAA$_BAD_EXT_1MHZ);
  check_integer_set(ext_1mhz, ext_1mhz_t, 2, INCAA$_BAD_EXT_1MHZ);
  read_integer_error(INCAA16_N_PTSC, ptsc, INCAA$_BAD_PTSC);
  status = incaa16___arm(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array

extern int incaa16___trigger();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}

EXPORT int incaa16__trigger(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InTriggerStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InTriggerStruct)

      read_string_error(INCAA16_N_NAME, name, DEV$_BAD_NAME);
  status = incaa16___trigger(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array

extern int incaa16___store();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}

EXPORT int incaa16__store(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InStoreStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InStoreStruct)

      read_string_error(INCAA16_N_NAME, name, DEV$_BAD_NAME);
  read_integer_error(INCAA16_N_PTSC, ptsc, INCAA$_BAD_PTSC);
  status = incaa16___store(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array
