public fun DT216a__PART_NAME(as_is _nid, optional in _method)
{
  _name = [
  '',
  ":NODE",
  ":BOARD",
  ":COMMENT",
   ":RANGES",
  ":STATUS_CMDS",
  ":BOARD_STATUS",
  ":SEG_LENGTH",
  ":DI0",
  ":DI0:WIRE",
  ":DI0:BUS",
  ":DI1",
  ":DI1:WIRE",
  ":DI1:BUS",
  ":DI2",
  ":DI2:WIRE",
  ":DI2:BUS",
  ":DI3",
  ":DI3:WIRE",
  ":DI3:BUS",
  ":DI4",
  ":DI4:WIRE",
  ":DI4:BUS",
  ":DI5",
  ":DI5:WIRE",
  ":DI5:BUS",
  ":CLOCK_SRC",
  ":CLOCK_DIV",
  ":DAQ_MEM",
  ":ACTIVE_CHAN",
  ":TRIG_SRC",
  ":POST_TRIG",
  ":PRE_TRIG"
  ];
  _chans = '';
  for (_i=1; _i<=16; _i++) {
    _cname = ':INPUT_'//char(_i/10+ichar('0'))//char(_i mod 10 +ichar('0'));
    _chans = [_chans, 
      _cname,
      _cname//':STARTIDX', 
      _cname//':ENDIDX', 
      _cname//':INC', 
      _cname//':FILTER_COEFS',
      _cname//':VIN'];
  } 
  _names = [ _name, _chans[1:*], 
  ":INIT_ACTION",
  ":STORE_ACTION"];
  _name = _names[getnci(_nid,'conglomerate_elt')-1];
  return(trim(_name));
}
