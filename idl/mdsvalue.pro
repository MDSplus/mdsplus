;function mds$value,expression,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12, $
;             arg13,arg14,arg15,arg16,arg17,arg18,arg19,arg20,arg21,arg22,arg23,arg24,arg25, $
;             arg26,arg27,arg28,arg29,arg30,arg31,arg32,status=status,quiet=quiet
;  return,answer
;end



function MdsValue,expression,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12,arg13,arg14,arg15,arg16,quiet=quiet,status=status

  forward_function mdsIsClient,mdsIdlImage,mds$socket,MdsRoutinePrefix,MdsIPImage,MdsGetAnsFN

  ;; note that MdsIpShr version of MdsValue had 32 arguments in addition
  ;; to expression 

  if (mdsIsClient()) then begin

    sock = mds$socket(status=status,quiet=quiet)
    if not status then return,0
    n = n_params()
    Mds$SendArg,sock,n,0,expression
    for i=1,n-1 do x = execute('Mds$SendArg,sock,n,i,arg'+strtrim(i,2))
    dtype = 0b
    ndims = 0b
    dims = lonarr(8)
    numbytes = 0l
    answer = 0
    length = 0
    ansptr = 0l
;Not sure here... hope Mac acts like others, if not maybe try OSF way
    if !version.os eq 'OSF' then ansptr = lonarr(2)
;;;;  status = call_external(MdsIPImage(),MdsRoutinePrefix()+'GetAnsInfo',sock,dtype,length,ndims,dims,numbytes,ansptr,value=[1,0,0,0,0,0,0])
;;; temporary fix Jeff Schachte 98.05.13
    status = call_external(MdsIPImage(),MdsGetAnsFn(),sock,dtype,length,ndims,dims,numbytes,ansptr,value=[1,0,0,0,0,0,0])

    if numbytes gt 0 then begin
      if dtype eq 14 then begin
        if ndims ne 0 then begin
          s = 'answer = bytarr(length,'
          for i=0,ndims-1 do s = s + 'dims(' + strtrim(i,2) + '),'
          strput,s,')',strlen(s)-1
          x = execute(s)
        endif else begin
          answer = bytarr(length)
        endelse
        MdsMemCpy,answer,ansptr,numbytes
        answer = string(answer)
      endif else begin
        if ndims ne 0 then begin
          s = 'answer = bytarr('
          for i=0,ndims-1 do s = s + 'dims(' + strtrim(i,2) + '),'
          strput,s,')',strlen(s)-1
          x = execute(s)
        endif else answer = 0b
        case dtype of
          2: answer = answer
          3: answer = fix(answer)
          4: answer = long(answer)
          7: answer = fix(answer)
          8: answer = long(answer)
          10: answer = float(answer)
          11: answer = double(answer)
          12: answer = complex(answer)
          else: message,'Data type '+string(dtype)+'  is not supported',/continue
        endcase
        MdsMemCpy,answer,ansptr,numbytes
      endelse
      if not (status and 1) then begin
        if keyword_set(quiet) then message,answer,/noprint,/continue else message,answer,/continue
        answer = 0
      endif
    endif else begin
      if not keyword_set(quiet) then message,'Error evaluating expression',/continue
      status = 0
      answer = 0
    endelse
    
    
  endif else begin

    if (!VERSION.OS eq 'vms') then begin

      cmd = 'answer = mds$value(expression'
      for i=1,n_params()-1 do cmd=cmd+',arg'+strtrim(i,2)
      cmd = cmd+',quiet=quiet,status=status)'
      dummy = execute(cmd)

    endif else begin
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
          if anscreate_post(0) ne 0 then dummy = execute(string(anscreate_post))
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
    endelse
  endelse

  return,answer
end
