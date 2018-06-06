      SUBROUTINE DCHDC(A,LDA,P,WORK,JPVT,JOB,INFO)
      INTEGER LDA,P,JPVT(*),JOB,INFO
      REAL*8 A(LDA,*),WORK(*)
      INTEGER PU,PL,PLP1,J,JP,JT,K,KB,KM1,KP1,L,MAXL
      REAL*8 TEMP
      REAL*8 MAXDIA
      LOGICAL SWAPK,NEGK
      PL=1
      PU=0
      INFO=P
      IF(JOB.EQ.0) GOTO 160
        DO 70 K=1,P
          SWAPK=JPVT(K).GT.0
          NEGK=JPVT(K).LT.0
          JPVT(K)=K
          IF(NEGK)JPVT(K)=-JPVT(K)
          IF(.NOT.SWAPK) GOTO 60
            IF(K.EQ.PL) GOTO 50
              CALL DSWAP(PL-1,A(1,K),1,A(1,PL),1)
              TEMP=A(K,K)
              A(K,K)=A(PL,PL)
              A(PL,PL)=TEMP
              A(PL,K)=(A(PL,K))
              PLP1=PL+1
              IF (P.LT.PLP1) GOTO 40
                DO 30 J=PLP1,P
                  IF(J.GE.K) GOTO 10
                    TEMP=(A(PL,J))
                    A(PL,J)=(A(J,K))
                    A(J,K)=TEMP
                    GOTO 20
10                CONTINUE
                  IF(J.EQ.K) GOTO 20
                    TEMP=A(K,J)
                    A(K,J)=A(PL,J)
                    A(PL,J)=TEMP
20                CONTINUE
30              CONTINUE
40            CONTINUE
              JPVT(K)=JPVT(PL)
              JPVT(PL)=K
50          CONTINUE
            PL=PL+1
60        CONTINUE
70      CONTINUE
        PU=P
        IF(P.LT.PL) GOTO 150
          DO 140 KB=PL,P
            K=P-KB+PL
            IF(JPVT(K).GE.0) GOTO 130
              JPVT(K)=-JPVT(K)
              IF(PU.EQ.K) GOTO 120
                CALL DSWAP(K-1,A(1,K),1,A(1,PU),1)
                TEMP=A(K,K)
                A(K,K)=A(PU,PU)
                A(PU,PU)=TEMP
                A(K,PU)=(A(K,PU))
                KP1=K+1
                IF(P.LT.KP1)GOTO 110
                  DO 100 J=KP1,P
                    IF(J.GE.PU) GOTO 80
                      TEMP=(A(K,J))
                      A(K,J)=(A(J,PU))
                      A(J,PU)=TEMP
                      GOTO 90
80                  CONTINUE
                    IF(J.EQ.PU) GOTO 90
                      TEMP=A(K,J)
                      A(K,J)=A(PU,J)
                      A(PU,J)=TEMP
90                  CONTINUE
100               CONTINUE
110             CONTINUE
                JT=JPVT(K)
                JPVT(K)=JPVT(PU)
                JPVT(PU)=JT
120           CONTINUE
              PU=PU-1
130         CONTINUE
140       CONTINUE
150     CONTINUE
160   CONTINUE
      DO 270 K=1,P
        MAXDIA=(A(K,K))
        KP1=K+1
        MAXL=K
        IF(K.LT.PL.OR.K.GE.PU) GOTO 190
          DO 180 L=KP1,PU
            IF((A(L,L)).LE.MAXDIA) GOTO 170
              MAXDIA=(A(L,L))
              MAXL=L
170         CONTINUE
180       CONTINUE
190     CONTINUE
        IF(MAXDIA.GT.0.0D0) GOTO 200
          INFO=K-1
          GOTO 280
200     CONTINUE
        IF(K.EQ.MAXL) GOTO 210
          KM1=K-1
          CALL DSWAP(KM1,A(1,K),1,A(1,MAXL),1)
          A(MAXL,MAXL)=A(K,K)
          A(K,K)=MAXDIA
          JP=JPVT(MAXL)
          JPVT(MAXL)=JPVT(K)
          JPVT(K)=JP
          A(K,MAXL)=(A(K,MAXL))
210     CONTINUE
        WORK(K)=SQRT((A(K,K)))
        A(K,K)=WORK(K)
        IF(P.LT.KP1) GOTO 260
          DO 250 J=KP1,P
            IF(K.EQ.MAXL) GOTO 240
              IF(J.GE.MAXL) GOTO 220
                TEMP=(A(K,J))
                A(K,J)=(A(J,MAXL))
                A(J,MAXL)=TEMP
                GOTO 230
220           CONTINUE
              IF(J.EQ.MAXL) GOTO 230
                TEMP=A(K,J)
                A(K,J)=A(MAXL,J)
                A(MAXL,J)=TEMP
230           CONTINUE
240         CONTINUE
            A(K,J)=A(K,J)/WORK(K)
            WORK(J)=(A(K,J))
            TEMP=-A(K,J)
            CALL DAXPY(J-K,TEMP,WORK(KP1),1,A(KP1,J),1)
250       CONTINUE
260     CONTINUE
270   CONTINUE
280   CONTINUE
      RETURN
      END
      
      SUBROUTINE DCHDD(R,LDR,P,X,Z,LDZ,NZ,Y,RHO,C,S,INFO)
      INTEGER LDR,P,LDZ,NZ,INFO
      REAL*8 R(LDR,*),X(*),Z(LDZ,*),Y(*),S(*)
      REAL*8 RHO(*),C(*)
      INTEGER I,II,J
      REAL*8 A,ALPHA,AZETA,NORM,DNRM2,SCALE
      REAL*8 DDOT,T,ZETA,B,XX
      INFO=0
      S(1)=(X(1))/(R(1,1))
      IF(P.LT.2) GOTO 20
        DO 10 J=2,P
          S(J)=(X(J))-DDOT(J-1,R(1,J),1,S,1)
          S(J)=S(J)/(R(J,J))
10      CONTINUE
20    CONTINUE
      NORM=DNRM2(P,S,1)
      IF(NORM.LT.1.0D0) GOTO 30
        INFO=-1
        GOTO 120
30    CONTINUE
      ALPHA=SQRT(1.0D0-NORM**2)
      DO 40 II=1,P
        I=P-II+1
        SCALE=ALPHA+ABS(S(I))
        A=ALPHA/SCALE
        B=S(I)/SCALE
        NORM=SQRT(A**2+(B)**2)
        C(I)=A/NORM
        S(I)=(B)/NORM
        ALPHA=SCALE*NORM
40    CONTINUE
      DO 60 J=1,P
        XX=(0.0D0)
        DO 50 II=1,J
          I=J-II+1
          T=C(I)*XX+S(I)*R(I,J)
          R(I,J)=C(I)*R(I,J)-(S(I))*XX
          XX=T
50      CONTINUE
60    CONTINUE
      IF(NZ.LT.1) GOTO 110
        DO 100 J=1,NZ
          ZETA=Y(J)
          DO 70 I=1,P
            Z(I,J)=(Z(I,J)-(S(I))*ZETA)/C(I)
            ZETA=C(I)*ZETA-S(I)*Z(I,J)
70        CONTINUE
          AZETA=ABS(ZETA)
          IF(AZETA.LE.RHO(J)) GOTO 80
            INFO=1
            RHO(J)=-1.0D0
            GOTO 90
80        CONTINUE
          RHO(J)=RHO(J)*SQRT(1.0D0-(AZETA/RHO(J))**2)
90        CONTINUE
100     CONTINUE
110   CONTINUE
120   CONTINUE
      RETURN
      END
      
      SUBROUTINE DCHEX(R,LDR,P,K,L,Z,LDZ,NZ,C,S,JOB)
      INTEGER LDR,P,K,L,LDZ,NZ,JOB
      REAL*8 R(LDR,*),Z(LDZ,*),S(*)
      REAL*8 C(*)
      INTEGER I,II,IL,IU,J,JJ,KM1,KP1,LMK,LM1
      REAL*8 T
      KM1=K-1
      KP1=K+1
      LMK=L-K
      LM1=L-1
      GOTO(10,130),JOB
10      CONTINUE
      DO 20 I=1,L
        II=L-I+1
        S(I)=R(II,L)
20    CONTINUE
      DO 40 JJ=K,LM1
        J=LM1-JJ+K
        DO 30 I=1,J
          R(I,J+1)=R(I,J)
30      CONTINUE
        R(J+1,J+1)=(0.0D0)
40    CONTINUE
      IF(K.EQ.1) GOTO 60
        DO 50 I=1,KM1
          II=L-I+1
          R(I,K)=S(II)
50      CONTINUE
60    CONTINUE
      T=S(1)
      DO 70 I=1,LMK
        CALL DROTG(S(I+1),T,C(I),S(I))
        T=S(I+1)
70    CONTINUE
      R(K,K)=T
      DO 90 J=KP1,P
        IL=MAX(1,L-J+1)
        DO 80 II=IL,LMK
          I=L-II
          T=C(II)*R(I,J)+S(II)*R(I+1,J)
          R(I+1,J)=C(II)*R(I+1,J)-(S(II))*R(I,J)
          R(I,J)=T
80      CONTINUE
90    CONTINUE
      IF(NZ.LT.1) GOTO 120
        DO 110 J=1,NZ
          DO 100 II=1,LMK
            I=L-II
            T=C(II)*Z(I,J)+S(II)*Z(I+1,J)
            Z(I+1,J)=C(II)*Z(I+1,J)-(S(II))*Z(I,J)
            Z(I,J)=T
100       CONTINUE
110     CONTINUE
120   CONTINUE
      GOTO260
130   CONTINUE
      DO 140 I=1,K
        II=LMK+I
        S(II)=R(I,K)
140   CONTINUE
      DO 160 J=K,LM1
        DO 150 I=1,J
          R(I,J)=R(I,J+1)
150     CONTINUE
        JJ=J-KM1
        S(JJ)=R(J+1,J+1)
160   CONTINUE
      DO 170 I=1,K
        II=LMK+I
        R(I,L)=S(II)
170   CONTINUE
      DO 180 I=KP1,L
        R(I,L)=(0.0D0)
180   CONTINUE
      DO 220 J=K,P
        IF(J.EQ.K) GOTO 200
          IU=MIN(J-1,L-1)
          DO 190 I=K,IU
            II=I-K+1
            T=C(II)*R(I,J)+S(II)*R(I+1,J)
            R(I+1,J)=C(II)*R(I+1,J)-(S(II))*R(I,J)
            R(I,J)=T
190       CONTINUE
200     CONTINUE
        IF(J.GE.L) GOTO 210
          JJ=J-K+1
          T=S(JJ)
          CALL DROTG(R(J,J),T,C(JJ),S(JJ))
210     CONTINUE
220   CONTINUE
      IF(NZ.LT.1) GOTO 250
        DO 240 J=1,NZ
          DO 230 I=K,LM1
            II=I-KM1
            T=C(II)*Z(I,J)+S(II)*Z(I+1,J)
            Z(I+1,J)=C(II)*Z(I+1,J)-(S(II))*Z(I,J)
            Z(I,J)=T
230       CONTINUE
240     CONTINUE
250   CONTINUE
260   CONTINUE
      RETURN
      END
      
      SUBROUTINE DCHUD(R,LDR,P,X,Z,LDZ,NZ,Y,RHO,C,S)
      INTEGER LDR,P,LDZ,NZ
      REAL*8 RHO(*),C(*)
      REAL*8 R(LDR,*),X(*),Z(LDZ,*),Y(*),S(*)
      INTEGER I,J,JM1
      REAL*8 AZETA,SCALE
      REAL*8 T,XJ,ZETA
      DO 30 J=1,P
        XJ=X(J)
        JM1=J-1
        IF(JM1.LT.1) GOTO 20
          DO 10 I=1,JM1
            T=C(I)*R(I,J)+S(I)*XJ
            XJ=C(I)*XJ-(S(I))*R(I,J)
            R(I,J)=T
10        CONTINUE
20      CONTINUE
        CALL DROTG(R(J,J),XJ,C(J),S(J))
30    CONTINUE
      IF(NZ.LT.1) GOTO 70
        DO 60 J=1,NZ
          ZETA=Y(J)
          DO 40 I=1,P
            T=C(I)*Z(I,J)+S(I)*ZETA
            ZETA=C(I)*ZETA-(S(I))*Z(I,J)
            Z(I,J)=T
40        CONTINUE
          AZETA=ABS(ZETA)
          IF(AZETA.EQ.0.0D0.OR.RHO(J).LT.0.0D0) GOTO 50
            SCALE=AZETA+RHO(J)
            RHO(J)=SCALE*SQRT((AZETA/SCALE)**2+(RHO(J)/SCALE)**2)
50        CONTINUE
60      CONTINUE
70    CONTINUE
      RETURN
      END
      
      SUBROUTINE DGBCO(ABD,LDA,N,ML,MU,IPVT,RCOND,Z)
      INTEGER LDA,N,ML,MU,IPVT(*)
      REAL*8 ABD(LDA,*),Z(*)
      REAL*8 RCOND
      REAL*8 DDOT,EK,T,WK,WKM
      REAL*8 ANORM,S,DASUM,SM,YNORM
      INTEGER IS,INFO,J,JU,K,KB,KP1,L,LA,LM,LZ,M,MM
      ANORM=0.0D0
      L=ML+1
      IS=L+MU
      DO 10 J=1,N
        ANORM=MAX(ANORM,DASUM(L,ABD(IS,J),1))
        IF(IS.GT.ML+1)IS=IS-1
        IF(J.LE.MU)L=L+1
        IF(J.GE.N-ML)L=L-1
10    CONTINUE
      CALL DGBFA(ABD,LDA,N,ML,MU,IPVT,INFO)
      EK=(1.0D0)
      DO 20 J=1,N
        Z(J)=(0.0D0)
20    CONTINUE
      M=ML+MU+1
      JU=0
      DO 100 K=1,N
        IF(ABS(Z(K)).NE.0.0D0)EK=SIGN(EK,-Z(K))
        IF(ABS(EK-Z(K)).LE.ABS(ABD(M,K)))GOTO 30
          S=ABS(ABD(M,K))/ABS(EK-Z(K))
          CALL DSCAL(N,S,Z,1)
          EK=S*EK
30      CONTINUE
        WK=EK-Z(K)
        WKM=-EK-Z(K)
        S=ABS(WK)
        SM=ABS(WKM)
        IF(ABS(ABD(M,K)).EQ.0.0D0) GOTO 40
          WK=WK/(ABD(M,K))
          WKM=WKM/(ABD(M,K))
          GOTO 50
40      CONTINUE
        WK=(1.0D0)
        WKM=(1.0D0)
50      CONTINUE
        KP1=K+1
        JU=MIN(MAX(JU,MU+IPVT(K)),N)
        MM=M
        IF(KP1.GT.JU) GOTO 90
          DO 60 J=KP1,JU
            MM=MM-1
            SM=SM+ABS(Z(J)+WKM*(ABD(MM,J)))
            Z(J)=Z(J)+WK*(ABD(MM,J))
            S=S+ABS(Z(J))
60        CONTINUE
          IF(S.GE.SM) GOTO 80
            T=WKM-WK
            WK=WKM
            MM=M
            DO 70 J=KP1,JU
              MM=MM-1
              Z(J)=Z(J)+T*(ABD(MM,J))
70          CONTINUE
80        CONTINUE
90      CONTINUE
        Z(K)=WK
100   CONTINUE
      S=1.0D0/DASUM(N,Z,1)
      CALL DSCAL(N,S,Z,1)
      DO 120 KB=1,N
        K=N+1-KB
        LM=MIN(ML,N-K)
        IF(K.LT.N)Z(K)=Z(K)+DDOT(LM,ABD(M+1,K),1,Z(K+1),1)
        IF(ABS(Z(K)).LE.1.0D0) GOTO 110
          S=1.0D0/ABS(Z(K))
          CALL DSCAL(N,S,Z,1)
110     CONTINUE
        L=IPVT(K)
        T=Z(L)
        Z(L)=Z(K)
        Z(K)=T
120   CONTINUE
      S=1.0D0/DASUM(N,Z,1)
      CALL DSCAL(N,S,Z,1)
      YNORM=1.0D0
      DO 140 K=1,N
        L=IPVT(K)
        T=Z(L)
        Z(L)=Z(K)
        Z(K)=T
        LM=MIN(ML,N-K)
        IF(K.LT.N)CALL DAXPY(LM,T,ABD(M+1,K),1,Z(K+1),1)
        IF(ABS(Z(K)).LE.1.0D0)GOTO 130
          S=1.0D0/ABS(Z(K))
          CALL DSCAL(N,S,Z,1)
          YNORM=S*YNORM
130     CONTINUE
140   CONTINUE
      S=1.0D0/DASUM(N,Z,1)
      CALL DSCAL(N,S,Z,1)
      YNORM=S*YNORM
      DO 160 KB=1,N
        K=N+1-KB
        IF(ABS(Z(K)).LE.ABS(ABD(M,K)))GOTO 150
          S=ABS(ABD(M,K))/ABS(Z(K))
          CALL DSCAL(N,S,Z,1)
          YNORM=S*YNORM
150     CONTINUE
        IF(ABS(ABD(M,K)).NE.0.0D0)Z(K)=Z(K)/ABD(M,K)
        IF(ABS(ABD(M,K)).EQ.0.0D0)Z(K)=(1.0D0)
        LM=MIN(K,M)-1
        LA=M-LM
        LZ=K-LM
        T=-Z(K)
        CALL DAXPY(LM,T,ABD(LA,K),1,Z(LZ),1)
160   CONTINUE
      S=1.0D0/DASUM(N,Z,1)
      CALL DSCAL(N,S,Z,1)
      YNORM=S*YNORM
      IF(ANORM.NE.0.0D0)RCOND=YNORM/ANORM
      IF(ANORM.EQ.0.0D0)RCOND=0.0D0
      RETURN
      END
      
      SUBROUTINE DGBDI(ABD,LDA,N,ML,MU,IPVT,DET)
      INTEGER LDA,N,ML,MU,IPVT(*)
      REAL*8 ABD(LDA,*),DET(*)
      REAL*8 TEN
      INTEGER I,M
      M=ML+MU+1
      DET(1)=(1.0D0)
      DET(2)=(0.0D0)
      TEN=10.0D0
      DO 50 I=1,N
        IF(IPVT(I).NE.I)DET(1)=-DET(1)
        DET(1)=ABD(M,I)*DET(1)
        IF(ABS(DET(1)).EQ.0.0D0) GOTO 60
10        IF(ABS(DET(1)).GE.1.0D0) GOTO 20
            DET(1)=TEN*DET(1)
            DET(2)=DET(2)-(1.0D0)
            GOTO 10
20        CONTINUE
30        IF(ABS(DET(1)).LT.TEN) GOTO 40
            DET(1)=DET(1)/TEN
            DET(2)=DET(2)+(1.0D0)
            GOTO 30
40        CONTINUE
50    CONTINUE
60    CONTINUE
      RETURN
      END
      
      SUBROUTINE DGBFA(ABD,LDA,N,ML,MU,IPVT,INFO)
      INTEGER LDA,N,ML,MU,IPVT(*),INFO
      REAL*8 ABD(LDA,*)
      REAL*8 T
      INTEGER I,IDAMAX,I0,J,JU,JZ,J0,J1,K,KP1,L,LM,M,MM,NM1
      M=ML+MU+1
      INFO=0
      J0=MU+2
      J1=MIN(N,M)-1
      IF(J1.LT.J0)  GOTO 30
        DO 20 JZ=J0,J1
          I0=M+1-JZ
          DO 10 I=I0,ML
            ABD(I,JZ)=(0.0D0)
10        CONTINUE
20      CONTINUE
30    CONTINUE
      JZ=J1
      JU=0
      NM1=N-1
      IF(NM1.LT.1) GOTO 130
        DO 120 K=1,NM1
          KP1=K+1
          JZ=JZ+1
          IF(JZ.GT.N)GOTO 50
            IF(ML.LT.1) GOTO 50
              DO 40 I=1,ML
                ABD(I,JZ)=(0.0D0)
40            CONTINUE
50        CONTINUE
          LM=MIN(ML,N-K)
          L=IDAMAX(LM+1,ABD(M,K),1)+M-1
          IPVT(K)=L+K-M
          IF(ABS(ABD(L,K)).EQ.0.0D0) GOTO 100
            IF(L.EQ.M) GOTO 60
              T=ABD(L,K)
              ABD(L,K)=ABD(M,K)
              ABD(M,K)=T
60          CONTINUE
            T=-(1.0D0)/ABD(M,K)
            CALL DSCAL(LM,T,ABD(M+1,K),1)
            JU=MIN(MAX(JU,MU+IPVT(K)),N)
            MM=M
            IF(JU.LT.KP1) GOTO 90
              DO 80 J=KP1,JU
                L=L-1
                MM=MM-1
                T=ABD(L,J)
                IF(L.EQ.MM)GOTO 70
                  ABD(L,J)=ABD(MM,J)
                  ABD(MM,J)=T
70              CONTINUE
                CALL DAXPY(LM,T,ABD(M+1,K),1,ABD(MM+1,J),1)
80            CONTINUE
90          CONTINUE
            GOTO 110
100       CONTINUE
          INFO=K
110       CONTINUE
120     CONTINUE
130   CONTINUE
      IPVT(N)=N
      IF(ABS(ABD(M,N)).EQ.0.0D0)INFO=N
      RETURN
      END
      
      SUBROUTINE DGBSL(ABD,LDA,N,ML,MU,IPVT,B,JOB)
      INTEGER LDA,N,ML,MU,IPVT(*),JOB
      REAL*8 ABD(LDA,*),B(*)
      REAL*8 DDOT,T
      INTEGER K,KB,L,LA,LB,LM,M,NM1
      M=MU+ML+1
      NM1=N-1
      IF(JOB.NE.0) GOTO 50
        IF(ML.EQ.0) GOTO 30
        IF(NM1.LT.1)GOTO 30
          DO 20 K=1,NM1
            LM=MIN(ML,N-K)
            L=IPVT(K)
            T=B(L)
            IF(L.EQ.K) GOTO 10
              B(L)=B(K)
              B(K)=T
10          CONTINUE
            CALL DAXPY(LM,T,ABD(M+1,K),1,B(K+1),1)
20        CONTINUE
30      CONTINUE
        DO 40 KB=1,N
          K=N+1-KB
          B(K)=B(K)/ABD(M,K)
          LM=MIN(K,M)-1
          LA=M-LM
          LB=K-LM
          T=-B(K)
          CALL DAXPY(LM,T,ABD(LA,K),1,B(LB),1)
40      CONTINUE
        GOTO 100
50    CONTINUE
      DO 60 K=1,N
        LM=MIN(K,M)-1
        LA=M-LM
        LB=K-LM
        T=DDOT(LM,ABD(LA,K),1,B(LB),1)
        B(K)=(B(K)-T)/(ABD(M,K))
60    CONTINUE
      IF(ML.EQ.0) GOTO 90
      IF(NM1.LT.1) GOTO 90
        DO 80 KB=1,NM1
          K=N-KB
          LM=MIN(ML,N-K)
          B(K)=B(K)+DDOT(LM,ABD(M+1,K),1,B(K+1),1)
          L=IPVT(K)
          IF(L.EQ.K) GOTO 70
            T=B(L)
            B(L)=B(K)
            B(K)=T
70        CONTINUE
80      CONTINUE
90    CONTINUE
100   CONTINUE
      RETURN
      END
      
      SUBROUTINE DGECO(A,LDA,N,IPVT,RCOND,Z)
      INTEGER LDA,N,IPVT(*)
      REAL*8 A(LDA,*),Z(*)
      REAL*8 RCOND
      REAL*8 DDOT,EK,T,WK,WKM
      REAL*8 ANORM,S,DASUM,SM,YNORM
      INTEGER INFO,J,K,KB,KP1,L
      ANORM=0.0D0
      DO 10 J=1,N
        ANORM=MAX(ANORM,DASUM(N,A(1,J),1))
10    CONTINUE
      CALL DGEFA(A,LDA,N,IPVT,INFO)
      EK=(1.0D0)
      DO 20 J=1,N
        Z(J)=(0.0D0)
20    CONTINUE
      DO 100 K=1,N
        IF(ABS(Z(K)).NE.0.0D0)EK=SIGN(EK,-Z(K))
        IF(ABS(EK-Z(K)).LE.ABS(A(K,K)))GOTO 30
          S=ABS(A(K,K))/ABS(EK-Z(K))
          CALL DSCAL(N,S,Z,1)
          EK=S*EK
30      CONTINUE
        WK=EK-Z(K)
        WKM=-EK-Z(K)
        S=ABS(WK)
        SM=ABS(WKM)
        IF(ABS(A(K,K)).EQ.0.0D0)GOTO 40
          WK=WK/(A(K,K))
          WKM=WKM/(A(K,K))
          GOTO 50
40      CONTINUE
        WK=(1.0D0)
        WKM=(1.0D0)
50      CONTINUE
        KP1=K+1
        IF(KP1.GT.N)GOTO 90
          DO 60 J=KP1,N
            SM=SM+ABS(Z(J)+WKM*(A(K,J)))
            Z(J)=Z(J)+WK*(A(K,J))
            S=S+ABS(Z(J))
60        CONTINUE
          IF(S.GE.SM)GOTO 80
            T=WKM-WK
            WK=WKM
            DO 70 J=KP1,N
              Z(J)=Z(J)+T*(A(K,J))
70          CONTINUE
80        CONTINUE
90      CONTINUE
        Z(K)=WK
100   CONTINUE
      S=1.0D0/DASUM(N,Z,1)
      CALL DSCAL(N,S,Z,1)
      DO 120 KB=1,N
        K=N+1-KB
        IF(K.LT.N)Z(K)=Z(K)+DDOT(N-K,A(K+1,K),1,Z(K+1),1)
        IF(ABS(Z(K)).LE.1.0D0)GOTO 110
          S=1.0D0/ABS(Z(K))
          CALL DSCAL(N,S,Z,1)
110     CONTINUE
        L=IPVT(K)
        T=Z(L)
        Z(L)=Z(K)
        Z(K)=T
120   CONTINUE
      S=1.0D0/DASUM(N,Z,1)
      CALL DSCAL(N,S,Z,1)
      YNORM=1.0D0
      DO 140 K=1,N
        L=IPVT(K)
        T=Z(L)
        Z(L)=Z(K)
        Z(K)=T
        IF(K.LT.N)CALL DAXPY(N-K,T,A(K+1,K),1,Z(K+1),1)
        IF(ABS(Z(K)).LE.1.0D0)GOTO 130
          S=1.0D0/ABS(Z(K))
          CALL DSCAL(N,S,Z,1)
          YNORM=S*YNORM
130     CONTINUE
140   CONTINUE
      S=1.0D0/DASUM(N,Z,1)
      CALL DSCAL(N,S,Z,1)
      YNORM=S*YNORM
      DO 160 KB=1,N
        K=N+1-KB
        IF(ABS(Z(K)).LE.ABS(A(K,K)))GOTO 150
          S=ABS(A(K,K))/ABS(Z(K))
          CALL DSCAL(N,S,Z,1)
          YNORM=S*YNORM
150     CONTINUE
        IF(ABS(A(K,K)).NE.0.0D0)Z(K)=Z(K)/A(K,K)
        IF(ABS(A(K,K)).EQ.0.0D0)Z(K)=(1.0D0)
        T=-Z(K)
        CALL DAXPY(K-1,T,A(1,K),1,Z(1),1)
160   CONTINUE
      S=1.0D0/DASUM(N,Z,1)
      CALL DSCAL(N,S,Z,1)
      YNORM=S*YNORM
      IF(ANORM.NE.0.0D0)RCOND=YNORM/ANORM
      IF(ANORM.EQ.0.0D0)RCOND=0.0D0
      RETURN
      END
      
      SUBROUTINE DGEDI(A,LDA,N,IPVT,DET,WORK,JOB)
      INTEGER LDA,N,IPVT(*),JOB
      REAL*8 A(LDA,*),DET(*),WORK(*)
      REAL*8 T
      REAL*8 TEN
      INTEGER I,J,K,KB,KP1,L,NM1
      IF(JOB/10.EQ.0) GOTO 70
        DET(1)=(1.0D0)
        DET(2)=(0.0D0)
        TEN=10.0D0
        DO 50 I=1,N
          IF(IPVT(I).NE.I)DET(1)=-DET(1)
          DET(1)=A(I,I)*DET(1)
          IF(ABS(DET(1)).EQ.0.0D0) GOTO 60
10        IF(ABS(DET(1)).GE.1.0D0) GOTO 20
            DET(1)=TEN*DET(1)
            DET(2)=DET(2)-(1.0D0)
            GOTO 10
20        CONTINUE
30        IF(ABS(DET(1)).LT.TEN) GOTO 40
            DET(1)=DET(1)/TEN
            DET(2)=DET(2)+(1.0D0)
            GOTO 30
40        CONTINUE
50      CONTINUE
60      CONTINUE
70    CONTINUE
      IF(MOD(JOB,10).EQ.0) GOTO 150
        DO 100 K=1,N
          A(K,K)=(1.0D0)/A(K,K)
          T=-A(K,K)
          CALL DSCAL(K-1,T,A(1,K),1)
          KP1=K+1
          IF(N.LT.KP1) GOTO 90
            DO 80 J=KP1,N
              T=A(K,J)
              A(K,J)=(0.0D0)
              CALL DAXPY(K,T,A(1,K),1,A(1,J),1)
80          CONTINUE
90        CONTINUE
100     CONTINUE
        NM1=N-1
        IF(NM1.LT.1) GOTO 140
          DO 130 KB=1,NM1
            K=N-KB
            KP1=K+1
            DO 110 I=KP1,N
              WORK(I)=A(I,K)
              A(I,K)=(0.0D0)
110         CONTINUE
            DO 120 J=KP1,N
              T=WORK(J)
              CALL DAXPY(N,T,A(1,J),1,A(1,K),1)
120         CONTINUE
            L=IPVT(K)
            IF(L.NE.K)CALL DSWAP(N,A(1,K),1,A(1,L),1)
130       CONTINUE
140     CONTINUE
150   CONTINUE
      RETURN
      END
      
      SUBROUTINE DGEFA(A,LDA,N,IPVT,INFO)
      INTEGER LDA,N,IPVT(*),INFO
      REAL*8 A(LDA,*)
      REAL*8 T
      INTEGER IDAMAX,J,K,KP1,L,NM1
      INFO=0
      NM1=N-1
      IF(NM1.LT.1) GOTO 70
        DO 60 K=1,NM1
          KP1=K+1
          L=IDAMAX(N-K+1,A(K,K),1)+K-1
          IPVT(K)=L
          IF(ABS(A(L,K)).EQ.0.0D0) GOTO 40
            IF(L.EQ.K) GOTO 10
              T=A(L,K)
              A(L,K)=A(K,K)
              A(K,K)=T
10          CONTINUE
            T=-(1.0D0)/A(K,K)
            CALL DSCAL(N-K,T,A(K+1,K),1)
            DO 30 J=KP1,N
              T=A(L,J)
              IF(L.EQ.K) GOTO 20
                A(L,J)=A(K,J)
                A(K,J)=T
20            CONTINUE
              CALL DAXPY(N-K,T,A(K+1,K),1,A(K+1,J),1)
30          CONTINUE
            GOTO 50
40        CONTINUE
          INFO=K
50        CONTINUE
60      CONTINUE
70    CONTINUE
      IPVT(N)=N
      IF(ABS(A(N,N)).EQ.0.0D0)INFO=N
      RETURN
      END
      
      SUBROUTINE DGESL(A,LDA,N,IPVT,B,JOB)
      INTEGER LDA,N,IPVT(*),JOB
      REAL*8 A(LDA,*),B(*)
      REAL*8 DDOT,T
      INTEGER K,KB,L,NM1
      NM1=N-1
      IF(JOB.NE.0) GOTO 50
        IF(NM1.LT.1) GOTO 30
          DO 20 K=1,NM1
            L=IPVT(K)
            T=B(L)
            IF(L.EQ.K) GOTO 10
              B(L)=B(K)
              B(K)=T
10          CONTINUE
            CALL DAXPY(N-K,T,A(K+1,K),1,B(K+1),1)
20        CONTINUE
30      CONTINUE
        DO 40 KB=1,N
          K=N+1-KB
          B(K)=B(K)/A(K,K)
          T=-B(K)
          CALL DAXPY(K-1,T,A(1,K),1,B(1),1)
40      CONTINUE
        GOTO 100
50    CONTINUE
      DO 60 K=1,N
        T=DDOT(K-1,A(1,K),1,B(1),1)
        B(K)=(B(K)-T)/(A(K,K))
60    CONTINUE
        IF(NM1.LT.1) GOTO 90
          DO 80 KB=1,NM1
            K=N-KB
            B(K)=B(K)+DDOT(N-K,A(K+1,K),1,B(K+1),1)
            L=IPVT(K)
            IF(L.EQ.K) GOTO 70
              T=B(L)
              B(L)=B(K)
              B(K)=T
70          CONTINUE
80        CONTINUE
90      CONTINUE
100   CONTINUE
      RETURN
      END
      
      SUBROUTINE DGTSL(N,C,D,E,B,INFO)
      INTEGER N,INFO
      REAL*8 C(*),D(*),E(*),B(*)
      INTEGER K,KB,KP1,NM1,NM2
      REAL*8 T
      INFO=0
      C(1)=D(1)
      NM1=N-1
      IF(NM1.LT.1) GOTO 40
        D(1)=E(1)
        E(1)=(0.0D0)
        E(N)=(0.0D0)
        DO 30 K=1,NM1
          KP1=K+1
          IF(ABS(C(KP1)).LT.ABS(C(K))) GOTO 10
            T=C(KP1)
            C(KP1)=C(K)
            C(K)=T
            T=D(KP1)
            D(KP1)=D(K)
            D(K)=T
            T=E(KP1)
            E(KP1)=E(K)
            E(K)=T
            T=B(KP1)
            B(KP1)=B(K)
            B(K)=T
10        CONTINUE
          IF(ABS(C(K)).NE.0.0D0) GOTO 20
            INFO=K
            GOTO 100
20        CONTINUE
          T=-C(KP1)/C(K)
          C(KP1)=D(KP1)+T*D(K)
          D(KP1)=E(KP1)+T*E(K)
          E(KP1)=(0.0D0)
          B(KP1)=B(KP1)+T*B(K)
30      CONTINUE
40      CONTINUE
        IF(ABS(C(N)).NE.0.0D0) GOTO 50
          INFO=N
          GOTO 90
50      CONTINUE
        NM2=N-2
        B(N)=B(N)/C(N)
        IF(N.EQ.1) GOTO 80
          B(NM1)=(B(NM1)-D(NM1)*B(N))/C(NM1)
          IF(NM2.LT.1) GOTO 70
            DO 60 KB=1,NM2
              K=NM2-KB+1
              B(K)=(B(K)-D(K)*B(K+1)-E(K)*B(K+2))/C(K)
60          CONTINUE
70        CONTINUE
80      CONTINUE
90      CONTINUE
100   CONTINUE
      RETURN
      END

      SUBROUTINE DPBCO(ABD,LDA,N,M,RCOND,Z,INFO)
      INTEGER LDA,N,M,INFO
      REAL*8 ABD(LDA,*),Z(*)
      REAL*8 RCOND
      REAL*8 DDOT,EK,T,WK,WKM
      REAL*8 ANORM,S,DASUM,SM,YNORM
      INTEGER I,J,J2,K,KB,KP1,L,LA,LB,LM,MU
      DO 30 J=1,N
        L=MIN(J,M+1)
        MU=MAX(M+2-J,1)
        Z(J)=DASUM(L,ABD(MU,J),1)
        K=J-L
        IF(M.LT.MU) GOTO 20
          DO 10 I=MU,M
            K=K+1
            Z(K)=(Z(K))+ABS(ABD(I,J))
10        CONTINUE
20      CONTINUE
30    CONTINUE
      ANORM=0.0D0
      DO 40 J=1,N
        ANORM=MAX(ANORM,(Z(J)))
40    CONTINUE
      CALL DPBFA(ABD,LDA,N,M,INFO)
      IF(INFO.NE.0) GOTO 180
        EK=(1.0D0)
        DO 50 J=1,N
          Z(J)=(0.0D0)
50      CONTINUE
        DO 110 K=1,N
          IF(ABS(Z(K)).NE.0.0D0)EK=SIGN(EK,-Z(K))
          IF(ABS(EK-Z(K)).LE.(ABD(M+1,K))) GOTO 60
            S=(ABD(M+1,K))/ABS(EK-Z(K))
            CALL DSCAL(N,S,Z,1)
            EK=S*EK
60        CONTINUE
          WK=EK-Z(K)
          WKM=-EK-Z(K)
          S=ABS(WK)
          SM=ABS(WKM)
          WK=WK/ABD(M+1,K)
          WKM=WKM/ABD(M+1,K)
          KP1=K+1
          J2=MIN(K+M,N)
          I=M+1
          IF(KP1.GT.J2) GOTO 100
            DO 70 J=KP1,J2
              I=I-1
              SM=SM+ABS(Z(J)+WKM*(ABD(I,J)))
              Z(J)=Z(J)+WK*(ABD(I,J))
              S=S+ABS(Z(J))
70          CONTINUE
            IF(S.GE.SM) GOTO 90
              T=WKM-WK
              WK=WKM
              I=M+1
              DO 80 J=KP1,J2
                I=I-1
                Z(J)=Z(J)+T*(ABD(I,J))
80            CONTINUE
90          CONTINUE
100       CONTINUE
          Z(K)=WK
110     CONTINUE
        S=1.0D0/DASUM(N,Z,1)
        CALL DSCAL(N,S,Z,1)
        DO 130 KB=1,N
          K=N+1-KB
          IF(ABS(Z(K)).LE.(ABD(M+1,K))) GOTO 120
            S=(ABD(M+1,K))/ABS(Z(K))
            CALL DSCAL(N,S,Z,1)
120       CONTINUE
          Z(K)=Z(K)/ABD(M+1,K)
          LM=MIN(K-1,M)
          LA=M+1-LM
          LB=K-LM
          T=-Z(K)
          CALL DAXPY(LM,T,ABD(LA,K),1,Z(LB),1)
130     CONTINUE
        S=1.0D0/DASUM(N,Z,1)
        CALL DSCAL(N,S,Z,1)
        YNORM=1.0D0
        DO 150 K=1,N
          LM=MIN(K-1,M)
          LA=M+1-LM
          LB=K-LM
          Z(K)=Z(K)-DDOT(LM,ABD(LA,K),1,Z(LB),1)
          IF(ABS(Z(K)).LE.(ABD(M+1,K))) GOTO 140
            S=(ABD(M+1,K))/ABS(Z(K))
            CALL DSCAL(N,S,Z,1)
            YNORM=S*YNORM
140       CONTINUE
          Z(K)=Z(K)/ABD(M+1,K)
150     CONTINUE
        S=1.0D0/DASUM(N,Z,1)
        CALL DSCAL(N,S,Z,1)
        YNORM=S*YNORM
        DO 170 KB=1,N
          K=N+1-KB
          IF(ABS(Z(K)).LE.(ABD(M+1,K))) GOTO 160
            S=(ABD(M+1,K))/ABS(Z(K))
            CALL DSCAL(N,S,Z,1)
            YNORM=S*YNORM
160       CONTINUE
          Z(K)=Z(K)/ABD(M+1,K)
          LM=MIN(K-1,M)
          LA=M+1-LM
          LB=K-LM
          T=-Z(K)
          CALL DAXPY(LM,T,ABD(LA,K),1,Z(LB),1)
170     CONTINUE
        S=1.0D0/DASUM(N,Z,1)
        CALL DSCAL(N,S,Z,1)
        YNORM=S*YNORM
        IF(ANORM.NE.0.0D0)RCOND=YNORM/ANORM
        IF(ANORM.EQ.0.0D0)RCOND=0.0D0
180   CONTINUE
      RETURN
      END
      
      SUBROUTINE DPBDI(ABD,LDA,N,M,DET)
      INTEGER LDA,N,M
      REAL*8 ABD(LDA,*)
      REAL*8 DET(*)
      REAL*8 S
      INTEGER I
      DET(1)=1.0D0
      DET(2)=0.0D0
      S=10.0D0
      DO 50 I=1,N
        DET(1)=(ABD(M+1,I))**2*DET(1)
        IF(DET(1).EQ.0.0D0) GOTO 60
10        IF(DET(1).GE.1.0D0)GOTO 20
            DET(1)=S*DET(1)
            DET(2)=DET(2)-1.0D0
            GOTO 10
20        CONTINUE
30        IF(DET(1).LT.S) GOTO 40
            DET(1)=DET(1)/S
            DET(2)=DET(2)+1.0D0
            GOTO 30
40        CONTINUE
50    CONTINUE
60    CONTINUE
      RETURN
      END
      
      SUBROUTINE DPBFA(ABD,LDA,N,M,INFO)
      INTEGER LDA,N,M,INFO
      REAL*8 ABD(LDA,*)
      REAL*8 DDOT,T
      REAL*8 S
      INTEGER IK,J,JK,K,MU
      DO 30 J=1,N
        INFO=J
        S=0.0D0
        IK=M+1
        JK=MAX(J-M,1)
        MU=MAX(M+2-J,1)
        IF(M.LT.MU) GOTO 20
          DO 10 K=MU,M
            T=ABD(K,J)-DDOT(K-MU,ABD(IK,JK),1,ABD(MU,J),1)
            T=T/ABD(M+1,JK)
            ABD(K,J)=T
            S=S+(T*(T))
            IK=IK-1
            JK=JK+1
10        CONTINUE
20      CONTINUE
        S=(ABD(M+1,J))-S
        ABD(M+1,J)=SQRT(S)
30    CONTINUE
      INFO=0
      RETURN
      END
      
      SUBROUTINE DPBSL(ABD,LDA,N,M,B)
      INTEGER LDA,N,M
      REAL*8 ABD(LDA,*),B(*)
      REAL*8 DDOT,T
      INTEGER K,KB,LA,LB,LM
      DO 10 K=1,N
        LM=MIN(K-1,M)
        LA=M+1-LM
        LB=K-LM
        T=DDOT(LM,ABD(LA,K),1,B(LB),1)
        B(K)=(B(K)-T)/ABD(M+1,K)
10    CONTINUE
      DO 20 KB=1,N
        K=N+1-KB
        LM=MIN(K-1,M)
        LA=M+1-LM
        LB=K-LM
        B(K)=B(K)/ABD(M+1,K)
        T=-B(K)
        CALL DAXPY(LM,T,ABD(LA,K),1,B(LB),1)
20    CONTINUE
      RETURN
      END
      
      SUBROUTINE DPOCO(A,LDA,N,RCOND,Z,INFO)
      INTEGER LDA,N,INFO
      REAL*8 A(LDA,*),Z(*)
      REAL*8 RCOND
      REAL*8 DDOT,EK,T,WK,WKM
      REAL*8 ANORM,S,DASUM,SM,YNORM
      INTEGER I,J,JM1,K,KB,KP1
      DO 30 J=1,N
        Z(J)=DASUM(J,A(1,J),1)
        JM1=J-1
        IF(JM1.LT.1) GOTO 20
          DO 10 I=1,JM1
            Z(I)=(Z(I))+ABS(A(I,J))
10        CONTINUE
20      CONTINUE
30    CONTINUE
      ANORM=0.0D0
      DO 40 J=1,N
        ANORM=MAX(ANORM,(Z(J)))
40    CONTINUE
      CALL DPOFA(A,LDA,N,INFO)
      IF(INFO.NE.0) GOTO 180
        EK=(1.0D0)
      DO 50 J=1,N
        Z(J)=(0.0D0)
50    CONTINUE
      DO 110 K=1,N
        IF(ABS(Z(K)).NE.0.0D0)EK=SIGN(EK,-Z(K))
        IF(ABS(EK-Z(K)).LE.(A(K,K))) GOTO 60
          S=(A(K,K))/ABS(EK-Z(K))
          CALL DSCAL(N,S,Z,1)
          EK=S*EK
60      CONTINUE
        WK=EK-Z(K)
        WKM=-EK-Z(K)
        S=ABS(WK)
        SM=ABS(WKM)
        WK=WK/A(K,K)
        WKM=WKM/A(K,K)
        KP1=K+1
        IF(KP1.GT.N) GOTO 100
          DO 70 J=KP1,N
            SM=SM+ABS(Z(J)+WKM*(A(K,J)))
            Z(J)=Z(J)+WK*(A(K,J))
            S=S+ABS(Z(J))
70        CONTINUE
          IF(S.GE.SM) GOTO 90
            T=WKM-WK
            WK=WKM
            DO 80 J=KP1,N
              Z(J)=Z(J)+T*(A(K,J))
80          CONTINUE
90        CONTINUE
100     CONTINUE
        Z(K)=WK
110   CONTINUE
      S=1.0D0/DASUM(N,Z,1)
      CALL DSCAL(N,S,Z,1)
      DO 130 KB=1,N
        K=N+1-KB
        IF(ABS(Z(K)).LE.(A(K,K))) GOTO 120
          S=(A(K,K))/ABS(Z(K))
          CALL DSCAL(N,S,Z,1)
120     CONTINUE
        Z(K)=Z(K)/A(K,K)
        T=-Z(K)
        CALL DAXPY(K-1,T,A(1,K),1,Z(1),1)
130   CONTINUE
      S=1.0D0/DASUM(N,Z,1)
      CALL DSCAL(N,S,Z,1)
      YNORM=1.0D0
      DO 150 K=1,N
        Z(K)=Z(K)-DDOT(K-1,A(1,K),1,Z(1),1)
        IF(ABS(Z(K)).LE.(A(K,K))) GOTO 140
          S=(A(K,K))/ABS(Z(K))
          CALL DSCAL(N,S,Z,1)
          YNORM=S*YNORM
140     CONTINUE
        Z(K)=Z(K)/A(K,K)
150   CONTINUE
      S=1.0D0/DASUM(N,Z,1)
      CALL DSCAL(N,S,Z,1)
      YNORM=S*YNORM
      DO 170 KB=1,N
        K=N+1-KB
        IF(ABS(Z(K)).LE.(A(K,K))) GOTO 160
          S=(A(K,K))/ABS(Z(K))
          CALL DSCAL(N,S,Z,1)
          YNORM=S*YNORM
160     CONTINUE
        Z(K)=Z(K)/A(K,K)
        T=-Z(K)
        CALL DAXPY(K-1,T,A(1,K),1,Z(1),1)
170   CONTINUE
      S=1.0D0/DASUM(N,Z,1)
      CALL DSCAL(N,S,Z,1)
      YNORM=S*YNORM
      IF(ANORM.NE.0.0D0)RCOND=YNORM/ANORM
      IF(ANORM.EQ.0.0D0)RCOND=0.0D0
180   CONTINUE
      RETURN
      END
      
      SUBROUTINE DPODI(A,LDA,N,DET,JOB)
      INTEGER LDA,N,JOB
      REAL*8 A(LDA,*)
      REAL*8 DET(*)
      REAL*8 T
      REAL*8 S
      INTEGER I,J,JM1,K,KP1
      IF(JOB/10.EQ.0) GOTO 70
        DET(1)=1.0D0
        DET(2)=0.0D0
        S=10.0D0
        DO 50 I=1,N
          DET(1)=(A(I,I))**2*DET(1)
          IF(DET(1).EQ.0.0D0) GOTO 60
10          IF(DET(1).GE.1.0D0) GOTO 20
              DET(1)=S*DET(1)
              DET(2)=DET(2)-1.0D0
              GOTO 10
20          CONTINUE
30          IF(DET(1).LT.S) GOTO 40
              DET(1)=DET(1)/S
              DET(2)=DET(2)+1.0D0
              GOTO30
40          CONTINUE
50      CONTINUE
60      CONTINUE
70    CONTINUE
      IF(MOD(JOB,10).EQ.0) GOTO 140
        DO 100 K=1,N
          A(K,K)=(1.0D0)/A(K,K)
          T=-A(K,K)
          CALL DSCAL(K-1,T,A(1,K),1)
          KP1=K+1
          IF(N.LT.KP1) GOTO 90
            DO 80 J=KP1,N
              T=A(K,J)
              A(K,J)=(0.0D0)
              CALL DAXPY(K,T,A(1,K),1,A(1,J),1)
80          CONTINUE
90        CONTINUE
100    CONTINUE
       DO 130 J=1,N
         JM1=J-1
         IF(JM1.LT.1) GOTO 120
           DO 110 K=1,JM1
             T=(A(K,J))
             CALL DAXPY(K,T,A(1,J),1,A(1,K),1)
110        CONTINUE
120      CONTINUE
         T=(A(J,J))
         CALL DSCAL(J,T,A(1,J),1)
130    CONTINUE
140   CONTINUE
      RETURN
      END
      
      SUBROUTINE DPOFA(A,LDA,N,INFO)
      INTEGER LDA,N,INFO
      REAL*8 A(LDA,*)
      REAL*8 DDOT,T
      REAL*8 S
      INTEGER J,JM1,K
      DO 30 J=1,N
        INFO=J
        S=0.0D0
        JM1=J-1
        IF(JM1.LT.1) GOTO 20
          DO 10 K=1,JM1
            T=A(K,J)-DDOT(K-1,A(1,K),1,A(1,J),1)
            T=T/A(K,K)
            A(K,J)=T
            S=S+(T*(T))
10        CONTINUE
20      CONTINUE
        S=(A(J,J))-S
        A(J,J)=SQRT(S)
30    CONTINUE
      INFO=0
      RETURN
      END
      
      SUBROUTINE DPOSL(A,LDA,N,B)
      INTEGER LDA,N
      REAL*8 A(LDA,*),B(*)
      REAL*8 DDOT,T
      INTEGER K,KB
      DO 10 K=1,N
        T=DDOT(K-1,A(1,K),1,B(1),1)
        B(K)=(B(K)-T)/A(K,K)
10    CONTINUE
      DO 20 KB=1,N
        K=N+1-KB
        B(K)=B(K)/A(K,K)
        T=-B(K)
        CALL DAXPY(K-1,T,A(1,K),1,B(1),1)
20    CONTINUE
      RETURN
      END
      
      SUBROUTINE DPPCO(AP,N,RCOND,Z,INFO)
      INTEGER N,INFO
      REAL*8 AP(*),Z(*)
      REAL*8 RCOND
      REAL*8 DDOT,EK,T,WK,WKM
      REAL*8 ANORM,S,DASUM,SM,YNORM
      INTEGER I,IJ,J,JM1,J1,K,KB,KJ,KK,KP1
      J1=1
      DO 30 J=1,N
        Z(J)=DASUM(J,AP(J1),1)
        IJ=J1
        J1=J1+J
        JM1=J-1
        IF(JM1.LT.1) GOTO 20
          DO 10 I=1,JM1
            Z(I)=(Z(I))+ABS(AP(IJ))
            IJ=IJ+1
10        CONTINUE
20      CONTINUE
30    CONTINUE
      ANORM=0.0D0
      DO 40 J=1,N
        ANORM=MAX(ANORM,(Z(J)))
40    CONTINUE
      CALL DPPFA(AP,N,INFO)
      IF(INFO.NE.0) GOTO 180
        EK=(1.0D0)
        DO 50 J=1,N
          Z(J)=(0.0D0)
50      CONTINUE
        KK=0
        DO 110 K=1,N
          KK=KK+K
          IF(ABS(Z(K)).NE.0.0D0)EK=SIGN(EK,-Z(K))
          IF(ABS(EK-Z(K)).LE.(AP(KK))) GOTO 60
            S=(AP(KK))/ABS(EK-Z(K))
            CALL DSCAL(N,S,Z,1)
            EK=S*EK
60        CONTINUE
          WK=EK-Z(K)
          WKM=-EK-Z(K)
          S=ABS(WK)
          SM=ABS(WKM)
          WK=WK/AP(KK)
          WKM=WKM/AP(KK)
          KP1=K+1
          KJ=KK+K
          IF(KP1.GT.N) GOTO 100
            DO 70 J=KP1,N
              SM=SM+ABS(Z(J)+WKM*(AP(KJ)))
              Z(J)=Z(J)+WK*(AP(KJ))
              S=S+ABS(Z(J))
              KJ=KJ+J
70          CONTINUE
            IF(S.GE.SM) GOTO 90
              T=WKM-WK
              WK=WKM
              KJ=KK+K
              DO 80 J=KP1,N
                Z(J)=Z(J)+T*(AP(KJ))
                KJ=KJ+J
80            CONTINUE
90          CONTINUE
100       CONTINUE
          Z(K)=WK
110     CONTINUE
        S=1.0D0/DASUM(N,Z,1)
        CALL DSCAL(N,S,Z,1)
        DO 130 KB=1,N
          K=N+1-KB
          IF(ABS(Z(K)).LE.(AP(KK))) GOTO 120
            S=(AP(KK))/ABS(Z(K))
            CALL DSCAL(N,S,Z,1)
120       CONTINUE
          Z(K)=Z(K)/AP(KK)
          KK=KK-K
          T=-Z(K)
          CALL DAXPY(K-1,T,AP(KK+1),1,Z(1),1)
130     CONTINUE
        S=1.0D0/DASUM(N,Z,1)
        CALL DSCAL(N,S,Z,1)
        YNORM=1.0D0
        DO 150 K=1,N
          Z(K)=Z(K)-DDOT(K-1,AP(KK+1),1,Z(1),1)
          KK=KK+K
          IF(ABS(Z(K)).LE.(AP(KK))) GOTO 140
            S=(AP(KK))/ABS(Z(K))
            CALL DSCAL(N,S,Z,1)
            YNORM=S*YNORM
140       CONTINUE
          Z(K)=Z(K)/AP(KK)
150     CONTINUE
        S=1.0D0/DASUM(N,Z,1)
        CALL DSCAL(N,S,Z,1)
        YNORM=S*YNORM
        DO 170 KB=1,N
          K=N+1-KB
          IF(ABS(Z(K)).LE.(AP(KK))) GOTO 160
            S=(AP(KK))/ABS(Z(K))
            CALL DSCAL(N,S,Z,1)
            YNORM=S*YNORM
160       CONTINUE
          Z(K)=Z(K)/AP(KK)
          KK=KK-K
          T=-Z(K)
          CALL DAXPY(K-1,T,AP(KK+1),1,Z(1),1)
170     CONTINUE
        S=1.0D0/DASUM(N,Z,1)
        CALL DSCAL(N,S,Z,1)
        YNORM=S*YNORM
        IF(ANORM.NE.0.0D0)RCOND=YNORM/ANORM
        IF(ANORM.EQ.0.0D0)RCOND=0.0D0
180   CONTINUE
      RETURN
      END
      
      SUBROUTINE DPPDI(AP,N,DET,JOB)
      INTEGER N,JOB
      REAL*8 AP(*)
      REAL*8 DET(*)
      REAL*8 T
      REAL*8 S
      INTEGER I,II,J,JJ,JM1,J1,K,KJ,KK,KP1,K1
      IF(JOB/10.EQ.0) GOTO 70
        DET(1)=1.0D0
        DET(2)=0.0D0
        S=10.0D0
        II=0
        DO 50 I=1,N
          II=II+I
          DET(1)=(AP(II))**2*DET(1)
          IF(DET(1).EQ.0.0D0) GOTO 60
10          IF(DET(1).GE.1.0D0) GOTO 20
              DET(1)=S*DET(1)
              DET(2)=DET(2)-1.0D0
              GOTO 10
20          CONTINUE
30          IF(DET(1).LT.S) GOTO 40
              DET(1)=DET(1)/S
              DET(2)=DET(2)+1.0D0
              GOTO 30
40          CONTINUE
50      CONTINUE
60      CONTINUE
70    CONTINUE
      IF(MOD(JOB,10).EQ.0) GOTO 140
        KK=0
        DO 100 K=1,N
          K1=KK+1
          KK=KK+K
          AP(KK)=(1.0D0)/AP(KK)
          T=-AP(KK)
          CALL DSCAL(K-1,T,AP(K1),1)
          KP1=K+1
          J1=KK+1
          KJ=KK+K
          IF(N.LT.KP1) GOTO 90
            DO 80 J=KP1,N
              T=AP(KJ)
              AP(KJ)=(0.0D0)
              CALL DAXPY(K,T,AP(K1),1,AP(J1),1)
              J1=J1+J
              KJ=KJ+J
80          CONTINUE
90        CONTINUE
100     CONTINUE
        JJ=0
        DO 130 J=1,N
          J1=JJ+1
          JJ=JJ+J
          JM1=J-1
          K1=1
          KJ=J1
          IF (JM1.LT.1) GOTO 120
            DO 110 K=1,JM1
              T=(AP(KJ))
              CALL DAXPY(K,T,AP(J1),1,AP(K1),1)
              K1=K1+K
              KJ=KJ+1
110         CONTINUE
120       CONTINUE
          T=(AP(JJ))
          CALL DSCAL(J,T,AP(J1),1)
130     CONTINUE
140   CONTINUE
      RETURN
      END
      
      SUBROUTINE DPPFA(AP,N,INFO)
      INTEGER N,INFO
      REAL*8 AP(*)
      REAL*8 DDOT,T
      REAL*8 S
      INTEGER J,JJ,JM1,K,KJ,KK
      JJ=0
      DO 30 J=1,N
        INFO=J
        S=0.0D0
        JM1=J-1
        KJ=JJ
        KK=0
        IF(JM1.LT.1) GOTO 20
          DO 10 K=1,JM1
            KJ=KJ+1
            T=AP(KJ)-DDOT(K-1,AP(KK+1),1,AP(JJ+1),1)
            KK=KK+K
            T=T/AP(KK)
            AP(KJ)=T
            S=S+(T*(T))
10        CONTINUE
20      CONTINUE
        JJ=JJ+J
        S=(AP(JJ))-S
        AP(JJ)=SQRT(S)
30    CONTINUE
      INFO=0
      RETURN
      END
      
      SUBROUTINE DPPSL(AP,N,B)
      INTEGER N
      REAL*8 AP(*),B(*)
      REAL*8 DDOT,T
      INTEGER K,KB,KK
      KK=0
      DO 10 K=1,N
        T=DDOT(K-1,AP(KK+1),1,B(1),1)
        KK=KK+K
        B(K)=(B(K)-T)/AP(KK)
10    CONTINUE
      DO 20 KB=1,N
        K=N+1-KB
        B(K)=B(K)/AP(KK)
        KK=KK-K
        T=-B(K)
        CALL DAXPY(K-1,T,AP(KK+1),1,B(1),1)
20    CONTINUE
      RETURN
      END
      
      SUBROUTINE DPTSL(N,D,E,B)
      INTEGER N
      REAL*8 D(*),E(*),B(*)
      INTEGER K,KBM1,KE,KF,KP1,NM1,NM1D2
      REAL*8 T1,T2
      IF(N.NE.1) GOTO 10
        B(1)=B(1)/D(1)
        GOTO 70
10    CONTINUE
      NM1=N-1
      NM1D2=NM1/2
      IF(N.EQ.2) GOTO 30
        KBM1=N-1
        DO 20 K=1,NM1D2
          T1=(E(K))/D(K)
          D(K+1)=D(K+1)-T1*E(K)
          B(K+1)=B(K+1)-T1*B(K)
          T2=E(KBM1)/D(KBM1+1)
          D(KBM1)=D(KBM1)-T2*(E(KBM1))
          B(KBM1)=B(KBM1)-T2*B(KBM1+1)
          KBM1=KBM1-1
20      CONTINUE
30    CONTINUE
      KP1=NM1D2+1
      IF(MOD(N,2).NE.0) GOTO 40
        T1=(E(KP1))/D(KP1)
        D(KP1+1)=D(KP1+1)-T1*E(KP1)
        B(KP1+1)=B(KP1+1)-T1*B(KP1)
        KP1=KP1+1
40    CONTINUE
      B(KP1)=B(KP1)/D(KP1)
      IF(N.EQ.2) GOTO 60
        K=KP1-1
        KE=KP1+NM1D2-1
        DO 50 KF=KP1,KE
          B(K)=(B(K)-E(K)*B(K+1))/D(K)
          B(KF+1)=(B(KF+1)-(E(KF))*B(KF))/D(KF+1)
          K=K-1
50      CONTINUE
60    CONTINUE
      IF(MOD(N,2).EQ.0)B(1)=(B(1)-E(1)*B(2))/D(1)
70    CONTINUE
      RETURN
      END
      
      SUBROUTINE DQRDC(X,LDX,N,P,QRAUX,JPVT,WORK,JOB)
      INTEGER LDX,N,P,JOB
      INTEGER JPVT(*)
      REAL*8 X(LDX,*),QRAUX(*),WORK(*)
      INTEGER J,JJ,JP,L,LP1,LUP,MAXJ,PL,PU
      REAL*8 MAXNRM,DNRM2,TT
      REAL*8 DDOT,NRMXL,T
      LOGICAL NEGJ,SWAPJ
      PL=1
      PU=0
      IF(JOB.EQ.0) GOTO 60
        DO 20 J=1,P
          SWAPJ=JPVT(J).GT.0
          NEGJ=JPVT(J).LT.0
          JPVT(J)=J
          IF(NEGJ)JPVT(J)=-J
          IF(.NOT.SWAPJ) GOTO 10
            IF(J.NE.PL)CALL DSWAP(N,X(1,PL),1,X(1,J),1)
            JPVT(J)=JPVT(PL)
            JPVT(PL)=J
            PL=PL+1
10        CONTINUE
20      CONTINUE
        PU=P
        DO 50 JJ=1,P
          J=P-JJ+1
          IF(JPVT(J).GE.0) GOTO 40
            JPVT(J)=-JPVT(J)
            IF(J.EQ.PU) GOTO 30
              CALL DSWAP(N,X(1,PU),1,X(1,J),1)
              JP=JPVT(PU)
              JPVT(PU)=JPVT(J)
              JPVT(J)=JP
30          CONTINUE
            PU=PU-1
40        CONTINUE
50      CONTINUE
60    CONTINUE
      IF(PU.LT.PL) GOTO 80
        DO 70 J=PL,PU
          QRAUX(J)=DNRM2(N,X(1,J),1)
          WORK(J)=QRAUX(J)
70      CONTINUE
80    CONTINUE
      LUP=MIN(N,P)
      DO 200 L=1,LUP
        IF(L.LT.PL.OR.L.GE.PU) GOTO 120
          MAXNRM=0.0D0
          MAXJ=L
          DO 100 J=L,PU
            IF((QRAUX(J)).LE.MAXNRM) GOTO 90
              MAXNRM=(QRAUX(J))
              MAXJ=J
90          CONTINUE
100       CONTINUE
          IF(MAXJ.EQ.L) GOTO 110
            CALL DSWAP(N,X(1,L),1,X(1,MAXJ),1)
            QRAUX(MAXJ)=QRAUX(L)
            WORK(MAXJ)=WORK(L)
            JP=JPVT(MAXJ)
            JPVT(MAXJ)=JPVT(L)
            JPVT(L)=JP
110       CONTINUE
120     CONTINUE
        QRAUX(L)=(0.0D0)
        IF(L.EQ.N) GOTO 190
          NRMXL=DNRM2(N-L+1,X(L,L),1)
          IF(ABS(NRMXL).EQ.0.0D0) GOTO 180
            IF(ABS(X(L,L)).NE.0.0D0)NRMXL=SIGN(NRMXL,X(L,L))
            CALL DSCAL(N-L+1,(1.0D0)/NRMXL,X(L,L),1)
            X(L,L)=(1.0D0)+X(L,L)
            LP1=L+1
            IF(P.LT.LP1) GOTO 170
              DO 160 J=LP1,P
                T=-DDOT(N-L+1,X(L,L),1,X(L,J),1)/X(L,L)
                CALL DAXPY(N-L+1,T,X(L,L),1,X(L,J),1)
                IF(J.LT.PL.OR.J.GT.PU) GOTO 150
                IF(ABS(QRAUX(J)).EQ.0.0D0) GOTO 150
                  TT=1.0D0-(ABS(X(L,J))/(QRAUX(J)))**2
                  TT=MAX(TT,0.0D0)
                  T=TT
                  TT=1.0D0+0.05D0*TT*((QRAUX(J))/(WORK(J)))**2
                  IF(TT.EQ.1.0D0) GOTO 130
                    QRAUX(J)=QRAUX(J)*SQRT(T)
                    GOTO 140
130               CONTINUE
                  QRAUX(J)=DNRM2(N-L,X(L+1,J),1)
                  WORK(J)=QRAUX(J)
140               CONTINUE
150             CONTINUE
160           CONTINUE
170         CONTINUE
            QRAUX(L)=X(L,L)
            X(L,L)=-NRMXL
180       CONTINUE
190     CONTINUE
200   CONTINUE
      RETURN
      END
      
      SUBROUTINE DQRSL(X,LDX,N,K,QRAUX,Y,QY,QTY,B,RSD,XB,JOB,INFO)
      INTEGER LDX,N,K,JOB,INFO
      REAL*8 X(LDX,*),QRAUX(*),Y(*),QY(*),QTY(*)
      REAL*8 B(*),RSD(*),XB(*)
      INTEGER I,J,JJ,JU,KP1
      REAL*8 DDOT,T,TEMP
      LOGICAL CB,CQY,CQTY,CR,CXB
      INFO=0
      CQY=JOB/10000.NE.0
      CQTY=MOD(JOB,10000).NE.0
      CB=MOD(JOB,1000)/100.NE.0
      CR=MOD(JOB,100)/10.NE.0
      CXB=MOD(JOB,10).NE.0
      JU=MIN(K,N-1)
      IF(JU.NE.0) GOTO 40
        IF(CQY)QY(1)=Y(1)
        IF(CQTY)QTY(1)=Y(1)
        IF(CXB)XB(1)=Y(1)
        IF(.NOT.CB) GOTO 30
          IF(ABS(X(1,1)).NE.0.0D0) GOTO 10
            INFO=1
            GOTO 20
10        CONTINUE
          B(1)=Y(1)/X(1,1)
20        CONTINUE
30      CONTINUE
        IF(CR)RSD(1)=(0.0D0)
          GOTO 250
40    CONTINUE
      IF(CQY)CALL DCOPY(N,Y,1,QY,1)
      IF(CQTY)CALL DCOPY(N,Y,1,QTY,1)
      IF(.NOT.CQY) GOTO 70
        DO 60 JJ=1,JU
          J=JU-JJ+1
          IF(ABS(QRAUX(J)).EQ.0.0D0) GOTO 50
            TEMP=X(J,J)
            X(J,J)=QRAUX(J)
            T=-DDOT(N-J+1,X(J,J),1,QY(J),1)/X(J,J)
            CALL DAXPY(N-J+1,T,X(J,J),1,QY(J),1)
            X(J,J)=TEMP
50        CONTINUE
60      CONTINUE
70    CONTINUE
      IF(.NOT.CQTY) GOTO 100
        DO 90 J=1,JU
          IF(ABS(QRAUX(J)).EQ.0.0D0) GOTO 80
            TEMP=X(J,J)
            X(J,J)=QRAUX(J)
            T=-DDOT(N-J+1,X(J,J),1,QTY(J),1)/X(J,J)
            CALL DAXPY(N-J+1,T,X(J,J),1,QTY(J),1)
            X(J,J)=TEMP
80        CONTINUE
90      CONTINUE
100   CONTINUE
      IF(CB)CALL DCOPY(K,QTY,1,B,1)
      KP1=K+1
      IF(CXB)CALL DCOPY(K,QTY,1,XB,1)
      IF(CR.AND.K.LT.N)CALL DCOPY(N-K,QTY(KP1),1,RSD(KP1),1)
      IF(.NOT.CXB.OR.KP1.GT.N) GOTO 120
        DO 110 I=KP1,N
          XB(I)=(0.0D0)
110     CONTINUE
120   CONTINUE
      IF(.NOT.CR) GOTO 140
        DO 130 I=1,K
          RSD(I)=(0.0D0)
130     CONTINUE
140   CONTINUE
      IF(.NOT.CB) GOTO 190
        DO 170 JJ=1,K
          J=K-JJ+1
          IF(ABS(X(J,J)).NE.0.0D0) GOTO 150
            INFO=J
            GOTO 180
150       CONTINUE
          B(J)=B(J)/X(J,J)
          IF(J.EQ.1) GOTO 160
            T=-B(J)
            CALL DAXPY(J-1,T,X(1,J),1,B,1)
160       CONTINUE
170     CONTINUE
180     CONTINUE
190   CONTINUE
      IF(.NOT.CR.AND..NOT.CXB) GOTO 240
        DO 230 JJ=1,JU
          J=JU-JJ+1
          IF(ABS(QRAUX(J)).EQ.0.0D0) GOTO 220
            TEMP=X(J,J)
            X(J,J)=QRAUX(J)
            IF(.NOT.CR) GOTO 200
              T=-DDOT(N-J+1,X(J,J),1,RSD(J),1)/X(J,J)
              CALL DAXPY(N-J+1,T,X(J,J),1,RSD(J),1)
200         CONTINUE
            IF(.NOT.CXB) GOTO 210
              T=-DDOT(N-J+1,X(J,J),1,XB(J),1)/X(J,J)
              CALL DAXPY(N-J+1,T,X(J,J),1,XB(J),1)
210         CONTINUE
            X(J,J)=TEMP
220       CONTINUE
230     CONTINUE
240   CONTINUE
250   CONTINUE
      RETURN
      END
      
      SUBROUTINE DSICO(A,LDA,N,KPVT,RCOND,Z)
      INTEGER LDA,N,KPVT(*)
      REAL*8 A(LDA,*),Z(*)
      REAL*8 RCOND
      REAL*8 AK,AKM1,BK,BKM1,DDOT,DENOM,EK,T
      REAL*8 ANORM,S,DASUM,YNORM
      INTEGER I,INFO,J,JM1,K,KP,KPS,KS
      DO 30 J=1,N
        Z(J)=DASUM(J,A(1,J),1)
        JM1=J-1
        IF(JM1.LT.1) GOTO 20
          DO 10 I=1,JM1
            Z(I)=(Z(I))+ABS(A(I,J))
10        CONTINUE
20      CONTINUE
30    CONTINUE
      ANORM=0.0D0
      DO 40 J=1,N
        ANORM=MAX(ANORM,(Z(J)))
40    CONTINUE
      CALL DSIFA(A,LDA,N,KPVT,INFO)
      EK=(1.0D0)
      DO 50 J=1,N
        Z(J)=(0.0D0)
50    CONTINUE
      K=N
60    IF(K.EQ.0) GOTO 120
        KS=1
        IF(KPVT(K).LT.0)KS=2
        KP=IABS(KPVT(K))
        KPS=K+1-KS
        IF(KP.EQ.KPS) GOTO 70
          T=Z(KPS)
          Z(KPS)=Z(KP)
          Z(KP)=T
70      CONTINUE
        IF(ABS(Z(K)).NE.0.0D0)EK=SIGN(EK,Z(K))
        Z(K)=Z(K)+EK
        CALL DAXPY(K-KS,Z(K),A(1,K),1,Z(1),1)
        IF(KS.EQ.1) GOTO 80
          IF(ABS(Z(K-1)).NE.0.0D0)EK=SIGN(EK,Z(K-1))
          Z(K-1)=Z(K-1)+EK
          CALL DAXPY(K-KS,Z(K-1),A(1,K-1),1,Z(1),1)
80      CONTINUE
        IF(KS.EQ.2)GOTO 100
          IF(ABS(Z(K)).LE.ABS(A(K,K))) GOTO 90
            S=ABS(A(K,K))/ABS(Z(K))
            CALL DSCAL(N,S,Z,1)
            EK=S*EK
90        CONTINUE
          IF(ABS(A(K,K)).NE.0.0D0)Z(K)=Z(K)/A(K,K)
          IF(ABS(A(K,K)).EQ.0.0D0)Z(K)=(1.0D0)
          GOTO 110
100     CONTINUE
        AK=A(K,K)/A(K-1,K)
        AKM1=A(K-1,K-1)/A(K-1,K)
        BK=Z(K)/A(K-1,K)
        BKM1=Z(K-1)/A(K-1,K)
        DENOM=AK*AKM1-1.0D0
        Z(K)=(AKM1*BK-BKM1)/DENOM
        Z(K-1)=(AK*BKM1-BK)/DENOM
110     CONTINUE
        K=K-KS
        GOTO 60
120   CONTINUE
      S=1.0D0/DASUM(N,Z,1)
      CALL DSCAL(N,S,Z,1)
      K=1
130   IF(K.GT.N) GOTO 160
        KS=1
        IF(KPVT(K).LT.0)KS=2
        IF(K.EQ.1) GOTO 150
          Z(K)=Z(K)+DDOT(K-1,A(1,K),1,Z(1),1)
          IF(KS.EQ.2)Z(K+1)=Z(K+1)+DDOT(K-1,A(1,K+1),1,Z(1),1)
          KP=IABS(KPVT(K))
          IF(KP.EQ.K) GOTO 140
            T=Z(K)
            Z(K)=Z(KP)
            Z(KP)=T
140       CONTINUE
150     CONTINUE
        K=K+KS
        GOTO 130
160   CONTINUE
      S=1.0D0/DASUM(N,Z,1)
      CALL DSCAL(N,S,Z,1)
      YNORM=1.0D0
      K=N
170   IF(K.EQ.0) GOTO 230
        KS=1
        IF(KPVT(K).LT.0)KS=2
        IF(K.EQ.KS) GOTO 190
          KP=IABS(KPVT(K))
          KPS=K+1-KS
          IF(KP.EQ.KPS) GOTO 180
            T=Z(KPS)
            Z(KPS)=Z(KP)
            Z(KP)=T
180       CONTINUE
          CALL DAXPY(K-KS,Z(K),A(1,K),1,Z(1),1)
          IF(KS.EQ.2)CALL DAXPY(K-KS,Z(K-1),A(1,K-1),1,Z(1),1)
190     CONTINUE
        IF(KS.EQ.2) GOTO 210
          IF(ABS(Z(K)).LE.ABS(A(K,K))) GOTO 200
            S=ABS(A(K,K))/ABS(Z(K))
            CALL DSCAL(N,S,Z,1)
            YNORM=S*YNORM
200       CONTINUE
          IF(ABS(A(K,K)).NE.0.0D0)Z(K)=Z(K)/A(K,K)
          IF(ABS(A(K,K)).EQ.0.0D0)Z(K)=(1.0D0)
          GOTO 220
210     CONTINUE
        AK=A(K,K)/A(K-1,K)
        AKM1=A(K-1,K-1)/A(K-1,K)
        BK=Z(K)/A(K-1,K)
        BKM1=Z(K-1)/A(K-1,K)
        DENOM=AK*AKM1-1.0D0
        Z(K)=(AKM1*BK-BKM1)/DENOM
        Z(K-1)=(AK*BKM1-BK)/DENOM
220     CONTINUE
        K=K-KS
        GOTO 170
230   CONTINUE
      S=1.0D0/DASUM(N,Z,1)
      CALL DSCAL(N,S,Z,1)
      YNORM=S*YNORM
      K=1
240   IF(K.GT.N) GOTO 270
        KS=1
        IF(KPVT(K).LT.0)KS=2
        IF(K.EQ.1) GOTO 260
          Z(K)=Z(K)+DDOT(K-1,A(1,K),1,Z(1),1)
          IF(KS.EQ.2)Z(K+1)=Z(K+1)+DDOT(K-1,A(1,K+1),1,Z(1),1)
          KP=IABS(KPVT(K))
          IF(KP.EQ.K) GOTO 250
            T=Z(K)
            Z(K)=Z(KP)
            Z(KP)=T
250       CONTINUE
260     CONTINUE
        K=K+KS
        GOTO 240
270   CONTINUE
      S=1.0D0/DASUM(N,Z,1)
      CALL DSCAL(N,S,Z,1)
      YNORM=S*YNORM
      IF(ANORM.NE.0.0D0)RCOND=YNORM/ANORM
      IF(ANORM.EQ.0.0D0)RCOND=0.0D0
      RETURN
      END
      
      SUBROUTINE DSIDI(A,LDA,N,KPVT,DET,WORK,JOB)
      INTEGER LDA,N,JOB
      REAL*8 A(LDA,*),DET(*),WORK(*)
      INTEGER KPVT(*)
      REAL*8 AK,AKP1,AKKP1,DDOT,D,T,TEMP
      REAL*8 TEN
      INTEGER J,JB,K,KM1,KS,KSTEP
      LOGICAL NOINV,NODET
      NOINV=MOD(JOB,10).EQ.0
      NODET=MOD(JOB,100)/10.EQ.0
      IF(NODET) GOTO 100
        DET(1)=(1.0D0)
        DET(2)=(0.0D0)
        TEN=10.0D0
        T=(0.0D0)
        DO 90 K=1,N
          D=A(K,K)
          IF(KPVT(K).GT.0) GOTO 30
            IF(ABS(T).NE.0.0D0) GOTO 10
              T=A(K,K+1)
              D=(D/T)*A(K+1,K+1)-T
              GOTO 20
10          CONTINUE
            D=T
            T=(0.0D0)
20          CONTINUE
30        CONTINUE
          DET(1)=D*DET(1)
          IF(ABS(DET(1)).EQ.0.0D0) GOTO 80
40          IF(ABS(DET(1)).GE.1.0D0) GOTO 50
              DET(1)=TEN*DET(1)
              DET(2)=DET(2)-(1.0D0)
              GOTO 40
50          CONTINUE
60          IF(ABS(DET(1)).LT.TEN) GOTO 70
              DET(1)=DET(1)/TEN
              DET(2)=DET(2)+(1.0D0)
              GOTO 60
70          CONTINUE
80        CONTINUE
90      CONTINUE
100   CONTINUE
      IF(NOINV) GOTO 230
        K=1
110     IF(K.GT.N) GOTO 220
          KM1=K-1
          IF(KPVT(K).LT.0) GOTO 140
            A(K,K)=(1.0D0)/A(K,K)
            IF(KM1.LT.1) GOTO 130
              CALL DCOPY(KM1,A(1,K),1,WORK,1)
              DO 120 J=1,KM1
                A(J,K)=DDOT(J,A(1,J),1,WORK,1)
                CALL DAXPY(J-1,WORK(J),A(1,J),1,A(1,K),1)
120           CONTINUE
              A(K,K)=A(K,K)+DDOT(KM1,WORK,1,A(1,K),1)
130         CONTINUE
            KSTEP=1
            GOTO 180
140       CONTINUE
          T=A(K,K+1)
          AK=A(K,K)/T
          AKP1=A(K+1,K+1)/T
          AKKP1=A(K,K+1)/T
          D=T*(AK*AKP1-(1.0D0))
          A(K,K)=AKP1/D
          A(K+1,K+1)=AK/D
          A(K,K+1)=-AKKP1/D
          IF(KM1.LT.1) GOTO 170
            CALL DCOPY(KM1,A(1,K+1),1,WORK,1)
            DO 150 J=1,KM1
              A(J,K+1)=DDOT(J,A(1,J),1,WORK,1)
              CALL DAXPY(J-1,WORK(J),A(1,J),1,A(1,K+1),1)
150         CONTINUE
            A(K+1,K+1)=A(K+1,K+1)+DDOT(KM1,WORK,1,A(1,K+1),1)
            A(K,K+1)=A(K,K+1)+DDOT(KM1,A(1,K),1,A(1,K+1),1)
            CALL DCOPY(KM1,A(1,K),1,WORK,1)
            DO 160 J=1,KM1
              A(J,K)=DDOT(J,A(1,J),1,WORK,1)
              CALL DAXPY(J-1,WORK(J),A(1,J),1,A(1,K),1)
160         CONTINUE
            A(K,K)=A(K,K)+DDOT(KM1,WORK,1,A(1,K),1)
170       CONTINUE
          KSTEP=2
180       CONTINUE
          KS=IABS(KPVT(K))
          IF(KS.EQ.K) GOTO 210
            CALL DSWAP(KS,A(1,KS),1,A(1,K),1)
            DO 190 JB=KS,K
              J=K+KS-JB
              TEMP=A(J,K)
              A(J,K)=A(KS,J)
              A(KS,J)=TEMP
190         CONTINUE
            IF(KSTEP.EQ.1) GOTO 200
              TEMP=A(KS,K+1)
              A(KS,K+1)=A(K,K+1)
              A(K,K+1)=TEMP
200         CONTINUE
210       CONTINUE
          K=K+KSTEP
          GOTO 110
220     CONTINUE
230   CONTINUE
      RETURN
      END
      
      SUBROUTINE DSIFA(A,LDA,N,KPVT,INFO)
      INTEGER LDA,N,KPVT(*),INFO
      REAL*8 A(LDA,*)
      REAL*8 AK,AKM1,BK,BKM1,DENOM,MULK,MULKM1,T
      REAL*8 ABSAKK,ALPHA,COLMAX,ROWMAX
      INTEGER IMAX,IMAXP1,J,JJ,JMAX,K,KM1,KM2,KSTEP,IDAMAX
      LOGICAL SWAP
      ALPHA=(1.0D0+SQRT(17.0D0))/8.0D0
      INFO=0
      K=N
10    CONTINUE
      IF(K.EQ.0) GOTO 200
        IF(K.GT.1) GOTO 20
          KPVT(1)=1
          IF(ABS(A(1,1)).EQ.0.0D0)INFO=1
          GOTO 200
20      CONTINUE
        KM1=K-1
        ABSAKK=ABS(A(K,K))
        IMAX=IDAMAX(K-1,A(1,K),1)
        COLMAX=ABS(A(IMAX,K))
        IF(ABSAKK.LT.ALPHA*COLMAX)GOTO30
        KSTEP=1
        SWAP=.FALSE.
        GOTO 90
30      CONTINUE
        ROWMAX=0.0D0
        IMAXP1=IMAX+1
        DO 40 J=IMAXP1,K
          ROWMAX=MAX(ROWMAX,ABS(A(IMAX,J)))
40      CONTINUE
        IF(IMAX.EQ.1) GOTO 50
          JMAX=IDAMAX(IMAX-1,A(1,IMAX),1)
          ROWMAX=MAX(ROWMAX,ABS(A(JMAX,IMAX)))
50      CONTINUE
        IF(ABS(A(IMAX,IMAX)).LT.ALPHA*ROWMAX) GOTO 60
          KSTEP=1
          SWAP=.TRUE.
          GOTO 80
60      CONTINUE
        IF(ABSAKK.LT.ALPHA*COLMAX*(COLMAX/ROWMAX)) GOTO 70
          KSTEP=1
          SWAP=.FALSE.
          GOTO 80
70      CONTINUE
        KSTEP=2
        SWAP=IMAX.NE.KM1
80    CONTINUE
90    CONTINUE
      IF(MAX(ABSAKK,COLMAX).NE.0.0D0) GOTO 100
        KPVT(K)=K
        INFO=K
        GOTO 190
100   CONTINUE
      IF(KSTEP.EQ.2) GOTO 140
        IF(.NOT.SWAP) GOTO 120
          CALL DSWAP(IMAX,A(1,IMAX),1,A(1,K),1)
          DO 110 JJ=IMAX,K
            J=K+IMAX-JJ
            T=A(J,K)
            A(J,K)=A(IMAX,J)
            A(IMAX,J)=T
110       CONTINUE
120     CONTINUE
        DO 130 JJ=1,KM1
          J=K-JJ
          MULK=-A(J,K)/A(K,K)
          T=MULK
          CALL DAXPY(J,T,A(1,K),1,A(1,J),1)
          A(J,K)=MULK
130     CONTINUE
        KPVT(K)=K
        IF(SWAP)KPVT(K)=IMAX
        GOTO 190
140   CONTINUE
      IF(.NOT.SWAP) GOTO 160
        CALL DSWAP(IMAX,A(1,IMAX),1,A(1,K-1),1)
        DO 150 JJ=IMAX,KM1
          J=KM1+IMAX-JJ
          T=A(J,K-1)
          A(J,K-1)=A(IMAX,J)
          A(IMAX,J)=T
150     CONTINUE
        T=A(K-1,K)
        A(K-1,K)=A(IMAX,K)
        A(IMAX,K)=T
160   CONTINUE
      KM2=K-2
      IF(KM2.EQ.0) GOTO 180
        AK=A(K,K)/A(K-1,K)
        AKM1=A(K-1,K-1)/A(K-1,K)
        DENOM=1.0D0-AK*AKM1
        DO 170 JJ=1,KM2
          J=KM1-JJ
          BK=A(J,K)/A(K-1,K)
          BKM1=A(J,K-1)/A(K-1,K)
          MULK=(AKM1*BK-BKM1)/DENOM
          MULKM1=(AK*BKM1-BK)/DENOM
          T=MULK
          CALL DAXPY(J,T,A(1,K),1,A(1,J),1)
          T=MULKM1
          CALL DAXPY(J,T,A(1,K-1),1,A(1,J),1)
          A(J,K)=MULK
          A(J,K-1)=MULKM1
170     CONTINUE
180   CONTINUE
      KPVT(K)=1-K
      IF(SWAP)KPVT(K)=-IMAX
      KPVT(K-1)=KPVT(K)
190   CONTINUE
      K=K-KSTEP
      GOTO 10
200   CONTINUE
      RETURN
      END
      
      SUBROUTINE DSISL(A,LDA,N,KPVT,B)
      INTEGER LDA,N,KPVT(*)
      REAL*8 A(LDA,*),B(*)
      REAL*8 AK,AKM1,BK,BKM1,DDOT,DENOM,TEMP
      INTEGER K,KP
      K=N
10    IF(K.EQ.0) GOTO 80
        IF(KPVT(K).LT.0) GOTO 40
          IF(K.EQ.1) GOTO 30
            KP=KPVT(K)
            IF(KP.EQ.K) GOTO 20
              TEMP=B(K)
              B(K)=B(KP)
              B(KP)=TEMP
20          CONTINUE
            CALL DAXPY(K-1,B(K),A(1,K),1,B(1),1)
30        CONTINUE
          B(K)=B(K)/A(K,K)
          K=K-1
          GOTO 70
40      CONTINUE
        IF(K.EQ.2) GOTO 60
          KP=IABS(KPVT(K))
          IF(KP.EQ.K-1) GOTO 50
            TEMP=B(K-1)
            B(K-1)=B(KP)
            B(KP)=TEMP
50        CONTINUE
          CALL DAXPY(K-2,B(K),A(1,K),1,B(1),1)
          CALL DAXPY(K-2,B(K-1),A(1,K-1),1,B(1),1)
60      CONTINUE
        AK=A(K,K)/A(K-1,K)
        AKM1=A(K-1,K-1)/A(K-1,K)
        BK=B(K)/A(K-1,K)
        BKM1=B(K-1)/A(K-1,K)
        DENOM=AK*AKM1-1.0D0
        B(K)=(AKM1*BK-BKM1)/DENOM
        B(K-1)=(AK*BKM1-BK)/DENOM
        K=K-2
70      CONTINUE
        GOTO 10
80    CONTINUE
      K=1
90    IF(K.GT.N) GOTO 160
        IF(KPVT(K).LT.0) GOTO 120
          IF(K.EQ.1) GOTO 110
            B(K)=B(K)+DDOT(K-1,A(1,K),1,B(1),1)
            KP=KPVT(K)
            IF(KP.EQ.K) GOTO 100
              TEMP=B(K)
              B(K)=B(KP)
              B(KP)=TEMP
100         CONTINUE
110       CONTINUE
          K=K+1
          GOTO 150
120     CONTINUE
        IF(K.EQ.1) GOTO 140
          B(K)=B(K)+DDOT(K-1,A(1,K),1,B(1),1)
          B(K+1)=B(K+1)+DDOT(K-1,A(1,K+1),1,B(1),1)
          KP=IABS(KPVT(K))
          IF(KP.EQ.K) GOTO 130
            TEMP=B(K)
            B(K)=B(KP)
            B(KP)=TEMP
130       CONTINUE
140     CONTINUE
        K=K+2
150     CONTINUE
        GOTO 90
160   CONTINUE
      RETURN
      END

      SUBROUTINE DSPCO(AP,N,KPVT,RCOND,Z)
      INTEGER N,KPVT(*)
      REAL*8 AP(*),Z(*)
      REAL*8 RCOND
      REAL*8 AK,AKM1,BK,BKM1,DDOT,DENOM,EK,T
      REAL*8 ANORM,S,DASUM,YNORM
      INTEGER I,IJ,IK,IKM1,IKP1,INFO,J,JM1,J1
      INTEGER K,KK,KM1K,KM1KM1,KP,KPS,KS
      J1=1
      DO 30 J=1,N
        Z(J)=DASUM(J,AP(J1),1)
        IJ=J1
        J1=J1+J
        JM1=J-1
        IF(JM1.LT.1) GOTO 20
          DO 10 I=1,JM1
            Z(I)=(Z(I))+ABS(AP(IJ))
            IJ=IJ+1
10        CONTINUE
20      CONTINUE
30    CONTINUE
      ANORM=0.0D0
      DO 40 J=1,N
        ANORM=MAX(ANORM,(Z(J)))
40    CONTINUE
      CALL DSPFA(AP,N,KPVT,INFO)
      EK=(1.0D0)
      DO 50 J=1,N
        Z(J)=(0.0D0)
50    CONTINUE
      K=N
      IK=(N*(N-1))/2
60    IF(K.EQ.0) GOTO 120
        KK=IK+K
        IKM1=IK-(K-1)
        KS=1
        IF(KPVT(K).LT.0)KS=2
        KP=IABS(KPVT(K))
        KPS=K+1-KS
        IF(KP.EQ.KPS) GOTO 70
          T=Z(KPS)
          Z(KPS)=Z(KP)
          Z(KP)=T
70      CONTINUE
        IF(ABS(Z(K)).NE.0.0D0)EK=SIGN(EK,Z(K))
        Z(K)=Z(K)+EK
        CALL DAXPY(K-KS,Z(K),AP(IK+1),1,Z(1),1)
        IF(KS.EQ.1) GOTO 80
          IF(ABS(Z(K-1)).NE.0.0D0)EK=SIGN(EK,Z(K-1))
          Z(K-1)=Z(K-1)+EK
          CALL DAXPY(K-KS,Z(K-1),AP(IKM1+1),1,Z(1),1)
80      CONTINUE
        IF(KS.EQ.2) GOTO 100
          IF(ABS(Z(K)).LE.ABS(AP(KK))) GOTO 90
            S=ABS(AP(KK))/ABS(Z(K))
            CALL DSCAL(N,S,Z,1)
            EK=S*EK
90        CONTINUE
          IF(ABS(AP(KK)).NE.0.0D0)Z(K)=Z(K)/AP(KK)
          IF(ABS(AP(KK)).EQ.0.0D0)Z(K)=(1.0D0)
          GOTO 110
100     CONTINUE
        KM1K=IK+K-1
        KM1KM1=IKM1+K-1
        AK=AP(KK)/AP(KM1K)
        AKM1=AP(KM1KM1)/AP(KM1K)
        BK=Z(K)/AP(KM1K)
        BKM1=Z(K-1)/AP(KM1K)
        DENOM=AK*AKM1-1.0D0
        Z(K)=(AKM1*BK-BKM1)/DENOM
        Z(K-1)=(AK*BKM1-BK)/DENOM
110     CONTINUE
        K=K-KS
        IK=IK-K
        IF(KS.EQ.2)IK=IK-(K+1)
        GOTO 60
120   CONTINUE
      S=1.0D0/DASUM(N,Z,1)
      CALL DSCAL(N,S,Z,1)
      K=1
      IK=0
130   IF(K.GT.N) GOTO 160
        KS=1
        IF(KPVT(K).LT.0)KS=2
        IF(K.EQ.1) GOTO 150
          Z(K)=Z(K)+DDOT(K-1,AP(IK+1),1,Z(1),1)
          IKP1=IK+K
          IF(KS.EQ.2)Z(K+1)=Z(K+1)+DDOT(K-1,AP(IKP1+1),1,Z(1),1)
          KP=IABS(KPVT(K))
          IF(KP.EQ.K) GOTO 140
            T=Z(K)
            Z(K)=Z(KP)
            Z(KP)=T
140       CONTINUE
150     CONTINUE
        IK=IK+K
        IF(KS.EQ.2)IK=IK+(K+1)
        K=K+KS
        GOTO 130
160   CONTINUE
      S=1.0D0/DASUM(N,Z,1)
      CALL DSCAL(N,S,Z,1)
      YNORM=1.0D0
      K=N
      IK=N*(N-1)/2
170   IF(K.EQ.0) GOTO 230
        KK=IK+K
        IKM1=IK-(K-1)
        KS=1
        IF(KPVT(K).LT.0)KS=2
        IF(K.EQ.KS) GOTO 190
          KP=IABS(KPVT(K))
          KPS=K+1-KS
          IF(KP.EQ.KPS) GOTO 180
            T=Z(KPS)
            Z(KPS)=Z(KP)
            Z(KP)=T
180       CONTINUE
          CALL DAXPY(K-KS,Z(K),AP(IK+1),1,Z(1),1)
          IF(KS.EQ.2)CALL DAXPY(K-KS,Z(K-1),AP(IKM1+1),1,Z(1),1)
190     CONTINUE
        IF(KS.EQ.2) GOTO 210
          IF(ABS(Z(K)).LE.ABS(AP(KK))) GOTO 200
            S=ABS(AP(KK))/ABS(Z(K))
            CALL DSCAL(N,S,Z,1)
            YNORM=S*YNORM
200       CONTINUE
          IF(ABS(AP(KK)).NE.0.0D0)Z(K)=Z(K)/AP(KK)
          IF(ABS(AP(KK)).EQ.0.0D0)Z(K)=(1.0D0)
          GOTO 220
210     CONTINUE
        KM1K=IK+K-1
        KM1KM1=IKM1+K-1
        AK=AP(KK)/AP(KM1K)
        AKM1=AP(KM1KM1)/AP(KM1K)
        BK=Z(K)/AP(KM1K)
        BKM1=Z(K-1)/AP(KM1K)
        DENOM=AK*AKM1-1.0D0
        Z(K)=(AKM1*BK-BKM1)/DENOM
        Z(K-1)=(AK*BKM1-BK)/DENOM
220     CONTINUE
        K=K-KS
        IK=IK-K
        IF(KS.EQ.2)IK=IK-(K+1)
        GOTO 170
230   CONTINUE
      S=1.0D0/DASUM(N,Z,1)
      CALL DSCAL(N,S,Z,1)
      YNORM=S*YNORM
      K=1
      IK=0
240   IF(K.GT.N) GOTO 270
        KS=1
        IF(KPVT(K).LT.0)KS=2
        IF(K.EQ.1) GOTO 260
          Z(K)=Z(K)+DDOT(K-1,AP(IK+1),1,Z(1),1)
          IKP1=IK+K
          IF(KS.EQ.2)Z(K+1)=Z(K+1)+DDOT(K-1,AP(IKP1+1),1,Z(1),1)
          KP=IABS(KPVT(K))
          IF(KP.EQ.K) GOTO 250
            T=Z(K)
            Z(K)=Z(KP)
            Z(KP)=T
250       CONTINUE
260     CONTINUE
        IK=IK+K
        IF(KS.EQ.2)IK=IK+(K+1)
        K=K+KS
        GOTO 240
270   CONTINUE
      S=1.0D0/DASUM(N,Z,1)
      CALL DSCAL(N,S,Z,1)
      YNORM=S*YNORM
      IF(ANORM.NE.0.0D0)RCOND=YNORM/ANORM
      IF(ANORM.EQ.0.0D0)RCOND=0.0D0
      RETURN
      END
      
      SUBROUTINE DSPDI(AP,N,KPVT,DET,WORK,JOB)
      INTEGER N,JOB
      REAL*8 AP(*),WORK(*),DET(*)
      INTEGER KPVT(*)
      REAL*8 AK,AKKP1,AKP1,DDOT,D,T,TEMP
      REAL*8 TEN
      INTEGER IJ,IK,IKP1,IKS,J,JB,JK,JKP1
      INTEGER K,KK,KKP1,KM1,KS,KSJ,KSKP1,KSTEP
      LOGICAL NOINV,NODET
      NOINV=MOD(JOB,10).EQ.0
      NODET=MOD(JOB,100)/10.EQ.0
      IF(NODET) GOTO 110
        DET(1)=(1.0D0)
        DET(2)=(0.0D0)
        TEN=10.0D0
        T=(0.0D0)
        IK=0
        DO 100 K=1,N
          KK=IK+K
          D=AP(KK)
          IF(KPVT(K).GT.0) GOTO 30
            IF(ABS(T).NE.0.0D0) GOTO 10
              IKP1=IK+K
              KKP1=IKP1+K
              T=AP(KKP1)
              D=(D/T)*AP(KKP1+1)-T
              GOTO 20
10          CONTINUE
            D=T
            T=(0.0D0)
20          CONTINUE
30        CONTINUE
          IF(NODET)  GOTO 90
            DET(1)=D*DET(1)
            IF(ABS(DET(1)).EQ.0.0D0) GOTO 80
40            IF(ABS(DET(1)).GE.1.0D0) GOTO 50
                DET(1)=TEN*DET(1)
                DET(2)=DET(2)-(1.0D0)
                GOTO 40
50            CONTINUE
60            IF(ABS(DET(1)).LT.TEN) GOTO 70
                DET(1)=DET(1)/TEN
                DET(2)=DET(2)+(1.0D0)
                GOTO 60
70            CONTINUE
80          CONTINUE
90        CONTINUE
          IK=IK+K
100     CONTINUE
110   CONTINUE
      IF(NOINV) GOTO 240
        K=1
        IK=0
120     IF(K.GT.N) GOTO 230
          KM1=K-1
          KK=IK+K
          IKP1=IK+K
          IF(KPVT(K).LT.0) GOTO 150
            AP(KK)=(1.0D0)/AP(KK)
            IF(KM1.LT.1) GOTO 140
              CALL DCOPY(KM1,AP(IK+1),1,WORK,1)
              IJ=0
              DO 130 J=1,KM1
                JK=IK+J
                AP(JK)=DDOT(J,AP(IJ+1),1,WORK,1)
                CALL DAXPY(J-1,WORK(J),AP(IJ+1),1,AP(IK+1),1)
                IJ=IJ+J
130           CONTINUE
              AP(KK)=AP(KK)+DDOT(KM1,WORK,1,AP(IK+1),1)
140         CONTINUE
            KSTEP=1
            GOTO 190
150       CONTINUE
          KKP1=IKP1+K
          T=AP(KKP1)
          AK=AP(KK)/T
          AKP1=AP(KKP1+1)/T
          AKKP1=AP(KKP1)/T
          D=T*(AK*AKP1-(1.0D0))
          AP(KK)=AKP1/D
          AP(KKP1+1)=AK/D
          AP(KKP1)=-AKKP1/D
          IF(KM1.LT.1) GOTO 180
            CALL DCOPY(KM1,AP(IKP1+1),1,WORK,1)
            IJ=0
            DO 160 J=1,KM1
              JKP1=IKP1+J
              AP(JKP1)=DDOT(J,AP(IJ+1),1,WORK,1)
              CALL DAXPY(J-1,WORK(J),AP(IJ+1),1,AP(IKP1+1),1)
              IJ=IJ+J
160         CONTINUE
            AP(KKP1+1)=AP(KKP1+1)+DDOT(KM1,WORK,1,AP(IKP1+1),1)
            AP(KKP1)=AP(KKP1)+DDOT(KM1,AP(IK+1),1,AP(IKP1+1),1)
            CALL DCOPY(KM1,AP(IK+1),1,WORK,1)
            IJ=0
            DO 170 J=1,KM1
              JK=IK+J
              AP(JK)=DDOT(J,AP(IJ+1),1,WORK,1)
              CALL DAXPY(J-1,WORK(J),AP(IJ+1),1,AP(IK+1),1)
              IJ=IJ+J
170         CONTINUE
            AP(KK)=AP(KK)+DDOT(KM1,WORK,1,AP(IK+1),1)
180       CONTINUE
          KSTEP=2
190       CONTINUE
          KS=IABS(KPVT(K))
          IF(KS.EQ.K) GOTO 220
            IKS=(KS*(KS-1))/2
            CALL DSWAP(KS,AP(IKS+1),1,AP(IK+1),1)
            KSJ=IK+KS
            DO 200 JB=KS,K
              J=K+KS-JB
              JK=IK+J
              TEMP=AP(JK)
              AP(JK)=AP(KSJ)
              AP(KSJ)=TEMP
              KSJ=KSJ-(J-1)
200         CONTINUE
            IF(KSTEP.EQ.1) GOTO 210
              KSKP1=IKP1+KS
              TEMP=AP(KSKP1)
              AP(KSKP1)=AP(KKP1)
              AP(KKP1)=TEMP
210         CONTINUE
220       CONTINUE
          IK=IK+K
          IF(KSTEP.EQ.2)IK=IK+K+1
          K=K+KSTEP
          GOTO 120
230     CONTINUE
240   CONTINUE
      RETURN
      END
      
      SUBROUTINE DSPFA(AP,N,KPVT,INFO)
      INTEGER N,KPVT(*),INFO
      REAL*8 AP(*)
      REAL*8 AK,AKM1,BK,BKM1,DENOM,MULK,MULKM1,T
      REAL*8 ABSAKK,ALPHA,COLMAX,ROWMAX
      INTEGER IDAMAX,IJ,IJJ,IK,IKM1,IM,IMAX,IMAXP1,IMIM,IMJ,IMK
      INTEGER J,JJ,JK,JKM1,JMAX,JMIM,K,KK,KM1,KM1K,KM1KM1,KM2,KSTEP
      LOGICAL SWAP
      ALPHA=(1.0D0+SQRT(17.0D0))/8.0D0
      INFO=0
      K=N
      IK=(N*(N-1))/2
10    CONTINUE
      IF(K.EQ.0) GOTO 200
        IF(K.GT.1) GOTO 20
          KPVT(1)=1
          IF(ABS(AP(1)).EQ.0.0D0)INFO=1
          GOTO 200
20      CONTINUE
        KM1=K-1
        KK=IK+K
        ABSAKK=ABS(AP(KK))
        IMAX=IDAMAX(K-1,AP(IK+1),1)
        IMK=IK+IMAX
        COLMAX=ABS(AP(IMK))
        IF(ABSAKK.LT.ALPHA*COLMAX) GOTO 30
          KSTEP=1
          SWAP=.FALSE.
          GOTO 90
30      CONTINUE
        ROWMAX=0.0D0
        IMAXP1=IMAX+1
        IM=IMAX*(IMAX-1)/2
        IMJ=IM+2*IMAX
        DO 40 J=IMAXP1,K
          ROWMAX=MAX(ROWMAX,ABS(AP(IMJ)))
          IMJ=IMJ+J
40      CONTINUE
        IF(IMAX.EQ.1) GOTO 50
          JMAX=IDAMAX(IMAX-1,AP(IM+1),1)
          JMIM=JMAX+IM
          ROWMAX=MAX(ROWMAX,ABS(AP(JMIM)))
50      CONTINUE
        IMIM=IMAX+IM
        IF(ABS(AP(IMIM)).LT.ALPHA*ROWMAX) GOTO 60
          KSTEP=1
          SWAP=.TRUE.
          GOTO 80
60      CONTINUE
        IF(ABSAKK.LT.ALPHA*COLMAX*(COLMAX/ROWMAX)) GOTO 70
          KSTEP=1
          SWAP=.FALSE.
          GOTO 80
70      CONTINUE
        KSTEP=2
        SWAP=IMAX.NE.KM1
80      CONTINUE
90      CONTINUE
        IF(MAX(ABSAKK,COLMAX).NE.0.0D0) GOTO 100
          KPVT(K)=K
          INFO=K
          GOTO 190
100     CONTINUE
        IF(KSTEP.EQ.2) GOTO 140
          IF(.NOT.SWAP) GOTO 120
            CALL DSWAP(IMAX,AP(IM+1),1,AP(IK+1),1)
            IMJ=IK+IMAX
            DO 110 JJ=IMAX,K
              J=K+IMAX-JJ
              JK=IK+J
              T=AP(JK)
              AP(JK)=AP(IMJ)
              AP(IMJ)=T
              IMJ=IMJ-(J-1)
110         CONTINUE
120       CONTINUE
          IJ=IK-(K-1)
          DO 130 JJ=1,KM1
            J=K-JJ
            JK=IK+J
            MULK=-AP(JK)/AP(KK)
            T=MULK
            CALL DAXPY(J,T,AP(IK+1),1,AP(IJ+1),1)
            IJJ=IJ+J
            AP(JK)=MULK
            IJ=IJ-(J-1)
130       CONTINUE
          KPVT(K)=K
          IF(SWAP)KPVT(K)=IMAX
          GOTO1 90
140     CONTINUE
        KM1K=IK+K-1
        IKM1=IK-(K-1)
        IF(.NOT.SWAP) GOTO 160
          CALL DSWAP(IMAX,AP(IM+1),1,AP(IKM1+1),1)
          IMJ=IKM1+IMAX
          DO 150 JJ=IMAX,KM1
            J=KM1+IMAX-JJ
            JKM1=IKM1+J
            T=AP(JKM1)
            AP(JKM1)=AP(IMJ)
            AP(IMJ)=T
            IMJ=IMJ-(J-1)
150       CONTINUE
          T=AP(KM1K)
          AP(KM1K)=AP(IMK)
          AP(IMK)=T
160     CONTINUE
        KM2=K-2
        IF(KM2.EQ.0) GOTO 180
          AK=AP(KK)/AP(KM1K)
          KM1KM1=IKM1+K-1
          AKM1=AP(KM1KM1)/AP(KM1K)
          DENOM=1.0D0-AK*AKM1
          IJ=IK-(K-1)-(K-2)
          DO 170 JJ=1,KM2
            J=KM1-JJ
            JK=IK+J
            BK=AP(JK)/AP(KM1K)
            JKM1=IKM1+J
            BKM1=AP(JKM1)/AP(KM1K)
            MULK=(AKM1*BK-BKM1)/DENOM
            MULKM1=(AK*BKM1-BK)/DENOM
            T=MULK
            CALL DAXPY(J,T,AP(IK+1),1,AP(IJ+1),1)
            T=MULKM1
            CALL DAXPY(J,T,AP(IKM1+1),1,AP(IJ+1),1)
            AP(JK)=MULK
            AP(JKM1)=MULKM1
            IJJ=IJ+J
            IJ=IJ-(J-1)
170       CONTINUE
180     CONTINUE
        KPVT(K)=1-K
        IF(SWAP)KPVT(K)=-IMAX
        KPVT(K-1)=KPVT(K)
190     CONTINUE
        IK=IK-(K-1)
        IF(KSTEP.EQ.2)IK=IK-(K-2)
        K=K-KSTEP
        GOTO 10
200   CONTINUE
      RETURN
      END
      
      SUBROUTINE DSPSL(AP,N,KPVT,B)
      INTEGER N,KPVT(*)
      REAL*8 AP(*),B(*)
      REAL*8 AK,AKM1,BK,BKM1,DDOT,DENOM,TEMP
      INTEGER IK,IKM1,IKP1,K,KK,KM1K,KM1KM1,KP
      K=N
      IK=(N*(N-1))/2
10    IF(K.EQ.0) GOTO 80
        KK=IK+K
        IF(KPVT(K).LT.0) GOTO 40
          IF(K.EQ.1) GOTO 30
            KP=KPVT(K)
            IF(KP.EQ.K) GOTO 20
              TEMP=B(K)
              B(K)=B(KP)
              B(KP)=TEMP
20          CONTINUE
            CALL DAXPY(K-1,B(K),AP(IK+1),1,B(1),1)
30        CONTINUE
          B(K)=B(K)/AP(KK)
          K=K-1
          IK=IK-K
          GOTO70
40      CONTINUE
        IKM1=IK-(K-1)
        IF(K.EQ.2) GOTO 60
          KP=IABS(KPVT(K))
          IF(KP.EQ.K-1) GOTO 50
            TEMP=B(K-1)
            B(K-1)=B(KP)
            B(KP)=TEMP
50        CONTINUE
          CALL DAXPY(K-2,B(K),AP(IK+1),1,B(1),1)
          CALL DAXPY(K-2,B(K-1),AP(IKM1+1),1,B(1),1)
60      CONTINUE
        KM1K=IK+K-1
        KK=IK+K
        AK=AP(KK)/AP(KM1K)
        KM1KM1=IKM1+K-1
        AKM1=AP(KM1KM1)/AP(KM1K)
        BK=B(K)/AP(KM1K)
        BKM1=B(K-1)/AP(KM1K)
        DENOM=AK*AKM1-1.0D0
        B(K)=(AKM1*BK-BKM1)/DENOM
        B(K-1)=(AK*BKM1-BK)/DENOM
        K=K-2
        IK=IK-(K+1)-K
70      CONTINUE
        GOTO 10
80    CONTINUE
      K=1
      IK=0
90    IF(K.GT.N) GOTO 160
        IF(KPVT(K).LT.0) GOTO 120
          IF(K.EQ.1) GOTO 110
            B(K)=B(K)+DDOT(K-1,AP(IK+1),1,B(1),1)
            KP=KPVT(K)
            IF(KP.EQ.K) GOTO 100
              TEMP=B(K)
              B(K)=B(KP)
              B(KP)=TEMP
100         CONTINUE
110       CONTINUE
          IK=IK+K
          K=K+1
          GOTO 150
120     CONTINUE
        IF(K.EQ.1) GOTO 140
          B(K)=B(K)+DDOT(K-1,AP(IK+1),1,B(1),1)
          IKP1=IK+K
          B(K+1)=B(K+1)+DDOT(K-1,AP(IKP1+1),1,B(1),1)
          KP=IABS(KPVT(K))
          IF(KP.EQ.K) GOTO 130
            TEMP=B(K)
            B(K)=B(KP)
            B(KP)=TEMP
130       CONTINUE
140     CONTINUE
        IK=IK+K+K+1
        K=K+2
150     CONTINUE
        GOTO 90
160   CONTINUE
      RETURN
      END
      
      SUBROUTINE DSVDC(X,LDX,N,P,S,E,U,LDU,V,LDV,WORK,JOB,INFO)
      INTEGER LDX,N,P,LDU,LDV,JOB,INFO
      REAL*8 X(LDX,*),S(*),E(*),U(LDU,*),V(LDV,*),WORK(*)
      INTEGER I,ITER,J,JOBU,K,KASE,KK,L,LL,LLS,LM1,LP1,LS,LU,M,MAXIT
      INTEGER MM,MM1,MP1,NCT,NCTP1,NCU,NRT,NRTP1
      REAL*8 DDOT,T,R
      REAL*8 B,C,CS,EL,EMM1,F,G
      REAL*8 DNRM2,SCALE,SHIFT,SL,SM,SN,SMM1,T1,TEST,ZTEST
      LOGICAL WANTU,WANTV
      MAXIT=30
      WANTU=.FALSE.
      WANTV=.FALSE.
      JOBU=MOD(JOB,100)/10
      NCU=N
      IF(JOBU.GT.1)NCU=MIN(N,P)
      IF(JOBU.NE.0)WANTU=.TRUE.
      IF(MOD(JOB,10).NE.0)WANTV=.TRUE.
      INFO=0
      NCT=MIN(N-1,P)
      NRT=MAX(0,MIN(P-2,N))
      LU=MAX(NCT,NRT)
      IF(LU.LT.1)GOTO170
      DO 160 L=1,LU
        LP1=L+1
        IF(L.GT.NCT) GOTO 20
          S(L)=DNRM2(N-L+1,X(L,L),1)
          IF(ABS(S(L)).EQ.0.0D0) GOTO 10
            IF(ABS(X(L,L)).NE.0.0D0)S(L)=SIGN(S(L),X(L,L))
            CALL DSCAL(N-L+1,1.0D0/S(L),X(L,L),1)
            X(L,L)=(1.0D0)+X(L,L)
10        CONTINUE
          S(L)=-S(L)
20      CONTINUE
        IF(P.LT.LP1) GOTO 50
          DO 40 J=LP1,P
            IF(L.GT.NCT) GOTO 30
            IF(ABS(S(L)).EQ.0.0D0) GOTO 30
              T=-DDOT(N-L+1,X(L,L),1,X(L,J),1)/X(L,L)
              CALL DAXPY(N-L+1,T,X(L,L),1,X(L,J),1)
30          CONTINUE
            E(J)=(X(L,J))
40        CONTINUE
50      CONTINUE
        IF(.NOT.WANTU.OR.L.GT.NCT) GOTO 70
          DO 60 I=L,N
            U(I,L)=X(I,L)
60        CONTINUE
70      CONTINUE
        IF(L.GT.NRT) GOTO 150
          E(L)=DNRM2(P-L,E(LP1),1)
          IF(ABS(E(L)).EQ.0.0D0) GOTO 80
            IF(ABS(E(LP1)).NE.0.0D0)E(L)=SIGN(E(L),E(LP1))
            CALL DSCAL(P-L,1.0D0/E(L),E(LP1),1)
            E(LP1)=(1.0D0)+E(LP1)
80        CONTINUE
          E(L)=-(E(L))
          IF(LP1.GT.N.OR.ABS(E(L)).EQ.0.0D0) GOTO 120
            DO 90 I=LP1,N
              WORK(I)=(0.0D0)
90          CONTINUE
            DO 100 J=LP1,P
              CALL DAXPY(N-L,E(J),X(LP1,J),1,WORK(LP1),1)
100         CONTINUE
            DO 110 J=LP1,P
              CALL DAXPY(N-L,(-E(J)/E(LP1)),WORK(LP1),1,X(LP1,J),1)
110         CONTINUE
120       CONTINUE
          IF(.NOT.WANTV) GOTO 140
            DO 130 I=LP1,P
              V(I,L)=E(I)
130         CONTINUE
140       CONTINUE
150     CONTINUE
160   CONTINUE
170   CONTINUE
      M=MIN(P,N+1)
      NCTP1=NCT+1
      NRTP1=NRT+1
      IF(NCT.LT.P)S(NCTP1)=X(NCTP1,NCTP1)
      IF(N.LT.M)S(M)=(0.0D0)
      IF(NRTP1.LT.M)E(NRTP1)=X(NRTP1,M)
      E(M)=(0.0D0)
      IF(.NOT.WANTU) GOTO 300
      IF(NCU.LT.NCTP1) GOTO 200
      DO 190 J=NCTP1,NCU
        DO 180 I=1,N
          U(I,J)=(0.0D0)
180     CONTINUE
        U(J,J)=(1.0D0)
190   CONTINUE
200   CONTINUE
      IF(NCT.LT.1) GOTO 290
        DO 280 LL=1,NCT
          L=NCT-LL+1
          IF(ABS(S(L)).EQ.0.0D0) GOTO 250
            LP1=L+1
            IF(NCU.LT.LP1) GOTO 220
              DO 210 J=LP1,NCU
                T=-DDOT(N-L+1,U(L,L),1,U(L,J),1)/U(L,L)
                CALL DAXPY(N-L+1,T,U(L,L),1,U(L,J),1)
210           CONTINUE
220         CONTINUE
            CALL DSCAL(N-L+1,(-1.0D0),U(L,L),1)
            U(L,L)=(1.0D0)+U(L,L)
            LM1=L-1
            IF(LM1.LT.1) GOTO 240
              DO 230 I=1,LM1
                U(I,L)=(0.0D0)
230           CONTINUE
240         CONTINUE
            GOTO 270
250       CONTINUE
          DO 260 I=1,N
            U(I,L)=(0.0D0)
260       CONTINUE
          U(L,L)=(1.0D0)
270       CONTINUE
280     CONTINUE
290   CONTINUE
300   CONTINUE
      IF(.NOT.WANTV) GOTO 350
        DO 340 LL=1,P
          L=P-LL+1
          LP1=L+1
          IF(L.GT.NRT) GOTO 320
          IF(ABS(E(L)).EQ.0.0D0) GOTO 320
            DO 310 J=LP1,P
              T=-DDOT(P-L,V(LP1,L),1,V(LP1,J),1)/V(LP1,L)
              CALL DAXPY(P-L,T,V(LP1,L),1,V(LP1,J),1)
310         CONTINUE
320       CONTINUE
          DO 330 I=1,P
            V(I,L)=(0.0D0)
330       CONTINUE
          V(L,L)=(1.0D0)
340     CONTINUE
350   CONTINUE
      DO 380 I=1,M
        IF(ABS(S(I)).EQ.0.0D0) GOTO 360
          T=ABS(S(I))
          R=S(I)/T
          S(I)=T
          IF(I.LT.M)E(I)=E(I)/R
          IF(WANTU)CALL DSCAL(N,R,U(1,I),1)
360     CONTINUE
        IF(I.EQ.M) GOTO 390
          IF(ABS(E(I)).EQ.0.0D0) GOTO 370
            T=ABS(E(I))
            R=T/E(I)
            E(I)=T
            S(I+1)=S(I+1)*R
            IF(WANTV)CALL DSCAL(P,R,V(1,I+1),1)
370       CONTINUE
380     CONTINUE
390   CONTINUE
      MM=M
      ITER=0
400   CONTINUE
      IF(M.EQ.0) GOTO 660
        IF(ITER.LT.MAXIT) GOTO 410
          INFO=M
          GOTO 660
410     CONTINUE
        DO 430 LL=1,M
          L=M-LL
          IF(L.EQ.0) GOTO 440
          TEST=ABS(S(L))+ABS(S(L+1))
          ZTEST=TEST+ABS(E(L))
          IF(ZTEST.NE.TEST) GOTO 420
            E(L)=(0.0D0)
            GOTO 440
420       CONTINUE
430     CONTINUE
440     CONTINUE
        IF(L.NE.M-1) GOTO 450
          KASE=4
          GOTO 520
450     CONTINUE
        LP1=L+1
        MP1=M+1
        DO 470 LLS=LP1,MP1
          LS=M-LLS+LP1
          IF(LS.EQ.L) GOTO 480
            TEST=0.0D0
            IF(LS.NE.M)TEST=TEST+ABS(E(LS))
            IF(LS.NE.L+1)TEST=TEST+ABS(E(LS-1))
            ZTEST=TEST+ABS(S(LS))
          IF(ZTEST.NE.TEST) GOTO 460
            S(LS)=(0.0D0)
            GOTO 480
460       CONTINUE
470     CONTINUE
480     CONTINUE
        IF(LS.NE.L) GOTO 490
          KASE=3
          GOTO 510
490     CONTINUE
        IF(LS.NE.M) GOTO 500
          KASE=1
          GOTO 510
500     CONTINUE
        KASE=2
        L=LS
510     CONTINUE
520     CONTINUE
        L=L+1
        GOTO(530,560,580,610),KASE
530     CONTINUE
        MM1=M-1
        F=(E(M-1))
        E(M-1)=(0.0D0)
        DO 550 KK=L,MM1
          K=MM1-KK+L
          T1=(S(K))
          CALL DROTG(T1,F,CS,SN)
          S(K)=T1
          IF(K.EQ.L) GOTO 540
            F=-SN*(E(K-1))
            E(K-1)=CS*E(K-1)
540       CONTINUE
          IF(WANTV)CALL DROT(P,V(1,K),1,V(1,M),1,CS,SN)
550     CONTINUE
        GOTO 650
560     CONTINUE
        F=(E(L-1))
        E(L-1)=(0.0D0)
        DO 570 K=L,M
          T1=(S(K))
          CALL DROTG(T1,F,CS,SN)
          S(K)=T1
          F=-SN*(E(K))
          E(K)=CS*E(K)
          IF(WANTU)CALL DROT(N,U(1,K),1,U(1,L-1),1,CS,SN)
570     CONTINUE
        GOTO 650
580     CONTINUE
        SCALE=MAX(ABS(S(M)),ABS(S(M-1)),
     1   ABS(E(M-1)),ABS(S(L)),ABS(E(L)))
        SM=(S(M))/SCALE
        SMM1=(S(M-1))/SCALE
        EMM1=(E(M-1))/SCALE
        SL=(S(L))/SCALE
        EL=(E(L))/SCALE
        B=((SMM1+SM)*(SMM1-SM)+EMM1**2)/2.0D0
        C=(SM*EMM1)**2
        SHIFT=0.0D0
        IF(B.EQ.0.0D0.AND.C.EQ.0.0D0) GOTO 590
          SHIFT=SQRT(B**2+C)
          IF(B.LT.0.0D0)SHIFT=-SHIFT
          SHIFT=C/(B+SHIFT)
590     CONTINUE
        F=(SL+SM)*(SL-SM)-SHIFT
        G=SL*EL
        MM1=M-1
        DO 600 K=L,MM1
          CALL DROTG(F,G,CS,SN)
          IF(K.NE.L)E(K-1)=F
          F=CS*(S(K))+SN*(E(K))
          E(K)=CS*E(K)-SN*S(K)
          G=SN*(S(K+1))
          S(K+1)=CS*S(K+1)
          IF(WANTV)CALL DROT(P,V(1,K),1,V(1,K+1),1,CS,SN)
          CALL DROTG(F,G,CS,SN)
          S(K)=F
          F=CS*(E(K))+SN*(S(K+1))
          S(K+1)=-SN*E(K)+CS*S(K+1)
          G=SN*(E(K+1))
          E(K+1)=CS*E(K+1)
          IF(WANTU.AND.K.LT.N)CALL DROT(N,U(1,K),1,U(1,K+1),1,CS,SN)
600     CONTINUE
        E(M-1)=F
        ITER=ITER+1
        GOTO 650
610   CONTINUE
      IF((S(L)).GE.0.0D0) GOTO 620
        S(L)=-S(L)
        IF(WANTV)CALL DSCAL(P,(-1.0D0),V(1,L),1)
620   CONTINUE
630   IF(L.EQ.MM) GOTO 640
      IF((S(L)).GE.(S(L+1))) GOTO 640
        T=S(L)
        S(L)=S(L+1)
        S(L+1)=T
        IF(WANTV.AND.L.LT.P)CALL DSWAP(P,V(1,L),1,V(1,L+1),1)
        IF(WANTU.AND.L.LT.N)CALL DSWAP(N,U(1,L),1,U(1,L+1),1)
        L=L+1
        GOTO 630
640   CONTINUE
      ITER=0
      M=M-1
650   CONTINUE
      GOTO 400
660   CONTINUE
      RETURN
      END
      
      SUBROUTINE DTRCO(T,LDT,N,RCOND,Z,JOB)
      INTEGER LDT,N,JOB
      REAL*8 T(LDT,*),Z(*)
      REAL*8 RCOND
      REAL*8 W,WK,WKM,EK
      REAL*8 TNORM,YNORM,S,SM,DASUM
      INTEGER I1,J,J1,J2,K,KK,L
      LOGICAL LOWER
      LOWER=JOB.EQ.0
      TNORM=0.0D0
      DO 10 J=1,N
        L=J
        IF(LOWER)L=N+1-J
        I1=1
        IF(LOWER)I1=J
        TNORM=MAX(TNORM,DASUM(L,T(I1,J),1))
10    CONTINUE
      EK=(1.0D0)
      DO 20 J=1,N
        Z(J)=(0.0D0)
20    CONTINUE
      DO 100 KK=1,N
        K=KK
        IF(LOWER)K=N+1-KK
        IF(ABS(Z(K)).NE.0.0D0)EK=SIGN(EK,-Z(K))
        IF(ABS(EK-Z(K)).LE.ABS(T(K,K))) GOTO 30
          S=ABS(T(K,K))/ABS(EK-Z(K))
          CALL DSCAL(N,S,Z,1)
          EK=S*EK
30      CONTINUE
        WK=EK-Z(K)
        WKM=-EK-Z(K)
        S=ABS(WK)
        SM=ABS(WKM)
        IF(ABS(T(K,K)).EQ.0.0D0) GOTO 40
          WK=WK/(T(K,K))
          WKM=WKM/(T(K,K))
          GOTO 50
40      CONTINUE
        WK=(1.0D0)
        WKM=(1.0D0)
50      CONTINUE
        IF(KK.EQ.N) GOTO 90
          J1=K+1
          IF(LOWER)J1=1
          J2=N
          IF(LOWER)J2=K-1
          DO 60 J=J1,J2
            SM=SM+ABS(Z(J)+WKM*(T(K,J)))
            Z(J)=Z(J)+WK*(T(K,J))
            S=S+ABS(Z(J))
60        CONTINUE
          IF(S.GE.SM) GOTO 80
            W=WKM-WK
            WK=WKM
            DO 70 J=J1,J2
              Z(J)=Z(J)+W*(T(K,J))
70          CONTINUE
80        CONTINUE
90      CONTINUE
        Z(K)=WK
100   CONTINUE
      S=1.0D0/DASUM(N,Z,1)
      CALL DSCAL(N,S,Z,1)
      YNORM=1.0D0
      DO 130 KK=1,N
        K=N+1-KK
        IF(LOWER)K=KK
        IF(ABS(Z(K)).LE.ABS(T(K,K))) GOTO 110
          S=ABS(T(K,K))/ABS(Z(K))
          CALL DSCAL(N,S,Z,1)
          YNORM=S*YNORM
110     CONTINUE
        IF(ABS(T(K,K)).NE.0.0D0)Z(K)=Z(K)/T(K,K)
        IF(ABS(T(K,K)).EQ.0.0D0)Z(K)=(1.0D0)
        I1=1
        IF(LOWER)I1=K+1
        IF(KK.GE.N) GOTO 120
          W=-Z(K)
          CALL DAXPY(N-KK,W,T(I1,K),1,Z(I1),1)
120     CONTINUE
130   CONTINUE
      S=1.0D0/DASUM(N,Z,1)
      CALL DSCAL(N,S,Z,1)
      YNORM=S*YNORM
      IF(TNORM.NE.0.0D0)RCOND=YNORM/TNORM
      IF(TNORM.EQ.0.0D0)RCOND=0.0D0
      RETURN
      END
      
      SUBROUTINE DTRDI(T,LDT,N,DET,JOB,INFO)
      INTEGER LDT,N,JOB,INFO
      REAL*8 T(LDT,*),DET(*)
      REAL*8 TEMP
      REAL*8 TEN
      INTEGER I,J,K,KB,KM1,KP1
      IF(JOB/100.EQ.0) GOTO 70
        DET(1)=(1.0D0)
        DET(2)=(0.0D0)
        TEN=10.0D0
        DO 50 I=1,N
          DET(1)=T(I,I)*DET(1)
          IF(ABS(DET(1)).EQ.0.0D0) GOTO 60
10          IF(ABS(DET(1)).GE.1.0D0) GOTO 20
              DET(1)=TEN*DET(1)
              DET(2)=DET(2)-(1.0D0)
              GOTO 10
20          CONTINUE
30          IF(ABS(DET(1)).LT.TEN) GOTO 40
              DET(1)=DET(1)/TEN
              DET(2)=DET(2)+(1.0D0)
              GOTO 30
40          CONTINUE
50      CONTINUE
60      CONTINUE
70    CONTINUE
      IF(MOD(JOB/10,10).EQ.0) GOTO 170
        IF(MOD(JOB,10).EQ.0) GOTO 120
          DO 100 K=1,N
            INFO=K
            IF(ABS(T(K,K)).EQ.0.0D0) GOTO 110
            T(K,K)=(1.0D0)/T(K,K)
            TEMP=-T(K,K)
            CALL DSCAL(K-1,TEMP,T(1,K),1)
            KP1=K+1
            IF(N.LT.KP1) GOTO 90
              DO 80 J=KP1,N
                TEMP=T(K,J)
                T(K,J)=(0.0D0)
                CALL DAXPY(K,TEMP,T(1,K),1,T(1,J),1)
80            CONTINUE
90          CONTINUE
100       CONTINUE
          INFO=0
110       CONTINUE
          GOTO 160
120     CONTINUE
        DO 150 KB=1,N
          K=N+1-KB
          INFO=K
          IF(ABS(T(K,K)).EQ.0.0D0) GOTO 180
          T(K,K)=(1.0D0)/T(K,K)
          TEMP=-T(K,K)
          IF(K.NE.N)CALL DSCAL(N-K,TEMP,T(K+1,K),1)
          KM1=K-1
          IF(KM1.LT.1) GOTO 140
            DO 130 J=1,KM1
              TEMP=T(K,J)
              T(K,J)=(0.0D0)
              CALL DAXPY(N-K+1,TEMP,T(K,K),1,T(K,J),1)
130         CONTINUE
140       CONTINUE
150     CONTINUE
        INFO=0
160     CONTINUE
170   CONTINUE
180   CONTINUE
      RETURN
      END
      
      SUBROUTINE DTRSL(T,LDT,N,B,JOB,INFO)
      INTEGER LDT,N,JOB,INFO
      REAL*8 T(LDT,*),B(*)
      REAL*8 DDOT,TEMP
      INTEGER CASE,J,JJ
      DO 10 INFO=1,N
        IF(ABS(T(INFO,INFO)).EQ.0.0D0) GOTO 150
10    CONTINUE
      INFO=0
      CASE=1
      IF(MOD(JOB,10).NE.0)CASE=2
      IF(MOD(JOB,100)/10.NE.0)CASE=CASE+2
      GOTO(20,50,80,110),CASE
20    CONTINUE
      B(1)=B(1)/T(1,1)
      IF(N.LT.2) GOTO 40
        DO 30 J=2,N
          TEMP=-B(J-1)
          CALL DAXPY(N-J+1,TEMP,T(J,J-1),1,B(J),1)
          B(J)=B(J)/T(J,J)
30      CONTINUE
40    CONTINUE
      GOTO 140
50    CONTINUE
      B(N)=B(N)/T(N,N)
      IF(N.LT.2) GOTO 70
        DO 60 JJ=2,N
          J=N-JJ+1
          TEMP=-B(J+1)
          CALL DAXPY(J,TEMP,T(1,J+1),1,B(1),1)
          B(J)=B(J)/T(J,J)
60      CONTINUE
70    CONTINUE
      GOTO 140
80    CONTINUE
      B(N)=B(N)/(T(N,N))
      IF(N.LT.2) GOTO 100
        DO 90 JJ=2,N
          J=N-JJ+1
          B(J)=B(J)-DDOT(JJ-1,T(J+1,J),1,B(J+1),1)
          B(J)=B(J)/(T(J,J))
90      CONTINUE
100   CONTINUE
      GOTO 140
110   CONTINUE
      B(1)=B(1)/(T(1,1))
      IF(N.LT.2) GOTO 130
        DO 120 J=2,N
          B(J)=B(J)-DDOT(J-1,T(1,J),1,B(1),1)
          B(J)=B(J)/(T(J,J))
120     CONTINUE
130   CONTINUE
140   CONTINUE
150   CONTINUE
      RETURN
      END
