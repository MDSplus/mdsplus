@mdsconnect.pro
;function dbinfo, dbname, host, name, user, pass, mdshost
;  name = dbname
;  host = 'red'
;  user = 'test'
;  pass = 'pfcworld'
;  mdshost = 'local'
;  return, 1
;end
;
; function to return the information used to connect to
; a particular database from this node.
; note that if the mdshost is the server that has the database
; locally, then the  host can be blank.
;
; alternatively the mdshost can be 'local' and we will use
; a local copy of dblib to access the host over the network.
;
function dbinfo, dbname, host, name, user, pass, mdshost
;  host=getenv("SYBASE_HOST")
;  if (strlen(host) eq 0) then host = "red.psfc.mit.edu"
;  catch, err
;  if (err ne 0) then begin
;    catch,/cancel
;    spawn,'\whoami',result
;    dbuser = result[n_elements(result)-1]
;    dbpass="PFCWORLD"
;    name = dbname
;    mdshost='red'
;    return, 1
;  endif
  OPENR,1,getenv('HOME')+'/'+dbname+".sybase_login"
  mdshost=''
  host = ''
  user=''
  pass=''
  readf,1,mdshost
  readf,1,host
  readf,1,user
  readf,1,pass
  name = dbname
  close, 1
;  name = dbname
;  host = 'red'
;  user = 'test'
;  pass = 'pfcworld'
;  mdshost = 'red.psfc.mit.edu'
  return, 1
end

pro MDSDbDisconnect
   defsysv,'!MDSDB_SOCKET',exists=old_sock
   if (not old_sock) then begin
       defsysv, '!MDSDB_SOCKET', -1
       defsysv, '!MDSDB_HOST', ''
   endif
   MdsDisconnect, socket=!MDSDB_SOCKET
   !MDSDB_SOCKET = -1
   !MDSDB_HOST = ""
end

pro MDSDbConnect, host
   defsysv,'!MDSDB_SOCKET',exists=old_sock
   if (not old_sock) then begin
       defsysv, '!MDSDB_SOCKET', -1
       defsysv, '!MDSDB_HOST', ''
   endif
   if (!MDSDB_HOST ne host) then begin
       MDSDbDisconnect
   endif
   if (host ne "" and host ne "local") then begin
       socket=-1
       MdsConnect, host, socket=socket
       !MDSDB_SOCKET = socket
       !MDSDB_HOST=host
   endif
end

pro set_database, dbname, status=status, quiet=quiet,debug=debug
  status = dbinfo(dbname, host, name, user, pass, mdshost)
  MDSDbconnect, mdshost
  status = mdsvalue("DBLogin($, $, $)", host, user, pass, socket=!MDSDB_SOCKET)
  if (not status) then begin
      if not (keyword_set(quiet)) then begin
          Message, "Error logging on to DbHost "+host, /continue
      endif else begin
          Message, "Error logging on to DbHost "+host, /continue, /noprint
      endelse
      return
  endif
;  status = mdsvalue("SetDatabase('"+ name+"')", socket=!MDSDB_SOCKET)
  status = dsql('USE ?', name)
  if (status ne 0) then begin
      if not (keyword_set(quiet)) then begin
          Message, "Error attaching to database "+name, /continue
      endif else begin
          Message, "Error attaching to database "+name, /continue, /noprint
      endelse
      return
  endif
end

;
; function which replaces on '?' with the string of an idl variable
; it should check that the variable is a scalar
;
; change the tdi variable names to _SqlRetNNN
;
function replace_input, qry, idx, val
  return, strmid(qry, 0, idx-1)+STRING(val)+strmid(qry, idx, strlen(qry)-idx)
end

function QuoteQuotes, qry
  qry = strjoin(strsplit(qry, '"', escape="\", /extract), '\"')
  qry = strjoin(strsplit(qry, "'", escape="\", /extract), "\'")
  return, qry
end

function BreakupStringAnswer, str, count
    sz = size(str)
    if n_elements(sz) eq 3 then $
      str = [str]
	leftover=''
	for i = 0, n_elements(str)-1 do begin
		tmp = strsplit(str(i), string(1b), /extract)
		if leftover ne '' then begin
		  tmp[0] = leftover+tmp[0]
		  leftover = ''
		endif
		if strpos(str(i), string(1b), /reverse_search) ne strlen(str(i))-1 then begin
			leftover = tmp[n_elements(tmp)-1]
            if(n_elements(tmp) gt 1) then $
			  tmp = tmp(0:n_elements(tmp)-2) $
			else begin
			  tmp = leftover
			  leftover = ''
			endelse
		endif
		if (i eq 0) then begin
			answer = tmp
		endif else begin
			answer = [answer, tmp]
		endelse
	endfor
	return, answer
end

function dsql, $
               query, a001, a002, a003, a004, a005, a006, a007, a008, a009, $
               a010, a011, a012, a013, a014, a015, a016, a017, a018, a019, $
               a020, a021, a022, a023, a024, a025, a026, a027, a028, a029, $
               a030, a031, a032, a033, a034, a035, a036, a037, a038, a039, $
               a040, a041, a042, a043, a044, a045, a046, a047, a048, a049, $
               a050, a051, a052, a053, a054, a055, a056, a057, a058, a059, $
               a060, a061, a062, a063, a064, a065, a066, a067, a068, a069, $
               a070, a071, a072, a073, a074, a075, a076, a077, a078, a079, $
               a080, a081, a082, a083, a084, a085, a086, a087, a088, a089, $
               a090, a091, a092, a093, a094, a095, a096, a097, a098, a099, $
               a100, a101, a102, a103, a104, a105, a106, a107, a108, a109, $
               a110, a111, a112, a113, a114, a115, a116, a117, a118, a119, $
               a120, a121, a122, a123, a124, a125, a126, a127, a128, a129, $
               a130, a131, a132, a133, a134, a135, a136, a137, a138, a139, $
               a140, a141, a142, a143, a144, a145, a146, a147, a148, a149, $
               a150, a151, a152, a153, a154, a155, a156, a157, a158, a159, $
               date=date, quiet=quiet, status=status, count=count, $
               error=error, debug=debug, _extra=ex

status = 1
debug = keyword_set(debug)
if (debug) then $
  print, "MDS DSQL query", query
;
; first replace all the non quoted '?' with their coresponding
; input arguments.
; find all the quotes and ?
; and sort out the un quoted ?s
;
idxs = strsplit(query, "[?'""]", /preserve, /reg)
if (n_elements(idxs) gt 1) then begin
    ids = [0]
    looking_for=""
    for  i=1, n_elements(idxs)-1 do begin
        chr = strmid(query, idxs(i)-1, 1)
        if strlen(looking_for) eq  0 then begin
            case (chr) of
                '?' : begin
                        ids = [ids, i]
                end
                '"' :
                "'" : begin
                    looking_for = chr
                end
                default:
            endcase
        endif else begin
            if (chr eq looking_for) then $
              looking_for = ""
        endelse
    endfor

    if n_elements(ids) gt 1 then begin
        idxs = idxs(ids(1:*))
        num_inputs = n_elements(idxs)
    endif else begin
        num_inputs = 0
    endelse
endif else begin
    num_inputs = 0
endelse
;
; execute a command to replace each of the ? markers with
; the string of its argument (should check that it is a scalar)
;
if (debug) then $
  print, "        IDXs: ",idxs
for i = 0, num_inputs-1 do begin
    cmd = 'query = replace_input(query, idxs(i), a'+string(i+1, format='(I3.3)')+')'
    old_len = strlen(query)
    x = execute(cmd)
    idxs = idxs + (strlen(query)-old_len)
endfor

query = QuoteQuotes(query)

if (debug) then $
  print, "        Query: "+query+"***"
;
; Now build an expression to pass to mdsvalue with trailing output tdi
; variables named _Annn where Annn is the output argument name
;
expr = "DSQL('"+query+"'"
arg = 0
for i = num_inputs+1, n_params()-1 do begin
  expr = expr +','+'_a'+string(i, format='(I3.3)')
endfor
if (keyword_set(date)) then $
  expr = expr + ",'/date')" $
else $
  expr = expr + ")"
if (debug) then $
  print, '        Expr: ', expr
; now execute the query
; !!!  if it is a string it is a database error
;
count = mdsvalue(expr, socket=!MDSDB_SOCKET)
sz = size(count)
if (sz(n_elements(sz)-2) eq 7) then begin

; status = MdsValue('_dbstatus')
    error = 0
    status = 0
    if not (keyword_set(quiet)) then begin
        Message, count, /continue
    endif else begin
        Message, count, /noprint, /continue
    endelse
    count = -1
endif else begin
;
;  for each of the output arguments
;    build an expression which sets the argument to the variable of
;    the constructed name
;    (Annn = MdsValue("_Annn")
;  !!! what about error checking!
;
    if (debug) then $
      print, "        Count: ", count
    for i = num_inputs+1, n_params()-1 do begin
        if (debug) then $
          print, 'Working on arg ', i
        arg = 'a'+string(i, format="(I3.3)")
        cmd = arg+' = MDSVALUE("_'+arg+'",socket=!MDSDB_SOCKET)'
        status = execute(cmd)
        if (debug) then $
          print, "got back "+arg
;
; check if it is  a string.  If so parse
; out the delimiters (1b)
;
		cmd = 'sz = size('+arg+')'
		status = execute(cmd)
		if (sz(n_elements(sz)-2) eq 7) then begin
		  cmd = arg+' = BreakupStringAnswer('+arg+',count)'
		  status = execute(cmd)
		endif
    endfor
;
; free all the tdi variables returned by DSQL
;
    if (not debug) then $
      dummy = MdsValue("DeAllocate('_A%%%')",socket=!MDSDB_SOCKET)
;
; return the number of row
;
endelse
return, count
;
end
