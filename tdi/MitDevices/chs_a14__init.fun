public fun chs_a14__init(as_is _nid, optional _method)
{
  _dignum         = DevNodeRef(_nid,1);
  _clock_divide = DevNodeRef(_nid,3);
  _ext_clock    = DevNodeRef(_nid,4);
  _pts          = DevNodeRef(_nid,6);
  _mode         = DevNodeRef(_nid,7);
  _clk_polarity = DevNodeRef(_nid,9);
  _str_polarity = DevNodeRef(_nid,10);
  _stp_polarity = DevNodeRef(_nid,11);
  _gated        = DevNodeRef(_nid,12);
  _clock_speed  = data(build_signal([0,1,2,3,4,5,6],*,[1,2,4,10,20,40,100])[_clock_divide]);
  _is_ext_clock = getnci(_ext_clock,"LENGTH") > 0;
  _sr = _clock_speed | 
        (_is_ext_clock << 3) | 
        ((_mode & 7) << 8) | 
        ((_gated != 0) << 11) | 
        ((_stp_polarity != 0) << 12) |
        ((_str_polarity != 0) << 13) |
        ((_clk_polarity != 0) << 14);
  _bytes_written = 0;
  _VME_UDATA    = 0x00090000;
  _VME_D16      = 0x00400000;
  _VME_BS_LWORD = 0x03000000;
  _VME_DENSE    = 0x10000000;
  _mode = _VME_UDATA | _VME_D16 | _VME_BS_LWORD | _VME_DENSE;
  _addr         = 0x10500000 + _dignum * 8;
  return(MdsVme->VmePioWrite("/dev/dmaex0",val(_addr),val(_mode),val(4),_sr,ref(_bytes_written)));
}
