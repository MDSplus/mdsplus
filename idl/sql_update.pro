;+
; NAME:		SQL_UPDATE
; PURPOSE:	Update (optionally insert) a record in a time slice table
; CATEGORY:	MDSPLUS - SQL
; CALLING SEQUENCE:
; sql_update, table,  p_key, values=values, [, key=key][, master_table=master_table]
;               [, status=status] [,/quiet] [,/debug] [,mask=mask] [,/CREATE]
;	table - 	name of the table to update
;       p_key -         struct containing the record key for the primary table.
;                       for example: {shot:999999, time_slice:.05}
;	values=values	struct containing the fields to set 
;			like {ninja:'ninja string', ninja_time:2.2}
;	[key=key]	struct containing the values for the rest of the key of
;			this slice,  for example {side:'in', probe:12}
;       [master_table=master_table] - name of the 'key' table for this table
;                       if not specified then this table is assumed to be a
;                       standalone database table.  Like SUMMARY, or CORE...
;                       see /CREATE below
;	[status=status] optional return status
;	[/QUIET]        if set no messages will be printed out
;	[/DEBUG]	if set various diagnostic messages are printed
;       [MASK=mask]     an array of bytes 1/field in values where
;			-1 -> set this field to null
;			 0 -> leave this field alone
;                        1 -> update this field from values
;       [/CREATE]        if set and no master table is specified then if the 
;                        record to be updated is not present it will be inserted.
;
; KEYWORDS:	See above.
; OUTPUTS:	None except for status
; COMMON BLOCKS: None.
; SIDE EFFECTS:	SQL database opened or accessed.
; RESTRICTIONS:	
;	       -A SET_DATABASE command must have been executed prior to using this
;		routine.  
;	       -The relation specified by the master_table argument must have shot and 
;               time_slice fields
;
; PROCEDURE:	
;       	Select the record to be updated in the table relation
;               if it is not there then 
;                 select the record for this shot, time from the master talbe
;		  if it is not there then
;                   choke
;                 else
;                   insert this record shot, time (and any keys) into the talbe
;               endif
;               update the specified fields in table.
;
; MODIFICATION HISTORY:
;	 April 7, 1994 - Initial version                                             Josh Stillerman 
;	 May 3, 1994 -   add p_key and /CREATE and change name                       Josh Stillerman 
;-
pro sql_update, table, p_key, master_table=master_table, key=key, values=values, mask=mask, $
                status=status, quiet=quiet, debug=debug, create=create
  status = 0l
  if not keyword_set(quiet) then quiet = 0l
  if not keyword_set(debug) then debug = 0l
  sz = size(master_table)
  master = sz(n_elements(sz)-2) eq 7
  if (debug) then if (master) then print, "Master - ",master_table else print, "No master table"
  masking = (N_ELEMENTS(mask) gt 0)
  if (debug) then if (masking) then print, "Masking " else print, "Not Masking"
;
;  first build a selection string
;
  selection = ' where '
  n_pkeys = n_tags(p_key)
  p_keys = tag_names(p_key)
  for i=0, n_pkeys-1 do begin
    if i eq 0 then selection = selection + p_keys(i)+'=? ' else selection = selection + 'and '+p_keys(i)+'=? '
  endfor

  n_keys =  n_tags(key)
  if n_keys gt 0 then begin
    key_names = tag_names(key)
    for i=0, n_keys-1 do begin
      selection = selection + 'and '+key_names(i)+'=?'
    endfor
  endif
;
;  and the update command
;
  n_vals = n_tags(values)
  if n_vals gt 0 then begin
    cmd = 'UPDATE '+table+' set  '
    names = tag_names(values)
    count = 0
    for i=0, n_vals-1 do begin
      if (not masking) then begin
        if (count eq 0) then cmd = cmd +names(i)+'=? ' else cmd = cmd +', '+names(i)+'=? '
        count = count + 1
      endif else begin
        if (mask.(i) eq 1) then begin
          if (count eq 0) then cmd = cmd +names(i)+'=? ' else cmd = cmd +', '+names(i)+'=? '
          count = count + 1
        endif else begin
          if (mask.(i) eq -1) then begin
          if (count eq 0) then cmd = cmd +names(i)+'=NULL ' else cmd = cmd +', '+ names(i)+'=NULL '
          count = count + 1
          endif
        endelse
      endelse
    endfor
    if count eq 0 then begin
      if not quiet then MESSAGE, 'No fields to update...'
      return
    endif
  endif else begin
    if not quiet then MESSAGE, 'No fields to update...'
    return
  endelse
;
;  see if this slice is in this table
;
  select_cmd = "select count(*) from "+table+selection
  idl_expr = 'cnt = CALL_FUNCTION("DSQL",select_cmd'
  for i=0, n_pkeys-1 do begin
    idl_expr = idl_expr + ', p_key.'+p_keys(i)
  endfor
  if n_keys gt 0 then begin
    for i=0, n_keys-1 do begin
      idl_expr = idl_expr + ', key.'+key_names(i)
    endfor
  endif
  idl_expr = idl_expr + ', count, quiet='+string(quiet)+')'
  if debug then begin
    print, "Select the record to update '
    print, idl_expr
    print, select_cmd
  endif
  status = execute(idl_expr)
;
; if it isn't then if there is a master table
; and this slice is in it then insert this slice
; in this table
;
  if (count(0) eq 0) then begin
    if (master) then begin
      sel = ' where '
      for i=0, n_pkeys-1 do begin
        if i eq 0 then sel = sel + p_keys(i)+'=? ' else sel = sel + 'and '+p_keys(i)+'=? '
      endfor
      select_cmd = "select count(*) from "+master_table+sel
      idl_expr = 'cnt = CALL_FUNCTION("DSQL",select_cmd'
      for i=0, n_pkeys-1 do begin
        idl_expr = idl_expr + ', p_key.'+p_keys(i)
      endfor
      idl_expr = idl_expr + ', count, quiet='+string(quiet)+')'

      if debug then begin
        print, ' '
        print, 'Select from the master table ...'
        print, select_cmd
        print, idl_expr
      endif
      status = execute(idl_expr)
      if (count(0) eq 0) then begin
        if not quiet then message, "The requested slice does not exist in the master table"
        return
      endif else begin    
        status = sql_insert(table, p_key, s_key=key, quiet=quiet, debug=debug)
        if (not status) then begin
          if not quiet then message, "Could not create record... aborting"
          return
        endif
      endelse
    endif else begin
;
;   else (not master) if /CREATE then
;     insert this slice
;   else
;     error
;
      if (keyword_set(create)) then begin
        status = sql_insert(table, p_key, s_key=key, quiet=quiet, debug=debug)
        if (not status) then begin
          if not quiet then message, "Could not create record... aborting"
          return
        endif
      endif else begin
        if not quiet then message, "The slice  does not exist use /CREATE first"
        return
      endelse
    endelse   
  endif
;
;  now update the record
;
  cmd = cmd + selection
  idl_expr = 'dummy = CALL_FUNCTION("DSQL", cmd, '
  for i=0, n_vals-1 do begin
    if (not masking) then begin
      idl_expr = idl_expr +' values.'+names(i)+', '
    endif else begin
      if (mask.(i) eq 1) then begin
         idl_expr = idl_expr +' values.'+names(i)+', '
      endif 
    endelse
  endfor
  for i=0, n_pkeys-1 do begin
    if i eq 0 then idl_expr = idl_expr + 'p_key.'+p_keys(i) else  idl_expr = idl_expr + ',p_key.'+p_keys(i)
  endfor
  if n_keys gt 0 then begin
    for i=0, n_keys-1 do begin
      idl_expr = idl_expr + ', key.'+key_names(i)
    endfor
  endif
  idl_expr = idl_expr + ', status = status, quiet=quiet)'
  if debug then begin
    print, ' '
    print, 'update the record ...'
    print, idl_expr
    print, cmd
  endif
  dummy = execute(idl_expr)
  return
end
