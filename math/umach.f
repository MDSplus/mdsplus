!      Ken Klare, LANL CTR-7      (c)1990
!***********************************************************************
!***************** Machine-Dependent Constants *************************
!-----------------------------------------------------------------------
      Integer Function IMACH(i)
      Integer      i      !input, item number
!+machine-dependent integer parameters
      Integer mach(12)
      Data mach /
     1 32,            !bits per integer storage unit
     2 4,             !characters per integer
     3 2,             !integer base
     4 31,            !integer digits
     5 0,             !largest integer
     6 2,             !single base
     7 24,            !single digits
     8 -127,          !smallest single exp
     9 127,           !largest single exp
     1 56,            !double digits
     1 -127,          !smallest double exp
     2 127/           !largest double exp
      If (i.gt.0.and.i.le.12) Then
         mach(5)=int(x7fffffff)
         IMACH = mach(i)
      Else
            IMACH = 0
      Endif
      End
!-----------------------------------------------------------------------
      Real Function AMACH(i)
      Integer i      !input, item number
!+machine-dependent single parameters
      Real      xmach(8)
      Integer      mach(8)
      Equivalence (xmach,mach)
      If (i.gt.0.and.i.le.8) Then
        mach(1)=int(x00000080)         !smallest number
        mach(2)=int(xfffe7fff)       !largest number
        mach(3)=int(x00003480)            !base**(-number_of_digits)
        mach(4)=int(x00003500)            !base**(1-number_of_digits)
        mach(5)=0                       !log10(base)
        mach(6)=int(xffff7fff)            !not-a-number
        mach(7)=int(xfffe7fff)      !positive infinity
        mach(8)=int(xfffeffff)      !negative infinity
        If (i.eq.5.and.mach(5).eq.0) xmach(5) = alog10(2.0e0)
        AMACH = real(xmach(i))
      Else
         AMACH = 0
      Endif
      End
!-----------------------------------------------------------------------
      Real(KIND=8) Function DMACH(i)
      Integer i      !input, item number
!+machine-dependent double parameters
      Real(KIND=8)      xmach(8)
      Integer      mach(2,8)
      Equivalence (xmach,mach)
c      Data mach/
c     1 x00000080,0,            !smallest number
c     2 xffff7fff,xfffeffff,      !largest number
c     3 x00002480,0,            !base**(-number_of_digits)
c     4 x00002500,0,            !base**(1-number_of_digits)
c     5 0,0,                        !log10(base)
c     6 xffff7fff,xffffffff,      !not-a-number or bigger than infinity
c     7 xffff7fff,xfffeffff,      !positive infinity
c     8 xffffffff,xfffeffff/      !negative infinity
      If (i.gt.0.and.i.le.8) Then
         mach(1,1)=int(x00000080)         !smallest number
         mach(2,1)=0
         mach(1,2)=int(xffff7fff)
         mach(2,2)=int(xfffeffff) !largest number
         mach(1,3)=int(x00002480)
         mach(2,3)=0                      !base**(-number_of_digits)
         mach(1,4)=int(x00002500)
         mach(2,4)=0         !base**(1-number_of_digits)
         mach(1,5)=0
         mach(2,5)=0                    !log10(base)
         mach(1,6)=int(xffff7fff)
         mach(2,6)=int(xffffffff)      !not-a-number or bigger than infinity
         mach(1,7)=int(xffff7fff)
         mach(2,7)=int(xfffeffff)      !positive infinity
         mach(1,8)=int(xffffffff)
         mach(2,8)=int(xfffeffff)      !negative infinity

         If (i.eq.5.and.mach(1,5).eq.0) xmach(5) = dlog10(2.0d0)
         DMACH = Real(xmach(i))
      Else
            DMACH = 0
      Endif
      End
!-----------------------------------------------------------------------
      Subroutine UMACH(n,nunit)
      Integer      n      !1=input 2=output +=get -=set
      Integer      nunit      !the IO number
!+machine-dependent IO parameters
      Integer in,out
      Data      in,out/-4,-1/
      If (n.eq.1) Then
            nunit = in
      Elseif (n.eq.2) Then
            nunit = out
      Elseif (n.eq.-1) Then
            in = nunit
      Elseif (n.eq.-2) Then
            out = nunit
      Endif
      End
!-----------------------------------------------------------------------
      Subroutine ERSET()
!+Dummy error set routine
      End
