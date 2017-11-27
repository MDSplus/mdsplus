;function mds$value,expression,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12, $
;             arg13,arg14,arg15,arg16,arg17,arg18,arg19,arg20,arg21,arg22,arg23,arg24,arg25, $
;             arg26,arg27,arg28,arg29,arg30,arg31,arg32,status=status,quiet=quiet
;  return,answer
;end

;+
; NAME:
;	MDSVALUE (Native and TCP/IP version) 
; PURPOSE:
;       Return the value of an MDSplus expression
; CATEGORY:
;	MDSPLUS/IP
; CALLING SEQUENCE:
;	answer = MDSVALUE(expression[,arg1,...,argn][,/QUIET][STATUS=stat])
; INPUT PARAMETERS:
;       expression = character string containing a valid MDSplus expression
; OPTIONAL INPUT PARAMETERS:
;       arg1,...,argn = values to substitute into the expression where
;                       "$" or "$n" placeholders indicate.
; Keywords:
;       QUIET = prevents IDL error if TCL command fails
;       STATUS = return status, low bit set = success
; OUTPUTS:
;       stat = return status, low bit set = success
; OUTPUTS:
;	None.
; COMMON BLOCKS:
;	None.
; SIDE EFFECTS:
;	None.
; RESTRICTIONS:
;	None.
; PROCEDURE:
;	Calls MDSplus shared image library IdlMdsValue.  Works with
;	both native access (Unix, VMS, Win32/WinNT) and client/server
;	(Unix, VMS, Win32/WinNT, MacOS).  
; MODIFICATION HISTORY:
;	 VERSION 1.0, CREATED BY T.W. Fredian, September 22,1992
;        VERSION 2.0, Jeff Schachter 1998.10.06 - added support for
;                     both client/server (MdsIpShr) and native access.
;                     Test is based on mdsIsClient() function
;	 VERSION 2.1, Bob Johnson 2010.08.08 - replaced use of execute
;                     so virtual machine works.  Uses function evaluate.
;-


function MdsValue,expression,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12,arg13,arg14,arg15,arg16,quiet=quiet,status=status,socket=socket

  forward_function mdsIsClient,mdsIdlImage,mds$socket,MdsRoutinePrefix,MdsIPImage,MdsGetAnsFN,evaluate
  MdsCheckArg,expression,type="STRING",name="expression"
  ;; note that MdsIpShr version of MdsValue had 32 arguments in addition
  ;; to expression

  vm_being_used = 0
  defsysv,'!gdl',exists=is_gdl
  if (not is_gdl) then begin
    if (!version.release ge '6.2' or $
      strmid(!version.release,0,1) ge '1') then vm_being_used = lmgr(/vm)
  endif
  if (mdsIsClient(socket=socket)) then begin

    sock = mds$socket(status=status,quiet=quiet, socket=socket)
    if not status then return,0
    n = n_params()
    Mds$SendArg,sock,n,0,expression
    if (vm_being_used) then begin
      for i=1,n-1 do x = evaluate('Mds$SendArg,sock,n,i,arg'+strtrim(i,2))
    endif else begin
      for i=1,n-1 do x = execute('Mds$SendArg,sock,n,i,arg'+strtrim(i,2))
    endelse
    dtype = 0b
    ndims = 0b
    dims = lonarr(8)
    numbytes = 0l
    answer = 0
    length = 0
    ansptr = 0l
;Not sure here... hope Mac acts like others, if not maybe try OSF way
    if !version.memory_bits eq 64 then ansptr = 0ll 
;;;;  status = call_external(MdsIPImage(),MdsRoutinePrefix()+'GetAnsInfo',sock,dtype,length,ndims,dims,numbytes,ansptr,value=[1,0,0,0,0,0,0])
;;; temporary fix Jeff Schachte 98.05.13
    status = call_external(MdsIPImage(),MdsGetAnsFn(),sock,dtype,length,ndims,dims,numbytes,ansptr,value=[1,0,0,0,0,0,0])
    if numbytes gt 0 then begin
      if dtype eq 14 then begin
        if ndims ne 0 then begin
          answer=bytarr([length,dims[0:ndims-1]])
;          s = 'answer = bytarr(length,'
;          for i=0,ndims-1 do s = s + 'dims(' + strtrim(i,2) + '),'
;          strput,s,')',strlen(s)-1
;          x = execute(s)
        endif else begin
          answer = bytarr(length)
        endelse
        MdsMemCpy,answer,ansptr,numbytes
        answer = strtrim(answer,2)
      endif else begin
        if ndims ne 0 then begin
          answer=bytarr(dims[0:ndims-1])
;          s = 'answer = bytarr('
;          for i=0,ndims-1 do s = s + 'dims(' + strtrim(i,2) + '),'
;          strput,s,')',strlen(s)-1
;          x = execute(s)
        endif else answer = 0b
        case dtype of
          2: answer = answer
          3: answer = fix(answer)
          4: answer = long(answer)
          5: answer = ulong64(answer)
          6: answer = byte(answer)
          7: answer = fix(answer)
          8: answer = long(answer)
          9: answer = long64(answer)
          10: answer = float(answer)
          11: answer = double(answer)
          12: answer = complex(answer)
          13: answer = dcomplex(answer)
          else: message,'Data type '+string(fix(dtype))+'  is not supported',/continue
        endcase
        MdsMemCpy,answer,ansptr,numbytes
	if (dtype eq 6) then begin
	  ; signed char data type, value is < 0 
	  i = where(answer gt 127b,n)
	  if (n gt 0) then begin
	    answer = fix(answer)
	    answer[i] = answer[i] - 256 
          endif
	endif
      endelse
      if not (status and 1) then begin
        if keyword_set(quiet) then message,answer,/noprint,/continue else message,answer,/continue
        answer = 0
      endif
    endif else begin
      if (status and 1) and (dtype eq 14) then answer = "" else begin
        if not keyword_set(quiet) then message,'Error evaluating expression',/continue
        status = 0
        answer = 0
      endelse
    endelse


  endif else begin
    old_except=!except
    !except=0
      anscreate_pre = bytarr(512)
      anscreate_post = bytarr(512)
      answer = '*'
      status = 0L
      cmd = 'status = call_external(value=[1b,bytarr('+strtrim(n_params()*2,2)+')],MdsIdlImage(),"IdlMdsValue",expression,anscreate_pre,anscreate_post'
      n = n_params() - 1
      for i=1,n do begin
        arg = 'arg'+strtrim(i,2)
        cmd = cmd + ',size('+arg+'),'+arg
      endfor
      cmd = cmd + ')'
      msg = ''
;      dummy = execute(cmd)
      value = [1b,bytarr(strtrim(n_params()*2,2))]
      case (n) of
      0: status = call_external(value=value,MdsIdlImage(),"IdlMdsValue",expression,anscreate_pre,anscreate_post)
      1: status = call_external(value=value,MdsIdlImage(),"IdlMdsValue",expression,anscreate_pre,anscreate_post,size(arg1),arg1)
      2: status = call_external(value=value,MdsIdlImage(),"IdlMdsValue",expression,anscreate_pre,anscreate_post,size(arg1),arg1,size(arg2),arg2)
      3: status = call_external(value=value,MdsIdlImage(),"IdlMdsValue",expression,anscreate_pre,anscreate_post,size(arg1),arg1,size(arg2),arg2,size(arg3),arg3)
      4: status = call_external(value=value,MdsIdlImage(),"IdlMdsValue",expression,anscreate_pre,anscreate_post,size(arg1),arg1,size(arg2),arg2,size(arg3),arg3,size(arg4),arg4)
      5: status = call_external(value=value,MdsIdlImage(),"IdlMdsValue",expression,anscreate_pre,anscreate_post,size(arg1),arg1,size(arg2),arg2,size(arg3),arg3,size(arg4),arg4,size(arg5),arg5)
      6: status = call_external(value=value,MdsIdlImage(),"IdlMdsValue",expression,anscreate_pre,anscreate_post,size(arg1),arg1,size(arg2),arg2,size(arg3),arg3,size(arg4),arg4,size(arg5),arg5,size(arg6),arg6)
      7: status = call_external(value=value,MdsIdlImage(),"IdlMdsValue",expression,anscreate_pre,anscreate_post,size(arg1),arg1,size(arg2),arg2,size(arg3),arg3,size(arg4),arg4,size(arg5),arg5,size(arg6),arg6,size(arg7),arg7)
      8: status = call_external(value=value,MdsIdlImage(),"IdlMdsValue",expression,anscreate_pre,anscreate_post,size(arg1),arg1,size(arg2),arg2,size(arg3),arg3,size(arg4),arg4,size(arg5),arg5,size(arg6),arg6,size(arg7),arg7,size(arg8),arg8)
      9: status = call_external(value=value,MdsIdlImage(),"IdlMdsValue",expression,anscreate_pre,anscreate_post,size(arg1),arg1,size(arg2),arg2,size(arg3),arg3,size(arg4),arg4,size(arg5),arg5,size(arg6),arg6,size(arg7),arg7,size(arg8),arg8,size(arg9),arg9)
      10: status = call_external(value=value,MdsIdlImage(),"IdlMdsValue",expression,anscreate_pre,anscreate_post,size(arg1),arg1,size(arg2),arg2,size(arg3),arg3,size(arg4),arg4,size(arg5),arg5,size(arg6),arg6,size(arg7),arg7,size(arg8),arg8,size(arg9),arg9,size(arg10),arg10)
      11: status = call_external(value=value,MdsIdlImage(),"IdlMdsValue",expression,anscreate_pre,anscreate_post,size(arg1),arg1,size(arg2),arg2,size(arg3),arg3,size(arg4),arg4,size(arg5),arg5,size(arg6),arg6,size(arg7),arg7,size(arg8),arg8,size(arg9),arg9,size(arg10),arg10,size(arg11),arg11)
      12: status = call_external(value=value,MdsIdlImage(),"IdlMdsValue",expression,anscreate_pre,anscreate_post,size(arg1),arg1,size(arg2),arg2,size(arg3),arg3,size(arg4),arg4,size(arg5),arg5,size(arg6),arg6,size(arg7),arg7,size(arg8),arg8,size(arg9),arg9,size(arg10),arg10,size(arg11),arg11,size(arg12),arg12)
      13: status = call_external(value=value,MdsIdlImage(),"IdlMdsValue",expression,anscreate_pre,anscreate_post,size(arg1),arg1,size(arg2),arg2,size(arg3),arg3,size(arg4),arg4,size(arg5),arg5,size(arg6),arg6,size(arg7),arg7,size(arg8),arg8,size(arg9),arg9,size(arg10),arg10,size(arg11),arg11,size(arg12),arg12,size(arg13),arg13)
      14: status = call_external(value=value,MdsIdlImage(),"IdlMdsValue",expression,anscreate_pre,anscreate_post,size(arg1),arg1,size(arg2),arg2,size(arg3),arg3,size(arg4),arg4,size(arg5),arg5,size(arg6),arg6,size(arg7),arg7,size(arg8),arg8,size(arg9),arg9,size(arg10),arg10,size(arg11),arg11,size(arg12),arg12,size(arg13),arg13,size(arg14),arg14)
      15: status = call_external(value=value,MdsIdlImage(),"IdlMdsValue",expression,anscreate_pre,anscreate_post,size(arg1),arg1,size(arg2),arg2,size(arg3),arg3,size(arg4),arg4,size(arg5),arg5,size(arg6),arg6,size(arg7),arg7,size(arg8),arg8,size(arg9),arg9,size(arg10),arg10,size(arg11),arg11,size(arg12),arg12,size(arg13),arg13,size(arg14),arg14,size(arg15),arg15)
      16: status = call_external(value=value,MdsIdlImage(),"IdlMdsValue",expression,anscreate_pre,anscreate_post,size(arg1),arg1,size(arg2),arg2,size(arg3),arg3,size(arg4),arg4,size(arg5),arg5,size(arg6),arg6,size(arg7),arg7,size(arg8),arg8,size(arg9),arg9,size(arg10),arg10,size(arg11),arg11,size(arg12),arg12,size(arg13),arg13,size(arg14),arg14,size(arg15),arg15,size(arg16),arg16)
      endcase
      dummy = 1
      if dummy then begin
        if status then begin
          if(vm_being_used) then begin
            dummy = evaluate(string(anscreate_pre))
            if dummy ne 'OK' then begin
              msg = 'error evaluating '+string(anscreate_pre)
              status = 0
            endif else begin
              dummy = call_external(MdsIdlImage(),"IdlGetAns",answer,value=[0b])
              if anscreate_post(0) ne 0 then begin
                dummy = evaluate(string(anscreate_post))
                if dummy ne 'OK' then begin
                  msg = 'error evaluating '+string(anscreate_post)
                  status = 0
                endif
              endif
            endelse
          endif else begin
            dummy = execute(string(anscreate_pre))
            if dummy then dummy = call_external(MdsIdlImage(),"IdlGetAns",answer,value=[0b])
            if anscreate_post(0) ne 0 then dummy = execute(string(anscreate_post))
            if ((size(answer,/struct)).type eq 7) then answer=strtrim(answer)
          endelse
        endif else begin
          if (status ne 0) then begin
            msg = mdsgetmsg(status)
          endif else begin
              msg = 'Error evaluating expression'
          endelse
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
    dummy=check_math()
    !except=old_except
  endelse
  return,answer
end
