public fun CAENVME_Init(in _board_id)
{
	private  _cvV1718 = 0L;                    /* CAEN V1718 USB-VME bridge                    */
        private  _cvV2718 = 1L;                    /* V2718 PCI-VME bridge with optical link       */
        private  _cvA2818 = 2L;                    /* PCI board with optical link                  */
        private  _cvA2719 = 3L;                     /* Optical link piggy-back                      */

write(*, 'INIT');

    _handle = 0L;
    _status = libCAENVME->CAENVME_Init(val(_cvV2718), val(0L), val(long(_board_id)), _handle);
    if(_status == 0)
	return (_handle);
    else
	return (-1);
}


    
