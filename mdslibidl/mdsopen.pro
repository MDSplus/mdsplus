pro MdsOpen,tree,shot,quiet=quiet,status=status
  status=call_external(MdsIdlImage(),'IdlMdsOpen',strtrim(tree,2),long(shot),value=[1b,1b])
  if not status then begin
    msg = 'Error opening tree '+strtrim(tree,2)+' shot '+strtrim(shot,2)
    if keyword_set(quiet) then $
      message,msg,/continue,/noprint $
    else $
      message,msg,/continue
  endif
end
