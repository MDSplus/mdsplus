; support for the ORNL CAMAC routines as called by IDL
pro CAMV2
;+
;CAM$V2 defines the system variables used by IDL version 2 for CAMAC.
;
;MODULE	read scalar	logical name or key (unsupported)
;A	read scalar	subaddress 0-15
;F	read scalar	function 0-31
;TC	read scalar	transfer count
;		1-16383 longs or 1-32767 words
;		optional for defined data array in block mode
;DATA	write		f(0-7) new read values, optional
;	read		f(16-23) written values
;		scalar for PIO, vector for block.
;IOSB	read/write 4-word vector status
;KEY	read/write scalar	quick module name (unsupported)
;XSTATE	read scalar	tested X: 1=yes 0=no else=undefined
;QSTATE	read scalar	tested Q: 1=yes 0=no else=undefined
;		Fortran CAM$CSTATE wants CAM$QDC/QNE/QE/XI/XE
;		CCL's SET XANDQ defaults to ANY
;TIMEOUT read scalar	seconds to wait for LAM 1-32767
;MEM	read scalar
;
;	Ken Klare, LANL CTR-7	(c)1990
;	Limitation: KEY would require descriptor of long
;	Limitation: EFN requires by-value.
;		Event Flags useful only for overlap I/O.
;	It needs LIB$GET_EF/FREE_EF interface.
;	LAM waits using AST routines cannot be supported by IDL
;	CANLAMW LAMASTW and routines without W-wait
;	List commands are not supported:
;	DCLST EXECUTEW LPIO LQSCAN LRTQ LSTATE LSTOP RDATA WDATA
;-
	defsysv,'!CAM_IOSB',[0,0,0,0]
;	defsysv,'!CAM$LMEM',24
	defsysv,'!CAM_MEM',24
	defsysv,'!CAM_QSTATE',1L
	defsysv,'!CAM_XSTATE',1L
end
