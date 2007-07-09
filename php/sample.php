<?php
if (!extension_loaded('mdsplus')){dl('mdsplus.so');}
$handle=mdsplus_connect("unknown_host_specified");
if ($msg = mdsplus_error())
{
     print "mdsplus_connect returned false and error is set to " . $msg . "\n";
}
$handle=mdsplus_connect("alcdata.psfc.mit.edu");
$value=mdsplus_value($handle,"1+2");
if (mdsplus_error() == FALSE)
{
  print "mdsplus_value returned $value\n";
}
else
{
  print "mdsplus_value returned false and error is set to " . mdsplus_error() . "\n";
}
$value=mdsplus_value($handle,"1+2/");
if (mdsplus_error() == FALSE)
{
  print "mdsplus_value returned $value\n";
}
else
{
  print "mdsplus_value returned false and error is set to " . mdsplus_error() . "\n";
}

$value=mdsplus_value($handle,"0");
if (mdsplus_error() == FALSE)
{
  print "mdsplus_value returned $value\n";
}
else
{
  print "mdsplus_value returned false and error is set to " . mdsplus_error() . "\n";
}
if (mdsplus_open($handle,'gubkjla',-1))
{
  print "Successfully opened tree\n";
}
else
{
  print "Error opening tree with error " . mdsplus_error() . "\n";
}
if (mdsplus_open($handle,'cmod',-1))
{
  print "Successfully opened tree\n";
}
else
{
  print "Error opening tree with error " . mdsplus_error() . "\n";
}
$children=mdsplus_value($handle,"getnci(\".*\",\"NODE_NAME\")");
if (mdsplus_error() == FALSE)
{
  print_r($children);
}
else
{
  print "Error getting children, error = " . mdsplus_error() . "\n";
}
$array=mdsplus_value($handle,"1:20");
if (mdsplus_error() == FALSE)
{
  print_r($array);
}
else
{
  print "Error getting arry, error = " . mdsplus_error() . "\n";
}
if (mdsplus_close($handle))
{
  print "Successfully closed tree\n";
}
else
{
  print "Error closing tree, error = " . mdsplus_error() . "\n";
}
if (mdsplus_close($handle))
{
  print "Successfully closed tree\n";
}
else
{
  print "Error closing tree, error = " . mdsplus_error() . "\n";
}
if (mdsplus_open($handle,"main",1))
{
  print "Successfully opened tree\n";
}
else
{
  print "Failed to open tree, error = " . mdsplus_error() . "\n";
}
if (mdsplus_put($handle,"member","1 :  $", 42))
{
  print "Successfully wrote to tree\n";
  print_r($value=mdsplus_value($handle,"member"));
  print "decompile of member = " . mdsplus_value($handle,"tcl(\"decompile member\",_out),_out") . "\n";
}
else
{
  print "Failed to write to tree, error = " . mdsplus_error() . "\n";
}
if (mdsplus_put($handle,"$","member",$value)) /*** Note this is currently unsupported ***/
{
  print "Successfully wrote an array to tree\n";
  print "decompile of member = " .  mdsplus_value($handle,"tcl(\"decompile member\",_out),_out") . "\n";
}
else
{
  print "Failed to write an array to tree, error = " . mdsplus_error() . "\n";
}
mdsplus_close($handle);
mdsplus_disconnect($handle);
?>
