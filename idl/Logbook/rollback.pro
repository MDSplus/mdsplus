pro rollback
  if (get_database() eq 'RDB') then $
    cnt = DSQL("ROLLBACK")
end
