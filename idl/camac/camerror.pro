function CAMERROR,XSTATE,QSTATE,IOSB
;+
;CAMERROR test the X and Q states for an error, 1 is an error.
;XSTATE/QSTATE is 0 for off, 1 for on, other for untested.
;-
	if (n_elements(IOSB) ne 4L) then IOSB = !CAM_IOSB
	if (n_elements(XSTATE) ne 1L) then if (XSTATE ge 0) then if (XSTATE ne -CAMX(IOSB)) then return,1b
	if (n_elements(QSTATE) ne 1L) then if (QSTATE ge 0) then if (QSTATE ne -CAMQ(IOSB)) then return,1b
	return,0b
end
