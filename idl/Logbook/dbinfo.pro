;++
;  name: DBINFO
;
;  format:  status = dbinfo(dbname, host, conn, dbtype, dbuser, dbpass)
;
;  routine to return the connection information for the current user to
;  a database given its name.  This routine should be site customized to
;  reflect the local environment.
;
;  at CMOD: if translation of dbname does not have a ',' in it then
;              this is an RDB database with no host, user, or  password ness
;           else
;              this is a sybase/sqlserver database
;              host name preceeds the comma, database name follows
;              if there is a file sys$login:host.sybase_login and
;                 it contains exactly 2 lines then
;                 read the user and pass from the file
;                 on error set it to the default
;
; input arguments:
;	dbname - name of the database to return info for
;
; output arguments:
;	host - host name to connect to
;       conn - connection string to send to the host
;       dbtype - either RDB or SYBASE
;       dbuser - username to send to the host
;       dbpass - password to send to the host
;
; Josh Stillerman 5/7/99
;
;  This routine returns the connection information for the
;  local SQLSERVER database server.  To use NT authorization
;  return empty strings for the dbuser, and dbpass arguments.
;
;  An environment variable SYBASE_HOST must contain the name of
;  your SQLSERVER database server.
;
function dbinfo, dbname, host, conn, dbtype, dbuser, dbpass
  SQL
  dbtype ="SYBASE"
  host=getenv("SYBASE_HOST")
  if (strlen(host) eq 0) then host = "RED"
  catch, err
  if (err ne 0) then begin
    dbuser=getenv("USER")
    dbpass="PFCWORLD"
    return, 1
  endif
  OPENR,1,getenv('HOME')+'/'+host+".sybase_login"
  dbuser=''
  dbpass=''
  readf,1,dbuser
  readf,1,dbpass
  close, 1
  conn = ["USE "+dbname, "SET TEXTSIZE 8192"]
  return, 1
end
