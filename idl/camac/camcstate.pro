pro CAMCSTATE,EFN,XSTATE,QSTATE,MEM
;+
;CAMCSTATE sets the default settings of the CAMAC Information Block.
;-
	if (n_elements(XSTATE) eq 1L) then if (abs(XSTATE) le 1) then !CAM_XSTATE = -long(XSTATE)
	if (n_elements(QSTATE) eq 1L) then if (abs(QSTATE) le 1) then !CAM_QSTATE = -long(QSTATE)
	if (n_elements(MEM) eq 1L) then if (MEM eq 16 or MEM eq 24) then !CAM_MEM = fix(MEM)
end
