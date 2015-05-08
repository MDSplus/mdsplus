public fun matrox__store(as_is _nid, optional _method)
{
  /* new thin client code
  _status = MdsConnect("CHARGE");
  if (_status <= 0) {
    Abort("Could not connect back to the data server");
  }
  _status = MdsValue("TreeOpen($, $)", $expt, $shot);
  if ((_status & 1) == 0) {
    Abort("Could not open cmod tree (thin client)");
  }
  end of thin client addtions */
  _num_frames = 0;
  _max_frames = DevNodeRef(_nid, 2);
  _times = zero(_max_frames, 0);
  _status = MatroxLib->MatroxGetTimes(val(_max_frames), ref(_times), ref(_num_frames));
  _times = _times[ 0 : _num_frames-1];
  _trigger = if_error(data(DevNodeRef(_nid, 16)), 0.0);
  if ((_status&1) && (_num_frames gt 0)) {
    _data = zero([640, 480, _num_frames], 0bu);
    for (_camera=0; _camera < 3; _camera++)
    {
      _camera_offset = _camera * 4 + 4;
      _camera_nid = DevHead(_nid) + _camera_offset;
      if (DevIsOn(_camera_nid))
      {
        _requested = DevNodeRef(_nid,_camera_offset+1);
        _status = MatroxLib->MatroxGetFrames(val(_camera),val(_num_frames), ref(_data));
        if (_status) {
          _requested = DATA(_requested);
          _out=_data[*,*,_requested];
          _tim = _times[_requested]*1e-3+_trigger;
          _signal = compile('make_signal(BUILD_WITH_UNITS((`_out), "Counts"), *, Build_with_units(`_tim, "Seconds"))');
          
          /* old thick ... */
           _status = TreeShr->TreePutRecord(val(_camera_nid),xd(_signal),val(0));
          /*  new thin
           _status = MdsValue("treeshr->TREE$PUT_RECORD($, xd($), $)", _camera_nid, _signal, 0);
           */
           if ((_status & 1) == 0) {
             Abort("could not write channel");
           } 
        }
      }
    }
  }
/*  new thin ....
  MdsDisconnect();
 */
  MatroxLib->MatroxFree();
  return(1);
}