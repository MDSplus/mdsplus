public fun DT196__REBOOT(as_is _nid, optional _method)
{
   _DT200_NODE = 1;
   _DT200_BOARD = 2;
   _DT200_DIs = 4;
   _DT200_NODES_PER_DI = 3;
   _DT200_DI_WIRE=1;
   _DT200_DI_BUS=2;
   _DT200_CLOCK_SRC=22;
   _DT200_CLOCK_DIV=23;
   _DT200_DAQ_MEM=24;
   _DT200_ACTIVE_CHAN=25;
   _DT200_TRIG_SRC=26;
   _DT200_POST_TRIG=27;
   _DT200_PRE_TRIG=28;

  _node = if_error(data(DevNodeRef(_nid,_DT200_NODE)), "");
  if (Len(_node) <= 0) {
     _node = 'local';
  }
  _status = MdsConnect(_node);
  if ((_status < 0) && (_node != 'local')) {
    Write(*,"Could not connect to "//_node);
    Abort();
  }
  _board = if_error(data(DevNodeRef(_nid,_DT200_BOARD)), DevError("Dt200 board must be specified"));

  _cmd = "reboot_dtaq_satelite "//_board;
  write(*, "about to /"//_cmd//"/");  
  MdsValue("spawn('"//_cmd//"')");
  return(1);
}
