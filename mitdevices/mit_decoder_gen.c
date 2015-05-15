#include <mitdevices_msg.h>
#include <mds_gendevice.h>
#include "mit_decoder_gen.h"
int mit_decoder__add(struct descriptor *name_d_ptr, struct descriptor *dummy_d_ptr, int *nid_ptr)
{
  static DESCRIPTOR(library_d, "MIT$DEVICES");
  static DESCRIPTOR(model_d, "MIT_DECODER");
  static DESCRIPTOR_CONGLOM(conglom_d, &library_d, &model_d, 0, 0);
  int usage = TreeUSAGE_DEVICE;
  int curr_nid, old_nid, head_nid, status;
  long int flags = NciM_WRITE_ONCE;
  NCI_ITM flag_itm[] = { {2, NciSET_FLAGS, 0, 0}, {0, 0, 0, 0} };
  char *name_ptr = strncpy(malloc(name_d_ptr->length + 1), name_d_ptr->pointer, name_d_ptr->length);
  flag_itm[0].pointer = (unsigned char *)&flags;
  name_ptr[name_d_ptr->length] = 0;
  status = TreeStartConglomerate(MIT_DECODER_K_CONG_NODES);
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
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:COMMENT, TreeUSAGE_TEXT)
 ADD_NODE_INTEGER(:MASTER_REG, 45056, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:CHANNEL_0, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_0:PSEUDO_DEV, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:CHANNEL_1, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_1:PSEUDO_DEV, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:CHANNEL_2, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_2:PSEUDO_DEV, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:CHANNEL_3, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_3:PSEUDO_DEV, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:CHANNEL_4, TreeUSAGE_SIGNAL)
      flags |= NciM_WRITE_ONCE;
  flags |= NciM_NO_WRITE_MODEL;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(CHANNEL_4:PSEUDO_DEV, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE_ACTION(:INIT_ACTION, INIT, INIT, 50, 0, 0, CAMAC_SERVER, 0)
      status = TreeEndConglomerate();
  if (!(status & 1))
    return status;
  return (TreeSetDefaultNid(old_nid));
}

int mit_decoder__part_name(struct descriptor *nid_d_ptr, struct descriptor *method_d_ptr,
			   struct descriptor *out_d)
{
  int element = 0, status;
  NCI_ITM nci_list[] = { {4, NciCONGLOMERATE_ELT, 0, 0}, {0, 0, 0, 0} };
  nci_list[0].pointer = (unsigned char *)&element;
  status = TreeGetNci(*(int *)nid_d_ptr->pointer, nci_list);
  if (!(status & 1))
    return status;
  switch (element) {
  case (MIT_DECODER_N_HEAD + 1):
    StrFree1Dx(out_d);
    break;
  case (MIT_DECODER_N_NAME + 1):
 COPY_PART_NAME(:NAME) break;
  case (MIT_DECODER_N_COMMENT + 1):
 COPY_PART_NAME(:COMMENT) break;
  case (MIT_DECODER_N_MASTER_REG + 1):
 COPY_PART_NAME(:MASTER_REG) break;
  case (MIT_DECODER_N_CHANNEL_0 + 1):
 COPY_PART_NAME(:CHANNEL_0) break;
  case (MIT_DECODER_N_CHANNEL_0_PSEUDO_DEV + 1):
 COPY_PART_NAME(CHANNEL_0:PSEUDO_DEV) break;
  case (MIT_DECODER_N_CHANNEL_1 + 1):
 COPY_PART_NAME(:CHANNEL_1) break;
  case (MIT_DECODER_N_CHANNEL_1_PSEUDO_DEV + 1):
 COPY_PART_NAME(CHANNEL_1:PSEUDO_DEV) break;
  case (MIT_DECODER_N_CHANNEL_2 + 1):
 COPY_PART_NAME(:CHANNEL_2) break;
  case (MIT_DECODER_N_CHANNEL_2_PSEUDO_DEV + 1):
 COPY_PART_NAME(CHANNEL_2:PSEUDO_DEV) break;
  case (MIT_DECODER_N_CHANNEL_3 + 1):
 COPY_PART_NAME(:CHANNEL_3) break;
  case (MIT_DECODER_N_CHANNEL_3_PSEUDO_DEV + 1):
 COPY_PART_NAME(CHANNEL_3:PSEUDO_DEV) break;
  case (MIT_DECODER_N_CHANNEL_4 + 1):
 COPY_PART_NAME(:CHANNEL_4) break;
  case (MIT_DECODER_N_CHANNEL_4_PSEUDO_DEV + 1):
 COPY_PART_NAME(CHANNEL_4:PSEUDO_DEV) break;
  case (MIT_DECODER_N_INIT_ACTION + 1):
 COPY_PART_NAME(:INIT_ACTION) break;
  default:
    status = TreeILLEGAL_ITEM;
  }
  return status;
}

extern int mit_decoder___init();
#define free_xd_array { int i; for(i=0; i<1;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}
#define error(nid,code,code1) {free_xd_array return GenDeviceSignal(nid,code,code1);}

int mit_decoder__init(struct descriptor *nid_d_ptr, struct descriptor *method_d_ptr)
{
  declare_variables(InInitStruct)
  struct descriptor_xd work_xd[1];
  int xd_count = 0;
  memset((char *)work_xd, '\0', sizeof(struct descriptor_xd) * 1);
  initialize_variables(InInitStruct)

      read_string_error(MIT_DECODER_N_NAME, name, DEV$_BAD_NAME);
  status = mit_decoder___init(nid_d_ptr, &in_struct);
  free_xd_array return status;
}

#undef free_xd_array
