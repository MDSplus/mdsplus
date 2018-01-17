/* 
/   This function queries the SQL code run database to return the last entry for 
/   a given EFIT run, shot number, and runtag.  
/
/   Note that this is an initial draft, so there are a few assumptions:
/
/	1) The user has connected to the RDB via DBLOGIN()
/	2) The query skips runs marked as deleted
/	3) The query provides the TOP 1 result ordered by idx 
/
/   Created: 20160811, Sean Flanagan
/
/   Modification History:
/
*/

PUBLIC FUN FINDEFIT_CODERUNDB (IN _shot, INOUT _tag, OUT _tree, OUT _runid, OUT _runby)
{

   _tree = "";
   _runid = "";
   _runby = "";
  
   _sql_expr = "select top 1 tree,run_id,run_by from plasmas where code_name='efit'"; 
   _sql_expr = _sql_expr // " and experiment='d3d'";
   _sql_expr = _sql_expr // " and shot="//ADJUSTL(_shot);
   _sql_expr = _sql_expr // " and runtag='"//_tag//"'";
   _sql_expr = _sql_expr // " and deleted!=1";
   _sql_expr = _sql_expr // " order by idx desc";
   write (*,_sql_expr);

   DSQL("USE code_rundb");
   DSQL(_sql_expr,_tree,_runid,_runby);
   write (*,_tree,_runid,_runby);

   return (1);

}
