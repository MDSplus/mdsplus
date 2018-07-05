pro setenv_,envstring
case (!version.os) of
  'MacOS' :
  else : setenv,envstring
endcase
end

function mds$socket,quiet=quiet,status=status,socket=socket
  status = 1
  sockmin=1l-(!version.os eq 'MacOS')
  sock=sockmin-1
  if (keyword_set(socket)) then $
      if (socket ge sockmin) then $
          return, socket
  defsysv,'!MDS_SOCKET',exists=old_sock
  if not old_sock then defsysv,'!MDS_SOCKET',sock else sock=!MDS_SOCKET
  if sock lt sockmin then begin
    status = 0
    if not keyword_set(quiet) then message,'Use MDS$CONNECT to connect to a host.',/continue
    return,0
  endif
  status = 1
  return,sock
end

Function MdsIPImage
;Path to shared library
  case !version.os of
    'windows' : return,'mdsipshr'
	'Win32'	: return,'mdsipshr'
    'AIX' : return,'libMdsIpShr.a'
    'IRIX' : return,'libMdsIpShr.so'
    'OSF' : return,'libMdsIpShr.so'
    'sunos' : return,'libMdsIpShr.so'
    'hp-ux' : begin
              if getenv('MDS_SHLIB_PATH') eq '' then setenv_,'MDS_SHLIB_PATH=/usr/local/lib'
              return,getenv('MDS_SHLIB_PATH')+'/libMdsIpShr.sl'
              end
    'MacOS': return,!dir+'libMdsIpShr.lib'
    'linux': return,'libMdsIpShr.so'
    'darwin': return,'libMdsIpShr.dylib'
    else  : message,'MDS is not supported on this platform',/IOERROR 
  endcase
end

Pro MdsMemCpy,outvar,inptr,num
;Does pointer to input data need to be passed by value or reference?
;else, passed by value
;OSF by reference
  case !version.os of
    'OSF' :  dummy = call_external(MdsIpImage(),'Idlmemcpy',outvar,inptr,num,value=[0,0,1])
    else  :  dummy = call_external(MdsIpImage(),'Idlmemcpy',outvar,inptr,num,value=[0,1,1])
  endcase
  return
end


function IsWindows
  return,!version.os eq 'windows' or !version.os eq 'Win32'
end

pro Mds$SendArg,sock,n,idx,arg
  s = size(arg)
  ndims = s[0]
  dims = lonarr(8)
  if ndims gt 0 then for i=1,ndims do dims[i-1] = s[i]
  dtype = s[ndims + 1]
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
  length = lengths[dtype]
  dtype = dtypes[dtype]
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
  x = call_external(MdsIPImage(),'IdlSendArg',sock,idx,dtype,n,length,ndims,dims,arg,value=[1b,1b,1b,1b,1b,1b,0b,argByVal])
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
  if (!version.release ne '5.0.3') then !ERROR_STATE.MSG="About to connect"
  sock = call_external(MdsIPImage(),'IdlConnectToMds',host,value=[byte(!version.os ne 'windows')])
  sockmin=1l-(!version.os eq 'MacOS')
  if (sock ge sockmin) then begin
    status = 1
    !MDS_SOCKET = sock
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
    status = call_external(MdsIPImage(),'IdlDisconnectFromMds',sock,value=[1b])
    if (status eq 0) then status = 1 else status = 0
    !MDS_SOCKET = 0l
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

  sock = call_external(MdsIPImage(),'IdlConnectToMds',host,value=[byte(!version.os ne 'windows')])
  sockmin=1l-(!version.os eq 'MacOS')
  if (sock ge sockmin) then begin
    status = 1
    if not keyword_set(socket) then $
      !MDS_SOCKET = sock $
    else $
      socket = sock
  endif else begin
    if not keyword_set(quiet) then message,'Error connecting to '+host,/IOERROR
    status = 0
  endelse
  return
end

