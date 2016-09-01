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
    _stat = 0;
    while (_stat == 0)
    {
      _host = _queues[long(random()*size(_queues))];
      write(*,"Trying ",_host);
      _stat = mdsconnect(_host);
    }
  }
  else
    _stat=mdsconnect(_host);
  if (_stat > 0)
  {
    _stat=mdsvalue('tcl("spawn/nowait unix_submit '//_file//' '//_shot//'")');
  } else {
    tcl('spawn unix_submit '//_file//' '//_shot//' CONNECTFAIL '//_host);
  }
  return(_stat);
}
