xmlstart() {
echo "<?xml version=\"1.0\"?>"
echo "<marshal>"
echo "<dictionary id=\"i2\">"
}
xmlfinish() {
echo "</dictionary>"
echo "</marshal>"
}

xmlcmd() {
  cmd=$1
  echo "<string>$cmd</string>"
  echo "<string>`$cmd`</string>"
}
xmlacqcmd() {
  cmd=$1
  echo "<string>$cmd</string>"
  echo "<string>`acqcmd $cmd`</string>"
}
settingsf=/tmp/settings.xml
xmlstart > $settingsf
xmlacqcmd getNumSamples >> $settingsf
xmlacqcmd getChannelMask >> $settingsf
xmlacqcmd getInternalClock >> $settingsf
xmlcmd date >> $settingsf
xmlcmd hostname >> $settingsf
xmlcmd 'sysmon -T 0' >> $settingsf
xmlcmd 'sysmon -T 1' >> $settingsf
xmlcmd get.channelMask >> $settingsf
xmlcmd get.channel_mask >> $settingsf
xmlcmd get.d-tacq.release >> $settingsf
xmlcmd get.event0 >> $settingsf
xmlcmd get.event1 >> $settingsf
xmlcmd get.extClk  >> $settingsf
xmlcmd get.ext_clk >> $settingsf
xmlcmd get.int_clk_src >> $settingsf
xmlcmd get.modelspec >> $settingsf
xmlcmd get.numChannels >> $settingsf
xmlcmd get.pulse_number >> $settingsf
xmlcmd get.trig >> $settingsf
xmlcmd 'get.vin'>> $settingsf

xmlfinish >> $settingsf
tree=$1
shot=$2
path=$3
host=192.168.0.254
echo $tree $shot $path > /tmp/$tree.$shot.$path

curl -s -T "/dev/acq200/data/[01-96]" --ftp-create-dirs ftp://mdsftp:mdsftp@$host/scratch/$tree/$shot/$path/
curl -s -T $settingsf --ftp-create-dirs ftp://mdsftp:mdsftp@$host/scratch/$tree/$shot/$path/
curl -s -T /tmp/$tree.$shot.$path --ftp-create-dirs ftp://mdsftp:mdsftp@$host/scratch/triggers/$tree.$shot.$path
