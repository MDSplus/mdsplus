public fun INCAA_TR10__TRIGGER(as_is _nid, optional _method)
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
  _TR10_DAQ_MEM=18;
  _TR10_ACTIVE_CHAN=19;
  _TR10_POST_TRIG=20;
  _TR10_PRE_TRIG=21;

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

  _handle = MdsValue('TR10Open($), _board);
  MdsValue('TR10Trigger($), _handle);
  MdsValue('TR10Close($), _handle);
  return(1);
}
