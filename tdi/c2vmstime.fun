/*
; NAME:		C2VMSTIME
; PURPOSE:	Converts a C longword time to a VMS quadword time
; CATEGORY:	MDSplus
; CALLING SEQUENCE:	vms_quadword_time=C2VMSTIME(c_longword_time)
; INPUTS:
;	c_longword_time  longword time given in seconds since 1-JAN-1970 00:00:00
*/
FUN	PUBLIC	C2VMSTIME(IN		_ctime) {
        return(0x7c95674beb4000q + _ctime * 10000000q);
}
