#/bin/sh
echo "/**********************************************************************" > $1
echo "Function is automatic generated with:" >> $1	
echo " pxie6368devList.sh '$'MDSPLUS_DIR/tdi/RfxDevices/getPxie6368BoarId.fun " >> $1	
echo "command in /etc/rc.local" >> $1	
echo "***********************************************************************/" >> $1
echo "public fun getPxie6368BoarId(in _serialNumber)" >> $1	
echo "{" >> $1	
echo "_snList = [" >> $1	
if [ -r /etc/.mdsplus_dir ]
then
    MDSPLUS_DIR=`cat /etc/.mdsplus_dir`
else
    MDSPLUS_DIR=/usr/local/mdsplus
fi
for dev in /dev/pxie-6368.?
do 
  $MDSPLUS_DIR/bin/print_device_info $dev | grep "Serial number" | awk -F':' '{ print $2, "," }' >> $1
done
echo "0 ];" >> $1	
echo "	_numModule =  size( _snList ) - 1; " >> $1
echo "	for( _i = 0; _i < _numModule  && _serialNumber != _snList[ _i ] ; _i++);" >> $1
echo "	return ( _i == _numModule ? -1 : _i );">> $1
echo "}">> $1
 
