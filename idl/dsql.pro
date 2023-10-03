;+
; NAME:         DSQL
; PURPOSE:      Do dynamic SQL expression using TDISQL
; CATEGORY:     MDSPLUS
; CALLING SEQUENCE:
;       ans = DSQL(expression [,arg1,...,argn]
;       [,COUNT=count]$         ;number of selected items
;       [,DATE=date]$           ;binary date format
;       [,ERROR=error]$         ; Database error string (if any)
;       [,EXCESS=excess]$       ;allow excessive args (ignored on this pass)
;       [,QUIET=quiet]$         ;suppress printing of error message
;       [,STATUS=status]$       ;VMS error code
;       [,DEBUG=debug])			;Print out debugging information.
;
; INPUT PARAMETERS:       expression = character string with SQL operation
; OPTIONAL INPUT PARAMETERS:
;       arg1,...,argn = values to substitute into the expression for
;       "?" input parameters.
; KEYWORDS:     See above.
; OUTPUTS:      None except for SELECT operations where the remaining
;               arg1,...argn variables receive the output.
;               They may not be expressions or constants.
; COMMON BLOCKS: None.
; SIDE EFFECTS: SQL database opened or accessed.
; RESTRICTIONS: set_database must preceed calls to DSQL.  If local access is used
;               an appropriate database client library (DBLIB) must be installed.
;               If remote access is used, the database server must be running an
;               mdsip server also.
; PROCEDURE:
;  1. '?' markers in the query are replaced with their cooresponding input arguments.
;  2. a TDI expression for the query with tdi variables for all the answers is constructed
;  3. Execute the expression either locally or remotely
;  4. Retrieve the values of each of the output arguments (again either locally or not)
;
; MODIFICATION HISTORY:
;        Idea by T.W. Fredian, September 22, 1992
;        TDI based implementation Josh Stillerman Feb. 2002.
;-

;
;function QuoteQuotes, qry
;  qry = strjoin(strsplit(qry, '"', escape="\", /extract,/preserve_null), '\"')
;  qry = strjoin(strsplit(qry, "'", escape="\", /extract,/preserve_null), "\'")
;  return, qry
;end

function QuoteQuotes, qry
  idxs = strsplit(qry, '\', /preserve_null, length=lens)
  qry= strjoin(strmid(qry, idxs, lens), '\\')
  idxs = strsplit(qry, '"', escape="\", /preserve_null, length=lens)
  qry= strjoin(strmid(qry, idxs, lens), '\"')
  idxs = strsplit(qry, "'", escape="\",/preserve_null, length=lens)
  qry= strjoin(strmid(qry, idxs, lens), "\'")
  return, qry
end

;
; function which replaces on '?' with the string of an idl variable
; it should check that the variable is a scalar
;
; change the tdi variable names to _SqlRetNNN
;
function replace_input, qry, idx, val, debug=debug

  sz = size(val)
  if (sz(n_elements(sz)-2) eq 7) then begin
      ans =  strmid(qry, 0, idx-1)+"'"+STRING(val[0])+"'"+strmid(qry, idx, strlen(qry)-idx)
      if (debug) then $
        print, "replace_input - string ("+ans+")"
  endif else begin
      if (not finite(val, /NAN)) then $
        ans = strmid(qry, 0, idx-1)+STRING(val)+strmid(qry, idx, strlen(qry)-idx) $
      else $
        ans = strmid(qry, 0, idx-1)+' NULL '+strmid(qry, idx, strlen(qry)-idx)
      ans=ans(0)
      if (debug) then $
        print, "replace_input - number ("+ans+")"
  endelse
  return, ans
end

function BreakupStringAnswer, str, count
    sz = size(str)
    if n_elements(sz) eq 3 then $
      str = [str]
	leftover=''
	for i = 0, n_elements(str)-1 do begin
		if leftover ne '' then begin
		  str(i) = leftover+str(i)
		  leftover = ''
		endif
		tmp = strsplit(str(i), string(1b), /extract,/preserve)
		if strpos(str(i), string(1b), /reverse_search) ne strlen(str(i))-1 then begin
			leftover = tmp[n_elements(tmp)-1]
            if(n_elements(tmp) gt 1) then $
			  tmp = tmp(0:n_elements(tmp)-2) $
			else begin
			  tmp = leftover
			  leftover = ''
			endelse
		endif else tmp=tmp(0:n_elements(tmp)-2)
		if (i eq 0) then begin
			answer = tmp
		endif else begin
			answer = [answer, tmp]
		endelse
	endfor
	return, answer
end

function dsql, $
               lquery, a001, a002, a003, a004, a005, a006, a007, a008, a009, $
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
               error=error, debug=debug, nan=nan, _extra=ex

forward_function evaluate,mdsvalue

query = lquery
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
    if (debug) then begin
      help, i
      print, 'About to replace input  - i = ', i, 'idxs(i) = ', idxs(i)
      help, idxs, query
    endif
    old_len = strlen(query)
    cmd = 'b=a'+string(i+1, format='(I3.3)')
    ok = evaluate(cmd)
    query = replace_input(query, idxs(i), b, debug=debug)
    if (debug) then begin
      print, "After query replace IDXs "
      help, idxs
      help, query
    endif
    idxs = idxs + (strlen(query)-old_len)
    if (debug) then begin
      print, "After adding the offset "
      help, idxs
    endif
endfor

if (debug) then $
  print, "        Query (before QuoteQuotes: "+query+"***"

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
if (keyword_set(date) and keyword_set(nan)) then $
  expr = expr + ",'/NaN', '/date')" $   
else if (keyword_set(date)) then $
  expr = expr + ",'/date')" $
else if (keyword_set(nan)) then $
  expr = expr + ",'/NaN', *)" $
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

    error = count
    status = 0
    Message, count, noprint=quiet,/continue
    count = -1
endif else begin
    error = ''
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
        val = MDSVALUE("_"+arg,socket=!MDSDB_SOCKET,/QUIET)
        ok = evaluate(arg + ' = val')
        if (ok eq "OK") then status = 1L else status = 0L
        if (debug) then $
          print, "got back "+arg
;
; check if it is  a string.  If so parse
; out the delimiters (1b)
;
		cmd = 'sz = size('+arg+')'
		ok = evaluate(cmd)
                if (ok eq "OK") then status = 1L else status = 0L
		if (sz(n_elements(sz)-2) eq 7) then begin
		  cmd = arg+' = BreakupStringAnswer('+arg+',count)'
		  ok = evaluate(cmd)
                  if (ok eq "OK") then status = 1L else status = 0L
		endif
    endfor
;
; free all the tdi variables returned by DSQL
;
    if (not debug) then $
      dummy = MdsValue("DeAllocate('_A%%%')",socket=!MDSDB_SOCKET,/QUIET)
;
; return the number of row
;
endelse
return, count
;
end
