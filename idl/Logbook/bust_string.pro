function bust_string, str, max
  len = strlen(str)
  if (len lt max) then return, str

  idx = strpos(str, ' ', max)
  if (idx eq -1) then return, str

  return, [strmid(str, 0, idx), bust_string(strmid(str, idx, len-idx+1), max)]
end
