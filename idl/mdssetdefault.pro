;+
; NAME:
;	MdsSetDefault
; PURPOSE:
;	Set default to a node in an MDSplus tree (native and TCP/IP version)
; CATEGORY:
;	MDSPLUS
; CALLING SEQUENCE:
;	MdsSetDefault,NODE
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
;       procedure MDS$SET_DEFAULT and checks for errors.
; MODIFICATION HISTORY:
;	 VERSION 1.0, CREATED BY T.W. Fredian, April 22,1991
;        VERSION 2.0, Jeff Schachter 1998.10.06 - added support for
;                     both client/server (MdsIpShr) and native access.
;                     Test is based on mdsIsClient() function
;-

pro MdsSetDefault,node,quiet=quiet,status=status

  forward_function mdsIsClient,mdsIdlImage,mds$socket,MdsRoutinePrefix,MdsIPImage,IsWindows

  if (mdsIsClient()) then begin

    ON_ERROR,2                  ;RETURN TO CALLER IF ERROR
    sock = mds$socket(quiet=quiet,status=status)
    if not status then return
    status = call_external(MdsIPImage(),MdsRoutinePrefix()+'MdsSetDefault',sock,string(node),value=[1b,byte(not IsWindows())])
    if not status then begin
      if keyword_set(quiet) then message,mdsgetmsg(status,quiet=quiet),/continue,/noprint $
                            else message,mdsgetmsg(status,quiet=quiet),/continue
    endif

  endif else begin

    if (!VERSION.OS eq 'vms') then begin

      mds$set_def,node,quiet=quiet,status=status

    endif else begin
      status=call_external(MdsIdlImage(),'IdlMdsSetDefault',strtrim(node,2),value=[1b])
      if not status then begin
        msg = 'Error setting default to '+strtrim(node,2) 
        if keyword_set(quiet) then $
            message,msg,/continue,/noprint $
        else $
            message,msg,/continue
      endif
    endelse
  endelse

end
