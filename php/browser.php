<?php
if (!extension_loaded('mdsplus')){dl('mdsplus.so');}

if ($_GET["nid"] == "")
{
?>
<html>
<FORM METHOD="get" ACTION="<?php echo $SCRIPT_NAME ?>">
<INPUT TYPE="hidden" name="nid" value="0">
<TABLE>
<TR><TD>Server:</TD><TD><INPUT TYPE="text" NAME="server" size=40 value=<?php echo $_COOKIE["server"] == "" ? 'alcdata.psfc.mit.edu' : $_COOKIE["server"]; ?>></TD></TR>
<TR><TD>Tree:</TD><TD><INPUT TYPE="text" NAME="tree" size=15 value=<?php echo $_COOKIE["tree"] == "" ? 'cmod' : $_COOKIE["tree"]; ?>></TD></TR>
<TR><TD>Shot:</TD><TD><INPUT TYPE="text" NAME="shot" size=15 value=<?php echo $_COOKIE["shot"] == "" ? '-1' : $_COOKIE["shot"]; ?>></TD></TR>
</TABLE>
<INPUT type="submit" value="Go">
</FORM>

<?php
}
else
{
  if ($_GET["nid"] == "0")
  {
    setcookie("server",$_GET["server"],time()+60*60*24*30);
    setcookie("tree",$_GET["tree"],time()+60*60*24*30);
    setcookie("shot",$_GET["shot"],time()+60*60*24*30);
    if ($_GET["server"] == "" || $_GET["tree"] == "")
    {
?>
<html>
You must provide a server and a tree name!
<p>
<FORM METHOD="get" ACTION="<?php echo $SCRIPT_NAME ?>">
<INPUT TYPE="hidden" name="nid" value="0">
<TABLE>
<TR><TD>Server:</TD><TD><INPUT TYPE="text" NAME="server" size=40 value=<?php echo $_COOKIE["server"]; ?>></TD></TR>
<TR><TD>Tree:</TD><TD><INPUT TYPE="text" NAME="tree" size=15 value=<?php echo $_COOKIE["tree"]; ?>></TD></TR>
<TR><TD>Shot:</TD><TD><INPUT TYPE="text" NAME="shot" size=15 value=<?php echo $_COOKIE["shot"]; ?>></TD></TR>
</TABLE>
<INPUT type="submit" value="Go">
</FORM>
<?php
    }
    else
    {
      $server=$_GET["server"];
      $tree=$_GET["tree"];
      $shot=$_GET["shot"];
    }
  }
  else
  {
    $server=$_COOKIE["server"];
    $tree=$_COOKIE["tree"];
    $shot=$_COOKIE["shot"];
  }
  $handle=mdsplus_connect($server);
  mdsplus_open($handle,$tree,$shot);
  $nidin=mdsplus_value($handle,'evaluate(' . $_GET["nid"] . ')');
  $fullpath=mdsplus_value($handle,"getnci($,'FULLPATH')",$nidin);
  $usage_img = array(0 => "structure", 1 => "structure", 2 => "action", 3 => "device", 4 => "dispatch", 5 => "numeric",
                     6 => "signal", 7 => "task", 8 => "text", 9 => "window", 10 => "axis", 11 => "subtree", 12 => "compound");

  printf("<P>%s</P>\n",$fullpath);
?>

<html>
<TABLE border=1>
<TR><TD style="background-color: rgb(102,255,153);"><BOLD>Members</BOLD></TD></TR>
<?php
   if (mdsplus_value($handle,"getnci($,'NUMBER_OF_MEMBERS')",$nidin) > 0)
     {
       $nids=mdsplus_value($handle,"getnci(getnci($,'MEMBER_NIDS'),'NID_NUMBER')",$nidin);
       $count=count($nids);
       for ($i=0;$i<$count;$i++)
       {
	 $nid=$nids[$i];
         $usage=mdsplus_value($handle,"getnci($,'USAGE')",$nid);
	 $name=mdsplus_value($handle,"getnci($,'NODE_NAME')",$nid);
         if (mdsplus_value($handle,"getnci($1,'number_of_children')+getnci($1,'number_of_members')",$nid) > 0)
	   {
	     printf("<TR><TD><a href=\"%s?nid=%s\"><img src=%s>%s</a></TD></TR>\n",$SCRIPT_NAME,$nid,'/browser-icons/' . $usage_img[$usage] . '.gif',$name);
	   }
	 else
	   {
	     printf("<TR><TD><img src=%s>%s</TD></TR>\n",'/browser-icons/' . $usage_img[$usage] . '.gif',$name);
	   }
       }
     }
?>
<TR><TD style="background-color: rgb(102,255,153);"><BOLD>Children</BOLD></TD></TR>
<?php
   if (mdsplus_value($handle,"getnci($,'NUMBER_OF_CHILDREN')",$nidin) > 0)
     {
       $nids=mdsplus_value($handle,"getnci(getnci($,'CHILDREN_NIDS'),'NID_NUMBER')",$nidin);
       $count=count($nids);
       for ($i=0;$i<$count;$i++)
       {
	 $nid=$nids[$i];
	 $name=mdsplus_value($handle,"getnci($,'NODE_NAME')",$nid);
         $usage=mdsplus_value($handle,"getnci($,'USAGE')",$nid);
         if (mdsplus_value($handle,"getnci($1,'number_of_children')+getnci($1,'number_of_members')",$nid) > 0)
	   {
	     printf("<TR><TD><a href=\"%s?nid=%s\"><img src=%s>%s</a></TD></TR>\n",$SCRIPT_NAME,$nid,'/browser-icons/' . $usage_img[$usage] . '.gif',$name);
	   }
	 else
	   {
	     printf("<TR><TD><img src=%s>%s</TD></TR>\n",'/browser-icons/' . $usage_img[$usage] . '.gif',$name);
	   }
       }
     }
mdsplus_disconnect($handle);
}
?>
</TABLE>
</html>
