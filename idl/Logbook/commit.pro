pro commit
  if (get_database() eq 'RDB') then begin
    cnt = DSQL("COMMIT", status = status)
    if (status NE 1) then rollback
  endif
end
