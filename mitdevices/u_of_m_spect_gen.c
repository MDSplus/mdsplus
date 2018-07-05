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
#include "u_of_m_spect_gen.h"
EXPORT int u_of_m_spect__add(struct descriptor *name_d_ptr, struct descriptor *dummy_d_ptr __attribute__ ((unused)), int *nid_ptr)
{
  static DESCRIPTOR(library_d, "MIT$DEVICES");
  static DESCRIPTOR(model_d, "U_OF_M_SPECT");
  static DESCRIPTOR_CONGLOM(conglom_d, &library_d, &model_d, 0, 0);
  int usage = TreeUSAGE_DEVICE;
  int curr_nid, old_nid, head_nid, status;
  long int flags = NciM_WRITE_ONCE;
  NCI_ITM flag_itm[] = { {2, NciSET_FLAGS, 0, 0}, {0, 0, 0, 0} };
  char *name_ptr = strncpy(malloc(name_d_ptr->length + 1), name_d_ptr->pointer, name_d_ptr->length);
  flag_itm[0].pointer = (unsigned char *)&flags;
  name_ptr[name_d_ptr->length] = 0;
  status = TreeStartConglomerate(U_OF_M_SPECT_K_CONG_NODES);
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
 ADD_NODE(:GO_FILE, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:DATA_FILE, TreeUSAGE_TEXT)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 0.	"
 ADD_NODE_EXPR(:TRIGGER, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
  ADD_NODE(.SETUP, TreeUSAGE_STRUCTURE)
 ADD_NODE_INTEGER(.SETUP:WAVE_LENGTH, 2000, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(.SETUP:SLIT_WIDTH, 25, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(.SETUP:NUM_SPECTRA, 5, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(.SETUP:GRATING_TYPE, 1, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(.SETUP:EXPOSURE, 30, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(.SETUP:FILTER, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:WAVE_LENGTH, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:SLIT_WIDTH, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:GRATING_TYPE, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:EXPOSURE, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:FILTER, TreeUSAGE_NUMERIC)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:SPECTRA, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_COMPRESS_ON_PUT;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_INTEGER(:ORDER_SORTER, 0, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_ACTION(:INIT_ACTION, INIT, INIT, 50, 0, 0, U_OF_M_SERVER, 0)
 ADD_NODE_ACTION(:STORE_ACTION, STORE, STORE, 50, 0, 0, U_OF_M_SERVER, 0)
      status = TreeEndConglomerate();
  if (!(status & 1))
    return status;
  return (TreeSetDefaultNid(old_nid));
}

EXPORT int u_of_m_spect__part_name(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)),
			    struct descriptor_d *out_d)
{
  int element = 0, status;
  NCI_ITM nci_list[] = { {4, NciCONGLOMERATE_ELT, 0, 0}, {0, 0, 0, 0} };
  nci_list[0].pointer = (unsigned char *)&element;
  status = TreeGetNci(*(int *)nid_d_ptr->pointer, nci_list);
  if (!(status & 1))
    return status;
  switch (element) {
  case (U_OF_M_SPECT_N_HEAD + 1):
    StrFree1Dx(out_d);
    break;
  case (U_OF_M_SPECT_N_COMMENT + 1):
 COPY_PART_NAME(:COMMENT) break;
  case (U_OF_M_SPECT_N_GO_FILE + 1):
 COPY_PART_NAME(:GO_FILE) break;
  case (U_OF_M_SPECT_N_DATA_FILE + 1):
 COPY_PART_NAME(:DATA_FILE) break;
  case (U_OF_M_SPECT_N_TRIGGER + 1):
 COPY_PART_NAME(:TRIGGER) break;
  case (U_OF_M_SPECT_N_SETUP + 1):
    COPY_PART_NAME(.SETUP) break;
  case (U_OF_M_SPECT_N_SETUP_WAVE_LENGTH + 1):
 COPY_PART_NAME(.SETUP:WAVE_LENGTH) break;
  case (U_OF_M_SPECT_N_SETUP_SLIT_WIDTH + 1):
 COPY_PART_NAME(.SETUP:SLIT_WIDTH) break;
  case (U_OF_M_SPECT_N_SETUP_NUM_SPECTRA + 1):
 COPY_PART_NAME(.SETUP:NUM_SPECTRA) break;
  case (U_OF_M_SPECT_N_SETUP_GRATING_TYPE + 1):
 COPY_PART_NAME(.SETUP:GRATING_TYPE) break;
  case (U_OF_M_SPECT_N_SETUP_EXPOSURE + 1):
 COPY_PART_NAME(.SETUP:EXPOSURE) break;
  case (U_OF_M_SPECT_N_SETUP_FILTER + 1):
 COPY_PART_NAME(.SETUP:FILTER) break;
  case (U_OF_M_SPECT_N_WAVE_LENGTH + 1):
 COPY_PART_NAME(:WAVE_LENGTH) break;
  case (U_OF_M_SPECT_N_SLIT_WIDTH + 1):
 COPY_PART_NAME(:SLIT_WIDTH) break;
  case (U_OF_M_SPECT_N_GRATING_TYPE + 1):
 COPY_PART_NAME(:GRATING_TYPE) break;
  case (U_OF_M_SPECT_N_EXPOSURE + 1):
 COPY_PART_NAME(:EXPOSURE) break;
  case (U_OF_M_SPECT_N_FILTER + 1):
 COPY_PART_NAME(:FILTER) break;
  case (U_OF_M_SPECT_N_SPECTRA + 1):
 COPY_PART_NAME(:SPECTRA) break;
  case (U_OF_M_SPECT_N_ORDER_SORTER + 1):
 COPY_PART_NAME(:ORDER_SORTER) break;
  case (U_OF_M_SPECT_N_INIT_ACTION + 1):
 COPY_PART_NAME(:INIT_ACTION) break;
  case (U_OF_M_SPECT_N_STORE_ACTION + 1):
 COPY_PART_NAME(:STORE_ACTION) break;
  default:
    status = TreeILLEGAL_ITEM;
  }
  return status;
}

extern int u_of_m_spect___init();
#define free_xd_array { int i; for(i=0; i<2;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}
#define error(nid,code,code1) {free_xd_array return GenDeviceSignal(nid,code,code1);}

EXPORT int u_of_m_spect__init(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InInitStruct)
  struct descriptor_xd work_xd[2];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 2);
  initialize_variables(InInitStruct)

      read_string_error(U_OF_M_SPECT_N_GO_FILE, go_file, DEV$_BAD_NAME);
  read_string_error(U_OF_M_SPECT_N_DATA_FILE, data_file, DEV$_BAD_NAME);
  read_integer_error(U_OF_M_SPECT_N_SETUP_WAVE_LENGTH, setup_wave_length, U_OF_M$_BAD_WAVE_LENGTH);
  check_range(setup_wave_length, 0, 13500, U_OF_M$_BAD_WAVE_LENGTH);
  read_integer_error(U_OF_M_SPECT_N_SETUP_SLIT_WIDTH, setup_slit_width, U_OF_M$_BAD_SLIT_WIDTH);
  check_range(setup_slit_width, 0, 500, U_OF_M$_BAD_SLIT_WIDTH);
  read_integer_error(U_OF_M_SPECT_N_SETUP_NUM_SPECTRA, setup_num_spectra, U_OF_M$_BAD_NUM_SPECTRA);
  check_range(setup_num_spectra, 1, 100, U_OF_M$_BAD_NUM_SPECTRA);
  read_integer_error(U_OF_M_SPECT_N_SETUP_GRATING_TYPE, setup_grating_type, U_OF_M$_BAD_GRATING);
  check_range(setup_grating_type, 1, 5, U_OF_M$_BAD_GRATING);
  read_integer_error(U_OF_M_SPECT_N_SETUP_EXPOSURE, setup_exposure, U_OF_M$_BAD_EXPOSURE);
  check_range(setup_exposure, 30, 3000, U_OF_M$_BAD_EXPOSURE);
  read_integer_error(U_OF_M_SPECT_N_SETUP_FILTER, setup_filter, U_OF_M$_BAD_FILTER);
  check_range(setup_filter, 0, 5, U_OF_M$_BAD_FILTER);
  status = u_of_m_spect___init(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array

extern int u_of_m_spect___store();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}

EXPORT int u_of_m_spect__store(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)))
{
  declare_variables(InStoreStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InStoreStruct)

      read_string_error(U_OF_M_SPECT_N_DATA_FILE, data_file, DEV$_BAD_NAME);
  status = u_of_m_spect___store(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array
