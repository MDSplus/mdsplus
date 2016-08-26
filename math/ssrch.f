!      Ken Klare, LANL CTR-7      (c)1990
!***********************************************************************
!********************* 10.4 Search *************************************
!-----------------------------------------------------------------------
      Subroutine SSRCH(n,value,x,incx,index)
      Integer      n      !input, length of vector Y
      Integer      incx      !input,      displacement between elements of X
      Character*(*)      value      !input, scalar to be found
      Character*(*)      x(incx,*)!input, [incx,n] table, Y(j)=x(1,j)
      Integer      index      !modify, index of Y pointing to value.
!                  1 to n      for exact match
!                  -1      for value < Y(1) or n=0
!                  -n to -2 for Y(-index-1) < value < Y(-index)
!                  -(n+1)      for value > Y(n)
!+Find index of value from an ascending list or bounding element.
! O(1) for successive values if the user maintains index.
! O(log2(n)) binary bisection search for random lookup.
! Adapted from "Numerical Recipes" and "A Practical Guide to Splines"
! Previous value of index is used as starting guess, multiple streams.
!! O(n) linear search version
!!      Do index = 1,n
!!          If (value.le.x(1,index)) Goto 10
!!      Enddo
!!      index = -(n+1)
!!      Return
!! 10      If (value.ne.x(1,index)) index = - index
      Integer      lo,hi,mid,inc,nn

      nn = n
      lo = index
      If (lo.lt.0) lo = -1-lo
      If (lo.gt.nn) Then
          lo = 0
          hi = nn+1
      Elseif (lo.le.0 .or. value.ge.x(1,lo)) Then
          inc = 1
1          hi = lo + inc
          If (hi.gt.nn) Then
            hi = nn+1
          Elseif (value.ge.x(1,hi)) Then
            lo = hi
            inc = inc+inc
            Goto 1
          Endif
      Else
          inc = 1
2          hi = lo
          lo = lo-inc
          If (lo.le.0) Then
            lo = 0
          Elseif (value.lt.x(1,lo)) Then
            inc = inc+inc
            Goto 2
          Endif
      Endif
3      If (hi-lo.gt.1) Then
          mid = (hi+lo)/2
          If (value.ge.x(1,mid)) Then
            lo = mid
          Else
            hi = mid
          Endif
          Goto 3
      Endif

      If (lo.eq.0) Then
          lo = -1
        Elseif (value.ne.x(1,lo)) Then
            lo = -1-lo
      Endif
      index = lo
      End
