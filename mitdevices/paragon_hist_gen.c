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
#include "paragon_hist_gen.h"
EXPORT int paragon_hist__add(struct descriptor *name_d_ptr, struct descriptor *dummy_d_ptr __attribute__ ((unused)), int *nid_ptr)
{
  static DESCRIPTOR(library_d, "MIT$DEVICES");
  static DESCRIPTOR(model_d, "PARAGON_HIST");
  static DESCRIPTOR_CONGLOM(conglom_d, &library_d, &model_d, 0, 0);
  int usage = TreeUSAGE_DEVICE;
  int curr_nid, old_nid, head_nid, status;
  long int flags = NciM_WRITE_ONCE;
  NCI_ITM flag_itm[] = { {2, NciSET_FLAGS, 0, 0}, {0, 0, 0, 0} };
  char *name_ptr = strncpy(malloc(name_d_ptr->length + 1), name_d_ptr->pointer, name_d_ptr->length);
  flag_itm[0].pointer = (unsigned char *)&flags;
  name_ptr[name_d_ptr->length] = 0;
  status = TreeStartConglomerate(PARAGON_HIST_K_CONG_NODES);
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
 ADD_NODE(:COMMENT, TreeUSAGE_TEXT)
 ADD_NODE(:REPORT_NAME, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:NODE, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:NAME_0, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:NAME_1, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:NAME_2, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:NAME_3, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:NAME_4, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:NAME_5, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:NAME_6, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:NAME_7, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:NAME_8, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:NAME_9, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:VALUE_0, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:VALUE_1, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:VALUE_2, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:VALUE_3, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:VALUE_4, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:VALUE_5, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:VALUE_6, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:VALUE_7, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:VALUE_8, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:VALUE_9, TreeUSAGE_SIGNAL)
      flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:LIMIT_0, TreeUSAGE_NUMERIC)
 ADD_NODE(:LIMIT_1, TreeUSAGE_NUMERIC)
 ADD_NODE(:LIMIT_2, TreeUSAGE_NUMERIC)
 ADD_NODE(:LIMIT_3, TreeUSAGE_NUMERIC)
 ADD_NODE(:LIMIT_4, TreeUSAGE_NUMERIC)
 ADD_NODE(:LIMIT_5, TreeUSAGE_NUMERIC)
 ADD_NODE(:LIMIT_6, TreeUSAGE_NUMERIC)
 ADD_NODE(:LIMIT_7, TreeUSAGE_NUMERIC)
 ADD_NODE(:LIMIT_8, TreeUSAGE_NUMERIC)
 ADD_NODE(:LIMIT_9, TreeUSAGE_NUMERIC)
 ADD_NODE_INTEGER(:DELETE_FILE, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_ACTION(:STORE_ACTION, STORE, STORE, 50, 0, 0, PARAGON_SERVER, 0)
      status = TreeEndConglomerate();
  if (!(status & 1))
    return status;
  return (TreeSetDefaultNid(old_nid));
}

EXPORT int paragon_hist__part_name(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)),
			    struct descriptor_d *out_d)
{
  int element = 0, status;
  NCI_ITM nci_list[] = { {4, NciCONGLOMERATE_ELT, 0, 0}, {0, 0, 0, 0} };
  nci_list[0].pointer = (unsigned char *)&element;
  status = TreeGetNci(*(int *)nid_d_ptr->pointer, nci_list);
  if (!(status & 1))
    return status;
  switch (element) {
  case (PARAGON_HIST_N_HEAD + 1):
    StrFree1Dx(out_d);
    break;
  case (PARAGON_HIST_N_COMMENT + 1):
 COPY_PART_NAME(:COMMENT) break;
  case (PARAGON_HIST_N_REPORT_NAME + 1):
 COPY_PART_NAME(:REPORT_NAME) break;
  case (PARAGON_HIST_N_NODE + 1):
 COPY_PART_NAME(:NODE) break;
  case (PARAGON_HIST_N_NAME_0 + 1):
 COPY_PART_NAME(:NAME_0) break;
  case (PARAGON_HIST_N_NAME_1 + 1):
 COPY_PART_NAME(:NAME_1) break;
  case (PARAGON_HIST_N_NAME_2 + 1):
 COPY_PART_NAME(:NAME_2) break;
  case (PARAGON_HIST_N_NAME_3 + 1):
 COPY_PART_NAME(:NAME_3) break;
  case (PARAGON_HIST_N_NAME_4 + 1):
 COPY_PART_NAME(:NAME_4) break;
  case (PARAGON_HIST_N_NAME_5 + 1):
 COPY_PART_NAME(:NAME_5) break;
  case (PARAGON_HIST_N_NAME_6 + 1):
 COPY_PART_NAME(:NAME_6) break;
  case (PARAGON_HIST_N_NAME_7 + 1):
 COPY_PART_NAME(:NAME_7) break;
  case (PARAGON_HIST_N_NAME_8 + 1):
 COPY_PART_NAME(:NAME_8) break;
  case (PARAGON_HIST_N_NAME_9 + 1):
 COPY_PART_NAME(:NAME_9) break;
  case (PARAGON_HIST_N_VALUE_0 + 1):
 COPY_PART_NAME(:VALUE_0) break;
  case (PARAGON_HIST_N_VALUE_1 + 1):
 COPY_PART_NAME(:VALUE_1) break;
  case (PARAGON_HIST_N_VALUE_2 + 1):
 COPY_PART_NAME(:VALUE_2) break;
  case (PARAGON_HIST_N_VALUE_3 + 1):
 COPY_PART_NAME(:VALUE_3) break;
  case (PARAGON_HIST_N_VALUE_4 + 1):
 COPY_PART_NAME(:VALUE_4) break;
  case (PARAGON_HIST_N_VALUE_5 + 1):
 COPY_PART_NAME(:VALUE_5) break;
  case (PARAGON_HIST_N_VALUE_6 + 1):
 COPY_PART_NAME(:VALUE_6) break;
  case (PARAGON_HIST_N_VALUE_7 + 1):
 COPY_PART_NAME(:VALUE_7) break;
  case (PARAGON_HIST_N_VALUE_8 + 1):
 COPY_PART_NAME(:VALUE_8) break;
  case (PARAGON_HIST_N_VALUE_9 + 1):
 COPY_PART_NAME(:VALUE_9) break;
  case (PARAGON_HIST_N_LIMIT_0 + 1):
 COPY_PART_NAME(:LIMIT_0) break;
  case (PARAGON_HIST_N_LIMIT_1 + 1):
 COPY_PART_NAME(:LIMIT_1) break;
  case (PARAGON_HIST_N_LIMIT_2 + 1):
 COPY_PART_NAME(:LIMIT_2) break;
  case (PARAGON_HIST_N_LIMIT_3 + 1):
 COPY_PART_NAME(:LIMIT_3) break;
  case (PARAGON_HIST_N_LIMIT_4 + 1):
 COPY_PART_NAME(:LIMIT_4) break;
  case (PARAGON_HIST_N_LIMIT_5 + 1):
 COPY_PART_NAME(:LIMIT_5) break;
  case (PARAGON_HIST_N_LIMIT_6 + 1):
 COPY_PART_NAME(:LIMIT_6) break;
  case (PARAGON_HIST_N_LIMIT_7 + 1):
 COPY_PART_NAME(:LIMIT_7) break;
  case (PARAGON_HIST_N_LIMIT_8 + 1):
 COPY_PART_NAME(:LIMIT_8) break;
  case (PARAGON_HIST_N_LIMIT_9 + 1):
 COPY_PART_NAME(:LIMIT_9) break;
  case (PARAGON_HIST_N_DELETE_FILE + 1):
 COPY_PART_NAME(:DELETE_FILE) break;
  case (PARAGON_HIST_N_STORE_ACTION + 1):
 COPY_PART_NAME(:STORE_ACTION) break;
  default:
    status = TreeILLEGAL_ITEM;
  }
  return status;
}

extern int paragon_hist___store();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}
#define error(nid,code,code1) {free_xd_array return GenDeviceSignal(nid,code,code1);}

EXPORT int paragon_hist__store(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InStoreStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InStoreStruct)

      read_string_error(PARAGON_HIST_N_REPORT_NAME, report_name, DEV$_BAD_NAME);
  status = paragon_hist___store(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array

extern int paragon_hist___insert();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}

EXPORT int paragon_hist__insert(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InInsertStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InInsertStruct)

      read_string_error(PARAGON_HIST_N_REPORT_NAME, report_name, DEV$_BAD_NAME);
  status = paragon_hist___insert(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array
