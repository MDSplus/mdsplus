function mdsisclient,socket=socket
if (keyword_set(socket)) then $
  if (socket ge 0) then $
    return, 1 $
  else $
    return, 0
  defsysv,'!MDS_SOCKET',exists=mdsClient
  if (mdsClient) then begin
    value= (!MDS_SOCKET ge 0)
    return,value
  endif else begin
    defsysv,'!MDS_SOCKET',-1
    return,0
  endelse
end
