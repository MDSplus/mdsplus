/*	FINDATREE.FUN
; PURPOSE:	DETERMINE THE TREE NAME GIVEN A POINTNAME BY SEARCHING A TABLE 
;		ON THE POSTGRESQL SERVER, DELPHI.GAT.COM. 
;               AN ARRAY OF STRINGS WITH 4 ELEMENTS PER MATCH OF THE FORM
;               [ name, tree, path, errpath, name, tree, path, errpath, .. ]
;               IS RETURNED ON NORMAL TERMINATION, WHERE name=INPUT NAME, tree=MDS+ TREE
;               OR PTDATA DATASOURC(E.G. .MAG), path=MDS+ PATH, err_path=MDS+ PATH TO ERROR BAR.
;               0 IS RETURNED IF NO MATCHES ARE FOUND.
;               IF ERRORS OCCUR THE ERROR STRING IS RETURNED.
; CATEGORY:	Inquiry
; CALLING SEQUENCE: string-array = FINDATREE( name-string, [, table_path-string  ] )
; INPUTS:	
;    name (STRING) LOGICAl POINTNAME, E.G. IP,  CORRESPONDING TO MDS+ MINPATH AND TREE.
;           FOR D3D MAY ALSO CORRESPOND TO PTDATA POINTAME AND
;           PTDATA STORAGE BRANCH, E.G. .MAG .
;           EFIT NAMES (IN EFIT01 TREE) CAN HAVE _XX APPENDED TO SELECT
;           WHICH EFIT BRANCH, E.G. denr0_02.
;           WILD CARDS: * OR % MATCHES ANY LENGTH, ~ MATCHES ONE CHARACTER.
;
; OPTIONAL INPUT PARAMETERS:
;    table_path (STRING) DEFAULT = 'SIGNAME_D3D',  SINGLE TABLE NAME OR SEVERAL PATH DELIMINATOR
;               SEPARATED TABLE NAMES TO SEARCH FOR MATCHES TO name.
;               PATH DELIMINATOR CAN BE EITHER : OR #. SEARCH WILL BE DONE
;               FOR ALL TABLES IN PATH FROM LEFT TO RIGHT AND RETURN ALL MATCHING
;               RESULTS FOR ALL TABLES UNLESS THE PATH DELIMINATOR FOLLOWING A TABLE
;               NAME IS # IN WHICH CASE THE SEACH WILL NOT CONTINUE IF ANY RESULTS
;               HAVE ALREADY BEEN FOUND. 
;
; OUTPUTS: -- 	
; OPTIONAL OUTPUT PARAMETERS: --
; COMMON BLOCKS: --
; SIDE EFFECTS: --
; RESTRICTIONS:	Only works for General Atomics MDS+ servers inc postgreSQL server is behind GA firewall
; PROCEDURE:    --
; MODIFICATION HISTORY:
;       Tom Osborne osborne@fusion.gat.com  22-April-2005. Initial coding.
*/
FUN PUBLIC findatree( IN _point, OPTIONAL IN _table ) {

  if ( not PRESENT(_table)   ) _table = 'SIGNAME_D3D'; 
	
  _errmes = ' ';     /* ERROR STRING */
  _result = [' '];   /* STRING ARRAY FOR RESULTS */

  _stat = libftree->findtree( _point, _table, DESCR(_result), DESCR(_errmes) );
  
  if (     _stat == 1 ) return( _result ); /* RESULT OK */
  else if( _stat == 3 ) return( 0       ); /* NO ROWS RETURNED */
  else                  return( _errmes ); /* AN ERROR OCCURRED */
} 
