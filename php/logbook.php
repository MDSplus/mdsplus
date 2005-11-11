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


$event_server="alcserv3";

$html_preface="<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n" .
    "    \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n" .
    "<html xmlns=\"http://www.w3.org/1999/xhtml\"  lang=\"en\" xml:lang=\"en\">\n";

$self=$_SERVER["PHP_SELF"];

function fontsize($preferences)
{
  switch ($preferences['display']['fontsize'])
  {
  case 2: return "medium"; break;
  case 3: return "large"; break;
  default: return "small"; break;
  }
}

function fixentry($text)
{
  $offset=0;
  $ans='';
  $lower=strtolower($text);
  while(!(($pos=strpos($lower,"<pre>",$offset)) === FALSE))
  {
    $ans=$ans . str_replace(chr(10),chr(255),substr($text,$offset,$pos-$offset));
    $offset=$pos+5;
    if (($pos=strpos($lower,"</pre>",$offset)) === FALSE)
    {
      $ans=$ans . substr($text,$offset-5);
      $offset=strlen($text);
      break;
    }
    else
    {
      $ans=$ans . substr($text,$offset-5,$pos+6-$offset+5);
      $offset=$pos+6;
    }
  }
  if ($offset < strlen($text))
    $ans = $ans . str_replace(chr(10),chr(255),substr($text,$offset));
  return str_replace(chr(255),"<br />",$ans);
}

function fieldchecks($numeric_only)
{
   return  "<script language=\"JAVASCRIPT\">\n" .
     "function checkrun()\n" .
     "{ if (!$numeric_only && window.document.form1.run.value == \"Current\")\n" .
     "    return  true;\n" .
     "  else if (window.document.form1.run.value == \"\")\n" .
     "    return true;\n" .
     "  else if (((window.document.form1.run.value-0) >= 910000) && \n" .
     "           ((window.document.form1.run.value-0) <= 1100000)) \n" .
     "    return true;\n" .
     "  else\n" .
     "  {\n" .
     "    alert(" . ($numeric_only ? "\"Invalid run specified\\nUser a date in [1]yymmdd format or leave blank.\"" :
		     "\"Invalid run specified\\nUse 'Current', a date in [1]yymmdd format or leave blank.\"") . ");\n" .
     "    return false;\n" .
     "  }\n" .
     "}\n" .
     "function checkshot()\n" .
     "{ if (!$numeric_only && window.document.form1.shot.value == \"Cur\")\n" .
     "    return  true;\n" .
     "  else if (window.document.form1.shot.value == \"\")\n" .
     "    return true;\n" .
     "  else if (((window.document.form1.shot.value-0) > 0) && \n" .
     "           ((window.document.form1.shot.value-0) < 999)) \n" .
     "    return true;\n" .
     "  else\n" .
     "  {\n" .
     "    alert(" . ($numeric_only ? "'Invalid shot specified\\nUse a number between 1-999 or leave blank.'" : 
		     "\"Invalid shot specified\\nUse 'Cur', a number between 1-999 or leave blank.\"") . ");\n" .
     "    return false;\n" .
     "  }\n" .
     "}\n" .
     "function checktext()\n" .
     "{ if (window.document.form1.text.value == '')\n" .
     "  {\n" .
     "    alert('No text supplied');\n" .
     "    return false;\n" .
     "  }\n" .
     "  else\n" .
     "  {\n" .
     "    if (window.document.form1.text.value == window.document.form1.text.defaultValue)\n" .
     "      return(confirm('Text may not have been changed Are you sure you want to make this entry?'));\n" .
     "  }\n" .
     "  return true;\n" .
     "}\n" .
     "</script>\n";
}
function preferences()
{
  include 'logbook_dbconnect.php';
  $prefs=array();
  $q=mssql_query("select * from entry_display_prefs where username=suser_sname()");
  if (mssql_num_rows($q) == 0)
  {
    $q=mssql_query("insert into entry_display_prefs (run) values (0)");
    $q=mssql_query("select * from entry_display_prefs where username=suser_sname()");
  }
  $ans=mssql_fetch_array($q);
  $prefs['selection'] = array("run" => $ans['run'], 
			      "topics" => $ans['topics'],
			      "user" => $ans['user_select'],
			      "show_voided" => $ans['show_voided']);
  $prefs['display'] = array("sort by shot" => $ans['sort_by_shot'], 
			    "ascending" => $ans['ascending'], 
			    "null shots first" => $ans['null_shots_first'],
			    "updates" => $ans['auto_update'],
			    "auto scroll" => $ans['auto_scroll'],
			    "scroll top" => $ans['scroll_top'],
			    "limit" => $ans['max_rows'],
			    "fontsize" => $ans['fontsize']);
  return $prefs;
}

function setevent()
{
  global $event_server;
  $handle=mdsplus_connect($event_server);
  $dummy=mdsplus_value($handle,'setevent("LOGBOOK_ENTRY")');
  mdsplus_disconnect($handle);
}

if ((array_key_exists('setprefs',$_GET)) || (array_key_exists('setprefs_select_all',$_GET)) || (array_key_exists('setprefs_clear_all',$_GET)))
{
  $preferences=preferences();
  print $html_preface .
    "<head>\n" .
    "<title>Logbook Selecletion Preferences</title>\n" .
    "<link rel=\"stylesheet\" type=\"text/css\" href=\"logbook.css\" media=\"screen\" />\n" .
    "</head><body class=\"" . fontsize($preferences) . "\">\n";
  print "<form name=\"prefs\" method=\"get\">\n";
  switch($preferences['selection']['run'])
  {
  case -1: 
    $run="Previous"; 
    break;
  case 0:  
    $run="Current"; 
    break;
  case 1:  
    $run="Next"; 
    break;
  default: 
    $run=$preferences['selection']['run']; 
    break;
  }

  $voided_1='checked ';
  $voided_2='';
  $voided_3='';
  switch ($preferences['selection']['show_voided'])
  {
  case 'O':
    $voided_1='';
    $voided_2='checked ';
    break;
  case 'B':
    $voided_1='';
    $voided_3='checked ';
    break;
  }
  print "Select RUN = <input type=text name=RUN value=\"" . $run . "\" size=12 /> and entries are";
  print "<input type=RADIO name=VOIDED $voided_1 VALUE=\"N\" /><label>Not Voided</label>\n";
  print "<input type=RADIO name=VOIDED $voided_2 VALUE=\"O\" /><label>Voided</label>\n";
  print "<input type=RADIO name=VOIDED $voided_3 VALUE=\"B\" /><label>Both</label><br />\n";
  print "and Username = <input type=text name=USER value=\"" . $preferences['selection']['user'] . "\" size=20 /><br />\n";
  print "or Topic = <br />\n";
  $q=mssql_query("select topic from topics order by topic");
  $n=0;
  $topics=array();
  while ($ans=mssql_fetch_array($q)) $topics[$n++]=rtrim($ans['topic']);
  $nrows=ceil($n/4);
  $k=0;
  print "<table>\n";
  for ($i=0;$i<$nrows;$i++)
  {
    print "<tr>\n";
    for ($j=0;$j<4;$j++)
    {
      $topic =  $topics[$j * $nrows + $i];
      $checked = ! (strstr($preferences['selection']['topics'],"'" . $topic . "'") === FALSE);
      print "<td class=\"smallcheckbox\"><input type=CHECKBOX " . ($checked ? " checked " : "") . "name=\"" . $topic . "\" />" . $topic . "</td>\n";
    }
    print "</tr>\n";
  }
  print "</table>\n";
  print "<input type=submit name=\"setprefs_apply\" value=\"Ok\" />\n" .
    "<input type=submit value=\"Reset\" name=\"setprefs\" />\n" .
    "<input type=submit value=\"Cancel\" />\n" .
    "<input type=button " .
    " onclick=\"for (i=0;i<window.document.prefs.length;i++)" .
    " {if (window.document.prefs.elements[i].type=='checkbox'  && " .
    "!window.document.prefs.elements[i].checked) {window.document.prefs.elements[i].click()};}\" " .
    " value=\"Select All\" name=\"setprefs_select_all\" />\n" .
    "<input type=button " .
    " onclick=\"for (i=0;i<window.document.prefs.length;i++)" .
    " {if (window.document.prefs.elements[i].type=='checkbox'  && " .
    "window.document.prefs.elements[i].checked) {window.document.prefs.elements[i].click()};}\" " .
    " value=\"Clear All\" name=\"setprefs_clear_all\" />\n" .
    "</form>\n";
  print "</body></html>\n";
}
else if (array_key_exists('setprefs_apply',$_GET))
{
  $preferences=preferences();
  switch (strtoupper($_GET['RUN']))
    {
    case 'PREVIOUS':
      $run=-1;
      break;
    case 'CURRENT':
      $run=0;
      break;
    case 'NEXT':
      $run=1;
      break;
    default:
      $run=$_GET['RUN'];
      break;
    }
  $q=mssql_query("select topic from topics order by topic");
  $topics=array();
  $n=0;
  while($ans=mssql_fetch_array($q))
  {
    if (isset($_GET[rtrim($ans['topic'])]))
    {
      $topics[$n++]=rtrim($ans['topic']);
    }
  }
  if (count($topics) == 0)
    $topics_list='';
  else
  {
    $topics_list="(";
    for ($i=0;$i<count($topics)-1;$i++)
    {
      $topics_list = $topics_list . "'" . $topics[$i] . "',";
    }
    $topics_list = $topics_list . "'" . $topics[$i] . "')";
  }
  $q="update entry_display_prefs set run=$run,topics=\"$topics_list\",user_select=\"" . 
    trim($_GET['USER']) . "\",show_voided=\"" . $_GET['VOIDED'] . "\" where username=suser_sname()";
  $q=mssql_query($q);
  header("location: " .  $_SERVER["PHP_SELF"] . "?main_display");
}
else if (array_key_exists('setdisprefs',$_GET))
{
  $preferences=preferences();
  print "<html><body class=\"" . fontsize($preferences) . "\">\n";
  print "<form method=\"get\">\n";
  print "<table border=\"1\">\n";
  print "<tr><td><table><tr><td>Query sorted by</td>\n" .
    "<td><input type=RADIO name=SORT " . ($preferences['display']['sort by shot'] ? "checked " : "") . "VALUE=\"1\" /><label>shot</label>\n" .
    "<input type=RADIO name=SORT " . ($preferences['display']['sort by shot'] ? "" : "checked ") . 
    "VALUE=\"0\"><label>time entered</label></td></tr></table></td></tr>\n";
  print "<tr><td><table><tr><td>In</td>\n" .
    "<td><input type=RADIO name=ORDER " . ($preferences['display']['ascending'] ? "" : "checked ") . "VALUE=\"0\" /><label>descending</label>\n" .
    "<input type=RADIO name=ORDER " . ($preferences['display']['ascending'] ? "checked " : "") . "VALUE=\"1\" /><label>ascending</label></td><td>order</td></tr></table></td></tr>\n";
  print "<tr><td><table><tr><td>Null shots at</td>\n" .
    "<td><input type=RADIO name=NULLSHOTS " . ($preferences['display']['null shots first'] ? "checked " : "") . "VALUE=\"1\" /><label>at beginning</label>\n" .
    "<input type=RADIO name=NULLSHOTS " . ($preferences['display']['null shots first'] ? "" : "checked ") . "VALUE=\"0\" /><label>at end</label></td></tr></table></td></tr>\n";
  print "<tr><td><table><tr><td>Automatic Updates</td>\n" .
    "<td><input type=RADIO name=UPDATES " . ($preferences['display']['updates'] ? "checked " : "") . "VALUE=\"1\" /><label>on</label>\n" .
    "<input type=RADIO name=UPDATES " . ($preferences['display']['updates'] ? "" : "checked ") . "VALUE=\"0\" /><label>off</label></td></tr></table></td></tr>\n";
  print "<tr><td><table><tr><td>Automatic Scrolling</td>\n" .
    "<td><input type=RADIO name=AUTOSCROLL " . ($preferences['display']['auto scroll'] ? "checked " : "") . "VALUE=\"1\" /><label>on</label>\n" .
    "<input type=RADIO name=AUTOSCROLL " . ($preferences['display']['auto scroll'] ? "" : "checked ") . "VALUE=\"0\" /><label>off</label></td></tr></table></td></tr>\n";
  print "<tr><td><table><tr><td>Scroll to</td>\n" .
    "<td><input type=RADIO name=SCROLL " . ($preferences['display']['scroll top'] ? "checked " : "") . "VALUE=\"1\" /><label>top</label>\n" .
    "<input type=RADIO name=SCROLL " . ($preferences['display']['scroll top'] ? "" : "checked ") . "VALUE=\"0\" /><label>bottom</label></td></tr></table></td></tr>\n";
  print "<tr><td><table><tr><td>Limit to</td>\n" .
    "<td><select  name=LIMIT>\n" . 
    "<option VALUE=10  " . (($preferences['display']['limit'] == "10") ? "SELECTED " : "") . ">10</option>\n" .
    "<option VALUE=20  " . (($preferences['display']['limit'] == "20") ? "SELECTED " : "") . ">20</option>\n" .
    "<option VALUE=40  " . (($preferences['display']['limit'] == "40") ? "SELECTED " : "") . ">40</option>\n" .
    "<option VALUE=80  " . (($preferences['display']['limit'] == "80") ? "SELECTED " : "") . ">80</option>\n" .
    "<option VALUE=160 " . (($preferences['display']['limit'] == "160") ? "SELECTED " : "") . ">160</option>\n" .
    "<option VALUE=320 " . (($preferences['display']['limit'] == "320") ? "SELECTED " : "") . ">320</option>\n" .
    "</select>\n" .
    "</td>\n" .
    "<td>rows</td>\n" .
    "</tr>\n" .
    "</table>\n" .
    "</td>\n" .
    "</tr>\n" .
    "<tr>\n" .
    "<td>Font Size" .
    "<select  name=\"FONTSIZE\">\n" . 
    "<option VALUE=1  " . (($preferences['display']['fontsize'] != "2" && $preferences['display']['fontsize'] != "3") ? "SELECTED " : "") .
    ">Small</option>\n" .
    "<option VALUE=2  " . (($preferences['display']['fontsize'] == "2") ? "SELECTED " : "") . ">Medium</option>\n" .
    "<option VALUE=3  " . (($preferences['display']['fontsize'] == "3") ? "SELECTED " : "") . ">Large</option>\n" .
    "</select>\n" .
    "</td>\n" .
    "</tr>\n" .
    "</table>\n";
  print "<input type=submit name=\"setdisprefs_apply\" value=\"Ok\" />\n" .
    "<input type=submit value=\"Reset\" name=\"setdisprefs\" />\n" .
    "<input type=submit value=\"Cancel\" />\n" .
    "</form>\n";
  print "</body></html>\n";
}
else if (array_key_exists('setdisprefs_apply',$_GET))
{
  $preferences=preferences();
  $q="update entry_display_prefs set sort_by_shot=" . $_GET['SORT'] .",ascending=" . $_GET['ORDER'] .
    ",null_shots_first=" . $_GET['NULLSHOTS'] . ",auto_update=" . $_GET['UPDATES'] . 
    ",auto_scroll=" . $_GET['AUTOSCROLL'] . ",scroll_top=" . $_GET['SCROLL'] .
    ",max_rows=" . $_GET['LIMIT'] . 
    ",fontsize=" . $_GET['FONTSIZE'] . " where username=suser_sname()";
  $q=mssql_query($q);
  $preferences=preferences();
  header("location: " .  $_SERVER["PHP_SELF"] . "?main_display");
}
else if (array_key_exists('newentry',$_GET) || array_key_exists('newentry',$_POST))
{
  if (array_key_exists('newentry',$_POST)) $_GET = $_POST;
  include 'dbconnect.php';
  $q=mssql_query("select max(run) as maxr from runs");
  $ans=mssql_fetch_array($q);
  $maxrun=$ans['maxr'];
  $q=mssql_query("select max(shot) as maxs from shots");
  $ans=mssql_fetch_array($q);
  $maxshot=$ans['maxs'];
  $lastentry="";
  if ($_GET['newentry'] == "make_entry")
  {
    if ($_GET['run'] == "Current")
      $run=$maxrun;
    else if ($_GET['run'] == "")
      $run="NULL";
    else
      $run=$_GET['run'];
    if ($_GET['shot'] == "Cur")
      $shot=$maxshot;
    else if ($_GET['shot'] == "")
      $shot="NULL";
    else
      $shot=($run == "NULL") ? $maxrun * 1000 + $_GET['shot'] : $run * 1000 + $_GET['shot'];
    $cmd="insert into entries (run,shot,topic,text) values ($run,$shot,\"" . $_GET['topic']. "\",'" .
      str_replace("'","''",stripslashes($_GET['text'])) . "')";
    $q=mssql_query($cmd);
    setevent();
    $lastentry="Last new entry submitted at " . strftime("%H:%M:%S") .  " for run $run, shot " . $shot % 1000 . " and topic " . $_GET['topic'];
    if (isset($_GET['clear_text'])) $_GET['text']='';
    setcookie("last_topic",stripslashes($_GET['topic']),time()+60*60*24*30);
  }
  else if (array_key_exists('newtemplate',$_GET) && $_GET['newtemplate'] != "")
  {
    $q=mssql_query("select text from entry_display_templates where username=suser_sname() and template_name='" . 
		   str_replace("'","''",stripslashes(trim($_GET['newtemplate']))) ."'");
    if (mssql_num_rows($q) > 0)
    {
      $ans=mssql_fetch_array($q);
      $_GET['text']=$ans['text'];
      $_GET['template'] = stripslashes($_GET['newtemplate']);
    }
    else
      $_GET['text']='';
  }
  else if (array_key_exists('save_template',$_GET))
  {
    if (trim($_GET['template_name']) == '')
    {
      print "<script language=\"JAVASCRIPT\">alert(\"You must specify a template name\");</script>\n";
    }
    else
    {
      $q="delete from entry_display_templates where username=suser_sname() and template_name='" . 
		     str_replace("'","''",stripslashes(trim($_GET['template_name']))) . "'";
      $q=mssql_query($q);
      $q="insert into entry_display_templates (template_name,text) values ('" .
		     str_replace("'","''",stripslashes(trim($_GET['template_name']))) . "','" .
		     str_replace("'","''",stripslashes($_GET['text'])) . "')";
      $q=mssql_query($q);
      $_GET['template']=stripslashes($_GET['template_name']);
    }
  }
  else if (array_key_exists('delete_template',$_GET))
  {
    if (trim($_GET['template_name']) == '')
    {
      print "<script language=\"JAVASCRIPT\">alert(\"You must specify a template name\");</script>\n";
    }
    else
    {
      $q="delete from entry_display_templates where username=suser_sname() and template_name='" . 
		     str_replace("'","''",stripslashes(trim($_GET['template_name']))) . "'";
      $q=mssql_query($q);
      $_GET['template']='';
    }
  }
  if (array_key_exists('run_c',$_GET))
  {
    $_GET['run']=$maxrun;
  }
  else if (array_key_exists('run_n',$_GET))
  {
    $_GET['run']=$maxrun+1;
  }
  else if (array_key_exists('run_p',$_GET))
  {
    $_GET['run']=$maxrun-1;
  }
  else if (array_key_exists('shot_c',$_GET))
  {
    $_GET['shot']=$maxshot % 1000;
    $_GET['run']=$maxrun;
  }
  else if (array_key_exists('shot_n',$_GET))
  {
    $_GET['shot']=($maxshot+1) % 1000;
    $_GET['run']=$maxrun;
  }
  else if (array_key_exists('shot_p',$_GET))
  {
    $_GET['shot']=($maxshot-1) % 1000;
    $_GET['run']=$maxrun;
  }
  else if ($_GET['shot'] == 'Current+1')
  {
    $_GET['shot']=($maxshot+1) % 1000;
    $_GET['run']=$maxrun;
  }
  else if ($_GET['shot'] == 'Current-1')
  {
    $_GET['shot']=($maxshot-1) % 1000;
    $_GET['run']=$maxrun;
  }
  print $html_preface .
    "<head>\n<title>New Entry</title>\n" .
    "<link rel=\"stylesheet\" type=\"text/css\" href=\"logbook.css\" media=\"screen\" />\n" .
    fieldchecks(0) . "</head><body class=\"" . fontsize($preferences) . "\">\n";
  print "<form name=form1 method=\"post\"><table><tr><td>Run <input type=\"Text\" name=\"run\" size=\"8\" value=\"" . $_GET['run'] . "\" />\n" .
    "<input type=\"TEXT\" name=\"shot\" value=\"" .  $_GET['shot'] . "\" size=\"3\" /> shot</td></tr>\n" .
    "<tr><td>User " . $_SERVER['PHP_AUTH_USER'] . " Topic <select name=\"topic\">\n";
  $q=mssql_query("select topic from topics");
  $last_topic = ($_GET['topic'] == "") ? $_COOKIE['last_topic'] : $_GET['topic'];
  while ($ans=mssql_fetch_array($q))
  {
    print "<option value=\"" . $ans['topic'] . "\"";
    if ($last_topic == $ans['topic']) print " SELECTED";
    print ">" . $ans['topic'] . "</option>\n";
  }
  print "</select></td></tr>\n";
  $q=mssql_query("select template_name from entry_display_templates where username=suser_sname()");
  if (mssql_num_rows($q) > 0)
  {
    $selected=" SELECTED";
    print "<tr><td><input type=\"HIDDEN\" name=\"newtemplate\" VALUE=\"\" />\n" . 
      "<input type=\"HIDDEN\" name=\"template\" VALUE=\"" . $_GET['template'] . "\" /><select name=\"load_template\" " .
      "onChange=\"window.document.form1.newtemplate.value=this.options[this.selectedIndex].value; window.document.form1.submit();\">\n";
    while ($ans=mssql_fetch_array($q))
    {
      if (trim($ans['template_name']) == $_GET['template'])
        $selected="";
      print "<option value=\"" . $ans['template_name'] . "\"";
      if (trim($ans['template_name']) == $_GET['template']) print " SELECTED";
      print ">" . $ans['template_name'] . "</option>\n";
    }
    print "<option  value=\"\" $selected>Select Template</option>\n</select></td></tr>\n";
  }
  print "<tr><td><TEXTAREA COLS=80 ROWS=20 name=\"text\">" . 
    stripslashes($_GET['text']) . "</TEXTAREA></td><tr>\n" .
    "<tr><td>Run\n" .
    "<input type=\"Hidden\" name=\"newentry\" />\n" .
    "<input type=\"BUTTON\" class=\"button\" onclick=\"window.document.form1.run.value='Current';\" value=\"T\" title=\"Track Current\" />\n" .
    "<input type=\"SUBMIT\" class=\"button\" name=\"run_c\" value=\"C\" title=\"Current\" />\n" .
    "<input type=\"SUBMIT\" class=\"button\" name=\"run_p\" value=\"P\" title=\"Previous\" />\n" .
    "<input type=\"SUBMIT\" class=\"button\" name=\"run_n\" value=\"N\" title=\"Next\" />\n" .
    "<input type=\"BUTTON\" class=\"button\" onclick=\"if (window.document.form1.run.value == 'Current')" .
    "{window.document.form1.run.value='Current+1'; window.document.form1.submit();} " .
    "else if ((window.document.form1.run.value/window.document.form1.run.value == 1) || ((window.document.form1.run.value-0) == 0)) " .
    "window.document.form1.run.value=(window.document.form1.run.value-0)+1;\" value=\"I\" title=\"Increment\" />\n" .
    "<input type=\"BUTTON\" class=\"button\" onclick=\"if (window.document.form1.run.value == 'Current')" .
    "{window.document.form1.run.value='Current-1'; window.document.form1.submit();} " .
    "else if ((window.document.form1.run.value/window.document.form1.run.value == 1) || ((window.document.form1.run.value-0) == 0))" .
    "window.document.form1.run.value=window.document.form1.run.value-1;\" value=\"D\" title=\"Decrement\" />\n" .
    "<input type=\"BUTTON\" class=\"button\" onclick=\"window.document.form1.run.value=''; window.document.form1.shot.value='';\" value=\"X\" title=\"None\" />\n" .
    "Shot\n" . 
    "<input type=\"BUTTON\" class=\"button\" onclick=\"window.document.form1.shot.value='Cur';\" value=\"T\" title=\"Track Current\" />\n" .
    "<input type=\"SUBMIT\" class=\"button\" name=\"shot_c\" value=\"C\" title=\"Current\" />\n" .
    "<input type=\"SUBMIT\" class=\"button\" name=\"shot_p\" value=\"P\" title=\"Previous\" />\n" .
    "<input type=\"SUBMIT\" class=\"button\" name=\"shot_n\" value=\"N\" title=\"Next\" />\n" .
    "<input type=\"BUTTON\" class=\"button\" onclick=\"if (window.document.form1.shot.value == 'Cur')" .
    "{window.document.form1.shot.value='Current+1'; window.document.form1.submit();} " .
    "else if ((window.document.form1.shot.value/window.document.form1.shot.value == 1) || ((window.document.form1.shot.value-0) == 0)) " .
    "window.document.form1.shot.value=(window.document.form1.shot.value-0)+1;\" value=\"I\" title=\"Increment\" />\n" .
    "<input type=\"BUTTON\" class=\"button\" onclick=\"if (window.document.form1.shot.value == 'Cur')" .
    "{window.document.form1.shot.value='Current-1'; window.document.form1.submit();} " .
    "else if ((window.document.form1.shot.value/window.document.form1.shot.value == 1) || ((window.document.form1.shot.value-0) == 0))" .
    "window.document.form1.shot.value=window.document.form1.shot.value-1;\" value=\"D\" title=\"Decrement\" />\n" .
    "<input type=\"BUTTON\" class=\"button\" onclick=\"window.document.form1.shot.value='';\" value=\"X\" title=\"None\" />\n" .
    "Template Name:<input type=\"TEXT\" SIZE=20 name=\"template_name\" value=\"" . $_GET['template'] . "\" />\n" .
    "</td></tr><tr><td align=\"center\">$lastentry</td></tr><tr><td><hr /></td></tr><tr><td align=\"center\">\n" .
    "<input type=\"CHECKBOX\" name=\"clear_text\" " . (isset($_GET['clear_text']) ? "checked" : "") . " />Clear Text After Entry\n" .
    "<input type=\"BUTTON\" class=\"button\" onclick=\"if (checkrun() && checkshot() && checktext()) " .
    "{ window.document.form1.newentry.value='make_entry';window.document.form1.submit();}\" name=\"make_entry\" value=\"Make Entry\" title=\"Make Entry\" />\n" .
    "<input type=\"BUTTON\" class=\"button\" " .
    "onclick=\"window.document.form1.text.value='';" . 
    "window.document.form1.load_template.selectedIndex=window.document.form1.load_template.length-1;return 1;\" value=\"Erase Text\" title=\"Erase Text\" />\n" .
    "<input type=\"SUBMIT\" class=\"button\" value=\"Delete Template\" name=\"delete_template\" title=\"Erase Template\" />\n" .
    "<input type=\"SUBMIT\" class=\"button\" value=\"Save As Template\" name=\"save_template\" title=\"Save Template\" />\n" .
    "<input type=\"BUTTON\" class=\"button\" onclick=\"window.close();\" value=\"Close\" title=\"Close this window\" />\n" .
    "</td></tr>\n" .
    "</table></form>\n";
  print "</body></html>\n";
  return;
}
else if (array_key_exists('edit',$_GET) || array_key_exists('edit',$_POST))
{
  if (array_key_exists('edit',$_POST)) $_GET=$_POST;
  include 'dbconnect.php';
  if ($_GET['edit'] == "write")
  {
    $dbkey=$_GET['dbkey'];
    $q=mssql_query("select * from entries where dbkey=$dbkey");
    $ans=mssql_fetch_array($q);
    if ($_GET['run'] == "")
      $run="NULL";
    else
      $run=$_GET['run'];
    if ($_GET['shot'] == "")
      $shot="NULL";
    else
      $shot=($run == "NULL") ? "NULL" : $run * 1000 + $_GET['shot'];
    $cmd="insert into entries (run,shot,topic,voided,text) values ($run,$shot,\"" . $ans['TOPIC'] . "\"," .
      ($ans['VOIDED'] === NULL ? "NULL" : "current_timestamp") . "," .
      "'" . str_replace("'","''",stripslashes($_GET['text'])) . "')";
    $q=mssql_query($cmd);
    $q=mssql_query("update entries set voided=current_timestamp where dbkey=$dbkey");
    setevent();
    print "<body onload=\"window.close();\"></body>\n";
  }
  else
  {
    $q=mssql_query("select * from entries where dbkey=" . $_GET['edit']);
    $ans=mssql_fetch_array($q);
    print $html_preface .
      "<head>\n<title>Edit Entry</title>\n" .
      "<link rel=\"stylesheet\" type=\"text/css\" href=\"logbook.css\" media=\"screen\" />\n" . fieldchecks(1) .
      "</head><body class=\"" . fontsize($preferences) . "\">\n";
    print "<form name=form1 method=\"post\"><input type=\"HIDDEN\" name=\"dbkey\" VALUE=\"" . $_GET['edit'] .  "\" />\n" .
      "<input type=\"HIDDEN\" name=\"edit\" value=\"\" /><table><tr><td><b>Run:</b>" . 
      "<input type=\"TEXT\" SIZE=7 name=\"run\" value=\"" . $ans['RUN'] . "\" /><b>Shot:</b> " .
      "<input type=\"TEXT\" SIZE=3 name=\"shot\" value=\"" . (($ans['SHOT'] == "") ? "" : ($ans['SHOT'] % 1000)) . "\" /></td></tr>\n" .
      "<tr><td><b>User: </b>" . $_SERVER['PHP_AUTH_USER'] . " <b>Topic:</b> " . $ans['TOPIC'] . "</td></tr>\n" .
      "<tr><td><textarea COLS=80 ROWS=20 name=\"text\">" . stripslashes($ans['TEXT']) . "</textarea></td><tr>\n" .
      "<tr><td align=\"center\">\n" .
      "<input type=\"BUTTON\" class=\"button\" " .
      "onclick=\"if (checkrun() && checkshot() && checktext()) {window.document.form1.edit.value='write'; window.document.form1.submit();}\" " .
      "value=\"Ok\" title=\"Ok\" />\n" .
      "<input type=\"BUTTON\" class=\"button\" onclick=\"window.close();\" Value=\"Close\" title=\"Close this window\" />\n" .
      "</td></tr>\n" .
      "</table></form>\n";
    print "</body></html>\n";
  }
  return;
}
else if (array_key_exists('main_display',$_GET))
{
  $preferences = preferences();
  if (array_key_exists('custom',$_GET) && array_key_exists('customtext',$_GET))
    setcookie("custom_query",stripslashes($_GET['customtext']),time()+60*60*24*30);
  print $html_preface .
    "<head>\n" .
    "<link rel=\"stylesheet\" type=\"text/css\" href=\"logbook.css\" media=\"screen\" />\n" .
    "<title>Logbook - " . strftime("%H:%M:%S") . "</title>\n</head>\n<body" .
    ($preferences['display']['auto scroll'] ? 
     ($preferences['display']['scroll top'] ? " onload=\"window.scrollTo(0,0);\"" : 
      " onload=\"window.scrollTo(0,window.document.body.scrollHeight);\"") : "") .
    " class=\"" . fontsize($preferences) . "\">\n";
  $q=mssql_query("select max(run) as mrun from runs");
  $ans=mssql_fetch_array($q);
  $mrun=$ans['mrun'];
  switch ($preferences['selection']['run'])
  {
  case -1: 
    $q=mssql_query("select top 1 run from runs where run < $run order by run desc");
    $ans=mssql_fetch_array($q);
    $run=$ans['run'];
    break;
  case 0: 
    $run=$mrun; 
    break;
  case 1: 
    $run=$mrun+1; 
    break;
  default: 
    $run=$preferences['selection']['run'];
    break;
  }
  if (array_key_exists('delete',$_GET))
  {
    $q=mssql_query("update entries set voided=current_timestamp where dbkey=" . $_GET['delete']);
    setevent();
  }
  else if (array_key_exists('undelete',$_GET))
  {
    $q=mssql_query("update entries set VOIDED=NULL where dbkey=" . $_GET['undelete']);
    setevent();
  }
  $direction = $preferences['display']['ascending'] ? " asc" : " desc";
  if ($preferences['display']['sort by shot'])
    $sort=" run $direction,shot $direction,entered $direction";
  else
    $sort=" entered $direction,run $direction,shot $direction";
  $updates=$preferences['display']['updates'] && !array_key_exists('custom',$_GET);
  print "<form id=\"form1\" action=\"$self\">\n" .
    "<input type=\"hidden\" name=\"main_display\" \>\n" .
    "<table width=\"100%\"><tr>\n" .
    "<td>\n" .
    ((strtolower($_SERVER['PHP_AUTH_USER']) == "anonymous") ? "" :
    "<input class=\"button\" type=\"button\" onclick=\"window.open('" . $_SERVER["PHP_SELF"] .
    "?newentry','','toolbar=no,location=no,status=no,width=800,height=500,scrollbars=yes,resizable=yes')\" name=\"newentry\" value=\"New Entry\" />\n") .
    "<input class=\"button\" type=\"submit\" " .
    (array_key_exists('custom',$_GET) ? "value=\"Cancel Custom\"" : "name=\"custom\" value=\"Custom Query\"") ." />\n" .
    "<input class=\"button\" type=\"submit\" name=\"setprefs\" value=\"Selection Prefs\" />\n" .
    "<input class=\"button\" type=\"submit\" name=\"setdisprefs\" value=\"Display Prefs\" />\n" .
    "<input class=\"button\" type=\"submit\" name=\"switchuser\" value=\"New Login\" />\n" .
    "</td></form><form name=\"updates\" method=\"get\"><td>\n" .
    "<input type=\"hidden\" name=\"main_display\" />\n" .
    "<input type=\"CHECKBOX\" " . ($updates ? " checked " : "") . 
    "name=\"updates\" value=\"AutoUpdate\" " . 
    (array_key_exists('custom',$_GET) ? 
     "onclick=\"this.checked=false;\"" : 
     "onclick=\"document.updates2.updates.checked=this.checked; if (this.checked) submit();\"") . "/>AutoUpdate</td>\n" .
    "<td class=\"lastchanged\">Last updated: " . strftime("%H:%M:%S%p") . "</td>\n" .
    "</tr></table></form>\n";
  if (array_key_exists('custom',$_GET) || array_key_exists('customtext',$_GET))
  {
    $custom="?main_display&custom&" . (isset($_GET['show_voided']) ? 'show_voided=yes&' : '&');
    if ($_GET['customtext'] == '')
      $q=mssql_query("select top 0 * from entries");
    else
    {
      $custom=$custom . "customtext=" . stripslashes($_GET['customtext']) . "&";
      $query="select * from entries where " . stripslashes($_GET['customtext']) . (isset($_GET['show_voided']) ? "" : " and voided is null") . " order by $sort";
      ob_start();
      $q=mssql_query($query);
      $errors=ob_get_clean();
      if ($errors != '')
      {
	print "<script language=\"JAVASCRIPT\">alert(\"" . addslashes(addcslashes("Error in query:  " . mssql_get_last_message(),"\n\r")) . "\");</script>\n";
        $q=mssql_query("select top 0 * from entries");
      }
    }
    print "<form><table><tr><td><b>Custom query:</b></td><td><input type=\"text\" SIZE=60 name=\"customtext\" value=\"" . 
      stripslashes($_GET['customtext'] == "" ? $_COOKIE['custom_query'] : $_GET['customtext']) . "\" /></td>\n
      <td><input class=\"button\" type=\"SUBMIT\" name=\"custom\" value=\"Do Query\" />" .
      "</td></tr>\n" .
      "<tr>\n" .
      "<td class=\"smallcheckbox\"><input type=\"checkbox\" name=\"show_voided\" " . (isset($_GET['show_voided']) ? "checked" : "") . " />Show voided</td>\n" .
      "<td colspan=\"1\" class=\"customhelp\">Available fields are: RUN,SHOT,USERNAME,ENTERED,VOIDED,TOPIC and TEXT</td></tr>\n" .
      "</table>\n" .
      "<input type=\"hidden\" name=\"main_display\" \>\n" .
      "</form>\n";
  }
  else
  {
    $custom="?main_display&";
    if ($preferences['selection']['topics'] == '')
      $topic_list = "";
    $topic_list = ($preferences['selection']['topics'] != '') ?
      " or topic in " . $preferences['selection']['topics'] . " " : "";
    switch ($preferences['selection']['show_voided'])
    {
    case 'O': $voided=' and VOIDED is not NULL '; break;
    case 'B': $voided=' '; break;
    default: $voided=' and VOIDED is NULL '; break;
    }
    $nesort = ($preferences['display']['null shots first']) ? " ne asc " : " ne desc ";
    $q="select isnumeric(shot) as ne,* from entries where (run = $run) and (username=\"" . 
      ((trim($preferences['selection']['user']) == "ME") ? $_SERVER['PHP_AUTH_USER'] : $preferences['selection']['user'])
      . "\"" . $topic_list . ") " . $voided . " order by $nesort, $sort";
    $q=mssql_query($q);
  }
  $rows=mssql_num_rows($q);
  $pages=(int)(($rows+$preferences['display']['limit']-1)/$preferences['display']['limit']);
  $page = (array_key_exists('page',$_GET)) ? $_GET['page'] : 1;
  print "<table width=\"100%\"><tr><td align=\"right\">";
  print "Results " . (($page - 1)*$preferences['display']['limit']+1) . " - " . 
    (($rows < ($page * $preferences['display']['limit'])) ? $rows :($page * $preferences['display']['limit']))
    . " of $rows.";
  print "</td></tr></table>\n";
  print "<table width=\"100%\">\n";
  $row=0;
  if (mssql_num_rows($q) > 0) mssql_data_seek($q,($page-1) * $preferences['display']['limit']);
  while (($ans=mssql_fetch_array($q)) && ($row < $preferences['display']['limit']))
  {
    $row++;
    if ($ans['RUN'] != '')
    {
      if ($ans['SHOT'] == '')
        $link="<a target=\"_blank\" href=\"program/cmod_runs.php?run=" . $ans['RUN'] . "\">" . $ans['RUN'] . "xxx</a>";
      else
        $link="<a target=\"_blank\" href=\"program/shot_display.php?shot=" . $ans['SHOT'] . "\">" . $ans['SHOT'] . "</a>";
    }
    else if ($ans['SHOT'] != '')
        $link="<a target=\"_blank\" href=\"program/shot_display.php?shot=" . $ans['SHOT'] . "\">" . $ans['SHOT'] . "</a>";
    else
      $link="";
    print "<tr><td class=\"entries\">\n" .
      "<table border=\"0\" width=\"100%\">\n" .
      "<tr class=\"entry-title\">\n" .
      "<td class=\"entry-left\" >$link</td>\n" .
      "<td class=\"entry-center\" >" . $ans['USERNAME'] . "</td>\n" .
      "<td class=\"entry-center\" >" . $ans['TOPIC'] . "</td>\n" .
      "<td class=\"entry-right\" >" . $ans['ENTERED'] . "</td>\n" .
      "<td>" . ((strcasecmp(trim($ans['USERNAME']),trim($_SERVER['PHP_AUTH_USER'])) == 0) ? 
		"<a href=\"" . $_SERVER["PHP_SELF"] . "?edit=" . 
		$ans['DBKEY'] . "\"  target=_blank><img title=\"Edit\" src=\"edit.gif\" border=\"0\" alt=\"Edit\" /></a>" .
		"<a href=\"" . $_SERVER["PHP_SELF"] . $custom .
		(($ans['VOIDED'] == "") ? "delete=" . $ans['DBKEY'] . 
		 "\"><img title=\"Delete\" src=\"del.gif\" border=\"0\" alt=\"Delete\" /></a>" : 
		 "undelete=" . $ans['DBKEY'] .
		 "\"><img title=\"Undelete\" src=\"undel.gif\" border=\"0\" alt=\"Undelete\" /></a>") : "") . "</td>\n" .
      "</tr>\n" .
      "<tr class=\"entry\">\n" .
      "<td class=\"" . (($ans['VOIDED'] == "") ? "entry" : "voidedentry") . "\" colspan=\"6\">" . fixentry($ans['TEXT']) . "</td>\n" .
      "</tr>\n</table>\n</td>\n</tr>\n";
  }
  print "</table>\n";
  if ($pages > 1)
  {
    print "<p /><hr />\n";
    if ($_SERVER["QUERY_STRING"] == '')
      $qstr="?";
    else
      $qstr="?" . ereg_replace("page=[[:digit:]]+","",$_SERVER["QUERY_STRING"]) . "&";
    if ($page > 1) print "<a href=\"" . $_SERVER["PHP_SELF"] . $qstr . "page=" . ($page-1) . "\">Previous</a>,";
    for ($i=1;$i<=$pages;$i++)
    {
      if ($i > 1) print ",";
      if ($i==$page)
        print $page;
      else
        print "<a href=\"" . $_SERVER["PHP_SELF"] . $qstr . "page=" . $i . "\">$i</a>";
    }      
    if ($page < $pages)
    {
      print ",<a href=\"" . $_SERVER["PHP_SELF"] . $qstr . "page=" . ($page+1) . "\">Next</a>\n";
    }
  }
  print "<table width=\"100%\"><tr><td align=\"right\">";
  print "Results " . (($page - 1)*$preferences['display']['limit']+1) . " - " . 
    (($rows < ($page * $preferences['display']['limit'])) ? $rows :($page * $preferences['display']['limit']))
    . " of $rows.";
  print "</td></tr></table>\n";
  print "<form method=\"get\" action=\"$self\">\n" .
    "<input type=\"hidden\" name=\"main_display\" \>\n" .
    "<table width=\"100%\"><tr>\n" .
    "<td>\n" .
    ((strtolower($_SERVER['PHP_AUTH_USER']) == "anonymous") ? "" :
    "<input class=\"button\" type=\"button\" onclick=\"window.open('" . $_SERVER["PHP_SELF"] .
    "?newentry','','toolbar=no,location=no,status=no,width=800,height=500,scrollbars=yes,resizable=yes')\" name=\"newentry\" value=\"New Entry\" />\n") .
    "<input class=\"button\" type=\"submit\" " .
    (array_key_exists('custom',$_GET) ? "value=\"Cancel Custom\"" : "name=\"custom\" value=\"Custom Query\"") ." />\n" .
    "<input class=\"button\" type=\"submit\" name=\"setprefs\" value=\"Selection Prefs\" />\n" .
    "<input class=\"button\" type=\"submit\" name=\"setdisprefs\" value=\"Display Prefs\" />\n" .
    "<input class=\"button\" type=\"submit\" name=\"switchuser\" value=\"New Login\" />\n" .
    "</td></form><form name=\"updates2\" method=\"get\"><td>\n" .
    "<input type=\"hidden\" name=\"main_display\" />\n" .
    "<input type=\"CHECKBOX\" " . ($updates ? " checked " : "") . 
    "name=\"updates\" value=\"AutoUpdate\" " . 
    (array_key_exists('custom',$_GET) ? 
     "onclick=\"this.checked=false;\"" : 
     "onclick=\"document.updates.updates.checked=this.checked; if (this.checked) submit();\"") . "/>AutoUpdate</td>\n" .
    "<td class=\"lastchanged\">Last updated: " . strftime("%H:%M:%S%p") . "</td>\n" .
    "</tr></table></form>\n";
  print "</body></html>";
}
else if (array_key_exists('updater',$_GET))
{
?>
<script type="text/javascript">
 
function update()
{
  if (top.logbook.document.updates.updates.checked)
    open("?main_display","logbook");
  return 0;
}

</script>
<!--[if !IE]> -->
   <object classid="java:MdsPlus/MdsPlusEventMonitor" archive="MdsPlus.jar" width="1" height="1" type="application/x-java-applet;version=1.4" 
      eventName="LOGBOOK_ENTRY" jscript="update();" mayscript="true">
      <param name="eventName" value="LOGBOOK_ENTRY" />
      <param name="jscript" value="update();">
      <param name="mayscript" value="true" />
   </object>
 <!-- <![endif]-->
 <object classid="clsid:8AD9C840-044E-11D1-B3E9-00805F499D93" width="1" height="1">
  <param name="code" value="MdsPlus/MdsPlusEventMonitor" />
  <param name="archive" value="MdsPlus.jar" />
  <param name="eventName" value="LOGBOOK_ENTRY" />
  <param name="mayscript" value="true" />
  <param name="jscript" value="update();">
</object>
<?php
}
else
{
?>
<html>
 <head>
 <title>Logbook</title>
 </head>
 <script type="text/javascript">

 if (top.location != self.location)
 {
    top.location = self.location;
 }

 </script>
 <frameset border="0" rows="0,20,*">
    <frame src="?updater" height="0" marginwidth="0" marginheight="0" scrolling="no"/>
    <frame name="state" src="state.html" marginwidth="0" marginheight="0" scrolling="no"/>
    <frame name="logbook" src="?main_display" />
 </frameset>
</html>
<?php
}
?>
