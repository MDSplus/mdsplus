public fun CAENVME_WriteCycle(in _handle, in _addr, in _data)
{
    private  _cvA32_S_DATA    = 0x0D;         /* A32 supervisory data access                  */
    private  _cvD32 = 0x04;

    _status = libCAENVME->CAENVME_WriteCycle(val(_handle), val(_addr), ref(long(_data)), 
		val(long(_cvA32_S_DATA)), val(long(_cvD32)));
    return (_status);
}

