function EventImage
  case !version.os of
  'OSF' : ans = 'libIdlMdsEvent.so'
  'sunos' : ans = 'libIdlMdsEvent.so'
  'Win32' : ans = 'IDLMDSEVENT'
  'hp-ux' : begin
        file=''
        path = getenv('SHLIB_PATH')
        if (strlen(path) gt 0) then begin
          paths = str_sep(path, ':')
          i = 0
          while i lt n_elements(paths) and strlen(file) eq 0 do begin
            file = findfile(paths(i)+'/libIdlMdsEvent.sl', count=count)
            if (count gt 0) then file = file(0)
            i = i + 1
          end
        endif
        if strlen(file) eq 0 then $
          file = (findfile('/usr/local/lib/libIdlMdsEvent.sl'))[0]
        if strlen(file) eq 0 then $
           message, 'Could not find libIdlMdsEvent.sl in SHLIB_PATH'
        ans = file
      end
   else : ans = 'NotImplemented'
  endcase
  return, ans
end

function PointerSize
  case !version.os of
  'OSF' : ans = 8
  'sunos' : ans = 4
  'hp-ux' : ans = 4
  else  : ans = 4
  endcase
  return, ans
end
 
function mdsevent_getevi,id
  widget_control,id,get_uvalue=event_struct
  if PointerSize() eq 4 then begin
    event_stuff = bytarr(56)
    MdsMemCpy, event_stuff, event_struct, 56
    e = {mds_event_info,stub_id:long(event_stuff,0),   $
                         base_id:long(event_stuff,4), $
                         event_id:long(event_stuff,8), $
                         pid:long(event_stuff,12), $
                         name:strtrim(event_stuff(16:43)), $
                         data:event_stuff(44:55), $
                         event_ptr:event_struct}
  endif else begin
    event_stuff = bytarr(60)
    MdsMemCpy, event_stuff, event_struct, 60
    e = {mds_event_info,stub_id:long(event_stuff,0),   $
                         base_id:long(event_stuff,4), $
                         event_id:long64(event_stuff,8), $
                         pid:long(event_stuff,16), $
                         name:strtrim(event_stuff(20:47)), $
                         data:event_stuff(48:59), $
                         event_ptr:event_struct}
  endelse
  return,e
end

function mdsevent_func,ev
  e = mdsevent_getevi(widget_info(ev.id,/child))
  return,{mds_event,id:ev.id,top:ev.top,handler:ev.handler,event_info:e}
end

pro mdsevent_can,id
  forward_function mds$socket
    e = mdsevent_getevi(id)
    dummy=call_external(EventImage(),'IDLMdsEventCan',mds$socket(),e.event_id,value=[1,1])
  ;  dummy=call_external(decw$image(/xt),'XtFree',e.event_ptr,value=[1])
  return
end

function mdsevent,parent,name,uvalue=uvalue
  forward_function mds$socket
  stub = widget_base(parent)
  ss = widget_base(stub)  ;; this child widget will hold the event_struct as the user value
  event_struct=call_external(EventImage(),'IDLMdsEvent',mds$socket(), parent, stub, name, value=[1,0,0,1])
  widget_control,stub,event_func='mdsevent_func'
  widget_control,ss,set_uvalue=event_struct,kill_notify='mdsevent_can'  ;; kill_notify on this widget so still have access to uvalue
  if (keyword_set(uvalue)) then widget_control,stub,set_uvalue=uvalue
  return, stub
end
