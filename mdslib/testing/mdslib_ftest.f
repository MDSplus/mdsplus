      program mdslib_test

      implicit none

      integer descr, MdsValue, MdsOpen, MdsPut

      integer IDTYPE_LONG, IDTYPE_FLOAT, IDTYPE_CSTRING

      parameter (IDTYPE_LONG=8, IDTYPE_FLOAT=10, IDTYPE_CSTRING=14)

      integer dsc
      integer sts
      integer i,j
      integer iresult
      integer nx,ny
      integer size
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

      dsc = descr(IDTYPE_FLOAT,result,0,0)
      sts = MdsValue("1.23"//CHAR(0),dsc,0,size)
      if (abs(result-1.23) .gt. 1e-5) then
        write (6,*) "MdsValue('1.23') : ",result,sts
        stop 1
      end if

      dsc = descr(IDTYPE_FLOAT,resultarr,5,0)
      sts = MdsValue("2.2 : 8. : 2."//CHAR(0),dsc,0,size)
      if (abs(resultarr(1)-2.2) .gt. 1e-5) then
        write (6,*) "MdsValue('2. : 40. : 2.') : ",resultarr,sts,size
        stop 1
      end if

      dsc = descr(IDTYPE_LONG,iresult,0)
      sts = MdsValue("_=SetEnv('test_path=.')"//CHAR(0),dsc,0,size)
      if (and(and(sts, iresult), 1) .ne. 1) then
        write (6,*) "setenv(): ",iresult,sts,size
        stop 1
      end if
      sts = MdsValue("TreeOpenNew('test',1)"//CHAR(0),dsc,0,size)
      if (and(and(sts, iresult), 1) .ne. 1) then
        write (6,*) "TreeOpenNew(): ",iresult,sts,size
        stop 1
      end if
      sts = MdsValue("TreeAddNode('A',_,'ANY')"//CHAR(0),dsc,0,size)
      if (and(and(sts, iresult), 1) .ne. 1) then
        write (6,*) "TreeAddNode: ",iresult,sts,size
        stop 1
      end if
      sts = MdsValue("TreeWrite()"//CHAR(0),dsc,0,size)
      if (and(and(sts, iresult), 1) .ne. 1) then
        write (6,*) "TreeWrite(): ",iresult,sts,size
        stop 1
      end if
      sts = MdsValue("TreeClose()"//CHAR(0),dsc,0,size)
      if (and(and(sts, iresult), 1) .ne. 1) then
        write (6,*) "TreeClose(): ",iresult,sts,size
        stop 1
      end if

      dsc = descr(IDTYPE_CSTRING,cresult,0,12)
      sts = MdsValue("$EXPT"//CHAR(0),dsc,0,size)
      if (and(sts, 1) .eq. 1) then
        write (6,*) "MdsValue($EXPT)/*before*/: ",cresult,sts,size
        stop 1
      end if

      sts = MdsOpen("test"//CHAR(0),1)
      if (and(sts, 1) .ne. 1) then
        write (6,*) "MdsOpen('test',1) :", sts
        stop 1
      end if

      dsc = descr(IDTYPE_CSTRING,cresult,0,12)
      sts = MdsValue("$EXPT"//CHAR(0),dsc,0,size)
      if (and(sts, 1) .ne. 1) then
        write (6,*) "MdsValue($EXPT)/*after*/: ",cresult,sts,size
        stop 1
      end if

      dsc = descr(IDTYPE_LONG,42042,0)
      sts = MdsPut("A"//CHAR(0),"$"//CHAR(0),dsc,0)
      if (and(sts, 1) .ne. 1) then
        write (6,*) "MdsPut('A',$,42042): ",sts
        stop 1
      end if

      dsc = descr(IDTYPE_LONG,iresult,0)
      sts = MdsValue("A"//CHAR(0),dsc,0,size)
      if (and(sts, 1) .ne. 1) then
        write (6,*) "MdsValue('A')/*i*/: ",iresult,sts
        stop 1
      end if

      dsc = descr(IDTYPE_FLOAT,resultarr,20,0)
      do i=1,20
         resultarr(i) = 0.
      enddo
      sts = MdsPut("A"//CHAR(0),"$"//CHAR(0),dsc,0)
      if (and(sts, 1) .ne. 1) then
        write (6,*) "MdsPut('A','$',array) : ",sts
        stop 1
      end if

      sts = MdsValue("A"//CHAR(0),dsc,0,size)
      if (and(sts, 1) .ne. 1) then
        write (6,*) "MdsValue('A')/*/array*/: ",resultarr,sts,size
        stop 1
      end if

      dsc = descr(IDTYPE_FLOAT,array2d,nx,ny,0)
      sts = MdsPut("A"//CHAR(0),"$"//CHAR(0),dsc,0)
      if (and(sts, 1) .ne. 1) then
        write (6,*) "MdsPut('A',$,array2d) : ",sts
        stop 1
      end if

      sts = MdsValue("A"//CHAR(0),dsc,0,size)
      if (and(sts, 1) .ne. 1) then
        write (6,*) "MdsValue('A')/*/array2d*/: ",resultarr,sts,size
        stop 1
      end if

      end
