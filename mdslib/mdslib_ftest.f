c23456789012345678901234567890123456789012345678901234567890123456789012
c
c
c  Note on VMS this must be compiled with /ASSUME=NOBACKSLASH
c

      program mdslib_test

      implicit none 

      include 'mdslib.inc'
      integer dsc,dsc2
      integer status
      integer i,j
      integer iresult
      integer nx,ny
      integer size
      parameter (nx=2, ny=13)
      real    result
      real    resultarr(20)
      real    array2d(nx,ny)
      character cresult*12,ctag*50
      
      do i=1,nx
         do j=1,ny
            array2d(i,j) = i*10 + j
         enddo
      enddo


c      call MdsConnect('alpha1.psfc.mit.edu')

      dsc = descr(IDTYPE_FLOAT,result,0)
      status = MdsValue('1.'//CHAR(0),dsc,0,)
      write (6,*) 'MdsValue("1.") : ',result,status

      dsc = descr(IDTYPE_FLOAT,resultarr,20,0)
      status = MdsValue('2. : 40. : 2.'//CHAR(0),dsc,0,size)
      write (6,*) 'MdsValue("2. : 40. : 2.") : ',resultarr,status,size

      dsc = descr(IDTYPE_CSTRING,cresult,0,12)
      status = MdsValue('$EXPT'//CHAR(0),dsc,0,size)
      write (6,*) 'MdsValue($EXPT): ',cresult,status,size

      status = MdsOpen('main'//CHAR(0),-1)
      write (6,*) 'MdsOpen("main",-1) :', status

      dsc = descr(IDTYPE_CSTRING,cresult,0,12)
      status = MdsValue('$EXPT'//CHAR(0),dsc,0,size)
      write (6,*) 'MdsValue($EXPT): ',cresult,status,size

      dsc = descr(IDTYPE_LONG,42042,0)
      status = MdsPut('NUMERIC'//CHAR(0),'$',dsc,0)
      write (6,*) 'MdsPut("NUMERIC",$,42042): ',status 

      dsc = descr(IDTYPE_LONG,iresult,0)
      status = MdsValue('NUMERIC'//CHAR(0),dsc,0,)
      write (6,*) 'MdsValue("NUMERIC"): ',iresult,status

      dsc = descr(IDTYPE_FLOAT,resultarr,20,0)
      status = MdsPut('NUMERIC'//CHAR(0),'$',dsc,0)
      write (6,*) 'MdsPut("NUMERIC","$",array) : ',status
      do i=1,20
         resultarr(i) = 0.
      enddo

      status = MdsValue('NUMERIC'//CHAR(0),dsc,0,size)
      write (6,*) 'MdsValue("NUMERIC"): ' ,resultarr, status, size

      dsc = descr(IDTYPE_FLOAT,array2d,nx,ny,0)
      status = MdsPut('NUMERIC'//CHAR(0),'$',dsc,0)
      write (6,*) 'MdsPut("NUMERIC",$,array2d) : ',status

      status = MdsSetDefault('\\MAIN::TOP'//CHAR(0))
      write (6,*) 'MdsSetDefault("\\MAIN::TOP"): ',status

c      call MdsDisconnect
c23456789012345678901234567890123456789012345678901234567890123456789012
      end
