pro setenv_,envstring
case (!version.os) of
  'MacOS' :
  'vms' : 
  else : setenv,envstring
endcase
end

Function MdsIPImage
;Path to shared library
  case !version.os of
    'vms' : return,'mdsipshr'
    'windows' : return,'mdsipshr'
	'Win32'	: return,'mdsipshr'
    'AIX' : return,'libMdsIpShr.lib'
    'IRIX' : return,'libMdsIpShr.so'
    'OSF' : return,'libMdsIpShr.so'
    'sunos' : return,'libMdsIpShr.so'
    'hp-ux' : begin
      a=getenv('MDS_SHLIB_PATH')
      if (a eq '') then a = '/usr/local/lib'
      return,a+'/libMdsIpShr.sl'
    end
    'MacOS': return,!dir+'libMdsIpShr.lib'
    'linux': return,'/home_ejet/ntcdemo/twf/mdsplus/shlib/libMdsIpShr.so'
    else  : message,'MDS is not supported on this platform',/IOERROR 
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

function mds$socket,quiet=quiet,status=status
  sockmin=1l-(!version.os eq 'MacOS')
  sock=sockmin-1
  defsysv,'!MDS_SOCKET',exists=old_sock
  if not old_sock then defsysv,'!MDS_SOCKET',sock else tmp = execute('sock=!MDS_SOCKET')
  if sock lt sockmin then begin
    status = 0
    if not keyword_set(quiet) then message,'Use MDS$CONNECT to connect to a host.',/continue
    return,0
  endif
  status = 1
  return,sock
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
  dtypes =  [0,2,7,8,10,11,12,14,0]
  lengths = [0,1,2,4,4,8,8,1,0]
  length = lengths(dtype)
  dtype = dtypes(dtype)
  if dtype eq 14 then begin
    if (n_elements(arg) gt 1) then message,'Argument to MDS$PUT must be a scalar string',/ioerror
    length = strlen(arg)
    argByVal = 1b 
;;;    if !version.os eq 'windows' then argByVal = 0b  ; removed as
;;;    part of CVS update by Jeff Schachter 1998.08.21
  endif else begin
    argByVal = 0b
  endelse
  x = call_external(MdsIPImage(),MdsRoutinePrefix()+'SendArg',sock,idx,dtype,n,length,ndims,dims,arg,value=[1b,1b,1b,1b,1b,1b,0b,argByVal])
  return
end

pro mds$connect,host,status=status,quiet=quiet,port=port
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


pro mdsconnect,host,status=status,quiet=quiet,port=port
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
    x=execute('!MDS_SOCKET = sock')
  endif else begin
    if not keyword_set(quiet) then message,'Error connecting to '+host,/IOERROR
    status = 0
  endelse
  return
end
