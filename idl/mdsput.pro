;pro mds$put,node,expression,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12, $
;             arg13,arg14,arg15,arg16,arg17,arg18,arg19,arg20,arg21,arg22,arg23,arg24,arg25, $
;             arg26,arg27,arg28,arg29,arg30,arg31,arg32,status=status,quiet=quiet
;  return
;end



pro MdsPut,node,expression,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12,arg13,arg14,arg15,arg16,quiet=quiet,status=status
  ;; note that MdsIpShr version of MdsPut had 32 arguments in addition
  ;; to expression 

  forward_function mdsIsClient,mdsIdlImage,mds$socket,MdsRoutinePrefix,MdsIPImage,MdsGetAnsFn

  if (mdsIsClient()) then begin

    sock = mds$socket(status=status,quiet=quiet)
    if not status then return
    n = n_params()
    lexpression = 'mdslib->mds$put('
    for i=0,n-1 do lexpression = lexpression + 'descr($'+strtrim(i+1,2)+'),'
    strput,lexpression,')',strlen(lexpression)-1
    mds$sendarg,sock,n+1,0,lexpression
    mds$sendarg,sock,n+1,1,node
    mds$sendarg,sock,n+1,2,expression
    for i=1,n-2 do begin
      argstr = 'arg'+strtrim(i,2)
      x = execute('if (n_elements('+argstr+') eq 0) then '+argstr+'=0 & mds$sendarg,sock,n+1,i+2,'+argstr)
;;      x = execute('mds$sendarg,sock,n+1,i+2,arg'+strtrim(i,2))
    endfor
    dtype = 0b
    ndims = 0b
    dims = lonarr(8)
    numbytes = 0l
    answer = 0
    length = 0
    ansptr = 0l
;;;;  status = call_external(MdsIPImage(),MdsRoutinePrefix()+'GetAnsInfo',sock,dtype,length,ndims,dims,numbytes,ansptr,value=[1,0,0,0,0,0,0])
;;; temporary fix by Jeff Schachter 98.05.13
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
          else: message,'Data type '+string(dtype)+'  is not supported',/ioerror
        endcase
        MdsMemCpy,answer,ansptr,numbytes
      endelse
      status = answer
    endif else begin
      if not keyword_set(quiet) then message,'Error evaluating expression',/IOERROR
      answer = -1
    endelse

  endif else begin

    if (!VERSION.OS eq 'vms') then begin

      cmd = 'mds$put,node,expression'
      for i=1,n_params()-2 do cmd=cmd+',arg'+strtrim(i,2)
      cmd=cmd+',quiet=quiet,status=status'
      dummy = execute(cmd)
      

    endif else begin
        
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
    endelse
  endelse 
  
end
