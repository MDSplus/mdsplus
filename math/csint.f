!      Ken Klare, LANL P-4      (c)1990,1992
!***********************************************************************
!********************* 3.1 Cubic Spline Interpolation ******************
! A piecewise polynomial is a polynomial defined on line segment.
! We assign the breakpoint to the next segment, right-continuous.
! The knots must be strictly ascending, so they are sorted.
!-----------------------------------------------------------------------
      Subroutine CSINT(ndata,xdata,fdata,break,cscoef)
!     ENTRY CSINT_(ndata,xdata,fdata,break,cscoef)
      Integer      ndata            !input, number of data points
      Real      xdata(*)      !input, [ndata] abscissae
      Real      fdata(*)      !input, [ndata] ordinates
      Real      break(*)      !output, [ndata] breakpoints
      Real      cscoef(4,*)      !output, [4,ndata] coefficients, f f' f'' f'''
!+Cubic spline with "not-a-knot" endpoints, that is, the slopes
! at ends make the polynomial piece the same as next interior one.
      Call CSDEC(ndata, xdata, fdata, 0, 0., 0, 0., break, cscoef)
      End
!-----------------------------------------------------------------------
      Subroutine CSDEC(ndata,xdata,fdata,
     1      ileft,dleft,iright,dright,break,c)
      Integer      ndata      !input, number of data points
      Real      xdata(*)!input, [ndata] abscissae
      Real      fdata(*)!input, [ndata] ordinates
      Integer      ileft      !input, end type: 0 = not-a-knot 1 = f', 2 = f''
      Real      dleft      !input, derivative at endpoint
      Integer      iright      !input, end type: 0 = not-a-knot 1 = f', 2 = f''
      Real      dright      !input, derivative at endpoint
      Real      break(*)!output, [ndata] breakpoints
      Real      c(4,*)      !output, [4,ndata] coefficients, f f' f'' f'''
!+Cubic splice with specified deriative or "not-a-knot" endpoints
! O(n) Create and solve tridagonal linear system for the unknown slopes
! c(1,*) = f(x[*]) value
! c(2,*) = s(x[*]) = df/dx, the slope
! c(3,*) and c(4,*) used for scratch initially.
! Carl de Boor "A practical Guide to Splines" Springer-Verlag p.57

      Integer      j,m,n
      Real     g

      n = ndata
      If (n.le.0) Return
      Call C9INI(ndata,xdata,fdata,fdata,break,c)
! dx and df/dx secant
      Do m = 2,n
          c(3,m) = break(m) - break(m-1)
          c(4,m) = (c(1,m) - c(1,m-1)) / c(3,m)
      Enddo
! select left constraint c(2,1) = c(4,1)*s(1) + c(3,1)*s(2)
      If (ileft.eq.1) Then
          c(4,1) = 1
          c(3,1) = 0
          c(2,1) = dleft
      Elseif (ileft.eq.2) Then
          c(4,1) = 2
          c(3,1) = 1
          c(2,1) = 3*c(4,2) - c(3,2)*dleft*0.5E0
      Elseif (n.eq.2) Then
          c(4,1) = 1
          c(3,1) = 1
          c(2,1) = 2*c(4,2)
      Else
          c(4,1) = c(3,3)
          c(3,1) = c(3,2) + c(3,3)
          c(2,1) = ((c(3,2)+2*c(3,1))*c(4,2)*c(3,3)
     1            +c(3,2)**2*c(4,3))/c(3,1)
      Endif
! interior knots forward pass of gauss elimination
      Do m = 2,n-1
          g = -c(3,m+1)/c(4,m-1)
          c(2,m) = g*c(2,m-1) + 3*(c(3,m)*c(4,m+1)+c(3,m+1)*c(4,m))
          c(4,m) = g*c(3,m-1) + 2*(c(3,m) + c(3,m+1))
      Enddo
! select right constraint c(2,n) = -g*c(4,n-1)*s(n-1) + c(4,n)*s(n)
      If (iright.eq.1) Then
          c(2,n) = dright
          Goto 30
      Elseif (iright.eq.2) Then
          c(2,n) = 3*c(4,n) + c(3,n)*dright*0.5E0
          c(4,n) = 2
          g = 1
      Elseif (n.eq.2.and..not.(ileft.ne.1.and.ileft.ne.2) .or.
     1      n.eq.3.and.ileft.ne.1.and.ileft.ne.2) Then
          c(2,n) = 2*c(4,n)
          c(4,n) = 1
          g = 1
      Elseif (n.eq.2) Then
          c(2,n) = c(4,n)
          go to 30
      Else
          g = c(3,n-1) + c(3,n)
          c(2,n) = ((c(3,n)+2*g)*c(4,n)*c(3,n-1)
     1      + c(3,n)**2/c(3,n-1)*(c(1,n-1)-c(1,n-2)))/g
          c(4,n) = c(3,n-1)
      Endif
! finish forward gauss elimination
      g = g/c(4,n-1)
      c(4,n) = c(4,n) - g*c(3,n-1)
      c(2,n) = (c(2,n) - g*c(2,n-1))/c(4,n)
! back substitution
30      Do j = n-1,1,-1
          c(2,j) = (c(2,j) - c(3,j)*c(2,j+1))/c(4,j)
      Enddo
      Call C9FIN(ndata,break,c)
      End
!-----------------------------------------------------------------------
      Subroutine CSHER(ndata,xdata,fdata,dfdata,break,c)
      Integer      ndata      !input, number of data points
      Real      xdata(*)!input, [ndata] abscissae
      Real      fdata(*)!input, [ndata] ordinates
      Real      dfdata(*)!input, [ndata] derivatives
      Real      break(*)!output, [ndata] breakpoints
      Real      c(4,*)      !output, [4,ndata] coefficients, f f' f'' f'''
!+Hermite cubic spline, given derivatives everywhere
! O(n) operations, O(step**4) approximation on uniform mesh
      Call C9INI(ndata,xdata,fdata,dfdata,break,c)
      Call C9FIN(ndata,break,c)
      End
!-----------------------------------------------------------------------
      Subroutine CSAKM(ndata,xdata,fdata,break,c)
!     ENTRY CSAKM_(ndata,xdata,fdata,break,c)
      Integer      ndata      !input, number of data points
      Real      xdata(ndata)!input, [ndata] abscissae
      Real      fdata(ndata)!input, [ndata] ordinates
      Real      break(ndata)!output, [ndata] breakpoints
      Real      c(4,ndata)      !output, [4,ndata] coefficients, f f' f'' f'''
!+Akima cubic spline, local order(step**-2) approximation
! less wiggles than most methods
! NOT ADDITIVE cc(a+b).ne.cc(a)+cc(b) in general
! slope = (W(i+1)*secant(i-1)+W(i-1)*secant(i))/(W(i+1)+W(i-1))
! W(i) = abs(secant(i) - secant(i-1))
      Integer j,n
      Real*8      g,gm,wm,wp,secant
      secant(j) = (c(1,j+1) - c(1,j))/(break(j+1) - break(j))

      n = ndata
      Call C9INI(ndata,xdata,fdata,fdata,break,c)
      If (n.eq.1) Then
          c(2,1) = 0
      Elseif (n.eq.2) Then
          c(2,1) = real(secant(1))
          c(2,2) = c(2,1)
      Else
          Do j = 3,n-2
            gm = secant(j-1)
            g = secant(j)
            wm = abs(gm - secant(j-2))
            wp = abs(secant(j+1) - g)
            If (wm + wp  .gt. 0) Then
                  c(2,j) = real((wp*gm + wm*g)/(wm + wp))
                Else
                  c(2,j) = real(wp*gm + wm*g)
                Endif
!            If (wm + wp .gt. 0) c(2,j) = c(2,j) / (wm + wp)
          Enddo
! move weight for second (matches IMSL) and f''=0 at first
! Warning in IMSL their method is only C1 continuous.
! They fail at breaks 2 and n-1.
! This method is C2 because DC9FIN makes it so.
          gm = secant(1)
          g = secant(2)
          wm = abs(g - gm)
          wp = abs(secant(3) - g)
          c(2,2) = real(wp*gm + wm*g)
          If (wm + wp .gt. 0) c(2,2) = real(c(2,2) / (wm + wp))
          c(2,1) = real((3*gm - c(2,2))/2)
          gm = secant(n-1)
          g = secant(n-2)
          wm = abs(g - gm)
          wp = abs(secant(n-3) - g)
          c(2,n-1) = real(wp*gm + wm*g)
          If (wm + wp .gt. 0) c(2,n-1) = real(c(2,n-1) / (wm + wp))
          c(2,n) = real((3*gm - c(2,n-1))/2)
      Endif
      Call C9FIN(ndata,break,c)
      End
!-----------------------------------------------------------------------
      Subroutine CSBES(ndata,xdata,fdata,break,c)
      Integer      ndata      !input, number of data points
      Real      xdata(*)!input, [ndata] abscissae
      Real      fdata(*)!input, [ndata] ordinates
      Real      break(*)!output, [ndata] breakpoints
      Real      c(4,*)      !output, [4,ndata] coefficients, f f' f'' f'''
!+Bessel cubic spline, local Order(n**-4) approximation
! This is NOT an IMSL routine.
! slope = (W(i)*secant(i-1)+W(i-1)*secant(i))/(W(i)+W(i-1))
! W(i) = x(i)-x(i-1)
      Integer j,n
      Real      dtau,dtaum,secant
      secant(j) = (c(1,j+1) - c(1,j))/(break(j+1) - break(j))

      n = ndata
      If (n.le.0) Return
      Call C9INI(ndata,xdata,fdata,fdata,break,c)
      Do j = 2,n-1
          dtau = break(j+1) - break(j)
          dtaum = break(j) - break(j-1)
          c(2,j)=(dtau*secant(j-1)+dtaum*secant(j))/(dtau+dtaum)
      Enddo
! take parabola using value and slope at 2 and value at 1
! expand f=c1+c2*x+c3*x**2, at x2, c1=f2 c2=df/dx2
! df/dx1=c2+2*c3*x=c2+2*(df(1)/dx1-c2)
      If (n.eq.1) Then
          c(2,1) = 0
      Elseif (n.eq.2) Then
          c(2,1) = secant(1)
          c(2,2) = c(2,1)
      Else
          c(2,1)=2*secant(1)-c(2,2)
          c(2,n)=2*secant(n-1)-c(2,n-1)
      Endif
      Call C9FIN(ndata,break,c)
      End
!-----------------------------------------------------------------------
      Subroutine C9FIN(ndata,break,c)
      Integer      ndata      !input, number of data points
      Real      break(*)!output, [ndata] breakpoints
      Real      c(4,*)      !modify, [4,ndata] coefficients, f f' f'' f'''
!+Internal, generate cubic coefficients, derivatives at left endpoint
! Assumes c(1,*) and c(2,*) have been set
      Integer      j,n
      Real      divdf1,divdf3,dtau

      n = ndata
      If (n.le.0) Return
! fill in second and third derivatives
      Do j = 2,n
          dtau = break(j) - break(j-1)
          divdf1 = (c(1,j) - c(1,j-1))/dtau
          divdf3 = c(2,j-1) + c(2,j) - 2*divdf1
          c(3,j-1) = 2*(divdf1 - c(2,j-1) - divdf3)/dtau
          c(4,j-1) = 6*divdf3/(dtau*dtau)
      Enddo
! last column is normally dummy, fill it too
      If (n.eq.1) Then
          c(3,1) = 0
          c(4,1) = 0
      Else
          c(3,n) = c(3,n-1) + (break(n) - break(n-1))*c(4,n-1)
          c(4,n) = c(4,n-1)
      Endif
      End
!-----------------------------------------------------------------------
      Subroutine C9INI(ndata,xdata,fdata,dfdata,break,c)
      Integer      ndata      !input, number of data points
      Real      xdata(*)!input, [ndata] abscissae
      Real      fdata(*)!input, [ndata] ordinates
      Real      dfdata(*)!input, [ndata] derivatives or dummy
      Real      break(*)!output, [ndata] breakpoints
      Real      c(4,*)      !output, [4,ndata] coefficients, f f' f'' f'''
!+Internal, initialize spline vectors
        Integer    idx(ndata)
      Integer j
      Do j = 1,ndata
            idx(j) = j
      Enddo
      Call SVRGP(ndata,xdata,break,1,idx)
      Do j = 1,ndata
            c(1,idx(j)) = fdata(j)
            c(2,idx(j)) = dfdata(j)
      Enddo
      End
