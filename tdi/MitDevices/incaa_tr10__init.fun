public fun INCAA_TR10__INIT(as_is _nid, optional _method)
{
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

  _node = if_error(data(DevNodeRef(_nid,_TR10_NODE)), "");
  if (Len(_node) <= 0) {
     _node = 'local';
  }
  _status = MdsConnect(_node);
  if ((_status < 0) && (_node != 'local')) {
    Write(*,"Could not connect to "//_node);
    Abort();
  }
  _board = if_error(data(DevNodeRef(_nid,_TR10_BOARD)), DevError("INCAA TR10 board must be specified"));

  _handle = MdsValue('TR10Open($)', _board);
  MdsValue('TR10Reset($)', _handle);

  _clk_src= if_error(data(DevNodeRef(_nid, _TR10_CLK_SRC)), 'int');
  _clk_output = if_error(data(DevNodeRef(_nid, _TR10_CLK_OUTPUT)), 0);
  _clk_sense = if_error(data(DevNodeRef(_nid, _TR10_CLK_SENSE)), '+');
  _clk_term = if_error(data(DevNodeRef(_nid, _TR10_CLK_TERM)), 0);
  _clk_divide = if_error(data(DevNodeRef(_nid, _TR10_CLK_DIV)), 1);
  MdsValue('TR10SetClockMode($,$,$,$,$,$,$)',_handle, _clk_src, _clk_output, 'div', _clk_sense, _clk_term, _clk_divide);

  _trig_chan = 0;
  _trig_src = if_error(data(DevNodeRef(_nid, _TR10_TRIG_SRC)), 'ext');
  if (len(_trig_src) == 1) {
    _trig_chan = execute(_trig_src);
    _trig_src = 'pxi';
  }
  _trig_output = if_error(data(DevNodeRef(_nid,_TR10_TRIG_OUTPUT)), 0);
  _trig_sense = if_error(data(DevNodeRef(_nid, _TR10_TRIG_SENSE)),'+');
  _trig_term = if_error(data(DevNodeRef(_nid, _TR10_TRIG_TERM)), 0);
  _trig_sync = if_error(data(DevNodeRef(_nid, _TR10_TRIG_SYNC)), 0);
  _trig_level = if_error(data(DevNodeRef(_nid, _TR10_TRIG_LEVEL)), 1);
  MdsValue('TR10SetTrigger($,$,$,$,$,$,$,$)',_handle, _trig_src, _trig_chan, _trig_output, _trig_sense, _trig_term, _trig_sync, _trig_level);

  _post_trig = if_error(2097150L-data(DevNodeRef(_nid, _TR10_PRE_TRIG))*1024, 2097150L );

  MdsValue('TR10SetPostSamples($,$)', _handle, _post_trig); 
  MdsValue('TR10StartSampling($)', _handle);
  MdsValue('TR10Close($)', _handle);
  
  return(1);
}
