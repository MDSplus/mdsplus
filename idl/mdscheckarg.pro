pro MdsCheckArg,arg,name=name,type=type
  on_error,2
  info = size(arg,/structure)
  if (info.type_name ne type) then message,"Argument /"+name+"/ must be type "+type,/noname,/noprefix
  if (info.n_dimensions ne 0) then message,"Argument /"+name+"/ must be a scalar",/noname,/noprefix
  return
end

