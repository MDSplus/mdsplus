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

  if (n_params() ne 2 or n_elements(tree) eq 0 or n_elements(shot) eq 0) then begin
    message,'Must specify valid tree and shot to open',/continue,noprint=keyword_set(quiet)
    status = 0
    return
  endif
  if (strcompress(tree,/remove) eq '') then begin
    message,'Must specify valid tree name',/continue,noprint=keyword_set(quiet)
    status = 0
    return
  endif

  status = MdsValue('treeopen($,$)',string(tree),long(shot),quiet=quiet,status=status)
  if not status then begin
    if keyword_set(quiet) then message,mdsgetmsg(status,quiet=quiet),/continue,/noprint $
                          else message,mdsgetmsg(status,quiet=quiet),/continue
  endif

end
