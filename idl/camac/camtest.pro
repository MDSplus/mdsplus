function logfile,intime
  parts = str_sep(strcompress(intime)," ")
  return,'camtest_'+strlowcase(parts(1)+parts(2)+parts(4))+'.html'
end

function stime,intime
  parts = str_sep(strcompress(intime)," ")
  case parts(1) of
  'Jan': month=1
  'Feb': month=2
  'Mar': month=3
  'Apr': month=4
  'May': month=5
  'Jun': month=6
  'Jul': month=7
  'Aug': month=8
  'Sep': month=9
  'Oct': month=10
  'Nov': month=11
  'Dec': month=12
  endcase
  return,string(month,format='(I2.2)')+'/'+parts(2)+'/'+strmid(parts(4),2,2)+' '+parts(3)
end

Function HighwayName,bus,unit
  return,'GK'+string(byte('A')+byte(bus))+strtrim(unit,2)
end

Function OnOffGif,bus,unit,crate,crates
  gifs = ['blackball.gif','greenball.gif']
  return,gifs(crates.on(bus,unit,crate) and 1)
end

Function StatusGif,bus,unit,crate,crates
  status = crates.status(bus,unit,crate)
  if status then return,'greenball.gif'
  if crates.on(bus,unit,crate) then begin
    return,'redflashball.gif'
  endif
  if (status eq '801986A'x and unit eq 7) or (status eq '8019842'x and unit ne 7) then return,'blackball.gif'
  return,'redball.gif'
end

pro WriteHtml,crates
  save,crates,file='user10:[cmod-www.camac_status]camtest.sav'
  spawn,'purge user10:[cmod-www.camac_status]camtest.sav'
  openw,2,'user10:[cmod-www.camac_status]camac_status.html'
  printf,2,'<HTML><HEAD><TITLE>Alcator C-Mod CAMAC Status</TITLE>'
  printf,2,'<BODY TEXT="#000000" BGCOLOR="#FFFFFF" LINK="#0000EE" VLINK="#551A8B" ALINK="#FF0000">'
;  printf,2,'<applet codebase=/mdsplus/classes code=Reloader.class id=Reloader width=2 height=2 >'
;  printf,2,' <param name=host value="www.pfc.mit.edu">'
;  printf,2,'<param name=port value=8001> <param name=event value="CAMTEST_UPDATE">'
;  printf,2,'<param name=url value="http://www.pfc.mit.edu/cmod/camac_status.html"></applet>'
  printf,2,'<TABLE BORDER=0 WIDTH=100%><TR>'
  lasthighway=-1
  buses = (size(crates.exists))[1]
  max_crates = (size(crates.exists))[3]
  for unit = 7,0,-1 do begin
    for bus = 0,buses-1 do begin
      for crate = 1,max_crates-1 do begin
        if crates.exists(bus,unit,crate) then begin
          highway = bus * 256L + unit
          if highway ne lasthighway then begin
            printf,2,'<TH>',HighwayName(bus,unit),'</TH>'
            lasthighway = highway
          endif
        endif
      endfor
    endfor
  endfor
  printf,2,'</TR><TR VALIGN="TOP">'
  lasthighway=-1
  for unit = 7,0,-1 do begin
    for bus = 0,buses-1 do begin
      for crate = 1,max_crates-1 do begin
        if crates.exists(bus,unit,crate) then begin
          highway = bus * 256L + unit
          if highway ne lasthighway then begin
            if lasthighway ne -1 then printf,2,'</TABLE></TD>'
            printf,2,'<TD><TABLE CELLSPACING=0 CELLPADDING=0 BORDER WIDTH=100%>'
            lasthighway = highway
          endif
          printf,2,'<TR><TD>',string(crate,format='(I2.2)'),'</TD>'
          printf,2,'<TD><IMG SRC=',OnOffGif(bus,unit,crate,crates),'></TD>'
          printf,2,'<TD><IMG SRC=',StatusGif(bus,unit,crate,crates),'></TD>'
          if crates.changed(bus,unit,crate) then begin
            printf,2,'<TD NOWRAP BGCOLOR="#CCCCCC">'
          endif else begin
            printf,2,'<TD NOWRAP>'
          endelse
          tm = stime(crates.time(bus,unit,crate))
          anchor = strmid(tm,9,8)
          printf,2,'<FONT SIZE=-1>','<A href=/cmod/camac_status/camtest_logs/'+logfile(crates.time(bus,unit,crate))+'#'+anchor+'>'
          printf,2,tm,'></FONT></TD></TR>'
        endif
      endfor
    endfor
  endfor
  printf,2,'</TABLE></TD></TR></TABLE><HR>'
  printf,2,'<TABLE><TR><TD><TABLE BORDER=0 VALIGN=TOP><TR><TD>Column 1</TD><TD>crate number</TD></TR>'
  printf,2,'<TR><TD>Column 2</TD><TD>online/offline (<img src=greenball.gif>=on,<img src=blackball.gif>=off)</TD></TR>'
  printf,2,'<TR><TD>Column 3</TD><TD>controller status (<img src=greenball.gif>=ok,<img src=blackball.gif>=offline,'
  printf,2,'<img src=redball.gif>=error)</TD></TR>'
  printf,2,'<TR><TD>Column 4</TD><TD>time of last status change<BR></TD></TR></TABLE></TD>'
  printf,2,'<TD><A HREF=/cmod/camac_status/camtest_logs/>Viewlogs</A></TD></TR></TABLE><HR>'
  printf,2,'</BODY>'
  close,2
  spawn,'purge user10:[cmod-www.camac_status]camac_status.html'
  return
end

Function CrateName,bus,unit,crate
  return,'GK'+string(byte('A')+byte(bus))+strtrim(unit,2)+string(crate,format='(I2.2)')+':'
end

pro printlog,tm,crate_name,msg,color
  common anchorcom,anchor
  filnam = 'user10:[cmod-www.camac_status.camtest_logs]'+logfile(tm)
  x = findfile(filnam,count=count)
  catch,error_status
  if error_status ne 0 then begin
    close,1
    return
  endif
  if count eq 0 then begin
    openw,1,filnam,/shared
    parts = str_sep(strcompress(tm)," ")
    title = 'CAMAC status log for '+parts(0)+' '+parts(1)+' '+parts(2)+', '+parts(4)
    printf,1,'<html><head><Title>',title,'</Title></head>'
    printf,1,'<BODY TEXT="#000000" BGCOLOR="#FFFFFF" LINK="#0000EE" VLINK="#551A8B" ALINK="#FF0000">'
    printf,1,'<H1>',title,'</H1><BR><HR>'
  endif else openu,1,filnam,/shared,/append
  printf,1,'<FONT COLOR="',color,'">'
  anc = strmid(stime(tm),9,8)
  if anc ne anchor then begin
    printf,1,'<A NAME="',anc,'">',tm,', ',crate_name,' ',msg,'</A><BR>'
    anchor = anc
  endif else begin
    printf,1,tm,',',crate_name,' ',msg,'<BR>'
  endelse
  printf,1,'</FONT>'
  close,1
  return
end

Function CheckOnOff,crate_name,bus,unit,crate,crates
  changed = 0
  iosb = intarr(4)
  data = 0
  online = call_vms('CAMSHR','CAM$PIOW',crate_name+'N1',0,8,DATA,16,IOSB) ne 134322282
  if online then begin
    msg = 'online'
    color = '#00CC00'
  endif else begin
    msg = 'offline'
    color = '#000000'
  endelse
  if crates.on(bus,unit,crate) ne online then begin
    changed = 1
    crates.on(bus,unit,crate) = online
    tm = systime()
    crates.time(bus,unit,crate) = tm
    printlog,tm,crate_name,'went '+msg,color
  endif
  return,changed
end

Function MsgText,status
  text = string(bytarr(132)+32b)
  x=call_external('librtl','lib$sys_getmsg',status,0,text,value=[0,1,0])
  return,strtrim(text,2)
end

Function CheckController,crate_name,bus,unit,crate,crates
  changed = 0
  iosb = intarr(4)
  data = 16
  status = call_vms('CAMSHR','CAM$PIOW',crate_name+'N30',0,23,DATA,16,IOSB)
  color = '#000000'
  if not status then begin
    message = 'unexpected error, '+MsgText(status)
    color = '#FF0000'
  endif else begin
    status = long(iosb(0)) and 'ffff'xl or '8010000'x
    if not status then begin
      if status eq '8019842'x then begin
        message = 'not responding, probably not connected to highway'
        color = '#000000'
      endif else begin
        message = 'unexpected error, '+MsgText(status)
        color = '#FF0000'
      endelse
    endif else begin
      message = 'is now ok'
      color = '#000000'
    endelse
  endelse
  if crates.status(bus,unit,crate) ne status then begin
    crates.status(bus,unit,crate) = status
    tm = systime()
    crates.time(bus,unit,crate) = tm
    printlog,tm,crate_name,message,color
    changed = 1
  endif
  return,changed
end

Function CheckCrate,bus,unit,crate,crates
  changed = 0
  crate_name = CrateName(bus,unit,crate)
  if trnlog(crate_name,value,table='LNM_CAMAC') then begin
    if not crates.exists(bus,unit,crate) then changed = 1
    crates.exists(bus,unit,crate) = 1
    changed = changed or CheckOnOff(crate_name,bus,unit,crate,crates) or CheckController(crate_name,bus,unit,crate,crates)
  endif else begin
    if crates.exists(bus,unit,crate) then changed = 1
    crates.exists(bus,unit,crate) = 0
  endelse
  crates.changed(bus,unit,crate) = byte(changed)
  return,changed
end

pro camtest
  common anchorcom,anchor
  scsi_buses = 10
  max_crates = 41
  anchor=''
  x=findfile('user10:[cmod-www.camac_status]camtest.sav',count=count)
  if count eq 0 then begin
    crates = {crates_struct,exists:bytarr(scsi_buses,8,max_crates),on:bytarr(scsi_buses,8,max_crates) + 255b, $
              status:lonarr(scsi_buses,8,max_crates) * $
                          255b,time:strarr(scsi_buses,8,max_crates),changed:bytarr(scsi_buses,8,max_crates)}
  endif else begin
    restore,'user10:[cmod-www.camac_status]camtest.sav'
  endelse
  while 1 do begin
;    print,systime(),', starting pass'
    changed = 0
    for bus = 0,scsi_buses-1 do begin
      for unit = 0,7 do begin
        for crate = 1,max_crates-1 do begin
          changed = changed or CheckCrate(bus,unit,crate,crates)
        endfor
      endfor
    endfor
    if (changed) then begin
;      print,'settings changed'
      WriteHtml,crates
      wait,5.
      mds$setevent,'CAMTEST_UPDATE'
    Endif
;    print,systime(),', ending pass'
    wait,55.
  endwhile
  return
end
