pro MdsCheckArg,arg,name=name,type=type
  on_error,2
  if float(strmid(!version.release,0,3)) ge 5.1 then begin
    info = execute("size(arg,/structure)")
    if (info.type_name ne type) then message,"Argument /"+name+"/ must be type "+type,/noname,/noprefix
    if (info.n_dimensions ne 0) then message,"Argument /"+name+"/ must be a scalar",/noname,/noprefix
  endif
  return
end

