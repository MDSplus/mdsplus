pro CAMCHECK,STAT
	if (not STAT) then begin
		message,MdsValue('getmsg($)',stat),/noname,/traceback
	end
end
