;pro mdstcl,command,output=output,status=status



pro MdsTcl,command,output=output,status=status,quiet=quiet
  forward_function MdsValue
  if n_elements(command) ne 0 then begin
    status = MdsValue('Tcl($,_output)',command)
    output = MdsValue('_output')
    if (not arg_present(output)) and (strlen(output) gt 0) then print,output
    if not status then begin
      msg = mdsvalue('getmsg($)',status)
      if keyword_set(quiet) then $
        message,msg,/continue,/noprint $
      else $
        message,msg,/continue
      endif
  endif else begin
    command = ""
    while (strlen(strcompress(command)) eq 0) or $
          (strcompress(strupcase(command)) ne strmid("EXIT",0,strlen(strcompress(command)))) do begin
      if command ne "" then MdsTcl,command,status=status,quiet=quiet
      read,command,prompt="TCL> "
    endwhile
  endelse
  return
end
