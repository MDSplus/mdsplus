public fun remote_submit(in _file, optional in _host, optional in _shot)
{
  if (!present(_host)) _host="any";
  if (!present(_shot)) _shot=$shot;
  tcl('spawn/nowait remote_submit '//_host//' '//_file//' '//_shot);
  return(0);
}
