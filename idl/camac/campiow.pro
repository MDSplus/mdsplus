pro CAMPIOW,MODULE,A,F,DATA,MEM,IOSB
;+
;CAMPIOW does single programmed input/output.
;-
	if (n_elements(DATA) ne 1L) then DATA = 0L
	if (n_elements(MEM) ne 1L) then MEM = !CAM_MEM
	if (MEM ne 24) then if (MEM ne 16) then MEM = !CAM_MEM
	if (MEM eq 16) then DATA = fix(DATA) else DATA = long(DATA)
        piostat = MdsValue('_data=$,_iosb=zero(4,0w),CamShr->CamPiow($,val($),val($),ref(_data),val($),ref(_iosb))',$
                  data,module,a,f,mem,status=status,/quiet)
        !cam_iosb = MdsValue('_iosb');
	IOSB = !CAM_IOSB
        data = MdsValue('_data');
	CAMCHECK,piostat
	if (!CAM_XSTATE le 0L) then if ((!CAM_XSTATE and 1) ne CamX(IOSB)) then $
		message,string('module,A,F,bad_X: ',MODULE,A,F,CamX(IOSB))
	if (!CAM_QSTATE le 0L) then if ((!CAM_QSTATE and 1) ne CamQ(IOSB)) then $
		message,string('module,A,F,bad_Q: ',MODULE,A,F,CamQ(IOSB))
end
