#include <mitdevices_msg.h>
#include <mds_gendevice.h>
#include "mit__dclock_gen.h"
EXPORT int mit__dclock__add(struct descriptor *name_d_ptr, struct descriptor *dummy_d_ptr __attribute__ ((unused)), int *nid_ptr)
{
  static DESCRIPTOR(library_d, "MIT$DEVICES");
  static DESCRIPTOR(model_d, "MIT__DCLOCK");
  static DESCRIPTOR_CONGLOM(conglom_d, &library_d, &model_d, 0, 0);
  int usage = TreeUSAGE_DEVICE;
  int curr_nid, old_nid, head_nid, status;
  long int flags = NciM_WRITE_ONCE;
  NCI_ITM flag_itm[] = { {2, NciSET_FLAGS, 0, 0}, {0, 0, 0, 0} };
  char *name_ptr = strncpy(malloc(name_d_ptr->length + 1), name_d_ptr->pointer, name_d_ptr->length);
  flag_itm[0].pointer = (unsigned char *)&flags;
  name_ptr[name_d_ptr->length] = 0;
  status = TreeStartConglomerate(MIT__DCLOCK_K_CONG_NODES);
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
#define expr " 1000.     "
 ADD_NODE_EXPR(:FREQUENCY_1, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " 1000.     "
 ADD_NODE_EXPR(:FREQUENCY_2, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " -4.	"
 ADD_NODE_EXPR(:TSTART, TreeUSAGE_NUMERIC)
#undef expr
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:START_LOW, TreeUSAGE_NUMERIC)
      flags |= NciM_NO_WRITE_MODEL;
  flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
 ADD_NODE(:GATE, TreeUSAGE_AXIS)
      flags |= NciM_NO_WRITE_SHOT;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " MIT__DCLOCK(TSTART,FREQUENCY_1,FREQUENCY_2,!GETNCI(START_LOW,'STATE'),GATE,1) "
 ADD_NODE_EXPR(:EDGES_R, TreeUSAGE_AXIS)
#undef expr
      flags |= NciM_WRITE_ONCE;
  status = TreeSetNci(curr_nid, flag_itm);
#define expr " MIT__DCLOCK(TSTART,FREQUENCY_1,FREQUENCY_2,!GETNCI(START_LOW,'STATE'),GATE,0) "
 ADD_NODE_EXPR(:EDGES_F, TreeUSAGE_AXIS)
#undef expr
      flags |= NciM_WRITE_ONCE;
  status = TreeSetNci(curr_nid, flag_itm);
  status = TreeEndConglomerate();
  if (!(status & 1))
    return status;
  return (TreeSetDefaultNid(old_nid));
}

EXPORT int mit__dclock__part_name(struct descriptor *nid_d_ptr __attribute__ ((unused)), struct descriptor *method_d_ptr __attribute__ ((unused)),
			   struct descriptor_d *out_d)
{
  int element = 0, status;
  NCI_ITM nci_list[] = { {4, NciCONGLOMERATE_ELT, 0, 0}, {0, 0, 0, 0} };
  nci_list[0].pointer = (unsigned char *)&element;
  status = TreeGetNci(*(int *)nid_d_ptr->pointer, nci_list);
  if (!(status & 1))
    return status;
  switch (element) {
  case (MIT__DCLOCK_N_HEAD + 1):
    StrFree1Dx(out_d);
    break;
  case (MIT__DCLOCK_N_COMMENT + 1):
 COPY_PART_NAME(:COMMENT) break;
  case (MIT__DCLOCK_N_FREQUENCY_1 + 1):
 COPY_PART_NAME(:FREQUENCY_1) break;
  case (MIT__DCLOCK_N_FREQUENCY_2 + 1):
 COPY_PART_NAME(:FREQUENCY_2) break;
  case (MIT__DCLOCK_N_TSTART + 1):
 COPY_PART_NAME(:TSTART) break;
  case (MIT__DCLOCK_N_START_LOW + 1):
 COPY_PART_NAME(:START_LOW) break;
  case (MIT__DCLOCK_N_GATE + 1):
 COPY_PART_NAME(:GATE) break;
  case (MIT__DCLOCK_N_EDGES_R + 1):
 COPY_PART_NAME(:EDGES_R) break;
  case (MIT__DCLOCK_N_EDGES_F + 1):
 COPY_PART_NAME(:EDGES_F) break;
  default:
    status = TreeILLEGAL_ITEM;
  }
  return status;
}

extern int mit__dclock___get_setup();
#define free_xd_array { int i; for(i=0; i<0;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}
#define error(nid,code,code1) error_code = code1;
EXPORT int mit__dclock___get_setup(struct descriptor *nid_d_ptr __attribute__ ((unused)), InGet_setupStruct * in_ptr)
{
  declare_variables(InGet_setupStruct)
    //struct descriptor_xd work_xd[1];
  initialize_variables(InGet_setupStruct)

      read_float_error(MIT__DCLOCK_N_FREQUENCY_1, frequency_1, TIMING$_INVCLKFRQ);
  check_range(frequency_1, .01, .5E6, TIMING$_INVCLKFRQ);
  read_float_error(MIT__DCLOCK_N_FREQUENCY_2, frequency_2, TIMING$_INVCLKFRQ);
  check_range(frequency_2, .01, .5E6, TIMING$_INVCLKFRQ);
  build_results_and_return;
}

#undef free_xd_array
