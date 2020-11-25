C*26-NOV-90 11:35:29 MDS$SOURCE:[KKLARE]ZBLAS.FOR;5
! Subprograms based on "LINPACK User's Guide" by J.J. Dongarra et al.
! The Basic Linear Algebra Subprograms portion.
!
! root    prefix-suffix of name     Function
! -AMAX   IS- ID-     IC-     IZ-   index of max |x|  or  max  |x|+|y|
! -ASUM   S-  D-      SC-     DZ-   w := sum |x|  or  sum |x|+|y|
! -AXPY   S-  D-      C-      Z-    w := a*x+y
! -COPY   S-  D-      C-      Z-    y := x
! -DOT-   S-  D-  C-U C-C Z-U Z-C   w:=sum x*y, for -C  w:=(x)*y
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
      Integer Function IDAMAX(n,x,incx)
      Integer i,incx,j,n
      Real*8 xmax
      REAL*8 x(*)
      If (n.le.0) Then
         IDAMAX=0
      ElseIf (n.eq.1) Then
         IDAMAX=1
      Else
         IDAMAX=1
         xmax=ABS((x(1)))
         If (incx.ne.1) Then
            j=1
            Do i=2,n
               j=j+incx
               If (ABS((x(j))).gt.xmax) Then
                xmax=ABS((x(j)))
                IDAMAX=i
               EndIf
            EndDo
         Else
            Do i=2,n
               If (ABS((x(i))).gt.xmax) Then
                  xmax=ABS((x(i)))
                  IDAMAX=i
               EndIf
            EndDo
         EndIf
      EndIf
      End
!+ Euclidean norm of vector
      Real*8 Function DNRM2(n,x,incx)
      Real*8 DASUM
      Integer i,incx,n
      Real*8 cutlo,cuthi,test
      REAL*8 x(1)
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
      DNRM2=0
      If (n.le.0) Return
      test=DASUM(n,x,incx)
      If (test.le.0) Return
      If (test.le.cuthi.and.test*n.ge.cutlo) Then
         Do j=1,n*incx,incx
            DNRM2=DNRM2+(x(j))**2
     1                  
         EndDo
         DNRM2=sqrt(DNRM2)
      Else
         Do i=1,n*incx,incx
            DNRM2=DNRM2+((x(i))/test)**2
         EndDo
         DNRM2=test*sqrt(DNRM2)
      Endif
      End
!+ absolute values summed
      Real*8 Function DASUM(n,x,incx)
      Integer i,incx,m,n
      REAL*8 x(1)
      DASUM=0
      If (n.le.0) Then
      ElseIf (incx.ne.1) Then
         Do i=1,n*incx,incx
            DASUM=DASUM+ABS((x(i)))
         EndDo
      Else
         m=mod(n,6)
         Do i=1,m
            DASUM=DASUM+ABS((x(i)))
         EndDo
         Do i=m+1,n,6
            DASUM=DASUM+ABS((x(i)))
     1+ABS((x(i+1)))
     2+ABS((x(i+2)))
     3+ABS((x(i+3)))
     4+ABS((x(i+4)))
     5+ABS((x(i+5)))
         EndDo
      EndIf
      End
!+ y=a*x+y  scale and add, backwards if negative increment
      SUBROUTINE DAXPY(n,a,x,incx,y,incy)
      Integer i,incx,incy,j,k,m,n
      REAL*8 x(*),y(*),a
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
      SUBROUTINE DCOPY(n,x,incx,y,incy)
      Integer i,incx,incy,j,k,m,n
      REAL*8 x(*),y(*)
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
      REAL*8 Function DDOT(n,x,incx,y,incy)
      Integer i,incx,incy,j,k,m,n
      REAL*8 x(1),y(1)
      DDOT=0
      If (n.le.0) Return
      If (incx.ne.1.or.incy.ne.1) Then
         j=1
         k=1
         If (incx.lt.0) j=(1-n)*incx+1
         If (incy.lt.0) k=(1-n)*incy+1
         Do i=1,n
            DDOT=DDOT+x(j)*y(k)
            j=j+incx
            k=k+incy
         EndDo
      Else
         m=mod(n,5)
         Do i=1,m
            DDOT=DDOT+x(i)*y(i)
         Enddo
         Do i=m+1,n,5
            DDOT=DDOT+x(i)*y(i)+y(i+1)*x(i+1)+
     1     x(i+2)*y(i+2)+y(i+3)*x(i+3)+x(i+4)*y(i+4)
         EndDo
      EndIf
      End

!+ apply a plane rotation, backwards if negative increment
      Subroutine DROT(n,x,incx,y,incy,c,s)
      Integer i,incx,incy,j,k,n
      REAL*8 x(1),y(1),temp
      Real*8 c,s
      If (n.le.0.or.s.eq.0) Return
      If (incx.ne.1.or.incy.ne.1) Then
         j=1
         k=1
         If (incx.lt.0) j=(1-n)*incx+1
         If (incy.lt.0) k=(1-n)*incy+1
         Do i=1,n
            temp=
     1((x(j))*c+(y(k))*s)
            y(k)=
     1(-(x(j))*s+(y(k))*c)
            x(j)=temp
            j=j+incx
            k=k+incy
         EndDo
      Else
         Do i=1,n
            temp=
     1 ((x(i))*c+(y(i))*s)
            y(i)=
     1 (-(x(i))*s+(y(i))*c)
            x(i)=temp
         EndDo
      Endif
      End
!+ construct Givens plane rotation
      SUBROUTINE DROTG(a,b,c,s)
      Real*8 c,aa,bb
      REAL*8 a,b,s,r
      bb=ABS((b))
      If (bb.gt.0) Then
         aa=ABS((a))
         If (aa.gt.bb) Then
            r=a*sqrt(1+(bb/aa)**2)
            c=a/r
            s=b/r
            b=s
            a=r
         Else
            r=b*sqrt(1+(aa/bb)**2)
            c=a/r
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

!+ x=a*x  scale
      SUBROUTINE DSCAL(n,a,x,incx)
      Integer i,incx,m,n
      REAL*8 x(*),a
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
      SUBROUTINE DSWAP(n,x,incx,y,incy)
      Integer i,incx,incy,j,k,m,n
      REAL*8 x(*),y(*),temp
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
