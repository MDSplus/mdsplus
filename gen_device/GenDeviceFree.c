/*------------------------------------------------------------------------------

		Name:   GEN_DEVICE$FREE   

		Type:   C function

     		Author:	Gabriele Manduchi
			Istituto Gas Ionizzati del CNR - Padova (Italy)

		Date:   17-SEP-1993

    		Purpose: Free dynamic data structures allocated by routines generatted by GEN_DEVICE 

------------------------------------------------------------------------------*/
#include <mdsdescrip.h>
#include <stdlib.h>
#include <mdsshr.h>
#include "gen_device.h"

int GenDeviceFree(CommonInStruct *in_struct)
{ 
    int i, status = 1;
    for(i = 0; (status & 1)&& (i < in_struct->num_xds); i++)
	status = MdsFree1Dx(&in_struct->xds[i],0);
    if(in_struct->num_xds)
    {
	free((char *)in_struct->xds);
        in_struct->num_xds = 0;
    }
    return status;
}
