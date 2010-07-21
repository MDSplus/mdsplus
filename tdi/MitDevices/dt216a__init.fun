public fun DT216A__INIT(as_is _nid, optional _method)
{
   _DT200_NODE = 1;
   _DT200_BOARD = 2;
   _DT200_RANGES = 4;
   _DT200_STATUS_CMDS = 5;
   _DT200_BOARD_STATUS = 6;
   _DT200_SEG_LENGTH = 7;
   _DT200_DIs = 8;
   _DT200_NODES_PER_DI = 3;
   _DT200_DI_WIRE=1;
   _DT200_DI_BUS=2;
   _DT200_CLOCK_SRC=26;
   _DT200_CLOCK_DIV=27;
   _DT200_DAQ_MEM=28;
   _DT200_ACTIVE_CHAN=29;
   _DT200_TRIG_SRC=30;
   _DT200_POST_TRIG=31;
   _DT200_PRE_TRIG=32;
   _DT200_AI_CHANS = 33;
   _DT200_NODES_PER_AI = 6;
   _DT200_AI_START = 1;
   _DT200_AI_END = 2;
   _DT200_AI_INC = 3;
   _DT200_AI_COEFFS = 4;
   _DT200_AI_VIN = 5;


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

  MdsValue("Dt196Reset($)", _board);


  for (_di=0; _di<6; _di++)
  {
    _line = "D"//TEXT(_di,1);
    _wire=if_error(data(DevNodeRef(_nid, _DT200_DIs+_di*_DT200_NODES_PER_DI+_DT200_DI_WIRE)), " ");
    _bus=if_error(data(DevNodeRef(_nid, _DT200_DIs+_di*_DT200_NODES_PER_DI+_DT200_DI_BUS)), " ");
    MdsValue("Dt196SetRoute($,$,$,$)", _board, _line, _wire, _bus);
  }
              
  _memSize = if_error(data(DevNodeRef(_nid,_DT200_DAQ_MEM)), 512);
  _activeChans = if_error(data(DevNodeRef(_nid,_DT200_ACTIVE_CHAN)), 96);
  _activeChans = min(max(_activeChans, 0), 96);
  if (_activeChans == 0) {
    Write(*, "No active channels aborting...");
    Abort();
  }

  _chansize = _memSize*1024*1024 / 2 / _activeChans;
  _preTrig = if_error(data(DevNodeRef(_nid, _DT200_PRE_TRIG))*1024, 0); 
  _postTrig = if_error(data(DevNodeRef(_nid, _DT200_POST_TRIG))*1024, 0); 
  _postTrig = ((_postTrig+_preTrig) < _chansize) ? _postTrig : _chanSize-_preTrig-1;  

  _trigSource = if_error(data(DevNodeRef(_nid, _DT200_TRIG_SRC)), "DI3");

  _clockSource = if_error(data(DevNodeRef(_nid, _DT200_CLOCK_SRC)), '');
  _clockFreq = if_error(data(DevNodeRef(_nid, _DT200_CLOCK_DIV)), 0);

  for (_chan=0; _chan < 16; _chan++) {
    _chan_offset = _chan * _DT200_NODES_PER_AI + _DT200_AI_CHANS;
    _vin = if_error(data(DevNodeRef(_nid, _chan_offset+_DT200_AI_VIN)),10);
    _vinstr = (_vin < 2.6) ? fformat(_vin, 3,1) : text(int(_vin));
    MdsValue('Dt200WriteMaster($, $, 1)', _board, 'set.vin '//_chan+1//' '//_vinstr);
  }

  MdsValue('Dt216AInit($,$,$,$,$,$,$)', _board, _activeChans, _trigSource, _clockSource, _clockFreq, _preTrig, _postTrig);
  
  return(1);
}
