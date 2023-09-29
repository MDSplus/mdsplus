pro mdsdisconnect,status=status,quiet=quiet, socket=socket
  forward_function mds$socket, MdsIPImage
  image = MdsIPImage()
  status = 1
  ; The arg_present() only works if the optional keyword argument 
  ; is bound to a regular variable in the calling code.
  if arg_present(socket) then $
    sock = socket $
  else $
    sock = mds$socket(status=status,quiet=quiet)
  if status then begin
    status = call_external(image,'DisconnectFromMds',sock,value=[1b])
    if (status eq 0) then status = 1 else status = 0
    if not arg_present(socket) then $
      !MDS_SOCKET = -1l
  endif
  return
end

