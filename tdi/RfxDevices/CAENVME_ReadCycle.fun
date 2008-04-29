public fun CAENVME_ReadCycle(in _handle, in _addr, out _data)
{
    private  _cvA32_S_DATA    = 0x0D;         /* A32 supervisory data access                  */
    private  _cvD32 = 0x04;

    _read_data = 0L;
    _status = libCAENVME->CAENVME_ReadCycle(val(_handle), val(_addr), ref(_read_data), 
		val(long(_cvA32_S_DATA)), val(long(_cvD32)));
    _data = _read_data;
    return (_status);
}

