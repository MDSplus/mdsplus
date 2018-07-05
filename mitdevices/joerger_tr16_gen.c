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
#include "joerger_tr16_gen.h"
EXPORT int joerger_tr16__add(struct descriptor *name_d_ptr, struct descriptor *dummy_d_ptr __attribute__ ((unused)), int *nid_ptr)
{
  static DESCRIPTOR(library_d, "MIT$DEVICES");
  static DESCRIPTOR(model_d, "JOERGER_TR16");
  static DESCRIPTOR_CONGLOM(conglom_d, &library_d, &model_d, 0, 0);
  int usage = TreeUSAGE_DEVICE;
  int curr_nid, old_nid, head_nid, status;
  long int flags = NciM_WRITE_ONCE;
  NCI_ITM flag_itm[] = { {2, NciSET_FLAGS, 0, 0}, {0, 0, 0, 0} };
  char *name_ptr = strncpy(malloc(name_d_ptr->length + 1), name_d_ptr->pointer, name_d_ptr->length);
  flag_itm[0].pointer = (unsigned char *)&flags;
  name_ptr[name_d_ptr->length] = 0;
  status = TreeStartConglomerate(JOERGER_TR16_K_CONG_NODES);
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
 ADD_NODE_INTEGER(:FREQ, 10, TreeUSAGE_NUMERIC)
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
 ADD_NODE_INTEGER(:MEM_SIZE, 256, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:ACTIVE_MEM, 256, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:ACTIVE_CHANS, 16, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:POST_TRIG, 16384, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:MASTER, 1, TreeUSAGE_NUMERIC)
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
 ADD_NODE_INTEGER(INPUT_01:GAIN, 1, TreeUSAGE_NUMERIC)
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
 ADD_NODE_INTEGER(INPUT_02:GAIN, 1, TreeUSAGE_NUMERIC)
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
 ADD_NODE_INTEGER(INPUT_03:GAIN, 1, TreeUSAGE_NUMERIC)
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
 ADD_NODE_INTEGER(INPUT_04:GAIN, 1, TreeUSAGE_NUMERIC)
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
 ADD_NODE_INTEGER(INPUT_05:GAIN, 1, TreeUSAGE_NUMERIC)
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
 ADD_NODE_INTEGER(INPUT_06:GAIN, 1, TreeUSAGE_NUMERIC)
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
 ADD_NODE_INTEGER(INPUT_07:GAIN, 1, TreeUSAGE_NUMERIC)
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
 ADD_NODE_INTEGER(INPUT_08:GAIN, 1, TreeUSAGE_NUMERIC)
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
 ADD_NODE_INTEGER(INPUT_09:GAIN, 1, TreeUSAGE_NUMERIC)
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
 ADD_NODE_INTEGER(INPUT_10:GAIN, 1, TreeUSAGE_NUMERIC)
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
 ADD_NODE_INTEGER(INPUT_11:GAIN, 1, TreeUSAGE_NUMERIC)
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
 ADD_NODE_INTEGER(INPUT_12:GAIN, 1, TreeUSAGE_NUMERIC)
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
 ADD_NODE_INTEGER(INPUT_13:GAIN, 1, TreeUSAGE_NUMERIC)
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
 ADD_NODE_INTEGER(INPUT_14:GAIN, 1, TreeUSAGE_NUMERIC)
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
 ADD_NODE_INTEGER(INPUT_15:GAIN, 1, TreeUSAGE_NUMERIC)
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
 ADD_NODE_INTEGER(INPUT_16:GAIN, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_ACTION(:INIT_ACTION, INIT, INIT, 50, 0, 0, CAMAC_SERVER, 0)
 ADD_NODE_ACTION(:STORE_ACTION, STORE, STORE, 50, 0, 0, CAMAC_SERVER, 0)
      status = TreeEndConglomerate();
  if (!(status & 1))
    return status;
  return (TreeSetDefaultNid(old_nid));
}

EXPORT int joerger_tr16__part_name(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)),
			    struct descriptor_d *out_d)
{
  int element = 0, status;
  NCI_ITM nci_list[] = { {4, NciCONGLOMERATE_ELT, 0, 0}, {0, 0, 0, 0} };
  nci_list[0].pointer = (unsigned char *)&element;
  status = TreeGetNci(*(int *)nid_d_ptr->pointer, nci_list);
  if (!(status & 1))
    return status;
  switch (element) {
  case (JOERGER_TR16_N_HEAD + 1):
    StrFree1Dx(out_d);
    break;
  case (JOERGER_TR16_N_NAME + 1):
 COPY_PART_NAME(:NAME) break;
  case (JOERGER_TR16_N_COMMENT + 1):
 COPY_PART_NAME(:COMMENT) break;
  case (JOERGER_TR16_N_FREQ + 1):
 COPY_PART_NAME(:FREQ) break;
  case (JOERGER_TR16_N_EXT_CLOCK_IN + 1):
 COPY_PART_NAME(:EXT_CLOCK_IN) break;
  case (JOERGER_TR16_N_STOP_TRIG + 1):
 COPY_PART_NAME(:STOP_TRIG) break;
  case (JOERGER_TR16_N_MEM_SIZE + 1):
 COPY_PART_NAME(:MEM_SIZE) break;
  case (JOERGER_TR16_N_ACTIVE_MEM + 1):
 COPY_PART_NAME(:ACTIVE_MEM) break;
  case (JOERGER_TR16_N_ACTIVE_CHANS + 1):
 COPY_PART_NAME(:ACTIVE_CHANS) break;
  case (JOERGER_TR16_N_POST_TRIG + 1):
 COPY_PART_NAME(:POST_TRIG) break;
  case (JOERGER_TR16_N_MASTER + 1):
 COPY_PART_NAME(:MASTER) break;
  case (JOERGER_TR16_N_INPUT_01 + 1):
 COPY_PART_NAME(:INPUT_01) break;
  case (JOERGER_TR16_N_INPUT_01_STARTIDX + 1):
 COPY_PART_NAME(INPUT_01:STARTIDX) break;
  case (JOERGER_TR16_N_INPUT_01_ENDIDX + 1):
 COPY_PART_NAME(INPUT_01:ENDIDX) break;
  case (JOERGER_TR16_N_INPUT_01_GAIN + 1):
 COPY_PART_NAME(INPUT_01:GAIN) break;
  case (JOERGER_TR16_N_INPUT_02 + 1):
 COPY_PART_NAME(:INPUT_02) break;
  case (JOERGER_TR16_N_INPUT_02_STARTIDX + 1):
 COPY_PART_NAME(INPUT_02:STARTIDX) break;
  case (JOERGER_TR16_N_INPUT_02_ENDIDX + 1):
 COPY_PART_NAME(INPUT_02:ENDIDX) break;
  case (JOERGER_TR16_N_INPUT_02_GAIN + 1):
 COPY_PART_NAME(INPUT_02:GAIN) break;
  case (JOERGER_TR16_N_INPUT_03 + 1):
 COPY_PART_NAME(:INPUT_03) break;
  case (JOERGER_TR16_N_INPUT_03_STARTIDX + 1):
 COPY_PART_NAME(INPUT_03:STARTIDX) break;
  case (JOERGER_TR16_N_INPUT_03_ENDIDX + 1):
 COPY_PART_NAME(INPUT_03:ENDIDX) break;
  case (JOERGER_TR16_N_INPUT_03_GAIN + 1):
 COPY_PART_NAME(INPUT_03:GAIN) break;
  case (JOERGER_TR16_N_INPUT_04 + 1):
 COPY_PART_NAME(:INPUT_04) break;
  case (JOERGER_TR16_N_INPUT_04_STARTIDX + 1):
 COPY_PART_NAME(INPUT_04:STARTIDX) break;
  case (JOERGER_TR16_N_INPUT_04_ENDIDX + 1):
 COPY_PART_NAME(INPUT_04:ENDIDX) break;
  case (JOERGER_TR16_N_INPUT_04_GAIN + 1):
 COPY_PART_NAME(INPUT_04:GAIN) break;
  case (JOERGER_TR16_N_INPUT_05 + 1):
 COPY_PART_NAME(:INPUT_05) break;
  case (JOERGER_TR16_N_INPUT_05_STARTIDX + 1):
 COPY_PART_NAME(INPUT_05:STARTIDX) break;
  case (JOERGER_TR16_N_INPUT_05_ENDIDX + 1):
 COPY_PART_NAME(INPUT_05:ENDIDX) break;
  case (JOERGER_TR16_N_INPUT_05_GAIN + 1):
 COPY_PART_NAME(INPUT_05:GAIN) break;
  case (JOERGER_TR16_N_INPUT_06 + 1):
 COPY_PART_NAME(:INPUT_06) break;
  case (JOERGER_TR16_N_INPUT_06_STARTIDX + 1):
 COPY_PART_NAME(INPUT_06:STARTIDX) break;
  case (JOERGER_TR16_N_INPUT_06_ENDIDX + 1):
 COPY_PART_NAME(INPUT_06:ENDIDX) break;
  case (JOERGER_TR16_N_INPUT_06_GAIN + 1):
 COPY_PART_NAME(INPUT_06:GAIN) break;
  case (JOERGER_TR16_N_INPUT_07 + 1):
 COPY_PART_NAME(:INPUT_07) break;
  case (JOERGER_TR16_N_INPUT_07_STARTIDX + 1):
 COPY_PART_NAME(INPUT_07:STARTIDX) break;
  case (JOERGER_TR16_N_INPUT_07_ENDIDX + 1):
 COPY_PART_NAME(INPUT_07:ENDIDX) break;
  case (JOERGER_TR16_N_INPUT_07_GAIN + 1):
 COPY_PART_NAME(INPUT_07:GAIN) break;
  case (JOERGER_TR16_N_INPUT_08 + 1):
 COPY_PART_NAME(:INPUT_08) break;
  case (JOERGER_TR16_N_INPUT_08_STARTIDX + 1):
 COPY_PART_NAME(INPUT_08:STARTIDX) break;
  case (JOERGER_TR16_N_INPUT_08_ENDIDX + 1):
 COPY_PART_NAME(INPUT_08:ENDIDX) break;
  case (JOERGER_TR16_N_INPUT_08_GAIN + 1):
 COPY_PART_NAME(INPUT_08:GAIN) break;
  case (JOERGER_TR16_N_INPUT_09 + 1):
 COPY_PART_NAME(:INPUT_09) break;
  case (JOERGER_TR16_N_INPUT_09_STARTIDX + 1):
 COPY_PART_NAME(INPUT_09:STARTIDX) break;
  case (JOERGER_TR16_N_INPUT_09_ENDIDX + 1):
 COPY_PART_NAME(INPUT_09:ENDIDX) break;
  case (JOERGER_TR16_N_INPUT_09_GAIN + 1):
 COPY_PART_NAME(INPUT_09:GAIN) break;
  case (JOERGER_TR16_N_INPUT_10 + 1):
 COPY_PART_NAME(:INPUT_10) break;
  case (JOERGER_TR16_N_INPUT_10_STARTIDX + 1):
 COPY_PART_NAME(INPUT_10:STARTIDX) break;
  case (JOERGER_TR16_N_INPUT_10_ENDIDX + 1):
 COPY_PART_NAME(INPUT_10:ENDIDX) break;
  case (JOERGER_TR16_N_INPUT_10_GAIN + 1):
 COPY_PART_NAME(INPUT_10:GAIN) break;
  case (JOERGER_TR16_N_INPUT_11 + 1):
 COPY_PART_NAME(:INPUT_11) break;
  case (JOERGER_TR16_N_INPUT_11_STARTIDX + 1):
 COPY_PART_NAME(INPUT_11:STARTIDX) break;
  case (JOERGER_TR16_N_INPUT_11_ENDIDX + 1):
 COPY_PART_NAME(INPUT_11:ENDIDX) break;
  case (JOERGER_TR16_N_INPUT_11_GAIN + 1):
 COPY_PART_NAME(INPUT_11:GAIN) break;
  case (JOERGER_TR16_N_INPUT_12 + 1):
 COPY_PART_NAME(:INPUT_12) break;
  case (JOERGER_TR16_N_INPUT_12_STARTIDX + 1):
 COPY_PART_NAME(INPUT_12:STARTIDX) break;
  case (JOERGER_TR16_N_INPUT_12_ENDIDX + 1):
 COPY_PART_NAME(INPUT_12:ENDIDX) break;
  case (JOERGER_TR16_N_INPUT_12_GAIN + 1):
 COPY_PART_NAME(INPUT_12:GAIN) break;
  case (JOERGER_TR16_N_INPUT_13 + 1):
 COPY_PART_NAME(:INPUT_13) break;
  case (JOERGER_TR16_N_INPUT_13_STARTIDX + 1):
 COPY_PART_NAME(INPUT_13:STARTIDX) break;
  case (JOERGER_TR16_N_INPUT_13_ENDIDX + 1):
 COPY_PART_NAME(INPUT_13:ENDIDX) break;
  case (JOERGER_TR16_N_INPUT_13_GAIN + 1):
 COPY_PART_NAME(INPUT_13:GAIN) break;
  case (JOERGER_TR16_N_INPUT_14 + 1):
 COPY_PART_NAME(:INPUT_14) break;
  case (JOERGER_TR16_N_INPUT_14_STARTIDX + 1):
 COPY_PART_NAME(INPUT_14:STARTIDX) break;
  case (JOERGER_TR16_N_INPUT_14_ENDIDX + 1):
 COPY_PART_NAME(INPUT_14:ENDIDX) break;
  case (JOERGER_TR16_N_INPUT_14_GAIN + 1):
 COPY_PART_NAME(INPUT_14:GAIN) break;
  case (JOERGER_TR16_N_INPUT_15 + 1):
 COPY_PART_NAME(:INPUT_15) break;
  case (JOERGER_TR16_N_INPUT_15_STARTIDX + 1):
 COPY_PART_NAME(INPUT_15:STARTIDX) break;
  case (JOERGER_TR16_N_INPUT_15_ENDIDX + 1):
 COPY_PART_NAME(INPUT_15:ENDIDX) break;
  case (JOERGER_TR16_N_INPUT_15_GAIN + 1):
 COPY_PART_NAME(INPUT_15:GAIN) break;
  case (JOERGER_TR16_N_INPUT_16 + 1):
 COPY_PART_NAME(:INPUT_16) break;
  case (JOERGER_TR16_N_INPUT_16_STARTIDX + 1):
 COPY_PART_NAME(INPUT_16:STARTIDX) break;
  case (JOERGER_TR16_N_INPUT_16_ENDIDX + 1):
 COPY_PART_NAME(INPUT_16:ENDIDX) break;
  case (JOERGER_TR16_N_INPUT_16_GAIN + 1):
 COPY_PART_NAME(INPUT_16:GAIN) break;
  case (JOERGER_TR16_N_INIT_ACTION + 1):
 COPY_PART_NAME(:INIT_ACTION) break;
  case (JOERGER_TR16_N_STORE_ACTION + 1):
 COPY_PART_NAME(:STORE_ACTION) break;
  default:
    status = TreeILLEGAL_ITEM;
  }
  return status;
}

extern int joerger_tr16___init();
#define free_xd_array { int i; for(i=0; i<2;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}
#define error(nid,code,code1) {free_xd_array return GenDeviceSignal(nid,code,code1);}

EXPORT int joerger_tr16__init(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InInitStruct)
      static struct {
    short code;
    int value;
  } mem_size_t[] = {
    {0, 128}, {1, 256}, {2, 512}, {3, 1024}, {0, 0}};
  static struct {
    short code;
    int value;
  } active_mem_t[] = {
    {0, 8}, {1, 16}, {2, 32}, {3, 64}, {4, 128}, {5, 256}, {6, 512}, {7, 1024}, {0, 0}};
  static struct {
    short code;
    int value;
  } active_chans_t[] = {
    {0, 1}, {1, 2}, {2, 4}, {3, 8}, {4, 16}, {0, 0}};
  static struct {
    short code;
    int value;
  } post_trig_t[] = {
    {0, 32},
    {1, 64},
    {2, 128},
    {3, 256},
    {4, 512},
    {5, 1024},
    {6, 2048},
    {7, 4096},
    {8, 8192},
    {9, 16384}, {10, 32768}, {11, 65536}, {12, 131072}, {13, 262144}, {14, 524288}, {15, 1048576}, {0, 0}};
  static struct {
    short code;
    float value;
  } freq_t[] = {
    {7, 0.0}, {6, 2.5}, {5, 5.}, {4, 10.}, {3, 20.}, {2, 40.}, {1, 80.}, {0, 160.}, {0, 0}};
  static struct {
    short code;
    int value;
  } master_t[] = {
    {0, 0}, {1, 1}, {0, 0}};
  static struct {
    short code;
    int value;
  } input_01_gain_t[] = {
    {0, 1}, {1, 2}, {2, 4}, {3, 8}, {0, 0}};
  static struct {
    short code;
    int value;
  } input_02_gain_t[] = {
    {0, 1}, {1, 2}, {2, 4}, {3, 8}, {0, 0}};
  static struct {
    short code;
    int value;
  } input_03_gain_t[] = {
    {0, 1}, {1, 2}, {2, 4}, {3, 8}, {0, 0}};
  static struct {
    short code;
    int value;
  } input_04_gain_t[] = {
    {0, 1}, {1, 2}, {2, 4}, {3, 8}, {0, 0}};
  static struct {
    short code;
    int value;
  } input_05_gain_t[] = {
    {0, 1}, {1, 2}, {2, 4}, {3, 8}, {0, 0}};
  static struct {
    short code;
    int value;
  } input_06_gain_t[] = {
    {0, 1}, {1, 2}, {2, 4}, {3, 8}, {0, 0}};
  static struct {
    short code;
    int value;
  } input_07_gain_t[] = {
    {0, 1}, {1, 2}, {2, 4}, {3, 8}, {0, 0}};
  static struct {
    short code;
    int value;
  } input_08_gain_t[] = {
    {0, 1}, {1, 2}, {2, 4}, {3, 8}, {0, 0}};
  static struct {
    short code;
    int value;
  } input_09_gain_t[] = {
    {0, 1}, {1, 2}, {2, 4}, {3, 8}, {0, 0}};
  static struct {
    short code;
    int value;
  } input_10_gain_t[] = {
    {0, 1}, {1, 2}, {2, 4}, {3, 8}, {0, 0}};
  static struct {
    short code;
    int value;
  } input_11_gain_t[] = {
    {0, 1}, {1, 2}, {2, 4}, {3, 8}, {0, 0}};
  static struct {
    short code;
    int value;
  } input_12_gain_t[] = {
    {0, 1}, {1, 2}, {2, 4}, {3, 8}, {0, 0}};
  static struct {
    short code;
    int value;
  } input_13_gain_t[] = {
  {0, 1}, {1, 2}, {2, 4}, {3, 8}, {0, 0}};
  static struct {
    short code;
    int value;
  } input_14_gain_t[] = {
  {0, 1}, {1, 2}, {2, 4}, {3, 8}, {0, 0}};
  static struct {
    short code;
    int value;
  } input_15_gain_t[] = {
  {0, 1}, {1, 2}, {2, 4}, {3, 8}, {0, 0}};
  static struct {
    short code;
    int value;
  } input_16_gain_t[] = {
  {0, 1}, {1, 2}, {2, 4}, {3, 8}, {0, 0}};
  struct descriptor_xd work_xd[2];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 2);
  initialize_variables(InInitStruct)

      read_string_error(JOERGER_TR16_N_NAME, name, DEV$_BAD_NAME);
  read_integer_error(JOERGER_TR16_N_MEM_SIZE, mem_size, TR16$_BAD_MEMSIZE);
  check_integer_set(mem_size, mem_size_t, 4, TR16$_BAD_MEMSIZE);
  read_integer_error(JOERGER_TR16_N_ACTIVE_MEM, active_mem, TR16$_BAD_ACTIVEMEM);
  check_integer_conv_set(active_mem, active_mem_convert, active_mem_t, 8, TR16$_BAD_ACTIVEMEM);
  read_integer_error(JOERGER_TR16_N_ACTIVE_CHANS, active_chans, TR16$_BAD_ACTIVECHAN);
  check_integer_conv_set(active_chans, active_chans_convert, active_chans_t, 5,
			 TR16$_BAD_ACTIVECHAN);
  read_integer_error(JOERGER_TR16_N_POST_TRIG, post_trig, TR16$_BAD_PTS);
  check_integer_conv_set(post_trig, post_trig_convert, post_trig_t, 16, TR16$_BAD_PTS);
  read_float_error(JOERGER_TR16_N_FREQ, freq, TR16$_BAD_FREQUENCY);
  check_float_conv_set(freq, freq_convert, freq_t, 8, TR16$_BAD_FREQUENCY);
  read_integer_error(JOERGER_TR16_N_MASTER, master, TR16$_BAD_MASTER);
  check_integer_set(master, master_t, 2, TR16$_BAD_MASTER);
  read_integer_error(JOERGER_TR16_N_INPUT_01_GAIN, input_01_gain, TR16$_BAD_GAIN);
  check_integer_conv_set(input_01_gain, input_01_gain_convert, input_01_gain_t, 4, TR16$_BAD_GAIN);
  read_integer_error(JOERGER_TR16_N_INPUT_02_GAIN, input_02_gain, TR16$_BAD_GAIN);
  check_integer_conv_set(input_02_gain, input_02_gain_convert, input_02_gain_t, 4, TR16$_BAD_GAIN);
  read_integer_error(JOERGER_TR16_N_INPUT_03_GAIN, input_03_gain, TR16$_BAD_GAIN);
  check_integer_conv_set(input_03_gain, input_03_gain_convert, input_03_gain_t, 4, TR16$_BAD_GAIN);
  read_integer_error(JOERGER_TR16_N_INPUT_04_GAIN, input_04_gain, TR16$_BAD_GAIN);
  check_integer_conv_set(input_04_gain, input_04_gain_convert, input_04_gain_t, 4, TR16$_BAD_GAIN);
  read_integer_error(JOERGER_TR16_N_INPUT_05_GAIN, input_05_gain, TR16$_BAD_GAIN);
  check_integer_conv_set(input_05_gain, input_05_gain_convert, input_05_gain_t, 4, TR16$_BAD_GAIN);
  read_integer_error(JOERGER_TR16_N_INPUT_06_GAIN, input_06_gain, TR16$_BAD_GAIN);
  check_integer_conv_set(input_06_gain, input_06_gain_convert, input_06_gain_t, 4, TR16$_BAD_GAIN);
  read_integer_error(JOERGER_TR16_N_INPUT_07_GAIN, input_07_gain, TR16$_BAD_GAIN);
  check_integer_conv_set(input_07_gain, input_07_gain_convert, input_07_gain_t, 4, TR16$_BAD_GAIN);
  read_integer_error(JOERGER_TR16_N_INPUT_08_GAIN, input_08_gain, TR16$_BAD_GAIN);
  check_integer_conv_set(input_08_gain, input_08_gain_convert, input_08_gain_t, 4, TR16$_BAD_GAIN);
  read_integer_error(JOERGER_TR16_N_INPUT_09_GAIN, input_09_gain, TR16$_BAD_GAIN);
  check_integer_conv_set(input_09_gain, input_09_gain_convert, input_09_gain_t, 4, TR16$_BAD_GAIN);
  read_integer_error(JOERGER_TR16_N_INPUT_10_GAIN, input_10_gain, TR16$_BAD_GAIN);
  check_integer_conv_set(input_10_gain, input_10_gain_convert, input_10_gain_t, 4, TR16$_BAD_GAIN);
  read_integer_error(JOERGER_TR16_N_INPUT_11_GAIN, input_11_gain, TR16$_BAD_GAIN);
  check_integer_conv_set(input_11_gain, input_11_gain_convert, input_11_gain_t, 4, TR16$_BAD_GAIN);
  read_integer_error(JOERGER_TR16_N_INPUT_12_GAIN, input_12_gain, TR16$_BAD_GAIN);
  check_integer_conv_set(input_12_gain, input_12_gain_convert, input_12_gain_t, 4, TR16$_BAD_GAIN);
  read_integer_error(JOERGER_TR16_N_INPUT_13_GAIN, input_13_gain, TR16$_BAD_GAIN);
  check_integer_conv_set(input_13_gain, input_13_gain_convert, input_13_gain_t, 4, TR16$_BAD_GAIN);
  read_integer_error(JOERGER_TR16_N_INPUT_14_GAIN, input_14_gain, TR16$_BAD_GAIN);
  check_integer_conv_set(input_14_gain, input_14_gain_convert, input_14_gain_t, 4, TR16$_BAD_GAIN);
  read_integer_error(JOERGER_TR16_N_INPUT_15_GAIN, input_15_gain, TR16$_BAD_GAIN);
  check_integer_conv_set(input_15_gain, input_15_gain_convert, input_15_gain_t, 4, TR16$_BAD_GAIN);
  read_integer_error(JOERGER_TR16_N_INPUT_16_GAIN, input_16_gain, TR16$_BAD_GAIN);
  check_integer_conv_set(input_16_gain, input_16_gain_convert, input_16_gain_t, 4, TR16$_BAD_GAIN);
  read_descriptor(JOERGER_TR16_N_EXT_CLOCK_IN, ext_clock_in);
  status = joerger_tr16___init(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array

extern int joerger_tr16___trigger();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}

EXPORT int joerger_tr16__trigger(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InTriggerStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InTriggerStruct)

      read_string_error(JOERGER_TR16_N_NAME, name, DEV$_BAD_NAME);
  status = joerger_tr16___trigger(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array

extern int joerger_tr16___store();
#define free_xd_array { int i; for(i=0; i<2;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}

EXPORT int joerger_tr16__store(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InStoreStruct)
      static struct {
    short code;
    int value;
  } mem_size_t[] = {
    {0, 128}, {1, 256}, {2, 512}, {3, 1024}, {0, 0}};
  static struct {
    short code;
    int value;
  } active_mem_t[] = {
    {0, 8}, {1, 16}, {2, 32}, {3, 64}, {4, 128}, {5, 256}, {6, 512}, {7, 1024}, {0, 0}};
  static struct {
    short code;
    int value;
  } active_chans_t[] = {
    {0, 1}, {1, 2}, {2, 4}, {3, 8}, {4, 16}, {0, 0}};
  static struct {
    short code;
    float value;
  } freq_t[] = {
    {0, 0.0}, {1, 2.5}, {2, 5.}, {3, 10.}, {4, 20.}, {5, 40.}, {6, 80.}, {7, 160.}, {0, 0}};
  struct descriptor_xd work_xd[2];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 2);
  initialize_variables(InStoreStruct)

      read_string_error(JOERGER_TR16_N_NAME, name, DEV$_BAD_NAME);
  read_integer_error(JOERGER_TR16_N_MEM_SIZE, mem_size, TR16$_BAD_MEMSIZE);
  check_integer_set(mem_size, mem_size_t, 4, TR16$_BAD_MEMSIZE);
  read_integer_error(JOERGER_TR16_N_ACTIVE_MEM, active_mem, TR16$_BAD_ACTIVEMEM);
  check_integer_set(active_mem, active_mem_t, 8, TR16$_BAD_ACTIVEMEM);
  read_integer_error(JOERGER_TR16_N_ACTIVE_CHANS, active_chans, TR16$_BAD_ACTIVECHAN);
  check_integer_set(active_chans, active_chans_t, 5, TR16$_BAD_ACTIVECHAN);
  read_float_error(JOERGER_TR16_N_FREQ, freq, TR16$_BAD_FREQUENCY);
  check_float_set(freq, freq_t, 8, TR16$_BAD_FREQUENCY);
  read_descriptor(JOERGER_TR16_N_EXT_CLOCK_IN, ext_clock_in);
  status = joerger_tr16___store(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array
