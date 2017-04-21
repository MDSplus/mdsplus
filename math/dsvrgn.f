!      Ken Klare, LANL CTR-7      (c)1990
!***********************************************************************
!********************* 10.3 Sort ***************************************
!-----------------------------------------------------------------------
      Subroutine DSVRGN(n,ra,rb)
      Integer      n      !input, number of elements in array
      Real*8      ra(*)      !input, [n] vector to be sorted
      Real*8      rb(*)      !output, [n] vector, may be ra sort without permutations
      Integer iperm(1)
      Call DSVRGP(n,ra,rb,.false.,iperm)
      End
!-----------------------------------------------------------------------
      Subroutine DSVRGP(n,ra,rb,perm,iperm)
      Integer      n      !input, number of elements in array
      Real*8      ra(*)      !input, [n] vector to be sorted
      Real*8      rb(*)      !output, [n] vector, may be ra
        Logical perm    !input, true if iperm provided
      Integer iperm(*)!modify, [n] vector of permutations
!            Normally, iperm is initialized to 1..n
!+Hoare quick sort modified from "Numerical Recipies" with permutations
!*uses randomizer and not median-of-three
      Integer ran,i,j,l,r,jstack,stack(64),kperm
      Real*8      keep
      Data      ran/0/

      jstack = 0
      l = 1
      r = n
      Do j=1,n
            rb(j) = ra(j)
      Enddo
      Do While (.true.)
! insertion sort for small runs is faster
            Do While (r-l .lt. 11)
                  Do j=l+1,r
                        If (perm) kperm = iperm(j)
                        keep = rb(j)
                        i = j-1
                        Do While (i.gt.0.and.rb(i).gt.keep)
                              If (perm) iperm(i+1) = iperm(i)
                              rb(i+1) = rb(i)
                              i = i-1
                        Enddo
                        If (perm) iperm(i+1) = kperm
                        rb(i+1) = keep
                  Enddo
                  If (jstack.le.0) Return
                  l = stack(jstack-1)
                  r = stack(jstack)
                  jstack = jstack-2
            Enddo
            ran = mod(ran*211+1663,7875)
            i = l + (r-l+1)*ran/7875
            If (perm) kperm = iperm(i)
            If (perm) iperm(i) = iperm(l)
            keep = rb(i)
            rb(i) = rb(l)
            i = l
            j = r
! bigger ones at higher location stick
! smaller ones at lower location stick
! stuff misfits in opposite side
            Do While (.true.)
                  Do While (i.lt.j.and.rb(j).gt.keep)
                        j = j-1
                  Enddo
                  If (i.eq.j) Goto 30
                  If (perm) iperm(i) = iperm(j)
                  rb(i) = rb(j)
                  i = i+1
                  Do While (i.lt.j.and.keep.gt.rb(i))
                        i = i+1
                  Enddo
                  If (i.eq.j) Goto 30
                  If (perm) iperm(j) = iperm(i)
                  rb(j) = rb(i)
            Enddo
30            If (perm) iperm(i) = kperm
            rb(i) = keep
! with i as preakpoint, put large block on stack for latter
            jstack = jstack+2
            If (i-l.le.r-i) Then
                  stack(jstack-1) = i+1
                  stack(jstack) = r
                  r = i - 1
            Else
                  stack(jstack-1) = l
                  stack(jstack) = i-1
                  l = i + 1
            Endif
      Enddo
      End
!-----------------------------------------------------------------------
      Subroutine DSVRBN(n,ra,rb)
      Integer      n      !input, number of elements in array
      Real*8      ra(*)      !input, [n] vector to be sorted
      Real*8      rb(*)      !output, [n] vector, may be ra
        Integer iperm(1)
!+sort absolute without permutations
      Call DSVRBP(n,ra,rb,.false.,iperm)
      End
!-----------------------------------------------------------------------
      Subroutine DSVRBP(n,ra,rb,perm,iperm)
      Integer      n      !input, number of elements in array
      Real*8      ra(*)      !input, [n] vector to be sorted
      Real*8      rb(*)      !output, [n] vector, may be ra
        Logical perm    !input, true if iperm provided
      Integer iperm(*)!modify, [n] vector of permutations
!            Normally, iperm is initialized to 1..n
!            If a NULL address it is not referenced.
!+Hoare quick sort modified from "Numerical Recipies" with permutations
      Integer ran,i,j,l,r,jstack,stack(64),kperm
      Real*8      keep,bkeep
      Data      ran/0/

      jstack = 0
      l = 1
      r = n
      Do j=1,n
            rb(j) = ra(j)
      Enddo
      Do While (.true.)
! insertion sort for small runs is faster
            Do While (r-l .lt. 11)
                  Do j=l+1,r
                        If (perm) kperm = iperm(j)
                        keep = rb(j)
                        bkeep = abs(keep)
                        i = j-1
                        Do While (i.gt.0.and.abs(rb(i)).gt.bkeep)
                              If (perm) iperm(i+1) = iperm(i)
                              rb(i+1) = rb(i)
                              i = i-1
                        Enddo
                        If (perm) iperm(i+1) = kperm
                        rb(i+1) = keep
                  Enddo
                  If (jstack.le.0) Return
                  l = stack(jstack-1)
                  r = stack(jstack)
                  jstack = jstack-2
            Enddo
            ran = mod(ran*211+1663,7875)
            i = l + (r-l+1)*ran/7875
            If (perm) kperm = iperm(i)
            If (perm) iperm(i) = iperm(l)
            keep = rb(i)
            bkeep = abs(keep)
            rb(i) = rb(l)
            i = l
            j = r
! bigger ones at higher location stick
! smaller ones at lower location stick
! stuff misfits in opposite side
            Do While (.true.)
                  Do While (i.lt.j.and.abs(rb(j)).gt.bkeep)
                        j = j-1
                  Enddo
                  If (i.eq.j) Goto 30
                  If (perm) iperm(i) = iperm(j)
                  rb(i) = rb(j)
                  i = i+1
                  Do While (i.lt.j.and.keep.gt.abs(rb(i)))
                        i = i+1
                  Enddo
                  If (i.eq.j) Goto 30
                  If (perm) iperm(j) = iperm(i)
                  rb(j) = rb(i)
            Enddo
30            If (perm) iperm(i) = kperm
            rb(i) = keep
! with i as preakpoint, put large block on stack for latter
            jstack = jstack+2
            If (i-l.le.r-i) Then
                  stack(jstack-1) = i+1
                  stack(jstack) = r
                  r = i - 1
            Else
                  stack(jstack-1) = l
                  stack(jstack) = i-1
                  l = i + 1
            Endif
      Enddo
      End
