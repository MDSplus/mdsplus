; +
; set_database.pro
;
; PURPOSE:
; 	routine to establish a database connection.
;
; FORMAT:
;	SET_DATABASE, name [,/reset] [,/debug]
; or
;       SET_DATABASE, file=db-file-name [,/reset] [,/debug]
;
; ARGUMENTS
;   name - name of the database.  This name is used to find a file
;          which describes how to access this database.  The file
;          is called name.sybase_login and should be in:
;          WIN32   - %USERPROFILE%
;          UNIXes  - $HOME
;          OpenVMS - SYS$LOGIN
;          (It should be readably only by the owner!)
;
; KEYWORDS
;   reset - if specified a new database definition file is created for
;           this database name, even if it already exists
;   debug - if present extra debugging information is displayed
;   file  - if present specifies the full filename of the database
;           definition file.  For example:  
;           set_database, file='/home/john_doe/logbook.sybase_login'
;
; DESCRIPTION
;  1. set_database first calls DBINFO to translate the name specified by its input
;  argument into an MDSHOST, DBHOST, DBNAME, USERNAME, and PASSWORD.  If an MDSHOST
;  is present, then it attempts to connect to an mdsip server running on that host,
;  otherwise it uses the local tdi interpreter.
;
;  2.  It then evaluates dblogin with the dbhost, username, and password.
;
;  3.  Finally it executes DSQL('use DBNAME') and DSQL('set text_size 8192')
;
;  DBINFO  will create a file  which contains these 5 pieces of information as
;  described above under 'name' if it does not exist or /RESET is specified'
;
;  Josh Stillerman 2/14/02
;  10/8/02  - added file keyword
;

@mdsconnect.pro

function NextSib, w
  ans = widget_info(w, /sibling)
  if (ans eq 0) then begin
    par = widget_info(w, /parent)
    ans = widget_info(par, /child)
  endif
  return, ans
end

function FindNextText, w
  ans = NextSib(w)
  while (ans ne w) do begin
    if (widget_info(ans, /type) eq 3) then break
    ans = NextSib(ans)
  endwhile
  return, ans
end

function TabAdvance, ev
  if (tag_names(ev, /STRUCTURE_NAME) eq 'WIDGET_TEXT_CH') then begin
    if (ev.ch eq 9b) then begin
      id = FindNextText(ev.id)
      widget_control, id, /input_focus
      return, 0
    endif
  endif
  return, ev
end

;
; event routine to support 'password' text widgets.
; whatever is typed, echo '*'
;
PRO pwd_event, event

; This event handler for the password widget. Asterisks are returned.


; Deal with simple one-character insertion events.

IF event.type EQ 0 THEN BEGIN

      ; If it is a tab then move the focus to the next widget
   IF event.ch EQ 9 THEN BEGIN
      id = FindNextText(event.id)
      Widget_Control, id, /input_focus
      RETURN
   ENDIF

   If event.ch eq 10 then begin
     return
   endif

      ; Get the current text in the widget and find its length.

   Widget_Control, event.id, Get_Value=text
   text = text[0]
   length = StrLen(text)

      ; Store the password and return asterisks.

;   selection = Widget_Info(event.id, /Text_Select)
   Widget_Control, event.id, set_value=replicate('*', length), Set_Text_Select=event.offset + 1

      ; Store the password.

   Widget_Control, event.id, Get_UValue=ptr
   IF ptr EQ "" THEN ptr=String(event.ch) ELSE $
      ptr = ptr + String(event.ch)

ENDIF ; of insertion event

   ; Deal with deletion events.

IF event.type EQ 2 THEN BEGIN

      ; Get the current text password.

   Widget_Control, event.id, Get_UValue=ptr
   text = ptr
   length = StrLen(text)

      ; Put it back with the deletion subtracted.

   ptr = StrMid(text, 0, length-event.length)
   passwordLen = StrLen(ptr)
   if (passwordLen gt 0) then begin
     Widget_Control, event.id, Set_Value=Replicate('*', passwordLen)

      ; Reset the text insertion point in the text widget.
     Widget_Control, event.id, Set_Text_Select=event.offset
   endif else begin
	 Widget_control, event.id, set_value=''
   endelse
ENDIF

Widget_Control, event.id, Set_UValue=ptr

END
;
; Event routine for CreateNewDBFILE widget procedure
;
pro CreateDBFile_event, ev
 case (tag_names(ev, /STRUCTURE_NAME)) of
  "WIDGET_BUTTON": begin
        WIDGET_CONTROL, ev.top, get_uvalue=ptr
        WIDGET_CONTROL, ev.id, get_value=button
        case (button) of
        "Cancel":  widget_control, ev.top , /destroy
		"Ok": begin
		    err = 0
        	widget_control, (*ptr).mds_host_w, get_value=str
        	(*ptr).mds_host = strtrim(str[0],2)
        	widget_control, (*ptr).db_host_w, get_value=str
        	(*ptr).db_host = strtrim(str[0], 2)
        	widget_control, (*ptr).db_name_w, get_value=str
        	(*ptr).db_name = strtrim(str[0], 2)
        	widget_control, (*ptr).username_w, get_value=str
        	(*ptr).username = str[0]
        	widget_control, (*ptr).password1_w, get_uvalue=str
        	(*ptr).password1 = str
        	widget_control, (*ptr).password2_w, get_uvalue=str
        	(*ptr).password2 = str
        	if ((*ptr).password1 ne (*ptr).password2) then begin
        	  x = dialog_message("Passwords do not match", /error)
        	  err = 1
        	endif
        	if (strlen((*ptr).db_host) eq 0) then begin
        	  x = dialog_message("Database Host name must be specified", /error)
        	  err = 1
        	endif
	        	if (strlen((*ptr).db_name) eq 0) then begin
        	  x = dialog_message("Database name must be specified", /error)
        	  err = 1
        	endif
        	if (strlen((*ptr).username) eq 0) then begin
        	  x = dialog_message("Database  Username must be specified", /error)
        	  err = 1
        	endif
        	if (strlen((*ptr).password1) eq 0) then begin
        	  x = dialog_message("Database password must be specified", /error)
        	  err = 1
        	endif
        	if (not err) then begin
        	  (*ptr).status = 1
        	  widget_control, ev.top, /destroy
        	endif
		end
        "Reset": begin
        	widget_control, (*ptr).mds_host_w, set_value=''
        	widget_control, (*ptr).db_host_w, set_value=''
        	widget_control, (*ptr).db_name_w, set_value=''
        	widget_control, (*ptr).username_w, set_value=''
        	widget_control, (*ptr).password1_w, set_value=''
        	widget_control, (*ptr).password1_w, set_uvalue=''
        	widget_control, (*ptr).password2_w, set_value=''
        	widget_control, (*ptr).password2_w, set_uvalue=''
        end
        "Help": begin
           XDisplayFile, "SetDatabase.Help", $
           text = $
           ['set_database - New database definition help', $
            ' ', $
            'Enter information about how to connect to this database', $
            ' ', $
            'MDS Host - If you are using mdsip to connect to the database', $
            '           with no local client library (freetds, sybase, MS etc...)', $
            '           enter the ipname of your database server.  Leave Blank' , $
            '           for "traditional" network database library access.', $
            'Database Host - The "DBLIB" database host name.', $
            '           This is the name that the MDS Host (above) uses to refer', $
            '           to the database server.', $
            'Database Name - The name of the database on this server.', $
            'Username - The username to log into the database with.', $
            'Password - The password to use to log into the database.', $
            '  ', $
            "  If you have questions about how to fill these in Ask your System's Manager"]

        end
       else:
       endcase
  end
;  "WIDGET_TEXT_CH" : begin
;  		IF ev.type EQ 0 THEN BEGIN
;
;      ; If it is a tab then move the focus to the next widget
;   			IF ev.ch EQ 9B THEN BEGIN
;      			id = FindNextText(ev.id)
;      			Widget_Control, id, /input_focus
;      			RETURN
;   			ENDIF
;		endif
;  end
  else:
  endcase
end
;
; Routine to prompt the user to create a new database definition file
;
; Widget based !
;
function CreateNewDBFile,  file, name, mdshost, dbhost, dbname, username, password
  b = widget_base(title="New Database Definition", /column)
  r = widget_base(b, row=6)
  mdshost=''
  dbhost=''
  username=getenv('LOGNAME')
  l = widget_label(r, value="MDS Host (blank for local)")
  mds_host_w = widget_text(r, xsize=20,/editable, /all_events, event_func='TabAdvance', value=mdshost)
  l = widget_label(r, value="Database server Name")
  db_host_w = widget_text(r, xsize=20,/editable, /all_events, event_func='TabAdvance', value=dbhost)
  l = widget_label(r, value="Database Name")
  if (n_elements(dbname) eq 0) then dbname = ''
  db_name_w = widget_text(r, xsize=20, /editable,/all_events, event_func='TabAdvance', value=dbname)
  l = widget_label(r, value="Database USERNAME")
  username_w = widget_text(r, xsize=20,/editable, /all_events, event_func='TabAdvance', value=username)
  l = widget_label(r, value="Database Password")
  password = ''
  for i=1, strlen(password) do $
    pass=pass+'*'
  password1_w = widget_text(r, xsize=20, uvalue=password, /all_events, event_pro='pwd_event',/editable, value=pass)
  l = widget_label(r, value="Re-ennter Password")
  password2_w = widget_text(r, xsize=20, uvalue=password, /all_events, event_pro='pwd_event', /editable, value=pass)
  r = widget_base(b, /row)
  bu = widget_button(r, value='Ok')
  bu = widget_button(r, value='Reset')
  bu = widget_button(r, value='Help')
  bu = widget_button(r, value='Cancel')
  ctx = {mds_host_w:mds_host_w, db_host_w:db_host_w, db_name_w:db_name_w, username_w:username_w, $
  		 password1_w:password1_w, password2_w:password2_w, $
  		 mds_host:mdshost, db_host:dbhost, db_name:dbname, username:username, $
  		 password1:password, password2:password, status:0}
  ptr = PTR_NEW(ctx)
  widget_control, b, /realize, set_uvalue=ptr
  XManager, 'CreateDBFile', b
  ctx = *ptr
  ptr_free, ptr
  if (ctx.status) then begin
    help, file
    openw, lun, file, /get_lun, err=err
    if (err ne 0) then begin
      x = dialog_message("SetDatabase - error creating file "+file)
      return, 0
    endif
    catch, err
    if (err ne 0) then begin
       x = dialog_message("SetDatabase - error writing to file "+file)
       free_lun, lun
       return, 0
    endif
    printf, lun, ctx.mds_host
    printf, lun, ctx.db_host
    printf, lun, ctx.db_name
    printf, lun, ctx.username
    printf, lun, ctx. password1
    close, lun
    free_lun, lun
    file_chmod, file, '0600'o
    return, 1
  endif else $
    return, 0
end

function ReadDBInfo, file, name, mdshost, dbhost, dbname, username, password
  OPENR,lun,file,/get_lun, error = err
  if (err ne 0) then return, 0
  catch, err
  if (err ne 0) then begin
      catch, /cancel
      Message, "Error reading database definition file "+file+'.  SetDatabase, name, /reset to redefine', /quiet
      return, 0
  endif
  mdshost=''
  dbhost=''
  dbname=''
  username=''
  password=''
  readf, lun, mdshost
  readf, lun, dbhost
  readf, lun, dbname
  readf, lun, username
  readf, lun, password
  close, lun
  free_lun, lun
  return , 1
end

;
;  DBInfo
;
;  routine to return the connection and login information for a database given
;  its name.
;
;  Looks for a file called 'name'.sybase_login in the user's login directory and
;  reads:
;      mdshost - name of machine to mdsconnect to for mdsip based access (or blank)
;      dbhost - name of the database server is known by the mdshost
;      dbname - name of the database to connect to
;      username - login name to use
;      password - password to use
;
; if the file does not exist, or /reset is specified then a new file
; is created and the procedure recurses.
;
function DBInfo, name, mdshost, dbhost, dbname, username, password, reset=reset, file=file
  using_dfile=0
  if n_elements(file) eq 0 then begin
      dfile = ''
      case !version.os of
          'Win32'	: begin
              envar = 'userprofile'
              dpath = 'C:\program files\mdsplus\'
          end
          else		: begin
              envar = 'HOME' 
              dpath = '/etc/sybase/'
          end
      endcase
      path = getenv(envar)
      if (strlen(path) ne 0) then begin
          file=path+'/'+strtrim(name,2)+'.sybase_login'
          lfile = path+'/'+strlowcase(strtrim(name,2))+'.sybase_login'
      endif else begin
          file=''
          lfile=''
      endelse

      dfile = dpath+strlowcase(strtrim(name,2))+'.sybase_login'
  endif
  status = 0
  if (strlen(file) gt 0) then begin
      status = ReadDBInfo(file,name, mdshost, dbhost, dbname, username, password )
  end

  if (not status) then $
    if (strlen(lfile) gt 0) then begin
      status = ReadDBInfo(lfile,name, mdshost, dbhost, dbname, username, password )
  endif

 if (not status) then $
    if (strlen(dfile) gt 0) then begin
      status = ReadDBInfo(dfile,name, mdshost, dbhost, dbname, username, password )
      if (status) then begin
          spawn, 'whoami', username
          username = username[0]
      endif
  endif
  if (keyword_set(reset) or (not status)) then begin
      if (CreateNewDBFile(lfile, name, mdshost, dbhost, dbname, username, password)) then $
        return, dbinfo(name, mdshost, dbhost, dbname, username, password) $
      else $
        return, 0
  endif
  return, status
  
end
    
pro MDSDbDisconnect
   defsysv,'!MDSDB_SOCKET',exists=old_sock
   if (not old_sock) then begin
       defsysv, '!MDSDB_SOCKET', -1
       defsysv, '!MDSDB_HOST', ''
   endif
   if !MDSDB_SOCKET ne -1 then $
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


pro set_database, dbname, status=status, quiet=quiet,debug=debug, reset=reset, file=file
  debug = keyword_set(debug)
  reset = keyword_set(reset)
  status = dbinfo(dbname, mdshost, host, name, user, pass, reset=reset, file=file)
  if (status eq 0) then $
    return
  MDSDbconnect, mdshost
  status = mdsvalue("dblogin($, $, $)", host, user, pass, socket=!MDSDB_SOCKET)
  if (not status) then begin
      if not (keyword_set(quiet)) then begin
          Message, mdsvalue('dsc:MdsSql->GetDBMsgText_dsc()'), /continue
          Message, "Error logging on to DbHost "+host, /continue
      endif else begin
          Message, "Error logging on to DbHost "+host, /continue, /noprint
      endelse
      return
  endif
  status = dsql('USE '+name, debug=debug)
  if (status ne 0) then begin
      if not (keyword_set(quiet)) then begin
          Message, "Error attaching to database "+name, /continue
      endif else begin
          Message, "Error attaching to database "+name, /continue, /noprint
      endelse
      return
  endif
  status = dsql('SET TEXTSIZE 8192', debug=debug)
  if (status ne 0) then begin
      if not (keyword_set(quiet)) then begin
          Message, "Error on setup query to "+name, /continue
      endif else begin
          Message, "Error on initial query to "+name, /continue, /noprint
      endelse
      return
  endif
  status = 1

end
