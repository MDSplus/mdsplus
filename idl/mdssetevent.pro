pro mdssetevent, ev, quiet=quiet, status=status
  dummy = MdsValue('SETEVENT($)',ev, quiet=quiet, status=status)
end
