!      Ken Klare, LANL P-4      (c)1990,1991
!***********************************************************************
!********************* 3.2 Cubic Spline Evaluation *********************
!-----------------------------------------------------------------------
      Real Function CSVAL(x,nintv,break,c)
      Real      x      !input, the point to evaluate
      Integer nintv      !input, the number of polynomial pieces
      Real      break(*)!input, [nintv+1] breakpoints
      Real      c(4,*)      !input, [4,nintv+1] polynomial coefficients
!+Cubic spline evaluation.
      Real      PPDER
      CSVAL = PPDER(0,x,4,nintv,break,c)
      End
!-----------------------------------------------------------------------
      Real Function CSDER(ideriv,x,nintv,break,c)
      Integer ideriv      !input, the order of deriavtive to be evaluated
      Real      x      !input, the point to evaluate
      Integer nintv      !input, the number of polynomial pieces
      Real      break(*)!input, [nintv+1] breakpoints
      Real      c(4,*)      !input, [4,nintv+1] polynomial coefficients
!+Cubic spline derivative evaluation.
      Real      PPDER
      CSDER = PPDER(ideriv,x,4,nintv,break,c)
      End
!-----------------------------------------------------------------------
      Real Function CSITG(a,b,nintv,break,c)
      Real      a,b      !input, the range to evaluate
      Integer nintv      !input, the number of polynomial pieces
      Real      break(*)!input, [nintv+1] breakpoints
      Real      c(4,*)      !input, [4,nintv+1] polynomial coefficients
!+Cubic spline evaluation.
      Real      PPITG
      CSITG = PPITG(a,b,4,nintv,break,c)
      End
!***********************************************************************
!********************* Vector extensions *******************************
!-----------------------------------------------------------------------
      Subroutine CSVALV(nx,x,nintv,break,c,y)
!     ENTRY CSVALV_(nx,x,nintv,break,c,y)
      Integer      nx      !input, the number of points
      Real      x(*)      !input, the [nx] points to evaluate
      Integer nintv      !input, the number of polynomial pieces
      Real      break(*)!input, [nintv+1] breakpoints
      Real      c(4,*)      !input, [4,nintv+1] polynomial coefficients
      Real      y(*)      !output, the evaluated points
!+Cubic spline evaluation.
      Call PPDERV(0,nx,x,4,nintv,break,c,y)
      End
!-----------------------------------------------------------------------
      Subroutine CSDERV(ideriv,nx,x,nintv,break,c,y)
!     ENTRY CSDERV_(ideriv,nx,x,nintv,break,c,y)
      Integer ideriv      !input, the order of deriavtive to be evaluated
      Integer      nx      !input, the number of points
      Real      x(*)      !input, the [nx] points to evaluate
      Integer nintv      !input, the number of polynomial pieces
      Real      break(*)!input, [nintv+1] breakpoints
      Real      c(4,*)      !input, [4,nintv+1] polynomial coefficients
      Real      y(*)      !output, the evaluated points
!+Cubic spline derivative evaluation.
      Call PPDERV(ideriv,nx,x,4,nintv,break,c,y)
      End
!-----------------------------------------------------------------------
      Subroutine CSITGV(na,a,nb,b,nintv,break,c,y)
!     ENTRY CSITGV_(na,a,nb,b,nintv,break,c,y)
      Integer      na      !input, the number of points
      Real      a(*)      !input, the [na] points to evaluate
      Integer      nb      !input, the number of points
      Real      b(*)      !input, the [nb] points to evaluate
      Integer nintv      !input, the number of polynomial pieces
      Real      break(*)!input, [nintv+1] breakpoints
      Real      c(4,*)      !input, [4,nintv+1] polynomial coefficients
      Real      y(*)      !output, the evaluated points
!+Cubic spline evaluation.
      Call PPITGV(na,a,nb,b,4,nintv,break,c,y)
      End
