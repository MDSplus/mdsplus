function mdsevent_getevi,id
  widget_control,id,get_uvalue=event_struct
  event_stuff = bytarr(56)
  MdsMemCpy, event_stuff, event_struct, 56
  e = {mds_event_info,stub_id:long(event_stuff,0),   $
                         base_id:long(event_stuff,4), $
                         event_id:long(event_stuff,8), $
                         pid:long(event_stuff,12), $
                         name:strtrim(event_stuff(16:43)), $
                         data:event_stuff(44:55), $
                         event_ptr:event_struct}
  return,e
end

function mdsevent_func,ev
  e = mdsevent_getevi(ev.id)
  return,{mds_event,id:ev.id,top:ev.top,handler:ev.handler,event_info:e}
end

pro mdsevent_can,id
  e = mdsevent_getevi(id)
  dummy=call_external('IDLMDSEVENT','IDLMdsEventCan',mds$socket(),e.event_id,value=[1,1])
;  dummy=call_external(decw$image(/xt),'XtFree',e.event_ptr,value=[1])
  return
end

function mdsevent,parent,name
  stub = widget_base(parent)
  event_struct=call_external('IDLMDSEVENT','IDLMdsEvent',mds$socket(), parent, stub, name, value=[1,0,0,1])
;  widget_control,stub,set_uvalue=event_struct,event_func='mdsevent_func'
  widget_control,stub,kill_notify='mdsevent_can',set_uvalue=event_struct,event_func='mdsevent_func'
  return, stub
end
