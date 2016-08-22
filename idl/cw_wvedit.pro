;+
; NAME:
;
; PURPOSE:
;
; CATEGORY:
;	Compound widgets.
;
; CALLING SEQUENCE:
;       widget = cw_wvedit([parent],ROWS = rows, [UVALUE = uval,] [TITLE = title,] [EVENT_PROC=eproc,] $
;                         [USER_BUTTONS=ubuttons,] [XSIZE = xsize,] [YSIZE = ysize,] $
;                         [XOFFSET = xoffset,] [YOFFSET = yoffset,] [/EDITABLE] [/BUTTON_IDS], $
;                         [KILL_NOTIFY=kill_proc][/TLB_SIZE_EVENTS,] [MENU_BUTTONS=menu_buttons])
;
; INPUTS:
;       None
;
; KEYWORD PARAMETERS:
;       ROWS         - Scalar or vector containing the number of rows of waveforms per column.
;                      A column is created for each element of the rows vector.
;	UVALUE       - Supplies the user value for the widget.
;       TITLE        - Supplies a title for the cw_wvedit window and its corresponding control 
;                      window. Default is no title.
;       EVENT_PROC   - Supplies a procedure to receive pointer events.
;       USER_BUTTONS - Array of button labels to be created across the bottom of the control
;                      window.
;       XSIZE        - Specifies the width of the cw_wvedit window
;       YSIZE        - Specifies the height of the cw_wvedit window
;       XOFFSET      - Specifies the offset from the left of the screen in pixels.
;       YOFFSET      - Specifies the offset from the top of the screen in pixels.
;       EDITABLE     - Controls whether the wave edit pointer modes will be enabled or not
;                      (select,add,control,slide/stretch).
;       BUTTON_IDS   - Returns array of user button id's
;       KILL_NOTIFY  - Kill notify proc
;       MENU_BUTTONS - array of string labels for extra menu buttons.  Generates a
;                      CB_USER event with a field 'label' containing the first 16 
;                      characters of the menu item selected.
;
; OUTPUTS:
;       The ID of the created widget is returned.
;
; COMMON BLOCKS:
;
; SIDE EFFECTS:
;
; PROCEDURE:
;	widget_control, id, SET_VALUE=value can be used to change the
;		               characteristics of a wave or all waves
;          Pass a structure containing the items you want to change
;          in a wave or all waves. To change a single wave, include
;          the tag IDX to specify the wave number you want to modify.
;          To apply the changes to all waves, ommit the IDX tag.
;          The following are a list of TAG that can be specified:
;
;       Value related tags:
;
;       X:flt_array           - X axis values
;       Y:flt_array           - Y axis values
;       SELECTIONS:byte_array - Selections used in drawing
;       PEN:byt_array         - Indicate whether to draw line segment
;       AUTOSCALE:1 or 0      - Force autoscale of axes (default)
;
;   Note: X,Y,SELECTIONS, and PEN must be ALL be specified or none.
;         AUTOSCALE is optional and only operates when the above
;         tags are specified
;
;       Boolean resources (value set to zero or one):
;
;   
;       XINCREASING            - Enforce increasing x values when drawing.
;                                Effects spline behavior.
;       YINCREASING            - Enforce increasing y values when drawing.
;                                Effects spline behavior.
;       XLABELS                - Display labels on x grid lines
;       YLABELS                - Display labels on y grid lines
;       ATTACHCROSSHAIRS       - Attach crosshairs to curve when pointing
;       CLOSED                 - Connect first and last point.
;                                Effects spline behavior.
;       SHOWSELECTIONS         - Show selected points.
;       STEPPLOT               - Draw curve as a step plot.
;       
;       Integer resources:
;
;       POINTERMODE            - Selects pointer behavior.
;                                Select one of:   1  - None
;                                                 2  - Point
;                                                 3  - Zoom
;                                                 4  - Drag
;                                                 5  - Edit
;                                                 6  - Select
;                                                 7  - Add
;                                                 8  - Pen Control
;                                                 9  - Slide/Stretch
;       SHOWMODE               - Selects mode of display.
;                                Select one of:   0  - Show lines only
;                                                 1  - Show points only
;                                                 2  - Show both
;       XGRIDLINES             - Selects number of gridlines on x axis
;       YGRIDLINES             - Selects number of gridlines on y axis
;
;       Floating point resources:
;
;       LOWX:flt               - Minimum x value a point can dragged.
;       NOLOWX:1               - No minumum x value
;       LOWY:flt               - Minimum y value a point can dragged.
;       NOLOWY:1               - No minumum y value
;       HIGHX:flt              - Maximum x value a point can dragged.
;       NOHIGHX:1              - No maxumum x value
;       HIGHY:flt              - Maximum y value a point can dragged.
;       NOHIGHY:1              - No maxumum y value
;       XMIN:flt               - Minimum x axis value
;       NOXMIN:1               - No minumum x axis value (autoscale)
;       YMIN:flt               - Minimum y axis value
;       NOYMIN:1               - No minumum y axis value (autoscale)
;       XMAX:flt               - Maximum x axis value
;       NOXMAX:1               - No maxumum x axis value (autoscale)
;       YMAX:flt               - Maximum y axis value
;       NOYMAX:1               - No maxumum y axis value (autoscale)
;       XMINDISTANCE:flt       - Minimum x distance between two adjacent points
;       NOXMINDISTANCE:1       - No Minimum distance         
;       YMINDISTANCE:flt       - Minimum y distance between two adjacent points
;       NOYMINDISTANCE:1       - No Minimum distance         
;       XCROSSHAIR             - Location of x crosshair
;       YCROSSHAIR             - Location of y crosshair
;       XGRIDSNAP:flt	       - Snap grid of x axis
;       NOXGRIDSNAP:1	       - No snap grid on x axis
;	YGRIDSNAP:flt	       - Snap grid of y axis
;       NOYGRIDSNAP:1	       - No snap grid on y axis
;
;       Floating point array resources:
;
;       XCHOICES:flt_array     - List of valid x values when drawing
;       YCHOICES:flt_array     - List of valid y choices when drawing
;
;       Text resources:
;
;       TITLE:string           - Title to be displayed above wave
;
;       Pixmap resources:
;
;       BITMAP:filename - Bitmap to use to paint background
;                         of waveform. Specify an X11
;                         bitmap file.
;
;       Callback resources:
;
;       The following callbacks can be set:
;
;       NOTE: THESE ARE ADVANCED FEATURES
;       AND SHOULD ONLY BE USED BY EXPERTS FAMILIAR WITH THE XMDSWAVEDRAW
;       MOTIF BASED WIDGET. tHE VALUE OF THE CALLBACK SHOULD BE THE ADDRESS
;       OF A ROUTINE FOUND BY USING LIB$FIND_IMAGE_SYMBOL:
;
;       CROSSHAIRSCALLBACK:proc_address
;       ALIGNCALLBACK:proc_address
;       LIMITSCALLBACK:proc_address
;       UNDERLAYCALLBACK:proc_address
;       OVERLAYCALLBACK:proc_address
;       CUTCALLBACK:proc_address
;       PASTECALLBACK:proc_address
;       MOVECALLBACK:proc_address
;       SELECTCALLBACK:proc_address
;       DESELECTCALLBACK:proc_address
;       ADDPOINTCALLBACK:proc_address
;       DELETEPOINTCALLBACK:proc_address
;       FITCALLBACK:proc_address
;
;	widget_control, id, GET_VALUE=var can be used to obtain the current
;                       contents of the waves or return the number of waves
;
;          To obtain number of waves:
;
;              widget_control, id, SET_VALUE={IDX:-1}
;              widget_control, id, GET_VALUE=global_data
;              numwaves=global_data.numwaves
;              event_proc=global_data.event_proc
;
;          To optain wave content:
;
;              widget_control, id, SET_VALUE={IDX:wave_number}
;              widget_control, id, GET_VALUE=data
;              x=data.x
;              y=data.y
;              pen=data.pen
;              selections=data.selections
;              widget_id = data.widg
;
;              Note: Wave indexes begin at 0
;
; MODIFICATION HISTORY:
;
; Initial creation:         T.W.Fredian     10/16/92
; Added MENU_BUTTONS argument  Josh Stillerman  3/14/95
;-

Function XmdsWaveFormSetCrossHairs, w, x,y, attach
  ans = mdsvalue('XmdsShr->XmdsWaveformSetCrosshairs(val($), ref($), ref($), val($))', w, x, y, attach)
  return, ans
end

Function XtScreen, w
    return, MdsValue('Xt->XtScreen(val($)', w)
end

Function XtFree, mem
  return, MdsValue('Xt->XtFree(val($))',mem)
end

Function XmGetPixmap, screen, bm, fg, bg
    return, MdsValue('Xm->XmGetPixmap(val($), val($), val($), val($)', screen, bm, fg, bg)
end

Function XmdsWaveformSetWave, w, count, x, y, selections, pen, auto, defer_update
    ans = MdsValue('XmdsShr->XmdsWaveformSetWave(val($), val($), ref($), ref($), ref($), ref($), val($), val($))', $
                     w, count, x, y, byte(selections), byte(pen), byte(auto), byte(defer_update),status=status)
end

Function XmdsWavedrawMovePoint, w, idx, x, y, newx, newy, callbacks
    status = MdsValue('_newx = 0.0, _newy = 0.0, XmdsShr->XmdsWavedrawMovePoint(val($), val($), ref($), ref($), ref(_newx), ref(_newy), val($), val(0))', $
                     w, idx, x, y, callbacks)
    newx = MdsValue('_newx')
    newy = MdsValue('_newy')
    return, status
end

Function XmdsWavedrawAddPoint, w, idx, x, y, call_callbacks, restrict_motion
    return, MdsValue('XmdsShr->XmdsWavedrawAddPoint(val($), val($), ref($), ref($), val($), val($))', $
                   w, idx, x, y, call_callbacks, restrict_motion  )
end

Function XmdsWavedrawDeletePoint, w, idx, call_callbacks
  return, MdsValue('XmdsShr->XmdsWavedrawDeletePoint(val($), val($), val($))', w, idx, call_callbacks)
end

Function XmdsWaveformSetPointerMode, w, mode
  return, MdsValue('XmdsShr->XmdsWaveformSetPointerMode(val($), val($))', w, mode)
end

Function cw_wvedit_external,subbase, id, num, rows,w,waves, menu_buttons, num_menu_button
  num = long(num)
  rows = long(rows)
  total = long(total(rows))
  if (!version.memory_bits eq 32) then alloc="_w=0L, _waves=zero($,0)" else alloc="_w=0q, _waves=zero($,0q)"
  status =  MdsValue(alloc + ', IdlMdsWidgets->CW_WVEDIT(ref($), ref($),ref($),ref($),ref(_w),ref(_waves),descr($),val($))', $
                 total ,subbase,id, num, rows, menu_buttons, num_menu_button)
  w = mdsvalue('_w')
  waves = mdsvalue('_waves')
  return, status
end

Function MemMove,dst, src, siz
  dst = MdsValue('_out=$, IdlMdsWidgets->memmoveext(ref(_out), val($), val($)),_out', dst, src, siz)
end
 
Function SiblingWaves,state,idx,count
  count = n_elements(state.waves)-1
  if count gt 0 then begin
    case idx of
      0: return,state.waves(1:*)
      count: return,state.waves(0:idx-1)
      else: return,[state.waves(0:idx-1),state.waves(idx+1:*)]
    endcase
  endif else return,-1
end

PRO wvedit_set_value, id, value

	; This routine is used by widget_control to set the value for
	; your compound widget.  It accepts one variable.  
	; You can organize the variable as you would like.  If you have
	; more than one setting, you may want to use a structure that
	; the user would need to build and then pass in using 
	; widget_control, compoundid, SET_VALUE = structure.


	; Return to caller.
  ON_ERROR, 2

	; Retrieve the state.
  widget_control, widget_info(id, /CHILD), get_uvalue=state,/no_copy

; Set the value here.
;
;
; Get wave index if present (if ommited, settings will be applied to all waves
;
;
  tags = TAG_NAMES(value)
  idx_idx = where(tags eq "IDX",idx_present)
  idx_invalid = 0
  if idx_present then begin
    state.idx = value.idx
    start_idx = state.idx
    end_idx = state.idx
    if (value.idx lt 0) or (value.idx ge total(state.rows)) then begin
      idx_invalid = 1
      invalid_idx_msg = "Wave index supplied ("+strtrim(string(value.idx),2)+") out of range. Must be between 0 and "+$
                                                strtrim(string(total(state.rows)),2)
    endif
  endif else begin
    start_idx = 0
    end_idx = total(state.rows)-1
  endelse
  if not idx_invalid then begin
    for i=start_idx,end_idx do begin
      XtVaSetValues,state.waves(i),"disabled",1,/xwidget
    endfor
  endif
;
;
; Set all Boolean and integer resources
;
;
  resource = ["xIncreasing","yIncreasing","pointerMode","showMode","showSelections","stepPlot","attachCrosshairs","closed", $
              "xGridLines","xLabels","yGridLines","yLabels"]
  for ridx=0,N_ELEMENTS(resource)-1 do begin
    idx = where(tags eq STRMID(STRUPCASE(resource(ridx)),0,15),present)
    if present then begin
      if idx_invalid then wve_complain,id,invalid_idx_msg else begin
        for i=start_idx,end_idx do begin
          XtVaSetValues,state.waves(i),resource(ridx),long(value.(idx(0))),/xwidget
        endfor
      endelse
    endif
  endfor
;
;
; Set all Optional floating point resources. Tag should be set to NOresource:1 to disable the resource.
;
;
  resource = ["xMin","xMax","lowX","highX","xMinDistance","xGridSnap",$
              "yMin","yMax","lowY","highY","yMinDistance","yGridSnap"]
  for ridx=0,N_ELEMENTS(resource)-1 do begin
    idx = where(tags eq STRMID(STRUPCASE(resource(ridx)),0,15),present)
    if present then begin
      if idx_invalid then wve_complain,id,invalid_idx_msg else begin
        for i=start_idx,end_idx do begin
          v = float(value.(idx(0)))
          XtVaSetValues,state.waves(i),resource(ridx),v,/xwidget,value=[0]
        endfor
      endelse
    endif
  endfor
  for ridx=0,N_ELEMENTS(resource)-1 do begin
    idx = where(tags eq STRMID(STRUPCASE("NO"+resource(ridx)),0,15),present)
    if present then begin
      if value.(idx(0)) eq 1 then begin
        if idx_invalid then wve_complain,id,invalid_idx_msg else begin
          for i=start_idx,end_idx do begin
            XtVaSetValues,state.waves(i),resource(ridx),0,/xwidget
          endfor
        endelse
      endif
    endif
  endfor
;
; deal with the crosshairs as a special case
;
    idx = where(tags eq "XCROSSHAIR", present)
    if present then begin
      if idx_invalid then wve_complain,id,invalid_idx_msg else begin
        x = float(value.(idx(0)))
        for i=start_idx,end_idx do begin
          d = XmdsWaveformSetCrossHairs(state.waves(i), x,0.0, 1)
        endfor
        widget_control,state.coordw,GET_VALUE=coord
        widget_control,state.coordw,SET_VALUE={x:x, y:coord.y}
      endelse
    endif
;
;
; Load other floating vector resources, use NOresource:1 to disable
;
;
  resource = ["xChoices","yChoices"]
  cnt_resource = ["xNumChoices","yNumChoices"]
  for ridx=0,N_ELEMENTS(resource)-1 do begin
    idx = where(tags eq STRMID(STRUPCASE(resource(ridx)),0,15),present)
    if present then begin
      if idx_invalid then wve_complain,id,invalid_idx_msg else begin
        for i=start_idx,end_idx do begin
          v = float(value.(idx(0)))
          XtVaSetValues,state.waves(i),cnt_resource(ridx),long(n_elements(value.(idx(0)))),$
                        resource(ridx),v,/xwidget,value=[1,0]
        endfor
      endelse
    endif
  endfor
  for ridx=0,N_ELEMENTS(resource)-1 do begin
    idx = where(tags eq STRMID(STRUPCASE("NO"+resource(ridx)),0,15),present)
    if present then begin
      if value.(idx(0)) eq 1 then begin
        if idx_invalid then wve_complain,id,invalid_idx_msg else begin
          for i=start_idx,end_idx do begin
            XtVaSetValues,state.waves(i),cnt_resource(ridx),0,resource(ridx),0,/xwidget
          endfor
        endelse
      endif
    endif
  endfor
;
;
; Load string resources
;
;
  resource = ["title"]
  for ridx=0,N_ELEMENTS(resource)-1 do begin
    idx = where(tags eq STRMID(STRUPCASE(resource(ridx)),0,15),present)
    if present then begin
      if idx_invalid then wve_complain,id,invalid_idx_msg else begin
        for i=start_idx,end_idx do begin
          XtVaSetValues,state.waves(i),resource(ridx),string(value.(idx(0))),value=[0],/xwidget
        endfor
      endelse
    endif
  endfor
;
;
; Load callbacks
;
;
  resource = ["crosshairsCallback","alignCallback","limitsCallback","underlayCallback","overlayCallback","cutCallback",   $
              "pasteCallback","moveCallback","selectCallback","deselectCallback","addPointCallback","deletePointCallback",$
              "fitCallback"]
  for ridx=0,N_ELEMENTS(resource)-1 do begin
    idx = where(tags eq STRMID(STRUPCASE(resource(ridx)),0,15),present)
    if present then begin
      if idx_invalid then wve_complain,id,invalid_idx_msg else begin
        for i=start_idx,end_idx do begin
          XtVaSetValues,state.waves(i),resource(ridx),long(value.(idx(0))),/xwidget
        endfor
      endelse
    endif
  endfor
;
;
; Load background pixmap
;
  idx = where(tags eq "BITMAP",present)
  if present then begin
    if idx_invalid then wve_complain,id,invalid_idx_msg else begin
      fg=0L
      bg=0L
      XtVaGetValues,state.waves(0),"foreground",fg,"background",bg,/xwidget
      screen = XtScreen(state.waves(0))
      pix = XmGetPixmap(screen, value.bitmap, fg, bg)
      for i=start_idx,end_idx do begin
        XtVaSetValues,state.waves(i),"backgroundPixmap",pix,/xwidget
      endfor
    endelse
  endif
;
;
; Load new data in waveform if X,Y,PEN and SELECTIONS are ALL supplied.
;
;
  x_idx = where(tags eq "X",x_present)
  y_idx = where(tags eq "Y",y_present)
  pen_idx = where(tags eq "PEN",pen_present)
  selections_idx = where(tags eq "SELECTIONS",selections_present)
  autoscale_idx = where(tags eq "AUTOSCALE",autoscale_present)
  if autoscale_present then autoscale = value.autoscale else autoscale = 1
  if x_present or y_present or pen_present or selections_present then begin
    if idx_invalid then wve_complain,id,invalid_idx_msg else begin
      if x_present and y_present and pen_present and selections_present then begin
        count = min([n_elements(value.x),n_elements(value.y),n_elements(value.selections),n_elements(value.pen)])
        x=float(value.x)
        y=float(value.y)
        for i=start_idx,end_idx do begin
          dummy = XmdsWaveformSetWave(state.waves(i),long(count),x, y, value.selections, value.pen, autoscale, 0)

        for j=0,count-1 do begin
            newx=0.0
            newy=0.0
            dummy = XmdsWavedrawMovePoint(state.waves(i), j, x(j), y(j), newx, newy, 0)
        endfor
        ENDFOR
      endif else wve_complain,id,"If specifying one of X,Y,PEN or SELECTIONS, all four must be specified"
    endelse
  endif
  if not idx_invalid then begin
    for i=start_idx,end_idx do begin
      XtVaSetValues,state.waves(i),"disabled",0,/xwidget
    endfor
  endif
  widget_control, widget_info(id, /CHILD), set_uvalue=state,/no_copy
  RETURN
END



FUNCTION wvedit_get_value, id

	; This routine is by widget_control to get the value from 
	; your compound widget.  As with the set_value equivalent,
	; you can only pass one value here so you may need to load
	; the value by using a structure or array.


	; Return to caller.
  ON_ERROR, 2

	; Retrieve the structure from the child that contains the sub ids.

  widget_control, widget_info(id, /CHILD), get_uvalue=state,/no_copy

	; Get the value here and return it.

  if state.idx ge 0 and state.idx lt total(state.rows) then begin
    count = 0L
    x=0.0
    y=0.0
    pen=0b
    selections=0b
    XtVaGetValues,state.waves(state.idx),'count',count,/xwidget
    if count gt 0 then begin
      x=fltarr(count)
      y=fltarr(count)
      pen=bytarr(count)
      selections=bytarr(count)
      xaddr = 0L
      yaddr = 0L
      penaddr = 0L
      seladdr = 0L
      XtVaGetValues,state.waves(state.idx),'xValue',xaddr,'yValue',yaddr,'selections',seladdr,'penDown',penaddr,/xwidget
      dummy = MemMove(x, xaddr, count*4)
      dummy = MemMove(y, yaddr, count*4)
      dummy = MemMove(pen, penaddr, count)
      dummy = MemMove(selections, seladdr, count)
    endif
    ans = {count:count,x:x,y:y,selections:selections,pen:pen, widg:state.waves(state.idx)}
  endif else ans = {numwaves:total(state.rows),event_proc:state.event_proc, stub:state.id}
  widget_control, widget_info(id, /CHILD), set_uvalue=state,/no_copy
  return, ans
END

pro wvedit_get_p_m, id, mode
  ON_ERROR, 2
  mode=0L
  widget_control, widget_info(id, /CHILD), get_uvalue=state,/no_copy
  XtVaGetValues,state.waves(0),'pointerMode',mode,/xwidget
  widget_control, widget_info(id, /CHILD), set_uvalue=state,/no_copy
end

pro wvedit_get_xy, id, idx, x, y, step


	; Return to caller.
  ON_ERROR, 2

	; Retrieve the structure from the child that contains the sub ids.

  widget_control, widget_info(id, /CHILD), get_uvalue=state,/no_copy

	; Get the value here and return it.

    count = 0L
    x = 0.0
    y = 0.0
    step=0
    XtVaGetValues,state.waves(idx),'count',count,/xwidget
    if count gt 0 then begin
      x=fltarr(count)
      y=fltarr(count)
      xaddr = 0L
      yaddr = 0L
      stepaddr = 0L
      XtVaGetValues,state.waves(idx),'xValue',xaddr,'yValue',yaddr,'stepPlot',stepaddr,/xwidget
      dummy = MemMove(x, xaddr, count*4)
      dummy = MemMove(y, yaddr, count*4)
      dummy = MemMove(step, stepaddr, 1)
    endif
  widget_control, widget_info(id, /CHILD), set_uvalue=state,/no_copy
  return
END

;-----------------------------------------------------------------------------

pro wvedit_MovePoint,state,x,y
  if state.idx ge 0 then begin
    dummy = XmdsWavedrawMovePoint(state.waves(state.idx), state.m_idx, x, y, x, y, 1)
    widget_control,state.coordw,SET_VALUE={X:x,Y:y}
  endif
  return
end

function CW_WVEDIT_CVT,ev
  event_info =  MdsValue('_info=long(zero(10)),IdlMdsWidgets->GetEventInfo($, ref(_info)),_info',ev.value)
  idx = event_info(0)
  reason = event_info(1)
  callback = event_info(2)
  m_idx = event_info(3)
  bstate = event_info(8)
  button = event_info(9)
  reasons = ['Unknown','Crosshairs','Align','ZoomIn','ZoomOut','BoxZoom','Drag','Button3','NewData','DragEnd', $
             'MovePointBegin','MovePoint','MovePointEnd','SelectPoint','DeselectPoint','AddPoint','DeletePoint',$
             'SetPen','Paste', "User"]
  if (reason lt 0 or reason ge N_ELEMENTS(reasons)) then reason = 0
  if (reasons(reason) ne "User") then begin
    values = float(event_info,16,4)
  endif else begin
    vals = BYTE(event_info, 16, 16)
    values = string(vals)
  endelse
  case callback of
    1: event = {autoscale_cb, id:ev.top, top:ev.top, handler:ev.top, idx:idx, state:bstate, button:button, $
                reason:reasons(reason), xmin:values(0),xmax:values(1),ymin:values(2),ymax:values(3)}
    2: event = {point_cb, id:ev.top, top:ev.top, handler:ev.top, idx:idx, state:bstate, button:button, $
                reason:reasons(reason), x:values(0), y:values(1)}
    3: event = {zoom_cb, id:ev.top, top:ev.top, handler:ev.top, idx:idx, state:bstate, button:button, $
                 reason:reasons(reason),xmin:values(0),xmax:values(1),ymin:values(2),ymax:values(3)}
    4: event = {move_cb, id:ev.top, top:ev.top, handler:ev.top, idx:idx, state:bstate, button:button, $
                 reason:reasons(reason), new_x:values(1), new_y:values(3), old_x:values(0), old_y:values(2), m_idx:m_idx}
    5: event = {stretch_cb, id:ev.top, top:ev.top, handler:ev.top, idx:idx, state:bstate, button:button, $
                reason:reasons(reason), new_x:values(1), new_y:values(3), old_x:values(0), old_y:values(2), m_idx:m_idx}
    6: event = {set_at_limits_cb, id:ev.top, top:ev.top, handler:ev.top, idx:idx, state:bstate, button:button, $
               reason:reasons(reason), xmin:values(0),xmax:values(1),ymin:values(2),ymax:values(3)}
    7: event = {add_point_cb, id:ev.top, top:ev.top, handler:ev.top, idx:idx, state:bstate, button:button, $
               reason:reasons(reason), new_x:values(1), new_y:values(3), old_x:values(0), old_y:values(2), m_idx:m_idx}
    8: event = {delete_point_cb, id:ev.top, top:ev.top, handler:ev.top, idx:idx, state:bstate, button:button, $
               reason:reasons(reason), new_x:values(1), new_y:values(3), old_x:values(0), old_y:values(2), m_idx:m_idx}
    9: event = {paste_cb, id:ev.top, top:ev.top, handler:ev.top, idx:idx, reason:'paste'}
   10: event = {fit_cb, id:ev.top, top:ev.top, handler:ev.top, idx:idx, reason:reasons(reason)}
   11: event = {user_cb, id:ev.top, top:ev.top, handler:ev.top, idx:idx, label:values, reason:reasons(reason)}
   ELSE: event = {unknown_cb, id:ev.top, top:ev.top, handler:ev.top, idx:idx, state:bstate, button:button, reason:reasons(reason)};
  endcase
  return,event
end

FUNCTION wvedit_events, ev
  return_ev = 0
  parent=widget_info(ev.handler,/parent)
  widget_control, widget_info(parent, /CHILD), get_uvalue=state,/no_copy
  case tag_names(ev, /STRUCTURE_NAME) of
  "WIDGET_BUTTON": begin
      return_ev = ev
      pointer_mode = -1
      widget_control, ev.id, get_value=button
      case (button) of
      "Point": begin
               if ev.select then pointer_mode = 2
               help_text = "Button 1: Position crosshairs"
               end
      "Zoom":  begin
               if ev.select then pointer_mode = 3
               help_text = "Button 1: Zoom in 2x, Button 1 drag: Zoom in box, Button 2: Zoom out"
               end
      "Pan": begin
             if ev.select then pointer_mode = 4
             help_text = "Button 1: Drag curves (change axis, not curve values)"
             end
      "Paste": begin
               if ev.select then pointer_mode = 5
               help_text = "Button 2: Paste selected curve from DWSCOPE"
               end
      "Select": begin
                if ev.select then pointer_mode = 6
                help_text = $
                "Button 1: Move knot (modifiers: shift=lock X, alt=lock Y, shift-lock=high res), "+ $
                "Button 2: Knot (modifiers: ctrl=UnKnot, shift=set X locked, alt=set Y locked)"
                end
      "Add": begin
             if ev.select then pointer_mode = 7
             help_text = $
       "Button 1: Move knot (modifiers: shift=lock X, alt=lock Y, shift-lock=high res), "+ $
        "Button 2: Add point (shift=Remove point)"
             end

      "Control": begin
                 if ev.select then pointer_mode = 8
                 help_text = "Button 1: Toggle line segment on and off"
                 end
      "Stretch": begin
                       if ev.select then pointer_mode = 9
                       help_text = $
"Button 1: Drag curve (change values)  (modifiers: shift=lock X, alt=lock Y, shift-lock=high res, ctrl=Stretch/Shrink curve)"
                       end
      "Insert": begin
                return_ev = 0
                for i=0,n_elements(state.waves)-1 do begin
                  widget_control, widget_info(parent, /CHILD), set_uvalue=state,/no_copy
                  widget_control,ev.top,set_value={idx:i}
                  widget_control,ev.top,get_value=wave
                  widget_control, widget_info(parent, /CHILD), get_uvalue=state,/no_copy
                  widget_control,state.coordw,get_Value=coord
                  if  n_elements(wave.y) ge 2 then begin
                    y=interpol(wave.y,wave.x,coord.x)
                    idx = where(wave.x gt coord.x,count)
                    if count gt 0 then idx = idx(0) else idx = 0
                    dummy = XmdsWavedrawAddPoint(state.waves(i),idx,coord.x,y,1,3)
                  endif
                endfor
                end
      "Remove": begin
                return_ev = 0
                for i=0,n_elements(state.waves)-1 do begin
                  widget_control, widget_info(parent, /CHILD), set_uvalue=state,/no_copy
                  widget_control,ev.top,set_value={idx:i}
                  widget_control,ev.top,get_value=wave
                  widget_control, widget_info(parent, /CHILD), get_uvalue=state,/no_copy
                  widget_control,state.coordw,get_Value=coord
                  if n_elements(wave.x) gt 2 then begin
                    dist = abs(wave.x - coord.x)
                    idx = where(dist eq min(dist))
                    idx = idx(0)
                    dummy = XmdsWavedrawDeletePoint(state.waves(i),idx,1)
                  endif
                endfor
                end
      else: begin
              widget_control, widget_info(parent, /CHILD), set_uvalue=state,/no_copy
              RETURN, ev
           end
      endcase
      if pointer_mode gt 0 then begin
        if pointer_mode eq 2 then sensitive = 1 else sensitive = 0
        if state.insertw ne 0 then widget_control,state.insertw,SENSITIVE=sensitive
        if state.removew ne 0 then widget_control,state.removew,SENSITIVE=sensitive
        state.pointer_mode = pointer_mode
        widget_control,state.helpw,SET_VALUE=help_text
        addr = 0L
        for i=0,n_elements(state.waves)-1 do $
          dummy = XmdsWaveformSetPointerMode(state.waves(i),pointer_mode)
        state.idx = -1
      endif
    end
  "WVEDIT_COORD": begin
    if state.pointer_mode eq 2 then begin
      for i=0,n_elements(state.waves)-1 do begin
          dummy = XmdsWaveFormSetCrossHairs(state.waves(i), ev.x, ev.y, 1)
      endfor
      event = {point_cb, id:ev.top, top:ev.top, handler:ev.top, idx:state.idx, state:0, button:0, reason:'Entered manually', $
               x:ev.x, y:ev.y}
    endif else if (state.pointer_mode eq 6 or state.pointer_mode eq 7) then begin
      wvedit_MovePoint,state,ev.x,ev.y
      event = {move_cb, id:ev.top, top:ev.top, handler:ev.top, idx:state.idx, state:0, button:0, reason:'Entered manually', $
                  new_x:ev.x, new_y:ev.y, old_x:0.0, old_y:0.0, m_idx:state.m_idx}
    endif
    end
  "WIDGET_STUB": begin
    event = CW_WVEDIT_CVT(ev)
    case (tag_names(event, /STRUCTURE_NAME)) of
      'POINT_CB': begin
                siblings = SiblingWaves(state, event.idx, count)
                if (count gt 0) then begin
                  for i=0,n_elements(siblings)-1 do begin
                      dummy = XmdsWaveformSetCrossHairs(siblings(i), event.x, event.y, 0)
                  endfor
                endif
                widget_control,state.coordw,SET_VALUE={X:event.x,Y:event.y}
                end
      'MOVE_CB': begin
                 widget_control,state.coordw,SET_VALUE={X:event.new_x,Y:event.new_y}
                 if event.reason eq "MovePointEnd" then begin
                   state.idx = event.idx
                   state.m_idx = event.m_idx
                 endif
               end
      else:
    endcase
    return_ev = event
    end
  else: return_ev = ev
  endcase
  widget_control, widget_info(parent, /CHILD), set_uvalue=state,/no_copy
  RETURN,return_ev
END

Pro WVEDIT_C_SET,id,value
  widget_control,widget_info(id,/CHILD),GET_UVALUE=widgets
  widget_control,widgets.xw,SET_VALUE= STRCOMPRESS(STRING(value.x),/REMOVE_ALL)
  widget_control,widgets.yw,SET_VALUE= STRCOMPRESS(STRING(value.y),/REMOVE_ALL)
  widget_control,id,SET_UVALUE=value
  return
end

Function WVEDIT_C_GET,id
  widget_control,id,GET_UVALUE=value
  return,value
end

function wve_cmpl_event,ev
  widget_control,ev.handler,/destroy
  return,0
end

pro wve_Complain, id, txt
  w = WIDGET_BASE(TITLE='Problem',/column,event_func='wve_cmpl_event')
  widget_control,w,/DELAY_DESTROY
  lab = WIDGET_LABEL(w, value=txt)
  button = WIDGET_BUTTON( w,value='Acknowledge')
  widget_control, w, /REALIZE
  return
end


Function WVEDIT_C_EVENT,ev
  widget_control,widget_info(ev.handler,/CHILD),GET_UVALUE=widgets
  widget_control,widgets.xw,GET_VALUE=text
  text=strcompress(text(0),/REMOVE_ALL)
  x=0.0
  on_ioerror,bad_cval
  reads,text,x
  widget_control,widgets.yw,GET_VALUE=text
  text=strcompress(text(0),/REMOVE_ALL)
  y=0.0
  reads,text,y
  widget_control,ev.handler,SET_UVALUE={X:x,Y:y}
  return,{wvedit_coord,id:ev.handler, top:ev.top, handler:0L,X:x,Y:y}
  BAD_CVAL:
  wve_complain,ev.top,"Cannot convert: /"+text+"/ to floating point number"
  return,0
end

Pro wvedit_resize,ev
  main_info = widget_info(ev.id,/geometry)
  if ev.x ne main_info.xsize or ev.y ne main_info.ysize then begin
    widget_control, ev.id, set_value = {idx:-1}
    widget_control, ev.id, get_value=state
    plots_info = widget_info(state.stub,/geometry)
    new_x = max([ev.x - main_info.xsize + plots_info.xsize,50])
    new_y = max([ev.y - main_info.ysize + plots_info.ysize,50])
    widget_control,xsize=new_x,ysize=new_y,state.stub
  endif
  return
end

function SetMargins,id,x=x,y=y
  XtVaSetValues,id,'marginHeight',long(y),'marginWidth',long(x)
  return,id
end

FUNCTION cw_wvedit, UVALUE = uval, TITLE = title, ROWS = rows, USER_BUTTONS=ubuttons, $
                    XSIZE = xsize, YSIZE = ysize, EDITABLE = editable, XOFFSET=xoffset, YOFFSET=yoffset, $
                    BUTTON_IDS=button_ids, KILL_NOTIFY=kill_proc, EVENT_PROc=event_proc, $
                    MENU_BUTTONS=menu_buttons, _EXTRA = extra, parent


;  ON_ERROR, 2					;return to caller
;
; Make sure UIDPATH is OK
;
  info = file_info('/usr/local/mdsplus/uid32')  
  if (info.exists and !version.MEMORY_BITS eq 32) then $
    setenv,'UIDPATH=/usr/local/mdsplus/uid32/%U'
	; Defaults for keywords
  IF NOT (KEYWORD_SET(uval))  THEN uval = 0
  IF NOT (KEYWORD_SET(title)) THEN title = ''
  IF NOT (KEYWORD_SET(rows)) THEN rows = 1
  IF NOT (KEYWORD_SET(xsize)) THEN xsize = 775
  IF NOT (KEYWORD_SET(ysize)) THEN ysize = 400
  IF NOT (KEYWORD_SET(xoffset)) THEN xoffset = 100
  IF NOT (KEYWORD_SET(yoffset)) THEN yoffset = 100
  IF NOT (KEYWORD_SET(editable)) THEN editable = 0
  IF NOT (KEYWORD_SET(kill_proc)) THEN kill_proc = ''
  IF NOT (KEYWORD_SET(event_proc)) THEN event_proc = ''
  IF NOT (KEYWORD_SET(menu_buttons)) THEN begin
    menu_buttons = ''
    num_menu_buttons = 0
  endif else num_menu_buttons = N_ELEMENTS(menu_buttons)

	; Rather than use a common block to store the widget IDs of the 
	; widgets in your compound widget, put them into this structure so
                                ; that you can have multiple instances
                                ; of your compound widget.
  if (!version.memory_bits eq 32) then waves=lonarr(total(rows)) else waves=lon64arr(10)

  waves = lonarr(total(rows))

	; Here the widget base that encompasses your compound widget's 
	; individual components is created.  This is the widget ID that
	; is passed back to the user to represent the entire compound
	; widget.  If it gets mapped, unmapped, sensitized or otherwise
	; effected, each of its individual subcomponents will also be 
	; effected.  You can see that the event handler is installed here.
	; As events occur in the sub-components of the compound widgets,
	; the events are passed up the tree until they hit this base 
	; where the event handler you define above handles them.  Similarily
	; whenever widget_control, SET/GET_VALUE is called on this base,
	; the routine defined by the FUNC_GET/PRO_SET_VALUE is called to
	; set the value of the compound widget.  None of the three keywords
	; that override the standard behaviour are not required so it 
	; depends on your usage whether they are needed.

  if n_params() eq 0 then begin
    mainbase = SetMargins(WIDGET_BASE(/COLUMN, UVALUE = uval, title=title, $
		FUNC_GET_VALUE = "wvedit_get_value", $
		PRO_SET_VALUE = "wvedit_set_value",xoffset=xoffset,yoffset=yoffset+ysize+40,_EXTRA=extra), $
                x=0,y=0)
  endif else begin
    mainbase = parent
    widget_control, mainbase, func_get_value = "wvedit_get_value", PRO_SET_VALUE = "wvedit_set_value"
  endelse
	; Here you would define the sub-components of your widget.  There
	; is an example component which is just a label.
  subbase = SetMargins(WIDGET_BASE(mainbase, /COLUMN, EVENT_FUNC="wvedit_events"),x=0,y=0)
  id = WIDGET_STUB(subbase)
  if (!version.memory_bits eq 32) then w=0L else w=0LL
  junk =  cw_wvedit_external(subbase,id,n_elements(rows),rows,w,waves, menu_buttons, num_menu_buttons)
  new_state = {coordw:0L, helpw:0L, idx:-1l, insertw:0L, removew:0L, pointer_mode:2l, m_idx:0L, plots:w, waves:waves, $
               rows:rows, kill_proc:kill_proc, event_proc:event_proc, id:id}
  widget_control,id,xsize=xsize,ysize=ysize-71
;  XtVaSetValues,w,'width',xsize,'height',ysize-71,/xwidget
  widx=0
  for i=0,n_elements(rows)-1 do begin
    height = (ysize-71)/rows(i) - 2
    for j=0,rows(i)-1 do begin
      XtVaSetValues,waves(widx),'height',height,/xwidget
      widx = widx + 1
    endfor
  endfor
  rowbase = SetMargins(WIDGET_BASE(subbase,/ROW),x=0,y=0)
  pointer_control = SetMargins(WIDGET_BASE(rowbase,/ROW,/EXCLUSIVE),x=0,y=0)
  w = SetMargins(WIDGET_BUTTON(pointer_control, VALUE = "Point",/NO_RELEASE),x=0,y=0)
  w = SetMargins(WIDGET_BUTTON(pointer_control,value = "Zoom",/NO_RELEASE),x=0,y=0)
  widget_control,w,set_button=1
  w = SetMargins(WIDGET_BUTTON(pointer_control,value = "Pan",/NO_RELEASE),x=0,y=0)
  if editable then begin
    w = SetMargins(WIDGET_BUTTON(pointer_control,value = "Select",/NO_RELEASE),x=0,y=0)
    w = SetMargins(WIDGET_BUTTON(pointer_control,value = "Add",/NO_RELEASE),x=0,y=0)
    w = SetMargins(WIDGET_BUTTON(pointer_control,value = "Control",/NO_RELEASE),x=0,y=0)
    w = SetMargins(WIDGET_BUTTON(pointer_control,value = "Stretch",/NO_RELEASE),x=0,y=0)
    w = SetMargins(WIDGET_BUTTON(pointer_control,value = "Paste",/NO_RELEASE),x=0,y=0)
    new_state.insertw = SetMargins(WIDGET_BUTTON(rowbase,value = "Insert"),x=0,y=0)
    widget_control,new_state.insertw,SENSITIVE=0
    new_state.removew = SetMargins(WIDGET_BUTTON(rowbase,value = "Remove"),x=0,y=0)
    widget_control,new_state.removew,SENSITIVE=0
  endif
  smallfont = '-*-HELVETICA-MEDIUM-R-*--*-100-*-*-*-*-ISO8859-1'
  new_state.helpw = SetMargins(WIDGET_LABEL(subbase,font=smallfont,/DYNAMIC_RESIZE, $
         value="Button 1: Zoom in 2x, Button 1 drag: Zoom in box, Button 2: Zoom out"),x=0,y=0)
  base = SetMargins(WIDGET_BASE(subbase,/ROW),x=0,y=0)
  if n_elements(ubuttons) gt 0 then button_ids=lonarr(n_elements(ubuttons))
  for i=0,n_elements(ubuttons)-1 do begin
    button_ids(i) = SetMargins(WIDGET_BUTTON(base,value = ubuttons(i)),x=0,y=0)
  endfor
  new_state.coordw = SetMargins( $
      WIDGET_BASE(base,/ROW,pro_set_value='wvedit_c_set',func_get_value='wvedit_c_get',event_func='wvedit_c_event'),x=0,y=0)
  label = SetMargins(WIDGET_LABEL(new_state.coordw,value='      X:'),x=0,y=0)
  xw = SetMargins(WIDGET_TEXT(new_state.coordw,/editable,xsize=14),x=0,y=0)
  w = SetMargins(WIDGET_LABEL(new_state.coordw,value='   Y:'),x=0,y=0)
  yw = SetMargins(WIDGET_TEXT(new_state.coordw,/editable,xsize=14),x=0,y=0)
  w = SetMargins(WIDGET_BUTTON(new_state.coordw,value="Set"),x=0,y=0)
  widget_control,label,SET_UVALUE={xw:xw,yw:yw}
  widget_control,new_state.coordw,SET_VALUE={X:0,Y:0}
  widget_control, widget_info(mainbase, /CHILD), set_uvalue=new_state,/no_copy
  RETURN, mainbase
end
