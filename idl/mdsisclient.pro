function mdsisclient
  defsysv,'!MDS_SOCKET',exists=mdsClient
  if (mdsClient) then begin
    dummy = execute('value= (!MDS_SOCKET ge 0)') 
    return,value
  endif else begin
    defsysv,'!MDS_SOCKET',-1
    return,0
  endelse
end
