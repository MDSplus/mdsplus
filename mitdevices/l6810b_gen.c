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
#include "l6810b_gen.h"
EXPORT int l6810b__add(struct descriptor *name_d_ptr, struct descriptor *dummy_d_ptr __attribute__ ((unused)), int *nid_ptr)
{
  static DESCRIPTOR(library_d, "MIT$DEVICES");
  static DESCRIPTOR(model_d, "L6810B");
  static DESCRIPTOR_CONGLOM(conglom_d, &library_d, &model_d, 0, 0);
  int usage = TreeUSAGE_DEVICE;
  int curr_nid, old_nid, head_nid, status;
  long int flags = NciM_WRITE_ONCE;
  NCI_ITM flag_itm[] = { {2, NciSET_FLAGS, 0, 0}, {0, 0, 0, 0} };
  char *name_ptr = strncpy(malloc(name_d_ptr->length + 1), name_d_ptr->pointer, name_d_ptr->length);
  flag_itm[0].pointer = (unsigned char *)&flags;
  name_ptr[name_d_ptr->length] = 0;
  status = TreeStartConglomerate(L6810B_K_CONG_NODES);
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
 ADD_NODE_INTEGER(:FREQ, 1000000, TreeUSAGE_NUMERIC)
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
 ADD_NODE_INTEGER(:MEMORIES, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:SEGMENTS, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:ACTIVE_MEM, 128, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:ACTIVE_CHANS, 4, TreeUSAGE_NUMERIC)
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
#define expr " 10.24	"
 ADD_NODE_EXPR(INPUT_01:FULL_SCALE, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(INPUT_01:SRC_CPLING, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 128BU    "
 ADD_NODE_EXPR(INPUT_01:OFFSET, TreeUSAGE_NUMERIC)
#undef expr
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
#define expr " 10.24	"
 ADD_NODE_EXPR(INPUT_02:FULL_SCALE, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(INPUT_02:SRC_CPLING, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 128BU    "
 ADD_NODE_EXPR(INPUT_02:OFFSET, TreeUSAGE_NUMERIC)
#undef expr
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
#define expr " 10.24	"
 ADD_NODE_EXPR(INPUT_03:FULL_SCALE, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(INPUT_03:SRC_CPLING, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 128BU    "
 ADD_NODE_EXPR(INPUT_03:OFFSET, TreeUSAGE_NUMERIC)
#undef expr
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
#define expr " 10.24	"
 ADD_NODE_EXPR(INPUT_04:FULL_SCALE, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(INPUT_04:SRC_CPLING, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 128BU    "
 ADD_NODE_EXPR(INPUT_04:OFFSET, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_ACTION(:INIT_ACTION, INIT, INIT, 50, 0, 0, CAMAC_SERVER, 0)
 ADD_NODE_ACTION(:STORE_ACTION, STORE, STORE, 50, 0, 0, CAMAC_SERVER, 0)
      status = TreeEndConglomerate();
  if (!(status & 1))
    return status;
  return (TreeSetDefaultNid(old_nid));
}

EXPORT int l6810b__part_name(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)),
		      struct descriptor_d *out_d)
{
  int element = 0, status;
  NCI_ITM nci_list[] = { {4, NciCONGLOMERATE_ELT, 0, 0}, {0, 0, 0, 0} };
  nci_list[0].pointer = (unsigned char *)&element;
  status = TreeGetNci(*(int *)nid_d_ptr->pointer, nci_list);
  if (!(status & 1))
    return status;
  switch (element) {
  case (L6810B_N_HEAD + 1):
    StrFree1Dx(out_d);
    break;
  case (L6810B_N_NAME + 1):
 COPY_PART_NAME(:NAME) break;
  case (L6810B_N_COMMENT + 1):
 COPY_PART_NAME(:COMMENT) break;
  case (L6810B_N_FREQ + 1):
 COPY_PART_NAME(:FREQ) break;
  case (L6810B_N_EXT_CLOCK_IN + 1):
 COPY_PART_NAME(:EXT_CLOCK_IN) break;
  case (L6810B_N_STOP_TRIG + 1):
 COPY_PART_NAME(:STOP_TRIG) break;
  case (L6810B_N_MEMORIES + 1):
 COPY_PART_NAME(:MEMORIES) break;
  case (L6810B_N_SEGMENTS + 1):
 COPY_PART_NAME(:SEGMENTS) break;
  case (L6810B_N_ACTIVE_MEM + 1):
 COPY_PART_NAME(:ACTIVE_MEM) break;
  case (L6810B_N_ACTIVE_CHANS + 1):
 COPY_PART_NAME(:ACTIVE_CHANS) break;
  case (L6810B_N_INPUT_01 + 1):
 COPY_PART_NAME(:INPUT_01) break;
  case (L6810B_N_INPUT_01_STARTIDX + 1):
 COPY_PART_NAME(INPUT_01:STARTIDX) break;
  case (L6810B_N_INPUT_01_ENDIDX + 1):
 COPY_PART_NAME(INPUT_01:ENDIDX) break;
  case (L6810B_N_INPUT_01_FULL_SCALE + 1):
 COPY_PART_NAME(INPUT_01:FULL_SCALE) break;
  case (L6810B_N_INPUT_01_SRC_CPLING + 1):
 COPY_PART_NAME(INPUT_01:SRC_CPLING) break;
  case (L6810B_N_INPUT_01_OFFSET + 1):
 COPY_PART_NAME(INPUT_01:OFFSET) break;
  case (L6810B_N_INPUT_02 + 1):
 COPY_PART_NAME(:INPUT_02) break;
  case (L6810B_N_INPUT_02_STARTIDX + 1):
 COPY_PART_NAME(INPUT_02:STARTIDX) break;
  case (L6810B_N_INPUT_02_ENDIDX + 1):
 COPY_PART_NAME(INPUT_02:ENDIDX) break;
  case (L6810B_N_INPUT_02_FULL_SCALE + 1):
 COPY_PART_NAME(INPUT_02:FULL_SCALE) break;
  case (L6810B_N_INPUT_02_SRC_CPLING + 1):
 COPY_PART_NAME(INPUT_02:SRC_CPLING) break;
  case (L6810B_N_INPUT_02_OFFSET + 1):
 COPY_PART_NAME(INPUT_02:OFFSET) break;
  case (L6810B_N_INPUT_03 + 1):
 COPY_PART_NAME(:INPUT_03) break;
  case (L6810B_N_INPUT_03_STARTIDX + 1):
 COPY_PART_NAME(INPUT_03:STARTIDX) break;
  case (L6810B_N_INPUT_03_ENDIDX + 1):
 COPY_PART_NAME(INPUT_03:ENDIDX) break;
  case (L6810B_N_INPUT_03_FULL_SCALE + 1):
 COPY_PART_NAME(INPUT_03:FULL_SCALE) break;
  case (L6810B_N_INPUT_03_SRC_CPLING + 1):
 COPY_PART_NAME(INPUT_03:SRC_CPLING) break;
  case (L6810B_N_INPUT_03_OFFSET + 1):
 COPY_PART_NAME(INPUT_03:OFFSET) break;
  case (L6810B_N_INPUT_04 + 1):
 COPY_PART_NAME(:INPUT_04) break;
  case (L6810B_N_INPUT_04_STARTIDX + 1):
 COPY_PART_NAME(INPUT_04:STARTIDX) break;
  case (L6810B_N_INPUT_04_ENDIDX + 1):
 COPY_PART_NAME(INPUT_04:ENDIDX) break;
  case (L6810B_N_INPUT_04_FULL_SCALE + 1):
 COPY_PART_NAME(INPUT_04:FULL_SCALE) break;
  case (L6810B_N_INPUT_04_SRC_CPLING + 1):
 COPY_PART_NAME(INPUT_04:SRC_CPLING) break;
  case (L6810B_N_INPUT_04_OFFSET + 1):
 COPY_PART_NAME(INPUT_04:OFFSET) break;
  case (L6810B_N_INIT_ACTION + 1):
 COPY_PART_NAME(:INIT_ACTION) break;
  case (L6810B_N_STORE_ACTION + 1):
 COPY_PART_NAME(:STORE_ACTION) break;
  default:
    status = TreeILLEGAL_ITEM;
  }
  return status;
}

extern int l6810b___init();
#define free_xd_array { int i; for(i=0; i<2;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}
#define error(nid,code,code1) {free_xd_array return GenDeviceSignal(nid,code,code1);}

EXPORT int l6810b__init(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InInitStruct)
      static struct {
    short code;
    int value;
  } active_mem_t[] = {
    {0, 1},
    {1, 2},
    {2, 4},
    {3, 8},
    {4, 16}, {5, 32}, {6, 64}, {7, 128}, {8, 256}, {9, 512},
    {10, 1024}, {11, 2048}, {12, 4096}, {13, 8192}, {0, 0}};
  static struct {
    short code;
    int value;
  } active_chans_t[] = {
    {0, 1}, {1, 2}, {2, 4}, {0, 0}};
  static struct {
    short code;
    int value;
  } freq_t[] = {
    {0, 0},
    {1, 20},
    {2, 50},
    {3, 100},
    {4, 200},
    {5, 500},
    {6, 1000},
    {7, 2000},
    {8, 5000},
    {9, 10000},
    {10, 20000},
    {11, 50000}, {12, 100000}, {13, 200000}, {14, 500000},
    {15, 1000000}, {16, 2000000}, {17, 5000000}, {0, 0}};
  static struct {
    short code;
    float value;
  } input_01_full_scale_t[] = {
    {0, .4096}, {1, 1.024}, {2, 2.048}, {3, 4.096},
    {4, 10.24}, {5, 25.6}, {6, 51.2}, {7, 102.4}, {0, 0}};
  static struct {
    short code;
    float value;
  } input_02_full_scale_t[] = {
    {0, .4096}, {1, 1.024}, {2, 2.048}, {3, 4.096},
    {4, 10.24}, {5, 25.6}, {6, 51.2}, {7, 102.4}, {0, 0}};
  static struct {
    short code;
    float value;
  } input_03_full_scale_t[] = {
    {0, .4096}, {1, 1.024}, {2, 2.048}, {3, 4.096}, {4, 10.24}, {5, 25.6},
    {6, 51.2}, {7, 102.4}, {0, 0}};
  static struct {
    short code;
    float value;
  } input_04_full_scale_t[] = {
    {0, .4096}, {1, 1.024}, {2, 2.048}, {3, 4.096}, {4, 10.24}, {5, 25.6},\
    {6, 51.2}, {7, 102.4}, {0, 0}};
  struct descriptor_xd work_xd[2];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 2);
  initialize_variables(InInitStruct)

      read_string_error(L6810B_N_NAME, name, DEV$_BAD_NAME);
  read_integer_error(L6810B_N_MEMORIES, memories, L6810$_BAD_MEMORIES);
  check_range(memories, 1, 16, L6810$_BAD_MEMORIES);
  read_integer_error(L6810B_N_SEGMENTS, segments, L6810$_BAD_SEGMENTS);
  check_range(segments, 1, 1024, L6810$_BAD_SEGMENTS);
  read_integer_error(L6810B_N_ACTIVE_MEM, active_mem, L6810$_BAD_ACTIVEMEM);
  check_integer_conv_set(active_mem, active_mem_convert, active_mem_t, 14, L6810$_BAD_ACTIVEMEM);
  read_integer_error(L6810B_N_ACTIVE_CHANS, active_chans, L6810$_BAD_ACTIVECHAN);
  check_integer_set(active_chans, active_chans_t, 3, L6810$_BAD_ACTIVECHAN);
  read_integer_error(L6810B_N_FREQ, freq, L6810$_BAD_FREQUENCY);
  check_integer_conv_set(freq, freq_convert, freq_t, 18, L6810$_BAD_FREQUENCY);
  read_float_error(L6810B_N_INPUT_01_FULL_SCALE, input_01_full_scale, L6810$_BAD_FULL_SCALE);
  check_float_conv_set(input_01_full_scale, input_01_full_scale_convert, input_01_full_scale_t, 8,
		       L6810$_BAD_FULL_SCALE);
  read_float_error(L6810B_N_INPUT_02_FULL_SCALE, input_02_full_scale, L6810$_BAD_FULL_SCALE);
  check_float_conv_set(input_02_full_scale, input_02_full_scale_convert, input_02_full_scale_t, 8,
		       L6810$_BAD_FULL_SCALE);
  read_float_error(L6810B_N_INPUT_03_FULL_SCALE, input_03_full_scale, L6810$_BAD_FULL_SCALE);
  check_float_conv_set(input_03_full_scale, input_03_full_scale_convert, input_03_full_scale_t, 8,
		       L6810$_BAD_FULL_SCALE);
  read_float_error(L6810B_N_INPUT_04_FULL_SCALE, input_04_full_scale, L6810$_BAD_FULL_SCALE);
  check_float_conv_set(input_04_full_scale, input_04_full_scale_convert, input_04_full_scale_t, 8,
		       L6810$_BAD_FULL_SCALE);
  read_integer_error(L6810B_N_INPUT_01_SRC_CPLING, input_01_src_cpling, L6810$_BAD_COUPLING);
  check_range(input_01_src_cpling, 0, 7, L6810$_BAD_COUPLING);
  read_integer_error(L6810B_N_INPUT_02_SRC_CPLING, input_02_src_cpling, L6810$_BAD_COUPLING);
  check_range(input_02_src_cpling, 0, 7, L6810$_BAD_COUPLING);
  read_integer_error(L6810B_N_INPUT_03_SRC_CPLING, input_03_src_cpling, L6810$_BAD_COUPLING);
  check_range(input_03_src_cpling, 0, 7, L6810$_BAD_COUPLING);
  read_integer_error(L6810B_N_INPUT_04_SRC_CPLING, input_04_src_cpling, L6810$_BAD_COUPLING);
  check_range(input_04_src_cpling, 0, 7, L6810$_BAD_COUPLING);
  read_integer_error(L6810B_N_INPUT_01_OFFSET, input_01_offset, L6810$_BAD_OFFSET);
  check_range(input_01_offset, 0, 255, L6810$_BAD_OFFSET);
  read_integer_error(L6810B_N_INPUT_02_OFFSET, input_02_offset, L6810$_BAD_OFFSET);
  check_range(input_02_offset, 0, 255, L6810$_BAD_OFFSET);
  read_integer_error(L6810B_N_INPUT_03_OFFSET, input_03_offset, L6810$_BAD_OFFSET);
  check_range(input_03_offset, 0, 255, L6810$_BAD_OFFSET);
  read_integer_error(L6810B_N_INPUT_04_OFFSET, input_04_offset, L6810$_BAD_OFFSET);
  check_range(input_04_offset, 0, 255, L6810$_BAD_OFFSET);
  read_descriptor(L6810B_N_EXT_CLOCK_IN, ext_clock_in);
  status = l6810b___init(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array

extern int l6810b___trigger();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}

EXPORT int l6810b__trigger(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InTriggerStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InTriggerStruct)

      read_string_error(L6810B_N_NAME, name, DEV$_BAD_NAME);
  status = l6810b___trigger(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array

extern int l6810b___store();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}

EXPORT int l6810b__store(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InStoreStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InStoreStruct)

      read_string_error(L6810B_N_NAME, name, DEV$_BAD_NAME);
  status = l6810b___store(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array
