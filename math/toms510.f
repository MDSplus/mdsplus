!  VAX/DEC CMS REPLACEMENT HISTORY, Element MDSIDL$TOMS510.FOR
!  *2    19-MAR-1993 14:11:16 TWF "Change name to avoid conflict"
!  *1    19-MAR-1993 14:10:08 TWF "Piecewise linear approximation for paste"
!  VAX/DEC CMS REPLACEMENT HISTORY, Element MDSIDL$TOMS510.FOR
c From:      IN%"netlibd@surfer.EPM.ORNL.GOV" 26-FEB-1991 22:49:24.88
c To:      HORNE@ALCVAX.PFC.MIT.EDU
c CC:      
c Subj:      Re:send 510 from toms
Caveat receptor.  (Jack) dongarra@cs.utk.edu, (Eric Grosse) research!ehg
Careful! Anything free comes with no guarantee.
c *** from netlib, Tue Feb 26 22:48:51 EST 1991 ***

      subroutine toms510(x, y, e, m, u, v, w, k, ip)
c piecewise linear approximations of fewest
c line segments within given tolerances.
c x,y,e and m contain input data.
c u,v,k and possibly w contain output.
c ip is a parameter determining the operation
c  of the program.
c x and y are input data arrays of m elements
c  x(i),y(i) contains the ith data point.
c e may be a single tolerance or a table of
c  tolerances depending on the value of ip.
c if e is an array, then e(i) is the tolerance
c  associated with x(i),y(i) and e must contain
c  m nonnegative elements.
c u and v are output arrays of k+1 elements.
c  u is a partition of the interval (x(1),x(n))
c  with u(1)=x(1) and u(k+1)=x(n).
c  v(i) is an ordinate to be associated with
c  u(i) in the approximation.  (if a continuous
c  approximation is requested, then v(i) is
c  'the' ordinate to be associated with u(i).)
c if a continuous approximation is requested,
c  then w is not used.  in this case the ith
c  approximating segment is the straight line
c  from u(i),v(i) to u(i+1),v(i+1).
c if a continuous approximation is not
c  requested, then w is a k-element output
c  array.  in this case the ith approximating
c  segment is the straight line from
c  u(i),w(i) to u(i+1),v(i+1), and v(1) is
c  set equal to w(1).
c k is the number of segments in the piece-
c  wise linear approximation generated.  in
c  case of an error return, k will be set to
c  zero.
c the control parameter ip is the product
c  of three indicators i1,i2 and i3.
c  i1 indicates whether or not e is an
c     array of tolerances.
c     i1 = -1 indicates e is an array
c     i1 = +1 indicates e is a single number.
c  i2 indicates whether or not the
c     approximation is to be restricted to
c     the 'tolerance band' about the data.
c     i2 = 1 indicates no band restriction
c     i2 = 2 indicates apply this restriction
c     (the 'tolerance band' is a piecewise
c     linear band centered at the data whose
c     width is determined by the tolerances
c     at the data points.)
c  i3 indicates whether or not the
c     approximation must be continuous.
c     i3 = 1 indicates continuity not required
c     i3 = 3 indicates continuity is required
c call stl2 (x,y,e,m,x,y,e,m,ip) will not
c  cause problems provided that
c  either a continuous approximation is
c  requested, or e is a sufficiently large
c  array.
c the program performs the following data
c  checks.  are the x-values in increasing
c  order.  are the tolerance(s) nonnegative.
c  is the number of data points greater than
c  one.  if any check fails, the program
c  returns with k set equal to 0.  in this
c  case no further processing is attempted.
      dimension x(m), y(m), e(m), u(m), v(m), w(m)
      n = m
      itch = ip
      j = 1
c error checks
      if (n.le.1) go to 400
      if (e(1).lt.0.0) go to 400
      do 10 l=2,n
        if (x(l-1).ge.x(l)) go to 400
        if (itch.ge.0) go to 10
        if (e(l).lt.0.0) go to 400
   10 continue
c initialization for entire program
      epsln = e(1)
      sgn = 1.0
      keep = 1
      i = 1
      u(1) = x(1)
      j = 2
      init = 1
      indc = 0
      go to 30
c initialization for each segment
   20 continue
      j = j + 1
      init = i
      indc = 0
      if (iabs(itch).lt.3) keep = i
      if (iabs(iabs(itch)-4).ne.2) go to 30
c restricted to tolerance band
      xeye = u(j-1)
      yeye = v(j-1)
      temp1 = epsln
      if (itch.lt.0) temp1 = temp1 + (sgn*e(i-1)-epsln)*(x(i)-u(j-1)
     * )/(x(i)-x(i-1))
      yinit = yeye - temp1 - temp1
      go to 40
   30 continue
c not restricted to tolerance band
      xeye = x(i)
      yeye = y(i) + epsln
      yinit = y(i) - epsln
      if (iabs(itch).eq.1 .or. i.eq.1) go to 40
      temp1 = epsln
      if (itch.lt.0) temp1 = sgn*e(i+1)
      smin = (y(i+1)-yeye-temp1)/(x(i+1)-xeye)
      if (itch.lt.0) temp1 = sgn*e(i-1)
      smax = (yeye-y(i-1)+temp1)/(xeye-x(i-1))
      if (keep.eq.i-1) go to 50
      it = i - 2
      xinit = xeye
      ipiv = i
      igraze = i
      i = i + 1
      go to 150
   40 continue
      if (xeye.ge.x(i)) i = i + 1
      if (itch.lt.0) epsln = sgn*e(i)
      dx = x(i) - xeye
      smax = (y(i)+epsln-yeye)/dx
      smin = (y(i)-epsln-yeye)/dx
   50 continue
      xinit = xeye
      ipiv = i
      igraze = i
c determination of individual segment
   60 continue
      if (i.eq.n) go to 260
      i = i + 1
   70 continue
c test for new *max* slope
      dx = x(i) - xeye
      if (itch.lt.0) epsln = sgn*e(i)
      temp1 = (y(i)+epsln-yeye)/dx
      test = temp1 - smax
      if (sgn.le.0.0) test = -test
      if (test.LT.0) GO TO 80
      if (test.EQ.0) GO TO 90
      GO TO 100
c      if (test) 80, 90, 100
   80 continue
c test for end of candidate segment
      test = temp1 - smin
      if (sgn.le.0.0) test = -test
      if (test.lt.0.0) go to 210
      smax = temp1
   90 continue
c test for new *min* slope
      ipiv = i
  100 continue
      temp2 = (y(i)-epsln-yeye)/dx
      test = temp2 - smax
      if (sgn.le.0.0) test = -test
      if (test.lt.0) GO TO 110
      if (test.eq.0) GO TO 120
      GO TO 140
c      if (test) 110, 120, 140
  110 continue
      test = smin - temp2
      if (sgn.le.0.0) test = -test
      if (test.lt.0) GO TO 120
      if (test.eq.0) GO TO 130
      GO TO 60
c      if (test) 120, 130, 60
  120 continue
      smin = temp2
  130 continue
      igraze = i
      go to 60
c check for pivot at new eye point
  140 continue
      if (xeye.eq.x(ipiv)) go to 220
      if (itch.lt.0) epsln = sgn*e(ipiv)
      indc = 1
      svx = xeye
      svy = yeye
      svmn = smin
      svmx = smax
      xeye = x(ipiv)
      yeye = y(ipiv) + epsln
      smin = smax
      smax = (yinit-yeye)/(xinit-xeye)
      if (keep.ge.ipiv) go to 170
      it = ipiv - 1
  150 continue
      temp2 = yeye + epsln
      do 160 l=keep,it
        if (itch.lt.0) temp2 = yeye + sgn*e(l)
        temp1 = (y(l)-temp2)/(x(l)-xeye)
        test = temp1 - smax
        if (sgn.le.0.0) test = -test
        if (test.lt.0.0) smax = temp1
  160 continue
  170 continue
      if (ipiv.ge.i-1) go to 70
      it = i - 2
      temp2 = yeye - epsln
      idiot = ipiv
      do 200 l=idiot,it
        dx = x(l+1) - xeye
        if (itch.lt.0) temp2 = yeye - sgn*e(l+1)
        temp1 = (y(l+1)-temp2)/dx
        test = temp1 - smax
        if (sgn.le.0.0) test = -test
        if (test.lt.0) GO TO 180
        if (test.eq.0) GO TO 190
        GO TO 200
c        if (test) 180, 190, 200
  180   continue
        smax = temp1
  190   continue
        ipiv = l + 1
  200 continue
      go to 70
c end of current segment
  210 continue
      temp2 = smin
      if (i.eq.n) go to 240
      keep = igraze
      go to 250
  220 continue
      temp2 = smax
      if (i.eq.n) go to 230
      sgn = -sgn
      epsln = -epsln
      keep = ipiv
      go to 250
  230 continue
      if (indc.eq.0 .or. xeye.ne.x(n-1)) go to 240
      xeye = svx
      yeye = svy
      smin = svmn
      smax = svmx
  240 continue
      u(j) = x(n-1)
      yinit = y(n-1)
      go to 270
  250 continue
      if (iabs(iabs(itch)-4).ne.2) go to 300
c determine knot on edge of tolerance band
      temp1 = 0.0
      if (itch.lt.0) temp1 = epsln - sgn*e(i-1)
      temp1 = (y(i)-y(i-1)+temp1)/(x(i)-x(i-1))
      u(j) = (y(i)+epsln-yeye-temp1*x(i)+temp2*xeye)/(temp2-temp1)
      go to 310
  260 continue
      u(j) = x(n)
      yinit = y(n)
  270 continue
c continuity check for last segment
      if (iabs(itch).ge.3 .or. init.eq.1) go to 290
      it = init - 1
      svmx = smax + sgn
      temp2 = yeye + epsln
      do 280 l=kp,it
        if (itch.lt.0) temp2 = yeye + sgn*e(l)
        temp1 = (y(l)-temp2)/(x(l)-xeye)
        test = temp1 - svmx
        if (sgn.le.0.0) test = -test
        if (test.lt.0.0) svmx = temp1
  280 continue
      if (abs(svmx-smax+svmx-smin).le.abs(smax-smin)) smax = svmx
  290 continue
c nearness check for last segment
      temp2 = smax
      temp1 = yeye + smax*(u(j)-xeye)
      test = yinit - temp1
      if (sgn.lt.0.0) test = -test
      if (test.gt.0.0) go to 310
      temp2 = smin
      temp1 = yeye + smin*(u(j)-xeye)
      test = yinit - temp1
      if (sgn.lt.0.0) test = -test
      if (test.lt.0.0) go to 310
      temp2 = (yinit-yeye)/(u(j)-xeye)
      v(j) = yinit
      go to 320
  300 continue
      if (iabs(itch).ge.3) go to 330
      u(j) = 0.5*(x(i)+x(i-1))
  310 continue
      v(j) = yeye + temp2*(u(j)-xeye)
  320 continue
      if (xeye.ne.xinit) go to 330
      if (iabs(itch).eq.2) go to 360
      if (iabs(itch).ne.6) go to 330
      if (j.le.2) go to 380
      go to 390
  330 continue
c recomputation of knot for continuity
      if (j.le.2) go to 370
      if (slope.eq.temp2) go to 360
      yinit = v(j-2)
      if (iabs(itch).lt.3) yinit = w(j-2)
      temp1 = (xeye*temp2-u(j-2)*slope+yinit-yeye)/(temp2-slope)
      if (iabs(itch).ge.3) go to 350
      if (temp1.gt.xinit) go to 360
      test = abs(epsln)
      idiot = init - kp
      do 340 l=1,idiot
        it = init - l
        if (temp1.ge.x(it)) go to 350
        dx = y(it) - yeye - temp2*(x(it)-xeye)
        if (itch.lt.0) test = e(it)
        if (abs(dx).gt.test) go to 360
  340 continue
  350 continue
      u(j-1) = temp1
      v(j-1) = yeye + temp2*(u(j-1)-xeye)
      if (iabs(itch).lt.3) w(j-1) = v(j-1)
      go to 390
  360 continue
      w(j-1) = yeye + temp2*(u(j-1)-xeye)
      go to 390
  370 continue
      if (iabs(itch).lt.3) go to 360
  380 continue
      v(1) = yeye + temp2*(u(1)-xeye)
  390 continue
      slope = temp2
      kp = keep
      if (i.lt.n) go to 20
      if (x(n).eq.u(j)) go to 400
      if (iabs(itch).lt.3) w(j) = v(j)
      j = j + 1
      u(j) = x(n)
      v(j) = y(n)
  400 continue
      if (j.ge.2 .and. iabs(itch).lt.3) v(1) = w(1)
      k = j - 1
      return
      end
