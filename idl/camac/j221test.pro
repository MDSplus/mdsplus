;
; j221test.pro
;
;
;  To invoke:
;
;  IDL> j221test,cts-name [,reps=n] [,/noremcam]
;
;  Where:
;
;    cts-name = The name of the j221, i.e. 'MOL_J221'
;    n        = The number of repetitions, defaults to 10
;    
;    /noremcam = by default the procedure will invoke remcam
;                to load module logical names. If the names
;                have already been loaded use /noremcam to
;                skip this operation.
;  
;
;  Writes 1024x12bit and 1024x24bit data to Jorway 221 module
;  and reads is back and compares. The writes and reads are
;  done using stop,fstop,qrep and qstop modes. The data contains the
;  following bit formations which should toggle each bit many times:
;
;  data(0) = bit 1 set all other clear
;  data(1) = bit 2 set all other clear
;  ...
;  data(bits) = all bits set
;  data(bits+1) = all bits set except highest bit
;  data(bits+2) = all bits set except highest two bits
;  ...
;  The above series repeats as many times as will fit in
;  the 1024 samples. The remaining samples are all bits
;  clear.
;
;  If errors are detected, a summary of the errors by bit position
;  is presented.
;
pro CheckData,in,out,func
  bad = where(in ne out,count)    ; See if memory is the same
  if count ne 0 then begin        ; If errors
    if (size(in))(2) eq 2 then bits=12 else bits=24
    print,'Memory mismatch on '+strtrim(bits,2)+'-bit memory using '+func+'  '+strtrim(count,2)+'/1024 bad' ;  print error message
    badbits = intarr(bits)
    for j=0,bits-1 do begin
      mask = 2l ^ j
      print,string(fix(total((in(bad) and mask) ne (out(bad) and mask))),format='(I4)')+' have bit '+ $
               string(j,format='(I2)')+' mismatched'
    endfor
  endif
  in = in(n_elements(in)-indgen(n_elements(in))-1) ; reverse data for next test
  return
end

function make_data,bits
  if (bits eq 12) then begin
    d = intarr(1024)
    sbits = 'fff'x
  endif else begin
    d=lonarr(1024)
    sbits = 'ffffff'x
  endelse
  i = indgen(bits)
  groups = 1024/2/bits
  for j=0,groups-1 do begin
    d(i+j*bits*2) = 2l ^ i
    d(i+bits+j*bits*2l) = sbits/(2l ^ i)
  endfor
  return,d
end  

pro j221test,name,reps=reps,noremcam=noremcam
  if n_elements(reps) eq 0 then reps = 10
;  if not keyword_set(noremcam) then begin
;    mds$tcl,'set library sys$library:ccl$library'
;    mds$tcl,'remcam'
;  endif
  camv2
  d12=make_data(12)
  d24=make_data(24)
  for i=0,reps-1 do begin
    campiow,name,0,9                ; Reset module
;
;   Write and read 12 bit memory using stopw
;
    campiow,name,2,16,0,16          ; Reset memory address register
    camstopw,name,0,16,1024,d12,16  ; Write data to 12 bit memory
    campiow,name,2,16,0,16          ; Reset memory address register
    camstopw,name,0,0,1024,t12,16   ; Read data from 12 bit memory
    CheckData,d12,t12,'stop'         ; Check data
;
;   Write and read 12 bit memory using fstopw
;
;    campiow,name,2,16,0,16          ; Reset memory address register
;    camfstopw,name,0,16,1024,d12,16  ; Write data to 12 bit memory
;    campiow,name,2,16,0,16          ; Reset memory address register
;    camfstopw,name,0,0,1024,t12,16   ; Read data from 12 bit memory
;    CheckData,d12,t12,'fstop'       ; Check data
;
;   Write and read 24 bit memory using stopw
;
    campiow,name,2,16,0,16          ; Reset memory address register
    camstopw,name,1,16,1024,d24,24  ; Write data to 24 bit memory
    campiow,name,2,16,0,16          ; Reset memory address register
    camstopw,name,1,0,1024,t24,24   ; Read data from 24 bit memory
    CheckData,d24,t24,'stop'         ; Check data
;
;   Write and read 24 bit memory using fstopw
;
;    campiow,name,2,16,0,16          ; Reset memory address register
;    camfstopw,name,1,16,1024,d24,24  ; Write data to 24 bit memory
;    campiow,name,2,16,0,16          ; Reset memory address register
;    camfstopw,name,1,0,1024,t24,24   ; Read data from 24 bit memory
;    CheckData,d24,t24,'fstop'       ; Check data
;
;   Write and read 24 bit memory using qrepw
;
    campiow,name,2,16,0,16          ; Reset memory address register
    camqrepw,name,1,16,1024,d24,24  ; Write data to 24 bit memory
    campiow,name,2,16,0,16          ; Reset memory address register
    camqrepw,name,1,0,1024,t24,24   ; Read data from 24 bit memory
    CheckData,d24,t24,'qrep'         ; Check data
;
;   Write and read 24 bit memory using fqrepw
;
;    campiow,name,2,16,0,16          ; Reset memory address register
;    camfqrepw,name,1,16,1024,d24,24  ; Write data to 24 bit memory
;    campiow,name,2,16,0,16          ; Reset memory address register
;    camfqrepw,name,1,0,1024,t24,24   ; Read data from 24 bit memory
;    CheckData,d24,t24,'fqrep'         ; Check data
;
;   Write and read 24 bit memory using qstopw
;
    campiow,name,2,16,0,16          ; Reset memory address register
    camqstopw,name,1,16,1024,d24,24 ; Write data to 24 bit memory
    campiow,name,2,16,0,16          ; Reset memory address register
    camqstopw,name,1,0,1024,t24,24  ; Read data from 24 bit memory
    CheckData,d24,t24,'qstop'        ; Check data
;
;   Write and read 24 bit memory using fqstopw
;
;    campiow,name,2,16,0,16          ; Reset memory address register
;    camfqstopw,name,1,16,1024,d24,24 ; Write data to 24 bit memory
;    campiow,name,2,16,0,16          ; Reset memory address register
;    camfqstopw,name,1,0,1024,t24,24  ; Read data from 24 bit memory
;    CheckData,d24,t24,'qstop'        ; Check data
  endfor
end
