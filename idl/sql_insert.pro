function sql_insert, table, p_key, s_key=s_key, quiet=quiet, debug=debug
  if Not keyword_set(quiet) then quiet = 0l
  if Not keyword_set(debug) then debug = 0l
  status = 0L
  i_cmd = 'Insert into '+table+' ('
  p_keys = tag_names(p_key)
  if (n_tags(s_key) gt 0) then begin
    s_keys = tag_names(s_key)
    keys = [p_keys, s_keys]
  endif else $
    keys = p_keys

  for i = 0, N_ELEMENTS(keys)-1 do begin
    if i eq 0 then $
      i_cmd = i_cmd + keys(i) $
    else $
      i_cmd = i_cmd + ','+keys(i)
  endfor
  i_cmd = i_cmd + ') values ('
  for i = 0, N_ELEMENTS(keys) - 1 do begin
    if i eq 0 then $
      i_cmd = i_cmd + '?' $
    else $
      i_cmd = i_cmd + ',?'
  endfor
  i_cmd = i_cmd + ')'

  idl_expr = 'count = CALL_FUNCTION("DSQL",i_cmd'
  for i = 0, n_elements(p_keys)-1 do begin
    idl_expr = idl_expr + ',p_key.'+p_keys(i)    
  endfor
  if (n_tags(s_key) gt 0) then begin
    for i=0, n_tags(s_key)-1 do begin
      idl_expr = idl_expr + ',s_key.'+s_keys(i)
    endfor
  endif
  idl_expr = idl_expr+',quiet='+string(quiet, format='(I1.1)')+')'

  if debug then begin
    print, ' '
    print, 'Insert into the table '+table
    print, i_cmd
    print, idl_expr
  endif
  status = execute(idl_expr)
  status = 1L
  return, status
end
