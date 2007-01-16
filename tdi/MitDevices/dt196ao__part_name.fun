public fun DT196AO__PART_NAME(as_is _nid, optional in _method)
{
  _name = [
  '',
  ":NODE",
  ":BOARD",
  ":COMMENT",
  ":DI0",
  ":DI1",
  ":DI2",
  ":DI3",
  ":DI4",
  ":DI5",
  ":AO_CLK",
  ":AO_TRIG",
  ":FAWG_DIV",
  ":CYCLE_TYPE",
  ":TRIG_TYPE",
  ":MAX_SAMPLES"
  ];
  _chans = '';
  for (_i=1; _i<=16; _i++) {
    _cname = ':OUTPUT_'//char(_i/10+ichar('0'))//char(_i mod 10 +ichar('0'));
    _chans = [_chans, 
      _cname,
      _cname//':FIT' ];
  } 
  _names = [ _name, _chans[1:*], 
  ":INIT_ACTION",
  ":STORE_ACTION"];
  _name = _names[getnci(_nid,'conglomerate_elt')-1];
  return(trim(_name));
}
