pro CAMSHOW_STAT,IOSB
;+
;CAMSHOW_STAT displays the status block.
;-
        if (n_elements(IOSB) ne 4) then iosb=!CAM_IOSB	
        print,'iosb status = ',iosb[0],',bytcnt = ',iosb[1],', X=',camx(),', Q=',camq(),format='(A,Z8, A, I,A,I1.1,A,I1.1)'
end
