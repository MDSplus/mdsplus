pro MdsClose,tree,shot,quiet=quiet,status=status
  if n_params() gt 1 then $
    status=call_external(MdsIdlImage(),'IdlMdsClose',strtrim(tree,2),long(shot),value=[1b,1b]) $
  else $
    status=call_external(MdsIdlImage(),'IdlMdsClose',0L,value=[1b])
  if not status then begin
    msg = 'Error closing tree '+strtrim(tree,2)+' shot '+strtrim(shot,2)
    if keyword_set(quiet) then $
      message,msg,/continue,/noprint $
    else $
      message,msg,/continue
  endif
end
