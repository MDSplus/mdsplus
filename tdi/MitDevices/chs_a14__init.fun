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
  _is_int_clock = getnci(_ext_clock,"LENGTH") == 0;
  _mode = 0; /* cinos only supports mode 0 */
  _sr = _clock_speed | 
        (_is_int_clock << 3) | 
        ((_mode & 7) << 8) | 
        ((_gated != 0) << 11) | 
        ((_stp_polarity != 0) << 12) |
        ((_str_polarity != 0) << 13) |
        ((_clk_polarity != 0) << 14);
  _settings = chs_vme_readwords(0x106e0000,128);
  _settings = [_settings[0:(33+_dignum*16)],word(_sr & 0xffff),_settings[(35+_dignum*16) : *]];
  return(chs_vme_writewords(0x106e0000,_settings));
}
