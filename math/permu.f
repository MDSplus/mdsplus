!      Ken Klare, LANL CTR-7      (c)1990
!***********************************************************************
!********************* 10.2 Permute ************************************
!-----------------------------------------------------------------------
      Subroutine PERMU(n,x,ipermu,ipath,xpermu)
      Integer      n      !input, length of x and xpermu
      Real      x(*)      !input, [n] vector to be permuted
      Integer      ipermu(*)!input, [n] vector of a permutation on 1 to n
      Integer      ipath      !input, 1=forward X(P(i)) to XP(i)
!                        2=backward X(i) to XP(P(i))
      Real      xpermu(*)!output, [n] vector of permuted values
!                  xpermu may be same storage as x
! WARNING loops forever if it is not a permutation
!+Rearrange the elements of an vector as specified
      Integer      j,k
      Real      tmp,xkeep

! need flag to see if we have been there
      Do j = 1,n
            ipermu(j) = -ipermu(j)
      Enddo
      If (ipath.eq.1) Then
            Do j = 1,n
                  If (ipermu(j).lt.0) Then
                        ipermu(j) = -ipermu(j)
                        k = ipermu(j)
                        tmp = x(k)
                        Do While (k.ne.j)
                              ipermu(k) = -ipermu(k)
                              xpermu(k) = x(ipermu(k))
                              k = ipermu(k)
                        Enddo
                        xpermu(j) = tmp
                  Endif
            Enddo
! idea from Linpack manual for SQRST
      Else
            Do j = 1,n
                  If (ipermu(j).lt.0) Then
                        xkeep = x(j)
                        ipermu(j) = -ipermu(j)
                        k = ipermu(j)
                        Do While (k.ne.j)
                              tmp = xkeep
                              xkeep = x(k)
                              xpermu(k) = tmp
                              ipermu(k) = -ipermu(k)
                              k = ipermu(k)
                        Enddo
                        xpermu(j) = xkeep
                  Endif
            Enddo
      Endif
      End
!-----------------------------------------------------------------------
      Subroutine PERMA(nra,nca,a,lda,ipermu,ipath,aper,ldaper)
      Integer      nra      !input, number of rows, first dim
      Integer nca      !input, number of columns, second dim
      Integer lda      !input, leading dimension of a in caller
      Real      a(lda,*)!input, [nra,nca] element matrix
      Integer      ipermu(*)!input, [nra or nca] permutation
      Integer      ipath      !input, option 1=rows, 2=columns permuted
      Integer ldaper      !input, leading dimension of aper in caller
      Real      aper(ldaper,*)!output, [nra,nca] element matrix
!            aper may be the same storage as a
!+Array permutation of rows or columns
      Integer      j,k,l
        Real temp(nra,nca)
      If (ipath.eq.1) Then
            Do j = 1,nca
                  Call PERMU(nra,a(1,j),ipermu,1,aper(1,j))
            Enddo
      Else
            Do j = 1,nra
                  ipermu(j) = -ipermu(j)
            Enddo
            Do j = 1,nca
               If (ipermu(j).lt.0) Then
                  ipermu(j) = -ipermu(j)
                  k = ipermu(j)
                  Call P9CPY(nra,a(1,k),1,temp,1)
                  Do While (k.ne.j)
                     ipermu(k) = -ipermu(k)
                     l=ipermu(k)
                     Call P9CPY(nca,a(1,ipermu(k)),1,aper(1,k),1)
                     k = ipermu(k)
                  Enddo
                  Call P9CPY(nra,temp,1,aper(1,j),1)
               Endif
            Enddo
      Endif
      End
!-----------------------------------------------------------------------
      Subroutine P9CPY(n,x,incx,y,incy)
      Integer      n,incx,incy
      Real      x(*),y(*)
      Integer ix,iy
      ix = 1
      Do iy = 1,n*incy,incy
            y(iy) = x(ix)
            ix = ix + incx
      Enddo
      End
