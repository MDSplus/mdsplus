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
#include "preamp_gen.h"
extern int preamp___add(int *nid);
EXPORT int preamp__add(struct descriptor *name_d_ptr, struct descriptor *dummy_d_ptr __attribute__ ((unused)), int *nid_ptr)
{
  static DESCRIPTOR(library_d, "MIT$DEVICES");
  static DESCRIPTOR(model_d, "PREAMP");
  static DESCRIPTOR_CONGLOM(conglom_d, &library_d, &model_d, 0, 0);
  int usage = TreeUSAGE_DEVICE;
  int curr_nid, old_nid, head_nid, status;
  long int flags = NciM_WRITE_ONCE;
  NCI_ITM flag_itm[] = { {2, NciSET_FLAGS, 0, 0}, {0, 0, 0, 0} };
  char *name_ptr = strncpy(malloc(name_d_ptr->length + 1), name_d_ptr->pointer, name_d_ptr->length);
  flag_itm[0].pointer = (unsigned char *)&flags;
  name_ptr[name_d_ptr->length] = 0;
  status = TreeStartConglomerate(PREAMP_K_CONG_NODES);
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
 ADD_NODE(:INPUT_01, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_01:GAIN1, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_01:GAIN2, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0.	"
 ADD_NODE_EXPR(INPUT_01:OFFSET, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_01:OUTPUT, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_02, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_02:GAIN1, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_02:GAIN2, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0.	"
 ADD_NODE_EXPR(INPUT_02:OFFSET, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_02:OUTPUT, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_03, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_03:GAIN1, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_03:GAIN2, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0.	"
 ADD_NODE_EXPR(INPUT_03:OFFSET, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_03:OUTPUT, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_04, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_04:GAIN1, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_04:GAIN2, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0.	"
 ADD_NODE_EXPR(INPUT_04:OFFSET, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_04:OUTPUT, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_05, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_05:GAIN1, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_05:GAIN2, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0.	"
 ADD_NODE_EXPR(INPUT_05:OFFSET, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_05:OUTPUT, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_06, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_06:GAIN1, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_06:GAIN2, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0.	"
 ADD_NODE_EXPR(INPUT_06:OFFSET, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_06:OUTPUT, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_07, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_07:GAIN1, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_07:GAIN2, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0.	"
 ADD_NODE_EXPR(INPUT_07:OFFSET, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_07:OUTPUT, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_08, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_08:GAIN1, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_08:GAIN2, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0.	"
 ADD_NODE_EXPR(INPUT_08:OFFSET, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_08:OUTPUT, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_09, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_09:GAIN1, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_09:GAIN2, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0.	"
 ADD_NODE_EXPR(INPUT_09:OFFSET, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_09:OUTPUT, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_10, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_10:GAIN1, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_10:GAIN2, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0.	"
 ADD_NODE_EXPR(INPUT_10:OFFSET, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_10:OUTPUT, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_11, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_11:GAIN1, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_11:GAIN2, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0.	"
 ADD_NODE_EXPR(INPUT_11:OFFSET, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_11:OUTPUT, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_12, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_12:GAIN1, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_12:GAIN2, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0.	"
 ADD_NODE_EXPR(INPUT_12:OFFSET, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_12:OUTPUT, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_13, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_13:GAIN1, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_13:GAIN2, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0.	"
 ADD_NODE_EXPR(INPUT_13:OFFSET, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_13:OUTPUT, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_14, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_14:GAIN1, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_14:GAIN2, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0.	"
 ADD_NODE_EXPR(INPUT_14:OFFSET, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_14:OUTPUT, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_15, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_15:GAIN1, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_15:GAIN2, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0.	"
 ADD_NODE_EXPR(INPUT_15:OFFSET, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_15:OUTPUT, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:INPUT_16, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_16:GAIN1, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.	"
 ADD_NODE_EXPR(INPUT_16:GAIN2, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0.	"
 ADD_NODE_EXPR(INPUT_16:OFFSET, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(INPUT_16:OUTPUT, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_ACTION(:INIT_ACTION, INIT, INIT, 50, 0, 0, CAMAC_SERVER, 0)
      status = preamp___add(&head_nid);
  status = TreeEndConglomerate();
  if (!(status & 1))
    return status;
  return (TreeSetDefaultNid(old_nid));
}

EXPORT int preamp__part_name(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)),
		      struct descriptor_d *out_d)
{
  int element = 0, status;
  NCI_ITM nci_list[] = { {4, NciCONGLOMERATE_ELT, 0, 0}, {0, 0, 0, 0} };
  nci_list[0].pointer = (unsigned char *)&element;
  status = TreeGetNci(*(int *)nid_d_ptr->pointer, nci_list);
  if (!(status & 1))
    return status;
  switch (element) {
  case (PREAMP_N_HEAD + 1):
    StrFree1Dx(out_d);
    break;
  case (PREAMP_N_NAME + 1):
 COPY_PART_NAME(:NAME) break;
  case (PREAMP_N_COMMENT + 1):
 COPY_PART_NAME(:COMMENT) break;
  case (PREAMP_N_INPUT_01 + 1):
 COPY_PART_NAME(:INPUT_01) break;
  case (PREAMP_N_INPUT_01_GAIN1 + 1):
 COPY_PART_NAME(INPUT_01:GAIN1) break;
  case (PREAMP_N_INPUT_01_GAIN2 + 1):
 COPY_PART_NAME(INPUT_01:GAIN2) break;
  case (PREAMP_N_INPUT_01_OFFSET + 1):
 COPY_PART_NAME(INPUT_01:OFFSET) break;
  case (PREAMP_N_INPUT_01_OUTPUT + 1):
 COPY_PART_NAME(INPUT_01:OUTPUT) break;
  case (PREAMP_N_INPUT_02 + 1):
 COPY_PART_NAME(:INPUT_02) break;
  case (PREAMP_N_INPUT_02_GAIN1 + 1):
 COPY_PART_NAME(INPUT_02:GAIN1) break;
  case (PREAMP_N_INPUT_02_GAIN2 + 1):
 COPY_PART_NAME(INPUT_02:GAIN2) break;
  case (PREAMP_N_INPUT_02_OFFSET + 1):
 COPY_PART_NAME(INPUT_02:OFFSET) break;
  case (PREAMP_N_INPUT_02_OUTPUT + 1):
 COPY_PART_NAME(INPUT_02:OUTPUT) break;
  case (PREAMP_N_INPUT_03 + 1):
 COPY_PART_NAME(:INPUT_03) break;
  case (PREAMP_N_INPUT_03_GAIN1 + 1):
 COPY_PART_NAME(INPUT_03:GAIN1) break;
  case (PREAMP_N_INPUT_03_GAIN2 + 1):
 COPY_PART_NAME(INPUT_03:GAIN2) break;
  case (PREAMP_N_INPUT_03_OFFSET + 1):
 COPY_PART_NAME(INPUT_03:OFFSET) break;
  case (PREAMP_N_INPUT_03_OUTPUT + 1):
 COPY_PART_NAME(INPUT_03:OUTPUT) break;
  case (PREAMP_N_INPUT_04 + 1):
 COPY_PART_NAME(:INPUT_04) break;
  case (PREAMP_N_INPUT_04_GAIN1 + 1):
 COPY_PART_NAME(INPUT_04:GAIN1) break;
  case (PREAMP_N_INPUT_04_GAIN2 + 1):
 COPY_PART_NAME(INPUT_04:GAIN2) break;
  case (PREAMP_N_INPUT_04_OFFSET + 1):
 COPY_PART_NAME(INPUT_04:OFFSET) break;
  case (PREAMP_N_INPUT_04_OUTPUT + 1):
 COPY_PART_NAME(INPUT_04:OUTPUT) break;
  case (PREAMP_N_INPUT_05 + 1):
 COPY_PART_NAME(:INPUT_05) break;
  case (PREAMP_N_INPUT_05_GAIN1 + 1):
 COPY_PART_NAME(INPUT_05:GAIN1) break;
  case (PREAMP_N_INPUT_05_GAIN2 + 1):
 COPY_PART_NAME(INPUT_05:GAIN2) break;
  case (PREAMP_N_INPUT_05_OFFSET + 1):
 COPY_PART_NAME(INPUT_05:OFFSET) break;
  case (PREAMP_N_INPUT_05_OUTPUT + 1):
 COPY_PART_NAME(INPUT_05:OUTPUT) break;
  case (PREAMP_N_INPUT_06 + 1):
 COPY_PART_NAME(:INPUT_06) break;
  case (PREAMP_N_INPUT_06_GAIN1 + 1):
 COPY_PART_NAME(INPUT_06:GAIN1) break;
  case (PREAMP_N_INPUT_06_GAIN2 + 1):
 COPY_PART_NAME(INPUT_06:GAIN2) break;
  case (PREAMP_N_INPUT_06_OFFSET + 1):
 COPY_PART_NAME(INPUT_06:OFFSET) break;
  case (PREAMP_N_INPUT_06_OUTPUT + 1):
 COPY_PART_NAME(INPUT_06:OUTPUT) break;
  case (PREAMP_N_INPUT_07 + 1):
 COPY_PART_NAME(:INPUT_07) break;
  case (PREAMP_N_INPUT_07_GAIN1 + 1):
 COPY_PART_NAME(INPUT_07:GAIN1) break;
  case (PREAMP_N_INPUT_07_GAIN2 + 1):
 COPY_PART_NAME(INPUT_07:GAIN2) break;
  case (PREAMP_N_INPUT_07_OFFSET + 1):
 COPY_PART_NAME(INPUT_07:OFFSET) break;
  case (PREAMP_N_INPUT_07_OUTPUT + 1):
 COPY_PART_NAME(INPUT_07:OUTPUT) break;
  case (PREAMP_N_INPUT_08 + 1):
 COPY_PART_NAME(:INPUT_08) break;
  case (PREAMP_N_INPUT_08_GAIN1 + 1):
 COPY_PART_NAME(INPUT_08:GAIN1) break;
  case (PREAMP_N_INPUT_08_GAIN2 + 1):
 COPY_PART_NAME(INPUT_08:GAIN2) break;
  case (PREAMP_N_INPUT_08_OFFSET + 1):
 COPY_PART_NAME(INPUT_08:OFFSET) break;
  case (PREAMP_N_INPUT_08_OUTPUT + 1):
 COPY_PART_NAME(INPUT_08:OUTPUT) break;
  case (PREAMP_N_INPUT_09 + 1):
 COPY_PART_NAME(:INPUT_09) break;
  case (PREAMP_N_INPUT_09_GAIN1 + 1):
 COPY_PART_NAME(INPUT_09:GAIN1) break;
  case (PREAMP_N_INPUT_09_GAIN2 + 1):
 COPY_PART_NAME(INPUT_09:GAIN2) break;
  case (PREAMP_N_INPUT_09_OFFSET + 1):
 COPY_PART_NAME(INPUT_09:OFFSET) break;
  case (PREAMP_N_INPUT_09_OUTPUT + 1):
 COPY_PART_NAME(INPUT_09:OUTPUT) break;
  case (PREAMP_N_INPUT_10 + 1):
 COPY_PART_NAME(:INPUT_10) break;
  case (PREAMP_N_INPUT_10_GAIN1 + 1):
 COPY_PART_NAME(INPUT_10:GAIN1) break;
  case (PREAMP_N_INPUT_10_GAIN2 + 1):
 COPY_PART_NAME(INPUT_10:GAIN2) break;
  case (PREAMP_N_INPUT_10_OFFSET + 1):
 COPY_PART_NAME(INPUT_10:OFFSET) break;
  case (PREAMP_N_INPUT_10_OUTPUT + 1):
 COPY_PART_NAME(INPUT_10:OUTPUT) break;
  case (PREAMP_N_INPUT_11 + 1):
 COPY_PART_NAME(:INPUT_11) break;
  case (PREAMP_N_INPUT_11_GAIN1 + 1):
 COPY_PART_NAME(INPUT_11:GAIN1) break;
  case (PREAMP_N_INPUT_11_GAIN2 + 1):
 COPY_PART_NAME(INPUT_11:GAIN2) break;
  case (PREAMP_N_INPUT_11_OFFSET + 1):
 COPY_PART_NAME(INPUT_11:OFFSET) break;
  case (PREAMP_N_INPUT_11_OUTPUT + 1):
 COPY_PART_NAME(INPUT_11:OUTPUT) break;
  case (PREAMP_N_INPUT_12 + 1):
 COPY_PART_NAME(:INPUT_12) break;
  case (PREAMP_N_INPUT_12_GAIN1 + 1):
 COPY_PART_NAME(INPUT_12:GAIN1) break;
  case (PREAMP_N_INPUT_12_GAIN2 + 1):
 COPY_PART_NAME(INPUT_12:GAIN2) break;
  case (PREAMP_N_INPUT_12_OFFSET + 1):
 COPY_PART_NAME(INPUT_12:OFFSET) break;
  case (PREAMP_N_INPUT_12_OUTPUT + 1):
 COPY_PART_NAME(INPUT_12:OUTPUT) break;
  case (PREAMP_N_INPUT_13 + 1):
 COPY_PART_NAME(:INPUT_13) break;
  case (PREAMP_N_INPUT_13_GAIN1 + 1):
 COPY_PART_NAME(INPUT_13:GAIN1) break;
  case (PREAMP_N_INPUT_13_GAIN2 + 1):
 COPY_PART_NAME(INPUT_13:GAIN2) break;
  case (PREAMP_N_INPUT_13_OFFSET + 1):
 COPY_PART_NAME(INPUT_13:OFFSET) break;
  case (PREAMP_N_INPUT_13_OUTPUT + 1):
 COPY_PART_NAME(INPUT_13:OUTPUT) break;
  case (PREAMP_N_INPUT_14 + 1):
 COPY_PART_NAME(:INPUT_14) break;
  case (PREAMP_N_INPUT_14_GAIN1 + 1):
 COPY_PART_NAME(INPUT_14:GAIN1) break;
  case (PREAMP_N_INPUT_14_GAIN2 + 1):
 COPY_PART_NAME(INPUT_14:GAIN2) break;
  case (PREAMP_N_INPUT_14_OFFSET + 1):
 COPY_PART_NAME(INPUT_14:OFFSET) break;
  case (PREAMP_N_INPUT_14_OUTPUT + 1):
 COPY_PART_NAME(INPUT_14:OUTPUT) break;
  case (PREAMP_N_INPUT_15 + 1):
 COPY_PART_NAME(:INPUT_15) break;
  case (PREAMP_N_INPUT_15_GAIN1 + 1):
 COPY_PART_NAME(INPUT_15:GAIN1) break;
  case (PREAMP_N_INPUT_15_GAIN2 + 1):
 COPY_PART_NAME(INPUT_15:GAIN2) break;
  case (PREAMP_N_INPUT_15_OFFSET + 1):
 COPY_PART_NAME(INPUT_15:OFFSET) break;
  case (PREAMP_N_INPUT_15_OUTPUT + 1):
 COPY_PART_NAME(INPUT_15:OUTPUT) break;
  case (PREAMP_N_INPUT_16 + 1):
 COPY_PART_NAME(:INPUT_16) break;
  case (PREAMP_N_INPUT_16_GAIN1 + 1):
 COPY_PART_NAME(INPUT_16:GAIN1) break;
  case (PREAMP_N_INPUT_16_GAIN2 + 1):
 COPY_PART_NAME(INPUT_16:GAIN2) break;
  case (PREAMP_N_INPUT_16_OFFSET + 1):
 COPY_PART_NAME(INPUT_16:OFFSET) break;
  case (PREAMP_N_INPUT_16_OUTPUT + 1):
 COPY_PART_NAME(INPUT_16:OUTPUT) break;
  case (PREAMP_N_INIT_ACTION + 1):
 COPY_PART_NAME(:INIT_ACTION) break;
  default:
    status = TreeILLEGAL_ITEM;
  }
  return status;
}

extern int preamp___init();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}
#define error(nid,code,code1) {free_xd_array return GenDeviceSignal(nid,code,code1);}

EXPORT int preamp__init(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InInitStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InInitStruct)

      read_string_error(PREAMP_N_NAME, name, DEV$_BAD_NAME);
  status = preamp___init(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array
