;+
; NAME:
;	MDSOPEN
; PURPOSE:
;	Open an MDSplus experiment model or pulse file
; CATEGORY:
;	MDSPLUS/IP
; CALLING SEQUENCE:
;	MDSOPEN,tree,shot[,/QUIET][,STATUS=ISTAT]
; INPUT PARAMETERS:
;	tree = name of the experiment whose model or pulse
;                    file you want to open.
;       shot       = shot number of the file.
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
;       procedure MDSOPEN and checks for errors.
; MODIFICATION HISTORY:
;	 VERSION 1.0, CREATED BY T.W. Fredian, April 22,1991
;        VERSION 2.0, Jeff Schachter 1998.10.06 - added support for
;                     both client/server (MdsIpShr) and native access.
;                     Test is based on mdsIsClient() function
;-


pro MdsOpen,tree,shot,quiet=quiet,status=status

  forward_function mdsIsClient,mdsIdlImage,mds$socket,MdsRoutinePrefix,MdsIPImage,IsWindows

  if (mdsIsClient()) then begin

    ON_ERROR,2                  ;RETURN TO CALLER IF ERROR

    ; Determine whether experiment and shot were provided.

    sock = mds$socket(quiet=quiet,status=status)
    if not status then return
    status = call_external(MdsIPImage(),MdsRoutinePrefix()+'MdsOpen',$
                           sock,string(tree),long(shot),$
                           value=[1b,byte(not IsWindows()),1b])
    return
    if not status then begin
      if keyword_set(quiet) then message,mdsgetmsg(status,quiet=quiet),/continue,/noprint $
                            else message,mdsgetmsg(status,quiet=quiet),/continue
    endif

  endif else begin
    status=call_external(MdsIdlImage(),'IdlMdsOpen',strtrim(tree,2),long(shot),value=[1b,1b])
    if not status then begin
      msg = 'Error opening tree '+strtrim(tree,2)+' shot '+strtrim(shot,2)
      if keyword_set(quiet) then $
          message,msg,/continue,/noprint $
      else $
          message,msg,/continue
    endif
  endelse
end
