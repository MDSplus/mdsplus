/*------------------------------------------------------------------------------

		Name:   GEN_DEVICE$SIGNAL   

		Type:   C function

     		Author:	Giulio Fregonese

		Date:    6-OCT-90

    		Purpose: Perform device specific error handling 

------------------------------------------------------------------------------
   
   Work done by 1999 Informatica Ricerca Sviluppo, s.c.r.l.,
   under contract from Istituto Gas Ionizzati del CNR - Padova (Italy)
   
--------------------------------------------------------------------------- */
#include <treeshr.h>

extern int GenDeviceCallData();

int GenDeviceSignal(int *head_nid_ptr, unsigned long status_id_1, unsigned long status_id_2) 
{ 
  char *pathname=0;
  pathname = TreeGetPath(*head_nid_ptr);
  if (pathname != 0)
  {
    printf("Error processing device %s\n",pathname);
    TreeFree(pathname);
  }
  return(status_id_1);
}
