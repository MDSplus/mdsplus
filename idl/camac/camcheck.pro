pro CAMCHECK,STAT
	if (not STAT) then begin
		message,MdsValue('getmsg($)',stat),/info ,/noname,/traceback
	end
end
