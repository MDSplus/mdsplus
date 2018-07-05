
c	mdslib.inc

c	Fortran include file for MdsLib


c23456789012345678901234567890123456789012345678901234567890123456789012
	integer descr, MdsValue, MdsConnect, MdsOpen, MdsPut, MdsClose
	integer MdsSetDefault, MdsDisconnect
	integer MdsConnectR, MdsValueR, MdsOpenR, MdsPutR, MdsCloseR
	integer MdsSetDefaultR, MdsDisconnectR,MdsValue2R,MdsPut2R
	integer descr2, MdsValue2, MdsPut2, MdsSetSocket
        integer IDTYPE_UNSIGNED_BYTE, IDTYPE_BYTE
        integer IDTYPE_UNSIGNED_SHORT, IDTYPE_SHORT
        integer IDTYPE_UNSIGNED_LONG, IDTYPE_LONG
	integer IDTYPE_UNSIGNED_LONGLONG, IDTYPE_LONGLONG
	integer IDTYPE_FLOAT, IDTYPE_DOUBLE, IDTYPE_COMPLEX
	integer IDTYPE_DOUBLE_COMPLEX, IDTYPE_CSTRING
        parameter (IDTYPE_UNSIGNED_BYTE=2, IDTYPE_BYTE=6)
        parameter (IDTYPE_UNSIGNED_SHORT=3, IDTYPE_SHORT=7)
        parameter (IDTYPE_UNSIGNED_LONG=4, IDTYPE_LONG=8)
	parameter (IDTYPE_UNSIGNED_LONGLONG=5, IDTYPE_LONGLONG=8)
	parameter (IDTYPE_FLOAT=10, IDTYPE_DOUBLE=11, IDTYPE_COMPLEX=12)
	parameter (IDTYPE_DOUBLE_COMPLEX=13, IDTYPE_CSTRING=14)



