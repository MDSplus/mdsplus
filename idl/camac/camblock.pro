pro CAMBLOCK,entry,MODULE,A,F,TC,DATA,MEM,IOSB
	if (n_elements(TC) ne 1L) then TC = 0L
	if (TC le 0) then begin
		sz = size(DATA)
		TC = sz(sz(0L)+2L)
	end
	if (n_elements(MEM) ne 1L) then MEM = !CAM_MEM
	if (MEM ne 24) then if (MEM ne 16) then MEM = !CAM_MEM
	if (MEM eq 16) then bytes = 2L else bytes = 4L
;	if (bytes*TC gt 65535L) then message,string('module,tc >65535 bytes: ',MODULE,TC)
	if (F lt 16) then begin
		DATA = 0b
		if (MEM eq 16) then tmp = intarr(TC,/nozero) else tmp = lonarr(TC,/nozero)
	end else begin
		sz = size(DATA)
		if (sz(sz(0L)+2L) lt TC) then TC = sz(sz(0L)+2L)
		if (MEM eq 16) then tmp = fix(DATA) else tmp = long(DATA)
	end
	IOSB = !CAM_IOSB
        camstat = MdsValue('_data=$,_iosb=zero(4,0w),build_call(8,"CamShr",$,$,val($),val($),val($),ref(_data),val($),ref(_iosb))',$
                tmp,entry,module,a,f,tc,mem)
	CAMCHECK,camstat
	!CAM_IOSB = MdsValue('_IOSB')
        IOSB = !CAM_IOSB
        tmp = MdsValue('_data')
	if (F lt 16) then begin
		nelem = (LONG(!CAM_IOSB(1) and 65535L) + (!CAM_IOSB(3)*65536L))/bytes
		if (nelem eq 0L or (not IOSB[0])) then DATA = 0b $
		else DATA = tmp(0L:nelem-1L)
	end
	;*** CSTATE does not seem to enforce X or Q but we do on request ***
	;note: CAM$X and CAM$Q give -1 for true
	if (!CAM_XSTATE le 0L) then if ((!CAM_XSTATE and 1) ne CAMX(IOSB)) then $
		message,string(entry,' module,A,F,bad_X: ',MODULE,A,F,CamX(IOSB))
	if (!CAM_QSTATE le 0L) then if ((!CAM_QSTATE and 1) ne CAMQ(IOSB)) then $
		message,string(entry,' module,A,F,bad_Q: ',MODULE,A,F,CAMQ(IOSB))
end
