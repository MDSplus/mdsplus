public fun CAENVME_FIFOBLTWriteCycle(in _handle, in _addr, in _data, in _data_len, out _ret_len)
{
    private  _cvA32_S_DATA    = 0x0D;         /* A32 supervisory data access                  */
    private  _cvD64 = 0x08;

    _out_len = 0L;
    _status = libCAENVME->CAENVME_FIFOBLTWriteCycle(val(_handle), val(_addr), ref(_data), 
		val(long(_cvA32_S_DATA)), val(long(_cvD64)), ref(_out_len));
    _ret_len = _out_len;
    return (_status);
}

