; Copyright (c) 1995, M.I.T.
;	Unauthorized reproduction prohibited.
;+
; NAME: CW_BASE
;
; PURPOSE: A base widget which lays itself out correctly in rows
;
; CATEGORY:
;	Compound widgets.
;
; CALLING SEQUENCE:
;	widget = CW_BASE([parent][,row = row] [,column=column] [any widget_base keywords])
;
; INPUTS:
;       PARENT - The ID of the parent widget.  if not present this is a top level base
;
; KEYWORD PARAMETERS:
;	row = row  - number of rows this base should have
;	column = column  - number of columns this base should have
;       any widget_base keywords -  _EXTRA=e
;
; OUTPUTS:
;       The ID of the created widget is returned.
;
; COMMON BLOCKS:
;	None.
;
; SIDE EFFECTS:
;
; PROCEDURE:
;
; MODIFICATION HISTORY:
;    8/16/95  JAS original version
;-
;***************************************************
; fixup a row base by makeing sure they all have
; the same width.  set it to the maximum width
;***************************************************
pro fix_rows, w
  width = 0
  c = widget_info(w, /child)
  while c ne 0 do begin
    geo = widget_info(c, /geometry)
    if (geo.scr_xsize gt width) then width = geo.scr_xsize
    c = widget_info(c, /sibling)
  endwhile
  c = widget_info(w, /child)
  while c ne 0 do begin
    widget_control, c, scr_xsize = width
    c = widget_info(c, /sibling)
  endwhile
end

;***************************************************
; fixup a column base by makeing sure they all have
; the same height.  set it to the maximum height
;***************************************************
pro fix_columns, w
  height = 0
  c = widget_info(w, /child)
  while c ne 0 do begin
    geo = widget_info(c, /geometry)
    if (geo.scr_ysize gt height) then height = geo.scr_ysize
    c = widget_info(c, /sibling)
  endwhile
  c = widget_info(w, /child)
  while c ne 0 do begin
    widget_control, c, scr_ysize = height
    c = widget_info(c, /sibling)
  endwhile
end
;************************************************************
;  create a base with one of the above procecdures as its
;  notify_realize
;************************************************************
function cw_base, parent, row=row, column=column, _extra=e
  psz = size(parent)
  rsz = size(row)
  if rsz(1) ne 0 then begin
    if (psz(1) ne 0) then $ 
      base = widget_base(parent, row=row, notify_realize='fix_rows', _extra=e) $
    else $
      base = widget_base(row=row, notify_realize='fix_rows', _extra=e)
  endif else begin
    csz = size(column)
    if csz(1) ne 0 then begin
      if (psz(1) ne 0) then $ 
        base = widget_base(parent, column=column, notify_realize='fix_columns', _extra=e) $
      else $
        base = widget_base( column=column, notify_realize='fix_columns', _extra=e)
    endif else begin
      if (psz(1) ne 0) then $ 
        base = widget_base(parent, _extra=e) $
      else $
        base = widget_base(_extra=e)
    endelse
  endelse
  return, base
end
