public fun incaa_tr10__store(as_is _nid, optional _method)
{
   _DTYPE_RANGE = 201BU;
   _TR10_NODE=1;
   _TR10_BOARD=2;
   _TR10_COMMENT=3;
   _TR10_TRIG_SRC=4;
   _TR10_TRIG_CHAN=5; 
   _TR10_TRIG_OUTPUT=6;
   _TR10_TRIG_SENSE=7;
   _TR10_TRIG_TERM=8; 
   _TR10_TRIG_SYNC=9; 
   _TR10_TRIG_LEVEL=10;
   _TR10_TRIG=11;
   _TR10_CLK_SRC=12;
   _TR10_CLK_OUTPUT=13;
   _TR10_CLK_SENSE=14;
   _TR10_CLK_TERM=15; 
   _TR10_CLK_DIV=16;
   _TR10_CLOCK=17;
   _TR10_EXT_1MHZ=18;
   _TR10_POST_TRIG=19;
   _TR10_PRE_TRIG=20;
   _TR10_INPUTS=21;
   _TR10_NODES_PER_INPUT=4;
   _TR10_INPUT_START=1;
   _TR10_INPUT_END=2;
   _TR10_INPUT_FILTER=3;

  _node = if_error(data(DevNodeRef(_nid,_TR10_NODE)), "");
  if (Len(_node) <= 0) {
     _node = 'local';
  }
  _status = MdsConnect(_node);
  if ((_status < 0) && (_node != 'local')) {
    Write(*,"Could not connect to "//_node);
    Abort();
  }
  _board = if_error(data(DevNodeRef(_nid,_TR10_BOARD)), DevError("TR10 board must be specified"));
  _handle = MdsValue('TR10Open($)', _board);

  /***********************************
   See if digitizer is done digitizing
  ************************************/
  _state = MdsValue('TR10GetState($)', _handle);
  if (_state != _TR10_IDLE) {
    write(*, '%INCAA TR10 board '//_board//' not triggered.  Card in State '//_state);
    Abort();
  }

 _clockSource =  MdsValue('TR10GetClockSource($)',_handle);

  if (_clockSource == _TR10_CLK_SOURCE_INTERNAL ) {
    _clockDiv = MdsValue('TR10GetClockDiv($)', _handle);
    _clockFreq=200000./_clockDiv;
    _clk = make_range(*, *, 1.0D0/_clockFreq);
  }
  else {
    _clk = DevNodeRef(_nid, _TR10_CLOCK);
  } 

  _trigger = DevNodeRef(_nid, _TR10_TRIG);

  /*************************************
   Get setup info
  *************************************/
  _num_chans=16;
  _total_samples = MdsValue("TR10GetActShotSamples($)", _handle);
  _post_trig = MdsValue("TR10GetActPostSamples($)", _handle);
  _pre_trig = _total_samples-_post_trig;

  /*************************************
    Get Requested pre and post trigger
   *************************************/
  _req_post_trig = if_error(data(DevNodeRef(_nid, _TR10_POST_TRIG))*1024, 2097150L );
  _req_post_trig = min(_req_post_trig, _post_trig);
  _req_pre_trig = if_error(data(DevNodeRef(_nid, _TR10_PRE_TRIG))*1024, 0);
  _req_pre_trig = min(_req_pre_trig, _pre_trig);

  _first_idx = - _req_pre_trig;
  _last_idx = _req_post_trig;
  _max_samples = _total_samples;

  _offset = 0.0;
 

  for (_chan=0; _chan < _num_chans; _chan++)
  {
    _chan_offset = _chan * _TR10_NODES_PER_INPUT + _TR10_INPUTS;
    _chan_nid = DevHead(_nid) + _chan_offset;
    if (DevIsOn(_chan_nid))
    {
      _startidx = DevNodeRef(_nid,_chan_offset+_TR10_INPUT_START);
      _endidx   = DevNodeRef(_nid,_chan_offset+_TR10_INPUT_END);
      _lbound = if_error(long(data(_startidx)),_first_idx);

      _ubound = if_error(long(data(_endidx)),_last_idx); /* or _post_trig -1 ? */
      _filter = if_error(text(data(DevNodeRef(_nid, _chan_offset+_TR10_INPUT_FILTER))), "");
      _data= MdsValue('TR10ReadChannel($,$,$,$,$)', _board, _chan+1, _pre_trig+_lbound, _ubound-_lbound, _filter); 
      _inc = (len(_filter) > 0) ? if_error(mdsvalue("public _increment"), 1) : 1;    

      if (_inc > 1) {
        _slope = IF_ERROR(SLOPE_OF(_clk), 0);
        if (_slope != 0) {
          write (*, "building new dim with "//_slope);
          _start = (begin_of(_clk) != $missing) ? begin_of(_clk)+(_slope*_inc)/2. : $missing;
          _dim = make_dim(make_window(_lbound/_inc, _ubound/_inc, _trigger), make_range( _start,  *, _slope*_inc));
        } else {
          _dim = make_function(builtin_opcode('MAP'), make_dim(make_window(_lbound, _ubound, _trigger), _clk), make_range(_lbound + _inc/2,  _ubound,  _inc));
        }
      } else {
        _dim = make_dim(make_window(_lbound,_ubound,_trigger),_clk);
      }
      DevPutSignalNoBounds(_chan_nid, _offset, 10./(32*1024), _data, _dim);
    }
  }
  return(1);
}
