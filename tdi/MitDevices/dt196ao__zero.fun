public fun DT196AO__ZERO(as_is _nid, optional _method)
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

  MdsValue('Dt200WriteMaster($, "set.A0 XX 0", 1)', _board);
  MdsValue('Dt200WriteMaster($, "set.AO commit 1", 1)', _board);
  return(1);
}
