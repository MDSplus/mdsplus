c==========================================================================
C MDSPTDATA - adapted from IPTREAD
C IPTREAD   CM Greenfield   24-FEB-1993
C                Modified   03-MAR-1993
C Generic subroutine to call PTDATA and return data for arbitrary
C signal names. 
C
C To just get headers without data, use shot = -shot
C
C For DFI's which do not have the "new multiple domain timing system,"
C an error will be returned unless the DFI information has been included
C in subroutine DFI_DECODE, below.
C
C CAUTION - This has not been tested for every pointname! Use at your
C           own risk!
C
C The PTDATA system is documented in DOC:PTDATA.DOC and DOC:DFI_*.DOC
C==========================================================================
C
C NPT = IPTREAD(SHOT,POINTNAME,F,T,N,SDATE,STIME,IER)
C
C Variable descriptions:
C
C IPTREAD    I*4   Returns number of data points read
C
C SHOT       I*4   Shot number (input) - negative to just read header
C POINTNAME  C*10  Pointname (input)
C F          R*4   Data (output)
C T          R*4   Timebase (output) in msec
C N          I*4   Maximum number of points, dimension of F and T (input)
C SDATE      C*10  Shot date DDNN-YYYY (output)
C STIME      C*8   Shot time HH:MM:SS (output)
C IER        I*4   PTDATA error code. See PTDATA_ERRORS, below.
C
C Other data is available, to access it include READ_PTDATA.INC in the 
C calling program.
C==========================================================================
	integer*4 function MDSPTREAD(shot,pointname,f,t,n,sdate,stime,
     >        units,ier)
	entry MDSPTREAD_(shot,pointname,f,t,n,sdate,stime,
     >        units,ier)
c	integer*4 function MDSPTREAD(shot,pointname,f,t,n,sdate,stime,ier)
c	implicit none
	integer n
	include 'mdsptdata.inc'
	integer shot
	character pointname*10, sdate*10, stime*8
	real t(n), f(n)
	integer*4 DFI_DECODE, ier
	character*4 units
C Array for conversion of dates to useful format
	character*3 months(12)
	data months / 	'JAN', 'FEB', 'MAR', 'APR', 'MAY', 'JUN', 
     1			'JUL', 'AUG', 'SEP', 'OCT', 'NOV', 'DEC' /
C First try call type 12. If that's invalid for the DFI, repeat with type 2
	parameter (ntype=2)
	integer*4 type(0:ntype)
	data type /6,12,2/

	MDSPTREAD = 0

	ier     = 35
	itype   = 0

	do while ((ier.eq.35).and.(itype.lt.ntype))
	    itype = itype + 1
	    if(shot.lt.0)then	! Read header only
		itype=0
		IARRAY.NREQ  = 0
	    else			! Read header and data
		IARRAY.NREQ  = min(LEN_DATA,n)  ! NOTE!!!! If n>LEN_DATA, PTDATA is only asked for LEN_DATA points, 
c						! but user is asking for n points.  Therefore trap error in TDI function
	    endif
	    IARRAY.START = 1
	    IARRAY.INC   = 1
	    ASCII.NREQ   = LEN_HDR
	    INT16.NREQ   = LEN_HDR
	    INT32.NREQ   = LEN_HDR
	    REAL32.NREQ  = LEN_HDR
	    call ptdata(type(itype),jiabs(shot),%ref('.PLA'), 
     2                  %ref(pointname),
     1			data,ier,iarray,rarray,ascii,int16,int32,real32)

C This should NEVER happen, but just in case...
	    if((itype.eq.0).and.(ier.eq.35))itype=ntype

	enddo


C Write data and time to SDATE and STIME
	write(SDATE,100)IARRAY.DAY, months(IARRAY.MONTH), IARRAY.YEAR
	write(STIME,110)IARRAY.HOUR, IARRAY.MINUTE, IARRAY.SECOND

C If PTDATA call failed, or we just wanted to read headers, then return

	if ((shot.lt.0).or.((ier.ne.0).and.(ier.ne.4))) return
C Else:
C If "multiple domain timing" info not include, perform special
C handling depending on the DFI
	if (itype.eq.2) then
	    MDSptread=DFI_DECODE(f,t,n,ier1)
c	    if (ier1.ne.0) ier=ier1
c	    return
	    if (ier1.eq.-1) then
c	       call GETDAT(shot,pointname,1,ier,t,f,n,-10000,10000,0,0,0)
c	       write (7,*) "GETDAT CALL: ",ier
c	       return
	       do i=1,MIN(n,iarray.nret)                              !added
		  rarray.time(i) = RARRAY.STHED + (i-1)*RARRAY.DTHED  !added
	       enddo                                                  !added
	       if (rarray.in.eq.0.0) rarray.in = 1.0                  !3/21
	       if (rarray.rcg.eq.0.0) rarray.rcg = 1.0
	    else                                                      !added
	       if (ier1.ne.0) then                                    !added 
		  ier=ier1                                            !added    
		  return                                              !added 
	       endif                                                  !added 
	    endif                                                     !added
	 endif

C Copy timebase data to t array
C Multiply data by calibration factors and treat appropriately
C depending on word size->signed/unsigned and data type
	MDSptread = min(IARRAY.NRET,n)

C	 if(IARRAY.WORD.le.12)then	! If data size LE 12 bits, unsigned
C The documentation says unsigned up to 12 bits, but the only 16 bit data
C I can find, PINJ, looks to be unsigned as well.

	if(IARRAY.WORD.le.16)then	! If data size LE 16 bits, unsigned
	    rsn = -1
	else
	    rsn = +1
	endif
	fac = RARRAY.IN*RARRAY.RCG	! Factor = calibration + RC gain
	
        do i=1,MDSptread

C Is data INTEGER or REAL?
	    if (IARRAY.DTYPE(1:2).EQ.'IN') then
		y = DATA.INT32(i)	! Integer data
	    elseif (IARRAY.DTYPE(1:2).EQ.'RE') then
		y = DATA.REAL32(i)	! Real data
	    endif
	    f(i) = fac*(RARRAY.OFF+rsn*y)
	    t(i) = 1000.0*RARRAY.TIME(i)
	enddo

	units=iarray.units
	return
  100	format(I2.2,a3,'-',I4.4)
  110	format(I2.2,':',I2.2,':',I2.2)
	end


c-------------------------------------------------------------------------------

	integer*4 function DFI_DECODE(f,t,n,ier)
C Data Format Index (DFI) specific PTDATA decoding
C ============================================================
C List of supported DFI's
	parameter (SPRED_  =  147)
	parameter (LOGFNR  =  155)
	parameter (BOLOCAL = 1143)
	parameter (PRAD    = 1144)
C ============================================================
	integer n
	real f(n), t(n)
	include 'mdsptdata.inc'
	integer SPRED_DECODE
	logical okay
	ier = 0
	if(IARRAY.DFI.eq.SPRED_)then
	 DFI_DECODE = SPRED_DECODE(t,f,n)
	 if(DFI_DECODE.lt.0)ier=DFI_DECODE
	elseif(IARRAY.DFI.eq.LOGFNR)then
	 DFI_DECODE = min(n,IARRAY.NRET/2)
	  do i=1,DFI_DECODE
	    t(i) = DATA.REAL32(i)
	    f(i) = DATA.REAL32(i+DFI_DECODE)
	  enddo
	elseif((IARRAY.DFI.eq.BOLOCAL).or.(IARRAY.DFI.eq.PRAD))then
	 DFI_DECODE = min(n,IARRAY.NRET/2)
	  do i=1,DFI_DECODE
	    t(i) = DATA.REAL32(i+DFI_DECODE)
	    f(i) = DATA.REAL32(i)
	  enddo
	else
	   DFI_DECODE=0
	   ier = -1
	   return
	endif
C Cut off extra times at end of data arrays
	i=2
	okay = .TRUE.
		do while ((i.le.DFI_DECODE).and.okay)
		okay = t(i).gt.t(i-1)
		if(okay)i=i+1
		enddo
	DFI_DECODE=i-1
	return
	end

c-------------------------------------------------------------------------------

	subroutine PTDATA_ERRMES(ier)
	entry PTDATA_ERRMES_(ier)
	parameter (n=1)
	include 'mdsptdata.inc'
	character string*80
		if(ier.eq.-1)then
		 print 100,IARRAY.DFI
		elseif(ier.ge.0)then
		 call MDSPTERROR(ier,string)
		 call str$trim(string,string,len)
		 print 110,ier,string(1:len)
		endif
	return
  100	format(' UNSUPPORTED DFI TYPE: ',i5)
  110	format(' PTDATA ',i2,': ',a)
	end


c-------------------------------------------------------------------------------

C PTDATA_ERRORS
C Returns a string corresponding to a given PTDATA error code
	subroutine MDSPTERROR(error,string)
	entry MDSPTERROR_(error,string)
	character string*50
	integer error
	if(error.lt.0)then
	 string='error in DFI_DECODE'
	elseif(error.eq.0)then
	 string='no errors'
	elseif(error.eq.1)then
	 string='pointname does not exist for this shot'
	elseif(error.eq.3)then
	 string='shot-source not found on disk'
	elseif(error.eq.4)then
	 string='fewer data points returned than requested.'
	elseif(error.eq.5)then
	 string='unrecoverable file access error'
	elseif(error.eq.7)then
	 string='invalid shot number (<0 or >999999)'
	elseif(error.eq.12)then
	 string='shotfile on optical disk'
        elseif(error.eq.13)then
         string='shotfile on tape'
	elseif(error.eq.14)then
	 string='invalid call type'
	elseif(error.eq.20)then
	 string='error allocating/deallocating buffer space for pointname'
	elseif(error.eq.21)then
	 string='error getting node name or unknown node name'
	elseif(error.eq.22)then
	 string='error opening network link connection'
	elseif(error.eq.23)then
	 string='error writing shot number to remote node'
	elseif(error.eq.24)then
	 string='error reading shot status from remote node'
	elseif(error.eq.25)then
	 string='error reading data from remote node'
	elseif(error.eq.30)then
	 string='digitizer or requested delta time .LE. 0'
	elseif(error.eq.31)then
	 string='number of data points requested is less than 1'
	elseif(error.eq.32)then
	 string='invalid integer bits/word in fixed header'
	elseif(error.eq.34)then
	 string='for call type 2,7,12, start or delta point less than 1'
	elseif(error.eq.35)then
	 string='data format index is inconsistent with calling type'
	elseif(error.eq.36)then
	 string='pointname requires special non ptdata timing handling.'
	elseif(error.eq.37)then
	 string='data format index is inconsistent with calling type'
	elseif(error.eq.40)then
	 string='pointname size was too big to be processed.'
	elseif(error.eq.41)then
	 string='pointname size less than fixed header size'
	elseif(error.eq.42)then
	 string='pointname size less than header word count'
	elseif(error.eq.43)then
	 string='fixed+variable header greater than header word count'
	elseif(error.eq.44)then
	 string='one or more variable header counts is negative'
	elseif(error.eq.45)then
	 string='header word size from Modcomp not a multiple of 128'
	elseif(error.eq.50)then
	 string='data file directory inconsistent with fixed header'
	elseif(error.eq.53)then
	 string='invalid data type in fixed header'
	elseif(error.eq.60)then
	 string='Modcomp error - fatal acquisition error. No data.'
	elseif(error.eq.61)then
	 string='Modcomp error - I/O error in writing disk file.'
	elseif(error.eq.62)then
	 string='Modcomp error - database error.'
	elseif(error.eq.63)then
	 string='Modcomp error - processing inconsistencies.'
	elseif(error.eq.64)then
	 string='Modcomp error - transfer error'
	elseif(error.eq.70)then
	 string='Invalid PTDATA version on MULTIFLOW'
	elseif(error.eq.71)then
	 string='MULTIFLOW error opening network link.'
	elseif(error.eq.72)then
	 string='MULTIFLOW error writing to network link.'
	elseif(error.eq.73)then
	 string='MULTIFLOW error reading from network link.'
	elseif(error.eq.74)then
	 string='Allocation error on VAX'
	else
	 string='Unknown error'
	endif
	return
	end

c-------------------------------------------------------------------------------

	integer function SPRED_DECODE(t,f,n)
	integer n
	real t(n), f(n)
	logical str_compare
	character name*10
	include 'mdsptdata.inc'
C First, put the data into the "SPRED" structure
		do j=1,1024
		SPRED.TIME(j) = 1000.0*DATA.REAL32(j)
		enddo
	SPRED.NLINES = INT32.HDR(1)
	SPRED.NPTS   = INT32.HDR(2)
		do i=1,SPRED.NLINES
		n1 = 10*(i-1)+1
		n2 = n1+9
		call STR$TRIM(SPRED.NAME(i),ASCII.HDR(n1:n2),l)
		call STR$UPCASE(SPRED.NAME(i),SPRED.NAME(i))
		n1 = 22*(i-1)+241
		n2 = n1+21
		call STR$TRIM(SPRED.LABEL(i),ASCII.HDR(n1:n2),l)
		 do j=1,SPRED.NPTS
		 SPRED.BRIGHTNESS(j,i) = DATA.REAL32(j+1024*i)
		 enddo
		enddo
C Should we query for a pointname to read?
		if(SPRED_QFLAG)then
		 print 100
		 print 110,(SPRED.NAME(i),i=1,SPRED.NLINES)
		 print 120
C The following line used to be 'accept' instead of 'read'
		 read '(a)',SPRED_NAME
		endif
C Look for SPRED_NAME in the list
	call str$trim(SPRED_NAME,SPRED_NAME,len)
	call str$upcase(SPRED_NAME,SPRED_NAME)
	i=1
		do while ((i.le.SPRED.NLINES).and.
     1			(.not.str_compare(SPRED_NAME,SPRED.NAME(i))))
		i=i+1
		enddo

		if (i.gt.SPRED.NLINES) then
		 print '('' Not found: "'',a,''"'')',SPRED_NAME(1:len)
		 SPRED_IND = -1
		 SPRED_DECODE=-2
		 return
		endif
	SPRED_IND = i
	SPRED_DECODE = SPRED.NPTS
		do j=1,SPRED_DECODE
		t(j) = SPRED.TIME(j)
		f(j) = SPRED.BRIGHTNESS(j,i)
		enddo
	return
  100	format(' The following SPRED names are available: ')
  110	format(6(1x,1a10))
  120	format($,' Enter name >> ')
	end

c-------------------------------------------------------------------------------

	logical function str_compare(str1,str2)
	character str1*(*), str2*(*)
	character*256 a,b
	str_compare = .FALSE.
	call str$trim(a,str1,la)
	call str$trim(b,str2,lb)
	if((la.eq.lb).and.(a.eq.b))str_compare=.TRUE.
	return
	end

	
c-------------------------------------------------------------------------------

	integer*4 function MDSPTNPTS(shot,pointname,ier)
	entry MDSPTNPTS_(shot,pointname,ier)

c	implicit none                   
	parameter (n=1)
	include 'mdsptdata.inc'
	integer shot
	integer*4 word
	character pointname*10
	integer*4 ier
	real frac

	IARRAY.NREQ  = 0

	IARRAY.START = 1
	IARRAY.INC   = 1
	ASCII.NREQ   = LEN_HDR
	INT16.NREQ   = LEN_HDR
	INT32.NREQ   = LEN_HDR
	REAL32.NREQ  = LEN_HDR

	call ptdata(6,shot,%ref('.PLA'),%ref(pointname),
     1		 data,ier,iarray,rarray,ascii,int16,int32,real32)

	word=iarray.word
	if ((word.gt.8).and.(word.lt.16)) word=16

c	10 and 12 bit data counted as 16 bits to calculate number of points
c	example signal: 15LS_V_ACC from NB shot 65310

	frac = 0
	if (word.gt.0) frac = 16./word

c	NBVAC33LT point for examine has iarray.word=8 and iarray.num16=8192
c	for a total number of points = 16384. 

	mdsptnpts = iarray.num16*frac

	return
	end

c----------------------------------------------------------------------------

	subroutine mdsptheadsize(shot,pointname,ier,size)
	entry mdsptheadsize_(shot,pointname,ier,size)

c	implicit none
	parameter (n=1)
	include 'mdsptdata.inc'

	integer shot
	character pointname*10
	integer*4 ier
	integer*4 size(6)

	IARRAY.NREQ  = 0

	IARRAY.START = 1
	IARRAY.INC   = 1
	ASCII.NREQ   = LEN_HDR
	INT16.NREQ   = LEN_HDR
	INT32.NREQ   = LEN_HDR
	REAL32.NREQ  = LEN_HDR

	call ptdata(6,shot,%ref('.PLA'),%ref(pointname),
     1		 data,ier,iarray,rarray,ascii,int16,int32,real32)

	size(1)=iarray.nasch		! character string should be 4* this value
	size(2)=iarray.n16ih
	size(3)=iarray.n32ih
	size(4)=iarray.n32rh
	size(5)=9
	if (iarray.hwords .gt. IFIX_SIZE) then 
	  size(6)=IFIX_SIZE
	else
	  size(6)=iarray.hwords
	end if
	size(7)=iarray.n64rh

	return
	end

c-------------------------------------------------------------------------------

	subroutine mdsptheada(shot,pointname,ier,apass)
	entry mdsptheada_(shot,pointname,ier,apass)

c	implicit none
	parameter (n=1)
	include 'mdsptdata.inc'

	integer shot
	character pointname*10
	integer*4 ier
c	character*(*) apass
	character apass*512

	IARRAY.NREQ  = 0

	IARRAY.START = 1
	IARRAY.INC   = 1
	ASCII.NREQ   = LEN_HDR
	INT16.NREQ   = 0
	INT32.NREQ   = 0
	REAL32.NREQ  = 0

	call ptdata(6,shot,%ref('.PLA'),%ref(pointname),
     1		 data,ier,iarray,rarray,ascii,int16,int32,real32)

	if ((ier.ne.0).and.(ascii.nret.gt.0)) ier=0
	apass=ascii.hdr
	return
	end


c-------------------------------------------------------------------------------

	subroutine mdsptheadi16(shot,pointname,ier,i16pass)
	entry mdsptheadi16_(shot,pointname,ier,i16pass)

c	implicit none
	parameter (n=1)
	include 'mdsptdata.inc'

	integer shot
	character pointname*10
	integer*4 ier
        integer i16pass(*)

	IARRAY.NREQ  = 0

	IARRAY.START = 1
	IARRAY.INC   = 1
	ASCII.NREQ   = 0
	INT16.NREQ   = LEN_HDR
	INT32.NREQ   = 0
	REAL32.NREQ  = 0

	call ptdata(6,shot,%ref('.PLA'),%ref(pointname),
     1		 data,ier,iarray,rarray,ascii,int16,int32,real32)

	if ((ier.ne.0).and.(int16.nret.gt.0)) ier=0
	do i=1,int16.nret
	  i16pass(i)=int16.hdr(i)
	enddo

	return
	end

c-------------------------------------------------------------------------------

	subroutine mdsptheadi32(shot,pointname,ier,i32pass)
	entry mdsptheadi32_(shot,pointname,ier,i32pass)

c	implicit none
	parameter (n=1)
	include 'mdsptdata.inc'

	integer shot
	character pointname*10
	integer*4 ier
        integer*4 i32pass(*)

	IARRAY.NREQ  = 0

	IARRAY.START = 1
	IARRAY.INC   = 1
	ASCII.NREQ   = 0
	INT16.NREQ   = 0
	INT32.NREQ   = LEN_HDR
	REAL32.NREQ  = 0

	call ptdata(6,shot,%ref('.PLA'),%ref(pointname),
     1		 data,ier,iarray,rarray,ascii,int16,int32,real32)

	if ((ier.ne.0).and.(int32.nret.gt.0)) ier=0
	do i=1,int32.nret
	  i32pass(i)=int32.hdr(i)
	enddo

	return
	end


c-------------------------------------------------------------------------------

	subroutine mdsptheadr32(shot,pointname,ier,r32pass)
	entry mdsptheadr32_(shot,pointname,ier,r32pass)

c	implicit none
	parameter (n=1)
	include 'mdsptdata.inc'

	integer shot
	character pointname*10
	integer*4 ier
	real*4 r32pass(*) 

	IARRAY.NREQ  = 0

	IARRAY.START = 1
	IARRAY.INC   = 1
	ASCII.NREQ   = 0
	INT16.NREQ   = 0
	INT32.NREQ   = 0
	REAL32.NREQ  = LEN_HDR

	call ptdata(6,shot,%ref('.PLA'),%ref(pointname),
     1		 data,ier,iarray,rarray,ascii,int16,int32,real32)

	if ((ier.ne.0).and.(real32.nret.gt.0)) ier=0
	do i=1,real32.nret
	   r32pass(i)=real32.hdr(i)
	enddo

	return
	end


c-------------------------------------------------------------------------------

	subroutine mdsptheadifix(shot,pointname,ier,ifixpass)
	entry mdsptheadifix_(shot,pointname,ier,ifixpass)

c	implicit none
	parameter (n=1)
	include 'mdsptdata.inc'

	integer shot
	character pointname*10
	integer*4 ier
	integer*4 ifixpass(*)
	integer ifixloop

c	record /PT_ASCII/ apass
c	record /PT_INT16/ i16pass
c	record /PT_REAL32/ r32pass

	IARRAY.NREQ  = 0

	IARRAY.START = 1
	IARRAY.INC   = 1
	ASCII.NREQ   = LEN_HDR
	INT16.NREQ   = LEN_HDR
	INT32.NREQ   = LEN_HDR
	REAL32.NREQ  = LEN_HDR

	call ptdata(6,shot,%ref('.PLA'),%ref(pointname),
     1		 data,ier,iarray,rarray,ascii,int16,int32,real32)

	if (iarray.hwords .gt. IFIX_SIZE) then 
	  ifixloop=IFIX_SIZE
	else
	  ifixloop=iarray.hwords
	end if

	if ((ier.ne.0).and.(iarray.hwords.gt.0)) ier=0
	do i=1,ifixloop
	  ifixpass(i)=iarray.ifix(i)
	enddo

	return
	end


c-------------------------------------------------------------------------------

	subroutine mdsptheadunits(shot,pointname,ier,units)
	entry mdsptheadunits_(shot,pointname,ier,units)

c	implicit none
	parameter (n=1)
	include 'mdsptdata.inc'

	integer shot
	character pointname*10
	integer*4 ier
	character*4 units

	IARRAY.NREQ  = 0

	IARRAY.START = 1
	IARRAY.INC   = 1
	ASCII.NREQ   = LEN_HDR
	INT16.NREQ   = LEN_HDR
	INT32.NREQ   = LEN_HDR
	REAL32.NREQ  = LEN_HDR

	call ptdata(6,shot,%ref('.PLA'),%ref(pointname),
     1		 data,ier,iarray,rarray,ascii,int16,int32,real32)

	units = iarray.units

	return
	end


c-------------------------------------------------------------------------------

	subroutine mdsptheadrfix(shot,pointname,ier,rfixpass)
	entry mdsptheadrfix_(shot,pointname,ier,rfixpass)

c	implicit none
	parameter (n=1)
	include 'mdsptdata.inc'

	integer shot
	character pointname*10
	integer*4 ier
	real*4 rfixpass(*)

c	record /PT_ASCII/ apass
c	record /PT_INT16/ i16pass
c	record /PT_REAL32/ r32pass

	IARRAY.NREQ  = 0

	IARRAY.START = 1
	IARRAY.INC   = 1
	ASCII.NREQ   = LEN_HDR
	INT16.NREQ   = LEN_HDR
	INT32.NREQ   = LEN_HDR
	REAL32.NREQ  = LEN_HDR

	call ptdata(6,shot,%ref('.PLA'),%ref(pointname),
     1		 data,ier,iarray,rarray,ascii,int16,int32,real32)

	if ((ier.ne.0).and.(rarray.rcg.ne.0.0)) ier=0
	do i=1,9 
	  rfixpass(i)=rarray.rfix(i)
	enddo
	return
	end


c----------------------------------------------------------------------------

	subroutine mdsptcomments(shot,ier,cx) 
	entry mdsptcomments_(shot,ier,cx) 

	include 'mdsptdata.inc'

	integer shot
	integer*4 ier
	integer*2 IDAT(256)
	CHARACTER*512 COMMENTS
	character*512 cx
	equivalence (idat,comments)

	IARRAY.NREQ  = 256

	IARRAY.START = 1
	IARRAY.INC   = 1
	ASCII.NREQ   = LEN_HDR
	INT16.NREQ   = LEN_HDR
	INT32.NREQ   = LEN_HDR
	REAL32.NREQ  = LEN_HDR

	call ptdata(2,shot,%ref('.PLA'),%ref('COMMENTS  '),
     1		 idat,ier,iarray,rarray,ascii,int16,int32,real32)

	do i=1,512
	  cx(i:i) = comments(i:i)
	enddo

	end
	
c----------------------------------------------------------------------------
	program test1

	integer, parameter :: nmax=524288
	external MDSptread
	real t(nmax),f(nmax)
	character pointname*10, sdate*10, stime*8, units*4
	integer shot,ier,npt
	character*512 comments

c	shot=73010
c	pointname='33LS_V_ACC'
	shot=100810
	pointname='PR075JI   '


	npt = mdsptnpts(shot,pointname,ier)

	write (6,*) 'NPT: ',npt
	write (6,*) 'IER: ',ier
	

	npt = mdsptread(shot,pointname,f,t,npt,sdate,stime,units,ier)

	write (6,*) 'NPT: ',npt
	write (6,*) 'IER: ',ier
	write (6,*) 'UNITS: ',units

	if (npt.gt.100) npt=100

	do i=1,npt
	  write (6,*) i,f(i),t(i)
	enddo

	call mdsptcomments(shot,ier,comments)
	write (6,*) 'COMMENTS: ',comments

	end			

c----------------------------------------------------------------------------
	subroutine test2

	integer shot
	character pointname*10
	integer*4 ier
        integer i16pass(10)
	integer*4 i32pass(1)
	real*4   r32pass(26) 
	real*4 rfixpass(9)
	integer*4 ifixpass(50)
	character*30 apass
	integer size(7)
	character *4 units

	shot=100810
	pointname='RDP30JI   '
	call mdsptheadsize(shot,pointname,ier,size)
	print *,'SIZE: ',size

 	call mdsptheadA(shot,pointname,ier,apass)
        call mdsptheadi16(shot,pointname,ier,i16pass)
        call mdsptheadi32(shot,pointname,ier,i32pass)
        call mdsptheadr32(shot,pointname,ier,r32pass)
	print *,'R32PASS: ',r32pass
 	call mdsptheadrfix(shot,pointname,ier,rfixpass)
 	call mdsptheadifix(shot,pointname,ier,ifixpass)
	call mdsptheadr32(shot,pointname,ier,r32pass)
	
	print *,'SHOT: ',shot,'  POINTNAME: ',pointname
	print *,'IER: ',ier
        print *,'APASS: ',apass
        print *,'R32PASS: ',r32pass
	print *,'IFIXPASS: ',ifixpass
	print *,'RFIXPASS: ',rfixpass
	print *,'I16PASS: ',i16pass

	units='    '
	call mdsptheadunits(shot,pointname,ier,units)
	print *,'UNITS: -->',units,'<--',ier

	end
