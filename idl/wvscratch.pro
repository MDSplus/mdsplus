pro wvscratch_event, ev
  case (tag_names(ev, /STRUCTURE_NAME)) of
  "WIDGET_BUTTON": begin
        WIDGET_CONTROL, ev.id, get_value=button
        case (button) of
        "Dismiss": begin
                  WIDGET_CONTROL, /DESTROY, ev.top
                end
        "Quit": begin
                  WIDGET_CONTROL, /DESTROY, ev.top
                end
        "Scratch": begin
               buttons = ['Dismiss']
               extr_buttons = ['Flip']
		  w = cw_wvedit(rows=[1],user_buttons=buttons, editable=1, button_ids = button_ids,event_proc='wvscratch_event',$
                                 /TLB_SIZE_EVENTS, menu_buttons = extr_buttons)
		  WIDGET_CONTROL,w,SET_VALUE={XINCREASING:1}
		  WIDGET_CONTROL,w, set_value={XMINDISTANCE:.002,LOWX:-4.0}
		  WIDGET_CONTROL, w, /REALIZE
                  XMANAGER,'wvscratch',w,group=ev.top
		end
        else: 
        endcase
        end
  "PASTE_CB": begin
        WIDGET_CONTROL,ev.id,SET_VALUE={idx:ev.idx}
        WIDGET_CONTROL,ev.id,GET_VALUE=wave
        n = n_elements(wave.x)
        ys = wave.y
        print,"PASTE_CB event received"
        if (n gt 1000) then ys=median(ys,31)
        if (n gt 32) then begin
          error = .02 * (max(ys) - min(ys))
          x = fltarr(n)
          y = x
          w = x
          k = 0l
          if (k ne 0) then begin
            s = bytarr(k+1)+1
            WIDGET_CONTROL, ev.id, set_value = {IDX:ev.idx, X:x(0:k), Y:y(0:k), PEN:s, SELECTIONS:s, STEPPLOT:0}
          endif
        endif
        end
  "WIDGET_BASE": wvedit_resize,ev
  "USER_CB" : begin
        case (ev.label) of
        "Flip": begin
                  widget_control, ev.id, get_value = wave
                  widget_control, ev.id, set_value = {idx:ev.idx, x:wave.x, y:-1*wave.y, selections:wave.selections, pen:wave.pen}
                end
        else: 
        endcase
        end
  else: 
  endcase
end

pro wvscratch,group=group
  base = widget_base(/COLUMN)
  button = widget_button(base,value="Scratch")
  button = widget_button(base,value="Quit")
  WIDGET_CONTROL, base, /REALIZE
  XMANAGER, 'wvscratch',base,group=group
  RETURN
END
