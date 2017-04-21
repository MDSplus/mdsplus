      SUBROUTINE stderr(message)
! "@(#) stderr writes a message to standard error using a standard f2003 method"
        USE ISO_FORTRAN_ENV, ONLY : ERROR_UNIT ! access computing environment
        CHARACTER(LEN=*) :: message
        WRITE(ERROR_UNIT,'(a)')message ! write message to standard error
      END SUBROUTINE stderr


      SUBROUTINE GN()
      call stderr("The GN routine has been removed from MdsMath")
      call stderr("library. The original code had undefined behavior")
      call stderr("If the GN routine is needed you might explore")
      call stderr("the following URLS to obtain an updated")
      call stderr("implementation. However the call arguments will")
      call stderr("need to be adjusted:")
      call stderr("")
      call stderr("   http://www.netlib.org/misc/gn/GN_ReadMe.pdf")
      call stderr("   http://www.netlib.org/misc/gn/GN.FOR")
      END SUBROUTINE GN
