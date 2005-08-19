/* Version Number 4.0 */
/* 2005.08.18 */
/* return value make double(real)*/
/* sampling rate, verti_Range_Offset, Channel Probe Attenuation, Trigger Level, Trigger Delay */

public fun NI5122__add(in _path, out _nidout)
{
    DevAddStart(_path, 'NI5122', 40, _nidout);
    DevAddNode(_path // ':ADDRESS', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':ID', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':CHANNELS', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.SAMPLE','STRUCTURE', *, *, _nid);
    DevAddNode(_path // '.SAMPLE:SAM_RATE', 'NUMERIC', 20000000.0, *, _nid);
    DevAddNode(_path // '.SAMPLE:SAM_NUM', 'NUMERIC', 1024, *, _nid);
    DevAddNode(_path // '.TRIG','STRUCTURE', *, *, _nid);
    DevAddNode(_path // '.TRIG:DELAY', 'NUMERIC', 0.0, *, _nid);
    DevAddNode(_path // '.TRIG:SOURCE', 'NUMERIC', 2, *, _nid);
    DevAddNode(_path // '.TRIG:LEVEL', 'NUMERIC', 0.0, *, _nid);
    DevAddNode(_path // '.TRIG:SLOPE', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.TRIG:COUPL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.TRIG:T0','NUMERIC', 0., *, _nid); 

    for (_i=0;_i<2;_i++)
  {
  	_nodeName = _path//':AI'//text(_i,1);
	  DevAddNode(_nodeName, 'SIGNAL', *, '/nomodel_write', _nid);
    DevAddNode(_nodeName // '.CHSET','STRUCTURE', *, *, _nid);
    DevAddNode(_nodeName // '.CHSET:CHANNEL_IMPE', 'NUMERIC', *, *, _nid);
  	DevAddNode(_nodeName // '.CHSET:VOLT_RANGE', 'NUMERIC', 10d0, *, _nid);
  	DevAddNode(_nodeName // '.CHSET:RANGE_OFFSET', 'NUMERIC', 0, *, _nid);
  	DevAddNode(_nodeName // '.CHSET:CH_COUPLING', 'NUMERIC', 0, *, _nid);
  	DevAddNode(_nodeName // '.CHSET:PROBE_ATTE', 'NUMERIC', 1.0, *, _nid);
	  DevAddNode(_nodeName//':FOO', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
/*  	DevAddNode(_nodeName//':OFFSET_MODE', 'NUMERIC', 0, *, _nid); */
/*  	DevAddNode(_nodeName//':OFFSET_COEFF', 'NUMERIC', *, *, _nid); */
    DevAddNode(_nodeName//':CH_LABEL', 'TEXT', *, *, _nid);
    }
    DevAddAction(_path// ':OPEN','OPEN','OPEN',50,'LOCALHOST:8001',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path// ':INIT','INIT','INIT',50,'LOCALHOST:8001',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path// ':SETUP','SETUP','SETUP',50,'LOCALHOST:8001',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path// ':ARM','ARM','ARM',50,'LOCALHOST:8001',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path// ':STORE','STORE','STORE',50,'LOCALHOST:8001',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path// ':ERROR','ERROR','ERROR',50,'LOCALHOST:8001',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path// ':STATUS','STATUS','STATUS',50,'LOCALHOST:8001',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path// ':CLOSE','CLOSE','STORE',50,'LOCALHOST:8001',getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}
