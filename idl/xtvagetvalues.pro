;+
; NAME:  XtVaGetValues.pro
;
; PURPOSE: get arbitrary resource values from an IDL widget.
;
; CATEGORY:
;	MDSWIDGETS
;
; CALLING SEQUENCE:
;      XtVaGetValues,id,name,value,name,value, ... up to 12 pairs [,parent=parent][,/XWIDGET]
;
; INPUTS:
;       id - the widget id (IDL) of the widget whoose resources to query
;       name - the resource name to get (eg "XmNbuttonState")
;       value - an IDL variable to put the value into.
;
; KEYWORD PARAMETERS:
;       PARENT       - return resources of the top level widget in the hierarchy containing id.
;	XWIDGET	     - if included, the id is a real X windows widget not an IDL widget id.
;
; OUTPUTS:
;       The values of the specified resources are returned.
;
; COMMON BLOCKS:
;
; SIDE EFFECTS:
;
; PROCEDURE:
;       finds the real widgetid of the specified IDL widget and calls
;       XtGetValues on it.
;
; MODIFICATION HISTORY:
;
; Initial creation:         T.W.Fredian     10/9/93
; These comments added      Josh Stillerman 1/3/93
;-
pro XtVaGetValues,id,r1,v1,r2,v2,r3,v3,r4,v4,r5,v5,r6,v6,r7,v7,r8,v8,r9,v9,r10,v10,r11,v11,r12,v12,parent=parent,xwidget=xwidget
  num_resources = (n_params()-1)/2
  top=0L
  w=0L
  v1=0L
  v2=0L
  v3=0L
  v4=0L
  v5=0L
  v6=0L
  v7=0L
  v8=0L
  v9=0L
  v10=0L
  v11=0L
  v12=0L
  if keyword_set(xwidget) then begin
      w = id
  endif else begin
    top=0l
    rec = MdsValue('idl->IDL_WidgetStubLookup(val($))', id)
    if rec ne 0 then begin
      top = 0l
      junk = MdsValue('_top=0l, _w = 0l, idl->IDL_WidgetGetStubIds(val($), ref(_top), ref(_w))', rec)
      top = mdsvalue('_top')
      w = mdsvalue('_w')
      if keyword_set(parent) then w=top
    endif
  endelse

  if w ne 0 then begin
    expr = 'Xt->XtVaGetValues(val($)'
    cmd = 'X = MdsValue(expr, w'
    for i = 1, num_resources do begin
      expr = '_v'+strtrim(i,2)+'=0l,'+expr + ', ref($), ref(_v'+strtrim(i,2)+')'
      cmd = cmd + ',r'+strtrim(i,2)
    endfor
    expr = expr + ',val(0))'
    cmd = cmd + ")"
    dummy=execute(cmd)
    for i = 1, num_resources do begin
      cmd = 'v'+strtrim(i,2)+' = mdsvalue("_v'+strtrim(i,2)+'")'
      dummy = execute(cmd)
    endfor
  endif
  return
end
