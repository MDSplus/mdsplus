function mdsgetmsg,status,quiet=quiet
  forward_function mdsvalue
  help,calls=x
  i = where(strpos(x, 'MDSGETMSG') ge 0,n)
  if (n lt 2) then begin
    msg = mdsvalue('getmsg($)',status,/quiet,status=stat)
    if (not(stat)) then msg='Cannot evaluate MDSplus expressions - is MDSplus properly installed?'
  endif else msg = '*'
  return,msg
end
