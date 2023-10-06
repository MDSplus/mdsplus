; This function detects if an optional keyword is present, regardless of value.
; IDL's keyword_set() only detects non-zero optional keywords (see Issue 2625).
; So replace it with this function that also handles zero.
function mds_keyword_set,socket=socket
  if (n_elements(socket) gt 0) then begin
    return, 1
  endif else begin
    return, 0
  endelse
end
