;pro mdstcl,command,output=output,status=status



pro MdsTcl,command,output=output,status=status,quiet=quiet
  forward_function MdsValue
  if n_elements(command) ne 0 then begin
    status = MdsValue('Tcl($,public _output)',command)
    output = MdsValue('if_error(public _output,"")')
    if strlen(output) gt 0 then begin
      output = str_sep(output,string([10b]))
      if not arg_present(output) then for i=0,(size(output))[1]-1 do print,output[i]
    endif
    s=size(status)
    if (s[s[0]+1] eq 7) then status=0
    if not status then begin
      msg = mdsvalue('getmsg($)',/quiet,status)
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
