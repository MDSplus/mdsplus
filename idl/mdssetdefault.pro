pro MdsSetDefault,node,quiet=quiet,status=status
  status=call_external(MdsIdlImage(),'IdlMdsSetDefault',strtrim(node,2),value=[1b])
  if not status then begin
    msg = 'Error setting default to '+strtrim(node,2) 
    if keyword_set(quiet) then $
      message,msg,/continue,/noprint $
    else $
      message,msg,/continue
  endif
end
