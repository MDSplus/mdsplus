      Integer*4 Function libgetvm(bytes, addr, zone)
	  Integer*4 LibGetVm
	  libgetvm = LibGetVm(bytes,addr,zone)
	  End

	  Subroutine libfreevm(bytes, addr, zone)
	  Call LibFreeVm(bytes,addr,zone)
	  End