.run mdsip
.run mdstcl
if (strmid(!version.release,0,1) eq '4') then file='mdsip_v4.sav' else file='mdsip.sav'
print,'Creating: ',file
save,/routines,/xdr,file=file
