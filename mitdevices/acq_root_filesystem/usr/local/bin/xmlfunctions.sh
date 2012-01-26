#!/bin/sh
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
  key=${2:-$cmd}
  echo "<string>$key</string>"
  echo "<string>`$cmd`</string>"
}
xmlacqcmd() {
  cmd=$1
  key=${2:-$cmd}
  echo "<string>$key</string>"
  echo "<string>`acqcmd $cmd`</string>"
}
