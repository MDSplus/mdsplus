public fun l6810__store(as_is _nid, optional _method)
{

/* node offsets in the conglomerate */
private _K_CONG_NODES = 36;
Private _N_HEAD = 0;
Private _N_NAME = 1;
Private _N_COMMENT = 2;
Private _N_FREQ = 3;
Private _N_EXT_CLOCK_IN = 4;
Private _N_STOP_TRIG = 5;
Private _N_MEMORIES = 6;
Private _N_SEGMENTS = 7;
Private _N_ACTIVE_MEM = 8;
Private _N_ACTIVE_CHANS = 9;
Private _N_INPUT_01 = 10;
Private _N_INIT_ACTION = 34;
Private _N_STORE_ACTION = 35;

/* nid offsets for each channel */
Private _K_NODES_PER_CHANNEL = 6;
Private _N_CHANNELS = _N_INPUT_01;  
Private _N_CHAN_HEAD = 0;
Private _N_CHAN_STARTIDX = 1;
Private _N_CHAN_ENDIDX = 2;
Private _N_CHAN_GAINIDX = 3;
Private _N_CHAN_SRC_CPLING = 4;
Private _N_CHAN_OFFSET = 5;

/* array offsets into setup structure */
Private _t_s_res = 0;
Private _sensitivity = 1;
Private _block_len = 5;
Private _num_blk_lsb = 6;
Private _num_blk_msb = 7 ;
Private _trigger_holdoff = 8;
Private _trigger_slope = 9;
Private _trigger_coupling = 10;
Private _trig_up = 11;
Private _trig_lo = 12;
Private _trig_src = 13;
Private _near_low = 14;
Private _near_hi = 15;
Private _ACTIVE_CHAN = 16;
Private _c1_off  = 17;
Private _c2_off = 18;
Private _c3_off = 19;
Private _c4_off = 20;
Private _c1_src = 21;
Private _c2_src = 22;
Private _c3_src = 23;
Private _c4_src = 24;
Private _trigger_delay = 25;
Private _SAMPS_PER_SEG = 26;
Private _NUM_SEG_LO = 27;
Private _NUM_SEG_HI = 28;
Private _dual_timebase = 29;
Private _F1_FREQ = 30;
Private _f2_freq = 31;
Private _mem_size = 32;

  /* 
     routine to read 1 channel from the 6810
     note that CamQstopw calls a routine which knows how to do
     I/Os > 64K bytes
   */
  Private fun ReadChannel( in _name , in _chan, in _segs, in _samples_per_segment, in _drop, in _samples_to_read, 
                                  OUT _channel_data )
  {
   DevCamChk(_name,CamPiow(_name, 5, 16, _d = 0, 16), 1, *);
    DevCamChk(_name,CamPiow(_name, 6, 16, _d = 0, 16), 1, *);
    DevCamChk(_name,CamPiow(_name, 7, 16, _d = 0, 16), 1, *);
    _ans_expr = "";
    for (_seg=0; _seg<_segs; _seg++) {
      DevCamChk(_name,CamPiow(_name, _chan+1, 18, _seg, 16), 1, *);
      l6810_wait(_name);
      for (_i=0; _i<_drop; _i++) {
        DevCamChk(_name,CamPiow(_name, 0, 2, _dummy, 16), 1, *);
      }
      _seg_var = '_seg_'//trim(adjustl(_seg));
      _cmd = "DevCamChk(_name, CamQstopw(_name, 0, 2, _samples_per_segment,"//_seg_var//",16),1,*)";
      _status = execute(_cmd);
      _exp = "_ans_expr = _ans_expr //'" // _seg_var // ((_seg < (_segs-1)) ? ",'" : "' ");
      _status = execute(_exp);
    }
    _a_expr = "_channel_data = [" // _ans_expr // "]";
    _status = execute(_a_expr);
    
   return(1);
  }

/*
  Arrays of setup constants
*/
Private _dts = [0.0, 1/20., 1/50., 1/100., 1/200., 1/500., 1/1000.,
                1/2000., 1/5000., 1/10000., 1/20000., 1/50000.,
                1/100000., 1/200000., 1/500000., 1/1000000., 1/2000000., 1/5000000.];

Private _coeffs = [100E-6, 250E-6, 500E-6, 1E-3, 2.5E-3, 6.26E-3, 12.5E-3, 25E-3];
 

  /***********************************
     start of store.
       get the name
       get the trigger
  ************************************/
    _name      = DevNodeRef(_nid,_N_NAME);
  _trigger   = DevNodeRef(_nid,_N_STOP_TRIG);

  /***********************************
   See if digitizer is done digitizing
  ************************************/
  DevCamChk(_name,CamPiow(_name,0,8,_d=0,16),1,*);
  if (!CamXandQ())
  {
    write(*,'%CAMERR, module '//_name//', Device not triggered');
    return(0);
  }

  /*************************************
   Get setup info
  *************************************/
/*  _setup = byte(66); */
  DevCamChk(_name,CamPiow(_name, 0, 18, _d=0, 16), 1, *);
  DevCamChk(_name, CamStopw(_name, 1, 2, 33, _setup, 16), 1, *);

  /*************************************
    if internal clock then get the frequency 
    otherwise use external clock
  *************************************/
  if (_setup[_F1_FREQ] != 0) {
    _clk = make_range(*, *, _dts[_setup[_F1_FREQ]]);
  }
  else {
    _clk = DevNodeRef(_nid, _N_EXT_CLOCK_IN);
  }
  _segs = _setup[_NUM_SEG_LO] + _setup[_NUM_SEG_HI]*256;
  _drop = (_setup[_ACTIVE_CHAN] == 4) ? 2 : (_setup[_ACTIVE_CHAN] == 2) ? 5 : 10;
  _samples_per_segment = ISHFT(1L,_setup[_SAMPS_PER_SEG])*1024L - _drop;
  _samples_per_channel = _samples_per_segment*_segs;
  _post_trig = _samples_per_channel;
  _min_idx = 0;
  _max_idx = _samples_per_channel - 2;

  /*************************************
    Loop through the channels
      read the data
      Store signal
  *************************************/
  for (_chan=0; _chan < _setup[_ACTIVE_CHAN]; _chan++) 
  {
    _chan_nid_offset = _chan * _K_NODES_PER_CHANNEL + _N_CHANNELS;
    _chan_nid = DevHead(_nid) + _chan_nid_offset;
    if (DevIsOn(_chan_nid))
    {
      _startidx = DevNodeRef(_nid,_chan_nid_offset+_N_CHAN_STARTIDX);
      _endidx   = DevNodeRef(_nid,_chan_nid_offset+_N_CHAN_ENDIDX);
      _lbound = if_error(long(data(_startidx)),0);
      _lbound = min(max(_lbound,0),_samples_per_channel-1);
      _ubound = if_error(long(data(_endidx)),_max_idx);
      _ubound = min(max(_ubound,_lbound),_samples_per_channel);
      _samples_to_read = _ubound - _lbound + 1;
      _status = ReadChannel(_name, _chan, _segs, _samples_per_segment, _drop, _samples_to_read, _channel_data);
      _coefficient = _coeffs[_setup[_SENSITIVITY+_chan]];
      _offset =  _setup[_C1_OFF+_chan] * 16 * -1;
      if (_segs != 1) {
        _dim_expr = '['; 
        _clock = execute('decompile(`_clk)');
        _trig = execute('decompile(`_trigger)');
        _seglen = _samples_per_segment+1;
        for (_i=0; _i<_segs; _i++) {
          _dim_expr = _dim_expr // 'make_dim(make_window('//_drop//','
                      //_seglen//','//_trig//'['  // _i // ']),'
                      //_clock//') ' // ((_i < _segs-1) ? ',' : ']');
        }
/*          _dim = compile(_dim_expr); */
        _dim = compile("set_range(0 : "//_ubound//","//_dim_expr//")");
      }
      else {
        _dim = make_dim(make_window(_lbound,_ubound,_trigger),_clk);
      }
      _stat = DevPutSignal(_chan_nid, _offset, 10./4096, _channel_data[_lbound : _ubound], _lbound, _ubound, _dim);
    }
  }
  return(1);
}