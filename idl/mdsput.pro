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
  status = 0L
  putstat = 0L
  n = n_params()
  case (n) of
  2:  putstat=MdsValue('TreePut($,$)',node,expression,status=status,quiet=quiet)
  3:  putstat=MdsValue('TreePut($,$,$)',node,expression,arg1,status=status,quiet=quiet)
  4:  putstat=MdsValue('TreePut($,$,$,$)',node,expression,arg1,arg2,status=status,quiet=quiet)
  5:  putstat=MdsValue('TreePut($,$,$,$,$)',node,expression,arg1,arg2,arg3,status=status,quiet=quiet)
  6:  putstat=MdsValue('TreePut($,$,$,$,$,$)',node,expression,arg1,arg2,arg3,arg4,status=status,quiet=quiet)
  7:  putstat=MdsValue('TreePut($,$,$,$,$,$,$)',node,expression,arg1,arg2,arg3,arg4,arg5,status=status,quiet=quiet)
  8:  putstat=MdsValue('TreePut($,$,$,$,$,$,$,$)',node,expression,arg1,arg2,arg3,arg4,arg5,arg6,status=status,quiet=quiet)
  9:  putstat=MdsValue('TreePut($,$,$,$,$,$,$,$,$)',node,expression,arg1,arg2,arg3,arg4,arg5,arg6,arg7,status=status,quiet=quiet)
  10:  putstat=MdsValue('TreePut($,$,$,$,$,$,$,$,$,$)',node,expression,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,status=status,quiet=quiet)
  11:  putstat=MdsValue('TreePut($,$,$,$,$,$,$,$,$,$,$)',node,expression,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,status=status,quiet=quiet)
  12:  putstat=MdsValue('TreePut($,$,$,$,$,$,$,$,$,$,$,$)',node,expression,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,status=status,quiet=quiet)
  13:  putstat=MdsValue('TreePut($,$,$,$,$,$,$,$,$,$,$,$,$)',node,expression,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,status=status,quiet=quiet)
  14:  putstat=MdsValue('TreePut($,$,$,$,$,$,$,$,$,$,$,$,$,$)',node,expression,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12,status=status,quiet=quiet)
  15:  putstat=MdsValue('TreePut($,$,$,$,$,$,$,$,$,$,$,$,$,$,$)',node,expression,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12,arg13,status=status,quiet=quiet)
  16:  putstat=MdsValue('TreePut($,$,$,$,$,$,$,$,$,$,$,$,$,$,$,$)',node,expression,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12,arg13,arg14,status=status,quiet=quiet)
  17:  putstat=MdsValue('TreePut($,$,$,$,$,$,$,$,$,$,$,$,$,$,$,$,$)',node,expression,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12,arg13,arg14,arg15,status=status,quiet=quiet)
  18:  putstat=MdsValue('TreePut($,$,$,$,$,$,$,$,$,$,$,$,$,$,$,$,$,$)',node,expression,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12,arg13,arg14,arg15,arg16,status=status,quiet=quiet)
  endcase
;  lexpression = 'TreePut('
;  for i=0,n-1 do lexpression = lexpression + '$'+strtrim(i+1,2)+','
;  strput,lexpression,')',strlen(lexpression)-1
;  MdsValueExp = "putstat = MdsValue('"+lexpression+"',node,expression"
;  for i=0,n-3 do MdsValueExp = MdsValueExp + ',arg'+strtrim(i+1,2)
;  MdsValueExp = MdsValueExp + ",status=status,quiet=quiet)"
;  x = execute(MdsValueExp)
  sz = size(putstat)
  if (sz[sz[0]+1] eq 7) then begin
    sz=size(status)
    if (sz[sz[0]+1] eq 7) then status=0
  endif else status = putstat
  msg = strtrim(MdsValue('getmsg($)',/quiet,status),2)
  if not status then begin
    if keyword_set(quiet) then $
        message,msg,/continue,/noprint $
    else $
         message,msg,/continue
  endif
end
