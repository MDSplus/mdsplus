; This function is used to confirm a valid connection has been established.
function mdsisclient,socket=socket_var

; For arg_present() to work, the optional keyword argument must be bound
; to a regular variable.  It will not work if bound to a system variable
; such as !MDS_SOCKET.  Refer to issue #2625 for more details.
if (arg_present(socket_var)) then $
  if (socket_var ge 0) then $
    return, 1 $
  else $
    return, 0

  ; If no connection specified, then assume using the most recent one.
  defsysv,'!MDS_SOCKET',exists=mdsClient
  if (mdsClient) then begin
    value= (!MDS_SOCKET ge 0)
    return,value

  ; If things have gone awry, indicate an INVALID_CONNECTION_ID
  endif else begin
    defsysv,'!MDS_SOCKET',-1
    return,0
  endelse
end
