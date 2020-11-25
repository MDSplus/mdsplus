C*26-NOV-90 11:35:29 MDS$SOURCE:[KKLARE]ZBLAS.FOR;5
! Subprograms based on "LINPACK User's Guide" by J.J. Dongarra et al.
! The Basic Linear Algebra Subprograms portion.
!
! root    prefix-suffix of name     Function
! -AMAX   IS- ID-     IC-     IZ-   index of max |x|  or  max  |x|+|y|
! -ASUM   S-  D-      SC-     DZ-   w := sum |x|  or  sum |x|+|y|
! -AXPY   S-  D-      C-      Z-    w := a*x+y
! -COPY   S-  D-      C-      Z-    y := x
! -DOT-   S-  D-  C-U C-C Z-U Z-C   w:=sum x*y, for -C  w:=conjg(x)*y
! -NRM2   S-  D-      SC-     DZ-   w := sqrt(sum |x|**2)
! -ROT    S-  D-      CS-     ZD-   apply rotation
! -ROTG   S-  D-      C-      Z-    setup Givens rotation
! -SCAL   S-  D-  CS- C-  ZD- Z-    x := ax
! -SWAP   S-  D-      C-      Z-    y :=: x
! -ROTG used by -CHEX and -CHUD.
! S-ROTG and CS-ROT used by C-SVDC etc.
!
! S- are single-precision.
! D- are double-precision.
! C- are single-precision complex.
! Z- are double-precision complex.
!
! N is the active number of elements.
! X and Y are vectors stepped in increments INCX and INCY.
! Dimension 1 is used for these arrays.
! LINPACK uses INCX=INCY=1 and special code for this case.
! Negative INCX or INCY can mean reversed order.
! A, B, C, and S are scalars.
!
!+ index of absolute-value maximum
      Integer Function ICAMAX(n,x,incx)
      Integer i,incx,j,n
      REAL xmax
      COMPLEX x(n*incx)
      If (n.le.0) Then
         ICAMAX=0
      ElseIf (n.eq.1) Then
         ICAMAX=1
      Else
         ICAMAX=1
         xmax=abs(REAL(x(1)))+abs(AIMAG(x(1)))
         If (incx.ne.1) Then
            j=1
            Do i=2,n
               j=j+incx
               If (abs(REAL(x(j)))+abs(AIMAG(x(j))).gt.xmax) Then
                xmax=abs(REAL(x(j)))+abs(AIMAG(x(j)))
                ICAMAX=i
               EndIf
            EndDo
         Else
            Do i=2,n
               If (abs(REAL(x(i)))+abs(AIMAG(x(i))).gt.xmax) Then
                  xmax=abs(REAL(x(i)))+abs(AIMAG(x(i)))
                  ICAMAX=i
               EndIf
            EndDo
         EndIf
      EndIf
      End
!+ Euclidean norm of vector
      REAL Function SCNRM2(n,x,incx)
      REAL SCASUM
      Integer i,incx,n
      REAL cutlo,cuthi,test
      COMPLEX x(1)
! machine precision      smallest-exp      largest-exp
! Prime            S 23      -128         127
! Prime            D 47      -32896(-32768-128) 32639(32767-128)
! Vax            S 24      -128         127
! Vax            D 56      -128         127
! DEC KL10      S 27      -128         127
! DEC KL10      D 62      -128         127 //worst case//
! DEC KI10      D 54      -128         127
! CDC-6600      S 48      -975(-1023+48)      1060(+1022+48) (+1023=infinity)
! CDC-6600      D 96      -975         1060
! The smallest number is 2**(smallest)*.5.
! The largest number is 2**(largest)*(1-2/2**precision).
! The expression eps+1 .gt. 1 is true for eps=2**(1-precision)
! unrounded or eps=2**(-precision) when rounded.
! This does not work on Prime where S-accumulator is 31 bits.
! For un-squared number halve the exponent.
! So we scale for maximum above largest/N (/2N for complex)
! And for maximum below CUTLO**2=smallest/eps.
! It is assumed that underflows are efficiently handled as zero.
! Round up CUTLO, down CUTHI.
!primecs          data cutlo,cuthi/1.570e-16,1.304e+19/
!primecd          data cutlo,cuthi/5.406e-4945,4.560e+4912/
!kl-10cs          data cutlo,cuthi/6.281e-16,1.304e+19/
!kl-10cd          data cutlo,cuthi/1.164e-10,1.304e+19/
!vaxcs          data cutlo,cuthi/8.881e-16,1.304e+19/
!vaxcd          data cutlo,cuthi/2.058e-11,1.304e+19/
      data cutlo,cuthi/8.232e-11,1.304e+19/
      SCNRM2=0
      If (n.le.0) Return
      test=SCASUM(n,x,incx)
      If (test.le.0) Return
!CIF
      If (test.le.cuthi.and.test*(n+n).ge.cutlo) Then
!CEND      If (test.le.cuthi.and.test*n.ge.cutlo) Then
         Do j=1,n*incx,incx
            SCNRM2=SCNRM2+REAL(x(j))**2
     1                  +AIMAG(x(j))**2
         EndDo
         SCNRM2=sqrt(SCNRM2)
      Else
         Do i=1,n*incx,incx
            SCNRM2=SCNRM2+(REAL(x(i))/test)**2
     1                  +(AIMAG(x(i))/test)**2
         EndDo
         SCNRM2=test*sqrt(SCNRM2)
      Endif
      End
!+ absolute values summed
      REAL Function SCASUM(n,x,incx)
      Integer i,incx,m,n
      COMPLEX x(1)
      SCASUM=0
      If (n.le.0) Then
      ElseIf (incx.ne.1) Then
         Do i=1,n*incx,incx
            SCASUM=SCASUM+abs(REAL(x(i)))+abs(AIMAG(x(i)))
         EndDo
      Else
         m=mod(n,6)
         Do i=1,m
            SCASUM=SCASUM+abs(REAL(x(i)))+abs(AIMAG(x(i)))
         EndDo
         Do i=m+1,n,6
            SCASUM=SCASUM+abs(REAL(x(i)))+abs(AIMAG(x(i)))
     1+abs(REAL(x(i+1)))+abs(AIMAG(x(i+1)))
     2+abs(REAL(x(i+2)))+abs(AIMAG(x(i+2)))
     3+abs(REAL(x(i+3)))+abs(AIMAG(x(i+3)))
     4+abs(REAL(x(i+4)))+abs(AIMAG(x(i+4)))
     5+abs(REAL(x(i+5)))+abs(AIMAG(x(i+5)))
         EndDo
      EndIf
      End
!+ y=a*x+y  scale and add, backwards if negative increment
      SUBROUTINE CAXPY(n,a,x,incx,y,incy)
      Integer i,incx,incy,j,k,m,n
      COMPLEX x(1),y(1),a
      If (n.le.0.or.a.eq.0) Then
      ElseIf (incx.ne.1.or.incy.ne.1) Then
         j=1
         k=1
         If (incx.lt.0) j=(1-n)*incx+1
         If (incy.lt.0) k=(1-n)*incy+1
         Do i=1,n
            y(k)=y(k)+x(j)*a
            j=j+incx
            k=k+incy
         EndDo
      Else
         m=mod(n,4)
         Do i=1,m
            y(i)=y(i)+x(i)*a
         EndDo
         Do i=m+1,n,4
            y(i)=y(i)+x(i)*a
            y(i+1)=y(i+1)+x(i+1)*a
            y(i+2)=y(i+2)+x(i+2)*a
            y(i+3)=y(i+3)+x(i+3)*a
         EndDo
      EndIf
      End
!+ copy array, backwards if negative increment
      SUBROUTINE CCOPY(n,x,incx,y,incy)
      Integer i,incx,incy,j,k,m,n
      COMPLEX x(1),y(1)
      If (n.le.0) Then
      ElseIf (incx.ne.1.or.incy.ne.1) Then
         j=1
         k=1
         If (incx.lt.0) j=(1-n)*incx+1
         If (incy.lt.0) k=(1-n)*incy+1
         Do i=1,n
            y(k)=x(j)
            j=j+incx
            k=k+incy
         EndDo
      Else
         m=mod(n,7)
         Do i=1,m
            y(i)=x(i)
         EndDo
         Do i=m+1,n,7
            y(i)=x(i)
            y(i+1)=x(i+1)
            y(i+2)=x(i+2)
            y(i+3)=x(i+3)
            y(i+4)=x(i+4)
            y(i+5)=x(i+5)
            y(i+6)=x(i+6)
         EndDo
      EndIf
      End
!+ dot product
      COMPLEX Function CDOTU(n,x,incx,y,incy)
      Integer i,incx,incy,j,k,m,n
      COMPLEX x(1),y(1)
      CDOTU=0
      If (n.le.0) Return
      If (incx.ne.1.or.incy.ne.1) Then
         j=1
         k=1
         If (incx.lt.0) j=(1-n)*incx+1
         If (incy.lt.0) k=(1-n)*incy+1
         Do i=1,n
            CDOTU=CDOTU+x(j)*y(k)
            j=j+incx
            k=k+incy
         EndDo
      Else
         m=mod(n,5)
         Do i=1,m
            CDOTU=CDOTU+x(i)*y(i)
         Enddo
         Do i=m+1,n,5
            CDOTU=CDOTU+x(i)*y(i)+y(i+1)*x(i+1)+
     1     x(i+2)*y(i+2)+y(i+3)*x(i+3)+x(i+4)*y(i+4)
         EndDo
      EndIf
      End
!CIF
!+ dot product
      COMPLEX Function CDOTC(n,x,incx,y,incy)
      Integer i,incx,incy,j,k,m,n
      COMPLEX x(1),y(1)
      CDOTC=0
      If (n.le.0) Return
      If (incx.ne.1.or.incy.ne.1) Then
         j=1
         k=1
         If (incx.lt.0) j=(1-n)*incx+1
         If (incy.lt.0) k=(1-n)*incy+1
         Do i=1,n
            CDOTC=CDOTC+conjg(x(j))*y(k)
            j=j+incx
            k=k+incy
         EndDo
      Else
         m=mod(n,5)
         Do i=1,m
            CDOTC=CDOTC+conjg(x(i))*y(i)
         Enddo
         Do i=m+1,n,5
            CDOTC=CDOTC+conjg(x(i))*y(i)
     1+conjg(x(i+1))*y(i+1)
     2+conjg(x(i+2))*y(i+2)
     3+conjg(x(i+3))*y(i+3)
     4+conjg(x(i+4))*y(i+4)
         EndDo
      Endif
      End
!CEND
!+ apply a plane rotation, backwards if negative increment
      Subroutine CSROT(n,x,incx,y,incy,c,s)
      Integer i,incx,incy,j,k,n
      COMPLEX x(1),y(1),temp
      REAL c,s
      If (n.le.0.or.s.eq.0) Return
      If (incx.ne.1.or.incy.ne.1) Then
         j=1
         k=1
         If (incx.lt.0) j=(1-n)*incx+1
         If (incy.lt.0) k=(1-n)*incy+1
         Do i=1,n
            temp=
     1CMPLX(REAL(x(j))*c+REAL(y(k))*s,AIMAG(x(j))*c+AIMAG(y(k))*s)
            y(k)=
     1CMPLX(-REAL(x(j))*s+REAL(y(k))*c,-AIMAG(x(j))*s+AIMAG(y(k))*c)
            x(j)=temp
            j=j+incx
            k=k+incy
         EndDo
      Else
         Do i=1,n
            temp=
     1 CMPLX(REAL(x(i))*c+REAL(y(i))*s,AIMAG(x(i))*c+AIMAG(y(i))*s)
            y(i)=
     1 CMPLX(-REAL(x(i))*s+REAL(y(i))*c,-AIMAG(x(i))*s+AIMAG(y(i))*c)
            x(i)=temp
         EndDo
      Endif
      End
!+ construct Givens plane rotation
      SUBROUTINE CROTG(a,b,c,s)
      REAL c,aa,bb
      COMPLEX a,b,s,r
      bb=abs(REAL(b))+abs(AIMAG(b))
      If (bb.gt.0) Then
         aa=abs(REAL(a))+abs(AIMAG(a))
         If (aa.gt.bb) Then
            r=a*sqrt(1+(bb/aa)**2)
            c=real(a/r)
            s=b/r
            b=s
            a=r
         Else
            r=b*sqrt(1+(aa/bb)**2)
            c=real(a/r)
            s=b/r
            a=r
            b=1
            If (c.ne.0) b=1/c
         Endif
      Else
            c=1
            s=0
      Endif
      End
!CIF
      Subroutine CSSCAL(n,a,x,incx)
      Integer i,incx,m,n
      REAL a
      COMPLEX x(1)
      If (n.le.0) Return
      If (incx.ne.1) Then
         Do i=1,n*incx,incx
            x(i)=CMPLX(REAL(x(i))*a,AIMAG(x(i))*a)
         EndDo
      Else
         m=mod(n,5)
         Do i=1,m
            x(i)=CMPLX(REAL(x(i))*a,AIMAG(x(i))*a)
         EndDo
         Do i=m+1,n,5
            x(i)=CMPLX(REAL(x(i))*a,AIMAG(x(i))*a)
            x(i+1)=CMPLX(REAL(x(i+1))*a,AIMAG(x(i+1))*a)
            x(i+2)=CMPLX(REAL(x(i+2))*a,AIMAG(x(i+2))*a)
            x(i+3)=CMPLX(REAL(x(i+3))*a,AIMAG(x(i+3))*a)
            x(i+4)=CMPLX(REAL(x(i+4))*a,AIMAG(x(i+4))*a)
         EndDo
      Endif
      End
!CEND
!+ x=a*x  scale
      SUBROUTINE CSCAL(n,a,x,incx)
      Integer i,incx,m,n
      COMPLEX x(1),a
      If (n.le.0) Return
      If (incx.ne.1) Then
         Do i=1,n*incx,incx
            x(i)=x(i)*a
         EndDo
      Else
         m=mod(n,5)
         Do i=1,m
            x(i)=x(i)*a
         EndDo
         Do i=m+1,n,5
            x(i)=x(i)*a
            x(i+1)=x(i+1)*a
            x(i+2)=x(i+2)*a
            x(i+3)=x(i+3)*a
            x(i+4)=x(i+4)*a
         EndDo
      EndIf
      End
!+ swap arrays, backwards if negative increment
      SUBROUTINE CSWAP(n,x,incx,y,incy)
      Integer i,incx,incy,j,k,m,n
      COMPLEX x(1),y(1),temp
      If (n.le.0) Return
      If (incx.ne.1.or.incy.ne.1) Then
         j=1
         k=1
         If (incx.lt.0) j=(1-n)*incx+1
         If (incy.lt.0) k=(1-n)*incy+1
         Do i=1,n
            temp=x(j)
            x(j)=y(k)
            y(k)=temp
            j=j+incx
            k=k+incy
         EndDo
      Else
         m=mod(n,3)
         Do i=1,m
            temp=x(i)
            x(i)=y(i)
            y(i)=temp
         EndDo
         Do i=m+1,n,3
            temp=x(i)
            x(i)=y(i)
            y(i)=temp
            temp=x(i+1)
            x(i+1)=y(i+1)
            y(i+1)=temp
            temp=x(i+2)
            x(i+2)=y(i+2)
            y(i+2)=temp
         EndDo
      EndIf
      End
