C *******************  CM Greenfield   24-FEB-1993
C * READ_PTDATA.INC *  Revised         03-MAR-1993
C *******************
C
C Data structure definitions for PTDATA
C See DOC:PTDATA.DOC
c	parameter (LEN_DATA=262140)	! Length of data array in bytes
	parameter (LEN_DATA=524288)	! Length of data array in bytes
	parameter (LEN_HDR=255)		! Number of words for headers
C======================================================================
C Define structure for DATA
c	parameter (LEN_DATA_4=LEN_DATA/4)
	parameter (LEN_DATA_4=65535)
	parameter (IFIX_SIZE=50)
	STRUCTURE /PT_DATA/
	 UNION
	  MAP
	   CHARACTER ASCII*(LEN_DATA_4)
	   CHARACTER ASCII2*(LEN_DATA_4)
	   CHARACTER ASCII3*(LEN_DATA_4)
	   CHARACTER ASCII4*(LEN_DATA_4)
	  END MAP
	  MAP
c	   INTEGER*4 INT32(LEN_DATA_4)
	   INTEGER*4 INT32(LEN_DATA)
c	   INTEGER*4 INT32(n)
	  END MAP
	  MAP
c	   REAL*4    REAL32(LEN_DATA_4)
	   REAL*4    REAL32(LEN_DATA)
c	   REAL*4    REAL32(n)
	  END MAP
	 END UNION
	END STRUCTURE

	RECORD /PT_DATA/ DATA
	COMMON /PT$$001/ DATA
C======================================================================
C Define structure for IARRAY
	STRUCTURE /PT_IARRAY/
	UNION
	MAP
	 INTEGER*4	NREQ,	! Number of data points requested
     1			NRET,   ! Number of data points returned
     2			START,	! Data start point for calls of type 2,7,12
     3			INC,	! Increment between data points
     4			HFI,	! Header format index
     5			HWORDS 	! Number of 16 bit words in header
      	 CHARACTER*4 	EXPTR,	! Experimentor
     1			PLTYPE 	! Plasma type
      	 INTEGER*4	SHOT 	! Shot number
       	 CHARACTER*4	EXPN,	! Experiment name - ASCII
     1			PHASE 	! Phase - ASCII
	 CHARACTER*12	POINT	! Pointname
	 INTEGER*4	HOUR,  	! Time of shot: hour
     1			MINUTE,	!               minute
     2			SECOND,	!               second
     3			MONTH,	! Date of shot: month
     4			DAY,    !               day
     5			YEAR,   !               year
     6			PTTYPE 	! Point type - UNUSED ON VAX
     	 CHARACTER*24	PDESC	! Point description
	 CHARACTER*4	UNITS	! Engineering units
	 INTEGER*4	REV,	! Number of revisions to pointname
     1			OFF,	! Zero offset
     2			DFI,	! Data format index
     3			NUM16,	! Number of words of 16 bit data
     4			WORD 	! Number of bits in data word
	 CHARACTER*4	DTYPE	! Data type  - 'IN  ','RE  ','AS  '
     	 INTEGER*4	CVAL,	! Compression value 0=un, 17=comp
     1			NASCH,	! Number of words from ASCII variable header
     2			N16IH,	! Number of words from INT*2 variable header
     3 			N32IH,	! Number of words from INT*4 variable header
     4			N32RH,   ! Number of words from REAL*4 variable header
     5                  N64RH   ! Number of words from REAL*8 variable header
	INTEGER*4  	DUMMY(10)  ! future use
	END MAP
	MAP
	  INTEGER*4  IFIX(IFIX_SIZE)
	END MAP
	END UNION
	END STRUCTURE

	RECORD /PT_IARRAY/ IARRAY
	COMMON /PT$$002/ IARRAY
C======================================================================
C Define structure for RARRAY
	STRUCTURE /PT_RARRAY/
	UNION
	MAP
	 REAL*4		STREQ,	! Requested start time (seconds)
     1			DTREQ,	! Requested delta time (seconds)
     2			DTACT,	! Delta time actually used (seconds)
     3			IN,	! Inherent number - calibration factor#1
     4			RCG,	! RC gain - calibration factor#2
     5			OFF,	! Zero offset - same as IARRAY.OFF
     6			STACT,	! Start time actually used (seconds)
     7			STHED,	! Start time of data from fixed header
     8			DTHED,	! Delta time of data from fixed header
     9			UNUSED(10:20),
c     A			TIME(LEN_DATA_4)	! Time array in seconds
     A			TIME(LEN_DATA)	! Time array in seconds
c     A			TIME(n)	! Time array in seconds
	END MAP
	MAP
	    REAL*4 RFIX(LEN_DATA+19)
	END MAP
	END UNION
	END STRUCTURE

	RECORD /PT_RARRAY/ RARRAY
	COMMON /PT$$003/ RARRAY
C======================================================================
C Define ASCII header
	parameter (NASC_HDR=2*LEN_HDR)
	STRUCTURE /PT_ASCII/
	 INTEGER*4	NREQ,	! Number of words requested
     1			NRET	! Number of words returned
	 CHARACTER*2	HDR*(NASC_HDR)	! ASCII header
	END STRUCTURE

	RECORD /PT_ASCII/ ASCII
	COMMON /PT$$004/ ASCII
C======================================================================
C Define INT16 header
	STRUCTURE /PT_INT16/
	 INTEGER*4	NREQ,	! Number of words requested
     1			NRET,	! Number of words returned
     2			HDR(LEN_HDR)	! INT16 header
	END STRUCTURE

	RECORD /PT_INT16/ INT16
	COMMON /PT$$005/ INT16
C======================================================================
C Define INT32 header
	STRUCTURE /PT_INT32/
	 INTEGER*4	NREQ,	! Number of words requested
     1			NRET,	! Number of words returned
     2			HDR(LEN_HDR)	! INT32 header
	END STRUCTURE

	RECORD /PT_INT32/ INT32
	COMMON /PT$$006/ INT32
C======================================================================
C Define REAL32 header
	STRUCTURE /PT_REAL32/
	 REAL*4		NREQ,	! Number of words requested
     1			NRET,	! Number of words returned
     2			HDR(LEN_HDR)	! REAL32 header
	END STRUCTURE

	RECORD /PT_REAL32/ REAL32
	COMMON /PT$$007/ REAL32
C======================================================================
C======================================================================
C
C PFI SPECIFIC DATA STRUCTURES
C
C======================================================================
C SPRED: PFI #147
	STRUCTURE /IPTREAD_SPRED/
	 CHARACTER*10 NAME(24)          ! 10 character name of wavelength array
                                        ! (used to identify/extract desired subset
                                        ! of data from full data block)
	 CHARACTER*22 LABEL(24)         ! 22 character array label for plot;
                                        ! LABEL contains additional identifying
                                        ! information
	 REAL*4	TIME(1024),             ! time (msec)
     1		BRIGHTNESS(1024,24)     ! up to 1024 data points for up to 24 spectral lines

	 INTEGER*4	NLINES,         ! Number of lines (max. 24)
     1			NPTS            ! Number of points for each line (max. 1024)
	END STRUCTURE

	RECORD /IPTREAD_SPRED/ SPRED

	LOGICAL SPRED_QFLAG             ! If TRUE, user will be queried for SPRED pointname. 
	CHARACTER*10 SPRED_NAME         ! Name of line to use
	INTEGER SPRED_IND

	COMMON /PT$010/ SPRED, SPRED_QFLAG, SPRED_IND, SPRED_NAME
