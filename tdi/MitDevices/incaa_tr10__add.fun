Public fun incaa_tr10__add(in _path, out _nidout)
{
  DevAddStart(_path,'INCAA_TR10',87,_nidout, 'MitDevices');
  DevAddNode(_path//':NODE','TEXT',*,*,_nid);
  DevAddNode(_path//':BOARD','NUMERIC',1,'/noshot_write',_nid);
  DevAddNode(_path//':COMMENT','TEXT',*,*,_nid);

  /* trigger source, destination and time */
  DevAddNode(_path//':TRIG_SRC', 'TEXT', 'ext', '/noshot_write', _nid); /* ext, pxi, star, int */
  DevAddNode(_path//':TRIG_CHAN', 'NUMERIC', 0, '/noshot_write', _nid); /* trigger channel if src is pxi (0-1) */
  DevAddNode(_path//':TRIG_OUTPUT', 'NUMERIC', 0, '/noshot_write', _nid); /* trigger output 0-off, 1-7 pxi 0-6 */
  DevAddNode(_path//':TRIG_SENSE', 'TEXT', '+', '/noshot_write', _nid); /* trig rising (+) or falling (-) */
  DevAddNode(_path//':TRIG_TERM', 'NUMERIC', 0, '/noshot_write', _nid); /* 50 ohm term? (0/1) */
  DevAddNode(_path//':TRIG_SYNC', 'NUMERIC', 0, '/noshot_write', _nid); /* trig synced with clock (0/1) */
  DevAddNode(_path//':TRIG_LEVEL', 'NUMERIC', 0, '/noshot_write', _nid); /* trig level (0/1) */
  DevAddNode(_path//':TRIG', 'NUMERIC', 0.0, '/noshot_write', _nid);     /* trig time (seconds) */


  /* clock controls */
  DevAddNode(_path//':CLK_SRC', 'TEXT', 'int', '/noshot_write', _nid); /* int, ext, pxi */
  DevAddNode(_path//':CLK_OUTPUT', 'NUMERIC', 0, '/noshot_write', _nid); /* clock output 0-off, 1-PXI7 */
  DevAddNode(_path//':CLK_SENSE', 'TEXT', '+', '/noshot_write', _nid); /* clock sense +/- */
  DevAddNode(_path//':CLK_TERM', 'NUMERIC', 0, '/noshot_write', _nid); /* 50 ohm term? (0/1) */
  DevAddNode(_path//':CLK_DIVIDE', 'NUMERIC', 1, '/noshot_write', _nid); /* clock divider 1-65536 (divides 200KHz) */
  DevAddNode(_path//':CLOCK', 'NUMERIC', *, '/noshot_write', _nid);     /* external clock ref */
  DevAddNode(_path//':EXT_1MHZ', 'NUMERIC', 0b, '/noshot_write', _nid);     /* use external 1Mhz to gen internal clock ? */


  /* data acquisition */
  DevAddNode(_path//':POST_TRIG','NUMERIC',64,'/noshot_write',_nid);
  DevAddNode(_path//':PRE_TRIG','NUMERIC',0,'/noshot_write',_nid);
   for (_c=1;_c<=16;_c++)
  {
    _cn = _path//':INPUT_'//TEXT(_c/10,1)//TEXT(_c mod 10,1);
    DevAddNode(_cn,'SIGNAL',*,'/write_once/compress_on_put/nomodel_write',_nid);
    DevAddNode(_cn//':STARTIDX','NUMERIC',*,'/noshot_write',_nid);
    DevAddNode(_cn//':ENDIDX','NUMERIC',*,'/noshot_write',_nid);
    DevAddNode(_cn//':FILTER','TEXT',*,'/noshot_write',_nid);
  }

  /* and the default actions */
  DevAddAction(_path//':INIT_ACTION','INIT','INIT',50,'CAMAC_SERVER',_path,_nid);
  DevAddAction(_path//':STORE_ACTION','STORE','STORE',50,'CAMAC_SERVER',_path,_nid);
  DevAddEnd();
  return (1);
}
