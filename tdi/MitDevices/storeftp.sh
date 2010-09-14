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

xmlstart
xmlacqcmd getNumSamples
xmlacqcmd getChannelMask
xmlacqcmd getInternalClock
xmlcmd date
xmlcmd hostname
xmlcmd 'sysmon -T 0'
xmlcmd 'sysmon -T 1'
xmlcmd get.channelMask
xmlcmd get.channel_mask
xmlcmd get.d-tacq.release
xmlcmd get.event0
xmlcmd get.event1
xmlcmd get.extClk 
xmlcmd get.ext_clk
xmlcmd get.int_clk_src
xmlcmd get.modelspec
xmlcmd get.numChannels
xmlcmd get.pulse_number
xmlcmd get.trig
xmlcmd get.vin
xmlfinish
tree=$1
shot=$2
path=$3
host=192.168.0.254
settings.sh > /tmp/settings.xml
echo $tree $shot $path > /tmp/$tree.$shot.$path
ftp $host <<EOF
put /tmp/$tree.$shot.$path triggers/$tree.$shot.$path 
mkdir $tree
cd $tree
mkdir $shot
cd $shot
mkdir $path
cd $path
lcd /tmp
put settings.xml
lcd /dev/acq200/data
prompt
mput [0-9][0-9]
EOF
