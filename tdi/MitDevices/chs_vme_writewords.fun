public fun chs_vme_writewords(in _addr, in _data)
{
  _VME_UDATA    = 0x00090000;
  _VME_D16      = 0x00400000;
  _VME_BS_BYTE  = 0x01000000;
  _VME_BS_LWORD = 0x03000000;
  _VME_DENSE    = 0x10000000;
  _mode = _VME_UDATA | _VME_D16 | _VME_BS_LWORD | _VME_DENSE;
  _status = MdsVme->PioWrite("/dev/vmp0",val(_addr),val(_mode),val(size(_data)*2),ref(_data));
  return(_status);
}
