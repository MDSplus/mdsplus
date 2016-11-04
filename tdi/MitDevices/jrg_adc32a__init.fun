public fun JRG_ADC32A__INIT(as_is _nid, optional _method) {
/*
 * $Id$
 *
 * TDI function to initialize the Joerger ADC32A digitizers
 * (here named JRG_ADC32A - 32 channels, n bits)
 *
 * Copyright (c) 2015
 * Massachusetts Institute of Technology
 * All rights reserved
 */

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
  
  _debug = getenv('DEBUG_DEVICES') != "";
  _name = DevNodeRef(_nid,_N_NAME);  /* get name */
  _mem_size_idx = 0wu;
  _mem_sizes = [128L*1024L, 512L*1024L, 1024L*1024L, 2048L*1024L] ;

  DevCamChk(_name,CamPiow(_name,1wu,25wu,_d=0wu,16wu),1,1);  /* master reset */
  if(_debug) write(*, "PIO/f=25/a=1/mem=16 "//_name);
  DevCamChk(_name,CamPiow(_name,0wu,3wu,_d=_mem_size_idx,16wu),1,1);  /* master reset */
  if(_debug) write(*, "PIO/f=3/a=0/mem=16 "//_name//" data="//_mem_size_idx);
  _mem_size = _mem_sizes[_mem_size_idx & 0x7];
  DevCamChk(_name,CamPiow(_name,0wu,9wu,_d=0wu,16wu),1,*);  /* master reset */
  if(_debug) write(*, "PIO/f=9/a=0/mem=16 "//_name);

  _status_reg = 0L;
  _active_chan = DevNodeRef(_nid, _N_ACTIVE_CHAN);
  _status_reg = _status_reg | (int(log2(_active_chan)) & 0x7);
  _status_reg = _status_reg | (int(log2(DevNodeRef(_nid,_N_ACTIVE_MEM))) << 3);
  _status_reg = _status_reg | 1 << 8;   /* burst mode*/
/* next bit multiburst = 0 */  /* single burst */
  _ext_clock = DevNodeRef(_nid,_N_EXT_CLOCK);
  if(getnci(_ext_clock,"LENGTH") > 0) {
    _status_reg |= (1 << 10);
  }
  _status_reg = _status_reg | (1 << 11); /* separate gains */
/* next bit common gain = 0 */
  DevCamChk(_name,CamPiow(_name,0wu,16wu,_status_reg,16wu),1,1);  /* write status register */
  if(_debug) write(*, "PIO/f=16/a=0/mem=16 "//_name//" /data="//_status_reg);
  /* now set the gain for each channel */
  for(_chan=0; _chan<32; _chan++) 
  {
    _chan_nid_offset = _chan * _K_NODES_PER_CHANNEL + _N_CHANNELS;
    _gain_nid_offset = _chan_nid_offset+_K_CHAN_GAIN_OFFSET;
    _gain = int(log2(min(max(DevNodeRef(_nid,_gain_nid_offset), 1), 8)));
    DevCamChk(_name,CamPiow(_name, (_chan mod 16), ((_chan < 16)? 17 : 20), _gain, 16wu),1,1);
    if(_debug) write(*, "PIO/a="//(_chan mod 16)//"/f="// ((_chan < 16)? 17 : 20)//"/mem=16 "//_name//" /data="//int(DevNodeRef(_nid,_gain_nid_offset)));
  }
  DevCamChk(_name,CamPiow(_name,0wu,26wu,_d=0wu,16wu),1,*);
  if(_debug) write(*, "PIO/f=26/a=0/mem=16 "//_name);
  return(1);  /* return nicely*/
}
