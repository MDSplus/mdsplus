function get_database
  ans = ''
  defsysv,'!DATABASE',exists=test
  if (test) then $
    stat = execute('ANS = !DATABASE.dbtype')
  return, ans
end
