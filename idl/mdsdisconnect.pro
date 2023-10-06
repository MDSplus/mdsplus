pro mdsdisconnect,status=status,quiet=quiet, socket=socket
  forward_function mds_keyword_set
  forward_function mds$socket, MdsIPImage
  image = MdsIPImage()
  status = 1
  if mds_keyword_set(socket=socket) then $
    sock = socket $
  else $
    sock = mds$socket(status=status,quiet=quiet)
  if status then begin
    ; IDL's AUTO_GLUE feature requires a C / C++ compiler on the system
    status = call_external(image,'DisconnectFromMds',sock,value=[1b], /AUTO_GLUE) 
    if (status eq 0) then status = 1 else status = 0
    if not mds_keyword_set(socket=socket) then $
      !MDS_SOCKET = -1l
  endif
  return
end

