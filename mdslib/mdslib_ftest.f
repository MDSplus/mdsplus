c23456789012345678901234567890123456789012345678901234567890123456789012
c
c
c  Note on VMS this must be compiled with /ASSUME=NOBACKSLASH
c

      program mdslib_test

      implicit none 

      include 'mdslib.inc'
      integer dsc
      integer status
      integer i,j
      integer iresult
      integer nx,ny,nxx,nyy
      integer size
      parameter (nx=2, ny=13, nxx=3, nyy=15)
      real    result
      real    resultarr(30)
      real    array2d(nx,ny)
      real    array2dck(nxx,nyy)
      real    array2dx(nx,ny)
      character cresult*25,cresult2*3
      character*20 ctest(20)
      character*3 ctest2(20)
      
      do i=1,nx
         do j=1,ny
            array2d(i,j) = i*100 + j
         enddo
      enddo


c   COMMENT OUT THE NEXT LINE TO TEST LOCAL ACCESS
      call MdsConnect('gemini.gat.com')

      dsc = descr(IDTYPE_FLOAT,result,0)
      status = MdsValue('1.'//CHAR(0),dsc,0,)
      write (6,*) 'MdsValue("1.") : ',result,status

      dsc = descr(IDTYPE_FLOAT,resultarr,30,0)
      status = MdsValue('2. : 40. : 2.'//CHAR(0),dsc,0,size)
      write (6,*) 'MdsValue("2. : 40. : 2.") : ',resultarr,status,
     >   "        LENGTH: ",size

      dsc = descr(IDTYPE_CSTRING,cresult,0,25)
      status = MdsValue('$EXPT'//CHAR(0),dsc,0,size)
      write (6,*) 'MdsValue($EXPT): ',cresult,status,
     >   "        LENGTH: ",size

      status = MdsOpen('main'//CHAR(0),-1)
      write (6,*) 'MdsOpen("main",-1) :', status

      dsc = descr(IDTYPE_CSTRING,cresult,0,25)
      status = MdsValue('$EXPT'//CHAR(0),dsc,0,size)
      write (6,*) 'MdsValue($EXPT) (len=25): ',cresult,'<<<',status,
     >   "        LENGTH: ",size

      dsc = descr(IDTYPE_CSTRING,cresult2,0,3)
      status = MdsValue('$EXPT'//CHAR(0),dsc,0,size)
      write (6,*) 'MdsValue($EXPT) (len=3): ',cresult2,'<<<',status,
     >   "        LENGTH: ",size

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

      dsc = descr(IDTYPE_FLOAT,array2d,nx,ny,0)
      status = MdsPut('NUMERIC'//CHAR(0),'$',dsc,0)
      write (6,*) 'MdsPut("NUMERIC",$,array2d) : ',status

      dsc = descr(IDTYPE_FLOAT,array2dx,nx,ny,0)
      status = MdsValue('NUMERIC'//CHAR(0),dsc,0,size)
      write (6,*) 'MdsValue(2x13): ',status,
     >   "        LENGTH: ",size
      do i=1,nx
        do j=1,ny
          write (6,*) 'Exact MDSvalue: ',i,j,array2dx(i,j)
        enddo
      enddo

 
      dsc = descr(IDTYPE_FLOAT,array2dck,nxx,nyy,0)
      status = MdsValue('NUMERIC'//CHAR(0),dsc,0,size)
      write (6,*) 'MdsValue(3x15): ',status,
     >   "        LENGTH: ",size
      do i=1,nxx
        do j=1,nyy
          write (6,*) 'Padded MDSvalue: ',i,j,array2dck(i,j)
        enddo
      enddo

      dsc = descr(IDTYPE_CSTRING,ctest,20,0,20)
      status = MdsValue('GETNCI("*","NODE_NAME")',dsc,0,size)
      write (6,*) "MdsValue(GETNCI(*,NODE_NAME)): ",status,
     >    " LENGTH: ",size
      do i=1,20
         write (6,*) 'NODENAME: ',ctest(i),'<<<'
      enddo
      
      dsc = descr(IDTYPE_CSTRING,ctest2,20,0,3)
      status = MdsValue('GETNCI("*","NODE_NAME")',dsc,0,size)
      write (6,*) "MdsValue(GETNCI(*,NODE_NAME)): ",status,
     >    " LENGTH: ",size
      do i=1,20
         write (6,*) 'NODENAME: ',ctest2(i),'<<<'
      enddo
      
      status = MdsSetDefault('\MAIN::TOP'//CHAR(0))
      write (6,*) 'MdsSetDefault("\MAIN::TOP"): ',status

c   COMMENT OUT THE NEXT LINE TO TEST LOCAL ACCESS
      call MdsDisconnect
c23456789012345678901234567890123456789012345678901234567890123456789012
      end
