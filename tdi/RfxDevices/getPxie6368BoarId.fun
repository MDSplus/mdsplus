/**********************************************************************
Function is automatic generated with:
 pxie6368devList.sh '$'MDSPLUS_DIR/tdi/RfxDevices/getPxie6368BoarId.fun 
command in /etc/rc.local
***********************************************************************/
public fun getPxie6368BoarId(in _serialNumber)
{
_snList = [
 0X20054AD ,
 0X20054A9 ,
 0X2005491 ,
 0X201F4CF ,
 0X2021BBA ,
 0X201F4CD ,
 0X204071D ,
 0X204071F ,
 0X201F4D3 ,
0 ];
	_numModule =  size( _snList ) - 1; 
	for( _i = 0; _i < _numModule  && _serialNumber != _snList[ _i ] ; _i++);
	return ( _i == _numModule ? -1 : _i );
}
