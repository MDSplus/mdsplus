public fun remote_submit_helper(_host,_file,_shot)
{

/*    Connect to remote mdsip server and issue a unix_submit on that server.

  format: remote_submit(_host,_file,_shot)

  _host = internet host name of remote system or "any" to cycle through
          list of execution servers. To use the "any" feature you must
          have a site-specific tdi function in the MDS_PATH called
          remote_submit_queues which takes no arguments and returns an array of
          host names. If the any feature is used, an index into the queues is
          saved in the file: /tmp/remote_submit_idx
  _file = name of the executable image or script. Must be present on the remote
          system.

  _shot = shot number passed to unix_submit.

*/

  if (_host == "any")
  {
    _lun=fopen("/tmp/remote_submit_idx","r");
    if (_lun == "")
      _idx=-1;
    else
    {
      _idx=execute(read(_lun));
      fclose(_lun);
    }
    _idx += 1;
    _queues = remote_submit_queues();
    if (_idx >= size(_queues))
      _idx = 0;
    _first_queue=_idx;
    _stat = 0;
    while (_stat == 0)
    {
      _host = _queues[_idx];
      write(*,"_host=",_host);
      _stat = mdsconnect(_host);
      if (_stat == 0)
      {
        _idx += 1;
        if (_idx > size(_queues))
          _idx = 0;
        if (_first_queue == _idx)
          return(0);
      }
    }
    _lun=fopen("/tmp/remote_submit_idx","w");
    write(_lun,_idx);
    fclose(_lun);
  }
  else
    _stat=mdsconnect(_host);
  if (_stat > 0)
  {
    _stat=mdsvalue('tcl("spawn/nowait unix_submit '//_file//' '//_shot//'")');
  }
  return(_stat);
}
