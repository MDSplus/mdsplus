;function mdsevent, parent, event-name
;widget_control, id, value=event_struct
;mdsevent_can, id
;
;+
; NAME:
;	MDSEVENT
; PURPOSE:
;       Set up an MDS Event ast on an event using the IDL's widget_stub infrastructure
;       to deliver the AST to the application.
; CATEGORY:
;	MDSPLUS/IP
; CALLING SEQUENCE:
;	id = MDSEvent(parent-widget, event-name)
;   WIDGET_CONTROL, id, get_value=event-struct
;   MDSEvent_Can, id
;
; INPUT PARAMETERS:
;		parent-widget - widget id of the parent.
;       event-name - name of the event to fire this event for.
; OPTIONAL INPUT PARAMETERS:
;       NONE
; Keywords:
;       NONE
; OUTPUTS:
;       returns the widget-id of the dummy widget used for delivering the events.
; 		event-struct - 	returned from get_value
;    					{mds_event_info,
;							stub_id:long,   $
;                        	base_id:long, $
;                        	event_id:long, $
;                         	pid:long, $
;                         	name:string, $
;                         	data:bytearr(12), $
;                         	event_ptr:event_struct}
;
; COMMON BLOCKS:
;	None.
; SIDE EFFECTS:
;	None.
; RESTRICTIONS:
;	MDSConnect to the host which is serving the events must be done
;	prior to calling this routine.
; PROCEDURE:
;	1 Create a stub widget to deliver the event asts through
;   2 Create a stub child of (1) to hold the data structures
;     for the event in its user value.
;   3 Setup the event with a call external
;   4 Attach widget handling routines to (1) and (2) and put the data
;     structure returned by (3) into the uservalue of (2)
;   - In the destroy routine cancel the events
;   - In the event function build a standard widget event with the
;     info on the event that occurred.
;   - In the get_value function return the info on the event.
;
; MODIFICATION HISTORY:
;   Original TWF from X-Windows Implementation
;   Ported to Windows JAS
;   Extra widget for uservalue Jeff Schachter
;   4/25/00 - GetValue JAS
;-
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

function mdsevent_get_value, id
  return, mdsevent_getevi(widget_info(id,/child))
end

pro mdsevent_can,id
  forward_function mds$socket
    widget_control, id, get_value=e
    dummy=call_external(EventImage(),'IDLMdsEventCan',mds$socket(),e.event_id,value=[1,1])
  ;  dummy=call_external(decw$image(/xt),'XtFree',e.event_ptr,value=[1])
  return
end

pro mdsevent_cleanup, id
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
  widget_control,stub,event_func='mdsevent_func', func_get_value='mdsevent_get_value'
  widget_control,ss,set_uvalue=event_struct,kill_notify='mdsevent_cleanup'  ;; kill_notify on this widget so still have access to uvalue
  if (keyword_set(uvalue)) then widget_control,stub,set_uvalue=uvalue
  return, stub
end
