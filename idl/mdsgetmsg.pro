function mdsgetmsg,status,quiet=quiet
  forward_function mdsvalue
  msg = mdsvalue('getmsg($)',status,quiet=quiet)
  sz = size(msg)
  if (sz[sz[0]+1] ne 7) then msg = '*** COULD NOT CALL MDSGETMSG ***'
  return,msg
end
