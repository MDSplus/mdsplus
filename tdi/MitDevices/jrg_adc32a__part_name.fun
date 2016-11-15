public fun jrg_adc32a__part_name(as_is _nid, optional in _method)
{
  private _N_NAME = 1;
  private _N_COMMENT = 2;
  private _N_ACTIVE_CHAN = 3;
  private _N_ACTIVE_MEM = 4;
  private _N_EXT_CLOCK = 5;
  private _N_TRIGGER = 6;
  private _N_CLOCK_OUT = 7;
  private _N_CHANNELS = 8;
  private _K_NODES_PER_CHANNEL = 4;
  private _K_CHAN_START_OFFSET = 1;
  private _K_CHAN_END_OFFSET = 2;
  private _K_CHAN_GAIN_OFFSET = 3;

  _name = [
  '',
  ":NAME",
  ":COMMENT",
  ":ACTIVE_CHAN",
  ":ACTIVE_MEM",
  ":EXT_CLOCK",
  ":TRIGGER",
  ":CLOCK_OUT"
  ];
  _chans = '';
  for (_i=1; _i<=32; _i++) {
    _cname = ':INPUT_'//char(_i/10+ichar('0'))//char(_i mod 10 +ichar('0'));
    _chans = [_chans, 
      _cname,
      _cname//':STARTIDX', 
      _cname//':ENDIDX', 
      _cname//':GAIN' 
      ];
  } 
  _names = [ _name, _chans[1:*], 
  ":INIT_ACTION",
  ":STORE_ACTION"];
  _name = _names[getnci(_nid,'conglomerate_elt')-1];
  return(trim(_name));
}
