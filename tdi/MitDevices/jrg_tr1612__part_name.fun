public fun jrg_tr1612__part_name(as_is _nid, optional in _method)
{
  _name = [
  '',
  ":NAME",
  ":COMMENT",
  ":MODULE_ID",
  ":SAMP_INTRVAL",
  ":EXT_CLOCK",
  ":TRIGGER",
  ":SAMP_2_STORE",
  ":PRETRIG_SAMP",
  ":ACTIVE_CHANS",
  ":SAMP_AVERAGE",
  ":TRIG_BY_CHAN",
  ":TRIG_DELAY",
  ":VALID_DATA",
  ":VALID_TRIG",
  ":LAM_SETUP",
  ":MUL_PRE_POST",
  ":RTC_ENABLE",
  ":RTC_TRIGS",
  ":MEMORY_SHARE"
  ];
  _chans = '';
  for (_i=1; _i<=16; _i++) {
    _cname = ':INPUT_'//char(_i/10+ichar('0'))//char(_i mod 10 +ichar('0'));
    _chans = [_chans, 
      _cname,
      _cname//':COMMENT', 
      _cname//':STARTIDX', 
      _cname//':ENDIDX', 
      _cname//':RANGE', 
      _cname//':OFFSET', 
      _cname//':TRIG_THRESH', 
      _cname//':VALID_THRESH'];
  } 
  _names = [ _name, _chans[1:*], 
  ":INIT_ACTION",
  ":STORE_ACTION"];
  _name = _names[getnci(_nid,'conglomerate_elt')-1];
  return(trim(_name));
}
