function CAMX,IOSB
;+
;CAMX tests latest or a specific status block for X response.
;-
	if (n_elements(IOSB) ne 4L) then IOSB = !CAM_IOSB
        val = MdsValue('CamShr->CamX($)',IOSB,/quiet,status=status)
        if (status) and (val) then return,1b
	return,0b
end
