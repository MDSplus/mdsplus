<?php
//Include the code
include('PHPlot.php');
if (!isset($_GET['plot']))
{
  if (!extension_loaded('mdsplus')){dl('mdsplus.so');}
  $handle=mdsplus_connect('alcdata');
  mdsplus_open($handle,'waveforms',$_GET['shot']);
  $signals=mdsplus_value($handle,'GETNCI("*","NODE_NAME")');
  $numsigs=count($signals);
  $numplots=0;
  print "<table border=0>\n";
  for ($sig=0;$sig<$numsigs;$sig++)
  {
    ob_start();
    $value=mdsplus_value($handle,'_sig=' . $signals[$sig]);
    $time=mdsplus_value($handle,'dim_of(_sig)');
    $title=mdsplus_value($handle,trim($signals[$sig]) . ":LABEL");
    $units=mdsplus_value($handle,trim($signals[$sig]) . ":UNITS");
    $num=count($time);
    $inc=$num/1000;
    $inc=($inc > 1) ? $inc : 1;
    ob_end_clean();
    if ($num > 2)
    {
      $data=array();
      for ($i=0;$i<$num;$i+=$inc)
        $data[]=array("",$time[$i],$value[$i]);
    //print_r($data);
    //    file_put_contents("/tmp/php_plot_" . $sig,serialize($data));
      $file = fopen("/tmp/php_plot_" . $sig,"w");
      fwrite($file,serialize($data));
      fclose($file);
      $numplots++;
      if ($numplots & 1)
	print "<tr>\n";
      print "<td><table cellpadding=\"0\" cellspacing=\"0\"><tr><td><center style=\"background-color: rgb(255,255,153);\" ><b>$title</b></center></td></tr><tr><td>" .
	"<img title=\"Maximum=" . max($value) . "\" src=\"waveforms.php?shot=" . $_GET['shot'] .  "&plot=" . urlencode($sig) . 
        "\" /></td></tr></table></td>\n";
      if (($numplots & 1) == 0)
        print "</tr>\n";
    }
  }
  print "</tr></table>\n";
  mdsplus_disconnect($handle);
}
else
{
  //  $data = unserialize(file_get_contents("/tmp/php_plot_" . $_GET['plot']));
  $file = fopen("/tmp/php_plot_" . $_GET['plot'],"r");
  $data = unserialize(fread($file,2065536));
  fclose($file);
  unlink("/tmp/php_plot_" . $_GET['plot']);
  $graph = new PHPlot(400,180);
  $graph->SetDataColors(array("blue"),array("red"));
  $graph->SetTitleFontSize("0");
  $graph->SetDataType("data-data");
  $graph->SetPlotType("lines");
  $graph->SetPointShape('line');
  $graph->SetPointSize(0);
  $graph->SetNumVertTicks(5);
  $graph->SetNumHorizTicks(5);
  $graph->SetXGridLabelType("data");
  $graph->SetXDataLabelPos('none');
  $graph->SetIsInline('1');
  $graph->SetDataValues($data);
  // $graph->SetTitle($_GET['label']);
  $graph->SetXLabel("Time (seconds)");
  // $graph->SetYLabel($_GET['units']);
  //Draw it
  $graph->DrawGraph(); // remember, since in this example we have one graph, PHPlot 
                        // does the PrintImage part for you 
}
?>
