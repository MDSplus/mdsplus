pro setenv_,envstring
case (!version.os) of
  'MacOS' :
  'vms' : 
  else : setenv,envstring
endcase
end


function MdsRoutinePrefix
;Descriptors or argc/argv convention?
;Shouldn't need to be changed for macos
;By the way, probably didn't need to port routines beginning with "Idl"
  if !version.os eq 'vms' then return,'' else return,'Idl'
end

function MdsGetAnsFn
  if (!version.os eq 'vms') then return,"GetAnswerInfo" else return,"IdlGetAnsInfo"
end

Pro MdsMemCpy,outvar,inptr,num
;Does pointer to input data need to be passed by value or reference?
;VMS & else, passed by value
;OSF by reference
  case !version.os of
    'vms' :  dummy = call_external('decc$shr','decc$memcpy',outvar,inptr,num,value=[0,1,1])
    'OSF' :  dummy = call_external(MdsIpImage(),MdsRoutinePrefix()+'memcpy',outvar,inptr,num,value=[0,0,1])
    else  :  dummy = call_external(MdsIpImage(),MdsRoutinePrefix()+'memcpy',outvar,inptr,num,value=[0,1,1])
  endcase
  return
end


function IsWindows
  return,!version.os eq 'windows' or !version.os eq 'Win32'
end

pro Mds$SendArg,sock,n,idx,arg
  s = size(arg)
  ndims = s(0)
  dims = lonarr(8)
  if ndims gt 0 then for i=1,ndims do dims(i-1) = s(i)
  dtype = s(ndims + 1)
;IDL_TYP_UNDEF           0
;IDL_TYP_BYTE            1
;IDL_TYP_INT             2
;IDL_TYP_LONG            3
;IDL_TYP_FLOAT           4
;IDL_TYP_DOUBLE          5
;IDL_TYP_COMPLEX         6
;IDL_TYP_STRING          7
;IDL_TYP_STRUCT          8
;IDL_TYP_DCOMPLEX        9
;IDL_TYP_PTR             10
;IDL_TYP_OBJREF          11
;IDL_TYP_UINT            12
;IDL_TYP_ULONG           13
;IDL_TYP_LONG64          14
;IDL_TYP_ULONG64         15
  dtypes =  [0,2,7,8,10,11,12,14,0,0,0,0,3,4,9,5]
  lengths = [0,1,2,4, 4, 8, 8, 1,0,0,0,0,2,4,8,8]
  length = lengths(dtype)
  dtype = dtypes(dtype)
  if dtype eq 14 then begin
    if (ndims gt 0) then begin
      argByVal = 0b 
      arg_saved = arg
      length = max(strlen(arg))
      arg = temporary(byte(arg))
      arg = temporary(reform(arg, n_elements(arg)))
      ispace = where(arg eq 0b,nspace)
      if (nspace gt 0) then arg[ispace]=32b
    endif else begin
      argByVal = 1b 
      length = strlen(arg)
    endelse
  endif else begin
    argByVal = 0b
  endelse
  x = call_external(MdsIPImage(),MdsRoutinePrefix()+'SendArg',sock,idx,dtype,n,length,ndims,dims,arg,value=[1b,1b,1b,1b,1b,1b,0b,argByVal])
  if (n_elements(arg_saved) gt 0) then arg=arg_saved
  return
end

pro mds$connect,host,status=status,quiet=quiet,port=port
  on_error,2
  mds$disconnect,/quiet
  if n_elements(port) ne 0 then begin
    setenv_,'mdsip='+strtrim(port,2)
  endif else if getenv('mdsip') eq '' then begin
    setenv_,'mdsip=8000'
  endif
  if (!version.release ne '5.0.3') then dummy = execute('!ERROR_STATE.MSG="About to connect"')
  sock = call_external(MdsIPImage(),MdsRoutinePrefix()+'ConnectToMds',host,value=[byte(!version.os ne 'windows')])
  sockmin=1l-(!version.os eq 'MacOS')
  if (sock ge sockmin) then begin
    status = 1
    x=execute('!MDS_SOCKET = sock')
  endif else begin
    if not keyword_set(quiet) then message,'Error connecting to '+host,/IOERROR
    status = 0
  endelse
  return
end

pro mds$disconnect,status=status,quiet=quiet
  status = 1
  sock = mds$socket(status=status,quiet=quiet)
  if status then begin
    status = call_external(MdsIPImage(),MdsRoutinePrefix()+'DisconnectFromMds',sock,value=[1b])
    if (status eq 0) then status = 1 else status = 0
    tmp = execute('!MDS_SOCKET = 0l')
  endif
  return
end


pro connect,host,_EXTRA=e
	mds$connect,host,_EXTRA=e
return
end

;*/


pro mdsconnect,host,status=status,quiet=quiet,port=port,socket=socket
  on_error,2
  if (not keyword_set(socket)) then $
    mdsdisconnect,/quiet
  if n_elements(port) ne 0 then begin
    setenv_,'mdsip='+strtrim(port,2)
  endif else if getenv('mdsip') eq '' then begin
    setenv_,'mdsip=8000'
  endif

  sock = call_external(MdsIPImage(),MdsRoutinePrefix()+'ConnectToMds',host,value=[byte(!version.os ne 'windows')])
  sockmin=1l-(!version.os eq 'MacOS')
  if (sock ge sockmin) then begin
    status = 1
    if not keyword_set(socket) then $
      x=execute('!MDS_SOCKET = sock') $
    else $
      socket = sock
  endif else begin
    if not keyword_set(quiet) then message,'Error connecting to '+host,/IOERROR
    status = 0
  endelse
  return
end

