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

  status = MdsValue('TreeSetDefault($)',string(node),/quiet,status=retstatus)
  if (status) then status = retstatus
  if not status then begin
      if keyword_set(quiet) then message,mdsgetmsg(status,quiet=quiet),/continue,/noprint $
                            else message,mdsgetmsg(status,quiet=quiet),/continue
  endif
end
