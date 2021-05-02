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
#include "mitdevices_msg.h"
#include "mds_gendevice.h"
#include "l3512_gen.h"
EXPORT int l3512__add(struct descriptor *name_d_ptr, struct descriptor *dummy_d_ptr __attribute__ ((unused)), int *nid_ptr)
{
  static DESCRIPTOR(library_d, "MIT$DEVICES");
  static DESCRIPTOR(model_d, "L3512");
  static DESCRIPTOR_CONGLOM(conglom_d, &library_d, &model_d, 0, 0);
  int usage = TreeUSAGE_DEVICE;
  int curr_nid, old_nid, head_nid, status;
  long int flags = NciM_WRITE_ONCE;
  NCI_ITM flag_itm[] = { {2, NciSET_FLAGS, 0, 0}, {0, 0, 0, 0} };
  char *name_ptr = strncpy(malloc(name_d_ptr->length + 1), name_d_ptr->pointer, name_d_ptr->length);
  flag_itm[0].pointer = (unsigned char *)&flags;
  name_ptr[name_d_ptr->length] = 0;
  status = TreeStartConglomerate(L3512_K_CONG_NODES);
  if (STATUS_NOT_OK)
    return status;
  status = TreeAddNode(name_ptr, &head_nid, usage);
  if (STATUS_NOT_OK)
    return status;
  *nid_ptr = head_nid;
  status = TreeSetNci(head_nid, flag_itm);
  status = TreePutRecord(head_nid, (struct descriptor *)&conglom_d, 0);
  if (STATUS_NOT_OK)
    return status;
  status = TreeGetDefaultNid(&old_nid);
  if (STATUS_NOT_OK)
    return status;
  status = TreeSetDefaultNid(head_nid);
  if (STATUS_NOT_OK)
    return status;
 ADD_NODE(:NAME, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:COMMENT, TreeUSAGE_TEXT)
 ADD_NODE(:MEMORY_NAME, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:ROUTER_NAME, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:NUM_SPECTRA, 10, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:NUM_CHANNELS, 256, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:OFFSET, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1.0	"
 ADD_NODE_EXPR(:DURATION, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:SPECTRA, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_ACTION(:INIT_ACTION, INIT, INIT, 50, 0, 0, CAMAC_SERVER, 0)
 ADD_NODE_ACTION(:STORE_ACTION, STORE, STORE, 50, 0, 0, CAMAC_SERVER, 0)
      status = TreeEndConglomerate();
  if (STATUS_NOT_OK)
    return status;
  return (TreeSetDefaultNid(old_nid));
}

EXPORT int l3512__part_name(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)),
		     struct descriptor_d *out_d)
{
  int element = 0, status;
  NCI_ITM nci_list[] = { {4, NciCONGLOMERATE_ELT, 0, 0}, {0, 0, 0, 0} };
  nci_list[0].pointer = (unsigned char *)&element;
  status = TreeGetNci(*(int *)nid_d_ptr->pointer, nci_list);
  if (STATUS_NOT_OK)
    return status;
  switch (element) {
  case (L3512_N_HEAD + 1):
    StrFree1Dx(out_d);
    break;
  case (L3512_N_NAME + 1):
 COPY_PART_NAME(:NAME) break;
  case (L3512_N_COMMENT + 1):
 COPY_PART_NAME(:COMMENT) break;
  case (L3512_N_MEMORY_NAME + 1):
 COPY_PART_NAME(:MEMORY_NAME) break;
  case (L3512_N_ROUTER_NAME + 1):
 COPY_PART_NAME(:ROUTER_NAME) break;
  case (L3512_N_NUM_SPECTRA + 1):
 COPY_PART_NAME(:NUM_SPECTRA) break;
  case (L3512_N_NUM_CHANNELS + 1):
 COPY_PART_NAME(:NUM_CHANNELS) break;
  case (L3512_N_OFFSET + 1):
 COPY_PART_NAME(:OFFSET) break;
  case (L3512_N_DURATION + 1):
 COPY_PART_NAME(:DURATION) break;
  case (L3512_N_SPECTRA + 1):
 COPY_PART_NAME(:SPECTRA) break;
  case (L3512_N_INIT_ACTION + 1):
 COPY_PART_NAME(:INIT_ACTION) break;
  case (L3512_N_STORE_ACTION + 1):
 COPY_PART_NAME(:STORE_ACTION) break;
  default:
    status = TreeILLEGAL_ITEM;
  }
  return status;
}

extern int l3512___init();
#define free_xd_array { int i; for(i=0; i<4;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}
#define error(nid,code,code1) {free_xd_array return GenDeviceSignal(nid,code,code1);}

EXPORT int l3512__init(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InInitStruct)
  struct descriptor_xd work_xd[4];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 4);
  initialize_variables(InInitStruct)

      read_string_error(L3512_N_NAME, name, DEV$_BAD_NAME);
  read_string_error(L3512_N_MEMORY_NAME, memory_name, DEV$_BAD_NAME);
  read_string_error(L3512_N_ROUTER_NAME, router_name, DEV$_BAD_NAME);
  read_integer(L3512_N_NUM_SPECTRA, num_spectra);
  read_integer(L3512_N_NUM_CHANNELS, num_channels);
  read_integer(L3512_N_OFFSET, offset);
  read_descriptor(L3512_N_DURATION, duration);
  status = l3512___init(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array

extern int l3512___store();
#define free_xd_array { int i; for(i=0; i<3;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}

EXPORT int l3512__store(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InStoreStruct)
  struct descriptor_xd work_xd[3];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 3);
  initialize_variables(InStoreStruct)

      read_string_error(L3512_N_NAME, name, DEV$_BAD_NAME);
  read_string_error(L3512_N_MEMORY_NAME, memory_name, DEV$_BAD_NAME);
  read_integer(L3512_N_NUM_SPECTRA, num_spectra);
  read_integer(L3512_N_NUM_CHANNELS, num_channels);
  read_descriptor(L3512_N_DURATION, duration);
  status = l3512___store(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array
