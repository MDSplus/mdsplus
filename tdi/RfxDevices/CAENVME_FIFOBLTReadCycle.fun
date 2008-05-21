public fun CAENVME_FIFOBLTReadCycle(in _handle, in _addr, in _data_len, out _ret_len, out _status)
{
    private  _cvA32_S_DATA    = 0x0D;         /* A32 supervisory data access                  */
    private  _cvD64 = 0x08;
write(*, 'data len ', _data_len);
    _data = zero(_data_len, 0BU);

    _out_len = 0L;
    _status = libCAENVME->CAENVME_FIFOBLTReadCycle(val(_handle), val(_addr), ref(_data), val(long(_data_len)),
		val(long(_cvA32_S_DATA)), val(long(_cvD64)), ref(_out_len));
    _ret_len = _out_len;


    return (_data);
}

