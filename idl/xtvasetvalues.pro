;+
; NAME:  XtVaSetValues.pro
;
; PURPOSE: set arbitrary resource values in an IDL widget.
;
; CATEGORY:
;	MDSWIDGETS
;
; CALLING SEQUENCE:
;      XtVaSetValues,id,name,value,name,value, ... up to 12 pairs [,parent=parent][/XWIDGET]
;
; INPUTS:
;       id - the widget id (IDL) of the widget whoose resources to query
;       name - the resource name to set (eg "XmNbuttonState")
;       value - an IDL variable to put the value into.
;
; KEYWORD PARAMETERS:
;       PARENT       - return resources of the top level widget in the hierarchy containing id.
;       /XWIDGET     - if set then id is expected to be a real X windows widget not an IDL widget id.
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
;       XtSetValues on it.
;
; MODIFICATION HISTORY:
;
; Initial creation:         T.W.Fredian     10/9/93
; These comments added      Josh Stillerman 1/3/93
;-
pro XtVaSetValues,id,r1,v1,r2,v2,r3,v3,r4,v4,r5,v5,r6,v6,r7,v7,r8,v8,r9,v9,r10,v10,r11,v11,r12,v12,value=value,parent=parent, $
                             xwidget=xwidget
  w=0l
  if keyword_set(xwidget) then begin
    w=id
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
  if (w ne 0) then begin
    num_resources = (n_params()-1)/2
    if (n_elements(value) ne num_resources) then value = lonarr(num_resources)+1
    cmd = 'x = MdsValue(expr,w'
    expr = "Xt->XtVaSetValues(val($)"
    for i=1,num_resources do begin
        if (value[i-1] ne 0) then argref='val($)' else argref='$'
        expr = expr + ',ref($), '+argref
        cmd = cmd + ',r'+strtrim(i,2)+', v'+strtrim(i,2)
    endfor
    expr = expr+',val(0))'
    cmd = cmd + ')'
    dummy=execute(cmd)
  endif
  return
end
