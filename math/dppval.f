!      Ken Klare, LANL P-4      (c)1990,1991
!***********************************************************************
!********************* 3.5 Piecewise Polynomial ************************
!-----------------------------------------------------------------------
      Real*8 Function DPPVAL(x,korder,nintv,break,c)
      Real*8      x      !input, the point to evaluate
      Integer      korder      !input, the order of the polynomial (degree+1)
      Integer nintv      !input, the number of polynomial pieces
      Real*8      break(*)!input, [nintv+1] breakpoints
      Real*8      c(*)      !input, [korder,nintv] polynomial coefficients
!+Evaluate a piecewise polynomial
      Real*8      DPPDER
      DPPVAL = DPPDER(0,x,korder,nintv,break,c)
      End
!-----------------------------------------------------------------------
      Real*8 Function DPPDER(ideriv,x,korder,nintv,break,c)
      Integer ideriv      !input, the order of deriavtive to be evaluated
      Real*8      x      !input, the point to evaluate
      Integer      korder      !input, the order of the polynomial (degree+1)
      Integer nintv      !input, the number of polynomial pieces
      Real*8      break(*)!input, [nintv+1] breakpoints
      Real*8      c(korder,*)!input, [korder,nintv] polynomial coefficients
!+Evaluate derivative of a piecewise polynomial
      Integer j,m,keep
      Real*8      h
      Data      keep/1000000/
! can only take so many derivatives
      If (ideriv.ge.korder) Then
          DPPDER = 0
      Else
! look up the place
          Call DSRCH(nintv,x,break,1,keep)
          If (keep.gt.0) Then
            DPPDER = c(ideriv+1,keep)
          Else
            j = -1-keep
            if (j.le.0) j = 1
            h = x - break(j)
            DPPDER = c(korder,j)
            Do m = korder-ideriv-1, 1, -1
                DPPDER = DPPDER*h/m + c(m+ideriv,j)
            Enddo
          Endif
      Endif
      End
!-----------------------------------------------------------------------
      Real*8 Function DPPITG(a,b,korder,nintv,break,c)
      Real*8      a,b      !input, the range to evaluate
      Integer      korder      !input, the order of the polynomial (degree+1)
      Integer nintv      !input, the number of polynomial pieces
      Real*8      break(*)!input, [nintv] breakpoints
      Real*8      c(korder,*)!input, [korder,nintv] polynomial coefficients
!+Evaluate the integral of a piecewise polynomial
      Integer      ia, ib, j
      Real*8      DP9ITG
        data ia /999999/
      data ib /999999/
      Call DSRCH(nintv,b,break,1,ib)
      If (ib.gt.0) Then
            DPPITG = 0
      Else
            ib = -1-ib
            if (ib.le.0) ib = 1
            DPPITG = DP9ITG(b-break(ib),korder,c(1,ib))
      Endif
      Call DSRCH(nintv,a,break,1,ia)
      If (ia.le.0) Then
            ia = -1-ia
            if (ia.le.0) ia = 1
            DPPITG = DPPITG - DP9ITG(a-break(ia),korder,c(1,ia))
      Endif
      Do j = ia,ib-1
            DPPITG = DPPITG + DP9ITG(break(j+1)-break(j),korder,c(1,j))
      Enddo
      Do j = ib,ia-1
            DPPITG = DPPITG - DP9ITG(break(j+1)-break(j),korder,c(1,j))
      Enddo
      End
!-----------------------------------------------------------------------
      Real*8 Function DP9ITG(h,korder,c)
      Real*8      h      !input, offset point
      Integer      korder      !input, number of terms, degree+1
      Real*8      c(*)      !input, [korder] value and derivatives
      Integer      j
!+Evaluate integral of single polynomial
      DP9ITG = 0
      Do j = korder,1,-1
            DP9ITG = (DP9ITG*h + c(j))/j
      Enddo
      DP9ITG = DP9ITG*h
      End
!-----------------------------------------------------------------------
      Subroutine DPCOEF(ndata,xdata,fdata,cof)
      Integer      ndata      !input, polynomial order (degree+1)
      Real*8      xdata(*)!input, [ndata] the ordinates, must be distinct
      Real*8      fdata(*)!input, [ndata] the abscissae
      Real*8      cof(*)      !input, [ndata] the polynomial coeff
!+Create single piecewise polynomial, value and derivatives at first
! Not an IMSL routine. Method of G.Rybicki from Numerical Recipes POLCOE
      Integer      nmax
      Parameter (nmax=15)
      Integer      i,j,k,n
      Real*8      xx,s(nmax),b,ff,phi,fact(3:nmax)
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
      Subroutine DPPVALV(nx,x,korder,nintv,break,c,y)
      Integer      nx      !input, the number of points
      Real*8      x(*)      !input, the [nx] points to evaluate
      Integer      korder      !input, the order of the polynomial (degree+1)
      Integer nintv      !input, the number of polynomial pieces
      Real*8      break(*)!input, [nintv+1] breakpoints
      Real*8      c(*)      !input, [korder,nintv] polynomial coefficients
      Real*8      y(*)      !output, the evaluated points
!+Evaluate a piecewise polynomial
      Call DPPDERV(0,nx,x,korder,nintv,break,c,y)
      End
!-----------------------------------------------------------------------
      Subroutine DPPDERV(ideriv,nx,x,korder,nintv,break,c,y)
      Integer ideriv      !input, the order of deriavtive to be evaluated
      Integer      nx      !input, the number of points
      Real*8      x(*)      !input, the [nx] points to evaluate
      Integer      korder      !input, the order of the polynomial (degree+1)
      Integer nintv      !input, the number of polynomial pieces
      Real*8      break(*)!input, [nintv+1] breakpoints
      Real*8      c(korder,*)!input, [korder,nintv] polynomial coefficients
      Real*8      y(*)      !output, the evaluated points
!+Evaluate derivative of a piecewise polynomial
      Integer j
      Real*8      DPPDER
      Do j=1,nx
          y(j) = DPPDER(ideriv,x(j),korder,nintv,break,c)
      Enddo
      End
!-----------------------------------------------------------------------
      Subroutine DPPITGV(na,a,nb,b,korder,nintv,break,c,y)
      Integer      na      !input, the number of points
      Real*8      a(*)      !input, the [na] points to evaluate from first
      Integer      nb      !input, the number of points
      Real*8      b(*)      !input, the [nb] points to evaluate from first
      Integer      korder      !input, the order of the polynomial (degree+1)
      Integer nintv      !input, the number of polynomial pieces
      Real*8      break(*)!input, [nintv] breakpoints
      Real*8      c(korder,*)!input, [korder,nintv] polynomial coefficients
      Real*8      y(*)      !output, the evaluated points
!+Evaluate the integral of a piecewise polynomial
      Real*8      DPPITG
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
          y(j) = DPPITG(a(ja),b(jb),korder,nintv,break,c)
          ja = ja + inca
          jb = jb + incb
      Enddo
      End
