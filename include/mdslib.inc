
c	mdslib.inc

c	Fortran include file for MdsLib

	integer descr, MdsValue, MdsConnect, MdsOpen, MdsPut, MdsClose
        integer MdsSetDefault, MdsDisconnect
	integer IDTYPE_UCHAR, IDTYPE_USHORT
	integer IDTYPE_ULONG, IDTYPE_ULONGLONG
	integer IDTYPE_CHAR, IDTYPE_SHORT
	integer IDTYPE_LONG, IDTYPE_LONGLONG
	integer IDTYPE_FLOAT, IDTYPE_DOUBLE
	integer IDTYPE_COMPLEX, IDTYPE_COMPLEX_DOUBLE
	integer IDTYPE_CSTRING

	parameter (IDTYPE_UCHAR=2, IDTYPE_USHORT=3)
	parameter (IDTYPE_ULONG=4, IDTYPE_ULONGLONG=5)	
	parameter (IDTYPE_CHAR=6, IDTYPE_SHORT=7)
	parameter (IDTYPE_LONG=8, IDTYPE_LONGLONG=9)
	parameter (IDTYPE_FLOAT=10, IDTYPE_DOUBLE=11)
	parameter (IDTYPE_COMPLEX=12, IDTYPE_COMPLEX_DOUBLE=13)
	parameter (IDTYPE_CSTRING=14)


 

