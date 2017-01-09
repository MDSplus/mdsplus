C*16-NOV-90 14:24:57 MDS$SOURCE:[KKLARE]ZMUL.FOR;5
      SUBROUTINE Cadd(m,n,a,lda,b,ldb,c,ldc)
! matrix add C=A+B, C(M,N)=A(M,N)+B(M,N)
      Integer m,n,lda,ldb,ldc,i,j
      COMPLEX a(lda,1),b(ldb,1),c(ldc,1)
      If(m.le.0) Return
      Do 20 j=1,n
         Do 10 i=1,m
            c(i,j)=a(i,j)+b(i,j)
10       Continue
20    Continue
      Return
      End
      SUBROUTINE Csub(m,n,a,lda,b,ldb,c,ldc)
! matrix subtract C=A-B, C(M,N)=A(M,N)-B(M,N)
      Integer m,n,lda,ldb,ldc,i,j
      COMPLEX a(lda,1),b(ldb,1),c(ldc,1)
      If(m.le.0) Return
      Do 20 j=1,n
         Do 10 i=1,m
            c(i,j)=a(i,j)-b(i,j)
10       Continue
20    Continue
      Return
      End
      SUBROUTINE Cmul(l,m,n,a,lda,b,ldb,c,ldc)
! matrix multiply C=A*B, C(L,N)=A(L,M)*B(M,N), C must not overlap A or B
ccif test
c      test2
ccendc test3
      external CDOTU
      Integer l,m,n,lda,ldb,ldc,i,j
      COMPLEX a(lda,1),b(ldb,1),c(ldc,1),CDOTU
      If(l.le.0) Return
      Do 20 j=1,n
         Do 10 i=1,l
            c(i,j)=CDOTU(m,a(i,1),lda,b(1,j),1)
10       Continue
20    Continue
      Return
      End
      SUBROUTINE Cmtm(l,m,n,a,lda,b,ldb,c,ldc)
! matrix transpose multiply C=AT*B, C(M,N)=A(L,M)*B(L,N)
! C must not overlap A or B
      external CDOTU
      Integer l,m,n,lda,ldb,ldc,i,j
      COMPLEX a(lda,1),b(ldb,1),c(ldc,1),CDOTU
      If(m.le.0) Return
      Do 20 j=1,n
         Do 10 i=1,m
            c(i,j)=CDOTU(l,a(1,i),1,b(1,j),1)
10       Continue
20    Continue
      Return
      End
