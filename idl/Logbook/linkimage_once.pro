pro LINKIMAGE_ONCE,name,image,type,entry,_extra=e
DEFSYSV,'!LinkImage',exists=exists
entries='/'
if exists then begin
  tmp=execute('entries=!LinkImage')
  if strpos(entries,'/'+entry+'/') ge 0 then return
endif
entries=entries+entry+'/'
defsysv,'!LinkImage',entries
linkimage,name,image,type,entry,_extra=e
return
end
