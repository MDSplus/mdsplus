pro mdsdisconnect,status=status,quiet=quiet, socket=socket
  forward_function mds$socket, MdsIPImage
  image = MdsIPImage()
  status = 1
  if keyword_set(socket) then $
    sock = socket $
  else $
    sock = mds$socket(status=status,quiet=quiet)
  if status then begin
    status = call_external(image,'DisconnectFromMds',sock,value=[1b])
    if (status eq 0) then status = 1 else status = 0
    if not keyword_set(socket) then $
      !MDS_SOCKET = -1l
  endif
  return
end

