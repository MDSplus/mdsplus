function MdsValue,expression,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12,arg13,arg14,arg15,arg16,quiet=quiet,status=status
  anscreate_pre = bytarr(512)
  anscreate_post = bytarr(512)
  answer = '*'
  status = 0L
  cmd = 'status = call_external(value=[1b,bytarr('+strtrim(n_params()*2,2)+')],MdsIdlImage(),"IdlMdsValue",expression,anscreate_pre,anscreate_post'
  args = n_params() - 1
  for i=1,args do begin
    arg = 'arg'+strtrim(i,2)
    cmd = cmd + ',size('+arg+'),'+arg
  endfor
  cmd = cmd + ')'
  msg = ''
  dummy = execute(cmd)
  if dummy then begin
    if status then begin
      dummy = execute(string(anscreate_pre))
      if dummy then dummy = call_external(MdsIdlImage(),"IdlGetAns",answer,value=[0b])
      if anscreate_post[0] ne 0 then dummy = execute(string(anscreate_post))
    endif else begin
      msg = 'Error evaluating expression'
    endelse
  endif else begin
    msg = 'Error in call external'
  endelse
  if not status then begin
    if keyword_set(quiet) then $
      message,msg,/continue,/noprint $
    else $
      message,msg,/continue
  endif
  return,answer
end
