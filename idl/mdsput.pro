;pro mds$put,node,expression,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12, $
;             arg13,arg14,arg15,arg16,arg17,arg18,arg19,arg20,arg21,arg22,arg23,arg24,arg25, $
;             arg26,arg27,arg28,arg29,arg30,arg31,arg32,status=status,quiet=quiet
;  return
;end



pro MdsPut,node,expression,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12,arg13,arg14,arg15,arg16,$
                status=status,quiet=quiet
  ;; note that MdsIpShr version of MdsPut had 32 arguments in addition
  ;; to expression 

  forward_function MdsValue
  status = 0
  n = n_params()
  lexpression = 'TreePut('
  for i=0,n-1 do lexpression = lexpression + '$'+strtrim(i+1,2)+','
  strput,lexpression,')',strlen(lexpression)-1
  MdsValueExp = "status = MdsValue('"+lexpression+"',node,expression"
  for i=0,n-3 do MdsValueExp = MdsValueExp + ',arg'+strtrim(i+1,2)
  MdsValueExp = MdsValueExp + ",status=status,quiet=quiet)"
  x = execute(MdsValueExp)
  msg = MdsValue('getmsg($)',/quiet,status)
  if not status then begin
    if keyword_set(quiet) then $
        message,msg,/continue,/noprint $
    else $
         message,msg,/continue
  endif
end
