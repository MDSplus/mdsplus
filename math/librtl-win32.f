      Integer*4 Function libgetvm_(bytes, addr, zone)
	  Integer*4 LibGetVm
	  libgetvm = LibGetVm(bytes,addr,zone)
	  End

	  Subroutine libfreevm_(bytes, addr, zone)
	  Call LibFreeVm(bytes,addr,zone)
	  End