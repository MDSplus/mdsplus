!      Ken Klare, LANL P-4      (c)1990,1991
!***********************************************************************
!********************* 3.5 Piecewise Polynomial ************************
!-----------------------------------------------------------------------
      Real Function PPVAL(x,korder,nintv,break,c)
      Real      x      !input, the point to evaluate
      Integer      korder      !input, the order of the polynomial (degree+1)
      Integer nintv      !input, the number of polynomial pieces
      Real      break(*)!input, [nintv+1] breakpoints
      Real      c(*)      !input, [korder,nintv] polynomial coefficients
!+Evaluate a piecewise polynomial
      Real      PPDER
      PPVAL = PPDER(0,x,korder,nintv,break,c)
      End
!-----------------------------------------------------------------------
      Real Function PPDER(ideriv,x,korder,nintv,break,c)
      Integer ideriv      !input, the order of deriavtive to be evaluated
      Real      x      !input, the point to evaluate
      Integer      korder      !input, the order of the polynomial (degree+1)
      Integer nintv      !input, the number of polynomial pieces
      Real      break(*)!input, [nintv+1] breakpoints
      Real      c(korder,*)!input, [korder,nintv] polynomial coefficients
!+Evaluate derivative of a piecewise polynomial
      Integer j,m,keep
      Real      h
      Data      keep/1000000/
! can only take so many derivatives
      If (ideriv.ge.korder) Then
          PPDER = 0
      Else
! look up the place
          Call SRCH(nintv,x,break,1,keep)
          If (keep.gt.0) Then
            PPDER = c(ideriv+1,keep)
          Else
            j = -1-keep
            if (j.le.0) j = 1
            h = x - break(j)
            PPDER = c(korder,j)
            Do m = korder-ideriv-1, 1, -1
                PPDER = PPDER*h/m + c(m+ideriv,j)
            Enddo
          Endif
      Endif
      End
!-----------------------------------------------------------------------
      Real Function PPITG(a,b,korder,nintv,break,c)
      Real      a,b      !input, the range to evaluate
      Integer      korder      !input, the order of the polynomial (degree+1)
      Integer nintv      !input, the number of polynomial pieces
      Real      break(*)!input, [nintv] breakpoints
      Real      c(korder,*)!input, [korder,nintv] polynomial coefficients
!+Evaluate the integral of a piecewise polynomial
      Integer      ia, ib, j
      Real      P9ITG
        data ia /999999/
      data ib /999999/
      Call SRCH(nintv,b,break,1,ib)
      If (ib.gt.0) Then
            PPITG = 0
      Else
            ib = -1-ib
            if (ib.le.0) ib = 1
            PPITG = P9ITG(b-break(ib),korder,c(1,ib))
      Endif
      Call SRCH(nintv,a,break,1,ia)
      If (ia.le.0) Then
            ia = -1-ia
            if (ia.le.0) ia = 1
            PPITG = PPITG - P9ITG(a-break(ia),korder,c(1,ia))
      Endif
      Do j = ia,ib-1
            PPITG = PPITG + P9ITG(break(j+1)-break(j),korder,c(1,j))
      Enddo
      Do j = ib,ia-1
            PPITG = PPITG - P9ITG(break(j+1)-break(j),korder,c(1,j))
      Enddo
      End
!-----------------------------------------------------------------------
      Real Function P9ITG(h,korder,c)
      Real      h      !input, offset point
      Integer      korder      !input, number of terms, degree+1
      Real      c(*)      !input, [korder] value and derivatives
      Integer      j
!+Evaluate integral of single polynomial
      P9ITG = 0
      Do j = korder,1,-1
            P9ITG = (P9ITG*h + c(j))/j
      Enddo
      P9ITG = P9ITG*h
      End
!-----------------------------------------------------------------------
      Subroutine PCOEF(ndata,xdata,fdata,cof)
      Integer      ndata      !input, polynomial order (degree+1)
      Real      xdata(*)!input, [ndata] the ordinates, must be distinct
      Real      fdata(*)!input, [ndata] the abscissae
      Real      cof(*)      !input, [ndata] the polynomial coeff
!+Create single piecewise polynomial, value and derivatives at first
! Not an IMSL routine. Method of G.Rybicki from Numerical Recipes POLCOE
      Integer      nmax
      Parameter (nmax=15)
      Integer      i,j,k,n
      Real      xx,s(nmax),b,ff,phi,fact(3:nmax)
      Data      fact/2,6,24,120,720,5040,40320,362880,3628800,39916800,
     1 479001600,6227020800E0,87178291200E0/

      n=ndata
      Do i=1,n
          cof(i)=0
          s(i)=0
      Enddo
!      s(n)=-(xdata(1)-xdata(1))
! recurrence
      Do i=2,n
          xx=xdata(i)-xdata(1)
          Do j=n+1-i,n-1
            s(j)=s(j)-xx*s(j+1)
          Enddo
          s(n)=s(n)-xx
      Enddo
      Do j=1,n
          xx=xdata(j)-xdata(1)
          phi=n
! phi=product(xj-xk) for j.ne.k
          Do k=n-1,1,-1
            phi=k*s(k+1)+xx*phi
          Enddo
          ff=fdata(j)/phi
          b=1
          Do k=n,1,-1
            cof(k)=cof(k)+b*ff
            b=s(k)+xx*b
          Enddo
      Enddo
      Do k=3,n
          cof(k)=cof(k)*fact(k)
      Enddo
      End
!***********************************************************************
!********************* Extension for vectors ***************************
!-----------------------------------------------------------------------
      Subroutine PPVALV(nx,x,korder,nintv,break,c,y)
      Integer      nx      !input, the number of points
      Real      x(*)      !input, the [nx] points to evaluate
      Integer      korder      !input, the order of the polynomial (degree+1)
      Integer nintv      !input, the number of polynomial pieces
      Real      break(*)!input, [nintv+1] breakpoints
      Real      c(*)      !input, [korder,nintv] polynomial coefficients
      Real      y(*)      !output, the evaluated points
!+Evaluate a piecewise polynomial
      Call PPDERV(0,nx,x,korder,nintv,break,c,y)
      End
!-----------------------------------------------------------------------
      Subroutine PPDERV(ideriv,nx,x,korder,nintv,break,c,y)
      Integer ideriv      !input, the order of deriavtive to be evaluated
      Integer      nx      !input, the number of points
      Real      x(*)      !input, the [nx] points to evaluate
      Integer      korder      !input, the order of the polynomial (degree+1)
      Integer nintv      !input, the number of polynomial pieces
      Real      break(*)!input, [nintv+1] breakpoints
      Real      c(korder,*)!input, [korder,nintv] polynomial coefficients
      Real      y(*)      !output, the evaluated points
!+Evaluate derivative of a piecewise polynomial
      Integer j
      Real      PPDER
      Do j=1,nx
          y(j) = PPDER(ideriv,x(j),korder,nintv,break,c)
      Enddo
      End
!-----------------------------------------------------------------------
      Subroutine PPITGV(na,a,nb,b,korder,nintv,break,c,y)
      Integer      na      !input, the number of points
      Real      a(*)      !input, the [na] points to evaluate from first
      Integer      nb      !input, the number of points
      Real      b(*)      !input, the [nb] points to evaluate from first
      Integer      korder      !input, the order of the polynomial (degree+1)
      Integer nintv      !input, the number of polynomial pieces
      Real      break(*)!input, [nintv] breakpoints
      Real      c(korder,*)!input, [korder,nintv] polynomial coefficients
      Real      y(*)      !output, the evaluated points
!+Evaluate the integral of a piecewise polynomial
      Real      PPITG
      Integer j,ja,jb,n,inca,incb
      n = min(na,nb)
      If (na.eq.1) n = nb
      inca = 0
      incb = 0
      If (na.gt.1) inca = 1
      If (nb.gt.1) incb = 1
      ja = 1
      jb = 1
      Do j=1,n
          y(j) = PPITG(a(ja),b(jb),korder,nintv,break,c)
          ja = ja + inca
          jb = jb + incb
      Enddo
      End
