C 2-Aug-1992 fcn loop allowed
C*25-APR-88 17:03:50 HSC007$DUA0:[105.MATH]GN.FOR;1
C* 1-AUG-86 15:06:34 HSC007$DUA0:[142.OPT]GN.FOR;77
C+damped Gauss-Newton nonlinear least squares
	SUBROUTINE GN(fcn,iprint,n,m,limit,ier,xacc,facc,x0,f0,
     1 Jac,H,L,r0,r,e,g,s,sN,x)
c The user routine must compute r(1:m) from x(1:n).
c Please scale the residuals: r(i)=(fun(n,x)-y(i))/yerror(i).
c To reject the x, set r(1).gt.sqrt(largest number).
c (Notation: <p,q> is inner product of p transposed with q.)
c	To reject the x, set r(1).gt.sqrt(largest number).
c
c<< Lines marked like this one use a Fortran/C EXTERNAL function to
c<< define your residuals. All other values must be passed by COMMON.
c<< fcn	in:	user routine name declared EXTERNAL, FCN(n,m,x,r).
c
c>> Lines like this one let you can use the inside-out version.
c>> fcn	in/out: derivative number. Must be set to -1 on first entry.
c>>	Program will return for each evaluation of function.
c>>	YOU MUST CHECK ier and EVALUATE at "x" and return as "r".
c>>	WARNING: All other call variables must be unchanged.
c>>	This allows you to use any call form you wish.
c>>	EXAMPLE USE:
c>>		ider=-1
c>>	111	CALL GN(ider,...
c>>		IF (ier.EQ.0) THEN
c>>			CALL FCN(n,m,x,r)
c>>			GOTO 111
c>>		ENDIF
c iprint in: degree of printout:
c	0=none, 1=final, 2=fina x0, 3=iterations, 4=each x tried,
c	5=gradient and weight, 6=others, 7=Jacobian and Hessian.
c n	in: number of variables.
c m	in: number of equation or fitting residuals.
c	Overdetermined for m>n, underdetermined for m<n.
c limit	in: number of evaluations allowed.
c ier	out: error code.
c	1 step less than tolerance (xacc).
c	2 relative improvement in <r,r> less than tolerance (facc).
c	4 gradient*step less than tolerance (facc/4). May be local minimum.
c	8 <r,r> less than absolute tolerance (facc).
c	32 <J,J> likely not be positive definite, unused variable.
c	64 residuals were too large, f0 returned is sum(abs(r0)).
c	128 input error.
c	130 evaluation limit exceeded.
c	131 maximum step taken 5 times, function runs away.
c	133 singular <J,J>, may be unchanging function.
c xacc	in: step size for relative convergence test and derivatives.
c	Minimum step accuracy (xacc) is set to precision**(2/3).
c	Minimum derivative step size (xacc/10) is precision**(1/2).
c facc	in: value relative/absolute convergence test.
c	Set xacc and facc smaller than desired but bigger than noise.
c	Minimum absolute accuracy (facc) is smallest number/precision.
c	Minimum relative accuracy (facc/4) is set to 8*precision.
c x0	in/out: initial guess and best position found.
c f0	out: final sum of squares. See ier=64.
c Jac	out: Jacobian, m-by-n array.
c H	out: n-by-n array upper triangle. JacT*J portion of Hessian.
c L	scratch: n-by-n array. May share n-by-(n+1), L(1,2)=H(1,1).
c r0	out: final residuals, m vector.
c r	scratch, m vector.
c e,g,sN,s,x scratch: n vectors.
c	These are scale factors, gradient, Newton step, and step tried.
c	You must scale them by e to get an f change of 1/2.
c	Use Jac(i,j)/e(j), H(i,j)/e(i)/e(j), g(j)/e(j) and, sN(j)*e(j).
c
c credits:
c	Dennis and Schnabel, "Numerical Methods for Unconstrained
c	Optimization and Nonlinear Equations" (1983), for many ideas
c	and the hook calculation, that is, mu from phi.
c	Jacobian update: orthogonal Barnes (1965), w=s Broyden (1965)
c	cited in Fletcher, "Practical Methods of Optimization" (1980).
c x(+) = x - (<J,J> + mu I)**-1 <J,R>, augmented Gauss-Newton method.
c<<<	external FCN
c>>>
	integer fcn
	external OASUM,ODOT,OCHOL,OSOLVE
	integer n,m,limit,ier,iprint
	real*8 xacc,facc,x0(n),f0
	real*8 Jac(m,n),H(n,n),L(n,n),r0(m),r(m)
	real*8 e(n),g(n),s(n),sN(n),x(n)
c
c	intrinsic abs,max,min,sign,sqrt
	logical first,final,hook,badf,badf0,take,done,full
	integer i,j,iter,nfcn,consec,nrank1,nbad
	real*8 OASUM,ODOT,abstol,del,dlast,EPS,epsn12,f
	real*8 gnorm,hnorm,mu,mulow,muup,phi
	real*8 phip,phipin,reltol,scal,scal0,sing,snewt,snorm,step
	real*8 stpmax,stpsiz,stptol,temp,TINY
c tuning constants:
c	ZLOW,ZHIGH bounds on adjustment of del (.5,2).
c	ZHIGH>2 improves linear equations, hurts fitting.
c	Small changes in the path set by them greatly affect results.
c	ZWILD prevents wild Jacobian updates (1e4).
c	ZDERIV fraction of step tolerance for a derivative (.1).
c	ZNEWT Newton step threshold of del <ZHIGH and <1/ZLOW (1.5).
c	ZFULL Newton step too small wrt del <1/ZHIGH (.5).
c	ZHOOKL,ZHOOKH tolerance on mu match (.75,1.5).
c	Tighter tolerance cost more computation here.
c	MXBAD number bad steps with rank-1 updates (1).
	integer MXBAD
	real*8 ZLOW,ZHIGH,ZWILD,ZDERIV,ZNEWT,ZFULL,ZHOOKL,ZHOOKH
	real*8 D1,D2,D8,D1000,P0,P001,P25,P375,P5,P6667
	parameter (MXBAD=1,ZLOW=.5,ZHIGH=2,ZWILD=1e4,ZDERIV=.1)
	parameter (ZNEWT=1.5,ZFULL=.5,ZHOOKL=.75,ZHOOKH=1.5)
	parameter (D1=1,D2=2,D8=8,D1000=1000)
	parameter (P0=.0,P001=.001,P25=.25,P375=.375,P5=.5,P6667=.6667)
c	data MXBAD/1/,ZLOW/.5/,ZHIGH/2/,ZWILD/1e4/,ZDERIV/.1/
c	data ZNEWT/1.5/,ZFULL/.5/,ZHOOKL/.75/,ZHOOKH/1.5/
c	data D1/1/,D2/2/,D8/8/,D1000/1000/
c	data P0/.0/,P001/.001/,P25/.25/,P375/.375/,P5/.5/,P6667/.6667/
	save TINY,EPS,epsn12,abstol,reltol,stptol,stpsiz,stpmax
	save consec,nfcn,nrank1,nbad,badf0,full
	save step,scal,scal0,snorm,del,mu,hook
c>>>
	if (fcn.eq.0) goto 12
c>>>
	if (fcn.gt.0) goto 27
600	format(1x,a,(5g15.7))
620	format(' iter'i4,' nf'i4,' f'g22.14,' ier'i4,l2)
	ier=128
	if(n.le.0.or.m.le.0.or.limit.le.0.or.
     1 xacc.ge.D1.or.facc.ge.D1) then
		WRITE(*,600) 'INPUT ERROR(n,m,limit,xacc,facc)'
     1		,n,m,limit,xacc,facc
		return
	endif
c Machine precision is number of mantissa bits less one.
c Half exponent range on low side. (.5/TINY)**2 should be OK.
c If underflows are not graceful, replace with:
c Computer EPS-s TINY-s	EPS-d	TINY-d	EPS-g	TINY-g	EPS-h	TINY-h
c VAX	.5**23	.5**64	.5**55	.5**64	.5**52	.5**512	.5**112	.5**16384
c Prime	.5**22	.5**64	.5**46	.5**16384 h?
c CDC7600 .5**47 .5**512 .5**95 .5**512
c IBM-370 .5**20 .5**128 .5**52 .5**128 h?
	EPS=P5
1	temp=D2+P375*EPS
	EPS=P5*EPS
	if(temp.gt.D2) goto 1
	TINY=P5
2	temp=P375*(TINY**2)**2
	TINY=TINY**2
	if(temp.gt.P0) goto 2
	EPS=max(EPS,TINY)
	epsn12=sqrt(n*EPS)
	abstol=max((TINY/EPS)*TINY,facc)
	reltol=max(D8*EPS,facc)
	stptol=max(EPS**P6667,xacc)
	stpsiz=max(sqrt(EPS),ZDERIV*xacc)
	stpmax=D1000*n
	do 3 j=1,n
		x(j)=x0(j)
		s(j)=D1
3		e(j)=D1
	consec=0
	nfcn=0
	nrank1=n
c REPEAT UNTIL ier.ne.0 and have a new Jacobian.
	iter=1
c<<<	do 90 iter=1,limit
10	ier=0
	nfcn=nfcn+1
c<<<	call FCN(n,m,x,r)
c>>>
	fcn=0
c>>>
	RETURN
12	f=OASUM(m,r,1)
	badf=f*TINY.ge.P25
	if(.not.badf) f=ODOT(m,r,1,r,1)
c Update trust region.
	full=.false.
	if(iter.eq.1) then
		take=.true.
		del=P0
		snorm=P0
	elseif(badf0.eqv.badf) then
c Strict descent requires f<f0.
		take=f.lt.f0
		if(abs(f-f0).le.max(reltol*f,abstol)) ier=2
c Adjust snorm/2 < del < 2*snorm based on parabola.
		temp=-ODOT(n,g,1,s,1)
		if(temp.gt.TINY*f.and.temp.GT.P0) then
c Function is sum f(i)**2 but g is d(f/2)/dx.
			temp=P5/max((P5*(f-f0)+temp)/temp,EPS)
			full=temp.ge.ZHIGH
			del=snorm*min(max(temp,ZLOW),ZHIGH)
		endif
	else
		take=.not.badf
		if(badf) del=ZLOW*del
	endif
	if(take) then
c Save the best.
		badf0=badf
		nbad=0
		f0=f
		do 14 j=1,n
14			x0(j)=x(j)
	else
c Overshot the mark. Try, then take a new Jacobian.
		hook=.true.
		nbad=nbad+1
	endif
c Some convergence tests.
	if(iter.gt.1) then
		consec=consec+1
		if(.not.take.or.del.lt.stpmax) consec=0
		if(del.le.stptol) ier=ier+1
		if(take.and..not.badf.and.
     1	snorm*gnorm.le.P25*max(reltol*f0,abstol)) ier=ier+4
	endif
	if(.not.badf.and.f.le.abstol) ier=ier+8
	done=(ier.ne.0.and.nrank1.eq.0).or.ier.ge.8
	if(done.and.sing.lt.hnorm) ier=ier+32
	if(done.and.badf0) ier=ier+64
	if(.not.done.and.nfcn.ge.limit) ier=130
	if(.not.done.and.consec.ge.5) ier=131
c Printout.
	if(iprint.ge.5) write(*,600)
	if(iprint.ge.3) write(*,620) iter,nfcn,f,ier,take
	if(iprint.ge.4) write(*,600) 'x',x
	if(iter.gt.1.and.(done.or.ier.ge.128)) then
		if(iprint.ge.1) write(*,620) iter,nfcn,f0,ier,take
		if(iprint.ge.2) write(*,600) 'x0',x0
		if(take) then
			do 18 i=1,m
18				r0(i)=r(i)
		endif
		return
	endif
c --- Jacobian dFi/dxj, gradient g=JacT*r, Hessian H=JacT*Jac ---
	if(ier.ne.0.or.nrank1.ge.n) nbad=MXBAD+1
	if(nbad.le.MXBAD.and..not.take) goto 40
	if(nbad.le.MXBAD.and.snorm.gt.stpsiz) then
c Rank-1 update to Jacobian. Jac' = Jac + (r-r0-Jac*s) sT / (sT s).
		nrank1=nrank1+1
		step=(D1/snorm)**2
		do 22 i=1,m
			temp=(r(i)-r0(i)-ODOT(n,Jac(i,1),m,s,1))*step
			if(take) r0(i)=r(i)
			do 22 j=1,n
22				Jac(i,j)=Jac(i,j)+temp*s(j)
	else
c Full Jacobian, and update scaling. Rescale last step (del).
c Scaling x0:x, Jac:f/x-->f, g:f/x-->f, H:f/mu-->f, s,sN:x-->1.
c e=(|x|+e)/2 seems most effective.
24		nrank1=0
		nbad=0
		if(take) then
			do 25 i=1,m
25				r0(i)=r(i)
		endif
		nfcn=nfcn+n
		scal0=P0
		scal=P0
		do 255 j=1,n
255			x(j)=x0(j)
c<<<		do 29 j=1,n
c>>>		
26		if(fcn.lt.n) then
c>>>
			fcn=fcn+1
c>>>
			j=fcn
			scal0=scal0+(s(j)*e(j))**2
			e(j)=max(P5*(abs(x0(j))+e(j)),stptol)
			scal=scal+(s(j)*e(j))**2
			step=sign(stpsiz,x0(j))
			x(j)=x0(j)+step*e(j)
c<<<			call FCN(n,m,x,r)
c>>>
			ier=0
c>>>
			RETURN
c>>>
27			j=fcn
			do 28 i=1,m
28				Jac(i,j)=(r(i)-r0(i))/step
29			x(j)=x0(j)
c>>>
		goto 26
c>>>
		endif		
		if(scal.gt.P0) del=sqrt(scal0/scal)*del
		if(iprint.ge.5) write(*,600) 'e',e
	endif
c Gradient and portion of Hessian.
	do 33 j=1,n
		g(j)=ODOT(m,Jac(1,j),1,r0,1)
		do 33 i=1,j
33			H(i,j)=ODOT(m,Jac(1,j),1,Jac(1,i),1)
	if(iprint.ge.7) then
		do 770 j=1,m
770			write(*,600) 'J',(Jac(j,i),i=1,n)
		do 771 j=1,n
771			write(*,600) 'H',(H(i,j),i=1,j)
	endif
	if(iprint.ge.5) write(*,600) 'g',g
	gnorm=sqrt(ODOT(n,g,1,g,1))
c --- get Newton step, H sN = -g ---
c L1 norm of H to augment matrix a little.
	hnorm=P0
	do 35 j=1,n
35		hnorm=max(hnorm,OASUM(j,H(1,j),1)+OASUM(n-j,H(j,j+1),n))
	do 36 j=1,n
36		if(H(j,j).le.P0) H(j,j)=hnorm
	hnorm=hnorm*epsn12
c Decomposition without addition if it works.
	call OCHOL(n,P0,H,g,L,sN,sing)
	if(sing.lt.hnorm) call OCHOL(n,hnorm,H,g,L,sN,sing)
	if(sing.le.-hnorm) goto 91
	snewt=sqrt(ODOT(n,sN,1,sN,1))
c Put bad update away, improves reliability.
	if(nrank1.ne.0.and.
     1	(snewt.lt.del/ZWILD.or.snewt.gt.ZWILD*del)) goto 24
	if(iprint.ge.6) write(*,600) 'sN',sN
	first=.true.
	if(sing.lt.P0.or.del.le.P0.and.gnorm.gt.P0) then
c Take Cauchy point, CP = -||g||**2/(g H g) g, g H g = ||LT g||**2.
c Select Cauchy distance in Newton direction.
c Steps are the same for identity Hessian and single directions.
		hook=.false.
		mu=P0
		temp=P0
		do 39 j=1,n
39			temp=temp+(ODOT(n-j+1,L(j,j),1,g(j),1)/gnorm)**2
		del=min(gnorm/temp,stpmax)
	endif
c --- Choose Newton or hook steps ---
40	if(iprint.ge.6) write(*,600)'g,del,sn,s',gnorm,del,snewt,snorm
	if(snewt.le.ZNEWT*del.or..not.hook) then
		mu=P0
		temp=D1
		if(snewt.gt.ZNEWT*del.and..not.hook) temp=del/snewt
c Take a giant step when del is pegged. May I?
	if(full.and.snewt.le.ZFULL*del.and.snewt.gt.P0) temp=del/snewt
		do 43 j=1,n
43			s(j)=sN(j)*temp
		snorm=snewt*temp
		del=min(del,snorm)
	else
c --- Hook search ---
		if(mu.gt.P0) mu=mu-((phi+dlast)/del)*(((dlast-del)+phi)/phip)
		phi=snewt-del
		if(first) then
		first=.false.
c Want s H**-1 s = ||L**-1 s||**2.
		call OSOLVE(n,L,x,sN)
		phipin=-ODOT(n,x,1,x,1)/snewt
	endif
	mulow=-phi/phipin
	muup=gnorm/del
c REPEAT UNTIL .75*del<||s||<1.5*del or mulow>muup.
44	continue
	if(iprint.ge.6) write(*,600) 'mu',mulow,mu,muup
	if(mu.lt.mulow.or.mu.gt.muup) mu=max(sqrt(mulow*muup),P001*muup)
	call OCHOL(n,mu,H,g,L,s,sing)
	if(sing.le.-mu) goto 91
	snorm=sqrt(ODOT(n,s,1,s,1))
	phi=snorm-del
	call OSOLVE(n,L,x,s)
	phip=P0
	IF (snorm.GT.P0) phip=-ODOT(n,x,1,x,1)/snorm
	if((snorm.lt.ZHOOKL*del.or.snorm.gt.ZHOOKH*del)
     1	.and.mulow.lt.muup) then
		mulow=max(mulow,mu-phi/phip)
		if(phi.lt.P0) muup=mu
		mu=mu-(snorm/del)*(phi/phip)
		goto 44
	endif
c Endof REPEAT.
		if(iprint.ge.6) write(*,600) 's',s
		dlast=del
	endif
c Take the step.
	do 61 j=1,n
61		x(j)=x0(j)+s(j)*e(j)
c<<<90	continue
c>>>
	iter=iter+1
c>>>
	goto 10
91	ier=133
	END
c-------------------------------------------------------
c+Cholesky decomposition and solution, (H + add) s = -g.
	SUBROUTINE OCHOL(n,add,H,g,L,s,sing)
c	intrinsic sqrt,min
	external ODOT
	integer n,i,j
	real*8 add,H(n,1),g(1),L(n,1),s(1),ODOT,sing
	sing=H(1,1)
	do 40 j=1,n
		do 38 i=j,n
38			L(i,j)=H(j,i)-ODOT(j-1,L(i,1),n,L(j,1),n)
		sing=min(sing,L(j,j))
		L(j,j)=L(j,j)+add
		if(L(j,j).le.0) return
		L(j,j)=sqrt(L(j,j))
		do 39 i=j+1,n
39			L(i,j)=L(i,j)/L(j,j)
40	continue
	do 41 i=1,n
41		s(i)=(-g(i)-ODOT(i-1,L(i,1),n,s,1))/L(i,i)
	do 42 i=n,1,-1
42		s(i)=(s(i)-ODOT(n-i,L(i+1,i),1,s(i+1),1))/L(i,i)
	END
c----------------------------------------------
c+solve L x = y. x and y may be the same vector.
	SUBROUTINE OSOLVE(n,L,x,y)
	external ODOT
	integer n,i
	real*8 L(n,1),x(1),y(1),ODOT
	do 41 i=1,n
41		x(i)=(y(i)-ODOT(i-1,L(i,1),n,x,1))/L(i,i)
	END
c-----------------------
c+sum of absolute values
	real*8 FUNCTION OASUM(nx,x,inc)
c	intrinsic abs
	integer nx,i,inc
	real*8 x(1)
	OASUM=0
	do 10 i=1,nx*inc,inc
10		OASUM=OASUM+abs(x(i))
	END
c--------------
c+inner product
	real*8 FUNCTION ODOT(nx,x,incx,y,incy)
	integer ix,iy,nx,incx,incy
	real*8 x(1),y(1)
	ODOT=0
	iy=1
	do 30 ix=1,nx*incx,incx
		ODOT=ODOT+x(ix)*y(iy)
30		iy=iy+incy
	END
c--------------------------------------------------
c+positive-definite decomposition to inverse matrix and scaling
	subroutine GN_ERR(n,L,e)
	integer i,j,k,n
	real*8 L(n,n),tmp,e(n)
c invert lower triangle decomposition matrix into itself
	do 40 k=1,n
		if (L(k,k).GT.0) L(k,k)=1/L(k,k)
		tmp=-L(k,k)
		do 10 j=1,k-1
10			L(k,j)=tmp*L(k,j)
		do 30 j=k+1,n
			tmp=L(j,k)
			L(j,k)=0
			do 20 i=1,k
20				L(j,i)=L(j,i)+tmp*L(k,i)
30		continue
40	continue
c multiply transpose by matrix to get inverse of L*LT
c (L*LT)**-1 = (L**-1)T*(L**-1)
	do 80 j=1,n
		do 60 k=1,j-1
			tmp=L(j,k)
			do 50 i=1,k
50				L(k,i)=L(k,i)+tmp*L(j,i)
60		continue
		tmp=L(j,j)
		do 70 k=1,j
70			L(j,k)=L(j,k)*tmp
80	continue
	do 90 j=1,n
		do 90 k=1,j
90			L(j,k)=L(j,k)*e(j)*e(k)
	end
