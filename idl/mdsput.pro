pro MdsPut,node,expression,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12,arg13,arg14,arg15,arg16,quiet=quiet,status=status
  status = 0L
  cmd = 'status = call_external(value=[1b,1b,bytarr('+strtrim(n_params()*2,2)+')],MdsIdlImage(),"IdlMdsPut",node,expression'
  args = n_params() - 2
  for i=1,args do begin
    arg = 'arg'+strtrim(i,2)
    cmd = cmd + ',size('+arg+'),'+arg
  endfor
  cmd = cmd + ')'
  msg = ''
  dummy = execute(cmd)
  if dummy then begin
    if not status then begin
      msg = 'Error evaluating expression'
    endif
  endif else begin
    msg = 'Error in call external'
  endelse
  if not status then begin
    if keyword_set(quiet) then $
      message,msg,/continue,/noprint $
    else $
      message,msg,/continue
  endif
  return
end
