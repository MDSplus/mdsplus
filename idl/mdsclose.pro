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

  if (n_params() eq 2) then begin
    status = MdsValue('TreeClose($,$)',string(tree),long(shot),/quiet,status=retstatus)
  endif else begin
    status = MdsValue('TreeClose()',/quiet,status=retstatus)
    dummy=status
    while (dummy and 1) do begin
      dummy=MdsValue('TreeClose()',/quiet,status=retstatus)
    endwhile
  endelse
  if (status) then status = retstatus
  if not status then begin
    if keyword_set(quiet) then message,mdsgetmsg(status,quiet=quiet),/continue,/noprint $
                          else message,mdsgetmsg(status,quiet=quiet),/continue
  endif
end
