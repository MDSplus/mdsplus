function mdsgetmsg,status,quiet=quiet
  forward_function mdsvalue
  return,mdsvalue('getmsg($)', status,quiet=quiet)
end
