<?php

// Date in the past
header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");

// always modified
header("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT");
 
// HTTP/1.1
header("Cache-Control: no-store, no-cache, must-revalidate");
header("Cache-Control: post-check=0, pre-check=0", false);

// HTTP/1.0
header("Pragma: no-cache");

include("roam-config.php");

if ($_SERVER["SERVER_PORT"] != 443)
{
 header("Location: $uri");
 return;
}

if (array_key_exists('check_access',$_GET))
{
  $result="no";
  ob_start();
  $db = pg_connect($dbinfo);
  $q = pg_query($db,"select uid,primary_uid,gridid from users where gridid='" . $_GET['uname'] . "'");
  if (pg_num_rows($q) > 0)
  {
    $ans=pg_fetch_array($q);
    if ($ans['primary_uid'] != "")
    {
      $uid = $ans['primary_uid'];
      $q = pg_query($db,"select gridid from users where uid=" . $uid);
      $ans=pg_fetch_array($q);
      $gridid=$ans['gridid'];
    }
    else
    {
      $uid = $ans['uid'];
      $gridid = $ans['gridid'];
    }
    $q = pg_query($db,"select auxilliary from resourceauthorizations where resource='" . $_GET['rname'] . 
		  "' and permit='" . $_GET['permission'] . 
		  "' and uid=" . $uid);
    $ans=pg_fetch_array($q);
    $result = (pg_num_rows($q) > 0) ? ("yes\n" . $ans['auxilliary']) : "no";
  }
  $q = pg_query($db,"insert into checklog (rname,permit,gridid,result) values ('" . $_GET['rname'] .
		"','" . $_GET['permission'] . "','" . $gridid . "','" . $result . "')");
  ob_end_clean();
  print $result ;
  return;
}

if (array_key_exists('check_access',$_GET))
{
  $result="no";
  ob_start();
  $db = pg_connect($dbinfo);
  $q = pg_query($db,"select uid,primary_uid,gridid from users where gridid='" . $_GET['uname'] . "'");
  if (pg_num_rows($q) > 0)
  {
    $ans=pg_fetch_array($q);
    if ($ans['primary_uid'] != "")
    {
      $uid = $ans['primary_uid'];
      $q = pg_query($db,"select gridid from users where uid=" . $uid);
      $ans=pg_fetch_array($q);
      $gridid=$ans['gridid'];
    }
    else
    {
      $uid = $ans['uid'];
      $gridid = $ans['gridid'];
    }
    $q = pg_query($db,"select auxilliary from resourceauthorizations where resource='" . $_GET['rname'] . 
		  "' and permit='" . $_GET['permission'] . 
		  "' and uid=" . $uid);
    $ans=pg_fetch_array($q);
    $result = (pg_num_rows($q) > 0) ? ("yes\n" . $ans['auxilliary']) : "no";
  }
  $q = pg_query($db,"insert into checklog (rname,permit,gridid,result) values ('" . $_GET['rname'] .
		"','" . $_GET['permission'] . "','" . $gridid . "','" . $result . "')");
  ob_end_clean();
  print $result ;
  return;
}

function AddUser($db,$user,$query)
{
  $q=pg_query($db,"select * from resourceauthorizations where uid=" . $user['uid'] . " and resource='ROAM' and permit='AddUser'");
  if (pg_num_rows($q) > 0)
  {
    $q=pg_query($db,"select * from users where gridid='" . $query['dn'] . "'");
    if (pg_num_rows($q) > 0)
    {
      print "Error: User with gridid " . $query['dn'] . " already exists\n";
      return;
    }
    $new_name=cn_without_serial($query['dn']);
    $q=pg_query($db,"select * from users");
    while ($ans=pg_fetch_array($q))
    {
      if (cn_without_serial($ans['gridid']) == $new_name)
  	break;
    }
    ob_start();
    if ($ans['gridid'] == "")
    {
      $q=pg_query($db,"insert into users (gridid,email,affiliation,mailing,phone,fax,first_name,last_name) values ('" . $query['dn'] . "','" .
		  $query['email'] . "','" . $query['affiliation'] . "','" . $query['mailing'] . "','" . $query['phone'] . "','" . $query['fax'] . "','" .
		  $query['first_name'] . "','" . $query['last_name'] . "')");
    }
    else
    {
      $q=pg_query($db,"insert into users (gridid,primary_uid) values ('" . $query['dn'] . "'," . (($ans['primary_uid'] == "") ? $ans['uid'] : $ans['primary_uid']) . ")");
    }
    ob_end_clean();
    if ($q == FALSE)
    {
      print "ERROR: Error adding user - database error\n";
    }
    else
    {
      print "SUCCESS: User successfully added\n";
    } 
  }
  else
  {
    print "ERROR: You are not authorized to add users\n";
  }

}

function remove_param($qstring, $param)
{
  $pos=strpos($qstring,$param);
  if ($pos === FALSE)
    return $qstring;
  else
  {
    $endpos=strpos($qstring,"&",$pos);
    if ($endpos === FALSE)
      return substr_replace($qstring,'',$pos);
    else
      return substr_replace($qstring,'',$pos,$endpos-$pos);
  }
  return substr_replace($qstring,$replace_param,strpos($qstring,$param),$size);
}

function cn_without_serial($gridid)
{
  $cn="";
  $tok=strtok($gridid,"/");
  while ($tok)
  {
    if (strncasecmp($tok,"CN=",3) == 0)
    {
      $cn=substr($tok,3);
      break;
    }
    $tok=strtok("/");
  }
  $cn=strrev($cn);
  $len=strspn($cn,"0123456789 ");
  $cn=strrev(substr($cn,$len));
  return $cn;
}

session_start();
$db = pg_connect($dbinfo);

$logged_in=0;

if (strpos($_SERVER['SSL_CLIENT_VERIFY'],'SUCCESS') === FALSE)
{
  if (isset($_GET['logout']))
    $_SESSION['gridid'] = "";
  if ($_SESSION['gridid'] != "")
  {
    $gridid=$_SESSION['gridid'];
    $q_user = pg_query($db,"select * from users where gridid='" . $gridid . "'");
    $user = pg_fetch_array($q_user);
    if ($user['primary_uid'] != "")
    {
      $q_user = pg_query($db,"select * from users where uid=" . $user['primary_uid']);
      $user = pg_fetch_array($q_user);
      $gridid=$user['gridid'];
    }
    $logged_in=1;
  }
  else
  {
    if ($_GET['login'] != "")
    {
      $descriptorspec = array( 0 => array("pipe", "r"), 1 => array("pipe","w"), 2 => array("file","/dev/null","a"));
      $process = proc_open($_SERVER['DOCUMENT_ROOT'] . "/roam-login.sh " . $_GET['uname'], $descriptorspec, $pipes);
      if (is_resource($process))
      {
        fwrite($pipes[0],$_GET['password']);
        fclose($pipes[0]);
        if (!feof($pipes[1]))
	{
	  $gridid=trim(fgets($pipes[1],1024));
	}
        fclose($pipes[1]);
        $status=proc_close($process);
        if ($status == 0)
	{
          $_SESSION['gridid'] = $gridid;
	  $logged_in=1;
	  header("Location: $uri");
	  return;
        }
      }
    }
  /////////////////////////////// No CERT Page ///////////////////////////////////////////////////////////////////////
    include("beginpage.html");
    if ($_GET['login'] != "")
      print "<span style=\"color: rgb(255,0,0); font-weight: bold\">Invalid username/password</span></P >\n";
?>
<H2>Login Please</H2>
<FORM METHOD="GET">
<TABLE>
<TR><TD><B>Username:</B></TD><TD><INPUT TYPE="TEXT" SIZE=20 NAME="uname"></TD></TR>
   <TR><TD><B>Password:</B></TD><TD><INPUT TYPE="PASSWORD" SIZE=20 NAME="password"></TD></TR>
<TABLE><P />
<INPUT TYPE="SUBMIT" VALUE="Log In" NAME="login">
</FORM>
<P />
<?php
    print $login_help;
    include("endpage.html");
  /////////////////////////////// No CERT Page ///////////////////////////////////////////////////////////////////////
    return;
  }
}
else
{
  $gridid=$_SERVER['SSL_CLIENT_S_DN'];
  $q_user = pg_query($db,"select * from users where gridid='" . $gridid . "'");
  $user = pg_fetch_array($q_user);
  if ($user['primary_uid'] != "")
  {
    $q_user = pg_query($db,"select * from users where uid=" . $user['primary_uid']);
    $user = pg_fetch_array($q_user);
    $gridid=$user['gridid'];
  }
}
if (pg_num_rows($q_user) == 0)
{
  $new_name=cn_without_serial($gridid);
  $q=pg_query($db,"select * from users");
  while ($ans=pg_fetch_array($q))
  {
    if (cn_without_serial($ans['gridid']) == $new_name)
	break;
  }
  if ($ans['gridid'] == "")
  {
    $q=pg_query($db,"insert into users (gridid) values ('" . $gridid . "')");
    header("Location: " . $uri . "?modify_profile=yes&new=yes");
    return;
  }
  else
  {
    $q=pg_query($db,"insert into users (gridid,primary_uid) values ('" . 
		$gridid . "'," . (($ans['primary_uid'] == "") ? $ans['uid'] : $ans['primary_uid']) . ")");
    header("Location: " . $uri);
    return;
  }
}
if (array_key_exists('adduser',$_POST))
{
  AddUser($db,$user,$_POST);
  return;
}
else if (array_key_exists('adduser',$_GET))
{
  AddUser($db,$user,$_GET);
  return;
}
else if (array_key_exists('manageusers',$_GET))
{
  include("beginpage.html");
  $q=pg_query($db,"select * from resourceauthorizations where uid=" . $user['uid'] . " and resource='ROAM' and permit='ManageUsers'");
  if (pg_num_rows($q) > 0)
  {
    print "<H2>Roam Users</H2>\n<TABLE>\n" .
      "<TR>\n" .
      "<TD><B>UID</B></TD>\n" .
      "<TD><B>Gridid</B></TD>\n" .
      "<TD><B>First Name</B></TD>\n" .
      "<TD><B>Last Name</B></TD>\n" .
      "<TD><B>Linked</B></TD>\n" .
      "</TR>\n";
    $q=pg_query($db,"select * from users order by uid");
    while ($ans=pg_fetch_array($q))
    {
      print "<TR><TD class=small>" . $ans['uid'] . "</TD>\n" .
	"<TD class=small>" . $ans['gridid'] . "</TD>\n" .
	"<TD class=small>" . $ans['first_name'] . "</TD>\n" .
	"<TD class=small>" . $ans['last_name'] . "</TD>\n" .
	"<TD class=small>" . $ans['primary_uid'] . "</TD>\n</TD>\n";
    }
  }
  else
  {
    print "<H2>You are not authorized to manage users</H2>\n";
  }
  include("endpage.html");
}
else if (array_key_exists('manageusers',$_GET))
{
  include("beginpage.html");
  $q=pg_query($db,"select * from resourceauthorizations where uid=" . $user['uid'] . " and resource='ROAM' and permit='ManageUsers'");
  if (pg_num_rows($q) > 0)
  {
    print "<H2>Roam Users</H2>\n<TABLE>\n" .
      "<TR>\n" .
      "<TD><B>UID</B></TD>\n" .
      "<TD><B>Gridid</B></TD>\n" .
      "<TD><B>First Name</B></TD>\n" .
      "<TD><B>Last Name</B></TD>\n" .
      "<TD><B>Linked</B></TD>\n" .
      "</TR>\n";
    $q=pg_query($db,"select * from users order by uid");
    while ($ans=pg_fetch_array($q))
    {
      print "<TR><TD class=small>" . $ans['uid'] . "</TD>\n" .
	"<TD class=small>" . $ans['gridid'] . "</TD>\n" .
	"<TD class=small>" . $ans['first_name'] . "</TD>\n" .
	"<TD class=small>" . $ans['last_name'] . "</TD>\n" .
	"<TD class=small>" . $ans['primary_uid'] . "</TD>\n</TD>\n";
    }
  }
  else
  {
    print "<H2>You are not authorized to manage users</H2>\n";
  }
  include("endpage.html");
}
else if ($_GET['do_modify_profile'] == "Ok")
{
  $return_url=$uri . "?" . remove_param($_SERVER['QUERY_STRING'],'do_modify_profile');
  if ($_GET['first_name'] == "")
  {
    header("Location: $return_url&error=" . urlencode("Please fill in a first name"));
    return;
  }
  if ($_GET['last_name'] == "")
  {
    header("Location: $return_url&error=" . urlencode("Please fill in a last name"));
    return;
  }
  if ($_GET['email'] == "")
  {
    header("Location: $return_url&error=" . urlencode("Please fill in an email address"));
    return;
  }
  $q = pg_query($db,"update users set first_name='" . $_GET['first_name'] . "',last_name='" . $_GET['last_name'] .
		"',phone='" . $_GET['phone'] . "',fax='" . $_GET['fax'] .  "',email='" . $_GET['email'] .
		"',affiliation='" . $_GET['affiliation'] . "',mailing='" . $_GET['mailing'] . "' where gridid='" .
		$gridid . "'");
  header("Location: $uri");
  return;
}
else if (isset($_GET['reset_modify_profile']))
{
  header("Location: $uri?modify_profile");
}
else if (isset($_GET['cancel_modify_profile']))
{
  header("Location: $uri");
}
else if (isset($_GET['modify_profile']))
{
  /////////////////////////////// User Profile Page ///////////////////////////////////////////////////////////////////////
  include("beginpage.html");
  if ($logged_in == 1)
    print "<FORM METHOD=\"GET\"><INPUT TYPE=\"SUBMIT\" NAME=\"logout\" VALUE=\"Logout\"></FORM>";
  print "<H2>User Profile</H2><P />\n";
  if ($_GET['error'] != "")
  {
    print "<span style=\"color: rgb(255,0,0); font-weight: bold\">" . $_GET['error'] . "</span>\n";
    $first_name=$_GET['first_name'];
    $last_name=$_GET['last_name'];
    $phone=$_GET['phone'];
    $fax=$_GET['fax'];
    $email=$_GET['email'];
    $affiliation=$_GET['affiliation'];
    $mailing=$_GET['mailing'];
  }
  else
  {
    if ($_GET['new'] != "")
    {
      print "Welcome to the $system_name Resource Authorization System. Before you can begin to use the system\n" .
	"we need to find out a few things about you. Please fill in the following information. While only\n" .
	"your first name, last name and email address are required, some resource providers may need the\n" .
	"remaining information before authorizing your use of a resource so it is a good idea to provide\n" .
	"as much information as possible.<P \>\n";
    }
    $first_name=$user['first_name'];
    $last_name=$user['last_name'];
    $phone=$user['phone'];
    $fax=$user['fax'];
    $email=$user['email'];
    $affiliation=$user['affiliation'];
    $mailing=$user['mailing'];
  }
  $required="<span style=\"color: rgb(255,0,0); font-weight: bold\">*</span>";
  print "<FORM METHOD=\"GET\">\n" .
    "<TABLE><TR><TD><B>First name:</B></TD><TD><INPUT TYPE=\"TEXT\" SIZE=20 value=\"$first_name\" name=\"first_name\">$required</TD></TR>\n" .
    "<TR><TD><B>Last name:</B></TD><TD><INPUT TYPE=\"TEXT\" SIZE=20 value=\"$last_name\" name=\"last_name\">$required</TD></TR>\n" .
    "<TR><TD><B>Telephone:</B></TD><TD><INPUT TYPE=\"TEXT\" SIZE=14 value=\"$phone\" name=\"phone\"></TD></TR>\n" .
    "<TR><TD><B>Fax:</B></TD><TD><INPUT TYPE=\"TEXT\" SIZE=14 value=\"$fax\" name=\"fax\"></TD></TR>\n" .
    "<TR><TD><B>Email:</B></TD><TD><INPUT TYPE=\"TEXT\" SIZE=20 value=\"$email\" name=\"email\">$required</TD></TR>\n" .
    "<TR><TD><B>Affiliation:</B></TD><TD><INPUT TYPE=\"TEXT\" SIZE=20 value=\"$affiliation\" name=\"affiliation\"></TD></TR>\n" .
    "<TR><TD><B>Mail Address:</B></TD><TD CLASS><INPUT TYPE=\"TEXT\" SIZE=80 value=\"$mailing\" name=\"mailing\"></TD></TR>\n" .
    "</TABLE>\n<span style=\"color: rgb(255,0,0); font-weight: bold\">* denotes required field</span>";
  print "<TABLE CLASS=\"clear\"><TR><TD><INPUT CLASS=\"managebutton\" TYPE=\"SUBMIT\" name=\"do_modify_profile\" value=\"Ok\"></TD>" .
    "<TD><INPUT CLASS=\"managebutton\" TYPE=\"SUBMIT\" name=\"reset_modify_profile\" value=\"Reset\"></TD>" .
    "<TD><INPUT CLASS=\"managebutton\" TYPE=\"SUBMIT\" value=\"Cancel\" name=\"cancel_modify_profile\"></TD></TR></TABLE>" .
    (($_GET['new'] != "") ? "<INPUT TYPE=\"HIDDEN\" NAME=\"new\" value=\"yes\">" : "") . "</FORM>\n";
  include("endpage.html");
  /////////////////////////////// User Profile Page ///////////////////////////////////////////////////////////////////////
}
else if ($_GET['cancel_request'] != "")
{
  header("Location: $uri?request=yes");
  return;
}
else if ($_GET['cancel_all_requests'] != "")
{
  header("Location: $uri");
  return;
}
else if ($_GET['do_request'] != "")
{
  $q=pg_query($db,"select * from resources where rid=" . $_GET['rid']);
  if (pg_num_rows($q) > 0)
  {
    $resource = pg_fetch_array($q);
    $qauth=pg_query($db,"select * from resourceauthorizations r join users u on r.uid=u.uid where resource='" . $resource['rname'] . "' and permit='Admin'");
    $message="A user has requested permissions for the resource " . $resource['rname'] . "\n\n" .
      "User information\n\n" .
      "First name:  " . $user['first_name'] . "\n" . 
      "Last name:   " . $user['last_name'] . "\n" .
      "Affiliation: " . $user['affiliation'] . "\n" .
      "Email:       " . $user['email'] . "\n" .
      "Phone:       " . $user['phone'] . "\n" .
      "Fax:         " . $user['fax'] . "\n" .
      "Mailing:     " . $user['mailing'] . "\n" .
      "Grid ID:     " . $user['gridid'] . "\n\n" .
      "Go to https://" . $_SERVER['SERVER_NAME'] . $uri . " to manage resources\n\n" .
      "Addition Information provided by the user:\n\n" .
      $_GET['request_info'];
    while (($admin=pg_fetch_array($qauth)))
    {
      if ($admin['email'] != "")
      {
        mail($admin['email'],"$system_name Resource request for resource " . $resource['rname'],$message,"From: " . $user['email'] . "\r\n" .
	     "Reply-To: " . $user['email'] . "\r\n" . "X-Mailer: PHP/" . phpversion());
      }
    }
  }
  header("Location: $uri");
}
else if ($_GET['request'] != "")
{
  /////////////////////////////// Request Page ///////////////////////////////////////////////////////////////////////
  include("beginpage.html");
  if ($logged_in == 1)
    print "<FORM METHOD=\"GET\"><INPUT TYPE=\"SUBMIT\" NAME=\"logout\" VALUE=\"Logout\"></FORM>";
  if (!isset($_GET['rid']))
  {
    print "Select a resource you wish to request permissions to use:<P />\n";
    $q_res=pg_query($db,"select * from resources where hidden is NULL order by rname");
    print "<TABLE border=1><TR><TD><B>Resource Name</B></TD><TD><B>Description</B></TD></TR>\n";
    while (($ans=pg_fetch_array($q_res)))
    {
      print "<TR><TD class=\"small\"><A HREF=\"$uri?request=yes&rid=" . $ans['rid'] . "\">" . $ans['rname'] . "</A></TD><TD class=\"small\">" . $ans['description'] . "</TD></TR>\n";
    }
     print "<FORM METHOD=\"GET\"><TABLE CLASS=\"clear\"><TR><TD><INPUT CLASS=\"managebutton\" TYPE=\"SUBMIT\" name=\"cancel_all_requests\" value=\"Cancel\"></TD>" .
    "</TR></TABLE></FORM>\n";
  }
  else
  {
    $q_res=pg_query($db,"select * from resources where rid=" . $_GET['rid']);
    $res=pg_fetch_array($q_res);
    print "<H2>Resource Permission Request<BR /><HR />" . $res['rname'] . " - " . $res['description'] . "</H2><P />" .
      $res['request_help'] . "</TD></TR>\n";
    print "<FORM METHOD=\"GET\"><INPUT TYPE=\"HIDDEN\" name=\"rid\" value=\"" . $_GET['rid'] . "\"><TEXTAREA ROWS=20 COLS=80 name=request_info></TEXTAREA>\n";
    print "<TABLE CLASS=\"clear\"><TR><TD><INPUT CLASS=\"managebutton\" TYPE=\"SUBMIT\" name=\"do_request\" value=\"Submit\"></TD>" .
    "<TD><INPUT CLASS=\"managebutton\" TYPE=\"SUBMIT\" value=\"Cancel\" name=\"cancel_request\"></TD></TR></TABLE></FORM>\n";
  }
  include("endpage.html");
  /////////////////////////////// Request Page ///////////////////////////////////////////////////////////////////////
}
else if ($_GET['request'] != "")
{
  /////////////////////////////// Request Page ///////////////////////////////////////////////////////////////////////
  include("beginpage.html");
  if ($logged_in == 1)
    print "<FORM METHOD=\"GET\"><INPUT TYPE=\"SUBMIT\" NAME=\"logout\" VALUE=\"Logout\"></FORM>";
  if (!isset($_GET['rid']))
  {
    print "Select a resource you wish to request permissions to use:<P />\n";
    $q_res=pg_query($db,"select * from resources where hidden is NULL order by rname");
    print "<TABLE border=1><TR><TD><B>Resource Name</B></TD><TD><B>Description</B></TD></TR>\n";
    while (($ans=pg_fetch_array($q_res)))
    {
      print "<TR><TD class=\"small\"><A HREF=\"$uri?request=yes&rid=" . $ans['rid'] . "\">" . $ans['rname'] . "</A></TD><TD class=\"small\">" . $ans['description'] . "</TD></TR>\n";
    }
     print "<FORM METHOD=\"GET\"><TABLE CLASS=\"clear\"><TR><TD><INPUT CLASS=\"managebutton\" TYPE=\"SUBMIT\" name=\"cancel_all_requests\" value=\"Cancel\"></TD>" .
    "</TR></TABLE></FORM>\n";
  }
  else
  {
    $q_res=pg_query($db,"select * from resources where rid=" . $_GET['rid']);
    $res=pg_fetch_array($q_res);
    print "<H2>Resource Permission Request<BR /><HR />" . $res['rname'] . " - " . $res['description'] . "</H2><P />" .
      $res['request_help'] . "</TD></TR>\n";
    print "<FORM METHOD=\"GET\"><INPUT TYPE=\"HIDDEN\" name=\"rid\" value=\"" . $_GET['rid'] . "\"><TEXTAREA ROWS=20 COLS=80 name=request_info></TEXTAREA>\n";
    print "<TABLE CLASS=\"clear\"><TR><TD><INPUT CLASS=\"managebutton\" TYPE=\"SUBMIT\" name=\"do_request\" value=\"Submit\"></TD>" .
    "<TD><INPUT CLASS=\"managebutton\" TYPE=\"SUBMIT\" value=\"Cancel\" name=\"cancel_request\"></TD></TR></TABLE></FORM>\n";
  }
  include("endpage.html");
  /////////////////////////////// Request Page ///////////////////////////////////////////////////////////////////////
}
else if ($_GET['cancel_inquiry'] != "")
{
  header("Location: $uri");
  return;
}
else if ($_GET['do_inquiry'] != "")
{
  $q=pg_query($db,"select * from resources where rid=" . $_GET['rid']);
  if (pg_num_rows($q) > 0)
  {
    $resource = pg_fetch_array($q);
    $qauth=pg_query($db,"select * from resourceauthorizations r join users u on r.uid=u.uid where resource='" . 
		    $resource['rname'] . "' and permit='Admin'");
    $message="A user has submitted an inquiry for the resource " . $resource['rname'] . " regarding permission " . $_GET['permit'] . "\n\n" .
      "User information\n\n" .
      "First name:  " . $user['first_name'] . "\n" . 
      "Last name:   " . $user['last_name'] . "\n" .
      "Affiliation: " . $user['affiliation'] . "\n" .
      "Email:       " . $user['email'] . "\n" .
      "Phone:       " . $user['phone'] . "\n" .
      "Fax:         " . $user['fax'] . "\n" .
      "Mailing:     " . $user['mailing'] . "\n" .
      "Grid ID:     " . $user['gridid'] . "\n\n" .
      "Go to https://" . $_SERVER['SERVER_NAME'] . $uri . " to manage resources\n\n" .
      "Addition Information provided by the user:\n\n" .
      $_GET['inquiry_info'];
    while (($admin=pg_fetch_array($qauth)))
    {
      if ($admin['email'] != "")
      {
        mail($admin['email'],"$system_name Resource Inquiry for " . $resource['rname'],$message,"From: " . $user['email'] . "\r\n" .
	     "Reply-To: " . $user['email'] . "\r\n" . "X-Mailer: PHP/" . phpversion());
      }
    }
  }
  header("Location: $uri");
}
else if ($_GET['inquiry'] != "")
{
  /////////////////////////////// Inquiry Page ///////////////////////////////////////////////////////////////////////
  include("beginpage.html");
  if ($logged_in == 1)
    print "<FORM METHOD=\"GET\"><INPUT TYPE=\"SUBMIT\" NAME=\"logout\" VALUE=\"Logout\"></FORM>";
  $q_auth=pg_query($db,"select * from resourceauthorizations a join resources r on a.resource=r.rname where aid=" . $_GET['aid']);
  $q=pg_query($db,"select * from resources where rid=" . $_GET['rid']);
  $res=pg_fetch_array($q);
  $auth=pg_fetch_array($q_auth);
  print "<H2>Resource Permission Inquiry<BR /><HR />" . $auth['rname'] . " - " . $auth['description'] . " - Permission " . $auth['permit'] . "</H2><P />";
  if ($res['inquiry_help'] == "")
  {
?>
You should submit an inquiry to either request a change to a resource permission or to report problems related to the resource permission you currently have.
Please provide as much detail as you can so that the resource provider can diagnose your problem.
<?php 
  }
  else
  {
    print $res['inquiry_help'];
  }
  print "<FORM METHOD=\"GET\"><INPUT TYPE=\"HIDDEN\" name=\"rid\" value=\"" . $_GET['rid'] . "\">\n" .
  "<INPUT TYPE=\"HIDDEN\" name=\"permit\" value=\"" . $auth['permit'] . "\"><TEXTAREA ROWS=20 COLS=80 name=inquiry_info></TEXTAREA>\n";
  print "<TABLE CLASS=\"clear\"><TR><TD><INPUT CLASS=\"managebutton\" TYPE=\"SUBMIT\" name=\"do_inquiry\" value=\"Submit\"></TD>" .
    "<TD><INPUT CLASS=\"managebutton\" TYPE=\"SUBMIT\" value=\"Cancel\" name=\"cancel_inquiry\"></TD></TR></TABLE></FORM>\n";
  include("endpage.html");
  /////////////////////////////// Inquiry Page ///////////////////////////////////////////////////////////////////////
}
else if ($_GET['do_create'] != "")
{
  if ($_GET["resource"] == "")
  {
    header("Location: $uri?create=yes&error=" . urlencode("Please fill in a resource name."));
    return;
  }
  if ($_GET["description"] == "")
  {
    header("Location: $uri?create=yes&error=" . urlencode("Please fill in a short description for this resource."));
    return;
  }
  $q_res=pg_query("select * from resources where rname='" . $_GET['resource'] . "'");
  if (pg_num_rows($q_res) != 0)
  {
    header("Location: $uri?create=yes&error=" . urlencode("A resource already exists with the name\"" . $_GET['resource'] . "\". Please choose a different name."));
    return;
  }
  $q=pg_query($db,"insert into resources (rname,description,hidden) values ('" . $_GET['resource'] . "','" . $_GET['description'] . "','*')");
  $q=pg_query($db,"insert into resourceauthorizations (uid,resource,permit) values (" . $user['uid'] . ",'" . $_GET['resource'] . "','Admin')");
  $q=pg_query($db,"select rid from resources where rname='" . $_GET['resource'] . "'");
  $ans=pg_fetch_array($q);
  $q=pg_query($db,"select pid from permissions where permission='Admin'");
  $perm=pg_fetch_array($q);
  $q=pg_query($db,"insert into resourcepermissions (rid,pid) values (" . $ans['rid'] . "," . $perm['pid'] . ")");
  header("Location: $uri?modify_resource=yes&rid=" . $ans['rid']);  
}
else if ($_GET['create'] != "")
{
  /////////////////////////////// Create Resource Page ///////////////////////////////////////////////////////////////////////
  include("beginpage.html");
  if ($logged_in == 1)
    print "<FORM METHOD=\"GET\"><INPUT TYPE=\"SUBMIT\" NAME=\"logout\" VALUE=\"Logout\"></FORM>";
  print "<H2>New Resource Creation</H2>\n" .
    "You are about to create a resource. You will become the administrator for this resource and will be able to grant or deny users the priviledges\n" .
    "necessary to access this resource. In order to enforce the resource authorizations you will have to instrument your $system_name service to\n" .
    "check the authorization system to determine if the person attempting to use your service is authorized or not. There are shell scripts\n" .
    "available for querying the resource authorization system which you can use. MDSplus can also be configured to query the resource authorization\n" .
    "system.\n";
  if ($_GET['error'] != "")
  {
    print "<span style=\"color: rgb(255,0,0); font-weight: bold\">" . $_GET['error'] . "</span>\n";
  }
?>

<FORM METHOD=\"GET\">
<TABLE>
<TR><TD><B>Name:</B></TD><TD><INPUT TYPE="TEXT" name="resource" SIZE=30></TD></TR>
<TR><TD><B>Description:</B></TD><TD><INPUT TYPE="TEXT" name="description" SIZE=80></TD></TR>
</TABLE>
<TABLE CLASS="clear">
<TR><TD><INPUT CLASS="managebutton" TYPE="Submit" name="do_create" value="Ok"></TD><TD><INPUT CLASS="managebutton" TYPE="Submit" name="cancel_create" value="Cancel"></TD></TR>
</FORM>

<?php
  include("endpage.html");
  /////////////////////////////// Create Resource Page ///////////////////////////////////////////////////////////////////////
}
else if ($_GET['delete_resource'])
{
  $qres=pg_query($db,'select rname from resources where rid=' . $_GET['rid']);
  $ans=pg_fetch_array($qres);
  pg_query($db,"delete from resourceauthorizations where resource='" . addslashes($ans['rname']) . "'");
  pg_query($db,"delete from resources where rid=" . $_GET['rid']);
  pg_query($db,"delete from resourcepermissions where rid=" . $_GET['rid']);
  header("Location: $uri");
}
else if ($_GET['delete_rpermission'] != "")
{
  $q=pg_query($db,"select pid from resourcepermissions where rpid=" . $_GET['rpid']);
  $ans=pg_fetch_array($q);
  pg_query($db,"delete from resourcepermissions where rpid=" . $_GET['rpid']);
  $q=pg_query($db,"select * from resourcepermissions where pid=" . $ans['pid']);
  if (pg_num_rows($q) == 0)
  {
    $q=pg_query($db,"delete from permissions where pid=" . $ans['pid']);
  }
  header("Location: $uri?rid=" . $_GET['rid'] . "&modify_resource=yes");
}
else if ($_GET['select_user'] != "")
{
  /////////////////////////////// Select User Page ///////////////////////////////////////////////////////////////////////
  include("beginpage.html");
  if ($logged_in == 1)
    print "<FORM METHOD=\"GET\"><INPUT TYPE=\"SUBMIT\" NAME=\"logout\" VALUE=\"Logout\"></FORM>";
  print "<H2>Select user by clicking on user name</H2>\n";
  print "<TABLE><TR><TD><B>User name</B></TD><TD><B>Affiliation</B></TD><TD><B>Email</B></TD><TD><B>CN</B></TD></TR>\n";
  $q=pg_query("select * from users where primary_uid is NULL order by last_name");
  while(($ans=pg_fetch_array($q)))
  {
    $cn="";
    $tok=strtok($ans['gridid'],"/");
    while ($tok)
    {
      if (strncasecmp($tok,"CN=",3) == 0)
      {
        $cn=substr($tok,3);
        break;
      }
      $tok=strtok("/");
    }
    print "<TR><TD CLASS=\"small\"><A HREF=$uri?add_authorization=yes&uid=" . $ans['uid'] . "&rid=" . $_GET['rid'] . ">"  . 
      $ans['last_name'] . (($ans['first_name'] == "") ? "" : ", " .$ans['first_name']) .
      "</A></TD><TD CLASS=\"small\">" . $ans['affiliation'] . "</TD><TD CLASS=\"small\">" . $ans['email'] . "</TD><TD CLASS=\"small\">" . $cn . "</TD></TR>\n";
  }
  print "</TABLE>\n";
  include("endpage.html");
  /////////////////////////////// Select User Page ///////////////////////////////////////////////////////////////////////
}
else if ($_GET['show_user'] != "")
{
  /////////////////////////////// Show User Page ///////////////////////////////////////////////////////////////////////
  include("beginpage.html");
  if ($logged_in == 1)
    print "<FORM METHOD=\"GET\"><INPUT TYPE=\"SUBMIT\" NAME=\"logout\" VALUE=\"Logout\"></FORM>";
  print "<H2>User information</H2>\n";
  $q=pg_query($db,"select * from users where uid=" . $_GET['show_user']);
  $ans=pg_fetch_array($q);
  print "<TABLE>\n" .
    "<TR><TD><B>First name:</B></TD><TD>" . $ans['first_name'] . "</TD></TR>\n" .
    "<TR><TD><B>Last name:</B></TD><TD>" . $ans['last_name'] . "</TD></TR>\n" .
    "<TR><TD><B>Telephone:</B></TD><TD>" . $ans['phone'] . "</TD></TR>\n" .
    "<TR><TD><B>Fax:</B></TD><TD>" . $ans['fax'] . "</TD></TR>\n" .
    "<TR><TD><B>Email:</B></TD><TD>" . $ans['email'] . "</TD></TR>\n" .
    "<TR><TD><B>Affiliation:</B></TD><TD>" . $ans['affiliation'] . "</TD></TR>\n" .
    "<TR><TD><B>Mail Address:</B></TD><TD>" . $ans['mailing'] . "</TD></TR>\n" .
    "<TR><TD><B>Gridid:</B></TD><TD>" . $ans['gridid'] . "</TD></TR>\n" .
    "</TABLE>\n";
  print "<FORM METHOD=\"GET\"><INPUT CLASS=\"managebutton\" TYPE=\"Submit\" value=\"Back\" name=\"modify_authorizations\"><INPUT TYPE=\"HIDDEN\" NAME=\"rid\" VALUE=\"" . $_GET['rid'] . "\"></FORM>\n";
  include("endpage.html");
  /////////////////////////////// Show User Page ///////////////////////////////////////////////////////////////////////
}
else if ($_GET['cancel_add_authorization'] != "")
{
  header("Location: $uri?modify_authorizations=yes&rid=" . $_GET['rid']);
}
else if ($_GET['do_send_user_email'] != "")
{
  if ($_GET['do_send_user_email'] == "Granted")
    $message="You have been granted the " . stripslashes($_GET['permit']) .
      " permission for resource " . stripslashes($_GET['rname']) . " on $system_name .\n\n" .
      "You can view your authorization information via the ROAM Website (https://" . $_SERVER["HTTP_HOST"] . "$uri)."
						 ;
  else
    $message="Your " . stripslashes($_GET['permit']) . " permission for resource " . stripslashes($_GET['rname']) . " has been revoked";
  mail($_GET['email'],"$system_name Resource Permission " . stripslashes($_GET['do_send_user_email']) . " for resource " . stripslashes($_GET['rname']),$message,"From: " . $user['email'] . "\r\n" .
	     "Reply-To: " . $user['email'] . "\r\n" . "X-Mailer: PHP/" . phpversion());
  header("Location: $uri?modify_authorizations=yes&rid=" . $_GET['rid']);
}
else if ($_GET['send_user_email'] != "")
{
  /////////////////////////////// Prompt for Send User Email Page ///////////////////////////////////////////////////////////////////////
  include("beginpage.html");
  if ($logged_in == 1)
    print "<FORM METHOD=\"GET\"><INPUT TYPE=\"SUBMIT\" NAME=\"logout\" VALUE=\"Logout\"></FORM>";
  print "Send user email? " .
    "<A HREF=\"$uri?rid=" . $_GET['rid'] . "&rname=" . urlencode(stripslashes($_GET['rname'])) . "&permit=" . urlencode(stripslashes($_GET['permit'])) . 
    "&do_send_user_email=" . $_GET['send_user_email'] . "&email=" . urlencode($_GET['email']) . "\">Yes</A>" .
    " <A HREF=\"$uri?rid=" . $_GET['rid'] . "&modify_authorizations=yes\">No</A>";
  include("endpage.html");
  /////////////////////////////// Prompt for Send User Email Page ///////////////////////////////////////////////////////////////////////
}
else if ($_GET['do_add_authorization'] != "")
{
  if ($_GET['rid'] == "")
  {
    header("Location: $uri");
  }
  else if ($_GET['uid'] == "")
  {
    header("Location: $uri?add_authorization=yes&rid=" . $_GET['rid'] . "&error=" . urlencode("Please select a user"));
  }
  else
  {
    $q1=pg_query("select * from resources where rid=" . $_GET['rid']);
    $res=pg_fetch_array($q1);
    $q=pg_query("select * from resourceauthorizations where uid=" . $_GET['uid'] . " and resource='" . addslashes($res['rname']) . "' and permit='" . $_GET['permit'] . "'");
    if (pg_num_rows($q) != 0)
    {
      header("Location: $uri?add_authorization=yes&rid=" . $_GET['rid'] . "&uid=" . $_GET['uid'] . "&error=" . urlencode("User already owns this permission"));
    }
    else
    {
      $qauth=pg_query($db,"select * from resourceauthorizations r join users u on r.uid=u.uid where resource='" . 
		      addslashes($res['rname']) . "' and permit='Admin'");
      $q=pg_query("select * from users where uid=" . $_GET['uid']);
      $ans=pg_fetch_array($q);
      $message=$user['first_name'] . " " . $user['last_name'] . " has granted " . $_GET['permit'] . " permission for resource " . $res['rname'] . " to " .
	"user " . $ans['first_name'] . " " . $ans['last_name'] . "\n\n" .
	"User information\n\n" .
	"First name:  " . $ans['first_name'] . "\n" . 
	"Last name:   " . $ans['last_name'] . "\n" .
	"Affiliation: " . $ans['affiliation'] . "\n" .
	"Email:       " . $ans['email'] . "\n" .
	"Phone:       " . $ans['phone'] . "\n" .
	"Fax:         " . $ans['fax'] . "\n" .
	"Mailing:     " . $ans['mailing'] . "\n" .
	"Grid ID:     " . $ans['gridid'] . "\n\n";
      while (($admin=pg_fetch_array($qauth)))
      {
	if ($admin['email'] != "")
	{
	  mail($admin['email'],"$system_name Resource Permission Granted for resource " . $res['rname'],$message,"From: " . $user['email'] . "\r\n" .
	     "Reply-To: " . $user['email'] . "\r\n" . "X-Mailer: PHP/" . phpversion());
	}
      }
      pg_query("insert into resourceauthorizations (uid,resource,permit) values (" . $_GET['uid'] . ",'" . addslashes($res['rname']) . "','" . $_GET['permit'] . "')");
      header("Location: $uri?send_user_email=Granted&rid=" . $_GET['rid'] . "&email=" . urlencode($ans['email']) . "&permit=" . urlencode($_GET['permit']) .
	     "&rname=" . urlencode($res['rname']));
    }
  }
}
else if ($_GET['add_authorization'] != "")
{
  /////////////////////////////// Add Authorization Page ///////////////////////////////////////////////////////////////////////
  include("beginpage.html");
  if ($logged_in == 1)
    print "<FORM METHOD=\"GET\"><INPUT TYPE=\"SUBMIT\" NAME=\"logout\" VALUE=\"Logout\"></FORM>";
  if ($_GET['error'] != "")
  {
    print "<span style=\"color: rgb(255,0,0); font-weight: bold\">" . $_GET['error'] . "</span>\n";
  }
  $q_res=pg_query($db,"select * from resources where rid=" . $_GET['rid']);
  $q_per=pg_query($db,"select * from resourcepermissions r join permissions p on r.pid=p.pid where rid=" . $_GET['rid'] . " order by permission");
  $res=pg_fetch_array($q_res);
  if ($_GET['uid'] != "")
  {
    $q_user=pg_query($db,"select * from users where uid=" . $_GET['uid']);
    $newuser=pg_fetch_array($q_user);
  }
  print "<H2>Add authorization for resource for resource: " . $res['rname'] . "</H2><HR />\n";
  print "<TABLE CLASS=\"clear\"><TR><TD><FORM METHOD=\"GET\"><TABLE>\n" .
    "<TR><TD><B>User:</B></TD><TD><INPUT TYPE=\"TEXT\" SIZE=40 READONLY VALUE=\"" . $newuser['first_name'] . " " . $newuser['last_name'] . "\"></TD></TR>\n" .
    "<TR><TD><B>Permission:</B></TD><TD><SELECT NAME=\"permit\">\n";
  while (($ans_per=pg_fetch_array($q_per)))
  {
    $q=pg_query($db,"select * from permissions where pid=" . $ans_per['pid']);
    $ans=pg_fetch_array($q);
    print "<OPTION VALUE=\"" . $ans['permission']  . "\">" .  $ans['permission'] . " --- " . $ans['pdescription'] . "</OPTION>\n";
  }
  print "</SELECT></TD></TR></TABLE>\n";
  print "</TD><TD style=\"vertical-align: top;\">\n" .
    "<FORM METHOD=\"GET\"><INPUT CLASS=\"managebutton\" TYPE=\"Submit\" Value=\"Select User\" NAME=\"select_user\"><INPUT TYPE=\"HIDDEN\" NAME=\"rid\" VALUE=\"" . $_GET['rid'] . "\"></TD></TR></TABLE>\n";
  print "<TABLE CLASS=\"clear\"><TR><TD><INPUT CLASS=\"managebutton\" TYPE=\"SUBMIT\" name=\"do_add_authorization\" value=\"Submit\"></TD>" .
    "<TD><INPUT CLASS=\"managebutton\" TYPE=\"SUBMIT\" value=\"Cancel\" name=\"cancel_add_authorization\"></TD></TR></TABLE>" .
    "<INPUT TYPE=\"HIDDEN\" NAME=\"rid\" VALUE=\"" . $_GET['rid'] . "\"><INPUT TYPE=\"HIDDEN\" NAME=\"uid\" VALUE=\"" . $_GET['uid'] . "\"></FORM>\n";
  include("endpage.html");
  /////////////////////////////// Add Authorization Page ///////////////////////////////////////////////////////////////////////
}
else if ($_GET['do_add_permission'] != "")
{
  $q=pg_query("select * from resourcepermissions where rid=" . $_GET['rid'] . " and pid=" . $_GET['pid']);
  if (pg_num_rows($q) == 0)
  {
    $q=pg_query("insert into resourcepermissions (rid,pid) values (" . $_GET['rid'] . "," . $_GET['pid'] . ")");
    header("Location: $uri?rid=" . $_GET['rid'] . "&add_permission=yes");
  }
}
else if ($_GET['do_add_new_permission'] != "")
{
  $return_url="Location: $uri?rid=" . $_GET['rid'] . "&add_new_permission=yes&permission=" . urlencode($_GET['permission']) .
    "&pdescription=" . urlencode($_GET['pdescription']);
  if ($_GET['permission'] == "")
  {
    header($return_url . "&error=" . urlencode("Please specify the permission name"));
  }
  else if ($_GET['pdescription'] == "")
  {
    header($return_url . "&error=" . urlencode("Please specify the permission description"));
  }
  else
  {
    $q=pg_query("select * from permissions where permission='" . $_GET['permission'] . "'");
    if (pg_num_rows($q) > 0)
    {
      header($return_url . "&error=" . urlencode("Permission " . $_GET['permission'] . " is already defined"));
    }
    else
    {
      $q=pg_query($db,"insert into permissions (permission,pdescription) values ('" . $_GET['permission'] . 
		  "','" . $_GET['pdescription'] . "')");
      $q=pg_query($db,"select pid from permissions where permission='" . $_GET['permission'] . "'");
      $ans=pg_fetch_array($q);
      $q=pg_query($db,"insert into resourcepermissions (rid,pid) values (" . $_GET['rid'] . "," . $ans['pid'] . ")");
      header("Location: $uri?rid=" . $_GET['rid'] . "&add_permission=yes");
    }
  }
}
else if ($_GET['add_new_permission'] != "")
{
  /////////////////////////////// Add New Permission Page ///////////////////////////////////////////////////////////////////////
  $q=pg_query($db,"select * from resources where rid=" . $_GET['rid']);
  $ans=pg_fetch_array($q);
  include("beginpage.html");
  if ($logged_in == 1)
    print "<FORM METHOD=\"GET\"><INPUT TYPE=\"SUBMIT\" NAME=\"logout\" VALUE=\"Logout\"></FORM>";
  print "<H2>Add new permission for resource " . $ans['rname'] . "</H2>\n";
  if ($_GET['error'] != "")
  {
    print "<span style=\"color: rgb(255,0,0); font-weight: bold\">" . $_GET['error'] . "</span>\n";
  }
  print "<FORM METHOD=\"GET\">\n" .
    "<INPUT TYPE=\"HIDDEN\" NAME=\"rid\" VALUE=\"" . $_GET['rid'] . "\">" .
    "<TABLE><TR><TD><B>Permission name:</B></TD><TD><INPUT TYPE=\"TEXT\" SIZE=20 NAME=\"permission\" VALUE=\"" . $_GET['permission'] . "\"></TD></TR>\n" .
    "<TR><TD><B>Description:</B></TD><TD><INPUT TYPE=\"TEXT\" SIZE=20 NAME=\"pdescription\" VALUE=\"" . $_GET['pdescription'] . "\"></TD></TR></TABLE>\n" .
    "<TABLE><TR><TD><INPUT CLASS=\"managebutton\" TYPE=\"Submit\" NAME=\"do_add_new_permission\" VALUE=\"Ok\"></TD>" .
    "<TD><INPUT CLASS=\"managebutton\" TYPE=\"Submit\" NAME=\"add_permission\" VALUE=\"Cancel\"></TD>" .
    "</FORM>\n";
  include("endpage.html");
  /////////////////////////////// Add New Permission Page ///////////////////////////////////////////////////////////////////////
}
else if ($_GET['add_permission'] != "")
{
  /////////////////////////////// Add Permission Page ///////////////////////////////////////////////////////////////////////
  $q=pg_query($db,"select * from resources where rid=" . $_GET['rid']);
  $ans=pg_fetch_array($q);
  include("beginpage.html");
  if ($logged_in == 1)
    print "<FORM METHOD=\"GET\"><INPUT TYPE=\"SUBMIT\" NAME=\"logout\" VALUE=\"Logout\"></FORM>";
  print "<H2>Select permission valid for resource " . $ans['rname'] . "</H2><P \>\n" .
    "<TABLE CLASS=\"clear\"><TR><TD><TABLE CLASS=\"border\"><TR><TD><B>Permission</B></TD><TD><B>Description</B></TD></TR>\n";
  $q=pg_query("select * from permissions where pid not in (select pid from resourcepermissions where rid=" . $_GET['rid'] . ")");
  while (($ans=pg_fetch_array($q)))
  {
    print "<TR><TD CLASS=\"small\"><A HREF=\"$uri?rid=" . $_GET['rid'] . "&do_add_permission=yes&pid=" . $ans['pid'] . "\">" . $ans['permission'] . "</A></TD>\n" .
      "<TD CLASS=\"small\">" . $ans['pdescription'] . "</TD><TR>\n";
  }
  print "</TABLE></TD><TD style=\"vertical-align: top; text-align: right;\">" .
    "<FORM METHOD=\"GET\">\n" .
    "<INPUT TYPE=\"HIDDEN\" NAME=\"rid\" VALUE=\"" . $_GET['rid'] . "\">" .
    "<INPUT CLASS=\"managebutton\" TYPE=\"SUBMIT\" VALUE=\"Add New\" name=\"add_new_permission\">\n</TD></TR></TABLE>\n" .
    "<INPUT CLASS=\"managebutton\" TYPE=\"SUBMIT\" VALUE=\"Cancel\" name=modify_resource>\n</FORM></TD></TR></TABLE>\n";
  include("endpage.html");
  /////////////////////////////// Add Permission Page ///////////////////////////////////////////////////////////////////////
}
else if (array_key_exists('do_modify_aux',$_GET))
{
  $q_auth=pg_query($db,"select resource,permit,first_name,last_name,auxilliary from resourceauthorizations r join users u on r.uid=u.uid where aid=" . $_GET['aid']);
  if (pg_num_rows($q_auth) > 0)
  {
    $auth=pg_fetch_array($q_auth);
    $q_ok=pg_query($db,"select aid from resourceauthorizations where resource='" . addslashes($auth['resource']) . "' and uid=" . $user['uid'] . " and permit='Admin'");
    if (pg_num_rows($q_ok) > 0)
    { 
      $q=pg_query($db,"update resourceauthorizations set auxilliary='" . $_GET['do_modify_aux'] . "' where aid=" . $_GET['aid']);
      header("Location: $uri?modify_authorizations=yes&rid=" . $_GET['rid']);
    }
    else
    {
      header("Location: $uri");
    }
  }
  else
    header("Location: $uri");
}
else if (array_key_exists('modify_aux',$_GET))
{
  $q_auth=pg_query($db,"select resource,permit,first_name,last_name,auxilliary from resourceauthorizations r join users u on r.uid=u.uid where aid=" . $_GET['aid']);
  if (pg_num_rows($q_auth) > 0)
  {
    $auth=pg_fetch_array($q_auth);
    $q_ok=pg_query($db,"select aid from resourceauthorizations where resource='" . addslashes($auth['resource']) . "' and uid=" . $user['uid'] . " and permit='Admin'");
    if (pg_num_rows($q_ok) > 0)
    { 
      include("beginpage.html");
      if ($logged_in == 1)
	print "<FORM METHOD=\"GET\"><INPUT TYPE=\"SUBMIT\" NAME=\"logout\" VALUE=\"Logout\"></FORM>";
      print "<H2>Auxillary Resource Authorization Information</H2>\n";
      print "<FORM METHOD=\"GET\"><INPUT TYPE=\"Hidden\" NAME=\"aid\" value=\"" . $_GET['aid'] . "\">\n" .
	"<INPUT TYPE=\"Hidden\" NAME=\"rid\" value=\"" . $_GET['rid'] . "\">\n" .
	"<TABLE><TR><TD><B>Resource:</B></TD><TD>" . $auth['resource'] . "</TD></TR>\n" .
	"<TR><TD><B>Permission:</B></TD><TD>" . $auth['permit'] . "</TD></TR>\n" .
	"<TR><TD><B>User:</B></TD><TD>" . $auth['first_name'] . " " . $auth['last_name'] . "</TD></TR>\n" .
	"<TR><TD><B>Info:</B></TD><TD><INPUT TYPE=\"TEXT\" SIZE=50 NAME=\"do_modify_aux\" VALUE=\"" . $auth['auxilliary'] . "\"></TD></TR>\n" .
	"</TABLE>\n</FORM>\n";
    }
    else
    {
      header("Location: $uri");
    }
  }
  else
  {
    header("Location: $uri");
  }
}
else if ($_GET['modify_authorizations'] != "")
{
  /////////////////////////////// Modify Resource Authorizations Page ///////////////////////////////////////////////////////////////////////
  $q_res=pg_query($db,"select * from resources where rid=" . $_GET['rid']);
  $res=pg_fetch_array($q_res);
  $q_auth=pg_query("select * from resourceauthorizations where resource='" .  addslashes($res['rname']) . 
		   "' and uid=" . $user['uid'] . " and permit='Admin'");
  if (pg_num_rows($q_auth) > 0)
  {
    include("beginpage.html");
    if ($logged_in == 1)
      print "<FORM METHOD=\"GET\"><INPUT TYPE=\"SUBMIT\" NAME=\"logout\" VALUE=\"Logout\"></FORM>";
    $delete_admins=0;
    $delete_resource=0;
    $q=pg_query($db,"select * from resourceauthorizations where resource='" . addslashes($res['rname']) . "' and permit != 'Admin'");
    if (pg_num_rows($q) == 0)
      $delete_resource=1;
    $q=pg_query($db,"select * from resourceauthorizations where resource='" . addslashes($res['rname']) . "' and permit = 'Admin'");
    if (pg_num_rows($q) > 1)
      $delete_admins=1;
    print "<H2>Resource permissions for " . $res['rname'] . "</H2>\n";
    print "<FORM METHOD=\"GET\"><TABLE CLASS=\"clear\"><TR><TD><INPUT CLASS=\"managebutton\" TYPE=\"Submit\" name=\"modify_resource\" value=\"Modify Resource Properties\"></TD>" .
      (($delete_resource == 1) ? "<TD><INPUT CLASS=\"managebutton\" TYPE=\"Submit\" name=\"delete_resource\" value=\"Delete Resource\"></TD>" : "") .
      "<INPUT TYPE=\"Hidden\" name=\"rid\" value=\"" . $_GET['rid'] . "\"></FORM><FORM METHOD=\"GET\"><TD><INPUT CLASS=\"managebutton\" TYPE=\"Submit\" value=\"Back\"></TD></TR>" .
      "</FORM></TABLE>";
    print "<HR />" . nl2br($res['admin_help']) . "<HR />\n";
    if ($_GET['error'] != "")
      print "<span style=\"color: rgb(255,0,0); font-weight: bold\">" . $_GET['error'] . "</span>\n";
    print "<TABLE CLASS=\"clear\"><TR><TD><H3>Resource Authorizations</H3></TD>\n" .
      "<TD style=\"vertical-align: middle; text-align: right;\">" .
      "<FORM METHOD=\"GET\"><INPUT CLASS=\"managebutton\" TYPE=\"SUBMIT\" VALUE=\"Add New\" name=add_authorization>" .
      "<INPUT TYPE=\"HIDDEN\" NAME=\"rid\" VALUE=\"" . $_GET['rid'] . "\"></FORM></TD></TR></TABLE>\n";
    print "<TABLE>\n" .
      "<TR><TD><B>User</B></TD><TD><B>Permission</B></TD><TD><B>Auxilliary</B></TD><TD></TD></TR>\n";
    $q_auth=pg_query($db,"select * from resourceauthorizations r inner join users u on r.uid=u.uid where resource='" . addslashes($res['rname']) . "' order by last_name");
    while(($auth=pg_fetch_array($q_auth)))
    {
      print "<FORM METHOD=\"GET\"><TR><TD CLASS=\"small\"><A HREF=\"$uri?rid=" . $_GET['rid'] . "&show_user=" . $auth['uid'] . "\"></FORM>" . $auth['last_name'] . ", " . $auth['first_name'] .
	"</A></TD><TD CLASS=\"small\">" . $auth['permit'] . "</TD><TD CLASS=\"small\">" . $auth['auxilliary'] . "</TD><TD CLASS=\"small\">" .
	"<FORM METHOD=\"GET\"><INPUT CLASS=\"managebutton\" TYPE=\"SUBMIT\" NAME=\"modify_aux\" VALUE=\"Modify\"></TD><INPUT TYPE=\"HIDDEN\" name=aid value=" . $auth['aid'] . ">" .
	"<INPUT TYPE=\"HIDDEN\" name=rid value=\"" . $_GET['rid'] . "\">" .
	"</FORM><TD CLASS=\"small\">\n" .
	((($auth['permit'] != "Admin") || ($delete_admins == 1)) ? 
	 "<FORM METHOD=\"GET\"><INPUT CLASS=\"managebutton\" TYPE=\"SUBMIT\" NAME=\"delete_authorization\" VALUE=\"Delete\">" : "") . "</TD></TR><INPUT TYPE=\"HIDDEN\" name=aid value=" . $auth['aid'] . ">" .
	"<INPUT TYPE=\"Hidden\" NAME=\"rid\" value=\"" . $_GET['rid'] . "\"></FORM>\n";
    }
    print "</TABLE>";
    include("endpage.html");
  }
  else
  {
    header("Location: $uri");
  }
  /////////////////////////////// Modify Resource Authorizations Page ///////////////////////////////////////////////////////////////////////
}
else if ($_GET['do_modify_description'] != "")
{
  if ($_GET['description'] == "")
    header("Location: $uri?rid=" . $_GET['rid'] . "&modify_description=yes&error=" . urlencode("You must provide a resource description"));
  $q=pg_query("update resources set description='" . $_GET['description'] . "' where rid=" . $_GET['rid']);
  header("Location: $uri?rid=" . $_GET['rid'] . "&modify_resource=yes");
}
else if ($_GET['modify_description'] != "")
{
  /////////////////////////////// Modify Resource Description Page ///////////////////////////////////////////////////////////////////////
  $q=pg_query($db,"select * from resources where rid=" . $_GET['rid']);
  $ans=pg_fetch_array($q);
  include("beginpage.html");
  if ($logged_in == 1)
    print "<FORM METHOD=\"GET\"><INPUT TYPE=\"SUBMIT\" NAME=\"logout\" VALUE=\"Logout\"></FORM>";
  print "<H2>Description for resource " . $ans['rname'] . "</H2>\n";
  if ($_GET['error'] != "")
    print "<span style=\"color: rgb(255,0,0); font-weight: bold\">" . $_GET['error'] . "</span>\n";
  print "<FORM METHOD=\"GET\"><INPUT TYPE=\"TEXT\" SIZE=80 NAME=\"description\" VALUE=\"" . $ans['description'] . "\">"  .
    "<TABLE><TR><TD><INPUT TYPE=\"SUBMIT\" NAME=\"do_modify_description\" VALUE=\"Ok\"></TD>" .
    "<TD><INPUT TYPE=\"SUBMIT\" NAME=\"modify_resource\" VALUE=\"Cancel\"></TD></TR></TABLE>" .
    "<INPUT TYPE=\"HIDDEN\" NAME=\"rid\" VALUE=\"" . $_GET['rid'] . "\"></FORM>\n";
  include("endpage.html");
  /////////////////////////////// Modify Resource Description Page ///////////////////////////////////////////////////////////////////////
}
else if ($_GET['do_modify_hidden'] != "")
{
  $q=pg_query($db,"update resources set hidden=" . (($_GET['hidden'] == "Visible") ? "NULL" : "'*'") . " where rid=" . $_GET['rid']);
  header("Location: $uri?rid=" . $_GET['rid'] . "&modify_resource=yes");
}
else if ($_GET['modify_hidden'] != "")
{
  /////////////////////////////// Modify Resource Hidden/Visible Page ///////////////////////////////////////////////////////////////////////
  $q=pg_query($db,"select * from resources where rid=" . $_GET['rid']);
  $ans=pg_fetch_array($q);
  include("beginpage.html");
  if ($logged_in == 1)
    print "<FORM METHOD=\"GET\"><INPUT TYPE=\"SUBMIT\" NAME=\"logout\" VALUE=\"Logout\"></FORM>";
  print "<H2>Hidden setting for resource " . $ans['rname'] . "</H2>\n";
  print "<FORM METHOD=\"GET\"><SELECT NAME=\"hidden\"><OPTION" . (($ans['hidden'] == "") ? " SELECTED" : "") . ">Visible</OPTION><OPTION" .
    (($ans['hidden'] != "") ? " SELECTED" : "") . ">Hidden</OPTION></SELECT>" .
    "<INPUT TYPE=\"SUBMIT\" NAME=\"do_modify_hidden\" VALUE=\"Ok\">" .
    "<INPUT TYPE=\"SUBMIT\" NAME=\"modify_resource\" VALUE=\"Cancel\">" .
    "<INPUT TYPE=\"HIDDEN\" NAME=\"rid\" VALUE=\"" . $_GET['rid'] . "\">" .
    "</FORM>\n";
  include("endpage.html");
  /////////////////////////////// Modify Resource Hidden/Visible Page ///////////////////////////////////////////////////////////////////////
}
else if ($_GET['do_modify_request_help'] != "")
{
  $q=pg_query($db,"select * from resources where rid=" . $_GET['rid']);
  $ans=pg_fetch_array($q);
  if (($ans['hidden'] == "") && ($_GET['request_help'] == ""))
  {
    header("Location: $uri?rid=" . $_GET['rid'] . "&modify_request_help=yes&error=" . urlencode("This resource is not hidden so you must supply request help"));
    return;
  }
  $q=pg_query($db,"update resources set request_help='" . $_GET['request_help'] . "' where rid=" . $_GET['rid']);
  header("Location: $uri?rid=" . $_GET['rid'] . "&modify_resource=yes");
}
else if ($_GET['modify_request_help'] != "")
{
  /////////////////////////////// Modify Request Help Page ///////////////////////////////////////////////////////////////////////
  $q=pg_query($db,"select * from resources where rid=" . $_GET['rid']);
  $ans=pg_fetch_array($q);
  include("beginpage.html");
  if ($logged_in == 1)
    print "<FORM METHOD=\"GET\"><INPUT TYPE=\"SUBMIT\" NAME=\"logout\" VALUE=\"Logout\"></FORM>";
  print "<H2>Request Help for resource " . $ans['rname'] . "</H2>\n";
  print "Specify some information which will be useful for a user when requesting permissions on a resource. For example you may want \n" .
    "the user to include what type of access he needs or some other additional information which would help you determine whether to \n" .
    "to grant the permission or not.<P \>Note: You do not need request help if the resource is hidden.\n";
  if ($_GET['error'] != "")
    print "<span style=\"color: rgb(255,0,0); font-weight: bold\">" . $_GET['error'] . "</span>\n";
  print "<FORM METHOD=\"GET\"><TEXTAREA COLS=80 ROWS=20 NAME=\"request_help\">" . $ans['request_help'] . "</TEXTAREA>\n" .
    "<TABLE><TR><TD><INPUT TYPE=\"SUBMIT\" NAME=\"do_modify_request_help\" VALUE=\"Ok\"></TD>" .
    "<TD><INPUT TYPE=\"SUBMIT\" NAME=\"modify_resource\" VALUE=\"Cancel\"></TD></TR>" .
    "<INPUT TYPE=\"HIDDEN\" NAME=\"rid\" VALUE=\"" . $_GET['rid'] . "\"></FORM>\n";
  include("endpage.html");
  /////////////////////////////// Modify Request Help Page ///////////////////////////////////////////////////////////////////////
}
else if ($_GET['do_modify_inquiry_help'] != "")
{
  $q=pg_query($db,"select * from resources where rid=" . $_GET['rid']);
  $ans=pg_fetch_array($q);
  $q=pg_query($db,"update resources set inquiry_help='" . $_GET['inquiry_help'] . "' where rid=" . $_GET['rid']);
  header("Location: $uri?rid=" . $_GET['rid'] . "&modify_resource=yes");
}
else if ($_GET['modify_inquiry_help'] != "")
{
  /////////////////////////////// Modify Inquire Help Page ///////////////////////////////////////////////////////////////////////
  $q=pg_query($db,"select * from resources where rid=" . $_GET['rid']);
  $ans=pg_fetch_array($q);
  include("beginpage.html");
  if ($logged_in == 1)
    print "<FORM METHOD=\"GET\"><INPUT TYPE=\"SUBMIT\" NAME=\"logout\" VALUE=\"Logout\"></FORM>";
  print "<H2>Inquiry Help for resource " . $ans['rname'] . "</H2>\n";
  print "Specify some information which will be useful for a user when requesting permissions on a resource. For example you may want \n" .
    "the user to include what type of access he needs or some other additional information which would help you determine whether to \n" .
    "to grant the permission or not.<P \>Note: You do not need request help if the resource is hidden.\n";
  print "<FORM METHOD=\"GET\"><TEXTAREA COLS=80 ROWS=20 NAME=\"inquiry_help\">" . $ans['inquiry_help'] . "</TEXTAREA>\n" .
    "<TABLE><TR><TD><INPUT TYPE=\"SUBMIT\" NAME=\"do_modify_inquiry_help\" VALUE=\"Ok\"></TD>" .
    "<TD><INPUT TYPE=\"SUBMIT\" NAME=\"modify_resource\" VALUE=\"Cancel\"></TD></TR>" .
    "<INPUT TYPE=\"HIDDEN\" NAME=\"rid\" VALUE=\"" . $_GET['rid'] . "\"></FORM>\n";
  include("endpage.html");
  /////////////////////////////// Modify Inquire Help Page ///////////////////////////////////////////////////////////////////////
}
else if ($_GET['do_modify_admin_help'] != "")
{
  $q=pg_query($db,"select * from resources where rid=" . $_GET['rid']);
  $ans=pg_fetch_array($q);
  if ($_GET['admin_help'] == "")
  {
    header("Location: $uri?rid=" . $_GET['rid'] . "&modify_admin_help=yes&error=" . urlencode("You must supply admin help"));
    return;
  }
  $q=pg_query($db,"update resources set admin_help='" . $_GET['admin_help'] . "' where rid=" . $_GET['rid']);
  header("Location: $uri?rid=" . $_GET['rid'] . "&modify_resource=yes");
}
else if ($_GET['modify_admin_help'] != "")
{
  /////////////////////////////// Modify Admin Help Page ///////////////////////////////////////////////////////////////////////
  $q=pg_query($db,"select * from resources where rid=" . $_GET['rid']);
  $ans=pg_fetch_array($q);
  include("beginpage.html");
  if ($logged_in == 1)
    print "<FORM METHOD=\"GET\"><INPUT TYPE=\"SUBMIT\" NAME=\"logout\" VALUE=\"Logout\"></FORM>";
  print "<H2>Administration Help for resource " . $ans['rname'] . "</H2>\n";
  print "Specify some information which will be useful for resource administrator. Include information about authorization policies \n" .
    "related to this resource including other auxilliary resource permissions that may be required in addition to permissions on this \n" .
    " resource. Note: This help is required.\n";
  if ($_GET['error'] != "")
    print "<span style=\"color: rgb(255,0,0); font-weight: bold\">" . $_GET['error'] . "</span>\n";
  print "<FORM METHOD=\"GET\"><TEXTAREA COLS=80 ROWS=20 NAME=\"admin_help\">" . $ans['admin_help'] . "</TEXTAREA>\n" .
    "<TABLE><TR><TD><INPUT TYPE=\"SUBMIT\" NAME=\"do_modify_admin_help\" VALUE=\"Ok\"></TD>" .
    "<TD><INPUT TYPE=\"SUBMIT\" NAME=\"modify_resource\" VALUE=\"Cancel\"></TD></TR>" .
    "<INPUT TYPE=\"HIDDEN\" NAME=\"rid\" VALUE=\"" . $_GET['rid'] . "\"></FORM>\n";
  include("endpage.html");
  /////////////////////////////// Modify Admin Help Page ///////////////////////////////////////////////////////////////////////
}
else if ($_GET['done_modify_resource'] != "")
{
  $q=pg_query($db,"select * from resources where rid=" . $_GET['rid']);
  $ans=pg_fetch_array($q);
  $return_url=$uri . "?modify_resource=yes&rid=" . $_GET['rid'];
  if ($ans['description'] == "")
  {
    header("Location: $return_url&error=" . urlencode("Please fill in a description for this resource."));
    return;
  }
  if ($ans['hidden'] == "No")
  {
    if ($ans['request_help'] == "")
    {
      header("Location: $return_url&error=" . urlencode("Please provide some help text to aid a resource requestor."));
      return;
    }
  }
  if ($ans['admin_help'] == "")
  {
    header("Location: $return_url&error=" . urlencode("Please fill in some adminitration help."));
    return;
  }
  header("Location: $uri?rid="  . $_GET['rid'] . "&modify_authorizations=yes");
  return;
}
else if ($_GET['modify_resource'] != "")
{
  /////////////////////////////// Resource Properties Page ///////////////////////////////////////////////////////////////////////
  $q_res=pg_query($db,"select * from resources where rid=" . $_GET['rid']);
  $res=pg_fetch_array($q_res);
  $q_auth=pg_query("select * from resourceauthorizations where resource='" .  addslashes($res['rname']) . "' and uid=" . $user['uid'] . " and permit='Admin'");
  if (pg_num_rows($q_auth) == 1)
  {
    include("beginpage.html");
    if ($logged_in == 1)
      print "<FORM METHOD=\"GET\"><INPUT TYPE=\"SUBMIT\" NAME=\"logout\" VALUE=\"Logout\"></FORM>";
    print "<H2>Properties for resource " . $res['rname'] . "</H2><P \>\n";
    print "<FORM METHOD=\"GET\"><INPUT TYPE=\"HIDDEN\" NAME=\"rid\" VALUE=\"" . $_GET['rid'] . "\">" .
      "<INPUT CLASS=\"managebutton\" TYPE=\"SUBMIT\" VALUE=\"Back\" name=\"done_modify_resource\"</FORM><P \>\n";
    if ($_GET['error'] != "")
      print "<span style=\"color: rgb(255,0,0); font-weight: bold\">" . $_GET['error'] . "</span>\n";
    print "<FORM METHOD=\"GET\">" .
      "<TABLE BORDER=1>\n" .
      "<TR><TD><B>Description:</B></TD><TD>" . $res['description'] . "</TD><FORM METHOD=\"GET\"><TD>" . 
      "<INPUT CLASS=\"managebutton\" TYPE=\"Submit\" name=\"modify_description\" value=\"Modify\">" .
      "<INPUT TYPE=\"HIDDEN\" name=\"rid\" value=\"" . $_GET['rid'] . "\"></TD></FORM></TR>\n" .
      "<TR><TD><B>Hidden:</B></TD><TD>" . (($res['hidden']=="") ? "No" : "Yes") . "</TD><FORM METHOD=\"GET\"><TD>" .
      "<INPUT CLASS=\"managebutton\" TYPE=\"Submit\" name=\"modify_hidden\" value=\"Modify\">" .
      "<INPUT TYPE=\"HIDDEN\" name=\"rid\" value=\"" . $_GET['rid'] . "\"></TD></FORM></TR>\n" .
      "<TR><TD COLSPAN=2><B>Request helper string. Presented to user when requesting authorization for this resource</B></TD><TD></TD></TR>\n" .
      "<TR><FORM METHOD=\"GET\"><TD COLSPAN=2><TEXTAREA ROWS=10 COLS=80 READONLY>" . $res['request_help'] . "</TEXTAREA></TD><TD>" .
      "<INPUT CLASS=\"managebutton\" TYPE=\"Submit\" name=\"modify_request_help\" value=\"Modify\">" .
      "<INPUT TYPE=\"HIDDEN\" name=\"rid\" value=\"" . $_GET['rid'] . "\"></TD></FORM></TR>\n" .
      "<TR><TD COLSPAN=2><B>Inquiry helper string. Presented to user when submitting an inquiry for this resource</B></TD><TD></TD></TR>\n" .
      "<TR><FORM METHOD=\"GET\"><TD COLSPAN=2><TEXTAREA ROWS=10 COLS=80 READONLY>" . $res['inquiry_help'] . "</TEXTAREA></TD><TD>" .
      "<INPUT CLASS=\"managebutton\" TYPE=\"Submit\" name=\"modify_inquiry_help\" value=\"Modify\">" .
      "<INPUT TYPE=\"HIDDEN\" name=\"rid\" value=\"" . $_GET['rid'] . "\"></TD></FORM></TR>\n" .
      "<TR><TD COLSPAN=2><B>Admin helper string. Should contain information regarding permission requirement and resource policies</B></TD><TD></TD></TR>\n" .
      "<TR><FORM METHOD=\"GET\"><TD COLSPAN=2><TEXTAREA ROWS=10 COLS=80 READONLY>" . $res['admin_help'] . "</TEXTAREA></TD><TD>" .
      "<INPUT CLASS=\"managebutton\" TYPE=\"Submit\" name=\"modify_admin_help\" value=\"Modify\">" .
      "<INPUT TYPE=\"HIDDEN\" name=\"rid\" value=\"" . $_GET['rid'] . "\"></TD></FORM></TR>\n" .
      "</TABLE>\n";
    print "<HR />\n" .
      "<FORM METHOD=\"GET\"><INPUT TYPE=\"HIDDEN\" name=\"rid\" value=\"" . $_GET['rid'] . "\"><TABLE CLASS=\"clear\"><TR><TD><H3>Valid Permissions for this Resource</H3></TD>\n" .
      "<TD style=\"vertical-align: middle; text-align: right;\">" .
      "<INPUT CLASS=\"managebutton\" TYPE=\"SUBMIT\" VALUE=\"Add New\" name=add_permission></TD></TR></TABLE></FORM>\n";
    print "<TABLE border=1>\n" .
      "<TR><TD><B>Permission</B></TD><TD><B>Description</B></TD></TR>\n";
    $q_perms=pg_query($db,"select * from resourcepermissions r inner join permissions p on r.pid=p.pid where rid=" . $_GET['rid'] . " order by permission");
    while(($perm=pg_fetch_array($q_perms)))
    {
      $q_auth=pg_query($db,"select * from resourceauthorizations where resource='" . addslashes($res['rname']) . "' and permit='" . addslashes($perm['permission']) . "'");
      print "<TR><TD CLASS=\"small\">" . $perm['permission'] . "</TD><TD CLASS=\"small\">" . $perm['pdescription'] . "</TD>" .
	((pg_num_rows($q_auth) == 0) ? ("<FORM METHOD=\"GET\"><TD CLASS=\"small\"><INPUT CLASS=\"managebutton\" TYPE=\"SUBMIT\" NAME=\"delete_rpermission\" VALUE=\"Delete\"></TD></TR>" .
					"<INPUT TYPE=\"HIDDEN\" name=rpid value=" . $perm['rpid'] . ">" .
					"<INPUT TYPE=\"HIDDEN\" name=rid value=" . $_GET['rid'] . "></FORM>\n") : "<TD CLASS=\"small\"></TD></TR>\n");
    }
    print "</TABLE>";
    print "<FORM METHOD=\"GET\"><INPUT TYPE=\"HIDDEN\" NAME=\"rid\" VALUE=\"" . $_GET['rid'] . "\">" .
      "<INPUT CLASS=\"managebutton\" TYPE=\"SUBMIT\" VALUE=\"Back\" name=\"done_modify_resource\"</FORM>\n";
    include("endpage.html");
  }
  else
  {
    header("Location: $uri");
  }
  /////////////////////////////// Resource Properties Page ///////////////////////////////////////////////////////////////////////
}
else if ($_GET['delete_authorization'] != "")
{
  $q=pg_query("select * from resourceauthorizations where aid=" . $_GET['aid']);
  $a=pg_fetch_array($q);
  $q=pg_query("select * from users where uid=" . $a['uid']);
  $duser=pg_fetch_array($q);
  $qauth=pg_query($db,"select * from resourceauthorizations r join users u on r.uid=u.uid where resource='" . 
		  addslashes($a['resource']) . "' and permit='Admin'");
  $message=$user['first_name'] . " " . $user['last_name'] . " has revoked " . $a['permit'] . " permission for resource " . $a['resource'] . " from " .
    "user " . $duser['first_name'] . " " . $duser['last_name'] . "\n\n" .
    "User information\n\n" .
    "First name:  " . $duser['first_name'] . "\n" . 
    "Last name:   " . $duser['last_name'] . "\n" .
    "Affiliation: " . $duser['affiliation'] . "\n" .
    "Email:       " . $duser['email'] . "\n" .
    "Phone:       " . $duser['phone'] . "\n" .
    "Fax:         " . $duser['fax'] . "\n" .
    "Mailing:     " . $duser['mailing'] . "\n" .
    "Grid ID:     " . $duser['gridid'] . "\n\n";
  while (($admin=pg_fetch_array($qauth)))
  {
    if ($admin['email'] != "")
    {
      mail($admin['email'],"$system_name Resource Permission Revoked for resource " . $a['resource'],$message,"From: " . $user['email'] . "\r\n" .
	     "Reply-To: " . $user['email'] . "\r\n" . "X-Mailer: PHP/" . phpversion());
    }
  }
  pg_query("delete from resourceauthorizations where aid='" . $_GET['aid'] . "'");
  header("Location: $uri?send_user_email=Revoked&rid=" . $_GET['rid'] . "&email=" . urlencode($duser['email']) . "&permit=" . urlencode($a['permit']) .
	     "&rname=" . urlencode($a['resource']));
}
else if ($_GET['show_resource_admin_log'])
{
  /////////////////////////////// Resource Log Page - admin ///////////////////////////////////////////////////////////////////////
  include("beginpage.html");
  if ($logged_in == 1)
    print "<FORM METHOD=\"GET\"><INPUT TYPE=\"SUBMIT\" NAME=\"logout\" VALUE=\"Logout\"></FORM>";
  $q=pg_query($db,"set datestyle to sql");
  $q=pg_query($db,"select rname from resources where rid=" . $_GET['rid']);
  $ans=pg_fetch_array($q);
  print "<H2>Resource check log for resource " . $ans['rname'] . "</H2>\n";
  print "<FORM METHOD=\"GET\"><INPUT TYPE=\"HIDDEN\" NAME=\"rid\" VALUE=\"" . $_GET['rid'] . "\">" .
      "<INPUT CLASS=\"managebutton\" TYPE=\"SUBMIT\" VALUE=\"Back\"></FORM><P \>\n";
  $q=pg_query($db,"select * from checklog where rname='" . $ans['rname'] . "' order by time desc limit 100");
  print "<TABLE CLASS=\"wide\"><TR><TD><CENTER><B>Time</B></CENTER></TD><TD><B><CENTER>Permission</CENTER></B></TD><TD><B><CENTER>GridId</CENTER></B></TD><TD><B><CENTER>Result</CENTER></B></TD></TR>\n";
  while (($ans=pg_fetch_array($q)))
  {
    print "<TR style=\"color: rgb(" . (($ans['result'] == 'no') ? "255,0,0" : "0, 153, 0") . "); text-align: center;\"><TD CLASS=\"small\">" . $ans['time'] . "</TD><TD CLASS=\"small\">" . 
      $ans['permit'] . "</SMALL></TD><TD CLASS=\"small\">" . $ans['gridid'] . "</TD><TD CLASS=\"small\">" . $ans['result'] . "</TD><TR>\n";
  }
  print "</TABLE>\n";
  print "<FORM METHOD=\"GET\"><INPUT TYPE=\"HIDDEN\" NAME=\"rid\" VALUE=\"" . $_GET['rid'] . "\">" .
      "<INPUT CLASS=\"managebutton\" TYPE=\"SUBMIT\" VALUE=\"Back\"></FORM>\n";
  include("endpage.html");
  /////////////////////////////// Resource Log Page - admin ///////////////////////////////////////////////////////////////////////
}
else if ($_GET['show_resource_log'])
{
  /////////////////////////////// Resource Log Page - non admin ///////////////////////////////////////////////////////////////////////
  include("beginpage.html");
  if ($logged_in == 1)
    print "<FORM METHOD=\"GET\"><INPUT TYPE=\"SUBMIT\" NAME=\"logout\" VALUE=\"Logout\"></FORM>";
  $q=pg_query($db,"set datestyle to sql");
  $q=pg_query($db,"select rname from resources where rid=" . $_GET['rid']);
  $ans=pg_fetch_array($q);
  print "<H2>Resource check log for resource " . $ans['rname'] . "</H2>\n";
  print "<FORM METHOD=\"GET\"><INPUT TYPE=\"HIDDEN\" NAME=\"rid\" VALUE=\"" . $_GET['rid'] . "\">" .
      "<INPUT CLASS=\"managebutton\" TYPE=\"SUBMIT\" VALUE=\"Back\"></FORM><P \>\n";
  $q=pg_query($db,"select * from checklog where rname='" . $ans['rname'] . "' and gridid='" . $user['gridid'] . "' order by time desc limit 100");
  print "<TABLE><TR><TD><B><CENTER>Time</CENTER></B></TD><TD><B><CENTER>Permission</CENTER></B></TD><TD><B><CENTER>Result</CENTER></B></TD></TR>\n";
  while (($ans=pg_fetch_array($q)))
  {
    print "<TR style=\"color: rgb(" . (($ans['result'] == 'no') ? "255,0,0" : "0, 153, 0") . "); text-align: center;\"><TD CLASS=\"small\">" . $ans['time'] . 
      "</TD><TD CLASS=\"small\">" . $ans['permit'] . "</TD><TD CLASS=\"small\">" . $ans['result'] . "</TD><TR>\n";
  }
  print "</TABLE>\n";
  print "<FORM METHOD=\"GET\"><INPUT TYPE=\"HIDDEN\" NAME=\"rid\" VALUE=\"" . $_GET['rid'] . "\">" .
      "<INPUT CLASS=\"managebutton\" TYPE=\"SUBMIT\" VALUE=\"Back\"></FORM><P \>\n";
  include("endpage.html");
  /////////////////////////////// Resource Log Page - non admin ///////////////////////////////////////////////////////////////////////
}
else if ($_GET['view_cred'] != "")
{
  include("beginpage.html");
  print "<H1>My $system_name Certificates</H1>\n" .
    "<P />The following is the list of certificates that are currently mapped to this ROAM user. You can use any of these\n" .
    "certficates to connect to $system_name services.\n<P /><TABLE>\n";
  $q = pg_query($db,"select gridid from users where uid=" . $user['uid'] . " or primary_uid=" . $user['uid'] . " order by primary_uid desc");
  while (($ans = pg_fetch_array($q)))
  {
    print "<TR><TD CLASS=\"small\">" . $ans['gridid'] . "</TD></TR>\n";
  }
  print "</TABLE>\n";
  print "<FORM METHOD=\"GET\"><INPUT CLASS=\"managebutton\" TYPE=\"SUBMIT\" VALUE=\"Back\"></FORM><P \>\n";
  include("endpage.html");
}
else
{
  /////////////////////////////// Starting Page ///////////////////////////////////////////////////////////////////////
  include("beginpage.html");
  if ($logged_in == 1)
    print "<FORM METHOD=\"GET\"><INPUT TYPE=\"SUBMIT\" NAME=\"logout\" VALUE=\"Logout\"></FORM>";
  print "<H2>Welcome " . $user['first_name'] . " " . $user['last_name'] . " to the $system_name Resource Authorization System</H2><P /><P />\n";
  print "<H3>You are currently authorized for the following resources:</H3><P />\n";
  $q = pg_query($db,"select * from resourceauthorizations a join resources r on a.resource = r.rname where uid=" . $user['uid'] . " and (hidden is NULL or permit='Admin') order by resource");
  print "<TABLE border=1><TR><TD><B>Resource</B></TD><TD><B>Permission</B></TD><TD><B>Description</B></TD><TD><B>Action</B></TD><TD><B>Logs</B></TD></TR>\n";
  while (($auth = pg_fetch_array($q)))
  {
    print "<TR><TD CLASS=\"small\" style=\"background-color: " . 
      (($auth['permit'] == "Admin") ? (($auth['hidden'] == "") ? "rgb(176,255,255)" : "rgb(223,255,255)") : "rgb(153, 255, 153)") .
      ";\">" . $auth['resource'] . "</TD><TD CLASS=\"small\">" . $auth['permit'] . "</TD>\n" .
      "<TD CLASS=\"small\">" . $auth['description'] . "</TD>\n" .
      "<TD CLASS=\"small\"><FORM METHOD=\"GET\"><INPUT TYPE=\"hidden\" name=\"rid\" value=\"" . $auth['rid'] . "\">" .
      "<INPUT CLASS=\"managebutton\" TYPE=\"hidden\" name=\"aid\" value=\"" . $auth['aid'] . "\">" .
      "<INPUT CLASS=\"managebutton\" TYPE=\"hidden\" name=\"permit\" value=\"" . $auth['permit'] . "\">" .
      "<INPUT CLASS=\"managebutton\" TYPE=\"Submit\" " . 
      (($auth['permit'] == "Admin") ? "name=\"modify_authorizations\" value=\"Administer Resource\">" : "name=\"inquiry\" value=\"Submit Inquiry\">")  . "</TD>" .
      "<TD CLASS=\"small\"><INPUT CLASS=\"managebutton\" TYPE=\"Submit\" " . 
      (($auth['permit'] == "Admin") ? "name=\"show_resource_admin_log\"" : "name=\"show_resource_log\"") ." value=\"Show log\">"  . "</TD>" .
      "</FORM>\n" .
      "</TR>\n";
  }
  print "</TABLE>\n<P />\n";
  print "<TABLE CLASS=\"clear\"><TR><TD><FORM METHOD=\"GET\"><INPUT CLASS=\"managebutton\" type=\"Submit\" value=\"Modify Profile\" name=\"modify_profile\"></FORM></TD>" .
    "<TD><FORM METHOD=\"GET\"><INPUT CLASS=\"managebutton\" type=\"Submit\" value=\"Request Resource Rermission\" name=\"request\"></FORM></TD>" .
    "<TD><FORM METHOD=\"GET\"><INPUT CLASS=\"managebutton\" type=\"Submit\" value=\"Create Resource\" name=\"create\"></FORM></TD>" .
    "<TD><FORM METHOD=\"GET\"><INPUT CLASS=\"managebutton\" type=\"Submit\" value=\"View Credentials\" name=\"view_cred\"></FORM></TD>" .
    "</TR></TABLE>\n";
  include("endpage.html");
  /////////////////////////////// Starting Page ///////////////////////////////////////////////////////////////////////
}
?>


