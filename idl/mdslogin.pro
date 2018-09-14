;+
; NAME:
;	MdsLogin
; PURPOSE:
;	Login to an MDSplus server which permits user logins
; CATEGORY:
;	MDSPLUS
; CALLING SEQUENCE:
;	MdsLogin,username,password
; INPUT PARAMETERS:
;	node = Node specification.
; Keywords:
;       QUIET = prevents IDL error if TCL command fails
;       STATUS = return status, low bit set = success
; OUTPUTS:
;       istat = return status, low bit set = success
; COMMON BLOCKS:
;	None.
; SIDE EFFECTS:
;	None.
; RESTRICTIONS:
;	None.
; PROCEDURE:
;	Straightforward.  Makes a call to MDSplus shared image library
;       procedure MdsLogin and checks for errors.
; MODIFICATION HISTORY:
;	 VERSION 1.0, CREATED BY T.W. Fredian, Oct 22,1999
;-

pro MdsLogin,user,password,quiet=quiet,status=status

  forward_function mdsIsClient,mdsIdlImage,mds$socket,MdsIPImage,IsWindows

  if (mdsIsClient()) then begin

    ON_ERROR,2                  ;RETURN TO CALLER IF ERROR
    sock = mds$socket(quiet=quiet,status=status)
    if not status then return
    status = call_external(MdsIPImage(),'MdsLogin',sock,string(user),string(password),value=[1b,byte([1,1] * (not IsWindows()))])
    if not status then begin
      if keyword_set(quiet) then message,mdsgetmsg(status,quiet=quiet),/continue,/noprint $
                            else message,mdsgetmsg(status,quiet=quiet),/continue
    endif

  endif else begin
    if keyword_set(quiet) then $
      message,"Must be connected to remote before logging in",/continue,/noprint $
    else $
      message,"Must be connected to remote before logging in",/continue
    status = 0
  endelse
end
