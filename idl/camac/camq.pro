function CAMQ,IOSB
;+
;CAMQ tests latest or a specific status block for Q response.
;-
	if (n_elements(IOSB) ne 4L) then IOSB = !CAM_IOSB
        val = MdsValue('CamShr->CamQ($)',iosb,status=status,/quiet)
        if (status) and (val) then return,1b
	return,0b
end
