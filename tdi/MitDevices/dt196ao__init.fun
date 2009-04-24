public fun DT196AO__INIT(as_is _nid, optional _method)
{
   _DT196AO_NODE = 1;
   _DT196AO_BOARD = 2;
   _DT196AO_DIs = 4;
   _DT196AO_NODES_PER_DI = 1;
   _DT196AO_AO_CLK = 10;
   _DT196AO_AO_TRIG = 11; 
   _DT196AO_FAWG_DIV = 12;
   _DT196AO_CYCLE_TYPE = 13;
   _DT196AO_TRIG_TYPE = 14;
   _DT196AO_MAX_SAMPLES = 15;
   _DT196AO_NODES_PER_AO =2;

   _DT196AO_AO_CHANS = 16;
   _DT196AO_AO_FIT = 1;

   _DI_NUMBERS = BUILD_SIGNAL([0,1,2,3,4,5], *, ['DI0', 'DI1', 'DI2', 'DI3', 'DI4', 'DI5' ]);

  _node = if_error(data(DevNodeRef(_nid,_DT196_NODE)), "");
  if (Len(_node) <= 0) {
     _node = 'local';
  }
  _status = MdsConnect(_node);
  if ((_status < 0) && (_node != 'local')) {
    Write(*,"Could not connect to "//_node);
    Abort();
  }
  _board = if_error(data(DevNodeRef(_nid,_DT196AO_BOARD)), DevError("Dt200 board must be specified"));


  _max_samples = IF_ERROR(DATA(DevNodeRef(_nid , _DT196AO_MAX_SAMPLES)), 16384);
  _max_samples = int(min(max(_max_samples, 2), 16384));

  _clock_source = DevNodeRef(_nid , _DT196AO_AO_CLK);
  _clock = DevNodeRef(_nid, _DT196AO_DIs+_DI_NUMBERS[_clock_source]);
  _trig_source = DevNodeRef(_nid , _DT196AO_AO_TRIG);
  _trigger = DevNodeRef(_nid, _DT196AO_DIs+_DI_NUMBERS[_trig_source]);

  _fawg_div = if_error(data(DevNodeRef(_nid, _DT196AO_FAWG_DIV)), 20);

  
  _slope = slope_of(_clock)*_fawg_div;
  _dim = data(build_dim(build_window(0, _max_samples-1, _trigger), * : * : _slope));

  _cmd = "set.ao_clk "//_clock_source//" rising";
  MdsValue('Dt200WriteMaster($, $, 1)',_board, _cmd);
  _cmd = "set.ao_trig "//_trig_source//" rising";
  MdsValue('Dt200WriteMaster($, $, 1)',_board, _cmd);
  _cmd = "set.dtacq FAWG_div "//_fawg_div;
  MdsValue('Dt200WriteMaster($, $, 1)',_board, _cmd);
  _cmd = "set.dtacq AO_sawg_rate 10000";
  MdsValue('Dt200WriteMaster($, $, 1)',_board, _cmd);

  _cycle_type = IF_ERROR(DATA(DevNodeRef(_nid , _DT196AO_CYCLE_TYPE)), 'ONCE');
  _trig_type = IF_ERROR(DATA(DevNodeRef(_nid , _DT196AO_TRIG_TYPE)), 'HARD_TRIG');
  _cmd = "set.arm.AO.FAWG "//_cycle_type//" "//_trig_type;
  MdsValue('Dt200WriteMaster($, $, 1)', _board, _cmd);
  _dim = cvt(_dim, 1.0);
  for (_chan=0; _chan < 16; _chan++)
  {
    _chan_offset = _chan * _DT196AO_NODES_PER_AO + _DT196AO_AO_CHANS;
    _chan_nid = DevHead(_nid) + _chan_offset;
    if (DevIsOn(_chan_nid))
    {
      _sig = if_error(getnci(_chan_nid, "RECORD"), build_signal([0., 0.], *, [0., 1.]));
      _knots_x = dim_of(_sig);
      _knots_y = data(_sig);
      _fit = DevNodeRef(_nid,_chan_offset+_DT196AO_AO_FIT);

      _knots_y = cvt(_knots_y, 1.0);
      _knots_x = cvt(_knots_x, 1.0);
      if(_fit == 'SPLINE') {
        _wave = SplineFit(_knots_x, _knots_y, _dim);
      } else {
        _wave = LinFit(_knots_x, _knots_y, _dim);
      }
      _wave = data(_wave);
      _wave=word(_wave/10.*2^15);
      if (size(_wave) gt _max_samples) {
        _wave=_wave[0: _max_samples-1];
      }
      MdsValue('Dt196AOWriteOutput($,$,$)', _board, _chan+1, _wave);
    }
    else
    {
      MdsValue('Dt196AOWriteOutput($,$,[0,0])', _board, _chan+1);
    }
  }
  MdsValue('Dt196AOSendFiles($)', _board);
  return(1);
}
