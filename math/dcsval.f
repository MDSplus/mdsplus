!      Ken Klare, LANL P-4      (c)1990,1991
!***********************************************************************
!********************* 3.2 Cubic Spline Evaluation *********************
!-----------------------------------------------------------------------
      Real*8 Function DCSVAL(x,nintv,break,c)
      Real*8      x      !input, the point to evaluate
      Integer nintv      !input, the number of polynomial pieces
      Real*8      break(*)!input, [nintv+1] breakpoints
      Real*8      c(4,*)      !input, [4,nintv+1] polynomial coefficients
!+Cubic spline evaluation.
      Real*8      DPPDER
      DCSVAL = DPPDER(0,x,4,nintv,break,c)
      End
!-----------------------------------------------------------------------
      Real*8 Function DCSDER(ideriv,x,nintv,break,c)
      Integer ideriv      !input, the order of deriavtive to be evaluated
      Real*8      x      !input, the point to evaluate
      Integer nintv      !input, the number of polynomial pieces
      Real*8      break(*)!input, [nintv+1] breakpoints
      Real*8      c(4,*)      !input, [4,nintv+1] polynomial coefficients
!+Cubic spline derivative evaluation.
      Real*8      DPPDER
      DCSDER = DPPDER(ideriv,x,4,nintv,break,c)
      End
!-----------------------------------------------------------------------
      Real*8 Function DCSITG(a,b,nintv,break,c)
      Real*8      a,b      !input, the range to evaluate
      Integer nintv      !input, the number of polynomial pieces
      Real*8      break(*)!input, [nintv+1] breakpoints
      Real*8      c(4,*)      !input, [4,nintv+1] polynomial coefficients
!+Cubic spline evaluation.
      Real*8      DPPITG
      DCSITG = DPPITG(a,b,4,nintv,break,c)
      End
!***********************************************************************
!********************* Vector extensions *******************************
!-----------------------------------------------------------------------
      Subroutine DCSVALV(nx,x,nintv,break,c,y)
      Integer      nx      !input, the number of points
      Real*8      x(*)      !input, the [nx] points to evaluate
      Integer nintv      !input, the number of polynomial pieces
      Real*8      break(*)!input, [nintv+1] breakpoints
      Real*8      c(4,*)      !input, [4,nintv+1] polynomial coefficients
      Real*8      y(*)      !output, the evaluated points
!+Cubic spline evaluation.
      Call DPPDERV(0,nx,x,4,nintv,break,c,y)
      End
!-----------------------------------------------------------------------
      Subroutine DCSDERV(ideriv,nx,x,nintv,break,c,y)
      Integer ideriv      !input, the order of deriavtive to be evaluated
      Integer      nx      !input, the number of points
      Real*8      x(*)      !input, the [nx] points to evaluate
      Integer nintv      !input, the number of polynomial pieces
      Real*8      break(*)!input, [nintv+1] breakpoints
      Real*8      c(4,*)      !input, [4,nintv+1] polynomial coefficients
      Real*8      y(*)      !output, the evaluated points
!+Cubic spline derivative evaluation.
      Call DPPDERV(ideriv,nx,x,4,nintv,break,c,y)
      End
!-----------------------------------------------------------------------
      Subroutine DCSITGV(na,a,nb,b,nintv,break,c,y)
      Integer      na      !input, the number of points
      Real*8      a(*)      !input, the [na] points to evaluate
      Integer      nb      !input, the number of points
      Real*8      b(*)      !input, the [nb] points to evaluate
      Integer nintv      !input, the number of polynomial pieces
      Real*8      break(*)!input, [nintv+1] breakpoints
      Real*8      c(4,*)      !input, [4,nintv+1] polynomial coefficients
      Real*8      y(*)      !output, the evaluated points
!+Cubic spline evaluation.
      Call DPPITGV(na,a,nb,b,4,nintv,break,c,y)
      End
