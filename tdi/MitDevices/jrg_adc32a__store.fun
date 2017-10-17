public fun jrg_adc32a__store(as_is _nid, optional _method) {
/*
 * $Id$
 *
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

  private _SIZES = [128L*1024L, 512L*1024L, 1024L*1024L, 2048L*1024L] ;

  _debug = getenv('DEBUG_DEVICES') != "";
  _name	       = DevNodeRef(_nid,_N_NAME);  /* get name */
  _trigger       = DevNodeRef(_nid,_N_TRIGGER);  /* get trigger */
  _active_chan       = DevNodeRef(_nid,_N_ACTIVE_CHAN);  /* get number of samples */
  _active_mem = DevNodeRef(_nid, _N_ACTIVE_MEM);
  _module_id = 0L;

  /* Read Module ID */
  _module_id = 0L;
  DevCamChk(_name,CamPiow(_name,0wu,3wu,_module_id,24wu),1,1);
  if (_debug) write(*, "got the module id "//_module_id);
  _memsize = _module_id & 7;
  _filters = (_module_id & 8 ) >> 3;
  _gain10 = (_module_id & 16) >> 4;
  _model = (_module_id & 32) >> 5;
  _unipolar = (_module_id & 64) >> 6;
  _active = (_module_id & 128) >> 7;

  _memsize = _SIZES[_memsize];
  if (_debug) write(*, "memsize is "//_memsize);

  /* Read Status Register */
  _status_reg = 0L;
  DevCamChk(_name,CamPiow(_name,0wu,4wu,_status_reg,24wu),1,1);
  if (_debug) write(*, "status_reg is "//_status_reg);

  _active_chans   = _status_reg & 7;
  _active_chans   = 2^_active_chans;
  _status_reg     = _status_reg >> 3;
  _active_mem     = _status_reg & 31;
  _active_mem     &=0x1F;
  _active_mem     = 2^_active_mem;
  _chan_size      = _active_mem/_active_chans;
  if (_debug) write(*, "_active_mem = "//_active_mem//" , _active_chans = "//_active_chans//" , _chan_size = "//_chan_size);
  _status_reg     = _status_reg >> 5;
  _burst_mode     = _status_reg & 1;
  _status_reg     = _status_reg >> 1;
  _mulit_burst    = _status_reg & 1;
  _status_reg     = _status_reg >> 1;
  _ext_clock      = _status_reg & 1;
  _status_reg     = _status_reg >> 1;
  _separate_gains = _status_reg & 1;
  _status_reg     = _status_reg >> 1;
  _status_reg     = _status_reg >> 2; /* fill */
  _common_gain    = _status_reg & 3;

  _chunk_buffer = zero(65535, 0W);
  _data_buffer = "";
  for (_samps_read=0; _samps_read < _active_mem; _samps_read += _chunk_read)
  {
    _chunk_read = min(65535, _active_mem-_samps_read);
    if (_debug) write(*, "about to read "//_chunk_read);
    DevCamChk(_name,CamStopw(_name,0,2wu,_chunk_read,_chunk_buffer,16wu),1,*);
    if (_samps_read==0) {
      _data_buffer = _chunk_buffer;
    } else {
      _data_buffer=[_data_buffer, _chunk_buffer];
    }
  }
  if (_debug) write(*,"read all the data");
/*  _data_buffer = set_range([_memsize],_data_buffer); */

  if (_ext_clock) {
    _clock = DevNodeRef(_nid, _N_EXT_CLOCK);
  } else {
    _clock = make_range(* , * , _active_chan*10E-6);
  }
/********************************/
/* for each active channel... */
  for (_input_chan=1; _input_chan <= _active_chans; _input_chan++) {
    if (_debug) write(*, "starting on ", _input_chan);
    _chan_nid_offset =  (_input_chan-1) * _K_NODES_PER_CHANNEL + _N_CHANNELS;
    _chan_nid = DevHead(_nid) + _chan_nid_offset;
    /* if channel is on */
    if (DevIsOn(_chan_nid)) {
      if (_debug) write(*, "\tit is on");
      _idx = _input_chan-1 : _active_mem-1 : _active_chans;
      _cbuf = _data_buffer[_idx];
      _gain_bits = (_cbuf[0] & 0xC000) >> 14;
      _cbuf = _cbuf & 0xfff;
      _startidx = DevNodeRef(_nid,_chan_nid_offset+_K_CHAN_START_OFFSET);
      _endidx   = DevNodeRef(_nid,_chan_nid_offset+_K_CHAN_END_OFFSET);
      _lbound = if_error(long(data(_startidx)), 0);
      _ubound = if_error(long(data(_endidx)), _chan_size-1);
      _ubound = min(max(_ubound,_lbound), _chan_size);
      _dim = make_dim(make_window(_lbound,_ubound,_trigger),_clock);
      if (_debug) write(*, "\tgain_bits="//_gain_bits);
      _coefficient = 20./(2^_gain_bits)/4096.;
      if (!_unipolar) {
        if (_debug) write(*, "\tBipolar, coeff=" //_coefficient);
        _offset = -2048;
      } else {
        write(*, "NB");
        _offset = 0.0;
      }
      /* Write data to tree */
      _cbuf &= 0xFFF;
      DevPutSignal(_chan_nid, _offset, _coefficient, _cbuf, _lbound, _ubound, _dim);
    }
  }

  return(1);  /* return nicely */
}
