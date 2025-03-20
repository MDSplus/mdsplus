public fun remote_submit_helper(_host,_file,_shot)
{

/*    Connect to remote mdsip server and issue a unix_submit on that server.

  format: remote_submit(_host,_file,_shot)

  _host = internet host name of remote system or "any" to randomly select a server
          from a list of execution servers. To use the "any" feature you must
          have a site-specific tdi function in the MDS_PATH called
          remote_submit_queues which takes no arguments and returns an array of
          host names.
  _file = name of the executable image or script. Must be present on the remote
          system.

  _shot = shot number passed to unix_submit.

*/

  if (_host == "any")
  {
    _queues = remote_submit_queues();

    _start = cvttime();
    _elapsed = 0;
    _max_seconds = 300;
    _invalid_id = -1;

    /* Note that mdsconnect() returns a connection id, not a status code */
    _stat = _invalid_id;
    while ((_stat == _invalid_id) AND (_elapsed < _max_seconds))
    {
      _host = _queues[long(random()*size(_queues))];
      write(*,"Trying ",_host);
      _stat = mdsconnect(_host);
      _elapsed = cvttime() - _start;
    }
  }
  else
    _stat=mdsconnect(_host);
  if (_stat > _invalid_id)
  {
    _stat=mdsvalue('tcl("spawn/nowait unix_submit '//_file//' '//_shot//'")');
  } else {
    tcl('spawn unix_submit '//_file//' '//_shot//' CONNECTFAIL '//_host);
  }
  return(_stat);
}
