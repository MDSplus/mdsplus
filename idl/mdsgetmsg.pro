function mdsgetmsg,status,quiet=quiet
  forward_function mdsvalue
  help,calls=x
  i = where(strpos(x, 'MDSGETMSG') ge 0,n)
  if (n eq 0) then begin
    msg = mdsvalue('getmsg($)',status,quiet=quiet)
    sz = size(msg)
  endif else msg = 'Cannot evaluate MDSplus expressions - is MDSplus properly installed?'
  return,msg
end
