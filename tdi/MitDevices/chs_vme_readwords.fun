public fun chs_vme_readwords(in _addr, in _num)
{
  _VME_UDATA    = 0x00090000;
  _VME_D16      = 0x00400000;
  _VME_BS_BYTE  = 0x01000000;
  _VME_BS_LWORD = 0x03000000;
  _VME_DENSE    = 0x10000000;
  _mode = _VME_UDATA | _VME_D16 | _VME_BS_LWORD | _VME_DENSE;
  _data = zero(_num,0w);
  _status = MdsVme->PioRead("/dev/vmp0",val(_addr),val(_mode),val(_num*2),ref(_data));
  return(_status ? _data : *);
}
