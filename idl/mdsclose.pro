
;+
; NAME:
;	MDSCLOSE (Native and TCP/IP version) 
; PURPOSE:
;	Close an open MDSplus experiment model or pulse file
; CATEGORY:
;	MDSPLUS/IP
; CALLING SEQUENCE:
;	MDSCLOSE[,experiment,shot][,/QUIET,STATUS=ISTAT]
; OPTIONAL INPUT PARAMETERS:
;	experiment = name of the experiment used in an invocation
;                    of MDSOPEN.
;       shot       = shot number of the file.
;       If both experiment and shot are omitted, all files will be
;       closed.
; Keywords:
;       QUIET = prevents IDL error if TCL command fails
;       STATUS = return status, low bit set = success
; OUTPUTS:
;       istat = return status, low bit set = success
; OUTPUTS:
;	None.
; COMMON BLOCKS:
;	None.
; SIDE EFFECTS:
;	None.
; RESTRICTIONS:
;	None.
; PROCEDURE:
;	Straightforward.  Makes a call to MDSplus shared image library
;       procedure MDSCLOSE and checks for errors.
; MODIFICATION HISTORY:
;	 VERSION 1.0, CREATED BY T.W. Fredian, April 22,1991
;        VERSION 2.0, Jeff Schachter 1998.10.06 - added support for
;                     both client/server (MdsIpShr) and native access.
;                     Test is based on mdsIsClient() function
;-
;

pro MdsClose,tree,shot,quiet=quiet,status=status

  forward_function mdsIsClient,mdsIdlImage,mds$socket,MdsRoutinePrefix,MdsIPImage

  if (mdsIsClient()) then begin
    ON_ERROR,2                  ;RETURN TO CALLER IF ERROR

    ; Determine whether experiment and shot were provided.
    
    sock = mds$socket(quiet=quiet,status=status)
    if not status then return
    sock = call_external(MdsIPImage(),MdsRoutinePrefix()+'MdsClose',sock,value=[1b])
    if not status then begin
      if keyword_set(quiet) then message,mdsgetmsg(status,quiet=quiet),/continue,/noprint $
                            else message,mdsgetmsg(status,quiet=quiet),/continue
    endif

  endif else begin

    if (!VERSION.OS eq 'vms') then begin

      if n_elements(tree) eq 0 then $
        mds$close, quiet=quiet, status=status else $
        mds$close, tree, shot, quiet=quiet,status=status

    endif else begin
        

      if n_params() gt 1 then $
          status=call_external(MdsIdlImage(),'IdlMdsClose',strtrim(tree,2),long(shot),value=[1b,1b]) $
      else $
          status=call_external(MdsIdlImage(),'IdlMdsClose',0L,value=[1b])
      if not status then begin
        msg = 'Error closing tree'
        if (n_params() gt 1) then msg = msg+strtrim(tree,2)+' shot '+strtrim(shot,2)
        if keyword_set(quiet) then $
            message,msg,/continue,/noprint $
        else $
            message,msg,/continue
      endif

    endelse

  endelse
end
