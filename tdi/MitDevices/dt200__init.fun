public fun DT200__INIT(as_is _nid, optional _method)
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

  MdsValue("Dt200Reset($)", _board);
  for (_di=0; _di<6; _di++)
  {
    _line = "DI"//TEXT(_di,1);
    _wire=if_error(data(DevNodeRef(_nid, _DT200_DIs+_di*_DT200_NODES_PER_DI+_DT200_DI_WIRE)), " ");
    _bus=if_error(data(DevNodeRef(_nid, _DT200_DIs+_di*_DT200_NODES_PER_DI+_DT200_DI_BUS)), " ");
    MdsValue("Dt200SetRoute($,$,$,$)", _board, _line, _wire, _bus);
  }
              
  _memSize = if_error(data(DevNodeRef(_nid,_DT200_DAQ_MEM)), 64);
  _activeChans = if_error(data(DevNodeRef(_nid,_DT200_ACTIVE_CHAN)), 32);
  _activeChans = min(max(_activeChans, 0), 32);
  if (_activeChans == 0) {
    Write(*, "No active channels aborting...");
    Abort();
  }

  _chansize = _memSize*1024*1024 / 2 / _activeChans;
  _preTrig = if_error(data(DevNodeRef(_nid, _DT200_PRE_TRIG))*1024, 0); 
  _postTrig = if_error(data(DevNodeRef(_nid, _DT200_POST_TRIG))*1024, 0); 
  _postTrig = ((_postTrig+_preTrig) < _chansize) ? _postTrig : _chanSize-_preTrig-1;  

  _trigSource = if_error(data(DevNodeRef(_nid, _DT200_TRIG_SRC)), "DI1");

  _clockSource = if_error(data(DevNodeRef(_nid, _DT200_CLOCK_SRC)), '');
  _clockFreq = if_error(data(DevNodeRef(_nid, _DT200_CLOCK_DIV)), 0);

  MdsValue('Dt200Init($,$,$,$,$,$,$)', _board, _activeChans, _trigSource, _clockSource, _clockFreq, _preTrig, _postTrig);

  /* now arm the hardware */
  _status = MdsValue('Dt200WriteMaster($,"set.Arm", 1)', _board);
  if (_status == "/bin/sh: set.Arm: not found")
    MdsValue('Dt200WriteMaster($,"setArm")', _board);
  
  return(1);
}
