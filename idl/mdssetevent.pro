pro mdssetevent, ev, quiet=quiet, status=status, data=data
  if (n_elements(data) eq 0) then $
    data=' '
  dummy = MdsValue('SETEVENT($, $)',ev, data, quiet=quiet, status=status)
end
