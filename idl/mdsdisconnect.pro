pro mdsdisconnect,status=status,quiet=quiet
  status = 1
  sock = mds$socket(status=status,quiet=quiet)
  if status then begin
    status = call_external(MdsIPImage(),MdsRoutinePrefix()+'DisconnectFromMds',sock,value=[1b])
    if (status eq 0) then status = 1 else status = 0
    tmp = execute('!MDS_SOCKET = -1l')
  endif
  return
end

