; @(#)evaluate.pro	1.25 10/10/11
;******************************************************************************
;FILE: evaluate.pro
;
;Collection of functions and procedures that replace some of 
;the functionality of the EXECUTE function.  The EVALUATE function
;takes a character string with a mathematical expression and, optionally,
;returns the value of the expression.  The return value of the function
;is a text string indicating success or failure.
;
;Variables can be set by specifying "<variable> = " in the expression.
;A variable by the given name will be set in the calling routine
;(this only works in IDL version 6.2 on up).
;
;Examples:
;
;   errstr = evaluate('3.4*20/56',result)
;   errstr = evaluate('d=3.4*20/56'[,result])
;
;Some things that this function can do.
; Simple arithmetic operators: +, -, *, /, ^.
; Any expressions with parantheses.
; Most strings and string concatenation.
; Ending letters to specify bytes and longs like "20B", "34L".
; Array concatenation like "[[1,2,1], [2,4,2], [1,2,1]]".
;
; Multi-dimensional arrays are handled as are simple structures and 
; structure arrays.
;
; System variables can be created and set, but known system variables can
; not be set unless the result is exactly the same type.
;
; Functions and procedures can be called but currently keywords cannot
; be passed.
;
;Some things that this function cannot do.
; Cannot evaluate "where", "if", "for", "while" expressions.
; Cannot evaluate boolean operators (like "and").
; Cannot evaluate relational operators (like "eq").
; Cannot evaluate minimum operator "<" or maximum operator ">".
; Cannot evaluate matrix multiplication operators "#" and "##".
; Cannot handle the syntax used by complex numbers.
; Cannot handle the syntax for hex and octal numbers or UL, LL, or ULL.
;
;TODO:
;Handle hex and octal numbers and allow for UL, LL, and ULL
;Allow for int's (currently all integers are considered longs unless B follows)
;Handle getting and setting of object fields
;Handle passing of keywords in the expression by creating a structure extra
;and passing it with _EXTRA=extra.
;
;Known problems:
;Functions and procesures that expect an argument to exist will give a traceback
;since the assumption here is that the argument will be set.  For example,
;"print,a" gives a traceback if a is undefined
;
;------------------------------------------------------------------------------
;Modifications:
;04/30/2010 RDJ created
;******************************************************************************

pro evaluate_listvar
common evaluate_common,vars,dbug

if(float(!version.release) lt '6.2') then begin
   if(n_elements(vars) eq 0) then return
   tags = tag_names(vars)
   for i=1l,n_elements(tags)-1 do begin
       print,tags(i)+' ',vars.(i)
   endfor
endif
end

pro evaluate_clearvar_all
common evaluate_common,vars,dbug
if(float(!version.release) lt '6.2') then vars = create_struct('xxxdummyxxx',0L)
end

pro evaluate_clearvar,varname
common evaluate_common,vars,dbug

if(float(!version.release) ge '6.2') then return
if(n_elements(vars) eq 0) then return
tags = tag_names(vars)
b = where(tags eq strupcase(varname),count)
if(count gt 0) then begin
   new_var = create_struct('xxxdummyxxx',0L)
   n = 1L
   for i=1, n_tags(vars)-1 do begin
       if(i ne b(0)) then begin
          new_var = create_struct(new_var,tags(i),vars.(i))
          n = n + 1
       endif
    endfor
    vars = new_var
endif
end

pro evaluate_setvalue,varname,value,errstr
forward_function scope_traceback,scope_varfetch
common evaluate_common,vars,dbug

   varname = strupcase(varname)
   nparams = n_params()
   errstr = "OK"
   ;print,'evaluate_setvalue: ',varname
   if(float(!version.release) lt '6.2') then goto,old_way

   a = scope_traceback()
   for level=n_elements(a)-1,0,-1 do begin
       space = strpos(a(level),' ')
       if(space gt -1) then a(level) = strmid(a(level),0,space)
       if(a(level) eq 'EVALUATE') then goto,endloop
   endfor
   endloop:
   if(level lt 0) then goto,old_way

   v = varname
   n = strpos(varname,'.')
   if(n eq -1) then begin
      if(strmid(varname,0,1) eq '!') then begin
         defsysv,varname,value
      endif else begin
         if(n_elements(value) gt 0) then $
         (scope_varfetch(varname,/enter,level=level)) = value
      endelse
   endif else begin
      v = strmid(varname,0,n)
      tmp = scope_varfetch(v,level=level)
      tag = strmid(varname,n+1)
      if(size(tmp,/tname) ne "STRUCT") then begin
         errstr = "Expression must be a structure in this context: " + v
         return
      endif
      tags = tag_names(tmp)
      a = where(tags eq strupcase(tag),count)
      if(count eq 0) then begin
         errstr = 'unknown structure tag for '+v+": "+tag
         return
      endif
      a = a(0)
      !ERROR = 0
      tmp.(a) = value
      if(!ERROR ne 0) then begin
         errstr = !ERR_STRING
         if(nparams eq 2) then begin
            print,forced_stop1
         endif
         return
      endif
      (scope_varfetch(v,/enter,level=level)) = tmp
   endelse
   return

old_way:
   evaluate_clearvar,varname
   if(n_elements(vars) eq 0) then vars = create_struct('xxxdummyxxx',0L)
   vars = create_struct(vars,varname,value)
   return
end

function evaluate_getvalue,varname,value
forward_function scope_traceback,scope_varfetch,scope_varname
common evaluate_common,vars,dbug

   varname = strupcase(varname)
   value = 0.0
   count = 0L
   if(float(!version.release) lt '6.2') then goto,old_way

;As of Version 6.3:
;System variables cannot be returned with scope_varfetch
;so this is very limited.  Use help to get the help string
;(note the string can change with idl version).
;Then look for strings, floats, and assume long if neither.
;Structures and arrays cannot be dealt with because there 
;is no way to get the system variable's value.

   if(strmid(varname,0,1) eq '!') then begin
      defsysv,varname,exists=test
      if(test) then begin
         test = 0L
         help,names=varname,/system,output=output
         output = output(0)
         n = strpos(output,"=")
         if(n ne -1) then begin
            svalue = strtrim(strmid(output,n+1),2)
            n = strpos(svalue,"'")
            if(n ne -1) then begin
               value = strmid(svalue,n+1)
               value = strmid(value,0,strlen(value)-1)
               test = 1
            endif else begin
               if(strpos(output,"->") ne -1) then begin
                  value = scope_varfetch(varname) ;can't get system vars
                  print,forced_stop2
               endif
               n = strpos(svalue,".")
               if(n ne -1) then begin
                  value = float(svalue)
               endif else begin
                  value = long(svalue)
               endelse
               test = 1
            endelse
         endif
      endif
      return,test
   endif
   a = scope_traceback()
   for level=n_elements(a)-1,0,-1 do begin
       space = strpos(a(level),' ')
       if(space gt -1) then a(level) = strmid(a(level),0,space)
       if(a(level) eq 'EVALUATE') then goto,endloop
   endfor
   endloop:
   if(level lt 0) then goto,old_way

   names = scope_varname(level=level)
   n = strpos(varname,'.')
   if(n eq -1) then begin
      a = where(names eq strupcase(varname),count)
      if(count gt 0) then begin
         help,names=varname,level=level,output=output
         if(strpos(output,'UNDEFINED') eq -1) then $
            value = scope_varfetch(varname,level=level)
            ;print,'fetched '+varname
      endif
   endif else begin
      v = strmid(varname,0,n)
      tmp = scope_varfetch(v,level=level)
      tag = strmid(varname,n+1)
      if(size(tmp,/tname) eq 'STRUCT') then begin
         tags = tag_names(tmp)
         a = where(tags eq strupcase(tag),count)
         if(count gt 0) then begin
            a = a(0)
            value = tmp.(a)
         endif
      endif
      if(size(tmp,/tname) eq 'OBJREF') then begin
      endif
   endelse
   return,count

old_way:
   if(n_elements(vars) gt 0) then begin
      tags = tag_names(vars)
      a = where(tags eq strupcase(varname),count)
      if(count gt 0) then begin
         value = vars.(a(0))
         return,1
      endif
   endif
   return,0
end

function evaluate_isstring,expression,location,length
length = 0L
if(location ge n_elements(expression)) then return,0
c = expression[location]
if(c ne byte('"') and c ne byte("'")) then return,0
start = location + 1
for i=start,n_elements(expression)-1 do begin
    if(expression[i] eq c) then goto,done
    length = length + 1
endfor
done:
return,1
end

function evaluate_isalnum,expression,location,first
if(location ge n_elements(expression)) then return,0
if(first) then begin
   if(expression[location] ge byte('A') and $
      expression[location] le byte('Z')) then return,1
   if(expression[location] ge byte('a') and $
      expression[location] le byte('z')) then return,1
   if(expression[location] eq byte('!')) then return,1
   return,0
endif
if(expression[location] ge byte('A') and $
   expression[location] le byte('Z')) then return,1
if(expression[location] ge byte('a') and $
   expression[location] le byte('z')) then return,1
if(expression[location] ge byte('0') and $
   expression[location] le byte('9')) then return,1
if(expression[location] eq byte('$')) then return,1
if(expression[location] eq byte('_')) then return,1
if(expression[location] eq byte('.')) then return,1
return,0
end

function evaluate_iswhite,expression,location
if(location ge n_elements(expression)) then return,0
if(expression[location] eq byte(' ')) then return,1
if(expression[location] eq 10) then return,1
return,0
end

function evaluate_isnumer,expression,location
if(location ge n_elements(expression)) then return,0
if(expression[location] ge byte('0') and $
   expression[location] le byte('9')) then return,1
if(expression[location] eq byte('.') and location gt 0) then begin
   if(expression[location-1] ge byte('0') and $
      expression[location-1] le byte('9')) then return,1
   if(location lt n_elements(expression)) then begin
      if(expression[location+1] ge byte('0') and $
         expression[location+1] le byte('9')) then return,1
   endif
endif
if(expression[location] eq byte('-') or $
   expression[location] eq byte('+')) then begin
   if(location eq 0) then begin
      ;cases where it can't be a unary operator
      if(location lt n_elements(expression)) then begin
         if(expression[location+1] ge byte('0') and $
            expression[location+1] le byte('9')) then return,1
         if(expression[location+1] eq byte('.')) then return,1
      endif
   endif else begin
      if(location lt n_elements(expression)) then begin
         ;must be unary if next character is part of variable name
         if(expression[location+1] ge byte('A') and $
            expression[location+1] le byte('Z')) then return,0
         if(expression[location+1] ge byte('a') and $
            expression[location+1] le byte('z')) then return,0
         if(expression[location+1] eq byte('$')) then return,0
         if(expression[location+1] eq byte('_')) then return,0
         if(expression[location+1] eq byte('!')) then return,0
         if(expression[location+1] eq byte('(')) then return,0
         if(expression[location+1] eq byte('[')) then return,0

         ;cases where it can't be a unary operator
         if(expression[location+1] eq byte('[')) then return,1
         if(expression[location-1] eq byte('[')) then return,1
         if(expression[location-1] eq byte('(')) then return,1
         if(expression[location-1] eq byte('=')) then return,1
         if(expression[location-1] eq byte(',')) then return,1
         if(expression[location-1] eq byte(':')) then return,1
         if(expression[location-1] eq byte('<')) then return,1
         if(expression[location-1] eq byte('>')) then return,1
      endif
   endelse
endif
return,0
end

function evaluate_isdelim,expression,location
if(location ge n_elements(expression)) then return,0
if(expression[location] eq byte('>')) then return,1
if(expression[location] eq byte('<')) then return,1
if(expression[location] eq byte('+')) then return,1
if(expression[location] eq byte('-')) then return,1
if(expression[location] eq byte('*')) then return,1
if(expression[location] eq byte('/')) then return,1
if(expression[location] eq byte('^')) then return,1
if(expression[location] eq byte('(')) then return,1
if(expression[location] eq byte(')')) then return,1
if(expression[location] eq byte('[')) then return,1
if(expression[location] eq byte(']')) then return,1
if(expression[location] eq byte(',')) then return,1
if(expression[location] eq byte('=')) then return,1
if(expression[location] eq byte(':')) then return,1
return,0
end

function evaluate_isdelim2,expression,location
if(location ge n_elements(expression)) then return,0
if(expression[location] eq byte('>')) then return,1
if(expression[location] eq byte('<')) then return,1
if(expression[location] eq byte('+')) then return,1
if(expression[location] eq byte('-')) then return,1
if(expression[location] eq byte('*')) then return,1
if(expression[location] eq byte('/')) then return,1
if(expression[location] eq byte('^')) then return,1
if(expression[location] eq byte('(')) then return,1
if(expression[location] eq byte('[')) then return,1
if(expression[location] eq byte(']')) then return,1
if(expression[location] eq byte(':')) then return,1
return,0
end

function evaluate_strtonum,expression,location
start = location
ifloat = 0
ibyte = 0
while(location lt n_elements(expression)) do begin
    if(expression[location] eq byte('+') or $
       expression[location] eq byte('-')) then begin
       location = location + 1
       goto,next
    endif
    if(expression[location] ge byte('0') and $
       expression[location] le byte('9')) then begin
       location = location + 1
       goto,next
    endif
    if(expression[location] eq byte('L')) then begin
       location = location + 1
       goto,next
    endif
    if(expression[location] eq byte('B')) then begin
       location = location + 1
       ibyte = 1
       goto,next
    endif
    if(expression[location] eq byte('.')) then begin
       location = location + 1
       ifloat = 1
       goto,next
    endif
    if(expression[location] eq byte('E') or $
       expression[location] eq byte('e')) then begin
       location = location + 1
       ifloat = 1
       if(location lt n_elements(expression)) then begin
          if(expression[location] eq byte('+') or $
             expression[location] eq byte('-')) then begin
             location = location + 1
             goto,next
          endif
       endif
       goto,next
    endif
    if(expression[location] eq byte('.')) then begin
       location = location + 1
       ifloat = 1
       goto,next
    endif
    goto,done
    next:
endwhile
done:
if(ifloat eq 1) then begin
   value = float(string(expression[start:*]))
endif else begin
   n = n_elements(expression)
   if(ibyte eq 1) then value = byte(long(string(expression[start:n-2])))
   if(ibyte eq 0) then value = long(string(expression[start:*]))
endelse
return,value
end

pro evaluate_gettoken,expression,location,type,token,ntoken
forward_function evaluate_iswhite,evaluate_isdelim,evaluate_isnumer
forward_function evaluate_isalnum,evaluate_strtonum,evaluate_isstring
common evaluate_common,vars,dbug

token = 0B
ntoken = 0L
type = 'NONE'
if(location ge n_elements(expression)) then begin
   if(dbug eq 1) then print,'gettoken: return with type=0
   return
endif

token = bytarr(1024)
while( evaluate_iswhite(expression,location) ) do location = location + 1
if(location ge n_elements(expression)) then begin
   if(dbug eq 1) then print,'gettoken: return with type=0
   return
endif
   
if( evaluate_isstring(expression,location,length) ) then begin
   type = 'STRING'
   c = expression[location]
   if(length eq 0) then begin
      token = 0B
      ntoken = 1
   endif else begin
      token = expression[location+1:location+length]
      ntoken = length
   endelse
   location = location + 1 + length
   if(location lt n_elements(expression)) then begin
      if(expression[location] eq c) then location = location + 1
   endif
   goto,next
endif
if( evaluate_isnumer(expression,location) ) then begin
   type = 'NUMBER'
   temp = expression[location:*]
   ntemp = 0L
   value = evaluate_strtonum(temp,ntemp)
   token[ntoken:ntoken+ntemp-1] = temp(0:ntemp-1)
   ntoken = ntoken + ntemp
   location = location + ntemp
   goto,next
endif
if( evaluate_isdelim(expression,location) ) then begin
   type = 'DELIMITER'
   token[ntoken] = expression[location]
   ntoken = ntoken + 1
   location = location + 1
   goto,next
endif
if( evaluate_isalnum(expression,location,1) ) then begin
   type = 'VARIABLE'
   first = 1
   while( evaluate_isalnum(expression,location,first) ) do begin
      token[ntoken] = expression[location]
      ntoken = ntoken + 1
      location = location + 1
      first = 0
   endwhile
   goto,next
endif

if(dbug eq 1) then print,string(expression),location
if( expression[location] ) then begin
   token[ntoken] = expression[location]
   ntoken = ntoken + 1
   location = location + 1
   if(dbug eq 1) then print,'gettoken: syntax error in '+string(token)
   goto,next
endif

token[ntoken] = expression[location]
if(dbug eq 1) then print,'gettoken: return with type=',type,' "'+string(token)+'"'
return

next:
while( evaluate_iswhite(expression,location) ) do location = location + 1
token = token[0:ntoken-1]
if(dbug eq 1) then print,'gettoken: return with type=',type,' "'+string(token)+'"'
end

;This function looks for the end of delimited clause, (i.e., ')' or ']')
;that matches the first occurance of the given delimiter.
;It also returns the count of each delimiter so that the caller can
;check for syntax errors.  A return of -1 indicates no ending delimiter
;(assuption is that the given delimiter was already found).  A return of
;-2 indicates not enough closing delimiters.  Note that the position of
;a matching delimiter will always be returned if one exists.

function evaluate_match_delim,expr,delim,ndelim,ndelim2
bdelim = byte(delim)
bdelim = bdelim[0]
if(bdelim eq byte('(')) then delim2 = ')'
if(bdelim eq byte('[')) then delim2 = ']'
bdelim2 = byte(delim2)
bdelim2 = bdelim2[0]
ndelim = 0L
ndelim2 = 0L
n = strlen(expr)
s2 = strpos(expr,delim2)
if(s2 eq -1) then return,-1L
for i=0L,n-1 do begin
    if(expr[i] eq bdelim) then ndelim = ndelim + 1
    if(expr[i] eq bdelim2) then ndelim2 = ndelim2 + 1
endfor
count = 0L
for i=0L,n-1 do begin
    if(expr[i] eq bdelim) then count = count + 1
    if(expr[i] eq bdelim2) then begin
       count = count - 1
       if(count eq 0) then return,i
    endif
endfor
return,-2L ;not enough ending delimiters
end

function evaluate_do_array,name,type,expression,location,$
    token,ntoken,result,left=left
forward_function evaluate_level1,evaluate_getvalue
common evaluate_common,vars,dbug

    known_var = evaluate_getvalue(name,var_value)
    if(location lt n_elements(expression)) then $
       c = expression[location] else c = 0B

;check if known variable with array element(s) given
;this handles multi-dimensional arrays

    if(known_var eq 1 and (c eq byte('[') or c eq byte('('))) then begin
       location_save = location
       evaluate_gettoken,expression,location,type,token,ntoken ;'(' or '['
       if(c eq byte('[')) then c1 = byte(']') else c1 = byte(')')

       stypes = strarr(8)
       dim1 = 0L
       dim2 = 0L
       dim3 = 0L
       dim4 = 0L
       dim5 = 0L
       dim6 = 0L
       dim7 = 0L
       dim8 = 0L
       vndim = size(var_value,/n_dimensions)
       vdims = size(var_value,/dimensions)
       if(vndim eq 0) then begin ;scalars index must be 0
          vdims = 1L
          vndim = 1L
       endif
       nsubscripts = 0L ;count the subscripts given
       for nth_dim=0L,vndim-1 do begin
          evaluate_gettoken,expression,location,type,token,ntoken
          if(token[0] eq 0) then goto,endloop
          if(token[0] eq c1) then return,"Syntax error"
          if(token[0] eq byte(',')) then return,"Missing argument"

          if(token[0] eq byte('*')) then begin ; c[*,0] is legal
             stypes[nth_dim] = 'all'
             evaluate_gettoken,expression,location,type,token,ntoken ;'*'
             index1 = 0L
             index2 = vdims(nth_dim) - 1
             goto,do_array
          endif

          errstr = evaluate_level1(type,expression,location,token,$
              ntoken,index1)
          if(dbug eq 1) then print,'level6 back3 from level1 ',errstr
          if(errstr ne "OK") then return,errstr

          t = size(index1,/tname)
          if(t ne "LONG" and t ne "INT" and t ne "BYTE") then begin
             if(c eq byte('[')) then return,"bad argument to array"
             location = location_save
             goto,do_func
          endif

          if(token[0] eq byte(':')) then begin
             stypes[nth_dim] = 'range'
             if(n_elements(index1) gt 1) then return,$
                "subscript used for "+name+" must be a scalar"
             evaluate_gettoken,expression,location,type,token,ntoken ;':'
             if(token[0] eq byte('*')) then begin
                evaluate_gettoken,expression,location,type,token,ntoken ;'*'
                index2 = vdims(nth_dim) - 1
                goto,do_array
             endif
             errstr = evaluate_level1(type,expression,location,token,$
                 ntoken,index2)
             t = size(index2,/tname)
             if(t ne "LONG" and t ne "INT" and t ne "BYTE") then begin
                if(c eq byte('[')) then return,"bad argument to array"
                location = location_save
                goto,do_func
             endif
do_array:
             if(index1 lt 0 or index1 gt index2) then $
                return,"subscript "+strtrim(index1,2)+$
                       " out of range for "+name
             if(index2 lt index1) then $
                return,"subscript "+strtrim(index2,2)+$
                       " out of range for "+name
             n = index2 - index1 + 1
             if(nth_dim eq 0) then dim1 = lindgen(n) + index1
             if(nth_dim eq 1) then dim2 = lindgen(n) + index1
             if(nth_dim eq 2) then dim3 = lindgen(n) + index1
             if(nth_dim eq 3) then dim4 = lindgen(n) + index1
             if(nth_dim eq 4) then dim5 = lindgen(n) + index1
             if(nth_dim eq 5) then dim6 = lindgen(n) + index1
             if(nth_dim eq 6) then dim7 = lindgen(n) + index1
             if(nth_dim eq 7) then dim8 = lindgen(n) + index1
          endif else begin

;If a scalar is used for a subscript, then the subscript has to be between
;0 and the number of elements in the dimension if the current dimension
;is greater than 1.  But if an array is given,
;then IDL quietly clips the values to fit.  So test if INDEX1 is a scalar
;and the current dimension is greater than 0.
;If only one scalar is given, and the right side is more than one value,
;then IDL quietly sets the array values starting at the scalar:
;e.g., aa[4] = [34,35] will set two elements starting at element 4 of aa
;no matter how many dimensions aa is (it is considered one dimension).

             ni = size(index1,/n_dim)
             if(ni eq 0) then stypes[nth_dim] = 'scalar' $
             else stypes[nth_dim] = 'array'
             if(nth_dim eq 0) then dim1 = index1
             if(nth_dim eq 1) then dim2 = index1
             if(nth_dim eq 2) then dim3 = index1
             if(nth_dim eq 3) then dim4 = index1
             if(nth_dim eq 4) then dim5 = index1
             if(nth_dim eq 5) then dim6 = index1
             if(nth_dim eq 6) then dim7 = index1
             if(nth_dim eq 7) then dim8 = index1
          endelse
          nsubscripts = nsubscripts + 1L
       endfor
       if(token[0] eq byte(',')) then $
          return,"too many subscripts to "+name
endloop:

;At this point we have the number of subscripts provided and the
;indices specified for each dimension as an array or scalar.
;IDL allows certain expressions that normally would give errors:
;
;1) If only one dimension is given for a multi-dimensional array,
;then the array is considered one dimensional and the given elements
;are used so c=[[0,1],[2,3],[4,5]] & cc=c[1:3] gives 1,2,3.  But 
;an index outside the range of the array gives an error.
;2) If the number of array dimensions is less than vndim, IDL does not
;give an error.  Instead, IDL returns a value as if the remaining 
;dimensions are zeros.  So "c=[[0,1],[2,3],[4,5]] & cc=c[1]" 
;returns c[1,0,0] and cc=c[1,1] returns c[1,1,0].  But IDL gives
;an error if the indices are out of range UNLESS only one dimension
;is given (see item 1).
;3) If a scalar is used for a subscript, then the subscript has to be 
;between 0 and the number of elements in the dimension if the current 
;dimension is greater than 1.  But if an array is given,
;then IDL quietly clips the values to fit.  So test if INDEX1 is a scalar
;and the current dimension is greater than 0.
;If only one scalar is given, and the right side is more than one value,
;then IDL quietly sets the array values starting at the scalar:
;e.g., aa[4] = [34,35] will set two elements starting at element 4 of aa
;no matter how many dimensions aa is (it is considered one dimension).

       if(nsubscripts eq 1) then begin  ;no errors if stype is 'array'
          if(stypes(0) eq 'scalar') then begin
             if(dim1 lt 0 or dim1 gt n_elements(var_value)-1) then $
                return,"subscript "+strtrim(dim1,2)+$
                       " out of range for "+name
          endif
          if(stypes(0) eq 'range') then begin
             if(dim1(0) lt 0) then $
                return,"subscript "+strtrim(dim1(0),2)+$
                       " out of range for "+name
             if(dim1(0) lt 0 or $
                dim1(n_elements(dim1)-1) gt n_elements(var_value)-1) then $
                return,"subscript out of range for "+name
          endif
          if(stypes(0) eq 'all') then begin
             dim1 = lindgen(n_elements(var_value))
          endif
       endif else begin
          for nth_dim=0L,nsubscripts-1 do begin
             if(nth_dim eq 0) then dim = dim1
             if(nth_dim eq 1) then dim = dim2
             if(nth_dim eq 2) then dim = dim3
             if(nth_dim eq 3) then dim = dim4
             if(nth_dim eq 4) then dim = dim5
             if(nth_dim eq 5) then dim = dim6
             if(nth_dim eq 6) then dim = dim7
             if(nth_dim eq 7) then dim = dim8

             stype = stypes(nth_dim)
             if(stype eq 'scalar' or stype eq 'range') then begin
                if(dim(0) lt 0) then $
                   return,"subscript "+strtrim(dim(0),2)+$
                          " out of range for "+name
                if(dim(n_elements(dim)-1) gt vdims(nth_dim)-1) then $
                   return,"subscript "+strtrim(dim(n_elements(dim)-1),2)+$
                          " out of range for "+name
             endif
          endfor
       endelse

;There are two cases: the indices specified are on the left side of =
;which results in an array of indices:  [2,1,[0:1]] = array of size 2
;representing the indices into the variable as if it was one-dimensional.
;The other case is on the right side of = where the VALUE of the variable
;with the given indices is returned.
       if(keyword_set(left)) then begin
          result = dim1
          if(stypes(1) eq '') then goto,left_done ;one-dim array
          if(vndim eq 2) then result = dim1+dim2*vdims[0]
          if(vndim eq 3) then result = dim1+dim2*vdims[0]+$
                         dim3*vdims[0]*vdims[1]
          if(vndim eq 4) then result = dim1+dim2*vdims[0]+$
                         dim3*vdims[0]*vdims[1]+$
                         dim4*vdims[0]*vdims[1]*vdims[2]
          if(vndim eq 5) then result = dim1+dim2*vdims[0]+$
                         dim3*vdims[0]*vdims[1]+$
                         dim4*vdims[0]*vdims[1]*vdims[2]+$
                         dim5*vdims[0]*vdims[1]*vdims[2]*vdims[3]
          if(vndim eq 6) then result = dim1+dim2*vdims[0]+$
                         dim3*vdims[0]*vdims[1]+$
                         dim4*vdims[0]*vdims[1]*vdims[2]+$
                         dim5*vdims[0]*vdims[1]*vdims[2]*vdims[3]+$
                         dim6*vdims[0]*vdims[1]*vdims[2]*vdims[3]*vdims[4]
          if(vndim eq 7) then result = dim1+dim2*vdims[0]+$
                         dim3*vdims[0]*vdims[1]+$
                         dim4*vdims[0]*vdims[1]*vdims[2]+$
                         dim5*vdims[0]*vdims[1]*vdims[2]*vdims[3]+$
                         dim6*vdims[0]*vdims[1]*vdims[2]*vdims[3]*vdims[4]+$
             dim7*vdims[0]*vdims[1]*vdims[2]*vdims[3]*vdims[4]*vdims[5]
          if(vndim eq 8) then result = dim1+dim2*vdims[0]+$
                         dim3*vdims[0]*vdims[1]+$
                         dim4*vdims[0]*vdims[1]*vdims[2]+$
                         dim5*vdims[0]*vdims[1]*vdims[2]*vdims[3]+$
                         dim6*vdims[0]*vdims[1]*vdims[2]*vdims[3]*vdims[4]+$
           dim7*vdims[0]*vdims[1]*vdims[2]*vdims[3]*vdims[4]*vdims[5]+$
           dim8*vdims[0]*vdims[1]*vdims[2]*vdims[3]*vdims[4]*vdims[5]*vdims[6]
left_done:
       endif else begin
          if(nsubscripts eq 1) then result = var_value(dim1) else $
          result = var_value(dim1,dim2,dim3,dim4,dim5,dim6,dim7,dim8)
          if(n_elements(result) eq 1 and size(result,/n_dim) eq 1) then $
             result = result[0]
       endelse
       evaluate_gettoken,expression,location,type,token,ntoken ;')' or ']'

;return an error code if the number of subscripts was greater than 1 but
;less than vndim.  The calling function can check for this in case the
;right hand side has a different number of element values as elements
;to set.
       if(keyword_set(left) eq 1 and nsubscripts gt 1 and $
          nsubscripts lt vndim) then return,$
          "fewer subscripts "+strtrim(nsubscripts,2)

;if var_value is multi-dimensional and all subscripts were given and
;only one element was specified [2,1,0] for 3-dim array, then return
;an array rather than a scalar to specify this case.
       if(keyword_set(left) eq 1 and nsubscripts gt 1 and $
          nsubscripts eq vndim and n_elements(result) eq 1) then $
          result = [result]
       return,"OK"
    endif

do_func:
    return,"Unknown variable1: "+name
end

function evaluate_do_procedure,name,type,expression,location,$
         token,ntoken,result
forward_function evaluate_level1,evaluate_getvalue
forward_function evaluate_isdelim2
common evaluate_common,vars,dbug

    if(location lt n_elements(expression)) then $
       c = expression[location] else c = 0B
    if(c eq byte(',')) then begin
       proc_name = name
       evaluate_gettoken,expression,location,type,token,ntoken ;','
       n = 0
       args = strarr(10)
       types = strarr(10)
       loop1:
           undefined = 0
           evaluate_gettoken,expression,location,type,token,ntoken
           types[n] = type

;procedures will set arguments unless the argument is an array element,
;part of a structure, or part of an expression.  So allow the setting
;of the variable after the procedure is executed only if the next
;character is not a delimiter other than ',' or ')'.
           if(type eq 'VARIABLE') then begin ;only set var if no element
              if( evaluate_isdelim2(expression,location) ) then $
                 types[n] = '' $
              else args[n] = string(token)
           endif
           if(token[0] eq 0B) then goto,nomore_args

;If variable is undefined, that is ok.

           errstr = evaluate_level1(type,expression,location,token,$
               ntoken,value)
           if(strmid(errstr,0,16) eq 'Unknown variable') then begin
              if(dbug eq 1) then print,'*** do_proc:: unknown variable OK ***'
              errstr = 'OK'
              undefined = 1
           endif
           if(errstr ne "OK") then begin
              if(dbug eq 1) then print,'do_proc: back4 from level1 ',errstr
              return,errstr
           endif
           if(n eq 0 and undefined eq 0) then a0 = value
           if(n eq 1 and undefined eq 0) then a1 = value
           if(n eq 2 and undefined eq 0) then a2 = value
           if(n eq 3 and undefined eq 0) then a3 = value
           if(n eq 4 and undefined eq 0) then a4 = value
           if(n eq 5 and undefined eq 0) then a5 = value
           if(n eq 6 and undefined eq 0) then a6 = value
           if(n eq 7 and undefined eq 0) then a7 = value
           if(n eq 8 and undefined eq 0) then a8 = value
           if(n eq 9 and undefined eq 0) then a9 = value
           if(type eq 'VARIABLE') then begin
              evaluate_gettoken,expression,location,type,token,ntoken
           endif
           n = n + 1
           if((n lt 10) and (token[0] eq byte(','))) then goto,loop1

;Call the procedure, passing the arguments.
;Note: this can lead to errors if a variable is undefined and it needs to
;be defined, for example, if the procedure is "print".
nomore_args:
       evaluate_gettoken,expression,location,type,token,ntoken
       case n of
          0: call_procedure,proc_name
          1: call_procedure,proc_name,a0
          2: call_procedure,proc_name,a0,a1
          3: call_procedure,proc_name,a0,a1,a2
          4: call_procedure,proc_name,a0,a1,a2,a3
          5: call_procedure,proc_name,a0,a1,a2,a3,a4
          6: call_procedure,proc_name,a0,a1,a2,a3,a4,a5
          7: call_procedure,proc_name,a0,a1,a2,a3,a4,a5,a6
          8: call_procedure,proc_name,a0,a1,a2,a3,a4,a5,a6,a7
          9: call_procedure,proc_name,a0,a1,a2,a3,a4,a5,a6,a7,a8
         10: call_procedure,proc_name,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9
          else: return,'too many arguments to procedure
       endcase
       errstr = "OK"
       if(types[0] eq 'VARIABLE') then evaluate_setvalue,args[0],a0,errstr
       if(errstr ne "OK") then goto,proc_done
       if(types[1] eq 'VARIABLE') then evaluate_setvalue,args[1],a1,errstr
       if(errstr ne "OK") then goto,proc_done
       if(types[2] eq 'VARIABLE') then evaluate_setvalue,args[2],a2,errstr
       if(errstr ne "OK") then goto,proc_done
       if(types[3] eq 'VARIABLE') then evaluate_setvalue,args[3],a3,errstr
       if(errstr ne "OK") then goto,proc_done
       if(types[4] eq 'VARIABLE') then evaluate_setvalue,args[4],a4,errstr
       if(errstr ne "OK") then goto,proc_done
       if(types[5] eq 'VARIABLE') then evaluate_setvalue,args[5],a5,errstr
       if(errstr ne "OK") then goto,proc_done
       if(types[6] eq 'VARIABLE') then evaluate_setvalue,args[6],a6,errstr
       if(errstr ne "OK") then goto,proc_done
       if(types[7] eq 'VARIABLE') then evaluate_setvalue,args[7],a7,errstr
       if(errstr ne "OK") then goto,proc_done
       if(types[8] eq 'VARIABLE') then evaluate_setvalue,args[8],a8,errstr
       if(errstr ne "OK") then goto,proc_done
       if(types[9] eq 'VARIABLE') then evaluate_setvalue,args[9],a9,errstr
proc_done:
       if(dbug eq 1) then print,'do_proc: return6 ',errstr
       return,errstr
    endif

    if(evaluate_getvalue(string(token),result) eq 0) then begin
       return,"Unknown variable2: "+string(token)
    endif
    evaluate_gettoken,expression,location,type,token,ntoken
    if(dbug eq 1) then print,'do_proc: return7 OK ',result
    return,"OK"
end

function evaluate_do_function,name,type,expression,location,$
         token,ntoken,result
forward_function evaluate_level1,evaluate_getvalue
forward_function evaluate_isdelim2,evaluate_do_procedure
common evaluate_common,vars,dbug

    if(location lt n_elements(expression)) then $
       c = expression[location] else c = 0B
    if(c eq byte('(')) then begin
       func_name = name
       evaluate_gettoken,expression,location,type,token,ntoken ;'('
       n = 0
       args = strarr(10)
       types = strarr(10)
       loop1:
           undefined = 0
           evaluate_gettoken,expression,location,type,token,ntoken
           types[n] = type

;functions will set arguments unless the argument is an array element,
;part of a structure, or part of an expression.  So allow the setting
;of the variable after the function is executed only if the next
;character is not a delimiter other than ',' or ')'.
           if(type eq 'VARIABLE') then begin ;only set var if no element
              if( evaluate_isdelim2(expression,location) ) then $
                 types[n] = '' $
              else args[n] = string(token)
           endif
           if(token[0] eq byte(',')) then return,"Missing argument"
           if(token[0] eq byte(')')) then goto,nomore_args

;If variable is undefined, that is ok.

           errstr = evaluate_level1(type,expression,location,token,$
               ntoken,value)
           if(strmid(errstr,0,16) eq 'Unknown variable') then begin
              if(dbug eq 1) then print,'*** do_func: unknown variable OK ***'
              errstr = 'OK'
              undefined = 1
           endif
           if(errstr ne "OK") then begin
              if(dbug eq 1) then print,'do_func: back4 from level1 ',errstr
              return,errstr
           endif
           if(n eq 0 and undefined eq 0) then a0 = value
           if(n eq 1 and undefined eq 0) then a1 = value
           if(n eq 2 and undefined eq 0) then a2 = value
           if(n eq 3 and undefined eq 0) then a3 = value
           if(n eq 4 and undefined eq 0) then a4 = value
           if(n eq 5 and undefined eq 0) then a5 = value
           if(n eq 6 and undefined eq 0) then a6 = value
           if(n eq 7 and undefined eq 0) then a7 = value
           if(n eq 8 and undefined eq 0) then a8 = value
           if(n eq 9 and undefined eq 0) then a9 = value
           if(type eq 'VARIABLE') then begin
              evaluate_gettoken,expression,location,type,token,ntoken
           endif
           n = n + 1
           if((n lt 10) and (token[0] eq byte(','))) then goto,loop1

nomore_args:
       if(token[0] ne byte(')')) then $
          return,"Expected ')' at '"+string(token)+"'"
       evaluate_gettoken,expression,location,type,token,ntoken
       case n of
          0: result = call_function(func_name)
          1: result = call_function(func_name,a0)
          2: result = call_function(func_name,a0,a1)
          3: result = call_function(func_name,a0,a1,a2)
          4: result = call_function(func_name,a0,a1,a2,a3)
          5: result = call_function(func_name,a0,a1,a2,a3,a4)
          6: result = call_function(func_name,a0,a1,a2,a3,a4,a5)
          7: result = call_function(func_name,a0,a1,a2,a3,a4,a5,a6)
          8: result = call_function(func_name,a0,a1,a2,a3,a4,a5,a6,a7)
          9: result = call_function(func_name,a0,a1,a2,a3,a4,a5,a6,a7,a8)
         10: result = call_function(func_name,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9)
          else: return,'too many arguments to function
       endcase
       errstr = "OK"
       if(types[0] eq 'VARIABLE') then evaluate_setvalue,args[0],a0,errstr
       if(errstr ne "OK") then goto,func_done
       if(types[1] eq 'VARIABLE') then evaluate_setvalue,args[1],a1,errstr
       if(errstr ne "OK") then goto,func_done
       if(types[2] eq 'VARIABLE') then evaluate_setvalue,args[2],a2,errstr
       if(errstr ne "OK") then goto,func_done
       if(types[3] eq 'VARIABLE') then evaluate_setvalue,args[3],a3,errstr
       if(errstr ne "OK") then goto,func_done
       if(types[4] eq 'VARIABLE') then evaluate_setvalue,args[4],a4,errstr
       if(errstr ne "OK") then goto,func_done
       if(types[5] eq 'VARIABLE') then evaluate_setvalue,args[5],a5,errstr
       if(errstr ne "OK") then goto,func_done
       if(types[6] eq 'VARIABLE') then evaluate_setvalue,args[6],a6,errstr
       if(errstr ne "OK") then goto,func_done
       if(types[7] eq 'VARIABLE') then evaluate_setvalue,args[7],a7,errstr
       if(errstr ne "OK") then goto,func_done
       if(types[8] eq 'VARIABLE') then evaluate_setvalue,args[8],a8,errstr
       if(errstr ne "OK") then goto,func_done
       if(types[9] eq 'VARIABLE') then evaluate_setvalue,args[9],a9,errstr
func_done:
       if(dbug eq 1) then print,'do_func: return6 ',errstr,' ',result
       return,errstr
    endif

    if(evaluate_getvalue(string(token),result) eq 0) then begin
       procedures = ['PRINT','SPAWN','RESOLVE_ROUTINE',routine_info(),'PLOT']
       name = strupcase(token)
       a = where(procedures eq name,count)
       if(count eq 0) then return,"Unknown variable3: "+name
       errstr = evaluate_do_procedure(name,type,expression,location,$
           token,ntoken,result)
       return,errstr
    endif
    evaluate_gettoken,expression,location,type,token,ntoken
    if(dbug eq 1) then print,'do_func: return7 OK ',result
    return,"OK"
end

function evaluate_bracket,level,dims,type,expression,location,$
         token,ntoken,result
forward_function evaluate_level1,evaluate_getvalue
forward_function evaluate_do_function
common evaluate_common,vars,dbug

    level_in = level
    level = level + 1
    dims = lonarr(level)
    if(dbug eq 1) then print,'bracket ',type,level,' '+string(token),ntoken
    evaluate_gettoken,expression,location,type,token,ntoken ;'['

top:
    if(token[0] eq byte('[')) then begin
       errstr = evaluate_bracket(level,dims,type,expression,location,$
                token,ntoken,result)
       if(errstr ne "OK") then return,errstr
       goto,top
    endif

    if(token[0] eq byte(']')) then begin
       level = level - 1
       if(dims[level] eq 0) then begin
          divisor = 1L
          for i=level+1,n_elements(dims)-1 do begin
              divisor = divisor * dims[i]
          endfor
          dims(level) = n_elements(result)/divisor
       endif
       evaluate_gettoken,expression,location,type,token,ntoken ;']'
       errstr = "OK"
       goto,do_return
    endif

    if(token[0] eq byte(',')) then begin
       evaluate_gettoken,expression,location,type,token,ntoken ;','
       if(token[0] eq byte('[')) then begin
          errstr = evaluate_bracket(level,dims,type,expression,$
                   location,token,ntoken,r1)
          if(errstr ne "OK") then return,errstr
          if(total(dims) eq 0.0) then result = r1 else result = [result,r1]
          goto,top
       endif
       errstr = evaluate_level1(type,expression,location,token,ntoken,r2)
       if(dbug eq 1) then print,'bracket back2 from level1 ',errstr
       if(errstr ne "OK") then return,errstr
       result = [result,r2]
       goto,top
    endif

    if(token[0] eq byte(':')) then begin
       if(n_elements(result) ne 1) then return,"Syntax error0"
       evaluate_gettoken,expression,location,type,token,ntoken ;':'
       if(token[0] eq byte('*')) then begin
          r2 = -1L
          result = [result,r2]
          evaluate_gettoken,expression,location,type,token,ntoken
       endif else begin
          errstr = evaluate_level1(type,expression,location,token,ntoken,r2)
          if(errstr ne "OK") then return,errstr
          if(r2 lt 0) then return,"Bad array specifier: "+string(expression)
          if(n_elements(r2) ne 1) then return,"Syntax error1"
          if(r2 lt result) then return,"bad range: "+string(expression)
          result = lindgen(r2-result+1)+result
       endelse
       goto,top
    endif

    if(type eq 'VARIABLE') then begin
       errstr = evaluate_level1(type,expression,location,token,ntoken,r1)
       if(errstr ne "OK") then return,errstr
       if(total(dims) eq 0.0) then result = r1 else result = [result,r1]
       if(dbug eq 1) then print,'bracket back1 from level1 ',errstr
       goto,top
    endif

;at this point the token should be a number

    if(type eq 'NUMBER' or type eq 'STRING') then begin
       errstr = evaluate_level1(type,expression,location,token,ntoken,r1)
       if(errstr ne "OK") then return,errstr
       if(total(dims) eq 0.0) then result = r1 else result = [result,r1]
       if(dbug eq 1) then print,'bracket back1 from level1 ',errstr
       goto,top
    endif

    if(token[0] eq 0B) then begin
       if(level ne 0) then return,"syntax error2, missing right bracket"
    endif

    print,'unexpected token: '+string(token)
    print,forced_stop3

do_return:
    if(level eq 0) then begin
       n = n_elements(dims)
       new_dims = lonarr(n)
       for i=0,n-1 do new_dims(i) = dims(n-i-1)
       result = reform(result,new_dims)
    endif
    return,errstr
 end

; This function handles any literal numbers, variables, strings,
; or functions.

function evaluate_level6,type,expression,location,token,ntoken,result
forward_function evaluate_level1,evaluate_getvalue,evaluate_strtonum
forward_function evaluate_isdelim,evaluate_bracket,evaluate_do_array
common evaluate_common,vars,dbug

    if(dbug eq 1) then print,'level6 ',type,location,' '+string(token),ntoken
    if(type eq 'STRING') then begin
        result = string(token[0:ntoken-1])
        evaluate_gettoken,expression,location,type,token,ntoken
        if(dbug eq 1) then print,'level6 return4 OK ',result
        return,"OK"
    endif

    c1 = token[0]
    if(c1 eq byte('[')) then begin
       level = 0L
       errstr = evaluate_bracket(level,dims,type,expression,location,$
           token,ntoken,result)
       if(errstr ne "OK") then return,errstr
       return,errstr
    endif

    if(c1 eq byte('(') or c1 eq byte('[')) then begin
        if(c1 eq byte('(')) then c2 = byte(')') else c2 = byte(']')
        if(c1 eq byte('(')) then m = 'parantheses' else m = 'brackets'
        evaluate_gettoken,expression,location,type,token,ntoken
        if(token[0] eq c2) then return,"Missing array specifier"
        errstr = evaluate_level1(type,expression,location,token,ntoken,result)
        if(dbug eq 1) then print,'level6 back1 from level1 ',errstr
        if(errstr ne "OK") then return,errstr
again:
        if(token[0] eq byte(',')) then begin
           evaluate_gettoken,expression,location,type,token,ntoken
           errstr = evaluate_level1(type,expression,location,token,ntoken,r2)
           if(dbug eq 1) then print,'level6 back2 from level1 ',errstr
           if(errstr ne "OK") then return,errstr
           result = [result,r2]
           goto,again
        endif
        if(token[0] eq byte(':')) then begin
           if(n_elements(result) ne 1) then return,"Syntax error3"
           evaluate_gettoken,expression,location,type,token,ntoken
           if(token[0] eq byte('*')) then begin
              r2 = -1L
              result = [result,r2]
              evaluate_gettoken,expression,location,type,token,ntoken
           endif else begin
              errstr = evaluate_level1(type,expression,location,token,ntoken,r2)
              if(errstr ne "OK") then return,errstr
              if(r2 lt 0) then return,"Bad array specifier: "+string(expression)
              if(n_elements(r2) ne 1) then return,"Syntax error4"
              if(r2 lt result) then return,"bad range: "+string(expression)
              result = lindgen(r2-result+1)+result
           endelse
        endif
        if(token[0] ne c2) then return,$
           "Unbalanced "+m+" at '"+string(expression[location:*])+"'"
        evaluate_gettoken,expression,location,type,token,ntoken
        if(dbug eq 1) then print,'level6 return2 OK ',result
        return,"OK"
    endif

    if(type eq 'NUMBER') then begin
        ntemp = 0L
        result = evaluate_strtonum(token,ntemp)
        evaluate_gettoken,expression,location,type,token,ntoken
        if(dbug eq 1) then print,'level6 return5 OK ',result
        return,"OK"
    endif

    if(type eq 'VARIABLE') then begin
       name = string(token)
       errstr = evaluate_do_array(name,type,expression,location,$
           token,ntoken,result)
       if(strmid(errstr,0,16) eq "Unknown variable") then begin
          errstr = evaluate_do_function(name,type,expression,location,$
              token,ntoken,result)
       endif
       return,errstr
    endif

    if(type eq 'DELIMITER') then begin
       return,"OK"
    endif

    if(dbug eq 1) then print,'level6 return8 syntax error in '+string(token)
help,type
help,result
    return,"Syntax error5"
end

; This function handles any unary + or - signs.
;
function evaluate_level5,type,expression,location,token,ntoken,result
forward_function evaluate_level6
common evaluate_common,vars,dbug

    if(dbug eq 1) then print,'level5 ',type,location,' '+string(token),ntoken
    old_token = 0B
    if(type eq 'DELIMITER') then begin
       if(ntoken gt 0) then begin
          if(token[0] eq byte('+') or $
             token[0] eq byte('-')) then begin
             old_token = token[0]
             evaluate_gettoken,expression,location,type,token,ntoken
          endif
       endif
    endif

    errstr = evaluate_level6(type,expression,location,token,ntoken,result)
    if(dbug eq 1) then print,'level5 back1 from level6 ',errstr
    if(errstr ne "OK") then return,errstr
    if(old_token eq byte('-')) then begin
       if(size(result,/tname) eq "STRING") then return,"math on string"
       result = -1B * result
    endif
    if(dbug eq 1) then print,'level5 return2 OK ',result
    return,"OK"
end

; This function handles any "to the power of" operations.
;
function evaluate_level4,type,expression,location,token,ntoken,result
forward_function evaluate_level5
common evaluate_common,vars,dbug

    if(dbug eq 1) then print,'level4 ',type,location,' '+string(token),ntoken
    errstr = evaluate_level5(type,expression,location,token,ntoken,result)
    if(dbug eq 1) then print,'level4 back1 from level5 ',errstr
    if(errstr ne "OK") then return,errstr

    while(token[0] eq byte('^')) do begin
       old_token = token[0]
       evaluate_gettoken,expression,location,type,token,ntoken
       errstr = evaluate_level5(type,expression,location,token,ntoken,value)
       if(dbug eq 1) then print,'level4 back2 from level5 ',errstr
       if(errstr ne "OK") then return,errstr
       if(size(result,/tname) eq "STRING") then return,"math on string"
       result = result ^ value
    endwhile
    if(dbug eq 1) then print,'level4 return2 OK ',result
    return,"OK"
end

; This function handles any multiplication and division.
;
function evaluate_level3,type,expression,location,token,ntoken,result
forward_function evaluate_level4
common evaluate_common,vars,dbug

    if(dbug eq 1) then print,'level3 ',type,location,' '+string(token),ntoken
    errstr = evaluate_level4(type,expression,location,token,ntoken,result)
    if(dbug eq 1) then print,'level3 back1 from level4 ',errstr
    if(errstr ne "OK") then return,errstr

    while(token[0] eq byte('*') or $
          token[0] eq byte('/')) do begin
       if(token[0] eq byte('*')) then mult = 1 else mult = 0
       evaluate_gettoken,expression,location,type,token,ntoken
       errstr = evaluate_level4(type,expression,location,token,ntoken,value)
       if(dbug eq 1) then print,'level3 back2 from level4 ',errstr
       if(errstr ne "OK") then return,errstr
       v = size(value,/tname)
       r = size(result,/tname)
       if(v eq "STRING" and r ne "STRING") then return,"math with string"
       if(v ne "STRING" and r eq "STRING") then return,"math on string"
       if(mult eq 1) then result = result * value
       if(mult eq 0) then begin
          if(value eq 0.0) then return,"Divide by zero"
          result = result / value
       endif
    endwhile
    if(dbug eq 1) then print,'level3 return4 OK ',result
    return,"OK"
end

;
; This function handles any addition and subtraction operations.
; Also, < (minimum) and > (maximum)
;
function evaluate_level2,type,expression,location,token,ntoken,result
forward_function evaluate_level3
common evaluate_common,vars,dbug

    if(dbug eq 1) then print,'level2 ',type,location,' '+string(token),ntoken
    errstr = evaluate_level3(type,expression,location,token,ntoken,result)
    if(dbug eq 1) then print,'level2 back1 from level3 ',errstr
    if (errstr ne "OK") then return,errstr

    while(token[0] eq byte('+') or $
          token[0] eq byte('<') or $
          token[0] eq byte('>') or $
          token[0] eq byte('-')) do begin
       old_token = token[0]
       evaluate_gettoken,expression,location,type,token,ntoken
       errstr = evaluate_level3(type,expression,location,token,ntoken,value)
       if(dbug eq 1) then print,'level2 back2 from level3 ',errstr
       if(errstr ne "OK") then return,errstr
       if(old_token eq byte('+') or old_token eq byte('-')) then begin
          v = size(value,/tname)
          r = size(result,/tname)
          if(v eq "STRING" and r ne "STRING") then return,"math with string"
          if(v ne "STRING" and r eq "STRING") then return,"math on string"
          if(old_token eq byte('+')) then result = result + value
          if(old_token eq byte('-')) then result = result - value
       endif
       if(old_token eq byte('>') or old_token eq byte('<')) then begin
          v = size(value,/tname)
          r = size(result,/tname)
          if(v eq "STRING" and r ne "STRING") then return,"math with string"
          if(v ne "STRING" and r eq "STRING") then return,"math on string"
          if(old_token eq byte('>')) then result = result > value
          if(old_token eq byte('<')) then result = result < value
       endif
    endwhile
    if(dbug eq 1) then print,'level2 return2 OK ',result
    return,"OK"
end

; This function handles any variable assignment operations.
;
function evaluate_level1,type,expression,location,token,ntoken,result
forward_function evaluate_level2,evaluate_getvalue,evaluate_match_delim
common evaluate_common,vars,dbug

    if(dbug eq 1) then print,'level1 ',type,location,' '+string(token),ntoken
    if(type eq 'VARIABLE') then begin
       if(location lt n_elements(expression)) then $
          c = expression[location] else c = 0B
       if(c eq 0B) then begin
          if(evaluate_getvalue(string(token),result) eq 0) then $
             return,"Unknown variable4: "+string(token)
          return,"OK"
       endif
       expr = ''
       fewer_error = 0

;check if the variable to set has an expression indicating one or
;more indices into an array.

       tag = ''
       equals = strpos(expression[location:*],'=')
       if(c ne byte('=') and equals ne -1) then begin
          if(c eq byte('.')) then begin ;struct array with field
             evaluate_gettoken,expression,location,type,token,ntoken ;'.'
             if(type eq 'VARIABLE') then begin
                tag = strupcase(string(token))
             endif else begin
                print,'stopping: type='+type
                print,forced_stop4
             endelse
          endif
          expr = expression[0:strpos(expression,'=')-1]
          if(c eq byte('[') or c eq byte('(')) then begin
             var = strupcase(string(token))
             type1 = type
             token1 = token
             errstr = evaluate_do_array(var,type1,expr,location,$
                token1,ntoken,index1,/left)
             if(strmid(errstr,0,16) eq "fewer subscripts") then begin
                fewer_error = long(strmid(errstr,17))
                errstr = "OK"
             endif
             if(token1[0] eq byte('.')) then begin
                tag = strupcase(string(token1))
             endif
          endif else begin
             evaluate_gettoken,expr,location,type1,token1,ntoken
             errstr = evaluate_level1(type1,expr,location,token1,ntoken,index1)
             if(errstr ne "OK") then return,errstr
             if(token1(0) ne 0B) then begin
                if(token1 eq byte('.')) then begin ;struct array with field
                   tag = strupcase(string(token1))
                endif else begin
                   return,'level1: syntax error6 at '+string(expr)
                endelse
             endif
             if(dbug eq 1) then print,'level1: back from level1 ',errstr
          endelse
          if(errstr ne "OK") then return,errstr
          if(location lt n_elements(expression)) then $
             c = expression[location] else c = 0B
       endif

;handle setting of the variable

       if(c eq byte('=')) then begin
           var = strupcase(string(token))
           evaluate_gettoken,expression,location,type,token,ntoken ;'='
           evaluate_gettoken,expression,location,type,token,ntoken
           if(type eq 'NONE') then begin
               return,'Illegal character at "'+string(token)+'"'
           endif
           if(ntoken eq 0) then begin
               evaluate_clearvar,var
               if(dbug eq 1) then print,'level1 return1 OK ',result
               return,"OK"
           endif

           errstr = evaluate_level2(type,expression,location,token,$
               ntoken,result)
           if(dbug eq 1) then print,'level1 back1 from level2 ',errstr
           if(errstr ne "OK") then return,errstr
           if(n_elements(index1) eq 0 and tag eq '') then begin
              evaluate_setvalue,var,result,errstr
              if(dbug eq 1) then print,'level1 return2 ',errstr,' ',result
              return,errstr
           endif
           known_var = evaluate_getvalue(var,var_value)
           if(known_var eq 1) then begin
              if(size(var_value,/tname) ne 'STRUCT') then begin
                 if(tag ne '') then return,var+" is not a structure"
              endif
           endif
           if(known_var eq 0) then return,'Unknown structure: '+var
           known_tag = evaluate_getvalue(var+tag,var_value)
           if(known_tag eq 0) then return,'Unknown structure tag: '+var+tag
           nv = n_elements(var_value)
           nr = n_elements(result)
           v_tname = size(var_value,/tname)
           r_tname = size(result,/tname)
           if(v_tname eq 'STRUCT') then begin
              if(r_tname ne 'STRUCT') then return,$
                 "conflicting data types setting "+var+tag+$
                 " : "+v_tname+','+r_tname
           endif else begin
              if(r_tname eq 'STRUCT') then return,$
                 "conflicting data types setting "+var+tag+$
                 " : "+v_tname+','+r_tname
           endelse
           ni = n_elements(index1)
           if(ni eq 0) then begin ;if no index was given
              if(nr ne nv) then return,$
                 "size mismatch setting "+var+tag+": "+$
                 strtrim(nv,2)+' vs. '+strtrim(nr,2)
              evaluate_setvalue,var,result,errstr
              if(errstr ne "OK") then return,errstr
           endif else begin

;special case where ':' is in the string and there are two values in
;index1:  they are starting and ending indices.  Give error if
;range is outside the current dimension.

              if(ni eq 2 and strpos(expr,':') ne -1) then begin
                 if(index1(1) lt 0) then index1(1) = nv-1
              endif

;Strict array indexing would require the following check.
;The unstrict behavior is that an array subscripted with an array
;of element numbers will clip the indexes so that each is valid.
;E.g., c=[0,2,4,6] & print,c[[-1,8]] prints:  0   6

              for i=0L,ni-1 do begin
                  ;if(index1(i) lt 0 or index1(i) ge nv) then return,$
                  ;   "subscript "+strtrim(index1(i),2)+$
                  ;   " out of range for variable "+var
              endfor
              if(ni eq 1) then begin ;one index given
                 if(index1+nr gt nv) then return,$
                    "out of range subscript for '"+var+tag+"'; cannot fit "+ $
                    strtrim(nr,2)+" elements starting at "+strtrim(index1,2)
                 if(ni eq 1 and fewer_error gt 1 and nr gt 1) then return,$
                    "array subscript for '"+var+tag+"' has too few values"
                 if(size(index1,/n_dim) eq 1 and ni eq 1 and nr gt 1) then $
                    return,"array subscript for '"+var+tag+"' must have "+$
                       "same size as source expression"
                 if(ni lt nr) then begin
                    var_value(index1:index1+nr-1) = result
                 endif else var_value(index1) = result
              endif else begin
                 if(ni eq 2 and strpos(expr,':') ne -1) then begin
                    if(index1(0) gt index1(1)) then return,$
                       "first subscript "+strtrim(index1(0),2)+$
                       " > second subscript "+strtrim(index1(1),2)+$
                       " for "+var
                    n = index1(1) - index1(0) + 1

;number of elements on both sides of equals must be the same UNLESS
;the number of elements on the right side is 1
;c=lonarr(10) & c[2:4] = [12,23,14] works, c[2:4] = [12,23] gives error
;c[2:4] = 12 sets elements 2:4 all to 12
                    if(n ne nr and nr gt 1) then return,$
                       "array subscript for '"+var+tag+"' must have same "+$
                       "size as source expression"
                    var_value(index1(0):index1(1)) = result
                 endif else begin ;index1 is array of indices
                    if(ni lt nr) then return,"array subscript for'"+$
                       var+"' must be size "+strtrim(nr,2)
                    var_value(index1) = result
                 endelse
              endelse
              evaluate_setvalue,var+tag,var_value,errstr
           endelse
           if(dbug eq 1) then print,'level1 return3 ',errstr,' ',result
           return,errstr
       endif
    endif
    errstr = evaluate_level2(type,expression,location,token,ntoken,result)
    if(dbug eq 1) then print,'level1 back2 from level2 ',errstr
    if(dbug eq 1) then print,'level1 return4 ',errstr
    return,errstr
end

function evaluate_strchar,expr,character
index = strpos(expr,character)
if(index eq -1) then return,-1L
temp = byte(expr)
dquote = 0L
squote = 0L
for i=0L, n_elements(temp)-1 do begin
    if(temp[i] eq byte(character)) then begin
       if(dquote eq 0 and squote eq 0) then return,i
    endif
    if(temp[i] eq byte('"')) then begin
       if(dquote eq 0) then dquote = 1L else dquote = 0L
    endif
    if(temp[i] eq byte("'")) then begin
       if(squote eq 0) then squote = 1L else squote = 0L
    endif
endfor
return,-1L
end

; This function evaluates a string expression like the function execute.
; Many expressions are handled but there are many restrictions.
; This function works best in IDL version 6.2 or greater where variables
; can be set in the procesure/function that calls evaluate.  
; Prior to 6.2, there was no way to set a variable without passing
; it to evaluate_setvalue first (see procedure evaluate_test).
; Currently, the result of the expression can be a float, long, or string.
; One-dimensional arrays are handled as are simple structures.
; System variables can be created and set, but known system variables can
; not be set unless the result is exactly the same type.
;
;Keyword debug has several values:
;  0 - nodebug messages (default)
;  1 - verbose debug messages (used for debugging)
; -1 - just print the input expression
; any other - print the input expression and the return string value

function evaluate,expr_in,result,debug=debug,quiet=quiet
forward_function evaluate_level1,evaluate_strchar
common evaluate_common,vars,dbug
common evaluate_test_common,t,d,aa

;Look for & which separates commands in IDL.

if(expr_in eq '') then return,'OK'
;remove idl comment
expr_copy = strtrim(expr_in,2)
semi = evaluate_strchar(expr_copy,';')
if(semi ne -1) then expr_copy = strmid(expr_copy,0,semi)
if(keyword_set(debug)) then dbug = debug else dbug = 0
if(dbug lt 0) then begin
   if(dbug eq -1) then print,'using execute function:
   print,expr_in
   if(dbug eq -1) then begin
      a = execute(expr_in)
   endif
endif
expr = strtrim(expr_copy,2)
index = -1L
start = 0L
loop:
ampersand = evaluate_strchar(expr,'&')
if(ampersand ne -1) then begin 
   if(ampersand eq strlen(expr)-1) then begin
      expr_copy = strmid(expr_copy,0,strlen(expr_copy)-1)
      expr = strmid(expr,0,strlen(expr)-1)
      goto,loop_done
   endif
   if(index(0) eq -1) then index = ampersand $
   else index = [index,ampersand+start]
   expr = strmid(expr,ampersand+1)
   start = ampersand + start + 1
   expr = strmid(expr_copy,start)
   if(expr gt '') then goto,loop
endif
loop_done:
if(index(0) ne -1) then begin
   expr = strarr(n_elements(index)+1)
   start = 0L
   for i=0L,n_elements(index)-1 do begin
       expr(i) = strtrim(strmid(expr_copy,start,index(i)-start-1),2)
       start = index(i) + 1
   endfor
   expr(i) = strtrim(strmid(expr_copy,start),2)
endif

for i=0, n_elements(expr)-1 do begin
   if(n_elements(result) eq 0) then result = 0.0
   if(n_elements(expr(i)) eq 0) then goto,empty
   if(expr(i) eq '') then goto,empty

   if(dbug gt 0) then print,expr(i)
   expression = byte(expr(i))
   location = 0L

   evaluate_gettoken,expression,location,type,token,ntoken
   if(ntoken eq 0) then begin
      if(expression[location-1] eq byte('.')) then begin
         errstr = "Syntax error7"
      endif else begin
         errstr = "Empty expression"
      endelse
      goto,done
   endif
   errstr = evaluate_level1(type,expression,location,token,ntoken,result)
   if(dbug eq 1) then print,'evaluate: back from level1 ',errstr
   if(errstr ne "OK") then goto,done
   if(location lt n_elements(expression) or ntoken gt 0) then begin
        if(ntoken eq 0) then begin
           errstr = "Syntax error8"
           goto,done
        endif
        if(token[0] eq byte(')') or token[0] eq byte('(')) then begin
           errstr = "Unbalanced parenthesis3"
           goto,done
        endif
        if(dbug gt 0) then $
           print,'OK returned but token is ',token[0],'='+string(token)
        if(location lt n_elements(expression)) then $
           errstr = "Syntax error9 at '"+string(expression[location:*])+"'" $
        else errstr = "Syntax error9"
        if(not keyword_set(quiet)) then begin
           print,forced_stop5
        endif
    endif
endfor
errstr = "OK"

done:
if(dbug gt 0) then print,'*** '+errstr
if((errstr ne "OK") and (not keyword_set(quiet))) then print,'*** '+errstr
return,errstr

empty:
reterr = "Empty expression"
goto,done

end

function evaluate_f1,a1
a1=20
return,1
end

function evaluate_f2,a1,a2
a1=20
a2=40
return,1
end

pro evaluate_test_arrays
forward_function evaluate

if(float(!version.release) lt '6.2') then return

;examples from "Building IDL Applications: Arrays"
;set single element of array
a = evaluate("arrOne = [1,2,3,4,5] & arrOne[2] = 9",debug=-1)
print,arrOne

;array is inserted starting at given element
a = evaluate("arrOne = [1,2,3,4,5] & arrTwo = [11,12] & arrOne[1] = arrTwo",debug=-1)
print,arrOne

;cannot insert if no room in array
a = evaluate("arrOne = [1,2,3,4,5] & arrTwo = [11,12] & arrOne[4] = arrTwo",debug=-1)  ;gives error

;set given elements to scalar value, note subscript is an array
a = evaluate("arrOne = [1,2,3,4,5] & arrOne[[2,4]] = 0",debug=-1)
print,arrOne

;whereas case with no square brackets around numbers gives an error
;because this is interpreted as two dimensions
a = evaluate("arrOne = [1,2,3,4,5] & arrOne[2,4] = 0",debug=-1)
print,arrOne

;elements of expression are stored in specified elements of variable
a = evaluate("arrOne = [1,2,3,4,5] & arrOne[[0,2]] = [111,333]",debug=-1)
print,arrOne

;if subscripts to variable are out of range, then the subscripts are clipped
a = evaluate("arrOne = [1,2,3,4,5] & arrOne[[-1,2]] = [111,333]",debug=-1)
print,arrOne

;this gives an error because there are too many expression values
a = evaluate("arrOne = [1,2,3,4,5] & arrOne[[0,2]] = [111,333,444]",debug=-1)
print,arrOne

;this should create a 3 dimensional array: 3,2,2
a = evaluate("aa=[[[2,3,4],[5,6,7]],[[8,9,10],[11,12,13]]]",debug=-1)
print,aa

;then try these expressions
a = evaluate("aa=[[[2,3,4],[5,6,7]],[[8,9,10],[11,12,13]]]")
a = evaluate("aa[0]=30",debug=-1)            ;sets element 0 of 1-dim array
a = evaluate("print,'answer: aa[0]',aa[0]")

a = evaluate("aa=[[[2,3,4],[5,6,7]],[[8,9,10],[11,12,13]]]")
a = evaluate("aa[5:6]=[31,32]",debug=-1)     ;sets elments 5:6 of 1-dim array
a = evaluate("print,'answer: aa[5:6]',aa[5:6]")

a = evaluate("aa=[[[2,3,4],[5,6,7]],[[8,9,10],[11,12,13]]]")
a = evaluate("aa[1:2,1]=[33,34]",debug=-1)   ;sets aa[1:2,1,0]
a = evaluate("print,'answer: aa[1:2,1]',aa[1:2,1]")

a = evaluate("aa=[[[2,3,4],[5,6,7]],[[8,9,10],[11,12,13]]]")
a = evaluate("aa[1:2,1,1]=[35,36]",debug=-1) ;sets aa[1:2,1,1]
a = evaluate("print,'answer: aa[1:2,1,1]',aa[1:2,1,1]")

a = evaluate("aa=[[[2,3,4],[5,6,7]],[[8,9,10],[11,12,13]]]")
a = evaluate("aa[4]=[37,38]",debug=-1)       ;quietly sets two values
a = evaluate("print,'answer: aa[4:5]',aa[4:5]")

a = evaluate("aa=[[[2,3,4],[5,6,7]],[[8,9,10],[11,12,13]]]")
a = evaluate("aa[[4]]=[40]",debug=-1)        ;sets element 4 of 1-dim array
a = evaluate("print,'answer: aa[4]',aa[4]")

a = evaluate("aa=[[[2,3,4],[5,6,7]],[[8,9,10],[11,12,13]]]")
a = evaluate("aa[2,1,1]=34",debug=-1)        ;sets one element of array
a = evaluate("print,'answer: aa[2,1,1]',aa[2,1,1]")

a = evaluate("aa=[[[2,3,4],[5,6,7]],[[8,9,10],[11,12,13]]]")
a = evaluate("aa[[2,1,1]]=44",debug=-1)      ;sets two elements of array: 1&2
a = evaluate("print,'answer: aa[[2,1,1]]',aa[[2,1,1]]")

a = evaluate("aa=[[[2,3,4],[5,6,7]],[[8,9,10],[11,12,13]]]")
a = evaluate("aa[[-1,20]]=[54,55]",debug=-1) ;quietly clips indexes
a = evaluate("print,'answer: aa[[-1,20]]',aa[[-1,20]]")

a = evaluate("aa=[[[2,3,4],[5,6,7]],[[8,9,10],[11,12,13]]]")
a = evaluate("aa[*]=58",debug=-1)
a = evaluate("print,'answer: aa' & print,aa")

;these should give errors
;Subscript range values of the form low:high must be >= 0, < size, with low <= high: AA
print
print,'should give error: subscript out of range
a = evaluate("aa[5:6,1]=[61,62]",debug=-1)

;Out of range subscript encountered: AA
print
print,'should give error: out of range subscript
a = evaluate("aa[2,1]=[63,64]",debug=-1)

;Out of range subscript encountered: AA
print
print,'should give error: out of range subscript
a = evaluate("aa[2,1,0]=[65,66]",debug=-1)

;Array subscript for AA must have same size as source expression
print
print,'should give error: Array subscript must have same size as source
a = evaluate("aa[[4]]=[67,68]",debug=-1)

;Attempt to subscript AA with <INT      (      -1)> is out of range
print
print,'should give error: subscript -1 out of range
a = evaluate("aa[-1,20]=70",debug=-1)

;try setting array of strings
a = evaluate('BB=["Surface","Contour","Image","ZvsX","ZvsY","Grid"]',debug=-1)
help,bb
for i=0,n_elements(bb)-1 do print,bb(i)

end

pro evaluate_test
forward_function evaluate

print,"variable t is defined to be a float array:  t = [0.5,1.5]"
if(float(!version.release) lt '6.2') then begin
   t = [0.5,1.5]
   print,"assign internal variable t so it can be used in an expression"
   print,"with the following:  evaluate_setvalue,'t',t,errstr"
   evaluate_setvalue,'t',t,errstr
endif else begin
   a=evaluate('t = [0.5,1.5]',debug=-1)
   help,t
   print,t
endelse
print,"variable d is defined to be a float array:  d = [2.0,3.0]"
if(float(!version.release) lt '6.2') then begin
   d = [2.0,3.0]
   print,"assign internal variable d so it can be used in an expression"
   print,"with the following:  evaluate_setvalue,'d',d,errstr"
   evaluate_setvalue,'d',d,errstr
endif else begin
   a=evaluate('d = [2.0,3.0]',debug=-1)
   help,d
   print,d
endelse
expr = "t = 2 * t & c = cos(d*t)"
if(float(!version.release) lt '6.2') then begin
   a=evaluate(expr,c,debug=-1)
   help,c
   print,'c=',c
   print,"get the value of t: a=evaluate_getvalue('t',t)"
   a=evaluate_getvalue('t',t)
   help,t
   print,'t=',t
   print,"get the value of d: a=evaluate_getvalue('d',d)"
   a=evaluate_getvalue('d',d)
   help,d
   print,'d=',d
endif else begin
   a=evaluate(expr,debug=-1)
   help,c
   print,'c=',c
   help,t
   print,'t=',t
   help,d
   print,'d=',d
   print,''
   a=evaluate("var = 900L",debug=-1)
   help,var
   print,var
endelse
end