function mdsisclient,socket=socket
  forward_function mds_keyword_set

  ; If optional socket provided, use it
  if (mds_keyword_set(socket=socket)) then $
   if (socket ge 0) then $
     return, 1 $
   else $
     return, 0

  ; Otherwise use the last socket  
  defsysv,'!MDS_SOCKET',exists=mdsClient
  if (mdsClient) then begin
    value= (!MDS_SOCKET ge 0)
    return,value

  ; Went awry, so return INVALID_CONNECTION_ID
  endif else begin
    defsysv,'!MDS_SOCKET',-1
    return,0
  endelse
end
