c23456789012345678901234567890123456789012345678901234567890123456789012

      program mdslib_test

      implicit none 

      include 'mdslib.inc'
      integer dsc
      integer status
      integer i,j
      integer iresult
      integer nx,ny
      parameter (nx=2, ny=13)
      real    result
      real    resultarr(20)
      real    array2d(nx,ny)
      character cresult*12
      
      do i=1,nx
         do j=1,ny
            array2d(i,j) = i*10 + j
         enddo
      enddo

      dsc = descr(IDTYPE_FLOAT,result,0)
      status = MdsValue("1."//CHAR(0),dsc,0)
      write (6,*) "MdsValue('1.') : ",result,status

      dsc = descr(IDTYPE_FLOAT,resultarr,20,0)
      status = MdsValue("2. : 40. : 2."//CHAR(0),dsc,0)
      write (6,*) "MdsValue('2. : 40. : 2.') : ",resultarr,status

      dsc = descr(IDTYPE_CSTRING,cresult,0)
      status = MdsValue("$EXPT"//CHAR(0),dsc,0)
      write (6,*) "MdsValue($EXPT): ",cresult,status

      status = MdsOpen("main"//CHAR(0),%val(-1))
      write (6,*) "MdsOpen('main',-1) :", status

      dsc = descr(IDTYPE_CSTRING,cresult,0)
      status = MdsValue("$EXPT"//CHAR(0),dsc,0)
      write (6,*) "MdsValue($EXPT): ",cresult,status

      dsc = descr(IDTYPE_LONG,42042,0)
      status = MdsPut("\TOP:NUMERIC"//CHAR(0),"$",dsc,0)
      write (6,*) "MdsPut('\TOP:NUMERIC',$,42042): ",status

      dsc = descr(IDTYPE_LONG,iresult,0)
      status = MdsValue("\TOP:NUMERIC"//CHAR(0),dsc,0)
      write (6,*) "MdsValue('\TOP:NUMERIC'): ",iresult,status

      dsc = descr(IDTYPE_FLOAT,resultarr,20,0)
      status = MdsPut("\TOP:NUMERIC"//CHAR(0),"$",dsc,0)
      write (6,*) "MdsPut('\TOP:NUMERIC','$',array) : ",status
      do i=1,20
         resultarr(i) = 0.
      enddo

      status = MdsValue("\TOP:NUMERIC"//CHAR(0),dsc,0)
      write (6,*) "MdsValue('\TOP:NUMERIC'): " ,resultarr, status

      dsc = descr(IDTYPE_FLOAT,array2d,nx,ny,0)
      status = MdsPut("\TOP:NUMERIC"//CHAR(0),"$",dsc,0)
      write (6,*) "MdsPut('\TOP:NUMERIC',$,array2d) : ",status

      
      
c23456789012345678901234567890123456789012345678901234567890123456789012
      end
