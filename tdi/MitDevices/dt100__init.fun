public fun DT100__INIT(as_is _nid, optional _method)
{
  _modes = ['SOFT_TRANSIENT', 'GATED_TRANSIENT', 'GATED_CONTINOUS'];
  _node = DevNodeRef(_nid,1);
  _node = if_error(data(_node), "");
  if (Len(_node) > 0) {
    _status = MdsConnect(_node);
    if (! _status) {
      Write(*,"Could not connect to "//_node);
      Abort();
    }
  }
  _board = data(DevNodeRef(_nid,2));
  _active_chans = DevNodeRef(_nid,7);
  _active_chans = min(max(_active_chans, 0), 32);
  if (_active_chans == 0) {
    Write(*, "No active channels aborting...");
    Abort();
  }
  _mem_size = DevNodeRef(_nid, 6);
  _chansize = _mem_size*1024*1024 / _active_chans;
  _chansize = if_error(min(_chansize, DevNodeRef(_nid, 8)*1024), _chansize);

  _cmd = 'Dt100Init('//_board//','//_active_chans//','//DevNodeRef(_nid, 9)//','//_chansize//')';
  write(*, _cmd);
  MdsValue(_cmd);
  return(1);
}
